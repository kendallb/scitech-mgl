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
* Description:  Header file for the MVFrame class. This class is used
*               to represent the frame around the outside of dialog
*               boxes and windows.
*
****************************************************************************/

#ifndef __MVIS_MFRAME_HPP
#define __MVIS_MFRAME_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVFrame class is used to represent the frames drawn around the
// outside of dialog boxes and windows.
//
// If the width of the frame is less than 3, then only a flat black frame
// will be used.
//---------------------------------------------------------------------------

class MVFrame : public MVView {
    int     width;                  // Width of the frame
    uint    flags;                  // Window creation flags
    MVRect  inside;                 // Inner rectangle for frame
    int     where;                  // Classification for last mouse click

    // Cursor classification codes
    enum {
        Inside,
        Left,
        Top,
        Right,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
        };

            // Classify where a point is in relation to frame
            int classifyPoint(const MVPoint& p);

public:
            // Constructor
            MVFrame(MGLDevCtx& dc,const MVRect& bounds,int width,
                uint flags);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to draw the frame
    virtual void draw(const MVRect& dirty);

            // Method to set the bounds of the view
    virtual void setBounds(const MVRect& bounds);

            // Method to change the cursor
    virtual MGLCursor *getCursor(const MVPoint& p);

            // Method to return the inner rectangle()
            MVRect inner();

            // Method to return the frame width
            int getWidth()  { return width; };

            // Method to adjust the re-size rectangle given a delta value
            void resizeDragRect(int deltaX,int deltaY,MVRect &dragRect);
    };

#endif  // __MVIS_MFRAME_HPP
