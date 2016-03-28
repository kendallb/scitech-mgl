#----------------------------------------------------------------------
# Makefile for SVGAlib demo programs.
#
# This file is a part of SVGAlib.
#----------------------------------------------------------------------

include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/demos

#----------------------------------------------------------------------
# Compiler Section (overrides Makefile.cfg)
#----------------------------------------------------------------------

CFLAGS = $(WARN) $(OPTIMIZE) -I$(srcdir)/include -I$(srcdir)/gl $(DEBUG)

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

PROGS    = fun testgl speedtest mousetest vgatest scrolltest testlinear \
	   keytest testaccel accel forktest eventtest spin bg_test printftest \
	   joytest mjoytest bankspeed

# Determine what library (static or shared) we will be linking programs with
ifdef INSTALLSHAREDLIB
  LIBS = -L../src -L../gl -lvgagl -lvga  -lm
endif
ifndef LIBS
  LIBS    = ../staticlib/libvgagl.a ../staticlib/libvga.a -lm
  LVGADEP = $(LIBS)
endif

all:	$(PROGS)

.PHONY: all clean cleanbin dep

$(PROGS): $(LVGADEP)

.c:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $* $*.c $(LIBS)
	chown 0 $*
	chmod 4755 $*

testaccel: testaccel.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o testaccel testaccel.c $(LIBS) -lm
	chown 0 testaccel
	chmod 4755 testaccel

accel: accel.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o accel accel.c $(LIBS) -lm
	chown 0 accel
	chmod 4755 accel

clean:	cleanbin
	rm -f .depend *.o *~

cleanbin:
	rm -f $(PROGS)

#
# No dependencies required here.
#

dep:
.depend:

