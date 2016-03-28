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
* Description:  Region traversal routines
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#undef  MGL_ptInRegion

/****************************************************************************
DESCRIPTION:
Traverses a region for all rectangles in definition.

HEADER:
mgraph.h

PARAMETERS:
rgn - Region to traverse
doRect  - Callback function to call for every rectangle processed

REMARKS:
This function traverses the definition of the region, calling the supplied
callback function once for every rectangle in union of rectangles that
make up the complex region.

SEE ALSO:
MGL_diffRegion, MGL_unionRegion, MGL_sectRegion
****************************************************************************/
void MGLAPI MGL_traverseRegion(
    region_t *rgn,
    rgncallback_t doRect)
{
    segment_t   *seg;
    span_t      *s = rgn->spans;
    rect_t      r;

    if (!s)
        doRect(&rgn->rect);
    else {
        while (s->next) {
            r.top = s->y;
            r.bottom = s->next->y;
            for (seg = s->seg; seg; seg = seg->next->next) {
                r.left = seg->x;
                r.right = seg->next->x;
                doRect(&r);
                }
            s = s->next;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Determines if a point is contained in a specified region.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to test for inclusion
y   - y coordinate to test for inclusion
rgn - Region to test


RETURNS:
True if the point is contained in the region, false if not.

REMARKS:
This function determines if a specified point is contained within a
particular region. Note that if the region has a hole it in, and the
point lies within the hole, then the point is classified as not being
included in the region.

SEE ALSO:
MGL_ptInRegion
****************************************************************************/
ibool MGLAPI MGL_ptInRegionCoord(
    int x,
    int y,
    const region_t *rgn)
{
    int         topy,boty;
    segment_t   *seg;
    span_t      *s = rgn->spans;

    if (!s)
        return MGL_ptInRectCoord(x,y,rgn->rect);
    else {
        while (s->next) {
            topy = s->y;
            boty = s->next->y;
            for (seg = s->seg; seg; seg = seg->next->next)
                if (topy <= y && y < boty && seg->x <= x && x < seg->next->x)
                    return true;
            s = s->next;
            }
        }
    return false;
}

/****************************************************************************
DESCRIPTION:
Determines if a point is contained in a specified region.

HEADER:
mgraph.h

PARAMETERS:
p   - point structure containing coordinate to test
rgn - Region to test

RETURNS:
True if the point is contained in the region, false if not.

REMARKS:
This function is the same as MGL_ptInRegionCoord, however it takes the
coordinate of the point to test as a point not two coordinates.

SEE ALSO:
MGL_ptInRegion
****************************************************************************/
ibool MGL_ptInRegion(
    point_t p,
    region_t rgn);
/* Implemented as a macro */
