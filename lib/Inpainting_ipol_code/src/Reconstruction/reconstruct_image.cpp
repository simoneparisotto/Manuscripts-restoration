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

/*this function estimates the colours of pixels, given a shift map
showing the nearest neighbours of patches*/

#include "reconstruct_image.h"

/*this function calculates a nearest neighbour field, from imgA to imgB*/
void reconstruct_image(nTupleImage* imgIn, nTupleImage* occIn,
        nTupleImage* shiftMap, float sigmaColour, int reconstructionType, bool initialisation)
{
	int useAllPatches;
	if (initialisation==true)
		useAllPatches = 0;
	else
		useAllPatches = 1;
	
	/*decalarations*/
    int iMin,iMax,jMin,jMax;
    int weightInd;
    int xDisp, yDisp,xDispShift,yDispShift;
    int hPatchSizeX,hPatchSizeY;
    int nbNeighbours;
    int correctInfo;
    float alpha, adaptiveSigma;
    float *weights,sumWeights, *colours, *avgColours;

    hPatchSizeX = imgIn->hPatchSizeX;
    hPatchSizeY = imgIn->hPatchSizeY;
    
    /*allocate the (maximum) memory for the weights*/
    nbNeighbours = (imgIn->patchSizeX)*(imgIn->patchSizeY);
    weights = new float[nbNeighbours];
    colours = new float[(imgIn->nTupleSize)*nbNeighbours];
    avgColours = new float[imgIn->nTupleSize];
    
	/*check certain parameters*/
	if( (imgIn->patchSizeX != (imgIn->patchSizeX)) || (imgIn->patchSizeY != (imgIn->patchSizeY)) )	/*check that the patch sizes are equal*/
	{
		MY_PRINTF("Error in estimate_colour, the size of the patches are not equal in the two image volumes.");
		return;
	}
	if ( ( imgIn->patchSizeX > imgIn->xSize) || ( imgIn->patchSizeY > imgIn->ySize) )	/*check that the patch size is less or equal to each dimension in the images*/
	{
		MY_PRINTF("Error in estimate_colour, the patch size is to large for one or more of the dimensions of the image volume.");
		return;
	}

    for (int j=0; j<(occIn->ySize); j++)
        for (int i=0; i<(occIn->xSize); i++)
        {    
            if ( ((occIn->get_value(i,j,0)) == 0) || ((occIn->get_value(i,j,0) == 2) )  )
                continue;
            else    /*an occluded pixel (therefore to be modified)*/
            {
                if (reconstructionType == 1 )
                {
                    xDisp = i + (int)shiftMap->get_value(i,j,0);
                    yDisp = j + (int)shiftMap->get_value(i,j,1);

                    ////if pure replacing of pixels
                    copy_pixel_values_nTuple_image(imgIn, imgIn,xDisp, yDisp, i, j);
                    continue;
                }
                 
                //initialisation of the weight and colour vectors
                for (int ii=0;ii<(imgIn->patchSizeX)*(imgIn->patchSizeY); ii++)
				{
					weights[ii] = (float)-1;
					for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
					{
						colours[ii + colourInd*nbNeighbours] = (float)-1;
					}
				}
				for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
				{
					avgColours[colourInd] = (float)0.0;
				}

                sumWeights = 0.0;
                alpha = FLT_MAX;
                correctInfo = 0;
                
                iMin = max_int(i - hPatchSizeX,0);
                iMax = min_int(i + hPatchSizeX,(imgIn->xSize)-1 );
                jMin = max_int(j - hPatchSizeY,0);
                jMax = min_int(j + hPatchSizeY,(imgIn->ySize)-1 );
                
                /*
                MY_PRINTF("iMin : %d, iMax : %d\n",iMin,iMax);
                MY_PRINTF("jMin : %d, jMax : %d\n",jMin,jMax);*/
                /*first calculate the weights*/
                for (int jj=jMin; jj<=jMax;jj++)
                    for (int ii=iMin; ii<=iMax;ii++)
                    {
                        /*get ssd similarity*/
                        xDisp = ii + (int)shiftMap->get_value(ii,jj,0);
                        yDisp = jj + (int)shiftMap->get_value(ii,jj,1);
                        /*(spatio-temporally) shifted values of the covering patches*/
                        xDispShift = xDisp - (ii-i);
                        yDispShift = yDisp - (jj-j);
                
                         if (useAllPatches == 1)
                         {
                             
                            alpha = (float)min_float(shiftMap->get_value(ii,jj,2),alpha); 
                            weightInd = (int)((jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                            weights[weightInd] = shiftMap->get_value(ii,jj,2);
                            
                            
                            for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
							{
								colours[weightInd + colourInd*nbNeighbours] = (float)(imgIn->get_value(xDispShift,yDispShift,colourInd));
							}
                            correctInfo = 1;
                         }
                         else   /*only use some of the patches*/
                         {
                             if (((occIn->get_value(ii,jj,0)) == 0) || (occIn->get_value(ii,jj,0) ==-1))
                             {
                                alpha = (float)min_float(shiftMap->get_value(ii,jj,2),alpha); 
                                weightInd = (int)((jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                                weights[weightInd] = shiftMap->get_value(ii,jj,2);
                                
                                for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
								{
									colours[weightInd + colourInd*nbNeighbours] = (float)(imgIn->get_value(xDispShift,yDispShift,colourInd));
								}
                                correctInfo = 1;
                             }
                             else
                             {
                                weightInd = (int)( (jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                                weights[weightInd] = -1;
                                 
                                for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
								{
									colours[weightInd + colourInd*nbNeighbours] = (float)-1;
								}
                                continue;
                             }
                         }
                    }
                
                alpha = max_float(alpha,1);
                if (correctInfo == 0)
                    continue;

                if (reconstructionType == 3)
                {
                    estimate_best_colour(imgIn, weights, nbNeighbours, colours, i, j);
                    continue;
                }
                //get the 75th percentile of the distances for setting the adaptive sigma
                adaptiveSigma = get_adaptive_sigma(weights,(imgIn->patchSizeX)*(imgIn->patchSizeY),sigmaColour);
				adaptiveSigma = max_float(adaptiveSigma,(float)0.1);
                
                /* ///MY_PRINTF("alpha : %f\n",alpha);
                //adjust the weights : note, the indices which are outside the image boundaries
                //will have no influence on the final weights (they are initialised to 0)  */
                for (int jj=jMin; jj<=jMax;jj++)
                    for (int ii=iMin; ii<=iMax;ii++)
                    {
                        if (useAllPatches)
                        {
                            /*weights = exp( -weights/(2*sigma*alpha))*/
                            weightInd = (int)((jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                            weights[weightInd] = (float)(exp( - ((weights[weightInd])/(2*adaptiveSigma*adaptiveSigma)) ));/*exp( - ((weights[ii])/(2*sigmaColour*sigmaColour*alpha)) );*/
                            //
                            sumWeights = (float)(sumWeights+weights[weightInd]);
                        }
                        else   /*only use some of the patches*/
                        {
                             if (((occIn->get_value(ii,jj,0)) == 0) || (occIn->get_value(ii,jj,0) ==-1))
                             {
                                /*weights = exp( -weights/(2*sigma*alpha))*/
                                weightInd = (int)((jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                                weights[weightInd] = (float)(exp( - ((weights[weightInd])/(2*adaptiveSigma*adaptiveSigma)) ));/*exp( - ((weights[ii])/(2*sigmaColour*sigmaColour*alpha)) );*/
                                //
                                sumWeights = (float)(sumWeights+weights[weightInd]);
                             }
                             else
                                 continue;
                        }
                    }

                /*now calculate the pixel value(s)*/
                for (int jj=jMin; jj<=jMax;jj++)
                    for (int ii=iMin; ii<=iMax;ii++)
                    {
                        if (useAllPatches)
                        {
                            weightInd = (int)( (jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                            /*get ssd similarity*/
                            xDisp = ii + (int)shiftMap->get_value(ii,jj,0);
                            yDisp = jj + (int)shiftMap->get_value(ii,jj,1);
                            /*(spatio-temporally) shifted values of the covering patches*/
                            xDispShift = xDisp - (ii-i);
                            yDispShift = yDisp - (jj-j);
                            
                            for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
							{
								avgColours[colourInd] = avgColours[colourInd] + (float)(weights[weightInd])*(imgIn->get_value(xDispShift,yDispShift,colourInd));
							}
                        }
                        else
                        {
                             if (((occIn->get_value(ii,jj,0)) == 0) || (occIn->get_value(ii,jj,0) ==-1))
                             {
                                weightInd = (int)((jj-jMin)*(imgIn->patchSizeX) + ii-iMin);
                                /*get ssd similarity*/
                                xDisp = ii + (int)shiftMap->get_value(ii,jj,0);
                                yDisp = jj + (int)shiftMap->get_value(ii,jj,1);
                                /*(spatio-temporally) shifted values of the covering patches*/
                                xDispShift = xDisp - (ii-i);
                                yDispShift = yDisp - (jj-j);
                                for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
								{
									avgColours[colourInd] = avgColours[colourInd] + (float)(weights[weightInd])*(imgIn->get_value(xDispShift,yDispShift,colourInd));
								}
                             }
                             else
                                 continue;
                        }
                    }
                     /*MY_PRINTF("SumWeights : %f\n",sumWeights);*/
                for (int colourInd=0; colourInd<(imgIn->nTupleSize); colourInd++)
				{
					imgIn->set_value(i,j,colourInd,(imageDataType)((avgColours[colourInd])/(sumWeights)));
				}
                /*set_value_nTuple_volume(occVol,i,j,k,0,0);*/
            }
        }

        delete[] weights;
        delete[] colours;
        delete[] avgColours;
        return;
}
