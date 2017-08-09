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

#ifndef __qSlicerReg2D3DModuleWidget_h
#define __qSlicerReg2D3DModuleWidget_h

#include "Matrices.h"

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerReg2D3DModuleExport.h"

#include <vtkMRMLNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkObserverManager.h>
#include "vtkSlicerModuleLogic.h"
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>

class qSlicerReg2D3DModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLReg2D3DParametersNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_REG2D3D_EXPORT qSlicerReg2D3DModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerReg2D3DModuleWidget(QWidget *parent=0);
  virtual ~qSlicerReg2D3DModuleWidget();

protected:
  QScopedPointer<qSlicerReg2D3DModuleWidgetPrivate> d_ptr;

  virtual void setup();	// Just signals/slots are connected
  virtual void enter(); //done
  virtual void setMRMLScene(vtkMRMLScene*);

  void initializeParameterNode(vtkMRMLScene*);
  void initializeOutputVolumeNode(vtkMRMLScene*);

protected slots:
	  void initializeNode(vtkMRMLNode*); //done
	  void onInputVolumeChanged(); //done, dummy
      void onXRayVolumeChanged(); //done, dummy
      void onOutputVolumeChanged();
      void onLinearTransformChanged();
      void onSetIdentity();
      void onInvert();
      void onCalculateMerit(); //done
      void onSaveImages();  //ToDo
	  void onRenderDRR();  //done
	  void updateParameters(); //done
	  void updateWidget();  //done
	  void onEndCloseEvent(); //done
      void onLinearTransformModified();
      void onFocalWidthChanged();
      void onIntensityDividerChanged();
      void setDRRasOutputVolume(vtkMRMLScene* scene);
      void onOptimize();

private:
  Q_DECLARE_PRIVATE(qSlicerReg2D3DModuleWidget);
  Q_DISABLE_COPY(qSlicerReg2D3DModuleWidget);

  vtkMRMLReg2D3DParametersNode *parametersNode;
  Matrices* sourceTransform;       // Transform Matrix of position of radiation source
  unsigned short volumeSize=330;   // Hardcoded, very messi
};

#endif
