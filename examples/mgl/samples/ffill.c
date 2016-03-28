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
* Environment:  IBM PC (MS DOS)
*
* Description:  Flood fill region fill routines. This code shows how to
*               build upon the low level MegaGraph library routines to
*               build fast line adjacency flood fill routines. Two distinct
*               routines are implemented here - a boundary flood fill,
*               and an interior flood fill. Note that since the low
*               level scanline routines do not do any clipping or view
*               port mapping, we must do that within the routines.
*
*               To mimise the amount of stack space that is used, only the
*               variable that absolutely MUST be local are stored as local
*               or passed as parameters, while the rest are stored globally
*               to the recursive routines.
*
*               NOTE:   These routines use a lot of stack space since they
*                       are recursive. If you wish to use these in real
*                       programs, you should use a large stack of at least
*                       20k.
*
*
****************************************************************************/

#include "mgraph.h"
#include <dos.h>

/*------------------------- Implementation --------------------------------*/

#define UP      -1
#define DOWN    1

PRIVATE color_t     border;         /* Color of border pixels           */
PRIVATE color_t     fill;           /* Color of current fill color      */
PRIVATE color_t     interior;       /* Color of interior pixels         */
PRIVATE color_t     back;           /* Background color                 */
PRIVATE rect_t      viewPort;       /* The current viewport             */
PRIVATE rect_t      clipRect;       /* The current clipping rectangle   */
PRIVATE color_t     v;              /* Global variable                  */

PRIVATE int BorderAdjFill(int x,int y,int D,int prevXL,
    int prevXR)
/****************************************************************************
*
* Function:     BorderAdjFill
* Parameters:   x       - Seed x coordinate
*               y       - Seed y coordinate
*               D       - Direction to fill in (UP,DOWN)
*               prevXL  - Previous x left coordinate
*               prevXR  - Previous x right coordinate
* Returns:      Right coordinate filled up to.
*
* Description:  Recursively border fills an area starting at the seed
*               point (seedX,seedY) and travelling in the direction D.
*
*               A border fill is terminated when it comes across a border
*               pixel of the specified border color.
*
****************************************************************************/
{
    int     xl,xr;

    xl = xr = x;        /* Initialise to seed coordinates           */

    /* Scan left in video buffer from the current seed coordinate looking
     * for border pixels.
     */

    if (--xl >= clipRect.left) {
        xl = MGL_scanLeftForColor(xl+viewPort.left,y+viewPort.top,border);
        if ((xl -= viewPort.left) < clipRect.left)
            xl = clipRect.left-1;
        xl++;
        }
    else
        xl = clipRect.left;

    /* Scan right in video buffer from the current seed coordinate looking
     * for border pixels.
     */

    if (++xr < clipRect.right) {
        xr = MGL_scanRightForColor(xr+viewPort.left,y+viewPort.top,border);
        if ((xr -= viewPort.left) >= clipRect.right)
            xr = clipRect.right;
        xr--;
        }
    else
        xr = clipRect.right-1;

    /* Fill in the current scan line */

    MGL_scanLine(y+viewPort.top,xl+viewPort.left,xr+viewPort.left+1);

    /* Find and fill adjacent line segments in same direction */

    if (y+D >= clipRect.top && y+D < clipRect.bottom)
        for (x = xl; x <= xr; x++) {
            v = MGL_getPixelCoord(x,y+D);
            if (v != border && v != fill && v != back)
                x = BorderAdjFill(x,y+D,D,xl,xr);
            }

    /* Find and fill adjacent line segments in opposite direction */

    if (y-D >= clipRect.top && y-D < clipRect.bottom) {
        prevXL -= 2;
        for (x = xl; x < prevXL; x++) {
            v = MGL_getPixelCoord(x,y-D);
            if (v != border && v != fill && v != back)
                x = BorderAdjFill(x,y-D,-D,xl,xr);
            }

        for (x = prevXR+2; x < xr; x++) {
            v = MGL_getPixelCoord(x,y-D);
            if (v != border && v != fill && v != back)
                x = BorderAdjFill(x,y-D,-D,xl,xr);
            }
        }

    return xr;
}

PUBLIC void MGL_boundaryFill(int x,int y,color_t bdr)
/****************************************************************************
*
* Function:     MGL_boundaryFill
* Parameters:   x   - Seed x coordinate
*               y   - Seed y coordinate
*               bdr - Border color used for fill
*
* Description:  Does a boundary fill starting at the point (x,y) and
*               filling all the area bounded by pixels of the color bdr.
*
*               The area is filled in with the current drawing color and
*               the current pattern. Note however that you can only use
*               MGL_BITMAP_OPAQUE pattern's, or solid fills.
*
****************************************************************************/
{
    int penstyle = MGL_getPenStyle();
    int writemode = MGL_getWriteMode();

    if (penstyle == MGL_BITMAP_TRANSPARENT || writemode != MGL_REPLACE_MODE)
        return;
    back = -1;
    if (penstyle == MGL_BITMAP_OPAQUE)
        back = MGL_getBackColor();
    border = bdr;
    fill = MGL_getColor();

    /* Set up viewport and clipping rectangles */

    MGL_getViewport(&viewPort);
    MGL_getClipRect(&clipRect);

    if (MGL_ptInRectCoord(x,y,clipRect))
        BorderAdjFill(x,y,DOWN,x,x);
}

PRIVATE int InteriorAdjFill(int x,int y,int D,int prevXL,
    int prevXR)
/****************************************************************************
*
* Function:     InteriorAdjFill
* Parameters:   x       - Seed x coordinate
*               y       - Seed y coordinate
*               D       - Direction to fill in (UP,DOWN)
*               prevXL  - Previous x left coordinate
*               prevXR  - Previous x right coordinate
* Returns:      Right coordinate filled up to.
*
* Description:  Recursively fills an area starting at the seed point
*               (seedX,seedY) and travelling in the direction D.
*
*               An interior fill is terminated when it comes across a
*               pixel of different to that of the original seed pixel.
*
****************************************************************************/
{
    int     xl,xr;

    xl = xr = x;        /* Initialise to seed coordinates           */

    /* Scan left in video buffer from the current seed coordinate looking
     * for border pixels.
     */

    if (--xl >= clipRect.left) {
        xl = MGL_scanLeftWhileColor(xl+viewPort.left,y+viewPort.top,interior);
        if ((xl -= viewPort.left) < clipRect.left)
            xl = clipRect.left-1;
        xl++;
        }
    else
        xl = clipRect.left;

    /* Scan right in video buffer from the current seed coordinate looking
     * for border pixels.
     */

    if (++xr < clipRect.right) {
        xr = MGL_scanRightWhileColor(xr+viewPort.left,y+viewPort.top,interior);
        if ((xr -= viewPort.left) >= clipRect.right)
            xr = clipRect.right;
        xr--;
        }
    else
        xr = clipRect.right-1;

    /* Fill in the current scan line */

    MGL_scanLine(y+viewPort.top,xl+viewPort.left,xr+viewPort.left+1);

    /* Find and fill adjacent line segments in same direction */

    if (y+D >= clipRect.top && y+D < clipRect.bottom)
        for (x = xl; x <= xr; x++) {
            v = MGL_getPixelCoord(x,y+D);
            if (v == interior && v != fill && v != back)
                x = InteriorAdjFill(x,y+D,D,xl,xr);
            }

    /* Find and fill adjacent line segments in opposite direction */

    if (y-D >= clipRect.top && y-D < clipRect.bottom) {
        prevXL -= 2;
        for (x = xl; x < prevXL; x++) {
            v = MGL_getPixelCoord(x,y-D);
            if (v == interior && v != fill && v != back)
                x = InteriorAdjFill(x,y-D,-D,xl,xr);
            }

        for (x = prevXR+2; x < xr; x++) {
            v = MGL_getPixelCoord(x,y-D);
            if (v == interior && v != fill && v != back)
                x = InteriorAdjFill(x,y-D,-D,xl,xr);
            }
        }

    return xr;
}

PUBLIC void MGL_interiorFill(int x,int y)
/****************************************************************************
*
* Function:     MGL_interiorFill
* Parameters:   x   - Seed x coordinate
*               y   - Seed y coordinate
*
* Description:  Does a flood fill starting at the point (x,y) and
*               filling in all accessible areas of the same color as the
*               seed point.
*
*               The area is filled in with the current drawing color and
*               the current pattern. Note however that you can only use
*               MGL_BITMAP_OPAQUE pattern's, or solid fills in replace
*               mode.
*
****************************************************************************/
{
    int penstyle = MGL_getPenStyle();
    int writemode = MGL_getWriteMode();

    if (penstyle == MGL_BITMAP_TRANSPARENT || writemode != MGL_REPLACE_MODE)
        return;
    back = -1;
    if (penstyle == MGL_BITMAP_OPAQUE)
        back = MGL_getBackColor();
    if ((interior = MGL_getPixelCoord(x,y)) == -1UL)
        return;
    fill = MGL_getColor();

    /* Set up viewport and clipping rectangles */

    MGL_getViewport(&viewPort);
    MGL_getClipRect(&clipRect);
    InteriorAdjFill(x,y,DOWN,x,x);
}
