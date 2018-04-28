#! /bin/bash
# Non-Local Patch-Based Image Inpainting with TV initialization
# applied to Manuscript Restoration TV inpainting
#
# Author: Simone Parisotto (sp751@cam.ac.uk)
# Date: 21/02/2018

# Software required (in the zip folder):
#
# Total Variation Inpainting using Split Bregman 
# Pascal Getreuer 
# https://doi.org/10.5201/ipol.2012.g-tvi
#
# Non-Local Patch-Based Image Inpainting, 
# Alasdair Newson, Andrés Almansa, Yann Gousseau, Patrick Pérez 
# https://doi.org/10.5201/ipol.2017.189

### SELECT EXPERIMENT
TS="101 106 201 203 204 110"

### INPAINTING PARAMETERS
## TOTAL VARIATION
LAMBDA=1000
MAXITER=5000
## PATCHMATCH
NITERS="12"
PS="5 7 9" # PATCHSIZE

## -------------------------------------- DO NOT TOUCH FROM HERE

# EXTRACT FILE TO BE MODIFIED FROM PACKAGES 
cd zip
tar -xf tvinpaint_20120701.tar.gz  -C ./../lib
tar -xf Inpainting_ipol_code.tar.gz -C ./../lib
#tar -xf tvinpaint_20120701.tar.gz $(tvinpaint_20120701/tvinpaint.c) -C ./../
#tar -xf Inpainting_ipol_code.tar.gz $(Inpainting_ipol_code/src/image_inpainting.cpp) -C ./../Inpainting_ipol_code/src
cd ..

# CUSTOMIZATION OF tvinpaint.c AND image_inpainting.cpp 
cd ./lib/tvinpaint_20120701
sed -i_bak "183s/.*/    TvRegSetMaxIter(Opt, $MAXITER);/" tvinpaint.c
make -f makefile.gcc
cd ../..

cd ./lib/Inpainting_ipol_code/
cd ./src
ROW24="            patchMatchParams->nIters = $NITERS;/"
ROW266="            initialise_inpainting(imgInpaint,occInpaint,featuresPyramid,shiftMap,patchMatchParams); imgInpaint = copy_image_nTuple(imgPyramid[level]);/"
ROW407="		nTupleImage *firstGuess = copy_image_nTuple(imgIn);/"
ROW266ORIGINAL="            initialise_inpainting(imgInpaint,occInpaint,featuresPyramid,shiftMap,patchMatchParams);/"
ROW407ORIGINAL="             nTupleImage *firstGuess = copy_image_nTuple(shiftMap);/"
sed -i_bak "24s/.*/$ROW24" image_inpainting.cpp 
sed -i_bak "266s/.*/$ROW266" image_inpainting.cpp 
sed -i_bak "407s/.*/$ROW407" image_inpainting.cpp 
cd ..
make
cd ../..
	
for T in $TS 
	do

	## FILENAMES
	localpath="./results/paper_results/test"$T"/"

	INPUT=$localpath"input_orig"$T".png"
	MASK=$localpath"mask"$T".png"

	MASKED=$localpath"masked"$T".png"
	TVINPAINTED=$localpath"TVinpainted"$T".png"

	### TV INITIALIZATION
	# Set the image to gray within the inpainting domain to create masked.bmp
	./lib/tvinpaint_20120701/applymask $INPUT $MASK $MASKED
	# Inpaint masked.png using the inpainting domain D.bmp with lambda = LAMBDA.
	./lib/tvinpaint_20120701/tvinpaint $MASK $LAMBDA $MASKED $TVINPAINTED

	# EXEMPLAR BASED INPAINTING
	for P in $PS
		do

		PATCHINPAINTED=$localpath"PATCHinpainted"$T"_"$P"x"$P".png"
		./lib/Inpainting_ipol_code/bin/inpaint_image $TVINPAINTED $MASK $PATCHINPAINTED -patchSizeX $P -patchSizeY $P

	done

done
