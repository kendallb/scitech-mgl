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
* Language:     ANSI C
* Environment:  Any
*
* Description:  Demo showing how to use do alpha blended sprites with the MGL.
*               Shows how to use 32-bit ARGB or 8-bit+alpha bitmap data
*               for blending.
*
****************************************************************************/

#include "mglsamp.h"
#include "ztimer.h"

#define ALPHA_RGBA_IMAGE_NAME    "blend.png"
#define ALPHA_GREY_IMAGE_NAME    "blndgrey.png"
#define ALPHA_8BIT_IMAGE_NAME    "blend8.png"

/*----------------------------- Global Variables --------------------------*/

char            demoName[] = "alpha";
int             bmpWidth,bmpHeight,bmpDepth;
int             maxx,maxy,dogCount;
pixel_format_t  bmpPF;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
REMARKS:
Initialise the dog counter for performance measurements
****************************************************************************/
static void StartDogCounter(void)
{
    LZTimerOn();
    dogCount = 0;
}

/****************************************************************************
REMARKS:
Initialise the dog counter for performance measurements
****************************************************************************/
static void UpdateDogCounter(void)
{
    float   lapTime,dogsPerSec;
    char    buf[80];

    if ((++dogCount & 0x7F) == 0) {
        lapTime = (LZTimerLap() / 1000000.0);
        dogsPerSec = dogCount / lapTime;
        sprintf(buf,"%.1f dogs/second - any key to continue, ESC to Abort", dogsPerSec);
        statusLine(buf);
        }
}

/****************************************************************************
REMARKS:
Initialise the dog counter for performance measurements
****************************************************************************/
static void EndDogCounter(void)
{
    LZTimerOff();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc. Note that this version draws
the sprites direct to the screen, which is rather slow when drawing to
the hardware framebuffer because reads from the hardware framebuffer are
very slow.
****************************************************************************/
ibool alphaRGBABlit(
    MGLDC *dc)
{
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    int             bmpWidth,bmpHeight,bmpDepth;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Alpha Blit Demo - Direct to Screen");

    /* Create a memory device context */
    if (!MGL_getPNGSize(ALPHA_RGBA_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_RGBA_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        MGL_bitBltFxCoord(dc,memdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);

    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc. Note that this version draws
the sprites direct to the screen, which is rather slow when drawing to
the hardware framebuffer because reads from the hardware framebuffer are
very slow.
****************************************************************************/
ibool alphaGreyBlit(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    int             bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Grey Alpha Blit Demo - Direct to Screen");

    /* Create a memory device context */
    if (!MGL_getPNGSize(ALPHA_GREY_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_GREY_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        MGL_bitBltFxCoord(dc,memdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

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
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc. Note that this version draws
the sprites direct to the screen, which is rather slow when drawing to
the hardware framebuffer because reads from the hardware framebuffer are
very slow.
****************************************************************************/
ibool alpha8BitBlit(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    int             bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 8-bit + Alpha Blit Demo - Direct to Screen");

    /* Create a memory device context */
    if (!MGL_getPNGSize(ALPHA_8BIT_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_8BIT_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        MGL_bitBltFxCoord(dc,memdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

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
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alphaRGBABlitMem(
    MGLDC *dc)
{
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Alpha Blit Demo - Draw to system memory buffer");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_RGBA_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_RGBA_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(backdc,memdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alphaGreyBlitMem(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Grey Alpha Blit Demo - Draw to system memory buffer");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_GREY_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_GREY_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(backdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(backdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(backdc,memdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alpha8BitBlitMem(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 8-bit + Alpha Blit Demo - Draw to system memory buffer");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_8BIT_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_8BIT_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(backdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(backdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(backdc,memdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate offscreen buffer
which will be compatible with the framebuffer. If hardware acceleration is
available, then the blending operations should be fastest using both source
and destination buffers in video memory.
****************************************************************************/
ibool alphaRGBABlitBuf(
    MGLDC *dc)
{
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    MGLDC           *offdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth;
    bltfx_t         fx;

    /* Test does not make sense unless video memory has alpha channels */
    if (dc->mi.bitsPerPixel != 16 && dc->mi.bitsPerPixel != 32)
        return true;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Alpha Blit Demo - Draw from offscreen buffer");

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_RGBA_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_RGBA_IMAGE_NAME,0,0,true))
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
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(dc,offdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(offdc);
    MGL_destroyDC(memdc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate offscreen buffer
which will be compatible with the framebuffer. If hardware acceleration is
available, then the blending operations should be fastest using both source
and destination buffers in video memory.
****************************************************************************/
ibool alphaGreyBlitBuf(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    MGLDC           *offdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Test does not make sense unless video memory has alpha channels */
    if (dc->mi.bitsPerPixel != 16 && dc->mi.bitsPerPixel != 32)
        return true;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Grey Alpha Blit Demo - Draw from offscreen buffer");

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_GREY_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_GREY_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Create an offscreen buffer to hold copy of bitmap in video memory */
    if ((offdc = MGL_createOffscreenDC(dc,bmpWidth,bmpHeight)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(offdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(offdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Copy the bitmap image into offscreen buffer video memory. */
    MGL_bitBltCoord(offdc,memdc,0,0,bmpWidth,bmpHeight,0,0,MGL_REPLACE_MODE);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in offscreen buffer to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(dc,offdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(offdc);
    MGL_destroyDC(memdc);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate offscreen buffer
which will be compatible with the framebuffer. If hardware acceleration is
available, then the blending operations should be fastest using both source
and destination buffers in video memory.
****************************************************************************/
ibool alpha8BitBlitBuf(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *memdc;
    MGLDC           *offdc;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;

    /* Test does not make sense unless video memory has alpha channels */
    if (dc->mi.bitsPerPixel != 16 && dc->mi.bitsPerPixel != 32)
        return true;

    /* Display title message at the top of the window. */
    mainWindow(dc,"8-bit + Alpha Blit Demo - Draw from offscreen buffer");

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_8BIT_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_8BIT_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Create an offscreen buffer to hold copy of bitmap in video memory */
    if ((offdc = MGL_createOffscreenDC(dc,bmpWidth,bmpHeight)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(offdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(offdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Copy the bitmap image into offscreen buffer video memory. */
    MGL_bitBltCoord(offdc,memdc,0,0,bmpWidth,bmpHeight,0,0,MGL_REPLACE_MODE);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;

    /* Copy image from image in offscreen buffer to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        MGL_bitBltFxCoord(dc,offdc,0,0,bmpWidth,bmpHeight,x,y,&fx);
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(offdc);
    MGL_destroyDC(memdc);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use load alpha sprites into a bitmap_t and to blit
them from the bitmap_t to the display dc.
****************************************************************************/
ibool alphaRGBABlitBitmap(
    MGLDC *dc)
{
    MGLDC           *backdc;
    bitmap_t        *bitmap;
    int             x,y;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Alpha Blit Demo - Draw from bitmap_t");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Load the PNG bitmap */
    if ((bitmap = MGL_loadPNG(ALPHA_RGBA_IMAGE_NAME,true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx - bitmap->width);
        y = MGL_random(maxy - bitmap->height);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bitmap->width,y+bitmap->height,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use load alpha sprites into a bitmap_t and to blit
them from the bitmap_t to the display dc.
****************************************************************************/
ibool alphaGreyBlitBitmap(
    MGLDC *dc)
{
    bitmap_t        *bitmap;
    MGLDC           *backdc;
    int             x,y,oldCheckId;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Grey Alpha Blit Demo - Draw from bitmap_t");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Load the PNG bitmap */
    if ((bitmap = MGL_loadPNG(ALPHA_GREY_IMAGE_NAME,true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    if (bitmap->pal) {
        MGL_setPalette(dc,bitmap->pal,256,0);
        MGL_realizePalette(dc,256,0,true);
        MGL_setPalette(backdc,bitmap->pal,256,0);
        MGL_realizePalette(backdc,256,0,true);
        }
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx - bitmap->width);
        y = MGL_random(maxy - bitmap->height);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bitmap->width,y+bitmap->height,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use load alpha sprites into a bitmap_t and to blit
them from the bitmap_t to the display dc.
****************************************************************************/
ibool alpha8BitBlitBitmap(
    MGLDC *dc)
{
    bitmap_t        *bitmap;
    MGLDC           *backdc;
    int             x,y,oldCheckId;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 8-bit + Alpha Blit Demo - Draw from bitmap_t");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Load the PNG bitmap */
    if ((bitmap = MGL_loadPNG(ALPHA_8BIT_IMAGE_NAME,true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    if (bitmap->pal) {
        MGL_setPalette(dc,bitmap->pal,256,0);
        MGL_realizePalette(dc,256,0,true);
        MGL_setPalette(backdc,bitmap->pal,256,0);
        MGL_realizePalette(backdc,256,0,true);
        }
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx - bitmap->width);
        y = MGL_random(maxy - bitmap->height);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bitmap->width,y+bitmap->height,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alphaRGBABlitBitmapFromDC(
    MGLDC *dc)
{
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    bitmap_t        *bitmap;
    int             x,y,bmpWidth,bmpHeight,bmpDepth;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Alpha Blit Demo - Bitmap from DC");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_RGBA_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_RGBA_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((bitmap = MGL_getBitmapFromDC(memdc,0,0,MGL_sizex(memdc),MGL_sizey(memdc),true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alphaGreyBlitBitmapFromDC(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    bitmap_t        *bitmap;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory Grey Alpha Blit Demo - Bitmap from DC");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_GREY_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_GREY_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((bitmap = MGL_getBitmapFromDC(memdc,0,0,MGL_sizex(memdc),MGL_sizey(memdc),true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
    MGL_setPalette(backdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(backdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc

REMARKS:
Demo showing how to use memory dc's to store alpha sprites and to use blit
them from the memory dc to the display dc.

This version draws all the sprites to an intermediate system memory buffer
and then copies the changed rectangle of data to the display screen. This is
a lot faster than drawing direct to the framebuffer (even with the extra
copy to the screen) because system memory reads a cached and very fast.
****************************************************************************/
ibool alpha8BitBlitBitmapFromDC(
    MGLDC *dc)
{
    palette_t       pal[256];
    pixel_format_t  bmpPF;
    MGLDC           *backdc;
    MGLDC           *memdc;
    bitmap_t        *bitmap;
    int             x,y,bmpWidth,bmpHeight,bmpDepth,oldCheckId;
    bltfx_t         fx;
    ibool           haveHwSupport;

    /* Display title message at the top of the window. */
    mainWindow(dc,"System Memory 8-bit + Alpha Blit Demo - Bitmap from DC");

    /* Create a back buffer memory DC the size of the screen */
    if ((backdc = MGL_createMemoryDC(MGL_sizex(dc),MGL_sizey(dc),MGL_getBitsPerPixel(dc),&dc->pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_bitBltCoord(backdc,dc,0,0,MGL_sizex(dc),MGL_sizey(dc),0,0,MGL_REPLACE_MODE);

    /* Create a memory device context for the sprite bitmap */
    if (!MGL_getPNGSize(ALPHA_8BIT_IMAGE_NAME,&bmpWidth,&bmpHeight,&bmpDepth,&bmpPF))
        MGL_fatalError("Unable to load PNG file!");
    if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,bmpDepth,&bmpPF)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadPNGIntoDC(memdc,ALPHA_8BIT_IMAGE_NAME,0,0,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((bitmap = MGL_getBitmapFromDC(memdc,0,0,MGL_sizex(memdc),MGL_sizey(memdc),true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the palette for the destination DC and turn of identity palette
     * checking. This provides the fastest blit performance because the
     * color translation vector is not re-built for every blit but is
     * instead taken from the destination DC color table (ie: bitmap
     * source palette is ignored).
     */
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
    MGL_setPalette(backdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(backdc,MGL_getPaletteSize(memdc),0,true);
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Setup alpha blended effects */
    fx.flags = MGL_BLT_BLEND;
    fx.srcBlendFunc = MGL_BLEND_SRCALPHAFAST;
    fx.dstBlendFunc = MGL_BLEND_SRCALPHAFAST;
    haveHwSupport = MGL_bitBltFxTestBitmap(dc,bitmap,&fx);

    /* Copy image from image in memory to the screen. */
    StartDogCounter();
    while (!checkEvent()) {
        x = MGL_random(maxx-bmpWidth);
        y = MGL_random(maxy-bmpHeight);
        if (haveHwSupport)
            MGL_putBitmapFx(dc,x,y,bitmap,&fx);
        else {
            MGL_putBitmapFx(backdc,x,y,bitmap,&fx);
            MGL_bitBltCoord(dc,backdc,x,y,x+bmpWidth,y+bmpHeight,x,y,MGL_REPLACE_MODE);
            }
        UpdateDogCounter();
        }
    EndDogCounter();

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);
    MGL_destroyDC(backdc);
    MGL_unloadBitmap(bitmap);
    MGL_checkIdentityPalette(oldCheckId);
    MGL_setDefaultPalette(dc);
    return pause();
}


void demo(MGLDC *dc)
{
    /* Get the screen size and store them. */
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    /* Display title message at the top of the window. */
    if (dc->mi.bitsPerPixel < 15) {
        mainWindow(dc,"Alpha blending requires 15bpp or higher!");
        statusLine("Press any key to abort");
        pause();
        }
    else {
        if (!alphaRGBABlit(dc))
            return;
        if (!alphaGreyBlit(dc))
            return;
        if (!alpha8BitBlit(dc))
            return;
        if (dc->mi.bitsPerPixel == 32) {
            if (!alphaRGBABlitBuf(dc))
                return;
            if (!alphaGreyBlitBuf(dc))
                return;
            if (!alpha8BitBlitBuf(dc))
                return;
            }
        if (!alphaRGBABlitMem(dc))
            return;
        if (!alphaGreyBlitMem(dc))
            return;
        if (!alpha8BitBlitMem(dc))
            return;
        if (!alphaRGBABlitBitmap(dc))
            return;
        if (!alphaGreyBlitBitmap(dc))
            return;
        if (!alpha8BitBlitBitmap(dc))
            return;
        if (!alphaRGBABlitBitmapFromDC(dc))
            return;
        if (!alphaGreyBlitBitmapFromDC(dc))
            return;
        if (!alpha8BitBlitBitmapFromDC(dc))
            return;
        }
}

