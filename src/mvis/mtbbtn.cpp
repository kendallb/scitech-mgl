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
* Description:  Member functions for the MVTBButton classes.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mtbbtn.hpp"
#include "mvis/mgroup.hpp"

/*----------------------------- Implementation ----------------------------*/

void MVTBButton::drawBody(const MVRect& clip,uint bodyState)
/****************************************************************************
*
* Function:     MVTBButton::drawBody
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
    ibool depressed = (bodyState & bsPressed);

    MS_obscure();

    // Clip to bounds of button
    setClipRect(clip);

    // Clear the button background
    if (!(flags & bfDontDrawFace)) {
        dc.setColor(getColor(depressed ? scButtonFace : scPressedButtonFace));
        dc.fillRect(bounds);
        }

    // Draw the normal borders around the button
    if (depressed) {
        dc.setColor(getColor(scDefaultButtonBorder));
        drawLine(bounds.left(),bounds.top(),
                 bounds.left(),bounds.bottom()-1);
        drawLine(bounds.left(),bounds.top(),
                 bounds.right()-1,bounds.top());
        MV_setBorderColors(getColor(scHighlight),getColor(scShadow));
        MV_drawBorderCoord(
            bounds.left()+1,bounds.top()+1,
            bounds.right(),bounds.bottom(),
            (depressed ? MV_BDR_INSET : MV_BDR_OUTSET),1);
        }
    else {
        dc.setColor(getColor(scDefaultButtonBorder));
        drawLine(bounds.right()-1,bounds.top(),
                 bounds.right()-1,bounds.bottom()-1);
        drawLine(bounds.left(),bounds.bottom()-1,
                 bounds.right()-1,bounds.bottom()-1);
        MV_setBorderColors(getColor(scHighlight),getColor(scShadow));
        MV_drawBorderCoord(bounds.left(),bounds.top(),bounds.right()-1,
            bounds.bottom()-1,
            (depressed ? MV_BDR_INSET : MV_BDR_OUTSET),1);
        }

    MS_show();
}

void MVTBGlyphButton::drawBody(const MVRect& clip,uint bodyState)
/****************************************************************************
*
* Function:     MVTBGlyphButton::drawBody
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
    MVRect  b;
    ibool   depressed = (bodyState & bsPressed);
    ibool   disabled = (bodyState & bsDisabled);

    MS_obscure();
    MVTBButton::drawBody(clip,bodyState);
    MV_getSysGlyphBounds(glyph,b);
    if (disabled) {
        dc.setColor(getColor(scHighlight));
        MV_putSysGlyph(
            bounds.left()+(bounds.width()-b.width()) / 2 + 1,
            bounds.top()+(bounds.height()-b.height()) / 2 + 1,
            glyph);
        dc.setColor(getColor(scDisabledButtonText));
        }
    else
        dc.setColor(getColor(scButtonText));
    MV_putSysGlyph(
        bounds.left()+(bounds.width()-b.width()) / 2 + (depressed ? 1 : 0),
        bounds.top()+(bounds.height()-b.height()) / 2 + (depressed ? 1 : 0),
        glyph);
    MS_show();
}

void MVTBGlyphButton::setGlyph(char g)
/****************************************************************************
*
* Function:     MVTBGlyphButton::setGlyph
* Parameters:   g   - New glyph for the button
*
****************************************************************************/
{
    if (glyph != g) {
        glyph = g;
        repaint();
        }
}

MVTBBitmapButton::MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,char index,uint flags,ibool selected)
    : MVTBButton(dc,bounds,command,flags | bfDontDrawFace,selected),
      index(index), bmp((bitmap_t*)NULL)
/****************************************************************************
*
* Function:     MVTBBitmapButton::MVTBBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
* Description:  Constructor for the standard MVTBBitmapButton class, which is a
*               button with a text caption.
*
****************************************************************************/
{
    if (index > 0 && index < sbMaxSysBitmap) {
        MVRect b;
        MV_getSysBitmapBounds(index,b);
        setBounds(b);
        }
    else
        this->index = -1;
}

MVTBBitmapButton::MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,const char *bitmapName,uint flags,ibool selected)
    : MVTBButton(dc,bounds,command,flags | bfDontDrawFace,selected),
      index(-1), bmp(bitmapName)
/****************************************************************************
*
* Function:     MVTBBitmapButton::MVTBBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
* Description:  Constructor for the standard MVTBBitmapButton class, which is a
*               button with a text caption.
*
****************************************************************************/
{
    if (bmp.valid()) {
        setBounds(MVRect(bounds.left(),bounds.top(),
            bounds.left() + bmp.getBitmap()->width,
            bounds.top() + bmp.getBitmap()->height));
        }
}

MVTBBitmapButton::MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,const MGLBitmap& bitmap,uint flags,ibool selected)
    : MVTBButton(dc,bounds,command,flags | bfDontDrawFace,selected),
      index(-1), bmp(bitmap)
/****************************************************************************
*
* Function:     MVTBBitmapButton::MVTBBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
* Description:  Constructor for the standard MVTBBitmapButton class, which is a
*               button with a text caption.
*
****************************************************************************/
{
    if (bmp.valid()) {
        setBounds(MVRect(bounds.left(),bounds.top(),
            bounds.left() + bmp.getBitmap()->width,
            bounds.top() + bmp.getBitmap()->height));
        }
}

MVTBBitmapButton::MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
    ulong command,bitmap_t *bitmap,uint flags,ibool selected)
    : MVTBButton(dc,bounds,command,flags | bfDontDrawFace,selected),
      index(-1), bmp(bitmap)
/****************************************************************************
*
* Function:     MVTBBitmapButton::MVTBBitmapButton
* Parameters:   bounds      - Bounding rectangle for the button
*               command     - Command code to send when activated
*               flags       - Flags for the button
*
* Description:  Constructor for the standard MVTBBitmapButton class, which is a
*               button with a text caption.
*
****************************************************************************/
{
    if (bmp.valid()) {
        setBounds(MVRect(bounds.left(),bounds.top(),
            bounds.left() + bmp.getBitmap()->width,
            bounds.top() + bmp.getBitmap()->height));
        }
}

void MVTBBitmapButton::drawBody(const MVRect& clip,uint bodyState)
/****************************************************************************
*
* Function:     MVTBBitmapButton::drawBody
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
    MVRect  b;
    ibool   depressed = (bodyState & bsPressed) != 0;
    ibool   disabled = (bodyState & bsDisabled);

    MS_obscure();
    MVTBButton::drawBody(clip,bodyState);
    if (index != -1) {
        MV_getSysBitmapBounds(index,b);
        MV_putSysBitmap(dc,4 + depressed,4 + depressed,index);
        }
    else if (bmp.valid()) {
        dc.putBitmap(4 + depressed,4 + depressed,bmp,MGL_REPLACE_MODE);
        }
    if (disabled) {
        // To show a disabled bitmap button, we re-render over the top with
        // a pattern fill in the original face color
        attributes_t attr;
        dc.getAttributes(attr);
        dc.setColor(getColor(depressed ? scButtonFace : scPressedButtonFace));
        dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
        dc.setPenBitmapPattern(0,MGL_GRAY_FILL);
        dc.usePenBitmapPattern(0);
        dc.fillRect(bounds);
        dc.restoreAttributes(attr);
        }
    MS_show();
}
