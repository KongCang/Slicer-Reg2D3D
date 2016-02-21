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
#include "DrrRenderer.h"
#include "Geometry.h"


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
  connect(d->btnRenderImage, SIGNAL(clicked()),
            this, SLOT(onRender()) );

}

void qSlicerReg2D3DModuleWidget::onApply()
{
  Q_D(const qSlicerReg2D3DModuleWidget);
  vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());
  //float intensityDivider;
  //Check if VolumeData is loaded and chosen
  if(!d->InputVolumeComboBox->currentNode() ||
       !d->InputXRayVolumeComboBox->currentNode())
    return;
/*
  this->parametersNode->SetInputVolumeNodeID(d->InputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetInputXRayVolumeNodeID(d->InputXRayVolumeComboBox->currentNode()->GetID());
*/

  vtkMRMLVolumeNode *inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
  vtkMRMLVolumeNode *inputXRayVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputXRayVolumeComboBox->currentNode());

  vtkMRMLScalarVolumeNode *inputnode = vtkMRMLScalarVolumeNode::SafeDownCast(inputVolume);
  vtkMRMLScalarVolumeNode *xraynode = vtkMRMLScalarVolumeNode::SafeDownCast(inputXRayVolume);

  int intensityDivider=d->hsIntensityDivider->value();
 /* vtkNew<vtkImageData> imageDataWorkingCopy;
    imageDataWorkingCopy->DeepCopy(inputnode->GetImageData());
*/

  //Prepare the resulting Image
  //vtkImageData resImage;
  vtkSmartPointer<vtkImageData> resultImage = vtkImageData::New();
  //resultImage = &resImage;
  //vtkSmartPointer<vtkImageData> pResultImage=&resultImage;
  resultImage->SetExtent(0,409,0,409,0,0);
  resultImage->SetSpacing(1,1,1);
  resultImage->SetOrigin(0, 0, 0);
  //resultImage->SetNumberOfScalarComponents(3);
  //resultImage->SetScalarType(VTK_UNSIGNED_SHORT);
  resultImage->AllocateScalars(VTK_UNSIGNED_SHORT,3);
  cerr << "resultImageExtentSet" << endl;
  unsigned short int* dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
  cerr << "resultImagePointer" << endl;
  for (int i=0; i<410*410; i++){
      *(dummy++)=0;
  }
  cerr << "resultImageInitialized" << endl;
  Geometry machine(1536.0f,intensityDivider);    //focal width =1536mm
  Geometry* pMachine = &machine;
  cout << intensityDivider;

  //Later used for transformation
  vtkNew<vtkMatrix4x4> inputRASToIJK;
    inputnode->GetRASToIJKMatrix(inputRASToIJK.GetPointer());

  vtkNew<vtkMatrix4x4> inputIJKToRAS;
    inputnode->GetIJKToRASMatrix(inputIJKToRAS.GetPointer());

  // Rescale VolumeData
  int iDims[3],xDims[3];
  inputnode->GetImageData()->GetDimensions(iDims);
  xraynode->GetImageData()->GetDimensions(xDims);
  vtkSmartPointer<vtkImageData> id=inputnode->GetImageData();
  vtkSmartPointer<vtkImageData> xd=xraynode->GetImageData();
int testdims[3];
  id->GetDimensions(testdims);

  cerr << "Input Extents:" << testdims[0] << ", " << testdims[1] << ", "<< testdims[2] << endl;


  double iRange[2],xRange[2];
  id->GetScalarRange(iRange);
  xd->GetScalarRange(xRange);

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


  inputnode->SetAndObserveImageData(xShiftScale->GetOutput());
  id=inputnode->GetImageData();

  id->GetDimensions(testdims);

  cerr << "After rescale: " << testdims[0] << ", " << testdims[1] << ", "<< testdims[2] << endl;

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

  int iPoint[3],xPoint[3];

  unsigned int start = clock();

  unsigned short int* iPixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,150));
  unsigned short int* xPixel = static_cast<unsigned short int*>(id->GetScalarPointer(0,0,190));
  // cerr << "Test vorher 3 \n ";
  double MValue = logic->CalculateMeritFctMutualInformation(iPixel,xPixel,iDims[0],iDims[1],256*256);
  cerr << iDims[0] << "," << iDims[1] << endl;
  // cerr << "Test nachher   \n";

  cerr << "MeritFunction Value=" << MValue << endl << "Yeah" << endl;



  DrrRenderer drrRenderer(inputnode, pMachine);
  drrRenderer.computeDrr(resultImage);
  dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
   logic->writepgmimagefile(dummy, 410, 410, "Drr.pgm");



/*    const int N = 16;

    char a[N] = "Hello \0\0\0\0\0\0";
    int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    cout <<  a << "-->" << endl;

    cuda_hello(a, b, N);

    cout <<  a << endl;
*/
}

void qSlicerReg2D3DModuleWidget::onRender(){onApply();}

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
