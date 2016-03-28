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
* Description:  Member functions for the MVRenderArea class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mrendera.hpp"
#include "mvis/mprogram.hpp"
#include "tcl/techlib.hpp"

/*----------------------------- Implementation ----------------------------*/

MVRenderArea::MVRenderArea(MGLDevCtx& dc,const MVRect& bounds)
    : MVView(dc,bounds)
/****************************************************************************
*
* Function:     MVRenderArea::MVRenderArea
* Parameters:   bounds  - Bounding box for the render area
*
* Description:  Constructor for the MVRenderArea class.
*
****************************************************************************/
{
    CHECK(MGL_getActivePage(dc.getDC()) == 0 && MGL_getVisualPage(dc.getDC()) == 0);
    doubleBuffered = false;
}

MVRenderArea *MVRenderArea::clipAndRenderView;

void MVRenderArea::_clipAndRender(const rect_t *r)
{
    rect_t  offsetR = *r;
    MGL_offsetRect(offsetR,
        -clipAndRenderView->bounds.left(),
        -clipAndRenderView->bounds.top());
    clipAndRenderView->dc.setClipRect(offsetR);
    clipAndRenderView->render();
}

static void _ASMAPI clipAndRender(const rect_t *r)
{ MVRenderArea::_clipAndRender(r); }

void MVRenderArea::paint()
/****************************************************************************
*
* Function:     MVRenderArea::paint
*
* Description:  This function paints the view with the current clip
*               region. We set the viewport to the coordinate space for
*               the view and then call the render function clipping to
*               each of the clip rectangles in the clip region.
*
****************************************************************************/
{
    MS_obscure();
    setupViewport();
    clipAndRenderView = this;
    clipRgn.traverse(clipAndRender);
    resetViewport();
    MS_show();
}

void MVRenderArea::doubleBufferOn()
/****************************************************************************
*
* Function:     MVRenderArea::doubleBufferOn
*
* Description:  Turns on double buffering for the render area. This member
*               function should be called just before peforming any
*               animation in the render area.
*
****************************************************************************/
{
    if (dc.doubleBuffer()) {
        MVEvent  evt;

        // Force a quick repaint onto the hidden display page
        doubleBuffered = true;
        MVProgram::application->quickRepaint();
        getEvent(evt,evRepaint);
        }
}

void MVRenderArea::doubleBufferOff()
/****************************************************************************
*
* Function:     MVRenderArea::doubleBufferOff
*
* Description:  Turns off double buffering. The visual display page will
*               not necessarily be zero after this.
*
****************************************************************************/
{
    dc.singleBuffer();
}
