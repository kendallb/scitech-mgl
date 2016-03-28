/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Language:     C++ 3.0
* Environment:  Any
*
* Description:  Member functions for the MVFrame class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mframe.hpp"
#include "mvis/mwindow.hpp"

/*----------------------------- Implementation ----------------------------*/

MVFrame::MVFrame(MGLDevCtx& dc,const MVRect& bounds,int width,uint flags)
    : MVView(dc,bounds), width(width), flags(flags)
/****************************************************************************
*
* Function:     MVFrame::MVFrame
* Parameters:   dc      - Device context
*               bounds  - Bounding rectangle for frame
*               width   - Width of frame
*               flags   - Window creation flags
*
****************************************************************************/
{
    options &= ~ofClipToClient; // Dont clip to client rect
    setBounds(bounds);
}

void MVFrame::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     MVFrame::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the MVFrame class. If we have
*               a mouse down event inside the frame, we post a resize
*               event to resize the window.
*
****************************************************************************/
{
    switch (event.what) {
        case evMouseDown:
            if (event.mouse.buttons & mbLeftButton) {
                if ((where = classifyPoint(event.where)) != Inside) {
                    MV_postMessage(owner,evCommand,cmResize,MV_PASS_POINT(event.where));
                    clearEvent(event);
                    }
                }
            break;
        }
}

void MVFrame::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVFrame::draw
* Parameters:   clip    - Clipping rectangle to use
*
* Description:  Draws the representation of the frame.
*
****************************************************************************/
{
    if (width < 3) {
        dc.setColor(scBlack);
        drawRect(bounds,width);
        }
    else {
        int w = 2;
        MVRect r(bounds);
        drawFrame(r,false);
        dc.setColor(getColor(scFrame));
        r.inset(w,w);
        for (int i = 0; i < width-w; i++) {
            dc.rect(r);
            r.inset(1,1);
            }
        }
}

void MVFrame::setBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVFrame::setBounds
* Parameters:   bounds  - New bounding rectangle for the view
*
* Description:  Sets the bounding rectangle for the view. Note that we
*               subtract the inner portion of our frame from the visRgn
*               as frame dont draw anything in the middle.
*
****************************************************************************/
{
    MVView::setBounds(bounds);
    inside = inner();
    visRgn -= inside;
}

MVRect MVFrame::inner()
/****************************************************************************
*
* Function:     MVFrame::inner
* Parameters:   bounds  - New bounding rectangle for the view
*
* Description:  Sets the bounding rectangle for the view. Note that we
*               subtract the inner portion of our frame from the visRgn
*               as frame dont draw anything in the middle.
*
****************************************************************************/
{
    return MVRect(bounds.left()+width,bounds.top()+width,
                  bounds.right()-width,bounds.bottom()-width);
}

int MVFrame::classifyPoint(const MVPoint& ptGlobal)
/****************************************************************************
*
* Function:     MVFrame::classifyPoint
* Parameters:   ptGlobal    - Point to classify
* Returns:      Classification code for the point
*
* Description:  Classifies the location of the point inside the frame for
*               the purpose of re-sizing the frame.
*
****************************************************************************/
{
    // Convert from global coords to owner viewport coords
    MVPoint p(ptGlobal);
    globalToLocal(p);
    int closeness = width*2;

    // Check if point is actually inside the frame
    if (inside.includes(p))
        return Inside;

    // Check if in top left corner
    MVRect r(bounds.left(),bounds.top(),
             bounds.left()+closeness,bounds.top()+closeness);
    if (r.includes(p))
        return TopLeft;

    // Check if in top right corner
    r.set(bounds.right()-closeness,bounds.top(),
          bounds.right(),bounds.top()+closeness);
    if (r.includes(p))
        return TopRight;

    // Check if in bottom left corner
    r.set(bounds.left(),bounds.bottom()-closeness,
          bounds.left()+closeness,bounds.bottom());
    if (r.includes(p))
        return BottomLeft;

    // Check if in bottom right corner
    r.set(bounds.right()-closeness,bounds.bottom()-closeness,
          bounds.right(),bounds.bottom());
    if (r.includes(p))
        return BottomRight;

    // Check if in top part
    if (p.y <= bounds.top()+width)
        return Top;

    // Check if in left part
    if (p.x <= bounds.left()+width)
        return Left;

    // Check if in right part
    if (p.x >= bounds.right()-width)
        return Right;

    // Must be in bottom part
    return Bottom;
}

MGLCursor *MVFrame::getCursor(const MVPoint& p)
/****************************************************************************
*
* Function:     MVFrame::getCursor
* Returns:      Returns the appropriate frame cursor
*
* Description:  Sets the cursor definition to the appropriate cursor
*               when inside the frame re-size areas.
*
****************************************************************************/
{
    if ((state & sfActive) && (flags & wfResizeable)) {
        switch (classifyPoint(p)) {
            case Left:
            case Right:
                return MV_SIZEWE_CURSOR;
            case Top:
            case Bottom:
                return MV_SIZENS_CURSOR;
            case TopLeft:
            case BottomRight:
                return MV_SIZENWSE_CURSOR;
            case TopRight:
            case BottomLeft:
                return MV_SIZENESW_CURSOR;
            }
        }
    return NULL;
}

void MVFrame::resizeDragRect(int deltaX,int deltaY,MVRect &dragRect)
/****************************************************************************
*
* Function:     MVFrame::resizeDragRect
* Parameters:   deltaX,deltaY   - Change in mouse location
*               dragRect        - Current drag rect to update
*
* Description:  Re-sizes the drag rectangle depending on which part of
*               the frame was clicked on to initiate the resize operation.
*
****************************************************************************/
{
    switch (where) {
        case Left:
            dragRect.left() += deltaX;
            break;
        case Top:
            dragRect.top() += deltaY;
            break;
        case Right:
            dragRect.right() += deltaX;
            break;
        case Bottom:
            dragRect.bottom() += deltaY;
            break;
        case TopLeft:
            dragRect.top() += deltaY;
            dragRect.left() += deltaX;
            break;
        case TopRight:
            dragRect.top() += deltaY;
            dragRect.right() += deltaX;
            break;
        case BottomLeft:
            dragRect.bottom() += deltaY;
            dragRect.left() += deltaX;
            break;
        case BottomRight:
            dragRect.bottom() += deltaY;
            dragRect.right() += deltaX;
            break;
        }
}
