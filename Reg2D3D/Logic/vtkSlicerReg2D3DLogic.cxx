/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// C++ includes
//#include <QDebug>
#include <new>
#include <string>
#include <netinet/in.h>

// Reg2D3D Logic includes
#include "vtkSlicerReg2D3DLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//#include "../MRML/vtkMRMLReg2D3DParametersNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerReg2D3DLogic);

//----------------------------------------------------------------------------
vtkSlicerReg2D3DLogic::vtkSlicerReg2D3DLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerReg2D3DLogic::~vtkSlicerReg2D3DLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::ProcessMRMLNodesEvents ( vtkObject* caller, unsigned long event, void* vtkNotUsed(callData) )
{
  if (caller != NULL)
    {
      vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
      if ( event == vtkMRMLLinearTransformNode::TransformModifiedEvent && strcmp( transformNode->GetID(), this->ObservedTransformNode->GetID() ) == 0 )
        if (this->GetMRMLScene()==NULL)
          {
            vtkErrorMacro("UpdateFromMRMLScene failed: scene is invalid");
            return;
          }
/*      vtkCollection* parameterNodes = this->GetMRMLScene()->GetNodesByClass("vtkMRMLReg2D3DParametersNode");

      if(parameterNodes->GetNumberOfItems() > 0)
        {
        this->ParametersNode = vtkMRMLReg2D3DParametersNode::SafeDownCast(parameterNodes->GetItemAsObject(0));
          if(!this->ParametersNode)
          {
          qCritical() << "FATAL ERROR: Cannot instantiate Reg2D3DParameterNode";
          Q_ASSERT(this->ParametersNode);
          }

        }
*/
      }
}

//-----------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::SetAndObserveTransformNode( vtkMRMLLinearTransformNode* transformNode )
{
   vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
   events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
   vtkSetAndObserveMRMLNodeEventsMacro( this->ObservedTransformNode, transformNode, events.GetPointer() );
}


//-----------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
//  vtkMRMLReg2D3DParametersNode* pNode = vtkMRMLReg2D3DParametersNode::New();
//  this->GetMRMLScene()->RegisterNodeClass(pNode);
}

//---------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerReg2D3DLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

double vtkSlicerReg2D3DLogic::CalculateMeritFctMutualInformation(imageType *imageBRawPtr, imageType *imageMRawPtr, unsigned short width, unsigned short height, unsigned long depth)
{

//    writepgmimagefile(imageBRawPtr, width, height, "InputVol.pgm");
//    writepgmimagefile(imageMRawPtr, width, height, "xRayVol.pgm");


    //Both camera
    int mRankCorrNbrPix;
    unsigned short **mRankCorrRandomArray;
    const unsigned short nbin=256;

    double *jointproba = NULL;
    jointproba = new double[nbin*nbin];

    for (int i=0;i<depth;i++){
        jointproba[i]=0;
    }

    unsigned short bPixVal,	//BaseImagePixelValue
      mPixVal,          //MatchingImagePixelValue
      bBin,		//baseImageNumberOfBins
      mBin;		//MatchingImageNumberOfBins
    int n = 0;
    float  hm  = 0;
    float  hb  = 0;
    float  hmb = 0;
    double zRes = 0.0;

    unsigned int imageBDepth = depth;
    unsigned int imageMDepth = depth;

    unsigned short imageBWidth  = width;
    unsigned short imageBHeight = height;
    unsigned short imageMWidth  = width;
    unsigned short imageMHeigth = height;

    // if there is a mask (forget about this now)
    // if (this->mRankCorrRandomArray!= NULL)
    if(0) // commented out for the moment
    {

        for (int idx = 0; idx < mRankCorrNbrPix; idx++)
        {
            // get pixel values from base and match images
            bPixVal = imageBRawPtr[mRankCorrRandomArray[idx][1] + mRankCorrRandomArray[idx][0]*imageBWidth];
            mPixVal = imageMRawPtr[mRankCorrRandomArray[idx][1] + (mRankCorrRandomArray[idx][0])*imageMWidth];
            //cerr << "Hier sollte es nicht sein \n ";

            // datatype nbins are used -> assign the appropriate bin
            bBin    = (int)((float)bPixVal*((float)nbin/(float)imageBDepth));
            mBin    = (int)((float)mPixVal*((float)nbin/(float)imageMDepth));

           // jointproba[mBin+][bBin] +=  1.0;
            n++;
        }

        for (int i = 0; i < nbin; i++) {
            for (int j = 0; j < nbin; j++) {
               // jointproba[i][j] = jointproba[i][j]/n;
            }
        }
    }
    else // if there is no mask
    {
        for (int x = 0; x < imageBWidth; x++)
        {
        //cerr << "Test in Merit-Schleife " << x << endl;
            for (int y = 0; y < imageBHeight; y++)
            {

                // get pixel values from base and match images
                bPixVal = imageBRawPtr[y + x*imageBWidth];
                mPixVal = imageMRawPtr[y + x*imageMWidth];
           //     cerr << "Test in Merit-Schleife " << y << endl;

                // datatype nbins are used -> assign the appropriate bin
                bBin    = (int)((float)bPixVal*((float)nbin/(float)imageBDepth));
                mBin    = (int)((float)mPixVal*((float)nbin/(float)imageMDepth));
                //cerr << bBin << "," << mBin << "  " << bPixVal << "  " << imageBDepth << " " << nbin << endl;
                jointproba[mBin+nbin*bBin] +=  1.0;
                n++;

            }
        }

        //cerr << "n: " << n << endl;
        for (int i = 0; i < nbin; i++) {
            for (int j = 0; j < nbin; j++) {
 //               cerr << jointproba[i+nbin*j];
                jointproba[i+nbin*j] = jointproba[i+nbin*j]/n;
 //               cerr << "normalized:" << jointproba[i+nbin*j] <<endl;
            }
        }

    }

   double* marg_base = new double [nbin];
   double* marg_match= new double [nbin];


   //marginal base and entropy
   double *ptr;
   ptr = marg_base;

      for (int j = 0; j < nbin; j++) {
        *ptr = 0.0;
        for (int i = 0; i < nbin; i++) {
            *ptr +=  jointproba[i+nbin*j];
        }

        if(*ptr>0.0)
        {
            hb  -= ((*ptr)*log(*ptr));
           // cerr << *ptr << ",\n"  << endl;
        }
        ptr++;
    }


    //marginal match and entropy
      ptr = marg_match;
    for (int i = 0; i < nbin; i++) {
        *ptr = 0.0;
        for (int j = 0; j < nbin; j++) {
            *ptr += jointproba[i+nbin*j];
        }

        if(*ptr>0.0)
        {
            hm  -= ((*ptr)*log(*ptr));
        }
        ptr++;
    }

    //Joint entropy
    for(int i=0; i<nbin; i++)
    {
        for(int j=0; j<nbin; j++)
        {
            if(jointproba[i+nbin*j]>0.0)
            {
                hmb -= jointproba[i+nbin*j]*log(jointproba[i+nbin*j]);
            }
        }
    }

    zRes = 1000 * (-1.0+((2.0*hmb)/(hm+hb)));
    delete[]jointproba;

    return zRes;

}

void vtkSlicerReg2D3DLogic::writepgmimagefile(imageType *pImage, unsigned short width, unsigned short height, std::string FileName){ //write unsigned char and unsigned short images

    ofstream ofp;
    ofp.open(FileName.c_str(), ios::out);

    if (!ofp) {
      cout << "Can't open file: " << FileName << endl;
      exit(1);
    }

    if(1)
    //is unsigned short, bytes must be swap (the most significant byte is first on pgm specifications)
    {

        ofp << "P5" << " "<<width<<" "<< height<<" "<< 65535 << endl;
        int cnt = width*height;
        unsigned short * im_swap;
        im_swap = new unsigned short[cnt];

        for (int i = 0; i<cnt;i++)
        {
            im_swap[i] = htons(pImage[i]);
        }

        char * ptr = reinterpret_cast<char *>(im_swap);
        ofp.write( ptr, (width*height)*sizeof(unsigned short));

        delete []im_swap;
    }


    if (ofp.fail()) {
      cout << "Can't write image " << FileName << endl;
      exit(0);
    }

    ofp.close();
}

