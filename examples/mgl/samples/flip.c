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
* Language:             ANSI C
* Environment:  Any
*
* Description:  Demo showing how to use do X and Y flippped sprites with the
*               MGL.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME "doggie2.bmp"
#define IMAGE_ARGB "blend.png"      /* ARGB version of same bitmap image */

/*----------------------------- Global Variables --------------------------*/

char            demoName[] = "flip";
int             bmpWidth,bmpHeight,bmpDepth;
int             maxx,maxy;
pixel_format_t  bmpPF;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store sprites and to use blit them
from the memory dc to the display dc.
****************************************************************************/
ibool flipBlit(
    MGLDC *dc)
{
    palette_t       pal[256];
    int             i,oldCheckId;
    MGLDC           *memdc;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 8bpp Flipped Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    /* Create a memory device context */
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadBitmapIntoDC(memdc,IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup transparent effects blit */
    MGL_makeCurrentDC(memdc);
    fx.flags = MGL_BLT_COLOR_KEY_SRC_SINGLE;
    fx.colorKeyLo = MGL_realColor(dc,MGL_getPixelCoord(0,0));
    MGL_makeCurrentDC(dc);

    /* Copy image from image in memory to the screen. */
    i = 0;
    while (!checkEvent()) {
        if (i & 1)
            fx.flags |= MGL_BLT_FLIPX;
        else
            fx.flags &= ~MGL_BLT_FLIPX;
        if (i & 2)
            fx.flags |= MGL_BLT_FLIPY;
        else
            fx.flags &= ~MGL_BLT_FLIPY;
        MGL_bitBltFxCoord(dc,memdc,0,0,bmpWidth,bmpHeight,
            MGL_random(maxx+bmpWidth)-bmpWidth/2,
            MGL_random(maxy+bmpWidth)-bmpWidth/2,&fx);
        i++;
        }

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store sprites and to use blit them
from the memory dc to the display dc.
****************************************************************************/
ibool flipBitmap(
    MGLDC *dc)
{
    int             i,oldCheckId;
    bitmap_t        *bitmap;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Bitmap 8bpp Flipped Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    /* Load the bitmap */
    if ((bitmap = MGL_loadBitmap(IMAGE_NAME,true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */
    MGL_setPalette(dc,bitmap->pal,256,0);
    MGL_realizePalette(dc,256,0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup transparent effects blit */
    fx.flags = MGL_BLT_COLOR_KEY_SRC_SINGLE;
    fx.colorKeyLo = *((uchar*)bitmap->surface);

    /* Copy image from image in memory to the screen. */
    i = 0;
    while (!checkEvent()) {
        if (i & 1)
            fx.flags |= MGL_BLT_FLIPX;
        else
            fx.flags &= ~MGL_BLT_FLIPX;
        if (i & 2)
            fx.flags |= MGL_BLT_FLIPY;
        else
            fx.flags &= ~MGL_BLT_FLIPY;
        MGL_putBitmapFx(dc,
            MGL_random(maxx+bmpWidth)-bmpWidth/2,
            MGL_random(maxy+bmpWidth)-bmpWidth/2,bitmap,&fx);
        i++;
        }

    /* Remove all device contexts from memory. */
    MGL_unloadBitmap(bitmap);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store sprites and to use blit them
from the memory dc to the display dc. This version loads an ARGB bitmap
with alpha channel for transparency instead of color key.
****************************************************************************/
ibool flipBlitARGB(
    MGLDC *dc)
{
    MGLDC           *memdc;
    bltfx_t         fx;
    int             i;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 32bpp ARGB Flipped Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    /* Create a memory device context */
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,IMAGE_ARGB,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    i = 0;
    while (!checkEvent()) {
        if (i & 1)
            fx.flags |= MGL_BLT_FLIPX;
        else
            fx.flags &= ~MGL_BLT_FLIPX;
        if (i & 2)
            fx.flags |= MGL_BLT_FLIPY;
        else
            fx.flags &= ~MGL_BLT_FLIPY;
        MGL_bitBltFxCoord(dc,memdc,0,0,bmpWidth,bmpHeight,
            MGL_random(maxx+bmpWidth)-bmpWidth/2,
            MGL_random(maxy+bmpWidth)-bmpWidth/2,&fx);
        i++;
        }

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store sprites and to use blit them
from the memory dc to the display dc. This version loads an ARGB bitmap
with alpha channel for transparency instead of color key, and loads the
bitmap image into offscreen memory when hardware acceleration is available.
****************************************************************************/
ibool flipBlitBufARGB(
    MGLDC *dc)
{
    MGLDC           *memdc;
    MGLDC           *offdc;
    bltfx_t         fx;
    int             i;

    /* Test does not make sense unless video memory has alpha channels */
    if (dc->mi.bitsPerPixel != 16 && dc->mi.bitsPerPixel != 32)
        return true;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Offscreen Memory 32bpp ARGB Flipped Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    /* Create a memory device context for the sprite bitmap */
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,IMAGE_ARGB,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Create an offscreen buffer to hold copy of bitmap in video memory */
    if ((offdc = MGL_createOffscreenDC(dc,bmpWidth,bmpHeight)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(offdc,memdc,0,0,bmpWidth,bmpHeight,0,0,MGL_REPLACE_MODE);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from offscreen buffer to the screen. */
    i = 0;
    while (!checkEvent()) {
        if (i & 1)
            fx.flags |= MGL_BLT_FLIPX;
        else
            fx.flags &= ~MGL_BLT_FLIPX;
        if (i & 2)
            fx.flags |= MGL_BLT_FLIPY;
        else
            fx.flags &= ~MGL_BLT_FLIPY;
        MGL_bitBltFxCoord(dc,offdc,0,0,bmpWidth,bmpHeight,
            MGL_random(maxx+bmpWidth)-bmpWidth/2,
            MGL_random(maxy+bmpWidth)-bmpWidth/2,&fx);
        i++;
        }

    /* Remove all device contexts from memory. */
    MGL_destroyDC(offdc);
    MGL_destroyDC(memdc);
    return pause();
}

void demo(MGLDC *dc)
{
    /* Get the screen size and store them. */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    /* Get image information */
    if (!MGL_getBitmapSize(IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    if (!flipBlit(dc))
        return;
    if (!flipBitmap(dc))
        return;

    /* Get image information for ARGB format bitmap (PNG file) */
    if (!MGL_getPNGSize(IMAGE_ARGB,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");

    if (!flipBlitARGB(dc))
        return;
    if (!flipBlitBufARGB(dc))
        return;
}

