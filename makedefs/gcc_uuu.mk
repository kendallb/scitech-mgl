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
#               Linux version for GNU C/C++.
#
#############################################################################

# Include standard startup script definitions
.IMPORT .IGNORE: SCITECH;
.INCLUDE: "$(SCITECH)/makedefs/startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : GCC2_LIBBASE USE_PPC_BE USE_ALPHA USE_CXX_LINKER

# Override some file suffix definitions
   L            := .a     # Libraries
   O            := .o     # Objects
.IF $(USE_PPC_BE)
   A            := .s_ppc # Assembler files
.ENDIF

# Override the file prefix/suffix definitions for library naming.
   LP           := lib      # LP - Library file prefix (name of file on disk)
   LL           := -l       # Library link prefix (name of library on link command line)
   LE           :=          # Library link suffix (extension of library on link command line)

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Define that we are compiling for Unununium
   USE_UUU      := 1

# Default commands for compiling, assembling linking and archiving.
   CC           := gcc
   CFLAGS       := -Wall -I. -Iinclude -I$(SCITECH:s,\,/)/include -I$(PRIVATE:s,\,/)/include
   SHOW_CFLAGS  := -c
   CXX          := g++
   LD           := gcc
   LDXX         := g++
   LDFLAGS      := -L.
   SYMLINK      := ln -s -f
   LIB          := ar
   LIBFLAGS     := rcs
   YACC         := bison -y
   LEX          := flex
   SED          := sed

.IF $(USE_PPC_BE)
   AS           := as
   ASFLAGS      := -mregnames -I include -I $(SCITECH)/include
   SHOW_ASFLAGS :=
.ELSE
   AS           := nasm
   ASFLAGS      := -t -O2 -f elf -d__FLAT__ -d__GNUC__ -iinclude -i$(SCITECH)/include -d__NOU__
   SHOW_ASFLAGS := -f elf
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -g
   SHOW_CFLAGS  += -g
   # NASM does not support debugging information yet
  .IF $(USE_PPC_BE)
      ASFLAGS      += --gstabs
      SHOW_ASFLAGS += --gstabs
  .ENDIF
.ENDIF

# Optionally turn on optimisations
.IF $(OPT_MAX)
   CFLAGS       += -O6
   SHOW_CFLAGS  += -O6
.ELIF $(OPT)
   CFLAGS       += -O2
   SHOW_CFLAGS  += -O2
.ELIF $(OPT_SIZE)
   CFLAGS       += -O1
   SHOW_CFLAGS  += -O1
.ENDIF

# Optionally turn on direct i387 FPU instructions
.IF $(FPU)
   CFLAGS       += -DFPU387
  .IF $(USE_PPC_BE)
  .ELSE
      ASFLAGS   += -dFPU387
  .ENDIF
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   SHOW_CFLAGS  += -DBETA
  .IF $(USE_PPC_BE)
  .ELSE
      ASFLAGS      += -dBETA
      SHOW_ASFLAGS += -dBETA
  .ENDIF
.ENDIF

# Disable standard C runtime library

.IF $(NO_RUNTIME)
CFLAGS          += -fno-builtin -nostdinc
.ENDIF

# Compile flag for whether to build X11 or non-X11 lib
.IF $(USE_X11)
   CFLAGS       += -D__X11__
.ENDIF

# Target environment dependant flags
CFLAGS          += -D__UUU__
.IF $(USE_PPC_BE)
   CFLAGS       += -D__PPC__ -fsigned-char
.ELIF $(USE_ALPHA)
   CFLAGS       += -D__ALPHA__
.ELSE
   # disabled, because bochs doesn't like it
   CFLAGS       += #-mcpu=i686
.ENDIF

.IF $(USE_PPC_BE)
.ELSE
   ASFLAGS      += -d__UUU__
.ENDIF

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/debug
CFLAGS          += -DCHECKED=1
SHOW_CFLAGS     += -DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)/lib/release
.ENDIF

# Define where to install library files
.IF $(USE_PPC_BE)
   LIB_DEST_SHARED  := $(LIB_BASE_DIR)/uuu/gcc/ppc-be/so
   LIB_DEST_STATIC  := $(LIB_BASE_DIR)/uuu/gcc/ppc-be/a
.ELIF $(USE_ALPHA)
   LIB_DEST_SHARED  := $(LIB_BASE_DIR)/uuu/gcc/alpha/so
   LIB_DEST_STATIC  := $(LIB_BASE_DIR)/uuu/gcc/alpha/a
.ELSE
   USE_X86          := 1
   LIB_DEST_SHARED  := $(LIB_BASE_DIR)/uuu/gcc/x86/so
   LIB_DEST_STATIC  := $(LIB_BASE_DIR)/uuu/gcc/x86/a
.ENDIF

# Link to static libraries if requested
.IF $(STATIC_LIBS_ALL)
   LDFLAGS      += -static
   STATIC_LIBS  := 1
.ENDIF

# Link to static libraries if requested
.IF $(STATIC_LIBS)
   LDFLAGS      += -L$(LIB_DEST_STATIC)
.ELSE
   LDFLAGS      += -L$(LIB_DEST_SHARED) -L$(LIB_DEST_STATIC)
.ENDIF

# Default version is 5.0, but it can be overridden from the dmake command
# line when building release library.
.IF $(VERSIONMAJ)
.ELSE
   VERSIONMAJ   := 5
   VERSIONMID   := 0
   VERSIONMIN   := 0
.ENDIF

# Aliases for macros to enable building shared libraries
.IF $(BUILD_DLL)
   BUILD_SO     := 1
.ENDIF
.IF $(BUILD_FOR_DLL)
   BUILD_FOR_SO := 1
.ENDIF

# Optionally enable some  dynamic libraries to be built.
.IF $(BUILD_FOR_SO)
.IF $(NO_PIC)
.ELSE
   CFLAGS       += -fPIC
   SHOW_CFLAGS  += -fPIC
   .IF $(USE_PPC_BE)
   .ELSE
      ASFLAGS      += -D__PIC__
      SHOW_ASFLAGS += -D__PIC__
   .ENDIF
.ENDIF
   LIB_DEST     := $(LIB_DEST_STATIC)
   LIB_BASE     := $(LIB_DEST_STATIC)
.ELIF $(BUILD_SO)
   VERSION      := $(VERSIONMAJ).$(VERSIONMID).$(VERSIONMIN)
   CFLAGS       += -D__SHARED__
   LIB          := gcc -shared
   LIBFLAGS     :=
   L            := .so
.IF $(NO_PIC)
.ELSE
   CFLAGS       += -fPIC
   SHOW_CFLAGS  += -fPIC
   .IF $(USE_PPC_BE)
   .ELSE
      ASFLAGS      += -D__PIC__
      SHOW_ASFLAGS += -D__PIC__
   .ENDIF
.ENDIF
   LIB_DEST     := $(LIB_DEST_SHARED)
   LIB_BASE     := $(LIB_DEST_SHARED)
.ELSE
   LIB_DEST     := $(LIB_DEST_STATIC)
   LIB_BASE     := $(LIB_DEST_STATIC)
.ENDIF

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := gcc_uuu.mk

