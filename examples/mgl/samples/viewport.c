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
* Description:  Simple demo showing how to use the viewport functions to
*               move the output of a simple drawing to a different
*               location on the screen.
*
****************************************************************************/

#include "mglsamp.h"

#define IMAGE_NAME "frog.bmp"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "viewport";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    bitmap_t *bitmap = NULL;
    rect_t rect = {0};

    /* Display title message at the top of the window. */
    mainWindow(dc,"Viewport Demo");

    /* Load a bitmap and the palette from the image file. */
    if((bitmap = MGL_loadBitmap(IMAGE_NAME,TRUE)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Set the display device context with the palette from the
     * memory device context.
     */
    MGL_setPalette(dc,bitmap->pal,MGL_getPaletteSize(dc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);

    /* Put the image onto the display device context. */
    MGL_putBitmapSrcTrans(dc,0,0,bitmap,0,MGL_REPLACE_MODE);

    /* change the dimensions of the current active viewport */
    rect.left = bitmap->width/3;
    rect.top = bitmap->height/3;
    rect.right = rect.left + bitmap->width;
    rect.bottom = rect.top + bitmap->height;
    MGL_setViewportDC(dc,rect);

    /* Put the image onto the display device context. */
    MGL_putBitmapSrcTrans(dc,0,0,bitmap,0,MGL_REPLACE_MODE);

    /* Pause until the user hits a key or presses a mouse button. */
    pause();
}
