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
* Description:  Member functions for the MVView class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mview.hpp"
#include "mvis/mgroup.hpp"

/*----------------------------- Implementation ----------------------------*/

MVView  *MVView::capturedView = NULL;

MVView::MVView(MGLDevCtx& dc,const MVRect& bounds)
    : dc(dc), owner(NULL), state(sfVisible), options(ofClipToClient),
      dragMode(0), growMode(0), helpCtx(hcNoContext)
/****************************************************************************
*
* Function:     MVView::MVView
* Parameters:   bounds  - Bounding box for the view
*
* Description:  Constructor for the view, given a bounding box.
*
****************************************************************************/
{
    setBounds(bounds);
}

MVView::~MVView()
/****************************************************************************
*
* Function:     MVView::~MVView
*
* Description:  Destructor for a view. If the view has an owner, it
*               removes the view from the owner's group.
*
****************************************************************************/
{
    if (owner != NULL)
        owner->remove(this);
}

void MVView::drawRect(int left,int top,int right,int bottom,int width)
/****************************************************************************
*
* Function:     MVView::drawRect
* Parameters:   left    - Left coordinate of rectangle to draw
*               top     - Top coordinate of rectangle to draw
*               right   - Right coordinate of rectangle to draw
*               bottom  - Bottom coordinate of rectangle to draw
*               width   - Width of the rectangle (in sysLineWidth dimensions)
*
* Description:  Draws a rectangle using the current system line width. The
*               rectangle is completely contained within the bounding
*               rectangle, which is different to the way that the MGL draws
*               rectangles with large line widths.
*
****************************************************************************/
{
    int     w,h;

    dc.getPenSize(w,h);
    dc.setPenSize(width,width);         // Adjust pen size
    bottom -= width-1;                  // Compensate rectangle size
    right -= width-1;
    dc.rect(left,top,right,bottom);     // Draw the frame
    dc.setPenSize(w,h);
}

void MVView::drawFrame(int left,int top,int right,int bottom,ibool inset)
/****************************************************************************
*
* Function:     MVView::drawFrame
* Parameters:   r       - Rectangle to draw frame inside of
*               inset   - True if frame should be inset, false if outset
*
* Description:  Draws a standard 3d frame in the standard system colors.
*
****************************************************************************/
{
    dc.setColor(getColor(inset ? scShadow : scMediumLight));
    dc.line(left,top,left,bottom-1);
    dc.line(left,top,right-1,top);
    dc.setColor(getColor(inset ? scDarkShadow : scHighlight));
    dc.line(left+1,top+1,left+1,bottom-2*1);
    dc.line(left+1,top+1,right-2*1,top+1);
    dc.setColor(getColor(inset ? scMediumLight : scShadow));
    dc.line(right-2*1,top+1,right-2*1,bottom-2*1);
    dc.line(left+1,bottom-2*1,right-2*1,bottom-2*1);
    dc.setColor(getColor(inset ? scHighlight : scDarkShadow));
    dc.line(right-1,top,right-1,bottom-1);
    dc.line(left,bottom-1,right-1,bottom-1);
}

void MVView::drawFocusRect(int left,int top,int right,int bottom,int color)
/****************************************************************************
*
* Function:     MVView::drawFocusRect
* Parameters:   r       - Rectangle to draw
*               color   - Color to draw with
*
* Description:  Draws a focus rectangle in the specified color.
*
****************************************************************************/
{
    int oldPenStyle = dc.getPenStyle();
    dc.setColor(getColor(color));
    dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
    dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
    dc.usePenBitmapPattern(0);
    dc.rect(left,top,right,bottom);
    dc.setPenStyle(oldPenStyle);
}

void MVView::drawDragRect(int left,int top,int right,int bottom,int width)
/****************************************************************************
*
* Function:     MVView::drawDragRect
* Parameters:   r   - Rectangle to draw
*
* Description:  Draws a drag rectangle frame at the specified location
*               with the specified width. The drag rect is drawn in XOR
*               mode with the GRAY bitmap pattern with a color selected to
*               invert the current color. Note also that we obscure and
*               show the mouse cursor during this routine as it is called
*               during event handling code.
*
****************************************************************************/
{
    MS_obscure();
    int oldPenStyle = dc.getPenStyle();
    int oldWriteMode = dc.getWriteMode();
    dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
    dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
    dc.usePenBitmapPattern(0);
    dc.setWriteMode(MGL_XOR_MODE);
    if (dc.getBitsPerPixel() <= 8)
        dc.setColor(0xFF);
    else dc.setColorRGB(0xFF,0xFF,0xFF);
    drawRect(left,top,right,bottom,width);
    dc.setPenStyle(oldPenStyle);
    dc.setWriteMode(oldWriteMode);
    MS_show();
}

void MVView::drawLine(int left,int top,int right,int bottom,int width)
/****************************************************************************
*
* Function:     MVView::drawLine
* Parameters:   left    - Left coordinate of rectangle to draw
*               top     - Top coordinate of rectangle to draw
*               right   - Right coordinate of rectangle to draw
*               bottom  - Bottom coordinate of rectangle to draw
*               width   - Width of the rectangle (in sysLineWidth dimensions)
*
* Description:  Draws a line with the specified with adjusted to the current
*               system line width units. The line will be placed below and
*               to the right of the coordinates which is the default for
*               the MGL.
*
****************************************************************************/
{
    int w,h;
    dc.getPenSize(w,h);
    dc.setPenSize(width,width);         // Adjust pen size
    dc.line(left,top,right,bottom);     // Draw the line
    dc.setPenSize(w,h);
}

void MVView::drawHotStr(const MVPoint& start,const MVPoint& hot1,
    const MVPoint& hot2,ibool hotChar,const MVPoint& offset,ibool depressed,
    const char *str,ibool disabled,color_t textColor)
/****************************************************************************
*
* Function:     MVView::drawHotStr
* Parameters:   start       - Starting coordinate for string
*               hot1,hot2   - Hot character underline location
*               hotChar     - True if string has hot character underline
*               offset      - Amount to offset text by
*               depressed   - True if text is depressed (ie: button down)
*               str         - String to draw
*               disabled    - True if string should be drawn disabled
*               textColor   - Color to draw the text in
*
* Description:  Draws a line with the specified with adjusted to the current
*               system line width units. The line will be placed below and
*               to the right of the coordinates which is the default for
*               the MGL.
*
****************************************************************************/
{
    if (!depressed && disabled) {
        dc.setColor(getColor(scHighlight));
        dc.drawStr(start.x + offset.x + 1,
                   start.y + offset.y + 1,str);
        if (hotChar) {
            drawLine(hot1.x + offset.x + 1,
                     hot1.y + offset.y + 1,
                     hot2.x + offset.x + 1,
                     hot2.y + offset.y + 1);
            }
        }
    dc.setColor(textColor);
    dc.drawStr(start.x + offset.x + depressed,
               start.y + offset.y + depressed,str);
    if (hotChar) {
        drawLine(hot1.x + offset.x + depressed,
                 hot1.y + offset.y + depressed,
                 hot2.x + offset.x + depressed,
                 hot2.y + offset.y + depressed);
        }
}

MVView *MVView::clipAndDrawView;

void MVView::_clipAndDraw(const rect_t *r)
{
    clipAndDrawView->clipRect = *r;
    clipAndDrawView->dc.setClipRect(*r);
    clipAndDrawView->draw(*r);
}

static void _ASMAPI clipAndDraw(const rect_t *r)
{ MVView::_clipAndDraw(r); }

void MVView::paint()
/****************************************************************************
*
* Function:     MVView::paint
*
* Description:  This function paints the view with the current clip
*               region. The default action is simply to call the draw()
*               function for every clip rectangle in the region.
*
****************************************************************************/
{
    clipAndDrawView = this;
    clipRgn.traverse(clipAndDraw);
}

void MVView::setClipRect(const MVRect& clip)
/****************************************************************************
*
* Function:     MVView::setClipRect
*
* Description:  Sets the clipping rectangle for the view. This function
*               should be used within the draw() procedure if the class
*               needs to change the clipping rectangle with the draw
*               function.
*
****************************************************************************/
{
    dc.setClipRect(clipRect & clip);
}

void MVView::setupViewport()
/****************************************************************************
*
* Function:     MVView::setupViewport
*
* Description:  Sets up the viewport to that of the inner part of the
*               view, for drawing objects within the view and for
*               simplified event processing.
*
****************************************************************************/
{
    MV_pushViewport(dc);
    dc.setRelViewport(bounds);
}

void MVView::setupOwnerViewport()
/****************************************************************************
*
* Function:     MVView::setupOwnerViewport
*
* Description:  Sets the viewport to the owner's viewport, regardless of
*               what the current viewport is already set to. This requires
*               traversing the view tree, so is slower than the above
*               routine but can be used to draw something in a view at
*               any time (and hence must be used with care).
*
****************************************************************************/
{
    MV_pushViewport(dc);

    if (owner) {
        MVRect  view;
        MVPoint d;

        owner->findGlobalDelta(d);
        owner->getExtent(view);
        view.offset(d);
        dc.setViewport(view);
        }
    else
        dc.setViewport(bounds);
}

void MVView::findGlobalDelta(MVPoint& d)
/****************************************************************************
*
* Function:     MVView::findGlobalDelta
* Parameters:   d   - Place to store the delta coordinate
*
* Description:  Computes the delta value to convert a global coordinate
*               to a local coordinate (or vice versa), by traversing
*               the currently active view tree. This is different
*               to using the globalToLocal()/localToGlobal() calls, as
*               these calls assume that the current viewport is set to
*               that containing the view.
*
****************************************************************************/
{
    if (owner) {
        MVPoint newd;
        owner->findGlobalDelta(newd);
        d = newd;
        }
    else d.x = d.y = 0;
    d += bounds.topLeft;
}

MVView *MVView::findModalView()
/****************************************************************************
*
* Function:     MVView::findModalView
* Returns:      Pointer to the currently active modal view (NULL if none).
*
* Description:  By default MVView's cannot be a modal view.
*
****************************************************************************/
{
    return NULL;
}

void MVView::idle()
/****************************************************************************
*
* Function:     MVView::idle
*
* Description:  Routine called when nothing is happening. By default we
*               do nothing.
*
****************************************************************************/
{
}

MGLCursor *MVView::getCursor(const MVPoint&)
/****************************************************************************
*
* Function:     MVView::getCursor
* Returns:      Pointer to the cursor definition for the view.
*
* Description:  We return NULL, which means use the normal cursor defintion
*               by default. If this is overloaded to return a pointer
*               to another cursor definition, the cursor will be
*               changed to this when the cursor is within the bounds
*               of the view.
*
****************************************************************************/
{
    return NULL;
}

ibool MVView::valid(ulong)
/****************************************************************************
*
* Function:     MVView::valid
* Parameters:   command - cmValid, or command ending the modal view
* Returns:      True if the view is valid
*
****************************************************************************/
{
    return true;
}

void MVView::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     MVView::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Default view event handling mechanism. If the event is
*               a mouse down event with the left button, and the view
*               is selectable, then the view is selected.
*
****************************************************************************/
{
    if (event.what == evMouseDown && (event.mouse.buttons & mbLeftButton)
            && !(state & (sfFocused | sfDisabled))
            && (options & ofSelectable)) {
        // The view is selectable but not currently selected or disabled,
        // so select the view.
        if (options & ofSelectFront)
            selectToFront();
        else
            select();

        // If the ofFirstClick flag is not set, then we clear the event
        // so that it will not be handled by the view as a mouse down.
        if (!(options & ofFirstClick))
            clearEvent(event);
        }
}

void MVView::putEvent(MVEvent& event)
/****************************************************************************
*
* Function:     MVView::putEvent
* Parameters:   event   - Event to post
*
* Description:  Simply calls the owing viewing to store the event. If this
*               is not overridden by any class along the way, it eventually
*               calls TProgram::putEvent() which will simply store it as
*               the next pending event. It will _not_ be stored in the
*               event queue proper, you will need to call
*               eventQueue.post() to do this (for macro expansion etc).
*
****************************************************************************/
{
    if (owner)
        owner->putEvent(event);
}

ibool MVView::getEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVView::getEvent
* Parameters:   event   - Place to store the event
*               mask    - Event mask for obtaining events
* Returns:      True if an event was pending, false if not.
*
* Description:  Simply calls the owner to get the event. If this is not
*               overridden by any class along the way, it eventually gets
*               back to TProgram::getEvent which will return the next
*               pending event (it will always look for the event posted
*               with putEvent() first, then looks in the event queue).
*
****************************************************************************/
{
    if (owner)
        return owner->getEvent(event,mask);
    return false;
}

ibool MVView::peekEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVView::peekEvent
* Parameters:   event   - Place to store the event
*               mask    - Event mask for obtaining events
* Returns:      True if an event is pending, false if not.
*
* Description:  Simply calls the owner to peek the event. If this is not
*               overridden by any class along the way, it eventually gets
*               back to TProgram::getEvent which will return the next
*               pending event (it will always look for the event posted
*               with putEvent() first, then looks in the event queue).
*
*               The event is not removed if it was pending.
*
****************************************************************************/
{
    if (owner)
        return owner->peekEvent(event,mask);
    return false;
}

void MVView::show()
/****************************************************************************
*
* Function:     MVView::show
*
* Description:  Makes the view visible, by setting the visible flag.
*
****************************************************************************/
{
    if (!(state & sfVisible))
        setState(sfVisible,true);
}

void MVView::hide()
/****************************************************************************
*
* Function:     MVView::hide
*
* Description:  Makes the view not visible, by resetting the visible flag.
*
****************************************************************************/
{
    if (state & sfVisible)
        setState(sfVisible,false);
}

void MVView::enable()
/****************************************************************************
*
* Function:     MVView::enable
*
* Description:  Enables the view to accept events.
*
****************************************************************************/
{
    if (state & sfDisabled)
        setState(sfDisabled,false);
}

void MVView::disable()
/****************************************************************************
*
* Function:     MVView::disable
*
* Description:  Disables the view from accepting events, and will cause the
*               visual style of it to be updated.
*
****************************************************************************/
{
    if (!(state & sfDisabled))
        setState(sfDisabled,true);
}

void MVView::setBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVView::setBounds
* Parameters:   bounds  - New bounding rectangle for the view
*
* Description:  Sets the bounding rectangle for the view.
*
****************************************************************************/
{
    MVView::bounds = bounds;
    visRgn = bounds;
    size = bounds.botRight - bounds.topLeft;
}

void MVView::moveTo(int x,int y)
/****************************************************************************
*
* Function:     MVView::moveTo
* Parameters:   x,y - New position to move view to
*
****************************************************************************/
{
    bounds.moveTo(x,y);     // Move to new location
    visRgn.moveTo(x,y);
}

void MVView::invalidRect(MVRect& rect)
/****************************************************************************
*
* Function:     MVView::invalidRect
* Parameters:   rect    - Rectangle to invalidate (modified!)
*               global  - True if repaint event should be performed globally
*
* Description:  Invalidates the specified rectangle, requiring it to be
*               refreshed. If the view has an owner, we do a recursive
*               call to the owner with the rectangle moved relative to the
*               owner's viewport. Eventually this will get to the
*               TProgram view (hopefully), which will save the invalid
*               rectangle for the pending repaint event (posted by
*               the TProgram object).
*
****************************************************************************/
{
    if (owner && !(state & sfLockRepaint) && (state & sfVisible)) {
        rect.offset(bounds.topLeft);
        owner->invalidRect(rect);
        }
}

void MVView::repaint(const MVRect& rect)
/****************************************************************************
*
* Function:     MVView::repaint
* Parameters:   rect    - Rectangle to repaint
*
* Description:  Simply invalidates the specified rectangle. Note that the
*               invalidRect routine must be able to modify the rectangle
*               so we create a local copy.
*
****************************************************************************/
{
    MVRect r(rect);
    invalidRect(r);
}

void MVView::validRect(MVRect& rect)
/****************************************************************************
*
* Function:     MVView::validRect
* Parameters:   rect    - Rectangle to validate (modified)
*
* Description:  Validates the specified rectangle, removing it from the
*               pending repaint event (if any). This function is used when
*               a view generates a repaint event by knows that the view
*               is just about to be drawn so will handle the repaint event
*               locally. If the view has an owner, we do a recursive call
*               to the owner with the rectangle moved relative to the
*               owner's viewport. Eventually this will get to the
*               TProgram view (hopefully), which will remove the valid
*               rectangle from the pending repaint event.
*
****************************************************************************/
{
    if (owner && !(state & sfLockRepaint) && (state & sfVisible)) {
        rect.offset(bounds.topLeft);
        owner->validRect(rect);
        }
}

void MVView::unrepaint(const MVRect& rect)
/****************************************************************************
*
* Function:     MVView::unrepaint
* Parameters:   rect    - Rectangle to remove from repaint region
*
* Description:  Simple invalidates the specified rectangle. Note that the
*               validRect routine must be able to modify the rectangle
*               so we create a local copy.
*
****************************************************************************/
{
    MVRect r(rect);
    validRect(r);
}

ibool MVView::includes(const MVPoint& p) const
/****************************************************************************
*
* Function:     MVView::includes
* Parameters:   p   - Point to check for inclusion
* Returns:      True if the point is within the view's bounds
*
* Description:  Determines if the 'global' coordinate p is within the
*               bounds of the view. It is assume by this stage that the
*               current viewport is set to that of the owner.
*
****************************************************************************/
{
    MVPoint temp(p);
    globalToLocal(temp);
    ibool result = bounds.includes(temp);
    return result;
}

void MVView::setState(uint aState,ibool set)
/****************************************************************************
*
* Function:     MVView::setState
* Parameters:   aState  - State flag/s to set
*               set     - True if flag should be set, false if cleared
*
****************************************************************************/
{
    // Disabled views cannot become active
    if ((aState & sfActive) && (state & sfDisabled))
        return;

    if (set)
        state |= aState;
    else
        state &= ~aState;

    if (owner) {
        if (aState & sfVisible) {
            if (owner->state & sfExposed)
                setState(sfExposed,set);
            }
        if (aState & sfFocused) {
            MV_message(owner,evBroadcast,
                (set ? cmReceivedFocus : cmReleasedFocus), this);
            }
        if (aState & (sfActive | sfFocused | sfDisabled | sfExposed))
            repaint();
        }
}

void MVView::setOption(uint aOption,ibool set)
/****************************************************************************
*
* Function:     MVView::setOption
* Parameters:   aOption - Option flag/s to set
*               set     - True if flag should be set, false if cleared
*
****************************************************************************/
{
    if (set)
        options |= aOption;
    else
        options &= ~aOption;
}

void MVView::select()
/****************************************************************************
*
* Function:     MVView::select
*
* Description:  Selects the view by asking the views owner to select it.
*
****************************************************************************/
{
    if (owner != NULL)
        owner->select(this);
}

void MVView::selectToFront()
/****************************************************************************
*
* Function:     MVGroup::selectToFront
* Parameters:   view    - View to select
* Returns:      Selects the view and moves it to the front of the list
*
****************************************************************************/
{
    MV_postMessage(NULL,evCommand,cmMoveToFront,this);
}

color_t MVView::getColor(int index)
/****************************************************************************
*
* Function:     MVView::getColor
* Parameters:   index   - Index of the color to map
* Returns:      Actual color value for the current video mode
*
* Description:  Gets the proper color value for drawing a particular item.
*               If the value is positive we assume it is a standard
*               system color index. If it is negative, we assume it is a
*               specific color index for the current color palette and
*               simply pass it back directly (after mapping with
*               dc.realColor() first).
*
****************************************************************************/
{
    if (index < 0)
        return dc.realColor(index);
    else return dc.realColor(MV_getSystemColor(index));
}

uint MVView::getHelpCtx() const
/****************************************************************************
*
* Function:     MVView::getHelpCtx
*
* Description:  Returns the help context number for the view.
*
****************************************************************************/
{
    return helpCtx;
}
