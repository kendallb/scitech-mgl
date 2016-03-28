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
* Description:  Header file for the TDeskTop class. This class is a
*               special MVGroup that maintains all of the views used by
*               the program, along with a simple background view if
*               specified.
*
****************************************************************************/

#ifndef __MVIS_MDESKTOP_HPP
#define __MVIS_MDESKTOP_HPP

#ifndef __MVIS_MGROUP_HPP
#include "mvis/mgroup.hpp"
#endif

#ifndef __MVIS_MBACKGND_HPP
#include "mvis/mbackgnd.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVDeskTop class is a special MVGroup to group together all views used
// by the main program.
//---------------------------------------------------------------------------

class MVDeskTop : public MVGroup {
protected:
    MVBackGround    *backGround;

            // Method to draw the background
    virtual void drawBackground(const MVRect& dirty);

public:
            // Constructor
            MVDeskTop(MGLDevCtx& dc,const MVRect& bounds,
                MVBackGround *backGround = NULL);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);
    };

#endif  // __MVIS_MESKTOP_HPP
