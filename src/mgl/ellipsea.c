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
* Description:  Elliptical arc drawing routines and software only rendering
*               engine code.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

static scanline2    *scanList;  /* Start of scanline list               */
static scanline2    *scanPtr;   /* Pointer to current scan line         */
static int          length,B;   /* Minor axis size for ellipse          */
static int          topY,botY;  /* Current top/bottom scan line         */
static int          left,right; /* Current scan line extents            */
static ibool        draw[5];
static int          centreX,centreY;
static int          startX,startY,startQuad,startAng;
static int          endX,endY,endQuad,endAng;
static void         (MGLAPIP putPixel)(N_int32 x,N_int32 y);

/* Integer cosine values for angle's between 0-90 degrees, with values
 * between 0-10000.
 */
static int CosTable[] = {
    0x2710,0x270E,0x270A,0x2702,0x26F8,0x26EA,0x26D9,0x26C5,0x26AF,0x2695,
    0x2678,0x2658,0x2635,0x2610,0x25E7,0x25BB,0x258D,0x255B,0x2527,0x24EF,
    0x24B5,0x2478,0x2438,0x23F5,0x23AF,0x2367,0x231C,0x22CE,0x227D,0x222A,
    0x21D4,0x217C,0x2120,0x20C3,0x2062,0x2000,0x1F9A,0x1F32,0x1EC8,0x1E5B,
    0x1DEC,0x1D7B,0x1D07,0x1C92,0x1C19,0x1B9F,0x1B23,0x1AA4,0x1A23,0x19A1,
    0x191C,0x1895,0x180D,0x1782,0x16F6,0x1668,0x15D8,0x1546,0x14B3,0x141E,
    0x1388,0x12F0,0x1257,0x11BC,0x1120,0x1082,0x0FE3,0x0F43,0x0EA2,0x0E00,
    0x0D5C,0x0CB8,0x0C12,0x0B6C,0x0AC4,0x0A1C,0x0973,0x08CA,0x081F,0x0774,
    0x06C8,0x061C,0x0570,0x04C3,0x0415,0x0368,0x02BA,0x020B,0x015D,0x00AF,
    0x0000};

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
A           - Major axis for ellipse
B           - Minor axis for ellipse
cx,cy       - Centre of the ellipse (*2)
angle       - Angle in degrees
rx,ry       - Place to store the results
quadrant    - Quadrant point is in

REMARKS:
This routine computes the coordinates of a point on an ellipse centered on
the point (cx/2,cy/2) with major and minor axes of A and B respectively at
an 'angle' in degrees. The angle can be any multiple of 360 degrees.

The point is only a very close approximation. It is quite possible that the
point be inside or outside of the ellipse by one pixel, but this is good
enough for what we need it for.

Note that an angle of 45 degrees will always produce a point such that a
line between the centre of the ellipse and the point will go through the
top right hand corner of the bounding rectangle.

Note that the values of cx and cy should be twice the real value of the
centre, to take into account the possibility of having an ellipse with a
non-integer centre.
{secret}
****************************************************************************/
void _MGL_computeCoords(
    int A,
    int B,
    int cx,
    int cy,
    int angle,
    int *rx,
    int *ry,
    int *quadrant)
{
    int     x,y,q;

    /* Lookup the coordinates on an ellipse with a radius of 10000 */
    if (angle < 180) {
        if (angle < 90) {       /* Quadrant 1   */
            x = CosTable[angle];
            y = -CosTable[90 - angle];
            q = 1;
            }
        else {                  /* Quadrant 2   */
            x = -CosTable[180 - angle];
            y = -CosTable[angle - 90];
            q = 2;
            }
        }
    else {
        if (angle < 270) {      /* Quadrant 3   */
            x = -CosTable[angle - 180];
            y = CosTable[270 - angle];
            q = 3;
            }
        else {                  /* Quadrant 4   */
            x = CosTable[360 - angle];
            y = CosTable[angle - 270];
            q = 4;
            }
        }

    /* Scale the results to the size of the ellipse's major and minor
     * axes. Note that the major and minor axes are twice the size of
     * the semi-major and minor axes, so they are twice the size of the
     * radius for our generic ellipse.
     *
     * Note that the ellipse may be centred on a non-integer coordinate,
     * in which case the result must be offset by a 1/2 a pixel.
     */
    x = ((long)x * A) / 10000;
    y = ((long)y * B) / 10000;

    /* Return the coordinates relative to the centre of the ellipse */
    switch (q) {
        case 1:                 /* Quadrant 1   */
            *rx = (cx + x+1) / 2;
            *ry = (cy + y) / 2;
            break;
        case 2:                 /* Quadrant 2   */
            *rx = (cx + x) / 2;
            *ry = (cy + y) / 2;
            break;
        case 3:                 /* Quadrant 3   */
            *rx = (cx + x) / 2;
            *ry = (cy + y+1) / 2;
            break;
        default:                /* Quadrant 4   */
            *rx = (cx + x+1) / 2;
            *ry = (cy + y+1) / 2;
            break;
        }

    *quadrant = q;

    /* Fixup the coordinates for angles 0, 90, 180 and 270 for odd sized
     * ellipses
     */
    if (angle == 0 || angle == 180)
        *ry = cy/2;
    if (angle == 90 || angle == 270)
        *rx = cx/2;
}

/****************************************************************************
PARAMETERS:
angle

RETURNS:
Equivalent angle in the range 0-360 degrees.
{secret}
****************************************************************************/
int _MGL_fixAngle(
    int angle)
{
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
    return angle;
}

/****************************************************************************
PARAMETERS:
_topY   - Y coordinate of top pixels
_botY   - Y coordinate of bottom pixels
_left   - Left coordinate of scan lines
_right  - Right coordinate of scan lines

REMARKS:
Sets up the static variables required by all the ellipse arc scan conversion
engine.
****************************************************************************/
static void MGLAPI arc_setup(
    int _topY,
    int _botY,
    int _left,
    int _right)
{
    topY    = _topY;
    botY    = _botY;
    left    = _left;
    right   = _right;
}

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines
region1 - True if scan converting region 1

REMARKS:
This is the workhorse routine for drawing arcs. We handle all four quadrants
with symmetry in this routine. To do this we basically need to check when
to begin and when to stop drawing for each quadrant. Because of the fact
that the starting and ending coordinates may not fall _EXACTLY_ on the
ellipse, we check for the condition where we have hit or gone past the
toggle point.

In order to avoid toggling more than once, we reset the quadrant flags so
that the tests will not be performed again.
****************************************************************************/
static void MGLAPI arc_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    if (inc_y) {
        topY++;     botY--;
        }
    if (inc_x) {
        right++;    left--;
        }

    /* Note: When we hit a toggle point, two things can happen. Drawing can
     * be toggled on, in which case we wish to begin drawing with the
     * toggle point, or drawing can be toggled off, in which case we still
     * wish to draw the point where the toggle occurred.
     *
     * To handle this, when drawing is toggled on for a quadrant, the flag
     * is set to 1, when it is toggled off, it is set to 2, which we can
     * later set to 1 or 0.
     */

    /* Quadrant 1 - Checks to see if drawing should be toggled  */
    if (startQuad == 1 && ((region1 && right >= startX) || (!region1 && topY >= startY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }

    if (endQuad == 1 && ((region1 && right >= endX) || (!region1 && topY >= endY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 2 - Checks to see if drawing should be toggled  */
    if (startQuad == 2 && ((region1 && left <= startX) || (!region1 && topY >= startY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }

    if (endQuad == 2 && ((region1 && left <= endX) || (!region1 && topY >= endY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 3 - Checks to see if drawing should be toggled  */
    if (startQuad == 3 && ((region1 && left <= startX) || (!region1 && botY <= startY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }

    if (endQuad == 3 && ((region1 && left <= endX) || (!region1 && botY <= endY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }

    /* Quadrant 4 - Checks to see if drawing should be toggled  */
    if (startQuad == 4 && ((region1 && right >= startX) || (!region1 && botY <= startY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }

    if (endQuad == 4 && ((region1 && right >= endX) || (!region1 && botY <= endY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }

    /* If we are currently drawing in each of the quadrants, then update
     * the scanline data to reflect the new pixels added.
     */
    if (draw[1]) {
        putPixel(right,topY);                   /* Draw the pixel   */
        draw[1] = draw[1] & 1;                  /* Correct toggle   */
        }
    if (draw[2]) {
        putPixel(left,topY);                    /* Draw the pixel   */
        draw[2] = draw[2] & 1;                  /* Correct toggle   */
        }
    if (draw[3]) {
        putPixel(left,botY);                    /* Draw the pixel   */
        draw[3] = draw[3] & 1;                  /* Correct toggle   */
        }
    if (draw[4]) {
        putPixel(right,botY);                   /* Draw the pixel   */
        draw[4] = draw[4] & 1;                  /* Correct toggle   */
        }
}

static void MGLAPI arc_finished(void) {}

/****************************************************************************
PARAMETERS:
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)
plotPoint   - Pixel plotting routine

REMARKS:
Renders the points on the frame of an elliptical arc just inside the
mathematical boundary of extentRect.
{secret}
****************************************************************************/
void __MGL_ellipseArcEngine(
    int left,
    int top,
    int A,
    int B,
    int startAngle,
    int endAngle,
    arc_coords_t *ac,
    void (MGLAPIP plotPoint)(
        N_int32 x,
        N_int32 y))
{
    int     i;

    putPixel = plotPoint;
    centreX = left * 2 + A;
    centreY = top * 2 + B;

    /* Ensure that the angles are within the range 0 - 2*PI */
    startAngle = _MGL_fixAngle(startAngle);
    endAngle = _MGL_fixAngle(endAngle);
    _MGL_computeCoords(A,B,centreX,centreY,startAngle,&startX,&startY,&startQuad);
    _MGL_computeCoords(A,B,centreX,centreY,endAngle,&endX,&endY,&endQuad);

    /* Divide centre values by 2 and make viewport relative */
    centreX = centreX / 2;
    centreY = centreY / 2;

    /* Determine initial quadrant drawing flag conditions */
    draw[0] = draw[1] = draw[2] = draw[3] = draw[4] = false;
    if (startQuad == 1 || startQuad == 3)
        draw[startQuad] = true;
    if (endQuad == 2 || endQuad == 4)
        draw[endQuad] = true;
    for (i = (endQuad < startQuad) ? endQuad+2 : endQuad-2; i >= startQuad; i--)
        draw[i % 4 + 1] = true;
    if (startQuad == endQuad) {
        if (startAngle < endAngle)
            draw[startQuad] = false;
        else
            draw[1] = draw[2] = draw[3] = draw[4] = true;
        }
    __MGL_ellipseEngine(left,top,A,B,arc_setup,arc_set4pixels,arc_finished);

    /* Save the arc coordinate information for later */
    ac->x = centreX;
    ac->y = centreY;
    ac->startX = startX;
    ac->startY = startY;
    ac->endX = endX;
    ac->endY = endY;
}

/****************************************************************************
PARAMETERS:
_topY   - Y coordinate of top pixels
_botY   - Y coordinate of bottom pixels
_left   - Left coordinate of scan lines
_right  - Right coordinate of scan lines

REMARKS:
Sets up the static variables required by all the ellipse filled arc scan
converion routines.
****************************************************************************/
static void MGLAPI fill_arc_setup(
    int _topY,
    int _botY,
    int _left,
    int _right)
{
    /* Adjust coordinates so that values are always relative to the
     * y cooridinate held in scanline list.
     */
    topY    = 0;
    botY    = _botY - _topY;
    startY  -= _topY;
    endY    -= _topY;
    centreY -= _topY;
    left    = _left;
    right   = _right;
}

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines
region1 - True if scan converting region 1

REMARKS:
This is the workhorse routine for drawing filled arcs. We handle all four
quadrants with symmetry in this routine. To do this we basically need to
check when to begin and when to stop drawing for each quadrant. Because of
the fact that the starting and ending coordinates may not fall _EXACTLY_
on the ellipse, we check for the condition where we have hit or gone past
the toggle point.

This routine basically scan converts the perimeter of the elliptical arc
into the scanline buffer.

In order to avoid toggling more than once, we reset then quadrant flags so
that the tests will not be performed again.
****************************************************************************/
static void MGLAPI fill_arc_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    if (inc_y) {
        topY++;     botY--;
        }
    if (inc_x) {
        right++;    left--;
        }

    /* Note: When we hit a toggle point, two things can happen. Drawing can
     * be toggled on, in which case we wish to begin drawing with the
     * toggle point, or drawing can be toggled off, in which case we still
     * wish to draw the point where the toggle occurred.
     *
     * To handle this, when drawing is toggled on for a quadrant, the flag
     * is set to 1, when it is toggled off, it is set to 2, which we can
     * later set to 1 or 0.
     */

    /* Quadrant 1 - Checks to see if drawing should be toggled  */
    if (startQuad == 1 && ((region1 && right >= startX) || (!region1 && topY >= startY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }

    if (endQuad == 1 && ((region1 && right >= endX) || (!region1 && topY >= endY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 2 - Checks to see if drawing should be toggled  */
    if (startQuad == 2 && ((region1 && left <= startX) || (!region1 && topY >= startY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }

    if (endQuad == 2 && ((region1 && left <= endX) || (!region1 && topY >= endY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 3 - Checks to see if drawing should be toggled  */
    if (startQuad == 3 && ((region1 && left <= startX) || (!region1 && botY <= startY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }

    if (endQuad == 3 && ((region1 && left <= endX) || (!region1 && botY <= endY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }

    /* Quadrant 4 - Checks to see if drawing should be toggled  */
    if (startQuad == 4 && ((region1 && right >= startX) || (!region1 && botY <= startY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }

    if (endQuad == 4 && ((region1 && right >= endX) || (!region1 && botY <= endY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }

    /* Quick hack to handle de-generate ellipses in the y axis - this
     * will stop the code from creating open-ended scanlines, but still
     * makes sure that we find the correct starting and ending points
     * on the ellipse.
     */
    if (topY > B)
        return;

    /* If we are currently drawing in each of the quadrants, then update
     * the scanline data to reflect the new pixels added.
     */
    if (draw[1]) {
        scanList[topY].rightR = right+1;        /* Exterior border  */
        draw[1] = draw[1] & 1;                  /* Correct toggle   */
        }
    if (draw[2]) {
        scanList[topY].leftL = left;            /* Exterior border  */
        draw[2] = draw[2] & 1;                  /* Correct toggle   */
        }
    if (draw[3]) {
        scanList[botY].leftL = left;            /* Exterior border  */
        draw[3] = draw[3] & 1;                  /* Correct toggle   */
        }
    if (draw[4]) {
        scanList[botY].rightR = right+1;        /* Exterior border  */
        draw[4] = draw[4] & 1;                  /* Correct toggle   */
        }
}

/****************************************************************************
PARAMETERS:
x,y - Position to plot point at

REMARKS:
Workhorse routine to cap the start or end of the filled elliptical wedge.
****************************************************************************/
static void MGLAPI fillPlotPoint(
    long x,
    long y)
{
    scanPtr[y].leftL = (short)x;
}

/****************************************************************************
REMARKS:
This is called when the ellipse engine has completed the scan conversion
process. At this stage the perimeter has been scan converted into the
scanline buffer. All that is left to do is to scan convert the edges of the
wedge from the point on the ellipse to the centre of the arc.

Note that we must be careful when the startAngle is < endAngle and the
values are in the same quadrant.
****************************************************************************/
static void MGLAPI fill_arc_finished(void)
{
    point_t p1,p2;

    /* Invert the starting and ending quadrant's back to normal */
    startQuad = -startQuad;
    endQuad = -endQuad;

    /* Now we need to cap the starting and ending segments of the wedge
     * by scan converting a line between the correct coordinate and the
     * centre of the arc. The y coordinate values passed by the line engine
     * will be the scanlist indexes to use, while the x coordinates will
     * be the extents to cap with.
     */
    p1.x = centreX; p1.y = centreY;
    p2.x = startX;  p2.y = startY;

    if (startAng != 0 && startAng != 180) {
        /* To simplify things, we set the leftL part of the scanPtr2 pointer
         * to point to the section of the scanline list that we need to modify
         * for each point on the line. This way we can use a single plotPoint
         * routine.
         */
        if (draw[0]) {
            /* Special case when the ending angle is before the starting angle
             * in the same quadrant, and we now have a non-convex object to
             * scan convert.
             */
            if (startQuad == 1 || startQuad == 2) {
                scanPtr = (scanline2 *)&scanList->leftR;
                p1.x++; p2.x++;
                }
            else
                scanPtr = (scanline2 *)&scanList->rightL;
            }
        else {
            if (startQuad == 1 || startQuad == 2) {
                scanPtr = (scanline2 *)&scanList->rightR;
                p1.x++; p2.x++;
                }
            else
                scanPtr = (scanline2 *)&scanList->leftL;
            }
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,fillPlotPoint);
        }
    else {
        if (startAng == 0)
            scanList[startY].rightR = startX+1;
        else
            scanList[startY].leftL = startX;
        }

    p1.x = centreX; p1.y = centreY;
    p2.x = endX;    p2.y = endY;

    if (endAng != 0 && endAng != 180) {
        /* To simplify things, we set the leftL part of the scanPtr2 pointer
         * to point to the section of the scanline list that we need to modify
         * for each point on the line. This way we can use a single plotPoint
         * routine.
         */
        if (draw[0]) {
            /* Special case when the ending angle is before the starting angle
             * in the same quadrant, and we now have a non-convex object to
             * scan convert.
             */
            if (endQuad == 1 || endQuad == 2)
                scanPtr = (scanline2 *)&scanList->rightL;
            else {
                scanPtr = (scanline2 *)&scanList->leftR;
                p1.x++; p2.x++;
                }
            }
        else {
            if (endQuad == 1 || endQuad == 2)
                scanPtr = (scanline2 *)&scanList->leftL;
            else {
                scanPtr = (scanline2 *)&scanList->rightR;
                p1.x++; p2.x++;
                }
            }
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,fillPlotPoint);
        }
    else {
        if (endAng == 0)
            scanList[endY].rightR = endX+1;
        else
            scanList[endY].leftL = endX;
        }
}

/****************************************************************************
PARAMETERS:
sl          - Scanline list to fill
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)

REMARKS:
Setup code to begin filled elliptical arc scan conversion.
{secret}
****************************************************************************/
void _MGL_scanArcSetup(
    scanlist2 *sl,
    int left,
    int top,
    int A,
    int _B,
    int startAngle,
    int endAngle)
{
    int     i;

    /* Get memory for the ellipse arc scan line extents */
    if ((int)((sl->length = length = _B+1) * sizeof(scanline2)) > _MGL_bufSize)
        _MGL_scratchTooSmall();
    sl->scans = scanList = _MGL_buf;
    sl->top = top;
    B = _B/2;
    centreX = left * 2 + A;
    centreY = top * 2 + _B;

    /* Ensure that the angles are within the range 0 - 2*PI */
    startAng = startAngle = _MGL_fixAngle(startAngle);
    endAng = endAngle = _MGL_fixAngle(endAngle);
    _MGL_computeCoords(A,_B,centreX,centreY,startAngle,&startX,&startY,&startQuad);
    _MGL_computeCoords(A,_B,centreX,centreY,endAngle,&endX,&endY,&endQuad);

    /* Divide centre values by 2 */
    centreX = centreX / 2;
    centreY = centreY / 2;

    /* Determine initial quadrant drawing flag conditions. The flags 1-4
     * flag whether drawing is on for that particular quadrant.
     */
    draw[0] = draw[1] = draw[2] = draw[3] = draw[4] = false;

    if (startQuad == 1 || startQuad == 3)
        draw[startQuad] = true;

    if (endQuad == 2 || endQuad == 4)
        draw[endQuad] = true;

    for (i = (endQuad < startQuad) ? endQuad+2 : endQuad-2; i >= startQuad; i--)
        draw[i % 4 + 1] = true;

    if (startQuad == endQuad) {
        if (startAngle < endAngle)
            draw[startQuad] = false;
        else
            draw[1] = draw[2] = draw[3] = draw[4] = true;
        }

    /* We use the draw[0] flag to determine when we have a special case that
     * will cause the scanline list to be non-convex, and must be terminated
     * in a special manner.
     */
    if (endAngle < startAngle) {
        if (startQuad == 1 || startQuad == 2) {
            if (endAngle > 0)
                draw[0] = true;
            }
        else if (endAngle > 180)
            draw[0] = true;
        }
}

/****************************************************************************
PARAMETERS:
sl          - Scanline list to fill
ac          - Place to store arc coordinates
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)

REMARKS:
Fills an elliptical arc (or wedge) just inside the mathematical boundary of
extentRect.
{secret}
****************************************************************************/
void _MGL_scanEllipseArc(
    scanlist2 *sl,
    arc_coords_t *ac,
    int left,
    int top,
    int A,
    int B,
    int startAngle,
    int endAngle)
{
    int         i;
    scanline2   *s;

    /* Scan the elliptical arc into the buffer */
    _MGL_scanArcSetup(sl,left,top,A,B,startAngle,endAngle);
    s = scanList;
    for (i = 0; i < length; i++,s++)
        s->leftL = s->leftR = s->rightL = s->rightR = SENTINEL;
    __MGL_ellipseEngine(left,top,A,B,fill_arc_setup,fill_arc_set4pixels,
        fill_arc_finished);

    /* Save the arc coordinate information for later */
    ac->x = centreX;
    ac->y = centreY + sl->top;
    ac->startX = startX;
    ac->startY = startY + sl->top;
    ac->endX = endX;
    ac->endY = endY + sl->top;
}

#if 0
// TODO: This needs to be fixed

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines
region1 - True if scan converting region 1

REMARKS:
This is the workhorse routine for drawing filled arcs. We handle all four
quadrants with symmetry in this routine. To do this we basically need to
check when to begin and when to stop drawing for each quadrant. Because of
the fact that the starting and ending coordinates may not fall _EXACTLY_
on the ellipse, we check for the condition where we have hit or gone past
the toggle point.

This routine basically scan converts the perimeter of the elliptical arc
into the scanline buffer.

In order to avoid toggling more than once, we reset then quadrant flags so
that the tests will not be performed again.
****************************************************************************/
static void MGLAPI rect_arc_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    if (inc_y) {
        topY++;     botY--;
        }
    if (inc_x) {
        right++;    left--;
        }

    /* Quadrant 1 - Checks to see if drawing should be toggled  */
    if (startQuad == 1 && ((region1 && right >= startX) || (!region1 && topY >= startY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }
    if (endQuad == 1 && ((region1 && right >= endX) || (!region1 && topY >= endY))) {
        ++draw[1];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 2 - Checks to see if drawing should be toggled  */
    if (startQuad == 2 && ((region1 && left <= startX) || (!region1 && topY >= startY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = topY;
        startQuad = -startQuad;
        }
    if (endQuad == 2 && ((region1 && left <= endX) || (!region1 && topY >= endY))) {
        ++draw[2];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = topY;
        endQuad = -endQuad;
        }

    /* Quadrant 3 - Checks to see if drawing should be toggled  */
    if (startQuad == 3 && ((region1 && left <= startX) || (!region1 && botY <= startY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        startX = left;          /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }
    if (endQuad == 3 && ((region1 && left <= endX) || (!region1 && botY <= endY))) {
        ++draw[3];              /* Toggle drawing for this quadrant */

        endX = left;            /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }

    /* Quadrant 4 - Checks to see if drawing should be toggled  */
    if (startQuad == 4 && ((region1 && right >= startX) || (!region1 && botY <= startY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        startX = right;         /* Save the _actual_ values of these */
        startY = botY;
        startQuad = -startQuad;
        }
    if (endQuad == 4 && ((region1 && right >= endX) || (!region1 && botY <= endY))) {
        ++draw[4];              /* Toggle drawing for this quadrant */

        endX = right;           /* Save the _actual_ values of these */
        endY = botY;
        endQuad = -endQuad;
        }
}

/****************************************************************************
PARAMETERS:
x,y - Position to plot point at
****************************************************************************/
static void MGLAPI rectPlotPoint(
    long x,
    long y)
{
    if (y >= 0 && y < length)
        scanPtr[y].leftL = x;
}

/****************************************************************************
REMARKS:
This is called when the ellipse engine has completed the scan conversion
process. At this stage the perimeter has been scan converted into the
scanline buffer. All that is left to do is to scan convert the edges of
the wedge from the point on the ellipse to the centre of the arc.

Note that we must be careful when the startAngle is < endAngle and the
values are in the same quadrant.
****************************************************************************/
static void MGLAPI rect_arc_finished(void)
{
    int     i;
    point_t p1,p2;

    /* Invert the starting and ending quadrant's back to normal */
    startQuad = -startQuad;
    endQuad = -endQuad;

    /* Now we need to cap the starting and ending segments of the wedge
     * by scan converting a line between the correct coordinate and the
     * centre of the arc. The y coordinate values passed by the line engine
     * will be the scanlist indexes to use, while the x coordinates will
     * be the extents to cap with.
     */
    p1.x = centreX; p1.y = centreY;
    p2.x = startX;  p2.y = startY;

    if (startAng != 0 && startAng != 180) {
        /* To simplify things, we set the leftL part of the scanPtr2 pointer
         * to point to the section of the scanline list that we need to modify
         * for each point on the line. This way we can use a single plotPoint
         * routine.
         */
        if (draw[0]) {
            /* Special case when the ending angle is before the starting angle
             * in the same quadrant, and we now have a non-convex object to
             * scan convert.
             */
            if (startQuad == 1 || startQuad == 2) {
                scanPtr = (scanline2 *)&scanList->leftR;
                p1.x++; p2.x++;
                }
            else
                scanPtr = (scanline2 *)&scanList->rightL;
            }
        else {
            if (startQuad == 1 || startQuad == 2) {
                scanPtr = (scanline2 *)&scanList->rightR;
                p1.x++; p2.x++;
                }
            else
                scanPtr = (scanline2 *)&scanList->leftL;
            }
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,rectPlotPoint);
        p1.x = 2*p2.x - p1.x;
        p1.y = 2*p2.y - p1.y;
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,rectPlotPoint);
        if (startQuad <= 2) {
            for (i = 0; i < p1.y; i++)
                scanPtr[i].leftL = p1.x;
            }
        else {
            for (i = p1.y+1; i < length; i++)
                scanPtr[i].leftL = p1.x;
            }
        }
    else {
        if (startAng == 0)
            scanList[startY].rightR = startX+1;
        else
            scanList[startY].leftL = startX;
        }

    p1.x = centreX; p1.y = centreY;
    p2.x = endX;    p2.y = endY;

    if (endAng != 0 && endAng != 180) {
        /* To simplify things, we set the leftL part of the scanPtr2 pointer
         * to point to the section of the scanline list that we need to modify
         * for each point on the line. This way we can use a single plotPoint
         * routine.
         */
        if (draw[0]) {
            /* Special case when the ending angle is before the starting angle
             * in the same quadrant, and we now have a non-convex object to
             * scan convert.
             */
            if (endQuad == 1 || endQuad == 2)
                scanPtr = (scanline2 *)&scanList->rightL;
            else {
                scanPtr = (scanline2 *)&scanList->leftR;
                p1.x++; p2.x++;
                }
            }
        else {
            if (endQuad == 1 || endQuad == 2)
                scanPtr = (scanline2 *)&scanList->leftL;
            else {
                scanPtr = (scanline2 *)&scanList->rightR;
                p1.x++; p2.x++;
                }
            }
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,rectPlotPoint);
        p1.x = 2*p2.x - p1.x;
        p1.y = 2*p2.y - p1.y;
        MGL_lineEngine(p1.x,p1.y,p2.x,p2.y,rectPlotPoint);
        if (startQuad <= 2) {
            for (i = 0; i < p1.y; i++)
                scanPtr[i].leftL = p1.x;
            }
        else {
            for (i = p1.y+1; i < length; i++)
                scanPtr[i].leftL = p1.x;
            }
        }
    else {
        if (endAng == 0)
            scanList[endY].rightR = endX+1;
        else
            scanList[endY].leftL = endX;
        }
}

/****************************************************************************
PARAMETERS:
sl          - Scanline list to fill
ac          - Place to store arc coordinates
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)

REMARKS:
Fills an elliptical arc (or wedge) just inside the mathematical boundary of
extentRect.
{secret}
****************************************************************************/
void _MGL_scanRectArc(
    scanlist2 *sl,
    arc_coords_t *ac,
    int left,
    int top,
    int A,
    int B,
    int startAngle,
    int endAngle,
    int penWidth,
    int penHeight)
{
    int         i;
    scanline2   *s;

    _MGL_scanArcSetup(sl,left,top,A,B,startAngle,endAngle);
    sl->length = (length += penHeight+1);
    for (s = scanList,i = 0; i < length; i++,s++) {
        s->leftL = left;
        s->rightL = s->leftR = left + A/2;
        s->rightR = left + A + penWidth + 2;
        }
    __MGL_ellipseEngine(left,top,A,B,fill_arc_setup,rect_arc_set4pixels,
        rect_arc_finished);

    /* Save the arc coordinate information for later */
    ac->x = centreX;
    ac->y = centreY;
    ac->startX = startX;
    ac->startY = startY;
    ac->endX = endX;
    ac->endY = endY;
}
#endif

/****************************************************************************
PARAMETERS:
extentRect  - Bounding rectangle defining the arc (syntax 2)
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)
ac          - Place to store arc coordinates

REMARKS:
Draws a fat pen elliptical arc just inside the mathematical boundary of
extentRect. The drawing is done as a clipped version of an enclosed fat
pen ellipse using the projected XY arc coordinates for the clipping
rectangle. Since the clipping rects become more complicated to derive for
arcs greater than 180 degrees, this routine expects the caller to draw
concave arcs in multiple passes.
{secret}
****************************************************************************/
void _MGL_fatPenArc(
    rect_t extentRect,
    int A,
    int B,
    int startAngle,
    int endAngle,
    arc_coords_t *ac)
{
    scanlist2       scanList;
    rect_t          r,clip;
    int             dx,dy,dw,dh,i;
    ibool           quad[5];

    /* Handle fat pen ellipse arc case as clipped fat ellipse outline */
    dx = DC.viewPort.left;
    dy = DC.viewPort.top;
    dw = DC.a.penWidth + 1;
    dh = DC.a.penHeight + 1;

    /* Calculate projected XY arc coordinates via scanlist setup step */
    _MGL_scanArcSetup(&scanList,extentRect.left + 0,extentRect.top + 0,A,B,startAngle,endAngle);

    /* Save the arc coordinate information for later */
    ac->x = centreX;
    ac->y = centreY;
    ac->startX = startX;
    ac->startY = startY;
    ac->endX = endX;
    ac->endY = endY;

    /* Set clip rect to projected XY arc coordinates */
    clip.left   = (startX > endX) ? endX - dw : startX - dw;
    clip.top    = (startY > endY) ? endY - dh : startY - dh;
    clip.right  = (startX > endX) ? startX + dw : endX + dw;
    clip.bottom = (startY > endY) ? startY + dh : endY + dh;

    /* Locate all inclusive quadrants based on drawing engine parameters */
    for (i = 0; i < 5; i++)
        quad[i] = draw[i];
    quad[startQuad] = true;
    quad[endQuad] = true;

    /* Handle clip rect cases where arc crosses quadrants */
    if (startQuad != endQuad || quad[0]) {
        if (quad[1]) {
            r = MGL_defRect(extentRect.left+A/2,extentRect.top,extentRect.right,extentRect.bottom-B/2);
            if (MGL_sectRect(clip,r,&r)) {
                if (quad[1] && quad[2])
                    r.top = extentRect.top - dh;
                if (quad[4] && quad[1])
                    r.right = extentRect.left + extentRect.right + dw;
                MGL_setClipRect(r);
                _MGL_fatPenEllipse(extentRect.left + dx,extentRect.top + dy,A,B,true);
                }
            }
        if (quad[2]) {
            r = MGL_defRect(extentRect.left,extentRect.top,extentRect.right-A/2,extentRect.bottom-B/2);
            if (MGL_sectRect(clip,r,&r)) {
                if (quad[1] && quad[2])
                    r.top = extentRect.top - dh;
                if (quad[2] && quad[3])
                    r.left = extentRect.left - dw;
                MGL_setClipRect(r);
                _MGL_fatPenEllipse(extentRect.left + dx,extentRect.top + dy,A,B,true);
                }
            }
        if (quad[3]) {
            r = MGL_defRect(extentRect.left,extentRect.top+B/2,extentRect.right-A/2,extentRect.bottom);
            if (MGL_sectRect(clip,r,&r)) {
                if (quad[2] && quad[3])
                    r.left = extentRect.left - dw;
                if (quad[3] && quad[4])
                    r.bottom = extentRect.top + extentRect.bottom + dh;
                MGL_setClipRect(r);
                _MGL_fatPenEllipse(extentRect.left + dx,extentRect.top + dy,A,B,true);
                }
            }
        if (quad[4]) {
            r = MGL_defRect(extentRect.left+A/2,extentRect.top+B/2,extentRect.right,extentRect.bottom);
            if (MGL_sectRect(clip,r,&r)) {
                if (quad[3] && quad[4])
                    r.bottom = extentRect.top + extentRect.bottom + dh;
                if (quad[4] && quad[1])
                    r.right = extentRect.left + extentRect.right + dw;
                MGL_setClipRect(r);
                _MGL_fatPenEllipse(extentRect.left + dx,extentRect.top + dy,A,B,true);
                }
            }
        }
    else {
        /* Draw normal fat pen ellipse clipped to our arc rect */
        MGL_setClipRect(clip);
        _MGL_fatPenEllipse(extentRect.left + dx,extentRect.top + dy,A,B,true);
        }
}
/****************************************************************************
DESCRIPTION:
Draws an elliptical arc outline.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle defining the arc (syntax 2)
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degrees)

REMARKS:
Draws the outline of an elliptical arc just inside the mathematical boundary
of extentRect. StartAngle specifies where the arc begins and is treated MOD
360. EndAngle specifies where the arc ends and is also treated MOD 360. The
angles are given in positive or negative degrees. Zero degrees is at 3
o'clock, 90 is at 12 o'clock high, 180 is at 9 o'clock and 270 is at 6
o'clock. Other angles are measured relative to the enclosing rectangle. Thus
an angle of 45 degrees always defines a line from the center of the
rectangle through it's top right corner, even if the rectangle isn't square.
The ellipse outline is drawn in the current pen color, style and size.

SEE ALSO:
MGL_ellipseArc, MGL_fillEllipseArc, MGL_ellipse, MGL_fillEllipse
****************************************************************************/
void MGLAPI MGL_ellipseArc(
    rect_t extentRect,
    int startAngle,
    int endAngle)
{
    int             A,B,clipit = 0;
    rect_t          r;
    region_t        *clipped,*ellipse,*pen;
    arc_coords_t    ac;
    rect_t          vp;
    int             startPhi,endPhi;

    /* Check for degenerate ellipses */
    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A <= 5 || B <= 5 || B < A/5 || A < B/5 || MGL_emptyRect(extentRect))
        return;

    /* Determine if the ellipse arc needs to be clipped.    */
    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (MGL_sectRect(DC.clipRectView,extentRect,&r))
        clipit = !MGL_equalRect(extentRect,r);
    else {
        /* Trivially reject ellipse             */
        END_VISIBLE_CLIP_LIST(&DC);
        return;
        }

    if (DC.clipRegionScreen) {
        MGL_offsetRect(extentRect,DC.viewPort.left,DC.viewPort.top);
        pen = MGL_rgnSolidRectCoord(0,0,DC.a.penWidth+1,DC.a.penHeight+1);
        ellipse = MGL_rgnEllipseArc(extentRect,startAngle,endAngle,pen);
        clipped = MGL_sectRegion(ellipse,DC.clipRegionScreen);
        if (!MGL_emptyRect(clipped->rect))
            __MGL_drawRegion(0,0,clipped);
        MGL_freeRegion(clipped);
        MGL_freeRegion(ellipse);
        MGL_freeRegion(pen);
        }
    else {
        if (DC.a.penWidth > 0 || DC.a.penHeight > 0) {
            /* Handle fat pen ellipse arc case as clipped fat ellipse outline */
            MGL_getViewport(&vp);

            /* Draw arcs up to 180 degrees as clipped fat ellipses */
            if (abs(endAngle - startAngle) > 180) {
                if (startAngle > endAngle)
                    SWAP(startAngle,endAngle);
                /* Draw concave arcs up to 90 degrees at a time */
                for (startPhi = endPhi = startAngle; endPhi < endAngle; startPhi = endPhi) {
                    endPhi = startPhi + 90;
                    if (endPhi > endAngle)
                        endPhi = endAngle;
                    _MGL_fatPenArc(extentRect,A,B,startPhi,endPhi,&ac);
                    }
                }
            else
                /* Draw convex arc as-is */
                _MGL_fatPenArc(extentRect,A,B,startAngle,endAngle,&ac);

            /* Restore viewport to disable clipping */
            MGL_setViewport(vp);
            }
        else {
            /* Draw the ellipse arc a pixel at a time */
            MGL_beginPixel();
            __MGL_ellipseArcEngine(extentRect.left + DC.viewPort.left,
                extentRect.top + DC.viewPort.top,A,B,
                startAngle,endAngle,&ac,
                clipit ? _MGL_clipPixel : DC.r.PutPixel);
            MGL_endPixel();
            }

        /* Save the arc coordinate information in the device context */
        DC.ac.x = ac.x - DC.viewPort.left;
        DC.ac.y = ac.y - DC.viewPort.top;
        DC.ac.startX = ac.startX - DC.viewPort.left;
        DC.ac.startY = ac.startY - DC.viewPort.top;
        DC.ac.endX = ac.endX - DC.viewPort.left;
        DC.ac.endY = ac.endY - DC.viewPort.top;
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Draws an elliptical arc outline.

HEADER:
mgraph.h

PARAMETERS:
x           - x coordinate for the center of elliptical arc (syntax 1)
y           - y coordinate for the center of elliptical arc (syntax 1)
xradius     - x radius for the elliptical arc (syntax 1)
yradius     - y radius for the elliptical arc (syntax 1)
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degrees)

REMARKS:
Draws the outline of an elliptical given the center and radii for the
ellipse. StartAngle specifies where the arc begins and is treated MOD 360.
EndAngle specifies where the arc ends and is also treated MOD 360. The angles
are given in positive or negative degrees. Zero degrees is at 3 o'clock, 90
is at 12 o'clock high, 180 is at 9 o'clock and 270 is at 6 o'clock. Other
angles are measured relative to the enclosing rectangle. Thus an angle of 45
degrees always defines a line from the center of the rectangle through it's
top right corner, even if the rectangle isn't square. The ellipse outline is
drawn in the current pen color, style and size.

Note that this routine can only work with integer semi-major and semi-minor
axes, but can sometimes be easier to work with (and is provided for
compatibility with other graphics packages). The MGL_ellipseArc routine is
more versatile than this, as you can have an ellipse with odd diameter
values, which you cannot get with the this routine.

SEE ALSO:
MGL_ellipseArc, MGL_fillEllipseArc, MGL_ellipse, MGL_fillEllipse
****************************************************************************/
void MGLAPI MGL_ellipseArcCoord(
    int x,
    int y,
    int xradius,
    int yradius,
    int startAngle,
    int endAngle)
{
    rect_t  r;

    r.left = x - xradius;
    r.right = x + xradius;
    r.top = y - yradius;
    r.bottom = y + yradius;
    MGL_ellipseArc(r,startAngle,endAngle);
}

/****************************************************************************
PARAMETERS:
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degress)
clip        - True if ellipse should be clipped

REMARKS:
Fills an elliptical arc (or wedge).
****************************************************************************/
static void __MGL_fillEllipseArc(
    int left,
    int top,
    int A,
    int B,
    int startAngle,
    int endAngle,
    int clip)
{
    int             i,lastIndex,leftL,leftR,rightL,rightR;
    arc_coords_t    ac;
    scanlist2       scanList;
    scanline2       *s;
    scanLineFunc    drawScanLine;

    /* Scan the outline of the ellipse into the memory buffer */
    _MGL_scanEllipseArc(&scanList,&ac,left,top,A,B,startAngle,endAngle);

    /* Draw the list of scanlines to be drawn. The following code looks
     * complicated, but it needs to take into account all the different
     * types of arrangements of the scanlines that can occur.
     */
    s = scanList.scans;
    lastIndex = scanList.top + scanList.length;
    drawScanLine = clip ? _MGL_clipScanLine : _MGL_drawScanLine;
    for (i = scanList.top; i < lastIndex; i++,s++) {
        leftL = s->leftL;   rightR = s->rightR;
        if (leftL == SENTINEL && rightR == SENTINEL)
            continue;
        leftR = s->leftR;   rightL = s->rightL;

        if (leftR < rightL) {
            drawScanLine(i,leftL,leftR);
            if (rightL != SENTINEL)
                drawScanLine(i,rightL,rightR);
            }
        else {
            if (leftL == SENTINEL)
                drawScanLine(i,rightL,rightR);
            else
                drawScanLine(i,leftL,rightR);
            }
        }

    /* Save the arc coordinate information in the device context */
    DC.ac.x = ac.x - DC.viewPort.left;
    DC.ac.y = ac.y - DC.viewPort.top;
    DC.ac.startX = ac.startX - DC.viewPort.left;
    DC.ac.startY = ac.startY - DC.viewPort.top;
    DC.ac.endX = ac.endX - DC.viewPort.left;
    DC.ac.endY = ac.endY - DC.viewPort.top;
}

/****************************************************************************
DESCRIPTION:
Fills an elliptical arc.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degrees)

REMARKS:
Fills an elliptical arc forming a wedge, just inside the mathematical
boundary of extentRect. StartAngle specifies where the arc begins and is
treated MOD 360. EndAngle specifies where the arc ends and is also treated
MOD 360. The angles are given in positive or negative degrees. Zero degrees
is at 3 o'clock, 90 is at 12 o'clock high, 180 is at 9 o'clock and 270 is
at 6 o'clock. Other angles are measured relative to the enclosing rectangle.
Thus an angle of 45 degrees always defines a line from the center of the
rectangle through it's top right corner, even if the rectangle isn't square.

This routine is more versatile than MGL_fillEllipseArcCoord, as it allows
an ellipse with odd diameter values, which you cannot get with the
MGL_fillEllipseArcCoord.

SEE ALSO:
MGL_fillEllipseArc, MGL_ellipseArc, MGL_ellipse, MGL_fillEllipse
****************************************************************************/
void MGLAPI MGL_fillEllipseArc(
    rect_t extentRect,
    int startAngle,
    int endAngle)
{
    int     A,B,clipit;
    rect_t  r;
    region_t *clipped,*ellipse;

    /* Check for degenerate ellipses */
    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A <= 5 || B <= 5 || B < A/5 || A < B/5 || MGL_emptyRect(extentRect))
        return;             /* Ignore degenerate ellipses           */

    /* Determine if the elliptical wedge needs to be clipped.   */
    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (MGL_sectRect(DC.clipRectView,extentRect,&r))
        clipit = !MGL_equalRect(extentRect,r);
    else {
        /* Trivially reject ellipse             */
        END_VISIBLE_CLIP_LIST(&DC);
        return;
        }

    if (DC.clipRegionScreen) {
        MGL_offsetRect(extentRect,DC.viewPort.left,DC.viewPort.top);
        ellipse = MGL_rgnSolidEllipseArc(extentRect,startAngle,endAngle);
        clipped = MGL_sectRegion(ellipse,DC.clipRegionScreen);
        if (!MGL_emptyRect(clipped->rect))
            __MGL_drawRegion(0,0,clipped);
        MGL_freeRegion(clipped);
        MGL_freeRegion(ellipse);
        }
    else {
        __MGL_fillEllipseArc(extentRect.left + DC.viewPort.left,
            extentRect.top + DC.viewPort.top,A,B,
            startAngle,endAngle,clipit);
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Fills an elliptical arc.

HEADER:
mgraph.h

PARAMETERS:
x           - x coordinate for center of arc
y           - y coordinate for center of arc
xradius     - x radius for the arc
yradius     - y radius for the arc
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degrees)

REMARKS:
Fills an elliptical arc given the center and radii for the ellipse.
StartAngle specifies where the arc begins and is treated MOD 360.
EndAngle specifies where the arc ends and is also treated MOD 360. The
angles are given in positive or negative degrees. Zero degrees is at 3
o'clock, 90 is at 12 o'clock high, 180 is at 9 o'clock and 270 is at 6
o'clock. Other angles are measured relative to the enclosing rectangle.
Thus an angle of 45 degrees always defines a line from the center of the
rectangle through it's top right corner, even if the rectangle isn't square.

SEE ALSO:
MGL_fillEllipseArc, MGL_ellipseArc, MGL_ellipse, MGL_fillEllipse
****************************************************************************/
void MGLAPI MGL_fillEllipseArcCoord(
    int x,
    int y,
    int xradius,
    int yradius,
    int startAngle,
    int endAngle)
{
    rect_t  r;

    r.left = x - xradius;
    r.right = x + xradius;
    r.top = y - yradius;
    r.bottom = y + yradius;
    MGL_fillEllipseArc(r,startAngle,endAngle);
}

/****************************************************************************
DESCRIPTION:
Generates the set of points on an elliptical arc.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle defining the arc
startAngle  - Starting angle for arc (in degrees)
endAngle    - Ending angle for arc (in degrees)
ac  - Place to store computed arc coordinates
plotPoint   - Function to call for every point on the elliptical arc

REMARKS:
This routine generates the set of points on a elliptical arc, and is the same
code used to generate elliptical arcs internally in MGL. You can call it
to generate the set of points on an elliptical arc, calling your own
plotPoint routine for every point on the arc. The points on the arc are
rasterized in order from the starting angle to the ending angle. After the
arc has been drawn, the arc coordinates are returned, which contains the
actual center, starting and ending points for the arc.

SEE ALSO:
MGL_ellipseEngine, MGL_lineEngine
****************************************************************************/
void MGLAPI MGL_ellipseArcEngine(
    rect_t extentRect,
    int startAngle,
    int endAngle,
    arc_coords_t *ac,
    void (MGLAPIP plotPoint)(
        N_int32 x,
        N_int32 y))
{
    int     A,B;

    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A <= 5 || B <= 5 || B < A/5 || A < B/5 || MGL_emptyRect(extentRect))
        return;         /* Ignore degenerate ellipses           */
    __MGL_ellipseArcEngine(extentRect.left,extentRect.top,A,B,startAngle,
        endAngle,ac,plotPoint);
}

