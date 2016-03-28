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
*               display fullscreen stereo bitmaps for viewing with
*               LC shutter glasses.
*
*               This version uses MGLSAMP.C front-end shell for
*               selecting display mode resolution for stereo. (DaveM)
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgraph.h"
#include "mglsamp.h"
#ifdef  __WINDOWS__
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif


/*----------------------------- Global Variables --------------------------*/

extern int refreshRate;

char demoName[] = "sterdemo";
char bitmapName[] = "stereo.bmp";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *orgdc)
{
    MGLDC           *dc;
    int             xRes,yRes,bits;
    int             mode,numPages;
    int             x,y,width,height;
    bitmap_t        *bitmap;
    ibool           quit = false;
    ibool           stereo = true;

    /* Get display mode already set. */
    xRes = MGL_sizex(orgdc)+1;
    yRes = MGL_sizey(orgdc)+1;
    bits = MGL_getBitsPerPixel(orgdc);
    if ((mode = MGL_findMode(xRes,yRes,bits)) == -1)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    numPages = MIN(MGL_availablePages(mode),2);

    /* Now load the bitmap file from disk. Note that we store the bitmap
     * files with the left and right images together side by side in a
     * single image, using the defacto standard "cross-eyed free-view"
     * format (right-eye image on left, left-eye image on right). (DaveM)
     */
    bitmap = MGL_loadBitmap(bitmapName,true);
    if (!bitmap)
        MGL_fatalError("Unable to locate stereo bitmap!");

    /* Create the stereo display mode */
    MGL_destroyDC(orgdc);
    if ((dc = MGL_createStereoDisplayDC(mode,numPages,refreshRate)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_makeCurrentDC(dc);

    /* Display the bitmap on the screen for the left and right images */
    width = bitmap->width/2;
    height = bitmap->height;
    x = (MGL_maxx()-width)/2;
    y = (MGL_maxy()-height)/2;
    MGL_setActivePage(dc,0 | MGL_LEFT_BUFFER);
    MGL_clearDevice();
    mainWindow(dc,"MGL Stereo Demo");
    statusLine("Press SPACEBAR to toggle Stereo Display, ESC to Abort");
    MGL_putBitmapSection(dc,width,0,width*2,height,x,y,bitmap,MGL_REPLACE_MODE);
    MGL_setActivePage(dc,0 | MGL_RIGHT_BUFFER);
    MGL_clearDevice();
    mainWindow(dc,"MGL Stereo Demo");
    statusLine("Press SPACEBAR to toggle Stereo Display, ESC to Abort");
    MGL_putBitmapSection(dc,0,0,width,height,x,y,bitmap,MGL_REPLACE_MODE);
    MGL_startStereo(dc);

    /* Loop for key press */
    while (!quit) {
        switch (waitEvent()) {
            case ' ':
                /* Toggle stereo display on or off */
                stereo = !stereo;
                if (stereo)
                    MGL_startStereo(dc);
                else
                    MGL_stopStereo(dc);
                break;
            case 's':
                /* Enable stereo display */
                MGL_startStereo(dc);
                stereo = true;
                break;
            case 'l':
                /* Disable stereo display and show only left eye buffer */
                MGL_stopStereo(dc);
                MGL_setVisualPage(dc,0 | MGL_LEFT_BUFFER,MGL_dontWait);
                stereo = false;
                break;
            case 'r':
                /* Disable stereo display and show right eye buffer */
                MGL_stopStereo(dc);
                MGL_setVisualPage(dc,0 | MGL_RIGHT_BUFFER,MGL_dontWait);
                stereo = false;
                break;
            case 'q':
            case 0x1B:
                quit = true;
                break;
            }
        }

    /* Disable free running stereo mode before destroying device context */
    MGL_stopStereo(dc);
    MGL_destroyDC(dc);
}
