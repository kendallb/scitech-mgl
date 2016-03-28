#----------------------------------------------------------------------
# Makefile for SVGAlib utilities.
#
# This file is a part of SVGAlib.
#----------------------------------------------------------------------

include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/utils

#----------------------------------------------------------------------
# Compiler Section (overrides Makefile.cfg)
#----------------------------------------------------------------------

CFLAGS = -g $(WARN) $(OPTIMIZE) -I../include -I../src -I$(SCITECH)/include
#Use the next one for the Alpha/AXP if you need it
LDFLAGS = -L../staticlib 
LDFLAGS += -L../src
LIBS = -lvga -lm

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

UTILPROGS = restorefont convfont restoretextmode restorepalette dumpreg 

OBJECTS = restorefont.o convfont.o restoretextmode.o restorepalette.o \
	dumpreg.o

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $* $*.o $(LIBS)
	chmod 4755 $*

all: $(UTILPROGS)
.PHONY: all clean cleanbin dep

$(OBJECTS): .depend

convfont: convfont.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o convfont convfont.o $(LIBS)

clean:
	rm -f $(UTILPROGS) *.o *~

dep:
	rm -f .depend
	make .depend

install:
	$(INSTALLPROG) $(UTILPROGS) $(UTILINSTALLDIR)

.depend:
	gcc -MM -I$(SCITECH)/include $(patsubst %.o,$(srcdir)/utils/%.c,$(OBJECTS)) >.depend

#
# include a dependency file if one exists
#
ifeq (.depend.src,$(wildcard .depend.src))
include .depend.src
endif
