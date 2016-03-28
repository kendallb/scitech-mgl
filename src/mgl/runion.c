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
* Description:  Module to implement the region union operation.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Computes the Boolean union of two regions and offsets the result.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region with which r2 is unioned, and also becomes the result region.
r2  - Region to be unioned with r1
xOffset - Offset to add to all x coordinates in region 2
yOffset - Offset to add to all y coordinates in region 2

RETURNS:
True if the union is valid, false if an empty region was created.

REMARKS:
Computes the Boolean union of two regions for the area covered by region r1
and region r2, computing the resulting region in r1, which may result in an
empty region. This routine also adds the specified (x,y) offset value to
all the coordinates in region 2 before they are unioned with region 1,
which allows you to quickly do a union with a translated region without
needing to explicitly translate the region itself. If you need to retain
the value of r1, you need to first copy r1 to a temporary region.

SEE ALSO:
MGL_diffRegion, MGL_sectRegion, MGL_unionRegion, MGL_unionRegionRect
****************************************************************************/
ibool MGLAPI MGL_unionRegionOfs(
    region_t *r1,
    const region_t *r2,
    int xOffset,
    int yOffset)
{
    int         yend;
    segment_t   *seg1,*seg2;        /* Pointers to segments             */
    span_t      *s1,*s2,*st;        /* Pointers to spans                */
    span_t      **rp;               /* Insert point for resultant list  */
    span_t      *start1;            /* Starting span in region1         */
    rect_t      rtmp;

    /* Compute offset bounding rectangle for r2 */
    rtmp.left = xOffset + r2->rect.left;
    rtmp.right = xOffset + r2->rect.right;
    rtmp.top = yOffset + r2->rect.top;
    rtmp.bottom = yOffset + r2->rect.bottom;

    /* Handle the case where either region is empty */
    s1 = r1->spans;
    s2 = r2->spans;
    if (!s1 && MGL_emptyRect(r1->rect)) {
        MGL_clearRegion(r1);
        r1->rect = rtmp;
        r1->spans = _MGL_copySpansOfs(r2->spans,xOffset,yOffset);
        return (r1->spans || !MGL_emptyRect(r1->rect));
        }
    if (!s2 && MGL_emptyRect(r2->rect))
        return (s1 || !MGL_emptyRect(r1->rect));

    /* Handle cases where either region is a rectangle and expand it */
    if ((s1 = r1->spans) == NULL)
        s1 = _MGL_rectRegion(r1);
    if ((s2 = r2->spans) == NULL) {
        _MGL_tmpRectRegion(r2);
        s2 = r2->spans;
        }

    /* Initialise result list and insert pointer    */
    r1->spans = NULL;
    rp = &r1->spans;
    seg1 = seg2 = NULL;

    /* Copy the initial spans before the first overlapping span in the
     * two regions. Because the r1 is the resultant region, we dont
     * actually copy the spans, but simply link them into the result list
     */
    yend = (yOffset + s2->y);
    if (s1->y < yend) {
        /* Copy over all inital spans in region 1 */
        while (s1 && s1->y < yend) {
            *rp = s1;
            rp = &s1->next;
            seg1 = s1->seg;
            s1 = s1->next;
            }
        }
    else if (yend < s1->y) {
        /* Copy over all inital spans in region 2 */
        while (s2 && (yOffset + s2->y) < s1->y) {
            *rp = st = _MGL_copySpanOfs(s2,xOffset,yOffset);
            rp = &st->next;
            seg2 = st->seg;
            s2 = s2->next;
            }
        /* Reset X offset to prevent duplication in subsequent scanlines */
        xOffset = 0;
        }

    /* Save the start of the overlapping region for region 1 */
    start1 = s1;

    /* Process the overlapping area between the two regions */
    while (s1 && s2) {
        int         flag,old,x,y,test;
        segment_t   **sp,*segment;
        segment_t   *p1,*p2,*pt;

        /* Compute the test value. The value is negative if the first
         * region is behind the second region, positive if the first shape
         * is ahead of the second one, and zero if they are in sync.
         */
        y = yOffset + s2->y;
        if ((test = s1->y - y) <= 0) {
            y = s1->y;
            seg1 = s1->seg;
            s1 = s1->next;
            }
        if (test >= 0) {
            seg2 = s2->seg;
            s2 = s2->next;
            }

        if (!seg1 && !seg2)
            segment = NULL;
        if (!seg1)
            segment = _MGL_copySegmentOfs(seg2,xOffset);
        else if (!seg2)
            segment = _MGL_copySegment(seg1);
        else {
            /* Both segments are non-empty, so compute their union */
            flag = old = 0;
            segment = NULL;
            sp = &segment;

            p1 = seg1;
            p2 = seg2;
            while (p1 && p2) {
                x = xOffset + p2->x;
                if ((test = p1->x - x) <= 0) {
                    x = p1->x;
                    flag ^= 1;
                    p1 = p1->next;
                    }
                if (test >= 0) {
                    flag ^= 2;
                    p2 = p2->next;
                    }

                /* A transition in or out of state 0 indicates a union,
                 * so update the position of the current segment.
                 */
                if (flag == 0 || old == 0) {
                    *sp = pt = _MGL_newSegment();
                    pt->x = x;
                    sp = &pt->next;
                    }
                old = flag;
                }

            /* Copy anything left over in whatever segment wasn't done  */
            while (p1) {
                *sp = pt = _MGL_newSegment();
                pt->x = p1->x;
                sp = &pt->next;
                p1 = p1->next;
                }
            while (p2) {
                *sp = pt = _MGL_newSegment();
                pt->x = xOffset + p2->x;
                sp = &pt->next;
                p2 = p2->next;
                }
            *sp = NULL;                 /* Terminate the segment list   */
            }

        /* Create a new span and link into result */
        *rp = st = _MGL_newSpan();
        st->seg = segment;
        st->y = y;
        rp = &st->next;
        }

    /* Delete overlapping spans from region 1 */
    while (start1 != s1) {
        st = start1;
        start1 = start1->next;
        _PM_freeSpan(st);
        }

    /* Copy the remaining spans from either region onto the output list */
    if (s1)
        *rp = s1;       /* Link on remaining spans from r1              */
    else {
        while (s2) {    /* Copy remaining spans from r2                 */
            *rp = _MGL_copySpanOfs(s2,xOffset,yOffset);
            rp = &(*rp)->next;
            s2 = s2->next;
            }
        *rp = NULL;
        }

    /* Compute the union bounding rectangle for quick clip rejection */
    MGL_unionRect(r1->rect,rtmp,&r1->rect);

    return true;
}

/****************************************************************************
DESCRIPTION:
Computes the Boolean union of two regions.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region with which r2 is unioned, and also becomes the result region.
r2  - Region to be unioned with r1

RETURNS:
True if the union is valid, false if an empty region was created.

REMARKS:
Computes the Boolean union of two regions for the area covered by region r1
and region r2, computing the resulting region in r1, which may result in an
empty region. If you need to retain the value of r1, you need to first copy
r1 to a temporary region.

SEE ALSO:
MGL_diffRegion, MGL_sectRegion, MGL_unionRegionOfs,
MGL_unionRegionRect
****************************************************************************/
ibool MGLAPI MGL_unionRegion(
    region_t *r1,
    const region_t *r2)
{
    return MGL_unionRegionOfs(r1,r2,0,0);
}

/****************************************************************************
DESCRIPTION:
Computes the Boolean union of a region and a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region with which r2 is unioned, and also becomes the result region.
r2  - Rectangle to be unioned with r1

RETURNS:
True if the union is valid, false if an empty region was created.

REMARKS:
Computes the Boolean union of a region r1 and rectangle r2, computing the
resulting region in r1, which may result in an empty region. If you need
to retain the value of r1, you need to first copy r1 to a temporary region.

This routine is faster than using MGL_unionRegion if the region to be
unioned is a simple rectangle rather than a complex region.

SEE ALSO:
MGL_diffRegion, MGL_sectRegion, MGL_unionRegion, MGL_unionRegionOfs
****************************************************************************/
ibool MGLAPI MGL_unionRegionRect(
    region_t *r1,
    const rect_t *r2)
{
    region_t    *rgn;
    _MGL_tmpRectRegion2(rgn,*r2);
    return MGL_unionRegionOfs(r1,rgn,0,0);
}
