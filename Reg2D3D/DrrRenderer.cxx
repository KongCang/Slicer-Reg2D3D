#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

typedef unsigned int uint;
typedef unsigned char uchar;

#include "DrrRendererRayCastingGpuCUDA.cuh"
#include "vtkMRMLTransformableNode.h"

#include "DrrRenderer.h"
#include "Geometry.h"

///////////////////////////////////
/// \brief DrrRenderer::DrrRenderer
/// \param objectNode
/// \param machine
///////////////////////////////////


DrrRenderer::DrrRenderer(vtkMRMLScalarVolumeNode* objectNode, Geometry* machine) {

    mNode=objectNode;
    mMachine=machine;

    //ShiftScale();   //Rescale for best contrast

    mVolumeData=objectNode->GetImageData();

}

/////////////////////////////////////
/// \brief DrrRenderer::ShiftScale
///

void DrrRenderer::ShiftScale(){

    vtkSmartPointer<vtkImageData> id=mNode->GetImageData();     //Here's the data

    double iRange[2];
    id->GetScalarRange(iRange);            //smallest and biggest value in volume

    vtkSmartPointer<vtkImageShiftScale> ShiftScale =
       vtkSmartPointer<vtkImageShiftScale>::New();

    ShiftScale->SetInputData(id);
    ShiftScale->SetShift(-iRange[0]);     //shift, so that all values are >= 0
    ShiftScale->Update();
    ShiftScale->SetOutputScalarTypeToUnsignedShort();   //Change data type to unsigned short int

    ShiftScale->Update();
    ShiftScale->SetScale(65535.0/(iRange[1]-iRange[0]));    //Use whole range of unsigned short int
    ShiftScale->Update();

    mNode->SetAndObserveImageData(ShiftScale->GetOutput()); //Apply ShiftScale

    /*id=mNode->GetImageData();             //Get changed data
    id->GetScalarRange(iRange);
    cerr << "nach ShiftScale: kleinster Wert: " << iRange[0] << ", groester Wert: " << iRange[1] << endl;
*/
}

DrrRenderer::~DrrRenderer() {}

void DrrRenderer::computeDrr(vtkImageData* resultImage){

    //Check if 2D square image
    int iResultDims[3];
    resultImage->GetDimensions(iResultDims);  //Side length of computed image
    if (iResultDims[0]==iResultDims[1]&&iResultDims[2]==1)
        this->imageSize=iResultDims[0];
    else {
        std::cerr << "No square image!!";
        return;
    }

    //Prepare result image
    mpResultImage=static_cast<unsigned short int*>(resultImage->GetScalarPointer(0,0,0));

    //Check Volume and prepare for CUDA
    int iVolumeDims[3];
    mVolumeData->GetDimensions(iVolumeDims);
    dim3 block_size;
    dim3 blocks;
    block_size = dim3(16,16);
    blocks = dim3(iDivUp(imageSize, block_size.x), iDivUp(imageSize, block_size.y));

    //Pointer on first pixel of Volume
    unsigned short int* iPixel = static_cast<unsigned short int*>(mVolumeData->GetScalarPointer(0,0,0));
    cerr << "Step2\n";
    float transferFunc[] =
    {0.0833, 0.1667, 0.2500, 0.3334, 0.4167, 0.5000, 0.5834, 0.6668, 0.7501, 0.8335, 0.9168, 1}; // initial values

    drrRendererRayCastingCuda_updateTransferFunction(transferFunc);
    drrRendererRayCastingCuda_prepareVolumeUS(this->imageSize, iPixel, iVolumeDims[0], iVolumeDims[1], iVolumeDims[2], NULL, 0); //NULL, 0 are dummies
    int intensityDivider=mMachine->GetIntensityDivider();
    bool useMask1,useMask2;
    useMask1=useMask2=false;
    short currentMask=0;
    float imagePixelSize=1.;
cerr << "Step4\n";

    //Prepare TransformMatrix
    float elems[16];
    vtkSmartPointer<vtkMatrix4x4> pMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    mNode->GetParentTransformNode()->GetMatrixTransformToWorld(pMatrix);
    int k=0;
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            elems[k++]=pMatrix->GetElement(i,j);
            cerr << elems[4*i+j] << " ";
        }
         cerr << "\n";
    }

cerr << "Step5\n";

    //CUDA part
    drrRendererRayCastingCuda_computeMatrix(elems, mMachine->GetFocalWidth());
    render_kernelUS(blocks, block_size, mpResultImage, this->imageSize, imagePixelSize, mMachine->GetFocalWidth(), useMask1, useMask2, 0, imageSize-1, 0, imageSize-1, intensityDivider, currentMask);
    drrRendererRayCastingCuda_free();

}

void DrrRenderer::computeDRRCuda(){

}


