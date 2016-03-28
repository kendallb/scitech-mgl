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
* Description:  Simple sample program showing how to use the bltBlt
*               functions to blt an image from a memory dc to the screen,
*               and between areas on the same screen.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME "frog.bmp"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "bitblit";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    int width,height,depth;
    palette_t pal[256];
    rect_t rect = {0};
    pixel_format_t pf;
    MGLDC *memdc;

    /* Display title message at the top of the window. */
    mainWindow(dc,"Bit Blit Demo");

    /* Get image information */
    if (!MGL_getBitmapSize(IMAGE_NAME,&width,&height,&depth,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Create a memory device context. */
    if ((memdc = MGL_createMemoryDC(width,height,depth,&pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if(!MGL_loadBitmapIntoDC(memdc,IMAGE_NAME,0,0,TRUE))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */
    if (depth <= 8) {
        MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
        MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
        MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);
        }

    rect.right = width;
    rect.bottom = height;

    /* Copy image from image in memory to the screen. */
    MGL_bitBlt(dc,memdc,rect,0,0,MGL_REPLACE_MODE);

    /* Copy image from screen to screen with an overlapping area. */
    MGL_bitBlt(dc,dc,rect,width/3,height/3,MGL_REPLACE_MODE);

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);

    /* Pause until the user hits a key or presses a mouse button. */
    pause();
}

