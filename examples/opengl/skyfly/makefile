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
# Descripton:   Makefile for the OpenGL demos.
#
#############################################################################

# Allow for compiling with GLUT library instead of Game Framework
.IMPORT .IGNORE: USE_GLUT USE_GLUT32

# Allow for stereo compilation
.IMPORT .IGNORE: USE_STEREO

.INCLUDE: "../common.mak"

.IF $(USE_GLUT)
CFLAGS          += -DNO_GL_MACROS -DNO_GLU_MACROS -DUSE_GLUT
.IF $(USE_WIN32)
CFLAGS          += -D_WIN32
.IF $(USE_GLUT32)
CFLAGS		+= /Gz
LDENDFLAGS      += /SUBSYSTEM:console
.ENDIF
.ENDIF
.ENDIF

.IF $(USE_STEREO)
CFLAGS          += -DSTEREO -DSTEREO_DEFAULT
.ENDIF

OBJECTS         = skyfly$O fly$O database$O perfdraw$O \
                  image$O random$O
.IF $(USE_GLUT)
OBJECTS         += glutmain$O
.ELSE
OBJECTS         += gm_main$O
.ENDIF

.IF $(USE_GLUT)
.IF $(USE_GLUT32)
EXELIBS         = $(LL)glut32$(LE) $(LL)opengl32$(LE) $(LL)glu32$(LE)
.ELSE
EXELIBS         = $(LL)glut$(LE) $(LL)opengl32$(LE) $(LL)glu32$(LE)
.ENDIF
.ELSE
EXELIBS         = $(LL)gm$(LE) $(BASELIBS)
.ENDIF

.IF $(WIN32_GUI)
RCFILE          = skyfly$R
.ELSE
RCFILE          =
.ENDIF

skyfly$E: $(OBJECTS) $(RCFILE)

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

