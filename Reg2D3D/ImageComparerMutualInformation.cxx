#include "ImageComparerMutualInformation.h"
#include <math.h>

double CalculateMeritFctMutualInformation(imageType *mImageBase, imageType *mImageMatch, unsigned short width, unsigned short height, unsigned short depth)
{

    //Both camera
    int mRankCorrNbrPix;
    unsigned short **mRankCorrRandomArray;
    unsigned short nbin=256;

    double **jointproba;

    T 	bPixVal,	//BaseImagePixelValue
	mPixVal,	//MatchingImagePixelValue
	bBin,		//baseImageNumberOfBins
	mBin;		//MatchingImageNumberOfBins
    double n = 0.0;
    float  hm  = 0;
    float  hb  = 0;
    float  hmb = 0;
    double zRes = 0.0;

    unsigned short imageBDepth = depth;
    unsigned short imageMDepth = depth;

    unsigned short imageBWidth  = width;
    unsigned short imageBHeight = height;
    unsigned short imageMWidth  = width;
    unsigned short imageMHeigth = height;

    T *imageBRawPtr = mImageBase;
    T *imageMRawPtr = mImageMatch;

    
    // if there is a mask (forget about this now)
    // if (this->mRankCorrRandomArray!= NULL)
    if(0) // commented out for the moment
    {

        for (int idx = 0; idx < mRankCorrNbrPix; idx++)
        {            
            // get pixel values from base and match images
            bPixVal = imageBRawPtr[mRankCorrRandomArray[idx][1] + mRankCorrRandomArray[idx][0]*imageBWidth];
            mPixVal = imageMRawPtr[mRankCorrRandomArray[idx][1] + (mRankCorrRandomArray[idx][0])*imageMWidth];

            // datatype nbins are used -> assign the appropriate bin
            bBin    = (int)((float)bPixVal*((float)nbin/(float)imageBDepth));
            mBin    = (int)((float)mPixVal*((float)nbin/(float)imageMDepth));

            jointproba[mBin][bBin] +=  1.0;
            n++;
        }

        for (int i = 0; i < nbin; i++) {
            for (int j = 0; j < nbin; j++) {
                jointproba[i][j] = jointproba[i][j]/n;
            }
        }

    }
    else // if there is no mask
    {
        for (int x = 0; x < imageBWidth; x++)
        {
            for (int y = 0; y < imageBHeight; y++)
            {
                
                // get pixel values from base and match images
                bPixVal = imageBRawPtr[y + x*imageBWidth];
                mPixVal = imageMRawPtr[y + x*imageMWidth];

                // datatype nbins are used -> assign the appropriate bin
                bBin    = (int)((float)bPixVal*((float)nbin/(float)imageBDepth));
                mBin    = (int)((float)mPixVal*((float)nbin/(float)imageMDepth));
                jointproba[mBin][bBin] +=  1.0;
                n++;
                
            }
        }

        for (int i = 0; i < nbin; i++) {
            for (int j = 0; j < nbin; j++) {
                jointproba[i][j] = jointproba[i][j]/n;
            }
        }

    }

   double* marg_base = new double [nbin];
   double* marg_match= new double [nbin];
   

   //marginal base and entropy
   double *ptr;
   ptr = marg_base;

      for (int j = 0; j < nbin; j++) {
        *ptr = 0.0;
        for (int i = 0; i < nbin; i++) {
            *ptr +=  jointproba[i][j];
        }

        if(*ptr>0.0)
        {
            hb  -= ((*ptr)*log(*ptr));
        }
        ptr++;
    }


    //marginal match and entropy
      ptr = marg_match;
    for (int i = 0; i < nbin; i++) {
        *ptr = 0.0;
        for (int j = 0; j < nbin; j++) {
            *ptr += jointproba[i][j];
        }

        if(*ptr>0.0)
        {
            hm  -= ((*ptr)*log(*ptr));
        }
        ptr++;
    }

    //Joint entropy
    for(int i=0; i<nbin; i++)
    {
        for(int j=0; j<nbin; j++)
        {
            if(jointproba[i][j]>0.0)
            {
                hmb -= jointproba[i][j]*log(jointproba[i][j]);
            }
        }
    }

    zRes = 1000 * (-1.0+((2.0*hmb)/(hm+hb)));

    return zRes;

}
