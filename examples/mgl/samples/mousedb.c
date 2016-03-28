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
* Description:  Double buffered mouse cursor sample program.
*
****************************************************************************/

#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

int     maxx,maxy,stepx,stepy,secAngle,minAngle;
rect_t  extent,dirtyRect;

char demoName[] = "mousedb";

/*------------------------------ Implementation ---------------------------*/

void initAnimation(void)
{
    maxx = MGL_maxx();
    maxy = MGL_maxy();
    extent.left = extent.top = 0;
    extent.right = maxx/5;
    extent.bottom = ((long)extent.right * 1000) / MGL_getAspectRatio();
    dirtyRect = extent;
    stepx = 1;
    stepy = 1;
    secAngle = minAngle = 90;
}

void clearDirtyRect(void)
{
    MGL_setColorCI(MGL_BLACK);
    MGL_fillRect(dirtyRect);
}

void drawClock(void)
{
    MGL_setColorCI(MGL_RED);
    MGL_fillEllipse(extent);
    MGL_setColorCI(MGL_WHITE);
    MGL_fillEllipseArc(extent,secAngle-5,secAngle);
    MGL_fillEllipseArc(extent,minAngle-5,minAngle);
}

int fixAngle(int angle)
{
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
    return angle;
}

void moveClock(void)
{
    /* Bounce the clock off the walls */
    dirtyRect = extent;
    MGL_insetRect(dirtyRect,-ABS(stepx),-ABS(stepy));
    if (extent.left + stepx < 0)
        stepx = -stepx;
    if (extent.right + stepx > maxx)
        stepx = -stepx;
    if (extent.top + stepy < 0)
        stepy = -stepy;
    if (extent.bottom + stepy > maxy)
        stepy = -stepy;
    MGL_offsetRect(extent,stepx,stepy);

    /* Update the hand movement */
    secAngle = fixAngle(secAngle - 5);
    if (secAngle == 90)
        minAngle = fixAngle(minAngle - 5);
}

void demo(MGLDC *dc)
{
    MGL_doubleBuffer(dc);
    mainWindow(dc,"Double buffered mouse cursor");
    statusLine("SciTech MGL completely takes care of the mouse cursor!");
    MGL_swapBuffers(dc,false);
    mainWindow(dc,"Double buffered mouse cursor");
    statusLine("SciTech MGL completely takes care of the mouse cursor!");
    MS_show();

    initAnimation();
    do {
        /* Draw the clock at the current location and move it */
        clearDirtyRect();
        drawClock();
        moveClock();

        /* Flip buffers */
        MGL_swapBuffers(dc,true);
        } while (!checkEvent());
    waitEvent();
}
