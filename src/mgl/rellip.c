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
* Description:  Ellipse region generation routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*--------------------------- Global Variables ----------------------------*/

static region_t         *_rgn;
static const region_t   *_pen;
static int              left,top,right,bot;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
_topY   - Top Y coordinate for initial 4 pixels
_botY   - Bottom Y coordinate for initial 4 pixels
_left   - Left X coordinate for initial 4 pixels
_right  - Right X coordinate for initial 4 pixels

REMARKS:
Sets up for the initial rendering of an ellipse.
****************************************************************************/
static void MGLAPI _pen_setup(
    int _topY,
    int _botY,
    int _left,
    int _right)
{
    top = _topY;
    bot = _botY;
    left = _left;
    right = _right;
}

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines
region1 - True if in the first region of the ellipse

REMARKS:
Plots each of the four pixels for the ellipse by rendering the _pen at each
of the four current locations.
****************************************************************************/
static void MGLAPI _pen_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    (void)region1;

    /* Draw the _pen at all four locations */
    MGL_unionRegionOfs(_rgn,_pen,left,top);
    MGL_unionRegionOfs(_rgn,_pen,left,bot);
    MGL_unionRegionOfs(_rgn,_pen,right,top);
    MGL_unionRegionOfs(_rgn,_pen,right,bot);

    /* Update coordinates */
    if (inc_y) {
        top++;  bot--;
        }
    if (inc_x) {
        left--; right++;
        }
}

/****************************************************************************
REMARKS:
Cleans up at the end of ellipse generation.
****************************************************************************/
static void MGLAPI _pen_finished(void)
{
}

/****************************************************************************
REMARKS:
Internal function to generate an elliptical region
{secret}
****************************************************************************/
region_t * __MGL_rgnEllipse(
    int left,
    int top,
    int A,
    int B,
    const region_t *pen)
{
    _pen = pen;
    __MGL_ellipseEngine(left,top,A,B,_pen_setup,_pen_set4pixels,_pen_finished);
    return _rgn;
}

/****************************************************************************
DESCRIPTION:
Generate an ellipse outline as a region.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle for the ellipse
pen         - Region to use as the _pen when drawing the ellipse

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function generates the outline of an ellipse as a complex region by
dragging the specified _pen region around the perimeter of the ellipse. The
_pen used can be any arbitrary shape, however rectangular _pens are special
cased to provide fast region generation.

SEE ALSO:
MGL_rgnEllipseArc
****************************************************************************/
region_t * MGLAPI MGL_rgnEllipse(
    rect_t extentRect,
    const region_t *pen)
{
    int         A,B,i,j,maxIndex,_penHeight,minx,maxx,l,lr,rl,r;
    scanlist2   scanList;
    scanline2   *scanPtr;
    span_t      **sp,*head,*st1,*st2,*tail;

    _rgn = MGL_newRegion();
    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A == 0 || B == 0 || MGL_emptyRect(extentRect))
        return _rgn;         /* Ignore degenerate ellipses           */

    if (pen->spans) {
        _pen = pen;
        __MGL_ellipseEngine(extentRect.left,extentRect.top,A,B,_pen_setup,
            _pen_set4pixels,_pen_finished);
        }
    else {
        /* Scan the outline of a fat ellipse drawn with a rectangular _pen
         * into the scratch buffer
         */
        _MGL_scanFatEllipse(&scanList,extentRect.left,extentRect.top,A,B,
            pen->rect.right-pen->rect.left-1,
            _penHeight = pen->rect.bottom-pen->rect.top-1);

        /* Initialise the span list */
        head = NULL;            sp = &head;
        tail = _MGL_newSpan();  tail->y = scanList.top+B+_penHeight+1;
        tail->seg = NULL;       tail->next = NULL;
        minx = 32767;           maxx = -32767;

        maxIndex = scanList.top + scanList.length;
        scanPtr = scanList.scans;
        for (i = scanList.top,j = tail->y-1; i < maxIndex; i++,j--,scanPtr++) {
            if (scanPtr->leftR < scanPtr->rightL) {
                *sp = st1 = _MGL_createSpan2(i,l = scanPtr->leftL,
                    lr = scanPtr->leftR,rl = scanPtr->rightL,
                    r = scanPtr->rightR);
                st2 = _MGL_createSpan2(j,l,lr,rl,r);
                minx = MIN(minx,l);     maxx = MAX(maxx,r);
                }
            else {
                *sp = st1 = _MGL_createSpan(i,l = scanPtr->leftL,r = scanPtr->rightR);
                st2 = _MGL_createSpan(j,l,r);
                minx = MIN(minx,l);     maxx = MAX(maxx,r);
                }
            sp = &st1->next;
            st2->next = tail;
            tail = st2;
            }

        /* Terminate the ellipse by joining top and bottom halves */
        if ((B+_penHeight+1) & 1) {
            if (scanPtr->leftR < scanPtr->rightL) {
                *sp = st1 = _MGL_createSpan2(i,scanPtr->leftL,scanPtr->leftR,
                    scanPtr->rightL,scanPtr->rightR);
                }
            else {
                *sp = st1 = _MGL_createSpan(i,scanPtr->leftL,scanPtr->rightR);
                }
            st1->next = tail;
            }
        else {
            *sp = tail;
            }

        /* Fill in the bounding rectangle for the region */
        _rgn->rect.left = minx;
        _rgn->rect.right = maxx;
        _rgn->rect.top = scanList.top;
        _rgn->rect.bottom = maxIndex;
        _rgn->spans = head;
        }
    return _rgn;
}

/****************************************************************************
DESCRIPTION:
Generates a solid ellipse as a region.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle for the ellipse

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function generates a solid ellipse as a complex region

SEE ALSO:
MGL_rgnSolidEllipseArc
****************************************************************************/
region_t * MGLAPI MGL_rgnSolidEllipse(
    rect_t extentRect)
{
    int         A,B,i,j,maxIndex,minx,maxx,l,r;
    scanlist    scanList;
    scanline    *scanPtr;
    span_t      **sp,*head,*st,*tail;
    region_t    *_rgn = MGL_newRegion();

    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A == 0 || B == 0 || MGL_emptyRect(extentRect))
        return _rgn;     /* Ignore degenerate ellipses           */

    /* Scan the outline of the ellipse into scratch buffer */
    _MGL_scanEllipse(&scanList,extentRect.left,extentRect.top,A,B);

    /* Initialise the span list */
    head = NULL;            sp = &head;
    tail = _MGL_newSpan();  tail->y = extentRect.bottom;
    tail->seg = NULL;       tail->next = NULL;
    minx = 32767;           maxx = -32767;

    /* Now build the region spans from the scratch buffer */
    maxIndex = scanList.top + scanList.length-1;
    scanPtr = scanList.scans;
    _rgn->rect.top = i = scanList.top;
    _rgn->rect.bottom = (j = i + B)+1;
    for (; i < maxIndex; i++,j--,scanPtr++) {
        /* Create a new span at top of ellipse      */
        *sp = st = _MGL_createSpan(i,scanPtr->left,scanPtr->right);
        sp = &st->next;

        /* Create a new span at bottom of ellipse   */
        st = _MGL_createSpan(j,l = scanPtr->left,r = scanPtr->right);
        minx = MIN(minx,l);     maxx = MAX(maxx,r);
        st->next = tail;
        tail = st;
        }

    /* Terminate the ellipse by joining top and bottom halves */
    if (B & 1)
        *sp = tail;
    else {
        /* Ellipse height is odd, so create a single last span, and link
         * the top and bottom span lists together
         */
        *sp = st = _MGL_createSpan(i,l = scanPtr->left,r = scanPtr->right);
        minx = MIN(minx,l);     maxx = MAX(maxx,r);
        st->next = tail;
        }

    /* Fill in the bounding rectangle for the region */
    _rgn->rect.left = minx;
    _rgn->rect.right = maxx;
    _rgn->spans = head;
    return _rgn;
}
