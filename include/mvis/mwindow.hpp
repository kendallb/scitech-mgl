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
* Description:  Header file for the MVWindow class. This class is used
*               to represent pop-up windows used by TDialogs etc.
*
****************************************************************************/

#ifndef __MVIS_MWINDOW_HPP
#define __MVIS_MWINDOW_HPP

#ifndef __MVIS_MGROUP_HPP
#include "mvis/mgroup.hpp"
#endif

#ifndef __MVIS_MFRAME_HPP
#include "mvis/mframe.hpp"
#endif

#ifndef __MVIS_MTITLEB_HPP
#include "mvis/mtitleb.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

const uint

// MVWindow creation flag masks

    wfMoveable      = 0x0001,       // Window can be moved
    wfResizeable    = 0x0002,       // Window can be resized
    wfSystemMenu    = 0x0004,       // Window has a system menu
    wfClose         = 0x0008,       // Window has a close icon
    wfMinimise      = 0x0010,       // Window has a minimise icon
    wfMaximise      = 0x0020,       // Window has a maximise icon
    wfFramed        = 0x0040,       // Window has a 3d frame
    wfLeftJust      = 0x0080,       // Window title is left justified
    wfRightJust     = 0x0100,       // Window title is right justified
    wfTopJust       = 0x0200,       // Window title is top justified
    wfBottomJust    = 0x0400,       // Window title is bottom justified
    wfFixedWin      = wfSystemMenu | wfClose | wfFramed | wfLeftJust,
    wfMoveableWin   = wfMoveable | wfSystemMenu | wfClose | wfMinimise
                        | wfFramed | wfLeftJust,
    wfNormal        = wfMoveable | wfResizeable | wfSystemMenu | wfClose
                        | wfMinimise | wfMaximise | wfFramed | wfLeftJust;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVWindowInit class is used to create a class to hold a pointer to a
// frame creation routine. We need this to solve the problem with virtual
// function calls from within a superclass's constructor.
//---------------------------------------------------------------------------

class MVWindowInit {
protected:
    MVFrame     *(*createFrame)(MGLDevCtx& dc,const MVRect& bounds,int,uint);
    MVTitleBar  *(*createTitleBar)(MGLDevCtx& dc,const MVRect& bounds,
                    const char *title,MVWindow *,uint flags);

public:
            MVWindowInit(
                MVFrame *(*createFrame)(MGLDevCtx& dc,const MVRect&,int,uint),
                MVTitleBar *(*createTitleBar)(MGLDevCtx& dc,const MVRect&,
                    const char*,MVWindow *,uint))
                : createFrame(createFrame),
                  createTitleBar(createTitleBar) {};
    };

//---------------------------------------------------------------------------
// The MVWindow class is special group that maintains a collection of views
// in a moveable and re-sizeable window. MVWindow's are modeless by default.
//
// The frame and title for the window are within the main window bounds.
//---------------------------------------------------------------------------

class MVWindow : public MVGroup, public virtual MVWindowInit {
protected:
    MVFrame     *frame;             // Frame for the window
    MVTitleBar  *titleBar;          // Title bar for the window
    MVRect      normalBounds;       // Normal bounds for window
    ibool       maximised;          // True if window is maximised
    ibool       minimised;          // True if window is minimised
    int         number;             // Number of this window
    uint        flags;              // Flags the type of window

    // TODO:    icon;               // Icon to display when minimised

            // Method to draw the window's background
    virtual void drawBackground(const MVRect& dirty);

            // Track the movement of the window bounds
            void trackMovement(MVEvent& event,MVRect& bounds);

            // Track resizing of window bounds
            void trackResize(MVEvent& event,MVRect& bounds);
public:
            // Constructor
            MVWindow(MGLDevCtx& dc,const MVRect& bounds,
                const char *title = NULL,uint flags = wfNormal,
                int frameWidth = 4);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to set the bounds of the view
    virtual void setBounds(const MVRect& bounds);

            // Method to move the window to another location
    virtual void moveTo(int x,int y);

            // Method to move the window
            void moveWindow(MVEvent& event);

            // Method to resize the window
            void resizeWindow(MVEvent& event);

            // Method to minimise/maximise/restore the window size
    virtual void minimiseWindow();
    virtual void maximiseWindow();
    virtual void restoreWindow();

            // Determine if the window is minimised/maximised
            ibool isMinimised() { return minimised; };
            ibool isMaximised() { return maximised; };

            // Default creation routines for windows
    static  MVFrame* initFrame(MGLDevCtx& dc,const MVRect& bounds,
                int width,uint flags);
    static  MVTitleBar *initTitleBar(MGLDevCtx& dc,const MVRect& bounds,
                const char *title,MVWindow *owner,uint flags);
    };

    // TODO: Overload the draw() or paint() function to handle windows
    // minimised to the owner drawn as an icon in owner client region

#endif  // __MVIS_MWINDOW_HPP
