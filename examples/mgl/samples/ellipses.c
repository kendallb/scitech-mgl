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
* Description:  Simple demo showing how to draw a bunch of simple ellipses
*               on the screen with the MGL.
*
****************************************************************************/

#include "mglsamp.h"
#include "pattern.h"

#define MIN_RADIUS  5

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "ellipse";
int maxx,maxy;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc  - Display dc

REMARKS:
This demo fills the entire screen with an ellipses. The MGL_fillEllipse
function uses a bounding rectangle defining the ellipses. The size, color
and pen style are randomly changing as well.
****************************************************************************/
ibool ellipsesDemo(
    MGLDC *dc)
{
    int     val = 0;
    rect_t  r;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Filled ellipses Demonstration");
    statusLine("Press any key to start random test, ESC to Abort");

    /* Fit the ellipses to this viewport now including status line */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    /* The pen style MGL_BITMAP_TRANSPARENT uses the foreground
     * color to fill in all pixels in the bitmap. Where the pixels in the
     * bitmap pattern are a 0, the original background color is retained.
     * In the MGL_BITMAP_OPAQUE mode, the background color is used to
     * fill in the pixels in the bitmap that are set to a 0.
     */
    while (!checkEvent()) {
        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());
        if (val == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            val = 0;
            }
        val++;
        r.top = maxy/2;
        r.left = maxx/2;
        r.right = maxx/2;
        r.bottom = maxy/2;
        while((r.top || (r.right<maxx)) && (!checkEvent())) {
            if(r.top) r.top--;
            if(r.left) r.left--;
            if(r.right<maxx) r.right++;
            if(r.bottom<maxy) r.bottom++;
            MGL_fillEllipse(r);
            }
        }
    defaultAttributes(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc  - Display dc

REMARKS:
Demo randomly places ellipses on the screen.
****************************************************************************/
ibool randomEllipsesDemo(
    MGLDC *dc)
{
    int val;
    rect_t  r;

    mainWindow(dc,"Random filled ellipses Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    while (!checkEvent()) {
        r.left = MGL_random(maxx-100);
        r.top = MGL_random(maxy-100);
        r.right = r.left + MGL_random(100);
        r.bottom = r.top + MGL_random(100);
        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());
        if ((val = MGL_random(5)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }
        MGL_fillEllipse(r);
        }

    defaultAttributes(dc);
    return pause();
}

void demo(MGLDC *dc)
{
    /* Get the screen size and store them. */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    if(!ellipsesDemo(dc)) return;
    if(!randomEllipsesDemo(dc)) return;
}
