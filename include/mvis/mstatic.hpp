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
* Description:  Header file for the static window classes.
*
****************************************************************************/

#ifndef __MVIS_MSTATIC_HPP
#define __MVIS_MSTATIC_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

// Static bitmap creation flags

    sfStretchBitmap = 0x0001,       // Bitmap is stretched to bounding box
    sfFitBitmap     = 0x0002;       // Bounds are fitted to bitmap

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVStaticIcon class is used to display an icon in a Window. The icon
// is displayed at a set location and will expand to fit the icon.
//---------------------------------------------------------------------------

class MVStaticIcon : public MVView {
protected:
    MGLIcon     icon;           // Icon to display in the window

public:
            // Constructor
            MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
                const char *iconName);
            MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
                const MGLIcon& icon);
            MVStaticIcon(MGLDevCtx& dc,const MVPoint& loc,
                icon_t *icon);

            // Method to draw the static icon item
    virtual void draw(const MVRect& dirty);
    };

//---------------------------------------------------------------------------
// The MVStaticBitmap class is used to display a bitmap in a Window. The
// bitmap is displayed at a set location and can either be stretched to fit
// the bounding box, clipped to the bounding box or have the bounding box
// expand to fit the bitmap.
//---------------------------------------------------------------------------

class MVStaticBitmap : public MVView {
protected:
    MGLBitmap   bitmap;         // Bitmap to display in the window
    uint        flags;          // Creation flags

public:
            // Constructor
            MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
                const char *bitmapName,uint flags = 0);
            MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
                const MGLBitmap& bitmap,uint flags = 0);
            MVStaticBitmap(MGLDevCtx& dc,const MVRect& bounds,
                bitmap_t *bitmap,uint flags = 0);

            // Method to draw the static bitmap item
    virtual void draw(const MVRect& dirty);
    };

#endif  // __MVIS_MSTATIC_HPP
