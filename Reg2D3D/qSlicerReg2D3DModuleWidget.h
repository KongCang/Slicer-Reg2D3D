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

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerReg2D3DModuleExport.h"
#include <vtkMRMLNode.h>

class qSlicerReg2D3DModuleWidgetPrivate;
class vtkMRMLNode;
//class vtkMRMLReg2D3DParametersNode;


/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_REG2D3D_EXPORT qSlicerReg2D3DModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerReg2D3DModuleWidget(QWidget *parent=0);
  virtual ~qSlicerReg2D3DModuleWidget();
  bool checkForVolumeParentTransform() const;

public slots:


protected:
  QScopedPointer<qSlicerReg2D3DModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);

protected slots:
//  void initializeNode(vtkMRMLNode*);
  void onInputVolumeChanged();
  void onApply();
  void updateWidget();
  void onRender();

//  void initializeParameterNode(vtkMRMLScene*);


private:
  Q_DECLARE_PRIVATE(qSlicerReg2D3DModuleWidget);
  Q_DISABLE_COPY(qSlicerReg2D3DModuleWidget);

//  vtkMRMLReg2D3DParametersNode *parametersNode;
};

#endif
