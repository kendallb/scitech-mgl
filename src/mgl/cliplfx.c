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
*                   Portions Copyright (c) 1987 X Consortium
*           Portions Copyright 1987 by Digital Equipment Corporation,
*                           Maynard, Massachusetts.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Fixed point line clipping routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */
#include "clipline.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
x1          - X coordinate of first endpoint to clip (fixed point)
y1          - Y coordinate of first endpoint to clip (fixed point)
x2          - X coordinate of second endpoint to clip (fixed point)
y2          - Y coordinate of second endpoint to clip (fixed point)
clipLeft    - Left coordinate of clip rectangle (integer)
clipTop     - Top coordinate of clip rectangle (integer)
clipRight   - Right coordinate of clip rectangle (integer)
clipBottom  - Bottom coordinate of clip rectangle (integer)

REMARKS:
Internal function to clip and draw a fixed point line, such that the clipped
line segment draws the *exact* same set of pixels that the unclipped line
would have drawn. We also correctly handle arbitrarily large lines, while
still allowing the clipped line segment to be drawn in hardware.
{secret}
****************************************************************************/
void _MGL_drawClippedLineFX(
    fix32_t x1,
    fix32_t y1,
    fix32_t x2,
    fix32_t y2,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    fix32_t absDeltaX,absDeltaY,initialError,majorInc,diagInc;
    fix32_t clipdx,clipdy;
    int     new_x1,new_y1,new_x2,new_y2;
    int     count,flags,clip1,clip2,outcode1,outcode2;

    /* Find integer starting coordinates */
    new_x1 = MGL_FIXROUND(x1);
    new_y1 = MGL_FIXROUND(y1);
    new_x2 = MGL_FIXROUND(x2);
    new_y2 = MGL_FIXROUND(y2);

    /* Calculate bresenham parameters */
    flags = gaLineXPositive | gaLineYPositive | gaLineXMajor | gaLineDoLastPel;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        flags &= ~gaLineXPositive;
        }
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        flags &= ~gaLineYPositive;
        }
    if (absDeltaX > absDeltaY) {
        majorInc = absDeltaY * 2;
        diagInc = majorInc - absDeltaX * 2;
        initialError = majorInc - absDeltaX;
        }
    else {
        majorInc = absDeltaX * 2;
        diagInc = majorInc - absDeltaY * 2;
        initialError = majorInc - absDeltaY;
        flags &= ~gaLineXMajor;
        }

    /* Clip the line and reject it if nothing is to be drawn */
    clip1 = clip2 = outcode1 = outcode2 = 0;
    OUTCODES(outcode1,new_x1,new_y1);
    OUTCODES(outcode2,new_x2,new_y2);
    if (_MGL_clipLine(clipLeft,clipTop,clipRight-1,clipBottom-1,
            &new_x1, &new_y1, &new_x2, &new_y2,
            abs(new_x2 - new_x1), abs(new_y2 - new_y1), &clip1, &clip2,
            flags, 0, outcode1, outcode2) == -1) {
        /* Trivial rejection */
        return;
        }
    if (flags & gaLineXMajor)
        count = abs(new_x2 - new_x1)+1;
    else
        count = abs(new_y2 - new_y1)+1;

    /* Now draw the clipped line */
    if (count) {
        /* Unwind bresenham error term to first point */
        clipdx = abs(N_TOFIX(new_x1) - x1);
        clipdy = abs(N_TOFIX(new_y1) - y1);
        if (flags & gaLineXMajor)
            initialError += MGL_FixMul(clipdy,diagInc) + MGL_FixMul((clipdx-clipdy),majorInc);
        else
            initialError += MGL_FixMul(clipdx,diagInc) + MGL_FixMul((clipdy-clipdx),majorInc);
        if (DC.a.lineStyle == MGL_LINE_STIPPLE)
            DC.r.DrawBresenhamStippleLine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags,DC.a.backMode == MGL_TRANSPARENT_BACKGROUND);
        else
            DC.r.DrawBresenhamLine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags);
        }
}
