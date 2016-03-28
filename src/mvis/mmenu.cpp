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
* Description:  Member functions for the MVMenu class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mmenu.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mfontmgr.hpp"

/*---------------------------- Global Variables ---------------------------*/

MVPoint MVMenu::arrow[7] =
{
    MVPoint(0,2),MVPoint(3,2),MVPoint(3,-1),MVPoint(7,3),
    MVPoint(3,7),MVPoint(3,5),MVPoint(0,5)
};

/*----------------------------- Implementation ----------------------------*/

MVMenu::MVMenu(MGLDevCtx& dc,const MVRect& bounds)
    : MVView(dc,bounds), parent(NULL)
/****************************************************************************
*
* Function:     MVMenu::MVMenu
* Parameters:   bounds          - bounding box for the menu
*
* Description:  Constructor for the menu.
*
****************************************************************************/
{
    items.setDelta(10);         // Use a reasonable delta value
    current = -1;               // Default to first item in menu

    options |= ofPreProcess;    // Enable pre-processing of events
    options &= ~ofClipToClient; // Dont clip the menu bar to the client rect
}

MVMenu::~MVMenu()
{
}

MVMenu& MVMenu::operator + (MVMenuItem* item)
/****************************************************************************
*
* Function:     MVMenu::operator +
* Parameters:   item    - Pointer to the item to insert
* Returns:      Reference to the menu the item was inserted into
*
****************************************************************************/
{
    items.add(item);
    if (item->isSubMenu())
        item->subMenu->parent = this;
    return *this;
}

void MVMenu::putEvent(MVEvent& event)
/****************************************************************************
*
* Function:     MVMenu::putEvent
* Parameters:   event   - Event to post
*
* Description:  Climbs up the parent tree first, before calling the owner's
*               event routine (SubMenu's have no owners).
*
****************************************************************************/
{
    if (parent)
        parent->putEvent(event);
    else if (owner)
        owner->putEvent(event);
}

ibool MVMenu::getEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVMenu::getEvent
* Parameters:   event   - Place to store the event
* Returns:      True if an event was pending, false if not.
*
* Description:  Climbs up the parent tree first, before calling the owner's
*               event routine (SubMenu's have no owners).
*
****************************************************************************/
{
    if (parent)
        return parent->getEvent(event,mask);
    else if (owner)
        return owner->getEvent(event,mask);
    return false;
}

ibool MVMenu::peekEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVMenu::peekEvent
* Parameters:   event   - Place to store the event
* Returns:      True if an event is pending, false if not.
*
* Description:  Climbs up the parent tree first, before calling the owner's
*               event routine (SubMenu's have no owners).
*
****************************************************************************/
{
    if (parent)
        return parent->getEvent(event,mask);
    else if (owner)
        return owner->peekEvent(event,mask);
    return false;
}

void MVMenu::invalidRect(MVRect& rect)
/****************************************************************************
*
* Function:     MVMenu::invalidRect
* Parameters:   rect    - Rectangle to invalidate.
*               global  - True if repaint event should be performed globally
*
* Description:  Overloaded to call the paren't routine if the parent exists,
*               otherwise we call the standard handling routine.
*
****************************************************************************/
{
    if (parent)
        parent->invalidRect(rect);
    else
        MVView::invalidRect(rect);
}

int MVMenu::findSelected(MVPoint& where)
/****************************************************************************
*
* Function:     MVMenu::findSelected
* Parameters:   where   - Selection point
* Returns:      Index of item that was selected (or closest one)
*
* Description:  Finds the item closest to the selection point. It is
*               assumed at this stage that the point is known to be contained
*               in the bounding rectangle for the entire menu.
*
****************************************************************************/
{
    uint    i;

    globalToLocal(where);
    for (i = 0; i < items.numberOfItems(); i++) {
        MVRect& b = items[i]->bounds;
        if (b.top() <= where.y && where.y < b.bottom())
            return items[i]->disabled ? -1 : (int)i;
        }
    if (where.y < items[0]->bounds.top() && !items[0]->disabled)
        return 0;
    i = items.numberOfItems()-1;
    if (where.y >= items[i]->bounds.bottom() && !items[i]->disabled)
        return i;
    return -1;
}

int MVMenu::findNextItem(int index)
/****************************************************************************
*
* Function:     MVMenu::findNextItem
* Parameters:   index   - Menu item index to start search with
* Returns:      Index of the next non-disabled menu item
*
****************************************************************************/
{
    ibool onceAround = false;
    do {
        if (++index >= (int)items.numberOfItems()) {
            if (onceAround) {
                index = -1;
                break;
                }
            index = 0;
            onceAround = true;
            }
        } while (items[index]->disabled);
    return index;
}

int MVMenu::findPrevItem(int index)
/****************************************************************************
*
* Function:     MVMenu::findPrevItem
* Parameters:   index   - Menu item index to start search with
* Returns:      Index of the previous non-disabled menu item
*
****************************************************************************/
{
    ibool onceAround = false;
    do {
        if (--index < 0) {
            if (onceAround) {
                index = -1;
                break;
                }
            index = items.numberOfItems()-1;
            onceAround = true;
            }
        } while (items[index]->disabled);
    return index;
}

ibool MVMenu::totalIncludes(MVPoint& where)
/****************************************************************************
*
* Function:     MVMenu::totalIncludes
* Parameters:   where   - Point to check for inclusion
* Returns:      True if point is included
*
* Description:  Checks to see if the point is within the bounds of the
*               current menu, or any of it's parents.
*
****************************************************************************/
{
    MVMenu *menu = this;
    ibool inside = false;
    while (menu) {
        if (menu->includes(where)) {
            inside = true;
            break;
            }
        menu = menu->parent;
        }
    return inside;
}

uint MVMenu::executeSubMenu(MVEvent& event)
/****************************************************************************
*
* Function:     MVMenu::executeSubMenu
* Parameters:   subMenu - Pointer to the submenu to execute
* Returns:      Command causing menu to stop executing.
*
****************************************************************************/
{
    // Draw the menu and execute it.
    switch (event.what) {
        case evMouseUp:
            event.what = evKeyDown;
            event.key.charScan.scanCode = kbHome;
            putEvent(event);
            break;
        case evMouseDown:
        case evMouseMove:
            if (subMenuID == current)
                items[subMenuID]->subMenu->refresh(-1);
            items[current]->subMenu->setCurrent(-1);
            break;
        case evKeyDown:
            event.what = evCommand;
            event.message.command = cmMenu;
            putEvent(event);
            break;
        }

    displayMenu(current);
    return items[current]->subMenu->execute();
}

void MVMenu::displayMenu(int selection)
/****************************************************************************
*
* Function:     MVMenu::displayMenu
*
* Description:  Saves the area behind the current pull down menu and
*               display's it.
*
****************************************************************************/
{
    if (repaintSave) {
        if (subMenuID == selection)
            return;
        restoreMenu();
        }

    MS_obscure();
    MVMenu *subMenu = items[subMenuID = selection]->subMenu;
    long size = dc.divotSize(subMenu->getBounds());
    save = NULL;
    if ((save = malloc(size)) != NULL) {
        dc.getDivot(subMenu->getBounds(),save);
        }

    subMenu->draw(MVRect(0,0,dc.sizex()+1,dc.sizey()+1));
    repaintSave = true;
    MS_show();
}

void MVMenu::restoreMenu()
/****************************************************************************
*
* Function:     MVMenu::restoreMenu
*
* Description:  Restore saved area behind pull down menu.
*
****************************************************************************/
{
    if (repaintSave) {
        if (save) {
            MS_obscure();
            dc.putDivot(save);
            MS_show();
            free(save);
            }
        else {
            repaint(items[subMenuID]->subMenu->getBounds());
            MVEvent event;
            getEvent(event,evRepaint);
            }
        }
    save = NULL;
    repaintSave = false;
    subMenuID = -1;
}

uint MVMenu::execute()
/****************************************************************************
*
* Function:     MVMenu::execute
* Returns:      Command causing the menu to stop executing.
*
* Description:  Main interaction handling routine for the MVMenu class. Here
*               we track the mouse in the menu and perform the selection
*               etc.
*
*               Is it assumed that the menu is visible when this routine
*               is called, and may only need refreshing.
*
****************************************************************************/
{
    MVEvent event;
    ibool   done = false;
    ibool   autoTracking = true;
    uint    result = cmValid;
    int     selection;

    selection = current;
    repaintSave = false;            // Nothing saved behind subMenu's yet
    subMenuID = -1;

    // The first event we recieve here is the original event that caused
    // the menu selection process to begin.
    while (!done) {
        getEvent(event);
        switch (event.what) {
            case evMouseDown:
            case evMouseUp:
                autoTracking = true;
                if (includes(event.where)) {
                    selection = findSelected(event.where);
                    if (selection != -1) {
                        if (items[selection]->isSubMenu()) {
                            refresh(selection);
                            result = executeSubMenu(event);
                            if (result != cmMenu)
                                done = true;
                            }
                        else if (event.what == evMouseUp) {
                            // The menu item was selected, so handle the
                            // selection here.
                            result = items[selection]->command;
                            done = true;
                            }
                        else
                            restoreMenu();
                        }
                    else {
                        if (event.what == evMouseUp)
                            done = true;
                        restoreMenu();
                        }
                    }
                else {
                    if (parent)
                        putEvent(event);
                    result = cmMenu;
                    done = true;
                    }
                break;
            case evMouseMove:
                // The mouse was moved, so if we are currently tracking
                // the position of the mouse cursor, check if we need
                // to update the current selection
                if (autoTracking) {
                    if (includes(event.where)) {
                        selection = findSelected(event.where);
                        if (selection != -1 && items[selection]->isSubMenu()) {
                            if (current != selection) {
                                restoreMenu();
                                refresh(selection);
                                }
                            result = executeSubMenu(event);
                            if (result != cmMenu)
                                done = true;
                            }
                        else
                            restoreMenu();
                        }
                    else if (parent && parent->totalIncludes(event.where)) {
                        putEvent(event);
                        result = cmMenu;
                        done = true;
                        }
                    else
                        selection = -1;
                    }
                break;
            case evKeyDown:
            case evKeyAuto: {
                ibool tracking = false;
                switch (event.key.charScan.scanCode) {
                    case kbUp:
                        selection = findPrevItem(selection);
                        break;
                    case kbDown:
                        selection = findNextItem(selection);
                        break;
                    case kbLeft:
                        if (items[selection]->isSubMenu() && subMenuID != -1)
                            restoreMenu();
                        else if (parent) {
                            putEvent(event);
                            result = cmMenu;
                            done = true;
                            }
                        break;
                    case kbRight:
                        if (items[selection]->isSubMenu() && subMenuID == -1) {
                            if ((result = executeSubMenu(event)) != cmMenu)
                                done = true;
                            }
                        else if (parent) {
                            putEvent(event);
                            result = cmMenu;
                            done = true;
                            }
                        break;
                    case kbHome:
                        selection = findNextItem(items.numberOfItems()-1);
                        break;
                    case kbEnd:
                        selection = findPrevItem(0);
                        break;
                    case kbEnter:
                    case kbGrayEnter:
                        if (items[selection]->isSubMenu()) {
                            if ((result = executeSubMenu(event)) != cmMenu)
                                done = true;
                            }
                        else {
                            result = items[selection]->command;
                            done = true;
                            }
                        break;
                    case kbEsc:
                        done = true;
                        break;
                    default:
                        // Check for global hotkey values here. We do this
                        // by finding the granddaddy menu of all and check
                        // for hotkeys from there.
                        MVMenu *menu = this;
                        while (menu->parent)
                            menu = menu->parent;

                        if (menu->checkHotKey(event)) {
                            done = true;
                            break;
                            }

                        // Check for keyboard shortcut's that could be
                        // valid for this menu (non-Alted ones).
                        if ((selection = findShortCut(event)) != -1) {
                            if (!items[selection]->disabled) {
                                if (items[selection]->isSubMenu()) {
                                    refresh(selection);
                                    if ((result = executeSubMenu(event)) != cmMenu)
                                        done = true;
                                    }
                                else {
                                    current = selection;
                                    result = items[selection]->command;
                                    done = true;
                                    }
                                }
                            }
                        else {
                            selection = current;
                            tracking = autoTracking;
                            }
                        break;
                    }
                autoTracking = tracking;
                } break;
            case evCommand:
                // Must have received a cmMenu command to get here
                CHECK(event.message.command == cmMenu);
                if (current == -1)
                    selection = findNextItem(items.numberOfItems()-1);
                autoTracking = false;
                break;
            }

        // Refresh the state of the menu if it has changed
        if (!done && current != selection)
            refresh(selection);
        }

    restoreMenu();                  // Restore stuff behind subMenu's
    return result;
}

void MVMenu::doSelection(MVEvent& event)
/****************************************************************************
*
* Function:     MVMenu::doSelection
* Parameters:   event   - Event causing the selection process to begin
*
* Description:  Sets up for a selection from the menu.
*
****************************************************************************/
{
    putEvent(event);            // Re-post the event to begin selection
    ulong command = execute();
    if (command != cmValid && command != cmMenu /* && commandEnabled(...) */) {
        event.what = evCommand;
        event.message.command = command;
        event.message.infoPtr = NULL;
        putEvent(event);
        }
    clearEvent(event);
}

int MVMenu::findShortCut(MVEvent& event)
/****************************************************************************
*
* Function:     MVMenu::findShortCut
* Parameters:   event   - Event to check
* Returns:      Index of menu item with specified hot character
*
****************************************************************************/
{
    for (uint i = 0; i < items.numberOfItems(); i++) {
        if (items[i]->hotChar &&
                items[i]->hotChar == event.key.charScan.charCode)
            return i;
        }
    return -1;
}

ibool MVMenu::checkHotKey(MVEvent& event)
/****************************************************************************
*
* Function:     MVMenu::checkHotKey
* Parameters:   event   - Event to check
* Returns:      True if command was posted
*
* Description:  Checks for a valid hot key combination, and posts the
*               appropriate command if one is found. In order to make the
*               hot key processing valid anywhere in the menu, we search
*               all subMenu's for hotkey values.
*
****************************************************************************/
{
    for (uint i = 0; i < items.numberOfItems(); i++) {
        MVMenuItem& item = *items[i];
        if (item.isSubMenu()) {
            if (item.subMenu->checkHotKey(event))
                return true;
            }
        else if (!item.disabled && item.hotKey == event.key) {
            event.what = evCommand;
            event.message.command = items[i]->command;
            event.message.infoPtr = NULL;
            putEvent(event);
            clearEvent(event);
            return true;
            }
        }
    return false;
}

void MVMenu::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     MVMenu::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Default event handling routine for the menu class.
*
****************************************************************************/
{
    if (items.numberOfItems() == 0)
        return;

    switch (event.what) {
        case evMouseDown:
            doSelection(event);
            break;
        case evKeyDown:
            if (!checkHotKey(event)) {
                // Check for Alt-?? combination keyboard shortcut to start
                // menu selection.
                int index;
                if (event.key.modifiers & mdAlt) {
                    if ((index = findShortCut(event)) != -1) {
                        current = index;
                        doSelection(event);
                        }
                    }
                }
            break;
        case evCommand:
            if (event.message.command == cmMenu)
                doSelection(event);
            break;
        case evBroadcast:
            // Handle change in active command set here!
            break;
        }
}

void MVMenu::drawItem(int index)
/****************************************************************************
*
* Function:     MVMenu::drawItem
* Parameters:   index   - Index of the menu item to draw
*
* Description:  Draws the menu item 'index'. If the item is the currently
*               selected item, we draw the item as selected, otherwise
*               we draw it as non-selected.
*
****************************************************************************/
{
    MVMenuItem& item = *items[index];

    MV_pushViewport(dc);
    dc.setRelViewport(item.bounds);

    int     maxx = dc.maxx();
    int     maxy = dc.maxy();
    color_t textColor;

    // Highlight the item if it is currently selected
    if (index == current) {
        dc.setColor(getColor(scMenuSelectedInterior));
        dc.fillRect(0,0,maxx+1,maxy+1);
        }

    if (item.name[0] == '-') {
        // Draw the separator item
        dc.setColor(getColor(scShadow));
        drawLine(0,maxy/2,maxx-2,maxy/2);
        dc.setColor(getColor(scHighlight));
        drawLine(1,maxy/2+1,maxx-3,maxy/2+1);
        }
    else {
        // Draw the item's text
        dc.setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
        drawHotStr(MVPoint(3,3),item.hot1,item.hot2,item.hotChar,
            MVPoint(0,0),0,item.name,item.disabled,
            textColor = getColor((uchar)(item.disabled ? scMenuDisabledText :
            ((index == current) ? scMenuSelectedText : scMenuText))));

        if (item.isSubMenu()) {
            // Draw an arrow to indicate that a submenu is present
            dc.fillPolygon(7,arrow,maxx-10,maxy/2-4);
            }
        else if (item.hotKeyText) {
            // Draw the hot key text
            dc.setTextJustify(MGL_RIGHT_TEXT,MGL_TOP_TEXT);
            if (item.disabled) {
                dc.setColor(getColor(scHighlight));
                dc.drawStr(maxx-2,4,item.hotKeyText);
                }
            dc.setColor(textColor);
            dc.drawStr(maxx-3,3,item.hotKeyText);
            }
        }

    MV_popViewport(dc);
}

void MVMenu::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVMenu::draw
* Parameters:   clip    - Clipping rectangle for the view
*
* Description:  Method to draw the menu representation.
*
****************************************************************************/
{
    MS_obscure();

    // Draw the background and frame for the menu
    dc.setColor(getColor(scMenuInterior));
    dc.fillRect(bounds);
    drawFrame(bounds,false);

    // Draw the menu items in the menu
    MVTextJust old;         // Save current text settings
    old.save(dc);
    useFont(fmSystemFont);

    for (uint i = 0; i < items.numberOfItems(); i++)
        drawItem(i);

    old.use(dc);            // Restore old text settings
    MS_show();
}

void MVMenu::refresh(int selection)
/****************************************************************************
*
* Function:     MVMenu::refresh
* Parameters:   selection   - New value for the current selection
*
* Description:  Refreshes the menu by drawing as little as possible. Assumes
*               that the menu is currently drawn in the correct state and
*               is fully visible.
*
****************************************************************************/
{
    if (current == selection)
        return;

    // Draw the menu items in the menu
    MVTextJust old;         // Save current text settings
    old.save(dc);
    useFont(fmSystemFont);

    MS_obscure();
    int oldSelect = current;
    if ((current = selection) != -1)
        drawItem(current);
    if (oldSelect != -1) {
        dc.setColor(getColor(scMenuInterior));
        dc.fillRect(items[oldSelect]->bounds);
        drawItem(oldSelect);
        }

    old.use(dc);            // Restore old text settings
    MS_show();
}

uint MVMenu::getHelpCtx() const
/****************************************************************************
*
* Function:     MVMenu::getHelpCtx
* Returns:      Help context number for the current menu item.
*
* Description:
*
****************************************************************************/
{
    return items[current]->helpCtx;
}

MVRect& MVMenu::setItemBounds(int index,const MVPoint& start)
/****************************************************************************
*
* Function:     MVMenu::setItemBounds
* Parameters:   index   - Index of menu item to compute bounds for
*               start   - Starting point for the item's bounds
* Returns       Reference to the bounding box
*
* Description:  Sets the bounding box for the indexed item.
*
****************************************************************************/
{
    MVRect& b = items[index]->bounds;

    b.topLeft = start;
    b.right() = b.left() + itemWidth + 6;
    if (items[index]->name[0] == '-')
        b.bottom() = b.top() + 4 + 2;
    else
        b.bottom() = b.top() + itemHeight;
    return b;
}

void MVMenu::doneDefinition()
/****************************************************************************
*
* Function:     MVMenu::doneDefinition
*
* Description:  Completes the definition of the menu structure. Here we
*               compute the size of the bounding box given the menu items
*               stored in it.
*
****************************************************************************/
{
    uint        i;
    int         width;
    metrics_t   m;

    MVTextJust old;         // Save current text settings
    old.save(dc);
    useFont(fmSystemFont);

    // Compute the height and width of the menu items
    dc.getFontMetrics(m);
    itemHeight = m.ascent-m.descent+1+5;

    itemWidth = 0;
    for (i = 0; i < items.numberOfItems(); i++) {
        MVMenuItem& item = *items[i];
        width = dc.textWidth(item.name) + dc.textWidth("  ");
        if (item.isSubMenu())
            width += 10;
        else
            width += dc.textWidth(item.hotKeyText);
        itemWidth = MAX(itemWidth,width);

        // Compute the position of the hot character underscore if
        // present.
        if (item.hotChar && item.name) {
            char old = item.name[item.hotIndex];
            item.name[item.hotIndex] = '\0';
            item.hot1.x = item.hot1.y = 3;
            dc.underScoreLocation(item.hot1.x,item.hot1.y,item.name);
            item.hot1.x += dc.textWidth(item.name);
            item.hot2 = item.hot1;
            metrics_t m;
            dc.getCharMetrics(old,m);
            item.hot2.x += m.fontWidth-1;
            item.name[item.hotIndex] = old;
            }
        }

    // Compute the bounding box for each item, and compute the bounding
    // box for the entire menu.
    bounds.right() = bounds.left()+1;
    bounds.bottom() = bounds.top()+1;
    MVPoint start(3,3);
    for (i = 0; i < items.numberOfItems(); i++) {
        MVRect& b = setItemBounds(i,start);
        bounds += b;
        start.y = b.bottom();
        }

    // Add a small buffer to the right and bottom edges
    bounds.right() += 3;
    bounds.bottom() += 3;
    setBounds(bounds);

    // Now fixup any submenus that are attached to this menu, moving them
    // to the correct location
    for (i = 0; i < items.numberOfItems(); i++) {
        MVMenuItem& item = *items[i];
        if (item.isSubMenu())
            item.subMenu->moveTo(bounds.right(),item.bounds.top());
        }

    old.use(dc);
}

void MVMenu::moveTo(int x,int y)
/****************************************************************************
*
* Function:     MVMenu::moveTo
* Parameters:   x,y - New position to move view to
*
* Description:  As well as moving the view, we also move all of the
*               menu items.
*
****************************************************************************/
{
    int dx = x-bounds.left();
    int dy = y-bounds.top();

    // Ensure that the menu is still within the bounds of the current
    // viewport.
    if (bounds.right()+dx > dc.maxx()+1)
        dx = (dc.maxx()-size.x) - bounds.left();
    if (bounds.bottom()+dy > dc.maxy()+1)
        dy = (dc.maxy()-size.y) - bounds.top();

    // Move each item, recursively moving submenu items.
    bounds.offset(dx,dy);
    for (uint i = 0; i < items.numberOfItems(); i++) {
        MVMenuItem& item = *items[i];
        item.bounds.offset(dx,dy);
        if (item.isSubMenu())
            item.subMenu->moveTo(bounds.right(),item.bounds.top());
        }
}
