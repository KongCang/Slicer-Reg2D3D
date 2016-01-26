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

#include <ctime>

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerReg2D3DModuleWidget.h"
#include "ui_qSlicerReg2D3DModuleWidget.h"

// MRML includes
//#include <MRML/vtkMRMLReg2D3DParametersNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkSmartPointer.h>

// Custom includes
#include "ImageComparerMutualInformation.h"

//#include <vtkMRMLSelectionNode.h>
//#include <vtkMRMLLinearTransformNode.h>

#include "vtkSlicerReg2D3DLogic.h"

#include "helloworld.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerReg2D3DModuleWidgetPrivate: public Ui_qSlicerReg2D3DModuleWidget
{
public:
  qSlicerReg2D3DModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidgetPrivate::qSlicerReg2D3DModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidget::qSlicerReg2D3DModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerReg2D3DModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidget::~qSlicerReg2D3DModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::setup()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(d->InputVolumeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onInputVolumeChanged()));
  connect(d->ComputeButton, SIGNAL(clicked()),
            this, SLOT(onApply()) );


}

void qSlicerReg2D3DModuleWidget::onApply()
{
  Q_D(const qSlicerReg2D3DModuleWidget);
  vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());


  if(!d->InputVolumeComboBox->currentNode() ||
       !d->InputXRayVolumeComboBox->currentNode())
    return;
/*
  this->parametersNode->SetInputVolumeNodeID(d->InputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetInputXRayVolumeNodeID(d->InputXRayVolumeComboBox->currentNode()->GetID());
*/
  vtkMRMLVolumeNode *inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
  vtkMRMLVolumeNode *inputXRayVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputXRayVolumeComboBox->currentNode());

  //vtkMRMLVolumeDisplayNode;;
  //vtkMRMLScalarVolumeDisplayNode
  //vtkMRMLDiffusionTensorVolumeNode *dtvnode= vtkMRMLDiffusionTensorVolumeNode::SafeDownCast(inputVolume);
  //vtkMRMLDiffusionWeightedVolumeNode *dwvnode= vtkMRMLDiffusionWeightedVolumeNode::SafeDownCast(inputVolume);
  //vtkMRMLVectorVolumeNode *vvnode= vtkMRMLVectorVolumeNode::SafeDownCast(inputVolume);
  vtkMRMLScalarVolumeNode *inputnode = vtkMRMLScalarVolumeNode::SafeDownCast(inputVolume);
  vtkMRMLScalarVolumeNode *xraynode = vtkMRMLScalarVolumeNode::SafeDownCast(inputXRayVolume);

  vtkNew<vtkImageData> imageDataWorkingCopy;
    imageDataWorkingCopy->DeepCopy(inputnode->GetImageData());

  vtkNew<vtkMatrix4x4> inputRASToIJK;
    inputnode->GetRASToIJKMatrix(inputRASToIJK.GetPointer());

  vtkNew<vtkMatrix4x4> inputIJKToRAS;
    inputnode->GetIJKToRASMatrix(inputIJKToRAS.GetPointer());



  int iDims[3],xDims[3];
  inputnode->GetImageData()->GetDimensions(iDims);
  xraynode->GetImageData()->GetDimensions(xDims);
  vtkSmartPointer<vtkImageData> id=inputnode->GetImageData();
  vtkSmartPointer<vtkImageData> xd=xraynode->GetImageData();

  double iRange[2],xRange[2];
  id->GetScalarRange(iRange);
  xd->GetScalarRange(xRange);

  std::cout << " max:" <<  iRange[1] << " min:" << iRange[0] << std::endl;
  std::cout << " max:" <<  xRange[1] << " min:" << xRange[0] << std::endl;

  /*if (id)
  {
      std::cout << " Type of Scalar " << id->GetScalarTypeAsString() << " max:" <<  iRange[1] << " min:" << iRange[0] << std::endl;

  }

  {
      std::cout << " Type of Scalar " << id->GetScalarType() << " max:" <<  id->GetScalarTypeMax() << " min:" << id->GetScalarTypeMin() << std::endl;
  }
*/
  vtkSmartPointer<vtkImageShiftScale> xShiftScale =
      vtkSmartPointer<vtkImageShiftScale>::New();

  if (iRange[1]-iRange[0]<256*256){
  xShiftScale->SetInputData(id);
  xShiftScale->SetShift(-iRange[0]);
  xShiftScale->Update();
  xShiftScale->SetOutputScalarTypeToUnsignedShort();
  xShiftScale->SetScale(65535.0/(iRange[1]-iRange[0]));
  xShiftScale->Update();

  // vtkSmartPointer<vtkMRMLScalarVolumeNode> testNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

 // vtkMRMLScene* ptestScene = this->mrmlScene();
  //ptestScene->AddNode(testNode);
 // imageDataWorkingCopy->DeepCopy(xShiftScale->GetOutput());
 // cerr << "&id vor SetAndObserve: " << id << endl;
  inputnode->SetAndObserveImageData(xShiftScale->GetOutput());
  id=inputnode->GetImageData();
  cerr << "Rescale iRange\n";
  }
  //cerr << "&id nach SetAndObserve: " << id << endl;
  if (xRange[1]-xRange[0]<256*250){
  xShiftScale->SetInputData(xd);
  xShiftScale->SetShift(-xRange[0]);
  xShiftScale->Update();
  xShiftScale->SetOutputScalarTypeToUnsignedShort();
  xShiftScale->SetScale(65535.0/(xRange[1]-xRange[0]));
  cerr << "Scale factor: " << 65535.0/(float)(xRange[1]-xRange[0]) << endl;
  xShiftScale->Update();
  xraynode->SetAndObserveImageData(xShiftScale->GetOutput());
  xd=xraynode->GetImageData();
  cerr << "Rescale xRange\n";
}


  // vtkSmartPointer<vtkPointData> pd=inputnode->GetImageData()->GetPointData();
  // vtkSmartPointer<vtkPointData> id=inputnode->GetImageData()->GetPointData();

  // std::cout << "Test\n";

  int iPoint[3],xPoint[3];
  //std::cout << inputnode->GetImageData()->ComputePointId(punkti);

  unsigned int start = clock();

  unsigned short int* iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,150));
  unsigned short int* xPixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,190));
  // cerr << "Test vorher 3 \n ";
  double MValue = logic->CalculateMeritFctMutualInformation(iPixel,xPixel,iDims[0],iDims[1],256*256);
  cerr << iDims[0] << "," << iDims[1] << endl;
  // cerr << "Test nachher   \n";

  cerr << "MeritFunction Value=" << MValue << endl << "Yeah" << endl;


  /*int* dimens = id->GetDimensions();
  for (int z = 0; z < dimens[2]; z++)
      {
      for (int y = 0; y < dimens[1]; y++)
        {
        for (int x = 0; x < dimens[0]; x++)
          {
          pixel = static_cast<unsigned short int*>(id->GetScalarPointer(x,y,z));
          if (pixel[0]!=refpixel[x+y*dimens[0]+z*dimens[0]*dimens[1]])
                  cout << "Fehler bei (" << x <<"," << y << "," << z <<")" << endl;
           }
        }
      }
  std::cout << "Time taken in millisecs: " << clock()-start << std::endl;
  std:cout << "\n\n\n";

*/
 /* start  = clock();

  dimens = id->GetDimensions();
  unsigned short int* pixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,0));

  for (int z = 0; z < dimens[2]; z++)
      {
      for (int y = 0; y < dimens[1]; y++)
        {
        for (int x = 0; x < dimens[0]; x++)
          {
          pixel[x+y*dimens[0]+z*dimens[0]*dimens[1]] = 65535-pixel[x+y*dimens[0]+z*dimens[0]*dimens[1]];
          }
        }
      }
  std::cout << "Part 2: Time taken in millisecs: " << clock()-start << std::endl;

  start  = clock();

  dimens = id->GetDimensions();
  pixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,0));

  int dummy=dimens[0]*dimens[1]*dimens[2];

  for (int z = 0; z < dummy; z++)
      {
          pixel[z] = 65535-pixel[z];
      }
  std::cout << "Part 3: Time taken in millisecs: " << clock()-start;

*/
  /*iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(150,150,150));
  std::cerr << pixel << " : " << pixel[0] << " : " << &(pixel[1]) << std::endl;
  iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(151,150,150));
  std::cerr << pixel << " : " << pixel[0] << " : " << &(pixel[dimens[0]]) << std::endl;
  iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(151,151,150));
  std::cerr << pixel << " : " << pixel[0] << " : " << &(pixel[dimens[1]*dimens[0]]) << std::endl;
  iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(151,151,151));
  std::cerr << pixel << " : " << pixel[0] << std::endl;
*/



  // Test: recenter
 /* int extent[6];
  double spacing[3];
  double origin[3];

  inputnode->GetSpacing(spacing);
  inputnode->GetOrigin(origin);
  inputnode->GetImageData()->GetExtent(extent);

  double center[3];
  center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
  center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
  center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);

  for (int i=0;i<3;i++){
  std::cerr << center[i] << " o: " << origin[i] << " sp: " << spacing[i] << " ext: " << extent[0+2*i] << "," << extent[1+2*i] << std::endl;
  }


  {
      std::cout << " Type of Scalar " << imageDataWorkingCopy->GetScalarType() << " max:" <<  imageDataWorkingCopy->GetScalarTypeMax() << " min:" << imageDataWorkingCopy->GetScalarTypeMin() << std::endl;

  }
  if (pd)
  {
    std::cout << " contains point data with " << pd->GetNumberOfArrays() << " arrays." << std::endl;
    for (int i = 0; i < pd->GetNumberOfArrays(); i++)
    {
      std::cout << "\tArray " << i << " is named " << (pd->GetArrayName(i) ? pd->GetArrayName(i) : "NULL") << std::endl;
    }
  }

  std::cerr << "Dimensions: " << dims[0] << "," << dims[1] << "," << dims[2] << std::endl;
  std::cerr << "Memory size: " << inputnode->GetImageData()->GetActualMemorySize() << std::endl;
  std::cerr << "Data object type: " << inputnode->GetImageData()->GetDataObjectType() << std::endl;


  std::cerr << "ComputeButton pressed" << inputnode->GetID() << std::endl;
  std::cerr << "ComputeButton pressed" << xraynode->GetID() << std::endl;

*/
  /* Sample to access points fastly
   * //-----------------------------------------------------------------------------
     //N.B.  The image data object must be correctly set up and scalars
allocated
     //before calling this method. That is: Allocate the object, set the
dimensions,
     //set the scalar type, and set the number of components.
     template< typename T >
     void
     fillImageData( vtkSmartPointer< vtkImageData > pImageData, const T&
val )
     {
         int dims[ 3 ];
         pImageData->GetDimensions ( dims );
         const int numComponents =
pImageData->GetNumberOfScalarComponents ();
         const int numElements = dims[ 0 ] * dims[ 1 ] * dims[ 2 ] *
numComponents;
         T* pPixel = static_cast< T* >( pImageData->GetScalarPointer () );
         for( int elemCnt = 0; elemCnt < numElements; ++elemCnt )
         {
             pPixel[ elemCnt ] = val;
         }
     }
*/

    const int N = 16;

    char a[N] = "Hello \0\0\0\0\0\0";
    int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    cout <<  a << "-->" << endl;

    cuda_hello(a, b, N);

    cout <<  a << endl;

}

void qSlicerReg2D3DModuleWidget::onInputVolumeChanged()
{

}

void qSlicerReg2D3DModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{

  this->Superclass::setMRMLScene(scene);
  if(scene == NULL)
    {
    return;
    }

  //this->initializeParameterNode(scene);

  //this->updateWidget();

  // observe close event
  //qvtkReconnect(this->mrmlScene(), vtkMRMLScene::EndCloseEvent,
  //  this, SLOT(onEndCloseEvent()));
}

//-----------------------------------------------------------------------------

bool qSlicerReg2D3DModuleWidget::checkForVolumeParentTransform() const
{
  Q_ASSERT(this->InputVolumeComboBox);
  Q_D(const qSlicerReg2D3DModuleWidget);


  //vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(this->InputVolumeComboBox->currentNode());

  vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());


  if(!inputVolume)
    return false;

   vtkSmartPointer<vtkMRMLLinearTransformNode> volTransform  = vtkMRMLLinearTransformNode::SafeDownCast(inputVolume->GetParentTransformNode());

   if(volTransform)
       return true;


   return false;
}



void qSlicerReg2D3DModuleWidget::updateWidget()
{
    return;
}



/*void qSlicerReg2D3DModuleWidget::initializeParameterNode(vtkMRMLScene* scene)
{
  vtkCollection* parameterNodes = scene->GetNodesByClass("vtkMRMLReg2D3DParametersNode");

  if(parameterNodes->GetNumberOfItems() > 0)
    {
    this->parametersNode = vtkMRMLReg2D3DParametersNode::SafeDownCast(parameterNodes->GetItemAsObject(0));
    if(!this->parametersNode)
      {
      qCritical() << "FATAL ERROR: Cannot instantiate Reg2D3DParameterNode";
      Q_ASSERT(this->parametersNode);
      }
    }
  else
    {
    qDebug() << "No Reg2D3D parameter nodes found!";
    this->parametersNode = vtkMRMLReg2D3DParametersNode::New();
    scene->AddNode(this->parametersNode);
    this->parametersNode->Delete();
    }

  parameterNodes->Delete();
}
*/
