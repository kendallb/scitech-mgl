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
* Description:  Header file for the 8 bit accelerated driver.
*
****************************************************************************/

#ifndef __DRIVERS_OPENGL_OPENGL_H
#define __DRIVERS_OPENGL_OPENGL_H

#ifndef __DRIVERS_COMMON_GDDRAW_H
#include "drivers/common/gddraw.h"
#endif

/*------------------------- Function Prototypes ---------------------------*/

ibool   MGLAPI OPENGL_detect(int id,int *force,int *driver,int *mode,modetab availableModes);
ibool   MGLAPI OPENGL_initDriver(MGLDC *dc,int driverId,int modeId,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);

/* OpenGL surface compatible device rendering functions */
void    MGLAPI OPENGL_setColor(color_t color);
void    MGLAPI OPENGL_setBackColor(color_t color);
void    MGLAPI OPENGL_drawStrBitmap(int x,int y,const char *str);
void    MGLAPI OPENGL_putMonoImage(MGLDC *dc,int x,int y,int byteWidth,int height,uchar *image);
void    MGLAPI OPENGL_getScanLine(MGLDC *dc,int y,int x1,int x2,void *buffer,int bufx,int op);
void    MGLAPI OPENGL_putScanLine(MGLDC *dc,int y,int x1,int x2,void *buffer,int bufx,int op);
void    MGLAPI OPENGL_getImage(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op,void *surface,int bytesPerLine);
void    MGLAPI OPENGL_putImage(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op,void *surface,int bytesPerLine, MGLDC *src);
void    MGLAPI OPENGL_stretchBlt1x2(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal);
void    MGLAPI OPENGL_stretchBlt2x2(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal);
void    MGLAPI OPENGL_stretchBlt(MGLDC *dst,MGLDC *src,int left,int top,int right,int bottom,int dstLeft,int dstTop,fix32_t zoomx,fix32_t zoomy,palette_t *pal,ibool idPal);

#endif  /* __DRIVERS_OPENGL_OPENGL_H */

