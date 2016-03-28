#############################################################################
#
#                   Copyright (C) 1999 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for the FreeType library. Requires the
#               SciTech Software makefile definitions package to be
#               installed.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

CFLAGS          += -Iextend

.IF $(USE_X86)
DLL_OBJS        = dllstart$O ttinit$O
.ELSE
DLL_OBJS        = initseg$O ttinit$O
.ENDIF

OBJ_MAIN        = ttapi$O ttcache$O ttcalc$O ttcmap$O ttdebug$O \
                  ttfile$O ttgload$O ttinterp$O ttload$O \
                  ttmemory$O ttmutex$O ttobjs$O ttraster$O ttextend$O
OBJ_EXT         = ftxgasp$O ftxkern$O ftxpost$O ftxcmap$O ftxwidth$O
OBJECTS         = $(OBJ_MAIN) $(OBJ_EXT)

.IF $(BUILD_DLL)
.ELSE
OBJ_EXT        += ftxsbit$O ftxgsub$O ftxgdef$O ftxopen$O ftxgpos$O
LIBFILE         = $(LP)freetype$L
LIBCLEAN        = *.bpd *.lib *.a
.ENDIF

#----------------------------------------------------------------------------
# Target to build debug binary drivers (default)
#----------------------------------------------------------------------------

.PHONY db:
    @+build_db $(SNAP_DRV_BUILD) freetype.bpd "BUILD_DLL=1" "DBG=1" $(MFLAGS)
    $(CP) freetype.bpd $(SCITECH)/drivers/$(ARCH)/freetype.bpd

#----------------------------------------------------------------------------
# Target to all binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @+build $(SNAP_DRV_BUILD) freetype.bpd -u "BUILD_DLL=1" "OPT=1"
    $(CP) freetype.bpd $(SCITECH)/drivers/$(ARCH)/freetype.bpd
    @dmake cleanexe

#----------------------------------------------------------------------------
# Target to build the Binary Portable DLL target
#----------------------------------------------------------------------------

freetype.bpd: $(DLL_OBJS) $(OBJECTS)

ftdump$E: ftdump$O fcommon$O $(LIBFILE)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(DLL_OBJS) $(OBJECTS)
DEPEND_SRC   	= extend;$(PRIVATE)/src/snap/common

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
