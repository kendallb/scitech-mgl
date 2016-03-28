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
#               DJGPP V2 port of GNU C/C++ to DOS with DPMI only.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Override some file suffix definitions
   L            := .a     # Libraries
   O            := .o     # Objects

# Override the file prefix/suffix definitions for library naming.
   LP           := lib      # LP - Library file prefix (name of file on disk)
   LL           := -l       # Library link prefix (name of library on link command line)
   LE           :=          # Library link suffix (extension of library on link command line)

# Import enivornment variables that we use
.IMPORT .IGNORE : DJ_LIBBASE

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Default commands for compiling, assembling linking and archiving
   CC           := gcc      # C-compiler and flags
   CFLAGS       := -Wall -I. -Iinclude -I$(SCITECH:s,\,/)/include -I$(PRIVATE:s,\,/)/include
   AS           := nasm
   ASFLAGS      := -t -O2 -f coff -F null -d__FLAT__ -d__GNUC__ -dSTDCALL_USCORE -iINCLUDE -i$(SCITECH)\INCLUDE
   LD           := dj_ld    # Loader and flags
   LDFLAGS      :=
   LIB          := ar       # Librarian
   LIBFLAGS     := rs
   USE_GCC      := 1

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -g       # Turn on debugging for C compiler
.END

# Optionally turn on optimisations
.IF $(OPT)
   CFLAGS       += -O2
.ELIF $(OPT_SIZE)
   CFLAGS       += -O1
.END

# Optionally turn on direct i387 FPU instructions

.IF $(FPU)
   CFLAGS       += -DFPU387
   ASFLAGS      += -dFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -dBETA
.END

# DOS extender dependant flags
   DX_CFLAGS    +=
   DX_ASFLAGS   += -dDJGPP
   USE_REALDOS  := 1

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
   LIB_DEST     := $(LIB_BASE_DIR)\DOS32\$(DJ_LIBBASE)
   LDFLAGS        += -L$(LIB_DEST)

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := dj32.mk
