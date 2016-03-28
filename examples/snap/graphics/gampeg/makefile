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
# Descripton:   Makefile script to build SNAP command line MPEG player
#
#############################################################################

.IMPORT .IGNORE : ISV_LICENSE TEST_HARNESS PLAY_UNDER_X

#----------------------------------------------------------------------------
# Set compiler flags
#----------------------------------------------------------------------------

.IF $(ISV_LICENSE)
CFLAGS      += -DISV_LICENSE
.ENDIF
.IF $(PLAY_UNDER_X)
CFLAGS      += -DPLAY_UNDER_X
.ENDIF

#----------------------------------------------------------------------------
# Enable SNAP source debugging when building a test harness without shared libs
#----------------------------------------------------------------------------

.IF $(TEST_HARNESS)
CFLAGS          += -DTEST_HARNESS
SHOW_CFLAGS     += -DTEST_HARNESS
PRO             := 1
NO_PMLIB        := 1
EXELIBS         += $(LL)n_ga$(LE) $(LL)gconsole$(LE) $(LL)n_driver$(LE) $(LL)n_ref2d$(LE) $(LL)pm_test$(LE)
.ELSE
EXELIBS         += $(LL)n_ga$(LE) $(LL)gconsole$(LE)
.ENDIF

#----------------------------------------------------------------------------
# Indicate that this program uses SNAP device drivers (so needs I/O access)
#----------------------------------------------------------------------------

NEEDS_IO    := 1

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

OBJECTS     = mpeg2dec$O getpic$O motion$O getvlc$O gethdr$O getblk$O \
              getbits$O idct$O display$O

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

gampeg$E:   $(OBJECTS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

