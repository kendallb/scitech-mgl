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
* Description:  Header file for the MVDialog class. This class is used
*               to group together a number of views into a pop-up box
*               used for displaying messages etc.
*
****************************************************************************/

#ifndef __MVIS_MDIALOG_HPP
#define __MVIS_MDIALOG_HPP

#ifndef __MVIS_MWINDOW_HPP
#include "mvis/mwindow.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

const uint

// Default MVDialog flags

    wfNormalDialog  = wfMoveable | wfSystemMenu | wfClose | wfFramed
                        | wfLeftJust;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVDialog class is a type of window that is designed to contain
// buttons and other interactive views. By default MVDialogs are moveable
// but not-resizeable and modal.
//---------------------------------------------------------------------------

class MVDialog : public MVWindow {
public:
            // Constructor
            MVDialog(MGLDevCtx& dc,const MVRect& bounds,const char *title = NULL,
                uint flags = wfNormalDialog);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Virtual to determine if the dialog has finished
    virtual ibool valid(ulong command);
    };

#endif  // __MVIS_MDIALOG_HPP
