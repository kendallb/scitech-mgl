include ../Makefile.cfg

srcdir    = ..
VPATH     = $(srcdir)/doc

SVGALIB1=man7/svgalib.7.gz man5/libvga.config.5.gz man7/svgalib.chips.7.gz\
	man7/svgalib.et4000.7.gz man7/svgalib.mach32.7.gz\
	man6/mach32info.6.gz man3/vga_* man3/keyboard_* man3/mouse_* \
	man1/convfont.1.gz man1/dumpreg.1.gz man1/fix132x43.1.gz man1/restorefont.1.gz \
	man1/restorepalette.1.gz man1/restoretextmode.1.gz man1/runx.1.gz man1/savetextmode.1.gz \
	man1/setmclk.1.gz man1/textmode.1.gz\
	man6/eventtest.6.gz man6/forktest.6.gz man6/fun.6.gz man6/keytest.6.gz\
	man6/mousetest.6.gz man6/speedtest.6.gz man6/spin.6.gz\
	man6/testaccel.6.gz man6/threed.6.gz man6/vgatest.6.gz man6/accel.6.gz\
	man6/bg_test.6.gz man6/scrolltest.6.gz man6/testlinear.6.gz

VGAGL1=man7/vgagl.7.gz $(shell ls man3/gl_* | grep -v tri ) man6/testgl.6.gz

THREED1=man7/threedkit.7.gz man3/gl_*tri* man6/plane.6.gz man6/wrapdemo.6.gz

SVGALIB=$(subst man,$(srcdir)/doc/man,$(SVGALIB1))
VGAGL=$(subst man,$(srcdir)/doc/man,$(VGAGL1))
THREED=$(subst man,$(srcdir)/doc/man,$(THREED1))
MANPAGES=$(shell cd $(srcdir)/doc; find man? -type f -print)
UNCOMANPAGES=$(subst .gz, , $(MANPAGES))

ifndef MANFORMAT
ifeq ($(shell echo /usr/man/man?/*.gz), /usr/man/man?/*.gz)
	MANFORMAT=uncompressed
else
	MANFORMAT=compressed
endif
endif

.PHONY: clean ascii dvi ps install

../0-README: man7/svgalib.7.gz
	gunzip < man7/svgalib.7.gz | nroff -man | col -b > ../0-README

ascii:
	zcat $(SVGALIB) | grep -v '^\.so ' | nroff -man -T$@ \
		| col -b > svgalib.$@
	zcat $(VGAGL) | grep -v '^\.so ' | nroff -man -T$@ \
		| col -b > vgagl.$@
	zcat $(THREED) | grep -v '^\.so ' | nroff -man -T$@ \
		| col -b > 3d.$@

lpr:
	zcat $(SVGALIB) | grep -v '^\.so ' | nroff -man -Tascii > svgalib.lpr
	zcat $(VGAGL) | grep -v '^\.so ' | nroff -man -Tascii > vgagl.lpr
	zcat $(THREED) | grep -v '^\.so ' | nroff -man -Tascii > 3d.lpr

ps dvi:
	zcat $(SVGALIB) | grep -v '^\.so ' | troff -man -T$@ > svgalib.$@
	zcat $(VGAGL) | grep -v '^\.so ' | troff -man -T$@ > vgagl.$@
	zcat $(THREED) | grep -v '^\.so ' | troff -man -T$@ > 3d.$@

install:
	mkdir -p $(mandir)/man1 $(mandir)/man3 $(mandir)/man5\
		 $(mandir)/man6 $(mandir)/man7
ifeq ($(MANFORMAT),compressed)
	$(INSTALL_DATA) $(srcdir)/doc/man1/* $(mandir)/man1
	$(INSTALL_DATA) $(srcdir)/doc/man3/* $(mandir)/man3
	$(INSTALL_DATA) $(srcdir)/doc/man5/* $(mandir)/man5
	$(INSTALL_DATA) $(srcdir)/doc/man6/* $(mandir)/man6
	$(INSTALL_DATA) $(srcdir)/doc/man7/* $(mandir)/man7
else
	@cd $(srcdir)/doc; \
        for dir in man?; do\
		echo Installing manpages in $$dir;\
		for file in $$dir/*; do \
			bfile=`echo $$file | sed 's/.gz$$//'`; \
			gunzip < $$file > $(mandir)/$$bfile; \
			chmod 644 $(mandir)/$$bfile; \
			chown root.bin $(mandir)/$$bfile; \
		done; \
	done
endif
ifdef MAKEWHATIS
	@echo "Rebuilding the whatis database (this is slow!)"
	@echo "You can comment this out in Makefile.cfg!"
	$(MAKEWHATIS)
endif

uninstall:
	@echo Removing all svgalib man pages.
	@for dir in $(mandir) /usr/man /usr/local/man; do \
		cd $$dir; \
		rm -f $(MANPAGES) $(UNCOMANPAGES); \
	done;\

clean:
	-gzip -9 man?/* 2>/dev/null
	rm -f svgalib.ascii vgagl.ascii 3d.ascii
	rm -f svgalib.dvi vgagl.dvi 3d.dvi
	rm -f svgalib.ps vgagl.ps 3d.ps
	rm -f svgalib.lpr vgagl.lpr 3d.lpr
