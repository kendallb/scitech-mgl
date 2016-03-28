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
#               Common startup script that defines all variables common to
#               all startup scripts. These define the DMAKE runtime
#               environment and the values are dependant on the version of
#               DMAKE in use.
#
#############################################################################

# Disable warnings for macros redefined here that were given
# on the command line.
__.SILENT       := $(.SILENT)
.SILENT         := yes

# Import enivornment variables that we use common to all compilers
.IMPORT .IGNORE : TEMP SHELL COMSPEC INCLUDE LIB SCITECH PRIVATE SCITECH_LIB
.IMPORT .IGNORE : DBG OPT OPT_SIZE SHW BETA USE_WIN32 FPU BUILD_DLL BUILD_FOR_DLL
.IMPORT .IGNORE : IMPORT_DLL WIN32_GUI USE_WIN16 CHECKED UNIX_TARGET
.IMPORT .IGNORE : OS2_SHELL SOFTICE_PATH MAX_WARN USE_SOFTICE
.IMPORT .IGNORE : USE_SNAP USE_X11 USE_LINUX STATIC_LIBS STATIC_LIBS_ALL LIBC
.IMPORT .IGNORE : SHOW_ARGS BOOT_STRAP_DMAKE USE_LIB_PATH USE_PPC_BE HOST_ARCH
.IMPORT .IGNORE : USE_X86 USE_X86_64 USE_MIPS_LE USE_MIPS_BE
   TMPDIR := $(TEMP)

# Determine if the host machine is a Windows/DOS or Unix box
.IF $(COMSPEC)
   WIN32_HOST   := 1
.ELSE
   UNIX_HOST    := 1
.ENDIF

# Determine host CPU architecture
.IF $(USE_PPC_BE)
   ARCH         := ppc-be
   ARCH_SHORT   := ppc
.ELIF $(USE_ALPHA)
   ARCH         := axp-be
   ARCH_SHORT   := axp
   ARCH_64BIT   := 1
.ELIF $(USE_X86_64)
   ARCH         := x86-64
   ARCH_SHORT   := amd64
   ARCH_64BIT   := 1
.ELIF $(USE_MIPS_LE)
   ARCH         := mips-le
   ARCH_SHORT   := mips
.ELIF $(USE_X86)
   ARCH         := x86
   ARCH_SHORT   := x86
.ELSE
   Error - unsupported platform
.ENDIF

.IF $(UNIX_HOST)

# Unix file separator
   SL           := /
   DIRSEPSTR    := /

# We use the Unix shell at all times
   SHELL        := /bin/sh
   SHELLFLAGS   := -c

.ELSE

# DOS/Windows style file separator
   SL           := \\
   DIRSEPSTR    := \\

# We use the DOS/Win/OS2 style shell at all times
   SHELL        := $(COMSPEC)
   GROUPSHELL   := $(SHELL)
   SHELLFLAGS   := $(SWITCHAR)c
   GROUPFLAGS   := $(SHELLFLAGS)
   SHELLMETAS   := *"?<>
.IF $(OS2_SHELL)
   GROUPSUFFIX  := .cmd
.ELSE
   GROUPSUFFIX  := .bat
.ENDIF
   DIVFILE       = $(TMPFILE:s,/,\)

.ENDIF

.IF $(UNIX_TARGET)
# Standard file suffix definitions
#
# NOTE: Linux/Unix does not require any extenion for executeable files, but you
#       can use an extension if you wish. We use the .exe extension for building
#       executeable files so that we can use implicit rules to make the
#       makefiles simpler and more portable between systems (exe also makes it
#       easier for cross-compile/debugging situations). When you install
#       the files to a local bin directory, you will probably want to remove
#       the .exe extension.
   L            := .a       # Libraries
   E            := .exe     # Executables for glibc
   O            := .o       # Objects
   A            := .asm     # Assembler sources
   S            := .S       # GNU assembler sources
   P            := .cpp     # C++ sources
   D            := .so      # Shared libraries

# File prefix/suffix definitions. The following prefixes are defined, and are
# used primarily to abstract between the Unix style libXX.a naming convention
# and the DOS/Windows/OS2 naming convention of XX.lib.
   LP           := lib      # LP - Library file prefix (name of file on disk)
   LL           := -l       # Library link prefix (name of library on link command line)
   LE           :=          # Library link suffix (extension of library on link command line)

.ELSE
# Standard file DOS/Win/OS2 suffix definitions
   L            := .lib     # Libraries
   E            := .exe     # Executables
   D            := .dll     # Dynamic Link Library file
   O            := .obj     # Objects
   A            := .asm     # Assembler sources
   P            := .cpp     # C++ sources
   R            := .res     # Compiled resource file
   S            := .S       # Assyntax.h style assembler

# File prefix/suffix definitions. The following prefixes are defined, and are
# used primarily to abstract between the Unix style libXX.a naming convention
# and the DOS/Windows/OS2 naming convention of XX.lib.
   LP           :=          # LP - Library file prefix (name of file on disk)
   LL           :=          # Library link prefix (name of library on link command line)
   LE           := .lib     # Library link suffix (extension of library on link command line)

.ENDIF

# Standard Unix style shell commands. Since these do not exist on
# regular DOS/Win/OS2 installations we use our own '' versions
# instead. To boostrtap a new OS you may wish to use the regular
# unix versions.

.IF $(BOOT_STRAP_DMAKE)
   CP           := +cp
   MD           := mkdir
   RM           := +rm
   ECHO         := echo
.ELSE
   CP           := k_cp
   MD           := k_md
   RM           := k_rm
   ECHO         := k_echo
.ENDIF

# Definition of $(MAKE) macro for recursive makes.
   MAKE = $(MAKECMD) $(MFLAGS)

# Macro to install a library file
   INSTALL      := $(CP)

# DMAKE uses this recipe to remove intermediate targets
.REMOVE :; $(RM) -f $<

# Turn warnings back to previous setting.
.SILENT := $(__.SILENT)

# We dont use TABS in our makefiles
.NOTABS         := yes

# Set the name of the makefile dependencies file
MAKEFILE_DEP    := makefile.dep

# Choose the tool used to build SNAP modules
.IF $(USE_PPC_BE)
   SNAP_DRV_BUILD := gcc-drv
.ELIF $(USE_X86_64)
   SNAP_DRV_BUILD := gcc-drv
.ELIF $(USE_MIPS_LE)
   SNAP_DRV_BUILD := gcc-drv
.ELSE
   SNAP_DRV_BUILD := ow10-drv
.ENDIF
