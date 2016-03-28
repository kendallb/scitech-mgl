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

.INCLUDE: "../common.mak"

OBJECTS         = a$O b$O d$O e$O f$O h$O i$O m$O n$O o$O \
                  p$O r$O s$O t$O w$O d_holder$O d_lamp$O \
                  d_logo$O d_logol$O d_logos$O track$O
EXELIBS         = $(BASELIBS)

.IF $(WIN32_GUI)
OBJECTS         += ideas$R
.ENDIF

ideas$E: $(OBJECTS)

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

