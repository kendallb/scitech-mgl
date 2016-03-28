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
* Description:  Header file for the windows compatible HalfToning tables
*               for creating a HalfTone palette and for performing the
*               HalfTone dithering algorithm in 8/15/16 bpp modes.
*
*               The halftone palette is set up to use entries from 20 to
*               226 in the physical palette, leaving the top 20 and bottom
*               20 for operating system specific use.
*
****************************************************************************/

#ifndef __HALFTONE_H
#define __HALFTONE_H

/*---------------------- Macro and type definitions -----------------------*/

#ifndef __MGRAPH_H
typedef struct {
    unsigned char   blue;       /* Blue component of color              */
    unsigned char   green;      /* Green component of color             */
    unsigned char   red;        /* Blue component of color              */
    unsigned char   alpha;      /* Alpha or alignment byte              */
    } palette_t;
#endif

/* Macros to halfTone dither an 8bpp RGB pixel */

#define PACK_COLOR_DITHER8(p,x,y,R,G,B)                         \
{                                                               \
    uchar dither = _MGL_dither8x8[(((y) & 7) << 3) + ((x) & 7)];\
    (p) = 20 +                                                  \
        _MGL_div51[R] + (_MGL_mod51[R] > dither) +              \
        _MGL_mul6[_MGL_div51[G] + (_MGL_mod51[G] > dither)] +   \
        _MGL_mul36[_MGL_div51[B] + (_MGL_mod51[B] > dither)];   \
}

#define HALFTONE_VARS_8                                         \
    int    __Rdiv51,__Rmod51;                                   \
    int    __Gdiv51,__Gmod51;                                   \
    int    __Bdiv51,__Bmod51;                                   \
    uchar  *__dp

#define SETUP_DITHER8(y,R,G,B)                                  \
{                                                               \
    __dp = &_MGL_dither8x8[((y) & 7) << 3];                     \
    __Rdiv51 = _MGL_div51[R];                                   \
    __Rmod51 = _MGL_mod51[R];                                   \
    __Gdiv51 = _MGL_div51[G];                                   \
    __Gmod51 = _MGL_mod51[G];                                   \
    __Bdiv51 = _MGL_div51[B];                                   \
    __Bmod51 = _MGL_mod51[B];                                   \
}

#define PACK_COLOR2_DITHER8(p,x)                                \
{                                                               \
    uchar __dither = __dp[(x) & 7];                             \
    (p) = 20 +                                                  \
        __Rdiv51 + (__Rmod51 > __dither) +                      \
        _MGL_mul6[__Gdiv51 + (__Gmod51 > __dither)] +           \
        _MGL_mul36[__Bdiv51 + (__Bmod51 > __dither)];           \
}

/* Macros to halfTone dither a 16bit 5/5/5 RGB pixel */

#define PACK_COLOR_DITHER555(p,x,y,R,G,B)                               \
{                                                                       \
    uchar _dither = _MGL_dither4x4[(((y) & 3) << 2) + ((x) & 3)];       \
    (p) = (ushort)                                                      \
       ((((M_uint32)_MGL_div8[R] + (_MGL_mod8[R] > _dither)) << 10) +      \
        (((M_uint32)_MGL_div8[G] + (_MGL_mod8[G] > _dither)) << 5) +       \
        (((M_uint32)_MGL_div8[B] + (_MGL_mod8[B] > _dither)) << 0));       \
}

#define HALFTONE_VARS_16                                        \
    int    __Rdiv8,__Rmod8;                                     \
    int    __Gdiv8,__Gmod8;                                     \
    int    __Bdiv8,__Bmod8;                                     \
    uchar  *__dp

#define SETUP_DITHER555(y,R,G,B)                                \
{                                                               \
    __dp = &_MGL_dither4x4[((y) & 3) << 2];                     \
    __Rdiv8 = _MGL_div8[R];                                     \
    __Rmod8 = _MGL_mod8[R];                                     \
    __Gdiv8 = _MGL_div8[G];                                     \
    __Gmod8 = _MGL_mod8[G];                                     \
    __Bdiv8 = _MGL_div8[B];                                     \
    __Bmod8 = _MGL_mod8[B];                                     \
}

#define PACK_COLOR2_DITHER555(p,x)                              \
{                                                               \
    uchar _dither = __dp[(x) & 3];                              \
    (p) = (ushort)                                              \
       (((__Rdiv8 + (__Rmod8 > _dither)) << 10) +               \
        ((__Gdiv8 + (__Gmod8 > _dither)) << 5) +                \
        ((__Bdiv8 + (__Bmod8 > _dither)) << 0));                \
}

/* Macros to halfTone dither a 16bit 5/6/5 RGB pixel */

#define PACK_COLOR_DITHER565(p,x,y,R,G,B)                               \
{                                                                       \
    uchar _dither = _MGL_dither4x4[(((y) & 3) << 2) + ((x) & 3)];       \
    (p) = (ushort)                                                      \
       ((((M_uint32)_MGL_div8[R] + (_MGL_mod8[R] > _dither)) << 11) +      \
        (((M_uint32)_MGL_div4[G] + (_MGL_mod4[G] > (_dither>>1))) << 5) +  \
        (((M_uint32)_MGL_div8[B] + (_MGL_mod8[B] > _dither)) << 0));       \
}

#define SETUP_DITHER565(y,R,G,B)                                \
{                                                               \
    __dp = &_MGL_dither4x4[((y) & 3) << 2];                     \
    __Rdiv8 = _MGL_div8[R];                                     \
    __Rmod8 = _MGL_mod8[R];                                     \
    __Gdiv8 = _MGL_div4[G];                                     \
    __Gmod8 = _MGL_mod4[G];                                     \
    __Bdiv8 = _MGL_div8[B];                                     \
    __Bmod8 = _MGL_mod8[B];                                     \
}

#define PACK_COLOR2_DITHER565(p,x)                              \
{                                                               \
    uchar __dither = __dp[(x) & 3];                             \
    (p) = (ushort)                                              \
       (((__Rdiv8 + (__Rmod8 > __dither)) << 11) +              \
        ((__Gdiv8 + (__Gmod8 > (__dither>>1))) << 5) +          \
        ((__Bdiv8 + (__Bmod8 > __dither)) << 0));               \
}

/*--------------------------- Global Variables ----------------------------*/

extern palette_t    _VARAPI _MGL_halftonePal[];

/* Division lookup tables.  These tables compute 0-255 divided by 51 and
 * modulo 51.  These tables could approximate gamma correction.
 */

extern unsigned char _VARAPI _MGL_div51[256];
extern unsigned char _VARAPI _MGL_mod51[256];
extern unsigned char _VARAPI _MGL_div8[256];
extern unsigned char _VARAPI _MGL_mod8[256];
extern unsigned char _VARAPI _MGL_div4[256];
extern unsigned char _VARAPI _MGL_mod4[256];

/* Multiplication lookup tables. These compute 0-5 times 6 and 36. */

extern unsigned char _VARAPI _MGL_mul6[6];
extern unsigned char _VARAPI _MGL_mul36[6];

/* Dither matrices */

extern unsigned char _VARAPI _MGL_dither8x8[64];
extern unsigned char _VARAPI _MGL_dither4x4[16];

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {
#endif

void    MGLAPI MGL_getHalfTonePalette(palette_t *pal);
uchar   MGLAPI MGL_halfTonePixel(int x,int y,uchar R,uchar G,uchar B);
ushort  MGLAPI MGL_halfTonePixel555(int x,int y,uchar R,uchar G,uchar B);
ushort  MGLAPI MGL_halfTonePixel565(int x,int y,uchar R,uchar G,uchar B);

#ifdef  __cplusplus
}
#endif

#endif  /* __HALFTONE_H */

