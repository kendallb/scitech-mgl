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
* Description:  Common header file for internal device driver code used
*               by all device drivers.
*
****************************************************************************/

#ifndef __DRIVERS_COMMON_COMMON_H
#define __DRIVERS_COMMON_COMMON_H

/*---------------------- Macro and type definitions -----------------------*/

#define DRV_useMode(modes,mode,id,_xRes,_yRes,_bits,pgs,flgs)   \
   {if (modes[mode].driver == 0xFF) {                           \
    modes[mode].driver = id;                                    \
    modes[mode].xRes = _xRes;                                   \
    modes[mode].yRes = _yRes;                                   \
    modes[mode].bits = _bits;                                   \
    modes[mode].pages = pgs;                                    \
    modes[mode].flags = flgs;}}

#define DRV_removeMode(modes,mode)          \
   {modes[mode].driver = 0xFF;              \
    modes[mode].xRes = 0;                   \
    modes[mode].yRes = 0;                   \
    modes[mode].bits = 0;                   \
    modes[mode].pages = 0;                  \
    modes[mode].flags = 0;}

#define SET_PTR(d,p)    *((ulong*)&(d)) = (ulong)(p)

#define PACKED24_pixelAddr(x,y) \
     ((void*)((uchar*)DC.surface + ((long)y * DC.mi.bytesPerLine) + x*3))

#define PACKED24_pixelAddr2(dc,x,y) \
     ((void*)((uchar*)(dc)->surface + ((long)y * (dc)->mi.bytesPerLine) + x*3))

#define MGL_packColorRGB(R,G,B)                                     \
 (((R) & 0xFF0000L) | (((G) >> 8) & 0xFF00L) | ((B) >> 16))

#define MGL_packColorBGR(R,G,B)                                     \
 (((B) & 0xFF0000L) | (((G) >> 8) & 0xFF00L) | ((R) >> 16))

extern palette_t _VARAPI _VGA8_defPal[256];
extern palette_t _VARAPI _VGA4_defPal[16];

/*------------------------- Function Prototypes ---------------------------*/

/* Generic device driver C helper routines */

void    DRV_setSNAPRenderingVectors(MGLDC *dc,REF2D_driver *ref2d);
void    MGLAPI DRV_ClipMonoBltSys(MGLDC *dc,N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,GA_color foreColor,GA_color backColor,N_int32 mix,N_int32 flags,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
void    MGLAPI DRV_makeCurrent(MGLDC *dc,ibool partial);
void    * MGLAPI DRV_getDefaultPalette(MGLDC *dc);

#endif  /* __DRIVERS_COMMON_COMMON_H */

