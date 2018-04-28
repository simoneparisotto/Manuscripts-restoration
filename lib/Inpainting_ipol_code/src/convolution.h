

//definitions for the convolution operations

#ifndef CONVOLUTIONS_H
#define CONVOLUTIONS_H

#include "image_structures.h"

nTupleImage * create_convolution_kernel(const char * kernelType, int xSizeKernel, int ySizeKernel, float stdDev = 1.5);
nTupleImage * normalised_convolution_masked(nTupleImage *imgIn, nTupleImage *convKernel, nTupleImage *occlusionMask=NULL);
nTupleImage * normalised_convolution_masked_separable(nTupleImage *imgIn,
	nTupleImage *convKernelX, nTupleImage *convKernelY, nTupleImage *occlusionMask=NULL);

#endif
