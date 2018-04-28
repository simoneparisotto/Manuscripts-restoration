//this function defines the image structures for use with patch match

#ifndef IMAGE_STRUCTURES_H
#define IMAGE_STRUCTURES_H

	#include <math.h>
	#include <float.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <sys/time.h>
    #include <ctime>
    #include <cstdlib> // C standard library
    #include <cstring>
    #include <climits>
    #include <string>
    #include <sstream>
    #include <fstream> // file I/O
    #include <iostream>
    #include <utility>
    #include <stdexcept>
    #include <vector>
    #include <queue>
    #include <bitset>
    #include <unistd.h>
	//#include <windows.h>
    //#include <omp.h>
        
    using std::runtime_error;

	#ifndef MY_PRINTF
	#define MY_PRINTF printf
	#endif
	
	#ifndef NCHANNELS
	#define NCHANNELS 3
	#endif
            
    /** PI */
	#ifndef M_PI
	#define M_PI   3.14159265358979323846
	#endif
            
    #ifndef BYTE_SIZE
	#define BYTE_SIZE 8
	#endif
    
    #ifndef mxPOINTER_CLASS
    #define mxPOINTER_CLASS mxUINT64_CLASS
    #endif
    
    #ifndef IMAGE_INDEXING
	#define IMAGE_INDEXING 0
	#endif
	
	#ifndef ROW_FIRST
	#define ROW_FIRST 0
	#endif
	
	#ifndef COLUMN_FIRST
	#define COLUMN_FIRST 1
	#endif
	
	#ifndef VERBOSE_MODE
	#define VERBOSE_MODE 0
	#endif
            
    #ifndef ASSERT
    #define ASSERT(cond) if (!(cond)) { std::stringstream sout; \
        sout << "Error (line " << __LINE__ << "): " << #cond; \
        throw runtime_error(sout.str()); }
    #endif

    #define GET_VALUE get_value_nTuple_volume
	#define NDIMS 2
	typedef struct coordinate
	{
		int x;
		int y;
	}coord;
    
    //various typedefs
    typedef std::pair<float,float> pairFloat;
    typedef std::pair<int,float> pairIntFloat;
    typedef std::vector<pairIntFloat> vectorPairIntFloat;
    
    typedef float imageDataType;

	class nTupleImage
	{
        private:
            imageDataType *values;
        
        public:
            int nTupleSize;
            int xSize;
            int ySize;
            int patchSizeX;
            int patchSizeY;
            int hPatchSizeX;
            int hPatchSizeY;
            int nElsTotal;

            int nX;
            int nY;
            int nC;

			int nDims;
                        
            int indexing;
            int destroyValues;
            
            nTupleImage(); //create an empty volume
            nTupleImage(nTupleImage *imgVolIn);
            nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn, int indexingIn);
            nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn, int patchSizeXIn, int patchSizeYIn, int indexingIn);
            nTupleImage(int xSizeIn, int ySizeIn, int nTupleSizeIn, int patchSizeXIn, int patchSizeYIn, int IndexingIn, imageDataType* valuesIn);
            ~nTupleImage();

            imageDataType get_value(int x, int y, int c);
            imageDataType* get_value_ptr(int x, int y, int c);
            imageDataType* get_data_ptr();
            void set_value(int x, int y, int c, imageDataType value);
            
            void set_all_image_values(imageDataType value);
            void add(imageDataType addScalar);
            void multiply(imageDataType multiplyFactor);
            imageDataType sum_nTupleImage();
            
            imageDataType max_value();
            imageDataType min_value();
            imageDataType mean_value();
            void absolute_value();
            void binarise();

            void display_attributes();
	};
	
    typedef struct paramPM
	{
		//patch sizes
        int patchSizeX;
        int patchSizeY;
		int nIters;	//number of propagation/random search steps in patchMatch
		int w;		//maximum search radius
		float alpha; //search radius shrinkage factor (0.5 in standard PatchMatch)
		float maxShiftDistance;		//maximum absolute search distance
        int partialComparison;		//indicate whether we only compare partial patches (in the case where some patches are partially occluded)
        int fullSearch;		//full (exhaustive) search instead of PatchMatch
        //texture attributes
        nTupleImage *normGradX;
        nTupleImage *normGradY;
        int verboseMode;
	}patchMatchParameterStruct;
	
	typedef nTupleImage** nTupleImagePyramid;
	
    typedef struct multiScaleFeaturePyramid
	{
        nTupleImagePyramid normGradX;
        nTupleImagePyramid normGradY;
        int nLevels;
	}featurePyramid;
    
float min_float(float a, float b);
float max_float(float a, float b);
int min_int(int a, int b);
int max_int(int a, int b);

char* int_to_string(int value);

float rand_float_range(float a, float b);
int rand_int_range(int a, int b);
float round_float(float a);

long getMilliSecs();

float pow_int(float a, int b);

void show_patch_match_parameters(patchMatchParameterStruct *patchMatchParams);
int check_patch_match_parameters(patchMatchParameterStruct *patchMatchParams);

int check_in_boundaries( nTupleImage *imgIn, int x, int y);
int check_in_inner_boundaries( nTupleImage *imgIn, int x, int y, const patchMatchParameterStruct *params);
void clamp_coordinates(nTupleImage* imgInA, int *x, int *y);

//copy nTupleValues from imgA(xA,yA) to imgB(xB,yB)
void copy_pixel_values_nTuple_image(nTupleImage *imgA, nTupleImage *imgB, int xA, int yA, int xB, int yB);
//copy nTupleValues from imgA(x1,y1) to imgB(x2,y2)
void copy_pixel_values_nTuple_image(nTupleImage *imgA, nTupleImage *imgB, int x1, int y1, int x2, int y2);

nTupleImage* copy_image_nTuple(nTupleImage *imgIn);

imageDataType calculate_residual(nTupleImage *imgIn, nTupleImage *imgInPrevious, nTupleImage *occIn);

#endif
