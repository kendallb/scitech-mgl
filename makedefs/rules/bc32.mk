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

.IF $(USE_VXD)

# Implicit rule generation to build VxD's

%$O: %.c ;
    $(CC) @$(mktmp $(_CFLAGS:s/\/\\)) -c $(<:s,/,\)
    @$(VTOOLSD)\bin\segalias.exe -p $(VTOOLSD)\include\default.seg $@

%$O: %$P ;
    $(CC) @$(mktmp $(_CFLAGS:s/\/\\)) -c $(<:s,/,\)
    @$(VTOOLSD)\bin\segalias.exe -p $(VTOOLSD)\include\default.seg $@

%$O: %$A ;
    $(AS) @$(mktmp -o $@ $(_ASFLAGS:s/\/\\)) $(<:s,/,\)
    @$(VTOOLSD)\bin\segalias.exe -p $(VTOOLSD)\include\default.seg $@

%$O: %.tsm ;
    $(TASM) @$(mktmp $(TASMFLAGS:s/\/\\)) $(<:s,/,\)
    @$(VTOOLSD)\bin\segalias.exe -p $(VTOOLSD)\include\default.seg $@

%$L: ; $(LIB) $(LIBFLAGS) $@ @$(mktmp -+$(?:t" &\n-+")\n)

%.dll: ;
    @$(CP) $(mktmp EXPORTS\n_The_DDB @1) $*.def
    tlink32.exe @$(mktmp $(_LDFLAGS) -Tpd $(VTOOLSD:s/\/\\)\lib\icrtbc4.obj+\n$(&:s/\/\\)\n$*.dll\n$*.map\n$(DEF_LIBS:s/\/\\) $(PMLIB:s/\/\\) $(EXELIBS:s/\/\\)\n$*.def)
    @$(RM) -S $(mktmp $*.def)

# Extra rules to handle adding real mode init code to VxD

.IF $(REALINIT)
realinit.com: $(REALINIT)
    tlink /t $(REALINIT),realinit.com

$(VXDFILE): realinit.com
.ENDIF

%.vxd: %.dll ;
    @$(CP) $(mktmp DYNAMIC\nATTRIB ICODE INIT\nATTRIB LCODE LOCKED\nATTRIB PCODE PAGEABLE\nATTRIB SCODE STATIC\nATTRIB DBOCODE DEBUG\nMERGE ICODE INITDAT0 INITDATA) $*.pel
    @$(VTOOLSD)\bin\vxdver.exe $*.vrc $*.res
.IF $(REALINIT)
    @$(VTOOLSD)\bin\pele.exe -rrealinit.com -d -s $*.smf -c $*.pel -o $@ -k 400 $*.dll
.ELSE
    @$(VTOOLSD)\bin\pele.exe -d -s $*.smf -c $*.pel -o $@ -k 400 $*.dll
.ENDIF
    @$(VTOOLSD)\bin\sethdr.exe -n $* -x $@ -r $*.res
.IF $(DBG)
    $(NMSYM) /TRANS:source,package /SOURCE:$(VXDSOURCE) $*.smf
.ENDIF
    @$(RM) -S $(mktmp $*.pel)

.ELSE

# Implicit generation rules for making object files, libraries and exe's

%$O: %.c ; $(CC) @$(mktmp $(_CFLAGS:s/\/\\)) -c $(<:s,/,\)
%$O: %$P ; $(CC) @$(mktmp $(_CFLAGS:s/\/\\)) -c $(<:s,/,\)
%$O: %$A ; $(AS) @$(mktmp -o $@ $(_ASFLAGS:s/\/\\)) $(<:s,/,\)
%$O: %.tsm ; $(TASM) @$(mktmp $(TASMFLAGS:s/\/\\)) $(<:s,/,\)

# Implicit rule for building resource files
%$R: %.rc ; $(RC) $(RCFLAGS) -r $<

# Implicit rule for building a DLL using a response file
.IF $(IMPORT_DLL)
.ELSE
.IF $(NO_RUNTIME)
%$D: ; $(LD) $(mktmp $(_LDFLAGS) -Tpd -aa $(&:s/\/\\)\n$@\n$*.map\n$(EXELIBS)\n$*.def)
.ELSE
%$D: ;
    makedef $(@:b)
    $(LD) $(mktmp $(_LDFLAGS) -Tpd -aa c0d32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS:s/\/\\) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
    tdstrp32 $@
.ENDIF
.ENDIF
.ENDIF
.ENDIF

# Implicit rule for building a library file using response file
.IF $(BUILD_DLL)
%$L: ;
    @$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $?
.ELIF $(IMPORT_DLL)
%$L: ;
    @$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $?
.ELSE
%$L: ;
    @$(RM) $@
    $(LIB) $(LIBFLAGS) $@ @$(mktmp +$(&:t" &\n+")\n)
.ENDIF

# Implicit rule for building an executable file using response file

.IF $(USE_WIN32)
.IF $(WIN32_GUI)
%$E: ;
    $(LD) $(mktmp $(_LDFLAGS) -Tpe -aa $(WIN_VERSION) c0w32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS:s/\/\\) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.IF $(DBG)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
    tdstrp32 $@
.ENDIF
.ENDIF
.ELSE
%$E: ;
    $(LD) $(mktmp $(_LDFLAGS) -Tpe -ap c0x32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS:s/\/\\) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.IF $(USE_SOFTICE)
    $(NMSYM) $(NMSYMFLAGS);$(SI_SOURCE) $@
    tdstrp32 $@
.ENDIF
.ENDIF
.ELIF $(USE_TNT)
%$E: ;
    @$(CP) $(mktmp stub 'gotnt.exe') $*.def
    @$(LD) $(mktmp $(_LDFLAGS) -Tpe -ap c0x32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.IF $(DOSSTYLE)
    @markphar $@
.ENDIF
    @$(RM) -S $(mktmp $*.def)
.ELIF $(USE_SMX32)
%$E: ; $(LD) $(mktmp $(_LDFLAGS) -Tpe -ap c0x32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.ELIF $(USE_RTTARGET)
%$E: ; $(LD) $(mktmp $(_LDFLAGS) c0x32.obj $(&:s/\/\\), $*.exe, $*.map, $(DEF_LIBS) $(EXELIBS) pm.lib)
.ELSE
%$E: ; $(LD) $(mktmp $(_LDFLAGS) -Tpe -ap c0x32.obj+\n$(&:s/\/\\)\n$@\n$*.map\n$(DEF_LIBS) $(PMLIB:s/\/\\) $(EXELIBS)\n$*.def)
.END

.ENDIF
