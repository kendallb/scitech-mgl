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
#               Microsoft Visual C++ 2.x 32 bit version. Supports Phar Lap
#               TNT DOS Extender and 32 bit Windows development.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)\makedefs\startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : TNT_PATH VC_LIBBASE DOSSTYLE USE_TNT USE_RTTARGET MSVCDIR
.IMPORT .IGNORE : USE_VXD USE_NTDRV USE_W2KDRV NT_DDKROOT USE_RTTI USE_CPPEXCEPT
.IMPORT .IGNORE : USE_RTFILES32 USE_RTKERNEL32 USE_RTT32DLL USE_WINCE
.IMPORT .IGNORE : _WINCEOSVER _WINCEOSVERMAJOR _WINCEOSVERMINOR	NO_MKDEF

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Default commands for compiling, assembling linking and archiving
   CC           := cl       # C-compiler and flags
   CFLAGS       := /Oi
   AS           := nasm
   ASFLAGS      := -t -O2 -f win32 -F null -d__FLAT__ -d__MSC__ -dSTDCALL_MANGLE -iINCLUDE -i$(SCITECH)\INCLUDE
   LD           := cl
.IF $(USE_WIN32)
   LDFLAGS       = $(CFLAGS)
.IF $(USE_NTDRV)
.IF $(USE_SOFTICE)
   LDENDFLAGS   = -link /INCREMENTAL:NO /ALIGN:0x20 /DRIVER /SUBSYSTEM:NATIVE,4.00 /VERSION:4.00 \
                  /MACHINE:I386 /NODEFAULTLIB /DEBUGTYPE:CV /DLL /PDB:$(@:b).pdb /BASE:0x10000 \
                  /OSVERSION:4.00 /MAP:$(@:b).map
.ELSE
   LDENDFLAGS   = -link /INCREMENTAL:NO /ALIGN:0x20 /DRIVER /SUBSYSTEM:NATIVE,4.00 /VERSION:4.00 \
                  /MACHINE:I386 /NODEFAULTLIB /DEBUGTYPE:CV /DLL /PDB:$(@:b).pdb /BASE:0x10000 \
                  /OSVERSION:4.00 /MAP:$(@:b).map
.ENDIF
.ELIF $(WIN32_GUI)
   LDENDFLAGS   = -link /INCREMENTAL:NO /DEF:$(@:b).def /SUBSYSTEM:WINDOWS /MACHINE:I386 /DEBUGTYPE:CV \
                  /PDB:$(@:b).pdb
.ELSE
   LDENDFLAGS   = -link /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /MACHINE:I386 /DEBUGTYPE:CV /PDB:$(@:b).pdb
.ENDIF
.ELIF $(USE_WINCE)
   LDFLAGS       = $(CFLAGS)
   LDENDFLAGS   = -link /INCREMENTAL:NO /DEBUGTYPE:CV /PDB:$(@:b).pdb /MAP:$(@:b).map \
                  /SUBSYSTEM:WINDOWSCE,$(_WINCEOSVERMAJOR).$(_WINCEOSVERMINOR) /DLL  /DEF:$(@:b).def
.ELIF $(USE_RTTARGET)
   LDFLAGS       = $(CFLAGS)
   LDENDFLAGS   = -link /INCREMENTAL:NO /DEBUGTYPE:CV /PDB:$(@:b).pdb /MAP:$(@:b).map
.ELSE
   LDFLAGS       = $(CFLAGS)
   LDENDFLAGS   := -link -stub:$(TNT_PATH:s/\/\\)\\bin\\gotnt.exe /PDB:$(@:b).pdb
.ENDIF
   RC           := rc       # Watcom resource compiler
   RCFLAGS      :=          # Mark as Win32 compatible resources
   LIB          := lib      # Librarian
   LIBFLAGS     :=
   ILIB         := lib      # Import librarian
   ILIBFLAGS    := /MACHINE:IX86
   INTEL_X86    := 1
   NMSYM        := $(SOFTICE_PATH)\nmsym.exe
.IF $(USE_NTDRV)
   NMSYMFLAGS   := /TRANSLATE:source,package,always /PROMPT /SOURCE:$(MSVCDIR)\crt\src\intel;$(SCITECH)\src\pm;$(SCITECH)\src\pm\common;$(SCITECH)\src\pm\ntdrv
.ELSE
   NMSYMFLAGS   := /TRANSLATE:source,package,always /PROMPT /SOURCE:$(SCITECH)\src\pm;$(SCITECH)\src\pm\common;$(SCITECH)\src\pm\win32
.ENDIF

# Set the compiler warning level
.IF $(MAX_WARN)
   CFLAGS       += -W3
.ELSE
   CFLAGS       += -W1
.ENDIF

# Optionally turn on debugging information
.IF $(DBG)
   CFLAGS       += /Yd /Z7  # Turn on debugging for C compiler
.ELSE
.END

# Optionally turn on optimisations
.IF $(VC_LIBBASE) == vc4
.IF $(OPT)
   CFLAGS       += /G5 /O2 /Ox
.ELIF $(OPT_SIZE)
   CFLAGS       += /G5 /O1
.END
.ELSE
.IF $(OPT)
   CFLAGS       += /G6 /O2 /Ox
.ELIF $(OPT_SIZE)
   CFLAGS       += /G6 /O1
.END
.ENDIF

# Optionally turn on direct i387 FPU instructions

.IF $(FPU)
   CFLAGS       += /DFPU387
   ASFLAGS      += -dFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += /DBETA
   ASFLAGS      += -dBETA
.END

# Use a larger stack during linking if requested, or use a default stack
# of 50k. The usual default stack provided by Visual C++ is *way* to small
# for real 32 bit code development.

.IF $(USE_WIN32)
    # Not necessary for Win32 code.
.ELSE
.IF $(STKSIZE)
    LDENDFLAGS  += /STACK:$(STKSIZE)
.ELSE
    LDENDFLAGS  += /STACK:51200
.ENDIF
.ENDIF

# OS Target dependant flags
.IF $(USE_NTDRV)                # Build 32 bit Windows NT driver
   CFLAGS       += /LD /Zl /Gy /Gz /GF /D_X86_=1 /Di386=1
.IF $(USE_W2KDRV)
   CFLAGS       += /D__NT_DRIVER__=5
.ELSE
   CFLAGS       += /D__NT_DRIVER__=4
.ENDIF
.IF $(DBG)
   CFLAGS       += /QIf
.ENDIF
   ASFLAGS      +=
   DEF_LIBS     :=
   DX_ASFLAGS   += -d__NT_DRIVER__
.IF $(USE_W2KDRV)               # Build 32 bit Windows 2000 driver
   LIB_OS       = W2KDRV
.ELSE
   LIB_OS       = NTDRV
.ENDIF
.ELIF $(USE_WIN32)              # Build 32 bit Windows app
.IF $(WIN32_GUI)
.ELSE
    CFLAGS       += -D__CONSOLE__
.ENDIF
.IF $(BUILD_DLL)
   CFLAGS       += /MT /LD /DBUILD_DLL
   ASFLAGS      += -dBUILD_DLL
.IF $(NO_RUNTIME)
   LDENDFLAGS   += /NODEFAULTLIB
   CFLAGS       += /Zl
   DEF_LIBS     :=
.ELSE
   DEF_LIBS     := kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib winmm.lib comdlg32.lib comctl32.lib ole32.lib oleaut32.lib version.lib winspool.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib rpcrt4.lib
.ENDIF
.ELSE
   CFLAGS       += /MT
   DEF_LIBS     := kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib winmm.lib comdlg32.lib comctl32.lib ole32.lib oleaut32.lib version.lib winspool.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib rpcrt4.lib
.ENDIF
   DX_ASFLAGS   += -d__WINDOWS32__
   LIB_OS       = WIN32
.ELIF $(USE_WINCE)              # Build 32 bit Windows CE app
   CFLAGS       += -D__WINCE__
   CFLAGS       += -D_WIN32_WCE=$(_WINCEOSVER) -D_X86_ -Dx86=1 -D_USRDLL=1 -DUNICODE -DWINCEOEM=1 -DUNDER_CE -DWIN32
   LIB_OS       = CEDRV
   LDENDFLAGS   += /NODEFAULTLIB:libc.lib /NODEFAULTLIB:oldnames.lib
   DEF_LIBS     := corelibc.lib coredll.lib ntcompat.lib
.ELIF $(USE_RTTARGET)           # Build 32 bit RTTarget app
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
   ASFLAGS      += -d__RTTARGET__
   LIB_OS       = RTT32
   LDENDFLAGS   += /fixed:no /opt:ref /opt:icf
   LDENDFLAGS   += /NODEFAULTLIB:kernel32.lib /INCLUDE:_malloc /INCLUDE:_EnterCriticalSection@4 /INCLUDE:_RTFileSystemList
.IF $(USE_RTFILES32)
   CFLAGS       += /MT
   LDENDFLAGS   += /NODEFAULTLIB:libc.lib
   DEF_LIBS     := rtfiles.lib rtfsk32.lib rtk32s.lib drvrt32.lib rtt32.lib
.ELIF $(USE_RTKERNEL32)
   CFLAGS       += /MT
   LDENDFLAGS   += /NODEFAULTLIB:libc.lib
   DEF_LIBS     := rtk32s.lib drvrt32.lib rtt32.lib
.ELIF $(USE_RTT32DLL)
   DEF_LIBS     := rtt32dll.lib kernel32.lib user32.lib
.ELSE
   DEF_LIBS     := rtt32.lib
.ENDIF
.ELSE                           # Build 32 bit TNT DOS Extender app
   USE_TNT      := 1
   USE_REALDOS  := 1
   CFLAGS       += /MT /D__MSDOS32__
   DX_CFLAGS    += -DTNT
   DX_ASFLAGS   += -dTNT
   LIB_OS       = DOS32
   DEF_LIBS     := dosx32.lib tntapi.lib
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
   LIB_BASE     := $(LIB_BASE_DIR)\$(LIB_OS)\$(VC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Place to look for PMODE library files

.IF $(USE_TNT)
PMLIB           := $(LIB_BASE:s/\/\\)\\tnt\\pm.lib
.ELSE
PMLIB           := $(LIB_BASE:s/\/\\)\\pm.lib
.ENDIF

# Define which file contains our rules

   RULES_MAK    := vc32.mk
