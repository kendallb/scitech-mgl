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

.IMPORT .IGNORE : ISV_LICENSE TEST_HARNESS

#----------------------------------------------------------------------------
# Set compiler flags
#----------------------------------------------------------------------------

.IF $(ISV_LICENSE)
CFLAGS      += -DISV_LICENSE
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
.IF $(DEBUG_MATROX)
EXELIBS         += $(LL)mgahal$(LE)
.ENDIF
.ELSE
EXELIBS         := $(LL)n_ga$(LE) $(LL)gconsole$(LE)
.ENDIF

#----------------------------------------------------------------------------
# LITE version compiles GATest as GACtrl for distribution with SNAP Graphics
#----------------------------------------------------------------------------

.IF $(LITE)
CFLAGS      += -DLITE
.ENDIF

#----------------------------------------------------------------------------
# PRO version compiles GATest with extra tests for testing as part of the DDK
#----------------------------------------------------------------------------

.IF $(PRO)
CFLAGS      += -DPRO
.ENDIF

#----------------------------------------------------------------------------
# Indicate that this program uses SNAP device drivers (so needs I/O access)
#----------------------------------------------------------------------------

NEEDS_IO    := 1

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

GATST_OBJS  = main$O cmdline$O util$O dpmstest$O ddctest$O ddccitst$O       \
              txttest$O tests$O center$O tv$O opengl$O bounce$O             \
              gears$O gears2$O moth$O mech$O video$O vidutil$O              \
              capture$O gabmp$O gapf$O cursors$O
.IF $(PRO)
GATST_OBJS  += 2dtests$O 2dconf$O
.ENDIF

#----------------------------------------------------------------------------
# Define the targets to build GATest
#----------------------------------------------------------------------------

gatest$E:   $(GATST_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(GATST_OBJS)
DEPEND_SRC      = opengl;$(PRIVATE)/src/snap/graphics/conform

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

