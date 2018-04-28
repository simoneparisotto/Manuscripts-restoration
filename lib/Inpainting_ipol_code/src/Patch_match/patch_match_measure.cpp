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

 
//this function defines the patch match measure with which we compare patches

#include "patch_match_measure.h"

float ssd_patch_measure(nTupleImage *imgA, nTupleImage *imgB, nTupleImage *occIn, int xA, int yA,
int xB, int yB, float minVal, const patchMatchParameterStruct *params)
{
	//declarations
	int i,j, p,xAtemp, yAtemp, xBtemp, yBtemp;
	int xMinA,yMinA;
	int xMinB,yMinB;
    int sumOcc, occA;
	imageDataType tempVal;
    float beta = 50.0;

	imageDataType ssd = 0;

	if ( ((imgA->patchSizeX) != (imgB->patchSizeX)) || ((imgA->patchSizeY) != (imgB->patchSizeY)) )
	{
		MY_PRINTF("Error in ssd_minimum_value, the patch sizes are not equal.\n");
		return -1;
	}

    xMinA = xA-imgA->hPatchSizeX;
    yMinA = yA-imgA->hPatchSizeY;
    xMinB = xB-imgB->hPatchSizeX;
    yMinB = yB-imgB->hPatchSizeY;
    
    sumOcc = 0;
    
    if (params->partialComparison)
    {
        for (j=0; j<imgA->patchSizeY; j++)
            for (i=0; i<imgA->patchSizeX; i++)
            {
                xAtemp = xMinA + i;
                yAtemp = yMinA + j;
                xBtemp = xMinB + i;
                yBtemp = yMinB + j;
                //do not compare the edges in any case
                if ((!check_in_boundaries( imgA, xAtemp, yAtemp)))
                    continue;   //if we are not in the boundaries, do not compare

                sumOcc = sumOcc + (int)( (!(occIn->get_value(xAtemp,yAtemp,0))) == 1);

            }
    }
    else    //calculate patch size
    {
        int patchSizeXtemp = min_int(xA + imgA->hPatchSizeX,imgA->xSize-1) - max_int(xA - imgA->hPatchSizeX,0) + 1;
        int patchSizeYtemp = min_int(yA + imgA->hPatchSizeY,imgA->ySize-1) - max_int(yA - imgA->hPatchSizeY,0) + 1;
        
        sumOcc = patchSizeXtemp * patchSizeYtemp ;
    }
	sumOcc = max_int(sumOcc,1);

	for (j=0; j<imgA->patchSizeY; j++)
		for (i=0; i<imgA->patchSizeX; i++)
		{
			xAtemp = xMinA + i;
			yAtemp = yMinA + j;
            
			xBtemp = xMinB + i;
			yBtemp = yMinB + j;

            //do not compare if we are not in the boundaries
            if ((!check_in_boundaries( imgA, xAtemp, yAtemp)))
                occA = 1;
            else
                occA = 0;
            if (occA == 1)
                continue;   //we do not wish to compare this pixel
            /*if we want partial patch comparison*/
            if (params->partialComparison && occIn->xSize >0)
                occA = (int)(*(occIn->get_value_ptr(xAtemp, yAtemp,0)) == 1);
            if (occA == 1)
                continue;   //we do not wish to compare this pixel
            
            /* similarity */
			for (p=0; p<imgA->nTupleSize; p++)
			{
				imageDataType imgAval = imgA->get_value(xAtemp, yAtemp,p);
				imageDataType imgBval = imgB->get_value(xBtemp, yBtemp,p);
				tempVal = (imgAval) - (imgBval);
				ssd = ssd + (imageDataType)(((tempVal)*(tempVal))/sumOcc);
                //ssd = ssd + (abs(tempFloat))/sumOcc;
			}
            
            if( params->normGradX != NULL)
            {            
                imageDataType normGradXtemp = (params->normGradX)->get_value(xAtemp,yAtemp,0) 
                							- (params->normGradX)->get_value(xBtemp,yBtemp,0);
                                    
                imageDataType normGradYtemp = (params->normGradY)->get_value(xAtemp,yAtemp,0) 
                							- (params->normGradY)->get_value(xBtemp,yBtemp,0);

                ssd = ssd + beta*normGradXtemp*normGradXtemp/sumOcc;
                ssd = ssd + beta*normGradYtemp*normGradYtemp/sumOcc;
            }
                
		if ((minVal != -1) && (ssd > minVal))
                {
			return(-1);
                }
			}
    
	return(ssd);
}
