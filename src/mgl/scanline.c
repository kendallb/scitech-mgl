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
* Description:  Scanline drawing routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
y   - Y coordinate for scanline
x1  - First X coordinate for scanline
x2  - Second X coordinate for scanline

REMARKS:
Renders the passed in scanline without clipping to the currently active
device context. This is an internal helper function used by lots of other
code in the MGL to implement clipped scanline drawing.
{secret}
****************************************************************************/
void _MGL_drawScanLine(
    int y,
    int x1,
    int x2)
{
    if (x2 < x1)
        SWAP(x1,x2);
    if (x1 < x2)
        DC.r.cur.DrawRect(x1,y,x2-x1,1);
}

/****************************************************************************
PARAMETERS:
y   - Y coordinate for scanline
x1  - First X coordinate for scanline
x2  - Second X coordinate for scanline

REMARKS:
Renders the passed in scanline with clipping to the currently active
device context. This is an internal helper function used by lots of other
code in the MGL to implement clipped scanline drawing.
{secret}
****************************************************************************/
void _MGL_clipScanLine(
    int y,
    int x1,
    int x2)
{
    if (y >= DC.clipRectScreen.top && y < DC.clipRectScreen.bottom) {
        if (x2 < x1)
            SWAP(x1,x2);
        x1 = MAX(x1,DC.clipRectScreen.left);
        x2 = MIN(x2,DC.clipRectScreen.right);
        if (x1 < x2)
            DC.r.cur.DrawRect(x1,y,x2-x1,1);
        }
}

/****************************************************************************
PARAMETERS:
y       - Y coordinate to begin rendering
length  - Number of scanlines in the list
scans   - Pointer to array of scanlines to render

REMARKS:
Renders the passed in list of scanlines beginning at the specified y
coordinate in the current drawing attributes. This routine performs clipping
on a scanline by scanline scissoring basis, and will be slower than the
above routine. The high level code will ensure that the correct routine is
called when clipping is required.
{secret}
****************************************************************************/
void _MGL_drawClippedScanList(
    int y,
    int length,
    short *scans,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    int     i,diff,xstart,xend;
    short   *p;

    /* Clip the y extents first. We will trivially reject the scanline list
     * if the y extents are outside of the clipping region
     */
    if ((diff = (clipTop - y)) > 0) {
        y = clipTop;
        if ((length -= diff) <= 0)
            return;
        scans += diff * 2;
        }
    if ((diff = (y + length - clipBottom)) > 0) {
        if ((length -= diff) <= 0)
            return;
        }

    /* Now go through each scanline in turn, clipping them to the clipLeft and
     * clipRight clipping coordinates.
     */
    for (i = 0, p = scans; i < length; i++) {
        xstart = *p;
        xend = *(p+1);
        if (xstart != xend) {
            if (xstart > xend)
                SWAP(xstart,xend);
            if (xstart >= clipRight || xend <= clipLeft) {
                xstart = xend = 0;              /* Clip entire line     */
                }
            else {
                if (xstart < clipLeft)
                    xstart = clipLeft;              /* Clip to clipLeft edge    */
                if (xend > clipRight)
                    xend = clipRight;               /* Clip to clipRight edge   */
                }
            *p++ = xstart;
            *p++ = xend;
            }
        else p += 2;
        }
    DC.r.cur.DrawScanList(y,length,scans);
}

/****************************************************************************
DESCRIPTION:
Fills a specified scanline with no clipping, in screen space.

HEADER:
mgraph.h

PARAMETERS:
y   - y coordinate of scanline to fill
x1  - Starting x coordinate of scanline to fill
x2  - Ending x coordinate of scanline to fill

REMARKS:
MGL_scanLine fills the specified portion of a scanline in the current
attributes and fill pattern. This can be used to implement higher level
complex fills, such as region fills, floodfills etc.

Note:   This function is intended as a low level building block, and as
        such does not do any clipping and takes coordinates in screen
        space directly. If you want to draw a clipped, viewport relative
        scanline use the MGL_lineCoord family of functions.

SEE ALSO:
MGL_penStyleType, MGL_setPenBitmapPattern, MGL_setPenPixmapPattern,
MGL_lineCoord
****************************************************************************/
void MGLAPI MGL_scanLine(
    int y,
    int x1,
    int x2)
{
    if (x2 < x1)
        SWAP(x1,x2);
    if (x1 < x2)
        DC.r.cur.DrawRect(x1,y,x2-x1,1);
}
