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
* Description:  Module to implement the mouse and keyboard event collection
*               queue routines for the toolkit.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mevent.hpp"
#include <string.h>

/*--------------------------- Global Variables ----------------------------*/

MVEventQueue    MV_eventQueue;

/*----------------------------- Implementation ----------------------------*/

ibool MVEventQueue::post(const MVEvent& evt)
/****************************************************************************
*
* Function:     MVEventQueue::post
* Parameters:   evt - Event to post to the event queue
* Returns:      True if the event was posted, false if queue is full.
*
* Description:  Posts an event to the event queue. If the event is a double
*               click event, we simulate this by duplicating a mouse down
*               event.
*
****************************************************************************/
{
    if (evt.what & evKeyboard)
        return EVT_post(0,evt.what,evt.key.keyCode,evt.key.modifiers);

    if (evt.what & evMouse) {
        if (!EVT_post(0,evt.what,evt.mouse.buttons,evt.mouse.modifiers))
            return false;
        if (evt.mouse.doubleClick)
            return EVT_post(0,evt.what,evt.mouse.buttons,evt.mouse.modifiers);
        }
    return EVT_post((ulong)evt.which,evt.what,evt.message.command,evt.message.infoLong);
}
