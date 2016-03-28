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
* Description:  Header file for a set of fixed point number manipulation
*               routines. The numbers are represented in a 32 bit fixed
*               point format (16.16 format) for normal (x,y,z) coordinates.
*               We also have a special 4.28 format for fixed point Z
*               coordinates, which is the format of the Z coordinates after
*               being passed through a perspective transform (normalised
*               to lie between -1 and 1 of course ;-).
*
*               This header file expects one of the following #define's to
*               be defined before it is included. These are:
*
*                   FIXED386    - Use faster code for 80386 based machines
*                   FIXEDHLL    - Use slow High Level C implementation
*
*               The fixed point numbers are stored in a 32 bit ANSI C long
*               integer. A number of specialised operations are provided
*               to peform non-standard arithmetic operations on the fixed
*               point numbers. A lot of the normal arithmetic operations
*               can be performed using the standard arithmetic operations
*               defined for long integers. The following describes how to
*               manipulate the fixed point numbers in your programs:
*
*               Addition and subtraction:
*               -------------------------
*
*               Addition and subtraction of fixed point numbers can be
*               accomplished by simply adding and subtracting the longs
*               representing the fixed point numbers. Likewise, negation
*               increment and decrement operations can be peformed as normal.
*
*               Multiplication:
*               ---------------
*
*               If you need to multiply a fixed point number by another
*               fixed point number, you will need to call the routine
*               FXmul() to do it for you. If you need to multiply a
*               fixed point number by an integer, you can simply multiply
*               the fixed point representation by the integer.
*
*               Division:
*               ---------
*
*               Division should always be performed by calling FXdiv().
*               Division is slow unless you are using the 386 library
*               (even then it is still slow), so you should try to avoid
*               it. If you need to divide more than one number by the
*               same value, calculate the inverse and multiply each number
*               by the inverse.
*
*               A special routine is provided to perform a multiply then
*               a divide, while maintaining 64 bit precision for the
*               temporary (only on a 386).
*
*               Multiplication and division by powers of two:
*               ---------------------------------------------
*
*               If you need to multiply or divide by whole powers of two,
*               you can use the simple shift left and shift right operators
*               on the fixed point representation as you would for when
*               working with normal integers. ie:
*
*                   myfixed <<= 2;  // Mutiply by 4 (2^2)
*                   myfixed >>= 3;  // Divide by 8 (2^3)
*
*               Note that no rounding is performed for division.
*
*               Relational operations:
*               ----------------------
*
*               To compare fixed point numbers with each other, simply
*               compare the long integer representations of them. ie:
*               use them like normal.
*
*               Conversions:
*               ------------
*
*               To convert numbers from doubles and integers to and from
*               fixed point, use the provided conversion routines:
*
*                   FXdblToReal();
*                   FXintToReal();
*                   FXrealToInt();
*                   FXrndToInt();
*                   FXrealToDbl();
*
*               Full precision 4.28 divide routines:
*               ------------------------------------
*
*                   FXdivZF();      4.28 = 4.28 / 16.16
*                   FXdivFF();      4.28 = 16.16 / 16.16
*
*               Miscellaneous operations:
*               -------------------------
*
*               A number of useful operations are also defined for
*               fixed point. These are:
*
*                   FXceil()
*                   FXfloor()
*                   FXround()
*                   FXoneOver()
*                   FXsquare()
*                   FXpow()
*                   FXlog()
*                   FXlog10()
*                   FXsin()
*                   FXcos()
*                   FXsincos()
*                   FXtan()
*                   FXasin()
*                   FXacos()
*                   FXatan()
*                   FXatan2()
*                   FXsqrt()
*
*               To display the value of a fixed point number, convert it
*               to a double and display that.
*
*               We have included a C++ wrapper class (TFixed) for the fixed
*               point routines which you can use if speed is not essential
*               for your particular application (but still faster than
*               floating point emulation). Current C++ optimising compiler
*               technology is not good enough to produce acceptable code
*               for a fixed point C++ class used in complex expressions.
*               Hopefully future optimising C++ compilers and/or extensions
*               to the C++ language may well do a better job.
*
****************************************************************************/

#ifndef __FX_FIXED_H
#define __FX_FIXED_H

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*------------------------- Internal Macro's etc --------------------------*/

/* Ensure that one of the implementation macros is set */

#if !defined(FIXED386) && !defined(FIXEDHLL)
#error  You must define one of either FIXED386, FIXEDHLL
#endif

/* All of our fixed point numbers are represented in a 32 bit ANSI long
 * integer. We define a typedef below for our type to aid readability.
 */

typedef long    FXFixed;            /* 16.16 format                 */
typedef long    FXZFixed;           /* 4.28 format                  */

/* Use the following macros when defining global data rather than using
 * the inline conversion routines. This will ensure that the compiler
 * calculates the fixed point value at compile time. Dont use it to convert
 * values at runtime!
 */

#define REAL(num)       (long)((num) * 65536.0 + 0.5)
#define REALDBL(r)      (long)((num) * 65536.0 + 0.5)
#define REALZ(num)      (long)((num) * 268435456.0)

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

/*----------------------------- Inline functions --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Define a number of inline functions that call the appropriate low
 * level fixed point routines for the target machine.
 */

#ifdef  FIXED386

/* Implementation dependant assembly language routines for fixed point
 * arithmetic.
 */

#if defined(__WATCOMC__) && defined(__386__) && !defined(NOINLINEASM)

/* For Watcom C++ we can use special inline assembler code that is much
 * faster than calling the 386 assembler functions.
 */

FXFixed FXmul(FXFixed a,FXFixed b);
#pragma aux FXmul =                 \
    "imul   edx"                    \
    "add    eax,8000h"              \
    "adc    edx,0"                  \
    "shrd   eax,edx,16"             \
    parm [eax] [edx]                \
    value [eax]                     \
    modify exact [eax edx];

FXFixed FXdiv(FXFixed dividend,FXFixed divisor);
#pragma aux FXdiv =                 \
    "xor    eax,eax"                \
    "shrd   eax,edx,16"             \
    "sar    edx,16"                 \
    "idiv   ebx"                    \
    parm [edx] [ebx]                \
    value [eax]                     \
    modify exact [eax edx];

FXFixed FXmuldiv(FXFixed a,FXFixed b,FXFixed c);
#pragma aux FXmuldiv =              \
    "imul   ebx"                    \
    "idiv   ecx"                    \
    parm [eax] [ebx] [ecx]          \
    value [eax]                     \
    modify exact [eax edx];

FXFixed FXoneOver(FXFixed f);
#pragma aux FXoneOver =             \
    "xor    eax,eax"                \
    "mov    edx,1"                  \
    "idiv   ebx"                    \
    parm [ebx]                      \
    value [eax]                     \
    modify exact [eax edx];

FXFixed FXsquare(FXFixed f);
#pragma aux FXsquare =              \
    "imul   eax"                    \
    "add    eax,8000h"              \
    "adc    edx,0"                  \
    "shrd   eax,edx,16"             \
    parm [eax]                      \
    value [eax]                     \
    modify exact [edx];

FXZFixed FXdivFF(FXFixed dividend,FXFixed divisor);
#pragma aux FXdivFF =               \
    "xor    eax,eax"                \
    "shrd   eax,edx,16"             \
    "sar    edx,16"                 \
    "idiv   ebx"                    \
    "mov    ecx,eax"                \
    "shl    edx,12"                 \
    "mov    eax,edx"                \
    "xor    edx,edx"                \
    "div    ebx"                    \
    "shl    eax,20"                 \
    "shld   ecx,eax,12"             \
    parm [edx] [ebx]                \
    value [ecx]                     \
    modify exact [eax edx];

FXZFixed FXdivZF(FXZFixed dividend,FXFixed divisor);
#pragma aux FXdivZF =               \
    "xor    eax,eax"                \
    "shrd   eax,edx,28"             \
    "sar    edx,28"                 \
    "idiv   ebx"                    \
    "mov    ecx,eax"                \
    "shl    edx,12"                 \
    "mov    eax,edx"                \
    "xor    edx,edx"                \
    "div    ebx"                    \
    "shl    eax,20"                 \
    "shld   ecx,eax,12"             \
    parm [edx] [ebx]                \
    value [ecx]                     \
    modify exact [eax edx];

#else

FXFixed _ASMAPI F386_mul(FXFixed a,FXFixed b);
FXFixed _ASMAPI F386_div(FXFixed dividend,FXFixed divisor);
FXFixed _ASMAPI F386_muldiv(FXFixed a,FXFixed b,FXFixed c);
FXFixed _ASMAPI F386_oneOver(FXFixed f);
FXFixed _ASMAPI F386_square(FXFixed f);
FXZFixed _ASMAPI F386_divZF(FXZFixed dividend,FXFixed divisor);
FXZFixed _ASMAPI F386_divFF(FXFixed dividend,FXFixed divisor);

#define FXmul(f,g)      F386_mul(f,g)
#define FXdiv(f,g)      F386_div(f,g)
#define FXoneOver(f)    F386_oneOver(f)
#define FXsquare(f)     F386_square(f)
#define FXmuldiv(a,b,c) F386_muldiv(a,b,c)
#define FXdivZF(f,g)    F386_divZF(f,g)
#define FXdivFF(f,g)    F386_divFF(f,g)

#endif

FXFixed _ASMAPI F386_sqrt(FXFixed f);
FXFixed _ASMAPI F386_sin(FXFixed f);
FXFixed _ASMAPI F386_cos(FXFixed f);
FXFixed _ASMAPI F386_tan(FXFixed f);
FXFixed _ASMAPI F386_asin(FXFixed f);
FXFixed _ASMAPI F386_acos(FXFixed f);
FXFixed _ASMAPI F386_atan(FXFixed f);
FXFixed _ASMAPI F386_atan2(FXFixed x,FXFixed y);
FXFixed _ASMAPI F386_log(FXFixed f);
FXFixed _ASMAPI F386_log10(FXFixed f);

#define FXmuleq(f,g)    ((f) = FXmul(f,g))
#define FXdiveq(f,g)    ((f) = FXdiv(f,g))
#define FXsqrt(f)       F386_sqrt(f)
#define FXsin(f)        F386_sin(f)
#define FXcos(f)        F386_cos(f)
#define FXtan(f)        F386_tan(f)
#define FXasin(f)       F386_asin(f)
#define FXacos(f)       F386_acos(f)
#define FXatan(f)       F386_atan(f)
#define FXatan2(x,y)    F386_atan2(x,y)
#define FXlog(f)        F386_log(f)
#define FXlog10(f)      F386_log10(f)

#endif  // FIXED386

#ifdef  FIXEDHLL

FXFixed FHLL_mul(FXFixed a,FXFixed b);
FXFixed FHLL_div(FXFixed dividend,FXFixed divisor);
FXFixed FHLL_sqrt(FXFixed f);
FXFixed FHLL_sin(FXFixed f);
FXFixed FHLL_cos(FXFixed f);
FXFixed FHLL_tan(FXFixed f);
FXFixed FHLL_asin(FXFixed f);
FXFixed FHLL_acos(FXFixed f);
FXFixed FHLL_atan(FXFixed f);
FXFixed FHLL_atan2(FXFixed x,FXFixed y);
FXFixed FHLL_log(FXFixed f);
FXFixed FHLL_log10(FXFixed f);
FXZFixed FHLL_divZF(FXZFixed dividend,FXFixed divisor);
FXZFixed FHLL_divFF(FXFixed dividend,FXFixed divisor);

#define FXmul(f,g)      FHLL_mul(f,g)
#define FXdiv(f,g)      FHLL_div(f,g)
#define FXmuleq(f,g)    ((f) = FXmul(f,g))
#define FXdiveq(f,g)    ((f) = FXdiv(f,g))
#define FXsqrt(f)       FHLL_sqrt(f)
#define FXsin(f)        FHLL_sin(f)
#define FXcos(f)        FHLL_cos(f)
#define FXtan(f)        FHLL_tan(f)
#define FXasin(f)       FHLL_asin(f)
#define FXacos(f)       FHLL_acos(f)
#define FXatan(f)       FHLL_atan(f)
#define FXatan2(x,y)    FHLL_atan2(x,y)
#define FXlog(f)        FHLL_log(f)
#define FXlog10(f)      FHLL_log10(f)
#define FXoneOver(f)    FHLL_div(REAL(1),f)
#define FXsquare(f)     FHLL_mul(f,f)
#define FXmuldiv(a,b,c) FHLL_mul(a,FHLL_div(b,c))
#define FXdivZF(f,g)    FHLL_divZF(f,g)
#define FXdivFF(f,g)    FHLL_divFF(f,g)

#endif

#define FXsincos(r,Sin,Cos) { *(Sin) = FXsin(r); *(Cos) = FXsin(REAL(90)+(r)); }
#define FXabs(f)        ((f) >= 0 ? (f) : -(f))

/* The ceil(), floor() and round() operations can be quickly implemented
 * using fast adds and masks. It is much quicker to do it here as inline
 * code than it is to do it in assembler or as non-inline code.
 */

#define FXceil(f)   ((FXFixed)(((f) + 0xFFFFL) & 0xFFFF0000L))
#define FXfloor(f)  ((FXFixed)((f) & 0xFFFF0000L))
#define FXround(f)  ((FXFixed)(((f) + 0x8000L) & 0xFFFF0000L))

/* Conversion operations. These can be done quickly inline. */

#define FXintToReal(i)      ((FXFixed)(i) << 16)
#define FXintToZFixed(i)    ((FXZFixed)(i) << 28)
#define FXfixedToReal(f)    (f)
#define FXzfixedToReal(f)   ((f) >> 12)
#define FXdblToReal(d)      ((FXFixed)((d) * 65536.0 + 0.5))
#define FXfltToReal(f)      ((FXFixed)((f) * 65536.0 + 0.5))
#define FXrealToInt(f)      ((int)((f) >> 16))
#define FXzfixedToInt(f)    ((int)((f) >> 28))
#define FXrndToInt(f)       ((int)(((f) + 0x8000L) >> 16))
#define FXrealToFixed(f)    (f)
#define FXrealToFlt(f)      ((float)((f) / 65536.0))
#define FXrealToDbl(f)      ((double)((f) / 65536.0))
#define FXrealToZFixed(f)   ((f) << 12)

/* These are not implemented in fixed point yet! */

FXFixed FXpow(FXFixed x,FXFixed y);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */

#ifndef __IOSTREAM_H
#include <iostream.h>
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The following defines a C++ wrapper class for our 16.16 fixed point
// routines. Using this class is convenient (can be used just like any
// normal C++ data type), however it is also a lot slower than calling
// the routines directly. In the future, better optimising C++ compilers
// may make using this class a feasible option for high performance code.
//---------------------------------------------------------------------------

class TFixed {
protected:
    FXFixed fixed;

public:
            // Default constructor that does nothing
            TFixed() {};

            // Constructors for the fixed point data type
            TFixed(double d)    { fixed = FXdblToReal(d); };
            TFixed(int i)       { fixed = FXintToReal(i); };
            TFixed(FXFixed l)   { fixed = l; };

            // Standard arithmetic operators for TFixed
    friend  TFixed operator + (const TFixed& f,const TFixed& g);
    friend  TFixed operator + (const TFixed& f,int i);
    friend  TFixed operator + (int i,const TFixed& f);
    friend  TFixed operator - (const TFixed& f,const TFixed& g);
    friend  TFixed operator - (const TFixed& f,int i);
    friend  TFixed operator - (int i,const TFixed& f);
    friend  TFixed operator * (const TFixed& f,const TFixed& g);
    friend  TFixed operator * (const TFixed& f,int i);
    friend  TFixed operator * (int i,const TFixed& f);
    friend  TFixed operator / (const TFixed& f,const TFixed& g);
    friend  TFixed operator / (const TFixed& f,int i);
    friend  TFixed operator / (int i,const TFixed& f);

            TFixed& operator += (const TFixed& f);
            TFixed& operator += (int i);
            TFixed& operator -= (const TFixed& f);
            TFixed& operator -= (int i);
            TFixed& operator *= (const TFixed& f);
            TFixed& operator *= (int i);
            TFixed& operator /= (const TFixed& f);
            TFixed& operator /= (int i);

            // Unary arithmetic operators for TFixed point
            TFixed operator - () const  {return TFixed(-fixed); };

            // Relational operators for TFixed point
            int operator < (const TFixed& f) const  { return fixed < f.fixed; };
            int operator <= (const TFixed& f) const { return fixed <= f.fixed; };
            int operator == (const TFixed& f) const { return fixed == f.fixed; };
            int operator != (const TFixed& f) const { return fixed != f.fixed; };
            int operator >= (const TFixed& f) const { return fixed >= f.fixed; };
            int operator > (const TFixed& f) const  { return fixed > f.fixed; };

            // Conversion routines
            operator int () const       { return FXrealToInt(fixed); };
            operator FXFixed () const   { return fixed; };
            operator float () const     { return FXrealToFlt(fixed); };
            operator double () const    { return FXrealToDbl(fixed); };

            // FXFixed point manipulation routines
    friend  TFixed ceil(const TFixed& f)  { return TFixed(FXceil(f.fixed)); };
    friend  TFixed floor(const TFixed& f) { return TFixed(FXfloor(f.fixed)); };
    friend  TFixed round(const TFixed& f) { return TFixed(FXround(f.fixed)); };

            // Routines to compute a fixed point trig functions, with
            // all angles in degrees
    friend  TFixed FXsin(const TFixed& a) { return TFixed(FXsin(a.fixed)); };
    friend  TFixed FXcos(const TFixed& a) { return TFixed(FXcos(a.fixed)); };
    friend  TFixed FXtan(const TFixed& a) { return TFixed(FXtan(a.fixed)); };
    friend  TFixed FXasin(const TFixed& a) { return TFixed(FXasin(a.fixed)); };
    friend  TFixed FXacos(const TFixed& a) { return TFixed(FXacos(a.fixed)); };
    friend  TFixed FXatan(const TFixed& a) { return TFixed(FXatan(a.fixed)); };
    friend  TFixed FXatan2(const TFixed& x,const TFixed& y);

            // Routine to compute the square root for fixed point
    friend  TFixed sqrt(const TFixed& f) { return TFixed(FXsqrt(f)); };

            // Misc routines
    friend  TFixed pow(const TFixed& x,const TFixed& y);
    friend  TFixed log(const TFixed& f);
    friend  TFixed log10(const TFixed& f);

            // Friend function to dump a fixed point value to a stream
    friend  ostream& operator << (ostream& o,const TFixed& f);
    };

/*------------------------ Inline member functions ------------------------*/

inline TFixed operator + (const TFixed& f,const TFixed& g)
{ return TFixed(f.fixed + g.fixed); }

inline TFixed operator + (const TFixed& f,int i)
{ return TFixed(f.fixed + FXintToReal(i)); }

inline TFixed operator + (int i,const TFixed& f)
{ return TFixed(FXintToReal(i) + f.fixed); }

inline TFixed operator - (const TFixed& f,const TFixed& g)
{ return TFixed(f.fixed - g.fixed); }

inline TFixed operator - (const TFixed& f,int i)
{ return TFixed(f.fixed - FXintToReal(i)); }

inline TFixed operator - (int i,const TFixed& f)
{ return TFixed(FXintToReal(i) - f.fixed); }

inline TFixed operator * (const TFixed& f,const TFixed& g)
{ return TFixed(FXmul(f.fixed,g.fixed)); }

inline TFixed operator * (const TFixed& f,int i)
{ return TFixed(f.fixed * i); }

inline TFixed operator * (int i,const TFixed& f)
{ return TFixed(f.fixed * i); }

inline TFixed operator / (const TFixed& f,const TFixed& g)
{ return TFixed(FXdiv(f.fixed,g.fixed)); }

inline TFixed operator / (const TFixed& f,int i)
{ return TFixed(FXdiv(f.fixed,FXintToReal(i))); }

inline TFixed operator / (int i,const TFixed& f)
{ return TFixed(FXdiv(FXintToReal(i),f)); }

inline TFixed& TFixed::operator += (const TFixed& f)
{ fixed += f.fixed; return *this; }

inline TFixed& TFixed::operator += (int i)
{ fixed += FXintToReal(i); return *this; }

inline TFixed& TFixed::operator -= (const TFixed& f)
{ fixed -= f.fixed; return *this; }

inline TFixed& TFixed::operator -= (int i)
{ fixed -= FXintToReal(i); return *this; }

inline TFixed& TFixed::operator *= (const TFixed& f)
{ FXmuleq(fixed,f.fixed); return *this; }

inline TFixed& TFixed::operator *= (int i)
{ fixed *= i; return *this; }

inline TFixed& TFixed::operator /= (const TFixed& f)
{ FXdiveq(fixed,f.fixed); return *this; }

inline TFixed& TFixed::operator /= (int i)
{ FXdiveq(fixed,FXintToReal(i)); return *this; }

inline TFixed FXatan2(const TFixed& x,const TFixed& y)
{ return FXatan2(x,y); }

inline TFixed pow(const TFixed& x,const TFixed& y)
{ return TFixed(FXpow(x.fixed,y.fixed)); }

inline TFixed log(const TFixed& f)
{ return TFixed(FXlog(f.fixed)); }

inline TFixed log10(const TFixed& f)
{ return TFixed(FXlog10(f.fixed)); }

inline ostream& operator << (ostream& o,const FXFixed& f)
{ return o << double(f); }

#endif  /* __cplusplus */

#endif  /* __FX_FIXED_H */
