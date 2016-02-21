#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

typedef unsigned int uint;
typedef unsigned char uchar;

#include "DrrRendererRayCastingGpuCUDA.cuh"

#include "DrrRenderer.h"
#include "Geometry.h"


DrrRenderer::DrrRenderer(){};

DrrRenderer::DrrRenderer(vtkMRMLScalarVolumeNode* objectNode, Geometry* machine) {

    mVolumeData=objectNode->GetImageData();

    int testdims[3];
    mVolumeData->GetDimensions(testdims);

    cerr << testdims[0] << ", " << testdims[1] << ", "<< testdims[2] << endl;

    mMachine=machine;
    //Rescale for best contrast
    vtkSmartPointer<vtkImageData> id=objectNode->GetImageData();     //Here's the data

    vtkSmartPointer<vtkImageShiftScale> ShiftScale =
      vtkSmartPointer<vtkImageShiftScale>::New();

    double iRange[2];       //smallest and biggest value
    id->GetScalarRange(iRange);

    if (iRange[1]-iRange[0]<256*256){
        ShiftScale->SetInputData(id);
        ShiftScale->SetShift(-iRange[0]);
        ShiftScale->Update();
        ShiftScale->SetOutputScalarTypeToUnsignedShort();
        ShiftScale->SetScale(65535.0/(iRange[1]-iRange[0]));
        ShiftScale->Update();

        objectNode->SetAndObserveImageData(ShiftScale->GetOutput());
        mVolumeData=objectNode->GetImageData();
    }
    mVolumeData->GetDimensions(testdims);

    cerr << "Finished preparing: " << testdims[0] << ", " << testdims[1] << ", "<< testdims[2] << endl;

}

DrrRenderer::~DrrRenderer() {}

void DrrRenderer::computeDrr(vtkImageData* resultImage){

    int iResultDims[3];
    resultImage->GetDimensions(iResultDims);  //Side length of computed image
    if (iResultDims[0]==iResultDims[1]&&iResultDims[2]==1)
        imageSize=iResultDims[0];
    else {
        std::cerr << "No square image!!";
        return;
    }

    mpResultImage=static_cast<unsigned short int*>(resultImage->GetScalarPointer(0,0,0));

    int iVolumeDims[3];
    mVolumeData->GetDimensions(iVolumeDims);

    dim3 block_size;
    dim3 blocks;
    block_size = dim3(16,16);
    blocks = dim3(iDivUp(imageSize, block_size.x), iDivUp(imageSize, block_size.y));

    //Pointer on first pixel of Volume
    unsigned short int* iPixel = static_cast<unsigned short int*>(mVolumeData->GetScalarPointer(0,0,0));

    float transferFunc[] =
    {0.0833, 0.1667, 0.2500, 0.3334, 0.4167, 0.5000, 0.5834, 0.6668, 0.7501, 0.8335, 0.9168, 1}; // initial values

    cerr << "before TransferFunction" << endl;
    drrRendererRayCastingCuda_updateTransferFunction(transferFunc);
    cerr << "before PrepareVolume" << this->imageSize << iVolumeDims[2] << endl;
    drrRendererRayCastingCuda_prepareVolumeUS(this->imageSize, iPixel, iVolumeDims[0], iVolumeDims[1], iVolumeDims[2], NULL, 0); //0, 0 are dummies
//    updateTransferFunction(transferFunc);
//    prepareVolumeUS(this->imageSize, iPixel, iDims[0], iDims[1], iDims[2], 0, 0); //0, 0 are dummies
    //    void (dim3 gridSize, dim3 blockSize, unsigned short *d_output, uint imageSize, float imagePixelSpacing, float focal_width, bool useMask1, bool useMask2, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider, short currentMask);
    int intensityDivider=mMachine->GetIntensityDivider();
    bool useMask1,useMask2;
    useMask1=useMask2=NULL;
    short currentMask=0;
    float imagePixelSize=1.;
    cerr << "before KernelUS and IntensityDivider=" << intensityDivider << endl;

    //computerMatrix: place here
    float elems[] = {  1,  0,  0,  0,
                       0,  1,  0,  0,
                       0,  0,  1,  0,
                       0,  0,  0,  1};
    drrRendererRayCastingCuda_computeMatrix(elems, mMachine->GetFocalWidth());
    render_kernelUS(blocks, block_size, mpResultImage, this->imageSize, imagePixelSize, mMachine->GetFocalWidth(), useMask1, useMask2, 0, imageSize-1, 0, imageSize-1, intensityDivider, currentMask);

    //int iDims[3];
    //objectNode->GetImageData()->GetDimensions(iDims);



}

