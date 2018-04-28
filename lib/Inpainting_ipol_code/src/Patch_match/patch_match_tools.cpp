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

//this function defines the functions which are tools used for the
//spatio-temporal patch_match algorithm


#include "patch_match_tools.h"

//check if the displacement values have already been used
bool check_already_used_patch( nTupleImage *shiftMap, int x, int y, int dispX, int dispY)
{
    
    if ( (((int)shiftMap->get_value(x,y,0)) == dispX) && 
            (((int)shiftMap->get_value(x,y,1)) == dispY)
            )
        return 1;
    else
        return 0;
    
}

//see if the maximum shift distance is respected
bool check_max_shift_distance(int xShift, int yShift, const patchMatchParameterStruct *params)
{
	int distance;
	
	distance = (int)floor( (float)sqrt( (float)xShift*xShift + yShift*yShift) );
	
	if (params->maxShiftDistance != -1)
		return(distance <= params->maxShiftDistance);
	else
		return(true);
}


//check if the pixel is occluded
int check_is_occluded( nTupleImage *imgOcc, int x, int y)
{
	if (imgOcc->xSize == 0)
		return 0;
	if ( (imgOcc->get_value(x,y,0)) > 0)
		return 1;
	else
		return 0;
}

void calclulate_patch_distances(nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *shiftMap, nTupleImage *occIn,
		const patchMatchParameterStruct *params)
{
	for (int j=0; j< (shiftMap->ySize); j++)
		for (int i=0; i< (shiftMap->xSize); i++)
		{
			if (check_in_inner_boundaries(departImage, i, j, params) == 1)
			{
				int xShift,yShift;
				xShift = shiftMap->get_value(i,j,0);
				yShift = shiftMap->get_value(i,j,1);
				float ssdTemp = ssd_patch_measure(departImage, arrivalImage,occIn, i, j, i+xShift, j+yShift, -1, params);
				shiftMap->set_value(i,j,2,ssdTemp);
			}
			else
				shiftMap->set_value(i,j,2,FLT_MAX);
		
		}
			

}

float calclulate_patch_error(nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *shiftMap, nTupleImage *occIn,
		int xA, int yA, float minError, const patchMatchParameterStruct *params)
{
	int xB, yB;
	float errorOut;

	xB = (xA) + (int)shiftMap->get_value(xA,yA,0);
	yB = (yA) + (int)shiftMap->get_value(xA,yA,1);

	errorOut = ssd_patch_measure(departImage, arrivalImage,occIn, xA, yA, xB, yB, minError, params);
	return(errorOut);
}

int check_disp_field(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage,
    nTupleImage *occIn, const patchMatchParameterStruct *params)
{
	int dispValX,dispValY,hPatchSizeX,hPatchSizeY;
	int xB,yB;
	int i,j,returnVal;

	hPatchSizeX = (int)floor((float)((departImage->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((departImage->patchSizeY)/2));	//half the patch size

	returnVal = 0;
	for (j=hPatchSizeY; j< ((shiftMap->ySize) -hPatchSizeY); j++)
		for (i=hPatchSizeX; i< ((shiftMap->xSize) -hPatchSizeX); i++)
		{
			dispValX = (int)shiftMap->get_value(i,j,0);
			dispValY = (int)shiftMap->get_value(i,j,1);

			xB = dispValX + i;
			yB = dispValY + j;

			if ( check_in_inner_boundaries(arrivalImage, xB, yB, params) == 0)
			{
				MY_PRINTF("Error, the displacement is incorrect.\n");
				MY_PRINTF("xA : %d\n yA : %d\n",i,j);
				MY_PRINTF(" dispValX : %d\n dispValY : %d\n",dispValX,dispValY);
				MY_PRINTF(" xB : %d\n yB : %d\n",xB,yB);
				returnVal= -1;
			}
			else if (check_is_occluded(occIn,xB,yB) == 1)
			{
				MY_PRINTF("Error, the displacement leads to an occluded pixel.\n");
				MY_PRINTF(" xB : %d\n yB : %d\n",xB,yB);
				returnVal= -1;
			}
		}
	return(returnVal);

}

void patch_match_full_search(nTupleImage *shiftMap, nTupleImage *imgA, nTupleImage *imgB,
        nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params)
{
    float minSSD,ssdTemp;
    int hPatchSizeX, hPatchSizeY;
    int bestX, bestY;
    
    hPatchSizeX = (int)floor((float)((shiftMap->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((shiftMap->patchSizeY)/2));	//half the patch size
            
    //#pragma omp parallel for shared(dispField, occIn, imgVA, imgB) private(jj,ii,minSSD,ssdTemp,bestX,bestY,i,j)
	for (int j=0; j< ((shiftMap->ySize)); j++)
    {
		for (int i=0; i< ((shiftMap->xSize)); i++)
        {
            minSSD = FLT_MAX;
            bestX = INT_MAX;
            bestY = INT_MAX;
            if (modImg->xSize >0)
                if (modImg->get_value(i,j,0) == 0)   //if we don't want to modify this match
                   continue;
            //search for the best match
            for (int jj=hPatchSizeY; jj< ((shiftMap->ySize) -hPatchSizeY); jj++)
                for (int ii=hPatchSizeX; ii< ((shiftMap->xSize) -hPatchSizeX); ii++)
                {
                    if (occIn->xSize >0)
                        if (check_is_occluded(occIn,ii,jj))   //if this pixel is occluded, continue
                            continue;
                            
                    if (check_max_shift_distance(ii-i,jj-j,params) == false)
                    	continue;

                    ssdTemp = ssd_patch_measure(imgA, imgB,occIn, i, j, ii, jj, minSSD,params);
                    if ( (ssdTemp != -1) && (ssdTemp <= minSSD))   //we have a new best match
                    {
                        minSSD = ssdTemp;
                        bestX = ii - i;
                        bestY = jj - j;
                    }
                }
            if (bestX==FLT_MAX || bestY==FLT_MAX)
            {
            	MY_PRINTF("Here patch_match_full_search. Error, a correct nearest neighbour was not found for the patch centred at :\nx:%d\ny:%d\n\n",i,j);
            }
            shiftMap->set_value(i,j,0,(imageDataType)bestX);
            shiftMap->set_value(i,j,1,(imageDataType)bestY);
            shiftMap->set_value(i,j,2,minSSD);
        }
    }

}

void initialise_displacement_field(nTupleImage *shiftMap, nTupleImage *departImage, 
            nTupleImage *arrivalImage, nTupleImage *firstGuess, nTupleImage *occIn, const patchMatchParameterStruct *params)
{
	//declarations
	int xDisp, yDisp;
	int xMin,xMax,yMin,yMax;
	int xFirst,yFirst;
	int isNotOcc;
    float ssdTemp;

	int hPatchSizeX = (int)floor(((float)arrivalImage->patchSizeX)/2);
	int hPatchSizeY = (int)floor(((float)arrivalImage->patchSizeY)/2);

	int hPatchSizeCeilX = (int)ceil(((float)arrivalImage->patchSizeX)/2);
	int hPatchSizeCeilY = (int)ceil(((float)arrivalImage->patchSizeY)/2);

	for (int i=0; i< (shiftMap->xSize); i++)
		for (int j=0; j< (shiftMap->ySize); j++)
		{
			isNotOcc = 0;
            //if there is a valid first guess
            while(isNotOcc == 0)
            {
                //if there is a first guess, and it is in the inner boundaries, and respects the minimum shift distance
                if ( (firstGuess->xSize >0) && (check_in_inner_boundaries(arrivalImage,i+(int)firstGuess->get_value(i,j,0),
                    j+(int)firstGuess->get_value(i,j,1),params )) &&
                    (check_max_shift_distance((int)firstGuess->get_value(i,j,0),
                    (int)firstGuess->get_value(i,j,1),params ))
                    )
                {
                    //if it is not occluded, we take the initial first guess and continue
                    if (!check_is_occluded(occIn,i+(int)firstGuess->get_value(i,j,0),j+(int)firstGuess->get_value(i,j,1) ) )
                    {
                        xDisp = (int)firstGuess->get_value(i,j,0);
                        yDisp = (int)firstGuess->get_value(i,j,1);
                        isNotOcc = 1;
                        continue;
                    }
                    else    //otherwise, we set up the calculation of a random initial starting point, centred on the initial guess
                    {
                        xFirst = i+(int)firstGuess->get_value(i,j,0);
                        yFirst = j+(int)firstGuess->get_value(i,j,1);
                        xMin = max_int(xFirst-params->w,hPatchSizeX);
                        xMax = min_int(xFirst+params->w,arrivalImage->xSize - hPatchSizeX -1);
                        yMin = max_int(yFirst-params->w,hPatchSizeY);
                        yMax = min_int(yFirst+params->w,arrivalImage->ySize - hPatchSizeY -1);
                        shiftMap->set_value(i,j,0,-1);
                        shiftMap->set_value(i,j,1,-1);
                        shiftMap->set_value(i,j,2,-1);
                        
                    }
                }
                else    //by default, set the displacement to float_max
                {
                    shiftMap->set_value(i,j,0,(imageDataType)FLT_MAX);
                    shiftMap->set_value(i,j,1,(imageDataType)FLT_MAX);
                }   
                if (arrivalImage->xSize == arrivalImage->patchSizeX)	//special case where the patch size is the size of the dimension
                {
                    xDisp = 0;
                }
                else{
                    if ( (shiftMap->get_value(i,j,0) == FLT_MAX) || (firstGuess->xSize == 0))  //default behaviour
                    {
                        xDisp = ((rand()%( (arrivalImage->xSize) -2*hPatchSizeCeilX-1)) + hPatchSizeX)-i;
                    }
                    else    //based on an initial guess
                    {
                        xDisp = (int)((int)round_float(rand_float_range((float)(xMin),(float)(xMax))) - i);
                    }
                }
                if (arrivalImage->ySize == arrivalImage->patchSizeY)	//special case where the patch size is the size of the dimension
                {
                    yDisp = 0;
                }
                else{
                    if ( (shiftMap->get_value(i,j,1) == FLT_MAX) || (firstGuess->xSize == 0))  //default behaviour
                    {
                        yDisp = ((rand()%( (arrivalImage->ySize) -2*hPatchSizeCeilY-1)) + hPatchSizeY)-j;
                    }
                    else    //based on an initial guess
                    {
                        yDisp = (int)(round_float(rand_float_range((float)(yMin),(float)(yMax))) - j);
                    }
                }

                isNotOcc = (!(check_is_occluded(occIn,xDisp+i,yDisp+j))
                         &&(check_in_inner_boundaries(arrivalImage,xDisp+i,yDisp+j,params))
                         &&(check_max_shift_distance(xDisp,yDisp,params))
                         );
            }
            //if everything is all right, set the displacements
            shiftMap->set_value(i,j,0, (imageDataType)(xDisp));
            shiftMap->set_value(i,j,1, (imageDataType)(yDisp));

            if (check_in_inner_boundaries(departImage,i,j,params))
            {
                ssdTemp = ssd_patch_measure(departImage, arrivalImage,occIn, i, j, i+xDisp, j+yDisp, -1,params);
                if(ssdTemp ==-1)
                    ssdTemp = FLT_MAX;
            }
            else
                ssdTemp = FLT_MAX;
            shiftMap->set_value(i,j,2,(imageDataType)ssdTemp); //set the ssd error
        }
}




/******************************************/
/******************************************/
/******   PATCH LEVEL INTERLEAVING   ******/
/******************************************/
/******************************************/

void patch_match_one_iteration_patch_level(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage,
        nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params, int iterationNb)
{
	int wMax, zMax;
	//calculate the maximum z (patch search index)
    wMax = min_int(params->w, max_int(arrivalImage->xSize,arrivalImage->ySize) );
	zMax = (int)ceil((float) (- (log((float)(wMax)))/(log((float)(params->alpha)))) );
    
    nTupleImage *wValues = new nTupleImage(zMax,1,1,departImage->indexing);
    //store the values of the maximum search parameters
    for (int z=0; z<zMax; z++)
    {
        wValues->set_value(z,0,0,
                (imageDataType)round_float((params->w)*((float)pow((float)params->alpha,z)))
                );
    }

    if (iterationNb&1)  //if we are on an odd iteration
    {
        for (int j=((shiftMap->ySize) -1); j>= 0; j--)
            for (int i=((shiftMap->xSize) -1); i>= 0; i--)
            {
                                //propagation
                patch_match_propagation_patch_level(shiftMap, departImage, arrivalImage, occIn,  
                params, iterationNb, i, j);
                
                //random search
                patch_match_random_search_patch_level(shiftMap, departImage, arrivalImage,
                occIn, modImg, params, i, j, wValues);
            }
    }
    else    //if we are on an even iteration
    {

    	for (int j=0; j< ((shiftMap->ySize) ); j++)
    		for (int i=0; i< ((shiftMap->xSize) ); i++)
    		{
    			//propagation
    			patch_match_propagation_patch_level(shiftMap, departImage, arrivalImage, occIn,  
        		params, iterationNb, i, j);
    			
    			//random search
    			patch_match_random_search_patch_level(shiftMap, departImage, arrivalImage,
        		occIn, modImg, params, i, j, wValues);
    		}
    }
	delete wValues
;}

void patch_match_random_search_patch_level(nTupleImage *shiftMap, nTupleImage *imgA, nTupleImage *imgB,
        nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params, int i, int j,
        nTupleImage *wValues)
{
	//create random number seed
	int xRand,yRand;
	int randMinX,randMaxX,randMinY,randMaxY;
	int hPatchSizeX,hPatchSizeY;
	int xTemp,yTemp,wTemp;
	float ssdTemp;

	hPatchSizeX = (int)floor((float)((shiftMap->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((shiftMap->patchSizeY)/2));	//half the patch size

	if (modImg->xSize >0)
		if (modImg->get_value(i,j,0) == 0)   //if we don't want to modify this match
			return;
	ssdTemp = shiftMap->get_value(i,j,2); //get the saved ssd value
	
	for (int z=0; z<(wValues->xSize); z++)	//test for different search indices
	{
		xTemp = i+(int)shiftMap->get_value(i,j,0);	//get the arrival position of the current offset
		yTemp = j+(int)shiftMap->get_value(i,j,1);	//get the arrival position of the current offset

		wTemp = wValues->get_value(z,0,0);
		// X values
		randMinX = max_int(xTemp - wTemp,hPatchSizeX);
		randMaxX = min_int(xTemp + wTemp,imgB->xSize - hPatchSizeX - 1);
		// Y values
		randMinY = max_int(yTemp - wTemp,hPatchSizeY);
		randMaxY = min_int(yTemp + wTemp,imgB->ySize - hPatchSizeY - 1);

		//new positions in the image imgB
		xRand = rand_int_range(randMinX, randMaxX);	//random values between xMin and xMax, clamped to the sizes of the image B
		yRand = rand_int_range(randMinY, randMaxY);	//random values between yMin and yMax, clamped to the sizes of the image B

		if (check_is_occluded(occIn,xRand,yRand))
			continue;	//the new position is occluded
		if (check_in_inner_boundaries(imgB,xRand,yRand,params) == 0)
			continue;	//the new position is not in the inner boundaries
		if (check_max_shift_distance( (xRand-i),(yRand-j),params) == false)
			continue;	//the new position is too far away

		ssdTemp =  ssd_patch_measure(imgA, imgB,occIn, i, j, xRand, yRand, ssdTemp,params);

		if (ssdTemp != -1)	//we have a better match
		{
			shiftMap->set_value(i,j,0, (imageDataType)(xRand-i));
			shiftMap->set_value(i,j,1, (imageDataType)(yRand-j));
			shiftMap->set_value(i,j,2, (imageDataType)(ssdTemp));
		}
		else
			ssdTemp = shiftMap->get_value(i,j,2); //set the saved ssd value bakc to its proper (not -1) value
	}
}

//one iteration of the propagation of the patch match algorithm, for a SINGLE patch
void patch_match_propagation_patch_level(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *occIn,  
        const patchMatchParameterStruct *params, int iterationNb, int i, int j)
{
	//declarations
	int *correctInd;
	float currentError, *minVector;
	
	correctInd = new int;
	minVector = new float[2];

	//calculate the error of the current displacement
	currentError = shiftMap->get_value(i,j,2);
                    
	get_min_correct_error(shiftMap,departImage,arrivalImage,occIn,
	i, j, iterationNb&1, correctInd,minVector,currentError,params);
	
	//if the best displacement is the current one. Note : we have taken into account the case
	//where none of the diplacements around the current pixel are valid
	if (*correctInd == -1)	//if the best displacement is the current one
	{
		shiftMap->set_value(i,j,2,currentError);
        delete correctInd;
        delete[] minVector;
		return;
	}
	if (iterationNb&1)	//if we are on an odd iteration
	{
		if ((*correctInd) == 0){
			copy_pixel_values_nTuple_image(shiftMap,shiftMap, min_int(i+1,((int)shiftMap->xSize)-1), j, i, j);
		}

		else if((*correctInd) == 1){
			copy_pixel_values_nTuple_image(shiftMap,shiftMap, i, min_int(j+1,((int)shiftMap->ySize)-1), i, j);
		}
		else
			MY_PRINTF("Error, correct ind not chosen\n.");
		//now calculate the error of the patch matching
		currentError = calclulate_patch_error(departImage,arrivalImage,shiftMap,occIn,i,j, -1,params);
		shiftMap->set_value(i,j,2,currentError);
	}
	else		//even iteration
	{
		if ( (*correctInd) == 0){
			copy_pixel_values_nTuple_image(shiftMap,shiftMap, max_int(i-1,0), j, i, j);
		}

		else if( (*correctInd) == 1){
			copy_pixel_values_nTuple_image(shiftMap,shiftMap, i, max_int(j-1,0), i, j);
		}
		else
			MY_PRINTF("Error, correct ind not chosen\n.");
		//now calculate the error of the patch matching
		currentError = calclulate_patch_error(departImage,arrivalImage,shiftMap,occIn,i,j, -1,params);
		shiftMap->set_value(i,j,2,currentError);
	}
	
	delete correctInd;
	delete[] minVector;
	
}


//this function returns the minimum error of the patch differences around the pixel at (i,j,k)
//and returns the index of the best position in correctInd :
// -1 : none are correct
// 0 : left/right
// 1 : upper/lower
float get_min_correct_error(nTupleImage *shiftMap,nTupleImage *departImage,nTupleImage *arrivalImage, nTupleImage *occIn,
							int x, int y, int evenIteration, int *correctInd, float *minVector, float minError,
                            const patchMatchParameterStruct *params)
{
	float minVal;
	int i;
    int dispX, dispY;

	minVal = minError;

	*correctInd = -1;	//initialise the correctInd vector to -1
    for (i=0;i<NDIMS;i++)
		minVector[i] = -1;

	if (evenIteration == 0)	//we are looking left, upper : we are on an even iteration
	{
        dispX = (int)shiftMap->get_value((int)max_int(x-1,0),y,0); dispY = (int)shiftMap->get_value((int)max_int(x-1,0),y,1);
        if ( check_in_inner_boundaries(arrivalImage,x+dispX,y+dispY,params) && (!check_is_occluded(occIn, x+dispX, y+dispY)) &&
                (!check_already_used_patch( shiftMap, x, y, dispX, dispY)))
            minVector[0] = ssd_patch_measure(departImage, arrivalImage,occIn,x,y,x+dispX,y+dispY,minError,params);
        
        dispX = (int)shiftMap->get_value(x,(int)max_int(y-1,0),0); dispY = (int)shiftMap->get_value(x,(int)max_int(y-1,0),1);
        if ( check_in_inner_boundaries(arrivalImage,x+dispX,y+dispY,params) && (!check_is_occluded(occIn, x+dispX, y+dispY)) && 
                (!check_already_used_patch( shiftMap, x, y, dispX, dispY)))
            minVector[1] = ssd_patch_measure(departImage, arrivalImage,occIn,x,y,x+dispX,y+dispY,minError,params);

		for (i=0;i<NDIMS;i++)
		{
            if (minVector[i] == -1)
                continue;
            if ( minVector[i] < minVal)
			{
				switch (i){
					case 0 :
							minVal = minVector[i];
							*correctInd = 0;
						break;
					case 1 :
							minVal = minVector[i];
							*correctInd = 1;
						break;
					default :
						MY_PRINTF("Error in get_min_correct_error. The index i : %d is above N_DIMS.\n",i);
				}
			}

		}


	}
	else	//we are looking right, lower
	{
        
        dispX = (int)shiftMap->get_value((int)min_int(x+1,(departImage->xSize)-1),y,0); dispY = (int)shiftMap->get_value((int)min_int(x+1,(departImage->xSize)-1),y,1);
        if ( check_in_inner_boundaries(arrivalImage,x+dispX,y+dispY,params) && (!check_is_occluded(occIn, x+dispX, y+dispY)) && 
                (!check_already_used_patch( shiftMap, x, y, dispX, dispY)))
            minVector[0] = ssd_patch_measure(departImage, arrivalImage,occIn,x,y,x+dispX,y+dispY,minError,params);
        
        dispX = (int)shiftMap->get_value(x,(int)min_int(y+1,(departImage->ySize)-1),0); dispY = (int)shiftMap->get_value(x,(int)min_int(y+1,(departImage->ySize)-1),1);
        if ( check_in_inner_boundaries(arrivalImage,x+dispX,y+dispY,params) && (!check_is_occluded(occIn, x+dispX, y+dispY)) && 
                (!check_already_used_patch( shiftMap, x, y, dispX, dispY)))
            minVector[1] = ssd_patch_measure(departImage, arrivalImage,occIn,x,y,x+dispX,y+dispY,minError,params);

		for (i=0;i<NDIMS;i++)
		{
            if (minVector[i] == -1)
                continue;
			if ( minVector[i] < minVal)
			{
				switch (i){
					case 0 :
							minVal = minVector[i];
							*correctInd = 0;
						break;
					case 1 :
							minVal = minVector[i];
							*correctInd = 1;
						break;
					default :
						MY_PRINTF("Error in get_min_correct_error. The index i : %d is above N_DIMS.\n",i);
				}
			}

		}
	}

	if ( (*correctInd) == -1)	//if none of the displacements are valid 
	{
		minVal = -1;
	}
	return(minVal);
}
