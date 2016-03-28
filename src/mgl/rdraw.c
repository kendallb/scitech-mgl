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
* Description:  Scanline coherent shape algebra routines for implementing
*               complex clipping paths in the MGL.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
x   - x coordinate to draw region at
y   - y coordinate to draw region at
r   - region to draw

REMARKS:
Draws the representation of the region at the specified location a scanline
at a time. The region is rendered in the current bitmap pattern and write
mode, and rendered without any clipping. When this routine gets called, it
is assumed that the region has already been clipped by the high level code
and will be rendered directly in screen space. This routine also assumes
that the region definition is valid and is not a special cased rectangle.
{secret}
****************************************************************************/
void __MGL_drawRegion(
    int x,
    int y,
    const region_t *r)
{
    segment_t   *seg;
    span_t      *s = r->spans;
    int         sy;

    y += (sy = s->y);
    while (s) {
        for (seg = s->seg; seg; seg = seg->next->next)
            DC.r.cur.DrawRect(x + seg->x,y,seg->next->x - seg->x,1);
        if (!s->next || (++sy == s->next->y))
            s = s->next;
        y++;
        }
}

/****************************************************************************
DESCRIPTION:
Draw a solid complex region.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to draw region at
y   - y coordinate to draw region at
rgn - region to draw

REMARKS:
Draws the complex region at the specified location in the current pattern
and write mode.

SEE ALSO:
MGL_newRegion, MGL_copyRegion, MGL_freeRegion, MGL_diffRegion,
MGL_unionRegion,  MGL_sectRegion
****************************************************************************/
void MGLAPI MGL_drawRegion(
    int x,
    int y,
    const region_t *rgn)
{
    region_t    *clipped = (region_t*)rgn,*tempRgn,*drawRgn;
    rect_t      r;

    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (MGL_sectRect(DC.clipRectView,rgn->rect,&r)) {
        if (!MGL_equalRect(rgn->rect,r)) {
            _MGL_tmpRectRegion2(tempRgn,DC.clipRectView);
            clipped = MGL_sectRegion(rgn,tempRgn);
            if (MGL_emptyRect(clipped->rect))
                goto Done;
            }
        }
    else {
        /* Trivially reject region */
        goto Done;
        }

    if (!clipped->spans) {
        /* Region is a special case rectangle, so render it directly */
        MGL_fillRectCoord(x+clipped->rect.left,y+clipped->rect.top,
            clipped->rect.right-clipped->rect.left,
            clipped->rect.bottom-clipped->rect.top);
        }
    else {
        if (DC.clipRegionScreen) {
            /* If we have a complex clip region, intersect the region
             * to draw with the complex clip region in screen space
             * and then draw it.
             */
            tempRgn = MGL_copyRegion(clipped);
            MGL_offsetRegion(tempRgn,DC.viewPort.left,DC.viewPort.top);
            drawRgn = MGL_sectRegion(tempRgn,DC.clipRegionScreen);
            __MGL_drawRegion(x,y,drawRgn);
            MGL_freeRegion(drawRgn);
            MGL_freeRegion(tempRgn);
            }
        else
            __MGL_drawRegion(DC.viewPort.left + x, DC.viewPort.top + y,clipped);
        }

Done:
    END_VISIBLE_CLIP_LIST(&DC);
    if (clipped != rgn)
        MGL_freeRegion(clipped);
}

