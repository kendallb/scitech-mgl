#############################################################################
#
#                   Copyright (C) 2004 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for zlib library.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the list of object files
#----------------------------------------------------------------------------

# Objects for building Binary Portable DLL
DLL_OBJS        = dllstart$O zlibinit$O

# ZLIB library sources
OBJECTS         = adler32$O compress$O crc32$O uncompr$O deflate$O \
                  trees$O zutil$O inflate$O inffast$O inftrees$O

.IF $(BUILD_DLL)
CFLAGS          += -I$(PRIVATE)\include\drvlib -I$(SCITECH)\include\drvlib -D__DRIVER__
ASFLAGS         += -d__DRIVER__
EXELIBS         = drvlib$L
.ELSE
LIBFILE         = $(LP)zlib$L
LIBCLEAN        = *.dll *.lib *.a
.ENDIF

#----------------------------------------------------------------------------
# Target to build the Binary Portable DLL target
#----------------------------------------------------------------------------

zlib.dll: $(DLL_OBJS) $(OBJECTS)

#----------------------------------------------------------------------------
# Target to build all Intel binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @build ow10-w32 zlib.dll -u BUILD_DLL=1 NO_RUNTIME=1 OPT=1
    $(CP) zlib.dll $(SCITECH)\drivers\zlib.bpd
    @dmake cleanexe

.PHONY db:
    @build_db ow10-w32 zlib.dll BUILD_DLL=1 NO_RUNTIME=1 DBG=1
    $(CP) zlib.dll $(PRIVATE)\drivers\zlib.bpd

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      := ../zlib;$(PRIVATE)/src/snap/common

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
