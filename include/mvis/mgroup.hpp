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
* Description:  Header file for the MVGroup class. This class is used
*               to group together a number of views together into a
*               single view.
*
****************************************************************************/

#ifndef __MVIS_MGROUP_HPP
#define __MVIS_MGROUP_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVGroup class is used to collect a number of views together into
// a single view.
//---------------------------------------------------------------------------

class MVGroup : public MVView {
protected:
    TCDList<MVView> children;       // List of all child views for group
    MVView          *selected;      // Pointer to currently selected view
    MVRect          clientRect;     // Bounds for client part of window
    ulong           endState;       // State ending the modal group
    ulong           modalState;     // State of the modal view (0 = active)

            // Internal method to process events
            void doEvent(MVView& view,MVEvent& event,phaseType phase);

            // Called when an event is unhandled
    virtual void eventError(MVEvent& event);

            // Get the next/prev view in the group in tab order
            MVView *getNext(MVView *view);
            MVView *getPrev(MVView *view);

            // Method to complete an insert operation
            void doInsert(MVView *view);

            // Method to draw the background
    virtual void drawBackground(const MVRect&) {};

            // Method to change the current cursor
            void changeCursor(const MVPoint& pos);

            // Method to paint the group to current clip region
    virtual void paint();

public:
            // Method to clip and draw the background
    static  void _drawBkgnd(const rect_t *r);

public:
            // Constructor
            MVGroup(MGLDevCtx& dc,const MVRect& bounds);

            // Virtual destructor
    virtual ~MVGroup();

            // Virtual to determine if the group was constructed correctly
    virtual ibool valid(ulong command);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to execute a specific group
    virtual uint execView(MVGroup *view);

            // Virtual to execute the modal group
    virtual uint execute();

            // Method to end the modal execution of the group
            void endModal(ulong command)    { endState = command; };

            // Methods to insert views into the group
            void insert(MVView *view);
            void insertAfter(MVView *view,MVView *after);

            // Method to remove a view from the group
            void remove(MVView *view);

            // Return a pointer to the first/last view in the group
            MVView *firstView() { return children.peekHead(); };
            MVView *lastView()  { return children.peekTail(); };

            // Moves the view to the front and selects it
            void moveToFront(MVView *v);

            // Selects the specified view in the group
            void select(MVView *v);

            // Selects the next/previous view in the group
            void selectNext();
            void selectPrev();

            // Method to set a state flag
    virtual void setState(uint state,ibool set);

            // Methods to draw the program (does nothing; we overload paint)
    virtual void draw(const MVRect&) {};

            // Method to set the bounds of the group
    virtual void setBounds(const MVRect& bounds);

            // Method to move the group to another location
    virtual void moveTo(int x,int y);

            // Method to return the client bounds for a window
            const MVRect& getClientRect()   { return clientRect; };

            // Method to find the currently active modal view
    virtual MVView *findModalView();

            // Method called when idle
    virtual void idle();

            // Method called to obtain the cursor definition
    virtual MGLCursor *getCursor(const MVPoint& p);

            // Method to obtain the help context number for the view
    virtual uint getHelpCtx() const;

            // Backwards compatability functions
            const MVRect& innerBounds()     { return getClientRect(); };
    };

#endif  // __MVIS_MGROUP_HPP
