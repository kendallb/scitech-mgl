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
* Description:  Member functions for the MVStaticText class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/msttext.hpp"
#include "mvis/mfontmgr.hpp"

/*----------------------------- Implementation ----------------------------*/

MVStaticText::MVStaticText(MGLDevCtx& dc,const MVRect& bounds,
    const char *text,uint flags,int color)
    : MVView(dc,bounds), text(text), textColor(color), flags(flags)
/****************************************************************************
*
* Function:     MVStaticText::MVStaticText
* Parameters:   bounds  - Bounding box for the text
*               text    - Pointer to the text to display
*               flags   - Static text creation flags
*
* Description:  Constructor for the MVStaticText class. Note that the
*               text is _not_ copied onto the heap, but is simply
*               used as a reference. This is more efficient, but also
*               allows the text to be quickly set to another value by
*               pointing it somewhere else.
*
****************************************************************************/
{
    setText(text);
}

void MVStaticText::setBounds(const MVRect& b)
/****************************************************************************
*
* Function:     MVStaticText::setBounds
* Parameters:   bounds  - New bounding box for the item.
*
* Description:  Sets the bounds for the text item. Here we precalculate the
*               position to display the text in the box.
*
****************************************************************************/
{
    MVView::setBounds(b);
    setText(text);
}

const char *MVStaticText::setText(const char *t)
/****************************************************************************
*
* Function:     MVStaticText::setText
* Parameters:   t   - New string for the text
* Returns:      Pointer to old text.
*
* Description:  Sets the text for the static text item, and also
*               re-computes the position to draw the text at.
*
****************************************************************************/
{
    const char *ret = text;
    text = t;

    // Set the current font and size, and obtain the fonts metrics
    metrics_t   m;
    useFont(fmSystemFont);
    dc.getFontMetrics(m);

    // Compute the location to draw the text at
    int hjust = (flags & sfLeftJust ? MGL_LEFT_TEXT :
                (flags & sfRightJust ? MGL_RIGHT_TEXT : MGL_CENTER_TEXT));
    int vjust = (flags & sfTopJust ? MGL_TOP_TEXT :
                (flags & sfBottomJust ? MGL_BOTTOM_TEXT : MGL_CENTER_TEXT));
    tjust.setJustification(hjust,vjust);

    MVRect r(bounds);
    r.moveTo(0,0);

    start.x =   (hjust == MGL_LEFT_TEXT ?    r.left() :
                (hjust == MGL_CENTER_TEXT ? (r.left() + r.right())/2 :
                                         r.right()-1));
    start.y =   (vjust == MGL_TOP_TEXT ?     r.top() :
                (vjust == MGL_CENTER_TEXT ? (r.top() + r.bottom())/2 :
                                         r.bottom()-1 + m.descent));
    repaint();
    return ret;
}

void MVStaticText::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVStaticText::draw
* Parameters:   clip    - Clipping rectangle for the view.
*
* Description:  Draw's the static text item.
*
****************************************************************************/
{
    MVTextJust old;
    old.save(dc);
    tjust.use(dc);
    useFont(fmSystemFont);
    dc.setColor(getColor(scBackground));
    dc.fillRect(bounds);
    dc.setColor(getColor(textColor));
    dc.drawStr(start.x + bounds.left(),
               start.y + bounds.top(),text);

    old.use(dc);
}
