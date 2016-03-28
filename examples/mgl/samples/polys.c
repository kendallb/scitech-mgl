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
* Description:  Simple demo showing how to draw complex and convex polygons
*               on the screen.
*
****************************************************************************/

#include "mglsamp.h"
#include "pattern.h"

#define MAXPTS 6       /* Maximum # of pts in polygon  */

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "polys";

/*------------------------------ Implementation ---------------------------*/

ibool fastPolyDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     fastPolyDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of convex triangular polygons
*               in replace mode at full speed.
*
****************************************************************************/
{
    int         maxx,maxy;
    fxpoint_t   poly[4];            /* Space to hold polygon data   */

    mainWindow(dc,"MGL_fillPolygonFast Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        /* Define a random polygon */

        poly[0].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[0].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[1].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[1].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[2].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[2].y = MGL_randoml(MGL_TOFIX(maxy));

        MGL_setColor(randomColor());
        MGL_fillPolygonFX(3,poly,sizeof(fxpoint_t),0,0);
        }

    defaultAttributes(dc);
    return pause();
}

ibool polyDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     polyDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of polygons on the screen with
*               random fill styles.
*
****************************************************************************/
{
    int         i,maxx,maxy,val;
    fxpoint_t   poly[MAXPTS];           /* Space to hold polygon data   */

    mainWindow(dc,"MGL_fillPolygon Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        /* Define a random polygon */

        for (i = 0; i < MAXPTS; i++) {
            poly[i].x = MGL_randoml(MGL_TOFIX(maxx));
            poly[i].y = MGL_randoml(MGL_TOFIX(maxy));
            }

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

        MGL_fillPolygonFX(MAXPTS,poly,sizeof(fxpoint_t),0,0);
        }

    defaultAttributes(dc);
    return pause();
}

void demo(MGLDC *dc)
{
    /* Display title message at the top of the window. */
    mainWindow(dc,"Polygon Demo");

    if(!fastPolyDemo(dc)) return;
    if(!polyDemo(dc)) return;
}






