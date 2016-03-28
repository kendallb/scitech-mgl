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
* Description:  Line region generation routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*--------------------------- Global Variables ----------------------------*/

static  region_t        *_rgn;
static  const region_t  *_pen;

/*------------------------- Implementation --------------------------------*/

#undef  MGL_rgnLine

/****************************************************************************
PARAMETERS:
x   - X coordinate to plot pixel at
y   - Y coordinate to plot pixel at

REMARKS:
Plots a pixel at the specified location for building the region.
****************************************************************************/
static void MGLAPI plotPoint(long x,long y)
{
    MGL_unionRegionOfs(_rgn,_pen,x,y);
}

/****************************************************************************
DESCRIPTION:
Generate a line as a region.

HEADER:
mgraph.h

PARAMETERS:
x1  - x coordinate for first endpoint
y1  - y coordinate for first endpoint
x2  - x coordinate for second endpoint
y2  - y coordinate for second endpoint
pen - Region to use as the pen when drawing the line

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
Generates a region as a line starting at the point (x1,y1) and ending at
the point (x2,y2). Note that this function takes the coordinates of the
lines in integer format.

SEE ALSO:
MGL_rgnLine
****************************************************************************/
region_t * MGLAPI MGL_rgnLineCoord(
    int x1,
    int y1,
    int x2,
    int y2,
    const region_t *pen)
{
    if (!pen)
        return NULL;

    /* Simple rectangular pen can use fat line scanlist routine directly.
     * Otherwise a complex region pen will have to iteratively generate
     * a unioned region using the MGL line engine.
     */
    if (!pen->spans) {
        scanlist scanList;
        _MGL_scanFatLine(&scanList,x1,y1,x2,y2,
            pen->rect.bottom - pen->rect.top-1,
            pen->rect.right - pen->rect.left-1);
        return _MGL_createRegionFromBuf(&scanList);
        }
    else {
        _rgn = MGL_newRegion();
        _pen = pen;
        MGL_lineEngine(x1,y1,x2,y2,plotPoint);
        return _rgn;
        }
}

/****************************************************************************
DESCRIPTION:
Generate a line as a region.

HEADER:
mgraph.h

PARAMETERS:
p1  - First endpoint
p2  - Second endpoint
pen - Region to use as the pen when drawing the line

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function is the same as MGL_rgnLine but takes the parameters for the
line as two points instead of coordinates.

SEE ALSO:
MGL_rgnLineCoord
****************************************************************************/
region_t *MGL_rgnLine(
    point_t p1,
    point_t p2,
    const region_t *pen);
/* Implemented as a macro */
