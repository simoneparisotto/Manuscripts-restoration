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


/**
 * @mainpage This code implements a patch-based variational image inpainting framework
 *
 * README.txt:
 * @verbinclude README.txt
 */


#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "image_inpainting.h"

static void show_help();

/// help on usage of inpainting code
static void show_help() {
    std::cerr <<"\nNon-local patch-based image inpainting.\n"
              << "Usage: " << " inpaint_image imgIn.png imgOccIn.png imgNameOut.png  [options]\n\n"
              << "Options (default values in parentheses)\n"
              << "Patch size in x direction \n"
              << "    -patchSizeX : patch size in the x direction ("
              <<7<<")\n"
              << "    -patchSizeY : patch size in the y direction ("
              <<7<<")\n"
              << "    -nLevels : number of pyramid levels (by default, determined automatically by the algorithm)\n"
              << "    -useFeatures : whether to use features, 0 for false, 1 for true ("
              <<1<<")\n"
              << "    -v : verbose mode, 0 for false, 1 for true ("
              <<0<<")\n"
              << std::endl;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/// main function of the inpainting code
int main(int argc, char* argv[])
{

	time_t startTime,stopTime;
		
	if(argc < 3) {
        show_help();
        return -1;
    }

	//get file names
	const char *fileIn = argv[1];
	const char *fileInOcc = argv[2];
	const char *fileOut = argv[3];
	
	const char * patchSizeX;
	const char * patchSizeY;
	const char * nLevels;
	const char * useFeatures = (argc >= 8) ? argv[7] : "1";
	const char * verboseMode = (argc >= 9) ? argv[8] : "0";
	
	//show help
	if(cmdOptionExists(argv, argv+argc, "-h"))
	{
		show_help();
		return(-1);
	}
	
	//patch sizes
	if(cmdOptionExists(argv, argv+argc, "-patchSizeX"))
		patchSizeX = getCmdOption(argv, argv + argc, "-patchSizeX");
	else
		patchSizeX = "7";
	if(cmdOptionExists(argv, argv+argc, "-patchSizeY"))
		patchSizeY = getCmdOption(argv, argv + argc, "-patchSizeY");
	else
		patchSizeY = "7";
	
	//number of pyramid levels
	if(cmdOptionExists(argv, argv+argc, "-nLevels"))
		nLevels = getCmdOption(argv, argv + argc, "-nLevels");
	else
		nLevels = "-1";

	//whether to use texture features or not
	if(cmdOptionExists(argv, argv+argc, "-useFeatures"))
		useFeatures = getCmdOption(argv, argv + argc, "-useFeatures");
	else
		useFeatures = "1";
		
	//whether to use texture features or not
	if(cmdOptionExists(argv, argv+argc, "-v"))
		verboseMode = "1";
	else
		verboseMode = "0";

	
	time(&startTime);//startTime = clock();
	
	inpaint_image_wrapper(fileIn,fileInOcc,fileOut,
		atoi(patchSizeX), atoi(patchSizeY), atoi(nLevels), (bool)atoi(useFeatures), (bool)atoi(verboseMode));
	
	time(&stopTime);
	printf("\n\nTotal execution time: %f\n",fabs(difftime(startTime,stopTime)));
	//MY_PRINTF("\n\nTotal execution time: %f\n",((double)(clock()-startTime)) / CLOCKS_PER_SEC);

	
	return(0);

}
