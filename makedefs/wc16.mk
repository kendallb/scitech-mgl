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
#               Watcom C++ 10.x 16 bit version. Supports 16-bit DOS,
#               16-bit Windows development and 16-bit OS/2 development.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : WC_LIBBASE USE_WIN16 USE_OS216 USE_OS2GUI

# Default commands for compiling, assembling linking and archiving
   CC           := wcc      # C-compiler and flags
   CPP          := wpp      # C++-compiler and flags
   CFLAGS       := -ml-zq-j-w2-s-fh -fhq
   AS           := nasm
   ASFLAGS      := /t /mx /m /D__LARGE__ /iINCLUDE /i$(SCITECH)\INCLUDE
   LD           := wlink    # Loader and flags
   LDFLAGS       =
   RC           := wrc      # Watcom resource compiler
   RCFLAGS      := /bt=windows
   LIB          := wlib     # Librarian
   LIBFLAGS     := -q
   ILIB         := wlib     # Import librarian
   ILIBFLAGS    := -c

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -d2      # Turn on debugging for C compiler
   LIBFLAGS     += -p=128   # Larger page size for libraries with debug info!
   ASFLAGS      += /zi      # Turn on debugging for assembler
   LDFLAGS      += D A      # Turn on debugging for linker
.ELSE
   ASFLAGS      += /q       # Suppress object records not needed for linking
.END

# Optionally turn on optimisations
.IF $(OPT)
   CFLAGS       += -onatx-5
.ELIF $(OPT_SIZE)
   CFLAGS       += -onaslmr-5
.END

# Optionally turn on direct i387 FPU instructions optimised for Pentium

.IF $(FPU)
   CFLAGS       += -fpi87-fp5-DFPU387
   ASFLAGS      += -DFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -DBETA
.END

# Use a larger stack during linking if requested

.IF $(STKSIZE)
    LDFLAGS     += OP STACK=$(STKSIZE)
.ENDIF

.IF $(USE_OS216)
.IF $(BUILD_DLL)
    CFLAGS       += -bd-bt=os2-DBUILD_DLL
    ASFLAGS      += -DBUILD_DLL
.ELSE
    CFLAGS       += -bt=os2
.ENDIF
    DX_ASFLAGS   += -D__OS216__
    LIB_OS       = os216
.ELIF $(USE_WIN16)
.IF $(BUILD_DLL)
    CFLAGS       += -bd-bt=windows-D_WINDOWS-DBUILD_DLL
    ASFLAGS      += -DBUILD_DLL
.ELSE
    CFLAGS       += -bt=windows-D_WINDOWS
.ENDIF
    DX_ASFLAGS   += -D__WINDOWS16__
    LIB_OS       = WIN16
.ELSE
    USE_REALDOS  := 1
    LIB_OS       = DOS16
.END

# Place to look for PMODE library files

PMLIB           := pm.lib,

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
   LIB_BASE     := $(LIB_BASE_DIR)\$(LIB_OS)\$(WC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Define which file contains our rules

   RULES_MAK    := wc16.mk
