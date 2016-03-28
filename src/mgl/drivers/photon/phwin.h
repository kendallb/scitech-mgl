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
* Description:  Main PHWIN device driver header file. This file contains all
*       of the C language declarations and definitions for the
*       PHWIN device driver.
*
****************************************************************************/

#ifndef __DRIVERS_QNX_PHWIN_H
#define __DRIVERS_QNX_PHWIN_H

/*------------------------- Function Prototypes ---------------------------*/

/* PHWIN C based routines */

ibool   MGLAPI PHWIN_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
void *  MGLAPI PHWIN_createInstance(void);
void    MGLAPI PHWIN_destroyInstance(void *data);

int     PH_getBitsPerPixel(MGLDC *dc);
void    PH_findPixelFormat(MGLDC *dc);
ulong   MGLAPI PH_getWinDC(MGLDC *dc);
ibool   MGLAPI PH_noZbuf(MGLDC *dc);
void *  MGLAPI PH_getDefaultPalette(MGLDC *dc);
void    MGLAPI PH_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
void    MGLAPI PH_putImage(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op,void *surface,int bytesPerLine, MGLDC *src);
void    MGLAPI PH_translateImage(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op,void *surface,int bytesPerLine,int bitsPerPixel,palette_t *pal,pixel_format_t *pf,MGLDC *src);
void    MGLAPI PH_stretchBlt1x2(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal);
void    MGLAPI PH_stretchBlt2x2(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal);
void    MGLAPI PH_stretchBlt(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,fix32_t zoomx,fix32_t zoomy,palette_t *pal,ibool idPal);

#endif  /* __DRIVERS_QNX_PHWIN_H */

