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
// Qt includes
#include <QDebug>
#include <QMessageBox>
#include <QScopedPointer>

// SlicerQt includes
#include "qSlicerReg2D3DModuleWidget.h"
#include "ui_qSlicerReg2D3DModuleWidget.h"
#include <qSlicerAbstractCoreModule.h>


// Reg2D3D includes
#include "ImageComparerMutualInformation.h"
#include "helloworld.h"
#include "DrrRenderer.h"
#include "Geometry.h"


// Reg2D3D Logic includes
#include "Logic/vtkSlicerReg2D3DLogic.h"

// qMRML includes
#include <qMRMLNodeFactory.h>

// MRML includes
#include <MRML/vtkMRMLReg2D3DParametersNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLSelectionNode.h>

// MRMLLogic includes
#include <vtkMRMLApplicationLogic.h>

//vtk includes
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkSmartPointer.h>

// for testing puposes
#include <ctime>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerReg2D3DModuleWidgetPrivate: public Ui_qSlicerReg2D3DModuleWidget
{
   Q_DECLARE_PUBLIC(qSlicerReg2D3DModuleWidget);
protected:
  qSlicerReg2D3DModuleWidget* const q_ptr;

public:
  qSlicerReg2D3DModuleWidgetPrivate(qSlicerReg2D3DModuleWidget& object);
  ~qSlicerReg2D3DModuleWidgetPrivate();

  vtkSlicerReg2D3DLogic* logic() const;

//  void performROIVoxelGridAlignment();
//  bool checkForVolumeParentTransform() const;
//  void showUnsupportedTransVolumeVoxelCroppingDialog() const;
};

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidgetPrivate::qSlicerReg2D3DModuleWidgetPrivate(qSlicerReg2D3DModuleWidget& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidgetPrivate::~qSlicerReg2D3DModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerReg2D3DLogic* qSlicerReg2D3DModuleWidgetPrivate::logic() const
{
    Q_Q(const qSlicerReg2D3DModuleWidget);
  return vtkSlicerReg2D3DLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------

//bool qSlicerReg2D3DModuleWidget::checkForVolumeParentTransform() const
//{
//  Q_ASSERT(this->InputVolumeComboBox);
//  Q_D(const qSlicerReg2D3DModuleWidget);


//  vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(this->InputVolumeComboBox->currentNode());

//  //vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());


//  if(!inputVolume)
//    return false;

//   vtkSmartPointer<vtkMRMLLinearTransformNode> volTransform  = vtkMRMLLinearTransformNode::SafeDownCast(inputVolume->GetParentTransformNode());

//   if(volTransform)
//       return true;


//   return false;
//}


//-----------------------------------------------------------------------------
// qSlicerReg2D3DModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidget::qSlicerReg2D3DModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerReg2D3DModuleWidgetPrivate(*this) )
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
  connect(d->XRayVolumeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onXRayVolumeChanged()));

  connect(d->ComputeButton, SIGNAL(clicked()),
            this, SLOT(onCalculateMerit()) );
  connect(d->btnRenderImage, SIGNAL(clicked()),
            this, SLOT(onRenderDRR()) );

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::enter()
{
  this->onInputVolumeChanged();
  this->onXRayVolumeChanged();

  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{

  this->Superclass::setMRMLScene(scene);
  if(scene == NULL)
    {
    return;
    }
  this->initializeParameterNode(scene);
  this->updateWidget();

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::initializeParameterNode(vtkMRMLScene* scene)
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

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::initializeNode(vtkMRMLNode *n)
{
  //vtkMRMLScene* scene = qobject_cast<qMRMLNodeFactory*>(this->sender())->mrmlScene();
  //vtkMRMLAnnotationROINode::SafeDownCast(n)->Initialize(scene);
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::initializeOutputVolume()  //not yet done
{

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onEndCloseEvent()
{
  this->initializeParameterNode(this->mrmlScene());
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::updateParameters()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  if(!this->parametersNode)
    return;
  vtkMRMLReg2D3DParametersNode *pNode = this->parametersNode;

  vtkMRMLNode *InputVolumeNode = d->InputVolumeComboBox->currentNode();
  vtkMRMLNode *XRayVolumeNode = d->XRayVolumeComboBox->currentNode();

  if(InputVolumeNode)
    pNode->SetInputVolumeNodeID(InputVolumeNode->GetID());
  else
    pNode->SetInputVolumeNodeID(NULL);

  if(XRayVolumeNode)
    pNode->SetXRayVolumeNodeID(XRayVolumeNode->GetID());
  else
    pNode->SetXRayVolumeNodeID(NULL);

}


//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::updateWidget()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  if(!this->parametersNode || !this->mrmlScene())
    {
    return;
    }
  vtkMRMLReg2D3DParametersNode *parameterNode = this->parametersNode;

  vtkMRMLNode *InputVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetInputVolumeNodeID());
  vtkMRMLNode *XRayVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetXRayVolumeNodeID());
  vtkMRMLNode *OutputVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetOutputVolumeNodeID());

  if(InputVolumeNode)
    d->InputVolumeComboBox->setCurrentNode(InputVolumeNode);
  if(XRayVolumeNode)
    d->XRayVolumeComboBox->setCurrentNode(XRayVolumeNode);
  if(OutputVolumeNode)
    d->OutputVolumeComboBox->setCurrentNode(OutputVolumeNode);


  return;
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onCalculateMerit()
{
  Q_D(const qSlicerReg2D3DModuleWidget);
  vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());

  //Check if VolumeData is loaded and chosen
  if(!d->InputVolumeComboBox->currentNode() || !d->XRayVolumeComboBox->currentNode())
    return;

  this->parametersNode->SetInputVolumeNodeID(d->InputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetOutputVolumeNodeID(d->OutputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetXRayVolumeNodeID(d->XRayVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetLinearTransformNodeID(d->LinearTransformComboBox->currentNode()->GetID());

  // Get selected nodes
  vtkMRMLVolumeNode *inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
  vtkMRMLVolumeNode *xRayVolume = vtkMRMLVolumeNode::SafeDownCast(d->XRayVolumeComboBox->currentNode());

  vtkMRMLScalarVolumeNode *inputnode = vtkMRMLScalarVolumeNode::SafeDownCast(inputVolume);
  vtkMRMLScalarVolumeNode *xraynode = vtkMRMLScalarVolumeNode::SafeDownCast(xRayVolume);

  //Prepare the resulting Image
  vtkSmartPointer<vtkImageData> resultImage = vtkImageData::New();
  resultImage->SetExtent(0,409,0,409,0,0);
  resultImage->SetSpacing(1,1,1);
  resultImage->SetOrigin(0, 0, 0);
  resultImage->AllocateScalars(VTK_UNSIGNED_SHORT,3);
  unsigned short int* dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
  for (int i=0; i<410*410; i++){
      *(dummy++)=0;
  }

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

}

void qSlicerReg2D3DModuleWidget::onRenderDRR(){
    Q_D(const qSlicerReg2D3DModuleWidget);
    vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());

    //Check if VolumeData is loaded and chosen
    if(!d->InputVolumeComboBox->currentNode())
      return;

    // Get data and prepare it
    // Get volumeNode
    vtkMRMLVolumeNode *inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
    vtkMRMLScalarVolumeNode *inputnode = vtkMRMLScalarVolumeNode::SafeDownCast(inputVolume);

    // Get other parameters
    int intensityDivider=d->hsIntensityDivider->value();
    float focalWidth = static_cast<float>(d->hsFocalWidth->value());

    //Prepare machine properties
    Geometry machine(focalWidth,intensityDivider);    //focal width =1536mm
    Geometry* pMachine = &machine;

    //Get scene
    //vtkMRMLScene *scene = this->GetMRMLScene();



    //Prepare the resulting Image, Size is hardcoded
    vtkSmartPointer<vtkImageData> resultImage = vtkImageData::New();
    resultImage->SetExtent(0,409,0,409,0,0);
    resultImage->SetSpacing(1,1,1);
    resultImage->SetOrigin(0, 0, 0);
    resultImage->AllocateScalars(VTK_UNSIGNED_SHORT,1);  //3 Components (=Dimensions)
    unsigned short int* dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
    for (int i=0; i<410*410; i++){  //size hardcoded
        *(dummy++)=0;
    }

    vtkSmartPointer<vtkMRMLScalarVolumeNode> outputNode = vtkMRMLScalarVolumeNode::New();
    outputNode->SetSpacing(1.0,1.0,1.0);
    outputNode->SetAndObserveImageData(resultImage);
    outputNode->SetName("DRR");
    mrmlScene()->AddNode(outputNode);

    //vtkSmartPointer<vtkMRMLScalarVolumeNode> outputVolume = vtkMRMLScalarVolumeNode::New();

    //Resulting node
    //vtkSmartPointer<vtkMRMLScalarVolumeNode> svNode = vtkMRMLScalarVolumeNode::New();

    //outputNode->CopyWithScene(svNode);
    vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> outputDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();
    mrmlScene()->AddNode(outputDisplayNode);
    //vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode = mrmlScene()->GetNodesByName("Transform");

    //outputDisplayNode->CopyWithScene(svNode->GetDisplayNode());
    //scene->AddNode(displayNode.GetPointer());

    //Check for Transfrom node
/*    if (checkForVolumeParentTransform())
        cerr << "Transformnode exists";
*/
    DrrRenderer drrRenderer(inputnode, pMachine);
    cerr << "vor DrrRenderer\n";
    drrRenderer.computeDrr(resultImage);
    dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
    logic->writepgmimagefile(dummy, 410, 410, "Drr.pgm");
    //outputNode->SetAndObserveImageData(resultImage);
    //outputNode->SetAndObserveDisplayNodeID(outputDisplayNode->GetID);
    //outputNode->SetAndObserveStorageNodeID(NULL);
    //scene->AddNode(outputNode);

    //outputVolume = outputNode.GetPointer();

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onInputVolumeChanged()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  Q_ASSERT(d->InputVolumeComboBox);
/*  Q_ASSERT(d->VoxelBasedModeRadioButton);

  vtkMRMLNode* node = d->InputVolumeComboBox->currentNode();
  if(node)
    {
    if(d->VoxelBasedModeRadioButton->isChecked())
      {
      if(d->checkForVolumeParentTransform())
        {
        d->showUnsupportedTransVolumeVoxelCroppingDialog();
        d->InputVolumeComboBox->setCurrentNode(NULL);
        }
      else
        {
        d->performROIVoxelGridAlignment();
        }
      }
    }
*/
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onXRayVolumeChanged()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  Q_ASSERT(d->XRayVolumeComboBox);
/*  Q_ASSERT(d->VoxelBasedModeRadioButton);

  vtkMRMLNode* node = d->InputVolumeComboBox->currentNode();
  if(node)
    {
    if(d->VoxelBasedModeRadioButton->isChecked())
      {
      if(d->checkForVolumeParentTransform())
        {
        d->showUnsupportedTransVolumeVoxelCroppingDialog();
        d->InputVolumeComboBox->setCurrentNode(NULL);
        }
      else
        {
        d->performROIVoxelGridAlignment();
        }
      }
    }
*/
}

