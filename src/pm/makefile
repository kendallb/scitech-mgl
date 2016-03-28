#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2.0 as published by the Free
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
# Descripton:   Generic makefile for the PM library. Builds the library
#               file and all test programs.
#
#############################################################################

.IMPORT .IGNORE : DEBUG_AGP_DRIVER TEST_HARNESS DEBUG_SDDPMI DEBUG_MATROX
.IMPORT .IGNORE : AGP_TEST_HARNESS NO_PTHREAD

USE_CXX_LINKER  := 1

#----------------------------------------------------------------------------
# Add DOS extender dependant flags to command line
#----------------------------------------------------------------------------

CFLAGS          += $(DX_CFLAGS)
ASFLAGS         += $(DX_ASFLAGS)
.IF $(USE_LINUX)
.IF $(NO_PTHREAD)
CFLAGS          += -DNO_PTHREAD
.ENDIF
NO_PMLIB        := 1
.ENDIF

#----------------------------------------------------------------------------
# Include definitions specific for the target system
#----------------------------------------------------------------------------

.IF $(USE_VXD)

# Building for Win32 VxD (minimal PM library implementation)

LIBNAME         = pm
OBJECTS         = pm$O gztimer$O gcpuinfo$O mtrr$O fileio$O pcilib$O \
                  agp$O malloc$O vgastate$O gavxd$O _pm$O _mtrr$O \
                  _cpuinfo$O _int64$O _pcihelp$O
DEPEND_SRC      := vxd;common;codepage;tests

.ELIF $(USE_NTDRV)

# Building for NT device drivers (minimal PM library implementation)

LIBNAME         = pm
OBJECTS         = pm$O gztimer$O gcpuinfo$O mtrr$O mem$O irq$O int86$O \
                  stdio$O stdlib$O pcilib$O agp$O malloc$O vgastate$O gantdrv$O \
                  _pm$O _mtrr$O _cpuinfo$O _int64$O _pcihelp$O _irq$O
DEPEND_SRC      := ntdrv;common;codepage;tests

.ELIF $(USE_WINCE)

# Building for WinCE device drivers (minimal PM library implementation)

LIBNAME         = pm
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O mtrr$O \
                  pcilib$O agp$O malloc$O vgastate$O gacedrv$O \
                  _pm$O _mtrr$O _cpuinfo$O _int64$O _pcihelp$O
DEPEND_SRC      := cedrv;common;codepage;tests

.ELIF $(USE_WIN32)

# Building for Win32

CFLAGS          += -DUSE_OS_JOYSTICK
LIBNAME         = pm
OBJECTS         = pm$O gevent$O ddraw$O gztimer$O gcpuinfo$O pcilib$O \
                  agp$O malloc$O gawin32$O ntservc$O _joy$O \
                  _cpuinfo$O _int64$O _pcihelp$O
DEPEND_SRC      := win32;common;codepage;tests

.ELIF $(USE_OS232)

# Building for OS/2

.IF $(USE_OS2GUI)
LIBNAME         = pm_pm
.ELSE
LIBNAME         = pm
.ENDIF
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O mtrr$O pcilib$O \
                  agp$O malloc$O vgastate$O gaos2$O _pmos2$O _joy$O \
                  _cpuinfo$O _int64$O _pcihelp$O os2$(SL)dossctl$O
DEPEND_SRC      := os2;common;codepage;tests

.ELIF $(USE_QNX)

# Building for QNX

USE_BIOS        := 1
.IF $(USE_PHOTON)
LIBNAME         = pm_ph
.ELIF $(USE_X11)
LIBNAME         = pm_x11
.ELSE
LIBNAME         = pm
.ENDIF
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O mtrr$O pcilib$O \
                  agp$O malloc$O mtrrqnx$O unixio$O vgastate$O gaqnx$O \
                  _joy$O _mtrrqnx$O _cpuinfo$O _int64$O _pcihelp$O
DEPEND_SRC      := qnx;common;codepage;tests

# Indicate that this program needs I/O port access
NEEDS_IO        := 1

.ELIF $(USE_LINUX)

# Building for Linux

CFLAGS          += -DENABLE_MTRR
.IF $(NO_THREADS)
CFLAGS          += -DNO_THREADS
SHOW_CFLAGS     += -DNO_THREADS
.ENDIF
.IF $(USE_X11)
LIBNAME         = pm_x11
.ELSE
LIBNAME         = pm
.ENDIF
OBJECTS         = pm$O gevent$O mouse$O gztimer$O gcpuinfo$O pcilib$O \
                  agp$O malloc$O unixio$O vgastate$O galinux$O
.IF $(USE_X86)
OBJECTS         += _cpuinfo$O _int64$O _pcihelp$O
.ELIF $(USE_X86_64)
OBJECTS         += int64$O io$O
.ELIF $(USE_PPC_BE)
OBJECTS         += _int64$O io$O
.ELIF $(USE_MIPS_LE)
OBJECTS         += int64$O iomips$O
.ELSE
Error - unsupported platform
.ENDIF
DEPEND_SRC      := linux;common;codepage;tests;x11

# Make sure that the PM library is based at the same address for each process

.IF $(ARCH_64BIT)
PM_BASE_ADDRESS := 0x1F7F800000
.ELSE
PM_BASE_ADDRESS := 0x005F800000
.ENDIF
CFLAGS          += -DPM_BASE_ADDRESS=$(PM_BASE_ADDRESS)
.IF $(BUILD_SO)
LIBFLAGS        += -Wl,--section-start -Wl,.hash=$(PM_BASE_ADDRESS)
.ENDIF

.ELIF $(USE_UUU)

# Building for Unununium

CFLAGS          += -DNO_THREADS
SHOW_CFLAGS     += -DNO_THREADS
LIBNAME         = pm
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O pcilib$O \
                  agp$O malloc$O vgastate$O gauuu$O mtrr$O _mtrr$O _cpuinfo$O _joy$O _int64$O _pcihelp$O
DEPEND_SRC      := uuu;common;codepage;tests

.ELIF $(USE_BEOS)

# Building for BeOS GUI

LIBNAME         = pm
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O pcilib$O \
                  agp$O malloc$O vgastate$O gabeos$O _joy$O \
                  _cpuinfo$O _int64$O _pcihelp$O
DEPEND_SRC      := beos;common;codepage;tests

.ELIF $(USE_SMX32)

# Building for SMX

LIBNAME         = pm
OBJECTS         = pm$O pmsmx$O gevent$O gztimer$O gcpuinfo$O mtrr$O pcilib$O \
                  agp$O malloc$O vgastate$O gasmx$O _pm$O _pmsmx$O _mtrr$O \
                  _event$O _joy$O _cpuinfo$O _int64$O _pcihelp$O _lztimer$O
DEPEND_SRC      := smx;common;codepage;tests

.ELIF $(USE_RTTARGET)

# Building for RTTarget-32

LIBNAME         = pm
OBJECTS         = pm$O gevent$O gztimer$O gcpuinfo$O mtrr$O pcilib$O \
                  agp$O malloc$O vgastate$O gartt$O _joy$O \
                  _cpuinfo$O _int64$O _pcihelp$O pmrtos$O _pmrtos$O
DEPEND_SRC      := rttarget;common;codepage;tests

.ELSE

# Building for MSDOS

LIBNAME         = pm
OBJECTS         = pm$O pmdos$O gevent$O gztimer$O gcpuinfo$O mtrr$O \
                  agp$O malloc$O pcilib$O vgastate$O gados$O aados$O \
                  _pm$O _pmdos$O _mtrr$O _event$O _joy$O _pcihelp$O \
                  _cpuinfo$O _int64$O _lztimer$O _dma$O
DEPEND_SRC      := dos;common;codepage;tests

.ENDIF

# Object modules for keyboard code pages

OBJECTS         += us_eng$O

# Common object modules

OBJECTS         += common$O debug$O ddclib$O

# SNAP loader library object modules. Note that when compiling a test harness
# library we need to exclude the SNAP loader library.

.IF $(AGP_TEST_HARNESS)
TEST_HARNESS    := 1
CFLAGS          += -DAGP_TEST_HARNESS
SHOW_CFLAGS     += -DAGP_TEST_HARNESS
.IF $(BUILD_SO)
LIBS            += $(LL)n_agp$(LE)
.ENDIF
.ENDIF
.IF $(TEST_HARNESS)
CFLAGS          += -DTEST_HARNESS -DPMLIB
SHOW_CFLAGS     += -DTEST_HARNESS
.IF $(BUILD_SO)
LIBS            += $(LL)n_driver$(LE) $(LL)n_ref2d$(LE)
.IF $(DEBUG_MATROX)
LIBS            += $(LL)mgahal$(LE)
.ENDIF
.ELSE
LIBNAME         := pm_test
.ENDIF
.ELSE
OBJECTS         += galib$O gaoption$O aalib$O
.IF $(USE_X86)
OBJECTS         += _ga_imp$O _aa_imp$O
.ELIF $(USE_X86_64)
OBJECTS         += _ga_imp$O
.ELIF $(USE_PPC_BE)
OBJECTS         += _ga_imp$O
.ELIF $(USE_MIPS_LE)
OBJECTS         += _ga_imp$O
.ELSE
Error - unsupported platform
.ENDIF
.ENDIF

.IF $(BUILD_SO)
.IF $(NO_PTHREAD)
.ELSE
EXTRA_LIBS      := $(LL)pthread$(LE)
.ENDIF
.ENDIF

.IF $(DEBUG_SDDPMI)
CFLAGS          += -DDEBUG_SDDPMI
.ENDIF

# AGP library object modules

.IF $(DEBUG_AGP_DRIVER)
CFLAGS          += -DDEBUG_AGP_DRIVER
OBJECTS         += agplib$O
.ELSE
OBJECTS         += agplib$O libcimp$O
.IF $(USE_X86)
   OBJECTS      += _gatimer$O peloader$O
.IF $(CHECKED)
   OBJECTS      += _peloadr$O
.ENDIF
.ELIF $(USE_X86_64)
   OBJECTS      += gatimer$O elfldr64$O
.ELIF $(USE_PPC_BE)
   OBJECTS      += _gatimer$O elfloadr$O
.ELIF $(USE_MIPS_LE)
   OBJECTS      += gatimer$O elfloadr$O
.ELSE
   Error - unsupported platform
.ENDIF
.ENDIF

#----------------------------------------------------------------------------
# Name of library and generic object files required to build it
#----------------------------------------------------------------------------

.IF $(STKCALL)
LIBFILE         = s$(LP)$(LIBNAME)$L
.ELSE
LIBFILE         = $(LP)$(LIBNAME)$L
.ENDIF
LIBCLEAN        = *.lib *.a

#----------------------------------------------------------------------------
# Change destination for library file depending the extender being used. This
# is only necessary for DOS extender since the file go into a subdirectory
# in the normal library directory, one for each supported extender. Other
# OS'es put the file into the regular library directory, since there is
# only one per OS in this case.
#----------------------------------------------------------------------------

MK_PMODE        = 1

.IF $(TEST_HARNESS)
LIB_DEST        := $(LIB_BASE)
.ELIF $(USE_TNT)
LIB_DEST        := $(LIB_BASE)/tnt
.ELIF $(USE_DOS4GW)
LIB_DEST        := $(LIB_BASE)/dos4gw
.ELIF $(USE_X32)
LIB_DEST        := $(LIB_BASE)/x32
.ELIF $(USE_DPMI16)
LIB_DEST        := $(LIB_BASE)/dpmi16
.ELIF $(USE_DPMI32)
LIB_DEST        := $(LIB_BASE)/dpmi32
.ELIF $(USE_DOSX)
LIB_DEST        := $(LIB_BASE)/dosx
.END

#----------------------------------------------------------------------------
# Names of all executable files built
#----------------------------------------------------------------------------

.IF $(USE_REALDOS)
EXEFILES        = memtest$E biosptr$E video$E isvesa$E callreal$E       \
                  mouse$E tick$E irq$E key$E key15$E brk$E altbrk$E     \
                  critical$E altcrit$E rtc$E getch$E                    \
                  cpu$E timerc$E showpci$E uswc$E block$E test$E        \
                  hello$E bpdpath$E fatal$E timercpp$E
.ELSE
EXEFILES        = memtest$E video$E isvesa$E callreal$E getch$E \
                  cpu$E timerc$E showpci$E uswc$E block$E \
                  name$E shmem$E test$E hello$E bpdpath$E fatal$E timercpp$E
.ENDIF

.IF $(USE_LINUX)
EXEFILES       += memiobase$E save$E restore$E
.ENDIF

all: $(EXEFILES)

memtest$E:  memtest$O
name$E:     name$O
biosptr$E:  biosptr$O
video$E:    video$O
isvesa$E:   isvesa$O
mouse$E:    mouse$O
tick$E:     tick$O
irq$E:      irq$O
key$E:      key$O
key15$E:    key15$O
brk$E:      brk$O
altbrk$E:   altbrk$O
critical$E: critical$O
altcrit$E:  altcrit$O
callreal$E: callreal$O
rtc$E:      rtc$O
getch$E:    getch$O
cpu$E:      cpu$O
timerc$E:   timerc$O
showpci$E:  showpci$O
uswc$E:     uswc$O
block$E:    block$O
save$E:     save$O
restore$E:  restore$O
shmem$E:    shmem$O
test$E:     test$O
hello$E:    hello$O
bpdpath$E:  bpdpath$O
fatal$E:    fatal$O
memiobase$E:memiobase$O
timercpp$E: timercpp$O

# This dependency must be down here to get objects/libs in correct order for ld
$(EXEFILES): $(LIBFILE)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      := $(OBJECTS) memtest$O biosptr$O video$O isvesa$O mouse$O  \
                   tick$O key$O key$O brk$O altbrk$O critical$O altcrit$O   \
                   callreal$O getch$O timercpp$O

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
