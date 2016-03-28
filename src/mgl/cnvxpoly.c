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
* Description:  Convex polygon drawing routines with clipping. Polygon
*               clipping is done on a scissoring basis, where each scan
*               line is clipped individually, rather than using a routine
*               such as the Sutherland Hodgeman polygon clipping algorithm.
*               This approach has been taken since we don't expect to clip
*               polygons all that often, and those that are clipped will
*               only require a small amount of clipping.
*
*               This algorithm is a lot faster than the more general complex
*               polygon scan conversion algorithm, since we can take
*               advantage of the fact that every scan line in the polygon
*               will only cross a maximum of two actives edges in the
*               polygon at a time.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of x coordinates
yOffset - Offset of y coordinates

REMARKS:
Scan converts a filled convex polygon. A "convex" polygon is defined as a
polygon such that eveyr horizontal line drawn through the polygon would
cross exactly two active edges (neither horizontal lines nor zero-length
edges count as active edges; both are acceptable anywhere in the polygon).
Right & left edges may cross (polygons may be nonsimple). Attempting to
scan convert a polygon that does non fit this description will produce
unpredictable results.

All vertices are offset by (xOffset,yOffset).
{secret}
****************************************************************************/
void _MGL_convexPolygon(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset,
    trapFunc trap)
{
    int         i,minIndex;
    fxpoint_t   *p,*v,*lArray,*rArray;
    fix32_t     y,endy;

    /* Get memory in which to store left and right edge arrays          */
    lArray = _MGL_buf;
    rArray = (fxpoint_t*)((uchar*)_MGL_buf + (count+1) * sizeof(fxpoint_t));

    /* Scan the list of vertices to determine the lowest vertex         */
    minIndex = 0;
    y = vArray->y;
    for (i = 1, p = VTX(vArray,vinc,1); i < count; i++, INCVTX(p,vinc)) {
        if (p->y < y) {
            y = p->y;
            minIndex = i;
            }
        }

    /* Copy all vertices from lowest to end into left list          */
    v = VTX(vArray,vinc,minIndex);
    for (i = minIndex,p = lArray; i < count; i++,INCVTX(v,vinc))
        *p++ = *v;

    /* Copy all vertices from start to lowest into left list            */
    for (i = 0,v = vArray; i < minIndex; i++,INCVTX(v,vinc))
        *p++ = *v;
    p->y = -1;                      /* Terminate the list               */

    /* Copy the right list as the left list in reverse order */
    p = rArray;
    *p++ = *lArray;                 /* First element the same           */
    for (i = count-1; i > 0; i--)
        *p++ = lArray[i];
    p->y = -1;                      /* Terminate the list               */

    /* Setup for rendering the first edge in left list */
    while (_MGL_computeSlope(lArray,lArray+1,&DC.tr.slope1) <= 0) {
        lArray++;
        if ((--count) == 0)
            return;                 /* Bail out for zero height polys   */
        }

    /* Setup for rendering the first edge in right list */
    while (_MGL_computeSlope(rArray,rArray+1,&DC.tr.slope2) <= 0)
        rArray++;

    /* Now render the polygon as a series of trapezoidal slices         */
    DC.tr.x1 = lArray->x+xOffset;
    DC.tr.x2 = rArray->x+xOffset;
    y = lArray->y;
    DC.tr.y = MGL_FIXROUND(y+yOffset);
    for (;;) {
        endy = lArray[1].y;
        if (endy > rArray[1].y)
            endy = rArray[1].y;

        /* Draw the trapezoid */
        DC.tr.count = MGL_FIXROUND(endy) - MGL_FIXROUND(y);
        trap(&DC.tr);

        /* Advance the left and right edges */
        if (lArray[1].y == endy) {  /* Has left edge ended?             */
            lArray++;
            while ((count = _MGL_computeSlope(lArray,lArray+1,&DC.tr.slope1)) <= 0) {
                lArray++;
                if (count < 0)
                    return;         /* No more left edges               */
                }
            DC.tr.x1 = lArray->x+xOffset;
            }
        if (rArray[1].y == endy) {  /* Has right edge ended?            */
            rArray++;
            while ((count = _MGL_computeSlope(rArray,rArray+1,&DC.tr.slope2)) <= 0) {
                rArray++;
                if (count < 0)
                    return;         /* No more right edges              */
                }
            DC.tr.x2 = rArray->x+xOffset;
            }
        y = endy;
        }
}
