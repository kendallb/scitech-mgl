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
# Descripton:   Specialised definitions for building the MGLX target
#               version of the MGL.
#
#############################################################################

# X11 specific includes

CFLAGS  += -I/usr/X11R6/include -D__X11__
# CFLAGS    += -DMGL_LITE
EXELIBS += -lX11 -lXext

# Name for the library file

LIBFILE    = $(LP)mglx11$(L)
LIBFILECPP = $(LP)mglcpp$(L)

# We use i386 asm on Intel machines (Linux)

# PCASM := 1

all: $(LIBFILE)

LOCALCOBJ   =   mglx11$O mouse$O event$O mglcpp$O

# List of all MGLX driver C & assembler object files

LOCALDRVCOBJ    = gunix$O xwindc$O pack1$O common$O \
          accel8$O accel16$O accel24$O accel32$O galib$O galinux$O \
          peloader$O libcimp$O # xdga$O

LOCALDRVASMOBJ  = _pack8$O _pack16$O _pack24$O _pack32$O

# Define the directories to search for local source files

LOCALSRC    = mglx drivers/unix drivers/common drivers/packed drivers/accel

DEPEND_OBJ  = $(LOCALCOBJ) $(LOCALDRVOBJ)
DEPEND_SRC  = mglx;drivers/unix;drivers/common

# Special target to build assembler files via pre-TASM'ed code from DOS

%$O: %$A ; +cp elfobj/$*.o $@
