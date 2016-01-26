#ifndef DRRRENDERERRAYCASTINGGPUCUDATHREADWORKER_H
#define DRRRENDERERRAYCASTINGGPUCUDATHREADWORKER_H


#include <QObject>
#include <QSemaphore>

#include <iostream>
#include <sys/time.h>
#include <math.h>

#include <gsl/gsl_vector.h>  //necessary?

#include "Matrices.h"

#include <assert.h>

// Cuda stuff

#include "../DrrRendererRayCastingGpuCUDA.cuh"

#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

typedef unsigned int uint;
typedef unsigned char uchar;

//=========================================================================
//
/// Worker thread to generates the DRR using CUDA based ray casting
//
//=========================================================================
class DrrRendererRayCastingGpuCUDAThreadWorker : public QObject {

    Q_OBJECT

protected:

        int threadId;
        QSemaphore *tSemaphore;

        bool initializationDone;

        unsigned int minx;
        unsigned int maxx;
        unsigned int miny;
        unsigned int maxy;

        unsigned int minxM2;
        unsigned int maxxM2;
        unsigned int minyM2;
        unsigned int maxyM2;

        float timesum;
        int timecounter;

        float CGInvModelViewMatrix[12];

        float invViewMatrix[12];

        //internal methods
        void init();
        int iDivUp(int a, int b);

public:
        DrrRendererRayCastingGpuCUDAThreadWorker(int tId, QSemaphore *semaphore);
        virtual ~DrrRendererRayCastingGpuCUDAThreadWorker();

private slots:

        void updateTransferFunction(int tFuncl, int tFunch, int cam);
        void createVolumeTexture(int thread, unsigned int imageSize, unsigned int volSizeX, unsigned int volSizeY, unsigned int volSizeZ, unsigned short *data);
        void createMask1(unsigned int xSize, unsigned int ySize, unsigned char *datatex, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy);
        void createMask2(unsigned int xSize, unsigned int ySize, unsigned char *datatex, unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy);
        void computeMatrix(float *CGInvModelViewMatrix, float focal_width);
        float computeDrr(int imageSize, double imagePixelSpacing, unsigned short *drrPtr, float focal_width, bool useMask1, bool useMask2, int intensityDivider, short currentMask);

        // slots for rendering a slice
        float computeDrr(int imageSize, double imagePixelSpacing, unsigned short *drrPtr, bool useMask1, int intensityDivider);

        void cleanGPU(void);
};

#endif // DRRRENDERERRAYCASTINGGPUCUDATHREADWORKER_H
