/*this is the include function for the colour estimation*/

#ifndef RECONSTRUCT_IMAGE_H
#define RECONSTRUCT_IMAGE_H


	#include "common_reconstruct_image.h"
    #include "image_structures.h"
    #include "reconstruct_image_tools.h"

    int check_is_occluded( nTupleImage *imgOcc, int x, int y);

    int check_shift_map(nTupleImage *shiftMap, nTupleImage *departImg, nTupleImage *arrivalImg, nTupleImage *occImg);
	
    void reconstruct_image(nTupleImage* imgIn, nTupleImage* occIn,
            nTupleImage* shiftMap, float sigmaColour, int reconstructionType=0, bool initialisation=false);

#endif
