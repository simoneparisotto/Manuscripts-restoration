
Total Variation Inpainting using Split Bregman
Pascal Getreuer, pascal.getreuer@yale.edu, Yale University
Version 20120701 (July 1, 2012)

== Overview ==

This C source code accompanies with Image Processing On Line (IPOL) article
"Total Variation Inpainting using Split Bregman" at 

    http://www.ipol.im/pub/algo/g_tv_inpainting/

This code is used by the online IPOL demo:

    http://www.ipol.im/pub/demo/g_tv_inpainting/

Future software releases and updates will be posted at 

    http://dev.ipol.im/~getreuer/code/


== License (BSD) ==

All files are distributed according to the simplified BSD license.  You 
should have received a copy of this license along this program.  If not, see 
<http://www.opensource.org/licenses/bsd-license.html>.


== Program Usage ==

This source code produces a command line program tvinpaint, which performs 
total variation regularized image inpainting.

Usage: tvinpaint <D> <lambda> <input> <inpainted>

where <D>, <input>, and <inpainted> are BMP images (JPEG, PNG, or TIFF files
can also be used if the program is compiled with libjpeg, libpng, and/or 
libtiff).  The argument <lambda> is a positive scalar specifying the fidelity
weight.

Example:

    # Generate an inpainting domain D.bmp from random text
    ./randmask text mountain.bmp D.bmp
    
    # Set the image to gray within the inpainting domain to create masked.bmp
    ./applymask mountain.bmp D.bmp masked.bmp

    # Inpaint masked.bmp using the inpainting domain D.bmp with lambda = 100.
    # The result is saved to inpainted.bmp.
    ./tvinpaint D.bmp 100 masked.bmp inpainted.bmp

To avoid changing the image outside of the inpainting domain, set 
lambda to a very large value, e.g.,

    # Inpainting with lambda = 10^4, image does not change outside of D.
    ./tvinpaint D.bmp 1e4 masked.bmp inpainted.bmp


== Compiling ==

Instructions are included below for compiling on Linux sytems with GCC, on
Windows with MinGW+MSYS, and on Windows with MSVC.


== Compiling (Linux) ==

To compile this software under Linux, first install the development files for
libjpeg, libpng, and libtiff.  On Ubuntu and other Debian-based systems, enter
the following into a terminal:
    sudo apt-get install build-essential libjpeg8-dev libpng-dev libtiff-dev
On Redhat, Fedora, and CentOS, use
    sudo yum install make gcc libjpeg-turbo-devel libpng-devel libtiff-devel

Then to compile the software, use make with makefile.gcc:

    tar -xf tvinpaint_20120701.tar.gz
    cd tvinpaint_20120701
    make -f makefile.gcc

This should produce three executables, tvinpaint, randmask, and applymask.

Source documentation can be generated with Doxygen (www.doxygen.org).

    make -f makefile.gcc srcdoc


== Compiling (Windows with MinGW+MSYS) ==

The MinGW+MSYS is a convenient toolchain for Linux-like development under 
Windows.  MinGW and MSYS can be obtained from

    http://downloads.sourceforge.net/mingw/


--- Building with BMP only ---

The simplest way to build the tvinpaint programs is with support for only BMP
images. In this case, no external libraries are required.  Edit makefile.gcc 
and comment the LDLIB lines to disable use of libjpeg, libpng, and libtiff:

    #LDLIBJPEG=-ljpeg
    #LDLIBPNG=-lpng -lz
    #LDLIBTIFF=-ltiff

Then open an MSYS terminal and compile the program with 

    make CC=gcc -f makefile.gcc

This should produce the tvinpaint executable.


--- Building with PNG, JPEG, and/or TIFF support ---

To use the tvinpaint program with PNG, JPEG, and/or TIFF images, the 
following libraries are needed.

    For PNG:    libpng and zlib
    For JPEG:   libjpeg 
    For TIFF:   libtiff

These libraries can be obtained at 
    
    http://www.libpng.org/pub/png/libpng.html
    http://www.zlib.net/
    http://www.ijg.org/
    http://www.remotesensing.org/libtiff/

It is not necessary to include support for all of these libraries, for 
example, you may choose to support only PNG by building zlib and libpng 
and commenting the LDLIBJPEG and LDLIBTIF lines in makefile.gcc.

Instructions for how to build the libraries with MinGW+MSYS are provided at

    http://permalink.gmane.org/gmane.comp.graphics.panotools.devel/103
    http://www.gaia-gis.it/spatialite-2.4.0/mingw_how_to.html

Once the libraries are installed, build the tvinpaint programs with the 
makefile.gcc included in this archive.

    make CC=gcc -f makefile.gcc

This should produce executables tvinpaint, randmask, and applymask.


== Compiling (Windows with MSVC) ==

The express version of the Microsoft Visual C++ (MSVC) compiler can be 
obtained for free at

    http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express


--- Building with BMP only ---

For simplicity, the makefile will build the programs with only BMP image 
support by default.  Open a Visual Studio Command Prompt (under Start Menu > 
Programs > Microsoft Visual Studio > Visual Studio Tools > Visual Studio 
Command Prompt), navigate to the folder containing the sources, and enter

    nmake -f makefile.vc all

This should produce execuatables tvinpaint.exe, randmask.exe, and 
applymask.exe.


--- Building with PNG and/or JPEG support ---

To include support for PNG and/or JPEG images, the libpng and libjpeg 
libraries are needed.  Edit the LIB lines at the top of makefile.vc to
tell where each library is installed, e.g.,

    LIBJPEG_DIR     = "C:/libs/jpeg-8b"
    LIBJPEG_INCLUDE = -I$(LIBJPEG_DIR)
    LIBJPEG_LIB     = $(LIBJPEG_DIR)/libjpeg.lib

Then compile using

    nmake -f makefile.vc all


== Code Overview ==

An overview description of the C source code is included in the file 
code_overview.txt.  Detailed documentation of the source code is available
online at

  http://www.ipol.im/pub/algo/g_tv_inpainting/doc/index.html

Altenatively, the documentation can be generated using Doxygen with the
command "make -f makefile.gcc srcdoc".


== Acknowledgements ==

This material is based upon work supported by the National Science 
Foundation under Award No. DMS-1004694.  Any opinions, findings, and 
conclusions or recommendations expressed in this material are those of 
the author(s) and do not necessarily reflect the views of the National
Science Foundation.
