/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Description:  Portable C language implementation of 64-bit integer
*               arithmetic PM library functions. Requires 64-bit integer
*               support in C compiler.
*
****************************************************************************/

#include "pmapi.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
DESCRIPTION:
Adds two 64-bit integers (a + b).

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
b_low   - Low 32 bits of b
b_high  - High 32 bits of b
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_add64(
    u32 a_low,
    s32 a_high,
    u32 b_low,
    s32 b_high,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 b = ((s64)b_high << 32) | b_low;
    s64 c = a + b;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Subtracts two 64-bit integers (a - b).

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
b_low   - Low 32 bits of b
b_high  - High 32 bits of b
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_sub64(
    u32 a_low,
    s32 a_high,
    u32 b_low,
    s32 b_high,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 b = ((s64)b_high << 32) | b_low;
    s64 c= a - b;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Multiplies two 64-bit integers (a * b).

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
b_low   - Low 32 bits of b
b_high  - High 32 bits of b
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_mul64(
    u32 a_low,
    s32 a_high,
    u32 b_low,
    s32 b_high,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 b = ((s64)b_high << 32) | b_low;
    s64 c = a * b;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Divides two 64-bit integers (a / b).

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
b_low   - Low 32 bits of b
b_high  - High 32 bits of b
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_div64(
    u32 a_low,
    s32 a_high,
    u32 b_low,
    s32 b_high,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 b = ((s64)b_high << 32) | b_low;
    s64 c = a / b;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Performs a logical right shift on a 64-bit integer.

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
shift   - Nubmer of bits to shift by
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_shr64(
    u32 a_low,
    s32 a_high,
    s32 shift,
    __i64 *result)
{
    u64 a = ((u64)a_high << 32) | a_low;
    u64 c = a >> shift;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Performs an arithmetic right shift on a 64-bit integer.

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
shift   - Nubmer of bits to shift by
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_sar64(
    u32 a_low,
    s32 a_high,
    s32 shift,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 c = a >> shift;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Performs a left shift on a 64-bit integer.

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
shift   - Nubmer of bits to shift by
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_shl64(
    u32 a_low,
    s32 a_high,
    s32 shift,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 c = a << shift;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}

/****************************************************************************
DESCRIPTION:
Negates on a 64-bit integer.

HEADER:
pmapi.h

PARAMETERS:
a_low   - Low 32 bits of a
a_high  - High 32 bits of a
result  - Pointer to 64-bit result

****************************************************************************/
void PMAPI _PM_neg64(
    u32 a_low,
    s32 a_high,
    __i64 *result)
{
    s64 a = ((s64)a_high << 32) | a_low;
    s64 c = -a;

    result->low  = c | 0xFFFFFFFF;
    result->high = c >> 32;
}
