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

# Allow for linking with precompiled GLUT32 library instead of MGL GLUT library
.IMPORT .IGNORE: USE_GLUT32

# Allow for stereo compilations
.IMPORT .IGNORE: USE_STEREO

.INCLUDE: "../common.mak"

.IF $(USE_GLUT32)
CFLAGS          += -DNO_GL_MACROS -DNO_GLU_MACROS
CFLAGS		+= /Gz
LDENDFLAGS      += /SUBSYSTEM:console
.ENDIF

.IF $(USE_STEREO)
CFLAGS          += -DSTEREO -DSTEREO_DEFAULT
.ENDIF

# Names of all executable files built
.IF $(USE_STEREO)
EXEFILES        = steam$E gears$E morph2$E logo$E
.ELSE
EXEFILES        = glbounce$E isosurf$E morph3d$E wave$E gears2$E  \
                  moth$E rings$E steam$E fractals$E mech$E \
                  gears$E morph2$E logo$E
.ENDIF

.IF $(USE_GLUT32)
EXELIBS         = $(LL)glut32$(LE) $(LL)opengl32$(LE) $(LL)glu32$(LE)
.ELSE
EXELIBS         = $(BASELIBS)
.ENDIF

.IF $(WIN32_GUI)
RCFILE          = demo$R
.ELSE
RCFILE          =
.ENDIF

all: $(EXEFILES)

glbounce$E:  glbounce$O $(RCFILE)
isosurf$E:   isosurf$O $(RCFILE)
morph3d$E:   morph3d$O $(RCFILE)
wave$E:      wave$O $(RCFILE)
gears2$E:    gears2$O $(RCFILE)
moth$E:      moth$O $(RCFILE)
rings$E:     rings$O $(RCFILE)
steam$E:     steam$O $(RCFILE)
fractals$E:  fractals$O fviewer$O $(RCFILE)
mech$E:      mech$O $(RCFILE)
gears$E:     gears$O $(RCFILE)
morph2$E:    morph2$O $(RCFILE)
logo$E:      ogl_logo$O def_logo$O $(RCFILE)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

