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
# Descripton:   Common makefile targets used by all SciTech Software
#               makefiles. This file includes targets for cleaning the
#               current directory, and maintaining the source files with
#               RCS.
#
#############################################################################

# Override global OpenGL includes when compiling against SNAP version

.IF $(DONT_USE_SNAP_OPENGL)
_DEPEND_INC = $(DEPEND_INC)
.ELSE
CFLAGS      += -I$(SCITECH)\include\snapgl
_DEPEND_INC = $(SCITECH)\include\snapgl;$(DEPEND_INC)
.ENDIF

# Define where to install all compiled DLL files

.IF $(CHECKED)
DLL_DEST    := $(SCITECH_LIB)\redist\debug
.ELSE
DLL_DEST    := $(SCITECH_LIB)\redist\release
.ENDIF

# Target to build the library and DLL file if specified

.IF $(LIBFILE)

lib: $(LIBFILE)

.IF $(DLLFILE)

# Build and install a DLL file, or simply build import library and install

.IF $(BUILD_DLL)

$(DLLFILE): $(OBJECTS)
$(LIBFILE): $(DLLFILE)
install: $(LIBFILE) $(DLLFILE)
    $(INSTALL) $(LIBFILE) $(LIB_DEST)$(LIB_EXTENDER)
    $(INSTALL) $(DLLFILE) $(DLL_DEST)
.IF $(USE_SOFTICE)
    $(INSTALL) $(DLLFILE:s/.dll/.nms) $(DLL_DEST)
.ENDIF
.ELSE

$(LIBFILE): $(DLL_DEST)\$(DLLFILE)
install: $(LIBFILE)
    $(INSTALL) $(LIBFILE) $(LIB_DEST)$(LIB_EXTENDER)

.ENDIF
.ELSE

.IF $(BUILD_SO)

.IF $(VERSION)
# Build and install a Unix shared library. We manually create the
# shared library symbolic links as necessary

$(LIBFILE): $(OBJECTS)
install: $(LIBFILE)
    $(INSTALL) $(LIBFILE) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSION)
    @$(SYMLINK) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSION) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE)
    @$(SYMLINK) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSION) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSIONMAJ)
    @$(SYMLINK) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSION) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE).$(VERSIONMAJ).$(VERSIONMID)

.ELSE
# Build and install a Unix shared library without version numbering

$(LIBFILE): $(OBJECTS)
install: $(LIBFILE)
    $(INSTALL) $(LIBFILE) $(LIB_DEST)$(LIB_EXTENDER)/$(LIBFILE)

.ENDIF

.ELSE

# Build and install a normal library file

.IF $(USE_DLL)
.ELSE
$(LIBFILE): $(OBJECTS)
install: $(LIBFILE)
    $(INSTALL) $(LIBFILE) $(LIB_DEST)$(LIB_EXTENDER)
.ENDIF
.ENDIF
.ENDIF
.ENDIF

# Build and install a VxD file, including debug information

.IF $(VXDFILE)
$(VXDFILE:s/.vxd/.dll): $(OBJECTS)
$(VXDFILE): $(VXDFILE:s/.vxd/.dll)
install: $(VXDFILE)
    $(INSTALL) $(VXDFILE) $(DLL_DEST)
.IF $(DBG)
    $(INSTALL) $(VXDFILE:s/.vxd/.nms) $(DLL_DEST)
.ENDIF
.ENDIF

# Clean up directory removing all files not needed to make the library.

__CLEAN_FILES := *.obj *.o *.sym *.bak *.tdk *.swp *.map *.err *.csm *.lib *.aps *.nms *.sys
__CLEAN_FILES += *.~* *.td *.tr *.tr? *.td? *.rws *.res *.exp *.ilk *.pdb *.pch *.a bcc32.*
__CLEAN_FILES += $(LIBCLEAN)
__CLEANEXE_FILES := $(__CLEAN_FILES) *$E *.drv *.rex *.dll *.bpd *.vxd *.nms *.pel *.smf *.so.* *.so

.PHONY clean:
    @$(RM) -f -S $(mktmp $(__CLEAN_FILES:t"\n"))

.PHONY cleanexe:
    @$(RM) -f -S $(mktmp $(__CLEANEXE_FILES:t"\n"))

# Define the source directories to find common files

.IF $(DEPEND_SRC)
.IF $(NO_SCITECH_COMMON)
_DEPEND_SRC     := $(DEPEND_SRC)
.ELSE
_DEPEND_SRC     := $(DEPEND_SRC);$(SCITECH)/src/common
.ENDIF
.ELSE
.IF $(NO_SCITECH_COMMON)
.ELSE
_DEPEND_SRC     := $(SCITECH)/src/common
.ENDIF
.ENDIF

# Set up source directories based on the depend_src variable, so that
# C, C++ and assembler files will be found correctly. Note that the
# use of .NULL in front of each source file path means that the
# current directory is always searched first. We cannot use .SOURCE
# without the suffixes, since that will then find .obj and .exe files
# in other directories on the source path.

_SRC_PATH       = $(_DEPEND_SRC:s,;, )
.SOURCE.c:         .NULL $(_SRC_PATH)
.SOURCE.cpp:       .NULL $(_SRC_PATH)
.SOURCE.asm:       .NULL $(_SRC_PATH)
.SOURCE.tsm:       .NULL $(_SRC_PATH)
.SOURCE.S:         .NULL $(_SRC_PATH)
.SOURCE.s_ppc:     .NULL $(_SRC_PATH)
.SOURCE.s_amd64:   .NULL $(_SRC_PATH)
.SOURCE.s_mips:    .NULL $(_SRC_PATH)
.SOURCE.rc:        .NULL $(_SRC_PATH)

# Create the include file dependencies using the MKUTIL makedep program if
# the list of dependent object files is defined

.IF $(DEPEND_OBJ)
depend:
    @$(RM) -f $(MAKEFILE_DEP)
.IF $(DEPEND_SRC)
.IF $(_DEPEND_INC)
    @makedep -a$(MAKEFILE_DEP) -r -s -I@$(mktmp $(_DEPEND_INC:s/\/\\)) -S@$(mktmp $(DEPEND_SRC:s/\/\\);$(SCITECH)/src/common) @$(mktmp $(DEPEND_OBJ:t"\n")\n)
.ELSE
    @makedep -a$(MAKEFILE_DEP) -r -s -S@$(mktmp $(DEPEND_SRC:s/\/\\);$(SCITECH)/src/common) @$(mktmp $(DEPEND_OBJ:t"\n")\n)
.ENDIF
.ELSE
.IF $(_DEPEND_INC)
    @makedep -a$(MAKEFILE_DEP) -r -s -I@$(mktmp $(_DEPEND_INC:s/\/\\)) -S@$(mktmp $(SCITECH)/src/common) @$(mktmp $(DEPEND_OBJ:t"\n")\n)
.ELSE
    @makedep -a$(MAKEFILE_DEP) -r -s -S@$(mktmp $(SCITECH)/src/common) @$(mktmp $(DEPEND_OBJ:t"\n")\n)
.ENDIF
.ENDIF
    @$(ECHO) Object file dependency information generated.
.ENDIF

# Convert path separators in CFLAGS, ASFLAGS and LDFLAGS for UNIX HOST
# platforms. We always use DOS/WIN style path separators in makefiles,
# and convert them as necessary to Unix format on Unix hosts.

.IF $(UNIX_HOST)
_CFLAGS     := $(CFLAGS:s,\,/)
_CPFLAGS    := $(CPFLAGS:s,\,/)
_ASFLAGS    := $(ASFLAGS:s,\,/)
_LDFLAGS    := $(LDFLAGS:s,\,/)
.ELSE
_CFLAGS     := $(CFLAGS)
_CPFLAGS    := $(CPFLAGS)
_ASFLAGS    := $(ASFLAGS)
_LDFLAGS    := $(LDFLAGS)
.ENDIF

# Include rule definitions for the compiler

.INCLUDE: "$(SCITECH)/makedefs/rules/$(RULES_MAK)"

# Include file dependencies

.INCLUDE .IGNORE: "$(MAKEFILE_DEP)"

