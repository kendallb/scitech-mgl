#----------------------------------------------------------------------
# Makefile for the main svgalib source.
#----------------------------------------------------------------------

# *** NO SERVICIBLE PARTS HERE!
#     All options are in Makefile.cfg.

include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/src

#----------------------------------------------------------------------
# Compiler Section (overrides Makefile.cfg)
#----------------------------------------------------------------------

INCLUDES += -I$(srcdir)/src

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

DRIVERS = 
MODULES = vga.o vgamodesel.o config.o
MOUSE   = mouse/mouse.o
KEYBOARD = keyboard/keyboard.o
JOYSTICK = joystick/joystick.o

# DEBUG_ACCEL	= y
# DEBUG_CONF	= y

# defines for all files.
ifdef SVGALIB_CONFIG_FILE
  DEFINES += -DSVGALIB_CONFIG_FILE=\"$(SVGALIB_CONFIG_FILE)\"
endif
ifdef ALLOW_MOUSE_OVERRIDE
  DEFINES += -DALLOW_MOUSE_OVERRIDE
endif
ifdef DEBUG_ACCEL
  DEFINES += -DDEBUG_ACCEL
endif
ifdef DEBUG_KEYBOARD
  DEFINES += -DDEBUG_KEYBOARD
endif
ifdef BACKGROUND
  DEFINES += -DBACKGROUND=1
endif

# defines for vga.c only.
ifdef ROOT_VC_SHORTCUT
  VGA_DEFINES += -DROOT_VC_SHORTCUT
endif

ifdef DEBUG_CONF
  DEFINES += -DDEBUG_CONF
endif

# Don't forget to export the LIBC environment variable if building on a libc5 system.
ifdef LIBC
  SCITECH_LIB = $(SCITECH)/lib/release/linux/gcc/libc
else
  SCITECH_LIB = $(SCITECH)/lib/release/linux/gcc/glibc
endif
MELTOBJS = meltobjs.sh # In the bin-linux directory from the scitech sources
DRIVERS +=  scitech.o
INCLUDES += -I$(SCITECH)/include

ALLOBJS = $(MODULES) $(MOUSE) $(JOYSTICK) $(KEYBOARD) $(DRIVERS)

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<
.asm.o:
	$(NASM) -i$(SCITECH)/include $<

.c.s:
	$(CC) $(CFLAGS) -S -o $*.s $<

.PHONY:	all dep depadd clean install FORCE

all:	libvga.so.$(VERSION) libvga.a

# These rules are for ELF only.
libvga.so.$(VERSION): $(ALLOBJS)
	$(CC) -shared -Wl,-soname,libvga.so.$(MAJOR_VER) \
	  -o libvga.so.$(VERSION) $(ALLOBJS) -lm

$(sharedlibdir)/libvga.so.$(VERSION): libvga.so.$(VERSION)
	$(INSTALL_PROGRAM) $< $(sharedlibdir)/$<
	ldconfig

install: $(sharedlibdir)/libvga.so.$(VERSION)

libvga.a: $(ALLOBJS)
	rm -f libvga.a
	$(AR) rcs libvga.a $(ALLOBJS)

libvgagl.a: FORCE
	make -f $(srcdir)/gl/Makefile $@ srcdir="$(srcdir)" DLLFLAGS="$(DLLFLAGS)"

vga.o: vga.c
	$(CC) $(CFLAGS) $(VGA_DEFINES) -c -o $@ $<

scitech.o: $(SCITECH_LIB)/libpm.a $(SCITECH_LIB)/libn_ga.a
	$(MELTOBJS) scitech.o $(SCITECH_LIB)/libpm.a $(SCITECH_LIB)/libn_ga.a

$(MODULES): .depend.src

dep:
	rm -f .depend.src
	make .depend.src

.depend.src:
	echo '# Module dependencies' >>.depend.src
	$(CC) $(INCLUDES) -MM $(patsubst %.o,$(srcdir)/src/%.c,$(MODULES)) >>.depend.src
	echo '# Mouse dependencies' >>.depend.src
	$(CC) $(INCLUDES) -MM $(patsubst %.o,$(srcdir)/src/%.c,$(MOUSE)) >.depend.src.mouse
	sed 's#^\(.*\.o\):#mouse/\1:#g' <.depend.src.mouse >>.depend.src
	rm -f .depend.src.mouse
	echo '# Joystick dependencies' >>.depend.src
	$(CC) $(INCLUDES) -MM $(patsubst %.o,$(srcdir)/src/%.c,$(JOYSTICK)) >.depend.src.joystick
	sed 's#^\(.*\.o\):#joystick/\1:#g' <.depend.src.joystick >>.depend.src
	rm -f .depend.src.joystick
	echo '# Keyboard dependencies' >>.depend.src
	$(CC) $(INCLUDES) -MM $(patsubst %.o,$(srcdir)/src/%.c,$(KEYBOARD)) >.depend.src.keyboard
	sed 's#^\(.*\.o\):#keyboard/\1:#g' <.depend.src.keyboard >>.depend.src
	rm -f .depend.src.keyboard

clean:
	rm -f .depend.src *~ *.o mouse/*.o joystick/*.o keyboard/*.o mouse/*~ keyboard/*~ joystick/*~ \
		libvga.a libvga.so.$(VERSION)

FORCE: # dummy target.

#
# include a dependency file if one exists
#
ifeq (.depend.src,$(wildcard .depend.src))
include .depend.src
endif
