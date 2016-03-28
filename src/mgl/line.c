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
* Description:  Line drawing routines (including code for the
*               Cohen-Sutherland line clipping algorithm).
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#undef  MGL_moveTo
#undef  MGL_moveRel
#undef  MGL_lineCoord
#undef  MGL_line
#undef  MGL_lineExt
#undef  MGL_lineFX
#undef  MGL_lineTo
#undef  MGL_lineRel

/*******************************************************************************
DESCRIPTION:
Moves the CP to a new location.

HEADER:
mgraph.h

PARAMETERS:
x   - New x coordinate for CP
y   - New y coordinate for CP

REMARKS:
Moves the current position (CP) to the new point (x,y).

SEE ALSO:
MGL_moveTo
*******************************************************************************/
void MGLAPI MGL_moveToCoord(
    int x,
    int y)
{
    DC.a.CP.x = x;
    DC.a.CP.y = y;
}

/*******************************************************************************
DESCRIPTION:
Moves the CP to a new location.

HEADER:
mgraph.h

PARAMETERS:
p   - New Point for CP

REMARKS:
This function is the same as MGL_moveToCoord, however it takes the
coordinate to move to as a point.

SEE ALSO:
MGL_moveToCoord
*******************************************************************************/
void MGL_moveTo(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Moves the CP to a new relative location.

HEADER:
mgraph.h

PARAMETERS:
dx  - Amount to move x coordinate
dy  - Amount to move y coordinate

REMARKS:
Moves the current position (CP) to the relative location that is a distance of (dx,dy)
away from the CP. Thus the location the CP is moved to is (CP.x + dx, CP.y + dy).

SEE ALSO:
MGL_moveRel
****************************************************************************/
void MGLAPI MGL_moveRelCoord(
    int dx,
    int dy)
{
    DC.a.CP.x += dx;
    DC.a.CP.y += dy;
}

/****************************************************************************
DESCRIPTION:
Moves the CP to a new relative location.

HEADER:
mgraph.h

PARAMETERS:
p   - Use coordinates of this point as offsets

REMARKS:
This function is the same as MGL_moveRelCoord, however it takes the
amount to move as a point.

SEE ALSO:
MGL_moveRelCoord
****************************************************************************/
void MGL_moveRel(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Returns the x coordinate of the current position.

HEADER:
mgraph.h

RETURNS:
x coordinate of current position

REMARKS:
Returns the x coordinate of the current position (CP). The CP is the current graphics
cursor position, and is used by a number of output routines to determine where to
being drawing.

SEE ALSO:
MGL_getY, MGL_getCP,
****************************************************************************/
int MGLAPI MGL_getX(void)
{
    return DC.a.CP.x;
}

/****************************************************************************
DESCRIPTION:
Returns the y coordinate of the current position.

HEADER:
mgraph.h

RETURNS:
y coordinate of current position

REMARKS:
Returns the y coordinate of the current position (CP). The CP is the current graphics
cursor position, and is used by a number of output routines to determine where to
being drawing.

SEE ALSO:
MGL_getX, MGL_getCP

****************************************************************************/
int MGLAPI MGL_getY(void)
{
    return DC.a.CP.y;
}

/****************************************************************************
DESCRIPTION:
Returns the current position value.

HEADER:
mgraph.h

PARAMETERS:
CP  - Place to store the current position

REMARKS:
Returns the current position (CP). The CP is the current logical graphics cursor
position, and is used by a number of routines to determine where to being drawing
output. You can use the MGL_moveTo routine to directly
move the CP to a new position.

SEE ALSO:
MGL_moveTo, MGL_moveRel, MGL_lineTo, MGL_lineRel, MGL_drawStr
****************************************************************************/
void MGLAPI MGL_getCP(
    point_t *CP)
{
    *CP = DC.a.CP;
}

/****************************************************************************
PARAMETERS:
x1              - X coordinate for first pixel
y1              - Y coordinate for first pixel
initialError    - Initial error term for the line
majorInc        - Major increment for the line
diagInc         - Diagonal increment for the line
count           - Number of pixels to draw
flags           - Line draw flags (from graphics.h)
plotPoint       - User supplied pixel plotting routine

REMARKS:
This function generates the set of points on a line, and calls a user
supplied plotPoint routine for every point generated.
{secret}
****************************************************************************/
void _MGL_bresenhamLineEngine(
    int x1,
    int y1,
    int initialError,
    int majorInc,
    int diagInc,
    int count,
    int flags,
    void (MGLAPIP plotPoint)(
        long x,
        long y))
{
    int xincr,yincr;

    xincr = (flags & gaLineXPositive) ? 1 : -1;
    yincr = (flags & gaLineYPositive) ? 1 : -1;
    if (flags & gaLineXMajor) {
        /* Plot the points on the line */
        while (count--) {
            plotPoint(x1,y1);
            x1 += xincr;
            if (initialError < 0) {
                initialError += majorInc;
                }
            else {
                initialError += diagInc;
                y1 += yincr;
                }
            }
        }
    else {
        /* Plot the points on the line */
        while (count--) {
            plotPoint(x1,y1);
            y1 += yincr;
            if (initialError < 0) {
                initialError += majorInc;
                }
            else {
                initialError += diagInc;
                x1 += xincr;
                }
            }
        }
}

/****************************************************************************
DESCRIPTION:
Generates the set of integer points on a line, given integer coordinates.

HEADER:
mgraph.h

PARAMETERS:
x1          - X coordinate for first endpoint
y1          - Y coordinate for first endpoint
x2          - X coordinate for second endpoint
y2          - Y coordinate for second endpoint
plotPoint   - User supplied pixel plotting routine

REMARKS:
This function generates the set of points on a line, and calls a user
supplied plotPoint routine for every point generated.

SEE ALSO:
MGL_lineEngineFX, MGL_ellipseEngine, MGL_ellipseArcEngine
****************************************************************************/
void MGLAPI MGL_lineEngine(
    int x1,
    int y1,
    int x2,
    int y2,
    void (MGLAPIP plotPoint)(
        long x,
        long y))
{
    int     initialError;           /* Decision variable                */
    int     absDeltaX,absDeltaY;    /* Dx and Dy values for the line    */
    int     majorInc,diagInc;       /* Decision variable increments     */
    int     count;                  /* Current (x,y) and count value    */
    int     xincr,yincr;            /* Increment for values             */
    ibool   lowslope = true;        /* True if slope <= 1               */

    xincr = 1;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        xincr = -1;
        }
    yincr = 1;
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        yincr = -1;
        }
    if (absDeltaY > absDeltaX) {
        SWAP(absDeltaX,absDeltaY);
        lowslope = false;
        }
    majorInc = 2*absDeltaY;             /* Increment to move to E pixel     */
    initialError = majorInc - absDeltaX;/* Decision var = 2 * dy - dx       */
    diagInc = initialError - absDeltaX; /* diagInc = 2 * (dy - dx)          */
    count = absDeltaX+1;
    if (lowslope) {
        /* Plot the points on the line */
        while (count--) {
            plotPoint(x1,y1);
            x1 += xincr;
            if (initialError < 0) {
                initialError += majorInc;
                }
            else {
                initialError += diagInc;
                y1 += yincr;
                }
            }
        }
    else {
        /* Plot the points on the line */
        while (count--) {
            plotPoint(x1,y1);
            y1 += yincr;
            if (initialError < 0) {
                initialError += majorInc;
                }
            else {
                initialError += diagInc;
                x1 += xincr;
                }
            }
        }
}

/****************************************************************************
DESCRIPTION:
Draws a line with integer coordinates.

HEADER:
mgraph.h

PARAMETERS:
x1          - x coordinate for first endpoint
y1          - y coordinate for first endpoint
x2          - x coordinate for second endpoint
y2          - y coordinate for second endpoint
drawLast    - True to draw the last pixel in the line

REMARKS:
Draws a line starting at the point (x1,y1) and ending at the point (x2,y2)
in the current pen style, color and dimensions. The CP is not updated, and
the line is clipped to the current clipping rectangle if clipping in on.
Note that this function takes the coordinates of the lines in integer format.

If the drawLast parameter is true, the last pixel in the line is drawn. If
not the last pixel is skipped. This is useful for drawing polylines so
that the lines join up correctly and do not cause over draw when drawing
in non-replace mode.

SEE ALSO:
MGL_line, MGL_lineCoord
****************************************************************************/
void MGLAPI MGL_lineCoordExt(
    int x1,
    int y1,
    int x2,
    int y2,
    ibool drawLast)
{
    rect_t              clip;
    clippedLineIntFunc  drawClippedLine;

    if (y1 == y2 && (DC.a.lineStyle == MGL_LINE_PENSTYLE)) {
        /* Draw a horizontal line as a rectangle */
        if (x2 < x1)
            SWAP(x1,x2);
        MGL_fillRectCoord(x1,y1,x2+DC.a.penWidth+1,y1+DC.a.penHeight+1);
        }
    else {
        if (DC.a.penStyle == MGL_BITMAP_SOLID && DC.a.penWidth == 0 && DC.a.penHeight == 0)
            drawClippedLine = _MGL_drawClippedLineInt;
        else
            drawClippedLine = _MGL_drawClippedFatLineInt;
        BEGIN_VISIBLE_CLIP_LIST(&DC);
        if (DC.clipRegionScreen) {
            x1 += DC.viewPort.left;   y1 += DC.viewPort.top;
            x2 += DC.viewPort.left;   y2 += DC.viewPort.top;
            BEGIN_CLIP_REGION(clip,DC.clipRegionScreen);
                drawClippedLine(
                    x1,y1,x2,y2,drawLast,
                    clip.left,clip.top,clip.right,clip.bottom);
            END_CLIP_REGION();
            }
        else {
            /* Clip and draw the line */
            drawClippedLine(
                x1 + DC.viewPort.left,y1 + DC.viewPort.top,
                x2 + DC.viewPort.left,y2 + DC.viewPort.top,drawLast,
                DC.clipRectScreen.left,DC.clipRectScreen.top,
                DC.clipRectScreen.right,DC.clipRectScreen.bottom);
            }
        END_VISIBLE_CLIP_LIST(&DC);
        }
}

/****************************************************************************
DESCRIPTION:
Draws a line with integer coordinates.

HEADER:
mgraph.h

PARAMETERS:
x1          - x coordinate for first endpoint
y1          - y coordinate for first endpoint
x2          - x coordinate for second endpoint
y2          - y coordinate for second endpoint

REMARKS:
Draws a line starting at the point (x1,y1) and ending at the point (x2,y2)
in the current pen style, color and dimensions. The CP is not updated, and
the line is clipped to the current clipping rectangle if clipping in on.
Note that this function takes the coordinates of the lines in integer format.

SEE ALSO:
MGL_line, MGL_lineCoordExt
****************************************************************************/
void MGLAPI MGL_lineCoord(
    int x1,
    int y1,
    int x2,
    int y2);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a line with integer coordinates.

HEADER:
mgraph.h

PARAMETERS:
p1  - First endpoint of line
p2  - Second endpoint of line

REMARKS:
This function is the same as MGL_lineCoord, however it takes the
coordinates of the line as two points.

SEE ALSO:
MGL_lineFX, MGL_lineCoord, MGL_lineCoordFX
****************************************************************************/
void MGL_line(
    point_t p1,
    point_t p2);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a line with integer coordinates.

HEADER:
mgraph.h

PARAMETERS:
p1          - First endpoint of line
p2          - Second endpoint of line
drawLast    - True to draw the last pixel in the line

REMARKS:
This function is the same as MGL_lineCoordExt, however it takes the
coordinates of the line as two points.

SEE ALSO:
MGL_lineFX, MGL_lineCoordExt, MGL_lineCoord, MGL_lineCoordFX
****************************************************************************/
void MGL_lineExt(
    point_t p1,
    point_t p2,
    ibool drawLast);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a line from the CP to the specified point.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to draw to
y   - y coordinate to draw to

REMARKS:
Draws a line from the current position (CP) to the new point (x,y). The CP
is set to the point (x,y) on return from this routine.

SEE ALSO:
MGL_line, MGL_lineCoord, MGL_lineTo, MGL_lineRel, MGL_lineRelCoord,
MGL_moveRel, MGL_moveRelCoord
****************************************************************************/
void MGLAPI MGL_lineToCoord(
    int x,
    int y)
{
    point_t p;

    p.x = x;    p.y = y;
    MGL_lineCoordExt(DC.a.CP.x,DC.a.CP.y,p.x,p.y,true);
    DC.a.CP = p;
}

/****************************************************************************
DESCRIPTION:
Draws a line from the CP to the specified point.

HEADER:
mgraph.h

PARAMETERS:
p   - Point to draw to

REMARKS:
This function is the same as MGL_lineToCoord, however the point to draw to
is passed as a point.

SEE ALSO:
MGL_line, MGL_lineCoord, MGL_lineToCoord, MGL_lineRel, MGL_lineRelCoord,
MGL_moveRel, MGL_moveRelCoord
****************************************************************************/
void MGL_lineTo(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a relative line.

HEADER:
mgraph.h

PARAMETERS:
dx  - Amount to offset in x coordinate
dy  - Amount to offset in y coordinate

REMARKS:
Draws a line from the current position (CP) to the relative location that is a distance
of (dx,dy) away from the CP. Thus the location of the next point on the line is:

    (CP.x + dx, CP.y + dy)

The CP is updated to this value.

SEE ALSO:
MGL_line, MGL_lineCoord, MGL_lineTo, MGL_lineToCoord, MGL_lineRel,
MGL_moveRel, MGL_moveRelCoord
****************************************************************************/
void MGLAPI MGL_lineRelCoord(
    int dx,
    int dy)
{
    point_t p1,p2;

    p1 = p2 = DC.a.CP;
    p2.x += dx; p2.y += dy;
    MGL_lineCoordExt(p1.x,p1.y,p2.x,p2.y,true);
    DC.a.CP = p2;
}

/****************************************************************************
DESCRIPTION:
Draws a relative line.

HEADER:
mgraph.h

PARAMETERS:
p   - Amount to offset in (x,y) coordinates

REMARKS:
This function is the same as MGL_lineRelCoord, however the amount to move
the CP by is passed as a point.

SEE ALSO:
MGL_line, MGL_lineCoord, MGL_lineTo, MGL_lineToCoord, MGL_lineRelCoord,
MGL_moveRel, MGL_moveRelCoord
****************************************************************************/
void MGL_lineRel(
    point_t p);
/* Implemented as a macro */

