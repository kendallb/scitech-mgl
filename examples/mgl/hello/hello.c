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
* Description:  Simple hello program showing how to get up and running
*               using the MGL.
*
****************************************************************************/

#include "mgraph.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*------------------------------ Implementation ---------------------------*/

int main(void)
{
    MGLDC   *dc;
    int     xres,yres,i;
    event_t evt;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Start the MGL in the 640x480x256 display mode */
    dc = MGL_quickInit(640,480,8,1);

    /* Draw a simple pattern of lines on the display */
    xres = MGL_maxx()+1;
    yres = MGL_maxy()+1;
    for (i = 0; i < xres; i += 5) {
        MGL_setColorCI(i % 255);
        MGL_lineCoord(xres/2,yres/2,i,0);
        MGL_setColorCI((i+1) % 255);
        MGL_lineCoord(xres/2,yres/2,i,yres-1);
        }
    for (i = 0; i < yres; i += 5) {
        MGL_setColorCI((i+2) % 255);
        MGL_lineCoord(xres/2,yres/2,0,i);
        MGL_setColorCI((i+3) % 255);
        MGL_lineCoord(xres/2,yres/2,xres-1,i);
        }
    MGL_setColorCI(MGL_WHITE);
    MGL_lineCoord(0,0,xres-1,0);
    MGL_lineCoord(0,0,0,yres-1);
    MGL_lineCoord(xres-1,0,xres-1,yres-1);
    MGL_lineCoord(0,yres-1,xres-1,yres-1);

    /* Wait for a keypress or mouse click */
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Close down the MGL and exit */
    MGL_exit();
    return 0;
}
