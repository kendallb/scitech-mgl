/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Language:     ANSI C
* Environment:  Any
*
* Description:  C based polygon rendering helper functions. These are
*               normally coded in high performance assembler for the
*               target machine and are used by the software polygon
*               rendering code.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

#if defined(NO_ASSEMBLER)

/****************************************************************************
PARAMETERS:
v1      - First vertex in edge
v2      - Second vertex in edge
slope   - Pointer to place to store slope

RETURNS:
Sign of slope (1 = +ve, 0, -1 = i-ve)

REMARKS:
Compute the slope of the edge and return the sign of the slope.
{secret}
****************************************************************************/
int MGLAPI _MGL_computeSlope(
    fxpoint_t *v1,
    fxpoint_t *v2,
    fix32_t *slope)
{
    fix32_t dy;

    if ((dy = v2->y - v1->y) > 0) {
        if (dy <= MGL_FIX_1)
            *slope = v2->x - v1->x;
        else
            *slope = MGL_FixDiv(v2->x - v1->x,dy);
        return 1;
        }
    if (dy < 0)
        return -1;
    return 0;
}

#ifndef  __64BIT__

/****************************************************************************
DESCRIPTION:
Multiplies two fixed point number in 16.16 format

HEADER:
mgraph.h

PARAMETERS:
a   - First number to multiply
b   - Second number to multiply

RETURNS:
Result of the multiplication.

REMARKS:
Multiplies two fixed point number in 16.16 format together
and returns the result. We cannot simply multiply the
two 32 bit numbers together since we need to shift the
64 bit result right 16 bits, but the result of a FXFixed
multiply is only ever 32 bits! Thus we must resort to
computing it from first principles (this is slow and
should ideally be re-coded in assembler for the target
machine).

We can visualise the fixed point number as having two
parts, a whole part and a fractional part:

    FXFixed = (whole + frac * 2^-16)

Thus if we multiply two of these numbers together we
get a 64 bit result:

    (f_whole + f_frac * 2^-16) * (g_whole + g_frac * 2^-16) =
        (f_whole * g_whole) +
        (f_whole * g_frac)*2^-16 +
        (g_whole * f_frac)*2^-16 +
        (f_frac * g_frac)*2^-32

To convert this back to a 64 bit fixed point number to 32
bit format we simply shift it right by 16 bits (we can round
it by adding 2^-17 before doing this shift). The formula
with the shift integrated is what is used below. Naturally
you can alleviate most of this if the target machine can
perform a native 32 by 32 bit multiplication (since it
will produce a 64 bit result).

SEE ALSO:
MGL_FixDiv, MGL_FixMulDiv
****************************************************************************/
fix32_t MGLAPI MGL_FixMul(
    fix32_t f,
    fix32_t g)
{
    fix32_t f_whole,g_whole;
    fix32_t f_frac,g_frac;

    // Extract the whole and fractional parts of the numbers. We strip the
    // sign bit from the fractional parts but leave it intact for the
    // whole parts. This ensures that the sign of the result will be correct.

    f_frac = f & 0x0000FFFF;
    f_whole = f >> 16;
    g_frac = g & 0x0000FFFF;
    g_whole = g >> 16;

    // We round the result by adding 2^(-17) before we shift the
    // fractional part of the result left 16 bits.

    return  ((f_whole * g_whole) << 16) +
            (f_whole * g_frac) +
            (f_frac * g_whole) +
            ((f_frac * g_frac + 0x8000) >> 16);
}

/****************************************************************************
DESCRIPTION:
Divides a fixed point number by another.

HEADER:
mgraph.h

PARAMETERS:
a   - Fixed point number to divide
b   - Fixed point number to divide by

RETURNS:
Result of the division.

SEE ALSO:
MGL_FixMul, MGL_FixMulDiv

REMARKS:
Divides a fixed point number by another fixed point number.
The idea is relatively simple; We want to set up a 64 bit
dividend to be divided by our 32 bit divisor, which will
give us a new 32 bit result.
****************************************************************************/
fix32_t MGLAPI MGL_FixDiv(
    fix32_t f,
    fix32_t g)
{
    unsigned long   quot,rem;
    unsigned long   a,b;
    int             i,apos,bpos,carry;

    // Remember the sign of the operands and divide only positive numbers.

    apos = (f >= 0);
    a = apos ? f : -f;
    bpos = (g >= 0);
    b = bpos ? g : -g;

    // Do a standard shift based FXFixed division to get the result. Since
    // this is fixed point we are essentially dividing a 48 bit number
    // by a 32 bit number where the bottom 16 bits of the 48 bit dividend
    // are zeros (since our 32 bit number goes in the middle of the
    // 64 bit dividend in the figure above).
    //
    // Thus it is just like dividing a 32 bit number by a 32 bit number
    // but we loop around 48 times to get the extra precision that we
    // need!

    quot = a;
    rem = 0;
    for (i = 48; i; i--) {

        // Shift the 64 bits of quotient and remainder left one bit
        carry = quot >> 31;
        quot <<= 1;
        rem = (rem << 1) | carry;

        if (rem >= b) {
            quot |= 1;          // Build the quotient
            rem -= b;
            }
        }

    return ((apos == bpos) ? quot : -quot);
}

/****************************************************************************
DESCRIPTION:
Multiplies a fixed point number by another and divides by a third number.

HEADER:
mgraph.h

PARAMETERS:
a   - First number to multiply
b   - Second number to multiply
c   - Third number to divide by

RETURNS:
Results of the multiplication and division.

REMARKS:
This function multiplies a 16.16 fixed point number by another producing a 32.32
intermediate result. This 32.32 result is then divided by another 16.16 number to
produce a 16.16 result. Because this routine maintains maximum precision during
the multiplication stage, you can multiply numbers that would normally overflow
the standard MGL_FixMul function.

SEE ALSO:
MGL_FixMul, MGL_FixDiv

****************************************************************************/
fix32_t MGLAPI MGL_FixMulDiv(
    fix32_t a,
    fix32_t b,
    fix32_t c)
{ return MGL_FixMul(a,MGL_FixDiv(b,c)); }

/****************************************************************************
DESCRIPTION:
Determines if a polygon is backfacing.

HEADER:
mgraph.h

PARAMETERS:
dx1 - change in x along first edge
dy1 - change in y along first edge
dx2 - change in x along second edge
dy2 - change in y along second edge

RETURNS:
1 if the polygon if backfacing, 0 if it is frontfacing

REMARKS:
Determine whether a polygon is backfacing given two fixed point vectors.  The
vectors need to be derived from two consecutive counterclockwise edges of the
polygon in order for this function to return accurate results.

Note that this function is written to correctly calculate the results for screen space
coordinates, which can cause overflow with a normal 16.16 fixed point multiply if
this is calculated directly using calls to MGL_FixMul.

****************************************************************************/
int MGLAPI MGL_backfacing(
    fix32_t dx1,
    fix32_t dy1,
    fix32_t dx2,
    fix32_t dy2)
{
    // Lose a bit of precision so screen space coordinates will fit within
    // a 32-bit multiply. On 64-bit machines this will be much simpler.
    return ((dx1>>3) * (dy2>>3)) - ((dx2>>3) * (dy1>>3)) < 0;
}

#else   /* __64BIT__ */

// TODO: Re-code all functions using 64-bit integers as this will be a lot
//       more efficient that doing it with 32-bit integers.
fix32_t MGLAPI MGL_FixMul(
    fix32_t f,
    fix32_t g)
{
    return (((u64)f * (u64)g) + 0x8000) >> 16;
}

fix32_t MGLAPI MGL_FixDiv(
    fix32_t f,
    fix32_t g)
{
    return ((s64)f << 16) / g;
}

#endif  /* __64BIT__ */

#endif  /* !NO_ASSEMBLER */

