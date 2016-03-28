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
* Description:  Routines to scan a line drawn with a rectangular pen into
*               the scratch buffer for fast rendering. We special case as
*               much as possible for maximum speed.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */
#include "clipline.h"

/*--------------------------- Global Variables ----------------------------*/

static  scanline    *scanPtr;
static  int         penHeight,penWidth;
static  int         oldy,oldx;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
x,y - Position to plot the point at

REMARKS:
Routine to plot a point on a fat line to the buffer for a line that is
moving downwards in y. For the left hand side of the line we only update
the last bottom-most pixel. This assumes that the scanline array will be
set up with the correct values for the first penHeight-1 left entries. We
also only update the first right coordinate for every pixel, assuming that
the correct values will be placed in the buffer after the line is scan
converted. This routine presumes that x values will always be increasing
in order for fat lines to be plotted correctly, so extra setup logic may be
required to select the plotPointUp function instead with swapped start/end
coordinates.
****************************************************************************/
static void MGLAPI plotPointDown(
    long x,
    long y)
{
    if (y != oldy) {
        scanPtr++;
        scanPtr[penHeight].left = (short)x;
        }
    scanPtr->right = (short)(x+penWidth+1);
    oldy = y;
    oldx = x;
}

/****************************************************************************
PARAMETERS:
x,y - Position to plot the point at

REMARKS:
Routine to plot a point on a fat line to the buffer for a line that is
moving upwards in y. For the left hand side of the line we only update
the last upper-most pixel. This assumes that the scanline array will be
set up with the correct values for the first penHeight-1 left entries. We
also only update the first right coordinate for every pixel, assuming that
the correct values will be placed in the buffer after the line is scan
converted.
****************************************************************************/
static void MGLAPI plotPointUp(
    long x,
    long y)
{
    if (y != oldy) {
        scanPtr--;
        scanPtr[-penHeight].left = (short)x;
        }
    scanPtr->right = (short)(x+penWidth+1);
    oldy = y;
    oldx = x;
}

/****************************************************************************
PARAMETERS:
scanList    - Scanline list to fill in
x1,y1       - First endpoint of line to draw
x2,y2       - Second endpoint of line to draw
penHeight   - Height of the pen
penWidth    - Width of the pen

RETURNS:
Maximum X coordinate

REMARKS:
Scans the image of a fat line into the scratch buffer.

Note:   This routine assumes that we are always scan
        converting the line from left to right.
{secret}
****************************************************************************/
int _MGL_scanFatLine(
    scanlist *scanList,
    int x1,
    int y1,
    int x2,
    int y2,
    int _penHeight,
    int _penWidth)
{
    int     i,height = y2 - y1;
    int     length = ABS(height) + 1 + _penHeight;

    scanList->length = length;
    if ((int)(length * sizeof(scanline)) > _MGL_bufSize)
        MGL_fatalError("Scratch buffer too small for rendering objects!\n");

    /* XY Plot routines presume increasing X. So if the X values are decreasing,
     * we will have to swap the XY start and end coordinates.
     */
    if (x2 < x1) {
        SWAP(x1,x2);
        SWAP(y1,y2);
        height = -height;
        }

    /* Fill in global variables */
    scanPtr = scanList->scans = _MGL_buf;
    oldy = y1;
    oldx = x1;
    penHeight = _penHeight;
    penWidth = _penWidth;

    if ((x2 - x1) == 0) {
        /* We have a vertical line, so special case it */
        x2 += penWidth + 1;
        for (i = 0; i < length; i++,scanPtr++) {
            scanPtr->left = x1;
            scanPtr->right = x2;
            }
        scanPtr = _MGL_buf;
        }
    else if (height == 0) {
        /* We have a horizontal line, so special case it */
        x2 += penWidth + 1;
        for (i = 0; i < length; i++,scanPtr++) {
            scanPtr->left = x1;
            scanPtr->right = x2;
            }
        scanPtr = _MGL_buf;
        }
    else if (height > 0) {
        /* We have a downward moving line. Scan convert most of the line
         * into the buffer by calling the lineEngine.
         */
        MGL_lineEngine(x1,y1,x2,y2,plotPointDown);

        /* Now fix the first penHeight+1 scanline left coordinates and the
         * last penHeight+1 scanline right coordinates.
         */
        scanPtr = _MGL_buf;
        scanPtr[length-1].left = (short)(x2);
        x2 += penWidth + 1;
        for (i = penHeight; i >= 0; i--) {
            scanPtr[i].left = (short)x1;
            scanPtr[length-i-1].right = (short)x2;
            }
        }
    else {
        /* We have an upward moving line. Scan convert most of the line
         * into the buffer by calling the lineEngine.
         */
        scanPtr += length-1;
        MGL_lineEngine(x1,y1,x2,y2,plotPointUp);

        /* Now fix the first penHeight+1 scanline left coordinates and the
         * last penHeight+1 scanline right coordinates.
         */
        scanPtr = _MGL_buf;
        scanPtr[0].left = (short)(x2);
        x2 += penWidth + 1;
        for (i = penHeight; i >= 0; i--) {
            scanPtr[length-i-1].left = (short)x1;
            scanPtr[i].right = (short)x2;
            }
        }

    /* We need to clip the scan converted line now to the clipping
     * rectangle, since some of the line MAY lie outside.
     */
    if (height < 0)
        scanList->top = y2;
    else
        scanList->top = y1;
    return x2;
}

/****************************************************************************
PARAMETERS:
x1          - X coordinate of first endpoint to clip
y1          - Y coordinate of first endpoint to clip
x2          - X coordinate of second endpoint to clip
y2          - Y coordinate of second endpoint to clip
drawLast    - True if last pixel should be drawn
clipLeft    - Left coordinate of clip rectangle
clipTop     - Top coordinate of clip rectangle
clipRight   - Right coordinate of clip rectangle
clipBottom  - Bottom coordinate of clip rectangle

REMARKS:
Internal function to clip and draw an integer line, such that the clipped
line segment draws the *exact* same set of pixels that the unclipped line
would have drawn. We also correctly handle arbitrarily large lines, while
still allowing the clipped line segment to be drawn in hardware.
{secret}
****************************************************************************/
void _MGL_drawClippedFatLineInt(
    int x1,
    int y1,
    int x2,
    int y2,
    ibool drawLast,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    int         absDeltaX,absDeltaY,initialError,majorInc,diagInc;
    int         clipdx,clipdy;
    int         new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;
    int         count,flags,clip1,clip2,outcode1,outcode2,i,height,length;
    scanlist    scanList;
    ibool       reversed = false;
    static ibool swapped = false;

    /* XY Plot routines presume increasing X, and so does the Clip routine.
     * So if the X values are decreasing, we will have to swap the XY start
     * and end coordinates and recursively call this routine a 2nd time
     * in order to compensate. Otherwise the Clip routine may loop forever
     * if clipping is required at one of the swapped endpoints.
     */
    if (x2 < x1) {
        reversed = true;
        }

    /* Calculate bresenham parameters */
    flags = gaLineXPositive | gaLineYPositive | gaLineXMajor | gaLineDoLastPel;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        flags &= ~gaLineXPositive;
        }
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        flags &= ~gaLineYPositive;
        }
    if (absDeltaX > absDeltaY) {
        majorInc = absDeltaY * 2;
        diagInc = majorInc - absDeltaX * 2;
        initialError = majorInc - absDeltaX;
        }
    else {
        majorInc = absDeltaX * 2;
        diagInc = majorInc - absDeltaY * 2;
        initialError = majorInc - absDeltaY;
        flags &= ~gaLineXMajor;
        }

    /* Clip the line and reject it if nothing is to be drawn */
    clip1 = clip2 = outcode1 = outcode2 = 0;
    OUTCODES(outcode1,x1,y1);
    OUTCODES(outcode2,x2,y2);
    if (_MGL_clipLine(clipLeft,clipTop,clipRight-1,clipBottom-1,
            &new_x1, &new_y1, &new_x2, &new_y2,
            absDeltaX, absDeltaY, &clip1, &clip2,
            flags, 0, outcode1, outcode2) == -1) {
        /* Trivial rejection */
        return;
        }
    if (flags & gaLineXMajor)
        count = abs(new_x2 - new_x1);
    else
        count = abs(new_y2 - new_y1);

    /* Always draw the last pixel if it has been clipped */
    if (clip2 != 0 || drawLast)
        count++;
    if (!count)
        return;

    /* If we need to swap XY endpoints, call all over again for correct logic. */
    if (reversed && !swapped) {
        SWAP(new_x1,new_x2);
        SWAP(new_y1,new_y2);
        swapped = true;
        _MGL_drawClippedFatLineInt(new_x1,new_y1,new_x2,new_y2,drawLast,clipLeft,clipTop,clipRight,clipBottom);
        swapped = false;
        return;
        }

    /* Unwind bresenham error term to first point */
    if (clip1) {
        clipdx = abs(new_x1 - x1);
        clipdy = abs(new_y1 - y1);
        if (flags & gaLineXMajor)
            initialError += ((clipdy*diagInc) + ((clipdx-clipdy)*majorInc));
        else
            initialError += ((clipdx*diagInc) + ((clipdy-clipdx)*majorInc));
        }

    /* Setup to scan the line into our buffer */
    height = new_y2 - new_y1;
    length = ABS(height) + 1 + DC.a.penHeight;
    scanList.length = length;
    if ((int)(length * sizeof(scanline)) > _MGL_bufSize)
        MGL_fatalError("Scratch buffer too small for rendering objects!\n");
    scanPtr = scanList.scans = _MGL_buf;
    oldy = new_y1;
    oldx = new_x1;
    penHeight = DC.a.penHeight;
    penWidth = DC.a.penWidth;

    /* Now scan the line into our buffer */
    if ((new_x2 - new_x1) == 0) {
        /* We have a vertical line, so special case it */
        new_x2 += penWidth + 1;
        for (i = 0; i < length; i++,scanPtr++) {
            scanPtr->left = new_x1;
            scanPtr->right = new_x2;
            }
        scanPtr = _MGL_buf;
        }
    else if (height == 0) {
        /* We have a horizontal line, so special case it */
        if (new_x2 < new_x1)
            SWAP(new_x1,new_x2);
        new_x2 += penWidth + 1;
        for (i = 0; i < length; i++,scanPtr++) {
            scanPtr->left = new_x1;
            scanPtr->right = new_x2;
            }
        scanPtr = _MGL_buf;
        }
    else if (height > 0) {
        /* We have a downward moving line. Scan convert most of the line
         * into the buffer by calling the lineEngine.
         */
        _MGL_bresenhamLineEngine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags,plotPointDown);

        /* Now fix the first penHeight+1 scanline left coordinates and the
         * last penHeight+1 scanline right coordinates.
         */
        scanPtr = _MGL_buf;
        scanPtr[length-1].left = (short)(new_x2);
        new_x2 += penWidth + 1;
        for (i = penHeight; i >= 0; i--) {
            scanPtr[i].left = (short)new_x1;
            scanPtr[length-i-1].right = (short)new_x2;
            }
        }
    else {
        /* We have an upward moving line. Scan convert most of the line
         * into the buffer by calling the lineEngine.
         */
        scanPtr += length-1;
        _MGL_bresenhamLineEngine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags,plotPointUp);

        /* Now fix the first penHeight+1 scanline left coordinates and the
         * last penHeight+1 scanline right coordinates.
         */
        scanPtr = _MGL_buf;
        scanPtr[0].left = (short)(new_x2);
        new_x2 += penWidth + 1;
        for (i = penHeight; i >= 0; i--) {
            scanPtr[length-i-1].left = (short)new_x1;
            scanPtr[i].right = (short)new_x2;
            }
        }
    if (height < 0)
        scanList.top = new_y2;
    else
        scanList.top = new_y1;

    /* We need to clip the scan converted line now to the clipping
     * rectangle, since some of the line MAY lie outside.
     */
    new_y2 = scanList.top+length;
    if (new_x2 < clipLeft || new_x2 >= clipRight || new_y2 < clipTop || new_y2 >= clipBottom)
        _MGL_drawClippedScanList(scanList.top,scanList.length,&scanList.scans->left,clipLeft,clipTop,clipRight,clipBottom);
    else
        DC.r.cur.DrawScanList(scanList.top,scanList.length,&scanList.scans->left);
}
