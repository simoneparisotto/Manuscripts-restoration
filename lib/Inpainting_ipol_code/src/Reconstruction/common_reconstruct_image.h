/*common includes necessary for the maximum meaningful code*/

#ifndef COMMON_COLOUR_ESTIMATION_H
#define COMMON_COLOUR_ESTIMATION_H

	#include <math.h>
	#include <float.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	//#include <windows.h>
    #include <vector>
    #include <utility>
    #include <numeric>
    #include <algorithm>
        
	#ifndef AGGREGATED_PATCHES
	#define AGGREGATED_PATCHES 0
	#endif
	
	#ifndef BEST_PATCH
	#define BEST_PATCH 3
	#endif
	
    #ifndef SIGMA_COLOUR
    #define SIGMA_COLOUR 75
    #endif


#endif
