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
* Description:  MGL 4.0 API compatibility header file for the SciTech MGL.
*
****************************************************************************/

#ifndef __MGL40_H
#define __MGL40_H

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(4)

/****************************************************************************
REMARKS:
Defines the graphics subsystems available. This is the value returned in
the driver parameter for MGL_detectGraph and MGL_init. Note that if a driver
returns a value of grOPENGL_MGL_MINI, it is an MGL OpenGL mini-driver and
does not implement the entire OpenGL API. It is up to the application
developer to find out from the hardware vendor who supplied the driver
exactly what features are supported and to make use of those features in
their application.

HEADER:
mgraph.h

MEMBERS:
grDETECT            - Auto detect the graphics subsystem
grNONE              - No graphics hardware detected
grVGA               - Standard VGA
grFULLDIB           - Fullscreen DIBSection's on Win95/NT
grVESA              - VESA VBE compliant SuperVGA
grSVGA              - Unaccelerated SuperVGA
grACCEL             - Accelerated SuperVGA
grDDRAW             - Unaccelerated DirectDraw
grDDRAWACCEL        - Accelerated DirectDraw
grDDRAW3D           - 3D Accelerated DirectDraw
grOPENGL            - Hardware Accelerated OpenGL (ICD/MCD)
grOPENGL_MGL_MINI   - MGL specific hardware OpenGL mini-driver
grOPENGL_MGL        - MGL specific hardware OpenGL driver
grXWIN              - X11 Window
grMAXDRIVER         - Maximum driver number
****************************************************************************/
typedef enum {
    grDETECT        = -1,
    grNONE          = 0,
    grVGA,
    grFULLDIB,
    grVESA,
    grSVGA,
    grACCEL,
    grDDRAW,
    grDDRAWACCEL,
    grDDRAW3D,
    grOPENGL,
    grOPENGL_MGL_MINI,
    grOPENGL_MGL,
    grXWIN,
    grMAXDRIVER,
    } MGL_driverType;

/****************************************************************************
REMARKS:
Defines the supported graphics modes. This is the value passed in the mode
parameter to MGL_init and MGL_changeDisplayMode. Note that although we
define symbolic constants for all the available graphics modes supported
by this version of the library, for maximum compatibility with future
versions of the library (which may define new modes as they become
available), the best way to search for a supported mode is to search the
list of modes returned by MGL_availableModes, and look for one that has
the desired color depth and resolution by calling MGL_modeResolution. Check
out how this is done in the SciTech Game Framework source code for more
information.

Note: The only video modes supported by this graphics library are those
that support at least 16 colors per pixel.
****************************************************************************/
typedef enum {
    /* 16 color VGA video modes */
    grVGA_320x200x16,
    grVGA_640x200x16,
    grVGA_640x350x16,
    grVGA_640x400x16,
    grVGA_640x480x16,
    grSVGA_800x600x16,

    /* 256 color VGA video modes */
    grVGA_320x200x256,
    grVGA_224x288x256,
    grVGA_256x256x256,
    grVGA_288x224x256,

    /* 256 color VGA ModeX video modes */
    grVGAX_320x200x256,
    grVGAX_320x240x256,
    grVGAX_224x288x256,
    grVGAX_256x256x256,
    grVGAX_288x224x256,
    grVGAX_320x400x256,
    grVGAX_320x480x256,

    /* 256 color VGA/SuperVGA video modes */
    grSVGA_320x200x256,
    grSVGA_320x240x256,
    grSVGA_320x400x256,
    grSVGA_320x480x256,
    grSVGA_400x300x256,
    grSVGA_512x384x256,
    grSVGA_640x350x256,
    grSVGA_640x400x256,
    grSVGA_640x480x256,
    grSVGA_800x600x256,
    grSVGA_1024x768x256,
    grSVGA_1152x864x256,
    grSVGA_1280x960x256,
    grSVGA_1280x1024x256,
    grSVGA_1600x1200x256,

    /* 32,768 color Super VGA video modes */
    grSVGA_320x200x32k,
    grSVGA_320x240x32k,
    grSVGA_320x400x32k,
    grSVGA_320x480x32k,
    grSVGA_400x300x32k,
    grSVGA_512x384x32k,
    grSVGA_640x350x32k,
    grSVGA_640x400x32k,
    grSVGA_640x480x32k,
    grSVGA_800x600x32k,
    grSVGA_1024x768x32k,
    grSVGA_1152x864x32k,
    grSVGA_1280x960x32k,
    grSVGA_1280x1024x32k,
    grSVGA_1600x1200x32k,

    /* 65,536 color Super VGA video modes */
    grSVGA_320x200x64k,
    grSVGA_320x240x64k,
    grSVGA_320x400x64k,
    grSVGA_320x480x64k,
    grSVGA_400x300x64k,
    grSVGA_512x384x64k,
    grSVGA_640x350x64k,
    grSVGA_640x400x64k,
    grSVGA_640x480x64k,
    grSVGA_800x600x64k,
    grSVGA_1024x768x64k,
    grSVGA_1152x864x64k,
    grSVGA_1280x960x64k,
    grSVGA_1280x1024x64k,
    grSVGA_1600x1200x64k,

    /* 16 million color, 24 bits per pixel Super VGA video modes */
    grSVGA_320x200x16m,
    grSVGA_320x240x16m,
    grSVGA_320x400x16m,
    grSVGA_320x480x16m,
    grSVGA_400x300x16m,
    grSVGA_512x384x16m,
    grSVGA_640x350x16m,
    grSVGA_640x400x16m,
    grSVGA_640x480x16m,
    grSVGA_800x600x16m,
    grSVGA_1024x768x16m,
    grSVGA_1152x864x16m,
    grSVGA_1280x960x16m,
    grSVGA_1280x1024x16m,
    grSVGA_1600x1200x16m,

    /* 16 million color, 32 bits per pixel Super VGA video modes */
    grSVGA_320x200x4G,
    grSVGA_320x240x4G,
    grSVGA_320x400x4G,
    grSVGA_320x480x4G,
    grSVGA_400x300x4G,
    grSVGA_512x384x4G,
    grSVGA_640x350x4G,
    grSVGA_640x400x4G,
    grSVGA_640x480x4G,
    grSVGA_800x600x4G,
    grSVGA_1024x768x4G,
    grSVGA_1152x864x4G,
    grSVGA_1280x960x4G,
    grSVGA_1280x1024x4G,
    grSVGA_1600x1200x4G,

    /* Render into Windowing System DC (Windows, OS/2 PM, X11) */
    grWINDOWED,

    /* Maximum mode number                  */
    grMAXMODE,
    } MGL_modeType;

/****************************************************************************
REMARKS:
Defines the marker types passed to MGL_setMarkerStyle

HEADER:
mgraph.h

MEMBERS:
MGL_MARKER_SQUARE   - Draw a solid square at the marker location
MGL_MARKER_CIRCLE   - Draw a solid circle at the marker location
MGL_MARKER_X        - Draw a cross (X) at the marker location
****************************************************************************/
typedef enum {
    MGL_MARKER_SQUARE,
    MGL_MARKER_CIRCLE,
    MGL_MARKER_X,
    } MGL_markerStyleType;

/****************************************************************************
REMARKS:
Defines the border drawing styles passed to MGL_drawBorderCoord.

HEADER:
mgraph.h

MEMBERS:
MGL_BDR_INSET   - Interior is inset into screen
MGL_BDR_OUTSET  - Interior is outset from screen
MGL_BDR_OUTLINE - Border is 3d outline
****************************************************************************/
typedef enum {
    MGL_BDR_INSET,
    MGL_BDR_OUTSET,
    MGL_BDR_OUTLINE,
    } MGL_bdrStyleType;

/* Obsolete driver names */

#define MGL_SVGA4NAME       "svga4.drv"     /* Generic SuperVGA drivers     */
#define MGL_SVGA8NAME       "svga8.drv"
#define MGL_SVGA16NAME      "svga16.drv"
#define MGL_SVGA24NAME      "svga24.drv"
#define MGL_SVGA32NAME      "svga32.drv"

#define MGL_PACKED1NAME     "pack1.drv"
#define MGL_PACKED4NAME     "pack4.drv"
#define MGL_PACKED8NAME     "pack8.drv"
#define MGL_PACKED16NAME    "pack16.drv"
#define MGL_PACKED24NAME    "pack24.drv"
#define MGL_PACKED32NAME    "pack32.drv"

/* The following dummy symbols are used to link in driver files to be used. A
 * driver is not active until it is linked in with the MGL_registerDriver
 * call. Because we dont export globals in DLLs, we provide functions to
 * get the address of the drivers. However for a static link library we
 * need to use globals so that if the driver data is unreferenced, it will
 * not be linked in with the code.
 */

#define VGA4_driver         NULL
#define VGAX_driver         NULL
#define SVGA4_driver        NULL
#define SVGA8_driver        NULL
#define SVGA16_driver       NULL
#define SVGA24_driver       NULL
#define SVGA32_driver       NULL
#define VGA8_driver         NULL
#define LINEAR8_driver      NULL
#define LINEAR16_driver     NULL
#define LINEAR24_driver     NULL
#define LINEAR32_driver     NULL
#define ACCEL8_driver       NULL
#define ACCEL16_driver      NULL
#define ACCEL24_driver      NULL
#define ACCEL32_driver      NULL
#define FULLDIB8_driver     NULL
#define FULLDIB16_driver    NULL
#define FULLDIB24_driver    NULL
#define FULLDIB32_driver    NULL
#define DDRAW8_driver       NULL
#define DDRAW16_driver      NULL
#define DDRAW24_driver      NULL
#define DDRAW32_driver      NULL
#define OPENGL_driver       NULL
#define FSOGL8_driver       NULL
#define FSOGL16_driver      NULL
#define FSOGL24_driver      NULL
#define FSOGL32_driver      NULL
#define PACKED1_driver      NULL
#define PACKED4_driver      NULL
#define PACKED8_driver      NULL
#define PACKED16_driver     NULL
#define PACKED24_driver     NULL
#define PACKED32_driver     NULL

#pragma pack()

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Environment detection and initialisation */

int     MGLAPI MGL40_registerDriver(const char *name,void *driver);
void    MGLAPI MGL40_registerAllDispDrivers(ibool useLinear,ibool useDirectDraw,ibool useWinDirect);
void    MGLAPI MGL40_registerAllDispDriversExt(ibool useWinDirect,ibool useDirectDraw,ibool useVGA,ibool useVGAX,ibool useVBE,ibool useLinear,ibool useVBEAF,ibool useFullscreenDIB);
void    MGLAPI MGL40_registerAllMemDrivers(void);
void    MGLAPI MGL40_detectGraph(int *driver,int *mode);
uchar * MGLAPI MGL40_availableModes(void);
int     MGLAPI MGL40_availablePages(int mode);
int     MGLAPI MGL40_modeResolution(int mode,int *xRes,int *yRes,int *bitsPerPixel);
ulong   MGLAPI MGL40_modeFlags(int mode);
ibool   MGLAPI MGL40_initWindowed(const char *mglpath);
const char * MGLAPI MGL40_modeName(int mode);
const char * MGLAPI MGL40_modeDriverName(int mode);
const char * MGLAPI MGL40_driverName(int driver);
int     MGLAPI MGL40_getDriver(MGLDC *dc);
int     MGLAPI MGL40_getMode(MGLDC *dc);

/* Device context management */

ibool   MGLAPI MGL40_changeDisplayMode(int mode);
MGLDC   * MGLAPI MGL40_createDisplayDC(int numBuffers);
MGLDC   * MGLAPI MGL40_createScrollingDC(int virtualX,int virtualY,int numBuffers);
MGLDC   * MGLAPI MGL40_createOffscreenDC(void);
MGLDC   * MGLAPI MGL40_createLinearOffscreenDC(void);

#ifndef MGL_LITE
void    MGLAPI MGL40_setPenBitmapPattern(const pattern_t *pat);
void    MGLAPI MGL40_getPenBitmapPattern(pattern_t *pat);
void    MGLAPI MGL40_setPenPixmapPattern(const pixpattern_t *pat);
void    MGLAPI MGL40_getPenPixmapPattern(pixpattern_t *pat);
#endif

/* Obsolete psuedo 3D border drawing */

#ifndef MGL_LITE
void    MGLAPI MGL40_setBorderColors(color_t bright,color_t dark);
void    MGLAPI MGL40_getBorderColors(color_t *bright,color_t *dark);
void    MGLAPI MGL40_drawBorderCoord(int left,int top,int right,int bottom,int style,int thickness);
void    MGLAPI MGL40_drawHDivider(int y,int x1,int x2);
void    MGLAPI MGL40_drawVDivider(int x,int y1,int y2);
#endif

/* Obsolete marker functions */

#ifndef MGL_LITE
void    MGLAPI MGL40_setMarkerSize(int size);
int     MGLAPI MGL40_getMarkerSize(void);
void    MGLAPI MGL40_setMarkerStyle(int style);
int     MGLAPI MGL40_getMarkerStyle(void);
void    MGLAPI MGL40_setMarkerColor(color_t color);
color_t MGLAPI MGL40_getMarkerColor(void);
void    MGLAPI MGL40_marker(point_t p);
void    MGLAPI MGL40_polyMarker(int count,point_t *vArray);
#endif

/* Macros to enable the MGL 4.0 API compatibility functions for legacy
 * applications that have not yet moved to the new MGL 5.0 API.
 */

#ifdef  MGL40_COMPAT
#define MGL_createDisplayDC(n)          MGL_createDisplayDC(n)
#define MGL_createScrollingDC(x,y,n)    MGL_createScrollingDC(x,y,n)
#define MGL_drawBorder(r,s,t)           MGL40_drawBorderCoord((r).left,(r).top,(r).right,(r).bottom,(s),(t))
#endif

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif  /* __cplusplus */

#endif  /* __MGL40_H */
