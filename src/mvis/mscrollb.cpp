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
* Description:  Member functions for the MVScrollBar class hierachy. You may
*               make the scroll bar's as wide as you wish, but the standard
*               scroll bar width is 14 pixels.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mscrollb.hpp"
#include "mvis/mgroup.hpp"

/*----------------------------- Implementation ----------------------------*/

MVScrollBar::MVScrollBar(MGLDevCtx &dc,const MVRect& bounds)
    : MVView(dc,bounds), flags(0)
/****************************************************************************
*
* Function:     MVScrollBar::MVScrollBar
* Parameters:   bounds  - Bounding box for the scroll bar
*
****************************************************************************/
{
    minVal = maxVal = value = 0;
    pageStep = arrowStep = 1;

    minVal = 100;
    maxVal = 200;
    value = 150;
    setBounds(bounds);
}

void MVScrollBar::moveTo(int x,int y)
/****************************************************************************
*
* Function:     MVScrollBar::moveTo
* Parameters:   x,y - Position to move the scroll bar to
*
****************************************************************************/
{
    MVView::moveTo(x,y);
    setBounds(bounds);
}

void MVScrollBar::setBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVScrollBar::setBounds
* Parameters:   bounds  - New bounding box for the scroll bar
*
* Description:  Changes the bounds for the scroll bar.
*
****************************************************************************/
{
    MVView::setBounds(bounds);
    vertical = (size.x < size.y);

    // Compute the location of the left and right arrow buttons
    leftArrow = bounds;
    leftArrow.inset(1,1);
    rightArrow = leftArrow;

    if (vertical) {
        leftArrow.bottom() = leftArrow.top()+(_MV_sysScrollBarWidth+1);
        rightArrow.top() = rightArrow.bottom()-(_MV_sysScrollBarWidth+1);
        }
    else {
        leftArrow.right() = leftArrow.left()+(_MV_sysScrollBarWidth+1);
        rightArrow.left() = rightArrow.right()-(_MV_sysScrollBarWidth+1);
        }

    // Compute the location of the thumb
    moveThumb();
}

void MVScrollBar::moveThumb()
/****************************************************************************
*
* Function:     MVScrollBar::moveThumb
*
* Description:  Computes the current location of the thumb, given the
*               current value of the scroll bar.
*
****************************************************************************/
{
    int left,right;

    if (vertical) {
        thumb.left() = leftArrow.left();
        thumb.right() = leftArrow.right();
        left = thumb.top() = leftArrow.bottom();
        right = thumb.bottom() = rightArrow.top();
        }
    else {
        thumb.top() = leftArrow.top();
        thumb.bottom() = leftArrow.bottom();
        left = thumb.left() = leftArrow.right();
        right = thumb.right() = rightArrow.left();
        }

    if (maxVal == minVal)
        return;

    left += ((right-left-(_MV_sysScrollBarWidth+1)) * (long)(value - minVal))
                    / (long)(maxVal-minVal);
    right = left + (_MV_sysScrollBarWidth+1);

    if (vertical) {
        thumb.top() = left;
        thumb.bottom() = right;
        }
    else {
        thumb.left() = left;
        thumb.right() = right;
        }
}

int MVScrollBar::getPartHit(const MVPoint& global)
/****************************************************************************
*
* Function:     MVScrollBar::getPartHit
* Parameters:   global  - Point to test for inclusion (global coords)
* Returns:      Code of part hit, -1 if none hit
*
* Description:  Determines which part of the scroll bar was hit.
*
****************************************************************************/
{
    MVPoint p(global);
    globalToLocal(p);

    if (bounds.includes(p)) {
        if (leftArrow.includes(p))
            return sbLeftArrow;
        if (rightArrow.includes(p))
            return sbRightArrow;

        if (thumb.includes(p))
            return sbThumb;

        if (vertical) {
            if (leftArrow.bottom() <= p.y && p.y < thumb.top())
                return sbPageLeft;
            if (thumb.bottom() <= p.y && p.y < rightArrow.top())
                return sbPageRight;
            }
        else {
            if (leftArrow.right() <= p.x && p.x < thumb.left())
                return sbPageLeft;
            if (thumb.right() <= p.x && p.x < rightArrow.left())
                return sbPageRight;
            }
        }

    return -1;
}

void MVScrollBar::changeValue(int part)
/****************************************************************************
*
* Function:     MVScrollBar::changeValue
* Parameters:   part    - Scroll bar arrow hit
*
* Description:  Adjusts the scroll bar value depending on which part of
*               the scroll bar was hit.
*
****************************************************************************/
{
    int oldValue = value;

    switch (part) {
        case sbLeftArrow:
            value -= arrowStep;
            break;
        case sbRightArrow:
            value += arrowStep;
            break;
        case sbPageLeft:
            value -= pageStep;
            break;
        case sbPageRight:
            value += pageStep;
            break;
        }
    if (value < minVal) value = minVal;
    if (value > maxVal) value = maxVal;

    if (oldValue != value) {
        moveThumb();
        drawThumb(part);

        // We need to redraw the opposite arrow if the arrow just became
        // active again.
        if (oldValue == minVal)
            drawLeftArrow(false);
        if (oldValue == maxVal)
            drawRightArrow(false);

        MV_message(owner,evBroadcast,cmScrollBarChanged,this);
        }
}

void MVScrollBar::changeValue(const MVPoint& global)
/****************************************************************************
*
* Function:     MVScrollBar::changeValue
* Parameters:   global  - New location of the mouse cursor
*
* Description:  Converts the mouse cursor location into a new value for
*               the scroll bar thumb, repositions the thumb and redraws
*               it.
*
****************************************************************************/
{
    MVPoint p(global);
    globalToLocal(p);

    int     left,right,start;
    int oldValue = value;

    if (vertical) {
        start = p.y - 7;
        left = leftArrow.bottom();
        right = rightArrow.top();
        }
    else {
        start = p.x - 7;
        left = leftArrow.right();
        right = rightArrow.left();
        }

    if (start < left)       start = left;
    if (start > right-(_MV_sysScrollBarWidth+1))
        start = right-(_MV_sysScrollBarWidth+1);

    value = ((2*(maxVal-minVal)*(long)(start-left))
            / (right-left-(_MV_sysScrollBarWidth+1))+1)/2;

    if (oldValue != value) {
        moveThumb();
        drawThumb(sbThumb);
        MV_message(owner,evBroadcast,cmScrollBarChanged,this);
        }
}

void MVScrollBar::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVScrollBar::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for scroll bars.
*
****************************************************************************/
{
    MVView::handleEvent(event,phase);

    flags |= sbInteracting;

    if (event.what == evMouseDown) {
        // Let the owning view know that the scroll bar has been clicked,
        // so that any associated lists etc can select themselves and
        // prepare for scroll bar changed events.
        MV_message(owner,evBroadcast,cmScrollBarClicked,this);
        MVEvent e;
        getEvent(e,evRepaint);              // Force repaint
        dc.setClipRect(bounds);

        ibool   down;
        int part = getPartHit(event.where);
        if (part == -1)
            goto QuickExit;

        if ((part == (int)sbLeftArrow && value == minVal) ||
            (part == (int)sbRightArrow && value == maxVal) ||
            (minVal == maxVal)) {
            MV_beep();
            goto QuickExit;
            }

        drawPart(part,down = true);
        while (event.what != evMouseUp) {
            switch (event.what) {
                case evMouseDown:
                case evMouseAuto:
                    if (part == getPartHit(event.where))
                        changeValue(part);
                    break;
                case evMouseMove:
                    if (part != (int)sbThumb) {
                        if (down != (part == getPartHit(event.where)))
                            drawPart(part,down = !down);
                        }
                    else changeValue(event.where);
                    break;
                }
            if (part != (int)sbThumb && (value == minVal || value == maxVal))
                break;
            getEvent(event);
            }
        if (down)
            drawPart(part,false);

        // Redraw the arrows if they have changed state during the
        // interaction, and we were adjusting the paging state.
        if (part == (int)sbPageLeft || part == (int)sbPageRight || part == (int)sbThumb) {
            if (value == minVal)
                drawLeftArrow(false);
            else if (value == maxVal)
                drawRightArrow(false);
            }

QuickExit:
        clearEvent(event);
        }

    flags &= ~sbInteracting;
}

void MVScrollBar::drawPart(int part,ibool down)
/****************************************************************************
*
* Function:     MVScrollBar::drawPart
* Parameters:   part    - Code of the part to draw
*               down    - True if part is down
*
* Description:  Draws the part in the state 'down'.
*
****************************************************************************/
{
    switch (part) {
        case sbLeftArrow:
            drawLeftArrow(down);
            break;
        case sbRightArrow:
            drawRightArrow(down);
            break;
        case sbPageLeft:
        case sbPageRight:
        case sbThumb:
            drawThumb(down ? part : -1);
            break;
        }
}

void MVScrollBar::drawLeftArrow(ibool down)
/****************************************************************************
*
* Function:     MVScrollBar::drawLeftArrow
* Parameters:   down    - True if arrow is pressed down
*
* Description:  Draw's the left/top arrow portion of the scroll bar. Assumes
*               the outer border for the scroll bar exists, and the
*               border colors are set up.
*
****************************************************************************/
{
    MS_obscure();
    dc.setColor(getColor(scScrollBarInterior));
    dc.fillRect(leftArrow);
    MV_drawBorder(leftArrow,down ? MV_BDR_INSET : MV_BDR_OUTSET,1);
    dc.setColor(getColor(
        value > minVal ? scActiveScrollArrow : scInactiveScrollArrow));

    int cx = down + (leftArrow.left() + leftArrow.right()-1)/2;
    int cy = down + (leftArrow.top() + leftArrow.bottom()-1)/2;

    if (vertical) {
        // Draw the top arrow

        dc.line(cx-3,cy+1,cx,cy-2);
        dc.line(cx+1,cy-2,cx+4,cy+1);
        dc.line(cx-2,cy+1,cx,cy-1);
        dc.line(cx+1,cy-1,cx+3,cy+1);
        }
    else {
        // Draw the left arrow

        dc.line(cx+1,cy-3,cx-2,cy);
        dc.line(cx-2,cy+1,cx+1,cy+4);
        dc.line(cx+1,cy-2,cx-1,cy);
        dc.line(cx-1,cy+1,cx+1,cy+3);
        }
    MS_show();
    flags &= ~sbDirtyLeft;
}

void MVScrollBar::drawRightArrow(ibool down)
/****************************************************************************
*
* Function:     MVScrollBar::drawRightArrow
* Parameters:   down    - True if arrow is pressed down
*
* Description:  Draw's the right/bottom arrow portion of the scroll bar. Assume
*               the outer border for the scroll bar exists, and the
*               border colors are set up.
*
****************************************************************************/
{
    MS_obscure();
    dc.setColor(getColor(scScrollBarInterior));
    dc.fillRect(rightArrow);
    MV_drawBorder(rightArrow,down ? MV_BDR_INSET : MV_BDR_OUTSET,1);
    dc.setColor(getColor(
        value < maxVal ? scActiveScrollArrow : scInactiveScrollArrow));

    int cx = down + (rightArrow.left() + rightArrow.right()-1)/2;
    int cy = down + (rightArrow.top() + rightArrow.bottom()-1)/2;

    if (vertical) {
        // Draw the bottom arrow

        dc.line(cx-3,cy-1,cx,cy+2);
        dc.line(cx+1,cy+2,cx+4,cy-1);
        dc.line(cx-2,cy-1,cx,cy+1);
        dc.line(cx+1,cy+1,cx+3,cy-1);
        }
    else {
        // Draw the right arrow

        dc.line(cx-1,cy-3,cx+2,cy);
        dc.line(cx+2,cy+1,cx-1,cy+4);
        dc.line(cx-1,cy-2,cx+1,cy);
        dc.line(cx+1,cy+1,cx-1,cy+3);
        }
    MS_show();
    flags &= ~sbDirtyRight;
}

void MVScrollBar::drawThumb(int which)
/****************************************************************************
*
* Function:     MVScrollBar::drawThumb
* Parameters:   which   - Which part of bar to highlight.
*
* Description:  Draws the thumb portion of the scroll bar, highlighting
*               the specific portion of the scroll bar.
*
****************************************************************************/
{
    MS_obscure();
    if (maxVal == minVal) {
        dc.setColor(getColor(scScrollBarInterior));
        dc.fillRect(thumb);
        }
    else {
        attributes_t attr;
        dc.getAttributes(attr);

        dc.setColor(getColor(scScrollBarInterior));
        dc.setPenStyle(MGL_BITMAP_OPAQUE);
        dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
        dc.usePenBitmapPattern(0);

        if (vertical) {
            dc.setBackColor(getColor(
                which == (int)sbPageLeft ? scSelectScrollBar : scUnselectScrollBar));
            dc.fillRect(thumb.left(),leftArrow.bottom(),
                thumb.right(),thumb.top());
            dc.setBackColor(getColor(
                which == (int)sbPageRight ? scSelectScrollBar : scUnselectScrollBar));
            dc.fillRect(thumb.left(),thumb.bottom(),
                thumb.right(),rightArrow.top());
            }
        else {
            dc.setBackColor(getColor(
                which == (int)sbPageLeft ? scSelectScrollBar : scUnselectScrollBar));
            dc.fillRect(leftArrow.right(),thumb.top(),
                thumb.left(),thumb.bottom());
            dc.setBackColor(getColor(
                which == (int)sbPageRight ? scSelectScrollBar : scUnselectScrollBar));
            dc.fillRect(thumb.right(),thumb.top(),
                rightArrow.left(),thumb.bottom());
            }

        dc.setPenStyle(MGL_BITMAP_SOLID);
        dc.fillRect(thumb);
        MV_drawBorder(thumb,(which == (int)sbThumb ? MV_BDR_INSET : MV_BDR_OUTSET),1);

        dc.restoreAttributes(attr);
        }
    MS_show();

    flags &= ~sbDirtyThumb;
}

void MVScrollBar::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVScrollBar::draw
* Parameters:   clip    - Clipping rectangle to use when drawing
*
* Description:  Draws the scroll bar in the current state.
*
****************************************************************************/
{
    if (flags & sbInteracting)
        return;

    // Draw the main body of the scroll bar inset into the screen
    MS_obscure();
    MV_setBorderColors(getColor(scHighlight),getColor(scShadow));
    MV_drawBorder(bounds,MV_BDR_INSET,1);

    drawLeftArrow(false);
    drawRightArrow(false);
    drawThumb(-1);
    MS_show();
}

void MVScrollBar::update()
/****************************************************************************
*
* Function:     MVScrollBar::update
*
* Description:  Updates the state of the scroll bar thumb, if the scroll
*               bar is visible.
*
****************************************************************************/
{
    if ((flags & sbInteracting) || !(flags & sbDirty))
        return;

    if ((state & sfVisible) && (state & sfExposed)) {
        setupOwnerViewport();
        MS_obscure();
        if (flags & sbDirtyLeft)
            drawLeftArrow(false);
        if (flags & sbDirtyRight)
            drawRightArrow(false);
        if (flags & sbDirtyThumb)
            drawThumb(-1);
        MS_show();
        resetViewport();
        }
}

void MVScrollBar::setValue(int v)
/****************************************************************************
*
* Function:     MVScrollBar::setValue
* Parameters:   v   - New value for the scroll bar
*
* Description:  Sets the value for the scroll bar, and updates it.
*
****************************************************************************/
{
    if (value == v)
        return;
    if (value < minVal) value = minVal;     // Clamp to specified range
    if (value > maxVal) value = maxVal;
    if (value == minVal || v == minVal)
        flags |= sbDirtyLeft;
    if (value == maxVal || v == maxVal)
        flags |= sbDirtyRight;
    flags |= sbDirtyThumb;
    value = v;
    moveThumb();
    update();
}

void MVScrollBar::setMinVal(int v)
/****************************************************************************
*
* Function:     MVScrollBar::setMinVal
* Parameters:   v   - New minimum value for the scroll bar
*
* Description:  Sets the minimum value for the scroll bar.
*
****************************************************************************/
{
    minVal = v;
    if (value < minVal) value = minVal;
    moveThumb();
    if (owner)
        owner->repaint(bounds);
}

void MVScrollBar::setMaxVal(int v)
/****************************************************************************
*
* Function:     MVScrollBar::setMaxVal
* Parameters:   v   - New maximum value for the scroll bar
*
* Description:  Sets the maximum value for the scroll bar.
*
****************************************************************************/
{
    maxVal = v;
    if (value > maxVal) value = maxVal;
    moveThumb();
    if (owner)
        owner->repaint(bounds);
}

void MVScrollBar::setRange(int min,int max)
/****************************************************************************
*
* Function:     MVScrollBar::setRange
* Parameters:   min - New minimum value for scroll bar
*               max - New maximum value for scroll bar
*
* Description:  Sets the minimum and maximum values for the scroll bar.
*
****************************************************************************/
{
    minVal = min;
    maxVal = max;
    if (value < minVal) value = minVal;
    if (value > maxVal) value = maxVal;
    moveThumb();
    if (owner)
        owner->repaint(bounds);
}
