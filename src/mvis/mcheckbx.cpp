/****************************************************************************
*
*                    MegaVision Application Framework
*
*           A C++ GUI Toolkit for the MegaGraph Graphics Library
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
* Environment:  IBM PC (MS DOS)
*
* Description:  Member functions for the MVCheckBox class
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mcheckbx.hpp"

/*----------------------------- Implementation ----------------------------*/

static int checkIndexes[32] = {
    sbCheck,                        // normal
    sbCheckSel,                     // selected
    sbCheckDown,                    // pressed
    sbCheckDownSel,                 // pressed selected
    sbCheckDisable,                 // disabled
    sbCheckDisableSel,              // disabled selected
    sbCheckDisable,                 // disabled pressed
    sbCheckDisableSel,              // disabled pressed selected

    sbCheck,                        // focused normal
    sbCheckSel,                     // focused selected
    sbCheckDown,                    // focused pressed
    sbCheckDownSel,                 // focused pressed selected
    sbCheckDisable,                 // focused disabled
    sbCheckDisableSel,              // focused disabled selected
    sbCheckDisable,                 // focused disabled pressed
    sbCheckDisableSel,              // focused disabled pressed selected

    sbCheck,                        // default normal
    sbCheckSel,                     // default selected
    sbCheckDown,                    // default pressed
    sbCheckDownSel,                 // default pressed selected
    sbCheckDisable,                 // default disabled
    sbCheckDisableSel,              // default disabled selected
    sbCheckDisable,                 // default disabled pressed
    sbCheckDisableSel,              // default disabled pressed selected

    sbCheck,                        // default focused normal
    sbCheckSel,                     // default focused selected
    sbCheckDown,                    // default focused pressed
    sbCheckDownSel,                 // default focused pressed selected
    sbCheckDisable,                 // default focused disabled
    sbCheckDisableSel,              // default focused disabled selected
    sbCheckDisable,                 // default focused disabled pressed
    sbCheckDisableSel,              // default focused disabled pressed selected
    };

MVCheckBox::MVCheckBox(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,uint flags,ibool selected)
    : MVSysBitmapButton(dc,bounds,command,checkIndexes,flags | bfTwoState,
      selected)
/****************************************************************************
*
* Function:     MVCheckBox::MVCheckBox
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
****************************************************************************/
{
}
