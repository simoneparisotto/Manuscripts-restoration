# GCC makefile for TV inpainting IPOL demo
# Pascal Getreuer
# May 16, 2012

# The following three statements determine the build configuration.
# For handling different image formats, the program can be linked with
# the libjpeg, libpng, and libtiff libraries.  For each library, set
# the flags needed for linking.  To disable use of a library, comment
# its statement.  You can disable all three (BMP is always supported).
LDLIBJPEG=-ljpeg
LDLIBPNG=-lpng -lz
LDLIBTIFF=-ltiff

TVREG_FLAGS=-DTVREG_INPAINT -DNUM_SINGLE

##
# Standard make settings
CFLAGS=-O3 -ansi -pedantic -Wall -Wextra $(TVREG_FLAGS)
LDFLAGS=
LDLIB=-lm $(LDLIBJPEG) $(LDLIBPNG) $(LDLIBTIFF)

TVINPAINT_SOURCES=tvinpaint.c tvreg.c imageio.c basic.c
RANDMASK_SOURCES=randmask.c randmt.c drawtext.c imageio.c basic.c
APPLYMASK_SOURCES=applymask.c imageio.c basic.c

ARCHIVENAME=tvinpaint_$(shell date -u +%Y%m%d)
SOURCES=tvinpaint.c tvreg.c tvreg.h tvregopt.h dsolve_inc.c usolve_gs_inc.c \
num.h randmask.c randmt.c randmt.h drawtext.c drawtext.h applymask.c \
imageio.c imageio.h basic.c basic.h makefile.gcc makefile.vc readme.txt \
code_overview.txt BSD_simplified.txt GPLv3.txt doxygen.conf mountain.bmp \
example.sh

## 
# These statements add compiler flags to define USE_LIBJPEG, etc.,
# depending on which libraries have been specified above.
ifneq ($(LDLIBJPEG),)
	CJPEG=-DUSE_LIBJPEG
endif
ifneq ($(LDLIBPNG),)
	CPNG=-DUSE_LIBPNG
endif
ifneq ($(LDLIBTIFF),)
	CTIFF=-DUSE_LIBTIFF
endif

ALLCFLAGS=$(CFLAGS) $(CJPEG) $(CPNG) $(CTIFF)
TVINPAINT_OBJECTS=$(TVINPAINT_SOURCES:.c=.o)
RANDMASK_OBJECTS=$(RANDMASK_SOURCES:.c=.o)
APPLYMASK_OBJECTS=$(APPLYMASK_SOURCES:.c=.o)
.SUFFIXES: .c .o
.PHONY: all clean rebuild srcdoc dist dist-zip

all: tvinpaint randmask applymask

tvinpaint: $(TVINPAINT_OBJECTS)
	$(CC) $(LDFLAGS) $(TVINPAINT_OBJECTS) $(LDLIB) -o $@

randmask: $(RANDMASK_OBJECTS)
	$(CC) $(LDFLAGS) $(RANDMASK_OBJECTS) $(LDLIB) -o $@

applymask: $(APPLYMASK_OBJECTS)
	$(CC) $(LDFLAGS) $(APPLYMASK_OBJECTS) $(LDLIB) -o $@

.c.o:
	$(CC) -c $(ALLCFLAGS) $< -o $@

clean:
	$(RM) $(TVINPAINT_OBJECTS) $(RANDMASK_OBJECTS) $(APPLYMASK_OBJECTS) \
	tvinpaint randmask applymask

rebuild: clean all

srcdoc:
	doxygen doxygen.conf

dist: $(SOURCES)
	-rm -rf $(ARCHIVENAME)
	mkdir $(ARCHIVENAME)
	ln $(SOURCES) $(ARCHIVENAME)
	tar vchzf $(ARCHIVENAME).tar.gz $(ARCHIVENAME)
	-rm -rf $(ARCHIVENAME)

dist-zip: $(SOURCES)
	-rm -rf $(ARCHIVENAME)
	mkdir $(ARCHIVENAME)
	ln $(SOURCES) $(ARCHIVENAME)
	-rm -f $(ARCHIVENAME).zip
	zip -r9 $(ARCHIVENAME).zip $(ARCHIVENAME)/*
	-rm -rf $(ARCHIVENAME)
