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

/*!
 * Various helper functions used for the image inpainting code
 *
 */

#include "image_operations.h"

void seed_random_numbers( double inputSeed)
{
 	srand ( (unsigned int)inputSeed );   //create random number seed
}

std::string remove_extension_from_file(const char* fileIn)
{
	std::string filename = fileIn;
	
	size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos)
    	return ( filename);
	else
	{
		std::string fileOut = filename.substr(0, lastdot);
    	return ( fileOut);
	}

}

float * read_image(const char *fileIn, size_t *nx, size_t *ny, size_t *nc)
{

	float * pixel_stream = NULL;
	pixel_stream = read_png_f32(fileIn,nx,ny,nc);

	if (pixel_stream == NULL)
	{
		printf("Unable to get the image\n");
		return(NULL);
	}
	
	printf("xSize : %d\n",(int)*nx);
	printf("ySize : %d\n",(int)*ny);
	printf("nChannels : %d\n",(int)*nc);
	
	return(pixel_stream);
}

nTupleImage *make_colour_wheel()
{

	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;
	
	int nCols = RY + YG + GC + CB + BM + MR;
	
	nTupleImage * colourWheel = new nTupleImage(nCols,1,3,IMAGE_INDEXING);
	colourWheel->set_all_image_values(0);
	
	int col = 0;
	//RY
	for (int i=0;i<RY; i++)
	{
		colourWheel->set_value(i,0,0,(imageDataType)255);
		colourWheel->set_value(i,0,1,(imageDataType)floor(255*((float)i) / ( (float)RY)) );
	}
	col = col+RY;

	//YG
	for (int i=col;i<(col+YG); i++)
	{
		colourWheel->set_value(i,0,0,(imageDataType)(255 - (float)floor( 255 * ( ((float)i-col) / ((float)YG) ) ) ) );
		colourWheel->set_value(i,0,1,(imageDataType)255);
	}
	col = col+YG;

	//GC
	for (int i=col;i<(col+GC); i++)
	{
		colourWheel->set_value(i,0,1,(imageDataType)255);
		colourWheel->set_value(i,0,2,(imageDataType) floor( (float) 255 * ( (float) (i-col) )/  ((float)GC)) );
	}
	col = col+GC;

	//CB
	for (int i=col;i<(col+CB); i++)
	{
		colourWheel->set_value(i,0,1,(imageDataType) 255 - floor(255*( (float) (i-col) / ( (float) CB ) ) ) );
		colourWheel->set_value(i,0,2,(imageDataType)255);
	}
	col = col+CB;

	//BM
	for (int i=col;i<(col+BM); i++)
	{
		colourWheel->set_value(i,0, 2, (imageDataType) 255);
		colourWheel->set_value(i,0, 0, (imageDataType) floor(255* (float) ( (float)(i-col))/( (float)BM)) );
	}
	col = col+BM;

	//MR
	for (int i=col;i<(col+MR); i++)
	{
		colourWheel->set_value(i,0, 2, (imageDataType) 255 - floor( (float) 255* ( (float) (i-col) )/ ((float) MR)) );
		colourWheel->set_value(i,0, 0, (imageDataType)255);
	}
	
	return(colourWheel);

}

void write_image(nTupleImage *imgIn, const char *fileName, imageDataType normalisationScalar)
{
	int readWriteSuccess;

		//remove extension from filename
		//std::string fileWithoutExtension = remove_extension_from_file(fileName);
		
		const char* stringOut = fileName;
		
		//write image
		if (normalisationScalar>0)	//for better visulisation of results
		{
			nTupleImage* imgInCopy;
			//make a copy of the input image, so as not to modify the input
			imgInCopy = copy_image_nTuple(imgIn);
			//set minimum value to 0
			imageDataType minValue = imgIn->min_value();
			imgInCopy->add(-minValue);
			//normalise values
			imageDataType maxValue = imgIn->max_value();
			imgInCopy->multiply((imageDataType)1/maxValue);
			//set maximum value to normalisationScalar
			imgInCopy->multiply((imageDataType)normalisationScalar);
			
			readWriteSuccess = write_png_f32((char*)stringOut, imgInCopy->get_data_ptr(),
			  imgInCopy->xSize, imgInCopy->ySize, imgInCopy->nTupleSize);
			if (readWriteSuccess == -1)
			{
				printf("Unable to write the image\n");
				delete imgInCopy;
				return;
			}
			delete imgInCopy;
		}
		else //no normalisation
		{
			readWriteSuccess = write_png_f32((char*)stringOut, imgIn->get_data_ptr(),
			  imgIn->xSize, imgIn->ySize, imgIn->nTupleSize);
			if (readWriteSuccess == -1)
			{
				printf("Unable to write the image\n");
				return;
			}
		}
		
}

void write_shift_map(nTupleImage *shiftMap, const char *fileName)
{
	int readWriteSuccess;
	
	imageDataType maxNorm;
	maxNorm = sqrt( (shiftMap->xSize)*(shiftMap->xSize) + (shiftMap->ySize)*(shiftMap->ySize));
	
	nTupleImage *shiftMapColour;
	shiftMapColour = new nTupleImage(shiftMap->xSize, shiftMap->ySize, 3, IMAGE_INDEXING);
	
	//create colour wheel
	nTupleImage *colourWheel = make_colour_wheel();
	
	//convert shift volume to colour
	for (int x=0; x<(shiftMap->xSize); x++)
		for (int y=0; y<(shiftMap->ySize); y++)
		{
			imageDataType u,v;
			u = shiftMap->get_value(x,y,0);
			v = shiftMap->get_value(x,y,1);
			
			//calculate norm and normalise this with respect to the maximum
			//distance in the image (the diagonal)
			float vectorNorm = sqrt(u*u + v*v);
			vectorNorm = vectorNorm/maxNorm;
			
			//calculate angle
			float a = atan2(-v, -u)/(M_PI);
			
			float fk = ((a+1) /2) * ((float) ((colourWheel->xSize)-1) ) + 1;  // -1~1 maped to 1~ncols
			int k0 = (int)floor(fk);
			int k1 = k0+1;
			if (k1>= (colourWheel->xSize) )
				k1 = (colourWheel->xSize)-1;
			
			float f = fk -k0;
			
			for (int i=0; i< (colourWheel->nTupleSize); i++)
			{
				float col0 = (float) ((float)colourWheel->get_value(k0,0,i))/((float)255);
				float col1 = (float) ((float)colourWheel->get_value(k1,0,i))/((float)255);
				float col = (1-f)*col0 + f * col1;
				
				col = 1-vectorNorm * (1 - col);
				
				shiftMapColour->set_value(x,y,i,(imageDataType)col);
			}
		}
	
	//set minimum value to 0
	imageDataType minValue = shiftMapColour->min_value();
	shiftMapColour->add(-minValue);
	//normalise values
	imageDataType maxValue = shiftMapColour->max_value();
	shiftMapColour->multiply((imageDataType)1/maxValue);
	//set maximum value to normalisationScalar
	shiftMapColour->multiply((imageDataType)255);
	
	//remove extension from filename
	std::string fileWithoutExtension = remove_extension_from_file(fileName);
	
	std::ostringstream os;
	os << fileWithoutExtension << "_shift_map.png";
	const char* stringOut = (os.str()).c_str();
	
  	readWriteSuccess = write_png_f32((char*)stringOut, shiftMapColour->get_data_ptr(),
	              shiftMapColour->xSize, shiftMapColour->ySize, shiftMapColour->nTupleSize);
	if (readWriteSuccess == -1)
	{
		printf("Unable to write the image\n");
		delete colourWheel;
		delete shiftMapColour;
		return;
	}
	delete colourWheel;
	delete shiftMapColour;

}

void write_image_pyramid(nTupleImagePyramid imgInPyramid, int nLevels, char *fileName, imageDataType normalisationScalar)
{
	int readWriteSuccess;
	for (int i=0; i<nLevels; i++)
	{
		nTupleImage * currImg = imgInPyramid[i];
		
		std::ostringstream os;
		os << fileName << "_level_" << int_to_string(i) << ".png";
		const char* stringOut = (os.str()).c_str();
		
		//write image
		if (normalisationScalar>0)	//for better visulisation of results
		{
			//set minimum value to 0
			imageDataType minValue = currImg->min_value();
			currImg->add(-minValue);
			//normalise values
			imageDataType maxValue = currImg->max_value();
			currImg->multiply((imageDataType)1/maxValue);
			//set maximum value to normalisationScalar
			currImg->multiply((imageDataType)normalisationScalar);
		}
		
	  	readWriteSuccess = write_png_f32((char*)stringOut, currImg->get_data_ptr(),
		              currImg->xSize, currImg->ySize, currImg->nTupleSize);
		if (readWriteSuccess == -1)
		{
			printf("Unable to write the image\n");
			return;
		}
	}
}

//image subsampling : the image is subsampled by 1/subSampleFactor
// that is to say : the size of imgOut is (1/subSampleFactor)*size(imgVol)
//Careful : this function does not deal with aliasing problems
nTupleImage * sub_sample_image(nTupleImage *imgIn, float subSampleFactor)
{
	int xSizeOut,ySizeOut;
	
	xSizeOut = (int) ceil( (imgIn->xSize)/subSampleFactor);
	ySizeOut = (int) ceil( (imgIn->ySize)/subSampleFactor);
	
	nTupleImage * imgOut = new nTupleImage(xSizeOut,ySizeOut,imgIn->nTupleSize,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	for (int x=0; x< (imgOut->xSize); x++)
		for (int y=0; y<(imgOut->ySize); y++)
			for (int c=0; c<imgOut->nTupleSize; c++)
				imgOut->set_value((int)x,(int)y,c,imgIn->get_value(subSampleFactor*x,subSampleFactor*y,c));
				
	return(imgOut);
}

//image upsampling : the image is upsampled by upSampleFactor
//that is to say : the size of imgOut is upSampleFactor*size(imgIn)
//the image is upsamled by nearest neighbour interpolation
nTupleImage * up_sample_image(nTupleImage *imgIn, float upSampleFactor, nTupleImage *imgFine)
{
	int xSizeOut,ySizeOut;
	
	if (imgFine == NULL)
	{
		xSizeOut = (int) ceil( (imgIn->xSize)*upSampleFactor);
		ySizeOut = (int) ceil( (imgIn->ySize)*upSampleFactor);
	}
	else
	{
		xSizeOut = (int) (imgFine->xSize);
		ySizeOut = (int) (imgFine->ySize);
	}
	
	nTupleImage * imgOut = new nTupleImage(xSizeOut,ySizeOut,imgIn->nTupleSize,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	for (int x=0; x< (imgOut->xSize); x++)
		for (int y=0; y<(imgOut->ySize); y++)
			for (int c=0; c<imgOut->nTupleSize; c++)
			{
				imgOut->set_value((int)x,(int)y,c,
				imgIn->get_value((int)(floor((1/upSampleFactor)*x)), (int)(floor((1/upSampleFactor)*y)),c));
			}
				
	return(imgOut);
}

nTupleImage * rgb_to_grey(nTupleImage * imgIn)
{
	nTupleImage *imgGreyOut = new nTupleImage(imgIn->xSize,imgIn->ySize,1,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	for (int x=0; x<(imgIn->xSize); x++)
		for (int y=0; y<(imgIn->ySize); y++)
		{
			float greyValue = 0.2126*imgIn->get_value(x,y,0) + 0.7152*imgIn->get_value(x,y,1) + 0.0722*imgIn->get_value(x,y,2);
			imgGreyOut->set_value(x,y,0,(imageDataType)greyValue);
		}
	
	return(imgGreyOut);
}

nTupleImage * image_gradient_x(nTupleImage * imgIn)
{
	int destroyGreyImg = 0;
	nTupleImage *imgGrey;
	nTupleImage *gradX = new nTupleImage(imgIn->xSize,imgIn->ySize,1,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	//if we need to convert the image to greyscale
	if (imgIn->nTupleSize ==3)
	{
		imgGrey = rgb_to_grey(imgIn);
		destroyGreyImg = 1;
	}
	else
		imgGrey = imgIn;


	for (int x=0; x<(imgIn->xSize); x++)
		for (int y=0; y<(imgIn->ySize); y++)
		{
			int xMin = max_int(x-1,0);
			int xMax = min_int(x+1,(imgIn->xSize)-1);
			
			//gradient calculation
			imageDataType gradTemp = ( imgGrey->get_value(xMax,y,0) - imgGrey->get_value(xMin,y,0) )
			/((imageDataType)xMax-xMin);
			gradX->set_value(x,y,0,gradTemp);
		}
	
	if (destroyGreyImg>0)
		delete imgGrey;
	return gradX;

}

nTupleImage * image_gradient_y(nTupleImage * imgIn)
{
	int destroyGreyImg = 0;
	nTupleImage *imgGrey;
	nTupleImage *gradY = new nTupleImage(imgIn->xSize,imgIn->ySize,1,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	//if we need to convert the image to greyscale
	if (imgIn->nTupleSize ==3)
	{
		imgGrey = rgb_to_grey(imgIn);
		destroyGreyImg = 1;
	}
	else
		imgGrey = imgIn;


	for (int x=0; x<(imgIn->xSize); x++)
		for (int y=0; y<(imgIn->ySize); y++)
		{
			int yMin = max_int(y-1,0);
			int yMax = min_int(y+1,(imgIn->ySize)-1);
			
			//gradient calculation
			imageDataType gradTemp = ( imgGrey->get_value(x,yMax,0) - imgGrey->get_value(x,yMin,0) )
			/((imageDataType)yMax-yMin);
			gradY->set_value(x,y,0,gradTemp);
		}
	
	if (destroyGreyImg>0)
		delete imgGrey;
	return gradY;
}

nTupleImagePyramid create_nTupleImage_pyramid_binary(nTupleImage * imgIn, int nLevels)
{
	nTupleImagePyramid pyramidOut = (nTupleImage**)malloc( (size_t)nLevels*sizeof(nTupleImage*));
	
	float sigmaFilter = 1.5;
	int convSize = 3;
	nTupleImage *convKernel = create_convolution_kernel("gaussian", convSize, convSize, sigmaFilter);
	
	
	for (int i=0; i<nLevels; i++)
	{
		if(i==0)	//first level
		{
			nTupleImage* imgTemp = copy_image_nTuple(imgIn);
			imgTemp->binarise();
			pyramidOut[i] = imgTemp;
		}
		else
		{
			nTupleImage * imgTemp = normalised_convolution_masked(pyramidOut[i-1],convKernel);
			pyramidOut[i] = sub_sample_image(imgTemp, 2);
			(pyramidOut[i])->binarise();
			delete imgTemp ;
		}
	}
	
	//delete subsampling convolution filter
	delete convKernel;
	
	return(pyramidOut);

}

nTupleImagePyramid create_nTupleImage_pyramid(nTupleImage * imgIn, int nLevels)
{
	nTupleImagePyramid pyramidOut = (nTupleImage**)malloc( (size_t)nLevels*sizeof(nTupleImage*));
	
	float sigmaFilter = 1.5;
	int convSize = 3;
	nTupleImage *convKernel = create_convolution_kernel("gaussian", convSize, convSize, sigmaFilter);
	
	
	for (int i=0; i<nLevels; i++)
	{
		if(i==0)	//first level
		{
			nTupleImage* imgTemp = copy_image_nTuple(imgIn);
			pyramidOut[i] = imgTemp;
		}
		else
		{
			nTupleImage * imgTemp = normalised_convolution_masked(pyramidOut[i-1],convKernel);
			pyramidOut[i] = sub_sample_image(imgTemp, 2);
			delete imgTemp;
		}
	}
	
	//delete subsampling convolution filter
	delete convKernel;
	
	return(pyramidOut);

}

featurePyramid create_feature_pyramid(nTupleImage * imgIn, nTupleImage * occVol, int nLevels)
{
	featurePyramid featurePyramidOut;
	nTupleImage* imgTemp;
	//create the smoothing filter
	nTupleImage *convKernelX = create_convolution_kernel("average", (int)pow(2,(float)(nLevels)), 1);
	nTupleImage *convKernelY = create_convolution_kernel("average", 1, (int)pow(2,(float)(nLevels)));

	//calculate the image gradient
	nTupleImage * imgGradX = image_gradient_x(imgIn);
	nTupleImage * imgGradY = image_gradient_y(imgIn);
	
	//take absolute value of gradients and then average these values
	imgGradX->absolute_value();
	imgGradY->absolute_value();
	nTupleImage * imgGradXavg = normalised_convolution_masked_separable(imgGradX, convKernelX,convKernelY, occVol);//normalised_convolution_masked(imgVolGradX, convKernel, occVol);//
	nTupleImage * imgGradYavg = normalised_convolution_masked_separable(imgGradY, convKernelX,convKernelY, occVol);//normalised_convolution_masked(imgVolGradY, convKernel, occVol);//

	
	nTupleImagePyramid normGradXPyramid = (nTupleImage**)malloc( (size_t)nLevels*sizeof(nTupleImage*));
	nTupleImagePyramid normGradYPyramid = (nTupleImage**)malloc( (size_t)nLevels*sizeof(nTupleImage*));
	
	//subsample images
	for (int i=0; i<nLevels; i++)
	{
		if(i==0)	//first level
		{
			//x gradient
			imgTemp = copy_image_nTuple(imgGradXavg);
			normGradXPyramid[i] = imgTemp;
			//y gradient
			imgTemp = copy_image_nTuple(imgGradYavg);
			normGradYPyramid[i] = imgTemp;
		}
		else
		{
			//x gradient
			imgTemp = normGradXPyramid[i-1];
			normGradXPyramid[i] = sub_sample_image(imgTemp, 2);
			//x gradient
			imgTemp = normGradYPyramid[i-1];
			normGradYPyramid[i] = sub_sample_image(imgTemp, 2);
		}
	}
	
	featurePyramidOut.normGradX = normGradXPyramid;
	featurePyramidOut.normGradY = normGradYPyramid;
	featurePyramidOut.nLevels = nLevels;
	
	delete imgGradX;
	delete imgGradY;
	delete imgGradXavg;
	delete imgGradYavg;
	delete convKernelX;
	delete convKernelY;
	
	return(featurePyramidOut);
}


void delete_feature_pyramid(featurePyramid featurePyramidIn)
{
	 nTupleImagePyramid normGradXpyramid = featurePyramidIn.normGradX;
	 nTupleImagePyramid normGradYpyramid = featurePyramidIn.normGradY;
	 
	 for (int i=0; i<(featurePyramidIn.nLevels); i++)
	 {
	 	delete (normGradXpyramid[i]);
	 	delete (normGradYpyramid[i]);
	 }
	delete normGradXpyramid;
	delete normGradYpyramid;
}

int determine_multiscale_level_number(nTupleImage *occImgIn, int patchSizeX, int patchSizeY)
{

	int nLevels;
	int maxOccDistance=0;
	int maxPatchSize = (int) max_int(patchSizeX,patchSizeY);

	nTupleImage *structElErode = create_structuring_element("rectangle", 3, 3);

	nTupleImage *occImg = copy_image_nTuple(occImgIn);
	//write_image(occImg, "out_temp.png",255);

	while(occImg->sum_nTupleImage() >0)
	{
		nTupleImage *occImgTemp = imerode(occImg,structElErode);
		delete occImg;
		occImg = copy_image_nTuple(occImgTemp);
		delete occImgTemp;
		maxOccDistance++;
	}
	
	maxOccDistance = 2*maxOccDistance;
	nLevels = (int) floor( (float)
				(
				log( ((float) maxOccDistance) /((float)maxPatchSize) )
					) /
					( (float) log(2) )
					);
	//the number of levels must be at least 1
	nLevels = max_int(nLevels,1);
	delete occImg;
	return(nLevels);

}



