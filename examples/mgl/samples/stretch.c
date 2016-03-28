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
* Environment:  MSDOS/Win32 full screen
*
* Description:  Demo showing the use of the stretchBlt functions to stretch
*               a bitmap from memory dc to the screen and stretchBitmap to
*               stretch a custom bitmap image from memory to the screen.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME "frog.bmp"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "stretch";
int maxx,maxy;

/*------------------------------ Implementation ---------------------------*/

ibool bitmapStretch(MGLDC *dc)
/****************************************************************************
*
* Function:     bitmapStretch
* Parameters:   dc  - Device context
*
* Description:  Demo showing how to use stretchBitmap to stretch a custom
*               bitmap image from memory to the screen.
*
****************************************************************************/
{
    int x=maxx/2,y=maxy/2;
    bitmap_t *bitmap = NULL;

    mainWindow(dc,"Bitmap Stretch Demo");
    statusLine("Press any key to continue, ESC to Abort");

    /* Load a bitmap and the palette from the image file. */
    if((bitmap = MGL_loadBitmap(IMAGE_NAME,TRUE)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the bitmap */
    MGL_setPalette(dc,bitmap->pal,MGL_getPaletteSize(dc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);

    while ((x<maxx) && (y<maxy) && (!checkEvent())) {
        MGL_stretchBitmap(dc,maxx-x,maxy-y,x,y,bitmap,MGL_REPLACE_MODE);
        x += 1;
        y += 1;
        }

    /* Remove all bitmaps from memory. */
    MGL_unloadBitmap(bitmap);

    return pause();
}

ibool memoryDCStretch(MGLDC *dc)
/****************************************************************************
*
* Function:     memoryDCStretch
* Parameters:   dc  - Device context
*
* Description:  Demo showing the use of the stretchBlt functions to stretch
*               a bitmap from memory dc to the screen.
****************************************************************************/
{
    MGLDC *memdc;
    int width,height,depth;
    pixel_format_t pf;
    palette_t pal[256];
    int x=maxx/2,y=maxy/2;

    mainWindow(dc,"Memory DC Stretch Demo");
    statusLine("Press any key to continue, ESC to Abort");

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
    MGL_getPalette(memdc,pal,MGL_getPaletteSize(memdc),0);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(memdc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(memdc),0,true);

    /* Put the image onto the display device context. */
    while ((x<maxx) && (y<maxy) && (!checkEvent())) {
        MGL_stretchBltCoord(dc,memdc,0,0,width,height,maxx-x,maxy-y,x,y,MGL_REPLACE_MODE);
        x += 1;
        y += 1;
        }

    /* Remove all device contexts from memory. */
    MGL_destroyDC(memdc);

    return pause();
}

void demo(MGLDC *dc)
{
    maxx = MGL_maxx();
    maxy = MGL_maxy();

    if(!bitmapStretch(dc)) return;
    if(!memoryDCStretch(dc)) return;
}

