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
* Description:  Module to implement the region difference operation.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Compute the Boolean difference of two regions.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region from which r2 is subtracted, which also becomes the result region.
r2  - Region to be subtracted from r1

RETURNS:
True if the difference is valid, false if an empty region was created.

REMARKS:
Computes the Boolean difference of two regions by subtracting the area
covered by region r2 from region r1, computing the resulting region in r1,
which may result in an empty region. If you need to retain the value of r1,
you need to first copy r1 to a temporary region.

SEE ALSO:
MGL_diffRegionRect, MGL_unionRegion, MGL_sectRegion
****************************************************************************/
ibool MGLAPI MGL_diffRegion(
    region_t *r1,
    const region_t *r2)
{
    segment_t   *seg1,*seg2;        /* Pointers to segments             */
    span_t      *s1,*s2,*st;        /* Pointers to spans                */
    span_t      **rp;               /* Insert point for resultant list  */
    span_t      *start1;            /* Overlapping spans in region1     */
    segment_t   **sp,*segment;
    segment_t   *p1,*p2,*pt;
    int         flag,old,x = 0,y = 0,test;

    /* The region is empty if the first region is empty, or the first
     * region if the second region is empty.
     */
    s1 = r1->spans;
    s2 = r2->spans;
    if (!s1 && MGL_emptyRect(r1->rect)) {
        MGL_clearRegion(r1);
        return false;
        }
    if (!s2 && MGL_emptyRect(r2->rect))
        return true;

    /* Handle cases where either region is a rectangle and expand it */
    if (!s1)
        s1 = _MGL_rectRegion(r1);
    if (!s2) {
        _MGL_tmpRectRegion(r2);
        s2 = r2->spans;
        }

    /* Initialise result list and insert pointer    */
    r1->spans = NULL;
    rp = &r1->spans;
    seg1 = seg2 = NULL;

    /* Copy all spans from the first region that come before the first
     * span of the second region. Skip any spans in the second region
     * that come before the first span in the first region.
     */
    r1->rect.top = MIN(s1->y,s2->y);
    if (s1->y < s2->y) {
        while (s1 && s1->y < s2->y) {
            *rp = s1;
            seg1 = s1->seg;
            rp = &s1->next;
            s1 = s1->next;
            }
        }
    else if (s2->y < s1->y) {
        while (s2 && s2->y < s1->y) {
            seg2 = s2->seg;
            s2 = s2->next;
            }
        }

    /* Save the start of the overlapping region for region 1 */
    start1 = s1;

    /* Process the overlapping area between the two regions */
    while (s1 && s2) {
        /* Compute the test value. The value is negative if the first
         * region is behind the second region, positive if the first shape
         * is ahead of the second one, and zero if they are in sync.
         */
        if ((test = s1->y - s2->y) <= 0) {
            y = s1->y;
            seg1 = s1->seg;
            s1 = s1->next;
            }
        if (test >= 0) {
            y = s2->y;
            seg2 = s2->seg;
            s2 = s2->next;
            }

        /* The resultant segment is empty if the first segment if empty,
         * or the first segment if the second segment is empty.
         */
        if (!seg1)
            segment = NULL;
        else if (!seg2)
            segment = _MGL_copySegment(seg1);
        else {
            /* Both segments are non-empty, so compute their difference */
            flag = old = 0;
            segment = NULL;
            sp = &segment;
            p1 = seg1;
            p2 = seg2;
            while (p1 && p2) {
                if ((test = p1->x - p2->x) <= 0) {
                    x = p1->x;
                    flag ^= 1;
                    p1 = p1->next;
                    }
                if (test >= 0) {
                    x = p2->x;
                    flag ^= 2;
                    p2 = p2->next;
                    }

                /* A transition in or out of state 1 indicates a difference,
                 * so update the position of the current segment.
                 */
                if (flag == 1 || old == 1) {
                    *sp = pt = _MGL_newSegment();
                    pt->x = x;
                    sp = &pt->next;
                    }
                old = flag;
                }

            /* Copy anything left over in the first segment */
            while (p1) {
                *sp = pt = _MGL_newSegment();
                pt->x = p1->x;
                sp = &pt->next;
                p1 = p1->next;
                }
            *sp = NULL;                 /* Terminate the segment list   */
            }

        /* Add a new span to the result if there's anything in the segment.
         * We also need to add a span if there's already something in the
         * result list since we need to mark the vertical gap.
         */
        if (segment || r1->spans) {
            *rp = st = _MGL_newSpan();
            st->seg = segment;
            st->y = y;
            rp = &st->next;
            }
        }

    /* Delete overlapping spans from region 1 */
    while (start1 != s1) {
        st = start1;
        start1 = start1->next;
        _PM_freeSpan(st);
        }

    /* Copy the leftover spans from the first region to the result */
    if (s1)
        *rp = s1;       /* Link on remaining spans from r1              */
    else
        *rp = NULL;     /* Terminate the spans list                     */

    /* If there are no spans in the resulting intersection, then it is
     * empty and we clear out the bounding rectangle to flag this.
     */
    if (!r1->spans)
        memset(r1,0,sizeof(region_t));
    return true;
}

/****************************************************************************
DESCRIPTION:
Compute the Boolean difference of a region and a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region from which r2 is subtracted, which also becomes the result region.
r2  - Rectangle to be subtracted from r1

RETURNS:
True if the difference is valid, false if an empty region was created.

REMARKS:
Computes the Boolean difference of a region and a simple rectangle by
subtracting the area covered by rectangle r2 from region r1, computing the
resulting region in r1, which may result in an empty region. If you need to
retain the value of r1, you need to first copy r1 to a temporary region.

This routine will produce a simple region with only a single bounding
rectangle if the original region was also a simple rectangle and the
resulting region is also a single rectangle, which makes it more efficient
if the region to be subtracted is a rectangle.

SEE ALSO:
MGL_diffRegion, MGL_unionRegion, MGL_sectRegion
****************************************************************************/
ibool MGLAPI MGL_diffRegionRect(
    region_t *r1,
    const rect_t *r2)
{
    region_t    *rgn;
    _MGL_tmpRectRegion2(rgn,*r2);
    return MGL_diffRegion(r1,rgn);
}
