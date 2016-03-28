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
* Description:  Header file for the MVMenu class. This class is the class
*               of menu object used to represent a vertical list of
*               items in a box.
*
*               The items in the menu are always drawn in the current
*               system font.
*
****************************************************************************/

#ifndef __MVIS_MMENU_HPP
#define __MVIS_MMENU_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

#ifndef __MVIS_MKEYS_HPP
#include "mvis/mkeys.hpp"
#endif

#ifndef __TCL_ARRAY_HPP
#include "tcl/array.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVMenuItem class is used to represent a single menu item stored in
// a menu. The MVMenu class is used to collect together MVMenuItems into
// the actual menu structure.
//---------------------------------------------------------------------------

class MVMenu;

class MVMenuItem {
public:
    char        *name;          // Name of the menu item
    ulong       command;        // Command to post when selected
    ibool       disabled;       // True if item is disabled
    MVHotKey    hotKey;         // Menu item hot key value
    char        hotChar;        // Character code for hot character
    uint        hotIndex;       // Index of hot character
    uint        helpCtx;        // Help context number for the menu item
    MVPoint     hot1,hot2;      // Coordinates of hot character underscore
    MVRect      bounds;         // Bounding rectangle for the item
    union {
        char    *hotKeyText;    // Text to display for hot key
        MVMenu  *subMenu;       // Pointer to submenu
        };

public:
            // Constructors
            MVMenuItem(const char *name,
                      ulong command,
                      const MVHotKey& hotKey = _MV_noHotKey,
                      uint helpCtx = hcNoContext,
                      const char *hotKeyText = NULL);

            MVMenuItem(const char *name,
                      MVMenu *subMenu,
                      uint helpCtx = hcNoContext);

            // Copy constructor
            MVMenuItem(const MVMenuItem& c);

            // Destructor
            ~MVMenuItem();

            // Overloaded operator equals - we never compare these
            ibool operator == (const MVMenuItem&)   { return false; };

            // Returns true if item is a sub menu
            ibool isSubMenu() const { return command == 0; };
    };

//---------------------------------------------------------------------------
// The TSubMenuItem class is used to represent the submenu item entry in a
// menu, and exists simply to clarify the code used to create the menu
// items.
//---------------------------------------------------------------------------

class MVSubMenuItem : public MVMenuItem {
public:
            // Constructor
            MVSubMenuItem(const char *name,
                     MVMenu *subMenu,
                     uint helpCtx = hcNoContext)
                : MVMenuItem(name,subMenu,helpCtx) {};
    };

//---------------------------------------------------------------------------
// The MVMenuItemSeparator class is used to represent the sepatatore menu
// item in the menus. Separators are always disabled.
//---------------------------------------------------------------------------

class MVMenuItemSeparator : public MVMenuItem {
public:
            // Constructor
            MVMenuItemSeparator() : MVMenuItem("-",cmError)
                { disabled = true; };
    };

//---------------------------------------------------------------------------
// The MVMenu class is used to represent a vertical list of menu items in
// a box.
//
// Note that not all of the text settings passed to the MVMenu constructor
// are recognised. The text is always drawn in the current system font.
//---------------------------------------------------------------------------

class MVMenu : public MVView {
protected:
    TCIArray<MVMenuItem>    items;      // Array of items for the menu
    int                     current;    // Index of the current item
    int                     itemWidth;  // Maximum menu item width
    int                     itemHeight; // Height of each item
    MVMenu                  *parent;    // Pointer to parent menu
    void                    *save;      // Save area behind submenu's
    ibool                   repaintSave;// Repaint the saved area
    int                     subMenuID;  // Id of displayed submenu
    static MVPoint          arrow[7];   // Arrow used for submenus

            // Method to draw a menu item in the menu
    virtual void drawItem(int index);

            // Method to compute the bounding rectangle for an item
    virtual MVRect& setItemBounds(int index,const MVPoint& start);

            // Method to handle a selection from the menu
            void doSelection(MVEvent& event);

            // Method to find the menuitem with a specified shortcut
            int findShortCut(MVEvent& event);

            // Method to check for and handle keyboard hot keys
            ibool checkHotKey(MVEvent& event);

            // Method to find the next valid menu item
            int findNextItem(int index);

            // Method to find the previous valid menu item
            int findPrevItem(int index);

            // Method to find which menu item was hit
            int findSelected(MVPoint& where);

            // Method to begin executing a subMenu
            uint executeSubMenu(MVEvent& event);

            // Method to display a menu and restore it
            void displayMenu(int selection);
            void restoreMenu();

            // Method to check for total inclusion of a point (all parents)
            ibool totalIncludes(MVPoint& where);

public:
            // Constructor
            MVMenu(MGLDevCtx& dc,const MVRect& bounds = _MV_emptyRect);

            // Destructor
    virtual ~MVMenu();

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Virtual to execute the menu tracking the mouse etc
    virtual uint execute();

            // Methods to retrieve and post events - eventually calls
            // eventQueue.xxx() if not overridden
    virtual void putEvent(MVEvent& event);
    virtual ibool getEvent(MVEvent& event,ulong mask = evAll);
    virtual ibool peekEvent(MVEvent& event,ulong mask = evAll);

            // Virtual to draw the menu
    virtual void draw(const MVRect& dirty);

            // Method to refresh the menu with a newly selected item
            void refresh(int selection);

            // Virtual to move view to another location
    virtual void moveTo(int x,int y);

            // Methods to invalidate the specified rectangle
    virtual void invalidRect(MVRect& rect);

            // Method to obtain the help context number for the menu
    virtual uint getHelpCtx() const;

            // Overloaded operator + to add a menu item
            MVMenu& operator + (MVMenuItem* item);

            // Method to complete the menu definition
    virtual void doneDefinition();

            // Method to set the current item
            void setCurrent(int index)  { current = index; };

            // Method to swap a menu item - old item is _not_ deleted
            void swapItem(int itemNo,MVMenuItem *item)
                { items[itemNo] = item; };

            // Method to disabled a menu item
            void disableItem(int itemNo)
                { items[itemNo]->disabled = true; };

            // Method to enable a menu item
            void enableItem(int itemNo)
                { items[itemNo]->disabled = false; };

            // Return the current item for the specified index
            MVMenuItem *getItem(int itemNo)
                { return items[itemNo]; };
    };

#endif  // __MVIS_MMENU_HPP
