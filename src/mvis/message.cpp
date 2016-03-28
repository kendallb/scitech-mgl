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
* Description:  Implementation for the message function.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mview.hpp"

/*----------------------------- Implementation ----------------------------*/

MVView *MV_message(MVView *view,ulong what,ulong command,void *info)
/****************************************************************************
*
* Function:     MV_message
* Parameters:   view    - View to send the message to
*               what    - Event code for the message
*               command - Command code for the message
*               info    - Pointer to optional information for the message
* Returns:      Pointer to the view that handled the message, NULL if none
*
* Description:  Sends a message to the specified view to be handled
*               immediately. This function does not return until the view
*               has finished processing the message.
*
****************************************************************************/
{
    if (view == NULL)
        return NULL;

    MVEvent event;
    event.which = view;
    event.what = what;
    event.message.command = command;
    event.message.infoPtr = info;

    view->setupOwnerViewport();
    view->handleEvent(event);
    view->resetViewport();

    // If the event was handled, the what field will be set to evNothing,
    // so return a pointer to the view that handled the message.
    if (event.what == evNothing)
        return (MVView*)event.message.infoPtr;
    return NULL;
}

void MV_postMessage(MVView *view,ulong what,ulong command,void *info)
/****************************************************************************
*
* Function:     MV_postMessage
* Parameters:   view    - View to send the message to
*               what    - Event code for the message
*               command - Command code for the message
*               info    - Pointer to optional information for the message
*
* Description:  Post a message to the application event queue to be handled
*               at a later date. This should be used for events that do
*               not need to be handled immediately, and may change the global
*               state of windows on the desktop. It will return immediately
*               and the message will be handled later.
*
*               If the view parameter is NULL, the message will be broadcast
*               to all views, otherwise it will be directed to a specific
*               view.
*
****************************************************************************/
{
    MVEvent event;
    event.which = view;
    event.what = what;
    event.message.command = command;
    event.message.infoPtr = info;
    MV_eventQueue.post(event);
}
