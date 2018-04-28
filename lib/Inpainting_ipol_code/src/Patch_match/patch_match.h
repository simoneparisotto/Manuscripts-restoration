//this is the include function for the spatio-temporal patch-match

#ifndef PATCH_MATCH_H
#define PATCH_MATCH_H


	#include "common_patch_match.h"
	#include "patch_match_tools.h"

	void patch_match_ANN(nTupleImage *imgA, nTupleImage *imgB, nTupleImage *shiftMap,
        nTupleImage *imgOcc, nTupleImage *imgMod, const patchMatchParameterStruct *params, nTupleImage *firstGuess=NULL);
        
#endif
