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
* Description:  Random number generation routines. These are normally
*               written in assembly language because of the need to work
*               with 64 bit intermediate results.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#if defined(NO_ASSEMBLER)

/****************************************************************************
DESCRIPTION:
Reseed MGL random number generator.

HEADER:
mgraph.h

PARAMETERS:
seed    - New seed value for the random number generator

REMARKS:
This function reseeds the random number generator to start generating a new
sequence of numbers. Generally this function is used to randomize the generator by
seeding it with the value obtained from the MGL_getTicks function.

SEE ALSO:
MGL_random, MGL_randoml
****************************************************************************/
void MGLAPI MGL_srand(
    uint seed)
{ srand(seed); }

/****************************************************************************
DESCRIPTION:
Generate a random 16-bit number between 0 and max.

HEADER:
mgraph.h

PARAMETERS:
max - Largest desired value

RETURNS:
Random 16-bit number between 0 and max.

SEE ALSO:
MGL_randoml, MGL_srand
****************************************************************************/
ushort MGLAPI MGL_random(
    ushort max)
{ return (rand() % max); }

/****************************************************************************
DESCRIPTION:
Generate a random 32-bit number between 0 and max.

HEADER:
mgraph.h

PARAMETERS:
max - Largest desired value

RETURNS:
Random 32-bit number between 0 and max.

SEE ALSO:
MGL_random, MGL_srand
****************************************************************************/
ulong MGLAPI MGL_randoml(
    ulong max)
{ return ( (((ulong)rand() << 17) | ((ulong)rand() << 1)) % max); }

#endif
