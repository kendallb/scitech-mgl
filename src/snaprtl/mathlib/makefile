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
#               Portable Driver math library. This library is based on
#               the Open Watcom math library.
#
#############################################################################

#----------------------------------------------------------------------------
# Target dependent configuration stuff
#----------------------------------------------------------------------------

STOP_ON_WARN    := 1
NO_DEFAULT_LIBS := 1
UNSIGNED_CHAR   := 1
CFLAGS          += -D__SNAPDRV__ -I..\h -Ih

.IF $(USE_PPC_BE)
   CFLAGS       += -U__LINUX__
.ENDIF

#----------------------------------------------------------------------------
# Since this runtime library *requires* the Open Watcom assembler, we have
# to override the assembler flags in here.
#----------------------------------------------------------------------------

.IF $(USE_X86)
USE_WASM        := 1
ASFLAGS         := -I..\h -Ih -ms-5r-e-fpi87-fp5-w4-zld-bt=SNAP -D__SNAPDRV__
.ENDIF

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

ALIAS_OBJS      = _iwtof__$O _i_ecvt_$O _i_fcvt_$O _i_finit$O _i_gcvt_$O \
                  _i__ecvt$O _i__fcvt$O _i__gcvt$O

ASM_OBJS        = acos387$O acosh387$O acosl87$O asin387$O asinh387$O asinl87$O \
                  atan387$O atanh387$O atanl87$O cotan387$O exp387$O fabs387$O \
                  fchop$O fmod387$O log387$O modf387$O pow387$O sqrt387$O \
                  trig387$O trigh387$O trigl87$O __87ldi2$O __87ldi4$O __mtherr$O

C_OBJS          = atof$O cabs$O ceil$O cnvd2f$O cnvs2d$O cvt$O \
                  difftime$O divc8$O divc16$O divc20$O efcvt$O efgfmt$O evalpoly$O \
                  finite$O floor$O frexp$O gcvt$O grabfpe$O hypot$O ifcdabs$O \
                  ifcdcos$O ifcdexp$O ifcdlog$O ifcdsin$O ifcdsqrt$O j0$O j1$O \
                  jn$O ldclass$O ldcvt$O ldexp$O log87e$O math2err$O math87e$O \
                  matherr$O mulc8$O mulc16$O mulc20$O pow87e$O powc8$O powc16$O \
                  powc20$O powic8$O powic16$O powic32$O powii$O powri$O powrr$O \
                  powxi$O qmatherr$O sgn$O strtod$O y0$O y1$O yn$O zmatherr$O \
                  _matherr$O

X86_OBJS        = clear87$O
PPC_OBJS        = sqrt$O modfaxp$O fpremaxp$O pow$O log$O exp$O fabs$O log10$O \
                  acos$O asin$O atan$O atan2$O cosh$O sincos$O sinh$O tanh$O fmod$O
AMD64_OBJS      = sqrt$O modfaxp$O fpremaxp$O pow$O log$O exp$O fabs$O log10$O \
                  acos$O asin$O atan$O atan2$O cosh$O sincos$O sinh$O tanh$O fmod$O

MIPS_OBJS       = sqrt$O modfaxp$O fpremaxp$O pow$O log$O exp$O fabs$O log10$O \
                  acos$O asin$O atan$O atan2$O cosh$O sincos$O sinh$O tanh$O fmod$O

#----------------------------------------------------------------------------

# Name of library and object files required to build it
#----------------------------------------------------------------------------

.IF $(USE_X86)
   OBJECTS      = $(ALIAS_OBJS) $(ASM_OBJS) $(C_OBJS) $(X86_OBJS)
.ELIF $(USE_PPC_BE)
   OBJECTS      = $(C_OBJS) $(PPC_OBJS)
.ELIF $(USE_X86_64)
   OBJECTS      = $(C_OBJS) $(AMD64_OBJS)
.ELIF $(USE_MIPS_LE)
   OBJECTS      = $(C_OBJS) $(MIPS_OBJS)
.ELSE
   Error - unsupported platform
.ENDIF
LIBFILE         = $(LP)mathlib$L
LIBCLEAN        = *.lib *.a

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_INC      = ../h;h
DEPEND_SRC      = alias;a;c

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

