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
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Header for the internal SPR_Bitmap class hierarchy.
*
****************************************************************************/

#ifndef __SPRBMP_HPP
#define __SPRBMP_HPP

#include "mgraph.hpp"
#include "gm/dlist.hpp"

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// This is the abstract base class for all bitmap objects. All bitmaps
// have an associated bitmap stored in host memory, which may optionally
// be destroyed when the bitmap is downloaded to the hardware.
//---------------------------------------------------------------------------

class SPR_Bitmap : public FX_DListNode {
protected:
    MGLBUF  *buf;       // MGL buffer containing the bitmap

public:
            // Constructor
            SPR_Bitmap(MGLDC *dc,bitmap_t *bmp,M_uint32 flags = MGL_BUF_CACHED | MGL_BUF_MOVEABLE);
            SPR_Bitmap(MGLDC *dc,MGLDC *srcDC,int left,int top,int right,int bottom,M_uint32 flags = MGL_BUF_CACHED | MGL_BUF_MOVEABLE);

            // Destructor
    virtual ~SPR_Bitmap();

            // Draw the bitmap at the specified location
    virtual void draw(int x,int y,int op = MGL_REPLACE_MODE);

            // Draw a section of the bitmap at the specififed location
    virtual void draw(int left,int top,int right,int bottom,int dstLeft,int dstTop,int op = MGL_REPLACE_MODE);

            // Determine if the bitmap was constructed properly
    virtual ibool isValid();
    };

typedef FX_DList<SPR_Bitmap>          SPR_BitmapList;
typedef FX_DListIterator<SPR_Bitmap>  SPR_BitmapListIter;

//---------------------------------------------------------------------------
// Source transparent bitmaps
//---------------------------------------------------------------------------

class SPR_TransparentBitmap : public SPR_Bitmap {
protected:
    color_t transparent;    // Transparent color

public:
            // Constructor
            SPR_TransparentBitmap(MGLDC *dc,bitmap_t *bmp,color_t transparent);
            SPR_TransparentBitmap(MGLDC *dc,MGLDC *srcDC,int left,int top,int right,int bottom,color_t transparent);

            // Draw the bitmap at the specified location
    virtual void draw(int x,int y,int op = MGL_REPLACE_MODE);

            // Draw a section of the bitmap at the specififed location
    virtual void draw(int left,int top,int right,int bottom,int dstLeft,int dstTop,int op = MGL_REPLACE_MODE);
    };

#endif  // __SPRBMP_HPP
