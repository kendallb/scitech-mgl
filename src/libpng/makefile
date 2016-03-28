#############################################################################
#
#                   Copyright (C) 1999 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for libpng library.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the list of object files
#----------------------------------------------------------------------------

# Objects for building Binary Portable DLL
.IF $(USE_X86)
DLL_OBJS        = dllstart$O pnginit$O
.ELSE
DLL_OBJS        = initseg$O pnginit$O
.ENDIF

# PNG library sources
PNG_OBJS        = png$O pngset$O pngget$O pngrutil$O pngtrans$O pngwutil$O \
                  pngmem$O pngpread$O pngread$O pngerror$O pngwrite$O \
                  pngrtran$O pngwtran$O pngrio$O pngwio$O

# ZLIB library sources
ZLIB_OBJS       = adler32$O compress$O crc32$O uncompr$O deflate$O \
                  trees$O zutil$O inflate$O inffast$O inftrees$O

# These objectfiles are included in png.lib
OBJECTS         = $(PNG_OBJS) $(ZLIB_OBJS)

CFLAGS          += -I..\zlib

# Uncomment to get logging in PNG code
#CFLAGS          += -DPNG_DEBUG=3 -DPNG_DEBUG_FILE=png_err_stream

.IF $(BUILD_DLL)
.ELSE

# When compiling as a static link library we need to compile in the default
# error handler that most applications expect. If we don't do this, the
# jmpbuf structure (which is compiler runtime library dependant in size)
# will be *different* and will cause the library to work incorrectly. This
# is bad design IMHO for the PNG library since the interface is not
# binary portable.

CFLAGS          += -DPNG_USE_DEFAULT_ERROR_HANDLER
LIBFILE         = $(LP)png$L
LIBCLEAN        = *.bpd *.lib *.a
EXELIBS         = $(LP)png$L
.ENDIF

#----------------------------------------------------------------------------
# Target to build debug binary drivers (default)
#----------------------------------------------------------------------------

.PHONY db:
    @+build_db $(SNAP_DRV_BUILD) png.bpd "BUILD_DLL=1" "DBG=1" $(MFLAGS)
    $(CP) png.bpd $(SCITECH)/drivers/$(ARCH)/png.bpd

#----------------------------------------------------------------------------
# Target to all binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @+build $(SNAP_DRV_BUILD) png.bpd -u "BUILD_DLL=1" "OPT=1"
    $(CP) png.bpd $(SCITECH)/drivers/$(ARCH)/png.bpd
    @dmake cleanexe

#----------------------------------------------------------------------------
# Target to build the Binary Portable DLL target
#----------------------------------------------------------------------------

png.bpd: $(DLL_OBJS) $(OBJECTS)

.IF $(TEST_BPD)
pngtest$E: pngtest$O pnglib$O _png_exp$O
.ELSE
pngtest$E: pngtest$O
.ENDIF

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      := ../zlib;$(PRIVATE)/src/snap/common

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
