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
* Description:  Header file for the MVTitleBar class. This class is used
*               to represent the title bar for the window class.
*
****************************************************************************/

#ifndef __MVIS_MTITLEB_HPP
#define __MVIS_MTITLEB_HPP

#ifndef __MVIS_MTBBTN_HPP
#include "mvis/mtbbtn.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVTitleBar class is used to represent the title bar drawn at the top
// of the window.
//---------------------------------------------------------------------------

class MVWindow;

class MVTitleBar : public MVGroup {
    char            *title;             // Title text
    MVWindow        *owner;             // Window that owns this titlebar
    MVTBButton      *minimiseBox;       // Minimise box button
    MVTBButton      *maximiseBox;       // Maximise box button
    MVTBButton      *closeBox;          // Close box button
    MVPoint         start;              // Place to start drawing title
    MVTextJust      tjust;              // Text justification

public:
            // Constructor
            MVTitleBar(MGLDevCtx& dc,const MVRect& bounds,const char *title,
                MVWindow *owner,uint flags);

            // Destructor
            ~MVTitleBar()   { delete [] title; };

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase);

            // Method to set the bounds of the view
    virtual void setBounds(const MVRect& bounds);

            // Method to draw the title bar
    virtual void drawBackground(const MVRect& dirty);
    };

#endif  // __MVIS_MTITLEBAR_HPP
