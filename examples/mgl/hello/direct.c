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
* Description:  Sample program showing how to render directly to a device
*               context surface. There are two methods shown here:
*
*                   1.  We create any MGL device context and use the
*                       mode_t information block to directly draw on
*                       the device surface.
*
*                   2.  We create our own system memory device context
*                       as a block of memory for the surface and fill
*                       in a bitmap_t header. We can then draw directly
*                       on the bitmap surface and the blt it to the
*                       display.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgraph.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*----------------------------- Global Variables --------------------------*/

int     maxx,maxy;          /* Maximum coordinates for direct surface   */
long    bytesPerLine;       /* Scanline width for surface               */
char    *surface;           /* Pointer to the surface to draw to        */

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
x,y     - Coordinate to plot pixel at
color   - Color to plot the pixel in

REMARKS:
Plots a pixel to an 8 bit device surface.
****************************************************************************/
void putPixel(
    int x,
    int y,
    int color)
{
    long address = y * bytesPerLine + x;
    *((char*)(surface + address)) = color;
}

/****************************************************************************
PARAMETERS:
x1,y1       - First endpoint of line
x2,y2       - Second endpoint of line

REMARKS:
Scan convert a line segment using the MidPoint Digital Differential
Analyser algorithm.
****************************************************************************/
void line(
    int x1,
    int y1,
    int x2,
    int y2,
    int color)
{
    int     d;                      /* Decision variable                */
    int     dx,dy;                  /* Dx and Dy values for the line    */
    int     Eincr,NEincr;           /* Decision variable increments     */
    int     yincr;                  /* Increment for y values           */
    int     t;                      /* Counters etc.                    */

    dx = ABS(x2 - x1);
    dy = ABS(y2 - y1);

    if (dy <= dx) {
        /* We have a line with a slope between -1 and 1
         *
         * Ensure that we are always scan converting the line from left to
         * right to ensure that we produce the same line from P1 to P0 as the
         * line from P0 to P1.
         */
        if (x2 < x1) {
            t = x2; x2 = x1; x1 = t;    /* Swap X coordinates           */
            t = y2; y2 = y1; y1 = t;    /* Swap Y coordinates           */
            }

        if (y2 > y1)
            yincr = 1;
        else
            yincr = -1;

        d = 2*dy - dx;              /* Initial decision variable value  */
        Eincr = 2*dy;               /* Increment to move to E pixel     */
        NEincr = 2*(dy - dx);       /* Increment to move to NE pixel    */

        putPixel(x1,y1,color);      /* Draw the first point at (x1,y1)  */

        /* Incrementally determine the positions of the remaining pixels
         */
        for (x1++; x1 <= x2; x1++) {
            if (d < 0) {
                d += Eincr;         /* Choose the Eastern Pixel         */
                }
            else {
                d += NEincr;        /* Choose the North Eastern Pixel   */
                y1 += yincr;        /* (or SE pixel for dx/dy < 0!)     */
                }
            putPixel(x1,y1,color);  /* Draw the point                   */
            }
        }
    else {
        /* We have a line with a slope between -1 and 1 (ie: includes
         * vertical lines). We must swap our x and y coordinates for this.
         *
         * Ensure that we are always scan converting the line from left to
         * right to ensure that we produce the same line from P1 to P0 as the
         * line from P0 to P1.
         */
        if (y2 < y1) {
            t = x2; x2 = x1; x1 = t;    /* Swap X coordinates           */
            t = y2; y2 = y1; y1 = t;    /* Swap Y coordinates           */
            }

        if (x2 > x1)
            yincr = 1;
        else
            yincr = -1;

        d = 2*dx - dy;              /* Initial decision variable value  */
        Eincr = 2*dx;               /* Increment to move to E pixel     */
        NEincr = 2*(dx - dy);       /* Increment to move to NE pixel    */

        putPixel(x1,y1,color);      /* Draw the first point at (x1,y1)  */

        /* Incrementally determine the positions of the remaining pixels
         */
        for (y1++; y1 <= y2; y1++) {
            if (d < 0) {
                d += Eincr;         /* Choose the Eastern Pixel         */
                }
            else {
                d += NEincr;        /* Choose the North Eastern Pixel   */
                x1 += yincr;        /* (or SE pixel for dx/dy < 0!)     */
                }
            putPixel(x1,y1,color);  /* Draw the point                   */
            }
        }
}

/****************************************************************************
REMARKS:
Draws a simple moire pattern on the current surface.
****************************************************************************/
void moire(void)
{
    int     i;

    for (i = 0; i < maxx; i += 4) {
        line(maxx/2,maxy/2,i,0,i % 255);
        line(maxx/2,maxy/2,i,maxy,(i+1) % 255);
        }
    for (i = 0; i < maxy; i += 4) {
        line(maxx/2,maxy/2,0,i,(i+2) % 255);
        line(maxx/2,maxy/2,maxx,i,(i+3) % 255);
        }
    line(0,0,maxx,0,15);
    line(0,0,0,maxy,15);
    line(maxx,0,maxx,maxy,15);
    line(0,maxy,maxx,maxy,15);
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(void)
{
    MGLDC           *dc,*memdc;
    event_t         evt;
    pixel_format_t  pf;
    palette_t       pal[256];
    bitmap_t        bmpHdr;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Start the MGL in 640x480 256 color mode */
    dc = MGL_quickInit(640,480,8,1);

    /* Now extract the information about the device surface to our
     * global variables and draw on it. Note that we must check that we
     * have direct access to the surface. If it is a bank switched
     * surface then we cannot directly write to it, and the surface
     * pointer will be NULL.
     *
     * Note that the MGL fully supports virtualised framebuffer surfaces,
     * so in most cases it would be very rare to get a bank switched
     * surface (in fact since we did not register the bank switched
     * drivers, it would be impossible in this sample program!).
     */
    if (MGL_surfaceAccessType(dc) == MGL_NO_ACCESS)
        MGL_fatalError("This program requires a linear access surface");

    /* If we are drawing directly to a surface, we must call
     * MGL_beginDirectAccess() before and MGL_endDirectAccess() after
     * doing any direct surface access
     */
    MGL_beginDirectAccess();

    /* Get variables after enabling direct access */
    maxx = dc->mi.xRes;
    maxy = dc->mi.yRes;
    bytesPerLine = dc->mi.bytesPerLine;
    surface = dc->surface;

    /* Now draw directly to display surface */
    moire();

    /* End direct surface access */
    MGL_endDirectAccess();

    /* Wait for a keypress */
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Now we allocate a system memory block for a 100x100 memory
     * surface manually and attach it to our bitmap header. Then we
     * will draw directly to it and then display it on our main device
     * context. Note that we allocate it without a palette and with
     * a pixel format info block.
     */
    bmpHdr.width = 320;
    bmpHdr.height = 240;
    bmpHdr.bitsPerPixel = 8;
    bmpHdr.bytesPerLine = bmpHdr.width;
    bmpHdr.pal = NULL;
    bmpHdr.pf = NULL;
    bmpHdr.surface = malloc(bmpHdr.bytesPerLine * bmpHdr.height);
    if (bmpHdr.surface == NULL)
        MGL_fatalError("Out of memory");
    maxx = bmpHdr.width-1;
    maxy = bmpHdr.height-1;
    bytesPerLine = bmpHdr.bytesPerLine;
    surface = bmpHdr.surface;
    memset(surface,0,bytesPerLine * (maxy+1));

    /* Now draw directly to bitmap surface and display it */
    moire();
    MGL_putBitmap(dc,160,120,&bmpHdr,MGL_REPLACE_MODE);

    /* Wait for a keypress */
    EVT_halt(&evt,EVT_KEYDOWN);

    /* Now we allocate a system memory device context and draw directly
     * onto this surface, then blit it to the display. It is similar to
     * the above code but does not use a lightweight bitmap as the drawing
     * surface but a full memory device context (which you can also render
     * to using the MGL functions).
     */
    if ((memdc = MGL_createMemoryDC(160,120,8,&pf)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    maxx = memdc->mi.xRes;
    maxy = memdc->mi.yRes;
    bytesPerLine = memdc->mi.bytesPerLine;
    surface = memdc->surface;
    memset(surface,0,bytesPerLine * (maxy+1));

    /* Now set the palette for the memory device to the same as the display
     * DC so that we avoid palette remapping (you could use a different
     * palette and it would be automatically mapped to the screen when you
     * do a blit, but this would be slower).
     */
    MGL_getPalette(dc,pal,256,0);
    MGL_setPalette(memdc,pal,256,0);
    MGL_realizePalette(memdc,256,0,false);

    /* Now draw directly to bitmap surface and display it */
    moire();
    MGL_bitBltCoord(dc,memdc,0,0,160,120,240,180,MGL_REPLACE_MODE);

    /* Wait for a keypress */
    EVT_halt(&evt,EVT_KEYDOWN);

    MGL_exit();
    return 0;
}
