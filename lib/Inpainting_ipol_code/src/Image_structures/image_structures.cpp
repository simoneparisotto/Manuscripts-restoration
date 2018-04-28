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

//this functions holds the definitions for the image_structures used in the spatio-temporal patchMatch

#include "image_structures.h"

float min_float(float a, float b)
{
    if (a<b)
        return a;
    else
        return b;
}

float max_float(float a, float b)
{
    if (a>b)
        return a;
    else
        return b;
}

int min_int(int a, int b)
{
    if (a<b)
        return a;
    else
        return b;
}

int max_int(int a, int b)
{
    if (a>b)
        return a;
    else
        return b;
}

float rand_float_range(float a, float b)
{
	if (a == b)
		return a;
	else
		return ((b-a)*((float)rand()/RAND_MAX))+a;
}

int rand_int_range(int a, int b)
{
	if (a == b)
		return a;
	else
		return ( rand()%(b-a+1) + a);
}

float round_float(float a)
{
	float aFloor,aCeil;
	aFloor = (float)floor((float)a);
	aCeil = (float)ceil((float)a);
	if (a<0)	//less than 0
	{
		if ( (a-aCeil) < -0.5)
			return(aFloor);	//round up
		else
			return(aCeil);
	}
	else	//greater or equal to 0
	{
		if ( (a-aFloor) < 0.5)	//round down
			return(aFloor);
		else
			return(aCeil);
	}
}

long getMilliSecs()
{ 
    timeval t;    
    gettimeofday(&t, NULL);
    return t.tv_sec*1000 + t.tv_usec/1000;
}

char* int_to_string(int value)
{
	std::ostringstream os ;
	os << value ;
	std::string stringTemp = os.str();
	char * stringOut = new char[stringTemp.length() + 1];
	std::strcpy(stringOut,stringTemp.c_str());
	
	stringOut[stringTemp.length()] = '\0';
	return (stringOut);
}

int sub_to_ind(nTupleImage* imgIn, int x, int y)
{
	if (imgIn->indexing == ROW_FIRST)
    	return( y*(imgIn->xSize) + x);
	else if(imgIn->indexing == COLUMN_FIRST)
    	return( x*(imgIn->ySize) + y);
	else
	{
		MY_PRINTF("Here sub_to_ind (image_structures.cpp). Error: unknown image indexing type\n.");
		return(-1);
	}
}

void ind_to_sub(nTupleImage* imgIn, int linearIndex, int *x, int *y)
{
	if (imgIn->indexing == ROW_FIRST)
	{
		*y = (int) floor((float) (linearIndex) /
		        ( imgIn->xSize ) );
		*x = (int) (linearIndex - (*y)*(imgIn->xSize));
	}
	else if (imgIn->indexing == COLUMN_FIRST)
	{
		*x = (int) floor((float) (linearIndex) /
		        ( imgIn->ySize ) );
		*y = (int) (linearIndex - (*x)*(imgIn->ySize));
	}
	else
	{
		MY_PRINTF("Here ind_to_sub (image_structures.cpp). Error: unknown image indexing type\n.");
	}
}

void patch_index_to_sub(nTupleImage *imgIn, int patchIndex, int *colourInd,int *xInd, int *yInd)
{
    ASSERT(patchIndex>=0 && (patchIndex<((imgIn->patchSizeX)*(imgIn->patchSizeY)*(imgIn->nTupleSize)) ));
    
		*xInd = (int) floor((float) (patchIndex) /
				(( imgIn->patchSizeY )*(imgIn->nTupleSize)) );
		*yInd = (int) floor((float)(patchIndex - (*xInd)*(imgIn->patchSizeY)*(imgIn->nTupleSize))/
                (imgIn->nTupleSize));
        *colourInd = (int) (patchIndex - (*xInd)*(imgIn->patchSizeY)*(imgIn->nTupleSize) 
							- (*yInd)*(imgIn->nTupleSize) );
}

void show_patch_match_parameters(patchMatchParameterStruct *patchMatchParams)
{
	MY_PRINTF("Patch size x : %d\n", patchMatchParams->patchSizeX);
	MY_PRINTF("Patch size y : %d\n", patchMatchParams->patchSizeY);
	MY_PRINTF("nIters : %d\n", patchMatchParams->nIters);
	MY_PRINTF("w : %d\n", patchMatchParams->w);
	MY_PRINTF("alpha : %f\n", patchMatchParams->alpha);
	MY_PRINTF("partialComparison : %d\n", patchMatchParams->partialComparison);
    MY_PRINTF("fullSearch : %d\n", patchMatchParams->fullSearch);
    
    MY_PRINTF("\n");
}

int check_patch_match_parameters(patchMatchParameterStruct *patchMatchParams)
{
	//verify that parameters are positive
	if( (patchMatchParams->patchSizeX <0) || (patchMatchParams->patchSizeY <0) ||
		 (patchMatchParams->nIters <0) || (patchMatchParams->w <0) ||
		 (patchMatchParams->alpha <0) )
	{
		printf("Error, the parameters should be positive.\n");
		return(-1);
	}
	 
	//verify that the patch sizes are odd
	if( ( (patchMatchParams->patchSizeX)%2 == 0) || ( (patchMatchParams->patchSizeY)%2 == 0) )
	{
		printf("Error, the parameters should be odd.\n");
		return(-1);
	}
	return(1);
}

int check_in_boundaries( nTupleImage *imgIn, int x, int y)
{
    if ( (x>=0) && (y>=0) && (x < ( (imgIn->xSize) )) && (y < ( (imgIn->ySize) )) )
    {
            return 1;
    }
    else
        return 0;
}

//check if the pixel is in the inner boundary : that is, none of the pixels in the patch centred on this pixel are outside of the image boundary 
int check_in_inner_boundaries( nTupleImage *imgIn, int x, int y, const patchMatchParameterStruct *params)
{

	int hPatchSizeX = imgIn->hPatchSizeX;
	int hPatchSizeY = imgIn->hPatchSizeY;

    if ( (x>=hPatchSizeX) && (y>=hPatchSizeY) &&
	(x < ( (imgIn->xSize)-hPatchSizeX )) && (y < ( (imgIn->ySize)-hPatchSizeY )) )
    {
        return 1;
    }
    else
        return 0;
}

nTupleImage::nTupleImage()    //create empty image
{
    xSize = 0;
    ySize = 0;
	nDims = 3;
    patchSizeX = 0;
	patchSizeY = 0;
	hPatchSizeX = 0;
	hPatchSizeY = 0;
    nX = 0;
    nY = 0;
    nC = 0;
	nElsTotal = 0;
	values = NULL;
    indexing = -1;
    destroyValues = 0;
}

nTupleImage::nTupleImage(nTupleImage *imgIn)
{
    //copy information
	nTupleSize = imgIn->nTupleSize;

	xSize = imgIn->xSize;
	ySize = imgIn->ySize;
	patchSizeX = imgIn->patchSizeX;
	patchSizeY = imgIn->patchSizeY;
	hPatchSizeX = imgIn->hPatchSizeX;
	hPatchSizeY = imgIn->hPatchSizeY;

	indexing = imgIn->indexing;
	nX = imgIn->nX;
	nY = imgIn->nY;
	nC = imgIn->nC;

	nElsTotal = imgIn->nElsTotal;
	//copy the image info
	values = new imageDataType[nElsTotal*nTupleSize];
    memcpy(values,imgIn->get_value_ptr(0, 0, 0),nElsTotal*nTupleSize*sizeof(imageDataType));

    destroyValues = 1;
}

nTupleImage::nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn, int indexingIn)
{
    //declarations
	int i;
	nTupleSize = nTupleSizeIn;

	xSize = xSizeIn;
	ySize = ySizeIn;
	nDims = 3;
	patchSizeX = 0;
	patchSizeY = 0;
	hPatchSizeX = 0;
	hPatchSizeY = 0;

    if (indexingIn == 0)    //row first
    {
	nC = (xSize)*(ySize);
        nY = (xSize);
        nX = 1;
    }
    else if (indexingIn == 1)   //column first
    {
        nC = (ySize)*(xSize);
        nX = (ySize);
        nY = 1;
    }
    else
    {
        MY_PRINTF("Unknown indexing : %d\n", indexingIn);
    }

	nElsTotal = (xSize)*(ySize);
	//get dimensions and total number of elements
	values = new imageDataType[nElsTotal*nTupleSize];

	for (i=0; i<(nElsTotal*nTupleSize); i++)
		values[i] = 0;
    
    indexing = indexingIn;
    destroyValues = 1;
}

nTupleImage::nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn,
            int patchSizeXIn, int patchSizeYIn, int indexingIn)
{
	//declarations
	int i;
	nTupleSize = nTupleSizeIn;

	xSize = xSizeIn;
	ySize = ySizeIn;
	nDims = 3;
	patchSizeX = patchSizeXIn;
	patchSizeY = patchSizeYIn;
	hPatchSizeX = (int)floor((float)patchSizeX/2);
	hPatchSizeY = (int)floor((float)patchSizeY/2);

    if (indexingIn == 0)    //row first
    {
		nC = (xSize)*(ySize);
        nY = (xSize);
        nX = 1;
    }
    else if (indexingIn == 1)   //column first
    {
        nC = (ySize)*(xSize);
        nX = (ySize);
        nY = 1;
    }
    else
    {
        MY_PRINTF("Unknown indexing : %d\n", indexingIn);
    }

	nElsTotal = (xSize)*(ySize);
	//get dimensions and total number of elements
	values = new imageDataType[nElsTotal*nTupleSize];

	for (i=0; i<(nElsTotal*nTupleSize); i++)
		values[i] = 0;
    
    indexing = indexingIn;  //row first
    destroyValues = 1;
}

//create image volume with an already existing array for the values
nTupleImage::nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn,
            int patchSizeXIn, int patchSizeYIn, int indexingIn, imageDataType* valuesIn)
{
	//declarations
	nTupleSize = nTupleSizeIn;

	xSize = xSizeIn;
	ySize = ySizeIn;
	nDims = 3;
	patchSizeX = patchSizeXIn;
	patchSizeY = patchSizeYIn;
	hPatchSizeX = (int)floor((float)patchSizeX/2);
	hPatchSizeY = (int)floor((float)patchSizeY/2);

    if (indexingIn == 0)    //row first
    {
		nC = (xSize)*(ySize);
        nY = (xSize);
        nX = 1;
    }
    else if (indexingIn == 1)   //column first
    {
        nC = (ySize)*(xSize);
        nX = (ySize);
        nY = 1;
    }
    else
    {
        MY_PRINTF("Unknown indexing : %d\n", indexingIn);
    }

	nElsTotal = (xSize)*(ySize);
	//get dimensions and total number of elements
	values = valuesIn;
    
    indexing = indexingIn;
    destroyValues = 0;
}

nTupleImage::~nTupleImage()
{
	if((xSize)> 0)
    {
        if (destroyValues  == 1)
            delete[] values;
    }
}

imageDataType nTupleImage::get_value(int x, int y, int c)
{
	//check parameters
	if( (x<0) || (y<0) || (c<0) || (x>=xSize) || (y>=ySize) || (c>=nTupleSize))
	{
		MY_PRINTF("Error, in get_value. At least one of the indices is incorrect.\n");
		MY_PRINTF(" x = %d \n y = %d\n c = %d\n",x,y,c);
		MY_PRINTF("xSize : %d, ySize : %d, nTupleSize : %d\n\n",xSize,ySize,nTupleSize);
		return(-1);
	}
    return( values[ (x*(nX)) + (y*(nY)) + c*(nC)] );
}

imageDataType* nTupleImage::get_value_ptr(int x, int y, int c)
{
	//check parameters
	if( (x<0) || (y<0) || (c<0) || (x>=xSize) || (y>=ySize) || (c>=nTupleSize))
	{
		MY_PRINTF("Error, in get_value_nTuple_volume. At least one of the indices is incorrect.\n");
		MY_PRINTF(" x = %d \n y = %d\n c = %d\n",x,y,c);
		MY_PRINTF("xSize : %d, ySize : %d, nTupleSize : %d\n\n",xSize,ySize,nTupleSize);
		return(NULL);
	}
	return( (values) + (  (y*(nY)) + (x*(nX)) + c*(nC) ) );
}

imageDataType* nTupleImage::get_data_ptr()
{
	return(values);
}

void nTupleImage::set_value(int x, int y, int c, imageDataType value)
{
	//check parameters
	if( (x<0) || (y<0) || (c<0) || (x>=xSize) || (y>=ySize) || (c>=nTupleSize))
	{
		MY_PRINTF("Error, at least one of the indices is incorrect.\n");
		MY_PRINTF("x = %d \n y = %d\n c = %d\n",x,y,c);
		MY_PRINTF("xSize : %d, ySize : %d, nTupleSize : %d\n\n",xSize,ySize,nTupleSize);
	}
	values[ (x*(nX)) + (y*(nY)) + c*(nC)] = value;
}

void nTupleImage::set_all_image_values(imageDataType value)
{
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
				set_value(x,y,c,value);
}

void nTupleImage::add(imageDataType addScalar)
{
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
				set_value(x,y,c,(imageDataType) (addScalar+get_value(x,y,c)));
}

imageDataType nTupleImage::sum_nTupleImage()
{
	int sumImage = 0;
	for (int x=0; x<(xSize); x++)
		for(int y=0; y<(ySize); y++)
			for (int c=0; c<nTupleSize; c++)
				sumImage = sumImage + (int)round_float(get_value(x,y,c));
	return((imageDataType)sumImage);
}

void nTupleImage::multiply(imageDataType multiplyFactor)
{
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
				set_value(x,y,c,(imageDataType) multiplyFactor*get_value(x,y,c));
}

imageDataType nTupleImage::max_value()
{
	imageDataType maxVal = 0;
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
			{
				imageDataType currVal = get_value(x,y,c);
				if (currVal>maxVal)
					maxVal = currVal;
			}
	return(maxVal);
}

imageDataType nTupleImage::min_value()
{
	imageDataType minVal = 100000;
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
			{
				imageDataType currVal = get_value(x,y,c);
				if (currVal < minVal)
					minVal = currVal;
			}
	return(minVal);
}

void nTupleImage::absolute_value()
{
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
			{
				imageDataType currVal = get_value(x,y,c);
				set_value(x,y,c,(imageDataType)fabs(currVal));
			}
}

imageDataType nTupleImage::mean_value()
{
	imageDataType meanVal = 0;
	int numberOfElements = 0;
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
			{
				meanVal = meanVal + get_value(x,y,c);
				numberOfElements++;
			}
	return(meanVal/numberOfElements);
}

void nTupleImage::binarise()
{
	for (int x=0; x<xSize; x++)
		for(int y=0; y<ySize; y++)
			for (int c=0; c<nTupleSize; c++)
			{
				imageDataType currValue = get_value(x,y,c);
				if (currValue>0)	//set this value to 1
				{
					set_value(x,y,c,(imageDataType)1);
				}
				else
				{
					set_value(x,y,c,(imageDataType)0);
				}
			}
}


void nTupleImage::display_attributes()
{
	MY_PRINTF("xSize : %d, ySize : %d, nTupleSize : %d\n",xSize,ySize,nTupleSize);
	MY_PRINTF("patchSizeX : %d, patchSizeY : %d\n",patchSizeX,patchSizeY);
	MY_PRINTF("max value : %f, min value : %f\n\n",this->max_value(),this->min_value());
}

void clamp_coordinates(nTupleImage* imgIn, int *x, int *y)
{
    *x = max_int(min_int( *x, (imgIn->xSize)-1),0);
    *y = max_int(min_int( *y, (imgIn->ySize)-1),0);
}

void copy_pixel_values_nTuple_image(nTupleImage *imgA, nTupleImage *imgB, int xA, int yA, int xB, int yB)
{
	if(imgA->nTupleSize != imgB->nTupleSize)
	{
		MY_PRINTF("Here copy_pixel_values_nTuple_image. Error, the images do not have the same number of channels.\n");
	}
	
	for (int c=0; c<imgA->nTupleSize; c++)
	{
		imageDataType pixelValueTemp = imgA->get_value(xA,yA,c);
		imgB->set_value(xB,yB,c,pixelValueTemp);
	}

}


nTupleImage* copy_image_nTuple(nTupleImage *imgIn)
{
	nTupleImage *imgOut = new nTupleImage(imgIn->xSize, imgIn->ySize, imgIn->nTupleSize, imgIn->patchSizeX, imgIn->patchSizeY, imgIn->indexing);
	
	for (int x=0; x< (int)imgOut->xSize; x++)
		for (int y=0; y< (int)imgOut->ySize; y++)
			for (int c=0; c< (int)imgOut->nTupleSize; c++)
			{
				imageDataType valTemp = imgIn->get_value(x,y,c);
				imgOut->set_value(x,y,c,valTemp);
			}
	return(imgOut);
}

imageDataType calculate_residual(nTupleImage *imgIn, nTupleImage *imgInPrevious, nTupleImage *occIn)
{
	imageDataType residual = 0.0;
	int sumOcc = 0;
	
	for (int x=0; x<(int)imgIn->xSize; x++)
		for (int y=0; y<(int)imgIn->ySize; y++)
			for (int c=0; c<(int)imgIn->nTupleSize; c++)
				if (occIn->get_value(x,y,0) > 0)
				{
					residual = residual + (imageDataType)fabs( (float)(imgIn->get_value(x,y,c)) - (float)(imgInPrevious->get_value(x,y,c))); 
					sumOcc++;
				}
	return( residual/( (imageDataType)sumOcc));
}





