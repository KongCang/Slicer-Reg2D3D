
#include "DrrRendererRayCastingGpuCUDAThreadWorker.h"

#define DEBUG 0


DrrRendererRayCastingGpuCUDAThreadWorker::DrrRendererRayCastingGpuCUDAThreadWorker(int tId, QSemaphore *semaphore) {

    this->threadId = tId;
    this->tSemaphore = semaphore;

    this->initializationDone = false;
    this->timesum = 0;
    this->timecounter = 0;

    std::cout << "Thread " << this->threadId << " Initializing rendering using CUDA" << std::endl;

    // CUDA stuff

    // check if the board has a pending error
    cudaError_t err = cudaGetLastError();
    if (err != 0)
    {
        std::cerr << "Thread constructor CUDA error, code is " << err << std::endl;
        exit(1);
    }
    cudaSetDevice(this->threadId);

    // check if cudaSetDevice was successfull
    err = cudaGetLastError();
    if (err != cudaSuccess && err != cudaErrorSetOnActiveProcess)
    {
        std::cerr << "Thread constructor CUDA error setting device, code is " << err << std::endl;
        exit(1);
    }

    this->initializationDone = true;

    // print the device name (just for debug)
    int device;
    struct cudaDeviceProp prop;
    cudaGetDevice( &device );
    cudaGetDeviceProperties( &prop, device );
    printf("Device nr. %d, name=%s\n", device, prop.name);

}

void DrrRendererRayCastingGpuCUDAThreadWorker::cleanGPU(void) {

    cudaError_t err = cudaGetLastError();
    if (err != 0)
    {
        std::cerr << "drrRendererRayCastingCuda_free error: pending error before clean, code is " << err << cudaGetErrorString(err) << std::endl;
        exit(1);
    }

    printf("T(%d) releasing CUDA memory\n", this->threadId);
    drrRendererRayCastingCuda_free();
    cudaThreadExit();

    // check if memory was cleaned successfully
    err = cudaGetLastError();
    if (err != 0)
    {
        std::cerr << "drrRendererRayCastingCuda_free error, code is " << err << std::endl;
        exit(1);
    }
    this->tSemaphore->release(1);
}

DrrRendererRayCastingGpuCUDAThreadWorker::~DrrRendererRayCastingGpuCUDAThreadWorker() {

    std::cout << "AVERAGE TIME:\t" << (double)timesum/(double)timecounter << std::endl;
    std::cout << "DrrRendererRayCastingGpuCUDAThreadWorker exiting..." << std::endl;
}

//=========================================================================
//
/// Set the Drr transfer function.
/// This method calculates a transfer function from the two thresholds
/// and transfers the result to the graphics card memory.
//
//=========================================================================
void DrrRendererRayCastingGpuCUDAThreadWorker::updateTransferFunction(int tFuncl, int tFunch, int cam)
{

    float transferFunc[] = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 // initialize the array
    };

    float A = 65535/((float)tFunch - (float)tFuncl);
    float B = -1 * (65535*(float)tFuncl/((float)tFunch-(float)tFuncl));
    float K = 65535.0 / 11.0;

    for(int i = 0 ; i < 12 ; i++)
    {
        if(i*K < tFuncl)
            transferFunc[i]= 0;
        else if(i*K > tFunch)
            transferFunc[i] = 1;
        else
            transferFunc[i] = (A*i*K+B)/65535.0;
    }

    if(cam == 1)
        drrRendererRayCastingCuda_updateTransferFunction(transferFunc);
    else
        drrRendererRayCastingCuda_updateTransferFunctionCam2(transferFunc);

    this->tSemaphore->release(1);
}

//=========================================================================
//
/// Main Preparation Method.
/// This method initializes all data structures required for the rendering.
/// At first, data is placed into a cube. The method then calls the CUDA
/// function to transfer the data to the graphics card memory.
//
//=========================================================================
void DrrRendererRayCastingGpuCUDAThreadWorker::createVolumeTexture(int thread, unsigned int imageSize, unsigned int volSizeX, unsigned int volSizeY, unsigned int volSizeZ, unsigned short *data)
{
    if(this->threadId != thread)
        return;

    struct timeval start, end;

    float transferFunc[] = {
        0.0833, 0.1667, 0.2500, 0.3334, 0.4167, 0.5000, 0.5834, 0.6668, 0.7501, 0.8335, 0.9168, 1 // initial
    };
    drrRendererRayCastingCuda_updateTransferFunction(transferFunc);
    //drrRendererRayCastingCuda_prepareVolumeUS(imageSize, (unsigned short *)data, volSize, volSize, volSize, 0, 0);
    //printf("X: %d y: %d z: %d\n", volSizeX, volSizeY, volSizeZ);
    drrRendererRayCastingCuda_prepareVolumeUS(imageSize, (unsigned short *)data, volSizeX, volSizeY, volSizeZ, 0, 0);

    this->minx = 0;
    this->maxx = imageSize-1;
    //this->miny = (3-this->threadId) * imageSize / 4;
    //this->maxy = ( (4-this->threadId) * imageSize / 4 ) - 1;
    this->miny = 0;
    this->maxy = imageSize -1;

    this->tSemaphore->release(1);
}

void DrrRendererRayCastingGpuCUDAThreadWorker::createMask1(unsigned int xSize, unsigned int ySize, unsigned char *datatex, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy)
{
    uint quarterY = floor( ((float)maxy + 3.0*(float)miny) / 4.0 );
    uint halfY = floor( ((float)maxy + (float)miny) / 2.0 );
    uint threeQuarterY = floor( (3.0*(float)maxy + (float)miny) / 4.0 );

    switch (this->threadId) {
    case 0:
        this->minx = minx;
        this->maxx = maxx-1;
        this->miny = miny;
        this->maxy = maxy;
        break;
    /*case 1:
        this->minx = minx;
        this->maxx = maxx-1;
        this->miny = quarterY+1;
        this->maxy = halfY;
        break;
    case 2:
        this->minx = minx;
        this->maxx = maxx-1;
        this->miny = halfY+1;
        this->maxy = threeQuarterY;
        break;
    case 3:
        this->minx = minx;
        this->maxx = maxx-1;
        this->miny = threeQuarterY+1;
        this->maxy = maxy;
        break;*/
    }

    drrRendererRayCastingCuda_prepareMask1(xSize, ySize, datatex);
    this->tSemaphore->release(1);
}

void DrrRendererRayCastingGpuCUDAThreadWorker::createMask2(unsigned int xSize, unsigned int ySize, unsigned char *datatex, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy)
{
    uint quarterY = floor( ((float)maxy + 3.0*(float)miny) / 4.0 );
    uint halfY = floor( ((float)maxy + (float)miny) / 2.0 );
    uint threeQuarterY = floor( (3.0*(float)maxy + (float)miny) / 4.0 );

    switch (this->threadId) {
    case 0:
        this->minxM2 = minx;
        this->maxxM2 = maxx-1;
        this->minyM2 = miny;
        this->maxyM2 = maxy;
        break;
    /*case 1:
        this->minxM2 = minx;
        this->maxxM2 = maxx-1;
        this->minyM2 = quarterY+1;
        this->maxyM2 = halfY;
        break;
    case 2:
        this->minxM2 = minx;
        this->maxxM2 = maxx-1;
        this->minyM2 = halfY+1;
        this->maxyM2 = threeQuarterY;
        break;
    case 3:
        this->minxM2 = minx;
        this->maxxM2 = maxx-1;
        this->minyM2 = threeQuarterY+1;
        this->maxyM2 = maxy;
        break;*/
    }

    drrRendererRayCastingCuda_prepareMask2(xSize, ySize, datatex);
    this->tSemaphore->release(1);
}

//=========================================================================
///
/// Computes the new Transformation Matrix on basis of an vector containing
/// x-, y-, and z- Translation and x-, y-, and z-Rotation
///
//=========================================================================
void DrrRendererRayCastingGpuCUDAThreadWorker::computeMatrix(float *CGInvModelViewMatrix, float focal_width) {

    if(!this->initializationDone)
    {
        cudaSetDevice(this->threadId);
        this->initializationDone = true;
    }

    drrRendererRayCastingCuda_computeMatrix(CGInvModelViewMatrix, focal_width);

    this->tSemaphore->release(1);
}


//=========================================================================
///
/// Starts the Rendering
///
//=========================================================================
float DrrRendererRayCastingGpuCUDAThreadWorker::computeDrr(int imageSize, double imagePixelSpacing, unsigned short *drrPtr, float focal_width, bool useMask1, bool useMask2, int intensityDivider, short currentMask) {

    struct timeval start, end;
    float mtime, seconds, useconds;

    if(!this->initializationDone)
    {
        cudaSetDevice(this->threadId);
        this->initializationDone = true;
    }

    dim3 block_size;
    dim3 blocks;
    block_size = dim3(16,16);
    blocks = dim3(iDivUp(imageSize, block_size.x), iDivUp(imageSize, block_size.y));

    gettimeofday(&start, NULL);

    if(useMask1 && currentMask == 0)
    {
        // cover the areas which won't be written with black
        if(this->threadId == 0)
            memset(drrPtr, 0, this->miny*imageSize*sizeof(unsigned short));
        if(this->threadId == 0)
            memset(drrPtr+(this->maxy*imageSize), 0, (imageSize-this->maxy)*imageSize*sizeof(unsigned short));
    }
    else if(useMask2 && currentMask == 1)
    {
        // cover the areas which won't be written with black
        if(this->threadId == 0)
            memset(drrPtr, 0, this->minyM2*imageSize*sizeof(unsigned short));
        if(this->threadId == 0)
            memset(drrPtr+(this->maxyM2*imageSize), 0, (imageSize-this->maxyM2)*imageSize*sizeof(unsigned short));
    }

    if(useMask1 && currentMask == 0)
        render_kernelUS(blocks, block_size, drrPtr, imageSize, imagePixelSpacing, focal_width, useMask1, useMask2, this->minx, this->maxx, this->miny, this->maxy, intensityDivider, currentMask);
    else if(useMask2 && currentMask == 1)
        render_kernelUS(blocks, block_size, drrPtr, imageSize, imagePixelSpacing, focal_width, useMask1, useMask2, this->minxM2, this->maxxM2, this->minyM2, this->maxyM2, intensityDivider, currentMask);
    else
        render_kernelUS(blocks, block_size, drrPtr, imageSize, imagePixelSpacing, focal_width, useMask1, useMask2, 0, imageSize-1, 0, imageSize-1, intensityDivider, currentMask);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0);
    this->timesum += mtime;
    this->timecounter++;

#if DEBUG
    printf("T(%d)->Start: %f ; end: %f ", this->threadId, (start.tv_sec)*1000+(start.tv_usec)/1000.0, (end.tv_sec)*1000+(end.tv_usec)/1000.0);
    printf("Iterations: %ld ; This time: %f\n", timecounter, mtime);
#endif

    this->tSemaphore->release(1);

    return mtime;
}

//=========================================================================
///
/// Starts the Rendering of a slice
///
//=========================================================================
float DrrRendererRayCastingGpuCUDAThreadWorker::computeDrr(int imageSize, double imagePixelSpacing, unsigned short *drrPtr, bool useMask1, int intensityDivider) {

    struct timeval start, end;
    float mtime, seconds, useconds;

    if(!this->initializationDone)
    {
        cudaSetDevice(this->threadId);
        this->initializationDone = true;
    }

    dim3 block_size;
    dim3 blocks;
    block_size = dim3(16,16);
    blocks = dim3(iDivUp(imageSize, block_size.x), iDivUp(imageSize, block_size.y));

    printf("Worker: Rendering a slice instead of a projection.\n");

    gettimeofday(&start, NULL);

    if(useMask1)
    {
        // cover the areas which won't be written with black
        if(this->threadId == 0)
            memset(drrPtr, 0, this->miny*imageSize*sizeof(unsigned short));
        if(this->threadId == 0)
            memset(drrPtr+(this->maxy*imageSize), 0, (imageSize-this->maxy)*imageSize*sizeof(unsigned short));
    }

    if(useMask1)
        renderSlice_kernelUS(blocks, block_size, drrPtr, imageSize, useMask1, this->minx, this->maxx, this->miny, this->maxy, intensityDivider);
    else
        renderSlice_kernelUS(blocks, block_size, drrPtr, imageSize, useMask1,  0, imageSize-1, 0, imageSize-1, intensityDivider);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0);
    this->timesum += mtime;
    this->timecounter++;

#if DEBUG
    printf("T(%d)->Start: %f ; end: %f ", this->threadId, (start.tv_sec)*1000+(start.tv_usec)/1000.0, (end.tv_sec)*1000+(end.tv_usec)/1000.0);
    printf("Iterations: %ld ; This time: %f\n", timecounter, mtime);
#endif

    this->tSemaphore->release(1);

    return mtime;
}

int DrrRendererRayCastingGpuCUDAThreadWorker::iDivUp(int a, int b){
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

