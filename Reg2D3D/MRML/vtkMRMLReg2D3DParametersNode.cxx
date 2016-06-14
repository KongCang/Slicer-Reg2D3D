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
#include <vtkImageData.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
// MRML includes
#include "vtkMRMLNode.h"

// Reg2D3DMRML includes
#include "vtkMRMLReg2D3DParametersNode.h"

// AnnotationModuleMRML includes
//#include "vtkMRMLAnnotationROINode.h"

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
  this->LinearTransformNodeID = NULL;

//  this->IntensityDivider=100;
//  this->FocalWidth = 1536;


  //this->InitializeOutputVolumeNode();

    //  this->ROIVisibility = false;
//  this->InterpolationMode = 2;

  //this->VoxelBased = true;

  //this->SpacingScalingConst = 1.;
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

  if (this->XRayVolumeNodeID)
    {
    this->SetXRayVolumeNodeID(NULL);
    }

  if (this->LinearTransformNodeID)
    {
    this->SetLinearTransformNodeID(NULL);
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
    if (!strcmp(attName, "xRayVolumeNodeID"))
    {
      this->SetXRayVolumeNodeID(attValue);
      continue;
    }
    if (!strcmp(attName, "linearTransformNodeID"))
    {
      this->SetLinearTransformNodeID(attValue);
      continue;
    }
/*    if (!strcmp(attName,"intensityDivider"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->IntensityDivider;
      continue;
    }
    if (!strcmp(attName,"focalWidth"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->FocalWidth;
      continue;
    }
*/
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
  of << indent << " xRayVolumeNodeID=\"" << (this->XRayVolumeNodeID ? this->XRayVolumeNodeID : "NULL") << "\"";
  of << indent << " linearTransformNodeID=\"" << (this->LinearTransformNodeID ? this->LinearTransformNodeID : "NULL") << "\"";
/*  of << indent << " intensityDivider=\"" << this->IntensityDivider << "\"";
  of << indent << " focalWidth=\"" << this->FocalWidth << "\"";
*/
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
  this->SetLinearTransformNodeID(node->GetLinearTransformNodeID());
/*  this->SetIntensityDivider(node->GetIntensityDivider());
  this->SetFocalWidth(node->GetFocalWidth());
*/
  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLReg2D3DParametersNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << "InputVolumeNodeID: " << ( (this->InputVolumeNodeID) ? this->InputVolumeNodeID : "None" ) << "\n";
  os << "OutputVolumeNodeID: " << ( (this->OutputVolumeNodeID) ? this->OutputVolumeNodeID : "None" ) << "\n";
  os << "XRayVolumeNodeID: " << ( (this->XRayVolumeNodeID) ? this->XRayVolumeNodeID : "None" ) << "\n";
  os << "LinearTransformNodeID: " << ( (this->LinearTransformNodeID) ? this->LinearTransformNodeID : "None" ) << "\n";
/*  os << "IntensityDivider: " << this->IntensityDivider << "\n";
  os << "FocalWidth: " << this->FocalWidth << "\n";
*/}

// End

