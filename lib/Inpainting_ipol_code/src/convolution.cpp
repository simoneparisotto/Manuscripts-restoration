

#include "convolution.h"


nTupleImage * create_convolution_kernel(const char * kernelType, int xSizeKernel, int ySizeKernel, float stdDev)
{
	nTupleImage *convKernel = new nTupleImage(xSizeKernel,ySizeKernel,1,0,0,IMAGE_INDEXING);

	
	if (strcmp(kernelType,"gaussian") == 0)	//gaussian filter
	{
		for (int x=(int)-(floor(convKernel->xSize)/2); x<=(int)(floor(convKernel->xSize)/2); x++)
			for (int y=(int)-(floor(convKernel->ySize)/2); y<=(int)(floor(convKernel->ySize)/2); y++)
			{
				float distance = (x)*(x) + (y)*(y);
				convKernel->set_value(x+(int)(floor(convKernel->xSize)/2),
				y+(int)(floor(convKernel->ySize)/2),0, exp( -(distance)/(2*stdDev*stdDev) ) );
			}
	}
	else if(strcmp(kernelType,"average") == 0)
	{
		//set convolution kernel
		for (int x=0; x<(convKernel->xSize); x++)
			for (int y=0; y<(convKernel->ySize); y++)
				convKernel->set_value(x,y,0,(imageDataType)1 );
	}
	else
	{
		MY_PRINTF("Here create_convolution_kernel. The convolution kernel type is unrecognised.\n");
	}
	
	return(convKernel);
}

nTupleImage * normalised_convolution_masked(nTupleImage *imgIn, nTupleImage *convKernel, nTupleImage *occlusionMask)
{
	nTupleImage * imgConvolved = new nTupleImage(imgIn->xSize, imgIn->ySize, imgIn->nTupleSize,
	imgIn->patchSizeX, imgIn->patchSizeY, imgIn->indexing);

	for (int p=0; p<imgIn->nTupleSize; p++)
		for (int x=0; x<imgIn->xSize; x++)
			for (int y=0; y<imgIn->ySize; y++)
			{
				imageDataType sumConv = 0;
				imageDataType convTemp = 0;
				for (int xKernel=0; xKernel<(convKernel->xSize); xKernel++)
					for (int yKernel=0; yKernel<(convKernel->ySize); yKernel++)
					{
						int xShift = x - (int)floor((convKernel->xSize)/2) + xKernel;
						int yShift = y - (int)floor((convKernel->ySize)/2) + yKernel;
				
						if ( xShift>=0 && xShift<(imgIn->xSize) && yShift>=0 && yShift<(imgIn->ySize) )
						{
							if ( occlusionMask != NULL)
							{
								if (occlusionMask->get_value(xShift,yShift,0) ==0)
								{
									convTemp = convTemp + ( imgIn->get_value(xShift,yShift,p) )*
									(convKernel->get_value(xKernel,yKernel,0) );
									sumConv = sumConv + convKernel->get_value(xKernel,yKernel,0);
								}
							}
							else
							{
								convTemp = convTemp + ( imgIn->get_value(xShift,yShift,p) )*
									(convKernel->get_value(xKernel,yKernel,0) );
								sumConv = sumConv + convKernel->get_value(xKernel,yKernel,0);
							}
						}
					}
				if (sumConv == 0)	//if no unmasked pixels are available here
					imgConvolved->set_value(x,y,p,(imageDataType)0);
				else
					imgConvolved->set_value(x,y,p,(imageDataType)convTemp/sumConv);
			}

	return imgConvolved;
}

nTupleImage * normalised_convolution_masked_separable(nTupleImage *imgIn, nTupleImage *convKernelX, nTupleImage *convKernelY, nTupleImage *occlusionMask)
{
	nTupleImage * imgConvolved = new nTupleImage(imgIn->xSize, imgIn->ySize,imgIn->nTupleSize,
	imgIn->patchSizeX, imgIn->patchSizeY, imgIn->indexing);

	for (int p=0; p<imgIn->nTupleSize; p++)
		for (int x=0; x<imgIn->xSize; x++)
			for (int y=0; y<imgIn->ySize; y++)
			{
				imageDataType sumConv = 0;
				imageDataType convTemp = 0;
				for (int xKernel=0; xKernel<(convKernelX->xSize); xKernel++)
					for (int yKernel=0; yKernel<(convKernelX->ySize); yKernel++)
					{
						int xShift = x - (int)floor((convKernelX->xSize)/2) + xKernel;
						int yShift = y - (int)floor((convKernelX->ySize)/2) + yKernel;
				
						if ( xShift>=0 && xShift<(imgIn->xSize) && yShift>=0 && yShift<(imgIn->ySize) )
						{
							if ( occlusionMask != NULL)
							{
								if (occlusionMask->get_value(xShift,yShift,0) ==0)
								{
									convTemp = convTemp + ( imgIn->get_value(xShift,yShift,p) )*
									(convKernelX->get_value(xKernel,yKernel,0) );
									sumConv = sumConv + convKernelX->get_value(xKernel,yKernel,0);
								}
							}
							else
							{
								convTemp = convTemp + ( imgIn->get_value(xShift,yShift,p) )*
									(convKernelX->get_value(xKernel,yKernel,0) );
								sumConv = sumConv + convKernelX->get_value(xKernel,yKernel,0);
							}
						}
					}
				if (sumConv == 0)	//if no unmasked pixels are available here
					imgConvolved->set_value(x,y,p,(imageDataType)0);
				else
					imgConvolved->set_value(x,y,p,(imageDataType)convTemp/sumConv);
			}

	//now convolve with the other filter
	for (int p=0; p<imgConvolved->nTupleSize; p++)
		for (int x=0; x<imgConvolved->xSize; x++)
			for (int y=0; y<imgConvolved->ySize; y++)
			{
				imageDataType sumConv = 0;
				imageDataType convTemp = 0;
				for (int xKernel=0; xKernel<(convKernelY->xSize); xKernel++)
					for (int yKernel=0; yKernel<(convKernelY->ySize); yKernel++)
					{
						int xShift = x - (int)floor((convKernelY->xSize)/2) + xKernel;
						int yShift = y - (int)floor((convKernelY->ySize)/2) + yKernel;
				
						if ( xShift>=0 && xShift<(imgIn->xSize) && yShift>=0 && yShift<(imgIn->ySize) )
						{
							if ( occlusionMask != NULL)
							{
								if (occlusionMask->get_value(xShift,yShift,0) ==0)
								{
									convTemp = convTemp + ( imgConvolved->get_value(xShift,yShift,p) )*
									(convKernelY->get_value(xKernel,yKernel,0) );
									sumConv = sumConv + convKernelY->get_value(xKernel,yKernel,0);
								}
							}
							else
							{
								convTemp = convTemp + ( imgConvolved->get_value(xShift,yShift,p) )*
									(convKernelY->get_value(xKernel,yKernel,0) );
								sumConv = sumConv + convKernelY->get_value(xKernel,yKernel,0);
							}
						}
					}
				if (sumConv == 0)	//if no unmasked pixels are available here
					imgConvolved->set_value(x,y,p,(imageDataType)0);
				else
					imgConvolved->set_value(x,y,p,(imageDataType)convTemp/sumConv);
			}
	return imgConvolved;
}
