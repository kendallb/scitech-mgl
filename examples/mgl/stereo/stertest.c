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
* Description:  Sample program showing how to use the SciTech MGL to
*               test if stereo page-flipping display modes are working.
*               (Test does not require LC shutter glasses. DaveM)
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgraph.h"
#ifdef  __WINDOWS__
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*------------------------------ Implementation ---------------------------*/

void stereo(int refreshRate)
{
    MGLDC   *dc;
    int     xRes=640,yRes=480,bits=8;
    int     mode,numPages;
    int     x,y,width,height;
    event_t evt;

    /* Use default refresh rate previously selected by user
     * with external utility or control panel. As much as we'd
     * like to use 120 Hz refresh for LCS glasses, this may be
     * too high for the *average* monitor. (DaveM)
     */
    if (refreshRate == 0)
        refreshRate = MGL_DEFAULT_REFRESH;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Start the MGL and create a stereo display device context */
    if (!MGL_init(".",NULL))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Select a typical stereo display mode */
    if ((mode = MGL_findMode(xRes,yRes,bits)) == -1)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    numPages = MIN(MGL_availablePages(mode),2);

    /* Create the stereo display mode */
    if ((dc = MGL_createStereoDisplayDC(mode,numPages,refreshRate)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_makeCurrentDC(dc);

    /* Display red and blue rectangles in left and right draw buffers */
    width = MGL_maxx() * 3/4;
    height = MGL_maxy() * 3/4;
    x = MGL_maxx() / 4;
    y = MGL_maxy() / 4;
    MGL_setActivePage(dc, 0 | MGL_LEFT_BUFFER);
    MGL_clearDevice();
    MGL_setColor(MGL_rgbColor(dc, 255, 0, 0));
    MGL_fillRectCoord(0, 0, width, height);

    MGL_setActivePage(dc, 0 | MGL_RIGHT_BUFFER);
    MGL_clearDevice();
    MGL_setColor(MGL_rgbColor(dc, 0, 0, 255));
    MGL_fillRectCoord(x, y, x+width, y+height);

    /* Overlapping red and blue regions should appear magenta
     * if stereo page-flipping is working correctly.
     */
    MGL_startStereo(dc);
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Disable free running stereo mode */
    MGL_stopStereo(dc);
    MGL_exit();
}

#if defined(__REALDOS__) || defined(__OS2_CONSOLE__) || defined(__UNIX__) || defined(__CONSOLE__)

int main(int argc,char **argv)
{
    int refreshRate = 0;

    if (argc > 1 && strncmp(argv[1],"-r",2) == 0) {
        refreshRate = atoi(&argv[1][2]);
        argc--;
        argv++;
        }
    stereo(refreshRate);
    return 0;
}

#elif defined(__OS2_PM__)

// TODO: Add code to support OS/2 Presentation Manager apps

#elif   defined(__WINDOWS__)
int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
{
    int refreshRate;

    if (strncmp(szCmdLine,"-r",2) == 0)
        refreshRate = atoi(szCmdLine+2);
    else
        refreshRate = 0;
    stereo(refreshRate);
    return 0;
}
#endif
