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
* Description:  Header file for the MVRadioButton class.
*
****************************************************************************/

#ifndef __MVIS_MRADIO_HPP
#define __MVIS_MRADIO_HPP

#ifndef __MVIS_MBITBTN_HPP
#include "mvis/mbitbtn.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following is a class for representing standard radio buttons.
//---------------------------------------------------------------------------

class MVRadioButton : public MVSysBitmapButton {
protected:
            // Internal routine to send clicked message to owner
    virtual void buttonClicked();

public:
            // Constructor
            MVRadioButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,uint flags = bfNormal,
                ibool selected = false);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);
    };

#endif  // __MVIS_MRADIO_HPP
