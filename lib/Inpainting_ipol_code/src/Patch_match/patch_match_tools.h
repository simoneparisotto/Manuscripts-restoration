//this function defines the tools necessary for the spatio-temporal patch match algorithm

#ifndef PATCH_MATCH_TOOLS
#define PATCH_MATCH_TOOLS

	#include "common_patch_match.h"
    #include "patch_match_measure.h"
    
    bool check_max_shift_distance(int xShift, int yShift, const patchMatchParameterStruct *params);

    int check_is_occluded( nTupleImage *imgOcc, int x, int y);
    
    void calclulate_patch_distances(nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *shiftMap, nTupleImage *occImg,
		const patchMatchParameterStruct *params);

    bool check_already_used_patch( nTupleImage *shiftMap, int x, int y, int dispX, int dispY);

    int check_in_boundaries( nTupleImage *imgImg, int x, int y, const patchMatchParameterStruct *params);

    int check_in_inner_boundaries( nTupleImage *imgImg, int x, int y, const patchMatchParameterStruct *params);
    
    //full search
    void patch_match_full_search(nTupleImage *shiftMap, nTupleImage *imgA, nTupleImage *imgB,
            nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params);
    
    //shift volume initialisation
    void initialise_displacement_field(nTupleImage *shiftMap, nTupleImage *departImage,
        nTupleImage *arrivalImage, nTupleImage *firstGuess, nTupleImage *occIn, const patchMatchParameterStruct *params);
	
	/*******************************/
	/*** PATCH LEVEL INTERLEAVING **/
	/*******************************/
	void patch_match_one_iteration_patch_level(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage,
        nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params, int iterationNb);
	
	//random search and propagation interleaving at patch levels
	//Random search
	void patch_match_random_search_patch_level(nTupleImage *shiftMap, nTupleImage *imgA, nTupleImage *imgB,
        nTupleImage *occIn, nTupleImage *modImg, const patchMatchParameterStruct *params, int i, int j,
        nTupleImage *wValues);
    //propagation functions
    void patch_match_propagation_patch_level(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage,
            nTupleImage *occIn,
		const patchMatchParameterStruct *params, int iterationNb, int i, int j);

	/*******************************/
	/******* UTILITY FUNCTIONS *****/
	/*******************************/

    float calclulate_patch_error(nTupleImage *departImage, nTupleImage *arrivalImage, nTupleImage *shiftMap, nTupleImage *occIn,
		int xA, int yA, float minError, const patchMatchParameterStruct *params);

	float get_min_correct_error(nTupleImage *shiftMap, nTupleImage *departImage,nTupleImage *arrivalImage, nTupleImage *occIn,
							int x, int y, int beforeOrAfter, int *correctInd, float *minVector, float minError,
                            const patchMatchParameterStruct *params);

	float ssd_minimum_value(nTupleImage *imgA, nTupleImage *imgB, nTupleImage *occIn, int xA, int yA,
						int xB, int yB, float minVal, const patchMatchParameterStruct *params);

    //utility functions
	int check_disp_field(nTupleImage *shiftMap, nTupleImage *departImage, nTupleImage *arrivalImage,
            nTupleImage *occIn, const patchMatchParameterStruct *params);
#endif
