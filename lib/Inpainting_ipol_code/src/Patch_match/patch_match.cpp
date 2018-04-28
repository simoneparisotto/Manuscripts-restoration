/**
 *  Copyright (C) 2017, Alasdair Newson <alasdairnewson.work@gmail.com>
 *  Copyright (C) 2017, Andrés Almansa <andres.almansa@parisdescartes.fr>
 *  Copyright (C) 2017, Yann Gousseau <yann.gousseau@telecom-paristech.fr>
 *  Copyright (C) 2017, Patrick Pérez <patrick.perez@technicolor.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

//this function calculates the approximate nearest neighbour field for patch match
//in a volume, with multivalued pixels

#include "patch_match.h"

//this function calculates a nearest neighbour field, from imgA to imgB
void patch_match_ANN(nTupleImage *imgA, nTupleImage *imgB, 
        nTupleImage *shiftMap, nTupleImage *imgOcc, nTupleImage *imgMod,
        const patchMatchParameterStruct *params,nTupleImage *firstGuess)
{
	long startTimeTotalPatchMatch = getMilliSecs();
	//check certain parameters
	if((imgA->nTupleSize) != (imgB->nTupleSize) )
	{
		MY_PRINTF("Error in patch_match_ANN, the size of the vector associated to each pixel is different for the two image volumes.");
		return;
	}
	if( (imgA->patchSizeX != (imgB->patchSizeX)) || (imgA->patchSizeY != (imgB->patchSizeY)) )	//check that the patch sizes are equal
	{
		MY_PRINTF("Error in patch_match_ANN, the size of the patches are not equal in the two image volumes.");
		return;
	}
	if ( ( imgA->patchSizeX > imgA->xSize) || ( imgA->patchSizeY > imgA->ySize)  ||
		( imgA->patchSizeX > imgB->xSize) || ( imgA->patchSizeY > imgB->ySize) )	//check that the patch size is less or equal to each dimension in the images
	{
		MY_PRINTF("Error in patch_match_ANN, the patch size is to large for one or more of the dimensions of the image volumes.");
		return;
	}
    
    if (params->fullSearch == 1)
    {
        if (firstGuess!= NULL)
    	{
        	initialise_displacement_field(shiftMap, imgA,imgB, firstGuess, imgOcc, params);
    	}
       
        startTimeTotalPatchMatch = clock();
        patch_match_full_search(shiftMap, imgA, imgB, imgOcc, imgMod,params);
    }
    else    //normal patchMatch
    {
    	if (firstGuess != NULL)
    	{
    		if ( (params->verboseMode) == true)
		    	MY_PRINTF("Initialisation\n");
		    long startTimeInitialisation = getMilliSecs();
		    initialise_displacement_field(shiftMap, imgA, imgB, firstGuess, imgOcc,params);
		    if ( (params->verboseMode) == true)
		    	MY_PRINTF("Initialisation time in s: %f\n",fabs(startTimeInitialisation-getMilliSecs())/1000);
	    }
        //show_nTuple_volume(shiftMap);
        if (check_disp_field(shiftMap, imgA, imgB,imgOcc,params) == -1)
            return;
        for (int i=0; i<(params->nIters); i++)
        {
        	patch_match_one_iteration_patch_level(shiftMap, imgA, imgB,
        	imgOcc, imgMod, params, i);
        }
    }
    if ( (params->verboseMode) == true)
    {
		MY_PRINTF("Total PatchMatch execution time in s: %f\n",fabs(startTimeTotalPatchMatch-getMilliSecs())/1000);
	}

}
