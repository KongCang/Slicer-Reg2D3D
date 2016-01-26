/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLReg2D3DParametersNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>

// MRML includes
#include "vtkMRMLVolumeNode.h"

// CropModuleMRML includes
#include "vtkMRMLReg2D3DParametersNode.h"

// AnnotationModuleMRML includes
#include "vtkMRMLAnnotationROINode.h"

// STD includes

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLReg2D3DParametersNode);

//----------------------------------------------------------------------------
vtkMRMLReg2D3DParametersNode::vtkMRMLReg2D3DParametersNode()
{
  this->HideFromEditors = 1;

  this->InputVolumeNodeID = NULL;
  this->OutputVolumeNodeID = NULL;
  this->XRayVolumeNodeID = NULL;

//  this->ROIVisibility = false;
//  this->InterpolationMode = 2;

  this->VoxelBased = true;

  this->SpacingScalingConst = 1.;
}

//----------------------------------------------------------------------------
vtkMRMLReg2D3DParametersNode::~vtkMRMLReg2D3DParametersNode()
{
  if (this->InputVolumeNodeID)
    {
    this->SetInputVolumeNodeID(NULL);
    }

  if (this->OutputVolumeNodeID)
    {
    this->SetOutputVolumeNodeID(NULL);
    }

  if (this->XRayVolumeeNodeID)
    {
    this->SetXRayVolumeNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLReg2D3DParametersNode::ReadXMLAttributes(const char** atts)
{
  std::cerr << "Reading Reg2D3D param node!" << std::endl;
  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
  {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "inputVolumeNodeID"))
    {
      this->SetInputVolumeNodeID(attValue);
      continue;
    }
    if (!strcmp(attName, "outputVolumeNodeID"))
    {
      this->SetOutputVolumeNodeID(attValue);
      continue;
    }
    if (!strcmp(attName, "XRayVolumeNodeID"))
    {
      this->SetXRayVolumeNodeID(attValue);
      continue;
    }
    if (!strcmp(attName,"XRayVolumeVisibility"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->XRayVolumeVisibility;
      continue;
    }
    if (!strcmp(attName,"interpolationMode"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->InterpolationMode;
      continue;
    }
  }

  this->WriteXML(std::cout,1);
}

//----------------------------------------------------------------------------
void vtkMRMLReg2D3DParametersNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " inputVolumeNodeID=\"" << (this->InputVolumeNodeID ? this->InputVolumeNodeID : "NULL") << "\"";
  of << indent << " outputVolumeNodeID=\"" << (this->OutputVolumeNodeID ? this->OutputVolumeNodeID : "NULL") << "\"";
  //of << indent << " ROIVisibility=\""<< this->ROIVisibility << "\"";
  of << indent << " xRayVolumeNodeID=\"" << (this->XRayVolumeNodeID ? this->XRayVolumeNodeID : "NULL") << "\"";
  of << indent << " interpolationMode=\"" << this->InterpolationMode << "\"";
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLReg2D3DParametersNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLReg2D3DParametersNode *node = vtkMRMLReg2D3DParametersNode::SafeDownCast(anode);
  this->DisableModifiedEventOn();

  this->SetInputVolumeNodeID(node->GetInputVolumeNodeID());
  this->SetOutputVolumeNodeID(node->GetOutputVolumeNodeID());
  this->SetXRayVolumeNodeID(node->GetXRayVolumeNodeID());
  this->SetInterpolationMode(node->GetInterpolationMode());
  //this->SetROIVisibility(node->GetROIVisibility());

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLReg2D3DParametersNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << "InputVolumeNodeID: " << ( (this->InputVolumeNodeID) ? this->InputVolumeNodeID : "None" ) << "\n";
  os << "OutputVolumeNodeID: " << ( (this->OutputVolumeNodeID) ? this->OutputVolumeNodeID : "None" ) << "\n";
  os << "xRayVolumeNodeID: " << ( (this->XRayVolumeNodeID) ? this->XRayVolumeNodeID : "None" ) << "\n";
//  os << "ROIVisibility: " << this->ROIVisibility << "\n";
//  os << "InterpolationMode: " << this->InterpolationMode << "\n";
  os << "IsotropicResampling: " << this->IsotropicResampling << "\n";
}

// End
