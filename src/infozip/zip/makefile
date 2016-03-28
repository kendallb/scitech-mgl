#############################################################################
#
#                Copyright (C) 1991-2004 SciTech Software, Inc.
#                           All rights reserved.
#
# Descripton:   Generic makefile for the ZIP program using DMAKE.
#
#############################################################################

.IMPORT .IGNORE: NO_ASM

# Disable SciTech common source modules

NO_SCITECH_COMMON   := 1
NO_PMLIB            := 1

# Define OS specific modules

.IF $(USE_WIN32)
CFLAGS      += -DWIN32
OS_OBJ      = win32$O nt$O win32zip$O
DEPEND_SRC  :=  win32
.ELIF $(USE_OS232)
CFLAGS      += -DOS2
OS_OBJ      = os2$O os2acl$O os2zip$O
DEPEND_SRC  :=  os2
.ELIF $(USE_QNX)
CFLAGS      += -DUNIX -DHAVE_DIRENT_H -DHAVE_TERMIOS_H -DNO_MKTEMP
OS_OBJ      = unix$O
DEPEND_SRC  :=  unix
.ELIF $(USE_LINUX)
CFLAGS      += -DUNIX -DNO_MKTEMP
OS_OBJ      = unix$O
DEPEND_SRC  :=  unix
.ELSE
CFLAGS      += -DDOS
OS_OBJ      = msdos$O
DEPEND_SRC  :=  msdos
.ENDIF

# Determine if we should compile the assembly version of the CRC code

.IF $(NO_ASM)
ASM_OBJ     = crc32$O
CFLAGS      += -DNO_ASM
.ELSE
ASM_OBJ     = match32$O crc_i386$O
CFLAGS      += -DASM_CRC
.ENDIF

# Generic object file lists

ZIP_OBJ     = zip$O crypt$O ttyio$O trees$O zipup$O util$O zipfile$O fileio$O \
              deflate$O globals$O crctab$O $(ASM_OBJ)

#OBJU1 = $(O)zipfile_.obj $(O)fileio_.obj $(O)util_.obj $(O)globals.obj
#OBJ_U = $(OBJU1) $(O)win32_.obj
#OBJC  = $(O)zipcloak.obj $(O)crctab.obj $(O)crypt_.obj $(O)ttyio.obj $(OBJ_U)
#OBJN  = $(O)zipnote.obj $(OBJ_U)
#OBJS  = $(O)zipsplit.obj $(OBJ_U)

# Targets to compile the executeable programs

all: zip.exe #zipnote.exe zipcloak.exe zipsplit.exe

zip.exe:	    $(ZIP_OBJ) $(OS_OBJ)

# Define object file dependencies

DEPEND_OBJ      = $(ZIP_OBJ) $(OS_OBJ)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

