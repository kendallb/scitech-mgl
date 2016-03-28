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
# Descripton:   Global makefile to build SciTech SNAP SDK and SciTech MGL
#               libraries. If MGL_ROOT is defined, the default behaviour
#               is to build the MGL libraries, otherwise the SciTech SNAP
#               SDK is built.
#
#############################################################################

# Determine if this is the build server or an engineer machine

.IMPORT .IGNORE: BUILDSERVER MGL_ROOT

# We don't want parallel builds of the primary makefile

MAXPROCESS  := 1

# Define directories for build

SOURCE      = $(SCITECH)$(SL)src$(SL)
EXAMPLES    = $(SCITECH)$(SL)examples$(SL)

# Common targets for all OS'es

T_PM        =   targets$(SL)pm
T_GCONSOLE  =   targets$(SL)gconsole
T_SNAP      =   targets$(SL)snap
T_TECH      =   targets$(SL)tech
T_MVIS      =   targets$(SL)mvis
T_FIXED     =   targets$(SL)fixed
T_GM        =   targets$(SL)gm
T_MGL       =   targets$(SL)mgl
T_GLUT      =   targets$(SL)glut

# Target to continue build of all libraries for current compiler

.PHONY single:
.IF $(MGL_ROOT)
    @$(ECHO) =============================================================================
    @$(ECHO) ** BUILDING: SciTech MGL libraries for current compiler
    @dmake all_mgl
.ELSE
    @$(ECHO) =============================================================================
    @$(ECHO) ** BUILDING: SciTech SNAP SDK libraries for current compiler
    @dmake all_sdk
.ENDIF

# Target to build all libraries for current compiler from scratch

.PHONY build: rmtargets single

# Target to remove all build targets to force libraries to rebuild

rmtargets:
    @$(RM) -f targets$(SL)*

# Target to build libraries for one compiler for SciTech SNAP SDK

all_sdk:    $(T_PM) $(T_GCONSOLE) $(T_SNAP)
    @$(ECHO) =============================================================================
    @$(ECHO) DONE: Single compiler SNAP SDK build completed successfully!

# Target to build libraries for one compiler for SciTech MGL Graphics Library

all_mgl:    $(T_PM) $(T_GCONSOLE) $(T_SNAP) $(T_TECH) $(T_MVIS) \
            $(T_GM) $(T_GLUT) $(T_MGL)
    @$(ECHO) =============================================================================
    @$(ECHO) DONE: Single compiler MGL build completed successfully!

# Targets to build each component

$(T_PM):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building PM Library
    @cdit $(SOURCE)pm dmake install -u
.IF $(USE_LINUX)
    @cdit $(SOURCE)pm dmake install BUILD_DLL=1 -u
.ENDIF
    @$(CP) done $(T_PM)

$(T_GCONSOLE):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building Graphical Console Library
    @cdit $(SOURCE)gconsole dmake install -u
    @$(CP) done $(T_GCONSOLE)

$(T_SNAP):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building SNAP Graphics Loader Library
    @cdit $(SOURCE)snap dmake install -u
    @$(CP) done $(T_SNAP)

$(T_TECH):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building Techniques Class Library
    @cdit $(SOURCE)tech dmake install -u
    @$(CP) done $(T_TECH)

$(T_MVIS):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building MegaVision GUI Library
    @cdit $(SOURCE)mvis dmake install -u
    @$(CP) done $(T_MVIS)

$(T_FIXED):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building Vector/Matrix Library
    @cdit $(SOURCE)fixed dmake install -u
    @$(CP) done $(T_FIXED)

$(T_GM):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building Game Framework Library
    @cdit $(SOURCE)gm dmake install -u
    @$(CP) done $(T_GM)

$(T_GLUT):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building MGL GLUT Library
    @cdit $(SOURCE)glut dmake install -u
    @$(CP) done $(T_GLUT)

$(T_MGL):
    @$(ECHO) =============================================================================
    @$(ECHO) START: Building Core MGL Library
    @cdit $(SOURCE)mgl dmake install -u
    @cdit $(SOURCE)mgl dmake installcpp -u
    @$(CP) done $(T_MGL)

