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
* Description:  Member functions for the MVMenuBar class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mmenubar.hpp"
#include "mvis/mfontmgr.hpp"
#include <stdlib.h>

#define OFFSETX     2
#define OFFSETY     4

/*----------------------------- Implementation ----------------------------*/

int MVMenuBar::findSelected(MVPoint& where)
/****************************************************************************
*
* Function:     MVMenuBar::findSelected
* Parameters:   where   - Selection point
* Returns:      Index of item that was selected (or closest one)
*
* Description:  Finds the item closest to the selection point. It is
*               assumed at this stage that the point is know to be contained
*               in the bounding rectangle for the entire menu.
*
****************************************************************************/
{
    uint    i;

    globalToLocal(where);
    for (i = 0; i < items.numberOfItems(); i++) {
        MVRect& b = items[i]->bounds;
        if (b.left() <= where.x && where.x < b.right())
            return items[i]->disabled ? -1 : (int)i;
        }
    if (where.x < items[0]->bounds.left() && !items[0]->disabled)
        return 0;
    i = items.numberOfItems()-1;
    if (where.x >= items[i]->bounds.right() && !items[i]->disabled)
        return i;
    return -1;
}

uint MVMenuBar::execute()
/****************************************************************************
*
* Function:     MVMenuBar::execute
* Returns:      Command causing the menu to stop executing.
*
* Description:  Main interaction handling routine for the MVMenuBar class.
*               Here we track the mouse in the menu and perform the
*               menu selection etc.
*
****************************************************************************/
{
    MVEvent event;
    ibool   autoTracking = true;
    uint    result = cmMenu;
    int     selection;

    if (current == -1)
        current = findNextItem(items.numberOfItems()-1);
    selection = current;

    repaintSave = false;            // Nothing saved behind menu's yet
    subMenuID = -1;

    // The first event we recieve here is the original event that caused
    // the menu selection process to begin.
    while (result == cmMenu) {
        getEvent(event);
        switch (event.what) {
            case evMouseDown:
            case evMouseUp:
                autoTracking = true;
                if (includes(event.where)) {
                    if ((selection = findSelected(event.where)) != -1) {
                        refresh(-1);
                        refresh(selection);

                        // The menu was selected, so pull it down and
                        // execute it.
                        result = executeSubMenu(event);
                        }
                    else
                        result = cmValid;
                    }
                else
                    result = cmValid;
                break;
            case evMouseMove:
                // The mouse was moved, so if we are currently tracking
                // the position of the mouse cursor, check if we need
                // to update the current selection
                if (autoTracking) {
                    if (includes(event.where)) {
                        selection = findSelected(event.where);
                        if (current != selection)
                            refresh(selection);
                        result = executeSubMenu(event);
                        }
                    }
                break;
            case evKeyDown:
            case evKeyAuto: {
                ibool tracking = false;
                switch (event.key.charScan.scanCode) {
                    case kbUp:
                    case kbDown:
                    case kbEnter:
                    case kbGrayEnter:
                        result = executeSubMenu(event);
                        break;
                    case kbLeft:
                        selection = findPrevItem(selection);
                        refresh(selection);
                        if (subMenuID != -1)
                            result = executeSubMenu(event);
                        break;
                    case kbRight:
                        selection = findNextItem(selection);
                        refresh(selection);
                        if (subMenuID != -1)
                            result = executeSubMenu(event);
                        break;
                    case kbEsc:
                        result = cmValid;
                        break;
                    default:
                        if ((selection = findShortCut(event)) != -1) {
                            if (!items[selection]->disabled) {
                                // Menu was selected, so pull it down
                                // and execute it.
                                refresh(-1);
                                refresh(selection);
                                result = executeSubMenu(event);
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
                refresh(-1);
                refresh(selection);
                autoTracking = false;
                break;
            }

        // Refresh the state of the menu if it has changed
        if (current != selection)
            refresh(selection);
        }

    // Unhilight the menu bar on return, and restore stuff behind menus
    restoreMenu();
    refresh(-1);
    return result;
}

void MVMenuBar::drawItem(int index)
/****************************************************************************
*
* Function:     MVMenuBar::drawItem
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


    // Highlight the item if it is currently selected
    if (index == current) {
        dc.setColor(getColor(scMenuSelectedInterior));
        dc.fillRect(0,0,maxx+1,maxy+1);
        }

    // Draw the item's text
    dc.setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
    drawHotStr(MVPoint(OFFSETX+dc.charWidth(' '),OFFSETY),item.hot1,
        item.hot2,item.hotChar,MVPoint(0,0),0,item.name,item.disabled,
        getColor((uchar)(item.disabled ? scMenuDisabledText :
        ((index == current) ? scMenuSelectedText : scMenuText))));

    MV_popViewport(dc);
}

MVRect& MVMenuBar::setItemBounds(int index,const MVPoint& start)
/****************************************************************************
*
* Function:     MVMenuBar::setItemBounds
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
    b.right() = b.left() + dc.textWidth(items[index]->name)
        + dc.textWidth("  ") + 4;
    b.bottom() = b.top() + itemHeight;
    return b;
}

void MVMenuBar::doneDefinition()
/****************************************************************************
*
* Function:     MVMenuBar::doneDefinition
*
* Description:  Completes the definition of the menu structure. Here we
*               compute the size of the bounding box given the menu items
*               stored in it.
*
****************************************************************************/
{
    uint        i;
    metrics_t   m;

    MVTextJust old;         // Save current text settings
    old.save(dc);
    useFont(fmSystemFont);

    dc.getFontMetrics(m);
    itemHeight = m.ascent-m.descent+1+5;

    // Compute the bounding box for each item, and compute the bounding
    // box for the entire menu.
    MVPoint start(3,3);
    bounds.bottom() = bounds.top()+1;
    for (i = 0; i < items.numberOfItems(); i++) {
        MVRect& b = setItemBounds(i,start);
        bounds += b;
        start.x = b.right();
        }
    bounds.bottom() += 3;
    setBounds(bounds);

    // Now fixup any submenus that are attached to this menu, moving them
    // to the correct location
    for (i = 0; i < items.numberOfItems(); i++) {
        MVMenuItem& item = *items[i];
        CHECK(item.isSubMenu());
        item.subMenu->moveTo(item.bounds.left()-1,bounds.bottom()-2);

        // Compute the position of the hot character underscore if
        // present.
        if (item.hotChar && item.name) {
                char oldc;
            oldc = item.name[item.hotIndex];
            item.name[item.hotIndex] = '\0';
            item.hot1.x = OFFSETX;  item.hot1.y = OFFSETY;
            dc.underScoreLocation(item.hot1.x,item.hot1.y,item.name);
            item.hot1.x += dc.textWidth(item.name) + dc.charWidth(' ');
            item.hot2 = item.hot1;
            metrics_t m;
            dc.getCharMetrics(oldc,m);
            item.hot2.x += m.fontWidth-1;
            item.name[item.hotIndex] = oldc;
            }
        }

    old.use(dc);
}
