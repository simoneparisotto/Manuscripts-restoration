/*this is the include function for the colour estimation*/

#ifndef IMAGE_AND_FEATURE_RECONSTRUCTION_H
#define IMAGE_AND_FEATURE_RECONSTRUCTION_H


    #include "image_structures.h"
   	#include "common_reconstruct_image.h"
    #include "reconstruct_image_tools.h"

    int check_is_occluded( nTupleImage *imgOcc, int x, int y);

    int check_disp_field(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *occIn);
	
    void reconstruct_image_and_features(nTupleImage* imgIn, nTupleImage* occIn,
        nTupleImage *normGradX, nTupleImage *normGradY,
        nTupleImage* shiftMap, float sigmaColour, int reconstructionType=0, bool initialisation=false);

#endif
