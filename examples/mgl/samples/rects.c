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
* Description:  Display a number of random rectangles with random
*               attributes.
*
****************************************************************************/

#include "mglsamp.h"
#include "pattern.h"

/*----------------------------- Global Variables --------------------------*/
char demoName[] = "rects";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    int     maxx,maxy,val;
    rect_t  r;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Rectangle Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random(maxx);
        r.right = MGL_random(maxx);
        r.top = MGL_random(maxy);
        r.bottom = MGL_random(maxy);

        /* Fix the rectangle so it is not empty */

        if (r.right < r.left)
            SWAP(r.left,r.right);
        if (r.bottom < r.top)
            SWAP(r.top,r.bottom);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        if ((val = MGL_random(3)) == 0) {
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

        MGL_fillRect(r);
        }

    defaultAttributes(dc);

    pause();
}








