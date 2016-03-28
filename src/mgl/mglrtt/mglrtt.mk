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
# Descripton:   Specialised definitions for building the MGLRTT target
#               version of the MGL.
#
#############################################################################

# Indicate that we are compiling for RTTarget32

ASFLAGS         += -d__RTTARGET__

# Find the name of the library files

.IF $(STKCALL)
LIBFILE         = $(LP)smgl$L
LIBFILECPP      = $(LP)smglcpp$L
.ELSE
LIBFILE         = $(LP)mgl$L
LIBFILECPP      = $(LP)mglcpp$L
.ENDIF

# Target to build the library file

all: $(LIBFILE)

# The MGLRTT version uses PC based 386 assembler code

PCASM           = 1

# List of all MGLRTT C & assembler object files

LOCALCOBJ       = mglrtt$O
LOCALASMOBJ     =

# List of all MGLRTT driver C & assembler object files

LOCALDRVCOBJ    = common$O vgapal$O gsnap$O snap$O snapvbe$O gartt$O
LOCALDRVASMOBJ  =

# Create the local include file dependencies

LDEPEND_OBJ     = $(LOCALCOBJ) $(LOCALDRVCOBJ) $(LOCALASMOBJ) $(LOCALDRVASMOBJ)
LDEPEND_SRC     = mglrtt;drivers\common;drivers\snap

# Override the makefile.dep file location

MAKEFILE_DEP    := mglrtt\mglrtt.dep

