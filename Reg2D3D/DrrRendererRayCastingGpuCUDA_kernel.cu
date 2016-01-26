/*
 *
 *  Insert comment here
 *
 */

#define DEBUG 0

// CUDA raycasting

#ifndef _VOLUMERENDERCUSTOM_KERNEL_CU_
#define _VOLUMERENDERCUSTOM_KERNEL_CU_

#include <helper_cuda.h>
#include <helper_math.h>

typedef unsigned int  uint;
typedef unsigned char uchar;

// output images
__constant__ unsigned short *image_raw_d_us;
__constant__ unsigned short *image_raw_d_mask1_us;
__constant__ unsigned short *image_raw_d_mask2_us;

// volume array and texture
cudaArray *d_volumeArray = 0;
texture<unsigned short, 3, cudaReadModeNormalizedFloat> texUS;

// mask arrays and textures for cam 1 & 2
cudaArray *d_mask1Array = 0;
texture<unsigned char, 2, cudaReadModeElementType> texMask1;

cudaArray *d_mask2Array = 0;
texture<unsigned char, 2, cudaReadModeElementType> texMask2;

// transfer function arrays for cam 1 & 2
cudaArray *d_transferFuncArray = 0;
texture<float, 1, cudaReadModeElementType> transferTex; // 1D transfer function texture

cudaArray *d_transferFuncArray_cam2 = 0;
texture<float, 1, cudaReadModeElementType> transferTexCam2; // 1D transfer function texture


unsigned short width, height, depth;
long voxel_number;


typedef struct {
    float4 m[3];
} float3x4;

__constant__ float3x4 myInvModelViewMatrix;

struct Ray {
        float3 o;	// origin
	float3 d;	// direction
};

// intersect ray with a box
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

__device__
int intersectBox(Ray r, float3 boxmin, float3 boxmax, float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    float3 invR = make_float3(1.0f) / r.d;
    float3 tbot = invR * (boxmin - r.o);
    float3 ttop = invR * (boxmax - r.o);

    // re-order intersections to find smallest and largest on each axis
    float3 tmin = fminf(ttop, tbot);
    float3 tmax = fmaxf(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = fmaxf(fmaxf(tmin.x, tmin.y), fmaxf(tmin.x, tmin.z));
    float smallest_tmax = fminf(fminf(tmax.x, tmax.y), fminf(tmax.x, tmax.z));

	*tnear = largest_tmin;
	*tfar = smallest_tmax;

	return smallest_tmax > largest_tmin;
}

// transform vector by matrix (no translation)
__device__
float3 mul(const float3x4 &M, const float3 &v)
{
    float3 r;
    r.x = dot(v, make_float3(M.m[0]));
    r.y = dot(v, make_float3(M.m[1]));
    r.z = dot(v, make_float3(M.m[2]));
    return r;

}

// transform vector by matrix with translation
__device__
float4 mul(const float3x4 &M, const float4 &v)
{
    float4 r;
    r.x = dot(v, M.m[0]);
    r.y = dot(v, M.m[1]);
    r.z = dot(v, M.m[2]);
    r.w = 1.0f;
    return r;
}

void checkCUDAError(const char* msg) {
cudaError_t err = cudaGetLastError();
  if (cudaSuccess != err) {
    fprintf(stderr, "Cuda error(%d): %s: %s.\n", err, msg, cudaGetErrorString(err));
    exit(EXIT_FAILURE);
  }
#if DEBUG
  else
      printf("%s - OK.\n", msg);
#endif
}

__global__ void
d_renderUS(unsigned short *d_output, uint imageSize, float focal_width, float imagePixelSpacing, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider, short currentMask)
{
    const int maxSteps = 50000;
    const float tstep = 0.00606f;
    const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f);
    const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f);


    uint x = blockIdx.x*blockDim.x + threadIdx.x;
    uint y = blockIdx.y*blockDim.y + threadIdx.y;

    if (x >= imageSize || y >= imageSize)
        return;

    // scale the coordinates to be between -1 and 1
    float u = (x / (float) imageSize)*2.0f-1.0f;
    float v = (y / (float) imageSize)*2.0f-1.0f;

    // calculate eye ray in world space
    Ray eyeRay;
    eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(0.0f, 0.0f, 0.0f, 1.0f)));
    //eyeRay.d = normalize(make_float3(u, v, -1.0f * ( (410.0f / 264.0f) * 2.0*focal_width/(float)imageSize)/imagePixelSpacing)); // ((float)nVoxX/(voxSpacingZ*(float)nVoxZ))
    eyeRay.d = normalize(make_float3(u, v, -1.0f * ( 2.0*focal_width/(float)imageSize)/imagePixelSpacing)); // ((float)nVoxX/(voxSpacingZ*(float)nVoxZ))
    eyeRay.d = mul(myInvModelViewMatrix, eyeRay.d);
    //eyeRay.d.z = eyeRay.d.z * 410.0f / 264.0f; // FIXME: all these values are hardcoded, must now fix this
    eyeRay.d.z = eyeRay.d.z; // FIXME: all these values are hardcoded, must now fix this

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay, boxMin, boxMax, &tnear, &tfar);
    if (!hit) return;
    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from front to back, accumulating color
    //float4 sum = make_float4(0.0f);
    float unisum = 0;
    float t = tnear;
    float3 pos = eyeRay.o + eyeRay.d*tnear;
    float3 step = eyeRay.d*tstep;
    //step.z = step.z * 410.0f / 264.0f;
    step.z = step.z;

    for(int i=0; i<maxSteps; i++) {

        float sample = tex3D(texUS, pos.x*0.5f+0.5f, pos.y*0.5f+0.5f, pos.z*0.5f+0.5f);

        unisum += sample/(float)intensityDivider;

        t += tstep;
        if (t > tfar) break;

        pos += step;
    }

    if(currentMask == 0)    // rendering cam1
        unisum = tex1D(transferTex, unisum) * 65535;
    else                    // rendering cam2
        unisum = tex1D(transferTexCam2, unisum) * 65535;

    // write to the output buffer
    d_output[y*imageSize + x] = ushort(unisum);

}

__global__ void
d_renderUS_mask1(unsigned short *d_output, uint imageSize, float focal_width, float imagePixelSpacing, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider)
{
    const int maxSteps = 50000;
    const float tstep = 0.00606f;
    const float opacityThreshold = 242.0f; // 0.95 * 255
    const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f);
    const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f);


    uint x = blockIdx.x*blockDim.x + threadIdx.x + minX;
    uint y = blockIdx.y*blockDim.y + threadIdx.y + minY;

    if( tex2D(texMask1, x, y) == uchar(0) )
        return;

    // scale the coordinates to be between -1 and 1
    float u = (x / (float) imageSize)*2.0f-1.0f;
    float v = (y / (float) imageSize)*2.0f-1.0f;

    // calculate eye ray in world space
    Ray eyeRay;
    eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(0.0f, 0.0f, 0.0f, 1.0f)));
    //eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(u, v, -1.0f * (focal_width/(float)imageSize), 1.0f))); // cast from the screen instead from 0
    eyeRay.d = normalize(make_float3(u, v, -1.0f * (2.0*focal_width/(float)imageSize)/imagePixelSpacing));
    eyeRay.d = mul(myInvModelViewMatrix, eyeRay.d);

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay, boxMin, boxMax, &tnear, &tfar);
    if (!hit) return;
    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from front to back, accumulating color
    float4 sum = make_float4(0.0f);
    float unisum = 0;
    float t = tnear;
    float3 pos = eyeRay.o + eyeRay.d*tnear;
    float3 step = eyeRay.d*tstep;

    //float sample = 0;

    for(int i=0; i<maxSteps; i++) {

        float sample = tex3D(texUS, pos.x*0.5f+0.5f, pos.y*0.5f+0.5f, pos.z*0.5f+0.5f);

        unisum += sample/(float)intensityDivider;

        // exit early if opaque
        //if (unisum > (opacityThreshold))
        //    break;

        t += tstep;
        if (t > tfar) break;

        pos += step;
    }

    unisum = tex1D(transferTex, unisum) * 65535;

    // write to the output buffer
    d_output[y*imageSize + x] = ushort(unisum);

    // Test pattern.
    //d_output[y*imageSize + x] = uchar( ((x/(float)imageSize)/2 + (y/(float)imageSize)/2 )*255);
}

__global__ void
d_renderUS_mask2(unsigned short *d_output, uint imageSize, float focal_width, float imagePixelSpacing, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider)
{
    const int maxSteps = 50000;
    const float tstep = 0.00606f;
    const float opacityThreshold = 242.0f; // 0.95 * 255
    const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f);
    const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f);


    uint x = blockIdx.x*blockDim.x + threadIdx.x + minX;
    uint y = blockIdx.y*blockDim.y + threadIdx.y + minY;

    if( tex2D(texMask2, x, y) == uchar(0) )
        return;

    // scale the coordinates to be between -1 and 1
    float u = (x / (float) imageSize)*2.0f-1.0f;
    float v = (y / (float) imageSize)*2.0f-1.0f;

    // calculate eye ray in world space
    Ray eyeRay;
    eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(0.0f, 0.0f, 0.0f, 1.0f)));
    //eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(u, v, -1.0f * (focal_width/(float)imageSize), 1.0f))); // cast from the screen instead from 0
    eyeRay.d = normalize(make_float3(u, v, -1.0f * (2.0*focal_width/(float)imageSize)/imagePixelSpacing));
    eyeRay.d = mul(myInvModelViewMatrix, eyeRay.d);

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay, boxMin, boxMax, &tnear, &tfar);
    if (!hit) return;
    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from front to back, accumulating color
    float4 sum = make_float4(0.0f);
    float unisum = 0;
    float t = tnear;
    float3 pos = eyeRay.o + eyeRay.d*tnear;
    float3 step = eyeRay.d*tstep;

    //float sample = 0;

    for(int i=0; i<maxSteps; i++) {

        float sample = tex3D(texUS, pos.x*0.5f+0.5f, pos.y*0.5f+0.5f, pos.z*0.5f+0.5f);

        unisum += sample/(float)intensityDivider;

        // exit early if opaque
        //if (unisum > (opacityThreshold))
        //    break;

        t += tstep;
        if (t > tfar) break;

        pos += step;
    }

    unisum = tex1D(transferTexCam2, unisum) * 65535;

    // write to the output buffer
    d_output[y*imageSize + x] = ushort(unisum);

    // Test pattern.
    //d_output[y*imageSize + x] = uchar( ((x/(float)imageSize)/2 + (y/(float)imageSize)/2 )*255);
}

__global__ void
d_renderSliceUS(unsigned short *d_output, uint imageSize, bool useMask1, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider)
{
    const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f);
    const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f);

    uint x = blockIdx.x*blockDim.x + threadIdx.x;
    uint y = blockIdx.y*blockDim.y + threadIdx.y;

    // scale the coordinates to be between -1 and 1
    float u = (x / (float) imageSize)*2.0f-1.0f;
    float v = (y / (float) imageSize)*2.0f-1.0f;

    // calculate eye ray in world space
    Ray eyeRay;
    eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(u, v, 0.0f, 1.0f)));
    eyeRay.d = normalize(make_float3(u, v, 0));
    eyeRay.d = mul(myInvModelViewMatrix, eyeRay.d);

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay, boxMin, boxMax, &tnear, &tfar);
    if (!hit) return;
    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from front to back, accumulating color
    //float3 pos = eyeRay.o + eyeRay.d*tnear;
    float3 pos = eyeRay.o;

    float sample = tex3D(texUS, pos.x*0.5f+0.5f, pos.y*0.5f+0.5f, pos.z*0.5f+0.5f);

    float unisum = tex1D(transferTex, sample) * 65535;

    // write to the output buffer
    d_output[y*imageSize + x] = ushort(unisum);
}

__global__ void
d_renderSliceUS_mask1(unsigned short *d_output, uint imageSize, bool useMask1, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider)
{
    const float3 boxMin = make_float3(-1.0f, -1.0f, -1.0f);
    const float3 boxMax = make_float3(1.0f, 1.0f, 1.0f);

    uint x = blockIdx.x*blockDim.x + threadIdx.x + minX;
    uint y = blockIdx.y*blockDim.y + threadIdx.y + minY;

    if( tex2D(texMask1, x, y) == uchar(0) )
        return;

    // scale the coordinates to be between -1 and 1
    float u = (x / (float) imageSize)*2.0f-1.0f;
    float v = (y / (float) imageSize)*2.0f-1.0f;

    // calculate eye ray in world space
    Ray eyeRay;
    eyeRay.o = make_float3(mul(myInvModelViewMatrix, make_float4(u, v, 0.0f, 1.0f)));
    eyeRay.d = normalize(make_float3(u, v, 0));
    eyeRay.d = mul(myInvModelViewMatrix, eyeRay.d);

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay, boxMin, boxMax, &tnear, &tfar);
    if (!hit) return;
    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from front to back, accumulating color
    float3 pos = eyeRay.o + eyeRay.d*tnear;



    float unisum = tex3D(texUS, pos.x*0.5f+0.5f, pos.y*0.5f+0.5f, pos.z*0.5f+0.5f);

    unisum = tex1D(transferTex, unisum) * 65535;

    // write to the output buffer
    d_output[y*imageSize + x] = ushort(unisum);
}

void drrRendererRayCastingCuda_updateTransferFunction(float *transferFunc)
{
    size_t tFunc_size = 12;
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();

    cudaMallocArray( &d_transferFuncArray, &channelDesc, tFunc_size, 1);

    cudaMemcpyToArray( d_transferFuncArray, 0, 0, transferFunc, tFunc_size*sizeof(float), cudaMemcpyHostToDevice);

    transferTex.filterMode = cudaFilterModeLinear;
    transferTex.normalized = true;    // access with normalized texture coordinates
    transferTex.addressMode[0] = cudaAddressModeClamp;   // wrap texture coordinates

    // Bind the array to the texture
    cudaBindTextureToArray( transferTex, d_transferFuncArray, channelDesc);
}

void drrRendererRayCastingCuda_updateTransferFunctionCam2(float *transferFunc)
{
    size_t tFunc_size = 12;
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();

    cudaMallocArray( &d_transferFuncArray_cam2, &channelDesc, tFunc_size, 1);

    cudaMemcpyToArray( d_transferFuncArray_cam2, 0, 0, transferFunc, tFunc_size*sizeof(float), cudaMemcpyHostToDevice);

    transferTexCam2.filterMode = cudaFilterModeLinear;
    transferTexCam2.normalized = true;    // access with normalized texture coordinates
    transferTexCam2.addressMode[0] = cudaAddressModeClamp;   // wrap texture coordinates

    // Bind the array to the texture
    cudaBindTextureToArray( transferTexCam2, d_transferFuncArray_cam2, channelDesc);
}

//===================================================================
//
// New copy function to CUDA arrays
//
//===================================================================
void drrRendererRayCastingCuda_prepareVolumeUS(unsigned short imageSize,
    unsigned short* volume, int volumeWidth, int volumeHeight,
    int volumeDepth, float* additionalParameters, unsigned char additionalParameterLength)
{
    #if DEBUG
        printf("drrRendererRayCastingCuda_prepareVolumeUS() called...\n");
    #endif

    checkCUDAError("Before Preparing volume");

    // initialize data and pointer
    voxel_number = (volumeWidth * volumeHeight * volumeDepth);
    size_t image_size = (imageSize * imageSize) * sizeof(unsigned short);

    cudaExtent extentvolumeSize = make_cudaExtent(volumeWidth, volumeHeight, volumeDepth);
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<unsigned short>();

    cudaMalloc3DArray(&d_volumeArray, &channelDesc, extentvolumeSize);

    checkCUDAError("After malloc");

    // copy data to 3D array
    cudaMemcpy3DParms copyParams = {0};
    copyParams.srcPtr   = make_cudaPitchedPtr(volume, extentvolumeSize.width*sizeof(unsigned short), extentvolumeSize.width, extentvolumeSize.height);
    copyParams.dstArray = d_volumeArray;
    copyParams.extent   = extentvolumeSize;
    copyParams.kind     = cudaMemcpyHostToDevice;
    cudaMemcpy3D(&copyParams);

    // set texture parameters
    texUS.normalized = true;                      // access with normalized texture coordinates
    texUS.filterMode = cudaFilterModeLinear;      // linear interpolation
    texUS.addressMode[0] = cudaAddressModeClamp;  // clamp texture coordinates
    texUS.addressMode[1] = cudaAddressModeClamp;

    // bind array to 3D texture
    cudaBindTextureToArray(texUS, d_volumeArray, channelDesc);

    checkCUDAError("After binding texture");

    cudaMalloc((void **) &image_raw_d_us, image_size);
    cudaMemset(image_raw_d_us, 0, image_size);

    cudaMalloc((void **) &image_raw_d_mask1_us, image_size);
    cudaMemset(image_raw_d_mask1_us, 0, image_size);

    cudaMalloc((void **) &image_raw_d_mask2_us, image_size);
    cudaMemset(image_raw_d_mask2_us, 0, image_size);

    checkCUDAError("After preparing volume");

}

void drrRendererRayCastingCuda_prepareMask1(unsigned int xSize, unsigned int ySize, unsigned char *datatex)
{

#if DEBUG
    printf("drrRendererRayCastingCuda_prepareMask1() called...\n");
#endif

    cudaMemset(image_raw_d_mask1_us, 0, xSize*ySize*sizeof(unsigned short));

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<unsigned char>();
    cudaMallocArray(&d_mask1Array, &channelDesc, xSize, ySize);

    cudaMemcpyToArray(d_mask1Array, 0, 0, datatex, xSize*ySize, cudaMemcpyHostToDevice);

    // set texture parameters
    texMask1.normalized = false;                      // access with normalized texture coordinates
    texMask1.filterMode = cudaFilterModePoint;      // linear interpolation
    texMask1.addressMode[0] = cudaAddressModeWrap;  // clamp texture coordinates
    texMask1.addressMode[1] = cudaAddressModeWrap;
    cudaBindTextureToArray(texMask1, d_mask1Array, channelDesc);

    #if DEBUG
        cudaError_t err = cudaGetLastError();
        if (err != 0)
        {
            printf("Error creating texture, code is %d\n", err);
            exit(1);
        }
    #endif
}

void drrRendererRayCastingCuda_prepareMask2(unsigned int xSize, unsigned int ySize, unsigned char *datatex)
{

#if DEBUG
    printf("drrRendererRayCastingCuda_prepareMask2() called...\n");
#endif

    cudaMemset(image_raw_d_mask2_us, 0, xSize*ySize*sizeof(unsigned short));

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<unsigned char>();
    cudaMallocArray(&d_mask2Array, &channelDesc, xSize, ySize);

    cudaMemcpyToArray(d_mask2Array, 0, 0, datatex, xSize*ySize, cudaMemcpyHostToDevice);

    // set texture parameters
    texMask1.normalized = false;                      // access with normalized texture coordinates
    texMask1.filterMode = cudaFilterModePoint;      // linear interpolation
    texMask1.addressMode[0] = cudaAddressModeWrap;  // clamp texture coordinates
    texMask1.addressMode[1] = cudaAddressModeWrap;
    cudaBindTextureToArray(texMask2, d_mask2Array, channelDesc);

    #if DEBUG
        cudaError_t err = cudaGetLastError();
        if (err != 0)
        {
            printf("Error creating texture, code is %d\n", err);
            exit(1);
        }
    #endif
}

void drrRendererRayCastingCuda_computeMatrix(float *invModelViewMatrix, float focalWith)
{
    #if DEBUG
        printf("drrRendererRayCastingCuda_computeMatrix() called...\n");
    #endif

#if DEBUG
    checkCUDAError("Before copying matrix");
#endif

    // copy transformation data to board
    size_t inv_transformation_data_size = (12) * sizeof(float);
    cudaMemcpyToSymbol(myInvModelViewMatrix, invModelViewMatrix, inv_transformation_data_size);

#if DEBUG
    checkCUDAError("After copying matrix");
#endif

}

void drrRendererRayCastingCuda_free()
{
    #if DEBUG
        printf("drrRendereRayCastingCuda_free() called...\n");
    #endif

    // free all cuda pointers
    cudaFreeArray(d_volumeArray);
    cudaFreeArray(d_transferFuncArray);
    cudaFree(image_raw_d_us);
    cudaFree(image_raw_d_mask1_us);
    cudaFree(image_raw_d_mask2_us);

    cudaFreeArray(d_mask1Array);
    cudaFreeArray(d_mask2Array);

    checkCUDAError("After freeing memory");

}

void render_kernelUS(dim3 gridSize, dim3 blockSize, unsigned short *d_output, uint imageSize, float imagePixelSpacing, float focal_width, bool useMask1, bool useMask2, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider, short currentMask)
{
    #if DEBUG
        printf("render_kernelUS() called...\n");
        checkCUDAError("Before render kernel");
    #endif

    if(useMask1 && currentMask == 0) // Using mask1 and rendering Cam1
    {
        d_renderUS_mask1<<<gridSize, blockSize>>>( image_raw_d_mask1_us, imageSize, focal_width, imagePixelSpacing, minX, maxX, minY, maxY, intensityDivider);
        cudaMemcpy(d_output+(minY)*imageSize, image_raw_d_mask1_us+(minY)*imageSize, sizeof(unsigned short)*imageSize*(maxY-minY+1), cudaMemcpyDeviceToHost);
    }
    else if(useMask2 && currentMask == 1) // Using mask2 and rendering Cam2
    {
        d_renderUS_mask2<<<gridSize, blockSize>>>( image_raw_d_mask2_us, imageSize, focal_width, imagePixelSpacing, minX, maxX, minY, maxY, intensityDivider);
        cudaMemcpy(d_output+(minY)*imageSize, image_raw_d_mask2_us+(minY)*imageSize, sizeof(unsigned short)*imageSize*(maxY-minY+1), cudaMemcpyDeviceToHost);
    }
    else // Rendering Cam1 or Cam2 without any mask
    {
        d_renderUS<<<gridSize, blockSize>>>( image_raw_d_us, imageSize, focal_width, imagePixelSpacing, minX, maxX, minY, maxY, intensityDivider, currentMask);
        cudaMemcpy(d_output, image_raw_d_us, sizeof(unsigned short)*imageSize*imageSize, cudaMemcpyDeviceToHost);
    }

    #if DEBUG
        checkCUDAError("After render kernel");
    #endif

}

void renderSlice_kernelUS(dim3 gridSize, dim3 blockSize, unsigned short *d_output, uint imageSize, bool useMask1, uint minX, uint maxX, uint minY, uint maxY, int intensityDivider)
{
    #if DEBUG
        printf("renderSlice_kernelUS() called...\n");
        checkCUDAError("Before render kernel");
    #endif

    if(useMask1) // Rendering with a mask
    {
        d_renderSliceUS_mask1<<<gridSize, blockSize>>>( image_raw_d_mask1_us, imageSize, useMask1, minX, maxX, minY, maxY, intensityDivider);
        cudaMemcpy(d_output+(minY)*imageSize, image_raw_d_mask1_us+(minY)*imageSize, sizeof(unsigned short)*imageSize*(maxY-minY+1), cudaMemcpyDeviceToHost);
    }
    else // Rendering without a mask
    {
        d_renderSliceUS<<<gridSize, blockSize>>>( image_raw_d_us, imageSize, useMask1, minX, maxX, minY, maxY, intensityDivider);
        cudaMemcpy(d_output, image_raw_d_us, sizeof(unsigned short)*imageSize*imageSize, cudaMemcpyDeviceToHost);
    }

    #if DEBUG
        checkCUDAError("After slice render kernel");
    #endif

}

#endif // #ifndef _VOLUMERENDER_KERNEL_CU_
