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
#               Microsoft C 6.0 16 bit version. Supports 16 bit
#               OS/2 development.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : VC_LIBBASE
.IMPORT .IGNORE : USE_MASM

# Default commands for compiling, assembling linking and archiving
   CC           := cl       # C-compiler and flags
   CFLAGS       := /w /Gs
   ASFLAGS      := /t /mx /m /D__COMM__ /iINCLUDE /i$(SCITECH)\INCLUDE
.IF $(USE_MASM)
   AS           := masm    # Assembler and flags
   ASFLAGS      := /D__COMM__ /iINCLUDE /i$(SCITECH)\INCLUDE
.ELSE
   AS           := nasm     # Assembler and flags
.ENDIF
   LD           := cl       # Loader and flags
   LDFLAGS       = $(CFLAGS)
   RC           := rc       # WIndows resource compiler
   RCFLAGS      :=
   LIB          := lib      # Librarian
   LIBFLAGS     := /NOI /NOE
   ILIB         := implib   # Import librarian
   ILIBFLAGS    := /noignorecase

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += /Zi  # Turn on debugging for C compiler
   ASFLAGS      += /zi      # Turn on debugging for assembler
.ELSE
   ASFLAGS      += /q       # Suppress object records not needed for linking
.END

# Optionally turn on optimisations
.IF $(OPT)
   CFLAGS       += /Ox
.END

# Optionally turn on direct i387 FPU instructions

.IF $(FPU)
   CFLAGS       += /FPi87 /DFPU387
   ASFLAGS      += /DFPU387 /DFPU_REG_RTN
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += /DBETA
   ASFLAGS      += /DBETA
.END

# Use a larger stack during linking if requested ???? How the fuck do you
# specify linker options on the CL command line?????

.IF $(STKSIZE)
.ENDIF

# Optionally compile for 16 bit Windows
.IF $(USE_WIN16)
.IF $(BUILD_DLL)
   CFLAGS       += /GD /Alfw /DBUILD_DLL
   ASFLAGS      += -DBUILD_DLL
.ELSE
   CFLAGS       += /GA /AL
.ENDIF
   DX_ASFLAGS   += -D__WINDOWS16__
   LIB_OS       = WIN16
.ELSE
   USE_REALDOS  := 1
   CFLAGS       += /AL
   LIB_OS       = DOS16
.END

# Place to look for PMODE library files

PMLIB           := pm.lib

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)\lib\debug
CFLAGS          += -DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)\lib\release
.ENDIF

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Define where to install library files
   LIB_BASE     := $(LIB_BASE_DIR)\$(LIB_OS)\$(VC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Define which file contains our rules

   RULES_MAK    := cl16.mk
