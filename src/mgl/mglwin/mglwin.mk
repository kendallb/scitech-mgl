#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2002 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2 as published by the Free
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
# Descripton:   Specialised definitions for building the MGLWIN target
#               version of the MGL.
#
#############################################################################

# Define the name of the library file

.IF $(BUILD_DLL)
MGL_DLL         = 1
.ELIF $(IMPORT_DLL)
MGL_DLL         = 1
.ENDIF

# Build Standard Windows DLL (C calling conventions)

.IF $(MGL_DLL)
CFLAGS          += -DMGL_DLL    # Build the standard DLL
LIBFILE         = mgli$L
DLLFILE         = mgl$D

# Build standard library

.ELSE
.IF $(STKCALL)
LIBFILE         = $(LP)smgl$L
LIBFILECPP      = $(LP)smglcpp$L
.ELSE
LIBFILE         = $(LP)mgl$L
LIBFILECPP      = $(LP)mglcpp$L
.ENDIF
.ENDIF

# Target to build the library file

all: $(LIBFILE)

# The MGLWIN version uses PC based 386 assembler code

PCASM           = 1

# List of all MGLWIN C & assembler object files

LOCALCOBJ       = mglwin$O public$O
.IF $(SAMPLE)
.ELSE
LOCALCOBJ       += winmain$O
.ENDIF
LOCALASMOBJ     =

# List of all MGLWIN driver C & assembler object files

LOCALDRVCOBJ    = common$O vgapal$O gsnap$O snap$O snapvbe$O windd$O
LOCALDRVASMOBJ  =

# Create the local include file dependencies

LDEPEND_OBJ     = $(LOCALCOBJ) $(LOCALDRVCOBJ) $(LOCALASMOBJ) $(LOCALDRVASMOBJ)
LDEPEND_SRC     = mglwin;drivers/common;drivers/snap;drivers/windows

# Override the makefile.dep file location

MAKEFILE_DEP    := mglwin/mglwin.dep

