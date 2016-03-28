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
* Description:  Implementation for the SPR_Bitmap class hierarchy.
*
****************************************************************************/

#include "gm/sprmgr.hpp"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
dc      - MGL device context to use
bmp     - Bitmap to load into the buffer
flags   - Buffer creation flags to pass to the MGL

REMARKS:
This constructor creates a bitmap classes object from a passed in bitmap_t
object. The contents of the bitmap_t object are copied to the MGL buffer,
so you can destroy the bitmap after you have loaded it into the sprite
manager.
***************************************************************************/
SPR_Bitmap::SPR_Bitmap(
    MGLDC *dc,
    bitmap_t *bmp,
    M_uint32 flags)
{
    if ((buf = MGL_createBuffer(dc,bmp->width,bmp->height,flags)) != NULL) {
        MGL_copyBitmapToBuffer(bmp,0,0,bmp->width,bmp->height,0,0,buf);
        }
}

/****************************************************************************
PARAMETERS:
dc      - MGL device context to use
srcDC   - Source device context to copy bitmap data from
left    - Left coordinate of area to copy to bitmap
top     - Top coordinate of area to copy to bitmap
right   - Right coordinate of area to copy to bitmap
bottom  - Bottom coordinate of area to copy to bitmap
flags   - Buffer creation flags to pass to the MGL

REMARKS:
This constructor creates a bitmap classes object. This version copies the
bitmap data for the bitmap from an existing MGL device context. You can
use this function to load bitmaps into a memory device context (and color
convert them), and then add them to the sprite manager.
***************************************************************************/
SPR_Bitmap::SPR_Bitmap(
    MGLDC *dc,
    MGLDC *srcDC,
    int left,
    int top,
    int right,
    int bottom,
    M_uint32 flags)
{
    int width = right - left;
    int height = bottom - top;

    if ((buf = MGL_createBuffer(dc,width,height,flags)) != NULL) {
        MGL_copyToBuffer(srcDC,left,top,right,bottom,0,0,buf);
        }
}

/****************************************************************************
REMARKS:
Destructor for the bitmap class.
***************************************************************************/
SPR_Bitmap::~SPR_Bitmap()
{
    MGL_destroyBuffer(buf);
}

/****************************************************************************
PARAMETERS:
x   - X coordinate to draw bitmap at
y   - Y coordinate to draw bitmap at
op  - Write mode to draw with

REMARKS:
This member function draws the bitmap at the specified location, with the
specified raster operation.
***************************************************************************/
void SPR_Bitmap::draw(
    int x,
    int y,
    int op)
{
    MGL_putBuffer(buf->dc,x,y,buf,op);
}

/****************************************************************************
PARAMETERS:
left    - Left coordinate of area to copy from bitmap
top     - Top coordinate of area to copy from bitmap
right   - Right coordinate of area to copy from bitmap
bottom  - Bottom coordinate of area to copy from bitmap
dstLeft - X coordinate to draw bitmap at
dstTop  - Y coordinate to draw bitmap at
op      - Write mode to draw with

REMARKS:
This member function draws a section of the bitmap to the specified location,
with the specified raster operation.
***************************************************************************/
void SPR_Bitmap::draw(
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int op)
{
    MGL_putBufferSection(buf->dc,left,top,right,bottom,dstLeft,dstTop,buf,op);
}

/****************************************************************************
RETURNS:
True if the bitmap object if valid.
***************************************************************************/
ibool SPR_Bitmap::isValid()
{
    return buf != NULL;
}

/****************************************************************************
PARAMETERS:
dc          - MGL device context to use
bmp         - Bitmap to load into the buffer
transparent - Transparent color for the bitmap

REMARKS:
This constructor creates a bitmap classes object from a passed in bitmap_t
object. The contents of the bitmap_t object are copied to the MGL buffer,
so you can destroy the bitmap after you have loaded it into the sprite
manager.
***************************************************************************/
SPR_TransparentBitmap::SPR_TransparentBitmap(
    MGLDC *dc,
    bitmap_t *bmp,
    color_t transparent)
    : SPR_Bitmap(dc,bmp), transparent(transparent)
{
}

/****************************************************************************
PARAMETERS:
dc          - MGL device context to use
srcDC       - Source device context to copy bitmap data from
left        - Left coordinate of area to copy to bitmap
top         - Top coordinate of area to copy to bitmap
right       - Right coordinate of area to copy to bitmap
bottom      - Bottom coordinate of area to copy to bitmap
transparent - Transparent color for the bitmap

REMARKS:
This constructor creates a bitmap classes object. This version copies the
bitmap data for the bitmap from an existing MGL device context. You can
use this function to load bitmaps into a memory device context (and color
convert them), and then add them to the sprite manager.
***************************************************************************/
SPR_TransparentBitmap::SPR_TransparentBitmap(
    MGLDC *dc,
    MGLDC *srcDC,
    int left,
    int top,
    int right,
    int bottom,
    color_t transparent)
    : SPR_Bitmap(dc,srcDC,left,top,right,bottom), transparent(transparent)
{
}

/****************************************************************************
PARAMETERS:
x   - X coordinate to draw bitmap at
y   - Y coordinate to draw bitmap at
op  - Write mode to draw with

REMARKS:
This member function draws the bitmap at the specified location, with the
specified raster operation.
***************************************************************************/
void SPR_TransparentBitmap::draw(
    int x,
    int y,
    int op)
{
    MGL_putBufferSrcTrans(buf->dc,x,y,buf,transparent,op);
}

/****************************************************************************
PARAMETERS:
left    - Left coordinate of area to copy from bitmap
top     - Top coordinate of area to copy from bitmap
right   - Right coordinate of area to copy from bitmap
bottom  - Bottom coordinate of area to copy from bitmap
dstLeft - X coordinate to draw bitmap at
dstTop  - Y coordinate to draw bitmap at
op      - Write mode to draw with

REMARKS:
This member function draws a section of the bitmap to the specified location,
with the specified raster operation.
***************************************************************************/
void SPR_TransparentBitmap::draw(
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int op)
{
    MGL_putBufferSrcTransSection(buf->dc,left,top,right,bottom,dstLeft,dstTop,buf,transparent,op);
}
