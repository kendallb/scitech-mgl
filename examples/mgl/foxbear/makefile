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
# Descripton:   Generic makefile for the ATI Fox & Bear demo ported to the
#               C++ and the MGL.
#
#############################################################################

.INCLUDE: "../common.mak"

# Set objects to be included in the make process
USE_CXX_LINKER	:= 1
OBJECTS         = foxbear$O fox$O bear$O apple$O fxsprite$O fxplane$O
DOSSND_OBJS     = err$O
SOUND_OBJS      = sound$O
WIN_OBJS        = foxbear$R
TOTAL_OBJS      = $(OBJECTS)
.IF $(STKCALL)
EXELIBS         = $(LL)sgm$(LE) $(LL)mglcpp$(LE) $(BASELIBS)
.ELSE
EXELIBS         = $(LL)gm$(LE) $(LL)mglcpp$(LE) $(BASELIBS)
.ENDIF

.IF $(USE_DWSTK)
CFLAGS          += -DUSE_DWSTK
TOTAL_OBJS      += $(SOUND_OBJS)
.ENDIF

.IF $(USE_WIN32)

# Building Windows version

.IF $(USE_DWSTK)
EXELIBS         += $(LL)dwsw32$(LE)
.ENDIF
TOTAL_OBJS      += $(WIN_OBJS)

.ELSE

# Building DOS version

.IF $(USE_DWSTK)
TOTAL_OBJS      += $(DOSSND_OBJS)
EXELIBS         += $(LL)dwsp$(LE)
.ENDIF

.ENDIF

foxbear$E: $(TOTAL_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS) $(WIN_OBJS) $(DOSSND_OBJS) $(SOUND_OBJS)

# Define the list of object files to create dependency information for

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

