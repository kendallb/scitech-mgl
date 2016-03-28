#############################################################################
#
#                Copyright (C) 1991-2004 SciTech Software, Inc.
#                           All rights reserved.
#
# Descripton:   Generic makefile for the UnZIP program using DMAKE.
#
#############################################################################

.IMPORT .IGNORE: NO_ASM

# Disable SciTech common source modules

NO_SCITECH_COMMON   := 1
NO_PMLIB            := 1

# Define OS specific modules

.IF $(USE_WIN32)
CFLAGS      += -DWIN32
OS_OBJ      = win32$O nt$O
DEPEND_SRC  :=  win32
.ELIF $(USE_OS232)
CFLAGS      += -DOS2
OS_OBJ      = os2$O os2acl$O
DEPEND_SRC  :=  os2
.ELIF $(USE_QNX)
OS_OBJ      = unix$O
DEPEND_SRC  :=  unix
.ELIF $(USE_LINUX)
OS_OBJ      = unix$O
DEPEND_SRC  :=  unix
.ELSE
CFLAGS      += -DMSDOS
OS_OBJ      = msdos$O
DEPEND_SRC  :=  msdos
.ENDIF

# Determine if we should compile the assembly version of the CRC code
.IF $(USE_X86)
.ELSE
NO_ASM = 1
.ENDIF

.IF $(NO_ASM)
CRC_OBJ     = crc32$O
CFLAGS      += -DNO_ASM
.ELSE
CRC_OBJ     = crc_i386$O
CFLAGS      += -DASM_CRC
.ENDIF

# Generic object file lists

UNZIP_OBJ   = unzip$O $(CRC_OBJ) crctab$O crypt$O envargs$O explode$O \
              extract$O fileio$O globals$O inflate$O list$O match$O \
              process$O ttyio$O unreduce$O unshrink$O zipinfo$O

SFX_OBJ     = unzip$O $(CRC_OBJ) crctab$O crypt$O extract$O fileio$O \
              globals$O inflate$O match$O process$O ttyio$O

# Targets to compile the executeable programs or library

.IF $(BUILD_SFX)
CFLAGS      += -DSFX
unzipsfx.exe:   $(SFX_OBJ) $(OS_OBJ)
.ELIF $(SFX_LIBRARY)
CFLAGS      += -DSFX -DSFX_LIBRARY
OBJECTS         = $(SFX_OBJ) $(OS_OBJ)
LIBFILE         = $(LP)unzipsfx$L
.ELSE
unzip.exe:	    $(UNZIP_OBJ) $(OS_OBJ)
.ENDIF

# Define object file dependencies

DEPEND_OBJ      = $(UNZIP_OBJ) $(SFX_OBJ) $(OS_OBJ)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

