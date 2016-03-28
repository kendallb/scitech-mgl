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
* Description:  Demo showing how to use the MGL region functions to create
*               simple regions
*
****************************************************************************/

#include "mglsamp.h"

#define R_UNION         0
#define R_DIFFERENCE    1
#define R_INTERSECTION  2

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "regions";
int maxx,maxy;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc          - Display dc
arithmetic  - Intersection, difference or union

REMARKS:
Displays two circles and shows there intersection, difference and union
using region functions from the MGL.
****************************************************************************/
ibool regionDemo(
    MGLDC *dc,
    char arithmetic)
{
    region_t    *region1;
    region_t    *region2;
    region_t    *regionh;
    rect_t      rect;
    int         radius;

    /* Find the dimensiosn of the ellipses */
    if (maxx <= maxy)
        radius = maxx/2;
    else
        radius = maxy/2;

    /* Display title message at the top of the window. */
    switch(arithmetic) {
        case R_INTERSECTION:
            mainWindow(dc,"Intersecting Region Demonstration");
            break;
        case R_DIFFERENCE:
            mainWindow(dc,"Difference Region Demonstration");
            break;
        case R_UNION:
            mainWindow(dc,"Union Region Demonstration");
            break;
        }
    statusLine("Press any key to continue...");

    /* Create first region */
    rect.left = maxx/2 - (radius*1.5)/2;
    rect.top = radius/4-7;
    rect.right = rect.left+radius;
    rect.bottom = rect.top+radius-7;
    region1 = MGL_rgnSolidEllipse(rect);
    MGL_setColorCI(1);
    MGL_drawRegion(0,0,region1);

    /* Create second region */
    rect.left += radius/2;
    rect.top += radius/2;
    rect.right += radius/2;
    rect.bottom += radius/2;
    region2 = MGL_rgnSolidEllipse(rect);
    MGL_setColorCI(2);
    MGL_drawRegion(0,0,region2);

    MGL_setColorCI(7);
    switch(arithmetic) {
        case R_INTERSECTION:
            /* Create a region to hold the intersection information */
            regionh = MGL_newRegion();

            /* Determine if the two regions have intersected */
            regionh = MGL_sectRegion(region1,region2);
            if (!MGL_emptyRegion(regionh))
                MGL_drawRegion(0,0,regionh);
            MGL_freeRegion(regionh);
            break;
        case R_DIFFERENCE:
            if (MGL_diffRegion(region1,region2))
                MGL_drawRegion(0,0,region1);
            break;
        case R_UNION:
            if (MGL_unionRegion(region1,region2))
                MGL_drawRegion(0,0,region1);
            break;
        }

    /* Free all allocated regions */
    MGL_freeRegion(region2);
    MGL_freeRegion(region1);
    return pause();
}

void demo(MGLDC *dc)
{
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    if(!regionDemo(dc,R_INTERSECTION)) return;
    if(!regionDemo(dc,R_DIFFERENCE)) return;
    if(!regionDemo(dc,R_UNION)) return;
}
