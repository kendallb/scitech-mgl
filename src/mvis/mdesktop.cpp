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
* Description:  Member functions for the TProgram class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mdesktop.hpp"

/*----------------------------- Implementation ----------------------------*/

MVDeskTop::MVDeskTop(MGLDevCtx& dc,const MVRect& bounds,
    MVBackGround *backGround)
    : MVGroup(dc,bounds), backGround(backGround)
/****************************************************************************
*
* Function:     MVDeskTop::MVDeskTop
* Parameters:   bounds      - Bounding rectangle for the desktop
*               backGround  - Pointer to background view to use
*
* Description:
*
****************************************************************************/
{
    if (backGround)
        insert(backGround);
}

void MVDeskTop::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVDeskTop::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the TDesktop class. Here we
*               handle the events to move the active window.
*
****************************************************************************/
{
    MVGroup::handleEvent(event,phase);

    if (event.what == evCommand) {
        switch (event.message.command) {
            case cmMoveToFront:
                MVView *view = (MVView*)event.message.infoPtr;
                if (firstView() != view) {
                    remove(view);
                    insert(view);
                    }
                select(view);
                break;

            // Here we handle the cmNext, cmPrev messages.

            }
        }
}

void MVDeskTop::drawBackground(const MVRect& clip)
/****************************************************************************
*
* Function:     MVDeskTop::drawBackground
* Parameters:   clip    - Clipping rectangle for the view.
*
* Description:  Draws the desktop's background if present.
*
****************************************************************************/
{
    if (backGround)
        backGround->draw(clip);
}
