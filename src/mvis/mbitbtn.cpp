/****************************************************************************
*
*                    MegaVision Application Framework
*
*           A C++ GUI Toolkit for the MegaGraph Graphics Library
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
* Environment:  IBM PC (MS DOS)
*
* Description:  Member functions for the MVBitmapButton classes.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mbitbtn.hpp"

/*----------------------------- Implementation ----------------------------*/

MVSysBitmapButton::MVSysBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,int *indexes,uint flags,ibool selected)
    : MVButtonBase(dc,bounds,command,flags,selected)
/****************************************************************************
*
* Function:     MVSysBitmapButton::MVSysBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
****************************************************************************/
{
    for (int i = 0; i < 32; i++)
        index[i] = (char)indexes[i];
}

void MVSysBitmapButton::drawBody(const MVRect& clip,uint bodyState)
/****************************************************************************
*
* Function:     MVSysBitmapButton::drawBody
* Parameters:   clip        - Clipping rectangle to draw the button with
*               depressed   - True if button is depressed
*
* Description:  Internal routine for the button class to draw the button
*               body in the current state. This can be called by all
*               subclasses of button to draw a default button body.
*
*               Note that this routine draws the button in the context
*               of the owner's viewport.
*
****************************************************************************/
{
    MS_obscure();
    setClipRect(clip);
    MV_putSysBitmap(dc,bounds.left(),bounds.top(),index[bodyState]);
    MS_show();
}

MVBitmapButton::MVBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,bitmap_t *bmps[],uint flags,ibool selected)
    : MVButtonBase(dc,bounds,command,flags,selected)
/****************************************************************************
*
* Function:     MVBitmapButton::MVBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
****************************************************************************/
{
    for (int i = 0; i < 32; i++)
        bmp[i] = bmps[i];
}

void MVBitmapButton::drawBody(const MVRect& clip,uint bodyState)
/****************************************************************************
*
* Function:     MVBitmapButton::drawBody
* Parameters:   clip        - Clipping rectangle to draw the button with
*               depressed   - True if button is depressed
*
* Description:  Internal routine for the button class to draw the button
*               body in the current state. This can be called by all
*               subclasses of button to draw a default button body.
*
*               Note that this routine draws the button in the context
*               of the owner's viewport.
*
****************************************************************************/
{
    MS_obscure();
    setClipRect(clip);
    MGL_putBitmap(dc,bounds.left(),bounds.top(),bmp[bodyState],MGL_REPLACE_MODE);
    MS_show();
}
