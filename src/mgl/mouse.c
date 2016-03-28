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
* Description:  Mouse support routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Global Variables ------------------------------*/

static int      mx,my,screenmx,screenmy,oldmx,oldmy;
static int      visible = -1,oldVisible;
static MGLDC    *DispDC = NULL;
static cursor_t cntCursor;
static color_t  cntCursorColor;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Initialises the mouse module.
{secret}
****************************************************************************/
void _MS_init(void)
{
    visible = -1;
}

/****************************************************************************
PARAMETERS:
dc  - Display device context to display the mouse on

REMARKS:
Saves the current display device context, and allocates the memory we
require for saving the image behind the mouse cursor. We also save the
internal color representation to draw the cursor in white.
{secret}
****************************************************************************/
void _MS_setDisplayDC(
    MGLDC *dc)
{
    if ((DispDC = dc) != NULL) {
        visible = -1;
        EVT_setMouseRange(dc->mi.xRes,dc->mi.yRes);
        MS_setCursorColor(MGL_realColor(dc,MGL_WHITE));
        }
}

/****************************************************************************
PARAMETERS:
x,y - New location for mouse cursor

REMARKS:
Routine called to move the mouse cursor to the new location. This is called
repeatedly by the event library to move the mouse cursor. This routine is
written to be able to be called from within an interrupt handler. The old
MGL used to do it this way, but the new MGL handler only moves the mouse
when the user application is processing events.
{secret}
****************************************************************************/
void EVTAPI _MS_moveCursor(
    int x,
    int y)
{
    int screenx,screeny;

    if (DispDC) {
        screenx = (mx = x) + DispDC->startX;
        screeny = (my = y) + DispDC->startY;
        if (screenx != screenmx || screeny != screenmy) {
            screenmx = screenx;
            screenmy = screeny;
            if (visible == 0)
                DispDC->v->d.setCursorPos(DispDC,mx,my);
            }
        }
}

/****************************************************************************
REMARKS:
Save the mouse cursor state internally to be restored later. Note that this
may well get called *after* the screen has been switched away from, so we
can't touch the framebuffer in this function.
{secret}
****************************************************************************/
void _MS_saveState(void)
{
    visible--;
    oldVisible = visible;
    oldmx = mx;
    oldmy = my;
    if (DispDC)
        DispDC->v->d.showCursor(DispDC,-1);
}

/****************************************************************************
REMARKS:
Restore the mouse cursor state that was saved previously. We first forcibly
hide the cursor, re-download the cursor image and then show it again.
{secret}
****************************************************************************/
void _MS_restoreState(void)
{
    visible = oldVisible;
    if (DispDC) {
        DispDC->v->d.setCursor(DispDC,&cntCursor);
        DispDC->v->d.setCursorColor(DispDC,cntCursorColor,0);
        }
    EVT_setMousePos(oldmx,oldmy);
    MS_show();
}

/*------------------------ Public interface routines ----------------------*/

/****************************************************************************
DESCRIPTION:
Displays the mouse cursor.

HEADER:
mgraph.h

REMARKS:
Increments the internal mouse cursor display counter, and displays the cursor
when the counter gets to zero. Calls to MS_hide decrement the counter, and
this call effectively cancels a single MS_hide call, allowing the MS_show and
MS_hide calls to be nested.

If the mouse was obscured with the MS_obscure function, this reverses the
effect and will redisplay the mouse cursor again. On systems with a hardware
mouse cursor, the MS_obscure function effectively does nothing, while on
systems using a software mouse cursor, the MS_obscure function simply calls
MS_hide.

Note that the mouse cursor display counter is reset to -1 by default when an
MGL fullscreen mode is started, so a single MS_show will display the mouse
cursor after the mode has been started.

SEE ALSO:
MS_hide, MS_obscure
****************************************************************************/
void MGLAPI MS_show(void)
{
    if (visible < 0) {
        visible++;
        if (DispDC && visible == 0) {
            // Show the mouse cursor
            DispDC->v->d.setCursorPos(DispDC,mx,my);
            DispDC->v->d.showCursor(DispDC,true);
            }
        }
}

/****************************************************************************
DESCRIPTION:
Hides the mouse cursor.

HEADER:
mgraph.h

REMARKS:
Decrements the internal mouse cursor display counter, and hides the cursor
if the counter was previously set to zero. Calls to MS_show increment the
counter, allowing the MS_show and MS_hide calls to be nested.

SEE ALSO:
MS_show, MS_obscure
***************************************************************************/
void MGLAPI MS_hide(void)
{
    visible--;
    if (DispDC && visible == -1)
        DispDC->v->d.showCursor(DispDC,false);
}

/****************************************************************************
DESCRIPTION:
Hides the mouse cursor from view during graphics output.

HEADER:
mgraph.h

REMARKS:
Hides the mouse cursor from view in order to perform graphics output using
MGL. If the graphics device driver supports a hardware cursor, this is
handled by the hardware, otherwise it is removed from the display. You should
call this routine rather than MS_hide in order to temporarily hide the mouse
cursor during graphics output as the MS_hide routine will always hide the
cursor, regardless of whether the system has a hardware mouse cursor or not.

SEE ALSO:
MS_show, MS_hide
****************************************************************************/
void MGLAPI MS_obscure(void)
{
    if (DispDC && (visible < 0 || !DispDC->v->d.hardwareCursor))
        MS_hide();
}

/****************************************************************************
DESCRIPTION:
Sets the mouse cursor shape.

HEADER:
mgraph.h

PARAMETERS:
curs    - Pointer to new mouse cursor shape

REMARKS:
Sets the graphics mouse cursor shape, passed in the cursor_t structure. The
cursor_t structure contains a mouse cursor AND mask and a mouse cursor XOR
mask that is used to display the cursor on the screen, along with the mouse
cursor hotspot location. Refer to the cursor_t structure definition for more
information.
****************************************************************************/
void MGLAPI MS_setCursor(
    cursor_t *curs)
{
    CHECK(curs);
    switch (curs->m.bitsPerPixel) {
        case 1:
            memcpy(&cntCursor,curs,sizeof(curs->m));
            break;
        case 4:
            memcpy(&cntCursor,curs,sizeof(curs->c16));
            break;
        case 8:
            memcpy(&cntCursor,curs,sizeof(curs->c256));
            break;
        case 24:
            memcpy(&cntCursor,curs,sizeof(curs->cRGB));
            break;
        case 32:
            memcpy(&cntCursor,curs,sizeof(curs->cRGBA));
            break;
        }
    if (DispDC)
        DispDC->v->d.setCursor(DispDC,curs);
}

/****************************************************************************
DESCRIPTION:
Sets the current mouse cursor color.

HEADER:
mgraph.h

PARAMETERS:
color   - New mouse cursor color, in current display mode format.

REMARKS:
Sets the color for the mouse cursor to the specified color, which is passed
in as a packed MGL color in the proper format for the current display mode
(either a color index or a packed RGB color value). By default the mouse
cursor is set to white, which is a color index of 15 by default in MGL. If
you re-program the color palette in 4 or 8 bit modes, you will need to reset
the mouse cursor value to the value that represents white.
****************************************************************************/
void MGLAPI MS_setCursorColor(
    color_t color)
{
    cntCursorColor = color;
    if (DispDC)
        DispDC->v->d.setCursorColor(DispDC,color,0);
}

/****************************************************************************
DESCRIPTION:
Sets the current mouse cursor color.

HEADER:
mgraph.h

PARAMETERS:
foreColor   - Mouse foreground cursor color, in current display mode format.
backColor   - Mouse background cursor color, in current display mode format.

REMARKS:
Sets the colors for the mouse cursor to the specified color, which are passed
in as a packed MGL colors in the proper format for the current display mode
(either a color index or a packed RGB color value). By default the mouse
cursor is set to white on black, which is a color index of 15 by default in
MGL. If you re-program the color palette in 4 or 8 bit modes, you will need
to reset the mouse cursor value to the value that represents white.
****************************************************************************/
void MGLAPI MS_setCursorColorExt(
    color_t foreColor,
    color_t backColor)
{
    if (DispDC)
        DispDC->v->d.setCursorColor(DispDC,foreColor,backColor);
}

/****************************************************************************
DESCRIPTION:
Moves the mouse cursor to a new location.

HEADER:
mgraph.h

PARAMETERS:
x   - New mouse x coordinate (screen coordinates)
y   - New mouse y coordinate (screen coordinates)

REMARKS:
Moves the mouse cursor to the specified location in screen coordinates.

Note that it is not usually a good idea to move the mouse cursor around while
the user is interacting with the application, but this can be used to
restore the mouse cursor to a known location if it has been hidden for a
long period of time.

SEE ALSO:
MS_getPos
****************************************************************************/
void MGLAPI MS_moveTo(
    int x,
    int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (DispDC) {
        int width = DispDC->size.right - DispDC->size.left;
        int height = DispDC->size.bottom - DispDC->size.top;
        if (x >= width)
            x = width-1;
        if (y >= height)
            x = height-1;
        }
    EVT_setMousePos(x,y);
}

/****************************************************************************
DESCRIPTION:
Returns the current mouse cursor location.

HEADER:
mgraph.h

PARAMETERS:
x   - Place to store value for mouse x coordinate (screen coordinates)
y   - Place to store value for mouse y coordinate (screen coordinates)

REMARKS:
Obtains the current mouse cursor position in screen coordinates. Normally
the mouse cursor location is tracked using the mouse movement events that
are posted to the event queue when the mouse moves, however this routine
provides an alternative method of polling the mouse cursor location.

SEE ALSO:
MS_moveTo
****************************************************************************/
void MGLAPI MS_getPos(
    int *x,
    int *y)
{
    EVT_getMousePos(x,y);
}
