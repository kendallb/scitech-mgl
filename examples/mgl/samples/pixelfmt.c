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
* Description:  Sample code showing how to manually create the different
*               pixel formats for memory DC buffers, which can be used to
*               create buffers in modes other than the main display DC mode.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME  "frog-8.bmp"
#define MAX_FORMATS 9

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "pixelfmt";
int  width,height,depth;
int  format_bits[MAX_FORMATS] = {8,15,16,24,24,32,32,32,32};
char format_rgb[MAX_FORMATS][11] ={
    "(INDEXED) ",
    "(555) ",
    "(565) ",
    "(RGB) ",
    "(BGR) ",
    "(ARGB) ",
    "(ABGR) ",
    "(RGBA) ",
    "(BGRA) "
    };
pixel_format_t mempf[MAX_FORMATS] = {
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0},  /* Color Index  */
    {0x1F,0x0A,3,0x1F,0x05,3,0x1F,0x00,3,0x01,0x0F,7},  /* 555 15bpp    */
    {0x1F,0x0B,3,0x3F,0x05,2,0x1F,0x00,3,0x00,0x00,0},  /* 565 16bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0},  /* RGB 24bpp    */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0x00,0x00,0},  /* BGR 24bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0xFF,0x18,0},  /* ARGB 32bpp   */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0},  /* ABGR 32bpp   */
    {0xFF,0x18,0,0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0},  /* RGBA 32bpp   */
    {0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0,0xFF,0x00,0},  /* BGRA 32bpp   */
    };

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc          - Display dc
bitmapFile  - Bitmap file name
format      - Format to pack pixels

REMARKS:
This function loads a bitmap image from disk, and converts it into the
appropriate pixel format in a memory device context. The palette gets loaded
and realized as well.
****************************************************************************/
MGLDC *loadAndConvertBitmap(
    MGLDC *dc,
    char *bitmapFile,
    int format)
{
    pixel_format_t  pf;
    palette_t       pal[256];
    MGLDC           *memDC,*oldDC;

    /* Get image information */
    if (!MGL_getBitmapSize(bitmapFile,&width,&height,&depth,&pf))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    depth = format_bits[format];
    pf = mempf[format];
    if (MGL_getBitsPerPixel(dc) == depth)
        MGL_getPixelFormat(dc,&pf);

    /* Create a memory device context. */
    if ((memDC = MGL_createMemoryDC(width,height,depth,&pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    oldDC = MGL_makeCurrentDC(memDC);

    /* Load a bitmap into the memory device context. The palette gets
     * loaded into the memory device context as well.
     */
    if (!MGL_loadBitmapIntoDC(memDC,IMAGE_NAME,0,0,TRUE))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */
    if (MGL_getBitsPerPixel(dc) <= 8) {
        if (MGL_getBitsPerPixel(memDC) == 4 || MGL_getBitsPerPixel(memDC) == 8) {
            MGL_getPalette(memDC,pal,MGL_getPaletteSize(memDC),0);
            MGL_setPalette(dc,pal,MGL_getPaletteSize(memDC),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(memDC),0,true);
            }
        else if (MGL_getBitsPerPixel(dc) >= 8) {
            MGL_getHalfTonePalette(pal);
            MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
            }
        }
    MGL_makeCurrentDC(oldDC);
    return memDC;
}

/****************************************************************************
PARAMETERS:
dc  - Display dc

REMARKS:
Main demo entry point.
****************************************************************************/
void demo(
    MGLDC *dc)
{
    int     posx,posy,format;
    MGLDC   *memdc;
    char    buf[50];

    for (format = 0; format < MAX_FORMATS; format++) {
        sprintf(buf,"%s%d Bit Pixel Format Demo",format_rgb[format],format_bits[format]);
        mainWindow(dc,buf);

        /* Load the bitmap into a memory DC of the specific pixel format */
        memdc = loadAndConvertBitmap(dc,IMAGE_NAME,format);

        /* Justify the image that will be put on the screen */
        posx = (MGL_sizex(dc)-width)/2;
        posy = (MGL_sizey(dc)-height)/2;

        /* Copy image from image in memory to the screen. */
        MGL_setDitherMode(MGL_DITHER_ON);
        MGL_bitBltCoord(dc,memdc,0,0,width,height,posx,posy,MGL_REPLACE_MODE);

        /* Pause until the user hits a key or presses a mouse button. */
        if (!pause())
            break;

        /* Remove all device contexts from memory. */
        MGL_destroyDC(memdc);
        }
}
