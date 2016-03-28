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
* Description:  Header file for the MVProgram class. This class is the base
*               class for all program instances. You must subclass from
*               this to have a useable class.
*
****************************************************************************/

#ifndef __MVIS_MPROGRAM_HPP
#define __MVIS_MPROGRAM_HPP

#ifndef __MVIS_MFONTMGR_HPP
#include "mvis/mfontmgr.hpp"
#endif

#ifndef __MVIS_MGROUP_HPP
#include "mvis/mgroup.hpp"
#endif

#ifndef __MVIS_MMENUBAR_HPP
#include "mvis/mmenubar.hpp"
#endif

#ifndef __MVIS_MDESKTOP_HPP
#include "mvis/mdesktop.hpp"
#endif

class MVStatusBar;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVProgInit class is used to initialise the pointers to create the
// menu bar, status line and desktop. It is required to get around the
// problem of calling virtual functions from within superclass constuctors.
//---------------------------------------------------------------------------

class MVProgInit {
protected:
    MVStatusBar *(*createStatusLine)(MGLDevCtx& dc,const MVRect& bounds);
    MVMenuBar       *(*createMenuBar)(MGLDevCtx& dc,const MVRect& bounds);
    MVDeskTop       *(*createDeskTop)(MGLDevCtx& dc,const MVRect& bounds);

public:
            MVProgInit(
                MVStatusBar *(*createStatusLine)(MGLDevCtx& dc,const MVRect& bounds),
                MVMenuBar *(*createMenuBar)(MGLDevCtx& dc,const MVRect& bounds),
                MVDeskTop *(*createDeskTop)(MGLDevCtx& dc,const MVRect& bounds))
                : createStatusLine(createStatusLine),
                  createMenuBar(createMenuBar),
                  createDeskTop(createDeskTop) {};
    };

//---------------------------------------------------------------------------
// The MVProgram class is the base class for all application programs
// written with the class library. It provides almost everything that you
// need to put together the pieces of a standard application.
//---------------------------------------------------------------------------

class MVProgram : public MVGroup, public virtual MVProgInit {
protected:
    MVEvent     pending;        // Next pending message event (if any)
    MGLRegion   invalid;        // Current update region for repainting

            // Method to perform a repaint operation
            void doRepaint();

            // Virtual function called to exit the program
    virtual ibool exitProgram();

public:
            // Constructor
            MVProgram(MGLDevCtx& dc,int systemFont = fmHelvetica,
                int paletteType = ptNormalPalette);

            // Destructor
            ~MVProgram();

            // Virtual to determine if the program was constructed correctly
    virtual ibool valid(ulong command);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Methods to retrieve and post events
    virtual void putEvent(MVEvent& event);
    virtual ibool getEvent(MVEvent& event,ulong mask = evAll);
    virtual ibool peekEvent(MVEvent& event,ulong mask = evAll);

            // Method to invalidate the specified rectangle
    virtual void invalidRect(MVRect& rect);

            // Methods to validate the specified rectangle
    virtual void validRect(MVRect& rect);

            // Member function to begin processing events for program
            uint run();

            // Members to suspend/resume program
    virtual void suspend();
    virtual void resume();

            // Virtual function called during idle moments
    virtual void idle();

            // Member function called when memory is exhausted
    virtual void outOfMemory();

            // Method to handle unhandled events
    virtual void eventError(MVEvent& event);

            // Method to quick paint to entire program display
            void quickRepaint(void);

            // Default methods to create the menubar
    static  MVStatusBar *initStatusBar(MGLDevCtx& dc,const MVRect& bounds);
    static  MVMenuBar *initMenuBar(MGLDevCtx& dc,const MVRect& bounds);
    static  MVDeskTop *initDeskTop(MGLDevCtx& dc,const MVRect& bounds);

    // Publicly accessible global variables
    static MVProgram    *application;   // Pointer to application instance
    static MVStatusBar  *statusBar;     // Program's status
    static MVMenuBar    *menuBar;       // Program's menu bar
    static MVDeskTop    *deskTop;       // Program's desktop
    };

#endif  // __MVIS_MPROGRAM_HPP

