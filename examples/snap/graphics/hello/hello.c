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
* Description:  Simple 'Hello World' program to initialise a user
*               specified graphics mode, and display a simple moire pattern
*               using the SciTech SNAP Graphics Architecture and the
*               simple GASDK API.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/gasdk.h"

/*---------------------------- Global Variables ---------------------------*/

GA_initFuncs    init;
GA_driverFuncs  driver;
PM_HWND         hwndConsole;
void            *stateBuf;
int             oldMode = -1;
int             xRes,yRes,bpp;


#ifdef  ISV_LICENSE
#include "isv.c"
#endif

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
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

/****************************************************************************
REMARKS:
Draw a simple moire pattern of lines on the display
****************************************************************************/
void DrawMoire(void)
{
    int     i,value;

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
Display a list of available resolutions
****************************************************************************/
void AvailableModes(void)
{
    N_uint16    *modes;

    printf("Available graphics modes:\n");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        printf("    %4d x %4d %d bits per pixel\n",
            modeInfo.XResolution, modeInfo.YResolution, modeInfo.BitsPerPixel);
        }
    printf("\nUsage: hello <xres> <yres> <bits>\n");
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
    int         oldMode;
    N_uint16    *modes;

    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
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
            return oldMode;
            }
        }
    return -1;
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
    GA_registerLicense(OemLicense,false);
#endif

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(0,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

    /* Display available modes for invalid command line */
    if (argc != 4) {
        /* Restore the console */
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        AvailableModes();
        GA_unloadDriver(dc);
        return 0;
        }

    /* Get requested resolution, start graphics and draw pattern */
    xRes = atoi(argv[1]);
    yRes = atoi(argv[2]);
    bpp = atoi(argv[3]);
    if ((oldMode = InitGraphics(xRes,yRes,bpp)) != -1) {
        DrawMoire();
        PM_getch();

        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
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

