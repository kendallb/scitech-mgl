#----------------------------------------------------------------------
# Makefile for SVGAlib GL routines.
#----------------------------------------------------------------------

# *** NO SERVICIBLE PARTS HERE!
#     All options are in Makefile.cfg.

include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/gl

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

MODULES	 = grlib.o driver.o line.o palette.o scale.o text.o font8x8.o \
	   cbitmap.o mem.o 

all:	libvgagl.a
.PHONY: all clean dep

libvgagl.so.$(VERSION): $(MODULES)
	$(CC) -s -shared -Wl,-soname,libvgagl.so.$(MAJOR_VER) -o libvgagl.so.$(VERSION) \
	  $(MODULES)

libvgagl.a: $(MODULES)
	rm -f libvgagl.a
	$(AR) rcs libvgagl.a $(MODULES)

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

.S.s:
	$(CC) $(CFLAGS) -E $< >$@

.s.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

.c.s:
	$(CC) $(CFLAGS) -S -o $*.s $<

.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $* $*.o $(LIBS)
	chmod 4755 $*

$(MODULES): .depend.gl

dep:
	rm -f .depend.gl
	make depend

.depend.gl:
	echo '# GL Module dependencies' >>.depend.gl
	$(CC) $(INCLUDES) -MM $(patsubst %.o,$(srcdir)/gl/%.c,$(MODULES)) >>.depend.gl

clean:
	rm -f .depend.gl *.o *~ libvgagl.a libvgagl.so.$(VERSION)

#
# include a dependency file if one exists
#
ifeq (.depend.gl,$(wildcard .depend.gl))
include .depend.gl
endif
