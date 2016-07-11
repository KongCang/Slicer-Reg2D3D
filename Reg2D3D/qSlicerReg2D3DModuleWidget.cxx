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
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>

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

};

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidgetPrivate::qSlicerReg2D3DModuleWidgetPrivate
(qSlicerReg2D3DModuleWidget& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerReg2D3DModuleWidgetPrivate::~qSlicerReg2D3DModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerReg2D3DLogic* qSlicerReg2D3DModuleWidgetPrivate::logic() const
{
    cerr < "in logic\n";
    Q_Q(const qSlicerReg2D3DModuleWidget);
  return vtkSlicerReg2D3DLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------

//bool qSlicerReg2D3DModuleWidget::checkForVolumeParentTransform() const
//{
//  Q_ASSERT(this->InputVolumeComboBox);
//  Q_D(const qSlicerReg2D3DModuleWidget);


//  vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(this->InputVolumeComboBox->currentNode());

//  vtkSmartPointer<vtkMRMLVolumeNode> inputVolume = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());


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
    this->parametersNode=NULL;
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
  connect(d->OutputVolumeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onOutputVolumeChanged()));
  connect(d->LinearTransformComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onLinearTransformChanged()));
  //connect(d->LinearTransformComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
  //          this, SLOT(initializeObserver(vtkMRMLNode*)));
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
  this->onOutputVolumeChanged();
  this->onLinearTransformChanged();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
/*void qSlicerReg2D3DModuleWidget::initializeObserver(vtkMRMLNode* node)
{
  Q_D(qSlicerReg2D3DModuleWidget);

  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  d->logic()->SetAndObserveTransformNode( transformNode );
}
*/

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  this->Superclass::setMRMLScene(scene);

    if(scene == NULL)
    {
      return;
    }
  this->initializeParameterNode(scene);
  this->initializeOutputVolumeNode(scene);
  this->setDRRasOutputVolume(scene);
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
  cerr << "Initialize Node\n";
    //vtkMRMLScene* scene = qobject_cast<qMRMLNodeFactory*>(this->sender())->mrmlScene();
  //vtkMRMLAnnotationROINode::SafeDownCast(n)->Initialize(scene);
}


//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onEndCloseEvent()
{
  this->initializeParameterNode(this->mrmlScene());
}

//-----------------------------------------------------------------------------

void qSlicerReg2D3DModuleWidget::setDRRasOutputVolume(vtkMRMLScene* scene)
{

}

void qSlicerReg2D3DModuleWidget::updateParameters()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  if(!this->parametersNode)
    return;
  vtkMRMLReg2D3DParametersNode *pNode = this->parametersNode;
  vtkMRMLNode *InputVolumeNode = d->InputVolumeComboBox->currentNode();
  vtkMRMLNode *XRayVolumeNode = d->XRayVolumeComboBox->currentNode();
  vtkMRMLNode *OutputVolumeNode = d->OutputVolumeComboBox->currentNode();
  vtkMRMLNode *LinearTransformNode = d->LinearTransformComboBox->currentNode();
  float focalWidth = static_cast<float>(d->hsFocalWidth->value());
  int intensityDivider=d->hsIntensityDivider->value();

  if(InputVolumeNode)
    pNode->SetInputVolumeNodeID(InputVolumeNode->GetID());
  else
    pNode->SetInputVolumeNodeID(NULL);

  if(XRayVolumeNode)
    pNode->SetXRayVolumeNodeID(XRayVolumeNode->GetID());
  else
    pNode->SetXRayVolumeNodeID(NULL);

  if(OutputVolumeNode)
    pNode->SetOutputVolumeNodeID(OutputVolumeNode->GetID());
  else
    pNode->SetOutputVolumeNodeID(NULL);

  if(LinearTransformNode)
    pNode->SetLinearTransformNodeID(LinearTransformNode->GetID());
  else
    pNode->SetLinearTransformNodeID(NULL);

  pNode->SetIntensityDivider(intensityDivider);
  pNode->SetFocalWidth(focalWidth);

}


//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::updateWidget()
{
  cerr << "in UpdateWidget!!!!!!!\n\n";
  Q_D(qSlicerReg2D3DModuleWidget);
  if(!this->parametersNode || !this->mrmlScene())
    {
      return;
    }
  vtkMRMLReg2D3DParametersNode *parameterNode = this->parametersNode;

  vtkMRMLNode *InputVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetInputVolumeNodeID());
  vtkMRMLNode *XRayVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetXRayVolumeNodeID());
  vtkMRMLNode *OutputVolumeNode = this->mrmlScene()->GetNodeByID(parameterNode->GetOutputVolumeNodeID());
  vtkMRMLNode *LinearTransformNode = this->mrmlScene()->GetNodeByID(parameterNode->GetLinearTransformNodeID());

  if(InputVolumeNode)
    d->InputVolumeComboBox->setCurrentNode(InputVolumeNode);
  if(XRayVolumeNode)
    d->XRayVolumeComboBox->setCurrentNode(XRayVolumeNode);
  if(OutputVolumeNode)
    d->OutputVolumeComboBox->setCurrentNode(OutputVolumeNode);
  if(LinearTransformNode)
    d->LinearTransformComboBox->setCurrentNode(LinearTransformNode);
// eventuell dasselbe für focalWidth und intensityIdentifier
  return;
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onCalculateMerit()
{
  Q_D(const qSlicerReg2D3DModuleWidget);
  vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());

  //Check if VolumeData is loaded and chosen
  if(!d->OutputVolumeComboBox->currentNode() || !d->XRayVolumeComboBox->currentNode())
    return;

  //this->parametersNode->SetInputVolumeNodeID(d->InputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetOutputVolumeNodeID(d->OutputVolumeComboBox->currentNode()->GetID());
  this->parametersNode->SetXRayVolumeNodeID(d->XRayVolumeComboBox->currentNode()->GetID());
  //this->parametersNode->SetLinearTransformNodeID(d->LinearTransformComboBox->currentNode()->GetID());

  // Get selected nodes
  vtkMRMLVolumeNode *DRRVolume=vtkMRMLVolumeNode::SafeDownCast(d->OutputVolumeComboBox->currentNode());
  vtkMRMLVolumeNode *xRayVolume = vtkMRMLVolumeNode::SafeDownCast(d->XRayVolumeComboBox->currentNode());

  vtkMRMLScalarVolumeNode *DRRnode = vtkMRMLScalarVolumeNode::SafeDownCast(DRRVolume);
  vtkMRMLScalarVolumeNode *xRaynode = vtkMRMLScalarVolumeNode::SafeDownCast(xRayVolume);

  unsigned short int DRRImage[410*410];
  unsigned short int XRayImage[410*410];

  int dDims[3],xDims[3];    //Dimensions of Images
  DRRnode->GetImageData()->GetDimensions(dDims);
  xRaynode->GetImageData()->GetDimensions(xDims);

  vtkSmartPointer<vtkImageData> DRRd=DRRnode->GetImageData();
  vtkSmartPointer<vtkImageData> XRayd=xRaynode->GetImageData();

  /*double dRange[2],xRange[2];
  DRRd->GetScalarRange(dRange);
  XRayd->GetScalarRange(xRange);
*/
  unsigned short int* dummyDRR=static_cast<unsigned short int*>(DRRd->GetScalarPointer(0,0,0));
  unsigned short int* dummyXRay=static_cast<unsigned short int*>(XRayd->GetScalarPointer(0,0,0));

  //double dScale=65535./(dRange[1]-dRange[0]);
  //double xScale=65535./(xRange[1]-xRange[0]);

  for (int i=0;i<410*410;i++){
      DRRImage[i]=(*(dummyDRR++));
      XRayImage[i]=(*(dummyXRay++));
  }

  double MValue = logic->CalculateMeritFctMutualInformation(DRRImage,XRayImage,dDims[0],dDims[1],256*256);
  cerr << "MeritFunction Value=" << MValue << endl;

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onRenderDRR(){

    Q_D(const qSlicerReg2D3DModuleWidget);
    vtkSlicerReg2D3DLogic *logic = vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());

    //Check if VolumeData is loaded and chosen
    if(!d->InputVolumeComboBox->currentNode())       return;
    if(!d->OutputVolumeComboBox->currentNode())      return;
    if(!d->LinearTransformComboBox->currentNode())   return;

    // Get data and prepare it
    // Get volumeNode
    vtkMRMLLinearTransformNode *transform = vtkMRMLLinearTransformNode::SafeDownCast(d->LinearTransformComboBox->currentNode());
    vtkMRMLScalarVolumeNode *inputVolume = vtkMRMLScalarVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
    vtkMRMLScalarVolumeNode *outputVolume = vtkMRMLScalarVolumeNode::SafeDownCast(d->OutputVolumeComboBox->currentNode());

    // Get other parameters
    int intensityDivider=d->hsIntensityDivider->value();
    float focalWidth = static_cast<float>(d->hsFocalWidth->value());

    float elems[16];
    vtkSmartPointer<vtkMatrix4x4> pMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    transform->GetMatrixTransformToParent(pMatrix);
//cerr << "nach elems\n";
    /* //pMatrix->Invert();
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            elems[4*i+j]=static_cast<float>(pMatrix->GetElement(i,j));
            cerr << elems[4*i+j] << " ";
        }
        cerr << "\n";
    }0.00°
*/
    //matrix->Invert();

    //Prepare machine properties
    Geometry machine(focalWidth,intensityDivider,pMatrix);    //focal width =1536mm
    Geometry* pMachine = &machine;

    vtkSmartPointer<vtkImageData> resultImage = outputVolume->GetImageData();
    DrrRenderer drrRenderer(inputVolume, pMachine);
    cerr << "vor DrrRenderer\n";

    drrRenderer.computeDrr(resultImage);
    cerr << "nach DrrRenderer\n";
    outputVolume->SetAndObserveImageData(resultImage);
    outputVolume->Modified();

}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onInputVolumeChanged()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  Q_ASSERT(d->InputVolumeComboBox);
  if (!this->parametersNode)
      return;

  vtkMRMLNode* node = d->InputVolumeComboBox->currentNode();
  if(node)
    {
      this->parametersNode->SetInputVolumeNodeID(d->InputVolumeComboBox->currentNode()->GetID());
    }
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onXRayVolumeChanged()
{
  Q_D(qSlicerReg2D3DModuleWidget);
  Q_ASSERT(d->XRayVolumeComboBox);
    if (!this->parametersNode)
        return;

    vtkMRMLNode* node = d->XRayVolumeComboBox->currentNode();
    if(node)
      {
        this->parametersNode->SetXRayVolumeNodeID(d->XRayVolumeComboBox->currentNode()->GetID());
      }
  }

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onOutputVolumeChanged()
{
    Q_D(qSlicerReg2D3DModuleWidget);
    Q_ASSERT(d->OutputVolumeComboBox);

    if (!this->parametersNode)
      return;

    vtkMRMLNode* node = d->OutputVolumeComboBox->currentNode();
    if(node)
      {
        this->parametersNode->SetOutputVolumeNodeID(d->OutputVolumeComboBox->currentNode()->GetID());
      }

    vtkMRMLLinearTransformNode *transform = vtkMRMLLinearTransformNode::SafeDownCast(d->LinearTransformComboBox->currentNode());
    if (transform)
      {
        vtkMRMLScalarVolumeNode *outputVolume = vtkMRMLScalarVolumeNode::SafeDownCast(d->OutputVolumeComboBox->currentNode());
//        outputVolume->SetAndObserveTransformNodeID(transform->GetID());
      }
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onLinearTransformChanged()
{
    Q_D(qSlicerReg2D3DModuleWidget);
    Q_ASSERT(d->LinearTransformComboBox);
    //cerr << " In onLinearTransfromChanged\n";
    if (!this->parametersNode)
      return;
    //cerr << "paramnode exists\n";
    vtkMRMLLinearTransformNode *transform = vtkMRMLLinearTransformNode::SafeDownCast(d->LinearTransformComboBox->currentNode());
    if (transform)
    {
      //cerr << "Transform exists\n";
      //cerr << "transform: " << transform->GetID() << endl;
      //cerr << "param: transform: " << /*this->parametersNode->GetLinearTransformNodeID() << */ endl;
      if (this->parametersNode->GetLinearTransformNodeID()){
        //cerr << "param-transfrom exists: " << this->parametersNode->GetLinearTransformNodeID();
        if (strcmp( transform->GetID(), this->parametersNode->GetLinearTransformNodeID() ) == 0 )
            {//cerr << "sind gleich\n";
            return;}
      }
      this->parametersNode->SetLinearTransformNodeID(d->LinearTransformComboBox->currentNode()->GetID());
      //cerr << "param: transform: " << this->parametersNode->GetLinearTransformNodeID() << endl;

    }

    vtkMRMLVolumeNode *node=vtkMRMLVolumeNode::SafeDownCast(d->OutputVolumeComboBox->currentNode());

    // vtkMRMLNode* node = d->OutputVolumeComboBox->currentNode();
    if (node) cerr << "DRR-Node exists\n";
    if(node && transform)
      {
        cerr << "SetAndObserve\n";
        d->logic()->SetAndObserveTransformNode(transform);
     }
}

void qSlicerReg2D3DModuleWidget::onFocalWidthChanged()
{
    Q_D(qSlicerReg2D3DModuleWidget);
    Q_ASSERT(d->hsIntensityDivider);
    if (!this->parametersNode)
      return;

    int intensityDivider=d->hsIntensityDivider->value();
    this->parametersNode->SetIntensityDivider(intensityDivider);
}

void qSlicerReg2D3DModuleWidget::onIntensityDividerChanged()
{
    Q_D(qSlicerReg2D3DModuleWidget);
    Q_ASSERT(d->hsFocalWidth);
    if (!this->parametersNode)
      return;
    float focalWidth = static_cast<float>(d->hsFocalWidth->value());
    this->parametersNode->SetFocalWidth(focalWidth);
}

//-----------------------------------------------------------------------------
/*void qSlicerReg2D3DModuleWidget::initializeObserver(vtkMRMLNode* node)
{
  Q_D(qSlicerReg2D3DModuleWidget);

  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  d->logic()->SetAndObserveTransformNode( transformNode );
}
*/


/*void qSlicerReg2D3DModuleWidget::ProcessMRMLNodesEvents ( vtkObject* caller, unsigned long event, void* vtkNotUsed(callData) )
{

    std::cout << "in ProcessMRMLNodesEvents" << std::endl;
    if (caller != NULL)
      {
        vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
        if ( event == vtkMRMLLinearTransformNode::TransformModifiedEvent && strcmp( transformNode->GetID(), this->ObservedTransformNode->GetID() ) == 0 )
            std::cout << "Observed Transform changed callback" << std::endl;
      }
  //vtkRenderWindowInteractor *iren =
    //static_cast<vtkRenderWindowInteractor*>(caller);

  //std::cout << "Pressed: " << iren->GetKeySym() << std::endl;
}

*/
//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::onLinearTransformModified()
{
  /*Q_D(qSlicerReg2D3DModuleWidget);
  Q_ASSERT(d->LinearTransformComboBox);
  vtkMRMLLinearTransformNode *transform = vtkMRMLLinearTransformNode::SafeDownCast(d->LinearTransformComboBox->currentNode());
  vtkMRMLScalarVolumeNode *outputVolume = vtkMRMLScalarVolumeNode::SafeDownCast(d->OutputVolumeComboBox->currentNode());
  //vtkMRMLScalarVolumeNode *inputVolume= vtkMRMLScalarVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
  cout << "onlinch\n";
  if (outputVolume&&transform){
      outputVolume->SetAndObserveTransformNodeID(transform->GetID());
      cout << "lin observed!\n";
  }
  cout << "onlinch-after\n";
  */
  cout << "onModified\n";
  onRenderDRR();
}



//-----------------------------------------------------------------------------
void qSlicerReg2D3DModuleWidget::initializeOutputVolumeNode(vtkMRMLScene* scene)
{
    //Prepare the resulting Image, Size is hardcoded
  cerr << "initOutputNode\n";
    const int imgSize = 410;
    vtkSmartPointer<vtkImageData> resultImage = vtkImageData::New();
    resultImage->SetExtent(0,imgSize-1,0,imgSize-1,0,0);
    resultImage->SetSpacing(1,1,1);
    resultImage->SetOrigin(0, 0, 0);
    resultImage->AllocateScalars(VTK_UNSIGNED_SHORT,1);  //3 Components (=Dimensions)

    unsigned short int* dummy=static_cast<unsigned short*>(resultImage->GetScalarPointer(0,0,0));
    for (int i=0; i<imgSize*imgSize; i++){  //size hardcoded
        *(dummy++)=0;
    }
    vtkSmartPointer<vtkMRMLScalarVolumeNode> outputNode = vtkMRMLScalarVolumeNode::New();
    vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> outputDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();

    outputNode->SetSpacing(1.0,1.0,1.0);
    outputNode->SetAndObserveImageData(resultImage);
    outputNode->SetName("DRR");
    outputNode->SetAndObserveDisplayNodeID(outputDisplayNode->GetID());

    this->parametersNode->SetOutputVolumeNodeID(outputNode->GetID());
    scene->AddNode(outputNode);
    Q_D(qSlicerReg2D3DModuleWidget);
    d->OutputVolumeComboBox->setCurrentNode(outputNode);
    onOutputVolumeChanged();
    cerr << outputNode->GetID() << endl;


}

/*
 * //-----------------------------------------------------------------------------
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
  vtkMRMLVolumeNode *inputVolum0.00°e = vtkMRMLVolumeNode::SafeDownCast(d->InputVolumeComboBox->currentNode());
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

*/
