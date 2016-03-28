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
* Description:  Member functions for the class DemoWindow, a class
*               performing each of the graphics library demos in the
*               current window.
*
****************************************************************************/

#include "demo.hpp"
#include "demowind.hpp"
#include "demoarea.hpp"

/*----------------------------- Implementation ----------------------------*/

DemoWindow::DemoWindow(MGLDevCtx &dc,const MVRect& bounds)
    : MVDialog(dc,bounds,NULL,wfFramed),
      MVWindowInit(MVWindow::initFrame,MVWindow::initTitleBar)
/****************************************************************************
*
* Function:     DemoWindow::DemoWindow
* Parameters:   bounds      - Bounding box for the window
*
* Description:  Constructor for the DemoWindow class. The window is used
*               to render each of the demos currently being executed.
*
****************************************************************************/
{
    options &= ~(ofSelectable | ofFirstClick);

    MVRect r(getClientRect());
    r.offset(-bounds.left(),-bounds.top());
    insert(new DemoArea(dc,r));
}

void DemoWindow::drawBackground(const MVRect&)
/****************************************************************************
*
* Function:     DemoWindow::drawBackground
*
* Description:  Draws the background for the window. Since the window is
*               completely covered by the rendering area, we do not
*               draw the window background, just the frame around the
*               demo window.
*
****************************************************************************/
{
}
