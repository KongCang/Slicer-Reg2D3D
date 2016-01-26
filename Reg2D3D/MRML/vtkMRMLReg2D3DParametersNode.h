/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2015/11/22 14:41:24 $
  Version:   $Revision: 1.0 $

=========================================================================auto=*/
// .NAME vtkMRMLReg2D3DParametersNode - MRML node for storing a slice through RAS space
// .SECTION Description
// This node stores the information about the currently selected volume
//
//

#ifndef __vtkMRMLReg2D3DParametersNode_h
#define __vtkMRMLReg2D3DParametersNode_h

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkSlicerReg2D3DModuleMRMLExport.h"

// class vtkMRMLAnnotationROINode;
class vtkMRMLVolumeNode;

/// \ingroup Slicer_QtModules_Reg2D3D
class VTK_SLICER_REG2D3D_MODULE_MRML_EXPORT vtkMRMLReg2D3DParametersNode : public vtkMRMLNode
{
  public:

  static vtkMRMLReg2D3DParametersNode *New();
  vtkTypeMacro(vtkMRMLReg2D3DParametersNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Reg2D3DParameters";};

  // Description:
  vtkSetStringMacro(InputVolumeNodeID);
  vtkGetStringMacro (InputVolumeNodeID);
  vtkSetStringMacro(OutputVolumeNodeID);
  vtkGetStringMacro (OutputVolumeNodeID);
  vtkSetStringMacro(InputXRayVolumeNodeID);
  vtkGetStringMacro (InputXRayVolumeNodeID);
//  vtkSetStringMacro(ROINodeID);
//  vtkGetStringMacro (ROINodeID);

/*  vtkSetMacro(IsotropicResampling,bool);
  vtkGetMacro(IsotropicResampling,bool);
  vtkBooleanMacro(IsotropicResampling,bool);

  vtkSetMacro(ROIVisibility,bool);
  vtkGetMacro(ROIVisibility,bool);
  vtkBooleanMacro(ROIVisibility,bool);

  vtkSetMacro(VoxelBased,bool);
  vtkGetMacro(VoxelBased,bool);
  vtkBooleanMacro(VoxelBased,bool);

  vtkSetMacro(InterpolationMode, int);
  vtkGetMacro(InterpolationMode, int);

  vtkSetMacro(SpacingScalingConst, double);
  vtkGetMacro(SpacingScalingConst, double);
*/
protected:
  vtkMRMLReg2D3DParametersNode();
  ~vtkMRMLReg2D3DParametersNode();

  vtkMRMLReg2D3DParametersNode(const vtkMRMLReg2D3DParametersNode&);
  void operator=(const vtkMRMLReg2D3DParametersNode&);

  char *InputVolumeNodeID;
  char *OutputVolumeNodeID;
  char *InputXRayVolumeNodeID;
//  char *ROINodeID;

//  bool ROIVisibility;
  bool VoxelBased;
//  int InterpolationMode;
//  bool IsotropicResampling;
//  double SpacingScalingConst;
};

#endif
