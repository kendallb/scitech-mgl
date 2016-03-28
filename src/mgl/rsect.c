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
* Description:  Module to implement the region intersection operation.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Compute the Boolean intersection between two regions.

HEADER:
mgraph.h

PARAMETERS:
r1  - First region to compute intersection with
r2  - Second region to compute intersection with

RETURNS:
Resulting intersection region, or NULL if out of memory.

REMARKS:
Computes the Boolean intersection of two regions, returning the result in
a new region. The region may actually be an empty region, in which case
the bounding rectangle for the region will be an empty rectangle.

SEE ALSO:
MGL_diffRegion, MGL_unionRegion, MGL_sectRegionRect
****************************************************************************/
region_t * MGLAPI MGL_sectRegion(
    const region_t *r1,
    const region_t *r2)
{
    int         flag,old,x = 0,y = 0,test;
    segment_t   **sp,*segment;
    segment_t   *p1,*p2,*pt;
    segment_t   *seg1,*seg2;        /* Pointers to segments             */
    span_t      *s1,*s2,*st;        /* Pointers to spans                */
    span_t      **rp;               /* Insert point for resultant list  */
    region_t    *result;            /* Resultant region                 */

    /* The region is empty if either region is empty */
    result = MGL_newRegion();
    s1 = r1->spans;
    s2 = r2->spans;
    if ((!s1 && MGL_emptyRect(r1->rect)) || (!s2 && MGL_emptyRect(r2->rect))) {
        memset(result,0,sizeof(region_t));
        return result;
        }

    /* Compute the intersection of the two bounding rectangles so we have
     * a quick clip rejection test available
     */
    MGL_sectRectFast(r1->rect,r2->rect,&result->rect);

    /* Handle special cases where both regions are rectangles */
    if (!s1 && !s2) {
        result->spans = NULL;
        return result;
        }

    /* Handle cases where either region is a rectangle and expand it */
    if (!s1) {
        _MGL_tmpRectRegion(r1);
        s1 = r1->spans;
        }
    else if (!s2) {
        _MGL_tmpRectRegion(r2);
        s2 = r2->spans;
        }

    /* Initialise result list and insert pointer    */
    result->spans = NULL;
    rp = &result->spans;
    seg1 = seg2 = NULL;

    /* Process the regions until either one ends. As soon as the first
     * shape ends, we have completed the calculation
     */
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

        /* Both segments are non-empty, so compute their intersection */
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

            /* A transition in or out of state 3 indicates in intersection.
             * Add a new segment element to record the coordinate.
             */
            if (flag == 3 || old == 3) {
                *sp = pt = _MGL_newSegment();
                pt->x = x;
                sp = &pt->next;
                }
            old = flag;
            }

        /* Add a new span to the result if there is anything if valid.
         * We also add a new spans if the result list is valid since we
         * we need to mark the vertical gap (and terminate the span list)
         */
        if (segment || result->spans) {
            *sp = NULL;                 /* Terminate the segment list   */
            *rp = st = _MGL_newSpan();
            if (!segment)
                st->seg = NULL;
            st->seg = segment;
            st->y = y;
            rp = &st->next;
            }
        }

    /* Terminate the resultant span list */
    *rp = NULL;

    /* If there are no spans in the resulting intersection, then it is
     * empty and we clear out the bounding rectangle to flag this.
     */
    if (!result->spans)
        memset(result,0,sizeof(region_t));

    return result;
}

/****************************************************************************
DESCRIPTION:
Compute the Boolean intersection between a region and a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r1  - Region to compute intersection with
r2  - Rectangle to compute intersection with

RETURNS:
Resulting intersection region, or NULL if out of memory.

REMARKS:
Computes the Boolean intersection of a region and a rectangle, returning
the result in a new region. The region may actually be an empty region,
in which case the bounding rectangle for the region will be an empty
rectangle. Note that this routine will compute the intersection faster
than calling MGL_sectRegion with a simple region as the second region to
intersect.

SEE ALSO:
MGL_sectRegion, MGL_diffRegion, MGL_unionRegion

****************************************************************************/
region_t * MGLAPI MGL_sectRegionRect(
    const region_t *r1,
    const rect_t *r2)
{
    region_t    *rgn;
    _MGL_tmpRectRegion2(rgn,*r2);
    return MGL_sectRegion(r1,rgn);
}
