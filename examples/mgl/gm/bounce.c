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
* Description:  Simple game framework demonstration program.
*
****************************************************************************/

#include "gm/gm.h"
#include <string.h>
#include <stdlib.h>
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

#define CSIZE 10        /* Circle Size */

int         pixelx,pixely;
int         xres,yres;
int         incx,incy;
int         crunchx,crunchy;
rect_t      prevDirtyRect;
region_t    *prevDirty;
GMDC        *gm;

GM_driverOptions driverOpt = {
    true,           /* UseSNAP          */
    false,          /* UseHWOpenGL      */
    MGL_GL_AUTO,    /* OpenGLType       */
    GM_MODE_ALLBPP, /* modeFlags        */
    };

/*------------------------------ Implementation ---------------------------*/

/* Initialisation function that sets up the internal's of the bounce
 * demo after a mode set.
 */

void myInit(void)
{
    palette_t pal[256] = {{0,0,0},{0,0,0},{0,0,0},
                          {0,0,0},{0,0,0},{0,0,0},
                          {0,0,0},{0,0,0},{0,0,0},
                          {0,0,0},{0,0,0},{0,0,0},
                          {255,255,255},{255,0,0}};

    xres = MGL_sizex(gm->dc)+1-CSIZE;
    yres = MGL_sizey(gm->dc)+1-CSIZE;
    pixelx = xres/2;
    pixely = yres/2;
    incx = 1;
    incy = -1;
    crunchx = CSIZE;
    crunchy = CSIZE;
    pal[255].red = 255;
    pal[255].green = 255;
    pal[255].blue = 255;
    GM_setPalette(pal,256,0);
    GM_realizePalette(256,0,true);
    prevDirtyRect.left = 0;
    prevDirtyRect.top = 0;
    prevDirtyRect.right = MGL_sizex(gm->dc)+1;
    prevDirtyRect.bottom = MGL_sizey(gm->dc)+1;
    prevDirty = MGL_rgnSolidRect(prevDirtyRect);
}

/* Main draw callback handler that draws the current frame to the device
 * context and then swaps the display buffers. When the game is
 * minimized on the task bar in fullscreen modes this function will not
 * be called to avoid drawing to memory that we do not own.
 */

void draw(void)
{
    rect_t      dirtyRect;
    region_t    *cntDirty,*fullDirty;

    /* Draw the ball at the current location */
    MGL_clearDevice();
    MGL_setColorCI(13);
    MGL_fillEllipseCoord(pixelx,pixely,CSIZE+crunchx,CSIZE+crunchy);
    MGL_setColorCI(12);
    MGL_ellipseCoord(pixelx,pixely,CSIZE+crunchx,CSIZE+crunchy);
    dirtyRect.left = pixelx - (CSIZE+crunchx);
    dirtyRect.right = pixelx + CSIZE+crunchx;
    dirtyRect.top = pixely - (CSIZE+crunchy);
    dirtyRect.bottom = pixely + CSIZE+crunchy;

    /* Swap display buffers with dirty rectangles */
    cntDirty = MGL_rgnSolidRect(dirtyRect);
    MGL_unionRegionRect(cntDirty,&prevDirtyRect);
    if (gm->dispdc->mi.maxPage > 0) {
        /* Note that if we have more than 1 display page and we are doing
         * double buffering, we have to union in the dirty region from the
         * previous buffer as the dirty regions from that frame are also
         * dirty in this buffer (as they two are distinct memory buffers in
         * video memory). This works for double buffering, and if you wish
         * to use more than 2 buffers you will need to use a chain of dirty
         * regions to find all the dirty portions to update for the current
         * buffer (ie: for triple buffering the region of the last 2 frames
         * instead of the last 1).
         */
        fullDirty = MGL_copyRegion(cntDirty);
        MGL_unionRegion(fullDirty,prevDirty);
        MGL_optimizeRegion(fullDirty);
        GM_swapDirtyBuffers(fullDirty,MGL_waitVRT);
        MGL_freeRegion(fullDirty);
        MGL_freeRegion(prevDirty);
        prevDirty = cntDirty;
        prevDirtyRect = dirtyRect;
        }
    else {
        MGL_optimizeRegion(cntDirty);
        GM_swapDirtyBuffers(cntDirty,MGL_waitVRT);
        MGL_freeRegion(cntDirty);
        prevDirtyRect = dirtyRect;
        }
}

/* Main game logic callback handler that moves the bouncing ball around
 * on the screen. This function is called once per main loop by the
 * Game Framework, and you should do all non-drawing related game logic
 * type stuff in here. This is where you would do all your sound processing,
 * network processing etc before drawing the next frame.
 */

void gameLogic(void)
{
    /* Move the ball to new location */
    if (pixelx >= xres-CSIZE)
        crunchx = xres-pixelx-1;
    if (pixely >= yres-CSIZE)
        crunchy = yres-pixely-1;
    if (pixelx <= CSIZE*2)
        crunchx = pixelx-CSIZE-1;
    if (pixely <= CSIZE*2)
        crunchy = pixely-CSIZE-1;
    pixelx += incx;
    pixely += incy;
    if ((pixelx >= xres) || (pixelx <= CSIZE))
        incx = -incx;
    if ((pixely >= yres) || (pixely <= CSIZE))
        incy = -incy;
}

/* Main key down event handler to allow you to process key down events. The
 * Game Framework allows you to register key repeat and key up event handlers
 * as well (you can also use the same handler if you wish).
 */

void keydown(event_t *evt)
{
    switch (EVT_asciiCode(evt->message)) {
        case 0x1B:
            GM_exit();
            break;
        }
}

/* Main mouse down event handler to allow you to process mouse down events. The
 * Game Framework allows you to register mouse up and mouse movement event
 * handlers as well (you can also use the same handler if you wish).
 */

void mousedown(event_t *evt)
{
    /* Simply re-start the animation when we get a mouse down */
    myInit();
}

/* Main callback to switch between fullscreen and windowed graphics modes. This
 * function is called by the game framework when the user requests to switch to
 * fullscreen modes either by typing 'Alt-Enter' or by clicking on the maximum
 * button when in windowed modes.
 */

void switchModes(GM_modeInfo *mode,ibool windowed)
{
    myInit();
}

/* This function gets called when our application is activated and
 * de-activated. If you have other components in your game that need
 * to be disabled while your are not active (such as sound or CD-ROM
 * Audio) you should do it here.
 */

void activate(ibool active)
{
}

/* This function gets called when the user switches away from our app
 * back to the desktop via an Alt-Tab or some other activation key. This
 * function can be used to suspend stuff like CD-ROM Audio while suspended.
 * You can also return MGL_NO_DEACTIVATE from this function and the MGL
 * will ignore the request to switch back to the desktop (but make sure this
 * is an option that is off by default!).
 */

int _ASMAPI suspendApp(MGLDC *dc,int flags)
{
    if (flags == MGL_DEACTIVATE) {
        /* We are switching back to GDI mode, so put code in here to disable
         * stuff when switched away from fullscreen mode. Note that this
         * function may get once to determine if the switch should occur,
         * and again when the switch actually happens.
         */
        return MGL_SUSPEND_APP;
        }
    else if (flags == MGL_REACTIVATE) {
        /* We are now back in fullscreen mode, so put code in here to renable
         * stuff for fullscreen modes.
         */
        }
    return MGL_NO_SUSPEND_APP;
}

int main(int argc, char *argv[])
{
    int         startWindowed = true;
    GM_modeInfo info = {640,480,8};

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    if (argc > 1) {
        if (stricmp(argv[1],"-nosnap") == 0)
            driverOpt.useSNAP = false;
        }

    GM_setDriverOptions(&driverOpt);
    if ((gm = GM_init("Bounce")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    GM_setDrawFunc(draw);
    GM_setGameLogicFunc(gameLogic);
    GM_setKeyDownFunc(keydown);
    GM_setMouseDownFunc(mousedown);
    GM_setModeSwitchFunc(switchModes);
    GM_setAppActivate(activate);
    GM_setSuspendAppCallback(suspendApp);

    /* Continue choosing modes until the user is done */
    while (GM_chooseMode(&info,&startWindowed)) {
        if (!GM_setMode(&info,startWindowed,2,false))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        myInit();
        GM_mainLoop();
        }
    return 0;
}

