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
* Description:  Polyline drawing routines
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Draws a set of pixels.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices in polyline
vArray  - Array of coordinates to draw the pixels at

REMARKS:
This function draws a set of pixels in the current color at the locations passed in the
vArray parameter.

SEE ALSO:
MGL_polyLine, MGL_polyMarker
****************************************************************************/
void MGLAPI MGL_polyPoint(
    int count,
    point_t *vArray)
{
    MGL_beginPixel();
    while (count--) {
        MGL_pixel(*vArray);
        vArray++;
        }
    MGL_endPixel();
}

/****************************************************************************
DESCRIPTION:
Draws a polyline as a complex region.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices in polyline
vArray  - Array of vertices in the polyline

REMARKS:
This function draws a polyline as a complex region. This is for cases where
the default polyline drawing routine might duplicate endpoint operations,
such as fat pen with XOR raster op. Constructing a complex region for the
polyline segments will guarantee that pixels will be rendered only once
with the specified raster op.
****************************************************************************/
void MGLAPI MGL_polyLineRgn(
    int count,
    point_t *vArray)
{
    int         i;
    point_t     *p1,*p2;
    region_t    *pen,*line,*poly;
    rect_t      r;

    /* Construct simple rectangular region for pen */
    r.left = r.top = 0;
    MGL_getPenSize(&r.bottom,&r.right);
    pen = MGL_rgnSolidRect(r);

    /* Start constructing complex region for polyline segments */
    poly = MGL_newRegion();

    /* Add line region for each polyline segment */
    p1 = vArray;
    p2 = vArray+1;
    i = count;
    while (--i) {
        line = MGL_rgnLine(*p1,*p2,pen);
        MGL_unionRegion(poly,line);
        MGL_freeRegion(line);
        p1 = p2++;
        }

    /* Finally draw the resulting region only once */
    MGL_drawRegion(0,0,poly);

    /* Dispose of all regions constructed */
    MGL_freeRegion(poly);
    MGL_freeRegion(pen);
}

/****************************************************************************
DESCRIPTION:
Draws a set of connected lines.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices in polyline
vArray  - Array of vertices in the polyline

REMARKS:
This function draws a set of connected line (a polyline). The coordinates of the
polyline are specified by vArray, and the lines are drawn in the current drawing
attributes.

Note that the polyline is not closed by default, so if you wish to draw the outline of
a polygon, you will need to add the starting point to the end of the vertex array.

SEE ALSO:
MGL_polyMarker, MGL_polyPoint
****************************************************************************/
void MGLAPI MGL_polyLine(
    int count,
    point_t *vArray)
{
    int     i;
    point_t *p1,*p2;

    /* Handle fat lines with raster ops as special complex region case in
     * order to guarantee that all pixels in the polyline are rendered
     * only once per MGL rasterization rules.
     */
    if ((DC.a.penWidth || DC.a.penHeight) && (DC.a.writeMode != MGL_REPLACE_MODE)) {
        MGL_polyLineRgn(count,vArray);
        }
    else {
        p1 = vArray;
        p2 = vArray+1;
        i = count;
        while (--i) {
            MGL_lineExt(*p1,*p2,i == 1);
            p1 = p2++;
            }
        }
}
