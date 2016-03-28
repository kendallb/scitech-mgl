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
* Environment:  Unix/X11
*
* Description:  Header file for the 8-bit X11 driver.
*
****************************************************************************/

#ifndef __DRIVERS_UNIX_XWIN8_H
#define __DRIVERS_UNIX_XWIN8_H

#include "mglunix.h"
#include "mgldd.h"

#ifndef __DRIVERS_PACKED_PACKED8_H
#include "drivers/packed/packed8.h"
#endif

#include "drivers/common/gunix.h"

typedef struct {
    // What the hell do I have to put here ?
    Display *dpy;
    Window  w;
    //  XImage  *img;
} XWIN8_data;

/*------------------------- Function Prototypes ---------------------------*/

/* 8 bit C based routines */

ibool   MGLAPI XWIN8_detect(void *data,int id,int force,int *driver,int *mode,modetab availableModes);
ibool   MGLAPI XWIN8_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
void   *MGLAPI XWIN8_createInstance(void);
void    MGLAPI XWIN8_destroyInstance(void *data);

void MGLAPI XWIN8_bitBlt(MGLDC *dc,int left,int top,int right,int bottom, int dstLeft,int dstTop,int op,int incx,int incy);
color_t MGLAPI XWIN8_getPixel(int x, int y);
void MGLAPI XWIN8_putPixel(int x, int y);
ulong   MGLAPI XWIN8_getWinDC(MGLDC *dc);
void *  MGLAPI XWIN8_getDefaultPalette(MGLDC *dc);
void    MGLAPI XWIN8_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
void MGLAPI  XWIN8_putImage(MGLDC *dc,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,int op,void *surface,
    int bytesPerLine,MGLDC *src);

#endif /* __DRIVERS_UNIX_XWIN8_H */
