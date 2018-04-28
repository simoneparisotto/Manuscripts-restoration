                      ━━━━━━━━━
                 README
                      ━━━━━━━━━


Table of Contents
─────────────────

1 About
2 Overview
3 Requirement
4 Compilation
5 Usage
.. 5.1 Inpaint_image
..... 5.1.1 Description
..... 5.1.2 Test command
6 Online Demo

1 About
═══════

  Author : Alasdair Newson, <alasdairnewson.work@gmail.com>, Université Paris Descartes, Frace
		Andres Almansa, <andres.almansa@telecom-paristech.fr>, France
		Yann Gousseau, <yann.gousseau@telecom-paristech.fr>, France
		Patrick Perez, <patrick.perez@technicolor.com>, France


  Copyright: (C) 2016 IPOL Image Processing On Line [http://www.ipol.im/]
  Licence : GPL V3+

  Copying and distribution of this file, with or without modification,
  are permitted in any medium without royalty provided the copyright
  notice and this notice are preserved.  This file is offered as-is,
  without any warranty.


2 Overview
══════════

  This source code provides an implementation of the non-local variational image inpainting algorithm of Newson et al.

	Video Inpainting of Complex Scenes
	Alasdair Newson, Andrés Almansa, Matthieu Fradet, Yann Gousseau, Patrick Pérez
	SIAM Journal of Imaging Science 2014 7:4, 1993-2019 

To install the software, you need to have a c++ compiler. The code has been tested with the gcc compiler.

The 'bin/inpaint_image' program reads an input PNG image to inpaint and a binary occlusion image. The output image is written to the root directory of the inpainting code.

3 Requirement
═════════════

  The code is written in UTF8 C++, and should compile on any system with
  an UTF8 C++ compiler. No external libraries are required.


4 Compilation
═════════════

  To compile the code, use the provided makefile, with the command 'make'. The
  makefile will produce a program called : 'bin/inpaint_image'.

5 Usage
═══════

5.1 Image inpainting
──────────

5.1.1 Description
╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌

  The 'inpaint_image' program can be called, for example, in the following manner :
  ┌────
  │ bin/inpaint_image input.png occlusion.png output.png 
  └────

  If you wish to specify parameters, then call as :

  ┌────
  │ bin/inpaint_image input.png occlusion.png output.png -patchSizeX 7 -patchSizeY 7 -nLevels 3 -useFeatures 1 -v 1
  └────

	PARAMETERS :
	
    -patchSizeX : patch column size (default : 7)
    -patchSizeY : patch row size (default : 7)
    -nLevels : number of pyramid levels (by default -1, which means that it
    is determined automatically by the algorithm)
    -useFeatures : use texture features, 0 = false, 1 = true (default, 1)
    -v : verbose, 0 = false, 1 = true (default, 0)

The main body of the inpainting code may be found in "image_inpainting.cpp".

5.1.2 Test command
╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌

You can test the code using the test image in the 'test' directory. Open a terminal and navigate to the root directory of the inpainting code. Then type the following command in the terminal :

./bin/inpaint_image test/barbara.png test/barbara_occlusion.png barbara_inpainted.png

This should execute without any problems. The result will be saved to the root directory.


6 Online demo
═════════════

The online demo for this code can be found at:

http://dev.ipol.im/~alasdair/ipol_demo/non_local_inpainting/

