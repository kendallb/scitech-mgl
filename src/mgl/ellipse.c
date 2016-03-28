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
* Description:  Ellipse drawing routines.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

static scanline     *scanPtr;   /* Pointer to current scan line         */
static scanline2    *scanPtr2;  /* Pointer to current scan line         */
static int          B;          /* Minor axis size for ellipse          */
static int          left,right; /* Current scan line extents            */
static int          penWidth;   /* Current pen width                    */
static int          penHeight;  /* Current pen height                   */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:

Generates the set of points on an ellipse.

HEADER:
mgraph.h

PARAMETERS:
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis dimension
B           - Minor axis dimension
setup       - Routine called to initialize pixel plotting routines
set4pixels  - Routine called repeatedly for each set of 4 pixels
finished    - Routine called to complete plotting pixels

REMARKS:
An ellipse generation routine. This routine starts by calling setup() with
the coordinates of the four seed points for the ellipse. Then it calls
set4pixels once for each pixel in the ellipse, which optionally increments
the x and y coordinates of the four pixels. Finish is called at algorithm
completion to tie up any loose ends.
{secret}
****************************************************************************/
void __MGL_ellipseEngine(
    int left,
    int top,
    int A,
    int B,
    void (MGLAPIP setup)(
        int topY,
        int botY,
        int left,
        int right),
    void (MGLAPIP set4pixels)(
        ibool inc_x,
        ibool inc_y,
        ibool region1),
    void (MGLAPIP finished)(void))
{
    ibool   inc_x,inc_y;
    float   d,dx,dy;
    float   FourAsquared,EightAsquared,FourBsquared,EightBsquared;
    float   Asquared,Bsquared;
    int     _left,_top,_right,_bottom;

    /* Compute static variables before entering any of the loops. Note that
     * all the setup calculations are done in floating point as the numbers
     * get very large.
     */
    Asquared = (float)((long)A*(long)A);
    FourAsquared = 4*Asquared;
    EightAsquared = 2*FourAsquared;
    Bsquared = (float)((long)B*(long)B);
    FourBsquared = 4*Bsquared;
    EightBsquared = 2*FourBsquared;

    /* Check to see if the semi-major axis is integer, and compute the
     * initial coordinates and decision variables depending on the outcome.
     */
    dx = FourAsquared * B;
    _right = _left = left + (A >> 1);
    _top = top;
    _bottom = top + B;
    if (A & 1) {
        /* We have a non-integer semi-major axis    */
        d = Asquared - 2*Asquared*B + 9*Bsquared;
        dy = FourBsquared;
        _right++;
        }
    else {
        /* We have an integer semi-major axis       */
        d = Asquared - 2*Asquared*B + FourBsquared;
        dy = 0;
        }
    setup(_top,_bottom,_left,_right);

    /* REGION 1: Draw the points on the ellipse where dy/dx > -1    */
    inc_x = inc_y = false;
    while (dx > dy) {
        set4pixels(inc_x,inc_y,true);
        inc_x = true;
        inc_y = false;
        if (d >= 0) {               /* Select pixel SE  */
            inc_y = true;
            dx -= EightAsquared;
            d -= dx;
            }
        dy += EightBsquared;
        d += FourBsquared + dy;
        }

    /* REGION 2: Draw the points on the ellipse where dy/dx < -1
     *
     * Compute new decision variable for region 2.
     */
    d += 3*(Asquared - Bsquared) - ((dx + dy)/2);
    while (dx >= 0) {
        set4pixels(inc_x,inc_y,false);
        inc_x = false;
        inc_y = true;
        if (d < 0) {                /* Select pixel SE  */
            inc_x = true;
            dy += EightBsquared;
            d += dy;
            }
        dx -= EightAsquared;
        d += FourAsquared - dx;
        }

    finished();
}

/****************************************************************************
PARAMETERS:
_topY   - Y coordinate of top pixels
_botY   - Y coordinate of bottom pixels
_left   - Left coordinate of scan lines
_right  - Right coordinate of scan lines

REMARKS:
Sets up the static variables required by all the patterned pen ellipse scan
conversion routines.
****************************************************************************/
static void MGLAPI pen_setup(
    int _topY,
    int _botY,
    int _left,
    int _right)
{
    int         i,j,k;
    scanline2   *s = scanPtr2;

    /* Plot the interior pixels touched by the first pen location */
    (void)_topY;
    (void)_botY;
    right = _right;
    left = _left;
    j = _right;
    k = _left+penWidth+1;
    for (i = penHeight; i >= 0; i--,s++) {
        s->rightL = j;
        s->leftR = k;
        }
}

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines

REMARKS:
This is the workhorse routine for patterned pen ellipses or ellipses drawn
with wide pens. Here we build an array of double scan lines that represent
each scan line of the top two quadrants of the ellipse. This array is then
scan converted by the pen_finished() routine.
****************************************************************************/
static void MGLAPI pen_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    if (inc_y) {
        /* Set the interior borders for the current scan line */
        scanPtr2[penHeight+region1].rightL = right+region1;
        scanPtr2[penHeight+region1].leftR = left + penWidth+1-region1;

        /* Set the exterior border for the current scan line */
        scanPtr2->leftL = left;
        scanPtr2->rightR = right+penWidth+1;
        scanPtr2++;
        }
    if (inc_x) {
        right++;    left--;
        }
}

/****************************************************************************
REMARKS:
This is called when the ellipse engine has completed the scan conversion
process, and we now have an array containing the scan lines of the top two
quadrants for the ellipse.
****************************************************************************/
static void MGLAPI pen_finished(void)
{
    int         i,j,k;
    scanline2   *s = scanPtr2;

    /* Set the interior pixels touched by the last pen location */
    scanPtr2[penHeight].rightL = right;
    scanPtr2[penHeight].leftR = left + penWidth+1;

    /* Set the exterior pixels touched by the last pen location */
    j = left;
    k = right+penWidth+1;
    for (i = penHeight; i >= 0; i--,s++) {
        s->leftL = j;
        s->rightR = k;
        }
}

/****************************************************************************
PARAMETERS:
scanList    - Place to store scanline list info
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis

REMARKS:
Private function to frame an ellipse with a wide pen in the current color
and fill style. The extent rectangle is expected to be in local viewport
coordinates. We use the internal buffer to scan convert the ellipse.

We need a 3k buffer to scan convert an ellipse that is 768 scan lines high.
{secret}
****************************************************************************/
void _MGL_scanFatEllipse(
    scanlist2 *scanList,
    int left,
    int top,
    int A,
    int _B,
    int _penWidth,
    int _penHeight)
{
    /* Get memory for the ellipse scan line extents.    */
    scanList->length = (_B+_penHeight+1)/2;
    if ((scanList->length+_penHeight) * sizeof(scanline2) > (uint)_MGL_bufSize)
        _MGL_scratchTooSmall();
    scanPtr2 = scanList->scans = _MGL_buf;
    scanList->top = top;
    B = _B;
    penWidth = _penWidth;
    penHeight = _penHeight;
    __MGL_ellipseEngine(left,top,A,B,pen_setup,pen_set4pixels,pen_finished);
}

/****************************************************************************
PARAMETERS:
_topY   - Y coordinate of top pixels
_botY   - Y coordinate of bottom pixels
_left   - Left coordinate of scan lines
_right  - Right coordinate of scan lines

REMARKS:
Sets up the static variables required by all the ellipse filling scan
conversion routines.
****************************************************************************/
static void MGLAPI fill_setup(
    int _topY,
    int _botY,
    int _left,
    int _right)
{
    (void)_topY;
    (void)_botY;
    left = _left;
    right = _right;
}

/****************************************************************************
PARAMETERS:
inc_x   - Increment x coordinates of current scan lines
inc_y   - Increment y coordinates of current scan lines

REMARKS:
This is the workhorse routine for filled ellipses. If we make a move in the
x direction, we simply adjust the size of the current top and bottom scan
lines of the ellipse. If we make a move in the y direction, then we know
that the current top and bottom scan lines are complete, so we insert them
into the list of scan lines and move to the next ones.
****************************************************************************/
static void MGLAPI fill_set4pixels(
    ibool inc_x,
    ibool inc_y,
    ibool region1)
{
    (void)region1;
    if (inc_y) {
        scanPtr->left = left;
        scanPtr->right = right+1;   /* Low level routines need this */
        scanPtr++;
        }
    if (inc_x) {
        left--; right++;
        }
}

/****************************************************************************
REMARKS:
Fills the scan lines generated for the ellipse that are held in the scan
line array.
****************************************************************************/
static void MGLAPI fill_finished(void)
{
    scanPtr->left = left;       /* Complete the last scan line      */
    scanPtr->right = right+1;
}

/****************************************************************************
PARAMETERS:
scanList    - Scanline buffer to fill
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
_B          - Minor axis

REMARKS:
Scans the outline of a filled ellipse by scanning into the MGL scratch
buffer. The scanned ellipse image remains in the MGL scratch buffer and can
then be rendered or used to generate an elliptical region. Note also that
we only scan the top half of the ellipse into the buffer, as the bottom
half can be generated via symmetry.
{secret}
****************************************************************************/
void _MGL_scanEllipse(
    scanlist *scanList,
    int left,
    int top,
    int A,
    int _B)
{
    /* Get memory for the ellipse scan line extents */
    scanList->length = (_B+3)/2;
    if (scanList->length * sizeof(scanline) > (uint)_MGL_bufSize)
        _MGL_scratchTooSmall();
    scanPtr = scanList->scans = _MGL_buf;
    scanList->top = top;
    B = _B;
    __MGL_ellipseEngine(left,top,A,B,fill_setup,fill_set4pixels,fill_finished);
}

/****************************************************************************
PARAMETERS:
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
clip        - True if ellipse should be clipped

REMARKS:
Private function to frame an ellipse with a wide pen in the current color
and fill style. The extent rectangle is expected to be in local viewport
coordinates. We use the internal buffer to scan convert the ellipse.
****************************************************************************/
void _MGL_fatPenEllipse(
    int left,
    int top,
    int A,
    int B,
    int clip)
{
    int             i,j,maxIndex;
    scanlist2       scanList;
    scanline2       *scanPtr;

    /* Scan the outline of the ellipse into the memory buffer */
    _MGL_scanFatEllipse(&scanList,left,top,A,B,DC.a.penWidth,DC.a.penHeight);

    /* Render the outline of the ellipse */
    if (clip) {
        maxIndex = scanList.top + scanList.length;
        scanPtr = scanList.scans;
        for (i = scanList.top,j = i + B+DC.a.penHeight; i < maxIndex; i++,j--,scanPtr++) {
            if (scanPtr->leftR < scanPtr->rightL) {
                _MGL_clipScanLine(i,scanPtr->leftL,scanPtr->leftR);
                _MGL_clipScanLine(i,scanPtr->rightL,scanPtr->rightR);
                _MGL_clipScanLine(j,scanPtr->leftL,scanPtr->leftR);
                _MGL_clipScanLine(j,scanPtr->rightL,scanPtr->rightR);
                }
            else {
                _MGL_clipScanLine(i,scanPtr->leftL,scanPtr->rightR);
                _MGL_clipScanLine(j,scanPtr->leftL,scanPtr->rightR);
                }
            }
        if ((B+DC.a.penHeight+1) & 1) {
            if (scanPtr->leftR < scanPtr->rightL) {
                _MGL_clipScanLine(i,scanPtr->leftL,scanPtr->leftR);
                _MGL_clipScanLine(i,scanPtr->rightL,scanPtr->rightR);
                }
            else {
                _MGL_clipScanLine(i,scanPtr->leftL,scanPtr->rightR);
                }
            }
        }
    else {
        DC.r.cur.DrawFatEllipseList(scanList.top,scanList.length,
            B+DC.a.penHeight,(N_int16*)scanList.scans);
        }
}

/****************************************************************************
DESCRIPTION:
Draws an ellipse outline.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle for the ellipse

REMARKS:
Draws the outline of an ellipse given the bounding rectangle for the ellipse.
The ellipse outline is drawn in the current pen color, style and size just
inside the mathematical boundary of the bounding rectangle for the ellipse.

SEE ALSO:
MGL_ellipseCoord, MGL_fillEllipse, MGL_ellipseArc, MGL_fillEllipseArc
****************************************************************************/
void MGLAPI MGL_ellipse(
    rect_t extentRect)
{
    int     A,B;
    ibool   clipit;
    rect_t  r;
    region_t *clipped,*ellipse,*pen;

    /* Check for degenerate ellipses */
    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A == 0 || B == 0 || MGL_emptyRect(extentRect))
        return;

    /* Determine if the ellipse needs to be clipped. If it does, we ask
     * the ellipse routine to perform clipping, otherwise we tell it to
     * scan convert without clipping (a lot faster).
     */
    BEGIN_VISIBLE_CLIP_LIST(&DC);
    r = extentRect;
    r.right += DC.a.penWidth;
    r.bottom += DC.a.penHeight;
    if (MGL_sectRect(DC.clipRectView,r,&r))
        clipit = !MGL_equalRect(extentRect,r);
    else {
        /* Trivially reject ellipse */
        END_VISIBLE_CLIP_LIST(&DC);
        return;
        }

    if (DC.clipRegionScreen) {
        /* To simplify drawing and clipping of ellipses with complex
         * clip regions, we simply generate a region that represents
         * the ellipse and then clip that region against the complex
         * clip region.
         */
        MGL_offsetRect(extentRect,DC.viewPort.left,DC.viewPort.top);
        pen = MGL_rgnSolidRectCoord(0,0,DC.a.penWidth+1,DC.a.penHeight+1);
        ellipse = MGL_rgnEllipse(extentRect,pen);
        clipped = MGL_sectRegion(ellipse,DC.clipRegionScreen);
        if (!MGL_emptyRect(clipped->rect))
            __MGL_drawRegion(0,0,clipped);
        MGL_freeRegion(pen);
        MGL_freeRegion(ellipse);
        MGL_freeRegion(clipped);
        }
    else {
        /* Handle fast drawing when no complex clipping is required */
        if (DC.a.penStyle == MGL_BITMAP_SOLID && DC.a.penWidth == 0 && DC.a.penHeight == 0) {
            if (clipit) {
                DC.r.ClipEllipse(extentRect.left + DC.viewPort.left,
                    extentRect.top + DC.viewPort.top,A,B,
                    DC.clipRectScreen.left, DC.clipRectScreen.top,
                    DC.clipRectScreen.right, DC.clipRectScreen.bottom);
                }
            else {
                DC.r.DrawEllipse(extentRect.left + DC.viewPort.left,
                    extentRect.top + DC.viewPort.top,A,B);
                }
            }
        else {
            _MGL_fatPenEllipse(extentRect.left + DC.viewPort.left,
                    extentRect.top + DC.viewPort.top,A,B,clipit);
            }
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Draws an ellipse outline.

HEADER:
mgraph.h

PARAMETERS:
x       - x coordinate for the center of ellipse (syntax 1)
y       - y coordinate for the center of ellipse (syntax 1)
xradius - x radius for the ellipse (syntax 1)
yradius - y radius for the ellipse (syntax 1)

REMARKS:
Draws the outline of an ellipse given the center and radii for the ellipse.
The ellipse outline is drawn in the current pen color, style and size just
inside the mathematical boundary of the bounding rectangle for the ellipse.

Note that this routine can only work with integer semi-major and semi-minor
axes, but can sometimes be easier to work with (and is provided for compatibility
with other graphics libraries). The MGL_ellipse routine is more versatile than
this, as you can have an ellipse with odd diameter values, which you cannot
get with this routine.

SEE ALSO:
MGL_ellipse, MGL_fillEllipse, MGL_ellipseArc, MGL_fillEllipseArc
****************************************************************************/
void MGLAPI MGL_ellipseCoord(
    int x,
    int y,
    int xradius,
    int yradius)
{
    rect_t  r;

    r.left = x - xradius;
    r.right = x + xradius;
    r.top = y - yradius;
    r.bottom = y + yradius;
    MGL_ellipse(r);
}

/****************************************************************************
PARAMETERS:
left        - Left coorindate of ellipse rectangle
top         - Top coodinate of ellipse rectangle
A           - Major axis
B           - Minor axis
clip        - True if ellipse should be clipped

REMARKS:
Private function to fill an ellipse by rendering into a memory buffer and
then rendering each scanline in the ellipse. This is the normal routine
used to emulate this for all drivers that cannot do ellipse rendering in
hardware.
****************************************************************************/
static void _MGL_fillEllipse(
    int left,
    int top,
    int A,
    int B,
    int clip)
{
    int             i,j,maxIndex;
    scanlist        scanList;
    scanline        *scanPtr;

    /* Scan the outline of the ellipse into the memory buffer */
    _MGL_scanEllipse(&scanList,left,top,A,B);

    /* Render the outline of the ellipse */
    if (clip) {
        maxIndex = scanList.top + scanList.length-1;
        scanPtr = scanList.scans;
        for (i = scanList.top; i < maxIndex; i++,scanPtr++) {
            _MGL_clipScanLine(i,scanPtr->left,scanPtr->right);
            }
        if (!(B & 1))
            _MGL_clipScanLine(i,scanPtr->left,scanPtr->right);
        scanPtr = scanList.scans;
        for (i = scanList.top,j = i + B; i < maxIndex; i++,j--,scanPtr++) {
            _MGL_clipScanLine(j,scanPtr->left,scanPtr->right);
            }
        }
    else {
        DC.r.cur.DrawEllipseList(scanList.top,scanList.length,B,
            (N_int16*)scanList.scans);
        }
}

/****************************************************************************
DESCRIPTION:
Fills an ellipse.

HEADER:
mgraph.h

PARAMETERS:
ExtentRect  - Bounding rectangle defining the ellipse

REMARKS:
Fills an ellipse given either the center and radii for the ellipse, or the bounding
rectangle for the ellipse. The ellipse is filled in the current pen color and style just
inside the mathematical boundary of the bounding rectangle for the ellipse.

Note that while this routine can only work with integer semi-major and semi-minor
axes, it can sometimes be easier to work with (and is provided for compatibility
with other graphics packages). MGL_fillEllipseCoord is a more versatile routine,
as it allows ellipses with odd diameter values, which you cannot get with
MGL_fillEllipse.

SEE ALSO:
MGL_ellipse, MGL_ellipseArc, MGL_fillEllipseArc, MGL_fillEllipseCoord
****************************************************************************/
void MGLAPI MGL_fillEllipse(
    rect_t extentRect)
{
    rect_t  r;
    int     A,B,clipit;
    region_t *clipped,*ellipse;

    /* Check for degenerate ellipses */
    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A == 0 || B == 0 || MGL_emptyRect(extentRect))
        return;

    /* Determine if the ellipse needs to be clipped. If it does, we ask
     * the ellipse routine to perform clipping, otherwise we tell it to
     * scan convert without clipping (a lot faster).
     */
    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (MGL_sectRect(DC.clipRectView,extentRect,&r))
        clipit = !MGL_equalRect(extentRect,r);
    else {
        /* Trivially reject ellipse             */
        END_VISIBLE_CLIP_LIST(&DC);
        return;
        }

    if (DC.clipRegionScreen) {
        /* To simplify drawing and clipping of ellipses with complex
         * clip regions, we simply generate a region that represents
         * the ellipse and then clip that region against the complex
         * clip region.
         */
        MGL_offsetRect(extentRect,DC.viewPort.left,DC.viewPort.top);
        ellipse = MGL_rgnSolidEllipse(extentRect);
        clipped = MGL_sectRegion(ellipse,DC.clipRegionScreen);
        if (!MGL_emptyRect(clipped->rect))
            __MGL_drawRegion(0,0,clipped);
        MGL_freeRegion(clipped);
        MGL_freeRegion(ellipse);
        }
    else {
        /* Draw the ellipse quickly when no complex clipping is needed */
        _MGL_fillEllipse(extentRect.left + DC.viewPort.left,
            extentRect.top + DC.viewPort.top,A,B,clipit);
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/*******************************************************************************
DESCRIPTION:
Fills an ellipse.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate for center of ellipse (syntax 1)
y   - y coordinate for center of ellipse (syntax 1)
xradius - x radius for the ellipse (syntax 1)
yradius - y radius for the ellipse (syntax 1)
extentRect  - Bounding rectangle defining the ellipse (syntax 2)

REMARKS:
Fills an ellipse given either the center and radii for the ellipse, or the bounding
rectangle for the ellipse. The ellipse is filled in the current pen color and style just
inside the mathematical boundary of the bounding rectangle for the ellipse.

Note that the first routine can only work with integer semi-major and semi-minor
axes, but can sometimes be easier to work with (and is provided for compatibility
with other graphics packages). The second routine is more versatile than the first, as
you can have an ellipse with odd diameter values, which you cannot get with the
first routine.

SEE ALSO:
MGL_ellipse, MGL_ellipseArc, MGL_fillEllipseArc
*******************************************************************************/
void MGLAPI MGL_fillEllipseCoord(
    int x,
    int y,
    int xradius,
    int yradius)
{
    rect_t  r;

    r.left = x - xradius;
    r.right = x + xradius;
    r.top = y - yradius;
    r.bottom = y + yradius;
    MGL_fillEllipse(r);
}

/****************************************************************************
DESCRIPTION:

Generates the set of points on an ellipse.

HEADER:
mgraph.h

PARAMETERS:
extentRect  - Bounding rectangle defining the ellipse
setup   - Routine called to initialize pixel plotting routines
set4pixels  - Routine called repeatedly for each set of 4 pixels
finished    - Routine called to complete plotting pixels

REMARKS:
This routine generates the set of points on a ellipse, and is the same code used to
generate ellipses internally in MGL. You can call it to generate the set of points on
an ellipse, calling your own user defined plotting routines.

The setup routine is called before any pixels are plotted with the coordinates of the
4 seed points in the four ellipse quadrants.

The set4pixels routine is called repeatedly for each set of 4 pixels to be plotted, and
specified whether the coordinates in the x and y directions should be incremented or
remain the same. This state of the 4 pixel coordinates will need to be maintained by
the user supplied routines.

The finished routine is called to clean up after generating all the points on the
ellipse, such as releasing memory and rasterizing the ellipse if the rasterizing was
deferred.

SEE ALSO:
MGL_ellipseArcEngine, MGL_lineEngine
****************************************************************************/
void MGLAPI MGL_ellipseEngine(
    rect_t extentRect,
    void (MGLAPIP setup)(
        int topY,
        int botY,
        int left,
        int right),
    void (MGLAPIP set4pixels)(
        ibool inc_x,
        ibool inc_y,
        ibool region1),
    void (MGLAPIP finished)(void))
{
    int     A,B;

    A = extentRect.right - extentRect.left - 1; /* Major Axis       */
    B = extentRect.bottom - extentRect.top - 1; /* Minor Axis       */
    if (A == 0 || B == 0 || MGL_emptyRect(extentRect))
        return;             /* Ignore degenerate ellipses           */
    __MGL_ellipseEngine(extentRect.left,extentRect.top,A,B,
        setup,set4pixels,finished);
}

