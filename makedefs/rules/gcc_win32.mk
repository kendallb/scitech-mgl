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
# Descripton:   Rules makefile definitions, which define the rules used to
#               build targets. We include them here at the end of the
#               makefile so the generic project makefiles can override
#               certain things with macros (such as linking C++ programs
#               differently).
#
#############################################################################

# Take out PMLIB if we don't need to link with it

.IF $(NO_PMLIB)
PMLIB :=
.ENDIF

.IF $(USE_CXX_LINKER)
LD  := $(LDXX)
.ENDIF

# Implicit generation rules for making object files from source files
%$O: %.c ;
.IF $(SHOW_ARGS)
    $(CC) -c $(_CFLAGS:s/\/\\) $(<:s,/,\)
.ELSE
    @$(ECHO) $(CC) $(SHOW_CFLAGS:s/\/\\) $(<:s,/,\)
    @$(CC) -c $(_CFLAGS:s/\/\\) $(<:s,/,\)
.ENDIF

%$O: %$P ;
.IF $(SHOW_ARGS)
    $(CXX) -c $(_CFLAGS:s/\/\\) $(<:s,/,\)
.ELSE
    @$(ECHO) $(CXX) $(SHOW_CFLAGS:s/\/\\) $(<:s,/,\)
    @$(CXX) -c $(_CFLAGS:s/\/\\) $(<:s,/,\)
.ENDIF

%$O: %$A ;
.IF $(SHOW_ARGS)
    $(AS) -o $(_ASFLAGS:s/\/\\) $(<:s,/,\)
.ELSE
    @$(ECHO) $(AS) $(SHOW_ASFLAGS:s/\/\\) $(<:s,/,\)
    @$(AS) @$(mktmp -o $@ $(_ASFLAGS:s/\/\\)) $(<:s,/,\)
.ENDIF

# Implicit rule for building resource files
%$R: %.rc ; $(RC) $< $(RCFLAGS) -o $@

# Implicit rule for building a DLL
# TODO!
#%$D: ; +rclink $(LD) $(RC) $@ $(mktmp $(_LDFLAGS) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS))

# Implicit rule for building a library file
%$L:     ;
.IF $(SHOW_ARGS)
    $(LIB) $(LIBFLAGS) $@ $&
.ELSE
    @$(ECHO) $(LIB) $@
    @$(LIB) $(LIBFLAGS) $@ @$(mktmp $(&:s/\/\\)\n)
.ENDIF

# Implicit rule for building an executable file
%$E:     ;
.IF $(SHOW_ARGS)
    $(LD) $(_LDFLAGS) -o $@ $& $(PMLIB) $(EXELIBS) $(DEFLIBS) -lm
.ELSE
    @$(ECHO) ld $@
    @$(LD) $(_LDFLAGS) -o $@ @$(mktmp $(&:s/\/\\) $(PMLIB) $(EXELIBS) $(DEFLIBS) -lm)
.ENDIF

