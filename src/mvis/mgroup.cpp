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
* Description:  Member functions for the MVGroup class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mprogram.hpp"

/*----------------------------- Implementation ----------------------------*/

extern void *_MV_quickPaint;

MVGroup::MVGroup(MGLDevCtx& dc,const MVRect& bounds)
    : MVView(dc,bounds), selected(NULL), clientRect(bounds)
/****************************************************************************
*
* Function:     MVGroup::MVGroup
* Parameters:   bounds  - Bounding rectangle for the group
*
* Description:  Constructor for the MVGroup class
*
****************************************************************************/
{
}

MVGroup::~MVGroup()
/****************************************************************************
*
* Function:     MVGroup::~MVGroup
*
* Description:  Destructor for the MVGroup class. We dont need to do
*               anything as the list of children will automatically
*               destruct itself correctly.
*
****************************************************************************/
{
}

ibool MVGroup::valid(ulong command)
/****************************************************************************
*
* Function:     MVGroup::valid
* Parameters:   command - Command ending the modal operation
* Returns:      True if the group is valid
*
* Description:  Simply checks that all children in the view are valid.
*
****************************************************************************/
{
    for (MVViewIterator i(children); i; i++)
        if (i.node()->valid(command) == false)
            return false;
    return true;
}

void MVGroup::doEvent(MVView& view,MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVGroup
* Parameters:   view    - View to send event to
*               event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Handles the event for a single view. We check to see
*               if the view can recieve the event, send the event to
*               it if it can.
*
****************************************************************************/
{
    // If the view is disabled, don't send any events to it
    if (view.getState() & sfDisabled)
        return;
    switch (phase) {
        case phPreProcess:
            if (!(view.getOptions() & ofPreProcess))
                return;
            break;
        case phPostProcess:
            if (!(view.getOptions() & ofPostProcess))
                return;
            break;
        case phFocused:
            break;
        }

    // Set local clip rectangle for view event handling
    if (view.options & ofClipToClient) {
        MV_pushClipRect(dc);
        view.clipRect = clientRect;
        dc.setClipRect(view.clipRect.offset(-bounds.left(),-bounds.top()));
        }
    else dc.getClipRect(view.clipRect);

    view.handleEvent(event,phase);

    // Restore clip rectangle to previous value
    if (view.options & ofClipToClient)
        MV_popClipRect(dc);
}

void MVGroup::eventError(MVEvent& event)
/****************************************************************************
*
* Function:     MVGroup::eventError
* Parameters:   event   - Unhandled event
*
* Description:  Handles the case of unhandled events. Simply calls the
*               owners routine if an owner exists.
*
****************************************************************************/
{
    if (owner)
        owner->eventError(event);
}

void MVGroup::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVGroup::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the MVGroup class. Here we
*               delegate the event to all children of the group, depending
*               on the type of event (focused etc).
*
*               Note that before the event is sent to any of the children
*               of the group, the viewport is set to that of the owning
*               group.
*
****************************************************************************/
{
    MVViewIterator  i(children);

    MVView::handleEvent(event,phase);       // Default handling first

    setupViewport();

    // Handle the case of the cmGrabModal and cmReleaseModal messages
    // specially for groups.
    if (event.what == evBroadcast && (state & sfModal)) {
        if (event.message.command == cmGrabModal) {
            modalState++;
            }
        else if (event.message.command == cmReleaseModal) {
            CHECK(modalState > 0);
            modalState--;
            }
        }

    if ((event.what & focusedEvents) && (state & sfFocused)) {
        // The event is one that can be focused, so send the event to
        // all views with the ofPreProcess flag set, then to the focused
        // view, then to all views with the ofPostProcess flag set.
        for (i.restart(); i; i++)
            doEvent(*i.node(),event,phPreProcess);

        if (selected)
            doEvent(*selected,event,phFocused);

        if (event.what != evNothing) {
            for (i.restart(); i; i++)
                doEvent(*i.node(),event,phPostProcess);
            }
        }
    else {
        if (event.what & positionalEvents) {
            // The event is a positional type event, so work out which
            // child view the event lies in, and send the event to it.
            ibool handled = false;
            for (i.restart(); i; i++) {
                if (i.node()->includes(event.where)) {
                    doEvent(*i.node(),event,phFocused);
                    handled = true;
                    }
                }
            if (!handled && (state & sfModal) && event.what == evMouseDown) {
                // Positional event outside of the modal viewport, so
                // beep the speaker to indicate this.
                MV_beep();
                }
            }
        else {
            // Must be a broadcast or some other type of event, so send
            // it to all children
            for (i.restart(); i; i++)
                doEvent(*i.node(),event,phFocused);
            }
        }

    resetViewport();
}

uint MVGroup::execView(MVGroup *view)
/****************************************************************************
*
* Function:     MVGroup::execView
* Parameters:   view    - Pointer to view to execute
* Returns:      Command that caused the view to complete execution
*
* Description:  Pop's up the group on the desktop, and executes
*               it until completion. When the group has finished
*               interacting with the user, the display under the group
*               is restored by posting a cmRepaint message with an
*               invalid rectangle set to the area affected by the
*               group.
*
****************************************************************************/
{
    MVEvent event;
    uint    retval = cmCancel;

    if (view) {
        // Save the state of the view before executing it.
        ibool   selectable = view->getOptions() & ofSelectable;
        MVView  *oldSelected = selected;

        view->setOption(ofSelectable,false);
        view->setState(sfModal,true);

        // TODO: Put support in here for re-painting the background for
        // the executed view so we dont have to use the repaint events
        // to handle this (better for complex scenes like 3D).

        insert(view);
        select(view);
        retval = view->execute();
        remove(view);
        select(oldSelected);

        view->setState(sfModal,false);
        view->setOption(ofSelectable,selectable);

        // Repaint the area behind the dialog box by checking for
        // the pending repaint event.
        getEvent(event,evRepaint);
        }
    return retval;
}

void MVGroup::changeCursor(const MVPoint& pos)
/****************************************************************************
*
* Function:     MVGroup::changeCursor
*
* Description:  Changes the cursor definition if need be.
*
****************************************************************************/
{
    MGLCursor *c = getCursor(pos);

    if (c == NULL)
        c = MV_ARROW_CURSOR;
    if (c != MV_currentCursor) {
        MV_currentCursor = c;
        MS_setCursor(*MV_currentCursor);
        }
}

uint MVGroup::execute()
/****************************************************************************
*
* Function:     MVGroup::execute
* Returns:      Command that caused the view to complete execution
*
* Description:  Handles event processing for the group until the modal
*               view has finished executing.
*
****************************************************************************/
{
    MVEvent     event;

    // Select the first view in tab order if a default selection has not
    // been set up.
    if (!selected)
        select(children.peekHead());

    // Force the view to be painted
    repaint();
    getEvent(event,evRepaint);

    // Save the old viewport and set the new viewport to be equal to the
    // owner's viewport, as all events will be handled relative to this
    // viewport.
    setupOwnerViewport();
    MV_message(this,evCommand,cmBeginEvents);
    do {
        endState = cmValid;
        do {
            if (getEvent(event)) {
                if (event.what == evMouseMove)
                    changeCursor(event.where);
                if ((event.what & positionalEvents) && MVView::capturedView) {
                    /* The mouse has been captured by a view, so send the
                     * mouse events directly to the view for processing.
                     */
                    MVView *view = MVView::capturedView;
                    view->setupOwnerViewport();
                    view->handleEvent(event);
                    view->resetViewport();
                    }
                else
                    handleEvent(event);
                }
            } while (endState == cmValid);
        } while (!valid(endState));
    MV_message(this,evCommand,cmEndEvents);
    resetViewport();
    return endState;
}

void MVGroup::doInsert(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::doInsert
* Parameters:   view    - View being inserted into the group
*
* Description:  Finish the insertion operation, doing all things common
*               to all insert operations.
*
****************************************************************************/
{
    view->setOwner(this);           // Set the owner field to point to us

    // Now check to see if the view should be centered, and if so
    // then center it. Note that we center it within the inner portion
    // of the group.
    if (view->getOptions() & ofCentered) {
        MVRect  b(view->getBounds());
        MVPoint p(b.topLeft);

        if (view->getOptions() & ofCenterX) {
            p.x = ((clientRect.right()-clientRect.left()) - (b.right()-b.left())) / 2
                    + clientRect.left() - bounds.left();
            }
        if (view->getOptions() & ofCenterY) {
            p.y = ((clientRect.bottom()-clientRect.top()) - (b.bottom()-b.top())) / 2
                    + clientRect.top() - bounds.top();
            }

        view->moveTo(p);
        }

    // If the owner is exposed, then expose the child, causing a
    // repaint event to be posted.
    view->setState(sfExposed,state & sfExposed);
}

void MVGroup::insert(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::insert
* Parameters:   view    - View to insert into group
*
* Description:  Inserts the view at the head of the list of children.
*
****************************************************************************/
{
    if (view) {
        children.addToHead(view);       // Add to head of the list
        doInsert(view);
        }
}

void MVGroup::insertAfter(MVView *view,MVView *after)
/****************************************************************************
*
* Function:     MVGroup::insertAfter
* Parameters:   view    - View to insert into group
*               after   - View to insert after
*
* Description:  Inserts the view into the child list after the specified
*               child.
*
****************************************************************************/
{
    if (view) {
        children.addAfter(view,after);
        doInsert(view);
        }
}

void MVGroup::remove(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::remove
* Parameters:   view    - View to remove from the group
*
* Description:  Removes the view from the child list.
*
****************************************************************************/
{
    children.remove(view);
    if (selected && view == selected)
        selected = NULL;

    // Request a repaint event for the spot where the view was removed
    // from, and unexpose the view causing a repaint event to be posted
    view->setState(sfExposed,false);
    view->setOwner(NULL);
}

void MVGroup::select(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::select
* Parameters:   view    - View to select
*
* Description:  Make the view the new selected view. If view is NULL,
*               then the current view is deselected and no view is
*               selected.
*
****************************************************************************/
{
    if (view != selected) {
        // We are changing the currently selected view, so deselect
        // the old view and select the new one
        if (view) {
            if (!(view->getState() & sfActive))
                view->setState(sfActive,true);
            }
        if (selected) {
            selected->setState(sfFocused,false);
            selected->setState(sfActive,false);
            }
        selected = view;
        if (view)
            view->setState(sfFocused,true);
        }
}

MVView* MVGroup::getNext(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::getNext
* Parameters:   view    - View to start with
* Returns:      Next view in tab order.
*
****************************************************************************/
{
    if (view == NULL)
        return children.peekHead();
    MVView *next = children.next(view);
    if (next == NULL)
        return children.peekHead();
    return next;
}

MVView* MVGroup::getPrev(MVView *view)
/****************************************************************************
*
* Function:     MVGroup::getPrev
* Parameters:   view    - View to start with
* Returns:      Previous view in tab order
*
****************************************************************************/
{
    if (view == NULL)
        return children.peekHead();
    MVView *prev = children.prev(view);
    if (prev == NULL)
        return children.peekTail();
    return prev;
}

void MVGroup::selectNext()
/****************************************************************************
*
* Function:     MVGroup::selectNext
*
* Description:  Selects the next view in the group, in tab order.
*
****************************************************************************/
{
    MVView  *next = selected;

    do {
        if ((next = getNext(next)) == NULL) {
            selected = NULL;
            return;
            }
        } while ((next != selected) &&
                 ((next->getState() & sfDisabled) ||
                  !(next->getOptions() & ofSelectable)));
    select(next);
}

void MVGroup::selectPrev()
/****************************************************************************
*
* Function:     MVGroup::selectPrev
*
* Description:  Selects the previous view in the group in tab order.
*
****************************************************************************/
{
    MVView  *prev = selected;
    do {
        if ((prev = getPrev(prev)) == NULL) {
            selected = NULL;
            return;
            }
        } while ((prev != selected) &&
                 ((prev->getState() & sfDisabled) ||
                  !(prev->getOptions() & ofSelectable)));
    select(prev);
}

void MVGroup::setState(uint aState,ibool set)
/****************************************************************************
*
* Function:     MVGroup::setState
* Parameters:   aState  - State flag to set
*               set     - True if flag should be set, false if cleared
*
****************************************************************************/
{
    if ((aState & sfModal) && set) {
        // The group is grabbing the modal state
        CHECK(!(state & sfModal));
        MV_message(MVProgram::application,evBroadcast,cmGrabModal);
        modalState = 0;
        }

    MVView::setState(aState,set);
    MVViewIterator i(children);

    if (aState & sfActive) {
        // The group just became active, so activate all of the views in
        // the group.
        for (i.restart(); i; i++)
            i.node()->setState(sfActive,set);
        }
    if (aState & sfFocused) {
        // The group just came into focus or went out of focus, so
        // change the focus on the currently selected view.
        if (selected)
            selected->setState(sfFocused,set);
        }
    if (aState & sfExposed) {
        // The group was just exposed or un-exposed, so change the
        // exposure of all the visible children
        for (i.restart(); i; i++) {
            if (i.node()->getState() & sfVisible)
                i.node()->setState(sfExposed,set);
            }
        // The group has just been exposed and it is the top view on the
        // desktop, so paint it with a quick paint rather than a normal
        // paint.
        if (set && MVProgram::deskTop->firstView() == this)
            _MV_quickPaint = this;
        }
    if ((aState & sfModal) && !set) {
        // The group is releasing the modal state.
        MV_message(MVProgram::application,evBroadcast,cmReleaseModal);
        }
}

void MVGroup::setBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVGroup::setBounds
* Parameters:   bounds  - New bounding rectangle for the view
*
* Description:  Sets the bounding rectangle for the view. By default the
*               client rectangle for a group is the same as the bounding
*               rectangle.
*
****************************************************************************/
{
    MVView::setBounds(bounds);
    clientRect = bounds;
}

void MVGroup::moveTo(int x,int y)
/****************************************************************************
*
* Function:     MVGroup::moveTo
* Parameters:   x,y - New position to move view to
*
****************************************************************************/
{
    MVPoint oldTopLeft(bounds.topLeft);
    MVView::moveTo(x,y);
    clientRect.offset(bounds.topLeft.x - oldTopLeft.x,
                      bounds.topLeft.y - oldTopLeft.y);
}

MVView *MVGroup::findModalView()
/****************************************************************************
*
* Function:     MVGroup::findModalView
* Returns:      Pointer to the currently active modal view (NULL if none).
*
* Description:  Checks to see if this group is the modal group, returning
*               a pointer to this if it is, otherwise we search all
*               children for the modal group.
*
****************************************************************************/
{
    if ((state & sfModal) && modalState == 0)
        return this;

    for (MVViewIterator i(children); i; i++) {
        MVView *view = i.node()->findModalView();
        if (view)
            return view;
        }
    return NULL;
}

void MVGroup::idle()
/****************************************************************************
*
* Function:     MVGroup::idle
*
* Description:  Routine called when the view is idle. We simply call the
*               currently focused view's idle routine.
*
****************************************************************************/
{
    if (selected) {
        setupViewport();
        selected->idle();
        resetViewport();
        }
}

MGLCursor *MVGroup::getCursor(const MVPoint& p)
/****************************************************************************
*
* Function:     MVGroup::getCursor
* Parameters:   p   - Current location of mouse cursor (global coords)
* Returns:      Pointer to the cursor definition for the view.
*
* Description:  Here we work out which child view contains the cursor
*               and call it to obtain the cursor definition.
*
****************************************************************************/
{
    MGLCursor *cursor = NULL;

    setupViewport();
    for (MVViewIterator i(children); i; i++)
        if (i.node()->includes(p)) {
            cursor = i.node()->getCursor(p);
            break;
            }
    resetViewport();
    return cursor;
}

static MVGroup *group;

void MVGroup::_drawBkgnd(const rect_t *r)
{ group->dc.setClipRect(*r); group->drawBackground(*r); }

static void _ASMAPI drawBkgnd(const rect_t *r)
{ MVGroup::_drawBkgnd(r); }

void MVGroup::paint()
/****************************************************************************
*
* Function:     MVGroup::paint
*
* Description:  Draws the group by drawing all of the children in the
*               group, starting with the last child and drawing in front
*               to back order. We also do quick rejection testing of the
*               clip rectangle against the clipRgn for each child, only
*               performing a draw operation if they intersect.
*
****************************************************************************/
{
    MVViewIterator  i(children);
    MVView          *child;
    MVRegion        occluding,visRgn,clientRgn(clientRect);
    ibool           quickPaint = (_MV_quickPaint == MVProgram::application);

    // Check if this group has been flagged for a special quick paint
    // operation (it has just been exposed).
    if (!quickPaint && _MV_quickPaint == this)
        quickPaint = true;

    // Setup proper viewport and adjust clip region
    setupViewport();
    clientRgn.offset(-bounds.left(),-bounds.top());
    clipRgn.offset(-bounds.left(),-bounds.top());

    // Go through the list of children in front to back order, computing
    // the clipRgn's for each child.
    for (i.restart(); i; i++) {
        child = i.node();
        CHECK((child->state & sfExposed) && (child->state & sfVisible));
        if (child->state & sfExposed) {
            // Clip the childs visRgn to the client rectangle if this is
            // not turned off.
            if (child->options & ofClipToClient)
                visRgn = child->visRgn & clientRgn;
            else visRgn = child->visRgn;

            // The visible region for the child is the default visRgn
            // for the child minus the occluding area for views
            // in front of this view.
            if (!quickPaint) {
                child->clipRgn = visRgn & clipRgn;
                child->clipRgn -= occluding;
                occluding += visRgn;
                }
            else child->clipRgn = visRgn;

            // Optimize the clip and visible regions for faster updates
            child->visRgn.optimize();
            child->clipRgn.optimize();
            }
        }

    // Draw the background, clipping to the clip region intersected with
    // the occluding region for all views in the group
    if (!(options & ofNoBackground)) {
        if (!quickPaint) {
            MVRegion clip(clipRgn);
            clip -= occluding;
            group = this;
            clip.optimize();
            clip.traverse(drawBkgnd);
            }
        else {
            MVRect clip(0,0,size.x,size.y);
            dc.setClipRect(clip);
            drawBackground(clip);
            }
        }

    // Draw each child, clipping to each individual clip region.
    for (i.restartTail(); i; i--) {
        child = i.node();
        if (child->getState() & sfExposed && !child->clipRgn.isEmpty())
            child->paint();
        }

    resetViewport();
}

uint MVGroup::getHelpCtx() const
/****************************************************************************
*
* Function:     MVGroup::getHelpCtx
*
* Description:  Returns the help context number for the group. If the
*               currently active view has no context, we return the
*               context of the entire group.
*
****************************************************************************/
{
    uint h = hcNoContext;

    if (selected && (h = selected->getHelpCtx()) == hcNoContext)
        h = MVView::getHelpCtx();
    return h;
}
