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
* Description:  Default HalfTone palette and associated lookup tables
*               for performing fast halftone dithering.
*
*               The halftone palette is set up to use entries from 20 to
*               226 in the physical palette, leaving the top 20 and bottom
*               20 for operating system specific use.
*
****************************************************************************/

#include "mgl.h"
#include <string.h>

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Returns a copy of the MGL halftone palette.

HEADER:
mgraph.h

PARAMETERS:
pal - Place to store the halftone palette values

REMARKS:
This function copies the MGL halftone palette into the specified palette structure.
The halftone palette always contains 256 colors, and hence the palette array must
contain 256 palette entries. This palette is a special palette used by MGL when
running in RGB dithered rasterizing mode for 8 bit video modes. If you intend to
enable RGB dithering with the MGL_setColorMapMode function, you must get a
copy of the halftone palette and program the hardware palette for your display
device or windowed device to be the same as this halftone palette.

Note that the MGL halftone palette is compatible with the standard Windows
halftone palette, so you can perform 8 bit dithering operations in a window without
needing to go into SYSPAL_STATIC mode.

SEE ALSO:
MGL_setPalette, MGL_realizePalette, MGL_setColorMapMode
****************************************************************************/
void MGLAPI MGL_getHalfTonePalette(
    palette_t *pal)
{ memcpy(pal,_MGL_halftonePal,sizeof(palette_t) * 256); }

/****************************************************************************
DESCRIPTION:
Compute color for pixel in halftone palette.

HEADER:
mgraph.h

PARAMETERS:
x   - x pixel coordinate
y   - y pixel coordinate
R   - Red component for pixel color
G   - Green component for pixel color
B   - Blue component for pixel color

RETURNS:
Color index for the pixel in MGL halftone palette

REMARKS:
This function computes the color index for a specified 24 bit RGB pixel value in the
standard MGL halftone palette, and can be used to perform real-time dithering of
pixel values from RGB colors to color index colors. This routine uses a fast table
lookup and an 8x8 ordered dither to do the conversion.

SEE ALSO:
MGL_getHalfTonePalette
****************************************************************************/
uchar MGLAPI MGL_halfTonePixel(
    int x,
    int y,
    uchar R,
    uchar G,
    uchar B)
{
    uchar dither = _MGL_dither8x8[((x & 7) << 3) + (y & 7)];
    return 20 +
        _MGL_div51[R] + (_MGL_mod51[R] > dither) +
        _MGL_mul6[_MGL_div51[G] + (_MGL_mod51[G] > dither)] +
        _MGL_mul36[_MGL_div51[B] + (_MGL_mod51[B] > dither)];
}

/***************************************************************************
DESCRIPTION:
Compute color for an 555 format RGB pixel using a halftone dither

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate of pixel (need only be relative)
y   - y coordinate of pixel (need only be relative)
R   - R value for pixel (8 bit components)
G   - G value for pixel (8 bit components)
B   - B value for pixel (8 bit components)

RETURNS:
Packed RGB 555 format pixel color

REMARKS:
This function computes the color of an RGB 555 format pixel using a fast
halftone dither algorithm. The resulting color is pre-packed into the
correct framebuffer format as part of the dithering process.

SEE ALSO:
MGL_halfTonePixel565
***************************************************************************/
ushort MGLAPI MGL_halfTonePixel555(
    int x,
    int y,
    uchar R,
    uchar G,
    uchar B)
{
    uchar _dither = _MGL_dither4x4[(((y) & 3) << 2) + ((x) & 3)];
    return (ushort)
       ((((ulong)_MGL_div8[R] + (_MGL_mod8[R] > _dither)) << 10) +
        (((ulong)_MGL_div8[G] + (_MGL_mod8[G] > _dither)) << 5) +
        (((ulong)_MGL_div8[B] + (_MGL_mod8[B] > _dither)) << 0));
}

/***************************************************************************
DESCRIPTION:
Compute color for an 565 format RGB pixel using a halftone dither

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate of pixel (need only be relative)
y   - y coordinate of pixel (need only be relative)
R   - R value for pixel (8 bit components)
G   - G value for pixel (8 bit components)
B   - B value for pixel (8 bit components)

RETURNS:
Packed RGB 565 format pixel color

REMARKS:
This function computes the color of an RGB 565 format pixel using a fast
halftone dither algorithm. The resulting color is pre-packed into the
correct framebuffer format as part of the dithering process.

SEE ALSO:
MGL_halfTonePixel555
***************************************************************************/
ushort MGLAPI MGL_halfTonePixel565(
    int x,
    int y,
    uchar R,
    uchar G,
    uchar B)
{
    uchar _dither = _MGL_dither4x4[(((y) & 3) << 2) + ((x) & 3)];
    return (ushort)
       ((((ulong)_MGL_div8[R] + (_MGL_mod8[R] > _dither)) << 11) +
        (((ulong)_MGL_div4[G] + (_MGL_mod4[G] > (_dither>>1))) << 5) +
        (((ulong)_MGL_div8[B] + (_MGL_mod8[B] > _dither)) << 0));
}

/*------------------------- Lookup Tables ---------------------------------*/

/* Default windows compatible halftone palette. This includes the default
 * Windows system colors in the first 10 and last 10 entries in the
 * palette.
 */

palette_t _VARAPI _MGL_halftonePal[256] = {
    {0x00,0x00,0x00,0}, {0xA8,0x00,0x00,0}, {0x00,0xA8,0x00,0}, {0xA8,0xA8,0x00,0},
    {0x00,0x00,0xA8,0}, {0xA8,0x00,0xA8,0}, {0x00,0x54,0xA8,0}, {0xA8,0xA8,0xA8,0},
    {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
    {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0},
    {0x00,0x00,0x00,0}, {0x14,0x14,0x14,0}, {0x20,0x20,0x20,0}, {0x2C,0x2C,0x2C,0},
    {0x00,0x00,0x00,0}, {0x00,0x00,0x33,0}, {0x00,0x00,0x66,0}, {0x00,0x00,0x99,0},
    {0x00,0x00,0xCC,0}, {0x00,0x00,0xFF,0}, {0x00,0x33,0x00,0}, {0x00,0x33,0x33,0},
    {0x00,0x33,0x66,0}, {0x00,0x33,0x99,0}, {0x00,0x33,0xCC,0}, {0x00,0x33,0xFF,0},
    {0x00,0x66,0x00,0}, {0x00,0x66,0x33,0}, {0x00,0x66,0x66,0}, {0x00,0x66,0x99,0},
    {0x00,0x66,0xCC,0}, {0x00,0x66,0xFF,0}, {0x00,0x99,0x00,0}, {0x00,0x99,0x33,0},
    {0x00,0x99,0x66,0}, {0x00,0x99,0x99,0}, {0x00,0x99,0xCC,0}, {0x00,0x99,0xFF,0},
    {0x00,0xCC,0x00,0}, {0x00,0xCC,0x33,0}, {0x00,0xCC,0x66,0}, {0x00,0xCC,0x99,0},
    {0x00,0xCC,0xCC,0}, {0x00,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x00,0xFF,0x00,0},
    {0x00,0xFF,0x66,0}, {0x00,0xFF,0x99,0}, {0x00,0xFF,0xCC,0}, {0x00,0xFF,0xFF,0},
    {0x33,0x00,0x00,0}, {0x33,0x00,0x33,0}, {0x33,0x00,0x66,0}, {0x33,0x00,0x99,0},
    {0x33,0x00,0xCC,0}, {0x33,0x00,0xFF,0}, {0x33,0x33,0x00,0}, {0x33,0x33,0x33,0},
    {0x33,0x33,0x66,0}, {0x33,0x33,0x99,0}, {0x33,0x33,0xCC,0}, {0x33,0x33,0xFF,0},
    {0x33,0x66,0x00,0}, {0x33,0x66,0x33,0}, {0x33,0x66,0x66,0}, {0x33,0x66,0x99,0},
    {0x33,0x66,0xCC,0}, {0x33,0x66,0xFF,0}, {0x33,0x99,0x00,0}, {0x33,0x99,0x33,0},
    {0x33,0x99,0x66,0}, {0x33,0x99,0x99,0}, {0x33,0x99,0xCC,0}, {0x33,0x99,0xFF,0},
    {0x33,0xCC,0x00,0}, {0x33,0xCC,0x33,0}, {0x33,0xCC,0x66,0}, {0x33,0xCC,0x99,0},
    {0x33,0xCC,0xCC,0}, {0x33,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x33,0xFF,0x33,0},
    {0x33,0xFF,0x66,0}, {0x33,0xFF,0x99,0}, {0x33,0xFF,0xCC,0}, {0x33,0xFF,0xFF,0},
    {0x66,0x00,0x00,0}, {0x66,0x00,0x33,0}, {0x66,0x00,0x66,0}, {0x66,0x00,0x99,0},
    {0x66,0x00,0xCC,0}, {0x66,0x00,0xFF,0}, {0x66,0x33,0x00,0}, {0x66,0x33,0x33,0},
    {0x66,0x33,0x66,0}, {0x66,0x33,0x99,0}, {0x66,0x33,0xCC,0}, {0x66,0x33,0xFF,0},
    {0x66,0x66,0x00,0}, {0x66,0x66,0x33,0}, {0x66,0x66,0x66,0}, {0x66,0x66,0x99,0},
    {0x66,0x66,0xCC,0}, {0x66,0x66,0xFF,0}, {0x66,0x99,0x00,0}, {0x66,0x99,0x33,0},
    {0x66,0x99,0x66,0}, {0x66,0x99,0x99,0}, {0x66,0x99,0xCC,0}, {0x66,0x99,0xFF,0},
    {0x66,0xCC,0x00,0}, {0x66,0xCC,0x33,0}, {0x66,0xCC,0x66,0}, {0x66,0xCC,0x99,0},
    {0x66,0xCC,0xCC,0}, {0x66,0xCC,0xFF,0}, {0x66,0xFF,0x00,0}, {0x66,0xFF,0x33,0},
    {0x66,0xFF,0x66,0}, {0x66,0xFF,0x99,0}, {0x66,0xFF,0xCC,0}, {0x66,0xFF,0xFF,0},
    {0x99,0x00,0x00,0}, {0x99,0x00,0x33,0}, {0x99,0x00,0x66,0}, {0x99,0x00,0x99,0},
    {0x99,0x00,0xCC,0}, {0x99,0x00,0xFF,0}, {0x99,0x33,0x00,0}, {0x99,0x33,0x33,0},
    {0x99,0x33,0x66,0}, {0x99,0x33,0x99,0}, {0x99,0x33,0xCC,0}, {0x99,0x33,0xFF,0},
    {0x99,0x66,0x00,0}, {0x99,0x66,0x33,0}, {0x99,0x66,0x66,0}, {0x99,0x66,0x99,0},
    {0x99,0x66,0xCC,0}, {0x99,0x66,0xFF,0}, {0x99,0x99,0x00,0}, {0x99,0x99,0x33,0},
    {0x99,0x99,0x66,0}, {0x99,0x99,0x99,0}, {0x99,0x99,0xCC,0}, {0x99,0x99,0xFF,0},
    {0x99,0xCC,0x00,0}, {0x99,0xCC,0x33,0}, {0x99,0xCC,0x66,0}, {0x99,0xCC,0x99,0},
    {0x99,0xCC,0xCC,0}, {0x99,0xCC,0xFF,0}, {0x99,0xFF,0x00,0}, {0x99,0xFF,0x33,0},
    {0x99,0xFF,0x66,0}, {0x99,0xFF,0x99,0}, {0x99,0xFF,0xCC,0}, {0x99,0xFF,0xFF,0},
    {0xCC,0x00,0x00,0}, {0xCC,0x00,0x33,0}, {0xCC,0x00,0x66,0}, {0xCC,0x00,0x99,0},
    {0xCC,0x00,0xCC,0}, {0xCC,0x00,0xFF,0}, {0xCC,0x33,0x00,0}, {0xCC,0x33,0x33,0},
    {0xCC,0x33,0x66,0}, {0xCC,0x33,0x99,0}, {0xCC,0x33,0xCC,0}, {0xCC,0x33,0xFF,0},
    {0xCC,0x66,0x00,0}, {0xCC,0x66,0x33,0}, {0xCC,0x66,0x66,0}, {0xCC,0x66,0x99,0},
    {0xCC,0x66,0xCC,0}, {0xCC,0x66,0xFF,0}, {0xCC,0x99,0x00,0}, {0xCC,0x99,0x33,0},
    {0xCC,0x99,0x66,0}, {0xCC,0x99,0x99,0}, {0xCC,0x99,0xCC,0}, {0xCC,0x99,0xFF,0},
    {0xCC,0xCC,0x00,0}, {0xCC,0xCC,0x33,0}, {0xCC,0xCC,0x66,0}, {0xCC,0xCC,0x99,0},
    {0xCC,0xCC,0xCC,0}, {0xCC,0xCC,0xFF,0}, {0xCC,0xFF,0x00,0}, {0xCC,0xFF,0x33,0},
    {0xCC,0xFF,0x66,0}, {0xCC,0xFF,0x99,0}, {0xCC,0xFF,0xCC,0}, {0xCC,0xFF,0xFF,0},
    {0xFF,0x00,0x00,0}, {0xFF,0x00,0x00,0}, {0xFF,0x00,0x66,0}, {0xFF,0x00,0x99,0},
    {0xFF,0x00,0xCC,0}, {0xFF,0x00,0xFF,0}, {0xFF,0x00,0x00,0}, {0xFF,0x33,0x33,0},
    {0xFF,0x33,0x66,0}, {0xFF,0x33,0x99,0}, {0xFF,0x33,0xCC,0}, {0xFF,0x33,0xFF,0},
    {0xFF,0x66,0x00,0}, {0xFF,0x66,0x33,0}, {0xFF,0x66,0x66,0}, {0xFF,0x66,0x99,0},
    {0xFF,0x66,0xCC,0}, {0xFF,0x66,0xFF,0}, {0xFF,0x99,0x00,0}, {0xFF,0x99,0x33,0},
    {0xFF,0x99,0x66,0}, {0xFF,0x99,0x99,0}, {0xFF,0x99,0xCC,0}, {0xFF,0x99,0xFF,0},
    {0xFF,0xCC,0x00,0}, {0xFF,0xCC,0x33,0}, {0xFF,0xCC,0x66,0}, {0xFF,0xCC,0x99,0},
    {0xFF,0xCC,0xCC,0}, {0xFF,0xCC,0xFF,0}, {0xFF,0xFF,0x00,0}, {0xFF,0xFF,0x33,0},
    {0xFF,0xFF,0x66,0}, {0xFF,0xFF,0x99,0}, {0xFF,0xFF,0xCC,0}, {0xFF,0xFF,0xFF,0},
    {0x2C,0x40,0x40,0}, {0x2C,0x40,0x3C,0}, {0x2C,0x40,0x34,0}, {0x2C,0x40,0x30,0},
    {0x2C,0x40,0x2C,0}, {0x30,0x40,0x2C,0}, {0x34,0x40,0x2C,0}, {0x3C,0x40,0x2C,0},
    {0x40,0x40,0x2C,0}, {0x40,0x3C,0x2C,0}, {0x40,0x34,0x2C,0}, {0x40,0x30,0x2C,0},
    {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
    {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0},
    };

/* Division lookup tables.  These tables compute 0-255 divided by 51 and
 * modulo 51.  These tables could approximate gamma correction.
 */

uchar _VARAPI _MGL_div51[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
    };

uchar _VARAPI _MGL_mod51[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0,
    };

/* Multiplication lookup tables. These compute 0-5 times 6 and 36. */

uchar _VARAPI _MGL_mul6[6] = {
    0, 6, 12, 18, 24, 30
    };

uchar _VARAPI _MGL_mul36[6] = {
    0, 36, 72, 108, 144, 180
    };

/* Ordered 8x8 dither matrix for 8 bit to 2.6 bit halftones. */

uchar _VARAPI _MGL_dither8x8[64] = {
     0, 38,  9, 47,  2, 40, 11, 50,
    25, 12, 35, 22, 27, 15, 37, 24,
     6, 44,  3, 41,  8, 47,  5, 43,
    31, 19, 28, 15, 34, 21, 31, 18,
     1, 39, 11, 49,  0, 39, 10, 48,
    27, 14, 36, 23, 26, 13, 35, 23,
     7, 46,  4, 43,  7, 45,  3, 42,
    33, 20, 30, 17, 32, 19, 29, 16,
    };

/* Division lookup tables for 16bpp dithering */

uchar _VARAPI _MGL_div8[256] = {
    0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,
    10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,
    12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,
    14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,
    16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,
    18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,
    20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,
    22,22,22,22,22,22,22,22,23,23,23,23,23,23,23,23,
    24,24,24,24,24,24,24,24,25,25,25,25,25,25,25,25,
    26,26,26,26,26,26,26,26,27,27,27,27,27,27,27,27,
    28,28,28,28,28,28,28,28,29,29,29,29,29,29,29,29,
    30,30,30,30,30,30,30,30,31,31,31,31,31,31,31,31,
    };

uchar _VARAPI _MGL_mod8[256] = {
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7,0,0,0,0,0,0,0,0,
    };

uchar _VARAPI _MGL_div4[256] = {
    0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,
    4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,
    8,8,8,8,9,9,9,9,10,10,10,10,11,11,11,11,
    12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,
    16,16,16,16,17,17,17,17,18,18,18,18,19,19,19,19,
    20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,23,
    24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27,
    28,28,28,28,29,29,29,29,30,30,30,30,31,31,31,31,
    32,32,32,32,33,33,33,33,34,34,34,34,35,35,35,35,
    36,36,36,36,37,37,37,37,38,38,38,38,39,39,39,39,
    40,40,40,40,41,41,41,41,42,42,42,42,43,43,43,43,
    44,44,44,44,45,45,45,45,46,46,46,46,47,47,47,47,
    48,48,48,48,49,49,49,49,50,50,50,50,51,51,51,51,
    52,52,52,52,53,53,53,53,54,54,54,54,55,55,55,55,
    56,56,56,56,57,57,57,57,58,58,58,58,59,59,59,59,
    60,60,60,60,61,61,61,61,62,62,62,62,63,63,63,63,
    };

uchar _VARAPI _MGL_mod4[256] = {
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,0,0,0,
    };

/* Ordered 4x4 dither matrix for 8 bit to 5 bit halftones. */

uchar _VARAPI _MGL_dither4x4[16] = {
    0, 4, 1, 5,
    6, 2, 7, 3,
    1, 5, 0, 4,
    7, 3, 6, 2,
    };

