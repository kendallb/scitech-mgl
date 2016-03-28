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

# Extra C flags based on makefile settings

.IF $(STOP_ON_WARN)
    _CFLAGS     += -we
.ENDIF
.IF $(NO_DEFAULT_LIBS)
    _CFLAGS     += -zl
.ENDIF
.IF $(UNSIGNED_CHAR)
.ELSE
    _CFLAGS     += -j
.ENDIF

# Compile and link against C runtime library DLL if desired. We have this
# option in here so that we can allow makefiles to turn this on and off
# as desired.
.IF $(DLL_CRTL)
   _CFLAGS       += -br
.END

# Use a larger stack during linking if requested, or use a default stack
# of 200k. The usual default stack provided by Watcom C++ is *way* to small
# for real 32 bit code development. We also need a *huge* stack for OpenGL
# software rendering also!
.IF $(USE_QNX4)
    # Not necessary for QNX code.
.ELSE
.IF $(USE_LINUX)
    # Not necessary for Linux code.
.ELSE
.IF $(STKSIZE)
    _LDFLAGS     += OP STACK=$(STKSIZE)
.ELSE
    _LDFLAGS     += OP STACK=204800
.ENDIF
.ENDIF
.ENDIF

# Turn on runtime type information as necessary
.IF $(USE_RTTI)
    _CPFLAGS     += -xr
.ENDIF

# Turn on exception handling as necessary
.IF $(USE_CPPEXCEPT)
    _CPFLAGS     += -xs
.ENDIF

# Optionally turn on pre-compiled headers
.IF $(PRECOMP_HDR)
    _CFLAGS      += -fh
.ENDIF

.IF $(USE_QNX)
# Whether to link in real VBIOS library, or just the stub library
.IF $(USE_BIOS)
VBIOSLIB := vbios.lib,
.ELSE
VBIOSLIB := vbstubs.lib,
.END
# Require special privledges for SNAP programs (requires root access)
.IF $(NEEDS_IO)
_LDFLAGS     += OP PRIV=1
.ENDIF
.ENDIF

# Convert slashes in path names for the host platform

.IF $(UNIX_HOST)
TARGETS = $(<:s,\,/)
.ELSE
TARGETS = $(<:s,/,\)
.ENDIF

# Implicit generation rules for making object files
%$O: %.c ;
.IF $(SHOW_ARGS)
    $(CC) $(_CFLAGS) -fo=$O $(TARGETS)
.ELSE
    @$(ECHO) $(CC) $(SHOW_CFLAGS) $(TARGETS)
    @$(CC) $(_CFLAGS) -fo=$O $(TARGETS)
.ENDIF

%$O: %$P ;
.IF $(SHOW_ARGS)
    $(CPP) $(_CFLAGS) $(_CPFLAGS) -fo=$O $(TARGETS)
.ELSE
    @$(ECHO) $(CPP) $(SHOW_CFLAGS) $(SHOW_CPFLAGS) $(TARGETS)
    @$(CPP) $(_CFLAGS) $(_CPFLAGS) -fo=$O $(TARGETS)
.ENDIF

.IF $(USE_WASM)

%$O: %$A ;
    wasm -q -fo=$@ $(_ASFLAGS) $(TARGETS)

%$O: %$A ;
.IF $(SHOW_ARGS)
    wasm -q -fo=$@ $(_ASFLAGS) $(TARGETS)
.ELSE
    @$(ECHO) wasm $(TARGETS)
    @wasm -q -fo=$@ $(_ASFLAGS) $(TARGETS)
.ENDIF


.ELSE

%$O: %$A ;
.IF $(SHOW_ARGS)
    $(AS) -o $@ $(_ASFLAGS) $(TARGETS)
.ELSE
    @$(ECHO) $(AS) $(SHOW_ASFLAGS) $(TARGETS)
    @$(AS) -o $@ $(_ASFLAGS) $(TARGETS)
.ENDIF

.ENDIF

%$O: %.tsm ;
.IF $(SHOW_ARGS)
    $(TASM) $(TASMFLAGS) $(TARGETS)
.ELSE
    @$(ECHO) $(TASM) $(TARGETS)
    @$(TASM) $(TASMFLAGS) $(TARGETS)
.ENDIF

# Implit rule to compile .S assembler files. The first version
# uses GAS directly and the second uses the GNU pre-processor to
# produce NASM code.

.IF $(USE_GAS)
.IF $(HAVE_WC11)
%$O: %$S ;
    $(GAS) -c @$(mktmp $(GAS_FLAGS:s/\/\\)) $(<:s,/,\)
.ELSE
# Black magic to build asm sources with Watcom 10.6 (requires sed)
%$O: %$S ;
    $(GAS) -c @$(mktmp $(GAS_FLAGS:s/\/\\)) $(<:s,/,\)
    wdisasm \\ -a $(*:s,/,\).o > $(*:s,/,\).lst
    sed -e "s/\.text/_TEXT/; s/\.data/_DATA/; s/\.bss/_BSS/; s/\.386/\.586/; s/lar *ecx,cx/lar ecx,ecx/" $(*:s,/,\).lst > $(*:s,/,\).asm
    wasm \\ $(WFLAGS) -zq -fr=nul -fp3 -fo=$@ $(*:s,/,\).asm
    @$(RM) $*.o
    @$(RM) $*.lst
    @$(RM) $*.asm
.ENDIF
.ELSE
%$O: %$S ;
    @+$(CC) $(_CFLAGS) -DNASM_ASSEMBLER -p -za $< > $*.asm
.IF $(SHOW_ARGS)
    nasm -f obj -o $@ $(_ASFLAGS) $*.asm
.ELSE
    @$(ECHO) nasm $(SHOW_ASFLAGS) $*.asm
    @nasm -f obj -o $@ $(_ASFLAGS) $*.asm
.ENDIF
    @$(RM) $*.asm
.ENDIF

# Implicit rule for building resource files
%$R: %.rc ; $(RC) $(RCFLAGS) -r $< -fo=$@

# Implicit rule for building a DLL using a response file
.IF $(IMPORT_DLL)
.ELSE
.IF $(USE_OS232)
%$D: ;
    @trimlib $(mktmp SYS os2v2 dll $(_LDFLAGS:s/\/\\) OP quiet,impl=$(LIBFILE) LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",\n":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk $*.ref
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_SNAP_DRV)
%.bpd: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet,impl=$*.lib option osname='SNAP binary portable' format windows nt dll LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",\n":s/\/\\)\nLIBR $(DEFLIBS),$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_WIN32)
%$D: ;
.IF $(NO_RUNTIME)
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet,impl=$*.lib SYS format windows nt dll LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",\n":s/\/\\)\nLIBR $(PMLIB)$(DEFLIBS)$(EXELIBS:t",")) $*.lnk
.ELSE
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet,impl=$(LIBFILE) SYS nt_dll LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",\n":s/\/\\)\nLIBR $(PMLIB)$(DEFLIBS)$(EXELIBS:t",")\n) $*.lnk $*.ref
.ENDIF
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELSE
%$D: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS win386 LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $*.rex\nF $(&:t",\n":s/\/\\)\nLIBR $(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
    wbind $* -d -q -n
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ENDIF
.ENDIF

# Implicit rule for building a library file using response file (DLL import library
# is built along with the DLL itself).
.IF $(BUILD_DLL)
.ELIF $(IMPORT_DLL)
%$L: ;
    @$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ +$?
.ELSE
%$L: ;
    @$(RM) $@
    $(LIB) $(LIBFLAGS) $@ @$(mktmp,$*.rsp +$(&:t"\n+":s/\/\\)\n)
.ENDIF

# Implicit rule for building an executable file using response file
.IF $(USE_X32)
%$E: ;
    @trimlib $(mktmp OP quiet\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk
    $(LD) $(_LDFLAGS:s/\/\\) @$*.lnk
    x32fix $@
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_OS232)
.IF $(USE_OS2GUI)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS os2v2_pm LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.IF $(LXLITE)
    lxlite $@
.ENDIF
.ELSE
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS os2v2 $(OS2_FS) LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.IF $(LXLITE)
    lxlite $@
.ENDIF
.ENDIF
.ELIF $(USE_SNAP)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS snap LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(DEFLIBS)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_WIN32)
.IF $(WIN32_GUI)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS nt_win LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(DEFLIBS)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELSE
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS nt LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(DEFLIBS)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ENDIF
.ELIF $(USE_RTTARGET)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS nt LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(DEFLIBS)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) $(RC) $@ $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_WIN386)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet SYS win386 LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $*.rex\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk
    rclink $(LD) wbind $*.rex $*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELIF $(USE_TNT)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet LIBPATH $(USE_LIB_PATH:s/\/\\)\nN $@\nF $(&:t",":s/\/\\)\nLIBR dosx32.lib,tntapi.lib,$(PMLIB)$(EXELIBS:t",")) $*.lnk
    $(LD) @$*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.IF $(DOSSTYLE)
    @markphar $@
.ENDIF
.ELIF $(USE_QNX4)
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(VBIOSLIB)$(EXELIBS:t",")) $*.lnk
    @+if exist $*.exe attrib -s $*.exe > NUL
    $(LD) @$*.lnk
    @attrib +s $*.exe
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ELSE
%$E: ;
    @trimlib $(mktmp $(_LDFLAGS:s/\/\\) OP quiet LIBPATH $(USE_LIB_PATH:s/\/\\)\n\nN $@\nF $(&:t",":s/\/\\)\nLIBR $(PMLIB)$(EXELIBS:t",")) $*.lnk
    $(LD) @$*.lnk
.IF $(LEAVE_LINKFILE)
.ELSE
    @$(RM) *.lnk
.ENDIF
.ENDIF

