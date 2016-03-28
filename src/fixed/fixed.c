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
* Language:     ANSI C
* Environment:  any
*
* Description:  Miscellaneous functions to implement a fixed point data type.
*               We use 32 bits to represent numbers in 16.16 format. The
*               implementation here is all in C, and is relatively slow.
*               All of these routines are recoded in assembler for the
*               target machine.
*
*               We have included a C++ wrapper class (TFixed) for the fixed
*               point routines which you can use if speed is not essential
*               for your particular application.
*
****************************************************************************/

#include "fx/fixed.h"
#include <math.h>

/*--------------------------- Member functions ----------------------------*/

#ifdef  FIXEDHLL

// Ensure C style linkage so we can get at the table from assembler

extern unsigned long FXsqrt_table[];
extern unsigned short FXlog10_table[];
extern unsigned long FXlog2_table[];
extern unsigned long FXsin_table[];
extern unsigned long FXatan_table[];

FXFixed FHLL_mul(FXFixed f,FXFixed g)
/****************************************************************************
*
* Function:     FHLL_mul
* Parameters:   f   - FXFixed point mutiplicand
*               g   - FXFixed point number to multiply by
* Returns:      Result of the multiplication.
*
* Description:  Multiplies two fixed point number in 16.16 format together
*               and returns the result. We cannot simply multiply the
*               two 32 bit numbers together since we need to shift the
*               64 bit result right 16 bits, but the result of a FXFixed
*               multiply is only ever 32 bits! Thus we must resort to
*               computing it from first principles (this is slow and
*               should ideally be re-coded in assembler for the target
*               machine).
*
*               We can visualise the fixed point number as having two
*               parts, a whole part and a fractional part:
*
*                   FXFixed = (whole + frac * 2^-16)
*
*               Thus if we multiply two of these numbers together we
*               get a 64 bit result:
*
*               (a_whole + a_frac * 2^-16) * (b_whole + b_frac * 2^-16)
*
*                 = (a_whole * b_whole) +
*                   (a_whole * b_frac)*2^-16 +
*                   (b_whole * a_frac)*2^-16 +
*                   (a_frac * b_frac)*2^-32
*
*               To convert this back to a 64 bit fixed point number to 32
*               bit format we simply shift it right by 16 bits (we can round
*               it by adding 2^-17 before doing this shift). The formula
*               with the shift integrated is what is used below. Natrually
*               you can alleviate most of this if the target machine can
*               perform a native 32 by 32 bit multiplication (since it
*               will produce a 64 bit result).
*
****************************************************************************/
{
    FXFixed a_whole,b_whole;
    FXFixed a_frac,b_frac;

    // Extract the whole and fractional parts of the numbers. We strip the
    // sign bit from the fractional parts but leave it intact for the
    // whole parts. This ensures that the sign of the result will be correct.

    a_frac = f & 0x0000FFFF;
    a_whole = f >> 16;
    b_frac = g & 0x0000FFFF;
    b_whole = g >> 16;

    // We round the result by adding 2^(-17) before we shift the
    // fractional part of the result left 16 bits.

    return  ((a_whole * b_whole) << 16) +
            (a_whole * b_frac) +
            (a_frac * b_whole) +
            ((a_frac * b_frac + 0x8000) >> 16);
}

FXFixed FHLL_div(FXFixed f,FXFixed g)
/****************************************************************************
*
* Function:     FHLL_div
* Parameters:   f   - Dividend
*               g   - Divisor
* Returns:      Result of the division
*
* Description:  Divides a fixed point number by another fixed point number.
*               The idea is relatively simple; We want to set up a 64 bit
*               dividend to be divided by our 32 bit divisor, which will
*               give us a new 32 bit result:
*
*                   +-----------------+ +-----------------+
*                   |     32 bits     |.|     32 bits     |
*                   +-----------------+ +-----------------+
*                 __________________________________________
*
*                           +---------+ +---------+
*                           | 16 bits |.| 16 bits |
*                           +---------+ +---------+
*
*               This should be done directly in assembly language on the
*               target machine, as it is horrendously slow to do it in
*               C code here (we need to do a 48 bit binary FXFixed division -
*               using shifts and adds with carry!). In fact on a 12Mhz
*               80286 machine it was faster to do the following:
*
*                   return FXFixed((double)f / (double)g);
*
*               which takes 3 floating point divisions and 1 multiply,
*               which was being done in emulation mode! The routine is
*               here to clarify just what the low level assembly routines
*               must accomplish.
*
*               We can implement rounding by adding 1 to the final result
*               (not 1 in fixed point, but 1 binary) if the remainder is
*               at least half as large as the divisor.
*
****************************************************************************/
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

FXZFixed FHLL_divZF(FXZFixed f,FXFixed g)
/****************************************************************************
*
* Function:     FHLL_divZF
* Parameters:   f   - Dividend in 4.28 format
*               g   - Divisor in 16.16 format
* Returns:      Fixed point value in 4.28 format
*
* Description:  Divides a fixed point 4.28 format number by a fixed point
*               16.16 number, returning the result in 4.28 format. We
*               haven't bothered to implement this with full precision
*               here, so we simply convert to and from 16.16 format to
*               perform the operation.
*
****************************************************************************/
{
    return FXrealToZFixed(FHLL_div(FXzfixedToReal(f),g));
}

FXZFixed FHLL_divFF(FXFixed f,FXFixed g)
/****************************************************************************
*
* Function:     FHLL_divFF
* Parameters:   f   - Dividend in 16.16 format
*               g   - Divisor in 16.16 format
* Returns:      Fixed point value in 4.28 format
*
* Description:  Divides a fixed point 16.16 format number by a fixed point
*               16.16 number, returning the result in 4.28 format. We
*               haven't bothered to implement this with full precision
*               here, so we simply convert to and from 16.16 format to
*               perform the operation.
*
****************************************************************************/
{
    return FXrealToZFixed(FHLL_div(f,g));
}

FXFixed FHLL_sqrt(FXFixed f)
/****************************************************************************
*
* Function:     FHLL_sqrt
* Parameters:   f   - Number to take the square root of
* Returns:      Approximate square root of the number f
*
* Description:  Caculates the square root of a fixed point number using
*               table lookup and linear interpolation.
*
*               First we isolate the first 8 bits of the mantissa in our
*               fixed point number. We do this by scanning along until we
*               find the first 1 bit in the number, and shift it all right
*               until this is in bit position 7. Since IEEE floating point
*               numbers have an implied 1 bit in the mantissa, we mask this
*               bit out and use the 7 bits as an index into the table. The
*               8th bit is a 1 if the exponent of our fixed point number
*               was odd (position of the binary point with respect to the
*               first 1 bit in the number).
*
*               Because of the linear interpolation, and the fact that we
*               store 24 bits of precision in the lookup table, this
*               routine will provide a square root of any 16.16 fixed point
*               number that is as good as you can get given the precision
*               of fixed point (approx 1e-4 deviation).
*
****************************************************************************/
{
    short   e,index;                // Exponent and index into table
    FXFixed r,diff,interpolant;

    if (f <= 0)                     // Check for -ve and zero
        return 0;

    // Search for the index of the first 1 bit in the number (start of
    // the mantissa. Note that we are only working with positive numbers
    // here, so we ignore the sign bit (bit 31).
    e = 14;                         // Exponent for number with 1 in bit
                                    // position 30
    while((f & 0x40000000) == 0) {  // Isolate first bit
        f <<= 1;                    // Shift all left 1
        e--;                        // Decrement exponent for number
        }

    // At this stage our number is in the following format:
    //
    //   bits 23-30        15-22       0-14
    //  +-------------+-------------+---------+
    //  |.table index.|.interpolant.|.ignored.|
    //  +-------------+-------------+---------+
    //
    // We compute the index into the table by shifting the mantissa
    // so that the first 1 bit ends up in bit position 7, and mask it
    // out. If the exponent is odd then we index into the second half
    // of the table. The interpolant factor that we use is the bottom 16
    // bits left in the original number after the index is extracted out,
    // and is used to linearly interpolate the results between the two
    // consecutive entries in the table.
    index = (short)(f >> 23);
    interpolant = (f >> 7) & 0xFFFF;
    if (!(e & 1))
        index &= 0x7F;

    // Divide the exponent by two (square root it) while preserving the
    // sign (shift right in C preserves the sign), then construct the
    // square root by combining the mantissa from the lookup table and
    // the square rooted exponent. To do this we shift it the number of
    // places indicated by the exponent. A positive exponent means shift
    // left, negative means shift right. Note also that the lookup table
    // contains 23 bits of precision, with the implied 1 bit already
    // stored in the table.
    e >>= 1;
    e -= 7;
    diff = FXsqrt_table[index+1] - (r = FXsqrt_table[index]);
    if (e >= 0) {
        r <<= e;
        diff <<= e;
        }
    else {
        r >>= -e;
        diff >>= -e;
        }

    // Add in interpolated difference between this and the next value in
    // the table.
    r += FXmul(diff,interpolant);

    return r;
}

FXFixed FHLL_sin(FXFixed angle)
/****************************************************************************
*
* Function:     FHLL_sin
* Parameters:   angle   - Integer angle in degrees
* Returns:      FXFixed point representing the sine of the angle
*
* Description:  Routine to compute the sine of an angle and return the
*               result in FXFixed point. We use table lookup to find the
*               closest values, and then interpolate the result to get
*               the accuracy we need.
*
****************************************************************************/
{
    int     index;
    FXFixed a,r,diff;

    // Ensure that the angle is within the range 0 - 360
    while (angle < 0)
        angle += REAL(360);
    while (angle > REAL(360))
        angle -= REAL(360);

    // Convert angle so that 90 degrees = 256. This gives us the following
    // as the resulting number:
    //
    //   bits 25-26     16-24         0-15
    //  +----------+-------------+-------------+
    //  |.quadrant.|.table index.|.interpolant.|
    //  +----------+-------------+-------------+
    a = angle = FXmul(angle, REAL(256.0 / 90.0));
    index = (a >> 16) & 0xFF;
    if (angle & 0x1000000L) {               // Handle quadrants 1 & 3
        a = ~a;
        index ^= 0xFF;
        }

    // Find the value at the index, and the difference between it and the
    // next value in the table.
    diff = FXsin_table[index+1] - (r = FXsin_table[index]);

    // Now add in the linearly interpolated difference to the result
    r += FXmul(diff,a & 0xFFFFL);

    if (angle & 0x2000000L)                 // Handle quadrants 3 & 4
        r = -r;
    return r;
}

FXFixed FHLL_cos(FXFixed angle)
/****************************************************************************
*
* Function:     FHLL_cos
* Parameters:   angle   - FXFixed point angle in degrees
* Returns:      FXFixed point representing the cosine of the angle
*
* Description:  Routine to compute the cosine of an angle and return the
*               result in FXFixed point.
*
****************************************************************************/
{
    return FXsin(REAL(90) + angle);     // Simply return sin(90 + angle)
}

FXFixed FHLL_tan(FXFixed angle)
/****************************************************************************
*
* Function:     FHLL_tan
* Parameters:   f   - FXFixed point angle in degree's
* Returns:      FXFixed point representing the tangent of the angle
*
****************************************************************************/
{
    // Ensure that the angle is within the range 0 - 360
    while (angle < 0)
        angle += REAL(360);
    while (angle > REAL(360))
        angle -= REAL(360);

    if (angle == REAL(90) || angle == REAL(270))
        return 0x7FFFFFFFL;
    return FXdiv(FXsin(angle),FXsin(REAL(90)+angle));
}

FXFixed FHLL_acos(FXFixed f)
/****************************************************************************
*
* Function:     FHLL_acos
* Parameters:   f   - FXFixed point value to find arc cosine of
* Returns:      FXFixed point angle representing the arc cosine of the value
*
****************************************************************************/
{
    return REAL(90) - FXasin(f);
}

FXFixed FHLL_atan(FXFixed f)
/****************************************************************************
*
* Function:     FHLL_atan
* Parameters:   f   - FXFixed point value to find arc tangent of
* Returns:      FXFixed point angle representing the arc tangent of the value
*
****************************************************************************/
{
    return FXatan2(f,REAL(1));
}

FXFixed FHLL_log10(FXFixed f)
/****************************************************************************
*
* Function:     FHLL_log10
* Parameters:   f   - Number to take the square root of
* Returns:      Approximate square root of the number f
*
* Description:  Caculates the common logarithm of a fixed point number
*               using table lookup and linear interpolation.
*
*               First we isolate the first 8 bits of the mantissa in our
*               fixed point number. We do this by scanning along until we
*               find the first 1 bit in the number, and shift it all right
*               until this is in bit position 7. Since IEEE floating point
*               numbers have an implied 1 bit in the mantissa, we mask this
*               bit out and use the 7 bits as an index into the table. We
*               then look up this value, and add in the appropriate logarithm
*               for the power for two represented by the numbers exponent.
*
*               Because of the linear interpolation, this routine will
*               provide a common logarithm of any 16.16 fixed point
*               number that is as good as you can get given the precision
*               of fixed point (approx 1e-4 deviation).
*
****************************************************************************/
{
    short   e,index;                // Exponent and index into table
    FXFixed r,diff,interpolant;

    if (f <= 0)                     // Check for -ve and zero
        return 0;

    // Search for the index of the first 1 bit in the number (start of
    // the mantissa. Note that we are only working with positive numbers
    // here, so we ignore the sign bit (bit 31).
    e = 14;                         // Exponent for number with 1 in bit
                                    // position 30
    while((f & 0x40000000) == 0) {  // Isolate first bit
        f <<= 1;                    // Shift all left 1
        e--;                        // Decrement exponent for number
        }

    // At this stage our number is in the following format:
    //
    //   bits 23-30        15-22       0-14
    //  +-------------+-------------+---------+
    //  |.table index.|.interpolant.|.ignored.|
    //  +-------------+-------------+---------+
    //
    // We compute the index into the table by shifting the mantissa
    // so that the first 1 bit ends up in bit position 7, and mask it
    // out. The interpolant factor that we use is the bottom 16
    // bits left in the original number after the index is extracted out,
    // and is used to linearly interpolate the results between the two
    // consecutive entries in the table.
    index = (short)(f >> 23) & 0x7F;
    interpolant = (f >> 7) & 0xFFFF;

    // Lookup the values for the 7 bits of mantissa in the table, and
    // linearly interpolate between the two entries.
    diff = FXlog10_table[index+1] - (r = FXlog10_table[index]);
    r += FXmul(diff,interpolant);

    // Now find the appropriate power of 2 logarithm to add to the final
    // result.
    if (e < 0)
        r -= FXlog2_table[-e];
    else r += FXlog2_table[e];

    return r;
}

FXFixed FHLL_log(FXFixed f)
/****************************************************************************
*
* Function:     FHLL_log
* Parameters:   f   - FXFixed point value to find natural logarithm of
* Returns:      FXFixed point value representing the natrual logarithm of f
*
****************************************************************************/
{
    return FXmul(FXlog10(f),REAL(2.302585093));
}

// These routines have yet to be written specifically for fixed point, so
// we simply emulate them via floating point for the time being.

FXFixed FHLL_asin(FXFixed f)
{
    return FXdblToReal(asin(FXrealToDbl(f)) * 180 / M_PI);
}

FXFixed FHLL_atan2(FXFixed x,FXFixed y)
{
    return FXdblToReal(atan2(FXrealToDbl(x),FXrealToDbl(y)) * 180 / M_PI);
}

#endif  // FIXEDHLL

#ifdef  FIXED386

FXFixed _ASMAPI F386_asin(FXFixed f)
{
    return FXdblToReal(asin(FXrealToDbl(f)) * 180 / M_PI);
}

FXFixed _ASMAPI F386_atan2(FXFixed x,FXFixed y)
{
    return FXdblToReal(atan2(FXrealToDbl(x),FXrealToDbl(y)) * 180 / M_PI);
}

FXFixed _ASMAPI F386_acos(FXFixed f)
{
    return REAL(90) - FXasin(f);
}

FXFixed _ASMAPI F386_atan(FXFixed f)
{
    return FXatan2(f,REAL(1));
}

#endif

// For the time being we dont have fast fixed point algorithms for
// the following, so we will simply do it via the standard runtime
// library routines (which works but is slow).

FXFixed FXpow(FXFixed x,FXFixed y)
{
    return FXdblToReal(pow(FXrealToDbl(x),FXrealToDbl(y)));
}
