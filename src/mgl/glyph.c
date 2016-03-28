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
* Description:  Monchrome bitmap manpulation routines, such as rotation,
*               mirroring and shearing.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

/* Declare a table for performing bitmap mirroring */

/* {secret} */
uchar _MGL_mirrorTable[] = {
    0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,
    0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
    0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,
    0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
    0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,
    0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
    0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,
    0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
    0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,
    0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
    0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,
    0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
    0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,
    0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
    0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,
    0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
    0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,
    0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
    0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,
    0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
    0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,
    0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
    0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,
    0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
    0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,
    0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
    0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,
    0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
    0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,
    0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
    0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,
    0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF,
    };

/* Declare a table for performing 90 degree bitmap rotations */

#define genTable(n)                                                         \
    {   0x00000000UL<<n,0x00000001UL<<n,0x00000100UL<<n,0x00000101UL<<n,    \
        0x00010000UL<<n,0x00010001UL<<n,0x00010100UL<<n,0x00010101UL<<n,    \
        0x01000000UL<<n,0x01000001UL<<n,0x01000100UL<<n,0x01000101UL<<n,    \
        0x01010000UL<<n,0x01010001UL<<n,0x01010100UL<<n,0x01010101UL<<n,    \
    }

/* {secret} */
ulong _MGL_rotTable[8][16] = {
    genTable(0),
    genTable(1),
    genTable(2),
    genTable(3),
    genTable(4),
    genTable(5),
    genTable(6),
    genTable(7),
    };

/* Define macros to extract part of the 32 bit result from the tables */

#define extract(n)                      \
    lo |= _MGL_rotTable[n][*src & 0xF]; \
    hi |= _MGL_rotTable[n][*src >> 4];  \
    src += srcStep

#define extractPartial(n)               \
    if (--height < 0) goto Done;        \
    extract(n);

/* Define macros to unpack 32 bits into the output bytes    */

#define unpackClockwise(w)                              \
    *dst = (uchar)((w >> 24) & 0xFF);   dst += dstStep; \
    *dst = (uchar)((w >> 16) & 0xFF);   dst += dstStep; \
    *dst = (uchar)((w >> 8) & 0xFF);    dst += dstStep; \
    *dst = (uchar)(w & 0xFF);           dst += dstStep;

#define unpackAClockwise(w)                             \
    *dst = (uchar)(w & 0xFF);           dst += dstStep; \
    *dst = (uchar)((w >> 8) & 0xFF);    dst += dstStep; \
    *dst = (uchar)((w >> 16) & 0xFF);   dst += dstStep; \
    *dst = (uchar)((w >> 24) & 0xFF);   dst += dstStep

/****************************************************************************
PARAMETERS:
dst     - Starting address is detination bitmap
dstStep - Difference in bytes between rows in dest bitmap
src     - Starting address is source bitmap
srcStep - Difference in bytes between rows in source bitmap

REMARKS:
Rotates a full 8x8 bitmap tile anti-clockwise by using table lookup. The
bit extraction is done a nybble at a time to reduce the table sizes.

Original idea was presented in Graphics Gems II.
{secret}
****************************************************************************/
void _MGL_rotateFull8x8AClockwise(
    uchar *dst,
    int dstStep,
    uchar *src,
    int srcStep)
{
    ulong   lo,hi;

    lo = hi = 0;
    extract(7); extract(6); extract(5); extract(4);
    extract(3); extract(2); extract(1); extract(0);
    unpackAClockwise(lo);
    unpackAClockwise(hi);
    (void)src;
    (void)dst;
}

/****************************************************************************
PARAMETERS:
dst     - Starting address is detination bitmap
dstStep - Difference in bytes between rows in dest bitmap
src     - Starting address is source bitmap
srcStep - Difference in bytes between rows in source bitmap
height  - Maximum height of source bitmap

REMARKS:
Rotates a partial 8x8 bitmap tile anti-clockwise by using table lookup. The
bit extraction is done a nybble at a time to reduce the table sizes. In this
version we check to ensure that we do not go past the end of the height of
the source bitmap.
{secret}
****************************************************************************/
void _MGL_rotatePartial8x8AClockwise(
    uchar *dst,
    int dstStep,
    uchar *src,
    int srcStep,
    int height)
{
    ulong   lo,hi;

    lo = hi = 0;
    extractPartial(7);  extractPartial(6);  extractPartial(5);
    extractPartial(4);  extractPartial(3);  extractPartial(2);
    extractPartial(1);  extractPartial(0);

Done:
    unpackAClockwise(lo);
    unpackAClockwise(hi);
    (void)src;
    (void)dst;
}

/****************************************************************************
PARAMETERS:
dst     - Starting address is detination bitmap
dstStep - Difference in bytes between rows in dest bitmap
src     - Starting address is source bitmap
srcStep - Difference in bytes between rows in source bitmap

REMARKS:
Rotates a full 8x8 bitmap tile clockwise by using table lookup. The bit
extraction is done a nybble at a time to reduce the table sizes.

Original idea was presented in Graphics Gems II.
{secret}
****************************************************************************/
void _MGL_rotateFull8x8Clockwise(
    uchar *dst,
    int dstStep,
    uchar *src,
    int srcStep)
{
    ulong   lo,hi;

    lo = hi = 0;
    extract(0); extract(1); extract(2); extract(3);
    extract(4); extract(5); extract(6); extract(7);
    unpackClockwise(hi);
    unpackClockwise(lo);
    (void)src;
    (void)dst;
}

/****************************************************************************
PARAMETERS:
dst     - Starting address is detination bitmap
dstStep - Difference in bytes between rows in dest bitmap
src     - Starting address is source bitmap
srcStep - Difference in bytes between rows in source bitmap
height  - Maximum height of source bitmap

REMARKS:
Rotates a partial 8x8 bitmap tile clockwise by using table lookup. The bit
extraction is done a nybble at a time to reduce the table sizes. In this
version we check to ensure that we do not go past the end of the height of
the source bitmap.
{secret}
****************************************************************************/
void _MGL_rotatePartial8x8Clockwise(
    uchar *dst,
    int dstStep,
    uchar *src,
    int srcStep,
    int height)
{
    ulong   lo,hi;

    lo = hi = 0;
    extractPartial(0);  extractPartial(1);  extractPartial(2);
    extractPartial(3);  extractPartial(4);  extractPartial(5);
    extractPartial(6);  extractPartial(7);

Done:
    unpackClockwise(hi);
    unpackClockwise(lo);
    (void)src;
    (void)dst;
}

/****************************************************************************
PARAMETERS:
dst         - Destination bitmap buffer
src         - Source bitmap buffer
byteWidth   - Width of the bitmap in bytes
height      - Height of the bitmap in scanlines

REMARKS:
Rotates the source bitmap 180 degrees and stores in the destination bitmap.
{secret}
****************************************************************************/
void _MGL_rotateBitmap180(
    uchar *dst,
    uchar *src,
    int byteWidth,
    int height)
{
    int     i,max = height * byteWidth;

    dst += max;
    for (i = 0; i < max; i++)
        *--dst = _MGL_mirrorTable[*src++];
}

/****************************************************************************
Description
Rotates a monochrome glyph by the specified rotation factor.

HEADER:
mgraph.h

PARAMETERS:
dst - Destination glyph buffer
src - Source glyph buffer
byteWidth   - Width of the glyph in bytes
height  - Height of the glyph in scanlines
rotation    - Rotation direction for the glyph

REMARKS:
This function computes the rotated image glyph of the source glyph, and stores the
value in the destination buffer. The source buffer is not modified, and the rotated
image glyph may possibly be larger than the source glyph. The resulting width and
height of the destination glyph is returned.  Supported directions are enumerated in
MGL_textDirType

Note:You must preallocate enough space to hold the rotated glyph in the
destination buffer, as this may actually be larger than the source glyph.

The final size will be the following:

    (height + 7)/8 + byteWidth * 8

****************************************************************************/
void MGLAPI MGL_rotateGlyph(
    uchar *dst,
    uchar *src,
    int *byteWidth,
    int *height,
    int rotation)
{
    int     i,j,oldWidth,newWidth,oldHeight,newHeight,maxFull,partialHeight;

    if (rotation == MGL_LEFT_DIR) {
        _MGL_rotateBitmap180(dst,src,*byteWidth,*height);
        return;
        }

    oldWidth        = *byteWidth;
    oldHeight       = *height;
    newWidth        = (oldHeight + 7) >> 3;
    newHeight       = oldWidth * 8;
    maxFull         = oldHeight >> 3;
    partialHeight   = oldHeight & 0x7;

    switch (rotation) {
        case MGL_UP_DIR:
            for (j = 0; j < oldWidth; j++) {
                for (i = 0; i < maxFull; i++) {
                    /* Rotate each full 8x8 bitmap tile */
                    _MGL_rotateFull8x8AClockwise(
                        dst + (((oldWidth-1-j) * newWidth) << 3) + i,
                        newWidth,
                        src + ((i * oldWidth) << 3) + j,
                        oldWidth);
                    }
                /* Now rotate the last partial 8x8 bitmap tile */
                if (partialHeight) {
                    _MGL_rotatePartial8x8AClockwise(
                        dst + (((oldWidth-1-j) * newWidth) << 3) + i,
                        newWidth,
                        src + ((i * oldWidth) << 3) + j,
                        oldWidth, partialHeight);
                    }
                }
            break;
        case MGL_DOWN_DIR:
            for (j = 0; j < oldWidth; j++) {
                for (i = 0; i < maxFull; i++) {
                    /* Rotate each full 8x8 bitmap tile */
                    _MGL_rotateFull8x8Clockwise(
                        dst + ((j * newWidth) << 3) + (newWidth-1-i),
                        newWidth,
                        src + ((i * oldWidth) << 3) + j,
                        oldWidth);
                    }
                /* Now rotate the last partial 8x8 bitmap tile */
                if (partialHeight) {
                    _MGL_rotatePartial8x8Clockwise(
                        dst + ((j * newWidth) << 3) + (newWidth-1-i),
                        newWidth,
                        src + ((i * oldWidth) << 3) + j,
                        oldWidth, partialHeight);
                    }
                }
            break;
        }

    *byteWidth = newWidth;
    *height = newHeight;
}

/****************************************************************************
DESCRIPTION:
Computes mirror image of a glyph (monochrome bitmap).

HEADER:
mgraph.h

PARAMETERS:
dst - Destination glyph buffer
src - Source glyph buffer
byteWidth   - Width of the glyph in bytes
height  - Height of the glyph in scanlines

REMARKS:
This function computes the mirror image glyph of the source glyph, and stores the
value in the destination buffer. The source buffer is not modified, and the mirror
image glyph will be no larger than the original glyph.

See also:
MGL_rotateGlyph,MGL_drawGlyph
****************************************************************************/
void MGLAPI MGL_mirrorGlyph(
    uchar *dst,
    uchar *src,
    int byteWidth,
    int height)
{
    int     i,j;

    for (j = 0; j < height; j++) {
        for (i = byteWidth-1; i >= 0; i--)
            *(dst+i) = _MGL_mirrorTable[*src++];
        dst += byteWidth;
        }
}

/****************************************************************************
DESCRIPTION:
Draws a monochrome glyph.

HEADER:
mgraph.h

PARAMETERS:
font    - Font containing the glyphs
x       - x coordinate to draw glyph at
y       - y coordinate to draw glyph at
glyph   - Index of glyph to draw

REMARKS:
Rasterizes the specified monochrome glyph from the font file in the current
color at the specified location. This is effectively the same as drawing a
monochrome bitmap, but by storing all your monochrome bitmaps in a font
file, the glyphs will be stored as efficiently as possible.

SEE ALSO:
MGL_rotateGlyph, MGL_mirrorGlyph
****************************************************************************/
void MGLAPI MGL_drawGlyph(
    font_t *font,
    int x,
    int y,
    uchar glyph)
{
    rect_t  d,r,clip;
    int     offset,width,byteWidth,height = font->fontHeight;
    uchar   *bytes;

    /* Find the height of the glyph */
    if ((width = BITFONT(font)->iwidth[glyph]) != 0) {
        if ((offset = BITFONT(font)->offset[glyph]) == -1) {
            offset = BITFONT(font)->offset[BIT_MISSINGSYMBOL(font)];
            width = BITFONT(font)->iwidth[BIT_MISSINGSYMBOL(font)];
            }

        /* Clip to destination device context */
        BEGIN_VISIBLE_CLIP_LIST(&DC);
        d.left = x;             d.top = y;
        d.right = x + width;    d.bottom = y + height;
        if (!MGL_sectRect(DC.clipRectView,d,&r)) {
            END_VISIBLE_CLIP_LIST(&DC);
            return;
            }

        /* Now draw the glyph */
        bytes = (uchar *)&BITFONT(font)->def[offset];
        byteWidth = (width + 7) >> 3;
        if (DC.clipRegionScreen) {
            /* Draw it clipped to a complex clip region */
            x += DC.viewPort.left;
            y += DC.viewPort.top;
            d.left = x;             d.top = y;
            d.right = x + width;    d.bottom = y + height;
            BEGIN_CLIP_REGION(clip,DC.clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    if (!MGL_equalRect(d,r)) {
                        /* Draw it clipped to the clip rectangle */
                        DC.r.ClipMonoImage_Old(x,y,width,height,byteWidth,bytes,DC.a.backMode,
                            r.left,r.top,r.right,r.bottom);
                        }
                    else {
                        /* Draw it unclipped for maximum speed */
                        DC.r.PutMonoImage_Old(x,y,width,height,byteWidth,bytes,DC.a.backMode);
                        }
                    }
            END_CLIP_REGION();
            }
        else if (!MGL_equalRect(d,r)) {
            /* Draw it clipped to the clip rectangle */
            DC.r.ClipMonoImage_Old(x+DC.viewPort.left,y+DC.viewPort.top,
                width,height,byteWidth,bytes,DC.a.backMode,
                DC.clipRectScreen.left,DC.clipRectScreen.top,
                DC.clipRectScreen.right,DC.clipRectScreen.bottom);
            }
        else {
            /* Draw it unclipped for maximum speed */
            DC.r.PutMonoImage_Old(x+DC.viewPort.left,y+DC.viewPort.top,
                width,height,byteWidth,bytes,DC.a.backMode);
            }
        END_VISIBLE_CLIP_LIST(&DC);
        }
}

/****************************************************************************
DESCRIPTION:
Returns the width of a specified glyph in the specified font.

HEADER:
mgraph.h

PARAMETERS:
font    - Font of interest
glyph   - Index of glyph in font file to measure

RETURNS:
Returns the width of the specified glyph.

SEE ALSO:
MGL_getGlyphHeight
****************************************************************************/
int MGLAPI MGL_getGlyphWidth(
    font_t *font,
    uchar glyph)
{
    return BITFONT(font)->iwidth[glyph];
}

/****************************************************************************
DESCRIPTION:
Returns the height of the glyphs in the specified font.

HEADER:
mgraph.h

PARAMETERS:
font    - Font of interest

RETURNS:
Height of all the glyphs in the specified font.

SEE ALSO:
MGL_getGlyphWidth

****************************************************************************/
int MGLAPI MGL_getGlyphHeight(
    font_t *font)
{
    return font->fontHeight;
}

