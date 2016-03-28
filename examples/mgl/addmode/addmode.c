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
* Description:  Simple sample program showing how to add custom display
*               modes using the MGL, so that your app can run in display
*               modes not originally enabled in the SciTech SNAP Graphics
*               driver.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgraph.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*------------------------------ Implementation ---------------------------*/

int main(
    int argc,
    char *argv[])
{
    MGLDC   *dc;
    int     mode,xRes,yRes,bitsPerPixel,i;
    event_t evt;
    font_t  *font;
    char    buf[80];

    /* Handle the command line */
    if (argc < 4)
        MGL_fatalError("Usage: addmode <xRes> <yRes> <bitsPerPixel>\n");
    xRes = atoi(argv[1]);
    yRes = atoi(argv[2]);
    bitsPerPixel = atoi(argv[3]);

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Add our custom display mode */
    if (MGL_init(".",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if (!MGL_addCustomMode(xRes,yRes,bitsPerPixel))
        MGL_fatalError("Unable to create custom display mode!");

    /* Start the MGL in the custom display mode */
    if ((mode = MGL_findMode(xRes,yRes,bitsPerPixel)) == -1)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((dc = MGL_createDisplayDC(mode,1,MGL_DEFAULT_REFRESH)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_makeCurrentDC(dc);

    /* Load the default 8x8 bitmap font into device context */
    if ((font = MGL_loadFont("pc8x8.fnt")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_useFont(font);

    /* Draw a simple pattern of lines on the display */
    xRes = MGL_maxx()+1;
    yRes = MGL_maxy()+1;
    for (i = 0; i < xRes; i += 5) {
        MGL_setColorCI(i % 255);
        MGL_lineCoord(xRes/2,yRes/2,i,0);
        MGL_setColorCI((i+1) % 255);
        MGL_lineCoord(xRes/2,yRes/2,i,yRes-1);
        }
    for (i = 0; i < yRes; i += 5) {
        MGL_setColorCI((i+2) % 255);
        MGL_lineCoord(xRes/2,yRes/2,0,i);
        MGL_setColorCI((i+3) % 255);
        MGL_lineCoord(xRes/2,yRes/2,xRes-1,i);
        }
    MGL_setColorCI(MGL_WHITE);
    MGL_lineCoord(0,0,xRes-1,0);
    MGL_lineCoord(0,0,0,yRes-1);
    MGL_lineCoord(xRes-1,0,xRes-1,yRes-1);
    MGL_lineCoord(0,yRes-1,xRes-1,yRes-1);

    /* Display the resolution dimensions on the screen */
    sprintf(buf,"Custom mode: %d x %d x %d", xRes, yRes, bitsPerPixel);
    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_CENTER_TEXT);
    MGL_drawStrXY(xRes/2,yRes/2,buf);

    /* Wait for a keypress or mouse click */
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Close down the MGL and exit */
    MGL_exit();
    return 0;
}
