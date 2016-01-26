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
#include <QtPlugin>

// Reg2D3D Logic includes
#include "vtkSlicerReg2D3DLogic.h"

// Reg2D3D includes
#include "qSlicerReg2D3DModule.h"
#include "qSlicerReg2D3DModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerReg2D3DModule, qSlicerReg2D3DModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerReg2D3DModulePrivate
{
public:
  qSlicerReg2D3DModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModulePrivate::qSlicerReg2D3DModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerReg2D3DModule methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DModule::qSlicerReg2D3DModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerReg2D3DModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerReg2D3DModule::~qSlicerReg2D3DModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerReg2D3DModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerReg2D3DModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerReg2D3DModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kaspar Schweiger");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerReg2D3DModule::icon() const
{
  return QIcon(":/Icons/Reg2D3D.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerReg2D3DModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerReg2D3DModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerReg2D3DModule::setup()
{
  this->Superclass::setup();

  vtkSlicerReg2D3DLogic* Reg2D3DLogic =
    vtkSlicerReg2D3DLogic::SafeDownCast(this->logic());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerReg2D3DModule
::createWidgetRepresentation()
{
  return new qSlicerReg2D3DModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerReg2D3DModule::createLogic()
{
  return vtkSlicerReg2D3DLogic::New();
}
