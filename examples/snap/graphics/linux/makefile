############################################################################
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
# Descripton:   Makefile script to build the Linux textmode program.
#
#############################################################################

#----------------------------------------------------------------------------
# DOS extender dependant flags
#----------------------------------------------------------------------------

CFLAGS      += $(DX_CFLAGS)
ASFLAGS     += $(DX_ASFLAGS)

# Indicate that this program uses SNAP device drivers (so needs I/O access)
NEEDS_IO    := 1

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

OBJECTS     = textmode$O

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

textmode$E:     $(OBJECTS)

install: textmode$E
.IF $(LIBC)
    cp textmode$E $(SCITECH)/src/linux/libc5/textmode
.ELSE
    cp textmode$E $(SCITECH)/src/linux/glibc/textmode
.ENDIF
#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
