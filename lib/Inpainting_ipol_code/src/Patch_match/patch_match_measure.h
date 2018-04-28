
//this function declares the patch match measure with which we compare patches


#ifndef PATCH_MATCH_MEASURE_H
#define PATCH_MATCH_MEASURE_H

    #include "common_patch_match.h"

    float ssd_patch_measure(nTupleImage *imgA, nTupleImage *imgB,
    nTupleImage *occIn, int xA, int yA, int xB, int yB, float minVal, const patchMatchParameterStruct *params);

#endif
