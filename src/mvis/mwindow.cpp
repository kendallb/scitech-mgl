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
* Description:  Member functions for the MVWindow class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mwindow.hpp"
#include "mvis/mkeys.hpp"
#include "mvis/mprogram.hpp"

/*----------------------------- Implementation ----------------------------*/

MVWindow::MVWindow(MGLDevCtx& dc,const MVRect& bounds,const char *title,
    uint flags,int frameWidth)
    : MVGroup(dc,bounds),
      MVWindowInit(MVWindow::initFrame,MVWindow::initTitleBar),
      maximised(false), minimised(false), number(-1), flags(flags)
/****************************************************************************
*
* Function:     MVWindow::MVWindow
* Parameters:   bounds  - Bounding box for the window
*               title   - Title for the window's title bar
*               flags   - Creation flags for the window
*
* Description:  Constructor for the MVWindow class.
*
****************************************************************************/
{
    options |= ofSelectable | ofFirstClick | ofSelectFront;

    // Create the frame if required and adjust the size of the group's
    // bounding rectangle to fit inside the frame.
    MVRect extent;  getExtent(extent);

    if ((flags & wfFramed) && createFrame) {
        insert(frame = createFrame(dc,extent,frameWidth,flags));
        clientRect = frame->inner();
        }
    else
        frame = NULL;

    if (title && createTitleBar) {
        insert(titleBar = createTitleBar(dc,clientRect,title,this,flags));
        clientRect.top() += titleBar->getSize().y;
        }
    else
        titleBar = NULL;

    // Make the clientRect bounds relative to owner viewport
    clientRect.offset(bounds.topLeft);
    setBounds(bounds);
}

void MVWindow::trackMovement(MVEvent& event,MVRect& bounds)
/****************************************************************************
*
* Function:     MVWindow::trackMovement
* Parameters:   event   - Event starting the movement tracking
*               bounds  - Current bounding rectangle
*
* Description:  Tracks the movement of the bounding rectangle for the
*               window until the user releases the mouse button.
*
****************************************************************************/
{
    // Set move mouse cursor
    MS_setCursor(*MV_MOVE_CURSOR);

    // Draw initial rectangle around bounds
    drawDragRect(bounds);

    ibool   done = false;
    MVPoint oldLoc = event.where;
    do {
        getEvent(event);
        switch (event.what) {
            case evMouseMove:
                drawDragRect(bounds);       // Remove old rectangle
                bounds.offset(event.where.x - oldLoc.x,
                              event.where.y - oldLoc.y);
                drawDragRect(bounds);       // Draw new rectangle
                oldLoc = event.where;
                break;
            case evMouseUp:
                if (event.mouse.buttons & mbLeftButton) {
                    drawDragRect(bounds);   // Remove old rectangle
                    bounds.offset(event.where.x - oldLoc.x,
                                  event.where.y - oldLoc.y);
                    done = true;
                    }
                break;
            }
        } while (!done);

    // Restore cursor
    MS_setCursor(*MV_currentCursor);
}

void MVWindow::moveWindow(MVEvent& event)
/****************************************************************************
*
* Function:     MVWindow::moveWindow
* Parameters:   event   - Event triggering the move operation
*
* Description:  Moves the window on the desktop to a new location.
*
****************************************************************************/
{
    if (!(flags & wfMoveable))
        return;

    MVPoint orgLoc;
    MVRect  r = bounds;
    ibool   done = false;

    MV_GET_POINT(event.message.infoLong,orgLoc);
    do {
        getEvent(event);
        switch (event.what) {
            case evMouseMove: {
                int movex = event.where.x - orgLoc.x;
                int movey = event.where.y - orgLoc.y;
                if (ABS(movex) > 3 || ABS(movey) > 3) {
                    trackMovement(event,r.offset(movex,movey));
                    moveTo(r.left(),r.top());
                    done = true;
                    }
                } break;
            case evMouseUp:
                if (event.mouse.buttons & mbLeftButton)
                    done = true;
                break;
            }
        } while (!done);
    clearEvent(event);
}

void MVWindow::trackResize(MVEvent& event,MVRect& bounds)
/****************************************************************************
*
* Function:     MVWindow::trackResize
* Parameters:   event   - Event starting the movement tracking
*               bounds  - Current bounding rectangle
*
* Description:  Tracks the resizing of the bounding rectangle for the
*               window until the user releases the mouse button.
*
****************************************************************************/
{
    int width = frame->getWidth();

    // Draw initial rectangle around bounds
    drawDragRect(bounds,width);

    ibool   done = false;
    MVPoint oldLoc = event.where;
    do {
        getEvent(event);
        switch (event.what) {
            case evMouseMove:
                drawDragRect(bounds,width);     // Remove old rectangle
                frame->resizeDragRect(event.where.x - oldLoc.x,
                    event.where.y - oldLoc.y,bounds);
                drawDragRect(bounds,width);     // Draw new rectangle
                oldLoc = event.where;
                break;
            case evMouseUp:
                if (event.mouse.buttons & mbLeftButton) {
                    drawDragRect(bounds,width); // Remove old rectangle
                    frame->resizeDragRect(event.where.x - oldLoc.x,
                        event.where.y - oldLoc.y,bounds);
                    done = true;
                    }
                break;
            }
        } while (!done);
}

void MVWindow::resizeWindow(MVEvent& event)
/****************************************************************************
*
* Function:     MVWindow::resizeWindow
* Parameters:   event   - Event triggering the re-size operation
*
* Description:  Resizes the window if resizeable.
*
****************************************************************************/
{
    if (!(flags & wfResizeable))
        return;

    MVPoint orgLoc;
    MVRect  r = bounds;
    ibool   done = false;

    MV_GET_POINT(event.message.infoLong,orgLoc);
    do {
        getEvent(event);
        switch (event.what) {
            case evMouseMove: {
                int movex = event.where.x - orgLoc.x;
                int movey = event.where.y - orgLoc.y;
                if (ABS(movex) > 3 || ABS(movey) > 3) {
                    frame->resizeDragRect(movex,movey,r);
                    trackResize(event,r);
                    setBounds(r);
                    done = true;
                    }
                } break;
            case evMouseUp:
                if (event.mouse.buttons & mbLeftButton)
                    done = true;
                break;
            }
        } while (!done);
    clearEvent(event);
}

void MVWindow::minimiseWindow()
/****************************************************************************
*
* Function:     MVWindow::minimiseWindow
*
* Description:  Minimised the window to an icon on the desktop.
*
****************************************************************************/
{
    if (!(flags & wfResizeable))
        return;
/*  maximised = false;
    minimised = true;
    setBounds(minBounds);*/
}

void MVWindow::maximiseWindow()
/****************************************************************************
*
* Function:     MVWindow::maximiseWindow
*
* Description:  Maximises the window to the full size of the desktop
*               window.
*
****************************************************************************/
{
    if (!(flags & wfResizeable))
        return;
    maximised = true;
    minimised = false;

    // Set the bounding rectangle so that the client rectangle for the
    // window completely fills the owners client rectangle.
    MVRect extent(owner->getClientRect());
    MVRect maxBounds(0,0,extent.width(),extent.height());

    // Tuck the frame and title bar away outside of owners client rect
    if (frame) {
        extent = frame->inner();
        maxBounds.inflate(extent.left(),extent.top());
        }

    // We can optionally tuck the title bar away also, but for the moment
    // we do not do that.
//  if (titleBar) {
//      extent = titleBar->getBounds();
//      maxBounds.top() -= extent.height();
//      }
    setBounds(maxBounds);
}

void MVWindow::restoreWindow()
/****************************************************************************
*
* Function:     void MVWindow::restoreWindow
*
* Description:  Restore the window to the original position
*
****************************************************************************/
{
    if (!(flags & wfResizeable))
        return;
    minimised = maximised = false;
    setBounds(normalBounds);
}

void MVWindow::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVWindow::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the MVWindow class.
*
****************************************************************************/
{
    MVGroup::handleEvent(event,phase);

    switch (event.what) {
        case evCommand:
            switch (event.message.command) {
                case cmMove:
                    if (event.which == this)
                        moveWindow(event);
                    break;
                case cmResize:
                    if (event.which == this)
                        resizeWindow(event);
                    break;
                case cmMinimise:
                    if (event.which == this) {
                        if (minimised)
                            restoreWindow();
                        else
                            minimiseWindow();
                        }
                    break;
                case cmMaximise:
                    if (event.which == this) {
                        if (maximised)
                            restoreWindow();
                        else
                            maximiseWindow();
                        }
                    break;
                }
            break;
        case evBroadcast:
            // Handle the select window command here
            break;
        case evKeyDown:
        case evKeyAuto:
            if (event.key.charScan.scanCode == kbTab) {
                if (event.key.modifiers & mdShift)
                    selectPrev();
                else
                    selectNext();
                }
            break;
        }
}

void MVWindow::setBounds(const MVRect& newbounds)
/****************************************************************************
*
* Function:     MVWindow::setBounds
* Parameters:   newbounds   - New bounding rectangle for the view
*
* Description:  Sets the bounding rectangle for the window. We need to
*               adjust the size of the frame and menu bars to match the
*               new bounding information.
*
****************************************************************************/
{
    ibool sizeChanged = false;

    // Repaint the old location before re-size
    if ((state & sfExposed) && newbounds != bounds) {
        repaint();
        sizeChanged = true;
        }

    // Adjust bounding rectangle to new size
    MVGroup::setBounds(newbounds);
    MVRect extent;
    getExtent(extent);

    // Adjust frame and title bar to new size
    if (frame)  {
        frame->setBounds(extent);
        clientRect = frame->inner();
        }
    if (titleBar) {
        titleBar->setBounds(clientRect);
        clientRect.top() = titleBar->getBounds().botRight.y;
        }
    // Make the clientRect bounds relative to owner viewport
    clientRect.offset(bounds.topLeft);

    // Save the normal bounds so we can restore it properly
    if (!maximised && !minimised)
        normalBounds = bounds;

    // Repaint the new location after re-size if size has changed
    if (sizeChanged)
        repaint();
}

void MVWindow::moveTo(int x,int y)
/****************************************************************************
*
* Function:     MVWindow::moveTo
* Parameters:   x,y - New position to move view to
*
****************************************************************************/
{
    MVRect  update(bounds),clip;
    ibool   bitBlt = false;

    if (state & sfExposed) {
        // Repaint old location and blt the contents of the old window to
        // the new location. If the window is currently off the edge of
        // the display (and hence being clipped) then we dont bitBlt
        // the window contents.
        repaint();
        dc.getClipRect(clip);
        update &= clip;
        if (update == bounds) {
            MS_obscure();
            dc.bitBlt(dc,update,x,y,MGL_REPLACE_MODE);
            MS_show();
            bitBlt = true;
            }
        }

    // Move the view to new location
    MVPoint oldTopLeft(bounds.topLeft);
    MVGroup::moveTo(x,y);
    normalBounds.offset(bounds.topLeft.x - oldTopLeft.x,
                        bounds.topLeft.y - oldTopLeft.y);

    if (state & sfExposed) {
        // The window was initially off the screen, so simply post a repaint
        // for the entire window. If we performed a blt operation, then
        // we can also remove the area covered by the new window from the
        // pending repaint event.
        if (!bitBlt)
            repaint();
        else unrepaint();
        }
}

void MVWindow::drawBackground(const MVRect&)
/****************************************************************************
*
* Function:     MVWindow::drawBackground
* Parameters:   clip    - Clipping rectangle to use when drawing
*
* Description:  Draws a solid background for the window in the background
*               color.
*
****************************************************************************/
{
    dc.setColor(getColor(scBackground));
    dc.fillRect(clientRect.offsetBy(-bounds.left(),-bounds.top()));
}

MVFrame* MVWindow::initFrame(MGLDevCtx& dc,const MVRect& bounds,int width,
    uint flags)
/****************************************************************************
*
* Function:     MVWindow::initFrame
* Parameters:   bounds  - Bounding box for the frame
* Returns:      Pointer to the allocated frame.
*
****************************************************************************/
{
    return new MVFrame(dc,bounds,width,flags);
}

MVTitleBar* MVWindow::initTitleBar(MGLDevCtx& dc,const MVRect& bounds,
    const char *title,MVWindow *owner,uint flags)
/****************************************************************************
*
* Function:     MVWindow::initTitleBar
* Parameters:   bounds  - Bounding box for the title bar
*               title   - Title text for the title bar
*               flags   - Window flags
* Returns:      Pointer to the allocated title bar.
*
****************************************************************************/
{
    return new MVTitleBar(dc,bounds,title,owner,flags);
}
