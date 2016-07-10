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

// .NAME vtkSlicerReg2D3DLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerReg2D3DLogic_h
#define __vtkSlicerReg2D3DLogic_h

//C++ - Include
#include <string>

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// Re2D3D Logic includes

// MRML includes

// STD includes

#include <cstdlib>

#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerReg2D3DModuleLogicExport.h"

class vtkMRMLReg2D3DParametersNode;

typedef unsigned short imageType;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_REG2D3D_MODULE_LOGIC_EXPORT vtkSlicerReg2D3DLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerReg2D3DLogic *New();
  vtkTypeMacro(vtkSlicerReg2D3DLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetAndObserveTransformNode(vtkMRMLLinearTransformNode* transformNode );
  double CalculateMeritFctMutualInformation(imageType *mImageBase, imageType *mImageMatch, unsigned short width, unsigned short height, unsigned long depth);
  void writepgmimagefile(imageType *pImage, unsigned short width, unsigned short height, std::string FileName);
  vtkMRMLLinearTransformNode* ObservedTransformNode=NULL;
  //vtkMRMLReg2D3DParametersNode* ParametersNode=NULL;
  vtkMRMLReg2D3DParametersNode* ParametersNode;

protected:
  vtkSlicerReg2D3DLogic();
  virtual ~vtkSlicerReg2D3DLogic();

  virtual void ProcessMRMLNodesEvents ( vtkObject* caller, unsigned long event, void* vtkNotUsed(callData) );
  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:
  vtkSlicerReg2D3DLogic(const vtkSlicerReg2D3DLogic&); // Not implemented
  void operator=(const vtkSlicerReg2D3DLogic&); // Not implemented
};

#endif
