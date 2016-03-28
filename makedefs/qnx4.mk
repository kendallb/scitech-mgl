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
#               QNX 4 version for Watcom C.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)/makedefs/startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : USE_QNX USE_QNX4 USE_PHOTON USE_BIOS WC_LIBBASE

# Define that we are compiling for QNX
   USE_QNX      := 1

# Default commands for compiling, assembling linking and archiving.
   CC           := wcc386
   CFLAGS       := -I. -Iinclude $(INCLUDE)
   CXX          := wpp386
   AS           := nasm
   ASFLAGS      := -t -O2 -f obj -d__FLAT__ -dSTDCALL_MANGLE -iinclude -i$(SCITECH)/include
   LD           := cc
   LDFLAGS      := -L.
   LIB          := ar
   LIBFLAGS     := rc

# Set the compiler warning level
.IF $(MAX_WARN)
   CFLAGS       += -w4
.ELSE
   CFLAGS       += -w1
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -d2
   LDFLAGS      += -g2
.ELSE
# NASM does not support debugging information yet
   ASFLAGS      +=
.ENDIF

# Optionally turn on optimisations
.IF $(OPT)
   CFLAGS       += -onatx-5r-fp5
.ELIF $(OPT_SIZE)
   CFLAGS       += -onaslmr-5r-fp5
.ELIF $(NOOPT)
   CFLAGS       += -od-5r
.END

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

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Define where to install library files
   LIB_BASE     := $(LIB_BASE_DIR)/qnx4/$(WC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)
   LDFLAGS      += -L$(LIB_DEST)

# Place to look for PMODE library files

PMLIB           := -lpm

# Define which file contains our rules

   RULES_MAK    := qnx4.mk
