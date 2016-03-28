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
* Description:  Member functions for the MVDialog class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mdialog.hpp"
#include "mvis/mkeys.hpp"

/*----------------------------- Implementation ----------------------------*/

MVDialog::MVDialog(MGLDevCtx& dc,const MVRect& bounds,const char *title,
    uint flags)
    : MVWindow(dc,bounds,title,flags),
      MVWindowInit(MVWindow::initFrame,MVWindow::initTitleBar)
/****************************************************************************
*
* Function:     MVDialog::MVDialog
* Parameters:   bounds  - Bounding box for the dialog
*               title   - Title text for the dialog (NULL for no title bar)
*               tjust   - Text justification for title bar
*               flags   - Window creation flags
*
* Description:  Constructor for the MVDialog class.
*
****************************************************************************/
{
}

void MVDialog::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVDialog::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the MVDialog class. Here we
*               handle events normally for the group, and also check to
*               see if the dialog box should be ended.
*
***************************************************************************/
{
    MVWindow::handleEvent(event,phase);

    switch (event.what) {
        case evKeyDown:
            switch (event.key.charScan.scanCode) {
                case kbEsc:
                    // The escape key was pressed, so post a cancel message
                    // to kill the dialog box.
                    event.what = evCommand;
                    event.message.command = cmCancel;
                    event.message.infoPtr = NULL;
                    putEvent(event);
                    clearEvent(event);
                    break;
                case kbEnter:
                case kbGrayEnter:
                    // The enter key was pressed, so post a cmDefault
                    // message to be processed by the default view (must
                    // be a broadcast as we do not know who is the
                    // default view.
                    event.what = evBroadcast;
                    event.message.command = cmDefault;
                    event.message.infoPtr = NULL;
                    putEvent(event);
                    clearEvent(event);
                    break;
                }
            break;
        case evCommand:
            if (event.message.command == cmClose && event.which == this)
                event.message.command = cmCancel;
            switch (event.message.command) {
                case cmYes:
                case cmNo:
                case cmOk:
                case cmCancel:
                    // One of the standard dialog commands was posted,
                    // so attempt to end the modal view with the command.
                    if (state & sfModal) {
                        endModal(event.message.command);
                        clearEvent(event);
                        }
                    break;
                }
            break;
        }
}

ibool MVDialog::valid(ulong command)
/****************************************************************************
*
* Function:     MVDialog::valid
* Parameters:   command - Command ending the modal operation
* Returns:      True if the dialog is valid
*
* Description:  Checks to see if the dialog is valid, or the modal view
*               should end.
*
****************************************************************************/
{
    if (command == cmCancel)
        return true;                    // Cancel always works
    else
        return MVGroup::valid(command);
}
