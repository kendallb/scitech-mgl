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
* Description:  Region memory management routines. Our shape algebra is
*               implemented using linked lists of spans and segments with
*               the structures allocated dymanically from large arrays
*               managed with a free list.
*
*               If we run out of local memory in the span and segment pools,
*               we attempt to allocate more memory to the region pools on
*               the fly. This allows us to start off with a reasonably small
*               amount of memory, and expand if the application demand for
*               complex regions is higher than normal.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

segmentList_t   _MGL_segList;       /* Global pool of segments      */
spanList_t      _MGL_spanList;      /* Global pool of spans         */
regionList_t    _MGL_regionList;    /* Global pool of regions       */
segment_t       _MGL_tmpX2  = { NULL, 0 };
segment_t       _MGL_tmpX1  = { &_MGL_tmpX2, 0 };
span_t          _MGL_tmpY2  = { NULL, NULL, 0 };
span_t          _MGL_tmpY1  = { &_MGL_tmpY2, &_MGL_tmpX1, 0 };
region_t        _MGL_rtmp   = { {0, 0, 0, 0}, &_MGL_tmpY1 };
region_t        _MGL_rtmp2  = { {0, 0, 0, 0}, NULL };

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Creates the initial regionlist by allocating and building the region
list free list pool. This is called by the MGL when it first starts up.
{secret}
****************************************************************************/
void _MGL_createRegionList(void)
{
    _MGL_createBlockList(&_MGL_regionList.bl,DEF_NUM_REGIONS,sizeof(region_t));
    _MGL_regionList.freeList = _MGL_buildFreeList(&_MGL_regionList.bl);
}

/****************************************************************************
REMARKS:
Resizes the region list if full by increasing its size by the original
default size and building a new free list in the resized portion.
{secret}
****************************************************************************/
static void resizeRegionList(void)
{
    _MGL_resizeBlockList(&_MGL_regionList.bl);
    _MGL_regionList.freeList = _MGL_buildFreeList(&_MGL_regionList.bl);
}

/****************************************************************************
REMARKS:
Frees the region list. This is called by the MGL when it shuts down.
{secret}
****************************************************************************/
void _MGL_freeRegionList(void)
{
    _PM_freeBlockList(&_MGL_regionList.bl);
}

/****************************************************************************
PARAMETERS:
r   - Region to expand

RETURNS:
Pointer to the first span in the region

REMARKS:
Expands the regions from a special case rectangle into a proper rectangular
region.
{secret}
****************************************************************************/
span_t * _MGL_rectRegion(
    region_t *r)
{
    span_t      *s1,*s2;
    segment_t   *seg;

    r->spans = s1 = _MGL_newSpan();
    s1->y = r->rect.top;
    seg = (s1->seg = _MGL_newSegment());
    seg->x = r->rect.left;
    seg = (seg->next = _MGL_newSegment());
    seg->x = r->rect.right;
    seg->next = NULL;
    s1->next = s2 = _MGL_newSpan();
    s2->y = r->rect.bottom;
    s2->seg = NULL;
    s2->next = NULL;
    return s1;
}
/****************************************************************************
PARAMETERS:
scanList    - Pointer to scanlist to build region from

RETURNS:
Pointer to the newly created region

REMARKS:
Builds a region definition from a shape rendered into the scratch buffer.
{secret}
****************************************************************************/
region_t *_MGL_createRegionFromBuf(
    scanlist *scanList)
{
    int         i,maxIndex,l,r,minx,maxx;
    scanline    *scanPtr;
    span_t      **sp,*head,*st;
    region_t    *rgn = MGL_newRegion();

    /* Initialise the span list */
    head = NULL;
    sp = &head;
    minx = 32767;
    maxx = -32767;

    /* Now build the region spans from the scratch buffer */
    maxIndex = scanList->top + scanList->length;
    scanPtr = scanList->scans;
    for (i = scanList->top; i < maxIndex; i++,scanPtr++) {
        *sp = st = _MGL_createSpan(i,l = scanPtr->left,r = scanPtr->right);
        sp = &st->next;
        minx = MIN(minx,l);
        maxx = MAX(maxx,r);
        }

    /* Terminate the span list */
    *sp = st = _MGL_newSpan();
    st->y = i;
    st->seg = NULL;
    st->next = NULL;

    /* Fill in the bounding rectangle for the region */
    rgn->rect.left = minx;
    rgn->rect.right = maxx;
    rgn->rect.top = scanList->top;
    rgn->rect.bottom = maxIndex;

    rgn->spans = head;
    return rgn;
}

/****************************************************************************
DESCRIPTION:
Allocate a new complex region.

HEADER:
mgraph.h

RETURNS:
Pointer to the new region, NULL if out of memory.

REMARKS:
Allocates a new complex region. The new region is empty when first created.
Note that MGL maintains a local memory pool for all region allocations in
order to provide the maximum speed and minimum memory overheads for region
allocations.

SEE ALSO:
MGL_freeRegion, MGL_unionRegion, MGL_diffRegion, MGL_sectRegion
****************************************************************************/
region_t * MGLAPI MGL_newRegion(void)
{
    region_t    *p;

    if (_MGL_regionList.freeList == NULL)
        resizeRegionList();
    p = _MGL_regionList.freeList;
    _MGL_regionList.freeList = FREELIST_NEXT(p);
    memset(p,0,sizeof(region_t));
    return p;
}

/****************************************************************************
DESCRIPTION:
Create a copy of the specified region.

HEADER:
mgraph.h

PARAMETERS:
s   - Pointer to source region

RETURNS:
Pointer to the copied region, or NULL if out of memory.

REMARKS:
Copies the definition for an entire region and returns a pointer to the
newly created region. The space for the copied region is allocated from
the region memory pool, which MGL uses to maintain a local memory
allocation scheme for regions to increase performance.

If there is not enough memory to copy the region, this routine will
return NULL.

SEE ALSO:
MGL_newRegion, MGL_freeRegion, MGL_clearRegion, MGL_copyIntoRegion
****************************************************************************/
region_t * MGLAPI MGL_copyRegion(
    const region_t *s)
{
    region_t    *region;

    region = MGL_newRegion();
    *region = *s;
    region->spans = _MGL_copySpans(s->spans);
    return region;
}

/****************************************************************************
DESCRIPTION:
Copy the contents of one region into another region.

HEADER:
mgraph.h

PARAMETERS:
d   - Pointer to destination region
s   - Pointer to source region

REMARKS:
Copies the definition for an entire region into the destination region,
clearing any region information already present in the destination. This
function is similar to MGL_copyRegion, however it does not allocate a new
region but rather copies the data into an existing region.

SEE ALSO:
MGL_newRegion, MGL_freeRegion, MGL_clearRegion, MGL_copyRegion
****************************************************************************/
void MGLAPI MGL_copyIntoRegion(
    region_t *d,
    const region_t *s)
{
    MGL_clearRegion(d);
    *d = *s;
    d->spans = _MGL_copySpans(s->spans);
}

/****************************************************************************
DESCRIPTION:
Frees all the memory allocated by the complex region.

HEADER:
mgraph.h

PARAMETERS:
r   - Pointer to the region to free

REMARKS:
Frees all the memory allocated by the complex region. When you are finished
with a complex region you must free it to free up the memory used to
represent the union of rectangles.

SEE ALSO:
MGL_newRegion, MGL_copyRegion
****************************************************************************/
void MGLAPI MGL_freeRegion(
    region_t *r)
{
    span_t  *s,*p;

    if (!r)
        return;
    for (s = r->spans; s; s = p) {
        p = s->next;
        _PM_freeSpan(s);
        }
    FREELIST_NEXT(r) = _MGL_regionList.freeList;
    _MGL_regionList.freeList = r;
}

/****************************************************************************
DESCRIPTION:
Clears the specified region to an empty region.

HEADER:
mgraph.h

PARAMETERS:
r   - region to be cleared

REMARKS:
This function clears the specified region to an empty region, freeing up
all the memory used to store the region data.

SEE ALSO:
MGL_newRegion, MGL_copyRegion, MGL_freeRegion
****************************************************************************/
void MGLAPI MGL_clearRegion(
    region_t *r)
{
    span_t  *s,*p;

    for (s = r->spans; s; s = p) {
        p = s->next;
        _PM_freeSpan(s);
        }
    memset(r,0,sizeof(region_t));
}

/****************************************************************************
DESCRIPTION:
Optimizes the union of rectangles in the region to the minimum number of
rectangles.

HEADER:
mgraph.h

PARAMETERS:
r   - Region to optimize

REMARKS:
This function optimizes the specified region by traversing the region
structure looking for identical spans in the region. The region algebra
functions (MGL_unionRegion, MGL_diffRegion, MGL_sectRegion etc.) do not
fully optimize the resulting region to save time, so after you have
created a complex region you may wish to call this routine to optimize it.

Optimizing the region will find the minimum number of rectangles required
to represent that region, and will result in faster drawing and traversing
of the resulting region.

SEE ALSO:
MGL_unionRegion, MGL_diffRegion, MGL_sectRegion
****************************************************************************/
void MGLAPI MGL_optimizeRegion(
    region_t *r)
{
    span_t *s = r->spans,*prev,*next;

    if (!s)
        return;

    prev = s;
    for (s = s->next; s; s = next) {
        segment_t *seg = s->seg;
        segment_t *segPrev = prev->seg;
        ibool identical = (seg && segPrev);

        /* Check if spans are identical */
        while (seg && segPrev) {
            if (seg->x != segPrev->x) {
                identical = false;
                break;
                }
            seg = seg->next;
            segPrev = segPrev->next;
            }

        /* Unlink the identical span from the list */
        if (identical && !seg && !segPrev) {
            next = prev->next = s->next;
            _PM_freeSpan(s);
            }
        else {
            next = s->next;
            prev = s;
            }
        }
}
