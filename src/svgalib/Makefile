#---------------------------------------------------------------------OU
# Makefile for SVGAlib.
#
# It's pretty unreadable, but just doing make install should be
# enough. This will install the headerfiles and shared library first
# (which is enough to compile things), after which the static version is
# optionally compiled and installed (if it fails, the shared libraries
# should still work fine).
#
#----------------------------------------------------------------------

# *** NO SERVICIBLE PARTS HERE!
#     All options are in Makefile.cfg.

include Makefile.cfg

#----------------------------------------------------------------------
# Rules Section
#----------------------------------------------------------------------

# In case someone goes for the demos w/o installing svgalib make
# the static libs first.
ifdef INSTALLSHAREDLIB
  PREDEMO =
else
  PREDEMO = static
endif

# A flag if this is a distribution:
DISTRIBUTION = $(shell sh -c "echo sharedlib/DIST*")

UTILS   = restorefont runx restorepalette restoretextmode textmode \
      savetextmode dumpreg

OBSOLETEHDIRS = /usr/include/ /usr/include/vga/ /usr/local/include/ /usr/local/include/vga/
OBSOLETEBDIRS = /usr/bin/ /usr/local/bin/

OBSOLETEHEADERS = /inc/vga.h /inc/vgakeyboard.h /inc/vgamouse.h /inc/vgagl.h /inc/vgajoystick.h

OBSOLETELIBLINKS = /lib/libvga.so /lib/libvga.so.1 /lib/libvgagl.so /lib/libvgagl.so.1

OBSOLETESHAREDIMAGES = /lib/libvga.so.1.0 /lib/libvga.so.1.0.1 \
    /lib/libvga.so.1.0.2 /lib/libvga.so.1.0.3 /lib/libvga.so.1.0.4 \
    /lib/libvga.so.1.0.5 /lib/libvga.so.1.0.6 /lib/libvga.so.1.0.7 \
    /lib/libvga.so.1.0.8 /lib/libvga.so.1.0.9 /lib/libvga.so.1.0.10 \
    /lib/libvga.so.1.0.11 /lib/libvga.so.1.0.12 /lib/libvga.so.1.1 \
    /lib/libvga.so.1.1.1 /lib/libvga.so.1.1.2 /lib/libvga.so.1.1.3 \
    /lib/libvga.so.1.1.4 /lib/libvga.so.1.1.5 /lib/libvga.so.1.1.6 \
    /lib/libvga.so.1.1.7 /lib/libvga.so.1.1.8 /lib/libvga.so.1.1.9 \
    /lib/libvga.so.1.2.0 /lib/libvga.so.1.2.1 /lib/libvga.so.1.2.2 \
    /lib/libvga.so.1.2.3 /lib/libvga.so.1.2.4 /lib/libvga.so.1.2.5 \
    /lib/libvga.so.1.2.6 /lib/libvga.so.1.2.7 /lib/libvga.so.1.2.8 \
    /lib/libvga.so.1.2.9 /lib/libvga.so.1.2.9 /lib/libvga.so.1.2.10 \
    /lib/libvga.so.1.2.11 /lib/libvga.so.1.2.12 /lib/libvga.so.1.2.13 \
    /lib/libvga.so.1.2.14 /lib/libvga.so.1.3.0 /lib/libvga.so.1.3.1

# for ELF
OBSOLETESHAREDIMAGES += /lib/libvgagl.so.1.2.0 /lib/libvgagl.so.1.2.1 /lib/libvgagl.so.1.2.2 \
    /lib/libvgagl.so.1.2.3 /lib/libvgagl.so.1.2.4 /lib/libvgagl.so.1.2.5 \
    /lib/libvgagl.so.1.2.6 /lib/libvgagl.so.1.2.7 /lib/libvgagl.so.1.2.8 \
    /lib/libvgagl.so.1.2.9 /lib/libvgagl.so.1.2.10 /lib/libvgagl.so.1.2.11 \
    /lib/libvgagl.so.1.2.12 /lib/libvgagl.so.1.2.13 /lib/libvgagl.so.1.2.14 \
    /lib/libvgagl.so.1.3.0 /lib/libvgagl.so.1.3.1

    OBSOLETELDIRS = /lib/ /usr/lib/ /usr/local/lib/ /usr/share/lib/ \
            $(shell sed 's?\#.*$$??' /etc/ld.so.conf 2>/dev/null | \
              sed 's?\([^/]\)[  ]*$$?\1/ ?' | grep -v aout/ )

    SHAREDLIBS = sharedlib/libvga.so.$(VERSION) sharedlib/libvgagl.so.$(VERSION)
    SVGALIBSHAREDSTUBS =

BACKUP   = svgalib-nucleus-$(VERSION).tar.gz

default:
    @echo "To install SVGAlib, do one of the following:"
    @echo ""
    @echo " make clean      - clean every thing. Do this after every change"
    @echo "                   of Makefile.cfg! Esp. when changing from a.out"
    @echo "                   to ELF and vice versa"
    @echo " make install    - compile & install components specified in Makefile.cfg"
    @echo " make demoprogs  - make demo programs in demo/ and threeDKit/"
    @echo ""
    @echo " make uninstall  - remove an existing installation from various"
    @echo "                   common places. (old traces often confuse the"
    @echo "                   compiler even when svgalib is not installed anew)"
    @echo "                   (make install includes an uninstall first)"
    @echo ""
    @echo " Be sure to read the file 0-INSTALL!"
    @echo ""

.PHONY: default all install installheaders installconfig
.PHONY: clean distclean indent uninstall
.PHONY: force_remake remake_shared shared static
.PHONY: indent-gnu

installheaders:
    @echo Installing header files in $(includedir).
    @if [ -f /usr/include/vga.h ]; then \
        rm -f /usr/include/vga.h /usr/include/vgagl.h; \
        echo Old header files in /usr/include removed.; \
    fi
    @cp $(srcdir)/src/vga.h $(includedir)/vga.h
    @chmod a+r $(includedir)/vga.h
    @cp $(srcdir)/gl/vgagl.h $(includedir)/vgagl.h
    @chmod a+r $(includedir)/vgagl.h
    @cp $(srcdir)/src/mouse/vgamouse.h $(includedir)/vgamouse.h
    @chmod a+r $(includedir)/vgamouse.h
    @cp $(srcdir)/src/joystick/vgajoystick.h $(includedir)/vgajoystick.h
    @chmod a+r $(includedir)/vgajoystick.h
    @cp $(srcdir)/src/keyboard/vgakeyboard.h $(includedir)/vgakeyboard.h
    @chmod a+r $(includedir)/vgakeyboard.h

installsharedlib: $(SHAREDLIBS) $(SVGALIBSHAREDSTUBS)
    @if [ -f /usr/lib/libvga.sa ]; then \
        rm -f /usr/lib/libvga.sa /usr/lib/libvgagl.sa; \
        echo Old stubs in /usr/lib removed.; \
    fi
    @echo Installing shared library image as \
        $(addprefix $(sharedlibdir)/,$(notdir $(SHAREDLIBS))).
    @rm -f $(OBSOLETESHAREDIMAGES)
    @for foo in $(notdir $(SHAREDLIBS)); do \
        $(INSTALL_SHLIB) sharedlib/$$foo $(sharedlibdir)/$$foo; \
        (cd $(sharedlibdir); \
         ln -sf $$foo `echo $$foo | sed 's/\.so\..*/.so/'` ); \
    done
# Generate libc5 compatibility libraries
    @for foo in $(notdir $(SHAREDLIBS)); do \
        $(INSTALL_SHLIB) sharedlib/$$foo $(compatlibdir)/$$foo; \
        (cd $(compatlibdir); \
         ln -sf $$foo `echo $$foo | sed 's/\.so\..*/.so/'` ); \
    done
    @ldconfig

installstaticlib: static
    @echo Installing static libraries in $(libdir).
    @rm -f /usr/lib/libvga.a
    @$(INSTALL_DATA) staticlib/libvga.a $(libdir)/libvga.a
    @chmod a+r $(libdir)/libvga.a
    @rm -f /usr/lib/libvgagl.a
    @$(INSTALL_DATA) staticlib/libvgagl.a $(libdir)/libvgagl.a
    @chmod a+r $(libdir)/libvgagl.a

installutils: textutils
    @if [ ! -d $(bindir) ]; then \
        echo No $(bindir) directory, creating it.; \
        mkdir $(bindir); \
    fi
    @if [ -f /usr/bin/restorefont ]; then \
        echo Removing old utilities in /usr/bin.; \
        for x in $(UTILS); do rm -f /usr/bin/$$x; done; \
    fi
    @if [ -f /usr/bin/convfont ]; then \
        echo Removing inappropriate utilities in /usr/bin.; \
        rm -f /usr/bin/convfont /usr/bin/setmclk; \
    fi
    @echo Installing textmode utilities in $(bindir):
    @echo "restorefont:      Save/restore textmode font."
    @cp utils/restorefont $(bindir)
    @echo "restorepalette:   Set standard VGA palette."
    @cp utils/restorepalette $(bindir)
    @echo "dumpreg:          Write ASCII dump of SVGA registers."
    @cp utils/dumpreg $(bindir)
    @echo "restoretextmode:  Save/restore textmode registers."
    @cp utils/restoretextmode $(bindir)
    @echo "textmode:         Script that tries to restore textmode."
    @cp utils/textmode $(bindir)
    @echo "savetextmode:     Script that saves textmode information used by 'textmode'."
    @cp utils/savetextmode $(bindir)

installconfig:
    @if [ ! -d $(datadir) ]; then \
        echo Creating configuration directory $(datadir).; \
        mkdir $(datadir); \
        chmod go-w $(datadir); \
    fi
    @if [ \( -f /usr/local/lib/libvga.config -a ! -f $(datadir)/libvga.config \) ]; then \
        echo "Moving old config file /usr/local/lib/libvga.config to $(datadir)." ; \
        mv -f /usr/local/lib/libvga.config $(datadir)/libvga.config; \
    fi
    @if [ ! -f $(datadir)/libvga.config ]; then \
        echo Installing default configuration file in $(datadir).; \
        cp $(srcdir)/libvga.config $(datadir)/libvga.config; \
        cp $(srcdir)/libvga.config.tmpl $(datadir)/libvga.config.tmpl; \
    fi

installman:
    (cd doc; $(MAKE) -f $(srcdir)/doc/Makefile srcdir="$(srcdir)" install )

install: $(INSTALLSHAREDLIB) installconfig \
    $(INSTALLSTATICLIB) $(INSTALLUTILS) $(INSTALLMAN) installheaders
    @echo
    @echo
    @echo Now run "'make demoprogs'" to make the test and demo programs in
    @echo demos/ and threedkit/.

uninstall:
    @echo "Removing textmode utilities..."
    @for i in $(OBSOLETEBDIRS); do \
        rm -f `echo $(UTILS) | sed s?'\<'?$$i?g`; \
         done
    @echo "Removing shared library stubs (old & current)..."
    @for i in $(OBSOLETELDIRS); do \
        rm -f `echo /lib/libvga.so.$(VERSION) /lib/libvgagl.so.$(VERSION) \
            $(OBSOLETELIBLINKS) /lib/libvga.sa /lib/libvgagl.sa \
             | sed s?/lib/?$$i?g`; \
         done
ifndef KEEPSHAREDLIBS
    @echo "Removing shared library images (old & current)..."
    @for i in $(OBSOLETELDIRS); do \
        rm -f `echo $(OBSOLETESHAREDIMAGES) | sed s?/lib/?$$i?g`; \
         done
endif
    @echo "Removing static libraries..."
    @for i in $(OBSOLETELDIRS); do \
        rm -f `echo /lib/libvga.a /lib/libvgagl.a | sed s?/lib/?$$i?g`; \
         done
    @echo "Removing header files..."
    @for i in $(OBSOLETEHDIRS); do \
        rm -f `echo $(OBSOLETEHEADERS) | sed s?/inc/?$$i?g`; \
         done
    (cd doc; $(MAKE) -f $(srcdir)/doc/Makefile srcdir="$(srcdir)" uninstall)

SHAREDDIRS0 = sharedlib/mouse sharedlib/keyboard sharedlib/ramdac \
        sharedlib/clockchip sharedlib/joystick
SHAREDDIRS = $(SHAREDDIRS0)
STATICDIRS = staticlib/mouse staticlib/keyboard staticlib/ramdac \
        staticlib/clockchip staticlib/joystick
UTILDIRS = utils
DEMODIRS = demos threeDKit

$(SHAREDDIRS0) $(STATICDIRS) $(DEMODIRS):
    mkdir -p $@

utils:
    mkdir -p utils
    if [ ! -f utils/runx ]; then \
        cp $(srcdir)/utils/runx $(srcdir)/utils/savetextmode \
           $(srcdir)/utils/textmode utils; \
    fi

static: staticlib/libvga.a staticlib/libvgagl.a

.PHONY: staticlib/libvgagl.a staticlib/libvga.a

staticlib/libvgagl.a staticlib/libvga.a: $(STATICDIRS)
    (cd $(dir $@); \
     $(MAKE) -f $(srcdir)/src/Makefile $(notdir $@) \
        srcdir="$(srcdir)" DLLFLAGS="" \
    )

# ELF

.PHONY: sharedlib/libvga.so.$(VERSION) sharedlib/libvgagl.so.$(VERSION)

shared: $(SHAREDLIBS) $(SVGALIBSHAREDSTUBS)

sharedlib/libvga.so.$(VERSION): $(SHAREDDIRS)
    @rm -f sharedlib/DISTRIBUTION
    (cd $(dir $@); \
     $(MAKE) -f $(srcdir)/src/Makefile $(notdir $@) \
        srcdir="$(srcdir)" DLLFLAGS="$(DLLFLAGS)" \
    )


sharedlib/libvgagl.a sharedlib/libvgagl.so.$(VERSION): $(SHAREDDIRS)
    (cd $(dir $@); \
     $(MAKE) -f $(srcdir)/gl/Makefile $(notdir $@) \
        srcdir="$(srcdir)" DLLFLAGS="$(DLLFLAGS)" \
    )

demoprogs: $(PREDEMO) $(DEMODIRS)
    @for dir in $(DEMODIRS); do \
        if [ -d $(srcdir)/$$dir ]; then \
            (cd $$dir; \
            $(MAKE) -f $(srcdir)/$$dir/Makefile srcdir="$(srcdir)"); \
        fi; \
    done

textutils: $(UTILDIRS)
    (cd utils; \
    $(MAKE) -f $(srcdir)/utils/Makefile srcdir="$(srcdir)")

backup: clean
    (cd ..; \
    tar cvf - svgalib | gzip -9 > $(BACKUP) )
distclean:
    (cd $(srcdir)/doc; $(MAKE) clean)
    (cd $(srcdir)/doc; $(MAKE) ../0-README)
    (cd $(srcdir)/src; $(MAKE) clean)
    (cd $(srcdir)/gl; $(MAKE) clean)
    (cd $(srcdir)/utils; $(MAKE) clean)
    (cd $(srcdir)/demos; $(MAKE) clean)
    (cd $(srcdir)/threeDKit; $(MAKE) clean)
    find . \( -name '.depend*' -o -name '*~*' \) -exec rm {} \;
    rm -rf sharedlib/[!l]* sharedlib/l[!i]* sharedlib/li[!b]* staticlib
    rm -rf sharedlib/*.a
    mkdir -p sharedlib
    touch sharedlib/DISTRIBUTION

clean: distclean
    rm -rf sharedlib

indent:
    find demos gl src support -name '*.[ch]' -exec indent -kr {} \;
    indent -kr src/*.regs

indent-gnu:
    find demos gl src support -name '*.[ch]' -exec indent -gnu {} \;
    indent -gnu src/*.regs

FORCE:
