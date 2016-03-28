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
* Description:  Header file for the MVList class. This class is used
*               to represent a scrollable list of text strings, that
*               can be used to inplement pick lists etc.
*
****************************************************************************/

#ifndef __MVIS_MLIST_HPP
#define __MVIS_MLIST_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

#ifndef __MVIS_MSCROLLB_HPP
#include "mvis/mscrollb.hpp"
#endif

#ifndef __TCL_ARRAY_HPP
#include "tcl/array.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

// Creation flags

    lsMultipleSelect    = 0x0001,   // Multiple cells can be selected
    lsDisjointSelect    = 0x0002,   // Multiple selections can be disjoint
    lsNormal            = 0x0000,   // Normal single selection list

// Flags used when extending selection

    lsExtendLeft        = 0x0010,   // Extending selection left
    lsExtendRight       = 0x0020,   // Extending selection right
    lsExtendUp          = 0x0040,   // Extending selection up
    lsExtendDown        = 0x0080,   // Extending selection down
    lsExtendHoriz       = lsExtendLeft | lsExtendRight,
    lsExtendVert        = lsExtendUp | lsExtendDown,
    lsExtending         = lsExtendHoriz | lsExtendVert,

// Flags used to determine where a mouse click occured

    lsAbove             = 0x0001,   // Mouse click was above
    lsBelow             = 0x0002,   // Mouse click was below
    lsLeft              = 0x0004,   // Mouse click was left
    lsRight             = 0x0008,   // Mouse click was right
    lsInside            = 0x0010,   // Mouse click was inside

// Flags used by each list cell

    lsSelected          = 0x0001,   // The list cell is selected
    lsDirty             = 0x0002;   // The list cell is dirty

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVListBase class is an abstract base class used to represent
// scrollable lists of items. The items themselves are not directly
// represented, but will be drawn by the the derived class.
//---------------------------------------------------------------------------

class MVListBase : public MVView {
protected:
    uint        flags;              // Creation flags
    MVRect      visible;            // Index's of visible cells
    MVRect      range;              // Range of selectable cells
    MVRect      selection;          // Selected cells
    MVPoint     cellSize;           // Size of each cell
    MVPoint     cursor;             // Current position in list
    int         leftIndent;         // Indent for list items from left
    int         topIndent;          // Indent for list items from top
    int         rightIndent;        // Indent for list items from right
    int         botIndent;          // Indent for list items from bottom
    MVScrollBar *hScroll;           // Horizontal scroll bar (owned)
    MVScrollBar *vScroll;           // Vertical scroll bar (owned)

            // Pure virtual to draw a list cell
    virtual void drawCell(int i,int j,const MVRect& bounds) = 0;

            // Pure virtual to refresh a list cell
    virtual void refreshCell(int i,int j) = 0;

            // Pure virtual to refresh the list
            void refresh();

            // Method to draw a given a cursor location
            void drawCell(int i,int j);
            void drawCell(const MVPoint& cursor)
                { drawCell(cursor.x,cursor.y); };

            // Methods to scroll to a specified item
            void hScrollTo(int i,ibool redraw = true);
            void vScrollTo(int j,ibool redraw = true);

            // Method to select a number of cells in specified direction
            void selectNext(uint direction,int count,ulong modifiers,
                ibool toTop = false);

            // Methods to select new cells
            void selectLeft(int count,ulong modifiers,ibool toTop = false)
                { selectNext(lsLeft,count,modifiers,toTop); };
            void selectRight(int count,ulong modifiers,ibool toTop = false)
                { selectNext(lsRight,count,modifiers,toTop); };
            void selectUp(int count,ulong modifiers,ibool toTop = false)
                { selectNext(lsAbove,count,modifiers,toTop); };
            void selectDown(int count,ulong modifiers,ibool toTop = false)
                { selectNext(lsBelow,count,modifiers,toTop); };

            // Method to focus the current cell
            void focusCurrent(ibool toTop,ibool redraw = true);

            // Method to find a cell from a mouse click
            uint findCellHit(const MVPoint& global,MVPoint& loc);

public:
            // Constructor
            MVListBase(MGLDevCtx& dc,const MVRect& bounds,const MVPoint& cellSize,
                MVScrollBar *vScroll,MVScrollBar *hScroll,
                uint flags = lsNormal);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Methods to draw the view
    virtual void draw(const MVRect& dirty);

            // Pure virtual to select a list cell
    virtual void selectCell(int i,int j) = 0;
            void selectCell(const MVPoint& p)   { selectCell(p.x,p.y); };

            // Pure virtal to de-select a list cell
    virtual void deselectCell(int i,int j) = 0;
            void deselectCell(const MVPoint& p) { deselectCell(p.x,p.y); };

            // Pure virtual to toggle a list cell
    virtual void toggleCell(int i,int j) = 0;
            void toggleCell(const MVPoint& p)   { toggleCell(p.x,p.y); };

            // Pure virtual to dirty a cell
    virtual void dirtyCell(int i,int j) = 0;
            void dirtyCell(const MVPoint& p)        { dirtyCell(p.x,p.y); };

            // Methods to select a range of cells
            void selectRange(int left,int top,int right,int bottom);
            void selectRange(const MVRect& r)
                { selectRange(r.left(),r.top(),r.right(),r.bottom()); };

            // Methods to clear a range of cells
            void clearRange(int left,int top,int right,int bottom);
            void clearRange(const MVRect& r)
                { clearRange(r.left(),r.top(),r.right(),r.bottom()); };

            // Method to clear all currently selected cells
            void clearSelection();

            // Method to set the current cell
            void setCurrentCell(int i,int j);

            // Method to get the current cell
            void getCurrentCell(MVPoint& c) { c = cursor; };

            // Method to set the horizontal scroll bar
            void setHScroll(MVScrollBar *h);

            // Method to set the vertical scroll bar
            void setVScroll(MVScrollBar *v);

            // Method to set the horizontal range
            void setHRange(int min,int max);

            // Method to set the vertical range
            void setVRange(int min,int max);

            // Methods to obtain the range values
            int minH() const    { return range.topLeft.x; };
            int maxH() const    { return range.botRight.x-1; };
            int minV() const    { return range.topLeft.y; };
            int maxV() const    { return range.botRight.y-1; };

            // Force a direct update to the list
            void update();
    };

//---------------------------------------------------------------------------
// The MVList class is a concrete class derived from MVListBase that is used
// to manipulate dynamic scrollable lists of text strings. The string data
// itself is not actually maintained in the list, but simply the pointers
// to the user's data.
//---------------------------------------------------------------------------

class MVList : public MVListBase {
protected:
    struct CellItem {
        const char  *text;          // Pointer to the cell's text
        uint        flags;          // Flags for the item

        ibool operator == (const CellItem& i)
            { return text == i.text && flags == i.flags; };
        };

    TCArray<CellItem>   cells;      // Array of cells;
    MVRect              dataBounds; // Bounds on number of items in the list

            // Method to clear all the items in the list
            void clearItems();

            // Method to find a reference to a cell
            CellItem& findCell(int i,int j)
            {
                return cells[i * (dataBounds.bottom()-dataBounds.top())
                               + j - dataBounds.left()];
            };

            // Method to draw a list cell
    virtual void drawCell(int i,int j,const MVRect& bounds);

            // Method to refresh a cell
    virtual void refreshCell(int i,int j);

public:
            // Constructor
            MVList(MGLDevCtx& dc,const MVRect& bounds,const MVPoint& cellSize,
                const MVRect& dataBounds = _MV_pointRect,
                MVScrollBar *vScroll = NULL,MVScrollBar *hScroll = NULL,
                uint flags = lsNormal);

            // Method to set the data bounds for the list
            void setDataBounds(const MVRect& bounds);

            // Method to select a list cell
    virtual void selectCell(int i,int j);

            // Method to de-select a list cell
    virtual void deselectCell(int i,int j);

            // Method to toggle a list cell
    virtual void toggleCell(int i,int j);

            // Method to dirty a list cell
    virtual void dirtyCell(int i,int j);

            // Method to set a specific cell's value
            void setCell(int i,int j,const char *text);

            // Method to get a specific cell's value, and flags
            ibool getCell(int i,int j,const char*& text);
    };

#endif  // __MVIS_MLIST_HPP
