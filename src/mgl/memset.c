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
* Description:  Routines for clearing memory blocks.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

#if defined(NO_ASSEMBLER) || !defined(__INTEL__)

#ifndef __WATCOMC__

/****************************************************************************
DESCRIPTION:
Copies a block of memory as fast as possible.

HEADER:
mgraph.h

PARAMETERS:
dst - Pointer to destination block
src - Pointer to source block
n   - Number of bytes to copy

REMARKS:
This function copies a block of memory as fast as possible, and has been optimized
to copy the data 32 bits at a time for maximum performance. This function is
similar to the standard C library memcpy function, but can correctly handle copying
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments. Note also that this function is generally a lot faster than some
standard C library functions.

SEE ALSO:
MGL_memcpyVIRTSRC, MGL_memcpyVIRTDST
****************************************************************************/
void MGLAPI MGL_memcpy(
    void *dst,
    void *src,
    int n)
{ memcpy(dst,src,n); }

#endif

/****************************************************************************
DESCRIPTION:
Copies a block of memory as fast as possible.


HEADER:
mgraph.h

PARAMETERS:
dst - Pointer to destination block
src - Pointer to source block
n   - Number of bytes to copy

REMARKS:
This function copies a block of memory as fast as possible, and has been optimized
to copy the data 32 bits at a time for maximum performance. This function is
similar to the standard C library memcpy function, but can correctly handle copying
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments. Note also that this function is generally a lot faster than some
standard C library functions.

This function is identical to MGL_memcpy except that it is virtual linear
framebuffer safe, and should be used for copying data where the source pointer
resides in a virtualized linear surface.

SEE ALSO:
MGL_memcpy, MGL_memcpyVIRTDST
****************************************************************************/
void MGLAPI MGL_memcpyVIRTSRC(
    void *dst,
    void *src,
    int n)
{ memcpy(dst,src,n); }

/****************************************************************************
DESCRIPTION:
Copies a block of memory as fast as possible.

HEADER:
mgraph.h

PARAMETERS:
dst - Pointer to destination block
src - Pointer to source block
n   - Number of bytes to copy

REMARKS:
This function copies a block of memory as fast as possible, and has been optimized
to copy the data 32 bits at a time for maximum performance. This function is
similar to the standard C library memcpy function, but can correctly handle copying
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments. Note also that this function is generally a lot faster than some
standard C library functions.

This function is identical to MGL_memcpy except that it is virtual linear
framebuffer safe, and should be used for copying data where the destination pointer
resides in a virtualized linear surface.

SEE ALSO:
MGL_memcpyVIRTSRC, MGL_memcpy
****************************************************************************/
void MGLAPI MGL_memcpyVIRTDST(
    void *dst,
    void *src,
    int n)
{ memcpy(dst,src,n); }

/****************************************************************************
DESCRIPTION:
Clears a memory block with 8-bit values.

HEADER:
mgraph.h

PARAMETERS:
p   - Pointer to block to clear
c   - Value to clear with
n   - Number of bytes to clear

REMARKS:
This function clears a memory block to the specified 8 bit value. This function is
similar to the standard C library memset function, but can correctly handle clearing
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments.

SEE ALSO:
MGL_memsetw, MGL_memsetl
****************************************************************************/
void MGLAPI MGL_memset(
    void *p,
    int c,
    long n)
{
    uchar *p1 = p;
    while (n--)
        *p1++ = c;
}

/****************************************************************************
DESCRIPTION:
Clears a memory block with 16-bit values.

HEADER:
mgraph.h

PARAMETERS:
p   - Pointer to block to clear
c   - Value to clear with
n   - Number of words to clear

REMARKS:
This function clears a memory block to the specified 16 bit value. This function is
similar to the standard C library memset function, but can correctly handle clearing
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments, and allows you to specify a specific 16 bit value to be cleared.

SEE ALSO:
MGL_memset, MGL_memsetl
****************************************************************************/
void MGLAPI MGL_memsetw(
    void *p,
    int c,
    long n)
{
    ushort *p1 = p;
    while (n--)
        *p1++ = c;
}

/****************************************************************************
DESCRIPTION:
Clears a memory block with 32-bit values.

HEADER:
mgraph.h

PARAMETERS:
p   - Pointer to block to clear
c   - Value to clear with
n   - Number of dwords to clear

REMARKS:
This function clears a memory block to the specified 32 bit value. This function is
similar to the standard C library memset function, but can correctly handle clearing
of memory blocks that are larger than 64Kb in size for 16 bit real mode
environments, and allows you to specify a specific 32 bit value to be cleared.

SEE ALSO:
MGL_memset, MGL_memsetw
****************************************************************************/
void MGLAPI MGL_memsetl(
    void *p,
    long c,
    long n)
{
    long *p1 = p;
    while (n--)
        *p1++ = c;
}

#endif
