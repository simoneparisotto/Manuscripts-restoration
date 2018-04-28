

//decalarations for morphological operations

#ifndef MORPHO_H
#define MORPHO_H

#include "image_structures.h"

nTupleImage* create_structuring_element(const char * structType, int xSize, int ySize);

nTupleImage* imerode(nTupleImage* imgIn, nTupleImage* structEl);

nTupleImage* imdilate(nTupleImage* imgIn, nTupleImage* structEl);

#endif
