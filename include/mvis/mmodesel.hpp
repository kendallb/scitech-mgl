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
* Description:  Header file for the MVModeSelector class. This class is used
*               to represent a dialog box for selecting a valid new
*               video mode to put the system into.
*
****************************************************************************/

#ifndef __MVIS_MMODESEL_HPP
#define __MVIS_MMODESEL_HPP

#ifndef __MVIS_MDIALOG_HPP
#include "mvis/mdialog.hpp"
#endif

#ifndef __MVIS_MINPUTL_HPP
#include "mvis/minputl.hpp"
#endif

#ifndef __MVIS_MLIST_HPP
#include "mvis/mlist.hpp"
#endif

#ifndef __TCL_ARRAY_HPP
#include "tcl/array.hpp"
#endif

#ifndef __TCL_STR_HPP
#include "tcl/str.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVModeSelector class is designed to provide a standard mechanism for
// allowing users to select a new video mode.
//
// Returns cmOK if a valid selection was made.
//---------------------------------------------------------------------------

class MVModeSelector : public MVDialog {
protected:
    int                 mode;           // Current video mode number
    MVList              *modeList;      // List of video modes
    TCISArray<TCDynStr> modenames;      // Array of video mode names

    static const char *setText;     // Text for standard buttons
    static const char *helpText;
    static const char *cancelText;
    static const char *modeListText;

public:
            // Constructor
            MVModeSelector(MGLDevCtx &dc,const char *title,int mode);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase);

            // Method to obtain the selected video mode
            int getMode();
    };

#endif  // __MVIS_MMODESEL_HPP
