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
#               QNX Neutrino version for GNU C/C++
#
#############################################################################

# Include standard startup script definitions
.IMPORT .IGNORE: SCITECH;
.INCLUDE: "$(SCITECH)/makedefs/startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : USE_QNX USE_QNXNTO USE_PHOTON USE_X11 USE_BIOS USE_CXX_LINKER

# Standard file suffix definitions
#
# NOTE: Qnx does not require any extension for executeable files, but you
#       can use an extension if you wish. We use the .x extension for building
#       executeable files so that we can use implicit rules to make the
#       makefiles simpler and more portable between systems. When you install
#       the files to a local bin directory, you will probably want to remove
#       the .x extension.
   L            := .a       # Libraries
   E            := .exe     # Executables
   O            := .o       # Objects
   A            := .asm     # Assembler sources
   S            := .s       # GNU assembler sources
   P            := .cpp     # C++ sources

# File prefix/suffix definitions. The following prefixes are defined, and are
# used primarily to abstract between the Unix style libXX.a naming convention
# and the DOS/Windows/OS2 naming convention of XX.lib.
   LP           := lib      # LP - Library file prefix (name of file on disk)
   LL           := -l       # Library link prefix (name of library on link command line)
   LE           :=          # Library link suffix (extension of library on link command line)

# We use the Unix shell at all times
   SHELL        := /bin/sh
   SHELLFLAGS   := -c

# Definition of $(MAKE) macro for recursive makes.
   MAKE = $(MAKECMD) $(MFLAGS)

# Macro to install a library file
   INSTALL      := cp

# DMAKE uses this recipe to remove intermediate targets
.REMOVE :; $(RM) -f $<

# Turn warnings back to previous setting.
.SILENT := $(__.SILENT)

# We dont use TABS in our makefiles
.NOTABS         := yes

# Define that we are compiling for QNX
   USE_QNX      := 1

# Default commands for compiling, assembling linking and archiving.
   CC           := qcc
   CFLAGS       := -Vgcc_ntox86 -I. -Iinclude -I$(SCITECH:s,\,/)/include -I$(PRIVATE:s,\,/)/include
   CPFLAGS      :=
   CXX          := QCC
   AS           := nasm
   ASFLAGS      := -t -O2 -f elf -d__FLAT__ -d__GNUC__ -iinclude -i$(SCITECH)/include -d__NOU__ -d__QNXNTO__
   LD           := qcc
   LDXX         := qcc -lang-c++
   LDFLAGS      := -Vgcc_ntox86 -L. -lm
   LIB          := ar
   LIBFLAGS     := rc
   STRIP        := strip

# Aliases for macros to enable building shared libraries
.IF $(BUILD_DLL)
   BUILD_SO     := 1
.ENDIF

# Optionally build dynamic library instead of static lib
.IF $(BUILD_SO)
   CFLAGS       += -D__SHARED__ -shared
   LIB          := $(CC) -shared
   LIBFLAGS     := $(SOFLAGS)
   L            := .so
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -g2
   LDFLAGS      += -g2
.ELSE
   CFLAGS       += -DNDEBUG -fomit-frame-pointer
# NASM does not support debugging information yet
   ASFLAGS      +=
.ENDIF

# Optionally turn on optimisations
.IF $(OPT_MAX)
   CFLAGS       += -Ot
.ELIF $(OPT)
   CFLAGS       += -O
.ELIF $(OPT_SIZE)
   CFLAGS       += -Os
.ENDIF

# Compile flag for whether to build photon or non-photon lib
.IF $(USE_PHOTON)
   CFLAGS       += -D__PHOTON__
.ENDIF

# Compile flag for whether to build X11 or non-X11 lib
.IF $(USE_X11)
   CFLAGS       += -D__X11__
.ENDIF

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -dBETA
.ENDIF

# Target environment dependant flags
   CFLAGS       += -D__QNX__ -D__UNIX__
   ASFLAGS      += -d__QNX__ -d__UNIX__

# Define the base directory for library files

.IF $(CHECKED)
  LIB_BASE_DIR  := $(SCITECH_LIB)/lib/debug
  CFLAGS        += -DCHECKED=1
.ELSE
  LIB_BASE_DIR  := $(SCITECH_LIB)/lib/release
.ENDIF

# Define where to install library files
LIB_DEST_SHARED := $(LIB_BASE_DIR)/qnxnto/gcc/x86/so
LIB_DEST_STATIC := $(LIB_BASE_DIR)/qnxnto/gcc/x86/a
.IF $(BUILD_SO)
   LIB_DEST     := $(LIB_DEST_SHARED)
   LIB_BASE     := $(LIB_DEST_SHARED)
   LDFLAGS      += -L$(LIB_DEST_SHARED) -L$(LIB_DEST_STATIC)
.ELSE
   LIB_DEST     := $(LIB_DEST_STATIC)
   LIB_BASE     := $(LIB_DEST_STATIC)
   LDFLAGS      += -L$(LIB_DEST_STATIC)
.ENDIF

# Place to look for QNX VBIOS library
.IF $(USE_BIOS)
   LDFLAGS      += -L$(SCITECH)/drivers/qnx/neutrino
.ENDIF

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := qnxnto.mk
