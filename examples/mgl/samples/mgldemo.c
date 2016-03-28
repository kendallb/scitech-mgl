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
* Description:  Demo program for the MegaGraph graphics library. This
*               program is very similar to the graphics demo program
*               that comes with Borland's range of compiler, but has
*               been extended to fully test the MGL library.
*
*               NOTE:   Fonts are searched for relative to the directory
*                       where the driver files are found, or in the
*                       relative to the directory specified by the
*                       MGL_ROOT environment variable.
*
*                       If the driver files are linked with the program
*                       code, fonts are searched relative to the path
*                       specified to MGL_init().
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mglsamp.h"

/*--------------------------- Macros and typedefs -------------------------*/

#define M_PI        3.14159265358979323846

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "mgldemo";

char *markerStyle[] = {             /* Marker style names               */
    "MGL_MARKER_SQUARE",
    "MGL_MARKER_CIRCLE",
    "MGL_MARKER_X"
    };

char *writeMode[] = {               /* Write mode operation names       */
    "MGL_REPLACE_MODE",
    "MGL_AND_MODE",
    "MGL_OR_MODE",
    "MGL_XOR_MODE"
    };

char *fillStyle[] = {               /* Fill style names                 */
    "MGL_BITMAP_SOLID",
    "MGL_BITMAP_OPAQUE",
    "MGL_BITMAP_TRANSPARENT",
    "MGL_PIXMAP"
    };

char *polyStyle[] = {               /* Polygon style names              */
    "MGL_CONVEX_POLYGON",
    "MGL_COMPLEX_POLYGON",
    "MGL_AUTO_POLYGON"
    };

char *textDirection[] = {           /* Text direction names             */
    "MGL_LEFT_DIR",
    "MGL_UP_DIR",
    "MGL_RIGHT_DIR",
    "MGL_DOWN_DIR"
    };

char *horizJust[] = {               /* Horizontal justification names   */
    "MGL_LEFT_TEXT",
    "MGL_CENTER_TEXT",
    "MGL_RIGHT_TEXT"
    };

char *vertJust[] = {                /* Vertical justification names     */
    "MGL_TOP_TEXT",
    "MGL_CENTER_TEXT",
    "MGL_BOTTOM_TEXT"
    };

/* Names of all vector fonts on disk */

#define NUM_FONTS   (sizeof(fontFilename) / sizeof(fontFilename[0]))

char *fontFilename[] = {
    "astrolog.fnt",
    "cyrillc.fnt",
    "gotheng.fnt",
    "gothger.fnt",
    "gothita.fnt",
    "greekc.fnt",
    "greekcs.fnt",
    "greeks.fnt",
    "italicc.fnt",
    "italiccs.fnt",
    "italict.fnt",
    "japanese.fnt",
    "little.fnt",
    "mathbig.fnt",
    "mathsmal.fnt",
    "musical.fnt",
    "romanc.fnt",
    "romancs.fnt",
    "romand.fnt",
    "romans.fnt",
    "romant.fnt",
    "scriptc.fnt",
    "scripts.fnt",
    "symbol.fnt",
    };

/* Array of useful patterns */

#define NUMPATS (sizeof(bitpat) / sizeof(bitpat[0]))

pattern_t bitpat[] = {
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {{0x7F, 0xFF, 0xF7, 0xFF, 0x7F, 0xFF, 0xF7, 0xFF}},
    {{0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF}},
    {{0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}},
    {{0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77}},
    {{0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88}},
    {{0xB1, 0x30, 0x03, 0x1B, 0xD8, 0xC0, 0x0C, 0x8D}},
    {{0x80, 0x10, 0x02, 0x20, 0x01, 0x08, 0x40, 0x04}},
    {{0xFF, 0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88}},
    {{0xFF, 0x80, 0x80, 0x80, 0xFF, 0x08, 0x08, 0x08}},
    {{0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x40, 0x20, 0x00, 0x02, 0x04, 0x08, 0x00}},
    {{0x82, 0x44, 0x39, 0x44, 0x82, 0x01, 0x01, 0x01}},
    {{0xF8, 0x74, 0x22, 0x47, 0x8F, 0x17, 0x22, 0x71}},
    {{0x55, 0xA0, 0x40, 0x40, 0x55, 0x0A, 0x04, 0x04}},
    {{0x20, 0x50, 0x88, 0x88, 0x88, 0x88, 0x05, 0x02}},
    {{0xBF, 0x00, 0xBF, 0xBF, 0xB0, 0xB0, 0xB0, 0xB0}},

    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00}},
    {{0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88}},
    {{0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00}},
    {{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}},
    {{0xAA, 0x00, 0x80, 0x00, 0x88, 0x00, 0x80, 0x00}},
    {{0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}},
    {{0x08, 0x1C, 0x22, 0xC1, 0x80, 0x01, 0x02, 0x04}},
    {{0x88, 0x14, 0x22, 0x41, 0x88, 0x00, 0xAA, 0x00}},
    {{0x40, 0xA0, 0x00, 0x00, 0x04, 0x0A, 0x00, 0x00}},
    {{0x03, 0x84, 0x48, 0x30, 0x0C, 0x02, 0x01, 0x01}},
    {{0x80, 0x80, 0x41, 0x3E, 0x08, 0x08, 0x14, 0xE3}},
    {{0x10, 0x20, 0x54, 0xAA, 0xFF, 0x02, 0x04, 0x08}},
    {{0x77, 0x89, 0x8F, 0x8F, 0x77, 0x98, 0xF8, 0xF8}},
    {{0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08}},
    {{0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08}},

    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x20, 0x00, 0x88, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x20, 0x00, 0x88, 0x00, 0x02, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x02, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0xA8, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0xA8, 0x00, 0x22, 0x00, 0x8A, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0x22, 0x00, 0x8A, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0x22, 0x00, 0xAA, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0xA2, 0x00, 0xAA, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0xA2, 0x00, 0xAA, 0x00, 0x2A, 0x00}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0x2A, 0x00}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xAA, 0x40, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xAA, 0x40, 0xAA, 0x00, 0xAA, 0x04, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x00, 0xAA, 0x04, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x00, 0xAA, 0x44, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x10, 0xAA, 0x44, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x10, 0xAA, 0x44, 0xAA, 0x01}},
    {{0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x01}},
    {{0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11}},
    {{0xAA, 0x54, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11}},
    {{0xAA, 0x54, 0xAA, 0x11, 0xAA, 0x45, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x11, 0xAA, 0x45, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x11, 0xAA, 0x55, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x51, 0xAA, 0x55, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x51, 0xAA, 0x55, 0xAA, 0x15}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x15}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0xEA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0xEA, 0x55, 0xAA, 0x55, 0xAE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xAA, 0x55, 0xAE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xAA, 0x55, 0xEE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xBA, 0x55, 0xEE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xBA, 0x55, 0xEE, 0x55, 0xAB, 0x55}},
    {{0xEE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xAB, 0x55}},
    {{0xEE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xBB, 0x55}},
    {{0xFE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xBB, 0x55}},
    {{0xFE, 0x55, 0xBB, 0x55, 0xEF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xBB, 0x55, 0xEF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xBB, 0x55, 0xFF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xFB, 0x55, 0xFF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xFB, 0x55, 0xFF, 0x55, 0xBF, 0x55}},
    {{0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xBF, 0x55}},
    {{0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55}},
    {{0xFF, 0xD5, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55}},
    {{0xFF, 0xD5, 0xFF, 0x55, 0xFF, 0x5D, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x55, 0xFF, 0x5D, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x55, 0xFF, 0xDD, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x75, 0xFF, 0xDD, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x75, 0xFF, 0xDD, 0xFF, 0x57}},
    {{0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x57}},
    {{0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77}},
    {{0xFF, 0xFD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77}},
    {{0xFF, 0xFD, 0xFF, 0x77, 0xFF, 0xDF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xDF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}
    };

/*---------------------------- Implementation -----------------------------*/

ibool reportStatus(MGLDC *dc)
/****************************************************************************
*
* Function:     reportStatus
* Parameters:   dc  - Device context
*
* Description:  Report the current graphics system configuration after
*               graphics has been initialised.
*
****************************************************************************/
{
    attributes_t    attr;
    rect_t          view,clip;

    mainWindow(dc,"Status report after MGL_init");

    MGL_moveToCoord(10,4);

    /* Get required information */
    MGL_getAttributes(&attr);
    MGL_getViewport(&view);
    MGL_getClipRect(&clip);

    gprintf("Graphics driver    : %s", MGL_modeDriverName(mode));
    gprintf("Graphics mode      : %d x %d x %d", MGL_sizex(dc)+1, MGL_sizey(dc)+1, MGL_getBitsPerPixel(dc));
    gprintf("Screen resolution  : ( 0, 0, %d, %d )",MGL_sizex(dc),MGL_sizey(dc));
    gprintf("View port          : ( %d, %d, %d, %d )",
        view.left,view.top,view.right,view.bottom);
    gprintf("Clipping rectangle : ( %d, %d, %d, %d )",
        clip.left,clip.top,clip.right,clip.bottom);
    gprintf("Current position   : ( %d, %d )", MGL_getX(),MGL_getY());
    gprintf("Maximum color      : %lu", maxcolor);
    gprintf("Color              : %lu", attr.color);
    gprintf("Background color   : %lu", attr.backColor);
    gprintf("Write mode         : %s", writeMode[attr.writeMode]);
    gprintf("Pen height         : %d", attr.penHeight);
    gprintf("Pen width          : %d", attr.penWidth);
    gprintf("Fill style         : %s", fillStyle[attr.penStyle]);
    gprintf("Polygon style      : %s", polyStyle[attr.polyType]);
    gprintf("Maximum Page Index : %d", MGL_maxPage(dc));

    gprintf("");
    gprintf("Text settings");
    gprintf("");

    gprintf("Current font       : %s", attr.ts.font->name);
    gprintf("Text direction     : %s", textDirection[attr.ts.dir]);
    gprintf("Horizontal justify : %s", horizJust[attr.ts.horizJust]);
    gprintf("Vertical justify   : %s", vertJust[attr.ts.vertJust]);
    gprintf("Horizontal size    : ( %d / %d )", attr.ts.szNumerx,attr.ts.szDenomx);
    gprintf("Vertical size      : ( %d / %d )", attr.ts.szNumery,attr.ts.szDenomy);
    gprintf("Space extra        : %d", attr.ts.spaceExtra);

    return pause();
}

ibool colorDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     colorDemo
* Parameters:   dc  - Device context
*
* Description:  Display the current color palette on the screen.
*
****************************************************************************/
{
    int         color,width,height,x,y,i,j,top,bottom,start;
    palette_t   pal[256];

    mainWindow(dc,"Color Demonstration");

    if (maxcolor <= 15) {
        /* Simple color demonstration for 16 color displays */

        width = 2 * ((MGL_maxx()+1) / 16);
        height = 2 * ((MGL_maxy()-10)  / 10);

        x = width / 2;
        y = height / 2;
        color = 1;
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 5; i++) {
                MGL_setColor(color++);
                MGL_fillRectCoord(x,y,x+width,y+height);
                x += (width/2) * 3;
                }
            y += (height / 2) * 3;
            x = width / 2;
            }
        }
    else if (maxcolor == 255) {
        /* Color demonstration for large numbers of colors */

        width = 2 * ((MGL_maxx()+1) / 46);
        height = 2 * ((MGL_maxy()-10)  / 47);

        x = width / 2;
        y = height / 2;
        color = 1;
        for (j = 0; j < 16; j++) {
            for (i = 0; i < 16; i++) {
                MGL_setColor(color++);
                MGL_fillRectCoord(x,y,x+width,y+height);
                x += (width/2) * 3;
                }
            y += (height / 2) * 3;
            x = width / 2;
            }

        MGL_setColor(MGL_defaultColor());
        statusLine("Press a key to rotate palette");
        waitEvent();
        statusLine("Press a key to rotate other direction");

        // Palette rotations
        while (!checkEvent()) {
            MGL_rotatePalette(dc,palsize-1,1,MGL_ROTATE_UP);
            MGL_realizePalette(dc,palsize-1,1,true);
            }
        waitEvent();
        statusLine("Press a key to fade palette");

        while (!checkEvent()) {
            MGL_rotatePalette(dc,palsize-1,1,MGL_ROTATE_DOWN);
            MGL_realizePalette(dc,palsize-1,1,true);
            }
        waitEvent();

        // Palette fade out
        MGL_getPalette(dc,pal,palsize,0);
        for (i = 63; i >= 0; i--) {
            MGL_fadePalette(dc,pal+1,palsize-1,1,(uchar)(i*4));
            MGL_realizePalette(dc,palsize-1,1,true);
            }

        // Palette fade in
        for (i = 0; i <= 63; i++) {
            MGL_fadePalette(dc,pal+1,palsize-1,1,(uchar)(i*4));
            MGL_realizePalette(dc,palsize-1,1,true);
            }
        }
    else {
        /* Color demonstration for Dithered/HiColor/TrueColor modes */

        start = MGL_maxx()/8;
        width = MGL_maxx() - start*2;
        top = MGL_maxy()/8;
        bottom = MGL_maxy()/8 + MGL_maxy()/5;
        for (x = 0; x < width; x++) {
            MGL_setColor(MGL_packColor(&dc->pf,(uchar)((x * 255L) / width),0,0));
            MGL_lineCoord(start+x,top,start+x,bottom);
            }

        top += MGL_maxy()/5;
        bottom += MGL_maxy()/5;
        for (x = 0; x < width; x++) {
            MGL_setColor(MGL_packColor(&dc->pf,0,(uchar)((x * 255L) / width),0));
            MGL_lineCoord(start+x,top,start+x,bottom);
            }

        top += MGL_maxy()/5;
        bottom += MGL_maxy()/5;
        for (x = 0; x < width; x++) {
            MGL_setColor(MGL_packColor(&dc->pf,0,0,(uchar)((x * 255L) / width)));
            MGL_lineCoord(start+x,top,start+x,bottom);
            }

        top += MGL_maxy()/5;
        bottom += MGL_maxy()/5;
        for (x = 0; x < width/2; x++) {
            MGL_setColor(MGL_packColor(&dc->pf,(uchar)((((width/2)-x) * 255L) / (width/2)),
                (uchar)((x * 255L) / (width/2)),0));
            MGL_lineCoord(start+x,top,start+x,bottom);
            }
        for (; x < width; x++) {
            MGL_setColor(MGL_packColor(&dc->pf,0,
                (uchar)(( ((width/2) - (x - (width/2))) * 255L) / (width/2)),
                (uchar)(((x-width/2) * 255L) / (width/2)) ));
            MGL_lineCoord(start+x,top,start+x,bottom);
            }
        }

    MGL_setColor(MGL_defaultColor());
    if (!pause())
        return false;
    MGL_setPalette(dc,defPal,palsize,0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
    return true;
}

ibool paletteDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     paletteDemo
* Parameters:   dc  - Device context
*
* Description:  Change the palette of the screen until the user has had
*               enough.
*
****************************************************************************/
{
    int     i,j,x,y,color,height,width;

    if (maxcolor > 255)
        return true;

    mainWindow(dc,"Palette Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    width  = (MGL_maxx()+1) / 15;       /* Compute width of boxes       */
    height = (MGL_maxy()+1) / 10;       /* Compute height of boxes      */

    x = y = 0;                          /* Start in upper corner        */
    color = 1;                          /* Begin width 1st color        */

    for (j = 0; j < 10; j++) {          /* For 10 rows of boxes         */
        for (i = 0; i < 15; i++) {      /* For 15 columns of boxes      */
            MGL_setColor(color++);
            MGL_fillRectCoord(x,y,x+width+1,y+height+1);
            x += width + 1;             /* Advance to next col          */

            /* Choose color out of order */

            color = 1 + (color % 14);
            }
        x = 0;
        y += height + 1;
        }

    while (!checkEvent()) {                 /* Until user hits a key...     */
        MGL_setPaletteEntry(dc,1+MGL_random(14),
            (uchar)MGL_random(256),
            (uchar)MGL_random(256),
            (uchar)MGL_random(256));
        MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
        }

    MGL_setColor(MGL_defaultColor());
    MGL_setPalette(dc,defPal,palsize,0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
    return pause();
}

ibool pixelDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     pixelDemo
* Parameters:   dc  - Device context
*
* Description:  Display a pattern of random dots on the screen and
*               pick them back up again.
*
****************************************************************************/
{
    int     seed = 1958;
    int     i,x,y,maxx,maxy;
    color_t color;

    mainWindow(dc,"MGL_pixel / MGL_getPixel Demonstration");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    MGL_srand(seed);        /* Seed random number gen with known value  */

    MGL_beginPixel();
    for (i = 0; i < 10000; i++) {           /* Put 10000 pixels on screen   */
        x = 1 + MGL_random((ushort)maxx);   /* Generate a random location   */
        y = 1 + MGL_random((ushort)maxy);
        MGL_setColor(randomColor());
        MGL_pixelCoordFast(x,y);
        }

    MGL_srand(seed);        /* Reseed random number gen with same value */

    MGL_setColorCI(MGL_BLACK);              /* Draw over in MGL_BLACK       */

    for (i = 0; i < 10000; i++) {           /* Take the 10000 pixels off    */
        x = 1 + MGL_random((ushort)maxx);   /* Generate a random location   */
        y = 1 + MGL_random((ushort)maxy);
        color = MGL_getPixelCoordFast(x,y);
        randomColor();                      /* Keep random in sync      */
        MGL_pixelCoordFast(x,y);
        }
    MGL_endPixel();

    MGL_setColor(MGL_defaultColor());
    return pause();
}

ibool rectangleDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     rectangleDemo
* Parameters:   dc  - Device context
*
* Description:  Display a number of random rectangles with random
*               attributes.
*
****************************************************************************/
{
    int     maxx,maxy,val;
    rect_t  r;

    mainWindow(dc,"Random rectangles");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random((ushort)maxx);
        r.right = MGL_random((ushort)maxx);
        r.top = MGL_random((ushort)maxy);
        r.bottom = MGL_random((ushort)maxy);

        /* Fix the rectangle so it is not empty */

        if (r.right < r.left)
            SWAP(r.left,r.right);
        if (r.bottom < r.top)
            SWAP(r.top,r.bottom);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        if ((val = MGL_random(3)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_fillRect(r);
        }

    defaultAttributes(dc);
    return pause();
}

ibool circleDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     circleDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of circles on the screen
*               until the user hits a key.
*
****************************************************************************/
{
    int     maxx,maxy,val;
    rect_t  r;

    mainWindow(dc,"Circle Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random((ushort)(maxx-100));
        r.top = MGL_random((ushort)(maxy-100));
        r.right = r.left + MGL_random(100);
        r.bottom = r.top + MGL_random(100);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        MGL_setPenSize(MGL_random(5)+1,MGL_random(5)+1);

        if ((val = MGL_random(10)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_ellipse(r);
        }

    defaultAttributes(dc);
    return pause();
}

ibool filledCircleDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     filledCircleDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of filled circles on the screen
*               until the user hits a key.
*
****************************************************************************/
{
    int     maxx,maxy,val;
    rect_t  r;

    mainWindow(dc,"Filled circle Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random((ushort)(maxx-100));
        r.top = MGL_random((ushort)(maxy-100));
        r.right = r.left + MGL_random(100);
        r.bottom = r.top + MGL_random(100);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        if ((val = MGL_random(5)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_fillEllipse(r);
        }

    defaultAttributes(dc);
    return pause();
}

ibool arcDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     arcDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of elliptical arc's on the screen
*               until the user hits a key.
*
****************************************************************************/
{
    int     maxx,maxy,startAngle,endAngle;
    rect_t  r;

    mainWindow(dc,"Arc Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random((ushort)(maxx-100));
        r.top = MGL_random((ushort)(maxy-100));
        r.right = r.left + MGL_random(100);
        r.bottom = r.top + MGL_random(100);
        startAngle = MGL_random(360);
        endAngle = MGL_random(360);

        MGL_setColor(randomColor());
        MGL_ellipseArc(r,startAngle,endAngle);
        }

    defaultAttributes(dc);
    return pause();
}

ibool filledArcDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     filledArcDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of filled arcs on the screen
*               until the user hits a key.
*
****************************************************************************/
{
    int     maxx,maxy,startAngle,endAngle,val;
    rect_t  r;

    mainWindow(dc,"Filled Arc Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        r.left = MGL_random((ushort)(maxx-100));
        r.top = MGL_random((ushort)(maxy-100));
        r.right = r.left + MGL_random(100);
        r.bottom = r.top + MGL_random(100);
        startAngle = MGL_random(360);
        endAngle = MGL_random(360);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        if ((val = MGL_random(5)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_fillEllipseArc(r,startAngle,endAngle);
        }

    defaultAttributes(dc);
    return pause();
}

#define adjasp(y)   ((int)(aspectRatio * (double)(y)))
#define torad(d)    (( (double)(d) * M_PI ) / 180.0 )

ibool pieDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     pieDemo
* Parameters:   dc  - Device context
*
* Description:  Draws a simple pie chart on the screen.
*
****************************************************************************/
{
    int             xcenter,ycenter,radius,lradius;
    int             x,y;
    double          radians,piesize;
    arc_coords_t    coords;

    mainWindow(dc,"Pie Chart Demonstration");

    xcenter = MGL_maxx()/2;         /* Center the Pie horizontally  */
    ycenter = MGL_maxy()/2 + 20;        /* Center the Pie vertically    */
    radius  = MGL_maxx()/5;         /* It will cover 2/5ths screen  */
    piesize = MGL_maxy() / 4.0;     /* Optimum height ratio of pie  */

    while ((aspectRatio*radius) < piesize)
        radius++;

    lradius = radius + (radius/5);      /* Labels placed 20% farther    */

    MGL_useFont(largeFont);
    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_TOP_TEXT);
    MGL_drawStrXY(MGL_maxx()/2, 6, "This is a Pie Chart");

    MGL_setTextSize(2,3,2,3);

    MGL_setColorCI(MGL_RED);
    MGL_fillEllipseArcCoord(xcenter+10,ycenter-adjasp(10),
        radius,adjasp(radius),0,90);
    MGL_setColorCI(MGL_WHITE);
    MGL_setPenStyle(MGL_BITMAP_SOLID);
    MGL_ellipseArcCoord(xcenter+10,ycenter-adjasp(10),
        radius,adjasp(radius),0,90);
    MGL_getArcCoords(&coords);
    MGL_lineCoord(coords.x,coords.y,coords.startX,coords.startY);
    MGL_lineCoord(coords.x,coords.y,coords.endX,coords.endY);
    radians = torad(45);
    x = xcenter + (int)(cos(radians) * (double)lradius );
    y = ycenter - (int)(sin(radians) * (double)lradius * aspectRatio );
    MGL_setTextJustify(MGL_LEFT_TEXT,MGL_BOTTOM_TEXT);
    MGL_drawStrXY(x,y,"25 %");

    MGL_setColorCI(MGL_GREEN);
    MGL_setPenStyle(MGL_BITMAP_OPAQUE);
    MGL_setPenBitmapPattern(0,&bitpat[3]);
    MGL_usePenBitmapPattern(0);
    MGL_fillEllipseArcCoord(xcenter,ycenter,radius,adjasp(radius),90,135);
    MGL_setColorCI(MGL_WHITE);
    MGL_setPenStyle(MGL_BITMAP_SOLID);
    MGL_ellipseArcCoord(xcenter,ycenter,radius,adjasp(radius),90,135);
    MGL_getArcCoords(&coords);
    MGL_lineCoord(coords.x,coords.y,coords.startX,coords.startY);
    MGL_lineCoord(coords.x,coords.y,coords.endX,coords.endY);
    radians = torad(113);
    x = xcenter + (int)(cos(radians) * (double)lradius );
    y = ycenter - (int)(sin(radians) * (double)lradius * aspectRatio );
    MGL_setTextJustify(MGL_RIGHT_TEXT,MGL_BOTTOM_TEXT);
    MGL_drawStrXY(x,y,"12.5 %");

    MGL_setColorCI(MGL_YELLOW);
    MGL_setPenStyle(MGL_BITMAP_OPAQUE);
    MGL_setPenBitmapPattern(0,&bitpat[7]);
    MGL_usePenBitmapPattern(0);
    MGL_fillEllipseArcCoord(xcenter-10,ycenter,radius,adjasp(radius),135,225);
    MGL_setColorCI(MGL_WHITE);
    MGL_setPenStyle(MGL_BITMAP_SOLID);
    MGL_ellipseArcCoord(xcenter-10,ycenter,radius,adjasp(radius),135,225);
    MGL_getArcCoords(&coords);
    MGL_lineCoord(coords.x,coords.y,coords.startX,coords.startY);
    MGL_lineCoord(coords.x,coords.y,coords.endX,coords.endY);
    radians = torad(180);
    x = xcenter + (int)(cos(radians) * (double)lradius );
    y = ycenter - (int)(sin(radians) * (double)lradius * aspectRatio );
    MGL_setTextJustify(MGL_RIGHT_TEXT,MGL_CENTER_TEXT);
    MGL_drawStrXY(x,y,"25 %");

    MGL_setColorCI(MGL_LIGHTBLUE);
    MGL_setPenStyle(MGL_BITMAP_OPAQUE);
    MGL_setPenBitmapPattern(0,&bitpat[10]);
    MGL_usePenBitmapPattern(0);
    MGL_fillEllipseArcCoord(xcenter,ycenter,radius,adjasp(radius),225,360);
    MGL_setColorCI(MGL_WHITE);
    MGL_setPenStyle(MGL_BITMAP_SOLID);
    MGL_ellipseArcCoord(xcenter,ycenter,radius,adjasp(radius),225,360);
    MGL_getArcCoords(&coords);
    MGL_lineCoord(coords.x,coords.y,coords.startX,coords.startY);
    MGL_lineCoord(coords.x,coords.y,coords.endX,coords.endY);
    radians = torad(293);
    x = xcenter + (int)(cos(radians) * (double)lradius );
    y = ycenter - (int)(sin(radians) * (double)lradius * aspectRatio );
    MGL_setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
    MGL_drawStrXY(x,y,"37.5 %");

    defaultAttributes(dc);
    return pause();
}

ibool lineRelDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     lineRelDemo
* Parameters:   dc  - Device context
*
* Description:  Display a pattern using relative move and line commands.
*
****************************************************************************/
{
    int     h,w,dx,dy,cx,cy;
    point_t outs[7];

    mainWindow(dc,"MGL_moveRel / MGL_lineRel Demonstration");

    cx = MGL_maxx() / 2;                /* Center of the viewport       */
    cy = MGL_maxy() / 2;
    h  = MGL_maxy() / 8;
    w  = MGL_maxx() / 9;
    dx = 2 * w;
    dy = 2 * h;

    MGL_setBackColor(MGL_realColor(dc,MGL_BLUE));
    MGL_clearViewport();

    outs[0].x = cx -  dx;
    outs[0].y = cy -  dy;
    outs[1].x = cx - (dx-w);
    outs[1].y = cy - (dy+h);
    outs[2].x = cx +  dx;
    outs[2].y = cy - (dy+h);
    outs[3].x = cx +  dx;
    outs[3].y = cy +  dy;
    outs[4].x = cx + (dx-w);
    outs[4].y = cy + (dy+h);
    outs[5].x = cx -  dx;
    outs[5].y = cy + (dy+h);
    outs[6].x = cx -  dx;
    outs[6].y = cy -  dy;

    MGL_setColorCI(MGL_WHITE);
    MGL_fillPolygon(7,outs,0,0);

    outs[0].x = cx - (w/2);
    outs[0].y = cy + h;
    outs[1].x = cx + (w/2);
    outs[1].y = cy + h;
    outs[2].x = cx + (w/2);
    outs[2].y = cy - h;
    outs[3].x = cx - (w/2);
    outs[3].y = cy - h;
    outs[4].x = cx - (w/2);
    outs[4].y = cy + h;

    MGL_setColorCI(MGL_BLUE);
    MGL_fillPolygon(5,outs,0,0);

    /*  Draw a Tesseract object on the screen using the MGL_lineRel and */
    /*  MGL_moveRel drawing commands.                                   */

    MGL_moveToCoord(cx-dx,cy-dy);
    MGL_lineRelCoord(w,-h);
    MGL_lineRelCoord(3*w,0);
    MGL_lineRelCoord(0, 5*h);
    MGL_lineRelCoord(-w,h);
    MGL_lineRelCoord(-3*w,0);
    MGL_lineRelCoord(0,-5*h);

    MGL_moveRelCoord(w,-h);
    MGL_lineRelCoord(0,5*h);
    MGL_lineRelCoord(w+(w/2),0);
    MGL_lineRelCoord(0,-3*h);
    MGL_lineRelCoord(w/2,-h);
    MGL_lineRelCoord(0,5*h);

    MGL_moveRelCoord(0,-5*h);
    MGL_lineRelCoord(-(w+(w/2)),0);
    MGL_lineRelCoord(0,3*h);
    MGL_lineRelCoord(-w/2,h);

    MGL_moveRelCoord(w/2,-h);
    MGL_lineRelCoord(w,0);

    MGL_moveRelCoord(0,-2*h);
    MGL_lineRelCoord(-w,0);

    defaultAttributes(dc);
    return pause();
}

#define MAXPTS  15

ibool lineToDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     lineToDemo
* Parameters:   dc  - Device context
*
* Description:  Display a pattern using MGL_lineTo and MGL_moveTo commands.
*
****************************************************************************/
{
    point_t points[MAXPTS];
    int     i,j,h,w,xcenter,ycenter;
    int     radius,angle,step;
    double  rads;

    mainWindow(dc,"MGL_moveTo / MGL_lineTo Demonstration");

    w = MGL_maxx();
    h = MGL_maxy();

    xcenter = w / 2;                /* Determine the center of circle   */
    ycenter = h / 2;
    radius  = (int)((h - 30) / (aspectRatio * 2));
    step    = 360 / MAXPTS;         /* Determine # of increments        */

    angle = 0;
    for(i = 0; i < MAXPTS; i++) {       /* Determine circle intercepts  */
        rads = (double)angle * M_PI / 180.0;/* Convert angle to radians */
        points[i].x = xcenter + (int)(cos(rads) * radius );
        points[i].y = ycenter - (int)(sin(rads) * radius * aspectRatio);
        angle += step;                  /* Move to next increment       */
        }

    /* Draw bounding circle */

    MGL_ellipseCoord(xcenter,ycenter,radius,(int)(radius*aspectRatio));

    for (i = 0; i < MAXPTS; i++) {      /* Draw the chords to the circle */
        for (j = i; j < MAXPTS; j++) {  /* For each remaining intersect */
            MGL_moveTo(points[i]);  /* Move to beginning of chord   */
            MGL_lineTo(points[j]);  /* Draw the chord               */
            }
        }

    return pause();
}

ibool lineDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     lineDemo
* Parameters:   dc  - Device context
*
* Description:  Draw a whole bunch of random lines with random pen sizes
*               and pattern modes.
*
****************************************************************************/
{
    int     maxx,maxy,val;
    point_t p1,p2;

    mainWindow(dc,"Line Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        p1.x = MGL_random((ushort)maxx);
        p1.y = MGL_random((ushort)maxy);
        p2.x = MGL_random((ushort)maxx);
        p2.y = MGL_random((ushort)maxy);

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());
        MGL_setPenSize(MGL_random(5)+1,MGL_random(5)+1);

        if ((val = MGL_random(5)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_line(p1,p2);
        }

    defaultAttributes(dc);
    return pause();
}

ibool textDump(MGLDC *dc)
/****************************************************************************
*
* Function:     textDump
* Parameters:   dc  - Device context
*
* Description:  Displays all the characters in each of the fonts in
*               the distribution.
*
****************************************************************************/
{
    int         i,j,maxx;
    font_t      *font;
    char        buf[80];

    maxx = MGL_maxx();

    for (i = 0; i < NUM_FONTS; i++) {   /* For each available font      */
        /* Attempt to load the font file from disk. If it cannot be
         * found, then go onto the next one
         */

        strcpy(buf,fontFilename[i]);
        font = MGL_loadFont(buf);
        err = MGL_result();
        if (err == grFontNotFound)
            continue;

        if ((err = MGL_result()) != grOK) {
            MGL_exit();
            exit(1);
            }

        sprintf(buf,"Character set (%s)", font->name);
        mainWindow(dc,buf);

        /* Display all characters on the screen */

        MGL_useFont(font);
        buf[1] = '\0';
        for (j = ' '; j < 256; j++) {
            buf[0] = j;
            MGL_drawStr(buf);
            if (MGL_getX() + MGL_maxCharWidth() > maxx) {
                MGL_moveToCoord(0,MGL_getY() + MGL_textHeight());
                }
            }

        /* Unload the font from memory */

        MGL_unloadFont(font);
        MGL_useFont(defFont);
        if (!pause())
            return false;
        }
    defaultAttributes(dc);
    return true;
}

ibool textStyleDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     textDemo
* Parameters:   dc  - Device context
*
* Description:  Show the large font in several different sizes and
*               orientations.
*
****************************************************************************/
{
    int     i,x,y;
    char    buf[80];

    mainWindow(dc,"Text styles Demonstration");

    MGL_useFont(largeFont);

    x = y = 5;
    for (i = 5; i > 1; i--) {
        MGL_setTextSize(2,i,2,i);
        sprintf(buf,"Size (2/%d)",i);
        MGL_drawStrXY(x,y,buf);
        y += MGL_textHeight();
        }

    MGL_setTextSize(2,3,2,3);

    x = MGL_maxx()/2;
    y = MGL_maxy()/2;

    MGL_setTextDirection(MGL_LEFT_DIR);
    MGL_drawStrXY(x,y,"This goes left");

    MGL_setTextDirection(MGL_UP_DIR);
    MGL_drawStrXY(x,y,"This goes up");

    MGL_setTextDirection(MGL_RIGHT_DIR);
    MGL_drawStrXY(x,y,"This goes right");

    MGL_setTextDirection(MGL_DOWN_DIR);
    MGL_drawStrXY(x,y,"This goes down");

    defaultAttributes(dc);
    return pause();
}

ibool patternDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     patternDemo
* Parameters:   dc  - Device context
*
* Description:  Display the set of fill patterns defined in this demo
*               program.
*
****************************************************************************/
{
    int     i,j,x,y,height,width;

    mainWindow(dc,"Pattern Demonstration");

    width  = (MGL_maxx()+1) / 20;       /* Compute width of boxes       */
    height = (MGL_maxy()+1) / 9;        /* Compute height of boxes      */

    x = y = 10;                         /* Start in upper corner        */
    MGL_setColorCI(MGL_LIGHTGRAY);
    MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);


    for (j = 0; j < 7; j++) {           /* For 7 rows of boxes      */
        for (i = 0; i < 15; i++) {      /* For 15 columns of boxes      */
            MGL_setPenBitmapPattern(0,&bitpat[j*15 + i]);
            MGL_usePenBitmapPattern(0);
            MGL_fillRectCoord(x,y,x+width+1,y+height+1);
            x += width + 10;            /* Advance to next col          */
            }
        x = 10;
        y += height + 10;
        }

    MGL_setColor(MGL_defaultColor());
    return pause();
}

#define MaxPts      6       /* Maximum # of pts in polygon  */

ibool polyDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     polyDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of polygons on the screen with
*               random fill styles.
*
****************************************************************************/
{
    int         i,maxx,maxy,val;
    fxpoint_t   poly[MaxPts];           /* Space to hold polygon data   */

    mainWindow(dc,"MGL_fillPolygon Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        /* Define a random polygon */

        for (i = 0; i < MaxPts; i++) {
            poly[i].x = MGL_randoml(MGL_TOFIX(maxx));
            poly[i].y = MGL_randoml(MGL_TOFIX(maxy));
            }

        MGL_setColor(randomColor());
        MGL_setBackColor(randomColor());

        if ((val = MGL_random(3)) == 0) {
            MGL_setPenStyle(MGL_BITMAP_TRANSPARENT);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else if (val == 1) {
            MGL_setPenStyle(MGL_BITMAP_OPAQUE);
            MGL_setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            MGL_usePenBitmapPattern(0);
            }
        else {
            MGL_setPenStyle(MGL_BITMAP_SOLID);
            }

        MGL_fillPolygonFX(MaxPts,poly,sizeof(fxpoint_t),0,0);
        }

    defaultAttributes(dc);
    return pause();
}

ibool fastPolyDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     fastPolyDemo
* Parameters:   dc  - Device context
*
* Description:  Display a random pattern of convex triangular polygons
*               in replace mode at full speed.
*
****************************************************************************/
{
    int         maxx,maxy;
    fxpoint_t   poly[3];            /* Space to hold polygon data   */

    mainWindow(dc,"MGL_fillPolygonFast Demonstration");
    statusLine("Press any key to continue, ESC to Abort");

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    while (!checkEvent()) {
        /* Define a random polygon */

        poly[0].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[0].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[1].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[1].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[2].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[2].y = MGL_randoml(MGL_TOFIX(maxy));

        MGL_setColor(randomColor());
        MGL_fillPolygonFX(3,poly,sizeof(fxpoint_t),0,0);
        }

    defaultAttributes(dc);
    return pause();
}

int fixAngle(int angle)
/****************************************************************************
*
* Function:     fixAngle
* Parameters:   angle
* Returns:      Equivalent angle in the range 0-360 degrees.
*
****************************************************************************/
{
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
    return angle;
}

ibool pageFlipDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     pageFlipDemo
* Parameters:   dc  - Device context
*
* Description:  If page flipping is available, this routine will perform
*               a reasonably complex animation sequence. We use a smart
*               dirty rectangle animation technique to only clear the
*               portion of the display page that has changed to get high
*               speed animation.
*
****************************************************************************/
{
    int     maxx,maxy,stepx,stepy,secAngle,minAngle;
    rect_t  extent,dirtyRect;

    if (MGL_doubleBuffer(dc)) {
        mainWindow(dc,"Page flip animation demo");
        statusLine("Press any key to continue, ESC to Abort");

        maxx = MGL_maxx();
        maxy = MGL_maxy();

        /* Draw title again for hidden page */

        MGL_swapBuffers(dc,true);
        mainWindow(dc,"Page flip animation demo");
        statusLine("Press any key to continue, ESC to Abort");

        extent.left = extent.top = 0;
        extent.right = maxx/5;
        extent.bottom = ((long)extent.right * 1000) / aspect;
        dirtyRect = extent;

        stepx = 1;
        stepy = 1;
        secAngle = minAngle = 90;

        while (!checkEvent()) {
            MGL_setColorCI(MGL_BLACK);
            MGL_fillRect(dirtyRect);
            MGL_setColorCI(MGL_RED);
            MGL_fillEllipse(extent);
            MGL_setColorCI(MGL_WHITE);
            MGL_fillEllipseArc(extent,secAngle-5,secAngle);
            MGL_fillEllipseArc(extent,minAngle-5,minAngle);

            /* Swap the display buffers */

            MGL_swapBuffers(dc,true);

            /* Bounce the clock off the walls */

            dirtyRect = extent;
            MGL_insetRect(dirtyRect,-ABS(stepx),-ABS(stepy));
            if (extent.left + stepx < 0)
                stepx = -stepx;
            if (extent.right + stepx > maxx)
                stepx = -stepx;

            if (extent.top + stepy < 0)
                stepy = -stepy;
            if (extent.bottom + stepy > maxy)
                stepy = -stepy;

            MGL_offsetRect(extent,stepx,stepy);

            /* Update the hand movement */

            secAngle = fixAngle(secAngle - 5);
            if (secAngle == 90)
                minAngle = fixAngle(minAngle - 5);
            }

        MGL_singleBuffer(dc);
        defaultAttributes(dc);
        }
    else {
        mainWindow(dc,"Page flip animation demo");

        maxx = MGL_maxx();
        maxy = MGL_maxy();

        MGL_setTextJustify(MGL_CENTER_TEXT,MGL_CENTER_TEXT);
        MGL_moveToCoord(maxx/2,maxy/2);
        MGL_drawStr("Video mode does not support page flipping");
        }
    return pause();
}

ibool mouseCursorDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     mouseCursorDemo
* Parameters:   dc  - Device context
*
* Description:  Display the mouse cursor on the screen, and change it to
*               a number of different styles.
*
****************************************************************************/
{
    cursor_t    *cursor;

    /* First check to ensure that a mouse is actually installed in the
     * system. MS_init() will return 0 if no mouse is installed, or the
     * number of buttons if it is.
     */
    mainWindow(dc,"Mouse Cursor Demonstration");
    statusLine("Press any key for Hour Glass cursor");
    if (MGL_getBitsPerPixel(dc) == 8) {
        MGL_setPaletteEntry(dc,1,100,100,100);
        MGL_realizePalette(dc,1,1,true);
        MGL_setColor(1);
        }
    else {
        MGL_setColorRGB(100,100,100);
        }
    MGL_fillRectCoord(0,0,MGL_maxx()+1,MGL_maxy()+1);
    MS_show();

    waitEvent();
    cursor = MGL_loadCursor("wait.cur");
    if (cursor == NULL)
        MGL_fatalError("Unable to load WAIT.CUR cursor file");
    MS_setCursor(cursor);
    statusLine("Press any key for IBEAM cursor");

    waitEvent();
    MS_setCursor(MGL_DEF_CURSOR);
    MGL_unloadCursor(cursor);
    cursor = MGL_loadCursor("ibeam.cur");
    if (cursor == NULL)
        MGL_fatalError("Unable to load IBEAM.CUR cursor file");
    MS_setCursor(cursor);

    waitEvent();
    MS_setCursor(MGL_DEF_CURSOR);
    MGL_unloadCursor(cursor);

    if (!pause())
        return false;
    MS_hide();
    return true;
}

void finale(MGLDC *dc)
/****************************************************************************
*
* Function:     finale
* Parameters:   dc  - Device context
*
* Description:  Display a goodbye message before signing off.
*
****************************************************************************/
{
    mainWindow(dc,"Finale");
    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_CENTER_TEXT);

    MGL_useFont(largeFont);
    MGL_drawStrXY(MGL_maxx()/2,MGL_maxy()/2,"Thats all folks!");
    statusLine("Press any key exit...");
    pause();
}

void demo(MGLDC *dc)
{
    if (!reportStatus(dc)) return;
    if (!colorDemo(dc)) return;
    if (!paletteDemo(dc)) return;
    if (!pixelDemo(dc)) return;
    if (!rectangleDemo(dc)) return;
    if (!circleDemo(dc)) return;
    if (!filledCircleDemo(dc)) return;
    if (!arcDemo(dc)) return;
    if (!filledArcDemo(dc)) return;
    if (!polyDemo(dc)) return;
    if (!fastPolyDemo(dc)) return;
    if (!pieDemo(dc)) return;
    if (!lineDemo(dc)) return;
    if (!lineRelDemo(dc)) return;
    if (!lineToDemo(dc)) return;
    if (!patternDemo(dc)) return;
    if (!pageFlipDemo(dc)) return;
    if (!mouseCursorDemo(dc)) return;
    if (!textDump(dc)) return;
    if (!textStyleDemo(dc)) return;
    finale(dc);
}

