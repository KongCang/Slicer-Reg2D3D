/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerReg2D3DFooBarWidget.h"
#include "ui_qSlicerReg2D3DFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Reg2D3D
class qSlicerReg2D3DFooBarWidgetPrivate
  : public Ui_qSlicerReg2D3DFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerReg2D3DFooBarWidget);
protected:
  qSlicerReg2D3DFooBarWidget* const q_ptr;

public:
  qSlicerReg2D3DFooBarWidgetPrivate(
    qSlicerReg2D3DFooBarWidget& object);
  virtual void setupUi(qSlicerReg2D3DFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerReg2D3DFooBarWidgetPrivate
::qSlicerReg2D3DFooBarWidgetPrivate(
  qSlicerReg2D3DFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerReg2D3DFooBarWidgetPrivate
::setupUi(qSlicerReg2D3DFooBarWidget* widget)
{
  this->Ui_qSlicerReg2D3DFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerReg2D3DFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerReg2D3DFooBarWidget
::qSlicerReg2D3DFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerReg2D3DFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerReg2D3DFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerReg2D3DFooBarWidget
::~qSlicerReg2D3DFooBarWidget()
{
}
