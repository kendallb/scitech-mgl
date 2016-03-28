/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C/C++
* Environment:  any
*
* Description:  Header file for floating point compatability routines. These
*               routines and macros are designed to provide the same
*               functionality as the corresponding fixed point routines.
*               We also have high performance 387 assembler routines for
*               direct co-processor support.
*
*               This header file expects one of the following #define's to
*               be defined before it is included. These are:
*
*                   FX_FLOAT    - Use IEEE floating point numbers
*                   FX_DOUBLE   - Use IEEE double precision numbers
*
****************************************************************************/

#ifndef __FX_FLOAT_H
#define __FX_FLOAT_H

#ifndef __MATH_H
#include <math.h>
#endif

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*------------------------- Internal Macro's etc --------------------------*/

/* Determine what type to use for floating point numbers, either float
 * or double.
 */

#if !defined(FX_FLOAT) && !defined(FX_DOUBLE)
#error  You must define one of either FX_FLOAT or FX_DOUBLE
#endif

#ifdef  FX_FLOAT
typedef float       real;
#endif

#ifdef  FX_DOUBLE
typedef double      real;
#endif

typedef double      realdbl;        /* Double for special case code     */
typedef long        FXZFixed;       /* 4.28 fixed point format          */

/* Define a number of macros that perform multiplication of real numbers
 * using the standard arithmetic operations. We can do them as inline
 * functions, but this seems to confuse the optimiser and slows the
 * resulting code.
 */

#define FXmul(a,b)          ((a) * (b))
#define FXmuldiv(a,b,c)     ((a) * (b) / (c))
#define FXmuleq(a,b)        ((a) *= (b))
#define FXdiv(a,b)          ((a) / (b))
#define FXdiveq(a,b)        ((a) /= (b))
#define FXoneOver(a)        (1.0 / (a))
#define FXsquare(a)         ((a) * (a))
#define FXceil(f)           ((real)ceil(f))
#define FXfloor(f)          ((real)floor(f))
#define FXround(a)          (real)floor((a) + 0.5)
#define FXdivZF(a,b)        ((long)((real)(a) / (b)))
#define FXdivFF(a,b)        ((long)((a) / (b) * (real)268435456.0))

/* Define a number of conversion macros. These are faster than inline
 * functions.
 */

#define FXintToReal(i)      ((real)(i))
#define FXintToZFixed(i)    ((long)(i) << 28)
#define FXfixedToReal(f)    ((f) / (real)65536.0)
#define FXzfixedToReal(f)   ((f) / (real)268435456.0)
#define FXdblToReal(d)      ((real)(d))
#define FXfltToReal(f)      ((real)(f))
#define FXzfixedToInt(f)    ((int)((long)(f) >> 28))
#define FXrealToFlt(r)      ((float)(r))
#define FXrealToDbl(r)      ((double)(r))
#define NEED_INT_CONVERT
#define FXrealToFixed(r)    FXrealToInt((r) * (real)65536.0)
#define FXrealToZFixed(r)   FXrealToInt((r) * (real)268435456.0)

/* Macro for declaring real data */

#define REAL(r)             (real)(r)
#define REALDBL(r)          (realdbl)(r)
#define REALZ(num)          (long)((num) * 268435456.0)

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

/*----------------------------- Inline functions --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

#ifdef  FPU387

extern real FXpi_180;

#if defined(__WATCOMC__)

#pragma aux FX_FUNC "_*" parm caller value [8087];

#if defined(__386__) && !defined(NOINLINEASM)

/* For Watcom C++ we can use special inline assembler code that is much
 * faster than calling the 386 assembler functions.
 */

/* These appear to be causing major problems with the optimiser at the
 * moment.
 */

#if 0
#define FXrndToInt(r)   FXrealToInt((r) + 0.5)

long FXrealToInt(real f);
#pragma aux FXrealToInt =           \
    "push   eax"                    \
    "fstcw  [esp]"                  \
    "wait"                          \
    "push   [esp]"                  \
    "mov    byte ptr +1[esp],1Fh"   \
    "fldcw  [esp]"                  \
    "push   eax"                    \
    "fistp  dword ptr [esp]"        \
    "pop    eax"                    \
    "fldcw  +4[esp]"                \
    "wait"                          \
    "lea    esp,+8[esp]"            \
    parm [8087]                     \
    value [eax]                     \
    modify exact [8087];

#undef  NEED_INT_CONVERT

real FXsqrt(real f);
#pragma aux FXsqrt =                \
    "fsqrt"                         \
    parm [8087]                     \
    value [8087]                    \
    modify exact [8087];

real FXsin(real angle);
#pragma aux FXsin =                 \
    "fmul   FXpi_180"               \
    "fsin"                          \
    parm [8087]                     \
    value [8087]                    \
    modify exact [8087];

real FXcos(real angle);
#pragma aux FXcos =                 \
    "fmul   FXpi_180"               \
    "fcos"                          \
    parm [8087]                     \
    value [8087]                    \
    modify exact [8087];

real FXtan(real angle);
#pragma aux FXtan =                 \
    "fmul   FXpi_180"               \
    "fptan"                         \
    "wait"                          \
    "nop"                           \
    "fstp   st(0)"                  \
    parm [8087]                     \
    value [8087]                    \
    modify exact [8087];
#endif

real FXsqrt(real f);
#pragma aux (FX_FUNC) FXsqrt;
real FXsin(real angle);
#pragma aux (FX_FUNC) FXsin;
real FXcos(real angle);
#pragma aux (FX_FUNC) FXcos;
real FXtan(real angle);
#pragma aux (FX_FUNC) FXtan;

#else   /* !__386__ */

/* For Watcom C++ we need to tell the compiler that although the above
 * routines are passed arguments on the stack, they return them on the
 * 80387 stack (default for most compilers).
 */

real FXsqrt(real f);
#pragma aux (FX_FUNC) FXsqrt;
real FXsin(real angle);
#pragma aux (FX_FUNC) FXsin;
real FXcos(real angle);
#pragma aux (FX_FUNC) FXcos;
real FXtan(real angle);
#pragma aux (FX_FUNC) FXtan;

#endif  /* !__386__ */

real FXlog(real f);
#pragma aux (FX_FUNC) FXlog;
real FXlog10(real f);
#pragma aux (FX_FUNC) FXlog10;
void _ASMAPI FXsincos(real angle,real *Sin,real *Cos);

#else   /* !__WATCOMC__ */

/* We have high performance 387 assembler routines for direct co-processor
 * support on PC's. Arguments are passed on the stack and values are
 * returned in the 80387 ST(0) register.
 */

real _ASMAPI FXsqrt(real f);
real _ASMAPI FXlog(real f);
real _ASMAPI FXlog10(real f);
real _ASMAPI FXsin(real angle);
real _ASMAPI FXcos(real angle);
void _ASMAPI FXsincos(real angle,real *Sin,real *Cos);
real _ASMAPI FXtan(real angle);

#endif  /* !__WATCOMC__ */

#else   /* !FPU387 */

#define FXsqrt(f)           sqrt(f)
#define FXlog(f)            log(f)
#define FXlog10(f)          log10(f)
#define FXsin(r)            sin((r) * (M_PI / 180))
#define FXcos(r)            cos((r) * (M_PI / 180))
#define FXtan(r)            tan((r) * (M_PI / 180))
#define FXsincos(r,Sin,Cos) { *(Sin) = FXsin(r); *(Cos) = FXcos(r); }

#endif  /* !FPU387 */

#ifdef  NEED_INT_CONVERT
#define FXrealToInt(r)      ((long)(r))
#define FXrndToInt(r)       FXrealToInt((r) + 0.5)
#undef  NEED_INT_CONVERT
#endif

#define FXpow(x,y)          pow(x,y)
#define FXasin(f)           (asin(f) * (180 / M_PI))
#define FXacos(f)           (acos(f) * (180 / M_PI))
#define FXatan(f)           (atan(f) * (180 / M_PI))
#define FXatan2(x,y)        (atan2(x,y) * (180 / M_PI))
#define FXabs(f)            fabs(f)

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __FX_FLOAT_H */
