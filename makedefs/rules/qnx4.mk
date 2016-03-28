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

# Whether to link in real VBIOS library, or just the stub library

.IF $(USE_BIOS)
VBIOSLIB := -lvbios.lib
.ELSE
VBIOSLIB := -lvbstubs.lib
.END

# Require special privledges for SNAP programs (requires root access)

.IF $(NEEDS_IO)
_LDFLAGS     += -T1
.ENDIF

# Implicit generation rules for making object files from source files
%$O: %.c ;
.IF $(SHOW_ARGS)
    $(CC) $(_CFLAGS) $<
.ELSE
    @echo $(CC) -c $<
    +@$(CC) $(_CFLAGS) $< > /dev/null
.ENDIF

%$O: %$P ;
.IF $(SHOW_ARGS)
    $(CXX) $(_CFLAGS) $<
.ELSE
    @echo $(CXX) -c $<
    +@$(CXX) $(_CFLAGS) $< > /dev/null
.ENDIF

%$O: %$A ;
.IF $(SHOW_ARGS)
    $(AS) -o $@ $(_ASFLAGS) $<
.ELSE
    @echo $(AS) $<
    @$(AS) -o $@ $(_ASFLAGS) $<
.ENDIF

# Implicit rule for building a library file
%$L:     ;
.IF $(SHOW_ARGS)
    $(LIB) $(LIBFLAGS) -q $@ $&
.ELSE
    @echo $(LIB) $@
    +@$(LIB) $(LIBFLAGS) -q $@ $& > /dev/null
.ENDIF


# Implicit rule for building an executable file
%$E:     ;
.IF $(SHOW_ARGS)
    $(LD) $(_LDFLAGS) -o $@ $& $(EXELIBS) $(PMLIB) $(VBIOSLIB)
.ELSE
    @echo wlink $@
    +@$(LD) $(_LDFLAGS) -o $@ $& $(EXELIBS) $(PMLIB) $(VBIOSLIB) > /dev/null
.ENDIF
