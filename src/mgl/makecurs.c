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
* Description:  Simple program to combine bitmaps into a color mouse cursor.
*
****************************************************************************/

#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "makeCursor";

/*------------------------------ Implementation ---------------------------*/

void writeCursor(FILE *f,char *cName,char *name,char *maskName)
{
    int         i,j,colors,colorDataSize;
    bitmap_t    *bmp,*mask;
    uchar       *p;
    palette_t   *pal;

    if ((bmp = MGL_loadBitmap(name,true)) == NULL)
        return;
    if ((mask = MGL_loadBitmap(maskName,true)) == NULL)
        return;
    colors = 1 << bmp->bitsPerPixel;
    colorDataSize = (64 * 64 * bmp->bitsPerPixel) / 8;
    if (colors == 16)
        fprintf(f,"GA_colorCursor %s = {\n", cName);
    else
        fprintf(f,"GA_colorCursor256 %s = {\n", cName);

    // Write the bitmap color data
    fprintf(f,"    {\n");
    p = bmp->surface;
    for (i = 0; i < colorDataSize/32; i++) {
        fprintf(f,"    ");
        for (j = 0; j < 32; j++)
            fprintf(f,"0x%02X,", *p++);
        fprintf(f,"\n");
        }
    fprintf(f,"    },\n");

    // Write the AND mask
    fprintf(f,"    {\n");
    p = mask->surface;
    for (i = 0; i < 512/32; i++) {
        fprintf(f,"    ");
        for (j = 0; j < 32; j++)
            fprintf(f,"0x%02X,", *p++);
        fprintf(f,"\n");
        }
    fprintf(f,"    },\n");

    // Write the color palette
    fprintf(f,"    {\n");
    pal = bmp->pal;
    for (i = 0; i < colors/4; i++) {
        fprintf(f,"    ");
        for (j = 0; j < 4; j++) {
            fprintf(f,"{0x%02X,0x%02X,0x%02X,0}, ", pal->blue,pal->green,pal->red);
            pal++;
            }
        fprintf(f,"\n");
        }
    fprintf(f,"    },\n");

    // Write the hot-spot and clean up
    fprintf(f,"    32,32\n");
    fprintf(f,"    };\n");
    fprintf(f,"\n");
    MGL_unloadBitmap(bmp);
    MGL_unloadBitmap(mask);
}

void demo(MGLDC *dc)
{
    FILE *f = fopen("bitmaps.txt","w");
    writeCursor(f,"frogCursor","k:\\temp\\small16.bmp","k:\\temp\\mask16.bmp");
    writeCursor(f,"doggieCursor","k:\\temp\\small.bmp","k:\\temp\\mask.bmp");
    fclose(f);
}
