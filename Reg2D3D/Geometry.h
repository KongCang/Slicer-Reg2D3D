#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vtkMatrix4x4.h"
#include <iostream>

class Geometry
{
public:
    Geometry(float focal_width, int intensityDivider, vtkMatrix4x4* pMatrix) {this->focal_width=focal_width;this->intensityDivider=intensityDivider;this->pMatrix=pMatrix;}
    ~Geometry() {}
    float GetFocalWidth(){return focal_width;}
    void  SetFocalWidth(float focal_width) {this->focal_width=focal_width;}
    int   GetIntensityDivider(){return intensityDivider;}
    void  SetIntensityDivider(int intensityDivider){this->intensityDivider=intensityDivider;}
    void  GetMatrix(vtkMatrix4x4* pMatrix ){pMatrix=this->pMatrix; }
    void  SetMatrix(vtkMatrix4x4* pMatrix ){this->pMatrix=pMatrix; }

private:
    float focal_width;
    int intensityDivider;
    vtkMatrix4x4* pMatrix;
    //float* pMatrix;
};




#endif //GEOMETRY_H
