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
#               OS/2 version for EMX/GNU C/C++.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : CRTDLL SHW NO_EXCEPT NO_RTTI FULLSCREEN EMX_LIBBASE USE_OS232 USE_OS2GUI

# Override File prefix/suffix definitions
   LP           :=              # LP - Library file prefix (name of file on disk)
   LL           := -l           # Library link prefix (name of library on link command line)
   LE           :=              # Library link suffix (extension of library on link command line)

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Default commands for compiling, assembling linking and archiving.
   CC           := gcc
   CFLAGS       := -Zmt -Zomf -Wall -I. -I$(INCLUDE)
   CXX          := gcc -x c++ -fno-exceptions -fno-rtti
   AS           := nasm
   ASFLAGS      := -t -O2 -f obj -F null -d__FLAT__ -d__NOU__ -iINCLUDE -i$(SCITECH)\INCLUDE
   LD           := gcc
   LDXX         := gcc -x c++
   LDFLAGS      := -L. -Zomf -Zmt
   LIB          := emxomfar
   LIBFLAGS     := -p32 rcv

   YACC         := bison -y
   LEX          := flex
   SED          := sed

# Optionally turn off exceptions and RTTI for C++ code
.IF $(NO_EXCEPT)
   CXX          += -fno-exceptions
.ENDIF
.IF $(NO_RTTI)
   CXX          += -fno-rtti
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -g
.ELSE
# Without -s, emx always runs LINK386 with the /DEBUG option
   CFLAGS       += -s
   LDFLAGS      += -s
# NASM does not support debugging information yet
   ASFLAGS      +=
.ENDIF

# Optionally turn on optimisations
.IF $(OPT_MAX)
   CFLAGS       += -O6
.ELIF $(OPT)
   CFLAGS       += -O3 -fomit-frame-pointer
.ELIF $(OPT_SIZE)
   CFLAGS       += -Os
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
CFLAGS                  += -fno-builtin -nostdinc
.ENDIF

# Link against EMX DLLs (CRTDLL=1) or link with static C runtime libraries
.IF $(CRTDLL)
  LDFLAGS       += -Zcrtdll
.ELSE
  CFLAGS        += -Zsys
  LDFLAGS       += -Zsys
.ENDIF

# Target environment dependant flags
   CFLAGS       += -D__OS2_32__
   CFLAGS       += -D__OS2__
   ASFLAGS      += -d__OS2__

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/debug
CFLAGS          += -DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/release
.ENDIF

# Define where to install library files
   LIB_DEST     := $(LIB_BASE_DIR)\OS232\$(EMX_LIBBASE)
   LDFLAGS      += -L$(LIB_DEST)

# Build 32-bit OS/2 apps
.IF $(BUILD_DLL)
   CFLAGS       += -Zdll -DBUILD_DLL
   LDFLAGS      += -Zdll
   ASFLAGS      += -dBUILD_DLL
.ELSE
.IF $(USE_OS2GUI)
   CFLAGS       += -D__OS2_PM__
   LDFLAGS      += -Zlinker /PMTYPE:PM
.ELSE
.IF $(FULLSCREEN)
   LDFLAGS      += -Zlinker /PMTYPE:NOVIO
.ELSE
   LDFLAGS      += -Zlinker /PMTYPE:VIO
.ENDIF
.ENDIF
.ENDIF

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := emx.mk
