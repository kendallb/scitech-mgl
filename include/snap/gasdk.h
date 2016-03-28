/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Header file for a small, simple SDK to access SNAP graphics
*               directly. This is mostly used by the SNAP Graphics test
*               and sample programs. It is not intended as a complete SDK.
*
****************************************************************************/

#ifndef __SNAP_GASDK_H
#define __SNAP_GASDK_H

#include "snap/graphics.h"
#include "snap/ref2d.h"

/*---------------------- Macros and type definitions ----------------------*/

/* Values stored in the isStereo global */

#define gaNoStereo      0       /* Not running in stereo mode           */
#define gaStereoOff     1       /* Stereo flipping is disabled          */
#define gaStereoOn      2       /* Stereo flipping is enabled           */

/* Defines for left and right buffers for SetActivePage() */

#define gaLeftBuffer    0x0000  /* Default is for left buffer           */
#define gaRightBuffer   0x8000  /* Signal to make right buffer active   */

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Global variables in gasdk.c */

extern N_uint16         cntMode;
extern N_int32          x,y,maxX,maxY,maxPage,activePage,visualPage,maxMem;
extern N_int32          virtualX,virtualY,bytesPerLine,bytesPerPixel;
extern N_int32          isStereo,stereoRunning,useBlueCode,blueCodeIndex;
extern GA_color         defcolor,maxcolor;
extern GA_modeInfo      modeInfo;
extern REF2D_driver     *ref2d;
extern GA_devCtx        *dc;
extern GA_driverFuncs   driver;
extern GA_2DStateFuncs  drv_state2d,state2d;
extern GA_2DRenderFuncs drv_draw2d,draw2d;
extern GA_palette       VGA8_defPal[256];
extern GA_palette       VGA4_defPal[64];
extern ibool            softwareOnly;
extern GA_bufferFuncs   bufmgr;
extern GA_buf           *primaryBuf;
extern GA_buf           *activeBuf;

/*------------------------- Function Prototypes ---------------------------*/

/* Rendering functions in gasdk.c */

GA_color rgbColor(uchar r,uchar g,uchar b);
GA_color rgbColorEx(uchar r,uchar g,uchar b,GA_pixelFormat *pf);
GA_color realColor(int index);
GA_color realColorEx(int index,int bitsPerPixel,uint colorMask,GA_pixelFormat *pf);
GA_color rgbColorPadded(uchar r,uchar g,uchar b);
GA_color rgbColorPaddedEx(uchar r,uchar g,uchar b,GA_pixelFormat *pf);
GA_color realColorPadded(int index);
GA_color realColorPaddedEx(int index,int bitsPerPixel,uint colorMask,GA_pixelFormat *pf);
void    SetForeColor(GA_color color);
void    SetBackColor(GA_color color);
void    SetMix(N_int32 mix);
void    Set8x8MonoPattern(N_int32 index,GA_pattern *pattern);
void    Use8x8MonoPattern(N_int32 index);
void    Use8x8TransMonoPattern(N_int32 index);
void    Set8x8ColorPattern(N_int32 index,GA_colorPattern *pattern);
void    Use8x8ColorPattern(N_int32 index);
void    Use8x8TransColorPattern(N_int32 index,GA_color transparent);
void    SetLineStipple(GA_stipple stipple);
void    SetLineStippleCount(N_uint32 count);
void    SetPlaneMask(N_uint32 planeMask);
void    SetActiveBuffer(GA_buf *buf);
void    SetActivePage(int page);
void    SetVisualPage(int page,int waitVRT);
void    SetVisualPageXY(int page,int x,int y,int waitVRT);
void    EnableShadowBuffer(int bitsPerPixel,GA_pixelFormat *pf,int flags);
void    CopyShadowBuffer(void);
ibool   StartStereo(void);
void    StopStereo(void);
void    ClearPage(GA_color color);
void    ClipMonoBltSys(int x,int y,int width,int height,int byteWidth,uchar *image,GA_color foreColor,GA_color backColor,int mix,int flags,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    WriteText(int x,int y,char *s,GA_color color);
void    WriteTextSlow(int x,int y,char *s,GA_color color);
void    gmoveto(int _x,int _y);
void    gnewline(void);
int     ggetx(void);
int     ggety(void);
int     gprintf(char *fmt, ... );
ibool   InitSoftwareRasterizer(int deviceIndex,int reservePages,ibool stereo);
void    ExitSoftwareRasterizer(void);
void    ForceSoftwareOnly(void);
void    RevertSoftwareOnly(void);
void    _ASMAPI SetBankDL(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __SNAP_GASDK_H */

