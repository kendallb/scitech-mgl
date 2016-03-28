#############################################################################
#
#                   Copyright (C) 1996 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for the MGL version of the JPEG image
#               library.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

# Objects for building Binary Portable DLL
.IF $(USE_X86)
DLL_OBJS        = dllstart$O jpeginit$O
.ELSE
DLL_OBJS        = initseg$O jpeginit$O
.ENDIF

# library object files common to compression and decompression
COMOBJECTS      = jcomapi$O jutils$O jerror$O jmemmgr$O jmemmgl$O

# compression library object files
CLIBOBJECTS     = jcapimin$O jcapistd$O jctrans$O jcparam$O jdatadst$O      \
                  jcinit$O jcmaster$O jcmarker$O jcmainct$O jcprepct$O      \
                  jccoefct$O jccolor$O jcsample$O jchuff$O jcphuff$O        \
                  jcdctmgr$O jfdctfst$O jfdctflt$O jfdctint$O

# decompression library object files
DLIBOBJECTS     = jdapimin$O jdapistd$O jdtrans$O jdatasrc$O                \
                  jdmaster$O jdinput$O jdmarker$O jdhuff$O jdphuff$O        \
                  jdmainct$O jdcoefct$O jdpostct$O jddctmgr$O jidctfst$O    \
                  jidctflt$O jidctint$O jidctred$O jdsample$O jdcolor$O     \
                  jquant1$O jquant2$O jdmerge$O

# These objectfiles are included in libjpeg.lib
OBJECTS         = $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)

.IF $(BUILD_DLL)
.ELSE
LIBFILE         = $(LP)jpeg$L
LIBCLEAN        = *.bpd *.lib *.a
.ENDIF

#----------------------------------------------------------------------------
# Target to build debug binary drivers (default)
#----------------------------------------------------------------------------

.PHONY db:
    @+build_db $(SNAP_DRV_BUILD) jpeg.bpd "BUILD_DLL=1" "DBG=1" $(MFLAGS)
    $(CP) jpeg.bpd $(SCITECH)/drivers/$(ARCH)/jpeg.bpd

#----------------------------------------------------------------------------
# Target to build all binary drivers
#----------------------------------------------------------------------------

.PHONY mkdrv:
    @+build $(SNAP_DRV_BUILD) jpeg.bpd -u "BUILD_DLL=1" "OPT=1"
    $(CP) jpeg.bpd $(SCITECH)/drivers/$(ARCH)/jpeg.bpd
    @dmake cleanexe

#----------------------------------------------------------------------------
# Target to build the Binary Portable DLL target
#----------------------------------------------------------------------------

jpeg.bpd: $(DLL_OBJS) $(OBJECTS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_SRC      = $(PRIVATE)/src/snap/common

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
