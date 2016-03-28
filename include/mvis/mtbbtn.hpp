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
* Description:  Header file for the MVTBButton classes for button to go
*               on toolbars.
*
****************************************************************************/

#ifndef __MVIS_MTBBTN_HPP
#define __MVIS_MTBBTN_HPP

#ifndef __MVIS_MBUTTON_HPP
#include "mvis/mbutton.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following is the base class for all toolbar button types. It basically
// overrides the default drawing for the button and handles drawing the
// proper border around the button body. It is subclassed by either the
// glyph or bitmap button toolbar button classes to create real classes that
// can be used in practice.
//---------------------------------------------------------------------------

class MVTBButton : public MVButtonBase {
protected:
            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVTBButton(MGLDevCtx& dc,const MVRect& bounds,ulong command,
                uint flags = bfNormal,ibool selected = false)
                : MVButtonBase(dc,bounds,command,flags,selected) {};
    };

//---------------------------------------------------------------------------
// The following is a class for representing toolbar buttons that have the
// face drawn as a monochrome glyph from the current system glyph font.
//---------------------------------------------------------------------------

class MVTBGlyphButton : public MVTBButton {
protected:
    char        glyph;          // System glyph for face of button

            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVTBGlyphButton(MGLDevCtx& dc,const MVRect& bounds,ulong command,
                char glyph,uint flags = bfNormal,ibool selected = false)
                : MVTBButton(dc,bounds,command,flags,selected),
                  glyph(glyph) {};

            // Set or get the current glyph
            char getGlyph()         { return glyph; };
            void setGlyph(char g);
    };

//---------------------------------------------------------------------------
// The following is a class for representing toolbar buttons that have the
// face drawn as a color bitmap. The button bounds shrink to fit the
// bitmap that it holds.
//---------------------------------------------------------------------------

class MVTBBitmapButton : public MVTBButton {
protected:
    char        index;          // System bitmap index (-1 for user bitmap)
    MGLBitmap   bmp;            // Optional user supplied bitmap

            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,char index,uint flags = bfNormal,
                ibool selected = false);
            MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,const char *bitmapName,uint flags = bfNormal,
                ibool selected = false);
            MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,const MGLBitmap& bitmap,uint flags = bfNormal,
                ibool selected = false);
            MVTBBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,bitmap_t *bitmap,uint flags = bfNormal,
                ibool selected = false);
    };

#endif  // __MVIS_MTBBTN_HPP
