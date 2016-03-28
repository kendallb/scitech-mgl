#############################################################################
#
#                   Copyright (C) 2004 SciTech Software.
#                           All rights reserved.
#
# Descripton:   Makefile to build the Portable Binary loader tests
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

DLL_OBJS        = testbpd$O tests$O
.IF $(USE_X86)
DLL_OBJS       += dllstart$O
.ELSE
DLL_OBJS       += initseg$O
.ENDIF
.IF $(USE_X86_64)
EXE_OBJS        = ldrtest$O elfldr64$O
.ELSE
EXE_OBJS        = ldrtest$O elfloadr$O peloader$O
.ENDIF
.IF $(USE_X86)
EXE_OBJS       += _peloadr$O
.ENDIF

.IF $(BUILD_DLL)
.ELSE
OBJECTS         = $(EXE_OBJS)
LIBFILE         = bogus$L
EXELIBS         = $(LL)n_ga$(LE)
.ENDIF
LIBCLEAN        = *.bpd *.lib *.a

all: ldrtest$E db

#----------------------------------------------------------------------------
# Target to build debug binary drivers (default)
#----------------------------------------------------------------------------

.PHONY db:
    @+build_db $(SNAP_DRV_BUILD) test.bpd "BUILD_DLL=1" "DBG=1" $(MFLAGS)

#----------------------------------------------------------------------------
# Target to build all Intel binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @+build $(SNAP_DRV_BUILD) test.bpd -u "BUILD_DLL=1" "OPT=1"
    @dmake cleanexe

#----------------------------------------------------------------------------
# Sample test programs
#----------------------------------------------------------------------------

ldrtest$E:  $(EXE_OBJS)

#----------------------------------------------------------------------------
# Default target to build the Portable binary DLL target
#----------------------------------------------------------------------------

test.bpd: $(DLL_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      = $(PRIVATE)/src/snap/common;$(PRIVATE)/src/snap/graphics

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

