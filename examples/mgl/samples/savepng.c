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

#define IMAGE_NAME "test.png"
#define IMAGE_NAME_OUT "copy.png"
/*----------------------------- Global Variables --------------------------*/

char demoName[] = "SavePng";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    bitmap_t *bitmap = NULL;
    int posx,posy;
    int mypalsize;
    int bits;
    pixel_format_t pf;

    palette_t halftone[256];
    /* Display title message at the top of the window. */
    mainWindow(dc,"PNG Demo");
#if(0)
    /* Load a bitmap and the palette from the image file. */
        if((bitmap = MGL_loadPNG(IMAGE_NAME,TRUE)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */

        if (bitmap->pal) {
            switch(bitmap->bitsPerPixel) {
                case 1 : mypalsize = 2; break;
                case 4 : mypalsize = 16; break;
                case 8 : mypalsize = 256; break;
            }
            MGL_setPalette(dc, bitmap->pal,min(MGL_getPaletteSize(dc), mypalsize),0);
            MGL_realizePalette(dc,min(MGL_getPaletteSize(dc), mypalsize),0,true);
            }
        else {
            MGL_getHalfTonePalette(halftone);
            MGL_setPalette(dc,halftone,MGL_getPaletteSize(dc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
            }

        /* Justify the image that will be put on the screen */
    posx = (MGL_sizex(dc)-bitmap->width)/2;
    posy = (MGL_sizey(dc)-bitmap->height)/2;

    /* Put the image onto the display device context. */
    MGL_putBitmap(dc,posx,posy,bitmap,MGL_REPLACE_MODE);

    /* Remove all bitmaps from memory. */
        MGL_unloadBitmap(bitmap);
#endif
    if(!MGL_getPNGSize(IMAGE_NAME, &posx, &posy, &bits, &pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if(!MGL_loadPNGIntoDC(dc, IMAGE_NAME, 0, 0, TRUE))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if(!MGL_savePNGFromDC(dc, IMAGE_NAME_OUT, 0, 0, posx, posy))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    /* Pause until the user hits a key or presses a mouse button. */
    pause();
}








