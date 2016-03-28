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

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Creates the initial span list by allocating and building the span
list free list pool. This is called by the MGL when it first starts up.
{secret}
****************************************************************************/
void _MGL_createSpanList(void)
{
    _MGL_createBlockList(&_MGL_spanList.bl,DEF_NUM_SPANS,sizeof(span_t));
    _MGL_spanList.freeList = _MGL_buildFreeList(&_MGL_spanList.bl);
}

/****************************************************************************
REMARKS:
Resizes the span list if full by increasing its size by the original
default size and building a new free list in the resized portion.
{secret}
****************************************************************************/
static void resizeSpanList(void)
{
    _MGL_resizeBlockList(&_MGL_spanList.bl);
    _MGL_spanList.freeList = _MGL_buildFreeList(&_MGL_spanList.bl);
}

/****************************************************************************
REMARKS:
Frees the span list. This is called by the MGL when it shuts down.
{secret}
****************************************************************************/
void _PM_freeSpanList(void)
{
    _PM_freeBlockList(&_MGL_spanList.bl);
}

/****************************************************************************
RETURNS:
Pointer to new span

REMARKS:
Allocates space for a new span in the span pool from the free list. If
there are no more spans, we attempt to re-size the span pool which will
bomb out if we run out of memory.
{secret}
****************************************************************************/
span_t * _MGL_newSpan(void)
{
    span_t  *p;

    if (_MGL_spanList.freeList == NULL)
        resizeSpanList();
    p = _MGL_spanList.freeList;
    _MGL_spanList.freeList = FREELIST_NEXT(p);
    return p;
}

/****************************************************************************
PARAMETERS:
s   - Pointer to source span

RETURNS:
Pointer to copied span

REMARKS:
Copies the definitions for a since span and returns the pointer to
the new span. The space for the copied span is allocated from the
memory pool.
{secret}
****************************************************************************/
span_t * _MGL_copySpan(
    span_t *s)
{
    span_t *span = _MGL_newSpan();
    span->y = s->y;
    span->seg = _MGL_copySegment(s->seg);
    span->next = NULL;
    return span;
}

/****************************************************************************
PARAMETERS:
s       - Pointer to source span
xOffset - X coordinate offset value
yOffset - Y coordinate offset value

RETURNS:
Pointer to copied span

REMARKS:
Copies the definitions for a since span and returns the pointer to
the new span. The space for the copied span is allocated from the
memory pool. This version offsets all the X and Y coordinates
in the span and segment lists by the specified offset values.
{secret}
****************************************************************************/
span_t * _MGL_copySpanOfs(
    span_t *s,
    int xOffset,
    int yOffset)
{
    span_t *span = _MGL_newSpan();
    span->y = yOffset + s->y;
    span->seg = _MGL_copySegmentOfs(s->seg,xOffset);
    span->next = NULL;
    return span;
}

/****************************************************************************
PARAMETERS:
s   - Pointer to source spans

RETURNS:
Pointer to copied spans

REMARKS:
Copies the definitions for an entire span list and returns the pointer to
the first span in the list. The space for the copied spans is allocated
from the memory pool.
{secret}
****************************************************************************/
span_t * _MGL_copySpans(
    span_t *s)
{
    span_t  *p,**prev,*head;

    for (head = NULL,prev = &head; s; s = s->next) {
        *prev = p = _MGL_newSpan();
        p->y = s->y;
        p->seg = _MGL_copySegment(s->seg);
        prev = &p->next;
        }
    *prev = NULL;
    return head;
}

/****************************************************************************
PARAMETERS:
s       - Pointer to source spans
xOffset - X coordinate offset value
yOffset - Y coordinate offset value

RETURNS:
Pointer to copied spans

REMARKS:
Copies the definitions for an entire span list and returns the pointer to
the first span in the list. The space for the copied spans is allocated
from the memory pool. This version offsets all the X and Y coordinates
in the span and segment lists by the specified offset values.
{secret}
****************************************************************************/
span_t * _MGL_copySpansOfs(
    span_t *s,
    int xOffset,
    int yOffset)
{
    span_t  *p,**prev,*head;

    for (head = NULL,prev = &head; s; s = s->next) {
        *prev = p = _MGL_newSpan();
        p->y = yOffset + s->y;
        p->seg = _MGL_copySegmentOfs(s->seg,xOffset);
        prev = &p->next;
        }
    *prev = NULL;
    return head;
}

/****************************************************************************
PARAMETERS:
span    - Pointer to the span to free

REMARKS:
Frees all the memory allocated by the span by moving though the list of
segments freeing each one.
{secret}
****************************************************************************/
void _PM_freeSpan(
    span_t *span)
{
    segment_t *s,*p;

    for (s = span->seg; s; s = p) {
        p = s->next;
        FREELIST_NEXT(s) = _MGL_segList.freeList;
        _MGL_segList.freeList = s;
        }
    FREELIST_NEXT(span) = _MGL_spanList.freeList;
    _MGL_spanList.freeList = span;
}

/****************************************************************************
PARAMETERS:
y   - Y coordinate for span
x1  - X1 coordinate for span
x2  - X2 coordinate for span

RETURNS:
Pointer to the new span

REMARKS:
Function to create a single scanline span. This should be turned into a
macro for speed.
{secret}
****************************************************************************/
span_t * _MGL_createSpan(
    int y,
    int x1,
    int x2)
{
    segment_t   *seg;
    span_t      *st;

    st = _MGL_newSpan();
    st->y = y;
    seg = (st->seg = _MGL_newSegment());
    seg->x = x1;
    seg = (seg->next = _MGL_newSegment());
    seg->x = x2;
    seg->next = NULL;
    return st;
}

/****************************************************************************
PARAMETERS:
y   - Y coordinate for span
x1  - X1 coordinate for first segment
x2  - X2 coordinate for first segment
x3  - X1 coordinate for second segment
x4  - X2 coordinate for second segment

RETURNS:
Pointer to the new span

REMARKS:
Function to create a double scanline span.
{secret}
****************************************************************************/
span_t * _MGL_createSpan2(
    int y,
    int x1,
    int x2,
    int x3,
    int x4)
{
    segment_t   *seg;
    span_t      *st;

    st = _MGL_newSpan();
    st->y = y;
    seg = (st->seg = _MGL_newSegment());
    seg->x = x1;
    seg = (seg->next = _MGL_newSegment());
    seg->x = x2;
    seg = (seg->next = _MGL_newSegment());
    seg->x = x3;
    seg = (seg->next = _MGL_newSegment());
    seg->x = x4;
    seg->next = NULL;
    return st;
}
