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
* Description:  Polygon drawing routines. Polygon clipping is
*               done on a scissoring basis, where each scan line is
*               clipped individually, rather than using a routine such as
*               the Sutherland Hodgeman polygon clipping algorithm. This
*               approach has been taken since we don't expect to clip
*               polygons all that often, and those that are clipped will
*               only require a small amount of clipping.
*
*               Clipping algorithms such as the Sutherland Hodgeman
*               algorithm can be implemented if needed in a higher level
*               graphics library.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Renders a flat shaded trapezoid with clipping.
{secret}
****************************************************************************/
void MGLAPI _MGL_clipTrap(
    GA_trap *trap)
{
    fix32_t x1 = trap->x1;
    fix32_t x2 = trap->x2;
    fix32_t slope1 = trap->slope1;
    fix32_t slope2 = trap->slope2;
    int ix1,ix2,y = trap->y,count = trap->count;

    while (count--) {
        if (y >= DC.clipRectScreen.top && y < DC.clipRectScreen.bottom) {
            ix1 = MGL_FIXROUND(x1);
            ix2 = MGL_FIXROUND(x2);
            if (ix2 < ix1)
                SWAP(ix1,ix2);
            ix1 = MAX(ix1,DC.clipRectScreen.left);
            ix2 = MIN(ix2,DC.clipRectScreen.right);
            if (ix1 < ix2)
                DC.r.cur.DrawRect(ix1,y,ix2-ix1,1);
            }
        x1 += slope1;
        x2 += slope2;
        y++;
        }
    trap->x1 = x1;
    trap->x2 = x2;
    trap->y = y;
}

#define SIGNUM(a) (((a) > 0) ? 1: (((a) < 0) ? -1 : 0))

/****************************************************************************
PARAMETERS:
count   - Number of vectices in polyon
vArray  - Array of vertices in polygon
vinc    - Increment to get to next vertex

RETURNS:
True if the polygon is convex, or 'Monotone vertical'

REMARKS:
Classifies the type of polygon that we are trying to scan convert. We
attempt to determine if the polygon is monotone with respect to a vertical
line. This means that any horizontal line drawn through the polygon will
ONLY intersect two edges at a time.

We do this by simply scanning around the perimeter of the polygon looking
for exactly two direction reversals.
****************************************************************************/
static ibool isConvex(
    int count,
    fxpoint_t *vArray,
    int vinc)
{
    int     i,DYSign,PrevDYSign;
    int     YReversals = 0;
    fix32_t temp;

    /* A polygon with three or fewer points is always convex */
    if (count < 4)
        return true;

    /* Scan to the first non-horizontal edge */
    temp = VTX(vArray,vinc,count-1)->y - vArray->y;
    PrevDYSign = SIGNUM(temp);
    i = 0;
    while ((PrevDYSign == 0) && (i < (count-1))) {
        temp = vArray->y - VTX(vArray,vinc,1)->y;
        PrevDYSign = SIGNUM(temp);
        i++;
        INCVTX(vArray,vinc);
        }
    if (i == (count-1))
        return true;    /* polygon is a flat line */

    /* Now count Y reversals. Might miss one reversal, at the last vertex,
     * but because reversal counts must be even, being off by one isn't a
     * problem.
     */
    do {
        temp = vArray->y - VTX(vArray,vinc,1)->y;
        if ((DYSign = SIGNUM(temp)) != 0) {
            if (DYSign != PrevDYSign) {
                /* Switched Y direction; not vertical-monotone if
                 * reversed Y direction as many as three times
                 */
                if (++YReversals > 2)
                    return false;
                PrevDYSign = DYSign;
                }
            }
        INCVTX(vArray,vinc);
        } while (i++ < (count-1));
    return true;
}

/****************************************************************************
PARAMETERS:
count   - Number of vertices to copy
vArray  - Polygon vertex array to convert

RETURNS:
Pointer to the array of converted vertices

REMARKS:
Converts an array of integer polygon vertices to fixed point format. The
fixed point verticees are stored at the end of the _MGL_buf buffer (to keep
out of the way of the main polygon rendering code).
{secret}
****************************************************************************/
fxpoint_t *_MGL_copyToFixed(
    int count,
    point_t *vArray)
{
    fxpoint_t *f,*fxvArray = f = (fxpoint_t*)(((char*)_MGL_buf + _MGL_bufSize)
        - (count * sizeof(fxpoint_t)));
    while (count--) {
        f->x = MGL_TOFIX(vArray->x);
        f->y = MGL_TOFIX(vArray->y);
        f++;
        vArray++;
        }
    return fxvArray;
}

/****************************************************************************
PARAMETERS:
count   - Number of polygon vertices
vArray  - Array of polygon vertices
vinc    - Increment to get to next vertex
xOffset - X coordinate offset for polygon
yOffset - Y coordinate offset for polygon

RETURNS:
0 for draw no clip, 1 for clip and draw, -1 for trival reject

REMARKS:
Determine if the polygon to be scan converted must be clipped, trivially
rejected or trivially accepted.
****************************************************************************/
static int _MGL_preClipPoly(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    fxpoint_t   *p;
    fix32_t     left,top,right,bottom;
    fix32_t     cleft,ctop,cright,cbottom;

    left = right = vArray->x;
    top = bottom = vArray->y;
    p = VTX(vArray,vinc,1);
    while (--count) {
        if (p->x < left)
            left = p->x;
        else if (p->x > right)
            right = p->x;
        if (p->y < top)
            top = p->y;
        else if (p->y > bottom)
            bottom = p->y;
        INCVTX(p,vinc);
        }
    left += xOffset;
    right += xOffset;
    cleft = MAX(left,DC.clipRectViewFX.left);
    cright = MIN(right,DC.clipRectViewFX.right);
    if (cleft >= cright)
        return -1;
    top += yOffset;
    bottom += yOffset;
    ctop = MAX(top,DC.clipRectViewFX.top);
    cbottom = MIN(bottom,DC.clipRectViewFX.bottom);
    if (ctop >= cbottom)
        return -1;
    if (cleft == left && ctop == top && cright == right && cbottom == bottom)
        return 0;
    return 1;
}

/****************************************************************************
DESCRIPTION:
Fills an arbitrary polygon.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices in polygon
vArray  - Array of vertices in polygon
vinc    - Increment to get to next vertex in bytes
xOffset - x coordinate offset value
yOffset - y coordinate offset value

REMARKS:
These routines rasterize a filled arbitrary polygon in the current color and
style. By default the routine will determine the type of the polygon being
rasterized, and will rasterize convex polygons using a faster scan
conversion routine, otherwise a general polygon scan conversion routine will
be used. Thus you can rasterize any type of polygon that you desire.

A convex polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right & left edges may cross (polygons
may be non-simple).

Non-simple or self intersecting polygons will be rasterized using the
standard in/out rule, where points are defined as being inside after
crossing the first edge in the polygon, and then alternate between defined
as inside then outside after crossing subsequent active edges in the polygon.

You may also use the MGL_setPolygonType routine to specify the type of
polygons being rasterized. This may be MGL_AUTO_POLYGON,
MGL_CONVEX_POLYGON or MGL_COMPLEX_POLYGON. Explicitly setting the polygon
type will speed the drawing process.

As with all MGL polygon rasterizing routines, this routine does not
rasterize the pixels down the right hand side or the bottom edges of the
polygon. This ensures that pixels along shared edges of polygons are not
rasterized twice, which can cause annoying pixel flashes in animation code.
Note also that the edges in the polygon will always be rasterized from top
to bottom, to ensure that all shared edges will actually generate the same
set of vertices, eliminating the possibility of pixel dropouts between
shared edges in polygons.

Note:   All vertices are offset by (xOffset,yOffset).

SEE ALSO:
MGL_setPolygonType
****************************************************************************/
void MGLAPI MGL_fillPolygonFX(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    region_t    *clipped,*poly;
    trapFunc    trap;

    __MGL_result = grOK;
    if (count < 3)
        return;                             /* reject empty polygons    */
    if ((DC.doClip = _MGL_preClipPoly(count,vArray,vinc,xOffset,yOffset)) == -1)
        return;
    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (DC.a.polyType == MGL_CONVEX_POLYGON ||
            (DC.a.polyType == MGL_AUTO_POLYGON && isConvex(count,vArray,vinc))) {
        if (DC.a.writeMode != MGL_REPLACE_MODE || DC.doClip)
            trap = _MGL_clipTrap;
        else
            trap = DC.r.cur.DrawTrap;
        if (DC.clipRegionScreen) {
            /* To simplify drawing and clipping of polygons with complex
             * clip regions, we simply generate a region that represents
             * the polygon and then clip that region against the complex
             * clip region.
             */
            poly = MGL_rgnPolygonCnvxFX(count,vArray,vinc,
                xOffset + DC.viewPortFX.left,
                yOffset + DC.viewPortFX.top);
            clipped = MGL_sectRegion(poly,DC.clipRegionScreen);
            if (!MGL_emptyRect(clipped->rect))
                __MGL_drawRegion(0,0,clipped);
            MGL_freeRegion(clipped);
            MGL_freeRegion(poly);
            }
        else {
            _MGL_convexPolygon(count,vArray,vinc,
                xOffset + DC.viewPortFX.left,
                yOffset + DC.viewPortFX.top,trap);
            }
        }
    else {
        if (DC.clipRegionScreen) {
            /* To simplify drawing and clipping of polygons with complex
             * clip regions, we simply generate a region that represents
             * the polygon and then clip that region against the complex
             * clip region.
             */
            poly = MGL_rgnPolygonFX(count,vArray,vinc,
                xOffset + DC.viewPortFX.left,
                yOffset + DC.viewPortFX.top);
            clipped = MGL_sectRegion(poly,DC.clipRegionScreen);
            if (!MGL_emptyRect(clipped->rect))
                __MGL_drawRegion(0,0,clipped);
            MGL_freeRegion(clipped);
            MGL_freeRegion(poly);
            }
        else {
            _MGL_complexPolygon(count,vArray,vinc,
                xOffset + DC.viewPortFX.left,
                yOffset + DC.viewPortFX.top);
            }
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Fills an arbitrary polygon.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices in polygon
vArray  - Array of vertices in polygon
xOffset - x coordinate offset value
yOffset - y coordinate offset value

REMARKS:
This function is provided for backwards compatibility, and expects the
array of points to be passed in as integers. Internally the MGL works in
fixed point, so this function simply converts the coordinates to fixed point
and passes them to the MGL_fillPolygonFX function. Hence MGL_fillPolygonFX is
more efficient, so you should use that version instead.

Note:   All vertices are offset by (xOffset,yOffset).

SEE ALSO:
MGL_setPolygonType
****************************************************************************/
void MGLAPI MGL_fillPolygon(
    int count,
    point_t *vArray,
    int xOffset,
    int yOffset)
{
    MGL_fillPolygonFX(count,_MGL_copyToFixed(count,vArray),
        sizeof(fxpoint_t),MGL_TOFIX(xOffset),MGL_TOFIX(yOffset));
}

/****************************************************************************
DESCRIPTION:
Scan converts a filled convex polygon.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
A "convex" polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right and left edges may cross (polygons
may be nonsimple). Attempting to scan convert a polygon that does non fit
this description will produce unpredictable results.

Note:   All vertices are offset by (xOffset,yOffset).

SEE ALSO:
MGL_fillPolygonFX
****************************************************************************/
void MGLAPI MGL_fillPolygonCnvxFX(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    region_t    *clipped,*poly;
    trapFunc    trap;

    if (count < 3)
        return;                             /* Reject empty polygons    */
    if ((DC.doClip = _MGL_preClipPoly(count,vArray,vinc,xOffset,yOffset)) == -1)
        return;                             /* Trivially reject polygon */
    if (DC.a.writeMode != MGL_REPLACE_MODE || DC.doClip)
        trap = _MGL_clipTrap;
    else
        trap = DC.r.cur.DrawTrap;

    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (DC.clipRegionScreen) {
        /* To simplify drawing and clipping of polygons with complex
         * clip regions, we simply generate a region that represents
         * the polygon and then clip that region against the complex
         * clip region.
         */
        poly = MGL_rgnPolygonCnvxFX(count,vArray,vinc,
            xOffset + DC.viewPortFX.left,
            yOffset + DC.viewPortFX.top);
        clipped = MGL_sectRegion(poly,DC.clipRegionScreen);
        if (!MGL_emptyRect(clipped->rect))
            __MGL_drawRegion(0,0,clipped);
        MGL_freeRegion(clipped);
        MGL_freeRegion(poly);
        }
    else {
        _MGL_convexPolygon(count,vArray,vinc,
            xOffset + DC.viewPortFX.left,
            yOffset + DC.viewPortFX.top,trap);
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Scan converts a filled convex polygon.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
This function is provided for backwards compatibility, and expects the
array of points to be passed in as integers. Internally the MGL works in
fixed point, so this function simply converts the coordinates to fixed point
and passes them to the MGL_fillPolygonCnvxFX function. Hence
MGL_fillPolygonCnvxFX is more efficient, so you should use that version
instead.

Note:   All vertices are offset by (xOffset,yOffset).

SEE ALSO:
MGL_fillPolygonCnvxFX
****************************************************************************/
void MGLAPI MGL_fillPolygonCnvx(
    int count,
    point_t *vArray,
    int xOffset,
    int yOffset)

{
    MGL_fillPolygonCnvxFX(count,_MGL_copyToFixed(count,vArray),
        sizeof(fxpoint_t),MGL_TOFIX(xOffset),MGL_TOFIX(yOffset));
}

