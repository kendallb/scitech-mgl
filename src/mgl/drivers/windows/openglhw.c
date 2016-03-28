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
* Description:  Main hardware OpenGL device driver file. This file
*               will be linked in when a call to MGL_registerDriver() is
*               made to register this device driver for use. When using
*               hardware OpenGL we can only do rendering via OpenGL functions,
*               so this module layers the MGL API directly on top of the
*               OpenGL API. Applications using these drivers are most likely
*               going to be doing real OpenGL calls anyway so this should be
*               fine.
*
****************************************************************************/

#include "mgldd.h"
#ifdef __WINDOWS__
#include "drivers/opengl/openglhw.h"
#include "drivers/common/gfulldib.h"

/*---------------------------- Driver Tables -----------------------------*/

drivertype  _VARAPI OPENGL_driver = {
    IDENT_FLAG,
    MGL_OPENGLNAME,
    MGL_OPENGLNAME,
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1997 SciTech Software\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    grOPENGL,
    OPENGL_detect,
    OPENGL_initDriver,
    };

/* List of mode specific device context information */

PRIVATE gmode_t OPENGL8_modes[] = {
    {319,199,8,1,0xFF,0,0,1200,0,0},
    {319,239,8,1,0xFF,0,0,1000,0,0},
    {319,399,8,1,0xFF,0,0,600,0,0},
    {319,479,8,1,0xFF,0,0,500,0,0},
    {399,299,8,1,0xFF,0,0,1000,0,0},
    {511,383,8,1,0xFF,0,0,1000,0,0},
    {639,349,8,1,0xFF,0,0,1371,0,0},
    {639,399,8,1,0xFF,0,0,1200,0,0},
    {639,479,8,1,0xFF,0,0,1000,0,0},
    {799,599,8,1,0xFF,0,0,1000,0,0},
    {1023,767,8,1,0xFF,0,0,1000,0,0},
    {1151,863,8,1,0xFF,0,0,1000,0,0},
    {1279,959,8,1,0xFF,0,0,1000,0,0},
    {1279,1023,8,1,0xFF,0,0,938,0,0},
    {1599,1199,8,1,0xFF,0,0,1000,0,0},
    };

PRIVATE gmode_t OPENGL15_modes[] = {
    {319,199,15,1,0x7FFF,0,0,1200,0,0},
    {319,239,15,1,0x7FFF,0,0,1000,0,0},
    {319,399,15,1,0x7FFF,0,0,600,0,0},
    {319,479,15,1,0x7FFF,0,0,500,0,0},
    {399,299,15,1,0x7FFF,0,0,1000,0,0},
    {511,383,15,1,0x7FFF,0,0,1000,0,0},
    {639,349,15,1,0x7FFF,0,0,1371,0,0},
    {639,399,15,1,0x7FFF,0,0,1200,0,0},
    {639,479,15,1,0x7FFF,0,0,1000,0,0},
    {799,599,15,1,0x7FFF,0,0,1000,0,0},
    {1023,767,15,1,0x7FFF,0,0,1000,0,0},
    {1151,863,15,1,0x7FFF,0,0,1000,0,0},
    {1279,959,15,1,0x7FFF,0,0,1000,0,0},
    {1279,1023,15,1,0x7FFF,0,0,938,0,0},
    {1599,1199,15,1,0x7FFF,0,0,1000,0,0},
    };

PRIVATE gmode_t OPENGL16_modes[] = {
    {319,199,16,1,0xFFFF,0,0,1200,0,0},
    {319,239,16,1,0x7FFF,0,0,1000,0,0},
    {319,399,16,1,0x7FFF,0,0,600,0,0},
    {319,479,16,1,0x7FFF,0,0,500,0,0},
    {399,299,16,1,0xFFFF,0,0,1000,0,0},
    {511,383,16,1,0xFFFF,0,0,1000,0,0},
    {639,349,16,1,0xFFFF,0,0,1371,0,0},
    {639,399,16,1,0xFFFF,0,0,1200,0,0},
    {639,479,16,1,0xFFFF,0,0,1000,0,0},
    {799,599,16,1,0xFFFF,0,0,1000,0,0},
    {1023,767,16,1,0xFFFF,0,0,1000,0,0},
    {1151,863,16,1,0xFFFF,0,0,1000,0,0},
    {1279,959,16,1,0xFFFF,0,0,1000,0,0},
    {1279,1023,16,1,0xFFFF,0,0,938,0,0},
    {1599,1199,16,1,0xFFFF,0,0,1000,0,0},
    };

PRIVATE gmode_t OPENGL24_modes[] = {
    {319,199,24,1,0xFFFFFFL,0,0,1200,0,0},
    {319,239,24,1,0xFFFFFFL,0,0,1000,0,0},
    {319,399,24,1,0xFFFFFFL,0,0,600,0,0},
    {319,479,24,1,0xFFFFFFL,0,0,500,0,0},
    {399,299,24,1,0xFFFFFFL,0,0,1000,0,0},
    {511,383,24,1,0xFFFFFFL,0,0,1000,0,0},
    {639,349,24,1,0xFFFFFFL,0,0,1371,0,0},
    {639,399,24,1,0xFFFFFFL,0,0,1200,0,0},
    {639,479,24,1,0xFFFFFFL,0,0,1000,0,0},
    {799,599,24,1,0xFFFFFFL,0,0,1000,0,0},
    {1023,767,24,1,0xFFFFFFL,0,0,1000,0,0},
    {1151,863,24,1,0xFFFFFFL,0,0,1000,0,0},
    {1279,959,24,1,0xFFFFFFL,0,0,1000,0,0},
    {1279,1023,24,1,0xFFFFFFL,0,0,938,0,0},
    {1599,1199,24,1,0xFFFFFFL,0,0,1000,0,0},
    };

PRIVATE gmode_t OPENGL32_modes[] = {
    {319,199,32,1,0xFFFFFFFFL,0,0,1200,0,0},
    {319,239,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {319,399,32,1,0xFFFFFFFFL,0,0,600,0,0},
    {319,479,32,1,0xFFFFFFFFL,0,0,500,0,0},
    {399,299,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {511,383,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {639,349,32,1,0xFFFFFFFFL,0,0,1371,0,0},
    {639,399,32,1,0xFFFFFFFFL,0,0,1200,0,0},
    {639,479,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {799,599,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {1023,767,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {1151,863,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {1279,959,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    {1279,1023,32,1,0xFFFFFFFFL,0,0,938,0,0},
    {1599,1199,32,1,0xFFFFFFFFL,0,0,1000,0,0},
    };

PRIVATE drv_vec *g_state = NULL;    /* Global state buffer pointer  */

/*------------------------- Implementation --------------------------------*/

ibool MGLAPI OPENGL_detect(int id,int *force,int *driver,int *mode,
    modetab modes)
/****************************************************************************
*
* Function:     OPENGL_detect
* Parameters:   id      - Id of this driver for storing in mode table
*               force   - Highest performance driver to be used
*               driver  - Place to store detected driver id
*               mode    - Place to store recommended video mode
*               modes   - Place to store list of supported modes
* Returns:      TRUE if the device was found, FALSE if not.
*
* Description:  Determines if a hardware accelerated OpenGL device is
*               available. Note that we also require the presence of
*               DirectDraw as we use DirectDraw to change display modes
*               on the fly when we create our OpenGL window.
*
****************************************************************************/
{
    if (*force >= grOPENGL && DDOPENGL_detect()) {
        /* Fill in information here for the available video modes by
         * querying the OPENGL.DRV for the information.
         */
        FDIBOPENGL_useMode(modes,grSVGA_320x200x256,id,&OPENGL8_modes[0]);
        FDIBOPENGL_useMode(modes,grSVGA_320x240x256,id,&OPENGL8_modes[1]);
        FDIBOPENGL_useMode(modes,grSVGA_320x400x256,id,&OPENGL8_modes[2]);
        FDIBOPENGL_useMode(modes,grSVGA_320x480x256,id,&OPENGL8_modes[3]);
        FDIBOPENGL_useMode(modes,grSVGA_400x300x256,id,&OPENGL8_modes[4]);
        FDIBOPENGL_useMode(modes,grSVGA_512x384x256,id,&OPENGL8_modes[5]);
        FDIBOPENGL_useMode(modes,grSVGA_640x350x256,id,&OPENGL8_modes[6]);
        FDIBOPENGL_useMode(modes,grSVGA_640x400x256,id,&OPENGL8_modes[7]);
        FDIBOPENGL_useMode(modes,grSVGA_640x480x256,id,&OPENGL8_modes[8]);
        FDIBOPENGL_useMode(modes,grSVGA_800x600x256,id,&OPENGL8_modes[9]);
        FDIBOPENGL_useMode(modes,grSVGA_1024x768x256,id,&OPENGL8_modes[10]);
        FDIBOPENGL_useMode(modes,grSVGA_1152x864x256,id,&OPENGL8_modes[11]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x960x256,id,&OPENGL8_modes[12]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x1024x256,id,&OPENGL8_modes[13]);
        FDIBOPENGL_useMode(modes,grSVGA_1600x1200x256,id,&OPENGL8_modes[14]);

        FDIBOPENGL_useMode(modes,grSVGA_320x200x32k,id,&OPENGL15_modes[0]);
        FDIBOPENGL_useMode(modes,grSVGA_320x240x32k,id,&OPENGL15_modes[1]);
        FDIBOPENGL_useMode(modes,grSVGA_320x400x32k,id,&OPENGL15_modes[2]);
        FDIBOPENGL_useMode(modes,grSVGA_320x480x32k,id,&OPENGL15_modes[3]);
        FDIBOPENGL_useMode(modes,grSVGA_400x300x32k,id,&OPENGL15_modes[4]);
        FDIBOPENGL_useMode(modes,grSVGA_512x384x32k,id,&OPENGL15_modes[5]);
        FDIBOPENGL_useMode(modes,grSVGA_640x350x32k,id,&OPENGL15_modes[6]);
        FDIBOPENGL_useMode(modes,grSVGA_640x400x32k,id,&OPENGL15_modes[7]);
        FDIBOPENGL_useMode(modes,grSVGA_640x480x32k,id,&OPENGL15_modes[8]);
        FDIBOPENGL_useMode(modes,grSVGA_800x600x32k,id,&OPENGL15_modes[9]);
        FDIBOPENGL_useMode(modes,grSVGA_1024x768x32k,id,&OPENGL15_modes[10]);
        FDIBOPENGL_useMode(modes,grSVGA_1152x864x32k,id,&OPENGL15_modes[11]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x960x32k,id,&OPENGL15_modes[12]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x1024x32k,id,&OPENGL15_modes[13]);
        FDIBOPENGL_useMode(modes,grSVGA_1600x1200x32k,id,&OPENGL15_modes[14]);

        FDIBOPENGL_useMode(modes,grSVGA_320x200x64k,id,&OPENGL16_modes[0]);
        FDIBOPENGL_useMode(modes,grSVGA_320x240x64k,id,&OPENGL16_modes[1]);
        FDIBOPENGL_useMode(modes,grSVGA_320x400x64k,id,&OPENGL16_modes[2]);
        FDIBOPENGL_useMode(modes,grSVGA_320x480x64k,id,&OPENGL16_modes[3]);
        FDIBOPENGL_useMode(modes,grSVGA_400x300x64k,id,&OPENGL16_modes[4]);
        FDIBOPENGL_useMode(modes,grSVGA_512x384x64k,id,&OPENGL16_modes[5]);
        FDIBOPENGL_useMode(modes,grSVGA_640x350x64k,id,&OPENGL16_modes[6]);
        FDIBOPENGL_useMode(modes,grSVGA_640x400x64k,id,&OPENGL16_modes[7]);
        FDIBOPENGL_useMode(modes,grSVGA_640x480x64k,id,&OPENGL16_modes[8]);
        FDIBOPENGL_useMode(modes,grSVGA_800x600x64k,id,&OPENGL16_modes[9]);
        FDIBOPENGL_useMode(modes,grSVGA_1024x768x64k,id,&OPENGL16_modes[10]);
        FDIBOPENGL_useMode(modes,grSVGA_1152x864x64k,id,&OPENGL16_modes[11]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x960x64k,id,&OPENGL16_modes[12]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x1024x64k,id,&OPENGL16_modes[13]);
        FDIBOPENGL_useMode(modes,grSVGA_1600x1200x64k,id,&OPENGL16_modes[14]);

        FDIBOPENGL_useMode(modes,grSVGA_320x200x16m,id,&OPENGL24_modes[0]);
        FDIBOPENGL_useMode(modes,grSVGA_320x240x16m,id,&OPENGL24_modes[1]);
        FDIBOPENGL_useMode(modes,grSVGA_320x400x16m,id,&OPENGL24_modes[2]);
        FDIBOPENGL_useMode(modes,grSVGA_320x480x16m,id,&OPENGL24_modes[3]);
        FDIBOPENGL_useMode(modes,grSVGA_400x300x16m,id,&OPENGL24_modes[4]);
        FDIBOPENGL_useMode(modes,grSVGA_512x384x16m,id,&OPENGL24_modes[5]);
        FDIBOPENGL_useMode(modes,grSVGA_640x350x16m,id,&OPENGL24_modes[6]);
        FDIBOPENGL_useMode(modes,grSVGA_640x400x16m,id,&OPENGL24_modes[7]);
        FDIBOPENGL_useMode(modes,grSVGA_640x480x16m,id,&OPENGL24_modes[8]);
        FDIBOPENGL_useMode(modes,grSVGA_800x600x16m,id,&OPENGL24_modes[9]);
        FDIBOPENGL_useMode(modes,grSVGA_1024x768x16m,id,&OPENGL24_modes[10]);
        FDIBOPENGL_useMode(modes,grSVGA_1152x864x16m,id,&OPENGL24_modes[11]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x960x16m,id,&OPENGL24_modes[12]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x1024x16m,id,&OPENGL24_modes[13]);
        FDIBOPENGL_useMode(modes,grSVGA_1600x1200x16m,id,&OPENGL24_modes[14]);

        FDIBOPENGL_useMode(modes,grSVGA_320x200x4G,id,&OPENGL32_modes[0]);
        FDIBOPENGL_useMode(modes,grSVGA_320x240x4G,id,&OPENGL32_modes[1]);
        FDIBOPENGL_useMode(modes,grSVGA_320x400x4G,id,&OPENGL32_modes[2]);
        FDIBOPENGL_useMode(modes,grSVGA_320x480x4G,id,&OPENGL32_modes[3]);
        FDIBOPENGL_useMode(modes,grSVGA_400x300x4G,id,&OPENGL32_modes[4]);
        FDIBOPENGL_useMode(modes,grSVGA_512x384x4G,id,&OPENGL32_modes[5]);
        FDIBOPENGL_useMode(modes,grSVGA_640x350x4G,id,&OPENGL32_modes[6]);
        FDIBOPENGL_useMode(modes,grSVGA_640x400x4G,id,&OPENGL32_modes[7]);
        FDIBOPENGL_useMode(modes,grSVGA_640x480x4G,id,&OPENGL32_modes[8]);
        FDIBOPENGL_useMode(modes,grSVGA_800x600x4G,id,&OPENGL32_modes[9]);
        FDIBOPENGL_useMode(modes,grSVGA_1024x768x4G,id,&OPENGL32_modes[10]);
        FDIBOPENGL_useMode(modes,grSVGA_1152x864x4G,id,&OPENGL32_modes[11]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x960x4G,id,&OPENGL32_modes[12]);
        FDIBOPENGL_useMode(modes,grSVGA_1280x1024x4G,id,&OPENGL32_modes[13]);
        FDIBOPENGL_useMode(modes,grSVGA_1600x1200x4G,id,&OPENGL32_modes[14]);
        *driver = grOPENGL;
        return true;
        }
    mode = mode;
    return false;
}

PRIVATE void MGLAPI destroyDC(MGLDC *dc)
/****************************************************************************
*
* Function:     destroyDC
* Parameters:   dc  - Device context
*
* Description:  Destroys all internal memory allocated by the device
*               context.
*
****************************************************************************/
{
    if (--dc->v->d.refCount == 0) {
        if (dc->v != g_state)
            MGL_fatalError("Internal error: dc->v != g_state!\n");
        PM_free(dc->v);
        g_state = NULL;
        }
    /* If we have a palette associated with this DC, then destroy it */
    if (dc->wm.windc.hpal)
        DeleteObject(dc->wm.windc.hpal);
    if (dc->wm.windc.hdc)
        ReleaseDC(dc->wm.windc.hwnd,dc->wm.windc.hdc);
}

ulong MGLAPI OPENGL_getWinDC(MGLDC *dc)
{ return (ulong)dc->wm.windc.hdc; }

void OPENGL_initInternal(MGLDC *dc)
/****************************************************************************
*
* Function:     OPENGL_initInternal
* Parameters:   dc  - Device context.
* Returns:      True if the device was correctly initialised.
*
* Description:  Initialises the device driver internal tables for 8 bit
*               unaccelerated driver routines.
*
****************************************************************************/
{
    // TODO: Flesh these functions out with versions that render via
    //       OpenGL so the MGL drawing functions can be used directly with
    //       OpenGL surfaces.
    dc->r.setColor              = OPENGL_setColor;
    dc->r.setBackColor          = OPENGL_setBackColor;
    dc->r.setWriteMode          = DRV_setWriteMode;
    dc->r.setClipRect           = DRV_setClipRect;
#ifndef MGL_LITE
    dc->r.setPenStyle           = DRV_setPenStyle;
    dc->r.setPenBitmapPattern   = DRV_setPenBitmapPattern;
    dc->r.setPenPixmapPattern   = DRV_setPenPixmapPattern;
    dc->r.setLineStipple        = DRV_setLineStipple;
    dc->r.setLineStippleCount   = DRV_setLineStippleCount;
#endif

//  dc->r.getPixel              = OPENGL_getPixel;
//  dc->r.putPixel              = OPENGL_putPixel;
    dc->r.getScanLine           = OPENGL_getScanLine;
    dc->r.putScanLine           = OPENGL_putScanLine;
//  _MGL_setLineVecs(dc,          OPENGL_line);
//  dc->r.solid.scanLine        = OPENGL_scanLine;
//  dc->r.ropSolid.scanLine     = OPENGL_scanLine;
//  _MGL_setFillRectVecs(dc,      OPENGL_fillRect);
    dc->r.putMonoImage          = OPENGL_putMonoImage;
//  dc->r.putMouseImage         = OPENGL_putMouseImage;
    dc->r.getImage              = OPENGL_getImage;
    dc->r.putImage              = OPENGL_putImage;
//  dc->r.divotSize             = OPENGL_divotSize;
//  dc->r.getDivot              = OPENGL_getDivot;
//  dc->r.putDivot              = OPENGL_putDivot;
//  dc->r.stretchScanLine2x     = OPENGL_stretchScanLine2x;
//  dc->r.stretchScanLine       = OPENGL_stretchScanLine;
    dc->r.stretchBlt1x2         = OPENGL_stretchBlt1x2;
    dc->r.stretchBlt2x2         = OPENGL_stretchBlt2x2;
    dc->r.stretchBlt            = OPENGL_stretchBlt;

#ifndef MGL_LITE
//  dc->r.stippleLine           = OPENGL_stippleLine;
//  _MGL_setTrapVecs(dc,          OPENGL_trap);
//  dc->r.putSrcTransparent     = OPENGL_putSrcTransparent;
//  dc->r.putDstTransparent     = OPENGL_putDstTransparent;
    dc->r.patt.line             = __EMU__fatPenLine;
    dc->r.colorPatt.line        = __EMU__fatPenLine;
    dc->r.fatSolid.line         = __EMU__fatPenLine;
    dc->r.fatRopSolid.line      = __EMU__fatPenLine;
    dc->r.fatPatt.line          = __EMU__fatPenLine;
    dc->r.fatColorPatt.line     = __EMU__fatPenLine;
//  dc->r.patt.scanLine         = OPENGL_scanLine;
//  dc->r.colorPatt.scanLine    = OPENGL_scanLine;
//  dc->r.fatSolid.scanLine     = OPENGL_scanLine;
//  dc->r.fatRopSolid.scanLine  = OPENGL_scanLine;
//  dc->r.fatPatt.scanLine      = OPENGL_scanLine;
//  dc->r.fatColorPatt.scanLine = OPENGL_scanLine;
//  dc->r.dither.scanLine       = OPENGL_scanLine;
    _MGL_setDrawScanListVecs(dc,  __EMU__drawScanList);
    _MGL_setDrawRegionVecs(dc,    __EMU__drawRegion);
    _MGL_setEllipseVecs(dc,       __EMU__ellipse);
//  _MGL_setEllipseVecs(dc,       OPENGL_ellipse);
    dc->r.fatSolid.ellipse      = __EMU__fatPenEllipse;
    dc->r.fatRopSolid.ellipse   = __EMU__fatPenEllipse;
    dc->r.fatPatt.ellipse       = __EMU__fatPenEllipse;
    dc->r.fatColorPatt.ellipse  = __EMU__fatPenEllipse;
    _MGL_setFillEllipseVecs(dc,   __EMU__fillEllipse);
    _MGL_setEllipseArcVecs(dc,    __EMU__ellipseArc);
    dc->r.fatSolid.ellipseArc   = __EMU__fatPenEllipseArc;
    dc->r.fatRopSolid.ellipseArc= __EMU__fatPenEllipseArc;
    dc->r.fatPatt.ellipseArc    = __EMU__fatPenEllipseArc;
    dc->r.fatColorPatt.ellipseArc= __EMU__fatPenEllipseArc;
    _MGL_setFillEllipseArcVecs(dc,__EMU__fillEllipseArc);
    dc->r.getArcCoords          = __EMU__getArcCoords;
    dc->r.drawStrBitmap         = OPENGL_drawStrBitmap;
    dc->r.drawCharVec           = __EMU__drawCharVec;
    dc->r.complexPolygon        = __EMU__complexPolygon;
    dc->r.polygon               = __EMU__polygon;
    dc->r.ditherPolygon         = __EMU__polygon;
//  dc->r.translateImage        = OPENGL_translateImage;
//  dc->r.bitBlt                = OPENGL_bitBlt;
//  dc->r.srcTransBlt           = OPENGL_srcTransBlt;
//  dc->r.dstTransBlt           = OPENGL_dstTransBlt;
//  dc->r.scanRightForColor     = OPENGL_scanRightForColor;
//  dc->r.scanLeftForColor      = OPENGL_scanLeftForColor;
//  dc->r.scanRightWhileColor   = OPENGL_scanRightWhileColor;
//  dc->r.scanLeftWhileColor    = OPENGL_scanLeftWhileColor;
#endif
}

ibool MGLAPI OPENGL_initDriver(MGLDC *dc,int driverId,int modeId,ulong hwnd,
    int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate)
/****************************************************************************
*
* Function:     OPENGL_initDriver
* Parameters:   dc  - Device context.
* Returns:      True if the device was correctly initialised.
*
* Description:  Initialises the device driver, and starts the specified
*               graphics mode. This is also where we fill in all of the
*               vectors in the device context to initialise our device
*               context properly.
*
****************************************************************************/
{
    FULLDIB_state   *state;
    display_vec *d;

    /* Allocate a single buffer for the driver specific state buffer */
    if (stereo)
        return false;
    if (modeId >= grSVGA_320x200x4G)
        dc->mi = OPENGL32_modes[modeId - grSVGA_320x200x4G];
    else if (modeId >= grSVGA_320x200x16m)
        dc->mi = OPENGL24_modes[modeId - grSVGA_320x200x16m];
    else if (modeId >= grSVGA_320x200x64k)
        dc->mi = OPENGL16_modes[modeId - grSVGA_320x200x64k];
    else if (modeId >= grSVGA_320x200x32k)
        dc->mi = OPENGL15_modes[modeId - grSVGA_320x200x32k];
    else
        dc->mi = OPENGL8_modes[modeId - grSVGA_320x200x256];
    if (g_state == NULL) {
        if ((g_state = PM_calloc(1,sizeof(FULLDIB_state))) == NULL) {
            _MGL_result = grNoMem;
            return false;
            }
        }
    state = (FULLDIB_state*)(dc->v = g_state);
    d = &dc->v->d;
    d->refCount++;
    d->graphDriver = driverId;
    d->graphMode = modeId;

    /* Load the device context with the device information for the
     * selected video mode. By default we set up the device to use the
     * software rendering routines, and the FULLDIB_setMode() function
     * will re-vector the appropriate routines to the hardware accelerated
     * versions depending on what the hardware supports.
     */
    dc->deviceType          = MGL_OPENGL_DEVICE;
    dc->xInch               = 9000;
    dc->yInch               = 7000;
    d->hardwareCursor       = FALSE;
    d->setCursorColor       = DRV_stubVector;
    d->setCursor            = DRV_stubVector;
    d->setCursorPos         = DRV_stubVector;
    d->showCursor           = DRV_stubVector;
    d->restoreTextMode      = FULLDIB_restoreTextMode;
    d->restoreGraphMode     = DRV_stubVector;
    d->makeCurrent          = FULLDIB_makeCurrent;
    d->destroy              = destroyDC;
    d->makeOffscreenDC      = NULL;
    d->makeLinearOffscreenDC = NULL;
    if (!FDIBOPENGL_setMode(dc))
        return false;
    dc->wm.windc.hwnd = (HWND)_MGL_hwndFullScreen;
    dc->wm.windc.hdc = GetDC(dc->wm.windc.hwnd);

    /* Fill in required device context vectors */
    dc->r.getWinDC              = OPENGL_getWinDC;
    dc->r.getDefaultPalette     = WIN_getDefaultPalette;
    dc->r.realizePalette        = WIN_realizePalette;
    dc->r.setClipRect           = DRV_setClipRect;

    /* Fill in OpenGL specific rendering vectors */
    OPENGL_initInternal(dc);
    return true;
}

#pragma warn -par

void MGLAPI OPENGL_setColor(color_t color)
{
    DC.a.color = DC.intColor = color;
    if (_MGL_hInstOpenGL) {
        uchar R,G,B;
        MGL_unpackColorFast(&_MGL_dc.pf,_MGL_dc.intColor,R,G,B);
        glColor3ub(R,G,B);
        }
}

void MGLAPI OPENGL_setBackColor(color_t color)
{
    DC.a.backColor = DC.intBackColor = color;
}

void MGLAPI OPENGL_getScanLine(MGLDC *dc,int y,int x1,int x2,void *buffer,
    int bufx,int op)
/****************************************************************************
*
* Function:     OPENGL_getScanLine
* Parameters:   dc      - Device context
*               y       - Scanline to get data from
*               x1      - Starting coordinate in scanline
*               y2      - Ending coordinate in scanline
*               buffer  - Pointer to buffer to store data in
*               bufx    - X coordinate in buffer (ignored)
*               op      - Write mode op to use during transfer
*
* Description:  Reads a scanline of information from the device context
*               into the memory buffer. Note that this routine _always_
*               stores data in the memory buffer in packed pixel format.
*
*               The write mode operation determines how the data will be
*               written into the memory buffer.
*
****************************************************************************/
{
    // TODO: Implement using glReadPixels
}

void MGLAPI OPENGL_putScanLine(MGLDC *dc,int y,int x1,int x2,void *buffer,
    int bufx,int op)
/****************************************************************************
*
* Function:     OPENGL_putScanLine
* Parameters:   dc      - Device context
*               y       - Scanline to get data from
*               x1      - Starting coordinate in scanline
*               y2      - Ending coordinate in scanline
*               buffer  - Pointer to buffer to get data from
*               bufx    - X coordinate in buffer (ignored)
*               op      - Write mode op to use during transfer
*
* Description:  Dumps a scanline of information to the device context
*               from the memory buffer. Note that this routine _always_
*               reads data from the memory buffer in packed pixel format.
*
*               The write mode operation determines how the data will be
*               written into the display context.
*
****************************************************************************/
{
    // TODO: Implement using glDrawPixels
}

void MGLAPI OPENGL_drawStrBitmap(int x,int y,const char *str)
/****************************************************************************
*
* Function:     OPENGL_drawStrBitmap
* Parameters:   x   - X coordinate to draw at
*               y   - Y coordinate to draw at
*               str - String to draw
*
* Description:  Draws the specified string at the current position (CP) in
*               the current drawing color, write mode and justification
*               using the current bitmap font. We call the OpenGL glBitmap
*               function to draw the bitmaps in the text string as efficiently
*               as possible.
*
****************************************************************************/
{
    int             i,width,byteWidth,height,ascent,descent,offset;
    int             fontAscent,iwidth,bitmapHeight;
    uchar           *bytes,*buf = _MGL_buf;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;

    /* Enable 2D OpenGL drawing */
    if (!_MGL_dc.mglEnabled)
        MGL_glEnableMGLFuncs(&_MGL_dc);

    /* Find the ascent, descent, maxKern and maxWidth values */
    ascent = font->ascent;          descent = font->descent;
    fontAscent = font->fontHeight + descent - 1;
    bitmapHeight = ((font->fontHeight + 7) & ~0x7) - 1;

    /* Adjust the first characters origin depending on the vertical
     * justification setting.
     */
    switch (ts.vertJust) {
        case MGL_TOP_TEXT:
            height = fontAscent - ascent;
            y -= (ts.dir == MGL_RIGHT_DIR ? height :
                 (ts.dir == MGL_LEFT_DIR ? font->fontHeight-1 - height : 0));
            x -= (ts.dir == MGL_UP_DIR ? height :
                 (ts.dir == MGL_DOWN_DIR ? bitmapHeight - height : 0));
            break;
        case MGL_CENTER_TEXT:
            height = fontAscent - ascent + (ascent-descent)/2;
            y -= (ts.dir == MGL_RIGHT_DIR ? height :
                 (ts.dir == MGL_LEFT_DIR ? font->fontHeight-1 - height : 0));
            x -= (ts.dir == MGL_UP_DIR ? height :
                 (ts.dir == MGL_DOWN_DIR ? bitmapHeight - height : 0));
            break;
        case MGL_BASELINE_TEXT:
            y -= (ts.dir == MGL_RIGHT_DIR ? fontAscent :
                 (ts.dir == MGL_LEFT_DIR ? font->fontHeight-1 - fontAscent : 0));
            x -= (ts.dir == MGL_UP_DIR ? fontAscent :
                 (ts.dir == MGL_DOWN_DIR ? bitmapHeight - fontAscent : 0));
            break;
        case MGL_BOTTOM_TEXT:
            height = font->fontHeight-1;
            y -= (ts.dir == MGL_RIGHT_DIR ? height :
                 (ts.dir == MGL_LEFT_DIR ? font->fontHeight-1 - height : 0));
            x -= (ts.dir == MGL_UP_DIR ? height :
                 (ts.dir == MGL_DOWN_DIR ? bitmapHeight - height : 0));
            break;
        }

    /* Now draw the string using a different loop for each of the four
     * text drawing directions. Note that we maintain a character clipping
     * rectangle through each loop, and check to see if the character can
     * be entirely rejected.
     */
    width = MGL_textWidth(str);
    height = font->fontHeight;

    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x - width/2 :
                    x - width));

            while (*str) {
                width = BITFONT(font)->width[(uchar)*str];
                iwidth = BITFONT(font)->iwidth[(uchar)*str];
                if ((offset = BITFONT(font)->offset[(uchar)*str++]) == -1) {
                    offset = BITFONT(font)->offset[MISSINGSYMBOL];
                    width = BITFONT(font)->width[MISSINGSYMBOL];
                    iwidth = BITFONT(font)->iwidth[MISSINGSYMBOL];
                    }
                bytes = (uchar *)&BITFONT(font)->def[offset];
                byteWidth = (iwidth + 7) >> 3;
                if (iwidth != 0) {
                    _MGL_rotateBitmap180(buf,bytes,byteWidth,height);
                    glRasterPos2i(i,y+height-1);
                    glBitmap(byteWidth * 8, height, 0, 0, 0, 0, buf);
                    }
                i += width;
                }
            break;
        case MGL_UP_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y + width/2 :
                    y + width));

            while (*str) {
                width = BITFONT(font)->width[(uchar)*str];
                iwidth = BITFONT(font)->iwidth[(uchar)*str];
                if ((offset = BITFONT(font)->offset[(uchar)*str++]) == -1) {
                    offset = BITFONT(font)->offset[MISSINGSYMBOL];
                    width = BITFONT(font)->width[MISSINGSYMBOL];
                    iwidth = BITFONT(font)->iwidth[MISSINGSYMBOL];
                    }
                bytes = (uchar *)&BITFONT(font)->def[offset];
                byteWidth = (iwidth + 7) >> 3;
                if (iwidth != 0) {
                    height = font->fontHeight;
                    MGL_rotateGlyph(buf,bytes,&byteWidth,&height,MGL_DOWN_DIR);
                    glRasterPos2i(x,i);
                    glBitmap(byteWidth * 8, height, 0, 0, 0, 0, buf);
                    }
                i -= width;
                }
            break;
        case MGL_DOWN_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y - width/2 :
                    y - width));

            while (*str) {
                width = BITFONT(font)->width[(uchar)*str];
                iwidth = BITFONT(font)->iwidth[(uchar)*str];
                if ((offset = BITFONT(font)->offset[(uchar)*str++]) == -1) {
                    offset = BITFONT(font)->offset[MISSINGSYMBOL];
                    width = BITFONT(font)->width[MISSINGSYMBOL];
                    iwidth = BITFONT(font)->iwidth[MISSINGSYMBOL];
                    }
                bytes = (uchar *)&BITFONT(font)->def[offset];
                byteWidth = (iwidth + 7) >> 3;
                if (iwidth != 0) {
                    height = font->fontHeight;
                    MGL_rotateGlyph(buf,bytes,&byteWidth,&height,MGL_UP_DIR);
                    glRasterPos2i(x,i+height-1);
                    glBitmap(byteWidth * 8, height, 0, 0, 0, 0, buf);
                    }
                i += width;
                }
            break;
        case MGL_LEFT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x + width/2 :
                    x + width));

            while (*str) {
                width = BITFONT(font)->width[(uchar)*str];
                iwidth = BITFONT(font)->iwidth[(uchar)*str];
                if ((offset = BITFONT(font)->offset[(uchar)*str++]) == -1) {
                    offset = BITFONT(font)->offset[MISSINGSYMBOL];
                    width = BITFONT(font)->width[MISSINGSYMBOL];
                    iwidth = BITFONT(font)->iwidth[MISSINGSYMBOL];
                    }
                bytes = (uchar *)&BITFONT(font)->def[offset];
                byteWidth = (iwidth + 7) >> 3;
                if (iwidth != 0) {
                    glRasterPos2i(i-(byteWidth<<3)+1,y+height-1);
                    glBitmap(byteWidth * 8, height, 0, 0, 0, 0, buf);
                    }
                i -= width;
                }
            break;
        }

    /* Re-enable 3D OpenGL drawing */
    if (_MGL_dc.mglEnabled)
        MGL_glDisableMGLFuncs(&_MGL_dc);
}

void MGLAPI OPENGL_putMonoImage(MGLDC *dc,int x,int y,int byteWidth,
    int height,uchar *image)
/****************************************************************************
*
* Function:     OPENGL_putMonoImage
* Parameters:   dc      - Device context
* Parameters:   x           - X coordinate to place image at
*               y           - Y coordinate to place image at
*               byteWidth   - Width of the image in bytes (8 pixels)
*               height      - Height of the image
*               image       - Pointer to the image data
*
* Description:  Draws a monochromatic image at the position (x,y) using
*               the definition provided by the data pointed to by 'image'.
*               The bitmap image definition should be width*height bytes in
*               size. Where a bit is a 1 in the image defintion, a pixel
*               will be plotted in the current foreground color. Where a bit
*               is a zero, the data is left unchanged.
*
*               We emulate this by repeatedly calling the MGL_pixelCoord
*               routine (to ensure correct clipping).
*
*               The image will be drawn in the current drawing color and
*               in the current write mode.
*
****************************************************************************/
{
    uchar *buf = _MGL_buf;

    if (!dc->mglEnabled)
        MGL_glEnableMGLFuncs(dc);
    glRasterPos2i(x,y+height-1);
    _MGL_rotateBitmap180(buf,image,byteWidth,height);
    glBitmap(byteWidth * 8, height, 0, 0, 0, 0, buf);
    if (dc->mglEnabled)
        MGL_glDisableMGLFuncs(dc);
}

void MGLAPI OPENGL_getImage(MGLDC *dc,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,int op,void *surface,int bytesPerLine)
/****************************************************************************
*
* Function:     OPENGL_getImage
* Parameters:   dc      - Device context
*
* Description:  Blt's a section from an OpenGL surface to a memory DC.
*
****************************************************************************/
{
    // TODO: Implement this using glReadPixels.
}

void MGLAPI OPENGL_putImage(MGLDC *dc,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,int op,void *surface,
    int bytesPerLine,MGLDC *src)
/****************************************************************************
*
* Function:     OPENGL_putImage
* Parameters:   dc      - Device context
*
* Description:  Blt's a system memory buffer DC of the same pixel depth
*               as the display device context to the display DC.
*
****************************************************************************/
{
    // TODO: Implement this using the OpenGL glDrawPixels function.
}

void MGLAPI OPENGL_stretchBlt1x2(MGLDC *dst,MGLDC *src,int left,int top,
    int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     OPENGL_stretchBlt1x2
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of possibly differing
*               pixel depths to the display device context with stretching.
*
****************************************************************************/
{
    OPENGL_stretchBlt(dst,src,left,top,right,bottom,dstLeft,dstTop,
        MGL_FIX_1,MGL_FIX_2,pal,idPal);
}

void MGLAPI OPENGL_stretchBlt2x2(MGLDC *dst,MGLDC *src,int left,int top,
    int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     OPENGL_stretchBlt2x2
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of possibly differing
*               pixel depths to the display device context with stretching.
*
****************************************************************************/
{
    OPENGL_stretchBlt(dst,src,left,top,right,bottom,dstLeft,dstTop,
        MGL_FIX_2,MGL_FIX_2,pal,idPal);
}

void MGLAPI OPENGL_stretchBlt(MGLDC *dst,MGLDC *src,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,fix32_t zoomx,fix32_t zoomy,
    palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     OPENGL_stretchBlt
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of the same pixel depth
*               as the display device context to the display DC with stretching.
*
****************************************************************************/
{
    // Implement this using the OpenGL glDrawPixels function.
}

#endif  /* __WINDOWS__ */

