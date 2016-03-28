############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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
# Descripton:   Makefile script to build standalone SNAP Audio mp3 player.
#
#############################################################################

.IMPORT .IGNORE : ISV_LICENSE

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
EXELIBS         += $(LL)pm_test$(LE)
.ENDIF

.IF $(ISV_LICENSE)
CFLAGS		+= -DISV_LICENSE
.ENDIF

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

AAMP3_OBJS	= bitstrm$O decoder$O huffman$O hufftab$O \
                  synthtab$O maindec$O

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

aamp3$E: 	$(AAMP3_OBJS) mmain$O cmdline$O

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(AAMP3_OBJS)
DEPEND_SRC   	= $(PRIVATE)/src/audio

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
