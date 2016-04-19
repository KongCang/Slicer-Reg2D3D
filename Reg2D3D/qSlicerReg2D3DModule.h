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

#ifndef __qSlicerReg2D3DModule_h
#define __qSlicerReg2D3DModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerReg2D3DModuleExport.h"

class qSlicerReg2D3DModulePrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_REG2D3D_EXPORT qSlicerReg2D3DModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerReg2D3DModule(QObject *parent=0);
  virtual ~qSlicerReg2D3DModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  virtual QIcon icon()const;
  virtual QStringList categories()const;

  virtual QString helpText()const;
  virtual QString acknowledgementText()const;
  virtual QStringList contributors()const;

  virtual QStringList dependencies() const;

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerReg2D3DModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerReg2D3DModule);
  Q_DISABLE_COPY(qSlicerReg2D3DModule);

};

#endif
