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
* Description:  Header file for the MVInputLine class. This class is used
*               to obtain a single line of text from the user.
*
****************************************************************************/

#ifndef __MVIS_MINPUTL_HPP
#define __MVIS_MINPUTL_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

    tlCursorVisible = 0x0001,       // Cursor is visible
    tlOverwriteMode = 0x0002,       // Key's overwrite the text
    tlDirty         = 0x0004;       // The input line is dirty (needs drawing)

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVInputLine class is used to obtain a single line of text from the
// user, up to a maximum specified length. Scrolling is supported if the
// line is too long to fit within the view. Note that the buffer passed
// to the input line routine will need to be large enough to store the
// edited text.
//
// MVInputLine's are intended to be inserted into TDialog's.
//---------------------------------------------------------------------------

class MVInputLine : public MVView {
protected:
    char    *text;              // Text being edited
    int     maxLen;             // Maximum length for input line
    int     len;                // Current text length
    int     curPos;             // Current insertion position (index)
    int     curLoc;             // Current cursor location (coordinate)
    int     firstPos;           // Index of first displayed character
    int     selStart;           // Index of first selected character
    int     selEnd;             // Index of last selected character
    int     starty;             // Starting y coordinate for text
    int     scrollChars;        // Number of characters to scroll
    uint    flags;              // Flags for the input line
    long    oldTicks;           // Old tick count
    long    cursorRate;         // Rate of cursor flashing

            // Method to reposition to text within the window
            void repositionText();

            // Method to clear the highlight
            void clearHighlight();

            // Method to delete a number of characters from the text
            void deleteChars(int start,int count);

            // Method to insert a character into the text
            void insertChar(char ch);

            // Delete the selected characters
            ibool deleteHighlight();

            // Method to find a character position given a coordinate
            int findPos(const MVPoint& p);

            // Method to select the next left character
            void selectLeft(int count,ulong modifiers);

            // Method to select the next right character
            void selectRight(int count,ulong modifiers);

            // Method to repaint the display
            void repaint(const MVRect& dirty);

            // Method to refresh the display (calls repaint if necessary)
            void refresh();

            // Method to draw the cursor
            void drawCursor(ibool visible);

public:
            // Constructor
            MVInputLine(MGLDevCtx& dc,const MVRect& bounds,int maxLen,
                const char *defaultText = NULL);

            // Virtual destructor
    virtual ~MVInputLine();

            // Method to set the bounds for the input line
    virtual void setBounds(const MVRect& bounds);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase);

            // Method to draw the input line
    virtual void draw(const MVRect& dirty);

            // Method called when idle
    virtual void idle();

            // Method to change the cursor
    virtual MGLCursor *getCursor(const MVPoint& p);

            // Method to set a state flag
    virtual void setState(uint state,ibool set);

            // Method to get the text from the input line
            const char *getText() const { return text; };

            // Method to set the text for the input line
            void setText(const char *text);

            // Method to set the cursor flash rate
            void setCursorRate(long rate)   { cursorRate = rate; };
            long getCursorRate()            { return cursorRate; };
    };

#endif  // __MVIS_MINPUTL_HPP
