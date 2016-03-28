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
# Descripton:   Makefile script to build all standalone SNAP Audio test
#               program.
#
#############################################################################

.IMPORT .IGNORE : ISV_LICENSE TEST_HARNESS

#----------------------------------------------------------------------------
# DOS extender dependant flags
#----------------------------------------------------------------------------

CFLAGS      += $(DX_CFLAGS)
ASFLAGS     += $(DX_ASFLAGS) -I$(PRIVATE)\include\snap

# Indicate that this program uses SNAP device drivers (so needs I/O access)
NEEDS_IO	:= 1

.IF $(TEST_HARNESS)
CFLAGS		+= -DTEST_HARNESS
NO_PMLIB        := 1
EXELIBS         += $(LL)pm_test$(LE) $(LL)n_audio$(LE)
.ENDIF

.IF $(ISV_LICENSE)
CFLAGS		+= -DISV_LICENSE
.ENDIF

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

AATEST_OBJS	= main$O cmdline$O wave$O

EXELIBS     += $(LL)console$(LE)

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

aatest$E: 	$(AATEST_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(AATEST_OBJS)
DEPEND_SRC   	= $(PRIVATE)/src/audio

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
