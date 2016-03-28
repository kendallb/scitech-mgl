#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2.0 as published by the Free
#   Software Foundation and appearing in the file LICENSE.GPL included
#   in the packaging of this file.
#
#   Licensees holding a valid Commercial License for this product from
#   SciTech Software, Inc. may use this file in accordance with the
#   Commercial License Agreement provided with the Software.
#
#   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
#   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   See http://www.scitechsoft.com/license/ for information about
#   the licensing options available and how to purchase a Commercial
#   License Agreement.
#
#   Contact license@scitechsoft.com if any conditions of this licensing
#   are not clear to you, or you have questions about licensing options.
#
#  ========================================================================
#
# Descripton:   Generic makefile for the MegaGraph Graphics Library.
#               Requires the SciTech Software makefile definitions package
#               to be installed.
#
#               Builds the appropriate version depending on what target
#               system is selected.
#
#############################################################################

# DOS Extender dependant flags

CFLAGS          += $(DX_CFLAGS)
ASFLAGS         += $(DX_ASFLAGS) -I..$(SL)libjpeg -I..$(SL)libpng -I..$(SL)freetype

LIBCLEAN        = mgl*.lib mgl*.a smgl*.lib mgl*.dll

# Import variables from environment

.IMPORT .IGNORE: NO_ASM BUILD_ASSEMBLER ELFOBJ SAMPLE TEST_HARNESS GM_SAMPLE
.IMPORT .IGNORE: DEBUG_MGL DEBUG_JPEG DEBUG_PNG DEBUG_FREETYPE DEBUG_OPENGL

# Compile for No Assembler if requested

# No optimized assembly routines on PowerPC or AMD64
.IF $(USE_PPC_BE)
NO_ASM = 1
.ENDIF
.IF $(USE_X86_64)
NO_ASM = 1
.ENDIF

.IF $(NO_ASM)
CFLAGS          += -DNO_ASSEMBLER
.ENDIF

# List of all generic object files common to all versions

COBJ            = mgraph$O createdc$O devctx$O buffer$O state$O palette$O   \
                  glyph$O line$O bitblt$O putbmp$O                          \
                  clipline$O cliplfx$O scanline$O pixel$O rect$O mglfile$O  \
                  viewport$O access$O list$O cursor$O memset$O random$O     \
                  color$O mgldll$O halftone$O rtrav$O blocklst$O            \
                  ellipse$O ellipsea$O fatline$O polygon$O polyline$O       \
                  rectangl$O rspan$O rseg$O region$O rdraw$O runion$O       \
                  rsect$O rdiff$O rmisc$O rrect$O rline$O                   \
                  rcvxpoly$O rellip$O rellipa$O text$O wtext$O texthelp$O   \
                  vecfont$O bitfont$O font$O icon$O bitmap$O pcx$O jpeg$O   \
                  tiff$O png$O cplxpoly$O cnvxpoly$O polyhlp$O mouse$O      \
                  textenc$O fontenum$O winmng$O mgl_gl$O

DRVCOBJ         = packed8$O packed16$O packed24$O packed32$O

CPPOBJ          = mglrect$O mglrgn$O

.IF $(NO_ASM)
.ELSE
ASMOBJ          = _fix32$O _memset$O _polyhlp$O _random$O
DRVASMOBJ       =
.ENDIF

# Enable Nucleus source debugging when building a test harness

.IF $(TEST_HARNESS)
CFLAGS          += -DTEST_HARNESS
NO_PMLIB    := 1
DEBUG_MGL       := 1
EXELIBS         += $(LL)n_ref2d$(LE) $(LL)n_driver$(LE) $(LL)pm_test$(LE)
.IF $(DEBUG_MATROX)
EXELIBS         += $(LL)mgahal$(LE)
.ENDIF
.ELSE
DRVCOBJ         += galib$O
DRVASMOBJ       += _ga_imp$O
.ENDIF

# Compile in stubs to BPD libraries when not building debug libraries

.IF $(DEBUG_MGL)
EXELIBS         +=
.ELSE
BPDOBJ          +=
.ENDIF

.IF $(DEBUG_JPEG)
EXELIBS         += $(LL)jpeg$(LE)
.ELSE
BPDOBJ          += jpeglib$O _jpg_exp$O
.ENDIF

.IF $(DEBUG_PNG)

# When compiling as a static link library we need to compile in the default
# error handler that most applications expect. If we don't do this, the
# jmpbuf structure (which is compiler runtime library dependant in size)
# will be *different* and will cause the library to work incorrectly. This
# is bad design IMHO for the PNG library since the interface is not
# binary portable.

CFLAGS          += -DPNG_USE_DEFAULT_ERROR_HANDLER
EXELIBS         += $(LL)png$(LE)
.ELSE
BPDOBJ          += pnglib$O _png_exp$O
.ENDIF

.IF $(DEBUG_FREETYPE)
EXELIBS         += $(LL)freetype$(LE)
.ELSE
BPDOBJ          += ttlib$O _tt_exp$O
.ENDIF

.IF $(DEBUG_OPENGL)
CFLAGS          += -DDEBUG_OPENGL -DNO_GL_MACROS -DNO_GLU_MACROS -DNO_MGL_IMPORTS
EXELIBS         += $(LL)mesagl$(LE) $(LL)mesaglu$(LE)
.ELSE
BPDOBJ          += _opengl$O _glu$O
.ENDIF

# Include definitions for the target system

.IF $(USE_SMX32)
.INCLUDE: "mglsmx/mglsmx.mk"
.ELIF $(USE_WIN32)
.INCLUDE: "mglwin/mglwin.mk"
.ELIF $(USE_RTTARGET)
.INCLUDE: "mglrtt/mglrtt.mk"
.ELIF $(USE_OS232)
.IF $(USE_OS2GUI)
.INCLUDE: "mglpm/mglpm.mk"
.ELSE
.INCLUDE: "mglos2/mglos2.mk"
.ENDIF
.ELIF $(USE_QNX)
.INCLUDE: "mglqnx/mglqnx.mk"
.ELIF $(USE_LINUX)
.IF $(USE_X11)
.INCLUDE: "mglx/mglx11.mk"
.ELSE
.INCLUDE: "mgllinux/mgllinux.mk"
.ENDIF
.ELSE
.INCLUDE: "mgldos/mgldos.mk"
.ENDIF

# Targets to build sample programs and the necessary lib file

.IF $(SAMPLE)
.PHONY sample: $(SAMPLE)$E
.ENDIF

.IF $(GM_SAMPLE)
.PHONY gmsample: $(GM_SAMPLE)$E
.ENDIF

# Target to build a sample program for debugging

.IF $(SAMPLE)
SAMP_DEPEND_SRC = $(SCITECH)/examples/mgl/samples;$(SCITECH)/examples/mgl/hello
.IF $(USE_WIN32)
RCFLAGS         += -i$(SCITECH)/examples/mgl/samples
RCFILE          := mglsamp$R
.ENDIF
$(SAMPLE)$E: $(SAMPLE)$O mglsamp$O cmdline$O $(RCFILE) $(LIBFILE)
.ENDIF

.IF $(WMGLDOG)
SAMP_DEPEND_SRC = $(SCITECH)/examples/mgl/wmgldog
RCFLAGS += -i$(SCITECH)/examples/mgl/wmgldog
wmgldog$E: wmgldog$O wmgldog$R $(LIBFILE)
.ENDIF

.IF $(GM_SAMPLE)
SAMP_DEPEND_SRC = $(SCITECH)/examples/mgl/gm
EXELIBS         += $(LL)gm$(LE)
.IF $(USE_WIN32)
RCFLAGS         += -i$(SCITECH)/examples/mgl/gm
RCFILE          := demo$R
.ENDIF
$(GM_SAMPLE)$E: $(GM_SAMPLE)$O $(RCFILE) $(LIBFILE)
.ENDIF

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(COBJ) $(DRVCOBJ) $(ASMOBJ) $(DRVASMOBJ) $(CPPOBJ) $(LDEPEND_OBJ)
DEPEND_SRC_GEN  = emulate;drivers/packed;$(SCITECH)/src/libjpeg;$(SCITECH)/src/libpng;$(SCITECH)/src/zlib;$(SCITECH)/src/freetype;$(LDEPEND_SRC);$(SAMP_DEPEND_SRC)

.IF $(USE_X86)
DEPEND_SRC     += $(DEPEND_SRC_GEN);asm386
.ELIF $(USE_PPC_BE)
DEPEND_SRC     += $(DEPEND_SRC_GEN);asmppc
.ELIF $(USE_X86_64)
DEPEND_SRC     += $(DEPEND_SRC_GEN);asmamd64
.ELSE
Error - unsupported platform
.ENDIF

# Define the list of object files to build

OBJECTS         = $(COBJ) $(DRVCOBJ) $(LOCALCOBJ) $(LOCALDRVCOBJ) $(BPDOBJ)
.IF $(PCASM)
OBJECTS         += $(ASMOBJ) $(DRVASMOBJ) $(LOCALASMOBJ) $(LOCALDRVASMOBJ)
.ENDIF

# Target to build the library file

all: $(LIBFILE)

# Target to build the C++ library file

$(LIBFILECPP): $(CPPOBJ)

libcpp: $(LIBFILECPP)

installcpp: libcpp
    $(INSTALL) $(LIBFILECPP) $(LIB_DEST)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

