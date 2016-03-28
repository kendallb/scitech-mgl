#----------------------------------------------------------------------
# Makefile for plane and wrapdemo. See README file.
#
# This file is a part of SVGAlib.
#----------------------------------------------------------------------

include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/threeDKit

#----------------------------------------------------------------------
# Compiler Section (overrides Makefile.cfg)
#----------------------------------------------------------------------

LOCAL_DEFINES = -DSVGALIB
CFLAGS = $(WARN) $(OPTIMIZE) -I$(srcdir)/include -I$(srcdir)/gl -I. $(LOCAL_DEFINES)
DLLFLAGS  =

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

OBJECTS = 3dinit.o 3dkit.o wrapsurf.o quickmath.o triangle.o striangle.o \
	    wtriangle.o swtriangle.o

# Determine what library (static or shared) we will be linking programs with
ifdef INSTALLSHAREDLIB
  LIBS = -L../gl -L../src -lvgagl -lvga -lm
endif
ifndef LIBS
  LIBS    = -lm ../staticlib/libvgagl.a ../staticlib/libvga.a
  LVGADEP = ../staticlib/libvgagl.a ../staticlib/libvga.a
endif

.PHONY: clean

all: plane wrapdemo

plane: planukit.o planinit.o $(OBJECTS) $(LVGADEP)
	$(CC) $(LDFLAGS) -o plane planukit.o planinit.o $(OBJECTS) $(LIBS)
	chown root plane
	chmod u+s plane

wrapdemo: wrapdemo.o $(OBJECTS) $(LVGADEP)
	$(CC) $(CFLAGS) $(LDFLAGS) -o wrapdemo wrapdemo.c $(OBJECTS) $(LIBS)
	chown root wrapdemo
	chmod u+s wrapdemo

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

clean:
	rm -f *.o plane wrapdemo
