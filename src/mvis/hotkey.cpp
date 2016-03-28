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
* Description:  Implementation for the MVHotKey class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mkeys.hpp"

ibool MVHotKey::operator == (const MVKeyEvent& event) const
/****************************************************************************
*
* Function:     MVHotKey::operator ==
* Parameters:   event   - Keyboard event to check
* Returns:      True if the hotkey value is present
*
****************************************************************************/
{
    if ((uint)event.charScan.scanCode != scanCode)
        return false;

    // Check that any shift key is down
    if (modifiers & mdShift) {
        if (!(event.modifiers & mdShift))
            return false;
        }

    // Check if any Alt key is down
    if (modifiers & mdAlt) {
        if (!(event.modifiers & mdAlt))
            return false;
        }

    // Check if any Ctrl key is down
    if (modifiers & mdCtrl) {
        if (!(event.modifiers & mdCtrl))
            return false;
        }

    // All conditions are true for this hot key!
    return true;
}
