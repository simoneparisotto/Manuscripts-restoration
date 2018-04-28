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

/*this function defines the functions which are tools used for the
colour estimation*/


#include "reconstruct_image_tools.h"

int check_shift_map(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage)
{
	int dispValX,dispValY,hPatchSizeX,hPatchSizeY;
	int xB,yB;
	int i,j,returnVal;

	hPatchSizeX = (int)floor((float)((departImage->patchSizeX)/2));	/*half the patch size*/
	hPatchSizeY = (int)floor((float)((departImage->patchSizeY)/2));	/*half the patch size*/

	returnVal = 0;
	for (j=hPatchSizeY; j< ((shiftMap->ySize) -hPatchSizeY); j++)
		for (i=hPatchSizeX; i< ((shiftMap->xSize) -hPatchSizeX); i++)
		{
			dispValX = (int)shiftMap->get_value(i,j,0);
			dispValY = (int)shiftMap->get_value(i,j,1);

			/*if ( (fabs(dispValX) > w) || (fabs(dispValY) > w))
			{
				MY_PRINTF("Error, the displacement is greater than the minimum value w : %d.\n",w);
				MY_PRINTF(" dispValX : %d\n dispValY : %d\n dispValT : %d\n",dispValX,dispValY,dispValT);
				returnVal= -1;
			}*/

			xB = dispValX + i;
			yB = dispValY + j;

			if ( (xB <hPatchSizeX) || (yB <hPatchSizeY) || 
				(xB >= (arrivalImage->xSize - hPatchSizeX)) || (yB >= (arrivalImage->ySize - hPatchSizeY)) )
			{
				MY_PRINTF("Error, the displacement is incorrect.\n");
				MY_PRINTF("xA : %d\n yA : %d\n",i,j);
				MY_PRINTF(" dispValX : %d\n dispValY : %d\n",dispValX,dispValY);
				MY_PRINTF(" xB : %d\n yB : %d\n",xB,yB);
				returnVal= -1;
			}
			/*else if (check_is_occluded(occVol,xB,yB) == 1)
			{
				MY_PRINTF("Error, the displacement leads to an occluded pixel.\n");
				MY_PRINTF(" xB : %d\n yB : %d\n",xB,yB);
				returnVal= -1;
			}*/
		}
	return(returnVal);
}

/*this function gets the nth percentile of the current weights*/
float get_adaptive_sigma(float *weights, int weightsLength, float sigmaPercentile)
{
    int i, weightsInd, percentileInd;
    float *weightsTemp, adaptiveSigmaOut;
    float percentile = (float)(sigmaPercentile)/((float)100);
    
    weightsTemp = (float*)malloc((size_t)weightsLength*sizeof(float));
    weightsInd = 0;
    for (i=0; i<weightsLength;i++)
    {
        if (weights[i] != -1)   /*we want to use this patch */
        {
            weightsTemp[weightsInd] = weights[i];
            weightsInd = weightsInd+1;
        }
    }
    weightsInd = weightsInd-1;
    std::sort(weightsTemp,weightsTemp+(weightsInd));
    
    percentileInd = (int)floor((float)percentile*weightsInd);
    
    adaptiveSigmaOut = sqrt(weightsTemp[percentileInd]);
    free(weightsTemp);
    return(adaptiveSigmaOut);
}

/*this function retieves the highest mode in the colour space of the
 different colours available for reconstructing a pixel*/
int estimate_best_colour(nTupleImage *imgIn, float *weights, int weightsLength,
                                float *colours, int i, int j)
{
    int ii;
    int minWeightInd;
    float minWeight;
    
    
    minWeight = INT_MAX;
    minWeightInd = 0;
    for (ii=0; ii<weightsLength; ii++)
    {
        if (weights[ii] != -1)
        {
			if (weights[ii] < minWeight)
            {
                minWeight = weights[ii];
                minWeightInd = ii;
            }
        }
    }

	for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
	{
		imgIn->set_value(i,j,colourInd,(imageDataType)(colours[minWeightInd+colourInd*weightsLength]));
	}
    
    return(1);
}
