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
#               Borland C++ 4.0 32 bit version. Supports Borland's DOS Power
#               Pack DPMI32 DOS extender, Phar Lap's TNT DOS Extender and
#               32 bit Windows development.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : USE_SMX32 USE_TNT USE_WIN32 USE_BC5 USE_VXD BC_LIBBASE
.IMPORT .IGNORE : VTOOLSD USE_RTTARGET USE_RTFILES32 USE_RTKERNEL32 USE_RTT32DLL

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Default commands for compiling, assembling linking and archiving
   CC           := bcc32
.IF $(USE_VXD)
   CFLAGS       := -4 -i60 -d -w-stu
.ELSE
   CFLAGS       := -4 -H=bcc32.sym -i60 -d -w-stu
.ENDIF
   AS           := nasm
   ASFLAGS      := -t -f obj -d__FLAT__ -iINCLUDE -i$(SCITECH)\INCLUDE
   TASM         := tasm
   TASMFLAGS    := /t /mx /m /w-res /w-mcp /D__FLAT__ /iINCLUDE /i$(SCITECH)\INCLUDE
   LD           := bclink tlink32.exe
   LDFLAGS      := -c
   RC           := brc32
.IF $(USE_BC5)
   WIN_VERSION  := -V4.0
   RCFLAGS      := -32
.ELSE
   RCFLAGS      := -w32
.ENDIF
   LIB          := tlib
   LIBFLAGS     := /C
   ILIB         := implib
   ILIBFLAGS    := -c
   INTEL_X86    := 1
   NMSYM        := $(SOFTICE_PATH)\nmsym.exe
   NMSYMFLAGS   := /TRANSLATE:source,package,always /PROMPT /SOURCE:$(SCITECH)\src\pm;$(SCITECH)\src\pm\common;$(SCITECH)\src\pm\win32

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += -v
   LDFLAGS      += -v
   LIBFLAGS     += /P256
   ASFLAGS      += -F borland -g
   TASMFLAGS    += /zi
.ELSE
   LDFLAGS      += -x
   LIBFLAGS     += /P128
   ASFLAGS      += -F null
   TASMFLAGS    += /q
.END

# Optionally disable nagging warnings if MAX_WARN is not on
.IF $(MAX_WARN)
.ELSE
   CFLAGS       += -w-aus -w-par -w-hid -w-pia
.ENDIF

# Optionally turn on optimisations (-5 -O2 breaks BC++ 4.0-4.5 sometimes)
.IF $(OPT)
   CFLAGS       += -5 -O2 -k-
.ELIF $(OPT_SIZE)
   CFLAGS       += -5 -O1 -k-
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

# Optionally use Phar Lap's TNT DOS Extender, otherwise use the DOS Power Pack
.IF $(USE_TNT)
   CFLAGS       += -D__MSDOS__
   DX_CFLAGS    += -DTNT
   DX_ASFLAGS   += -dTNT
   LIB_OS       = DOS32
   DEF_LIBS     := import32.lib cw32.lib dosx32.lib tntapi.lib
.ELIF $(USE_VXD)
   LDFLAGS      += -n -P- -x
   CFLAGS       += -RT- -x- -Oi -VC -I$(VTOOLSD)\INCLUDE -DIS_32 -DWANTVXDWRAPS -DVTOOLSD -DWIN40 -DWIN40_OR_LATER -DDEFSEG=1 -zC_LTEXT -zALCODE -zR_LDATA -zTLCODE
   DEF_LIBS     := $(VTOOLSD)\lib\cfbc440d.lib $(VTOOLSD)\lib\wr0bc440.lib $(VTOOLSD)\lib\wr1bc440.lib $(VTOOLSD)\lib\wr2bc440.lib $(VTOOLSD)\lib\wr3bc440.lib $(VTOOLSD)\lib\rtbc440d.lib
   TASMFLAGS    += -d__VXD__ -d__BORLANDC__=1 -i$(VTOOLSD)\INCLUDE -i$(VTOOLSD)\LIB\INCLUDE
   LIB_OS       = VXD
.ELIF $(USE_WIN32)
.IF $(WIN32_GUI)
.ELSE
    CFLAGS       += -D__CONSOLE__
.ENDIF
.IF $(BUILD_DLL)
   CFLAGS       += -WD -DBUILD_DLL
   ASFLAGS      += -dBUILD_DLL
.ELSE
   CFLAGS       += -W -WM
.ENDIF
.IF $(USE_BC5)
.ELSE
   CFLAGS       += -D_WIN32
.ENDIF
   DEF_LIBS     := import32.lib cw32mt.lib
   DX_ASFLAGS   += -d__WINDOWS32__
   LIB_OS       = WIN32
.ELIF $(USE_SMX32)
   CFLAGS       += -D__SMX32__ -DPME32
   DX_CFLAGS    +=
   DX_ASFLAGS   += -d__SMX32__ -dDPMI32 -dPME32
   USE_REALDOS  := 1
   LIB_OS       = SMX32
   DEF_LIBS     := cw32mt.lib
.ELIF $(USE_RTTARGET)
   CFLAGS       += -D__RTTARGET__
.IF $(USE_RTFILES32)
   CFLAGS       += -DUSE_RTFILES32
.ENDIF
.IF $(USE_RTKERNEL32)
   CFLAGS       += -DUSE_RTKERNEL32
.ENDIF
.IF $(USE_RTT32DLL)
   CFLAGS       += -DUSE_RTT32DLL
.ENDIF
   DX_ASFLAGS   += -d__RTTARGET__ 
   LIB_OS       = RTT32
.IF $(USE_RTFILES32)
   DEF_LIBS     := rtfiles.lib rtfsk32.lib rtk32s.lib drvrt32.lib rtt32.lib cw32mt.lib
.ELIF $(USE_RTKERNEL32)
   DEF_LIBS     := rtk32s.lib drvrt32.lib rtt32.lib cw32mt.lib
.ELIF $(USE_RTT32DLL)
   DEF_LIBS     := rtt32dll.lib import32.lib cw32.lib
.ELSE
   DEF_LIBS     := rtt32.lib cw32.lib
.ENDIF
.ELSE
   USE_DPMI32   := 1
   CFLAGS       += -D__MSDOS__
   DX_CFLAGS    += -WX -DDPMI32
   DX_ASFLAGS   += -dDPMI32
   USE_REALDOS  := 1
   LIB_OS       = DOS32
   DEF_LIBS     :=
.END

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
   LIB_BASE     := $(LIB_BASE_DIR)\$(LIB_OS)\$(BC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Place to look for PMODE library files

.IF $(USE_TNT)
PMLIB           := $(LIB_BASE)\tnt\pm.lib
.ELIF $(USE_DPMI32)
PMLIB           := $(LIB_BASE)\dpmi32\pm.lib
.ELSE
PMLIB           := $(LIB_BASE)\pm.lib
.END

# Define which file contains our rules

   RULES_MAK    := bc32.mk
