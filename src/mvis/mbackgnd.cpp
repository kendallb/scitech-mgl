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
#include "mvis/mbackgnd.hpp"
#include <string.h>

/*----------------------------- Implementation ----------------------------*/

MVBackGround::MVBackGround(MGLDevCtx& dc,const MVRect& bounds,int fillStyle,
    pattern_t *pat)
    : MVView(dc,bounds), fillStyle(fillStyle)
/****************************************************************************
*
* Function:     MVBackGround::MVBackGround
* Parameters:   bounds      - Bounding box for the background
*               fillStyle   - Style to fill the background with
*               pat         - Pattern to fill the background with
*
* Description:  Constructor for the background class.
*
****************************************************************************/
{
    MVBackGround::pat = *pat;
}

MVBackGround::~MVBackGround()
{
}

void MVBackGround::draw(const MVRect& dirty)
/****************************************************************************
*
* Function:     MVBackGround::draw
* Parameters:   clip    - Clip rectangle for the view
*
* Description:  Method to draw the background view.
*
****************************************************************************/
{
    int oldPenStyle = dc.getPenStyle();
    dc.setBackColor(getColor(scDesktopBackground));
    dc.setColor(getColor(scDesktopForeground));
    dc.setPenStyle(fillStyle);
    dc.setPenBitmapPattern(0,&pat);
    dc.usePenBitmapPattern(0);
    dc.fillRect(dirty);
    dc.setPenStyle(oldPenStyle);
}
