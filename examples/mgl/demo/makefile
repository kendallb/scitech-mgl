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
# Descripton:   Generic makefile for the MGL demo program. Requires the
#               SciTech Software makefile definitions package to be
#               installed.
#
#               NOTE: The floodfill code will break unless a LARGE stack is
#                     used to link the program. Run make with the command
#                     make STKSIZE=50000 or some other large number to
#                     link with a large stack for compilers other than
#                     Borland C++.
#
#############################################################################

.INCLUDE: "../common.mak"

# Comment the following out if you dont want pre-compiled headers

CFLAGS          += -DPC_HEADERS

# Name of libraries needed in link phase

.IF $(STKCALL)
EXELIBS         = $(LL)smvis$(LE) $(LL)stech$(LE) $(LL)smglcpp$(LE) $(BASELIBS)
.ELSE
EXELIBS         = $(LL)mvis$(LE) $(LL)tech$(LE) $(LL)mglcpp$(LE) $(BASELIBS)
.ENDIF

# Object files requied to build the demo

OBJECTS         = demo$O main$O explwind$O titlwind$O demoarea$O    \
				  demowind$O cmdline$O

.IF $(USE_WIN32)
OBJECTS         += demo$R
.ENDIF

USE_CXX_LINKER 	:= 1

demo$E: $(OBJECTS)

#----------------------------------------------------------------------------
# Target to build and install conformance program for DOS
#----------------------------------------------------------------------------

.PHONY mksdd:
	@build wc11-d32 -u demo.exe OPT=1
	@$(CP) demo.exe $(PRIVATE)/src/graphics/conform/dos
	@dmake cleanexe

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

