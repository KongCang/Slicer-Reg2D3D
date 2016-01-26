/*==============================================================================

  Program: Slicer-Reg2D3D-Module

CalculateMeritFctMutualInformation
Compares to images of samewidth, height and image depth

==============================================================================*/
#ifndef __ImageComparerMutualInformation_h
#define __ImageComparerMutualInformation_h
typedef unsigned short imageType;
typedef unsigned short T;


double CalculateMeritFctMutualInformation(imageType *mImageBase, imageType *mImageMatch, unsigned short width, unsigned short height, unsigned short depth);
#endif

