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
* Description:  Implementation of the viewport stack. This code used to be
*               in the MGL, but was removed for the MGL 2.0 so we implement
*               some compatability routines in here using a the C++ stack
*               class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "tcl/cstack.hpp"

/*----------------------------- Implementation ----------------------------*/

struct ClipAndView {
    MVRect  view;           // Original viewport before save
    MVRect  clip;           // Original clip rectangle before save
    };

static TCStaticCStack<ClipAndView,20>   viewStack;
static TCStaticCStack<MVRect,20>        clipStack;

void MV_pushViewport(MGLDevCtx& dc)
/****************************************************************************
*
* Function:     MV_pushViewport
*
* Description:  Pushes the currently active viewport onto the stack. If the
*               viewport stack overflows, we bomb out with an error message.
*
****************************************************************************/
{
    ClipAndView v;

    if (viewStack.isFull())
        MGL_fatalError("Viewport stack overflow!");
    dc.getViewport(v.view);
    dc.getClipRect(v.clip);
    viewStack.pushq(v);
}

void MV_popViewport(MGLDevCtx& dc)
/****************************************************************************
*
* Function:     MV_popViewport
*
* Description:  Pops the viewport off the top of the stack and makes it
*               the new active viewport. If the viewport stack underflows,
*               we bomb out with an error message. Note that we also
*               restore the original clipping rectangle as well.
*
****************************************************************************/
{
    if (viewStack.isEmpty())
        MGL_fatalError("Viewport stack underflow!");
    ClipAndView v(viewStack.popq());
    dc.setViewport(v.view);
    dc.setClipRect(v.clip);
}

void MV_pushClipRect(MGLDevCtx& dc)
/****************************************************************************
*
* Function:     MV_pushClipRect
*
* Description:  Pushes the currently active clip rectangle onto the stack
*               individually to the viewport. If the ClipRect stack
*               overflows, we bomb out with an error message.
*
****************************************************************************/
{
    MVRect  clip;

    if (clipStack.isFull())
        MGL_fatalError("ClipRect stack overflow!");
    dc.getClipRect(clip);
    clipStack.pushq(clip);
}

void MV_popClipRect(MGLDevCtx& dc)
/****************************************************************************
*
* Function:     MV_popClipRect
*
* Description:  Pops the ClipRect off the top of the stack and makes it
*               the new active ClipRect. If the ClipRect stack underflows,
*               we bomb out with an error message. Note that we also
*               restore the original clipping rectangle as well.
*
****************************************************************************/
{
    if (clipStack.isEmpty())
        MGL_fatalError("ClipRect stack underflow!");
    dc.setClipRect(clipStack.popq());
}
