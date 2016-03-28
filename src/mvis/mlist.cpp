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
* Description:  Member functions for the MVList class hierarchy.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mlist.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mkeys.hpp"

/*----------------------------- Implementation ----------------------------*/

#define INDENTLEFT  2
#define INDENTTOP   3

MVListBase::MVListBase(MGLDevCtx& dc,const MVRect& bounds,
    const MVPoint& cellSize,MVScrollBar *vScroll,MVScrollBar *hScroll,
    uint flags)
    : MVView(dc,bounds), flags(flags), cellSize(cellSize)
/****************************************************************************
*
* Function:     MVListBase::MVListBase
* Parameters:   bounds      - Bounding box for the list
*               cellSize    - Dimensions in pixels for each list cell
*               vScroll     - Pointer to horizontal scroll bar (NULL if none)
*               hScroll     - Pointer to vertical scroll bar (NULL if none)
*               flags       - Creation flags
*
* Description:  Constructor for the MVListBase class. Works out the number
*               of visible rows and columns in the list.
*
****************************************************************************/
{
    options |= ofSelectable | ofFirstClick;

    // Set the default indents for no visible scroll bars.
    leftIndent = 2;
    topIndent = 2;
    rightIndent = 2;
    botIndent = 2;

    // Find the visible set of cells
    visible.left() = visible.top() = 0;
    visible.bottom() = (bounds.bottom() - bounds.top() - (leftIndent + rightIndent))
                                / cellSize.y;
    visible.right() = (bounds.right() - bounds.left() - (topIndent + botIndent))
                                / cellSize.x;
    if (visible.bottom() == 0)
        visible.bottom() = 1;
    if (visible.right() == 0)
        visible.right() = 1;
    setHScroll(hScroll);
    setVScroll(vScroll);
    setCurrentCell(0,0);
}

void MVListBase::focusCurrent(ibool toTop,ibool redraw)
/****************************************************************************
*
* Function:     MVListBase::focusCurrent
* Parameters:   toTop   - True if the cell should be moved to the top pos.
*               redraw  - True if list should be redrawn
*
* Description:  Focuses the list on the current cell. If toTop is true, the
*               current cell is moved to the top position in the visible
*               window.
*
****************************************************************************/
{
    if (!visible.includes(cursor)) {
        if (cursor.y < visible.top())
            vScrollTo(cursor.y,redraw);
        if (cursor.y >= visible.bottom()) {
            if (toTop) {
                int y = range.bottom() - (visible.bottom() - visible.top());
                vScrollTo(MIN(cursor.y,y),redraw);
                }
            else
                vScrollTo(cursor.y - (visible.bottom() - visible.top()-1),
                    redraw);
            }
        if (cursor.x < visible.left())
            hScrollTo(cursor.x,redraw);
        if (cursor.x >= visible.right())
            hScrollTo(cursor.x - (visible.right() - visible.left()-1),
                redraw);
        }
}

void MVListBase::selectRange(int left,int top,int right,int bottom)
/****************************************************************************
*
* Function:     MVListBase::selectRange
* Parameters:   left,top,right,bottom   - Range of cells to select
*
* Description:  Selects all the cells that lie within the specified range.
*
****************************************************************************/
{
    int _left   = MAX(range.left(), left);
    int _top    = MAX(range.top(), top);
    int _right  = MIN(range.right(), right);
    int _bottom = MIN(range.bottom(), bottom);
    for (int i = _left; i < _right; i++)
        for (int j = _top; j < _bottom; j++)
            selectCell(i,j);
}

void MVListBase::clearRange(int left,int top,int right,int bottom)
/****************************************************************************
*
* Function:     MVListBase::clearRange
* Parameters:   left,top,right,bottom   - Range of cells to select
*
* Description:  Clears all of the cells that lie within the specified
*               range.
*
****************************************************************************/
{
    int _left   = MAX(range.left(), left);
    int _top    = MAX(range.top(), top);
    int _right  = MIN(range.right(), right);
    int _bottom = MIN(range.bottom(), bottom);
    for (int i = _left; i < _right; i++)
        for (int j = _top; j < _bottom; j++)
            deselectCell(i,j);
}

void MVListBase::clearSelection()
/****************************************************************************
*
* Function:     MVListBase::clearSelection
*
* Description:  Clears all of the cells in the list.
*
****************************************************************************/
{
    clearRange(selection);
    selection.topLeft = cursor;
    selection.right() = selection.left()+1;
    selection.bottom() = selection.top()+1;
    flags &= ~lsExtending;
}

void MVListBase::selectNext(uint direction,int count,ulong modifiers,
    ibool toTop)
/****************************************************************************
*
* Function:     MVListBase::selectNext
* Parameters:   direction   - Flags indicating the direction to move
*               count       - Number of cells to move down
*               modifiers   - Keyboard shift modifiers
*               toTop       - True if the cell should be moved to the top
*
* Description:  Adjusts the selection by the specified number of cells in
*               the specified direction. If the shift modifiers are set,
*               then the selection is extended.
*
****************************************************************************/
{
    MVPoint oldCursor(cursor);
    int maxv = maxV(),minv = minV();
    int maxh = maxH(),minh = minH();

    if (direction & lsBelow)
        if ((cursor.y += count) > maxv)
            cursor.y = maxv;
    if (direction & lsAbove)
        if ((cursor.y -= count) < minv)
            cursor.y = minv;
    if (direction & lsRight)
        if ((cursor.x += count) > maxh)
            cursor.x = maxh;
    if (direction & lsLeft)
        if ((cursor.x -= count) < minh)
            cursor.x = minh;

    if (cursor != oldCursor || (flags & lsExtending)) {
        if ((flags & lsMultipleSelect) && (modifiers & mdShift)) {
            if (cursor == oldCursor)
                return;

            if (direction & lsLeft) {
                if (flags & lsExtendRight) {
                    // We are currently extending in the opposite direction,
                    // so clear all of the cells from the old cursor position
                    // to one above the new cursor position. If the selection
                    // is only one high, then turn off the extending flags.
                    if (cursor.x <= selection.left()) {
                        flags &= ~lsExtendHoriz;
                        selection.right() = selection.left()+1;
                        selection.left() = cursor.x;
                        if (selection.left() != selection.right()-1) {
                            flags |= lsExtendLeft;
                            selectRange(selection);
                            }
                        }
                    else
                        selection.right() = cursor.x+1;
                    clearRange(selection.right(),selection.top(),
                        oldCursor.x+1,selection.bottom());
                    }
                else {
                    // We are currently extending the selection in the same
                    // direction, or have just started to extend the selection
                    flags |= lsExtendLeft;
                    selection.left() = cursor.x;
                    selectRange(cursor.x,selection.top(),
                        oldCursor.x,selection.bottom());
                    }
                }

            if (direction & lsRight) {
                if (flags & lsExtendLeft) {
                    if (cursor.x >= selection.right()-1) {
                        flags &= ~lsExtendHoriz;
                        selection.left() = selection.right()-1;
                        selection.right() = cursor.x+1;
                        if (selection.left() != selection.right()-1) {
                            flags |= lsExtendRight;
                            selectRange(selection);
                            }
                        }
                    else
                        selection.left() = cursor.x;
                    clearRange(oldCursor.x,selection.top(),
                        selection.left(),selection.bottom());
                    }
                else {
                    flags |= lsExtendRight;
                    selection.right() = cursor.x+1;
                    selectRange(oldCursor.x+1,selection.top(),
                        cursor.x+1,selection.bottom());
                    }
                }

            if (direction & lsAbove) {
                if (flags & lsExtendDown) {
                    if (cursor.y <= selection.top()) {
                        flags &= ~lsExtendVert;
                        selection.bottom() = selection.top()+1;
                        selection.top() = cursor.y;
                        if (selection.top() != selection.bottom()-1) {
                            flags |= lsExtendUp;
                            selectRange(selection);
                            }
                        }
                    else
                        selection.bottom() = cursor.y+1;
                    clearRange(selection.left(),selection.bottom(),
                        selection.right(),oldCursor.y+1);
                    }
                else {
                    flags |= lsExtendUp;
                    selection.top() = cursor.y;
                    selectRange(selection.left(),cursor.y,
                        selection.right(),oldCursor.y);
                    }
                }

            if (direction & lsBelow) {
                if (flags & lsExtendUp) {
                    if (cursor.y >= selection.bottom()-1) {
                        flags &= ~lsExtendVert;
                        selection.top() = selection.bottom()-1;
                        selection.bottom() = cursor.y+1;
                        if (selection.top() != selection.bottom()-1) {
                            flags |= lsExtendDown;
                            selectRange(selection);
                            }
                        }
                    else
                        selection.top() = cursor.y;
                    clearRange(selection.left(),oldCursor.y,
                        selection.right(),selection.top());
                    }
                else {
                    flags |= lsExtendDown;
                    selection.bottom() = cursor.y+1;
                    selectRange(selection.left(),oldCursor.y+1,
                        selection.right(),cursor.y+1);
                    }
                }
            dirtyCell(oldCursor);
            }
        else {
            // The selection is not being extended, so clear any previous
            // selection and turn extending off, and reselect the cell
            // under the cursor.
            flags &= ~lsExtending;
            if (!(flags & lsDisjointSelect)) {
                clearSelection();
                selectCell(cursor);
                }
            else {
                dirtyCell(oldCursor);
                dirtyCell(cursor);
                }
            }

        MV_message(owner,evBroadcast,cmListCursorChanged,this);
        refresh();
        }
    focusCurrent(toTop);
}

uint MVListBase::findCellHit(const MVPoint& global,MVPoint& loc)
/****************************************************************************
*
* Function:     MVListBase::findCellHit
* Parameters:   global  - Global mouse location point
*               loc     - Place to store computed cell location
* Returns:      Flags representing where the mouse click occurred
*
* Description:  Determines where the mouse click occurred in reference to
*               the list box. If the mouse click was inside the list box,
*               we compute the location of the cell that was hit and return
*               it in loc.
*
****************************************************************************/
{
    MVPoint p(global);
    globalToLocal(p);

    if (bounds.includes(p)) {
        loc.x = (p.x - (bounds.left()+leftIndent)) / cellSize.x;
        loc.y = (p.y - (bounds.top()+topIndent)) / cellSize.y;

        loc += visible.topLeft;     // Make relative to visible cells
        return lsInside;            // The mouse click was inside the list
        }

    // The mouse click was outside of the list, so determine where it
    // occurred and set the relevant flags
    uint    flags = 0;

    if (p.x < bounds.left()+leftIndent)     flags |= lsLeft;
    if (p.x >= bounds.right()-rightIndent)  flags |= lsRight;
    if (p.y < bounds.top()+topIndent)       flags |= lsAbove;
    if (p.y >= bounds.bottom()-botIndent)   flags |= lsBelow;

    return flags;
}

void MVListBase::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVListBase::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling mechanism for the MVListBase class.
*
****************************************************************************/
{
    MVView::handleEvent(event,phase);

    switch (event.what) {
        case evMouseDown:
            if (range.isEmpty())
                return;

            if (event.mouse.buttons & mbLeftButton) {
                while (event.what != evMouseUp) {
                    switch (event.what) {
                        case evMouseDown:
                            if (!(event.mouse.modifiers & mdShift)
                                    && !(flags & lsDisjointSelect)) {
                                clearSelection();
                                selectCell(cursor);
                                refresh();
                                }
                            if (event.mouse.doubleClick) {
                                MV_message(owner,evBroadcast,cmListItemSelected,this);
                                goto doneMouse;
                                }
                        case evMouseAuto:
                        case evMouseMove:
                            MVPoint loc;
                            uint    modifiers = mdShift;

                            if (event.what == evMouseDown)
                                modifiers = event.mouse.modifiers;
                            if (flags & lsDisjointSelect)
                                modifiers = 0;

                            uint pos = findCellHit(event.where,loc);
                            if (pos == lsInside && event.what != evMouseAuto) {
                                // Selection within the list
                                if (loc == cursor && event.what == evMouseDown) {
                                    // Post a message to ensure updates
                                    // occur for the first mouse down event
                                    // in the list box.
                                    MV_message(owner,evBroadcast,
                                        cmListCursorChanged,this);
                                    }

                                if (loc.x < cursor.x)
                                    selectLeft(cursor.x - loc.x,modifiers);
                                else if (loc.x > cursor.x)
                                    selectRight(loc.x - cursor.x,modifiers);

                                if (loc.y < cursor.y)
                                    selectUp(cursor.y - loc.y,modifiers);
                                else if (loc.y > cursor.y)
                                    selectDown(loc.y - cursor.y,modifiers);
                                }
                            else if (event.what == evMouseAuto) {
                                // Auto selection outside window to scroll
                                if (pos & lsAbove)
                                    selectUp(1,modifiers);
                                if (pos & lsBelow)
                                    selectDown(1,modifiers);
                                if (pos & lsLeft)
                                    selectLeft(1,modifiers);
                                if (pos & lsRight)
                                    selectRight(1,modifiers);
                                }
                            if (event.what == evMouseDown
                                    && flags & lsDisjointSelect) {
                                toggleCell(cursor);
                                refresh();
                                }
                            break;
                        }
                    getEvent(event);
                    }

doneMouse:
                clearEvent(event);
                }
            break;
        case evKeyDown:
        case evKeyAuto:
            if (range.isEmpty())
                return;

            switch (event.key.charScan.scanCode) {
                case kbSpace:
                    if (event.what == evKeyAuto)
                        break;
                    if (flags & lsDisjointSelect) {
                        toggleCell(cursor);
                        refresh();
                        }
                    break;
                case kbLeft:
                    selectLeft(1,event.key.modifiers);
                    break;
                case kbRight:
                    selectRight(1,event.key.modifiers);
                    break;
                case kbUp:
                    selectUp(1,event.key.modifiers);
                    break;
                case kbDown:
                    selectDown(1,event.key.modifiers);
                    break;
                case kbHome:
                    selectNext(lsLeft | lsAbove,range.bottom(),
                        event.key.modifiers);
                    break;
                case kbEnd:
                    selectNext(lsRight | lsBelow,range.bottom(),
                        event.key.modifiers);
                    break;
                case kbPgUp:
                    selectUp(visible.bottom()-visible.top(),
                        event.key.modifiers,true);
                    break;
                case kbPgDn:
                    selectDown(visible.bottom()-visible.top(),
                        event.key.modifiers,true);
                default:
                    // Key press is not handled by us, so simply return.
                    return;
                }
            clearEvent(event);
            break;
        case evBroadcast:
            if (options & ofSelectable) {
                if (event.message.command == cmScrollBarClicked &&
                        (event.message.infoPtr == hScroll ||
                         event.message.infoPtr == vScroll))
                    select();
                else if (event.message.command == cmScrollBarChanged) {
                    if (range.isEmpty())
                        return;
                    if (event.message.infoPtr == hScroll)
                        hScrollTo(hScroll->getValue());
                    else if (event.message.infoPtr == vScroll)
                        vScrollTo(vScroll->getValue());
                    }
                }
            break;
        }
}

void MVListBase::drawCell(int i,int j)
/****************************************************************************
*
* Function:     MVListBase::drawCell
* Parameters:   i,j - Location of the cell to draw
*
* Description:  Draws the cell, clipped to the bounds of the list. This
*               routine assumes that the list is entirely visible.
*
****************************************************************************/
{
    MS_obscure();

    if (visible.includes(i,j)) {
        // Compute the bounding box for the cell
        int dx = (i-visible.left()) * cellSize.x;
        int dy = (j-visible.top()) * cellSize.y;

        MVRect r(bounds.left()+leftIndent+dx,
                 bounds.top()+topIndent+dy,
                 bounds.left()+leftIndent+dx+cellSize.x,
                 bounds.top()+topIndent+dy+cellSize.y);
        r &= MVRect(bounds.left()+leftIndent,
                    bounds.top()+topIndent,
                    bounds.right()-rightIndent,
                    bounds.bottom()-botIndent);
        if (!r.isEmpty()) {
            dc.setClipRect(r);
            drawCell(i,j,r);
            }
        }

    MS_show();
}

void MVListBase::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVListBase::draw
* Parameters:   clip    - Clipping rectangle to use when drawing
*
* Description:  Draws the representation of the list.
*
****************************************************************************/
{
    MS_obscure();

    MVRect  clipBounds(bounds.left()+leftIndent,
                       bounds.top()+topIndent,
                       bounds.right()-rightIndent,
                       bounds.bottom()-rightIndent);

    // Clear the background for the list
    dc.setColor(getColor(scListInterior));
    dc.fillRect(bounds);
    MV_setBorderColors(getColor(scShadow),getColor(scHighlight));
    MV_drawBorder(bounds,MV_BDR_OUTSET,1);
    MV_setBorderColors(getColor(scBlack),getColor(scButtonFace));
    MV_drawBorder(bounds.insetBy(1,1),MV_BDR_OUTSET,1);

    // Draw each of the items in the list. Note that we set up the clip
    // rectangle to clip everything to the bounds of the list correctly,
    // and that we only draw those items that are visible and within the
    // range of selectable items.
    MVRect  v(visible & range);
    MVRect  start(bounds.left()+leftIndent,
                  bounds.top()+topIndent,
                  bounds.left()+leftIndent+cellSize.x,
                  bounds.top()+topIndent+cellSize.y);

    for (int i = v.left(); i < v.right(); i++) {
        MVRect r(start);
        for (int j = v.top(); j < v.bottom(); j++) {
            MVRect c(r);
            c &= clipBounds;
            if (!c.isEmpty()) {
                dc.setClipRect(c);
                drawCell(i,j,r);
                }
            r.top() += cellSize.y;  r.bottom() += cellSize.y;
            }
        start.left() += cellSize.x; start.right() += cellSize.x;
        }

    if (range.isEmpty()) {
        // There is nothing to draw in the list (it is currently empty),
        // but if the list is selected then we should draw a dotted outline
        // around the first empty cell to ensure the user knows when the
        // list is selected.
        if ((state & sfFocused)) {
            attributes_t attr;
            dc.getAttributes(attr);
            dc.setColor(getColor(scListCell));
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
            dc.usePenBitmapPattern(0);
            drawRect(bounds.left()+leftIndent,
                     bounds.top()+topIndent,
                     bounds.left()+leftIndent+cellSize.x,
                     bounds.top()+topIndent+cellSize.y);
            dc.restoreAttributes(attr);
            }
        }

    MS_show();
}

void MVListBase::refresh()
/****************************************************************************
*
* Function:     MVListBase::refresh
*
* Description:  Refreshes the list by refreshing each of the visible cells.
*
****************************************************************************/
{
    MVRect   v(visible & range);

    MS_obscure();
    for (int i = v.left(); i < v.right(); i++)
        for (int j = v.top(); j < v.bottom(); j++)
            refreshCell(i,j);
    MS_show();
}

const int syLineSize = 1;

void MVListBase::update()
/****************************************************************************
*
* Function:     MVListBase::update
*
* Description:  Forces an update for the entire list, by posting a repaint
*               event for the interior portion of the list.
*
****************************************************************************/
{
    CHECK(owner != NULL);
    MVRect  r(bounds);
    r.inset(syLineSize,syLineSize);
    owner->repaint(r);
}

void MVListBase::vScrollTo(int j,ibool redraw)
/****************************************************************************
*
* Function:     MVListBase::vScrollTo
* Parameters:   j       - New cell to scroll to in vertical direction
*               redraw  - True if list should be redrawn
*
* Description:  Scrolls the list to start at the new cell 'j' in the
*               vertical direction, redrawing all necessary cells to do
*               this.
*
]****************************************************************************/
{
    if (visible.top() != j) {
        // Setup the viewport for redrawing the list directly, and redraw
        // it.
        MVRect   v(visible & range);
        MVRect  b(bounds.left()+leftIndent,
                  bounds.top()+topIndent,
                  bounds.left()+leftIndent+cellSize.x*(visible.right()-visible.left()),
                  bounds.top()+topIndent+cellSize.y*(visible.bottom()-visible.top()));
        int     dy = (v.top() - j) * cellSize.y;

        if (b.right() > (bounds.right()-rightIndent))
            b.right() = bounds.right()-rightIndent;
        if (b.bottom() > (bounds.bottom()-botIndent))
            b.bottom() = bounds.bottom()-botIndent;

        visible.moveTo(hScroll ? hScroll->getValue() : 0,j);
        vScroll->setValue(j);

        if (redraw) {
            CHECK(owner != NULL);
            setupOwnerViewport();
            MS_obscure();

            if (j < v.top()) {
                b.bottom() -= dy;
                if (!b.isEmpty())
                    dc.bitBlt(dc,b,b.left(),bounds.top()+topIndent+dy,MGL_REPLACE_MODE);
                for (int y = j; y < v.top(); y++)
                    for (int x = v.left(); x < v.right(); x++)
                        drawCell(x,y);
                }
            else {
                b.top() -= dy;
                if (!b.isEmpty())
                    dc.bitBlt(dc,b,b.left(),bounds.top()+topIndent,MGL_REPLACE_MODE);
                j -= v.top();
                for (int y = 0; y < j; y++)
                    for (int x = v.left(); x < v.right(); x++)
                        drawCell(x,v.bottom()+y);
                }
            MS_show();
            resetViewport();
            }
        }
}

void MVListBase::hScrollTo(int i,ibool redraw)
/****************************************************************************
*
* Function:     MVListBase::hScrollTo
* Parameters:   i       - New cell to scroll to in horizontal direction
*               redraw  - True if list should be redrawn
*
* Description:  Scrolls the list to start at the new cell 'i' in the
*               horizontal direction, redrawing all necessary cells to do
*               this.
*
****************************************************************************/
{
    if (visible.left() != i) {
        // Setup the viewport for redrawing the list directly, and redraw
        // it.
        MVRect  v(visible & range);
        MVRect  b(bounds.left()+leftIndent,
                  bounds.top()+topIndent,
                  bounds.right()-rightIndent,
                  bounds.bottom()-botIndent);
        int     dx = (v.left() - i) * cellSize.x;

        visible.moveTo(i,vScroll ? vScroll->getValue() : 0);
        hScroll->setValue(i);

        if (redraw) {
            CHECK(owner != NULL);
            setupOwnerViewport();
            MS_obscure();

            if (i < v.left()) {
                b.right() -= dx;
                if (!b.isEmpty())
                    dc.bitBlt(dc,b,bounds.left()+leftIndent+dx,b.top(),MGL_REPLACE_MODE);
                for (int y = v.top(); y < v.bottom(); y++)
                    for (int x = i; x < v.left(); x++)
                        drawCell(x,y);
                }
            else {
                b.left() -= dx;
                if (!b.isEmpty())
                    dc.bitBlt(dc,b,bounds.left()+leftIndent,b.top(),MGL_REPLACE_MODE);
                i -= v.left();
                for (int y = v.top(); y < v.bottom(); y++)
                    for (int x = 0; x < i; x++)
                        drawCell(v.right() + x,y);
                }
            MS_show();
            resetViewport();
            }
        }
}

void MVListBase::setCurrentCell(int i,int j)
/****************************************************************************
*
* Function:     MVListBase::setCurrentCell
* Parameters:   i,j - Location of cell to set
*
* Description:  Sets the cell (i,j) to be the currently active cell.
*
****************************************************************************/
{
    cursor.x = i;
    cursor.y = j;
    selection.topLeft = cursor;
    selection.right() = selection.left()+1;
    selection.bottom() = selection.top()+1;
    focusCurrent(true,false);
}

void MVListBase::setHScroll(MVScrollBar *h)
/****************************************************************************
*
* Function:     MVListBase::setHScroll
* Parameters:   h   - Pointer to the new scroll bar for the list
*
* Description:  Sets the horizontal scroll bar for the list.
*
****************************************************************************/
{
    hScroll = h;
    setHRange(0,0);
    if (hScroll) {
        int step = visible.right() - visible.left()-1;
        hScroll->setPageStep(MAX(1,step));
        hScroll->setArrowStep(1);
        }
}

void MVListBase::setVScroll(MVScrollBar *v)
/****************************************************************************
*
* Function:     MVListBase::setVScroll
* Parameters:   v   - Pointer to the new scroll bar for the list
*
* Description:  Sets the vertical scroll bar for the list.
*
****************************************************************************/
{
    vScroll = v;
    setVRange(0,0);
    if (vScroll) {
        int step = visible.bottom() - visible.top()-1;
        vScroll->setPageStep(MAX(1,step));
        vScroll->setArrowStep(1);
        }
}

void MVListBase::setHRange(int min,int max)
/****************************************************************************
*
* Function:     MVListBase::setHRange
* Parameters:   min     - Minimum value for the range
*               max     - Maximum value for the range
*
* Description:  Sets the horizontal range for the list, by setting the
*               range of the horizontal scroll bar. The scrollbar value
*               is set to min.
*
****************************************************************************/
{
    if (hScroll) {
        range.left() = min;
        range.right() = max+1;
        if ((max -= (visible.right() - visible.left()-1)) < min)
            max = min;
        hScroll->setRange(min,max);
        hScroll->setValue(min);
        }
}

void MVListBase::setVRange(int min,int max)
/****************************************************************************
*
* Function:     MVListBase::setVRange
* Parameters:   min     - Minimum value for the range
*               max     - Maximum value for the range
*
* Description:  Sets the vertical range for the list, by setting the
*               range of the vertical scroll bar. The scrollbar value
*               is set to min.
*
****************************************************************************/
{
    if (vScroll) {
        range.top() = min;
        range.bottom() = max+1;
        if ((max -= (visible.bottom() - visible.top()-1)) < min)
            max = min;
        vScroll->setRange(min,max);
        vScroll->setValue(min);
        }
}

MVList::MVList(MGLDevCtx& dc,const MVRect& bounds,const MVPoint& cellSize,
    const MVRect& dataBounds,MVScrollBar *vScroll,MVScrollBar *hScroll,
    uint flags)
    : MVListBase(dc,bounds,cellSize,vScroll,hScroll,flags),
      dataBounds(dataBounds)
/****************************************************************************
*
* Function:     MVList::MVList
* Parameters:   bounds      - Bounding box for the list
*               dataBounds  - Bounds on number of items in the list
*               cellSize    - Dimensions in pixels for each list cell
*               vScroll     - Pointer to horizontal scroll bar (NULL if none)
*               hScroll     - Pointer to vertical scroll bar (NULL if none)
*               flags       - Creation flags
*
* Description:  Constructor for the MVList class, for manipulating
*               scrollable lists of text strings.
*
****************************************************************************/
{
    setDataBounds(dataBounds);
}

void MVList::clearItems()
/****************************************************************************
*
* Function:     MVList::clearItems
*
* Description:  Clears all of the items in the array, to point to nothing.
*
****************************************************************************/
{
    for (uint i = 0; i < cells.numberOfItems(); i++) {
        cells[i].text = NULL;
        cells[i].flags = 0;
        }
    setHRange(dataBounds.left(),dataBounds.left());
    setVRange(dataBounds.top(),dataBounds.top());
}

void MVList::setDataBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVList::setDataBounds
* Parameters:   bounds  - New bounds on the number of items in the list
*
* Description:  When this is called, the array is first emptied of all
*               data, then re-sized to fit the new bounds. All data in
*               the list is effectively lost, so will need to be
*               re-inserted.
*
****************************************************************************/
{
    dataBounds = bounds;
    visible.moveTo(dataBounds.topLeft);
    MVPoint size(bounds.botRight - bounds.topLeft);
    int count = size.x * size.y;
    cells.setCount(MAX(count,1));
    clearItems();
}

void MVList::drawCell(int i,int j,const MVRect& bounds)
/****************************************************************************
*
* Function:     MVList::drawCell
* Parameters:   i,j     - Index of the cell to draw
*               bounds  - Bounding box to draw the item in
*
* Description:  Draws the cell item within the specified bounds. Note that
*               the appropriate clipping rectangle will already have been
*               set up before this routine is called.
*
****************************************************************************/
{
    CellItem&   cell = findCell(i,j);

    if (cell.text) {
        // If the items is selected and this is the focused view, highlight
        // the item, otherwise clear the background for the item.
        dc.setColor(getColor(
            cell.flags & lsSelected ? scListHighlightCell : scListInterior));
        dc.fillRect(bounds);

        // Draw the text for the item
        useFont(fmSystemFont);
        dc.setColor(getColor(
            cell.flags & lsSelected ? scListSelectedCell : scListCell));
        dc.drawStr(bounds.left() + INDENTLEFT,
                   bounds.top() + INDENTTOP,cell.text);

        // If the cursor is on the item, and view is focused then draw
        // a dotted outline around the cell.
        if ((state & sfFocused) && cursor.x == i && cursor.y == j) {
            attributes_t attr;
            dc.getAttributes(attr);

            dc.setColor(getColor(
                cell.flags & lsSelected ? scListCursor : scListCell));
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
            dc.usePenBitmapPattern(0);
            drawRect(bounds);
            dc.restoreAttributes(attr);
            }
        }
    cell.flags &= ~lsDirty;
}

void MVList::refreshCell(int i,int j)
/****************************************************************************
*
* Function:     MVList::refreshCell
* Parameters:   i,j - Index of the cell to refresh
*
* Description:  Refreshes the indexed cell if it is dirty.
*
****************************************************************************/
{
    if (findCell(i,j).flags & lsDirty || (cursor.x == i && cursor.y == j))
        MVListBase::drawCell(i,j);
}

void MVList::selectCell(int i,int j)
/****************************************************************************
*
* Function:     MVList::selectCell
* Parameters:   i,j     - Index of the cell to select
*
* Description:  Sets the selected flag for the item. If the cell was not
*               already selected, we set the dirty bit for the cell.
*
****************************************************************************/
{
    CellItem& cell = findCell(i,j);

    if (!(cell.flags & lsSelected))
        cell.flags |= lsDirty;
    cell.flags |= lsSelected;
}

void MVList::deselectCell(int i,int j)
/****************************************************************************
*
* Function:     MVList::deselectCell
* Parameters:   i,j     - Index of the cell to de-select
*
* Description:  Clears the selected flags for the item. If the cell was
*               already selected, we set the dirty bit for the cell.
*
****************************************************************************/
{
    CellItem& cell = findCell(i,j);

    if (cell.flags & lsSelected)
        cell.flags |= lsDirty;
    cell.flags &= ~lsSelected;
}

void MVList::toggleCell(int i,int j)
/****************************************************************************
*
* Function:     MVList::toggleCell
* Parameters:   i,j     - Index of cell to toggle
*
* Description:  Toggles the selection flags for the specified cell.
*
****************************************************************************/
{
    CellItem& cell = findCell(i,j);;

    cell.flags ^= lsSelected;
    cell.flags |= lsDirty;
}

void MVList::dirtyCell(int i,int j)
/****************************************************************************
*
* Function:     MVList::dirtyCell
* Parameters:   i,j     - Index of the cell to dirty
*
* Description:  Sets the dirty bit for the specified cell.
*
****************************************************************************/
{
    findCell(i,j).flags |= lsDirty;
}

void MVList::setCell(int i,int j,const char *text)
/****************************************************************************
*
* Function:     MVList::setCell
* Parameters:   i,j     - Index of the cell whose text to set
*               text    - Pointer to the text for the cell
*
* Description:  Sets the text for the cell. If this is NULL, nothing is
*               drawn in the cell.
*
****************************************************************************/
{
    CellItem& cell = findCell(i,j);

    cell.text = text;
    cell.flags |= lsDirty;
}

ibool MVList::getCell(int i,int j,const char*& text)
/****************************************************************************
*
* Function:     MVList::getCell
* Parameters:   i,j     - Index of the cell whose text to set
*               text    - Place to store the text for the cell
* Returns:      True if the cell is selected, false if not.
*
* Description:  Finds the text of a cell, returning true if the cell is
*               selected.
*
****************************************************************************/
{
    CellItem& cell = findCell(i,j);

    text = cell.text;
    return (cell.flags & lsSelected);
}
