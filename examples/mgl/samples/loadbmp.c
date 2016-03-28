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
* Description:  Simple demo showing how to load a bitmap file from disk and
*               display it on the screen.
*
****************************************************************************/

#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "loadbmp";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    bitmap_t        *bitmap;
    int             posx,posy,oldCheck,width,height,bits;
    pixel_format_t  pf;
    palette_t       pal[256];

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 24-bit image. The bitmap
     * will always be a 24-bit, RGB format image.
     */
    mainWindow(dc,"Bitmap Loading Demo (24-bit)");

    if ((bitmap = MGL_loadBitmap("cow-24.bmp",true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* If this is an 8bpp display device context, we need to set the
     * palette to the RGB halftone palette as the MGL_putBitmap call
     * is going to do a dither of the input bitmap since this BMP file is
     * a 24-bit RGB bitmap when loaded from disk.
     */
    if (dc->mi.bitsPerPixel == 8) {
        MGL_getHalfTonePalette(pal);
        MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
        MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);

        /* If we disable identity palette checking, we get faster blitting
         * provided that we load the halftone palette. We don't load the
         * halftone palette, the MGL will automatically dither and map
         * the colors for us (which is a little slower for every blit).
         */
        oldCheck = MGL_checkIdentityPalette(false);
        }

    /* Justify the image that will be put on the screen */
    posx = (MGL_sizex(dc)-bitmap->width)/2;
    posy = (MGL_sizey(dc)-bitmap->height)/2;

    /* Put the image onto the display device context. */
    MGL_putBitmap(dc,posx,posy,bitmap,MGL_REPLACE_MODE);

    /* Remove all bitmaps from memory. */
    MGL_unloadBitmap(bitmap);

    /* Restore identity palette checking */
    MGL_checkIdentityPalette(oldCheck);

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 8-bit image. The bitmap
     * will always be a 8-bit, color index format.
     */
    mainWindow(dc,"Bitmap Loading Demo (8-bit)");

    if ((bitmap = MGL_loadBitmap("frog-8.bmp",true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* If this is an 8bpp display device context, we need to set the
     * palette to the palette contained in the bitmap.
     */
    if (dc->mi.bitsPerPixel == 8) {
        MGL_setPalette(dc,bitmap->pal,256,0);
        MGL_realizePalette(dc,256,0,true);
        }

    /* Justify the image that will be put on the screen */
    posx = (MGL_sizex(dc)-bitmap->width)/2;
    posy = (MGL_sizey(dc)-bitmap->height)/2;

    /* Put the image onto the display device context. */
    MGL_putBitmap(dc,posx,posy,bitmap,MGL_REPLACE_MODE);

    /* Remove all bitmaps from memory. */
    MGL_unloadBitmap(bitmap);

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 4-bit image. The bitmap
     * will always be a 4-bit, color index format.
     */
    mainWindow(dc,"Bitmap Loading Demo (4-bit)");

    if ((bitmap = MGL_loadBitmap("frog-4.bmp",true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* If this is an 8bpp display device context, we need to set the
     * palette to the palette contained in the bitmap.
     */
    if (dc->mi.bitsPerPixel == 8) {
        MGL_setPalette(dc,bitmap->pal,16,0);
        MGL_realizePalette(dc,16,0,true);
        }

    /* Justify the image that will be put on the screen */
    posx = (MGL_sizex(dc)-bitmap->width)/2;
    posy = (MGL_sizey(dc)-bitmap->height)/2;

    /* Put the image onto the display device context. */
    MGL_putBitmap(dc,posx,posy,bitmap,MGL_REPLACE_MODE);

    /* Remove all bitmaps from memory. */
    MGL_unloadBitmap(bitmap);

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 1-bit image. The bitmap
     * will always be a 1-bit, color index format.
     */
    mainWindow(dc,"Bitmap Loading Demo (1-bit)");

    if ((bitmap = MGL_loadBitmap("frog-1.bmp",true)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Justify the image that will be put on the screen */
    posx = (MGL_sizex(dc)-bitmap->width)/2;
    posy = (MGL_sizey(dc)-bitmap->height)/2;

    /* Put the image onto the display device context. */
    MGL_putBitmap(dc,posx,posy,bitmap,MGL_REPLACE_MODE);

    /* Remove all bitmaps from memory. */
    MGL_unloadBitmap(bitmap);

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 24-bit image. The bitmap
     * will always be a 24-bit, RGB format image. Note that for 8bpp
     * display modes the halfTone palette is automatically loaded.
     */
    mainWindow(dc,"Bitmap Loading Demo (24-bit into DC)");

    /* Justify the image that will be put on the screen */
    if (!MGL_getBitmapSize("cow-24.bmp",&width,&height,&bits,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    posx = (MGL_sizex(dc)-width)/2;
    posy = (MGL_sizey(dc)-height)/2;

    /* Load and draw the bitmap into the DC */
    if (!MGL_loadBitmapIntoDC(dc,"cow-24.bmp",posx,posy,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 8-bit color image.
     */
    mainWindow(dc,"Bitmap Loading Demo (8-bit color into DC)");

    /* Justify the image that will be put on the screen */
    if (!MGL_getBitmapSize("frog-8.bmp",&width,&height,&bits,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    posx = (MGL_sizex(dc)-width)/2;
    posy = (MGL_sizey(dc)-height)/2;

    /* Load and draw the bitmap into the DC */
    if (!MGL_loadBitmapIntoDC(dc,"frog-8.bmp",posx,posy,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 4-bit color image.
     */
    mainWindow(dc,"Bitmap Loading Demo (4-bit color into DC)");

    /* Justify the image that will be put on the screen */
    if (!MGL_getBitmapSize("frog-4.bmp",&width,&height,&bits,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    posx = (MGL_sizex(dc)-width)/2;
    posy = (MGL_sizey(dc)-height)/2;

    /* Load and draw the bitmap into the DC */
    if (!MGL_loadBitmapIntoDC(dc,"frog-4.bmp",posx,posy,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;

    /*------------------------------------------------------------------
     * Load a bitmap from the image file as a 1-bit color image.
     */
    mainWindow(dc,"Bitmap Loading Demo (1-bit color into DC)");

    /* Justify the image that will be put on the screen */
    if (!MGL_getBitmapSize("frog-1.bmp",&width,&height,&bits,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    posx = (MGL_sizex(dc)-width)/2;
    posy = (MGL_sizey(dc)-height)/2;

    /* Load and draw the bitmap into the DC */
    if (!MGL_loadBitmapIntoDC(dc,"frog-1.bmp",posx,posy,true))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Pause until the user hits a key or presses a mouse button. */
    if (!pause())
        return;
}

