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
# Descripton:   Makefile script to build all standalone SNAP Graphics test
#               programs.
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

GTFTEST_OBJS    = gtftest$O cmdline$O

EXELIBS         = $(LL)n_ga$(LE) $(LL)gconsole$(LE)

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

all: gtftest$E

gtftest$E:  $(GTFTEST_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(GTFTEST_OBJS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
