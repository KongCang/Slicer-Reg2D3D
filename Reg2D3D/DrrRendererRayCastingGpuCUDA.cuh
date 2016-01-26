#include <cuda_runtime.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>
#include <cuda_gl_interop.h>

void drrRendererRayCastingCuda_prepareVolumeUS(unsigned short imageSize, unsigned short* volume, int volumeWidth, int volumeHeight, int volumeDepth, float* additionalParameters, unsigned char additionalParameterLength);
void drrRendererRayCastingCuda_updateTransferFunction(float *transferFunc);
void drrRendererRayCastingCuda_updateTransferFunctionCam2(float *transferFunc);
void drrRendererRayCastingCuda_prepareMask1(unsigned int xSize, unsigned int ySize, unsigned char *datatex);
void drrRendererRayCastingCuda_prepareMask2(unsigned int xSize, unsigned int ySize, unsigned char *datatex);
void drrRendererRayCastingCuda_free();
void render_kernelUS(dim3 gridSize, dim3 blockSize, unsigned short *d_output, uint imageSize, float imagePixelSpacing, float focal_width, bool useMask1, bool useMask2, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider, short currentMask);
void renderSlice_kernelUS(dim3 gridSize, dim3 blockSize, unsigned short *d_output, uint imageSize, bool useMask1, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider);
void drrRendererRayCastingCuda_computeMatrix(float *CGInvModelViewMatrix, float focalWith);
