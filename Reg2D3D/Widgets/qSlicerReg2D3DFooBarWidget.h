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

#ifndef __qSlicerReg2D3DFooBarWidget_h
#define __qSlicerReg2D3DFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerReg2D3DModuleWidgetsExport.h"

class qSlicerReg2D3DFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_Reg2D3D
class Q_SLICER_MODULE_REG2D3D_WIDGETS_EXPORT qSlicerReg2D3DFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerReg2D3DFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerReg2D3DFooBarWidget();

protected slots:

protected:
  QScopedPointer<qSlicerReg2D3DFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerReg2D3DFooBarWidget);
  Q_DISABLE_COPY(qSlicerReg2D3DFooBarWidget);
};

#endif
