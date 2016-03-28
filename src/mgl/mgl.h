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
*   Software Foundation and appearing in the file LICENSE.GPL ld
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
* Description:  Header file for the MegaGraph graphics library, containing
*               the declarations for internal stuctures used by the library.
*
*               You define one of the following #defines to build the
*               library for each different platform:
*
*                   MGLDOS  - Build for MSDOS
*                   MGLWIN  - Build for Windows with WinG and DISPDIB
*                   MGLPM   - Build for OS/2 PM with DIVE and fullscreen
*                   MGLX    - Build for X11
*
****************************************************************************/

#ifndef __MGL_H
#define __MGL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <setjmp.h>
#include "mgl_int.h"
#include "mgl/list.h"
#include "resource.h"
#include "halftone.h"
#include "snap/graphics.h"
#include "pmapi.h"
#include "clib/os/os.h"
#include "freetype.h"
#include "ftxkern.h"

/*---------------------- Macro and type definitions -----------------------*/

#ifdef __ALPHA__
#define NO_ASSEMBLER        /* Compile as C only for DEC Alpha          */
#endif

#ifdef  __INTEL__
#pragma pack(1)             /* Pack structures to byte granularity      */
#endif

#if defined(__INTEL__) && defined(__WINDOWS__)
#define GET_DEFAULT_CW()                    \
{                                           \
    if (_MGL_cw_default == 0)               \
        _MGL_cw_default = _control87(0,0);  \
}
#define RESET_DEFAULT_CW()                  \
    _control87(_MGL_cw_default,0xFFFFFFFF)
#else
#define GET_DEFAULT_CW()
#define RESET_DEFAULT_CW()
#endif

/* Define the default size of the MGL scratch buffer    */

#define DEF_MGL_BUFSIZE 32*1024

/* Flags to ignore VBE drivers to speed up detection process */

#define grIGNORE_VBE    0xFD
#define grIGNORE_VBE20  0xFE

/* Name of environment variable pointing to MGL root directory  */

#define MGL_ROOT    "MGL_ROOT"

/* Names of MGL data file directories, located below this root  */

#ifdef  __UNIX__
#define MGL_DRIVERS  "drivers/"
#define MGL_FONTS    "fonts/"
#define MGL_BITMAPS  "bitmaps/"
#define MGL_ICONS    "icons/"
#define MGL_CURSORS  "cursors/"
#else
#define MGL_DRIVERS  "drivers\\"
#define MGL_FONTS    "fonts\\"
#define MGL_BITMAPS  "bitmaps\\"
#define MGL_ICONS    "icons\\"
#define MGL_CURSORS  "cursors\\"
#endif

/* The following defines the version number of the device driver files
 * that this version of the MGL must be used with. Older driver files
 * will not be compatible with this version of the MGL and an error will
 * be returned if an attempt is made to use them.
 */

#define DRIVER_MAJOR_VERSION    1
#define DRIVER_MINOR_VERSION    0
#define DRIVER_VERSION_STR      "1.0"

#define VBE_VIRTUAL_LINEAR      0x0800  /* Virtual linear framebuffer   */

/* Macro to normalise a huge pointer */

#define NORMALISE_PTR(p)

/* Maximum number of modes supported in the library */

#define MAX_DRIVER_MODES    200

/* Table of available modes, driver to use and mode information */

typedef struct {
    uchar   driver;             /* Id of driver supporting this mode    */
    short   xRes;               /* X resolution for display mode        */
    short   yRes;               /* Y resolution for display mode        */
    uchar   bits;               /* Color depth for display mode         */
    short   pages;              /* Number of available hardware pages   */
    ulong   flags;              /* Mode flags for mode                  */
    } modeent;

typedef modeent modetab[MAX_DRIVER_MODES];

/* Device driver header structure */

typedef struct _drivertype {
    char        name[40];       /* Driver's filename                    */
    char        realname[60];   /* Driver's real name to display        */
    char        copyright[95];  /* Copyright notice etc                 */
    char        majorversion;   /* Major version number                 */
    char        minorversion;   /* Minor version number                 */

    /* Pointers to detection and initialisation routines */
    void *  (MGLAPIP createInstance)(void);
    ibool   (MGLAPIP detect)(void *data,int id,int *numModes,modetab availableModes);
    ibool   (MGLAPIP initDriver)(void *data,MGLDC *dc,modeent *mode,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
    void    (MGLAPIP destroyInstance)(void *data);
    } drivertype;

/* Device driver table entry structure */

typedef struct {
    char        name[40];       /* Name of driver file                  */
    ibool       enabled;        /* True when driver has been enabled    */
    ibool       opengl;         /* True if an OpenGL only driver        */
    MOD_MODULE  *hModBPD;       /* Handle to binary portable DLL        */
    PM_MODULE   hModOS;         /* Handle to OS specific DLL            */
    drivertype  *driver;        /* Address of start of driver           */
    void        *data;          /* Global data for driver instance      */
    } driverent;

/* Define the structure for all display devices enumerated */

#define MAX_DRIVERS 10

typedef struct {
    driverent           driverTable[MAX_DRIVERS];
    int                 numDrivers;     /* Number of registered drivers */
    modetab             availableModes; /* List of available modes      */
    int                 numModes;       /* Number of modes in list      */
    ibool               fullScreen;     /* True if in fullscreen mode   */
    ibool               glLoaded;       /* True if OpenGL is loaded     */
    int                 glDevice;       /* Current OpenGL device index  */
    int                 cntOpenGL;      /* Current OpenGL selection     */
    void                *stateBuf;      /* Console state buffer         */
    void                *gaInfo;        /* Pointer to SNAP info         */
    GA_devCtx           *gaPtr;         /* Pointer to SNAP driver       */
    GA_glCtx            *glPtr;         /* SNAP OpenGL driver context   */
    GA_glInitFuncs      glInit;         /* SNAP OpenGL init functions   */
    REF2D_driver        *ref2d;         /* SNAP reference rasteriser    */
    MOD_MODULE          *hModRef2d;     /* SNAP ref2d module handle     */
    LIST                *dispDCList;    /* List of display contexts     */
    LIST                *offDCList;     /* List of offscreen contexts   */
    LIST                *overDCList;    /* List of overlay contexts     */
    } device_entry;

/* Structures used to scan convert filled primitives */

typedef struct {
    short   left;               /* left coordinate of scan line         */
    short   right;              /* right coordinate of scan line        */
    } scanline;

typedef struct {
    int     length;         /* Number of scan lines in list         */
    int     top;            /* Y coordinate of top scan line        */
    scanline    *scans;         /* Pointer to list of scan lines        */
    } scanlist;

typedef struct {
    short   leftL;              /* left coordinate of left scan line    */
    short   leftR;              /* right coordinate of left scan line   */
    short   rightL;             /* left coordinate of right scan line   */
    short   rightR;             /* right coordinate of right scan line  */
    } scanline2;

typedef struct {
    int     length;         /* Number of scan lines in list         */
    int     top;            /* Y coordinate of top scan line        */
    scanline2   *scans;         /* Pointer to list of scan lines        */
    } scanlist2;

/* Header used for divots. The scratch values are driver dependant.     */

typedef struct {
    int     left,top,right,bottom;
    short   bytesPerLine;
    } divotHeader;

#define SENTINEL    32767       /* Very large sentinel value            */

/* Internal structures for region manipulation */

#define DEF_NUM_SEGMENTS    500
#define DEF_NUM_SPANS       500
#define DEF_NUM_REGIONS     100
#define FREELIST_NEXT(p)    (*(void**)(p))

typedef struct block_t {
    struct block_t  *next;      /* Pointer to next block in list        */
    } block_t;

typedef struct {
    block_t *curBlock;          /* List of memory pool blocks           */
    int blockCount;             /* Number of pool blocks allocated      */
    int max;                    /* Maximum objects in block             */
    int objectSize;             /* Size of objects in block             */
    } blockList_t;

typedef struct {
    blockList_t bl;             /* Memory pool containg objects         */
    segment_t   *freeList;      /* Start of free list for pool          */
    } segmentList_t;

typedef struct {
    blockList_t bl;             /* Memory pool containg objects         */
    span_t      *freeList;      /* Start of free list for pool          */
    } spanList_t;

typedef struct {
    blockList_t bl;             /* Memory pool containg objects         */
    region_t    *freeList;      /* Start of free list for pool          */
    } regionList_t;

/* Bitmap Font library structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];   /* Face name for font lib       */
    short           fontLibType;            /* Type (MGL_BITMAPFONT_LIB)    */
    FILE            *f;                     /* File handle for font lib     */
    ibool           ownHandle;              /* TRUE if opened by us         */
    M_uint32        dwOffset;               /* Offset within file           */
    char            numFonts;               /* Number of fonts in lib       */
    char            *fontSizes;             /* Font sizes in lib            */
    M_uint32        *offset;                /* Offsets of fonts in library  */
    M_uint32        *length;                /* Length of font in library    */

    /* ... fontSizes[..] array  */
    /* ... offset[..] array     */
    /* ... length[..] array     */
    } bitmap_font_lib;

/* TrueType Font library structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];
    short           fontLibType;
    FILE            *f;                     /* File handle for font lib     */
    ibool           ownHandle;              /* TRUE if opened by us         */
    M_uint32        dwOffset;               /* Offset within file           */
    TT_Face         face;                   /* TrueType face handle         */
    TT_CharMap      charMap;                /* Character map                */
    char            numFonts;               /* Number of fonts in lib       */
    int             numGlyphs;              /* Number of glyphs in font     */
    char            *fontSizes;             /* Font sizes in lib            */
    M_uint32        *offset;                /* Offsets of fonts in library  */
    M_uint32        *length;                /* Length of font in library    */

    /* ... fontSizes[..] array  */
    /* ... offset[..] array     */
    /* ... length[..] array     */
    } tt_font_lib;

/* Type 1 Font library structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];
    short           fontLibType;
    char            tobedefined;
    } t1_font_lib;

/* Macro to convert to a bitmap font library */

#define BITFONTLIB(lib) ((bitmap_font_lib *)lib)

/* Macro to convert to a TrueType font library */

#define TTFONTLIB(lib)  ((tt_font_lib *)lib)

/* Macro to convert to a Type 1 font library */

#define T1FONTLIB(lib)  ((t1_font_lib *)lib)

/* Internal MGL bitmap font structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];/* Name of the font            */
    short           fontType;       /* Type of font                     */
    short           maxWidth;       /* Maximum character width          */
    short           maxKern;        /* Maximum character kern           */
    short           fontWidth;      /* Font width                       */
    short           fontHeight;     /* Font height                      */
    short           ascent;         /* Font ascent value                */
    short           descent;        /* Font descent value               */
    short           leading;        /* Font leading value               */
    short           pointSize;      /* Point size for the font          */
    short           startGlyph;     /* The index of the first character */
    short           numGlyphs;      /* The amount of glyphs in this font*/
    short           *width;         /* Character width table            */
    long            *offset;        /* Offset table into character defn.*/
    short           *iwidth;        /* Character image width table      */
    short           *loc;           /* Character location table         */
    short           *charAscent;    /* Character ascent value           */
    short           *charDescent;   /* Character descent value          */
    char            *valid;         /* True if the glyph is valid       */
    uchar           *def;           /* Bitmap definitions for font      */
    bitmap_font_lib *lib;           /* Pointer to the library for font  */
    long            bitmapBufferSize;/* Size of memory buffer for glyphs */
    long            bitmapBufferUsed;/* How much of the bitmap buffer memory is in use ie: last index for insertion */
    ibool           antialiased;     /* Font is antialiased             */
    TT_Instance     instance;        /* TrueType font instance handle  */
    } bitmap_font;

/* Define the overhead per character for the font index tables */

#define FONT_INDEX_SIZE (5 * sizeof(short) + sizeof(long) + sizeof(char))

/* Missing symbol for bitmap fonts is index numGlyphs-1 */

#define BIT_MISSINGSYMBOL(f) (BITFONT(f)->numGlyphs-1)

/* Macro to convert to a vector font pointer */

#define VECFONT(fnt)    ((vec_font *)fnt)

/* Macro to convert to a bitmap font pointer */

#define BITFONT(fnt)    ((bitmap_font *)fnt)

/* Macros to test for the missing symbol */

#define VEC_ISMISSING(fnt,ch)   (VECFONT(fnt)->offset[(uint)ch] == -1)
#define BIT_ISMISSING(fnt,ch)   (BITFONT(fnt)->offset[(uint)ch] == -1)

/* Macros to swap two values */

#define SWAP(a,b)   { a^=b; b^=a; a^=b; }

#define SWAPT(v1,v2,t) {    \
    t = v1;                 \
    v1 = v2;                \
    v2 = t;                 \
    }

/* Device driver type id's */

#define MGL_DISPLAY_DEVICE          0
#define MGL_OPENGL_DEVICE           2
#define MGL_FS_OPENGL_DEVICE        3
#define MGL_OFFSCREEN_DEVICE        4
#define MGL_OVERLAY_DEVICE          5
#define MGL_WINDOWED_DEVICE         6
#define MGL_MEMORY_DEVICE           7

#define _MGL_isFullscreenDevice(dc)                     \
    (dc->deviceType == MGL_DISPLAY_DEVICE ||            \
     dc->deviceType == MGL_OPENGL_DEVICE ||             \
     dc->deviceType == MGL_FS_OPENGL_DEVICE)

#define _MGL_isOffscreenDevice(dc)                      \
    (dc->deviceType == MGL_OFFSCREEN_DEVICE)

#define _MGL_isOverlayDevice(dc)                        \
    (dc->deviceType == MGL_OVERLAY_DEVICE)

#define _MGL_isOpenGLDevice(dc)                         \
    (dc->deviceType == MGL_WINDOWED_DEVICE ||           \
     dc->deviceType == MGL_OPENGL_DEVICE ||             \
     dc->deviceType == MGL_FS_OPENGL_DEVICE)

/* Function pointer prototypes for rendering code */
typedef void (*scanLineFunc)(int y,int x1,int x2);
typedef void (MGLAPIP pixelFunc)(N_int32 x,N_int32 y);
typedef void (MGLAPIP scanListFunc)(N_int32 y,N_int32 length,N_int16 *scans);
typedef void (MGLAPIP ellipseListFunc)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
typedef void (MGLAPIP fatEllipseListFunc)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
typedef void (MGLAPIP rectFunc)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
typedef void (MGLAPIP rectLinFunc)(N_int32 dstOfs,N_int32 dstPitch,N_int32 left,N_int32 top,N_int32 width,N_int32 height);
typedef void (MGLAPIP trapFunc)(GA_trap *trap);
typedef void (MGLAPIP lineIntFunc)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast);
typedef void (MGLAPIP bresenhamLineFunc)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 absDeltaX,N_int32 absDeltaY,N_int32 majorInc,N_int32 diagInc,N_int32 initialError,N_int32 flags);
typedef void (MGLAPIP regionFunc)(int x,int y,const region_t *r);
typedef void (MGLAPIP ellipseFunc)(int left,int top,int A,int B,int clip);
typedef void (MGLAPIP fillEllipseFunc)(int left,int top,int A,int B,int clip);
typedef void (MGLAPIP ellipseArcFunc)(int left,int top,int A,int B,int startangle,int endangle,int clip);
typedef void (MGLAPIP fillEllipseArcFunc)(int left,int top,int A,int B,int startangle,int endangle,int clip);
typedef void (*clippedLineIntFunc)(int x1,int y1,int x2,int y2,ibool drawLast,int clipLeft,int clipTop,int clipRight,int clipBottom);
typedef void (*clippedLineFXFunc)(fix32_t x1,fix32_t y1,fix32_t x2,fix32_t y2,int clipLeft,int clipTop,int clipRight,int clipBottom);

/* Macro to compute the address of a pixel with a device surface. This
 * works for all packed pixel modes (the only ones that can be directly
 * accessed through the device context surface anyway).
 */

#define X_ADDR(x,p)             ((x) * ((p+7)/8))
#define Y_ADDR(y,s,b)           ((uchar*)(s) + ((y) * (b)))
#define PIXEL_ADDR(x,y,s,b,p)   (Y_ADDR(y,s,b) + X_ADDR(x,p))
#define BYTESPERLINE(w,p)       X_ADDR(w,p)

/* Macros to halfTone dither an RGB pixel */

#define MGL_halfTonePixelFast(p,x,y,R,G,B)                      \
{                                                               \
    uchar dither = _MGL_dither8x8[((x & 7) << 3) + (y & 7)];    \
    (p) = 20 +                                                  \
        _MGL_div51[R] + (_MGL_mod51[R] > dither) +              \
        _MGL_mul6[_MGL_div51[G] + (_MGL_mod51[G] > dither)] +   \
        _MGL_mul36[_MGL_div51[B] + (_MGL_mod51[B] > dither)];   \
}

#define MGL_halfTonePixelFast2(p,x,y,c)                         \
{                                                               \
    uchar dither = _MGL_dither8x8[((x & 7) << 3) + (y & 7)];    \
    (p) = 20 +                                                  \
        _MGL_div51[MGL_rgbRed(c)] +                             \
        (_MGL_mod51[MGL_rgbRed(c)] > dither) +                  \
        _MGL_mul6[_MGL_div51[MGL_rgbGreen(c)] +                 \
        (_MGL_mod51[MGL_rgbGreen(c)] > dither)] +               \
        _MGL_mul36[_MGL_div51[MGL_rgbBlue(c)] +                 \
        (_MGL_mod51[MGL_rgbBlue(c)] > dither)];                 \
}

#define MGL_halfTonePixelSetup(y,c,dither,Rdiv51,Rmod51,        \
    Gdiv51,Gmod51,Bdiv51,Bmod51)                                \
{                                                               \
    dither = &_MGL_dither8x8[(y & 7)];                          \
    Rdiv51 = _MGL_div51[MGL_rgbRed(c)];                         \
    Rmod51 = _MGL_mod51[MGL_rgbRed(c)];                         \
    Gdiv51 = _MGL_div51[MGL_rgbGreen(c)];                       \
    Gmod51 = _MGL_mod51[MGL_rgbGreen(c)];                       \
    Bdiv51 = _MGL_div51[MGL_rgbBlue(c)];                        \
    Bmod51 = _MGL_mod51[MGL_rgbBlue(c)];                        \
}

#define MGL_halfTonePixelFast3(p,x,dp,Rdiv51,Rmod51,            \
    Gdiv51,Gmod51,Bdiv51,Bmod51)                                \
{                                                               \
    uchar dither = dp[((x & 7) << 3)];                          \
    (p) = 20 +                                                  \
        Rdiv51 + (Rmod51 > dither) +                            \
        _MGL_mul6[Gdiv51 + (Gmod51 > dither)] +                 \
        _MGL_mul36[Bdiv51 + (Bmod51 > dither)];                 \
}

/* Macro to check for an identity palette */

#define ID_PAL1(m1,p1,p2)   (memcmp(p1,p2,m1 * sizeof(color_t)) == 0)

#define ID_PAL(m1,p1,m2,p2) ID_PAL1(MIN((int)m1,(int)m2),p1,p2)

#define ID_PAL_DC(src,dst)                                              \
    ID_PAL((src)->mi.maxColor+1,(src)->colorTab,                        \
           (dst)->mi.maxColor+1,(dst)->colorTab)

/* Macro to compare two pixel formats structures */

#define CMP_PF(pf1,pf2)                                                 \
   (((pf1).redPos == (pf2).redPos) &&                                   \
    ((pf1).greenPos == (pf2).greenPos) &&                               \
    ((pf1).bluePos == (pf2).bluePos) &&                                 \
    ((pf1).alphaPos == (pf2).alphaPos))

/* Macros to check to see if the bitmap needs to be translated, which
 * is required if the pixel depths are different, the pixel format is
 * different or we dont have an identity palette mapping in color
 * index modes.
 */

#define NEED_TRANSLATE(sBits,sPal,sPF,sMax,dBits,dCI,dPal,dPF,dMax)     \
   (((sBits) != (dBits))                                                \
    ||                                                                  \
    (_MGL_checkIdentityPal && (sPal) && (dCI) &&                        \
        !ID_PAL((sMax),(sPal),(dMax),(dPal)))                           \
    ||                                                                  \
    ((dBits) > 8 && !CMP_PF(dPF,sPF)))

#define NEED_TRANSLATE_DC(src,dst)                                      \
    NEED_TRANSLATE((src)->mi.bitsPerPixel,                              \
        (src)->colorTab,(src)->pf,                                      \
        (src)->mi.maxColor+1,                                           \
        (dst)->mi.bitsPerPixel,                                         \
        (dst)->mi.modeFlags & MGL_IS_COLOR_INDEX,                       \
        (dst)->colorTab,(dst)->pf,                                      \
        (dst)->mi.maxColor+1)

#define NEED_TRANSLATE_BM(bm,dst)                                       \
    NEED_TRANSLATE((bm)->bitsPerPixel,(bm)->pal,*((bm)->pf),            \
        (((bm)->bitsPerPixel == 4) ? 16 : 256),                         \
        (dst)->mi.bitsPerPixel,                                         \
        (dst)->mi.modeFlags & MGL_IS_COLOR_INDEX,                       \
        (dst)->colorTab,(dst)->pf,                                      \
        (dst)->mi.maxColor+1)

/* Macro to setup the internal dithering for bitmap operations as follows:
 *
 * 1. If we are translating from color index source bitmaps to
 *    RGB destination bitmaps and the identity palette checking
 *    is turned off, we want to use the pre-computed palette
 *    translation table stored in the destination palette for the
 *    device context for speed. This allows the programmer to
 *    set the palette for the destination context once for a
 *    number of conversion blit operations to avoid the overhead
 *    of computing the palette translation table for every blit.
 *    Setting the ditherMode value passed to ConvertBltSys is
 *    how we signal to the SciTech SNAP Graphics drivers to use our
 *    passed in translation palette.
 *
 * 2. If we are translating from RGB source bitmaps to color index
 *    destination bitmaps and the identity palette checking
 *    is turned off, we assume that the programmer has loaded
 *    the MGL halfTone palette into the destination device context
 *    so we avoid the cost of computing the palette translation
 *    vector for every blit.
 */

#define SETUP_DITHER_MODE(dc,colorIndex,haveSrcPal)                         \
        if (colorIndex) {                                                   \
            if (!_MGL_checkIdentityPal || !haveSrcPal) {                    \
                if (!(dc->mi.modeFlags & MGL_IS_COLOR_INDEX))               \
                    dstPal = GAPAL(dc->colorTab);                           \
                else                                                        \
                    dstPal = GAPAL(_MGL_linearTranslate);                   \
                ditherMode = -1;                                            \
                }                                                           \
            else {                                                          \
                dstPal = GAPAL(dc->colorTab);                               \
                ditherMode = dc->a.ditherMode;                              \
                }                                                           \
            }                                                               \
        else {                                                              \
            if ((dc->mi.modeFlags & MGL_IS_COLOR_INDEX) && !_MGL_checkIdentityPal) { \
                dstPal = GAPAL(_MGL_halfToneTranslate);                     \
                ditherMode = -1;                                            \
                }                                                           \
            else {                                                          \
                dstPal = GAPAL(dc->colorTab);                               \
                ditherMode = dc->a.ditherMode;                              \
                }                                                           \
            }

/* Macro to determine if a bitmap is a color index bitmap. We have to do this
 * because we don't have a flag we can store in the bitmap to save this easily.
 */

#define IS_COLOR_INDEX_BMP(bmp)  \
    (((bmp)->bitsPerPixel <= 8) || ((bmp)->bitsPerPixel == 16 && (bmp)->pf->alphaPos == 8))

/* Macro to convert color table to a palette pointer */

#define TO_PAL(c)   ((palette_t*)(c))

/* Macro to convert MGLBUF to a GA_buf. This is a gross hack but it
 * cleans up the code so long as we keep the structures in sync. One
 * day SciTech SNAP Graphics will be a core part of the MGL API so this will not
 * be necessary.
 */

#define TO_BUF(b)   ((GA_buf*)(b))

/* Macro to build a translation vector between two palettes, mapping
 * colors from one palette to the closest colors in the other palette.
 */

#define _MGL_buildTranslateVector(t,sSize,sPal,dSize,dPal)              \
{                                                                       \
    int         i;                                                      \
    GA_color    *p = t;                                                 \
    palette_t   *sp = sPal;                                             \
                                                                        \
    for (i = 0; i < sSize; i++,sp++) {                                  \
        int         j,closest = 0,d,distance = 0x7FFFL;                 \
        palette_t   *dp = dPal;                                         \
                                                                        \
        for (j = 0; j < dSize; j++,dp++) {                              \
            if ((d = ABS((int)sp->red - (int)dp->red)                   \
                   + ABS((int)sp->green - (int)dp->green)               \
                   + ABS((int)sp->blue - (int)dp->blue)) < distance) {  \
                distance = d;                                           \
                closest = j;                                            \
                }                                                       \
            }                                                           \
        *p++ = closest;                                                 \
        }                                                               \
}

/* Macros to index into polygon vertex and color arrays */

#define VTX(vArray,vinc,i)  ((fxpoint_t*)((char*)(vArray) + (vinc)*(i)))
#define INCVTX(vArray,vinc) *((char**)&(vArray)) += (vinc)
#define VTX3(vArray,vinc,i) ((fxpoint3d_t*)((char*)(vArray) + (vinc)*(i)))
#define INCVTX3(vArray,vinc) *((char**)&(vArray)) += (vinc)
#define CLR(cArray,cinc,i)  ((color_t*)((char*)(cArray) + (cinc)*(i)))
#define INCCLR(cArray,cinc) *((char**)&(cArray)) += (cinc)

/* Macro to increment any pointer by a number of bytes */

#define BINC(v,inc) *((char**)&(v)) += (inc)

/* Define the indexes for the pre-defined pixel format types */

typedef enum {
    pfCI8,              /* 8 bits per pixel color index                 */
    pfCI8_A,            /* 16 bits per pixel 8-bit CI with alpha        */
    pfRGB555,           /* 15 bits per pixel 5:5:5 format               */
    pfRGB565,           /* 16 bits per pixel 5:6:5 format               */
    pfRGB24,            /* 24 bits per pixel RGB format                 */
    pfBGR24,            /* 24 bits per pixel BGR format                 */
    pfARGB32,           /* 32 bits per pixel ARGB format                */
    pfABGR32,           /* 32 bits per pixel ABGR format                */
    pfRGBA32,           /* 32 bits per pixel RGBA format                */
    pfBGRA32,           /* 32 bits per pixel BGRA format                */
    } _MGL_pixelFormatIdx;

/* Macro to access the currently active device table */

#define GET_CURRENT_DEVICE()        (_MGL_cntDevice & ~MM_MODE_MIXED)
#define DEV                         (_MGL_deviceTable[_MGL_cntDevice & ~MM_MODE_MIXED])
#define DEV0                        (_MGL_deviceTable[0])
#define DC_DEV(dc)                  (_MGL_deviceTable[dc->v->d.device])

/* Macros to make a device the active device and then to restore the
 * old active device. We also grab the SNAP semaphore when we do this
 * to make sure any OS code such as the PM desktop are blocked for
 * the duration of this operation.
 */

#define MAKE_ACTIVE_DEVICE(dc)                                              \
    if ((dc)->v->d.setActiveDevice) {                                       \
        int     oldDevice;                                                  \
                                                                            \
        if (((dc)->v->d.lockCount)++ == 0) {                                \
            PM_lockSNAPAccess(0,true);                                      \
            oldDevice = (dc)->v->d.getActiveDevice();                       \
            if ((oldDevice & ~MM_MODE_MIXED) != (dc)->v->d.device) {        \
                (dc)->v->d.setActiveDevice((dc)->v->d.device);              \
                (dc)->v->d.oldDevice = oldDevice;                           \
                }                                                           \
            }                                                               \
        }

#define RESTORE_ACTIVE_DEVICE(dc)                                           \
    if (dc->v->d.setActiveDevice && --((dc)->v->d.lockCount) == 0) {        \
        if (((dc)->v->d.oldDevice & ~MM_MODE_MIXED) != (dc)->v->d.device)   \
            (dc)->v->d.setActiveDevice((dc)->v->d.oldDevice);               \
        PM_unlockSNAPAccess(0);                                             \
        }

/* Macro to begin using the visible clip list as necessary. This function
 * should be called before *any* rendering is done to ensure that
 * if the visible clip list has changed, that we will pick this up
 * before we draw the item in a window. In non-windowed modes this
 * macro will simply do nothing. Note also that this must be
 * called before any code uses the clip rectangles, as the clip
 * rectangles for the device are changed automatically as needed
 * based on window clipped.
 */

#define BEGIN_VISIBLE_CLIP_LIST(dc)                 \
{                                                   \
    if ((dc)->deviceType == MGL_WINDOWED_DEVICE)    \
        (dc)->r.BeginVisibleClipRegion(dc);         \
}

/* Macro to end the visible clip list processing. Called after
 * rendering to clean up and exit..
 */

#define END_VISIBLE_CLIP_LIST(dc)                   \
{                                                   \
    if ((dc)->deviceType == MGL_WINDOWED_DEVICE)    \
        (dc)->r.EndVisibleClipRegion(dc);           \
}

/* Macros to make the hardware current with a specific DC, and to
 * restore the hardware to the old DC if it was also a display device
 * context. This handles cases where a blit is made from a hardware
 * offscreen DC to the main display DC, but the offscreen DC is the
 * currently active DC so we have to update the hardware for the
 * main display DC (ie: update clip rectangle, write modes, patterns etc).
 *
 * If we change the MGL BitBlt API to only work with the current device
 * context, we can solve this problem and eliminate thise code.
 */

#define MAKE_HARDWARE_CURRENT(dc,partial)                   \
{                                                           \
    if (dc != &DC && dc->currentLock++ == 0)                \
        dc->r.makeCurrent(dc,_MGL_dcPtr,partial);           \
}

#define RESTORE_HARDWARE(dc,partial)                        \
{                                                           \
    if (dc != &DC && dc->currentLock-- == 1 && _MGL_dcPtr)  \
        _MGL_dcPtr->r.makeCurrent(_MGL_dcPtr,dc,partial);   \
}

/* Macro to type cast a pointer to an MGL pixel format strucure to a
 * SciTech SNAP Graphics pixel format structure pointers. The structures
 * are identical, so we simply use a type cast.
 */

#define GAPF(pf)        (GA_pixelFormat*)(pf)

/* Macro to type cast a pointer to an MGL palette entry to a
 * SciTech SNAP Graphics palette entry pointer. The structures are identical,
 * so we simply use a type cast.
 */

#define GAPAL(pal)      (GA_palette*)(pal)

/* Macros to convert to SciTech SNAP Graphics style patterns. MGL patterns are
 * the same structures, so we can just use a cast to convert between them.
 */

#define GAMONOPAT(p)    (GA_pattern*)(p)
#define GACLRPAT(p)     (GA_colorPattern*)(p)

/* Macro to begin traversal of the clip region. This macro requires
 * a parameter to to code which will be filled with each of the clip
 * rectangles in the clip region. Code should go in between this and
 * the corresponding END_CLIP_REGION macro similar to the following:
 *
 *  BEGIN_CLIP_REGION(clip);
 *      if (MGL_sectRect(clip,d,&r))
 *          DC.r.cur.fillRect(r.left,r.top,r.right,r.bottom);
 *  END_CLIP_REGION;
 */

#define BEGIN_CLIP_REGION(clip,rgn)                         \
{                                                           \
    segment_t   *seg;                                       \
    span_t      *s = rgn->spans;                            \
                                                            \
    while (s->next) {                                       \
        clip.top = s->y;                                    \
        clip.bottom = s->next->y;                           \
        for (seg = s->seg; seg; seg = seg->next->next) {    \
            clip.left = seg->x;                             \
            clip.right = seg->next->x

/* Macro to end traversal of the clip region */

#define END_CLIP_REGION()       \
            }                   \
        s = s->next;            \
        }                       \
}

/* Line clipping return codes */

#define CLIP_REJECT     0       /* Line is completely clipped           */
#define CLIP_ACCEPT     1       /* Line is accepted, but is clipped     */
#define CLIP_INSIDE     2       /* Line is complete unclipped           */

/* Macros to destroy device contexts */

#define _MGL_destroyOverlayDC(dc)   __MGL_destroyDC(&DEV.overDCList,dc)
#define _MGL_destroyOffscreenDC(dc) __MGL_destroyDC(&DEV.offDCList,dc)
#define _MGL_destroyMemoryDC(dc)    __MGL_destroyDC(&_MGL_memDCList,dc)

/* Standard packed pixel device driver names */

#define MGL_PACKED8NAME     "packed8.drv"
#define MGL_PACKED16NAME    "packed16.drv"
#define MGL_PACKED24NAME    "packed24.drv"
#define MGL_PACKED32NAME    "packed32.drv"

/* Standard windowed device driver names */

#define MGL_WINDOWEDNAME    "window.drv"

/* Macro to set an error return value. For checked builds this also logs
 * the error as a warning to the log file.
 */

#ifdef  CHECKED
#define SETERROR(err)       _CHK_fail(0,"Warning: '%s', file %s, line %d\n",MGL_errorMsg(__MGL_result = err), __FILE__, __LINE__)
#define FATALERROR(err)     _CHK_fail(1,"Fatal: '%s', file %s, line %d\n",MGL_errorMsg(__MGL_result = err), __FILE__, __LINE__)
#else
#define SETERROR(err)       __MGL_result = err
#define FATALERROR(err)     __MGL_result = err
#endif

/*--------------------------- Global Variables ----------------------------*/

extern LIST             *_MGL_memDCList;
extern LIST             *_MGL_winDCList;
extern uint             _MGL_cw_default;
extern int              __MGL_result;
extern ibool            __MGL_useLinearBlits;
extern char             _MGL_path[PM_MAX_PATH];
extern device_entry     _MGL_deviceTable[MAX_DISPLAY_DEVICES];
extern int              _MGL_cntDevice;
extern int              _MGL_glDevice;
extern int              _MGL_numDevices;
extern MGLDC            *_MGL_dcPtr;
extern void             *_MGL_buf;
extern int              _MGL_bufSize;
extern segmentList_t    _MGL_segList;
extern spanList_t       _MGL_spanList;
extern regionList_t     _MGL_regionList;
extern segment_t        _MGL_tmpX1;
extern segment_t        _MGL_tmpX2;
extern span_t           _MGL_tmpY1;
extern span_t           _MGL_tmpY2;
extern region_t         _MGL_rtmp;
extern region_t         _MGL_rtmp2;
extern pixel_format_t   _MGL_pixelFormats[];
extern ibool            _MGL_checkIdentityPal;
extern GA_color         _MGL_translate[256];
extern GA_color         _MGL_linearTranslate[256];
extern GA_color         _MGL_halfToneTranslate[256];
extern FILE *           (*__MGL_fopen)(const char *filename,const char *mode);
extern int              (*__MGL_fclose)(FILE *f);
extern int              (*__MGL_fseek)(FILE *f,long offset,int whence);
extern long             (*__MGL_ftell)(FILE *f);
extern size_t           (*__MGL_fread)(void *ptr,size_t size,size_t n,FILE *f);
extern size_t           (*__MGL_fwrite)(const void *ptr,size_t size,size_t n,FILE *f);
extern int              _MGL_blueCodeIndex;
extern ibool            _MGL_inSuspendApp;
extern driverent        _MGL_staticDrivers[];
extern ibool            _MGL_fullScreen;
extern ibool            _MGL_isBackground;
extern MGL_HWND         _MGL_hwndFullScreen;
extern ibool            _MGL_consoleSupport;

/* Internal drivers */

extern drivertype       VBE_driver;
extern drivertype       SNAP_driver;
extern drivertype       OPENGL_driver;
extern drivertype       GLDIRECT_driver;
extern drivertype       PACKED1_driver;
extern drivertype       PACKED4_driver;
extern drivertype       PACKED8_driver;
extern drivertype       PACKED16_driver;
extern drivertype       PACKED24_driver;
extern drivertype       PACKED32_driver;
#ifdef  __WINDOWS__
extern drivertype       WINDD_driver;
#endif

/*------------------------- Function Prototypes ---------------------------*/

/* Private platform dependant routines */

int     _MGL_enumerateDevices(void);
void    _MGL_initInternal(void);
int     _MGL_setActiveDevice(int device);
int     _MGL_getActiveDevice(void);
void    _MGL_exitInternal(void);
void    _MGL_disableAutoPlay(void);
void    _MGL_restoreAutoPlay(void);
void    _MGL_initBitmapHandle(MGLDC *dc,MGL_HBITMAP hbm);
long    _MGL_allocateSurface(MGLDC *dc);
void    _PM_freeSurface(MGLDC *dc);
void    MGLAPI PACKED8_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
void    _MGL_hookWindowProc(MGLDC *dc);
void    _MGL_unhookWindowProc(MGLDC *dc);
void    _MGL_unlockStaticPalette(MGLDC *dc);
void    _MGL_lockStaticPalette(MGLDC *dc);

/* Private platform independant routines */

ibool   _MGL_initDC(MGLDC *dc,driverent *driver,modeent *mode,MGL_HWND hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate);
MGLDC * _MGL_createOffscreenDC(ibool linear);
ibool   __MGL_destroyDC(LIST **listHandle,MGLDC *dc);
ibool   _MGL_destroyDisplayDC(MGLDC *dc);
ibool   _MGL_destroyWindowedDC(MGLDC *dc);
driverent *_MGL_findStaticDriver(const char *name);
void    MGLAPI _MGL_setRenderingVectors(void);
long    _MGL_fileSize(FILE *f);
FILE *  _MGL_openFile(const char *dir, const char *name, const char *mode);
void    _MGL_initMalloc(void);
void    _MGL_scratchTooSmall(void);
void    _MGL_updateCurrentDC(MGLDC *dc);
void    _MGL_computePF(MGLDC *dc);
int     _MGL_closestColor(palette_t *color,palette_t *pal,int palSize);
int     MGLAPI _MGL_suspendAppProc(int flags);
ibool   _MGL_glLoadOpenGL(MGLDC *dc);
void    _MGL_glUnloadOpenGL(void);
ibool   _MGL_isOpenGLDC(MGLDC *dc);
ibool   _MGL_isFSOpenGLDC(MGLDC *dc);
void    __MGL_recomputeClipping(MGLDC *dc);
REF2D_driver    *__MGL_getRef2d(void);

/* Private platform independant drawing functions */

void    _MGL_drawScanLine(int y,int x1,int x2);
void    _MGL_clipScanLine(int y,int x1,int x2);
int     _MGL_clipLine(int xmin,int ymin,int xmax,int ymax,int *new_x1,int *new_y1,int *new_x2,int *new_y2,int absDeltaX,int absDeltaY,int *pt1_clipped,int *pt2_clipped,int flags,int bias,int outcode1,int outcode2);
void    _MGL_drawClippedLineInt(int x1,int y1,int x2,int y2,ibool drawLast,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    _MGL_drawClippedLineFX(fix32_t x1,fix32_t y1,fix32_t x2,fix32_t y2,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    _MGL_bresenhamLineEngine(int x1,int y1,int initialError,int majorInc,int diagInc,int count,int flags,void (MGLAPIP plotPoint)(long x,long y));
void    _MGL_drawClippedFatLineInt(int x1,int y1,int x2,int y2,ibool drawLast,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    _MGL_drawClippedFatLineFX(fix32_t x1,fix32_t y1,fix32_t x2,fix32_t y2,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    _MGL_drawClippedScanList(int y,int length,short *scans,int clipLeft,int clipTop,int clipRight,int clipBottom);
void    MGLAPI _MGL_clipTrap(GA_trap *trap);
void    MGLAPI _MGL_clipPixel(N_int32 x,N_int32 y);
void    __MGL_ellipseEngine(int left,int top,int A,int B,void (MGLAPIP setup)(int topY,int botY,int left,int right),void (MGLAPIP set4pixels)(ibool inc_x,ibool inc_y,ibool region1),void (MGLAPIP finished)(void));
void    __MGL_ellipseArcEngine(int left,int top,int A,int B,int startAngle,int endAngle,arc_coords_t *ac,void (MGLAPIP plotPoint)(N_int32 x,N_int32 y));
void    _MGL_rotateFull8x8AClockwise(uchar *dst,int dstStep,uchar *src,int srcStep);
void    _MGL_rotatePartial8x8AClockwise(uchar *dst,int dstStep,uchar *src,int srcStep,int height);
void    _MGL_rotateFull8x8Clockwise(uchar *dst,int dstStep,uchar *src,int srcStep);
void    _MGL_rotatePartial8x8Clockwise(uchar *dst,int dstStep,uchar *src,int srcStep,int height);
void    _MGL_rotateBitmap180(uchar *dst,uchar *src,int byteWidth,int height);
void    __MGL_drawRegion(int x,int y,const region_t *r);
void    _MGL_scanFatEllipse(scanlist2 *scanList,int left,int top,int A,int _B,int _penWidth,int _penHeight);
void    _MGL_scanEllipse(scanlist *scanList,int left,int top, int A, int _B);
void    _MGL_fatPenEllipse(int left,int top,int A,int B,int clip);
void    _MGL_computeCoords(int A,int B,int cx,int cy,int angle,int *rx,int *ry,int *quadrant);
int     _MGL_fixAngle(int angle);
void    _MGL_scanEllipseArc(scanlist2 *sl,arc_coords_t *ac,int left,int top,int A,int _B,int startAngle,int endAngle);
int     _MGL_scanFatLine(scanlist *scanList,int fx1,int fy1,int fx2,int fy2,int _penHeight,int _penWidth);
int     _MGL_scanFatLineFX(scanlist *scanList,fix32_t fx1,fix32_t fy1,fix32_t fx2,fix32_t fy2,int _penHeight,int _penWidth);
void    _MGL_convexPolygon(int count,fxpoint_t *vArray,int vinc,fix32_t xOffset,fix32_t yOffset,trapFunc trap);
void    _MGL_complexPolygon(int count,fxpoint_t *vArray,int vinc,fix32_t xOffset,fix32_t yOffset);
int     MGLAPI _MGL_computeSlope(fxpoint_t *v1,fxpoint_t *v2,fix32_t *slope);
fxpoint_t *_MGL_copyToFixed(int count,point_t *vArray);

/* Text helper functions */

int     __MGL_charWidth(wchar_t ch);
void    __MGL_findTextBounds(int x,int y,int width,rect_t *bounds);
void    __MGL_getCharMetrics(wchar_t ch,metrics_t *m);
void    __MGL_findUnderScoreLocation(int *x,int *y,int width);
void    _MGL_generateGlyph(int ch, bitmap_font *font);

/* Text rendering functions */

void    _MGL_drawStrVec(int x,int y,const char *str);
void    _MGL_drawStrBitmap(int x,int y,const char *str);
void    _MGL_drawStrBitmap_W(int x,int y,const wchar_t *str);

/* Font enumeration helper functions */

void    _MGL_initFontEnumCache(void);
void    _MGL_destroyFontEnumCache(void);

/* Bitmap allocation routines */

long    _MGL_allocateDIB(MGLDC *dc,winBITMAPINFO *info);
void    _PM_freeDIB(MGLDC *dc);

/* Internal mouse handling routines */

void    _MS_init(void);
void    _MS_setDisplayDC(MGLDC *dc);
void    EVTAPI _MS_moveCursor(int x,int y);
void    _MS_saveState(void);
void    _MS_restoreState(void);

/* Internal region management routines */

void    _MGL_createBlockList(blockList_t *l,int max,int objectSize);
void    _MGL_resizeBlockList(blockList_t *l);
void    _PM_freeBlockList(blockList_t *l);
void *  _MGL_buildFreeList(blockList_t *l);
void    _MGL_createSegmentList(void);
void    _PM_freeSegmentList(void);
segment_t * _MGL_newSegment(void);
segment_t * _MGL_copySegment(segment_t *s);
segment_t * _MGL_copySegmentOfs(segment_t *s,int xOffset);
void    _PM_freeSegment(segment_t *s);
void    _MGL_createSpanList(void);
void    _PM_freeSpanList(void);
span_t  * _MGL_newSpan(void);
span_t  * _MGL_createSpan(int y,int x1,int x2);
span_t  * _MGL_createSpan2(int y,int x1,int x2,int x3,int x4);
span_t  * _MGL_copySpan(span_t *s);
span_t  * _MGL_copySpanOfs(span_t *s,int xOffset,int yOffset);
span_t  * _MGL_copySpans(span_t *s);
span_t  * _MGL_copySpansOfs(span_t *s,int xOffset,int yOffset);
void    _PM_freeSpan(span_t *s);
void    _MGL_createRegionList(void);
void    _MGL_freeRegionList(void);
span_t  * _MGL_rectRegion(region_t *r);
region_t * _MGL_createRegionFromBuf(scanlist *scanList);
region_t * __MGL_rgnEllipse(int left,int top,int A,int B,const region_t *_pen);
region_t * __MGL_rgnEllipseArc(int left,int top,int A,int B,int startAngle,int endAngle,const region_t *pen);

/* Region manipulation macros for maximum speed */

#define _MGL_tmpRectRegion(r)               \
{                                           \
    _MGL_tmpY1.y = (r)->rect.top;           \
    _MGL_tmpY2.y = (r)->rect.bottom;        \
    _MGL_tmpX1.x = (r)->rect.left;          \
    _MGL_tmpX2.x = (r)->rect.right;         \
    _MGL_rtmp.rect = (r)->rect;             \
    (r) = &_MGL_rtmp;                       \
}

#define _MGL_tmpRectRegion2(r,rt)           \
{                                           \
    _MGL_rtmp2.rect = rt;                   \
    (r) = &_MGL_rtmp2;                      \
}

#ifdef  __INTEL__
#pragma pack()              /* Return to default packing                */
#endif

#endif  /* __MGL_H */
