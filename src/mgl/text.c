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
* Description:  Text output and control routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Returns the height of the current font in pixels.

HEADER:
mgraph.h

RETURNS:
Height of the current font in pixels

REMARKS:
Returns the height of the currently active font in pixels. This includes any scaling
transformations that are applied to the font and will be as accurate as possible at the
resolution of the display device.

SEE ALSO:
MGL_textWidth, MGL_drawStr, MGL_getCharMetrics, MGL_getFontMetrics
****************************************************************************/
int MGLAPI MGL_textHeight(void)
{
    font_t *font = DC.a.ts.font;

    if (font == NULL) return 0;

    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_VECTORFONT:
            return (DC.a.ts.szNumery *
                    (font->ascent - font->descent + 1 + font->leading))
                    / DC.a.ts.szDenomy;
        case MGL_FIXEDFONT:
        case MGL_PROPFONT:
            return font->ascent - font->descent + 1 + font->leading;
        default:
            return 0;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the width of a character in pixels.

HEADER:
mgraph.h

PARAMETERS:
ch  - Character to measure

RETURNS:
Width of the character in pixels (will depend on currently active font)

REMARKS:
Return the width of the specified character, given the currently active font and
attribute settings.

SEE ALSO:
MGL_textWidth, MGL_textHeight, MGL_useFont, MGL_charWidth_W
****************************************************************************/
int MGLAPI MGL_charWidth(
    char ch)
{
    ushort *enc = (DC.a.ts.useEncoding) ? DC.a.ts.encoding : NULL;
    ushort wc = ch;
    if (wc > 127 && enc)
        wc = enc[wc - 128];
    return __MGL_charWidth(wc);
}

/****************************************************************************
DESCRIPTION:
Returns the width of the character string in pixels.

HEADER:
mgraph.h

PARAMETERS:
str - Character string to measure

RETURNS:
Width of the character string in pixels

REMARKS:
Returns the width of the specified character string using the dimensions of
the currently active font in pixels. This includes any scaling
transformations that are applied to the font and will be as accurate as
possible at the resolution of the display device.

SEE ALSO:
MGL_textWidth_W, MGL_textHeight, MGL_drawStr, MGL_drawStr_W,
MGL_getCharMetrics, MGL_getCharMetrics_W, MGL_getFontMetrics
****************************************************************************/
int MGLAPI MGL_textWidth(
    const char *str)
{
    int    width = 0;
    ushort *enc = (DC.a.ts.useEncoding) ? DC.a.ts.encoding : NULL;
    ushort wc;

    if (DC.a.ts.font == NULL) return 0;

    if (str) {
        switch (DC.a.ts.font->fontType & MGL_FONTTYPEMASK) {
            case MGL_FIXEDFONT:
                return strlen(str) * DC.a.ts.font->fontWidth;
            case MGL_VECTORFONT:
            case MGL_PROPFONT:
                while (*str) {
                    wc = (uchar)*str++;
                    if (wc > 127 && enc)
                        wc = enc[wc - 128];
                    width += __MGL_charWidth(wc);
                    }
                break;
            default:
                return 0;
            }
        }
    return width;
}

/****************************************************************************
DESCRIPTION:
Compute the bounding box for a text string.

HEADER:
mgraph.h

PARAMETERS:
x       - x coordinate string would be drawn at
y       - y coordinate string would be drawn at
str     - String to measure
bounds  - Place to store the computed bounds

REMARKS:
This function computes the bounding box that fits tightly around a text
string drawn at a specified location on the current device context. This
routine correctly computes the bounding rectangle for the string given the
current text justification, size and direction settings.

SEE ALSO:
MGL_textBounds_W, MGL_textHeight, MGL_textWidth, MGL_textWidth_W
****************************************************************************/
void MGLAPI MGL_textBounds(
    int x,
    int y,
    const char *str,
    rect_t *bounds)
{
    if (!str || !DC.a.ts.font)
        return;
    __MGL_findTextBounds(x,y,MGL_textWidth(str),bounds);
}

/****************************************************************************
DESCRIPTION:
Returns the maximum character width for current font.

HEADER:
mgraph.h

RETURNS:
Maximum character width for current font.

REMARKS:
Returns the maximum character width for the currently active font. You can use this
routine to quickly determine if a character will possibly overlap something else on
the device surface.

SEE ALSO:
MGL_getCharMetrics, MGL_getFontMetrics, MGL_textHeight, MGL_textWidth,
MGL_charWidth
****************************************************************************/
int MGLAPI MGL_maxCharWidth(void)
{
    if (DC.a.ts.font == NULL) return 0;

    switch (DC.a.ts.font->fontType & MGL_FONTTYPEMASK) {
        case MGL_PROPFONT:
        case MGL_FIXEDFONT:
            return DC.a.ts.font->maxWidth;
        case MGL_VECTORFONT:
            return (DC.a.ts.szNumerx * DC.a.ts.font->maxWidth)
                    / DC.a.ts.szDenomx + DC.a.ts.spaceExtra;
        default:
            return 0;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the currently active font metrics.

HEADER:
mgraph.h

PARAMETERS:
metrics - Place to store the font metrics

REMARKS:
This function computes the font metrics for the current font. The metrics are
computed in pixels and will be as accurate as possible given the current font's
scaling factor (only vector fonts can be scaled however).

SEE ALSO:
MGL_getCharMetrics
****************************************************************************/
void MGLAPI MGL_getFontMetrics(
    metrics_t *m)
{
    font_t  *font = DC.a.ts.font;

    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_VECTORFONT: {
            text_settings_t ts = DC.a.ts;
            m->width = (ts.szNumerx * font->maxWidth) / ts.szDenomx;
            m->fontWidth = m->width;
            m->fontHeight = (ts.szNumerx * font->fontHeight) / ts.szDenomx;
            m->ascent = (ts.szNumery * font->ascent) / ts.szDenomy;
            m->descent = (ts.szNumery * font->descent) / ts.szDenomy;
            m->leading = (ts.szNumery * font->leading) / ts.szDenomy;
            m->kern = (ts.szNumerx * font->maxKern) / ts.szDenomx;
            break;
            }
        default:
            m->width = font->maxWidth;
            m->fontWidth = font->fontWidth;
            m->fontHeight = font->fontHeight;
            m->ascent = font->ascent;
            m->descent = font->descent;
            m->leading = font->leading;
            m->kern = font->maxKern;
            break;
        }
}

/****************************************************************************
DESCRIPTION:
Computes the character metrics for a specific character.

HEADER:
mgraph.h

PARAMETERS:
ch      - Character to measure
metrics - Place to store the resulting metrics

REMARKS:
This function computes the character metrics for a specific character. The
character metrics define specific characters width, height, ascent, descent
and other values. These values can then be used to correctly position the
character with pixel precise positioning.

All values are defined in pixels and will be as accurate as possible given
the current fonts scaling factor (only vector fonts can be scaled).

SEE ALSO:
MGL_getFontMetrics, MGL_getCharMetrics_W
****************************************************************************/
void MGLAPI MGL_getCharMetrics(
    char ch,
    metrics_t *metrics)
{
    ushort *enc = (DC.a.ts.useEncoding) ? DC.a.ts.encoding : NULL;
    ushort wc = ch;
    if (wc > 127 && enc)
        wc = enc[wc - 128];
    __MGL_getCharMetrics(wc,metrics);
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
****************************************************************************/
void MGLAPI MGL_underScoreLocation(
    int *x,
    int *y,
    const char *str)
{
    __MGL_findUnderScoreLocation(x,y,MGL_textWidth(str));
}

/****************************************************************************
DESCRIPTION:
Draws a text string at the specified position.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to begin rasterizing the string at
y   - y coordinate to begin rasterizing the string at
str - String to display

REMARKS:
Draws a string at the specified (x,y) position in the current drawing color, write
mode, font, text direction and justification.

SEE ALSO:
MGL_drawStrXY_W, MGL_drawStr, MGL_drawStr_W, MGL_textHeight, MGL_textWidth,
MGL_useFont
****************************************************************************/
void MGLAPI MGL_drawStrXY(
    int x,
    int y,
    const char *str)
{
    if (DC.a.ts.font == NULL) return;

    BEGIN_VISIBLE_CLIP_LIST(&DC);
    switch (DC.a.ts.font->fontType & MGL_FONTTYPEMASK) {
        case MGL_VECTORFONT:
            _MGL_drawStrVec(x+DC.viewPort.left,y+DC.viewPort.top,str);
            break;
        case MGL_FIXEDFONT:
        case MGL_PROPFONT:
            _MGL_drawStrBitmap(x+DC.viewPort.left,y+DC.viewPort.top,str);
            break;
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Draws a text string at the current position.

HEADER:
mgraph.h

PARAMETERS:
str - String to display

REMARKS:
Draws a string at the current position (CP) in the current drawing color, write mode,
font, text direction and justification. The CP is moved so that drawing will begin
directly after the end of the string, only if the horizontal justification is set to
MGL_LEFT_TEXT, otherwise the CP is not moved.

SEE ALSO:
MGL_drawStrXY, MGL_drawStrXY_W, MGL_drawStr_W, MGL_textHeight, MGL_textWidth,
MGL_useFont
****************************************************************************/
void MGLAPI MGL_drawStr(
    const char *str)
{
    int width;

    if (DC.a.ts.font == NULL) return;
    MGL_drawStrXY(DC.a.CP.x,DC.a.CP.y,str);
    if (DC.a.ts.horizJust == MGL_LEFT_TEXT) {
        width = MGL_textWidth(str);
        switch (DC.a.ts.dir) {
            case MGL_RIGHT_DIR:
                DC.a.CP.x += width;
                break;
            case MGL_UP_DIR:
                DC.a.CP.y -= width;
                break;
            case MGL_LEFT_DIR:
                DC.a.CP.x -= width;
                break;
            case MGL_DOWN_DIR:
                DC.a.CP.y += width;
                break;
            }
        }
}
