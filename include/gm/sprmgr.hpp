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
* Description:  Header for the SPR_BitmapManager class.
*
****************************************************************************/

#ifndef __GM_SPRMGR_HPP
#define __GM_SPRMGR_HPP

#include "gm/sprbmp.hpp"

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class to manage a list of bitmaps used for a particular level. This class
// can create either opaque or transparent bitmap objects for the level.
// If the hardware supports accelerated bitmap rendering it will cache as
// many of those in an MGL offscreen memory DC as possible. If there is
// no hardware support (or we have run out of offscreen display memory)
// bitmaps will be created and rendered in software.
//
// This class also manages all the bitmaps in the level, and will
// automatically delete all the bitmaps when the bitmap manager is emptied.
//---------------------------------------------------------------------------

class SPR_BitmapManager {
protected:
    MGLDC           *dc;            // Main MGL display DC to draw onto
    SPR_BitmapList  bitmaps;        // List of all loaded bitmaps for level

public:
            // Constructor
            SPR_BitmapManager(MGLDC *dc);

            // Destructor
            ~SPR_BitmapManager() {};

            // Add a new opaque bitmap object to the bitmap manager
            SPR_Bitmap *addOpaqueBitmap(bitmap_t *bmp);
            SPR_Bitmap *addOpaqueBitmap(MGLDC *srcDC,int left,int top,int right,int bottom);

            // Add a new source transparent bitmap object to the bitmap manager
            SPR_Bitmap *addTransparentBitmap(bitmap_t *bmp,color_t transparent);
            SPR_Bitmap *addTransparentBitmap(MGLDC *srcDC,int left,int top,int right,int bottom,color_t transparent);

            // Destroy all bitmaps and start again
            void empty();

            // Destroy a bitmap in the bitmap manager
            void destroyBitmap(SPR_Bitmap *bmp);
    };

#endif  // __GM_SPRMGR_HPP
