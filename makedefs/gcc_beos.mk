#############################################################################
#
#                       SciTech Makefile Utilities
#
#  ========================================================================
#
#   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
#               BeOS version for GNU C/C++.
#
#############################################################################

# Include standard startup script definitions
.IMPORT .IGNORE: SCITECH;
.INCLUDE: "$(SCITECH)/makedefs/startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE :  USE_X11 USE_LINUX USE_EGCS USE_PGCC STATIC_LIBS LIBC

# Define that we are compiling for BeOS
   USE_BEOS     := 1

# Default commands for compiling, assembling linking and archiving.
   CC           := gcc
   CFLAGS       := -Wall -I. -Iinclude $(INCLUDE)
   CXX          := g++
   AS           := nasm
   ASFLAGS      := -O2 -f elf -d__FLAT__ -iinclude -i$(SCITECH)/include -d__NOU__
   LD           := gcc
   LDFLAGS      := -L.
   LIB          := ar
   LIBFLAGS     := rcs

# Link to static libraries if requested
.IF $(STATIC_LIBS)
   LDFLAGS      += -static
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -g
.ELSE
# NASM does not support debugging information yet
   ASFLAGS      +=
.ENDIF

# Optionally turn on optimisations
.IF $(OPT_MAX)
   CFLAGS       += -O6
.ELIF $(OPT)
   CFLAGS       += -O2
.ELIF $(OPT_SIZE)
   CFLAGS       += -O1
.ENDIF

# Optionally turn on direct i387 FPU instructions
.IF $(FPU)
   CFLAGS       += -DFPU387
   ASFLAGS      += -dFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -dBETA
.ENDIF

# Disable standard C runtime library

.IF $(NO_RUNTIME)
CFLAGS          += -fno-builtin -nostdinc
.ENDIF

# Target environment dependant flags
   CFLAGS       += -D__BEOS__
   ASFLAGS      += -d__BEOS__ -d__UNIX__

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/debug
CFLAGS      += -DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/release
.ENDIF

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Define where to install library files
LIB_DEST     := $(LIB_BASE_DIR)/beos/gcc
LDFLAGS      += -L$(LIB_DEST)

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := gcc_beos.mk
