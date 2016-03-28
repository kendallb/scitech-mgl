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
* Description:  Demo showing how to use offscreen dc's to store sprites and
*               to use harware acceleration to blt them from the offscreen
*               dc to the display dc. Also shows falling back to a memory
*               dc if an offscreen dc is not available.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME "doggie2.bmp"

/*----------------------------- Global Variables --------------------------*/

char            demoName[] = "sprites";
int             bmpWidth,bmpHeight,bmpDepth;
int             maxx,maxy;
pixel_format_t  bmpPF;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc          - Display dc
trans       - Turn on/off transparent blitting

REMARKS:
Demo showing how to use memory dc's to store sprites and to use blit them
from the memory dc to the display dc.
****************************************************************************/
ibool systemBlit(
    MGLDC *dc,
    ibool trans)
{
    color_t         transparent;
    palette_t       pal[256];
    int             oldCheckId;
    MGLDC           *memdc;

    /* Display title message at the top of the window. */
    if (trans)
        mainWindow(dc,"System Memory 8bpp Transparent Blit Demo");
    else
        mainWindow(dc,"System Memory 8bpp Blit Demo");
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

    /* Get transparent color from pixel (0,0) */
    MGL_makeCurrentDC(memdc);
    transparent = MGL_realColor(dc,MGL_getPixelCoord(0,0));
    MGL_makeCurrentDC(dc);

    /* Copy image from image in memory to the screen. */
    while (!checkEvent()) {
        if (trans)
            MGL_srcTransBltCoord(dc,memdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),transparent,MGL_REPLACE_MODE);
        else
            MGL_bitBltCoord(dc,memdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),MGL_REPLACE_MODE);
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
trans       - Turn on/off transparent blitting

REMARKS:
Demo showing how to use offscreen dc's to store sprites and to use hardware
acceleration to blt them from the offscreen dc to the display dc.
****************************************************************************/
ibool offscreenBlit(
    MGLDC *dc,
    ibool trans)
{
    MGLDC       *offdc;
    color_t     transparent;
    palette_t   pal[256];

    /* Display title message at the top of the window. */
    if (trans)
        mainWindow(dc,"Offscreen Memory Accelerated Transparent Blit Demo");
    else
        mainWindow(dc,"Offscreen Memory Accelerated Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    if ((offdc = MGL_createOffscreenDC(dc,bmpWidth,bmpHeight)) == NULL) {
        gprintf("Hardware accelerated blit not available.");
        }
    else {
        /* Load a bitmap into the offscreen device context */
        if (!MGL_loadBitmapIntoDC(offdc,IMAGE_NAME,0,0,true))
            MGL_fatalError(MGL_errorMsg(MGL_result()));

        /* Set the display device context with the palette from the
         * offscreen device context.
         */
        if (MGL_getBitsPerPixel(dc) <= 8) {
            MGL_getPalette(offdc,pal,MGL_getPaletteSize(offdc),0);
            MGL_setPalette(dc,pal,MGL_getPaletteSize(offdc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(offdc),0,true);
            }

        /* Get transparent color from pixel (0,0) */
        MGL_makeCurrentDC(offdc);
        transparent = MGL_getPixelCoord(0,0);
        MGL_makeCurrentDC(dc);

        /* Copy image from offscreen video memory to the screen. */
        while (!checkEvent()) {
            if (trans)
                MGL_srcTransBltCoord(dc,offdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),transparent,MGL_REPLACE_MODE);
            else
                MGL_bitBltCoord(dc,offdc,0,0,bmpWidth,bmpHeight,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),MGL_REPLACE_MODE);
            }
        }

    /* Remove all device contexts from memory. */
    MGL_destroyDC(offdc);
    return pause();
}

/****************************************************************************
PARAMETERS:
dc          - Display dc
trans       - Turn on/off transparent blitting

REMARKS:
Demo showing how to use offscreen buffers to store sprites and to use
hardware acceleration to blt them to the display dc.
****************************************************************************/
ibool bufferBlit(
    MGLDC *dc,
    ibool trans)
{
    MGLBUF          *buf = NULL;
    color_t         transparent;
    palette_t       pal[256];
    pixel_format_t  pf;
    MGLDC           *memdc;

    /* Display title message at the top of the window. */
    if (trans)
        mainWindow(dc,"Offscreen Buffer Accelerated Transparent Blit Demo");
    else
        mainWindow(dc,"Offscreen Buffer Accelerated Blit Demo");
    statusLine("Press any key to continue, ESC to Abort");

    if ((buf = MGL_createBuffer(dc,bmpWidth,bmpHeight,MGL_BUF_CACHED | MGL_BUF_MOVEABLE)) == NULL) {
        gprintf("Hardware accelerated buffer not available.");
        }
    else {
        /* Create a memory device context */
        MGL_getPixelFormat(dc,&pf);
        if ((memdc = MGL_createMemoryDC(bmpWidth,bmpHeight,MGL_getBitsPerPixel(dc),&pf)) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));

        /* Load a bitmap into the memory device context. The palette gets
         * loaded into the memory device context as well.
         */
        if (!MGL_loadBitmapIntoDC(memdc,IMAGE_NAME,0,0,true))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        MGL_copyToBuffer(memdc,0,0,bmpWidth,bmpHeight,0,0,buf);

        /* Set the display device context with the palette from the
         * memory device context.
         */
        if (MGL_getBitsPerPixel(dc) <= 8) {
            MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
            MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);
            }

        /* Get transparent color from pixel (0,0) */
        MGL_makeCurrentDC(memdc);
        transparent = MGL_getPixelCoord(0,0);
        MGL_makeCurrentDC(dc);

        /* Copy image from offscreen video memory to the screen. */
        while (!checkEvent()) {
            if (trans)
                MGL_putBufferSrcTrans(dc,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),buf,transparent,MGL_REPLACE_MODE);
            else
                MGL_putBuffer(dc,MGL_random(maxx-bmpWidth),MGL_random(maxy-bmpHeight),buf,MGL_REPLACE_MODE);
            }
        MGL_destroyDC(memdc);
        }

    /* Destroy the buffer */
    MGL_destroyBuffer(buf);
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

    if (!systemBlit(dc,true))
        return;
    if (!offscreenBlit(dc,true))
        return;
    if (!bufferBlit(dc,true))
        return;
    if (!systemBlit(dc,false))
        return;
    if (!offscreenBlit(dc,false))
        return;
    if (!bufferBlit(dc,false))
        return;
}

