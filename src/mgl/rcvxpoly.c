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
* Description:  Convex polygon region generation routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*--------------------------- Global Variables ----------------------------*/

static region_t         *rgn;
static span_t           *head;
static span_t           **sp;
static int              minx,miny,maxx,maxy;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Scans a flat shaded trapezoid into a complex region structure
****************************************************************************/
static void MGLAPI _MGL_regionTrap(
    GA_trap *trap)
{
    fix32_t x1 = trap->x1;
    fix32_t x2 = trap->x2;
    fix32_t slope1 = trap->slope1;
    fix32_t slope2 = trap->slope2;
    int     ix1,ix2,y = trap->y,count = trap->count;
    span_t  *st;

    while (count--) {
        ix1 = MGL_FIXROUND(x1);
        ix2 = MGL_FIXROUND(x2);
        if (ix2 < ix1)
            SWAP(ix1,ix2);
        if (ix1 < ix2) {
            /* Create a new span for this scanline and link it at the
             * end of the list.
             */
            miny = MIN(miny,y);
            maxy = MAX(maxy,y);
            minx = MIN(minx,ix1);
            maxx = MAX(maxx,ix2);
            *sp = st = _MGL_createSpan(y,ix1,ix2);
            st->next = NULL;
            sp = &st->next;
            }
        x1 += slope1;
        x2 += slope2;
        y++;
        }
    trap->x1 = x1;
    trap->x2 = x2;
    trap->y = y;
}

/****************************************************************************
DESCRIPTION:
Generates a solid convex polygonal region.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
This function generates a complex region that represents a convex polygon.
A "convex" polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right and left edges may cross (polygons
may be nonsimple). Attempting to scan convert a polygon that does non fit
this description will produce unpredictable results.

Note:   All vertices are offset by (xOffset,yOffset) and are in 16.16
        fixed point format.

SEE ALSO:
MGL_rgnPolygonCnvx, MGL_rgnPolygon
****************************************************************************/
region_t * MGLAPI MGL_rgnPolygonCnvxFX(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    /* Initialise the span list */
    rgn = MGL_newRegion();
    head = NULL;
    sp = &head;

    /* Now build the region spans from the scratch buffer */
    minx = miny = 32767;
    maxx = maxy = -32767;
    _MGL_convexPolygon(count,vArray,vinc,xOffset,yOffset,_MGL_regionTrap);
    rgn->rect.left = minx;
    rgn->rect.right = maxx;
    rgn->rect.top = miny;
    rgn->rect.bottom = maxy+1;
    rgn->spans = head;
    return rgn;
}

/****************************************************************************
DESCRIPTION:
Generates a solid convex polygonal region.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
This function generates a complex region that represents a convex polygon.
A "convex" polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right and left edges may cross (polygons
may be nonsimple). Attempting to scan convert a polygon that does non fit
this description will produce unpredictable results.

Note:   All vertices are offset by (xOffset,yOffset) and are in regular
        integer format.

SEE ALSO:
MGL_rgnPolygonCnvx, MGL_rgnPolygon
****************************************************************************/
region_t * MGLAPI MGL_rgnPolygonCnvx(
    int count,
    point_t *vArray,
    int vinc,
    int xOffset,
    int yOffset)
{
    return MGL_rgnPolygonCnvxFX(count,_MGL_copyToFixed(count,vArray),
        sizeof(fxpoint_t),MGL_TOFIX(xOffset),MGL_TOFIX(yOffset));
}

