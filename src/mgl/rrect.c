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
* Description:  Rectangle region generation routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#undef MGL_rgnSolidRectPt
#undef MGL_rgnSolidRect

/****************************************************************************
DESCRIPTION:
Generate a solid rectangle as a region.

HEADER:
mgraph.h

PARAMETERS:
left    - Left coordinate of the rectangle
top     - Top coordinate of the rectangle
right   - Right coordinate of the rectangle
bottom  - Bottom coordinate of the rectangle

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function generates a solid rectangle as a region.

SEE ALSO:
MGL_rgnSolidRect, MGL_rgnSolidRectPt
****************************************************************************/
region_t * MGLAPI MGL_rgnSolidRectCoord(
    int left,
    int top,
    int right,
    int bottom)
{
    region_t *r = MGL_newRegion();
    r->rect.left = left;
    r->rect.top = top;
    r->rect.right = right;
    r->rect.bottom = bottom;
    r->spans = NULL;
    return r;
}

/****************************************************************************
DESCRIPTION:
Generate a solid rectangle as a region from two points.

HEADER:
mgraph.h

PARAMETERS:
lt  - Point containing left-top coordinates of the region
rb  - Point containing right-bottom coordinates of the region

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function generates a solid rectangle as a region.

SEE ALSO:
MGL_rgnSolidRectCoord, MGL_rgnSolidRect
****************************************************************************/
region_t MGL_rgnSolidRectPt(
    point_t lt,
    point_t rb);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Generate a solid rectangle as a region from two points.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle the coordinates of the region

RETURNS:
New region generated, NULL if out of memory.

REMARKS:
This function generates a solid rectangle as a region.

SEE ALSO:
MGL_rgnSolidRectCoord, MGL_rgnSolidRectPt
****************************************************************************/
void MGL_rgnSolidRect(
    rect_t r);
/* Implemented as a macro */
