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

//definitions for the morphological operations

#include "morpho.h"

nTupleImage* create_structuring_element(const char * structType, int xSize, int ySize)
{
	
	if (strcmp(structType,"rectangle") == 0)
	{
		nTupleImage *structElOut =
		new nTupleImage(xSize,ySize,2,0,0,IMAGE_INDEXING);
	
		int xMin = -floor((xSize)/2);
		int yMin = -floor((ySize)/2);
		
		for (int x=0; x<xSize; x++)
			for (int y=0; y<ySize; y++)
			{
				int xTemp = xMin+x;
				int yTemp = yMin+y;
				structElOut->set_value(x,y,0,(imageDataType)xTemp);
				structElOut->set_value(x,y,1,(imageDataType)yTemp);
			}
		
		return(structElOut);
	}
	else
	{
		MY_PRINTF("Error in create_structuring_element. The structuring element type is not recognised.\n");
		return NULL;
	}

}

nTupleImage* imerode(nTupleImage *imgIn, nTupleImage *structEl)
{
	//create output (eroded) image
	nTupleImage *imgEroded =
	new nTupleImage(imgIn->xSize,imgIn->ySize,1,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	//erosion
	for (int x=0;x<imgIn->xSize;x++)
		for (int y=0;y<imgIn->ySize;y++)
		{
			imageDataType newValue = imgIn->get_value(x,y,0);
			for (int xStructEl=0;xStructEl<structEl->xSize;xStructEl++)
			{
				for (int yStructEl=0;yStructEl<structEl->ySize;yStructEl++)
				{
					int xShift = x+structEl->get_value(xStructEl,yStructEl,0);
					int yShift = y+structEl->get_value(xStructEl,yStructEl,1);
					if (xShift>=0 && xShift<(imgIn->xSize) && yShift>=0 && yShift<(imgIn->ySize))
					{
						imageDataType currValue = imgIn->get_value(xShift,yShift,0);
				  		if (currValue<newValue)
				  			newValue=currValue;	  
					}
				}
			}
			//set eroded value
			imgEroded->set_value(x,y,0,newValue);
		}
	return(imgEroded);
}

nTupleImage* imdilate(nTupleImage *imgIn, nTupleImage *structEl)
{

	//create output (eroded) image
	nTupleImage *imgDilated =
	new nTupleImage(imgIn->xSize,imgIn->ySize,1,imgIn->patchSizeX,imgIn->patchSizeY,imgIn->indexing);
	
	//dilation
	for (int x=0;x<imgIn->xSize;x++)
		for (int y=0;y<imgIn->ySize;y++)
		{
			imageDataType newValue = imgIn->get_value(x,y,0);
			for (int xStructEl=0;xStructEl<structEl->xSize;xStructEl++)
			{
				for (int yStructEl=0;yStructEl<structEl->ySize;yStructEl++)
				{
					int xShift = x+structEl->get_value(xStructEl,yStructEl,0);
					int yShift = y+structEl->get_value(xStructEl,yStructEl,1);
					if (xShift>=0 && xShift<(imgIn->xSize) && yShift>=0 && yShift<(imgIn->ySize))
					{
						imageDataType currValue = imgIn->get_value(xShift,yShift,0);
				  		if (currValue>newValue)
				  			newValue=currValue;	  
					}
				}
			}
			//set eroded value
			imgDilated->set_value(x,y,0,newValue);
		}
	return(imgDilated);

}

