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

# Turn on pre-compiled headers as neccessary
.IF $(PRECOMP_HDR)
   _CFLAGS       += -YX"$(PRECOMP_HDR)"
.ENDIF

# Turn on runtime type information as necessary
.IF $(USE_RTTI)
    _CFLAGS      += /GR
.ENDIF

# Turn on C++ exception handling as necessary
.IF $(USE_CPPEXCEPT)
    _CFLAGS      += /GX
.ENDIF

# Take out PMLIB if we don't need to link with it

.IF $(NO_PMLIB)
PMLIB :=
.ENDIF

# Implicit generation rules for making object files
%$O: %.c ; $(CC) /nologo @$(mktmp $(_CFLAGS:s/\/\\)) /c $(<:s,/,\)
%$O: %$P ; $(CC) /nologo @$(mktmp $(_CFLAGS:s/\/\\)) /c $(<:s,/,\)
%$O: %$A ; $(AS) @$(mktmp -o $@ $(_ASFLAGS:s/\/\\)) $(<:s,/,\)

# Implicit rule to compile .S assembler files.
# Uses a pre-processor to produce NASM code (like Watcom C).
%$O: %$S ;
    @cl /nologo /DNASM_ASSEMBLER /EP $(SFLAGS:s/\/\\) $(<:s,/,\) > $(*:s,/,\).asm
    nasm @$(mktmp -f obj -o $@) $(*:s,/,\).asm $(_ASFLAGS)
    @$(RM) -S $(mktmp $(*:s,/,\).asm)

# Implicit rule for building resource files
%$R: %.rc ; $(RC) $(RCFLAGS) -r $<

# Implicit rules for building NT kernel device drivers

%.sys: ;
    $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS) /ENTRY:DriverEntry@8)
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
.ENDIF
.ENDIF

# Implicit rule for building a DLL using a response file
.IF $(IMPORT_DLL)
.ELSE
.IF $(USE_NTDRV)
%$D: ;
    $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS) /ENTRY:DrvEnableDriver@12)
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
.ENDIF
.ENDIF
.ELSE
.IF $(USE_WINCE)
%$D: ;
    $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS) /entry:_DllMainCRTStartup)
.ELSE
.IF $(NO_RUNTIME)
%$D: ; $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS))
.ELSE
%$D: ;
.IF $(NO_MKDEF)
.ELSE
    makedef -v $*
.ENDIF
    $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS))
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
.ENDIF
.ENDIF
.ENDIF
.ENDIF
.ENDIF
.ENDIF

# Implicit rule for building a library file using response file. Note that
# we use a special .VCD file that contains the EXPORT definitions for the
# Microsoft compiler, since the LIB utility automatically adds leading
# underscores to exported functions.
.IF $(IMPORT_DLL)
%$L: ;
.IF $(NO_MKDEF)
.ELSE
    makedef -v $(?:b)
.ENDIF
    @$(RM) $@
    $(ILIB) $(ILIBFLAGS) /DEF:$(?:b).def /OUT:$@
.ELSE
%$L: ;
    @$(RM) $@
    $(LIB) $(LIBFLAGS) /out:$@ @$(mktmp $(&:t"\n")\n)
.ENDIF

# Implicit rule for building an executable file using response file
.IF $(USE_WIN32)
%$E: ;
    $(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS))
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
.ENDIF
.ENDIF
.ELSE
%$E: ;
    @$(LD) /nologo @$(mktmp $(_LDFLAGS:s/\/\\) /Fe$@ $(&:t"\n"s/\/\\) $(PMLIB) $(EXELIBS) $(DEF_LIBS) $(LDENDFLAGS))
.IF $(DOSSTYLE)
    @markphar $@
.ENDIF
.ENDIF
