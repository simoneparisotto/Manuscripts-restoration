

//definitions for certain basic image operations

#ifndef IMAGE_OPERATIONS_H
#define IMAGE_OPERATIONS_H

#include <stdlib.h>

#include "image_structures.h"
#include "io_png.h"
#include "convolution.h"
#include "morpho.h"

//random seed
void seed_random_numbers( double inputSeed=0.0);

//visualisation tool
nTupleImage *make_colour_wheel();

//reading and writing functions
float * read_image(const char *fileIn, size_t *nx, size_t *ny, size_t *nc);
void write_image(nTupleImage *imgIn, const char *fileName, imageDataType normalisationScalar=0);
void write_image_pyramid(nTupleImagePyramid imgInPyramid, int nLevels, const char *fileName, imageDataType normalisationScalar=0);
void write_shift_map(nTupleImage *shiftMap, const char *fileName);

nTupleImage * sub_sample_image(nTupleImage *imgIn, float subSampleFactor);
nTupleImage * up_sample_image(nTupleImage *imgIn, float upSampleFactor, nTupleImage *imgFine=NULL);

nTupleImage * rgb_to_grey(nTupleImage * imgIn);

nTupleImage * image_gradient_x(nTupleImage * imgIn);
nTupleImage * image_gradient_y(nTupleImage * imgIn);

nTupleImagePyramid create_nTupleImage_pyramid_binary(nTupleImage * imgIn, int nLevels);
nTupleImagePyramid create_nTupleImage_pyramid(nTupleImage * imgIn, int nLevels);

featurePyramid create_feature_pyramid(nTupleImage * imgIn, nTupleImage * occVol, int nLevels);
void delete_feature_pyramid(featurePyramid featurePyramidIn);

int determine_multiscale_level_number(nTupleImage *occIn, int patchSizeX, int patchSizeY);

#endif
