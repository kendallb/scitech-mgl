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
* Language:     ANSI C++
* Environment:  any
*
* Description:  Implementation for an integer rectangle class.
*
****************************************************************************/

#include "mgraph.hpp"

/*---------------------------- Implementation -----------------------------*/

inline ibool _isEmpty(int l,int t,int r,int b)
{
    return (b <= t || r <= l);
}

MGLRect& MGLRect::makeUnion(int left,int top,int right,int bottom)
/****************************************************************************
*
* Function:     MGLRect::makeUnion
* Parameters:   left    - Left coordinate of rectangle
*               top     - Top coordinate of rectangle
*               right   - Right coordinate of rectangle
*               bottom  - Bottom coordinate of rectangle
* Returns:      Reference to the current rectangle.
*
* Description:  Computes the union of two rectangles.
*
****************************************************************************/
{
    if (isEmpty()) {
        topLeft.x = left;
        topLeft.y = top;
        botRight.x = right;
        botRight.y = bottom;
        }
    else if (!_isEmpty(left,top,right,bottom)) {
        topLeft.x = MIN(topLeft.x, left);
        topLeft.y = MIN(topLeft.y, top);
        botRight.x = MAX(botRight.x, right);
        botRight.y = MAX(botRight.y, bottom);
        }
    return *this;
}

MGLRect& MGLRect::expand(int x,int y)
/****************************************************************************
*
* Function:     MGLRect::expand
* Parameters:   x,y - Point to include in rectangle
* Returns:      Reference to the current rectangle.
*
* Description:  Expands the size of a rectangle to include the specified
*               point.
*
****************************************************************************/
{
    if (isEmpty()) {
        topLeft.x = x;
        topLeft.y = y;
        botRight.x = x+1;
        botRight.y = y+1;
        }
    else {
        topLeft.x = MIN(topLeft.x, x);
        topLeft.y = MIN(topLeft.y, y);
        botRight.x = MAX(botRight.x, x+1);
        botRight.y = MAX(botRight.y, y+1);
        }
    return *this;
}

MGLRect& MGLRect::makeIntersect(int left,int top,int right,int bottom)
/****************************************************************************
*
* Function:     MGLRect::makeIntersect
* Parameters:   left    - Left coordinate of rectangle
*               top     - Top coordinate of rectangle
*               right   - Right coordinate of rectangle
*               bottom  - Bottom coordinate of rectangle
* Returns:      Reference to the current rectangle.
*
* Description:  Computes the intersection of two rectangles.
*
****************************************************************************/
{
    topLeft.x = MAX(topLeft.x, left);
    topLeft.y = MAX(topLeft.y, top);
    botRight.x = MIN(botRight.x, right);
    botRight.y = MIN(botRight.y, bottom);
    if (isEmpty())
        empty();
    return *this;
}

ibool MGLRect::intersect(int left,int top,int right,int bottom) const
/****************************************************************************
*
* Function:     MGLRect::intersect
* Parameters:   left    - Left coordinate of rectangle
*               top     - Top coordinate of rectangle
*               right   - Right coordinate of rectangle
*               bottom  - Bottom coordinate of rectangle
* Returns:      True if the rectangles intersect
*
****************************************************************************/
{
    if (_isEmpty(MAX(topLeft.x, left), MAX(topLeft.y, top),
            MIN(botRight.x, right), MIN(botRight.y, bottom)))
        return false;
    return true;
}

