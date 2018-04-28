

#ifndef IMAGE_INPAINTING_H
#define IMAGE_INPAINTING_H

#include <stdlib.h>

#include "io_png.h"
#include "image_structures.h"
#include "patch_match.h"
#include "reconstruct_image.h"
#include "reconstruct_image_and_features.h"
#include "image_operations.h"
#include "morpho.h"

#ifndef SUBSAMPLE_FACTOR
#define SUBSAMPLE_FACTOR 2
#endif

    typedef struct paramInpaint
	{
		float residualThreshold; /*!< Threshold on the change of the inpainting solution to determine stop iterating on the current pyramid level */
		int maxIterations;	/*!< Maximum number of iterations allowed, in case sufficient convergence is not reached*/
		int nLevels; /*!< Number of multi-scale pyramid levels*/
		bool useFeatures; /*!< Boolean parameter to determine whether to use texture attributes in the patch metric*/
	}inpaintingParameterStruct;

patchMatchParameterStruct* initialise_patch_match_parameters(int patchSizeX, int patchSizeY, int imgSizeX, int imgSizeY, bool verboseMode=false);
inpaintingParameterStruct* initialise_inpainting_parameters(int nLevels, bool useFeatures, float residualThreshold, int maxIterations);

void display_inpainting_parameters(inpaintingParameterStruct *inpaintingParams);
void display_patch_match_parameters(patchMatchParameterStruct *patchMatchParams);

void initialise_inpainting(nTupleImage *imgIn, nTupleImage *occIn, featurePyramid featuresImgPyramid,
					nTupleImage *shiftMap, patchMatchParameterStruct *patchMatchParams);

void inpaint_image_wrapper(const char *fileIn,const char *fileOccIn, const char *fileOut,
			int patchSizeX, int patchSizeY, int nLevels=-1, bool useFeatures=false, bool verboseMode=false);
float *inpaint_image_wrapper(float *inputImage, int nx, int ny, int nc,
	float *inputOcc, int nOccx, int nOccy, int nOccc,
	int patchSizeX, int patchSizeY, int nLevels=-1, bool useFeatures=false, bool verboseMode=false);
						
nTupleImage * inpaint_image( nTupleImage *imgIn, nTupleImage *occIn,
patchMatchParameterStruct *patchMatchParams, inpaintingParameterStruct *inpaintingParameters);


#endif
