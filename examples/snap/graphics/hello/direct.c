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
*               specified graphics mode, and render directly on the linear
*               framebuffer using the SciTech SNAP Graphics Architecture.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/graphics.h"

/*---------------------------- Global Variables ---------------------------*/

GA_devCtx       *dc;
GA_initFuncs    init;
GA_driverFuncs  driver;
GA_modeInfo     modeInfo;
N_int32         maxMem;
N_int32         maxx,maxy;   /* Maximum coordinates for direct surface  */
N_int32         virtualX;    /* Scanline width for surface              */
N_int32         virtualX;    /* Scanline width for surface              */
N_int32         bytesPerLine;/* Scanline width for surface              */
char            *surface;    /* Pointer to the surface to draw to       */
PM_HWND         hwndConsole;
void            *stateBuf;
int             oldMode = -1;

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
        maxx = maxy = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&maxx,&maxy,&bytesPerLine,&maxMem,0,NULL);
        }
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

/****************************************************************************
PARAMETERS:
x,y     - Coordinate to plot pixel at
color   - Color to plot the pixel in

REMARKS:
Plots a pixel to an 8 bit device surface.
****************************************************************************/
void putPixel(
    int x,
    int y,
    int color)
{
    long address = y * bytesPerLine + x;
    *((char*)(surface + address)) = color;
}

/****************************************************************************
PARAMETERS:
x1,y1       - First endpoint of line
x2,y2       - Second endpoint of line

REMARKS:
Scan convert a line segment using the MidPoint Digital Differential
Analyser algorithm.
****************************************************************************/
void line(
    int x1,
    int y1,
    int x2,
    int y2,
    int color)
{
    int     d;                      /* Decision variable                */
    int     dx,dy;                  /* Dx and Dy values for the line    */
    int     Eincr,NEincr;           /* Decision variable increments     */
    int     yincr;                  /* Increment for y values           */
    int     t;                      /* Counters etc.                    */

    dx = ABS(x2 - x1);
    dy = ABS(y2 - y1);

    if (dy <= dx) {
        /* We have a line with a slope between -1 and 1
         *
         * Ensure that we are always scan converting the line from left to
         * right to ensure that we produce the same line from P1 to P0 as the
         * line from P0 to P1.
         */
        if (x2 < x1) {
            t = x2; x2 = x1; x1 = t;    /* Swap X coordinates           */
            t = y2; y2 = y1; y1 = t;    /* Swap Y coordinates           */
            }

        if (y2 > y1)
            yincr = 1;
        else
            yincr = -1;

        d = 2*dy - dx;              /* Initial decision variable value  */
        Eincr = 2*dy;               /* Increment to move to E pixel     */
        NEincr = 2*(dy - dx);       /* Increment to move to NE pixel    */

        putPixel(x1,y1,color);      /* Draw the first point at (x1,y1)  */

        /* Incrementally determine the positions of the remaining pixels
         */
        for (x1++; x1 <= x2; x1++) {
            if (d < 0) {
                d += Eincr;         /* Choose the Eastern Pixel         */
                }
            else {
                d += NEincr;        /* Choose the North Eastern Pixel   */
                y1 += yincr;        /* (or SE pixel for dx/dy < 0!)     */
                }
            putPixel(x1,y1,color);  /* Draw the point                   */
            }
        }
    else {
        /* We have a line with a slope between -1 and 1 (ie: includes
         * vertical lines). We must swap our x and y coordinates for this.
         *
         * Ensure that we are always scan converting the line from left to
         * right to ensure that we produce the same line from P1 to P0 as the
         * line from P0 to P1.
         */
        if (y2 < y1) {
            t = x2; x2 = x1; x1 = t;    /* Swap X coordinates           */
            t = y2; y2 = y1; y1 = t;    /* Swap Y coordinates           */
            }

        if (x2 > x1)
            yincr = 1;
        else
            yincr = -1;

        d = 2*dx - dy;              /* Initial decision variable value  */
        Eincr = 2*dx;               /* Increment to move to E pixel     */
        NEincr = 2*(dx - dy);       /* Increment to move to NE pixel    */

        putPixel(x1,y1,color);      /* Draw the first point at (x1,y1)  */

        /* Incrementally determine the positions of the remaining pixels
         */
        for (y1++; y1 <= y2; y1++) {
            if (d < 0) {
                d += Eincr;         /* Choose the Eastern Pixel         */
                }
            else {
                d += NEincr;        /* Choose the North Eastern Pixel   */
                x1 += yincr;        /* (or SE pixel for dx/dy < 0!)     */
                }
            putPixel(x1,y1,color);  /* Draw the point                   */
            }
        }
}

/****************************************************************************
REMARKS:
Draws a simple moire pattern on the current surface.
****************************************************************************/
void DrawMoire(void)
{
    int     i;

    for (i = 0; i < maxx; i += 4) {
        line(maxx/2,maxy/2,i,0,i % 255);
        line(maxx/2,maxy/2,i,maxy,(i+1) % 255);
        }
    for (i = 0; i < maxy; i += 4) {
        line(maxx/2,maxy/2,0,i,(i+2) % 255);
        line(maxx/2,maxy/2,maxx,i,(i+3) % 255);
        }
    line(0,0,maxx,0,15);
    line(0,0,0,maxy,15);
    line(maxx,0,maxx,maxy,15);
    line(0,maxy,maxx,maxy,15);
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
    int         oldMode,mode;
    N_uint16    *modes;

    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits) {
            mode = *modes;
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                mode |= gaLinearBuffer;
            oldMode = init.GetVideoMode();
            maxx = maxy = bytesPerLine = -1;
            if (init.SetVideoMode(mode,&maxx,&maxy,&bytesPerLine,&maxMem,0,NULL) != 0)
                return -1;

            /* Setup variables for our direct line drawing code */
            surface = dc->LinearMem;
            maxx -= 1;
            maxy -= 1;
            return oldMode;
            }
        }
    return -1;
}

/****************************************************************************
REMARKS:
Main routine. Does not expect a command line, and simply draws a bunch
of lines in the 640x480x8 display mode.
****************************************************************************/
int main(void)
{
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

    /* Get requested resolution, start graphics and draw pattern */
    if ((oldMode = InitGraphics(640,480,8)) != -1) {
        DrawMoire();
        PM_getch();
        maxx = maxy = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&maxx,&maxy,&bytesPerLine,&maxMem,0,NULL);
        }

    /* Restore the console */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);

    /* Unload the device driver */
    GA_unloadDriver(dc);

    if (oldMode == -1)
        printf("Unable to find specified graphics mode, or error starting mode.\n");
    return 0;
}

