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
* Description:  Rectangle drawing routines
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#undef  MGL_rect
#undef  MGL_rectPt
#undef  MGL_fillRect
#undef  MGL_fillRectPt

/****************************************************************************
DESCRIPTION:
Draws a rectangle outline.

HEADER:
mgraph.h

PARAMETERS:
left        - Left coordinate of the rectangle
top         - Top coordinate of the rectangle
right       - Right coordinate of the rectangle
bottom      - Bottom coordinate of the rectangle
r           - Rectangle to draw
leftTop     - Point for upper left corner of rectangle
rightBottom - Point containing lower right corner of rectangle

REMARKS:
This function draws a rectangle outline in the current drawing attributes
at the specified location.

SEE ALSO:
MGL_rect, MGL_rectPt
****************************************************************************/
void MGLAPI MGL_rectCoord(
    int left,
    int top,
    int right,
    int bottom)
{
    int width = right - left;
    int height = bottom - top;
    int h = DC.a.penHeight+1;

    if (bottom <= top || right <= left)
        return;
    MGL_lineCoord(left,top,right-1,top);
    if (height > 2) {
        MGL_lineCoord(left,bottom-1,right-1,bottom-1);
        if (width > 2)
            MGL_lineCoord(left,top+h,left,bottom-h-1);
        }
    if (height > 1)
        MGL_lineCoord(right-1,top+h,right-1,bottom-h-1);
}

/****************************************************************************
DESCRIPTION:
Draws a rectangle outline.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to draw

REMARKS:
This function is the same as MGL_rectCoord, however it takes an entire
rectangle as the parameter instead of coordinates.

SEE ALSO:
MGL_rectCoord, MGL_rectPt
****************************************************************************/
void MGL_rect(
    rect_t r);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a rectangle outline.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to draw

REMARKS:
This function is the same as MGL_rectCoord, however it takes the top left
and bottom right coordinates of the rectangle as two points instead of
four coordinates.

SEE ALSO:
MGL_rectCoord, MGL_rect
****************************************************************************/
void MGL_rectPt(
    point_t leftTop,
    point_t rightBottom);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Clears the currently active display page.

HEADER:
mgraph.h

REMARKS:
This function will clear the entire currently active display page in the
current background color. This is the fastest way to clear an entire display
page, but if you wish to only clear a portion of the page, use the
MGL_clearViewport routine instead.

SEE ALSO:
MGL_clearViewport
****************************************************************************/
void MGLAPI MGL_clearDevice(void)
{
    /* Save the current pen attributes */
    int         svColor = DC.a.color;
    color_t     svMode = DC.a.writeMode;
    rect_t      oldView,view;
    region_t    *oldClipRegion;

    /* Set the current pen attributes to those required for the
     * background color.
     */
    DC.r.SetForeColor(DC.a.backColor);
    DC.r.SetMix(MGL_REPLACE_MODE);
    if (DC.deviceType == MGL_WINDOWED_DEVICE) {
        /* If we have any active visible regions we need to go via
         * the regular rectangle functions so that clipping will be accounted
         * for.
         */
        oldClipRegion = MGL_newRegion();
        MGL_getClipRegion(oldClipRegion);
        MGL_getViewport(&oldView);
        view.left = view.top = 0;
        view.right = DC.size.right-DC.size.left;
        view.bottom = DC.size.bottom-DC.size.top;
        MGL_setViewport(view);
        MGL_fillRectCoord(0,0,view.right-view.left,view.bottom-view.top);
        MGL_setViewport(oldView);
        MGL_setClipRegion(oldClipRegion);
        MGL_freeRegion(oldClipRegion);
        }
    else
        DC.r.cur.DrawRect(0,0,DC.size.right-DC.size.left,DC.size.bottom-DC.size.top);

    /* Restore the pen attributes */
    DC.r.SetForeColor(svColor);
    DC.r.SetMix(svMode);
}

/****************************************************************************
DESCRIPTION:
Clears the currently active viewport.

HEADER:
mgraph.h

REMARKS:
This function will clear the currently active display page viewport to the current
background color. This is the fastest way to clear a rectangular viewport, but you
may also wish to use the MGL_fillRect routine to fill with
an arbitrary pattern instead, as this function always clears the viewport to the solid
background color.

SEE ALSO:
MGL_clearDevice, MGL_fillRect
****************************************************************************/
void MGLAPI MGL_clearViewport(void)
{
    /* Save the current pen attributes */
    int     svColor = DC.a.color;
    color_t svMode = DC.a.writeMode;
    rect_t      view;
    region_t    *oldClipRegion;

    /* Set the current pen attributes to those required for the
     * background color.
     */
    DC.r.SetForeColor(DC.a.backColor);
    DC.r.SetMix(MGL_REPLACE_MODE);
    if (DC.deviceType == MGL_WINDOWED_DEVICE) {
        /* If we have any active visible regions we need to go via
         * the regular rectangle functions so that clipping will be accounted
         * for.
         */
        oldClipRegion = MGL_newRegion();
        MGL_getClipRegion(oldClipRegion);
        MGL_getViewport(&view);
        MGL_setViewport(view);
        MGL_fillRectCoord(0,0,view.right-view.left,view.bottom-view.top);
        MGL_setViewport(view);
        MGL_setClipRegion(oldClipRegion);
        MGL_freeRegion(oldClipRegion);
        }
    else {
        DC.r.cur.DrawRect(DC.viewPort.left - DC.size.left,DC.viewPort.top - DC.size.top,
            DC.viewPort.right - DC.viewPort.left,DC.viewPort.bottom - DC.viewPort.top);
        }

    /* Restore the pen attributes */
    DC.r.SetForeColor(svColor);
    DC.r.SetMix(svMode);
}

/****************************************************************************
DESCRIPTION:
Draws a filled rectangle.

HEADER:
mgraph.h

PARAMETERS:
left    - Left coordinate of rectangle
top     - Top coordinate of rectangle
right   - Right coordinate of rectangle
bottom  - Bottom coordinate of rectangle

REMARKS:
Fills a rectangle in the current drawing attributes. The mathematical
definition of a rectangle does not include the right and bottom edges, so
effectively the right and bottom edges are not rasterized (solving problems
with shared edges).

SEE ALSO:
MGL_fillRect, MGL_fillRectPt, MGL_rect
****************************************************************************/
void MGLAPI MGL_fillRectCoord(
    int left,
    int top,
    int right,
    int bottom)
{
    rect_t  d,r,clip;

    BEGIN_VISIBLE_CLIP_LIST(&DC);
    if (DC.clipRegionScreen) {
        d.left = left + DC.viewPort.left;     d.top = top + DC.viewPort.top;
        d.right = right + DC.viewPort.left;   d.bottom = bottom + DC.viewPort.top;
        BEGIN_CLIP_REGION(clip,DC.clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                DC.r.cur.DrawRect(r.left,r.top,r.right-r.left,r.bottom-r.top);
                }
        END_CLIP_REGION();
        }
    else {
        d.left = left;      d.top = top;
        d.right = right;    d.bottom = bottom;
        if (MGL_sectRect(DC.clipRectView,d,&d)) {
            MGL_offsetRect(d,DC.viewPort.left,DC.viewPort.top);
            DC.r.cur.DrawRect(d.left,d.top,d.right-d.left,d.bottom-d.top);
            }
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Draws a filled rectangle.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to be filled

REMARKS:
This function is the same as MGL_fillRectCoord, however it takes an entire
rectangle as the parameter instead of coordinates.

SEE ALSO:
MGL_fillRectCoord, MGL_fillRectPt, MGL_rect
****************************************************************************/
void MGL_fillRect(
    rect_t r);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a filled rectangle.

HEADER:
mgraph.h

PARAMETERS:
leftTop     - Top left coordinate of rectangle
rightBottom - Bottom right coordinate of rectangle

REMARKS:
This function is the same as MGL_fillRectCoord, however it takes the top left
and bottom right coordinates of the rectangle as two points instead of
four coordinates.

SEE ALSO:
MGL_fillRect, MGL_fillRectPt, MGL_rect
****************************************************************************/
void MGL_fillRectPt(
    point_t leftTop,
    point_t rightBottom);
/* Implemented as a macro */

