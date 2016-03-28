#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2.0 as published by the Free
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
# Descripton:   Generic makefile for building the SciTech SNAP Binary
#               Portable Driver C++ runtime library. This library is based
#               on the Open Watcom math library.
#
#############################################################################

#----------------------------------------------------------------------------
# Target dependent configuration stuff
#----------------------------------------------------------------------------

STOP_ON_WARN    := 1
NO_DEFAULT_LIBS := 1
UNSIGNED_CHAR   := 1
USE_RTTI        := 1
CFLAGS          += -D__SNAPDRV__ -D__OBSCURE_STREAM_INTERNALS -DNDEBUG \
                   -D__USE_RW -Iiostream\h -Iruntime\h -Istring\h -Ih -I..\h

#----------------------------------------------------------------------------
# Since this runtime library *requires* the Open Watcom assembler, we have
# to override the assembler flags in here.
#----------------------------------------------------------------------------

USE_WASM        := 1
ASFLAGS         := -Iruntime\inc -I..\h -ms-5r-e-fpi87-fp5-w4-zld-bt=SNAP -D__SNAPDRV__

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

CONTAIN_OBJS    = wcexcept$O wchash$O wclist$O wcskip$O

# Note: First object file is not used if we build into a DLL

IOSTREAM_OBJS_1 = iofdefg$O flfattac$O flfclose$O flfconde$O flfconfd$O \
                  flfconsf$O flfdestr$O flfopen$O flfoprot$O flfoverf$O \
                  flfpbfai$O flfseeko$O flfsetbf$O flfsync$O flfundrf$O \
                  fsbattac$O fsbclose$O fsbconde$O fsbconfd$O fsbconnm$O \
                  fsbconsf$O fsbdestr$O fsbopen$O fsbsetbf$O fstconde$O \
                  fstconfd$O fstconnm$O fstconsf$O fstdestr$O ifsconde$O \
                  ifsconfd$O ifsconnm$O ifsconsf$O ifsdestr$O iofefg$O \
                  iofexdou$O iofexflo$O iofexldo$O iofgldfl$O iofldfst$O \
                  iofnoefg$O iofoutfl$O ioicerr$O ioicin$O ioiclog$O \
                  ioicout$O ioilfofg$O ioilock$O ioixainx$O iolglcon$O \
                  iolgldes$O iosbital$O iosclear$O ioscoeio$O ioscoios$O \
                  iosconde$O ioscostf$O iosdestr$O iosexcep$O iosfacau$O \
                  iosfail$O iosgnilk$O iosinit$O iosiword$O iospword$O \
                  iossetst$O iosxallo$O iotconbf$O iotconde$O iotconsm$O \
                  iotdestr$O iotopbuf$O iotopstm$O ioufgtba$O ioufrxal$O \
                  ioufuswr$O iouwrtit$O isscochz$O issconch$O issconsc$O \
                  issconuc$O isscoscz$O isscoucz$O issdestr$O istcoeis$O \
                  istcoesf$O istcoist$O istconde$O istconsf$O istdestr$O \
                  istdipfx$O istdoget$O istdread$O istdrshf$O isteatwh$O \
                  istexcha$O istexi64$O isteximn$O istexiom$O istexpch$O \
                  istexsin$O istexslo$O istexssh$O istexstf$O istexu64$O \
                  istexuin$O istexulo$O istexush$O istgalin$O istgbase$O \
                  istgch$O istget$O istgetul$O istgline$O istgnm64$O

IOSTREAM_OBJS_2 = istgnum$O istgpch$O istgsign$O istgstf$O istgtu64$O \
                  istignor$O istipfx$O istpback$O istpeek$O istread$O \
                  istsgsto$O istsgstp$O istsync$O isttellg$O mandec$O \
                  manendl$O manends$O manflush$O manhex$O manoct$O \
                  manriofg$O mansbase$O mansetw$O mansfill$O mansiofl$O \
                  mansprec$O manws$O ofsconde$O ofsconfd$O ofsconnm$O \
                  ofsconsf$O ofsdestr$O ossconch$O ossconde$O ossconsc$O \
                  ossconuc$O ossdestr$O ostadpre$O ostcoeos$O ostcoesf$O \
                  ostconde$O ostconsf$O ostcoost$O ostdestr$O ostdlshf$O \
                  ostflush$O ostincch$O ostincha$O ostini64$O ostiniom$O \
                  ostinomn$O ostinptr$O ostinslo$O ostinstf$O ostinu64$O \
                  ostinulo$O ostopfx$O ostput$O ostspsto$O ostspstp$O \
                  osttellp$O ostwrite$O sdfconde$O sdfconfp$O sdfdestr$O \
                  sdfoverf$O sdfsread$O sdfsync$O sdfundrf$O ssbconde$O \
                  ssbconsz$O ssbdestr$O ssfdestr$O ssfdoall$O ssfempsf$O \
                  ssfempsz$O ssfgpos$O ssfoverf$O ssfseeko$O ssfsetbf$O \
                  ssfstbuf$O ssfstr$O ssfsync$O ssfundrf$O sstconch$O \
                  sstconde$O sstconsc$O sstconuc$O sstdestr$O stfconb$O \
                  stfconde$O stfdbp$O stfdestr$O stfdoall$O stfdsgtn$O \
                  stfdspun$O stfgnslk$O stfpbfai$O stfsbuf$O stfseeko$O \
                  stfseekp$O  stfsetb$O stfsync$O

RUNTIME_A_OBJS  = fshand$O

RUNTIME_C_OBJS  = ljmpdtor$O ljmpinit$O areaexc$O fsroot$O moddtorr$O \
                  stabmod$O stablcl$O undefmbd$O prwdata$O fsregist$O \
                  stabfunc$O stabreg$O allocexc$O arrassgn$O arrdel$O \
                  arrnew$O bittest$O callterm$O callunex$O cppdata$O \
                  ctorarst$O darrnew$O dtorarst$O exc_pr$O exstring$O \
                  fatalerr$O generr$O lock$O opnew$O opnewarr$O \
                  opnewp$O opnewpar$O opdel$O opdelarr$O pgmthrd$O \
                  pure_err$O rttidync$O rttisupp$O rttiptr$O rttivoid$O \
                  rttiref$O rttitid$O setnwhdl$O _stnwhdl$O settrmnt$O \
                  setunexp$O sisema$O stabacp$O stabacpv$O stabact$O \
                  stabactv$O stabadt$O stabdtor$O stabinit$O stabtrav$O \
                  termnate$O thrddata$O tsequiv$O tssize$O ti_bfor$O \
                  ti_dtor$O ti_name$O ti_raw$O ti_opeq$O ti_opne$O \
                  undefed$O undefmbr$O unexpect$O varrnew$O vfun_err$O

STRING_OBJS     = allocrep$O allomusz$O constrch$O destruct$O opcostsc$O \
                  opasstcs$O opappcst$O opsubstr$O oprfchar$O zapcase$O \
                  compstr$O opeqchst$O opnechst$O oplqchst$O opgrchst$O \
                  opgqchst$O opcachst$O opinput$O opoutput$O matchcst$O \
                  compchar$O joinstr$O freestr$O indexcha$O appendst$O \
                  indexstr$O condefau$O constrst$O constcst$O matchstr$O \
                  matchs$O opappstr$O opcastst$O opcastch$O opcostch$O \
                  opco1sch$O opgrstst$O opgrstch$O opeqstst$O opeqstch$O \
                  oplqstst$O oplqstch$O oplestst$O oplestch$O oplechst$O \
                  opnestst$O opnestch$O opgqstst$O opgqstch$O opasstst$O \
                  opcrchar$O conszcap$O opeqcrst$O opnecrst$O oplecrst$O \
                  oplqcrst$O opgrcrst$O opgqcrst$O opcacrst$O opeqstcr$O \
                  opnestcr$O opgrstcr$O opgqstcr$O oplestcr$O oplqstcr$O \
                  opcastcr$O

#----------------------------------------------------------------------------
# Name of library and object files required to build it
#----------------------------------------------------------------------------

OBJECTS         = $(CONTAIN_OBJS) $(IOSTREAM_OBJS_1) $(IOSTREAM_OBJS_2) \
                  $(RUNTIME_A_OBJS) $(RUNTIME_C_OBJS) $(STRING_OBJS)
LIBFILE         = $(LP)cpplib$L
LIBCLEAN        = *.lib *.a

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_INC      = iostream/h;runtime/h;string/h;../h;h
DEPEND_SRC      = contain/cpp;iostream/cpp;runtime/a;runtime/cpp;string/cpp

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

