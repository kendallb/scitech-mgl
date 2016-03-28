/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Header file for the demo program's main class.
*
****************************************************************************/

#ifndef __DEMO_HPP
#define __DEMO_HPP

#include "mvis/mvision.hpp"
#include "mvis/mprogram.hpp"
#include "mvis/mmenubar.hpp"

/*------------------------------ Constants --------------------------------*/

// Application command codes

const uint
    cmNextDemo          = 1000,         // Move onto next demo
    cmPrevDemo          = 1001,         // Move back to prev demo
    cmAutoDemo          = 1002,         // Toggle automatic operation
    cmSetExplanation    = 1003,         // Set the explanation window text
    cmSetDemoTitle      = 1004,         // Set the demo title text

// Menu command codes

    cmAbout             = 100,          // Bring up the about box
    cmVideoMode         = 101,          // Change the current video mode
    cmLineDemo          = 102,
    cmEllipseDemo       = 103,
    cmArcDemo           = 104,
    cmPolygonDemo       = 105,
    cmRectangleDemo     = 106,
    cmColorDemo         = 107,
    cmPatternDemo       = 109,
    cmAnimationDemo     = 111,
    cmStatusDemo        = 112;

/*---------------------------- Global Variables ---------------------------*/

extern int  driver,mode,snowlevel;

/*-------------------------- Class definitions ----------------------------*/

class Demo : public MVProgram {
protected:
            // Method to change the current video mode
            void changeVideoMode();

public:
            // Constructor
            Demo(MGLDevCtx& dc);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Methods to create the menubar, status line and desktop etc
    static  MVMenuBar *initMenuBar(MGLDevCtx& dc,const MVRect& bounds);
    static  MVDeskTop *initDeskTop(MGLDevCtx& dc,const MVRect& bounds);
    };

#endif  // __DEMO_HPP
