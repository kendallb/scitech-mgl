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
* Description:  Member functions for the MVRadioButton class
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/msgbox.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mradio.hpp"

/*----------------------------- Implementation ----------------------------*/

static int radioIndexes[32] = {
    sbRadio,                        // normal
    sbRadioSel,                     // selected
    sbRadioDown,                    // pressed
    sbRadioDownSel,                 // pressed selected
    sbRadioDisable,                 // disabled
    sbRadioDisableSel,              // disabled selected
    sbRadioDisable,                 // disabled pressed
    sbRadioDisableSel,              // disabled pressed selected

    sbRadio,                        // focused normal
    sbRadioSel,                     // focused selected
    sbRadioDown,                    // focused pressed
    sbRadioDownSel,                 // focused pressed selected
    sbRadioDisable,                 // focused disabled
    sbRadioDisableSel,              // focused disabled selected
    sbRadioDisable,                 // focused disabled pressed
    sbRadioDisableSel,              // focused disabled pressed selected

    sbRadio,                        // default normal
    sbRadioSel,                     // default selected
    sbRadioDown,                    // default pressed
    sbRadioDownSel,                 // default pressed selected
    sbRadioDisable,                 // default disabled
    sbRadioDisableSel,              // default disabled selected
    sbRadioDisable,                 // default disabled pressed
    sbRadioDisableSel,              // default disabled pressed selected

    sbRadio,                        // default focused normal
    sbRadioSel,                     // default focused selected
    sbRadioDown,                    // default focused pressed
    sbRadioDownSel,                 // default focused pressed selected
    sbRadioDisable,                 // default focused disabled
    sbRadioDisableSel,              // default focused disabled selected
    sbRadioDisable,                 // default focused disabled pressed
    sbRadioDisableSel,              // default focused disabled pressed selected
    };

MVRadioButton::MVRadioButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,uint flags,ibool selected)
    : MVSysBitmapButton(dc,bounds,command,radioIndexes,flags | bfSelectable,
      selected)
/****************************************************************************
*
* Function:     MVRadioButton::MVRadioButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
****************************************************************************/
{
}

void MVRadioButton::buttonClicked()
/****************************************************************************
*
* Function:     MVRadioButton::buttonClicked
*
* Description:  Send both button clocked and radio selected messages to
*               owner window.
*
****************************************************************************/
{
    MV_message(owner,evBroadcast,cmButtonClicked,this);
    MV_message(owner,evBroadcast,cmRadioSelected,this);
}

void MVRadioButton::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVRadioButton::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for radio buttons.
*
****************************************************************************/
{
    MVSysBitmapButton::handleEvent(event,phase);

    /* Handle auto-matic deselection of radio buttons when a new one is
     * selected in the group.
     */
    if (event.what == evBroadcast && event.message.command == cmRadioSelected) {
        if ((MVView*)event.message.infoPtr != this) {
            if (selected) {
                selected = false;
                draw(bounds);
                }
            }
        }
}
