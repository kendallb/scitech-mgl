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
* Description:  SPR_BitmapManager class, which manages all bitmaps in
*               host memory and offscreen display memory.
*
****************************************************************************/

#include "gm/sprmgr.hpp"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
dc              - Device context for drawing all bitmaps to

REMARKS:
Constructor for the sprite bitmap manager class.
****************************************************************************/
SPR_BitmapManager::SPR_BitmapManager(
    MGLDC *dc)
    : dc(dc)
{
    empty();                    // Empty bitmap manager and offscreen mem
}

/****************************************************************************
REMARKS:
Empties the bitmap manager and destroys all bitmaps, allowing a new set of
bitmaps to be loaded.
****************************************************************************/
void SPR_BitmapManager::empty()
{
    bitmaps.empty();
}

/****************************************************************************
REMARKS:
Destroys a bitmap in the sprite manager.
****************************************************************************/
void SPR_BitmapManager::destroyBitmap(
    SPR_Bitmap *bmp)
{
    bitmaps.remove(bmp);
    delete bmp;
}

/****************************************************************************
PARAMETERS:
bmp - Bitmap to add to bitmap manager

RETURNS:
Pointer to loaded bitmap, or NULL on error.

REMARKS:
Adds the opaque bitmap to the bitmap manager for later use. If the bitmap
is cached in offscreen memory it is downloaded to the hardware.
****************************************************************************/
SPR_Bitmap *SPR_BitmapManager::addOpaqueBitmap(
    bitmap_t *bmp)
{
    SPR_Bitmap  *bitmap = new SPR_Bitmap(dc,bmp);

    if (bitmap) {
        if (bitmap->isValid())
            bitmaps.addToTail(bitmap);
        else {
            delete bitmap;
            bitmap = NULL;
            }
        }
    return bitmap;
}

/****************************************************************************
PARAMETERS:
bmp - Bitmap to add to bitmap manager

RETURNS:
Pointer to loaded bitmap, or NULL on error.

REMARKS:
Adds the opaque bitmap to the bitmap manager for later use. If the bitmap
is cached in offscreen memory it is downloaded to the hardware.
****************************************************************************/
SPR_Bitmap *SPR_BitmapManager::addOpaqueBitmap(
    MGLDC *srcDC,
    int left,
    int top,
    int right,
    int bottom)
{
    SPR_Bitmap  *bitmap = new SPR_Bitmap(dc,srcDC,left,top,right,bottom);

    if (bitmap) {
        if (bitmap->isValid())
            bitmaps.addToTail(bitmap);
        else {
            delete bitmap;
            bitmap = NULL;
            }
        }
    return bitmap;
}

/****************************************************************************
PARAMETERS:
bmp         - Bitmap to add to bitmap manager
transparent - Transparent color for the bitmap

RETURNS:
Pointer to loaded bitmap, or NULL on error.

REMARKS:
Attempts to load the specified bitmap, and will create the appropriate
bitmap given the current hardware characteristics. This function creates
a bitmap that will be drawn as transparent.
****************************************************************************/
SPR_Bitmap *SPR_BitmapManager::addTransparentBitmap(
    bitmap_t *bmp,
    color_t transparent)
{
    SPR_Bitmap  *bitmap = new SPR_TransparentBitmap(dc,bmp,transparent);

    if (bitmap) {
        if (bitmap->isValid())
            bitmaps.addToTail(bitmap);
        else {
            delete bitmap;
            bitmap = NULL;
            }
        }
    return bitmap;
}

/****************************************************************************
PARAMETERS:
bmp - Bitmap to add to bitmap manager

RETURNS:
Pointer to loaded bitmap, or NULL on error.

REMARKS:
Adds the opaque bitmap to the bitmap manager for later use. If the bitmap
is cached in offscreen memory it is downloaded to the hardware.
****************************************************************************/
SPR_Bitmap *SPR_BitmapManager::addTransparentBitmap(
    MGLDC *srcDC,
    int left,
    int top,
    int right,
    int bottom,
    color_t transparent)
{
    SPR_Bitmap  *bitmap = new SPR_TransparentBitmap(dc,srcDC,left,top,right,bottom,transparent);

    if (bitmap) {
        if (bitmap->isValid())
            bitmaps.addToTail(bitmap);
        else {
            delete bitmap;
            bitmap = NULL;
            }
        }
    return bitmap;
}
