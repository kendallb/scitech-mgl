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
* Description:  Bitmap font output and control routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
x           - x coordinate to draw from
y           - y coordinate to draw from
width       - Width of the glyph to draw
byteWidth   - Byte width of the glyph to draw
height      - Height of the glyph to draw
buffer      - Pointer to the glyph buffer

REMARKS:
This function draws an solid monochrome glyph at the specified location.
This function also takes care of clipping against the complex clip region
as necessary.
****************************************************************************/
static void drawGlyph(
    int x,
    int y,
    int width,
    int byteWidth,
    int height,
    uchar *buffer)
{
    rect_t  d,r,clip;

    /* Clip to destination device context and discard if complete clipped */
    d.left = x;             d.top = y;
    d.right = x + width;    d.bottom = y + height;
    if (!MGL_sectRect(DC.clipRectScreen,d,&r))
        return;

    /* Now draw the bitmap */
    if (DC.clipRegionScreen) {
        /* Draw it clipped to a complex clip region */
        d.left = x;             d.top = y;
        d.right = x + width;    d.bottom = y + height;
        BEGIN_CLIP_REGION(clip,DC.clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (!MGL_equalRect(d,r)) {
                    /* Draw it clipped to the clip rectangle */
                    DC.r.ClipMonoImage_Old(x,y,width,height,byteWidth,buffer,DC.a.backMode,
                        r.left,r.top,r.right,r.bottom);
                    }
                else {
                    /* Draw it unclipped for maximum speed */
                    DC.r.PutMonoImage_Old(x,y,width,height,byteWidth,buffer,DC.a.backMode);
                    }
                }
        END_CLIP_REGION();
        }
    else if (!MGL_equalRect(d,r)) {
        /* Draw it clipped to the clip rectangle */
        DC.r.ClipMonoImage_Old(x,y,width,height,byteWidth,buffer,DC.a.backMode,
            DC.clipRectScreen.left,DC.clipRectScreen.top,
            DC.clipRectScreen.right,DC.clipRectScreen.bottom);
        }
    else {
        /* Draw it unclipped for maximum speed */
        DC.r.PutMonoImage_Old(x,y,width,height,byteWidth,buffer,DC.a.backMode);
        }
}

/****************************************************************************
PARAMETERS:
x           - x coordinate to draw from
y           - y coordinate to draw from
width       - Width of the glyph to draw
byteWidth   - Byte width of the glyph to draw
height      - Height of the glyph to draw
buffer      - Pointer to the glyph buffer

REMARKS:
This function draws an solid monochrome glyph at the specified location.
This function also takes care of clipping against the complex clip region
as necessary.

The difference between this version and the one above is that the glyphs
are always drawn transparently.
****************************************************************************/
static void drawAAGlyph(
    int x,
    int y,
    int width,
    int byteWidth,
    int height,
    uchar *buffer)
{
    rect_t  d,r,clip;

    /* Clip to destination device context and discard if complete clipped */
    d.left = x;             d.top = y;
    d.right = x + width;    d.bottom = y + height;
    if (!MGL_sectRect(DC.clipRectScreen,d,&r))
        return;

    /* Now draw the bitmap */
    if (DC.clipRegionScreen) {
        /* Draw it clipped to a complex clip region */
        d.left = x;             d.top = y;
        d.right = x + width;    d.bottom = y + height;
        BEGIN_CLIP_REGION(clip,DC.clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (!MGL_equalRect(d,r)) {
                    /* Draw it clipped to the clip rectangle */
                    DC.r.ClipMonoImage_Old(x,y,width,height,byteWidth,buffer,true,
                        r.left,r.top,r.right,r.bottom);
                    }
                else {
                    /* Draw it unclipped for maximum speed */
                    DC.r.PutMonoImage_Old(x,y,width,height,byteWidth,buffer,true);
                    }
                }
        END_CLIP_REGION();
        }
    else if (!MGL_equalRect(d,r)) {
        /* Draw it clipped to the clip rectangle */
        DC.r.ClipMonoImage_Old(x,y,width,height,byteWidth,buffer,true,
            DC.clipRectScreen.left,DC.clipRectScreen.top,
            DC.clipRectScreen.right,DC.clipRectScreen.bottom);
        }
    else {
        /* Draw it unclipped for maximum speed */
        DC.r.PutMonoImage_Old(x,y,width,height,byteWidth,buffer,true);
        }
}

/****************************************************************************
PARAMETERS:
x           - x coordinate to draw from
y           - y coordinate to draw from
width       - Width of the glyph to draw
byteWidth   - Byte width of the glyph to draw
height      - Height of the glyph to draw
buffer      - Pointer to the glyph buffer
offset      - Offset to get to each glyph definition in list

REMARKS:
This function draws an anti-aliased monochrome glyph, which is defined as
a set of four levels of monochrome bitmaps.
****************************************************************************/
static void drawAntiAliasedGlyph(
    int x,
    int y,
    int width,
    int byteWidth,
    int height,
    uchar *buffer,
    int offset)
{
    uchar   fR,fG,fB,bR,bG,bB,R,G,B;
    color_t colorfg,color75,color50,color25,colorbg;
    rect_t  d,r;

    /* Clip to destination device context and discard if complete clipped */
    d.left = x;             d.top = y;
    d.right = x + width;    d.bottom = y + height;
    if (!MGL_sectRect(DC.clipRectScreen,d,&r))
        return;

    /* Find the glyph colors for anti-aliased rendering */
    if (!(DC.mi.modeFlags & MGL_IS_COLOR_INDEX)) {
        colorfg = DC.a.aaColor[0];
        colorbg = DC.a.aaColor[4];
        if (colorfg != DC.a.color || colorbg != DC.a.backColor) {
            /* The colors have changed, so re-compute the new blended
             * color values for the anti-aliased text.
             */
            colorfg = DC.a.color;
            colorbg = DC.a.backColor;
            MGL_unpackColorFast(&DC.pf, colorfg, fR, fG, fB);
            MGL_unpackColorFast(&DC.pf, colorbg, bR, bG, bB);
            R = (uchar) (((int)fR + (int)bR*3) >> 2);
            G = (uchar) (((int)fG + (int)bG*3) >> 2);
            B = (uchar) (((int)fB + (int)bB*3) >> 2);
            color25 = MGL_packColorFast(&DC.pf, R, G, B);
            R = (uchar) (((int)fR + (int)bR) >> 1);
            G = (uchar) (((int)fG + (int)bG) >> 1);
            B = (uchar) (((int)fB + (int)bB) >> 1);
            color50 = MGL_packColorFast(&DC.pf, R, G, B);
            R = (uchar) (((int)fR*3 + (int)bR) >> 2);
            G = (uchar) (((int)fG*3 + (int)bG) >> 2);
            B = (uchar) (((int)fB*3 + (int)bB) >> 2);
            color75 = MGL_packColorFast(&DC.pf, R, G, B);
            DC.a.aaColor[0] = colorfg;
            DC.a.aaColor[4] = colorbg;
            DC.a.aaColor[1] = color75;
            DC.a.aaColor[2] = color50;
            DC.a.aaColor[3] = color25;
            }
        else {
            color75 = DC.a.aaColor[1];
            color50 = DC.a.aaColor[2];
            color25 = DC.a.aaColor[3];
            }
        }
    else {
        colorfg = DC.a.aaColor[0];
        color75 = DC.a.aaColor[1];
        color50 = DC.a.aaColor[2];
        color25 = DC.a.aaColor[3];
        colorbg = DC.a.aaColor[4];
    }

    /* Draw the background pixels (only if in opaque mode) */
    if (DC.a.backMode == MGL_OPAQUE_BACKGROUND) {
        DC.r.SetForeColor(colorbg);
        DC.r.cur.DrawRect(x,y,width,height);
        }

    /* Draw the anti-aliased pixels */
    if (DC.a.fontBlendMode == MGL_AA_RGBBLEND && DC.a.backMode == MGL_TRANSPARENT_BACKGROUND) {
        /* Enable pixel blending */
        if (DC.r.SetBlendFunc)
            DC.r.SetBlendFunc(MGL_BLEND_CONSTANTALPHAFAST,MGL_BLEND_CONSTANTALPHAFAST);
        else {
            DC.r.SetSrcBlendFunc_Old(MGL_BLEND_CONSTANTALPHA);
            DC.r.SetDstBlendFunc_Old(MGL_BLEND_ONEMINUSCONSTANTALPHA);
            }
        DC.r.SetForeColor(colorfg);

        /* Draw the 25% blended pixels */
        DC.r.SetAlphaValue(63);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset*3);

        /* Draw the 50% blended pixels */
        DC.r.SetAlphaValue(127);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset*2);

        /* Draw the 75% blended pixels */
        DC.r.SetAlphaValue(191);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset);

        /* Disable pixel blending mode */
        if (DC.r.SetBlendFunc)
            DC.r.SetBlendFunc(DC.a.srcBlendFunc,DC.a.dstBlendFunc);
        else {
            DC.r.SetSrcBlendFunc_Old(DC.a.srcBlendFunc);
            DC.r.SetDstBlendFunc_Old(DC.a.dstBlendFunc);
            }
        }
    else {
        /* Draw the 25% blended pixels */
        DC.r.SetForeColor(color25);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset*3);

        /* Draw the 50% blended pixels */
        DC.r.SetForeColor(color50);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset*2);

        /* Draw the 75% blended pixels */
        DC.r.SetForeColor(color75);
        drawAAGlyph(x,y,width,byteWidth,height,buffer+offset);
        }

    /* Draw the solid foreground pixels */
    DC.r.SetForeColor(colorfg);
    drawAAGlyph(x,y,width,byteWidth,height,buffer);
}

/****************************************************************************
PARAMETERS:
bitFont     - Bitmap font to draw glyph for
ch          - character # to draw
x           - x coordinate to draw from
y           - y coordinate to draw from

RETURNS:
Next X position after this character.

REMARKS:
This function draws a Bitmap or Truetype font Glyph.
****************************************************************************/
static int drawFontGlyphRight(
    bitmap_font *bitFont,
    int ch,
    int x,
    int y)
{
    int     width,byteWidth,offset;
    int     iwidth, loc, glyph;
    uchar   *bytes;

    glyph = ch - bitFont->startGlyph;
    if ((glyph >= bitFont->numGlyphs) || (ch < bitFont->startGlyph)
            || ((offset = bitFont->offset[glyph]) == -1)) {
        /* Use the missing symbol definition */
        offset = bitFont->offset[BIT_MISSINGSYMBOL(bitFont)];
        width = bitFont->width[BIT_MISSINGSYMBOL(bitFont)];
        iwidth = bitFont->iwidth[BIT_MISSINGSYMBOL(bitFont)];
        glyph = BIT_MISSINGSYMBOL(bitFont);
        }
    else {
        /* Find the information for the glyph */
        if (!bitFont->valid[glyph])
            _MGL_generateGlyph(glyph,bitFont);
        width = bitFont->width[glyph];
        iwidth = bitFont->iwidth[glyph];
        }
    loc = bitFont->loc[glyph];
    bytes = (uchar*)bitFont->def+offset;
    byteWidth = (iwidth + 7) >> 3;
    if (iwidth != 0) {
        if (!bitFont->antialiased)
            drawGlyph(x+loc,y,iwidth,byteWidth,bitFont->fontHeight,bytes);
        else
            drawAntiAliasedGlyph(x+loc,y,iwidth,byteWidth,bitFont->fontHeight,bytes, ((iwidth + 7) / 8)*bitFont->fontHeight);
        }
    return (x+width+DC.a.ts.spaceExtra);
}

/****************************************************************************
PARAMETERS:
bitFont     - Bitmap font to draw glyph for
ch          - character # to draw
x           - x coordinate to draw from
y           - y coordinate to draw from

RETURNS:
Next Y position after this character.

REMARKS:
This function draws a Bitmap or Truetype font Glyph.
{secret}
****************************************************************************/
static int drawFontGlyphUp(
    bitmap_font *bitFont,
    int ch,
    int x,
    int y)
{
    int     width,byteWidth,offset;
    int     iwidth, loc, glyph, height;
    uchar   *bytes,*buf = _MGL_buf;

    glyph = ch - bitFont->startGlyph;
    if ((glyph >= bitFont->numGlyphs) || (ch < bitFont->startGlyph)
            || ((offset = bitFont->offset[glyph]) == -1)) {
        /* Use the missing symbol definition */
        offset = bitFont->offset[BIT_MISSINGSYMBOL(bitFont)];
        width = bitFont->width[BIT_MISSINGSYMBOL(bitFont)];
        iwidth = bitFont->iwidth[BIT_MISSINGSYMBOL(bitFont)];
        glyph = BIT_MISSINGSYMBOL(bitFont);
        }
    else {
        /* Find the information for the glyph */
        if (!bitFont->valid[glyph])
            _MGL_generateGlyph(glyph,bitFont);
        width = bitFont->width[glyph];
        iwidth = bitFont->iwidth[glyph];
        }
    loc = bitFont->loc[glyph];
    bytes = (uchar*)bitFont->def+offset;
    byteWidth = (iwidth + 7) >> 3;
    height = bitFont->fontHeight;
    if (iwidth != 0) {
        if (!bitFont->antialiased) {
            MGL_rotateGlyph(buf,bytes,&byteWidth,&height,MGL_UP_DIR);
            drawGlyph(x,y-height+1-loc,byteWidth<<3,byteWidth,height,buf);
            }
        else {
            int of2 = ((iwidth + 7) & -8)*((bitFont->fontHeight + 7) >> 3);
            int of3 = ((iwidth + 7) / 8)*bitFont->fontHeight;
            int bw2;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2*3,bytes+of3*3,&bw2,&height,MGL_UP_DIR);
            //of2 = iwidth*((bitfont->fontHeight + 7) >> 3);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2*2,bytes+of3*2,&bw2,&height,MGL_UP_DIR);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2,bytes+of3,&bw2,&height,MGL_UP_DIR);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf,bytes,&byteWidth,&height,MGL_UP_DIR);
            drawAntiAliasedGlyph(x, y-height+1-loc,byteWidth<<3,byteWidth, height,buf, of2);
            }
        }
    return (y-width-DC.a.ts.spaceExtra);
}

/****************************************************************************
PARAMETERS:
bitFont     - Bitmap font to draw glyph for
ch          - character # to draw
x           - x coordinate to draw from
y           - y coordinate to draw from

RETURNS:
Next Y position after this character.

REMARKS:
This function draws a Bitmap or Truetype font Glyph.
{secret}
****************************************************************************/
static int drawFontGlyphDown(
    bitmap_font *bitFont,
    int ch,
    int x,
    int y)
{
    int     width,byteWidth,offset;
    int     iwidth, loc, glyph, height;
    uchar   *bytes,*buf = _MGL_buf;

    glyph = ch - bitFont->startGlyph;
    if ((glyph >= bitFont->numGlyphs) || (ch < bitFont->startGlyph)
            || ((offset = bitFont->offset[glyph]) == -1)) {
        /* Use the missing symbol definition */
        offset = bitFont->offset[BIT_MISSINGSYMBOL(bitFont)];
        width = bitFont->width[BIT_MISSINGSYMBOL(bitFont)];
        iwidth = bitFont->iwidth[BIT_MISSINGSYMBOL(bitFont)];
        glyph = BIT_MISSINGSYMBOL(bitFont);
        }
    else {
        /* Find the information for the glyph */
        if (!bitFont->valid[glyph])
            _MGL_generateGlyph(glyph,bitFont);
        width = bitFont->width[glyph];
        iwidth = bitFont->iwidth[glyph];
        }
    loc = bitFont->loc[glyph];
    bytes = (uchar*)bitFont->def+offset;
    byteWidth = (iwidth + 7) >> 3;
    height = bitFont->fontHeight;
    if (iwidth != 0) {
        if (!bitFont->antialiased) {
            MGL_rotateGlyph(buf,bytes,&byteWidth,&height,MGL_DOWN_DIR);
            drawGlyph(x,y+loc,byteWidth<<3,byteWidth,iwidth,buf);
            }
        else {
            int of2 = ((iwidth + 7) / 8)*bitFont->fontHeight;
            int bw2;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf,bytes,&bw2,&height,MGL_DOWN_DIR);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2,bytes+of2,&bw2,&height,MGL_DOWN_DIR);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2+of2,bytes+of2+of2,&bw2,&height,MGL_DOWN_DIR);
            height = bitFont->fontHeight;
            bw2 = byteWidth;
            MGL_rotateGlyph(buf+of2+of2+of2,bytes+of2+of2+of2,&byteWidth,&height,MGL_DOWN_DIR);
            drawAntiAliasedGlyph(x, y+loc,byteWidth<<3,byteWidth,iwidth,buf, of2);
            }
        }
    return (y+width+DC.a.ts.spaceExtra);
}

/****************************************************************************
PARAMETERS:
bitFont     - Bitmap font to draw glyph for
ch          - character # to draw
x           - x coordinate to draw from
y           - y coordinate to draw from

RETURNS:
Next X position after this character.

REMARKS:
This function draws a Bitmap or Truetype font Glyph.
{secret}
****************************************************************************/
static int drawFontGlyphLeft(
    bitmap_font *bitFont,
    int ch,
    int x,
    int y)
{
    int     width,byteWidth,offset;
    int     iwidth, loc, glyph, height;
    uchar   *bytes,*buf = _MGL_buf;

    glyph = ch - bitFont->startGlyph;
    if ((glyph >= bitFont->numGlyphs) || (ch < bitFont->startGlyph)
            || ((offset = bitFont->offset[glyph]) == -1)) {
        /* Use the missing symbol definition */
        offset = bitFont->offset[BIT_MISSINGSYMBOL(bitFont)];
        width = bitFont->width[BIT_MISSINGSYMBOL(bitFont)];
        iwidth = bitFont->iwidth[BIT_MISSINGSYMBOL(bitFont)];
        glyph = BIT_MISSINGSYMBOL(bitFont);
        }
    else {
        /* Find the information for the glyph */
        if (!bitFont->valid[glyph])
            _MGL_generateGlyph(glyph,bitFont);
        width = bitFont->width[glyph];
        iwidth = bitFont->iwidth[glyph];
        }
    loc = bitFont->loc[glyph];
    bytes = (uchar*)bitFont->def+offset;
    byteWidth = (iwidth + 7) >> 3;
    height = bitFont->fontHeight;
    if (iwidth != 0) {
        if (!bitFont->antialiased) {
            _MGL_rotateBitmap180(buf,bytes,byteWidth,height);
            drawGlyph(x-(byteWidth<<3)+1-loc,y,(byteWidth<<3),byteWidth,height,buf);
            }
        else {
            int of2 = ((iwidth + 7) / 8)*bitFont->fontHeight;
            _MGL_rotateBitmap180(buf,bytes,byteWidth,height);
            _MGL_rotateBitmap180(buf+of2,bytes+of2,byteWidth,height);
            _MGL_rotateBitmap180(buf+of2+of2,bytes+of2+of2,byteWidth,height);
            _MGL_rotateBitmap180(buf+of2+of2+of2,bytes+of2+of2+of2,byteWidth,height);
            drawAntiAliasedGlyph(x-(byteWidth<<3)+1-loc, y,(byteWidth<<3), byteWidth,height,buf, of2);
            }
        }
    return (x-width-DC.a.ts.spaceExtra);
}

/****************************************************************************
PARAMETERS:
glyph   - Glyph to rasterise
font    - Bitmap font to rasterise glyph for

REMARKS:
This function rasterises a glyph for the bitmap font on the fly. This will
only ever happen for outline fonts such as TrueType and Adobe Type 1. We
call the FreeType library to generate the glyph on the fly, which speeds
up font load times as we only ever generate glyphs for the images we wish
to display (important for Unicode fonts that may have up to 65,535
characters!)
{secret}
****************************************************************************/
void _MGL_generateGlyph(
    int ch,
    bitmap_font *font)
{
    ushort              idx;
    TT_Raster_Map       rasterMap;
    TT_Glyph            glyph;
    tt_font_lib         *lib = TTFONTLIB(font->lib);
    TT_Glyph_Metrics    glyphMetrics;
    int                 width, height;
    uchar               *buf = _MGL_buf;

    /* fill in information for this glyph then rasterize it */
    idx = TT_Char_Index(lib->charMap,(ushort)(ch+font->startGlyph));
    if (idx == 0) {
        int i;
        i = '.' - font->startGlyph;
        if ((i >= font->numGlyphs) ||
           (ch < font->startGlyph)) {
            /* If the character '.' does not exist, create a null
             * definition
             */
            i = BIT_MISSINGSYMBOL(font);
            font->iwidth[i] = 0;
            font->width[i] = 0;
            font->offset[i] = 0;
            font->loc[i] = 0;
            font->charAscent[i] = 0;
            font->charDescent[i] = 0;
            font->valid[i] = true;
            }

        /* If the missing character symbol '.' is not valid, create it */
        if (!font->valid[i])
            _MGL_generateGlyph(i, font);

        /* Set the missing character info */
        font->width[ch] = font->width[i];
        font->iwidth[ch] = font->iwidth[i];
        font->offset[ch] = font->offset[i];
        font->loc[ch] = font->loc[i];
        font->charAscent[ch] = font->charAscent[i];
        font->charDescent[ch] = font->charDescent[i];
        font->valid[ch] = true;

        /* This second definition is created for characters that are past
         * the end of the available symbols.
         */
        font->width[BIT_MISSINGSYMBOL(font)] = font->width[i];
        font->iwidth[BIT_MISSINGSYMBOL(font)] = font->iwidth[i];
        font->offset[BIT_MISSINGSYMBOL(font)] = font->offset[i];
        font->loc[BIT_MISSINGSYMBOL(font)] = font->loc[i];
        font->charAscent[BIT_MISSINGSYMBOL(font)] = font->charAscent[i];
        font->charDescent[BIT_MISSINGSYMBOL(font)] = font->charDescent[i];
        font->valid[BIT_MISSINGSYMBOL(font)] = true;
        }
    else {
        if (!font->valid[ch]) {
            int glyphsize;

            /* Load the glyph */
            TT_New_Glyph(lib->face,&glyph);
            if (TT_Load_Glyph(font->instance,glyph, idx, TTLOAD_DEFAULT))
                MGL_fatalError("Failure in FreeType library!");

            /* Get this glyph's information (metrics) */
            TT_Get_Glyph_Metrics(glyph, &glyphMetrics);
            width = (glyphMetrics.bbox.xMax / 64) - (glyphMetrics.bbox.xMin / 64) + 1;
            height = (glyphMetrics.bbox.yMax / 64) - (glyphMetrics.bbox.yMin / 64) + 1;

            /* Fill in the tables with the info for this glyph */
            font->width[ch] = glyphMetrics.advance/64;
            font->iwidth[ch] = width;
            font->charAscent[ch] = glyphMetrics.bearingY/64;
            font->charDescent[ch] = (glyphMetrics.bearingY/64) - height;

            /* check to see if we need to reallocate the size of the bitmap
             * buffer, and if so do it in 4k chunks
             */
            glyphsize = ((width + 7) / 8) * font->fontHeight;
            if (font->antialiased)
                glyphsize *= 4;
            while (font->bitmapBufferSize < (font->bitmapBufferUsed + glyphsize)) {
                /* Allocate one more 4k chunk to the buffer */
                font->bitmapBufferSize += 4096;
                font->def = realloc(font->def, font->bitmapBufferSize);
                if (font->def == NULL)
                    MGL_fatalError("Out of memory in bitfont.c!");
                memset((void *) &((uchar *)font->def)[font->bitmapBufferSize-4096], 0, 4096);
                }
            font->offset[ch] = font->bitmapBufferUsed;
            font->bitmapBufferUsed += glyphsize;
            font->loc[ch] = glyphMetrics.bearingX/64;
            rasterMap.rows = font->fontHeight;
            rasterMap.width = font->iwidth[ch];
            rasterMap.flow = TT_Flow_Down;
            if (font->antialiased) {
                int w = (width + 7) / 8;
                int i, k, j;
                uchar * map = (&font->def[font->offset[ch]]);
                uchar * map2 = map + (glyphsize / 4);
                uchar * map3 = map2 + (glyphsize / 4);
                uchar * map4 = map3 + (glyphsize / 4);
                uchar * buf2 = buf;

                // TODO: check to see if the MGL_buf is large enough to hold the data
                rasterMap.cols = (font->iwidth[ch]+7) & -8;
                rasterMap.size = rasterMap.cols * font->fontHeight;
                rasterMap.bitmap = (void*)buf;
                memset(rasterMap.bitmap, 0, rasterMap.size);
                TT_Get_Glyph_Pixmap(glyph, &rasterMap, -font->loc[ch]*64, -font->descent*64);

                /* Now we convert the pixmap into a series of bitmaps */
                for (i = 0; i < font->fontHeight; i++) {
                    for (j = 0; j < w; j++) {
                        uchar b1, b2, b3, b4;
                        b1 = 0;
                        b2 = 0;
                        b3 = 0;
                        b4 = 0;
                        for (k = 0; k < 8; k++) {
                            switch (*buf2) {
                                case 4: b1 |= (0x80 >> k); break;
                                case 3: b2 |= (0x80 >> k); break;
                                case 2: b3 |= (0x80 >> k); break;
                                case 1: b4 |= (0x80 >> k); break;
                                }
                            buf2++;
                            }
                        *map = b1;
                        *map2 = b2;
                        *map3 = b3;
                        *map4 = b4;
                        map++;
                        map2++;
                        map3++;
                        map4++;
                        }
                   buf += rasterMap.cols;
                   buf2 = buf;
                   }
                }
            else {
                rasterMap.cols = (font->iwidth[ch] + 7) / 8;
                rasterMap.size = rasterMap.cols * font->fontHeight;
                rasterMap.bitmap = (void*)(&font->def[font->offset[ch]]);
                memset(rasterMap.bitmap, 0, rasterMap.size);
                TT_Get_Glyph_Bitmap(glyph, &rasterMap, -font->loc[ch]*64, -font->descent*64);
                }

            /* Indicate that the glyph is now valid */
            font->valid[ch] = true;

            /* Compute max values for this font */
            if (width > font->maxWidth)
                font->maxWidth = width;

            /* Free the glyph object when we are done with it */
            TT_Done_Glyph(glyph);
            }
        }
}

/****************************************************************************
PARAMETERS:
x   - X coordinate to draw at
y   - Y coordinate to draw at
str - String to draw

REMARKS:
Draws the specified string at the current position (CP) in the current
drawing color, write mode and justification using the current bitmap font.
We rely upon the underlying MGL_putMonoImage routine to handling all of
the clipping and to trivially reject bitmaps for us.
{secret}
****************************************************************************/
void _MGL_drawStrBitmap(
    int x,
    int y,
    const char *str)
{
    int             i,width,height,ascent,descent;
    int             fontAscent,bitmapHeight,ch;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;
    bitmap_font     *bitFont = BITFONT(font);
    ushort          *enc = ts.useEncoding ? ts.encoding : NULL;

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
    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x - width/2 :
                    x - width));
            while (*str) {
                ch = (uchar)*str++;
                if (ch > 127 && enc)
                    ch = enc[ch - 128];
                i = drawFontGlyphRight(bitFont, ch, i, y);
                }
            break;
        case MGL_UP_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y + width/2 :
                    y + width));
            while (*str) {
                ch = (uchar)*str++;
                if (ch > 127 && enc)
                    ch = enc[ch - 128];
                i = drawFontGlyphUp(bitFont, ch, x, i);
                }
            break;
        case MGL_DOWN_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y - width/2 :
                    y - width));
            while (*str) {
                ch = (uchar)*str++;
                if (ch > 127 && enc)
                    ch = enc[ch - 128];
                i = drawFontGlyphDown(bitFont, ch, x, i);
                }
            break;
        case MGL_LEFT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x + width/2 :
                    x + width));
            while (*str) {
                ch = (uchar)*str++;
                if (ch > 127 && enc)
                    ch = enc[ch - 128];
                i = drawFontGlyphLeft(bitFont, ch, i, y);
                }
            break;
        }
}

/****************************************************************************
PARAMETERS:
x   - X coordinate to draw at
y   - Y coordinate to draw at
str - String to draw

REMARKS:
Draws the specified string at the current position (CP) in the current
drawing color, write mode and justification using the current bitmap font.
We rely upon the underlying MGL_putMonoImage routine to handling all of
the clipping and to trivially reject bitmaps for us.
{secret}
****************************************************************************/
void _MGL_drawStrBitmap_W(
    int x,
    int y,
    const wchar_t *str)
{
    int             i,width,height,ascent,descent;
    int             fontAscent,bitmapHeight,ch;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;
    bitmap_font     *bitFont = BITFONT(font);

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
    width = MGL_textWidth_W(str);
    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x - width/2 :
                    x - width));
            while (*str) {
                ch = *str++;
                i = drawFontGlyphRight(bitFont, ch, i, y);
                }
            break;
        case MGL_UP_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y + width/2 :
                    y + width));
            while (*str) {
                ch = *str++;
                i = drawFontGlyphUp(bitFont, ch, x, i);
                }
            break;
        case MGL_DOWN_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y - width/2 :
                    y - width));
            while (*str) {
                ch = *str++;
                i = drawFontGlyphDown(bitFont, ch, x, i);
                }
            break;
        case MGL_LEFT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x + width/2 :
                    x + width));
            while (*str) {
                ch = *str++;
                i = drawFontGlyphLeft(bitFont, ch, i, y);
                }
            break;
        }
}

