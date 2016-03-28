/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Environment:  any
*
* Description:  Member functions for the class ExplanationWindow, a class
*               providing information about the current demo in progress.
*
****************************************************************************/

#include "demo.hpp"
#include "explwind.hpp"

/*----------------------------- Implementation ----------------------------*/

ExplanationWindow::ExplanationWindow(MGLDevCtx &dc,const MVRect& bounds)
    : MVDialog(dc,bounds,NULL,wfFramed),
      MVWindowInit(MVWindow::initFrame,MVWindow::initTitleBar)
/****************************************************************************
*
* Function:     ExplanationWindow::ExplanationWindow
* Parameters:   bounds      - Bounding box for the window
*
* Description:  Constructor for the ExplanationWindow class. The window will
*               be placed at the bottom of the bounding box passed to
*               this routine.
*
****************************************************************************/
{
    options &= ~(ofSelectable | ofFirstClick);

    useFont(fmSystemFont);

    // Adjust the bounds for the entire window

    MVRect  r(getClientRect());
    int     borderH = r.left() - bounds.left() + 1;
    int     borderV = r.top() - bounds.top() + 1;

    r.inset(1,1);
    r.top() = r.bottom() - dc.textHeight() * 3;
    r.inset(-borderH,-borderV);
    setBounds(r);

    getExtent(r);
    r.inset(borderH,borderV);

    explanation = new MVMultiLineText(dc,r,"");
    insert(explanation);
}

void ExplanationWindow::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     ExplanationWindow::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the ExplanationWindow class. Here
*               we handle events that change the text in the window.
*
****************************************************************************/
{
    if (event.what == evBroadcast) {
        if (event.message.command == cmSetExplanation) {
            explanation->setText((const char *)event.message.infoPtr);
            clearEvent(event);
            }
        }
}
