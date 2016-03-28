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

char demoName[] = "arcs";
int maxx,maxy;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc  - Display dc

REMARKS:
Demo places ellipse arcs in repeating pattern on the screen.
****************************************************************************/
ibool arcDemo(
    MGLDC *dc)
{
    int     startAngle,endAngle,i,dx,dy,da = 0;
    rect_t  r;

    mainWindow(dc,"Arc Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    /* Fit ellipse arcs to this viewport now including status line */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    startAngle = 0;
    endAngle = 20;

    /* Draw concentric arcs in gradually incrementing angles */
    while (!checkEvent()) {
		MGL_clearViewport();
        MGL_setColor(randomColor());
		dx = maxx/10 - 1;
		dy = maxy/10 - 1;
        r.top = maxy/2;
        r.left = maxx/2;
        r.right = maxx/2;
        r.bottom = maxy/2;
		for (i = 1; i <= 5; i++) {
            r.top -= dy;
            r.left -= dx;
            r.right += dx;
            r.bottom += dy;
			MGL_setPenSize(i,i);
			MGL_ellipseArc(r,startAngle,endAngle);
            }
        if (!pause())
            break;
        startAngle = endAngle + 10;
		endAngle = startAngle + 20 + da;
        da += 10;
        da %= 330;
        }

    defaultAttributes(dc);
    return true;
}

void demo(MGLDC *dc)
{
    /* Get the screen size and store them. */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    arcDemo(dc);
}
