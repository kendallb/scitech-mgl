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
# Descripton:   Generic makefile for the gm library. Requires the
#               SciTech Software makefile definitions package to be
#               installed.
#
#############################################################################

# DOS extender dependant flags

CFLAGS          += $(DX_CFLAGS)
ASFLAGS         += $(DX_ASFLAGS)

# Enable page flip debugging if desired

.IF $(DEBUG_FLIP)
CFLAGS          += -DDEBUG_FLIP
.ENDIF

# Name of library and object files required to build it

OBJECTS         = gm$O cmdline$O dlist$O sprbmp$O sprmgr$O sprite$O mglrgn$O

.IF $(BUILD_DLL)
DLLFILE         = mglgm$D
LIBFILE         = mglgm$L
EXELIBS         = mglfxi$L
.ELSE
.IF $(STKCALL)
LIBFILE         = $(LP)sgm$L
.ELSE
LIBFILE         = $(LP)gm$L
.ENDIF
.ENDIF
LIBCLEAN        = *.lib *.a

# Default target to build and install library

all:    install

# Target to build all DOS 32-bit libraries

.PHONY mkdos32:
    @build wc10-d32 install -u OPT=1 NO_PCH=1
    @build wc11-d32 install -u OPT=1 NO_PCH=1
    @build bc45-d32 install -u OPT=1
    @build bc50-d32 install -u OPT=1
    @build vc40-tnt install -u OPT=1
    @build vc50-tnt install -u OPT=1
    @build dj20-d32 install -u OPT=1

# Target to build all DOS libraries

.PHONY mkdos: mkdos32 cleanexe

# Target to build all win 32-bit libraries

.PHONY mkwin32:
    @build bc50-w32 BUILD_DLL=1 OPT=1
    @build wc10-w32 install -u OPT=1 NO_PCH=1
    @build wc11-w32 install -u OPT=1 NO_PCH=1
    @build bc45-w32 install -u OPT=1
    @build bc50-w32 install -u OPT=1
    @build vc40-w32 install -u OPT=1
    @build vc50-w32 install -u OPT=1

# Target to build all Windows libraries

.PHONY mkwin: mkwin32 cleanexe

# Target to build all libraries

.PHONY mkall: mkdos mkwin

# Target to build all libraries for SciTech Display Doctor

.PHONY mksdd:
    @build_db wc10-d32 install -u DBG=1
    @build    wc10-d32 install -u OPT=1
    @build_db wc11-d32 install -u DBG=1
    @build    wc11-d32 install -u OPT=1

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      := ../mgl

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
