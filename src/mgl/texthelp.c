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
* Description:  Helper functions for text output common to both standard
*               and wide character strings.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
ch  - Character to measure

RETURNS:
Width of the character in pixels (will depend on currently active font)

REMARKS:
Return the width of the specified character, given the currently active
font and attribute settings.
{secret}
****************************************************************************/
int __MGL_charWidth(
    wchar_t ch)
{
    font_t  *font = DC.a.ts.font;
    int     glyph;

    if (font == NULL) return 0;
    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_FIXEDFONT:
            return font->fontWidth;
        case MGL_VECTORFONT:
            if (VEC_ISMISSING(font,ch))
                return (DC.a.ts.szNumerx *
                        VECFONT(font)->width[VEC_MISSINGSYMBOL(font)])
                        / DC.a.ts.szDenomx + DC.a.ts.spaceExtra;
            else
                return (DC.a.ts.szNumerx *
                        VECFONT(font)->width[ch])
                        / DC.a.ts.szDenomx + DC.a.ts.spaceExtra;
        case MGL_PROPFONT:
            glyph = ch - BITFONT(font)->startGlyph;
            if (glyph < 0)
                return 0;
            if ((glyph < BITFONT(font)->numGlyphs) && !BITFONT(font)->valid[glyph])
                _MGL_generateGlyph(glyph,BITFONT(font));
            if ((glyph >= BITFONT(font)->numGlyphs) ||
                (ch < BITFONT(font)->startGlyph) ||
                (BIT_ISMISSING(font,glyph)))
                return BITFONT(font)->width[BIT_MISSINGSYMBOL(font)] + DC.a.ts.spaceExtra;
            else
                return BITFONT(font)->width[glyph] + DC.a.ts.spaceExtra;
        default:
            return 0;
        }
}

/****************************************************************************
PARAMETERS:
x       - x coordinate string would be drawn at
y       - y coordinate string would be drawn at
width   - Width of the string
bounds  - Place to store the computed bounds

REMARKS:
Internal function to find the bounds for a text string, given an already
computed string width.
{secret}
****************************************************************************/
void __MGL_findTextBounds(
    int x,
    int y,
    int width,
    rect_t *bounds)
{
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;
    int             newx,newy,height;

    /* Determine dimensions and offset of bounding rectangle */
    height = (font->ascent - font->descent + 1);
    newx =  (ts.horizJust == MGL_LEFT_TEXT ?    0 :
            (ts.horizJust == MGL_CENTER_TEXT ?  width/2 :
                                                width));
    newy =  (ts.vertJust == MGL_TOP_TEXT ?      0 :
            (ts.vertJust == MGL_CENTER_TEXT ?   (height-1)/2 :
            (ts.vertJust == MGL_BOTTOM_TEXT ?   height-1 :
                                                font->ascent)));
    if ((font->fontType & MGL_FONTTYPEMASK) == MGL_VECTORFONT)
        newy = (ts.szNumery * newy) / ts.szDenomy;

    /* Build the bounding rectangle depending on text direction */
    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            bounds->left = x - newx;
            bounds->right = bounds->left + width;
            bounds->top = y - newy;
            bounds->bottom = bounds->top + height;
            break;
        case MGL_UP_DIR:
            bounds->left = x - newy;
            bounds->right = bounds->left + height;
            bounds->bottom = y + newx + 1;
            bounds->top = bounds->bottom - width;
            break;
        case MGL_LEFT_DIR:
            bounds->right = x + newx + 1;
            bounds->left = bounds->right - width;
            bounds->bottom = y + newy + 1;
            bounds->top = bounds->bottom - height;
            break;
        case MGL_DOWN_DIR:
            bounds->right = x + newy + 1;
            bounds->left = bounds->right - height;
            bounds->top = y - newx;
            bounds->bottom = bounds->top + width;
            break;
        }
}

/****************************************************************************
PARAMETERS:
ch      - Character to measure
metrics - Place to store the resulting metrics

REMARKS:
Internal function to compute the character metrics that works for both
regular and wide characters.
{secret}
****************************************************************************/
void __MGL_getCharMetrics(
    wchar_t ch,
    metrics_t *m)
{
    int             glyph,offset,ascent,descent,fontwidth,kern,dx,dy;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;
    vector          *vec;

    if (font == NULL) return;

    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_VECTORFONT:
            if ((offset = VECFONT(font)->offset[(uchar)ch]) == -1)
                offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
            vec = (vector*)&VECFONT(font)->def[offset];

            /* Perform the sequence of operations to draw the character */
            ascent = descent = fontwidth = kern = 0;
            while (VEC_OP1(*vec)) {
                dx = VEC_DX(*vec);
                dy = VEC_DY(*vec);
                ascent = MAX(ascent,dy);
                descent = MIN(descent,dy);
                fontwidth = MAX(fontwidth,dx);
                kern = MIN(kern,dx);
                vec++;
                }

            /* Store the computed values */
            m->width = (ts.szNumerx *
                        VECFONT(font)->width[(uchar)ch])
                        / ts.szDenomx;
            m->fontWidth = (ts.szNumerx * (fontwidth+1)) / ts.szDenomx;
            m->ascent = (ts.szNumery * ascent) / ts.szDenomy;
            m->descent = (ts.szNumery * descent) / ts.szDenomy;
            m->leading = (ts.szNumery * font->leading) / ts.szDenomy;
            m->fontHeight = m->ascent - m->descent + 1;
            m->kern = (ts.szNumerx * kern) / ts.szDenomx;
            break;
        case MGL_PROPFONT:
        case MGL_FIXEDFONT:
            glyph = ch - BITFONT(font)->startGlyph;
            m->width = BITFONT(font)->width[glyph];
            m->fontWidth = BITFONT(font)->iwidth[glyph];
            m->ascent = BITFONT(font)->charAscent[glyph];
            m->descent = BITFONT(font)->charDescent[glyph];
            m->leading = font->leading;
            m->fontHeight = m->ascent - m->descent + 1;
            m->kern = -BITFONT(font)->loc[glyph];
            break;
        default:
            m->width = font->maxWidth;
            m->fontWidth = font->fontWidth;
            m->ascent = font->ascent;
            m->descent = font->descent;
            m->leading = font->leading;
            m->fontHeight = m->ascent - m->descent + 1;
            m->kern = font->maxKern;
            break;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the location to begin drawing an underscore for the font.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to be passed to MGL_drawStrXY
y   - y coordinate to be passed to MGL_drawStrXY
str - String to measure

REMARKS:
This function takes an (x,y) location that would normally be used to draw
a string with MGL_drawStrXY, and adjusts the coordinates to begin at the
under score location for the current font, in the current drawing attributes.
Thus the entire character string can be underlined by drawing a line
starting at the computed underscore location and extending for MGL_textWidth
pixels in length.

SEE ALSO:
MGL_underScoreLocation_W, MGL_drawStrXY, MGL_textWidth
{secret}
****************************************************************************/
void __MGL_findUnderScoreLocation(
    int *x,
    int *y,
    int width)
{
    int             newx,newy,adjust;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;

    if (font == NULL) return;
    newx =  (ts.horizJust == MGL_LEFT_TEXT ?    0 :
            (ts.horizJust == MGL_CENTER_TEXT ?  width/2 :
                                                width));
    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_FIXEDFONT:
        case MGL_PROPFONT:
            newy =  (ts.vertJust == MGL_TOP_TEXT ?      font->ascent+1 :
                    (ts.vertJust == MGL_CENTER_TEXT ?   (font->ascent+font->descent+1)/2 + 1 :
                    (ts.vertJust == MGL_BOTTOM_TEXT ?   (font->descent + 1) :
                                                        1)));
            break;
        default: /* case MGL_VECTORFONT: */
            newy =  (ts.vertJust == MGL_TOP_TEXT ?      font->ascent+1 :
                    (ts.vertJust == MGL_CENTER_TEXT ?   (font->ascent+font->descent+1)/2 + 1 :
                    (ts.vertJust == MGL_BOTTOM_TEXT ?   (font->descent + 1) :
                                                        1)));
            newy = (ts.szNumery * newy) / ts.szDenomy;
            break;
        }

    adjust = font->fontHeight / 10;
    if (adjust < 1)
        adjust = 1;

    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            *x -= newx; *y += newy + adjust;
            break;
        case MGL_UP_DIR:
            *x += newy + adjust;    *y += newx;
            break;
        case MGL_LEFT_DIR:
            *x += newx; *y -= newy + adjust;
            break;
        case MGL_DOWN_DIR:
            *x -= newy + adjust;    *y -= newx;
            break;
        }
}
