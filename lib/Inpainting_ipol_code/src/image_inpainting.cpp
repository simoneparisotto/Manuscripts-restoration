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

#include "image_inpainting.h"

patchMatchParameterStruct * initialise_patch_match_parameters(
	int patchSizeX, int patchSizeY, int imgSizeX, int imgSizeY, bool verboseMode)
{
	patchMatchParameterStruct *patchMatchParams = new patchMatchParameterStruct;

	//set parameter structure
	patchMatchParams->patchSizeX = patchSizeX;
	patchMatchParams->patchSizeY = patchSizeY;
            patchMatchParams->nIters = 12;
	patchMatchParams->w = max_int(imgSizeX,imgSizeY); //maximum search radius
	patchMatchParams->alpha = 0.5; //search radius shrinkage factor (0.5 in standard PatchMatch)
	patchMatchParams->maxShiftDistance = -1;
	patchMatchParams->partialComparison = 0;
	patchMatchParams->fullSearch = 0;
	//texture attributes
	patchMatchParams->normGradX = NULL;
	patchMatchParams->normGradY = NULL;
	patchMatchParams->verboseMode = verboseMode;
	
	return(patchMatchParams);	
}

void display_patch_match_parameters(patchMatchParameterStruct *patchMatchParams)
{

	printf("\n*************************\n");
	printf("* PATCHMATCH PARAMETERS *\n");
	printf("*************************\n");
	
	printf("Patch size X : %d\n",patchMatchParams->patchSizeX);
	printf("Patch size Y : %d\n",patchMatchParams->patchSizeY);
	printf("Number of propagation/random search iterations: %d\n",patchMatchParams->nIters);
	printf("Random search reduction factor (alpha) : %f\n",patchMatchParams->alpha);
	printf("Maximum search shift allowed (-1 for whole image) : %f\n",patchMatchParams->maxShiftDistance);
	printf("Full search (should be activated only for experimental purposes !!) : %d\n",patchMatchParams->fullSearch);
	printf("Verbose mode : %d\n",patchMatchParams->verboseMode);
}

inpaintingParameterStruct * initialise_inpainting_parameters(int nLevels, bool useFeatures,
	float residualThreshold, int maxIterations)
{
	inpaintingParameterStruct *inpaintingParams = new inpaintingParameterStruct;

	//set inpainting parameter structure
	inpaintingParams->nLevels = nLevels;
	inpaintingParams->useFeatures = useFeatures;
	inpaintingParams->residualThreshold = residualThreshold;
	inpaintingParams->maxIterations = maxIterations;
	
	return(inpaintingParams);	
}


void display_inpainting_parameters(inpaintingParameterStruct *inpaintingParams)
{
	printf("\n*************************\n");
	printf("* INPAINTING PARAMETERS *\n");
	printf("*************************\n");

	printf("Number of levels : %d\n",inpaintingParams->nLevels);
	printf("Use features : %d\n",inpaintingParams->useFeatures);
	printf("Residual threshold: %f\n",inpaintingParams->residualThreshold);
	printf("Maximum number of iterations: %d\n",inpaintingParams->maxIterations);
	
	printf("*************************\n\n");

}


float * inpaint_image_wrapper(float *inputImage, int nx, int ny, int nc,
	float *inputOcc, int nOccX, int nOccY, int nOccC,
	int patchSizeX, int patchSizeY, int nLevels, bool useFeatures)
{

	// ****************************************** //
	// **** INITIALISE PATCHMATCH PARAMETERS **** //
	// ****************************************** //
	patchMatchParameterStruct *patchMatchParams = initialise_patch_match_parameters(patchSizeX, patchSizeY, nx, ny);

	// ****************************************** //
	// **** INITIALISE INPAINTING PARAMETERS **** //
	// ****************************************** //
	float residualThreshold = 0.1;
	int maxIterations = 10;
	inpaintingParameterStruct *inpaintingParameters = initialise_inpainting_parameters(nLevels, useFeatures, residualThreshold, maxIterations);

	// ******************************** //
	// ***** CREATE IMAGE STRUCTURES*** //
	// ******************************** //
		
	nTupleImage *imgIn = new nTupleImage(nx, ny, nc, patchSizeX, patchSizeY, IMAGE_INDEXING, inputImage);
	nTupleImage *occIn;
	if (nOccC == 3)		//if we need to convert the input occlusion
	{
		nTupleImage *occTemp = new nTupleImage(nOccX, nOccY, nOccC, patchSizeX, patchSizeY, IMAGE_INDEXING, inputOcc);
		occIn = rgb_to_grey(occTemp);
		delete occTemp;
	}
	else
		occIn = new nTupleImage(nOccX,nOccY,1,patchSizeX,patchSizeY,IMAGE_INDEXING,inputOcc);
	
	occIn->binarise();

	// ***** CALL MAIN ROUTINE **** //

	nTupleImage * imgOut = inpaint_image(imgIn, occIn, patchMatchParams, inpaintingParameters);

	return(imgOut->get_data_ptr());
}

void inpaint_image_wrapper(const char *fileIn,const char *fileOccIn, const char *fileOut,
			int patchSizeX, int patchSizeY, int nLevels, bool useFeatures, bool verboseMode)
{

	// *************************** //
	// ***** READ INPUTS ********* //
	// ************************** //
	//read input image
	size_t nx,ny,nc;
	size_t nOccX,nOccY,nOccC;
	
	//read input image
	printf("Reading input image\n");
	float *inputImage = read_image(fileIn,&nx,&ny,&nc);
	
	//read input occlusion
	printf("Reading input occlusion\n");
	float *inputOcc = read_image(fileOccIn,&nOccX,&nOccY,&nOccC);
	
	// ****************************************** //
	// **** INITIALISE PATCHMATCH PARAMETERS **** //
	// ****************************************** //
	patchMatchParameterStruct *patchMatchParams = initialise_patch_match_parameters(patchSizeX, patchSizeY, nx, ny, verboseMode);
	if (check_patch_match_parameters(patchMatchParams) == -1)
		return;
	// ****************************************** //
	// **** INITIALISE INPAINTING PARAMETERS **** //
	// ****************************************** //
	float residualThreshold = 0.1;
	int maxIterations = 10;
	inpaintingParameterStruct *inpaintingParams =
		initialise_inpainting_parameters(nLevels, useFeatures, residualThreshold, maxIterations);
	
	// ******************************** //
	// ***** CREATE IMAGE STRUCTURES*** //
	// ******************************** //

	nTupleImage *imgIn = new nTupleImage(nx,ny,nc,patchSizeX,patchSizeY,IMAGE_INDEXING,inputImage);
	nTupleImage *occIn;
	if (nOccC == 3)		//if we need to convert the input occlusion
	{
		nTupleImage *occImgTemp = new nTupleImage(nx,ny,3,patchSizeX,patchSizeY,IMAGE_INDEXING,inputOcc);
		occIn = rgb_to_grey(occImgTemp);
		delete occImgTemp ;
	}
	else
		occIn = new nTupleImage(nOccX,nOccY,1,patchSizeX,patchSizeY,IMAGE_INDEXING,inputOcc);
	
	occIn->binarise();
	occIn->display_attributes();
	// ***** CALL MAIN ROUTINE **** //

	nTupleImage * imgOut = inpaint_image(imgIn, occIn, patchMatchParams, inpaintingParams);

	//write output
	//write_image(imgOut,fileOut,255);
	write_image(imgOut,fileOut);

	delete imgOut;
}

nTupleImage * inpaint_image( nTupleImage *imgInput, nTupleImage *occInput,
patchMatchParameterStruct *patchMatchParams, inpaintingParameterStruct *inpaintingParams)
{
	
	// ******************************************************************** //
	// **** AUTOMATICALLY DETERMINE NUMBER OF LEVELS, IF NOT SPECIFIED **** //
	// ******************************************************************** //


	if (inpaintingParams->nLevels == -1)
	{
		inpaintingParams->nLevels =
			determine_multiscale_level_number(occInput,imgInput->patchSizeX,imgInput->patchSizeY);
	}

	display_inpainting_parameters(inpaintingParams);
	display_patch_match_parameters(patchMatchParams);
	
	nTupleImage *imgOut;

	// ************************** //
	// **** CREATE PYRDAMIDS **** //
	// ************************** //
	nTupleImagePyramid imgPyramid = create_nTupleImage_pyramid(imgInput, inpaintingParams->nLevels);
	nTupleImagePyramid occPyramid = create_nTupleImage_pyramid_binary(occInput, inpaintingParams->nLevels);
	featurePyramid featuresPyramid;
	if (inpaintingParams->useFeatures == true)
	{
		double t1 = clock();
		featuresPyramid = create_feature_pyramid(imgInput, occInput, inpaintingParams->nLevels);
		MY_PRINTF("\n\nFeatures calculation time: %f\n",((double)(clock()-t1)) / CLOCKS_PER_SEC);
	}
	else
	{
		featuresPyramid.normGradX = NULL;
        featuresPyramid.normGradY = NULL;
        featuresPyramid.nLevels = -1;
	}

	//create structuring element
	nTupleImage *structElDilate = create_structuring_element("rectangle", imgInput->patchSizeX, imgInput->patchSizeY);
	
	//show_patch_match_parameters(patchMatchParams);
	
	// ****************************************** //
	// ************* START INPAINTING *********** //
	// ****************************************** //

	nTupleImage *imgInpaint,*normGradX,*normGradY;
	nTupleImage *shiftMap=NULL;
	for (int level=( (inpaintingParams->nLevels)-1); level>=0; level--)
	{
		printf("Current pyramid level : %d\n",level);
		nTupleImage *imgPrevious,*occInpaint,*occDilate;

		if (patchMatchParams->maxShiftDistance != -1)		
			patchMatchParams->maxShiftDistance =
			(float)( (patchMatchParams->maxShiftDistance)/( pow((float)SUBSAMPLE_FACTOR,(float)level) ));
		
		imgInpaint = copy_image_nTuple(imgPyramid[level]);
		occInpaint = copy_image_nTuple(occPyramid[level]);
		//create dilated occlusion
		occDilate = imdilate(occInpaint, structElDilate);
		
		if (featuresPyramid.nLevels >= 0)
		{
			normGradX = copy_image_nTuple((featuresPyramid.normGradX)[level]);
			normGradY = copy_image_nTuple((featuresPyramid.normGradY)[level]);
			//attach features to patchMatch parameters
			patchMatchParams->normGradX = normGradX;
			patchMatchParams->normGradY = normGradY;
		}
					
		//initialise solution
		if (level == ((inpaintingParams->nLevels)-1))
		{
			shiftMap = new nTupleImage(imgInpaint->xSize,imgInpaint->ySize,3,imgInpaint->patchSizeX,imgInpaint->patchSizeY,IMAGE_INDEXING);
			shiftMap->set_all_image_values(0);
			printf("\nInitialisation started\n\n\n");
            initialise_inpainting(imgInpaint,occInpaint,featuresPyramid,shiftMap,patchMatchParams); imgInpaint = copy_image_nTuple(imgPyramid[level]);
			patchMatchParams->partialComparison = 0;
			printf("\nInitialisation finished\n\n\n");
			
			if (featuresPyramid.nLevels >= 0)	//retrieve features from the pointers in the patchMatch parameters
			{
				normGradX = patchMatchParams->normGradX;
				normGradY = patchMatchParams->normGradY;
			}
		}
		else	//reconstruct current solution
		{
			if (featuresPyramid.nLevels >= 0)
			{
				reconstruct_image_and_features(imgInpaint, occInpaint,
				normGradX, normGradY,
				shiftMap, SIGMA_COLOUR);
			}
			else
			{
				reconstruct_image(imgInpaint,occInpaint,shiftMap,SIGMA_COLOUR);
				//write_shift_map(shiftMap,fileOut);
			}
		}
		
		calclulate_patch_distances(imgInpaint,imgInpaint,shiftMap,occDilate,patchMatchParams);
		
		//iterate ANN search and reconstruction
		int iterationNb = 0;
		imageDataType residual = FLT_MAX;
		while( (residual > (inpaintingParams->residualThreshold) ) && (iterationNb < (inpaintingParams->maxIterations) ) )
		{
			//copy current imgInpaint
			imgPrevious = copy_image_nTuple(imgInpaint);
			patch_match_ANN(imgInpaint,imgInpaint,shiftMap,occDilate,occDilate,patchMatchParams);
			if (featuresPyramid.nLevels >= 0)
			{
				reconstruct_image_and_features(imgInpaint, occInpaint,
        			normGradX, normGradY,
        			shiftMap, SIGMA_COLOUR);
			}
			else
				reconstruct_image(imgInpaint,occInpaint,shiftMap,SIGMA_COLOUR);
			residual = calculate_residual(imgInpaint,imgPrevious,occInpaint);
			if (patchMatchParams->verboseMode == true)
				printf("Iteration number %d, residual = %f\n",iterationNb,residual);
			iterationNb++;
		}
		//upsample shift volume, if we are not on the finest level
		if (level >0)
		{	
			nTupleImage * shiftMapTemp = up_sample_image(shiftMap, SUBSAMPLE_FACTOR,imgPyramid[level-1]);
			delete shiftMap;
			shiftMap = copy_image_nTuple(shiftMapTemp);
			shiftMap->multiply((imageDataType)SUBSAMPLE_FACTOR);
			delete shiftMapTemp;
		}
		else
		{
			reconstruct_image(imgInpaint,occInpaint,shiftMap,SIGMA_COLOUR,3);
			imgOut = new nTupleImage(imgInpaint);
		}
		//destroy structures
		delete imgInpaint;
		delete imgPrevious;
		delete occInpaint;
		delete occDilate;
		if (featuresPyramid.nLevels >= 0)
		{
			delete normGradX;
			delete normGradY;
		}
	}
	
	// ************************** //
	// **** DELETE STRUCTURES *** //
	// ************************** //
	for (int i=0; i< (inpaintingParams->nLevels); i++)
	{
		delete imgPyramid[i];
		delete occPyramid[i];
	}
	delete imgPyramid;
	delete occPyramid;

	delete shiftMap;
	delete_feature_pyramid(featuresPyramid);
	delete patchMatchParams;
	
	printf("Inpainting finished !\n");

	return(imgOut);
}


void initialise_inpainting(nTupleImage *imgIn, nTupleImage *occIn, featurePyramid featuresPyramid,
				nTupleImage *shiftMap, patchMatchParameterStruct *patchMatchParams)
{
	int iterNb=0;
	patchMatchParams->partialComparison = 1;
	bool initialisation = true;
	nTupleImage *occIter;
	occIter = copy_image_nTuple(occIn);
	
	seed_random_numbers((double)3);
	
	nTupleImage *structElErode = create_structuring_element("rectangle", 3, 3);
	nTupleImage *structElDilate = create_structuring_element("rectangle", imgIn->patchSizeX, imgIn->patchSizeY);
	
	nTupleImage *occDilate = imdilate(occIn, structElDilate);
	
	//extract features images from featuresPyramid (coarsest level)
	nTupleImage *normGradX,*normGradY;
	
	if (featuresPyramid.nLevels >= 0)
	{
		normGradX = copy_image_nTuple((featuresPyramid.normGradX)[featuresPyramid.nLevels-1]);
		normGradY = copy_image_nTuple((featuresPyramid.normGradY)[featuresPyramid.nLevels-1]);
		//attach features to patchMatch parameters
		patchMatchParams->normGradX = normGradX;
		patchMatchParams->normGradY = normGradY;
	}
	
	while ( (occIter->sum_nTupleImage()) >0)
	{
		nTupleImage *occErode = imerode(occIter, structElErode);
		nTupleImage *occPatchMatch = copy_image_nTuple(occDilate);
		
		/***************************/
		/*******   NNSEARCH   ******/
		/***************************/		
		//indicate which pixels can be used for comparing patches
		//but we are not allowed to point to in the patchMatch (we set these pixels to 2)
		for (int x=0; x<(occPatchMatch->xSize); x++)
			for (int y=0; y<(occPatchMatch->ySize); y++)
			{
				if ( (occDilate->get_value(x,y,0) - occIter->get_value(x,y,0)) == 1)
					occPatchMatch->set_value(x,y,0,(imageDataType)2);
			}
			
		//set first guess
		nTupleImage *firstGuess = copy_image_nTuple(imgIn);
		//carry out patchMatch
		patch_match_ANN(imgIn,imgIn,shiftMap,occPatchMatch,occDilate,patchMatchParams,firstGuess);
		/***************************/
		/****   RECONSTRUCTION   ***/
		/***************************/
		nTupleImage *occReconstruct = copy_image_nTuple(occIter);
		//Indicate which pixels are on the current border, and need to be inpainted
		//Also, we indicate that the pixels inside the occlusion (and not on the border) are occluded (and
		//therefore not to be used for reconstruction) but should not
		//be inpainted at the current iteration. To indicate this
		//we set them to 2
		for (int x=0; x<(occPatchMatch->xSize); x++)
			for (int y=0; y<(occPatchMatch->ySize); y++)
			{
				imageDataType valueTemp = abs(occIter->get_value(x,y,0) - occErode->get_value(x,y,0));
				occReconstruct->set_value(x,y,0,valueTemp);
				if (occErode->get_value(x,y,0) == 1)
					occReconstruct->set_value(x,y,0,(imageDataType)2);
			}

		//call reconstruction function
		if (featuresPyramid.nLevels >= 0)
			reconstruct_image_and_features(imgIn, occReconstruct,
		    	normGradX, normGradY,
		    	shiftMap, SIGMA_COLOUR, AGGREGATED_PATCHES,initialisation);
		else
		{
			reconstruct_image(imgIn, occReconstruct, shiftMap, SIGMA_COLOUR, AGGREGATED_PATCHES, initialisation);
		}
		
		iterNb++;
		if (patchMatchParams->verboseMode == true)
			printf("\n Initialisation iteration number : %d \n",iterNb);
		delete occPatchMatch;
		delete occReconstruct;
		
		//copy the information from the eroded occlusion to the current occlusion (occVolIter)
		delete occIter;
		occIter = copy_image_nTuple(occErode);
		delete occErode;
	}
}

