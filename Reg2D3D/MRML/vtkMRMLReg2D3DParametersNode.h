/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLReg2D3DParametersNode.h,v $
  Date:      $Date: 2016/03/15 11:20:00 $
  Version:   $Revision: 1.1 $

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

#include "../Geometry.h"

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
  // Get/Set-Methods
  vtkSetStringMacro(InputVolumeNodeID);
  vtkGetStringMacro(InputVolumeNodeID);
  vtkSetStringMacro(OutputVolumeNodeID);
  vtkGetStringMacro(OutputVolumeNodeID);
  vtkSetStringMacro(XRayVolumeNodeID);
  vtkGetStringMacro(XRayVolumeNodeID);
  vtkSetStringMacro(LinearTransformNodeID);
  vtkGetStringMacro(LinearTransformNodeID);

  vtkSetMacro(IntensityDivider,int);
  vtkGetMacro(IntensityDivider,int);
  vtkSetMacro(FocalWidth,float);
  vtkGetMacro(FocalWidth,float);



protected:
  vtkMRMLReg2D3DParametersNode();
  ~vtkMRMLReg2D3DParametersNode();

  vtkMRMLReg2D3DParametersNode(const vtkMRMLReg2D3DParametersNode&);
  void operator=(const vtkMRMLReg2D3DParametersNode&);

//  void InitializeOutputVolumeNode();

  char *InputVolumeNodeID;
  char *OutputVolumeNodeID;
  char *XRayVolumeNodeID;
  char *LinearTransformNodeID;

  int IntensityDivider;
  float FocalWidth;

};

#endif

