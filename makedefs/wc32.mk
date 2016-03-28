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
#               Watcom C++ 10.x/11.x 32 bit version. Supports Rational's DOS4GW
#               DOS Extender, PMODE/W, Causeway, FlashTek's X32-VM,
#               Phar Lap's TNT DOS Extender, 32-bit Windows development and
#               32-bit OS/2 development.
#
#############################################################################

# Include standard startup script definitions
.IMPORT: SCITECH
.INCLUDE: "$(SCITECH)/makedefs/startup.mk"

# Import enivornment variables that we use
.IMPORT .IGNORE : USE_TNT USE_X32 USE_X32VM USE_PMODEW STKCALL USE_CAUSEWAY
.IMPORT .IGNORE : USE_WIN386 USE_OS232 USE_OS2GUI WC_LIBBASE NOOPT DOSSTYLE
.IMPORT .IGNORE : OS2_SHELL USE_CODEVIEW USE_DOS32A USE_QNX4 LEAVE_LINKFILE
.IMPORT .IGNORE : DLL_CRTL USE_LINUX DEAD_CODE USE_SNAP_DRV WATCOM
.IMPORT .IGNORE : USE_RTTARGET USE_RTT32DLL FULLSCREEN USE_NTDRV USE_W2KDRV

# We are compiling for a 32 bit envionment
   _32BIT_      := 1

# Setup special environment for QNX 4 (Unix'ish)
.IF $(USE_QNX4)
   USE_QNX      := 1
   L            := .a       # Libraries
   LP           := lib      # LP - Library file prefix (name of file on disk)
   LL           := lib      # Library link prefix (name of library on link command line)
   LE           := .a       # Library link suffix (extension of library on link command line)
.ENDIF

# Default commands for compiling, assembling linking and archiving
   CC           := wcc386
   CFLAGS       := -zq-s-fpi87-zld-ei
   CPP          := wpp386
   CPFLAGS      :=
   AS           := nasm
   ASFLAGS      := -t -O2 -f obj -d__FLAT__ -dSTDCALL_MANGLE -iINCLUDE -i$(SCITECH)$(SL)include
   SHOW_ASFLAGS := -f obj
   TASM         := tasm
   TASMFLAGS    := /t /mx /m /w-res /w-mcp /D__FLAT__ /DSTDCALL_MANGLE /iINCLUDE /i$(SCITECH)$(SL)include
   GAS          := gcc
   GAS_FLAGS    := -D__WATCOMC__ -D__SW_3S -D__SW_S -U__GNUC__ -UDJGPP -U__unix__ -Wall -I. -I$(SCITECH)$(SL)include -x assembler-with-cpp
   LD           := wlink
   LDFLAGS       = OP MAP OP CASEEXACT
.IF $(USE_OS232)
.IF $(FULLSCREEN)
   OS2_FS       := FULL
.ENDIF
   RC           := rc
.ELSE
   RC           := wrc
.ENDIF
.IF $(USE_WIN32)
   RCFLAGS      := -q -bt=nt
.ELIF $(USE_OS232)
.IF $(USE_OS2GUI)
   CFLAGS       += -D__OS2_PM__
.ENDIF
.ELSE
   RCFLAGS      := -q
.ENDIF
   LIB          := wlib
   LIBFLAGS     := -q
   ILIB         := wlib
   ILIBFLAGS    := -c
   INTEL_X86    := 1

# Determine if we have Watcom 11 or higher compiler (Open Watcom is higher)
.IF $(WC_LIBBASE) == wc10a
.ELIF $(WC_LIBBASE) == wc10
.ELSE
HAVE_WC11       := 1
.ENDIF

# Set the compiler warning level
.IF $(MAX_WARN)
   CFLAGS       += -w4
.ELSE
   CFLAGS       += -w1
.ENDIF

# Optionally turn off dead code elimination. This is on by default
# and will compile all functions into a separate segment so they
# can be eliminated at link time. This is nearly always a win
# for the resulting code size.
.IF $(DEAD_CODE)
.ELSE
    CFLAGS      += -zm
    LDFLAGS     += OP EL
.ENDIF

# Optionally turn on debugging information (Codeview format)
.IF $(DBG)
.IF $(USE_WIN32)
.IF $(USE_CODEVIEW)
   CFLAGS       += -d2 -hc -DDBG_ON=1
   LDFLAGS      += D CODEVIEW OPT CVPACK
.ELSE
   CFLAGS       += -d2 -DDBG_ON=1
   LDFLAGS      += D A
.ENDIF
.ELSE
   CFLAGS       += -d2 -DDBG_ON=1
   LDFLAGS      += D A
.ENDIF
.IF $(HAVE_WC11)
   CPFLAGS      += -d2i
.ENDIF
   LIBFLAGS     += -p=1280
   ASFLAGS      += -F borland -g
   SHOW_ASFLAGS += -g
   TASMFLAGS    += /zi
   SHOW_CFLAGS  += -d2
.ELSE
   ASFLAGS      += -F null
   TASMFLAGS    += /q
.END

# Determine the CPU optimisation type to use

.IF $(HAVE_WC11)
CPU_TYPE        = 6
.ELSE
CPU_TYPE        = 5
.ENDIF

# Optionally turn on optimisations (with or without stack conventions)
.IF $(STKCALL)
.IF $(OPT)
   CFLAGS       += -onatx-$(CPU_TYPE)s-fp$(CPU_TYPE)
   CPFLAGS      += -zv
   SHOW_CFLAGS  += -onatx
.ELIF $(OPT_SIZE)
   CFLAGS       += -onaslmr-$(CPU_TYPE)s-fp$(CPU_TYPE)
   CPFLAGS      += -zv
   SHOW_CFLAGS  += -onaslmr
.ELIF $(NOOPT)
   CFLAGS       += -od-$(CPU_TYPE)s
   SHOW_CFLAGS  += -od
.ELSE
   CFLAGS       += -$(CPU_TYPE)s
.END
.ELSE
.IF $(OPT)
   CFLAGS       += -onatx-$(CPU_TYPE)r-fp$(CPU_TYPE)
   SHOW_CFLAGS  += -onatx
.ELIF $(OPT_SIZE)
   CFLAGS       += -onaslmr-$(CPU_TYPE)r-fp$(CPU_TYPE)
   SHOW_CFLAGS  += -onaslmr
.ELIF $(NOOPT)
   CFLAGS       += -od-$(CPU_TYPE)r
   SHOW_CFLAGS  += -od
.END
.END

# Optionally turn on direct i387 FPU instructions optimised for Pentium
.IF $(FPU)
   CFLAGS       += -DFPU387
   ASFLAGS      += -dFPU387
.END

# Optionally compile a beta release version of a product
.IF $(BETA)
   CFLAGS       += -DBETA
   ASFLAGS      += -dBETA
   SHOW_CFLAGS  += -DBETA
   SHOW_ASFLAGS += -dBETA
.END

.IF $(USE_NTDRV)                # Build 32-bit Windows NT driver
    CFLAGS       += -D_X86_=1 -Di386=1 #/LD /Zl /Gy /Gz /GF
    ASFLAGS      +=
    DEF_LIBS     :=
    DX_ASFLAGS   += -d__NT_DRIVER__
.IF $(USE_W2KDRV)               # Build 32-bit Windows 2000 driver
    CFLAGS       += -D__NT_DRIVER__=5
    LIB_OS       = W2KDRV
.ELSE
    CFLAGS       += -D__NT_DRIVER__=4
    LIB_OS       = NTDRV
.ENDIF
.ELIF $(USE_TNT)                # Use Phar Lap's TNT DOS Extender
    CFLAGS       += -bt=nt -DTNT
    ASFLAGS      += -dTNT
    LDFLAGS      += SYS NT OP STUB=GOTNT.EXE
    LIB_OS       = dos32
.ELIF $(USE_X32VM)              # Use FlashTek X-32VM DOS extender
    CFLAGS       += -bt=dos
    LDFLAGS      += SYS X32RV
    DX_CFLAGS    += -DX32VM
    DX_ASFLAGS   += -dX32VM
    LIB_OS       = dos32
.ELIF $(USE_X32)                # Use FlashTek X-32 DOS extender
    CFLAGS       += -bt=dos
    LDFLAGS      += SYS X32R
    DX_CFLAGS    += -DX32VM
    DX_ASFLAGS   += -dX32VM
    LIB_OS       = dos32
.ELIF $(USE_QNX4)               # Build QNX 4 app
    CFLAGS       += -bt=qnx386
    LDFLAGS      += SYS QNX386FLAT OP OFFSET=40k OP STACK=32k
    CFLAGS       += -D__QNX__ -D__UNIX__
    ASFLAGS      += -d__QNX__ -d__UNIX__
    LIB_OS       = qnx4
.ELIF $(USE_LINUX)              # Build Linux app
    CFLAGS       += -bt=linux
    LDFLAGS      += SYS LINUX
    CFLAGS       += -D__LINUX__ -D__UNIX__
    ASFLAGS      += -d__LINUX__ -d__UNIX__
    LIB_OS       = linux
.ELIF $(USE_OS232)
.IF $(BUILD_DLL)
    CFLAGS       += -bm-bd-bt=os2-sg-DBUILD_DLL
    ASFLAGS      += -dBUILD_DLL
.ELSE
    CFLAGS       += -bm-bt=os2-sg
.ENDIF
    DX_ASFLAGS   += -d__OS2__
    LIB_OS       = os232
.ELIF $(USE_SNAP)               # Build 32 bit Snap app or driver
.IF $(BUILD_DLL)
    CFLAGS       += -bd-bt=snap-DBUILD_DLL
    ASFLAGS      += -dBUILD_DLL
.ELSE
    CFLAGS       += -bt=snap
.ENDIF
.IF $(USE_SNAP_DRV)
    CFLAGS       += -D__DRIVER__
    ASFLAGS      += -D__DRIVER__
    LDFLAGS      += OP nodefaultlibs
    DEFLIBS      := clib.lib,mathlib.lib,cpplib.lib
    LIB_OS       = drv
.ELSE
    LIB_OS       = snap
.ENDIF
.ELIF $(USE_WIN32)              # Build 32 bit Windows NT app
.IF $(WIN32_GUI)
.ELSE
    CFLAGS       += -D__CONSOLE__
.ENDIF
.IF $(BUILD_DLL)
    CFLAGS       += -bm-bd-bt=nt-sg-DBUILD_DLL -D_WIN32
    ASFLAGS      += -dBUILD_DLL
    SHOW_CFLAGS  += -bd
.ELSE
    CFLAGS       += -bm-bt=nt-sg-D_WIN32
.ENDIF
    DX_ASFLAGS   += -d__WINDOWS32__
    LIB_OS       = win32
    DEFLIBS      := kernel32.lib,user32.lib,gdi32.lib,advapi32.lib,shell32.lib,winmm.lib,comdlg32.lib,comctl32.lib,ole32.lib,oleaut32.lib,version.lib,winspool.lib,uuid.lib,wsock32.lib,rpcrt4.lib,
.ELIF $(USE_RTTARGET)           # Build 32 bit RTTarget-32 app
    CFLAGS       += -D__RTTARGET__ -DUSE_RTT32DLL
    AFLAGS       += -d__RTTARGET__
    LIB_OS       = rtt32
    DEFLIBS      := rtt32dll.lib,kernel32.lib,user32.lib,
.ELIF $(USE_WIN386)             # Build 32 bit Win386 extended app
.IF $(BUILD_DLL)
    CFLAGS       += -bd-bt=windows-DBUILD_DLL
    ASFLAGS      += -dBUILD_DLL
.ELSE
    CFLAGS       += -bt=windows
.ENDIF
    DX_ASFLAGS   += -d__WIN386__
    LIB_OS       = win386
.ELIF $(USE_PMODEW)             # PMODE/W
    CFLAGS       += -bt=dos
    USE_DOS4GW   := 1
    USE_REALDOS  := 1
    LDFLAGS      += SYS PMODEW
    DX_CFLAGS    += -DDOS4GW
    DX_ASFLAGS   += -dDOS4GW
    LIB_OS       = dos32
.ELIF $(USE_CAUSEWAY)           # Causeway
    CFLAGS       += -bt=dos
    USE_DOS4GW   := 1
    USE_REALDOS  := 1
    LDFLAGS      += SYS CAUSEWAY
    DX_CFLAGS    += -DDOS4GW
    DX_ASFLAGS   += -dDOS4GW
    LIB_OS       = dos32
.ELIF $(USE_DOS32A)             # DOS32/A
    CFLAGS       += -bt=dos
    USE_DOS4GW   := 1
    USE_REALDOS  := 1
    LDFLAGS      += SYS DOS32A
    DX_CFLAGS    += -DDOS4GW
    DX_ASFLAGS   += -dDOS4GW
    LIB_OS       = dos32
.ELSE                           # Use DOS4GW
    CFLAGS       += -bt=dos
    USE_DOS4GW   := 1
    USE_REALDOS  := 1
    LDFLAGS      += SYS DOS4G
    DX_CFLAGS    += -DDOS4GW
    DX_ASFLAGS   += -dDOS4GW
    LIB_OS       = dos32
.END

# Disable linking to default C runtime library and PM library

.IF $(NO_RUNTIME)
LDFLAGS          += OP nodefaultlibs
DEFLIBS          :=
.ELSE

# Place to look for PM library files

.IF $(USE_SNAP)                 # Build 32 bit Snap app or dll
PMLIB           :=
.ELIF $(USE_WIN32)
.IF $(STKCALL)
PMLIB           := spm.lib,
.ELSE
PMLIB           := pm.lib,
.ENDIF
.ELIF $(USE_RTTARGET)
.IF $(STKCALL)
PMLIB           := spm.lib,
.ELSE
PMLIB           := pm.lib,
.ENDIF
.ELIF $(USE_OS232)
.IF $(STKCALL)
.IF $(USE_OS2GUI)
PMLIB           := spm_pm.lib,
.ELSE
PMLIB           := spm.lib,
.ENDIF
.ELSE
.IF $(USE_OS2GUI)
PMLIB           := pm_pm.lib,
.ELSE
PMLIB           := pm.lib,
.ENDIF
.ENDIF
.ELIF $(USE_QNX4)
.IF $(STKCALL)
PMLIB           := libspm.a,
.ELSE
PMLIB           := libpm.a,
.ENDIF
.ELIF $(USE_LINUX)
.IF $(STKCALL)
PMLIB           := spm.lib,
.ELSE
PMLIB           := pm.lib,
.ENDIF
.ELIF $(USE_TNT)
.IF $(STKCALL)
PMLIB           := tnt$(SL)spm.lib,
.ELSE
PMLIB           := tnt$(SL)pm.lib,
.ENDIF
.ELIF $(USE_X32)
.IF $(STKCALL)
PMLIB           := x32$(SL)spm.lib,
.ELSE
PMLIB           := x32$(SL)pm.lib,
.ENDIF
.ELSE
.IF $(STKCALL)
PMLIB           := dos4gw$(SL)spm.lib,
.ELSE
PMLIB           := dos4gw$(SL)pm.lib,
.ENDIF
.ENDIF
.ENDIF

# Define the base directory for library files

.IF $(CHECKED)
LIB_BASE_DIR    := $(SCITECH_LIB)$(SL)lib$(SL)debug
CFLAGS          += -DCHECKED=1
.ELSE
LIB_BASE_DIR    := $(SCITECH_LIB)$(SL)lib$(SL)release
.ENDIF

# Use X86 assembler code for this compiler
   USE_X86          := 1

# Define where to install library files
   LIB_BASE     := $(LIB_BASE_DIR)$(SL)$(LIB_OS)$(SL)$(WC_LIBBASE)
   LIB_DEST     := $(LIB_BASE)

# Define which file contains our rules

   RULES_MAK    := wc32.mk

