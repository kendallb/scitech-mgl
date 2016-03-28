/****************************************************************************
*
*                        Hello SciTech SNAP Graphics!
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Simple test program to test the VESA DPVL modes.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/gasdk.h"
#include "event.h"

/*---------------------------- Global Variables ---------------------------*/

#ifdef __WINDOWS__
#define SNAP_SHARED     true
#else
#define SNAP_SHARED     false
#endif

GA_initFuncs    init;
GA_driverFuncs  driver;
GA_DPVLFuncs    DPVL;
GA_cursorFuncs  curs;
PM_HWND         hwndConsole;
void            *stateBuf;
int             oldMode = -1;
int             xRes = 1280,yRes = 1024,bpp = 8;

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

/* cursors.c */
extern GA_monoCursor    arrowCursor;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    if (oldMode != -1) {
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        }
    EVT_exit();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    if (dc) {
        GA_unloadDriver(dc);
        dc = NULL;
        }
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to check if a key has
been hit. We check for keydown and keyrepeat events, and we also flush the
event queue of all non keydown events to avoid it filling up.
****************************************************************************/
static int EVT_kbhit(void)
{
    int     hit;
    event_t evt;

    hit = EVT_peekNext(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    EVT_flush(~(EVT_KEYDOWN | EVT_KEYREPEAT));
    return hit;
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to read an ASCII keypress
and return it. This function will block until a key is hit.
****************************************************************************/
static int EVT_getch(void)
{
    event_t evt;

    EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    return EVT_asciiCode(evt.message);
}

/****************************************************************************
PARAMETERS:
x   - X coordinate of the mouse cursor position (screen coordinates)
y   - Y coordinate of the mouse cursor position (screen coordinates)

REMARKS:
This gets called periodically to move the mouse. It will get called when
the mouse may not have actually moved, so check if it has before redrawing
it.
****************************************************************************/
void EVTAPI moveMouse(
    int x,
    int y)
{
}

/****************************************************************************
REMARKS:
Draw a simple moire pattern of lines on the display
****************************************************************************/
void DrawMoire(void)
{
    int     i,value;

    SetForeColor(0);
    draw2d.DrawRect(0,0,maxX,maxY);
    if (maxcolor >= 0x7FFFL) {
        for (i = 0; i < maxX; i++) {
            SetForeColor(rgbColor((uchar)((i*255L)/maxX),0,0));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,0,rgbColor((uchar)((i*255L)/maxX),0,0));
            SetForeColor(rgbColor(0,(uchar)((i*255L)/maxX),0));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,maxY,rgbColor(0,(uchar)((i*255L)/maxX),0));
            }
        for (i = 0; i < maxY; i++) {
            value = (int)((i*255L)/maxY);
            SetForeColor(rgbColor((uchar)value,0,(uchar)(255 - value)));
            draw2d.DrawLineInt(maxX/2,maxY/2,0,i,rgbColor((uchar)value,0,(uchar)(255 - value)));
            SetForeColor(rgbColor(0,(uchar)(255 - value),(uchar)value));
            draw2d.DrawLineInt(maxX/2,maxY/2,maxX,i,rgbColor(0,(uchar)(255 - value),(uchar)value));
            }
        }
    else {
        for (i = 0; i < maxX; i += 5) {
            SetForeColor(i % maxcolor);
            draw2d.DrawLineInt(maxX/2,maxY/2,i,0,true);
            SetForeColor((i+1) % maxcolor);
            draw2d.DrawLineInt(maxX/2,maxY/2,i,maxY,true);
            }
        for (i = 0; i < maxY; i += 5) {
            SetForeColor((i+2) % maxcolor);
            draw2d.DrawLineInt(maxX/2,maxY/2,0,i,true);
            SetForeColor((i+3) % maxcolor);
            draw2d.DrawLineInt(maxX/2,maxY/2,maxX,i,true);
            }
        }
    SetForeColor(defcolor);
    draw2d.DrawLineInt(0,0,maxX,0,true);
    draw2d.DrawLineInt(0,0,0,maxY,true);
    draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
    draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
}

/****************************************************************************
REMARKS:
Display a list of available resolutions which are DPVL compatible
****************************************************************************/
void AvailableModes(void)
{
    N_uint16    *modes;
    ibool       found = false;

    printf("Available DPVL graphics modes:\n");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (!(modeInfo.AttributesExt & gaIsDPVLMode))
            continue;
        printf("    %4d x %4d %d bits per pixel\n",
            modeInfo.XResolution, modeInfo.YResolution, modeInfo.BitsPerPixel);
        found = true;
        }
    if (!found)
        printf("    No DPVL modes found -- configure DPVL via gaoption utility.\n");
    printf("\nUsage: dpvltest <xres> <yres> <bits>\n");
}

/****************************************************************************
REMARKS:
Initialise the graphics mode with the specified resolution and
color depth.
****************************************************************************/
int InitGraphics(
    int x,
    int y,
    int bits)
{
    N_uint16    *modes;

    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (!(modeInfo.AttributesExt & gaIsDPVLMode))
            continue;
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits) {
            cntMode = *modes;
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                cntMode |= gaLinearBuffer;
            oldMode = init.GetVideoMode();
            virtualX = virtualY = bytesPerLine = -1;
            if (init.SetVideoMode(cntMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0)
                return -1;
            if (!InitSoftwareRasterizer(0,1,false))
                PM_fatalError("Unable to initialise software rasteriser!");

            /* Get the VESA DPVL functions (after Ref2d is loaded!) */
            DPVL.dwSize = sizeof(DPVL);
            if (!REF2D_queryFunctions(ref2d,GA_GET_DPVLFUNCS,&DPVL))
                PM_fatalError("Unable to get DPVL functions!");
            return oldMode;
            }
        }
    return -1;
}

/****************************************************************************
REMARKS:
Initialize cursor for the current graphics context.
****************************************************************************/
void InitCursor(void)
{
    GA_palette      fore = {0xFF,0,0,0};
    GA_palette      back = {0,0xFF,0,0};
    int             x,y;

    curs.dwSize = sizeof(curs);
    if (!REF2D_queryFunctions(ref2d,GA_GET_CURSORFUNCS,&curs))
        return;
    if (modeInfo.BitsPerPixel <= 8 && !(modeInfo.Attributes & gaHave8bppRGBCursor)) {
        fore.Red = realColor(1);
        back.Red = realColor(2);
        fore.Green = fore.Blue = back.Green = back.Blue = 0;
        }
    curs.SetMonoCursor(&arrowCursor);
    curs.SetMonoCursorColor(&fore,&back);
    EVT_getMousePos(&x,&y);
    curs.SetCursorPos(x,y);
    curs.ShowCursor(true);
}

/****************************************************************************
REMARKS:
Main routine. Expects the x & y resolution of the desired video mode
to be passed on the command line. Will print out a list of available
video modes if no command line is present.
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    int     xm,ym,xc,yc;
    ulong   startTime,lapseTime;

    /* Check to see if we are running in a window */
    if (PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        return -1;
        }

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,640,480,8,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,SNAP_SHARED);
#endif

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(0,SNAP_SHARED)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

    /* Display available modes for invalid command line */
    if (argc < 4) {
        /* Restore the console */
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        AvailableModes();
        GA_unloadDriver(dc);
        return 0;
        }

    /* Use Zen Timer services to schedule DPVL updates */
    ZTimerInit();

    /* Get requested resolution, start graphics and draw pattern */
    xRes = atoi(argv[1]);
    yRes = atoi(argv[2]);
    bpp = atoi(argv[3]);
    if ((oldMode = InitGraphics(xRes,yRes,bpp)) != -1) {
        DrawMoire();
        WriteText(10,10,"Text should appear at upper left corner",defcolor);
        WriteText(maxX-400,10,"Text should appear at upper right corner",defcolor);
        WriteText(10,maxY-20,"Text should appear at lower left corner",defcolor);
        WriteText(maxX-400,maxY-20,"Text should appear at lower right corner",defcolor);

        /* Initialize event library with mouse cursor support */
        EVT_init(moveMouse);
        EVT_setMouseRange(xRes-1,yRes-1);
        EVT_setMousePos(xRes/2,yRes/2);
        InitCursor();

        /* Switch to DPVL video mode */
        DPVL.Start();

        /* Update each DPVL rectangle in virtual desktop list */
        LZTimerOn();
        startTime = LZTimerLap();
        while (!EVT_kbhit()) {
            lapseTime = LZTimerLap();
            EVT_getMousePos(&xm,&ym);
            if (xm != xc || ym != yc)
                curs.SetCursorPos(xc = xm, yc = ym);
            if (lapseTime > startTime + 100000UL) {
                DPVL.UpdateNextRect();
                startTime = LZTimerLap();
                }
            }
        EVT_getch();
        LZTimerOff();

        /* Switch back to non-DPVL raster video mode */
        DPVL.Stop();

        /* Restore the original display mode */
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        EVT_exit();
        }

    /* Restore the console */
    ExitSoftwareRasterizer();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);

    /* Unload the device driver */
    GA_unloadDriver(dc);

    if (oldMode == -1)
        printf("Unable to find specified graphics mode, or error starting mode.\n");
    return 0;
}

