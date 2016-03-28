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
#ifndef _MAX_PATH
#define _MAX_PATH       255
#endif
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*------------------------------ Implementation ---------------------------*/

void stereo(const char *bitmapName,int refreshRate)
{
    MGLDC           *dc;
    int             xRes=640,yRes=480,bits=8;
    int             mode,numPages;
    int             x,y,width,height;
    bitmap_t        *bitmap;
    event_t         evt;

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

    /* Now load the bitmap file from disk. Note that we store the bitmap
     * files with the left and right images together side by side in a
     * single image, using the defacto standard "cross-eyed free-view"
     * format (right-eye image on left, left-eye image on right). (DaveM)
     */
    bitmap = MGL_loadBitmap(bitmapName,true);
    if (!bitmap)
        MGL_fatalError("Unable to locate stereo bitmap!");

    /* Search for a suitable graphics mode to display the image */
    if (bitmap->bitsPerPixel >= 24)
        bits = 32;
    else if (bitmap->bitsPerPixel >= 15)
        bits = 16;
    else
        bits = 8;
    if ((mode = MGL_findMode(xRes,yRes,bits)) == -1)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    numPages = MIN(MGL_availablePages(mode),2);

    /* Create the stereo display mode */
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
    MGL_putBitmapSection(dc,width,0,width*2,height,x,y,bitmap,MGL_REPLACE_MODE);
    MGL_setActivePage(dc,0 | MGL_RIGHT_BUFFER);
    MGL_clearDevice();
    MGL_putBitmapSection(dc,0,0,width,height,x,y,bitmap,MGL_REPLACE_MODE);
    MGL_startStereo(dc);
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Disable free running stereo mode */
    MGL_stopStereo(dc);
    MGL_exit();
}

#if defined(__REALDOS__) || defined(__OS2_CONSOLE__) || defined(__UNIX__) || defined(__CONSOLE__)

int main(int argc,char **argv)
{
    int     refreshRate = 0;
    char    bitmapName[_MAX_PATH];

    if (argc > 1 && strncmp(argv[1],"-r",2) == 0) {
        refreshRate = atoi(&argv[1][2]);
        argc--;
        argv++;
        }
    if (argc > 1)
        strcpy(bitmapName,argv[1]);
    else
        strcpy(bitmapName,"stereo.bmp");
    stereo(bitmapName,refreshRate);
    return 0;
}

#elif defined(__OS2_PM__)

// TODO: Add code to support OS/2 Presentation Manager apps

#elif   defined(__WINDOWS__)

static char    szAppFilter[] = "Bitmaps\0*.bmp\0";
static char    bitmapName[_MAX_PATH] = "";

ibool OpenBitmapFile(HWND hwnd)
/****************************************************************************
*
* Function:     OpenBitmapFile
* Parameters:   hwnd    - Handle to parent window
* Returns:      True for valid bitmap name, false otherwise
*
* Description:  Open the bitmap file and load it into the memory DC.
*
****************************************************************************/
{
    OPENFILENAME    ofn;

    /* Prompt user for file to open */
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szAppFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = bitmapName;
    ofn.nMaxFile = sizeof(bitmapName);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    return GetOpenFileName(&ofn);
}

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
{
    int refreshRate;
    if (strncmp(szCmdLine,"-r",2) == 0)
        refreshRate = atoi(szCmdLine+2);
    else
        refreshRate = 0;
    strcpy(bitmapName,"");
    while (OpenBitmapFile(NULL)) {
        stereo(bitmapName,refreshRate);
        }
    return 0;
}
#endif
