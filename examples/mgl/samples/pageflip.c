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
* Description:  Page flipping sample program.
*
****************************************************************************/

#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

int     maxx,maxy,stepx,stepy,secAngle,minAngle;
rect_t  extent;

char demoName[] = "pageflip";

/*------------------------------ Implementation ---------------------------*/

void initAnimation(void)
{
    maxx = MGL_maxx();
    maxy = MGL_maxy();
    extent.left = extent.top = 0;
    extent.right = maxx/5;
    extent.bottom = ((long)extent.right * 1000) / MGL_getAspectRatio();
    stepx = 1;
    stepy = 1;
    secAngle = minAngle = 90;
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
    int             numpages,aPage,vPage;
    int             sizex,sizey,bits;
    rect_t          fullView,oldView;
    pixel_format_t  pf;
    palette_t       pal[256];
    MGLDC           *memdc;

    /* The first way to do simple page flipping is using two display
     * pages for 'double' buffering. The MGL has functions to set the
     * system into double buffered modes and flip pages for you, so
     * this method is very easy. We use MGL_doubleBuffer() to change
     * to double buffered mode and MGL_swapBuffers() to change display
     * pages.
     */
    if (!MGL_doubleBuffer(dc))
        MGL_fatalError("Double buffereing not available!");
    mainWindow(dc,"Page Flip Demo");
    statusLine("Method 1: Double buffering with two pages");
    MGL_swapBuffers(dc,true);

    /* Draw to the hidden page */
    mainWindow(dc,"Page Flip Demo");
    statusLine("Method 1: Double buffering with two pages");

    initAnimation();
    do {
        /* Clear the entire display device before drawing the next frame */
        MGL_clearViewport();

        /* Draw the clock at the current location and move it */
        drawClock();
        moveClock();

        /* Swap the display buffers */
        MGL_swapBuffers(dc,true);
        } while (!checkEvent());
    waitEvent();

    /* Return to single buffered mode */
    MGL_singleBuffer(dc);

    /* The second way to use page flipping is with multiple display
     * pages (3+) to do multi-buffering. Because we are using multiple
     * display pages, we can turn off the wait for retrace flag when we
     * swap visual pages. In order for triple buffering to work properly,
     * we need to be doing a complete re-paint of the screen each frame
     * without doing a screen clear (otherwise you will see flicker).
     * Games like Quake and Duke3D repaint the entire frame from top to
     * bottom without any overdraw, which allows them to use triple
     * buffering techniques like this. We simulate this here by rendering
     * to a system buffer and blitting to the screen constantly to achieve
     * a similar effect for this sample program.
     *
     * Note that VBE/Core 3.0 and VBE/AF 2.0 include full support for
     * hardware triple buffering that can be used to eliminate this problem.
     * When these standards are ratified, the MGL will be updated to support
     * this and you will be able to do 'real' triple buffering without
     * any flicker on the screen.
     */
    numpages = MGL_maxPage(dc)+1;
    if (numpages < 3)
        return;

    /* Get the display device size, color depth and pixel format so that we
     * can create a compatible memory device context. Note that we also
     * need to copy the palette from the display DC to the memory DC.
     */
    sizex = MGL_sizex(dc);
    sizey = MGL_sizey(dc);
    bits = MGL_getBitsPerPixel(dc);
    MGL_getPixelFormat(dc,&pf);
    if ((memdc = MGL_createMemoryDC(sizex+1,sizey+1,bits,&pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if (bits == 8) {
        MGL_getPalette(dc,pal,256,0);
        MGL_setPalette(memdc,pal,256,0);
        MGL_realizePalette(memdc,256,0,true);
        }
    MGL_makeCurrentDC(memdc);

    /* Make the fullViewport for the display DC the full screen, as the
     * above code has changed it to a smaller value.
     */
    fullView.left = 0;
    fullView.top = 0;
    fullView.right = sizex+1;
    fullView.bottom = sizey+1;
    MGL_setViewportDC(dc,fullView);

    /* Draw the main window display on the memory buffer */
    mainWindow(memdc,"Page Flip Demo");
    statusLine("Method 2: Multi-buffering with 3+ pages");
    initAnimation();
    MGL_setActivePage(dc,aPage = 1);
    MGL_setVisualPage(dc,vPage = 0,false);
    MGL_getViewport(&oldView);
    do {
        /* Clear the fullViewport before drawing the next frame */
        MGL_clearViewport();

        /* Draw the clock at the current location and move it */
        drawClock();
        moveClock();

        /* Copy the memory device to the display device */
        MGL_setViewport(fullView);
        MGL_bitBlt(dc,memdc,fullView,0,0,MGL_REPLACE_MODE);
        MGL_setViewport(oldView);

        /* Swap the hardware display buffers */
        aPage = (aPage+1) % numpages;
        vPage = (vPage+1) % numpages;
        MGL_setActivePage(dc,aPage);
        MGL_setVisualPage(dc,vPage,false);
        } while (!checkEvent());
    waitEvent();
}
