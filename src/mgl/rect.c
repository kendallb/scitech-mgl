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
* Description:  Rectangle manipulation routines
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Create a new rectangle.

HEADER:
mgraph.h

PARAMETERS:
left    - Left coordinate of rectangle
top     - Top coordinate of rectangle
right   - Right coordinate of rectangle
bottom  - Bottom coordinate of rectangle

RETURNS:
Newly created rectangle returned by value.

REMARKS:
Creates a new rectangle given a set of coordinates.

SEE ALSO:
MGL_defRectPt
****************************************************************************/
rect_t MGLAPI MGL_defRect(
    int left,
    int top,
    int right,
    int bottom)
{
    rect_t  r;

    r.left = left;      r.top = top;
    r.right = right;    r.bottom = bottom;
    return r;
}

/****************************************************************************
DESCRIPTION:
Create a new rectangle from two points

HEADER:
mgraph.h

PARAMETERS:
leftTop - Upper left coordinate of rectangle
rightBottom - Lower right coordinate of rectangle

RETURNS:
Newly created rectangle returned by value.

REMARKS:
Creates a new rectangle given a set of points.

SEE ALSO:
MGL_defRect
****************************************************************************/
rect_t MGLAPI MGL_defRectPt(
    point_t leftTop,
    point_t rightBottom)
{
    rect_t  r;

    r.left = leftTop.x;         r.top = leftTop.y;
    r.right = rightBottom.x;    r.bottom = rightBottom.y;
    return r;
}

#undef  MGL_sectRect
/****************************************************************************
DESCRIPTION:
Compute the intersection between two rectangles.

HEADER:
mgraph.h

PARAMETERS:
r1  - First rectangle to intersect
r2  - Second rectangle to intersect
d   - Place to store the resulting intersection

RETURNS:
True if the rectangles intersect, false if not.

REMARKS:
Computes the intersection of two rectangles, and returns the result in a third. If the
rectangles actually intersect (the intersection is not an empty rectangle) then this
function returns true, otherwise it will return false.

SEE ALSO:
MGL_sectRectFast, MGL_sectRectCoord, MGL_unionRect
****************************************************************************/
ibool MGLAPI MGL_sectRect(
    rect_t r1,
    rect_t r2,
    rect_t *d)
{
    d->left = MAX(r1.left,r2.left);
    d->right = MIN(r1.right,r2.right);
    d->top = MAX(r1.top,r2.top);
    d->bottom = MIN(r1.bottom,r2.bottom);
    return !MGL_emptyRect(*d);
}

#undef  MGL_sectRectCoord
/****************************************************************************
DESCRIPTION:
Compute the intersection between two rectangles.

HEADER:
mgraph.h

PARAMETERS:
left1   - Left coordinate of first rectangle to intersect
top1    - Top coordinate of first rectangle to intersect
right1  - Right coordinate of first rectangle to intersect
bottom1 - Bottom coordinate of first rectangle to intersect
left2   - Left coordinate of second rectangle to intersect
top2    - Top coordinate of second rectangle to intersect
right2  - Right coordinate of second rectangle to intersect
bottom2 - Bottom coordinate of second rectangle to intersect
d       - Place to store the resulting intersection

RETURNS:
True if the rectangles intersect, false if not.

REMARKS:
Computes the intersection of two rectangles, and returns the result in a third. If the
rectangles actually intersect (the intersection is not an empty rectangle) then this
function returns true, otherwise it will return false.

SEE ALSO:
MGL_sectRectFastCoord, MGL_sectRect, MGL_unionRect
****************************************************************************/
ibool MGLAPI MGL_sectRectCoord(
    int left1,
    int top1,
    int right1,
    int bottom1,
    int left2,
    int top2,
    int right2,
    int bottom2,
    rect_t *d)
{
    (d)->left = MAX(left1,left2);
    (d)->right = MIN(right1,right2);
    (d)->top = MAX(top1,top2);
    (d)->bottom = MIN(bottom1,bottom2);
    return !MGL_emptyRect(*d);
}

#undef MGL_unionRect
/****************************************************************************
DESCRIPTION:
Computes the union of two rectangles.

HEADER:
mgraph.h

PARAMETERS:
r1  - First rectangle to compute union of
r2  - Second rectangle to compute union of
d   - Place to store resulting union rectangle

REMARKS:
This function computes the union of two rectangles, and stores the result in a third
rectangle.

SEE ALSO:
MGL_unionRectCoord, MGL_sectRect
****************************************************************************/
void MGLAPI MGL_unionRect(
    rect_t r1,
    rect_t r2,
    rect_t *d)
{
    (d)->left = MIN((r1).left,(r2).left);
    (d)->right = MAX((r1).right,(r2).right);
    (d)->top = MIN((r1).top,(r2).top);
    (d)->bottom = MAX((r1).bottom,(r2).bottom);
}

#undef MGL_unionRectCoord
/****************************************************************************
DESCRIPTION:
Computes the union of two rectangles.

HEADER:
mgraph.h

PARAMETERS:
left1   - Left coordinate of first rectangle to compute union of
top1    - Top coordinate of first rectangle to compute union of
right1  - Right coordinate of first rectangle to compute union of
bottom1 - Bottom coordinate of first rectangle to compute union of
left2   - Left coordinate of second rectangle to compute union of
top2    - Top coordinate of second rectangle to compute union of
right2  - Right coordinate of second rectangle to compute union of
bottom2 - Bottom coordinate of second rectangle to compute union of
d   - Place to store resulting union rectangle

REMARKS:
This function computes the union of two rectangles, and stores the result in a third
rectangle.

SEE ALSO:
MGL_sectRect
****************************************************************************/
void MGLAPI MGL_unionRectCoord(
    int left1,
    int top1,
    int right1,
    int bottom1,
    int left2,
    int top2,
    int right2,
    int bottom2,
    rect_t *d)
{
    (d)->left = MIN(left1,left2);
    (d)->right = MAX(right1,right2);
    (d)->top = MIN(top1,top2);
    (d)->bottom = MAX(bottom1,bottom2);
}

#undef MGL_sectRect
#undef MGL_sectRectFast
#undef MGL_sectRectCoord
#undef MGL_sectRectFastCoord
#undef MGL_unionRect
#undef MGL_unionRectCoord
#undef MGL_disjointRect
#undef MGL_equalPoint
#undef MGL_equalRect
#undef MGL_emptyRect
#undef MGL_insetRect
#undef MGL_leftTop
#undef MGL_rightBottom
#undef MGL_offsetRect
#undef MGL_ptInRectCoord
#undef MGL_ptInRect

/***************************************************************************
DESCRIPTION:
Returns the left top point of a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to return left top corner coordinate for.

RETURNS:
Returns the top left coordinate of the rectangle.

REMARKS:

SEE ALSO:
MGL_rightBottom
***************************************************************************/
point_t MGL_leftTop(
    rect_t r);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Returns the bottom right coordinate from a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to get coordinate from

RETURNS:
The bottom right coordinate of the rectangle.

REMARKS:
Returns the bottom right coordinates from a rectangle.

SEE ALSO:
MGL_leftTop
***************************************************************************/
point_t MGL_rightBottom(
    rect_t r);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Offsets a rectangle by the specified amount.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to offset
dx  - Amount to offset x coordinates by
dy  - Amount to offset y coordinates by

REMARKS:
This function offsets the specified rectangle by the dx and dy coordinates. This
function effectively performs the following operation on the rectangle:

    left += dx;
    top += dy;
    right += dx;
    bottom += dy;

SEE ALSO:
MGL_insetRect
***************************************************************************/
void MGL_offsetRect(
    rect_t r,
    int dx,
    int dy);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Determines if two rectangles are disjoint.

HEADER:
mgraph.h

PARAMETERS:
r1  - First rectangle to test
r2  - Second rectangle to test

RETURNS:
True if the rectangles are disjoint, false if they overlap.

REMARKS:
This function determines whether two rectangles are disjoint, which is true if the
rectangles do not overlap at any coordinates.

SEE ALSO:
MGL_emptyRect, MGL_equalRect, MGL_unionRect, MGL_sectRect
***************************************************************************/
ibool MGL_disjointRect(
    rect_t r1,
    rect_t r2);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Compares two points to determine if they are equal.

HEADER:
mgraph.h

PARAMETERS:
p1  - The first point to compare.
p2  - The second point to compare.

RETURNS:
True if the points are equal, false if they are not.

REMARKS:
***************************************************************************/
ibool MGL_equalPoint(
    point_t p1,
    point_t p2);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Compares two rectangles for equality.

HEADER:
mgraph.h

PARAMETERS:
r1  - First rectangle to compare
r2  - Second rectangle to compare

RETURNS:
True if the rectangles are equal, false if not.
REMARKS:
SEE ALSO:
MGL_equalPoint, MGL_equalRegion
***************************************************************************/
ibool MGL_equalRect(
    rect_t r1,
    rect_t r2);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Determines if a rectangle is empty.

HEADER:
mgraph.h

PARAMETERS:
r   - The rectangle to test

RETURNS:
True if the rectangle is empty, otherwise false.

REMARKS:
Determines if a rectangle is empty or not. A rectangle is defined as being empty if the right coordinate is less than or equal to the left coordinate, or if the bottom coordinate is less than or equal to the top coordinate.
***************************************************************************/
ibool MGL_emptyRect(
    rect_t r);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Insets the coordinates of a rectangle.

HEADER:
mgraph.h

PARAMETERS:
r   - Rectangle to inset
dx  - Amount to inset the x coordinates by
dy  - Amount to inset the y coordinates by

REMARKS:
This functions insets the rectangle by the specified values. This function effectively
performs the following operation on the rectangle:

    left += dx;
    top += dy;
    right -= dx;
    bottom -= dy;

SEE ALSO:
MGL_offsetRect
***************************************************************************/
void MGL_insetRect(
    rect_t r,
    int dx,
    int dy);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Compute the intersection between two rectangles.

HEADER:
mgraph.h

PARAMETERS:
r1  - First rectangle to intersect
r2  - Second rectangle to intersect
d   - Place to store the resulting intersection

REMARKS:
This is the same as MGL_sectRect but it is implemented as a macro and does
not test the rectangle for intersection.

SEE ALSO:
MGL_sectRect, MGL_sectRectCoord, MGL_unionRect
****************************************************************************/
void MGL_sectRectFast(
    rect_t r1,
    rect_t r2,
    rect_t *d);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Compute the intersection between two rectangles.

HEADER:
mgraph.h

PARAMETERS:
left1   - Left coordinate of first rectangle to intersect
top1    - Top coordinate of first rectangle to intersect
right1  - Right coordinate of first rectangle to intersect
bottom1 - Bottom coordinate of first rectangle to intersect
left2   - Left coordinate of second rectangle to intersect
top2    - Top coordinate of second rectangle to intersect
right2  - Right coordinate of second rectangle to intersect
bottom2 - Bottom coordinate of second rectangle to intersect
d       - Place to store the resulting intersection

RETURNS:
True if the rectangles intersect, false if not.

REMARKS:
Computes the intersection of two rectangles, and returns the result in a third. If the
rectangles actually intersect (the intersection is not an empty rectangle) then this
function returns true, otherwise it will return false.

SEE ALSO:
MGL_sectRectFastCoord, MGL_sectRect, MGL_unionRect
****************************************************************************/
ibool MGL_sectRectFastCoord(
    int left1,
    int top1,
    int right1,
    int bottom1,
    int left2,
    int top2,
    int right2,
    int bottom2,
    rect_t *d);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Returns true if supplied point is within the definition of a rectangle, otherwise false.

HEADER:
mgraph.h

PARAMETERS:
x   - X coordinate of rectangle to test
y   - Y coordinate of rectangle to test
r   - Rectangle to test

RETURNS:
True if supplied point is within the definition of the specified rectangle.

REMARKS:
This function tests whether a point is within the bounds of a rectangle or
not.

SEE ALSO:
MGL_ptInRect
***************************************************************************/
ibool MGL_ptInRectCoord(
    int x,
    int y,
    rect_t r);
/* Implemented as a macro */

/***************************************************************************
DESCRIPTION:
Returns true if supplied point is within the definition of a rectangle, otherwise false.

HEADER:
mgraph.h

PARAMETERS:
p   - Point to test
r   - Rectangle to test

RETURNS:
True if supplied point is within the definition of the specified rectangle.

REMARKS:
This function tests whether a point is within the bounds of a rectangle or
not. This version takes the coordinates of the rectangle as two points.

SEE ALSO:
MGL_ptInRect
***************************************************************************/
ibool MGL_ptInRect(
    point_t p,
    rect_t r);
/* Implemented as a macro */
