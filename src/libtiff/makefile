#############################################################################
#
#                   Copyright (C) 1996 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for the MGL version of the TIFF image
#               library.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

# Objects for building Binary Portable DLL
.IF $(USE_X86)
DLL_OBJS        = dllstart$O tiffinit$O
.ELIF $(USE_PPC_BE)
DLL_OBJS        = tiffinit$O
.ENDIF

# These objectfiles are included in libtiff.lib
OBJECTS         = tif_aux$O tif_close$O tif_codec$O tif_compress$O \
                  tif_dir$O tif_dirinfo$O tif_dirread$O tif_dirwrite$O \
                  tif_dumpmode$O tif_error$O tif_fax3$O tif_fax3sm$O \
                  tif_getimage$O tif_jpeg$O tif_flush$O tif_lzw$O \
                  tif_next$O tif_open$O tif_packbits$O tif_predict$O \
                  tif_print$O tif_read$O tif_swab$O tif_strip$O \
                  tif_thunder$O tif_tile$O tif_version$O tif_warning$O \
                  tif_write$O tif_zip$O

# Use Win32 file I/O and memory allocation for the time being on Win32

.IF $(USE_WIN32)
OBJECTS         += tif_win32$O
.ENDIF

.IF $(BUILD_DLL)
OBJECTS         += tif_bpd$O
.ELSE
LIBFILE         = $(LP)tiff$L
LIBCLEAN        = *.bpd *.lib *.a
.ENDIF
CFLAGS          += -DBSDTYPES

#----------------------------------------------------------------------------
# Target to build debug binary drivers (default)
#----------------------------------------------------------------------------

.PHONY db:
    @+build_db $(SNAP_DRV_BUILD) tiff.bpd "BUILD_DLL=1" "DBG=1" $(MFLAGS)
    $(CP) tiff.bpd $(SCITECH)/drivers/tiff.bpd

#----------------------------------------------------------------------------
# Target to all binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @+build $(SNAP_DRV_BUILD) tiff.bpd -u "BUILD_DLL=1" "OPT=1"
    $(CP) tiff.bpd $(PRIVATE)/snap/tiff.bpd
    @dmake cleanexe

#----------------------------------------------------------------------------
# Target to build the Binary Portable DLL target
#----------------------------------------------------------------------------

tiff.bpd: $(DLL_OBJS) $(OBJECTS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      = $(PRIVATE)/src/common

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
