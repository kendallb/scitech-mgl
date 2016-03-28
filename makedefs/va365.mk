#############################################################################
#
#                       SciTech Makefile Utilities
#
#  ========================================================================
#
#   Copyright (C) 1991-2002 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2 as published by the Free
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
# Descripton:   Generic DMAKE startup makefile definitions file. Assumes
#               that the SCITECH environment variable has been set to point
#               to where all our stuff is installed. You should not need
#               to change anything in this file.
#
#               IBM VisualAge C++ 3.65 OS/2 32-bit version.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : VA_LIBBASE USE_OS232 USE_OS2GUI FULLSCREEN NOOPT MAX_WARN

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Default commands for compiling, assembling linking and archiving
   CC           := icc
   CPP          := icc
   CFLAGS       := /Q /G5l /Fi /Si /J- /Ss+ /Sp1 /Gm+ /I.
   AS           := nasm
   ASFLAGS      := -t -O2 -f obj -F null -d__FLAT__ -dSTDCALL_MANGLE -d__NOU_VAR__ -iINCLUDE -i$(SCITECH)\INCLUDE
   LD           := ilink
   LDFLAGS       = /noi /exepack /packcode /packdata /align:32 /map /noe
   RC           := rc
   RCFLAGS      := /nologo
   LIB          := ilib
   LIBFLAGS     := /nologo
   ILIB         := implib
   ILIBFLAGS    := /nologo
   IBMCOBJ      := 1

# Set the compiler warning level
.IF $(MAX_WARN)
   CFLAGS       += /W3
.ELSE
   CFLAGS       += /W1
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += /Ti
   LDFLAGS      += /DE
.ELSE
.END

# Optionally turn on optimisations
.IF $(OPT)
   CFLAGS       += /Gfi /O /Oi
.ELIF $(OPT_SIZE)
   CFLAGS       += /Gfi /O /Oc
.ELIF $(NOOPT)
   CFLAGS       += /O-
.END

# Optionally turn on direct i387 FPU instructions optimised for Pentium
.IF $(FPU)
   CFLAGS       += -DFPU387
   ASFLAGS      += -dFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -dBETA
.END

# Build 32-bit OS/2 apps
.IF $(BUILD_DLL)
   CFLAGS       += /Gme- /DBUILD_DLL
   LDFLAGS      += /DLL /NOE
   ASFLAGS      += -dBUILD_DLL
.ELSE
.IF $(USE_OS2GUI)
   CFLAGS       += -D__OS2_PM__
   LDFLAGS      += /PMTYPE:PM
.ELSE
.IF $(FULLSCREEN)
   LDFLAGS      += /PMTYPE:NOVIO
.ELSE
   LDFLAGS      += /PMTYPE:VIO
.ENDIF
.ENDIF
.ENDIF
   DX_ASFLAGS   += -d__OS2__
   LIB_OS       = os232

# Place to look for PMODE library files

.IF $(USE_OS2GUI)
PMLIB           := pm_pm.lib
.ELSE
PMLIB           := pm.lib
.ENDIF

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)\lib\debug
CFLAGS          += /DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)\lib\release
.ENDIF

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Define where to install library files
   LIB_BASE     := $(LIB_BASE_DIR)\$(LIB_OS)\$(VA_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Define which file contains our rules

   RULES_MAK    := va365.mk
