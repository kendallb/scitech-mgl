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
* Description:  C API bindings for the Sprite Manager.
*
****************************************************************************/

#include "gm/sprmgr.hpp"
#include "gm/sprite.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Initializes the Game Framework Sprite Manager

PARAMETERS:
dc  - MGL device context to use

HEADER:
gm/sprite.h

RETURNS:
Pointer to the bitmap manager for the device context.

REMARKS:
This function initializes the Sprite Manager library and sets up for storing
bitmaps with the sprite manager. The device context you pass in can be any
MGL device context, but usually it should be an MGL display device context
or an MGL memory device context. The Sprite Manager will automatically
integrogate the capabilities of the device context, and if the device
context supports the creation of an offscreen device context for storing
sprites in video memory, an offscreen device context will be created
and managed by the sprite manager.

The sprite manager automatically does all the work to keep track of which
bitmaps are loaded in video memory and which are loaded in system memory. If
there is not enough video memory left to store a sprite when you add it to
the Sprite Manager, that sprite is stored in system memory automatically.

Note:   When you add bitmaps to the Sprite Manager, those bitmaps are copied
        into the allocated buffer, so you should destroy the bitmaps
        when you are done. You can also optionally add bitmaps from
        another device context, which allows you to use a common device
        context to do conversion of bitmaps to the hardware color depth.

Note:   If you switch fullscreen graphics modes or you switch from fullscreen
        modes to windowed modes, you /must/ call SPR_mgrExit to destroy
        the bitmap manager and re-initialize it with the newly created
        device contexts. This is necessary because the capabilities of the
        new device context and the amount of available offscreen video memory
        (if any) will be different in the new graphics mode and all the
        sprites will have to be re-loaded into the Sprite Manager.

SEE ALSO:
SPR_mgrExit
***************************************************************************/
SPR_bitmapManager * MGLAPI SPR_mgrInit(
    MGLDC *dc)
{
    return new SPR_BitmapManager(dc);
}

/****************************************************************************
DESCRIPTION:
Shuts down the Game Framework Sprite Manager

HEADER:
gm/sprite.h

PARAMETERS
mgr - Bitmap manager to shut down

REMARKS:
This function shuts down the Sprite Manager, and destroys all bitmaps
currently own by the Sprite Manager.

SEE ALSO:
SPR_mgrInit, SPR_mgrEmpty
***************************************************************************/
void MGLAPI SPR_mgrExit(
    SPR_bitmapManager *mgr)
{
    delete mgr;
}

/****************************************************************************
DESCRIPTION:
Empties Sprite Manager of all loaded bitmaps

HEADER:
gm/sprite.h

PARAMETERS
mgr - Bitmap manager to empty

REMARKS:
This function empties the Sprite Manager of all currently loaded bitmaps
and deallocates the memory owned by those bitmaps. This function is most
useful to clearing the Sprite Manager of all bitmaps when moving from one
level to another in your game.

SEE ALSO:
SPR_mgrInit, SPR_mgrEmpty
***************************************************************************/
void MGLAPI SPR_mgrEmpty(
    SPR_bitmapManager *mgr)
{
    mgr->empty();
}

/****************************************************************************
DESCRIPTION:
Adds an opaque or non-transparent bitmap to the Sprite Manager

HEADER:
gm/sprite.h

PARAMETERS:
mgr     - Bitmap manager to add sprite to
bmp     - MGL bitmap to add to the Sprite Manager

RETURNS:
Pointer to the loaded sprite object

REMARKS:
This function adds a new opaque or non-transparent bitmap to the Sprite
Manager. When you add the bitmap, the Sprite Manager takes over ownership
of the memory allocated to the bitmap and you /must/ not call
MGL_unloadBitmap on the bitmap to free the memory. Once the bitmap has
been added, you can then draw the bitmap by calling SPR_draw and pass in
the pointer to the bitmap returned by this function.

Note:   The bitmap added to the Sprite Manager /must/ be in the same
        format at the device context that the bitmaps will be copied to.
        Hence you should add code to your program to do any necessary
        conversions to the destination pixel format for the bitmaps (see
        the Fox & Bear sample program which contains code to do this).

Note:   The Sprite Manager also maintains ownership of the SPR_bitmap object
        that is returned by this function, and this object will be destroyed
        automatically when you empty or exit the Sprite Manager. You can
        call SPR_destroyBitmap if you wish to destroy the bitmap.

SEE ALSO:
SPR_mgrAddTransparentBitmap, SPR_draw, SPR_mgrEmpty, SPR_destroyBitmap
***************************************************************************/
SPR_bitmap * MGLAPI SPR_mgrAddOpaqueBitmap(
    SPR_bitmapManager *mgr,
    bitmap_t *bmp)
{
    return mgr->addOpaqueBitmap(bmp);
}

/****************************************************************************
DESCRIPTION:
Adds a source transparent bitmap to the Sprite Manager

HEADER:
gm/sprite.h

PARAMETERS:
mgr         - Bitmap manager to add sprite to
bmp         - MGL bitmap to add to the Sprite Manager
transparent - Transparent color for the bitmap

RETURNS:
Pointer to the loaded sprite object

REMARKS:
This function adds a new transparent bitmap to the Sprite Manager. When you
add the bitmap, the Sprite Manager takes over ownership of the memory allocated
to the bitmap and you /must/ not call MGL_unloadBitmap on the bitmap to free
the memory. Once the bitmap has been added, you can then draw the bitmap by
calling SPR_draw and pass in the pointer to the bitmap returned by this
function.

Note that the transparent color you pass in is a /source transparent/ color,
which means that pixels in the source bitmap that match the transparent
color will not be drawn when you call SPR_draw for the returned sprite
object (ie: they are transparent).

Note:   The bitmap added to the Sprite Manager /must/ be in the same
        format at the device context that the bitmaps will be copied to.
        Hence you should add code to your program to do any necessary
        conversions to the destination pixel format for the bitmaps (see
        the Fox & Bear sample program which contains code to do this).

Note:   The Sprite Manager also maintains ownership of the SPR_bitmap object
        that is returned by this function, and this object will be destroyed
        automatically when you empty or exit the Sprite Manager. You can
        call SPR_destroyBitmap if you wish to destroy the bitmap.

SEE ALSO:
SPR_mgrAddOpaqueBitmap, SPR_draw, SPR_mgrEmpty, SPR_destroyBitmap
***************************************************************************/
SPR_bitmap * MGLAPI SPR_mgrAddTransparentBitmap(
    SPR_bitmapManager *mgr,
    bitmap_t *bmp,
    color_t transparent)
{
    return mgr->addTransparentBitmap(bmp,transparent);
}

/****************************************************************************
DESCRIPTION:
Destroy a bitmap managed by the sprite manager

HEADER:
gm/sprite.h

PARAMETERS:
mgr         - Bitmap manager to destroy sprite in
bmp         - MGL bitmap to destroy

RETURNS:
Pointer to the loaded sprite object

REMARKS:
This function removes a bitmap to the Sprite Manager.

SEE ALSO:
SPR_mgrAddOpaqueBitmap, SPR_mgrAddTransparentBitmap
***************************************************************************/
void MGLAPI SPR_destroyBitmap(
    SPR_bitmapManager *mgr,
    SPR_bitmap *bmp)
{
    mgr->destroyBitmap(bmp);
}

/****************************************************************************
DESCRIPTION:
Draws the sprite object at the specified location.

PARAMETERS:
bmp     - Sprite object to draw
x       - X coordinate to draw the sprite at
y       - Y coordinate to draw the sprite at

HEADER:
gm/sprite.h

REMARKS:
This function draws the sprite object at the specified (x,y) location on
the device context currently bound to the Sprite Manager (ie: the device
context you used when you initialized it with SPR_mgrInit). The sprite
is drawn using the attributes of the sprite when you added it (ie: opaque
or source transparent).

SEE ALSO:
SRP_drawSection, SPR_drawExt, SPR_mgrAddOpaqueBitmap,
SPR_mgrAddTransparentBitmap,
***************************************************************************/
void MGLAPI SPR_draw(
    SPR_bitmap *bmp,
    int x,
    int y)
{
    bmp->draw(x,y);
}

/****************************************************************************
DESCRIPTION:
Draws the sprite object at the specified location.

PARAMETERS:
bmp     - Sprite object to draw
x       - X coordinate to draw the sprite at
y       - Y coordinate to draw the sprite at
op      - Write mode to draw with

HEADER:
gm/sprite.h

REMARKS:
This function draws the sprite object at the specified (x,y) location on
the device context currently bound to the Sprite Manager (ie: the device
context you used when you initialized it with SPR_mgrInit). The sprite
is drawn using the attributes of the sprite when you added it (ie: opaque
or source transparent).

SEE ALSO:
SRP_drawSection, SPR_draw, SPR_mgrAddOpaqueBitmap,
SPR_mgrAddTransparentBitmap,
***************************************************************************/
void MGLAPI SPR_drawExt(
    SPR_bitmap *bmp,
    int x,
    int y,
    int op)
{
    bmp->draw(x,y,op);
}

/****************************************************************************
DESCRIPTION:
Draws a section of a sprite object at the specified location.

PARAMETERS:
bmp     - Sprite object to draw
left    - Left coordinate of area to copy from bitmap
top     - Top coordinate of area to copy from bitmap
right   - Right coordinate of area to copy from bitmap
bottom  - Bottom coordinate of area to copy from bitmap
dstLeft - X coordinate to draw bitmap at
dstTop  - Y coordinate to draw bitmap at

HEADER:
gm/sprite.h

REMARKS:
This function draws the sprite object at the specified (x,y) location on
the device context currently bound to the Sprite Manager (ie: the device
context you used when you initialized it with SPR_mgrInit). The sprite
is drawn using the attributes of the sprite when you added it (ie: opaque
or source transparent).

SEE ALSO:
SRP_draw, SPR_drawExt, SPR_mgrAddOpaqueBitmap,
SPR_mgrAddTransparentBitmap,
***************************************************************************/
void MGLAPI SPR_drawSection(
    SPR_bitmap *bmp,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop)
{
    bmp->draw(left,top,right,bottom,dstLeft,dstTop);
}

/****************************************************************************
DESCRIPTION:
Draws a section of a sprite object at the specified location.

PARAMETERS:
bmp     - Sprite object to draw
left    - Left coordinate of area to copy from bitmap
top     - Top coordinate of area to copy from bitmap
right   - Right coordinate of area to copy from bitmap
bottom  - Bottom coordinate of area to copy from bitmap
dstLeft - X coordinate to draw bitmap at
dstTop  - Y coordinate to draw bitmap at
op      - Write mode to draw with

HEADER:
gm/sprite.h

REMARKS:
This function draws the sprite object at the specified (x,y) location on
the device context currently bound to the Sprite Manager (ie: the device
context you used when you initialized it with SPR_mgrInit). The sprite
is drawn using the attributes of the sprite when you added it (ie: opaque
or source transparent).

SEE ALSO:
SRP_draw, SPR_drawExt, SPR_mgrAddOpaqueBitmap,
SPR_mgrAddTransparentBitmap,
***************************************************************************/
void MGLAPI SPR_drawSectionExt(
    SPR_bitmap *bmp,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int op)
{
    bmp->draw(left,top,right,bottom,dstLeft,dstTop,op);
}
