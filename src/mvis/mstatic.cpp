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
* Description:  Member functions for the MVStaticIcon class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mstatic.hpp"

/*----------------------------- Implementation ----------------------------*/

MVStaticIcon::MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
    const char *iconName)
    : MVView(dc,MVRect(0,0,0,0)), icon(iconName)
/****************************************************************************
*
* Function:     MVStaticIcon::MVStaticIcon
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the icon at
*               iconName    - Name of the icon to load
*
* Description:  Constructor for the MVStaticIcon class.
*
****************************************************************************/
{
    if (icon.valid()) {
        setBounds(MVRect(loc.x,loc.y,
            loc.x + icon.getIcon()->xorMask.width,
            loc.y + icon.getIcon()->xorMask.height));
        }
}

MVStaticIcon::MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
    const MGLIcon& i)
    : MVView(dc,MVRect(0,0,0,0)), icon(i)
/****************************************************************************
*
* Function:     MVStaticIcon::MVStaticIcon
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the icon at
*               iconName    - Name of the icon to load
*
* Description:  Constructor for the MVStaticIcon class.
*
****************************************************************************/
{
    if (icon.valid()) {
        setBounds(MVRect(loc.x,loc.y,
            loc.x + icon.getIcon()->xorMask.width,
            loc.y + icon.getIcon()->xorMask.height));
        }
}

MVStaticIcon::MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
    icon_t *i)
    : MVView(dc,MVRect(0,0,0,0)), icon(i)
/****************************************************************************
*
* Function:     MVStaticIcon::MVStaticIcon
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the icon at
*               iconName    - Name of the icon to load
*
* Description:  Constructor for the MVStaticIcon class.
*
****************************************************************************/
{
    if (icon.valid()) {
        setBounds(MVRect(loc.x,loc.y,
            loc.x + icon.getIcon()->xorMask.width,
            loc.y + icon.getIcon()->xorMask.height));
        }
}

void MVStaticIcon::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVStaticIcon::draw
* Parameters:   clip    - Clipping rectangle for the view.
*
* Description:  Draw's the static text item.
*
****************************************************************************/
{
    if (icon.valid()) {
        // Clear the icon background
        dc.setColor(getColor(scBackground));
        dc.fillRect(bounds);
        dc.putIcon(bounds.left(),bounds.top(),icon);
        }
}

MVStaticBitmap::MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
    const char *bitmapName,uint flags)
    : MVView(dc,bounds), bitmap(bitmapName), flags(flags)
/****************************************************************************
*
* Function:     MVStaticBitmap::MVStaticBitmap
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the bitmap at
*               bitmapName  - Name of the bitmap to load
*
* Description:  Constructor for the MVStaticBitmap class.
*
****************************************************************************/
{
    if (bitmap.valid()) {
        if (flags & sfFitBitmap) {
            setBounds(MVRect(bounds.left(),bounds.top(),
                bounds.left() + bitmap.getBitmap()->width,
                bounds.top() + bitmap.getBitmap()->height));
            }
        }
}

MVStaticBitmap::MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
    const MGLBitmap& bmp,uint flags)
    : MVView(dc,bounds), bitmap(bmp), flags(flags)
/****************************************************************************
*
* Function:     MVStaticBitmap::MVStaticBitmap
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the bitmap at
*               bitmapName  - Name of the bitmap to load
*
* Description:  Constructor for the MVStaticBitmap class.
*
****************************************************************************/
{
    if (bitmap.valid()) {
        if (flags & sfFitBitmap) {
            setBounds(MVRect(bounds.left(),bounds.top(),
                bounds.left() + bitmap.getBitmap()->width,
                bounds.top() + bitmap.getBitmap()->height));
            }
        }
}

MVStaticBitmap::MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
    bitmap_t *bmp,uint flags)
    : MVView(dc,bounds), bitmap(bmp), flags(flags)
/****************************************************************************
*
* Function:     MVStaticBitmap::MVStaticBitmap
* Parameters:   dc          - Device context to draw to
*               loc         - Location to display the bitmap at
*               bitmapName  - Name of the bitmap to load
*
* Description:  Constructor for the MVStaticBitmap class.
*
****************************************************************************/
{
    if (bitmap.valid()) {
        if (flags & sfFitBitmap) {
            setBounds(MVRect(bounds.left(),bounds.top(),
                bounds.left() + bitmap.getBitmap()->width,
                bounds.top() + bitmap.getBitmap()->height));
            }
        }
}

void MVStaticBitmap::draw(const MVRect&)
/****************************************************************************
*
* Function:     MVStaticBitmap::draw
* Parameters:   clip    - Clipping rectangle for the view.
*
* Description:  Draw's the static text item.
*
****************************************************************************/
{
    if (bitmap.valid()) {
        if (flags & sfStretchBitmap)
            dc.stretchBitmap(bounds,bitmap,MGL_REPLACE_MODE);
        else {
            if (!(flags & sfFitBitmap)) {
                // Clear the bitmap background
                dc.setColor(getColor(scBackground));
                dc.fillRect(bounds);
                }
            dc.putBitmap(bounds.left(),bounds.top(),bitmap,MGL_REPLACE_MODE);
            }
        }
}
