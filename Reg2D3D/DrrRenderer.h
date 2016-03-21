#ifndef DRRRENDERER_H
#define DRRRENDERER_H

#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkSmartPointer.h>

#include "Geometry.h"

class DrrRenderer
{
public:
    DrrRenderer();
    DrrRenderer(vtkMRMLScalarVolumeNode* objectNode, Geometry* machine);
    ~DrrRenderer();
    void computeDrr(vtkImageData*);
    int iDivUp(int a, int b){return (a % b != 0) ? (a / b + 1) : (a / b);}   // IntegerDivision rounding up
    void ShiftScale();


private:
    vtkMRMLScalarVolumeNode* mNode; // Point to SlicerNode
    vtkImageData* mVolumeData;      // Contains the Volume Data
    unsigned short imageSize;       // side length of computed image (square)
    Geometry* mMachine;             // Contains the geometry data
    unsigned short* mpResultImage;  // Pointer to resulting Image
};




#endif //DRRRENDERER_H
