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
* Description:  Wide character string text output and control routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Returns the width of a wide character in pixels.

HEADER:
mgraph.h

PARAMETERS:
ch  - Wide character to measure

RETURNS:
Width of the wide character in pixels (will depend on currently active font)

REMARKS:
Return the width of the specified wide character, given the currently active
font and attribute settings. This function is the same as MGL_charWidth,
but provides support for Unicode wide characters (for far-east languages).

SEE ALSO:
MGL_textWidth_W, MGL_textHeight, MGL_useFont, MGL_charWidth
****************************************************************************/
int MGLAPI MGL_charWidth_W(
    wchar_t ch)
{
    return __MGL_charWidth(ch);
}

/****************************************************************************
DESCRIPTION:
Returns the width of the wide character string in pixels.

HEADER:
mgraph.h

PARAMETERS:
str - Wide character string to measure

RETURNS:
Width of the wide character string in pixels

REMARKS:
Returns the width of the specified character string using the dimensions of
the currently active font in pixels. This includes any scaling
transformations that are applied to the font and will be as accurate as
possible at the resolution of the display device.

This function is the same as MGL_textWidth, but provides support for
Unicode wide characters (for far-east languages).

Note: Wide character fonts are only supported for bitmap and TrueType fonts.
      Vector fonts are not supported via this function.

SEE ALSO:
MGL_textWidth, MGL_textHeight, MGL_drawStr, MGL_drawStr_W,
MGL_getCharMetrics, MGL_getCharMetrics_W, MGL_getFontMetrics
****************************************************************************/
int MGLAPI MGL_textWidth_W(
    const wchar_t *str)
{
    int width = 0;

    if (DC.a.ts.font == NULL) return 0;

    if (str) {
        switch (DC.a.ts.font->fontType & MGL_FONTTYPEMASK) {
            case MGL_FIXEDFONT:
                while (*str++)
                    width += DC.a.ts.font->fontWidth;
                break;
            case MGL_PROPFONT:
                while (*str)
                    width += __MGL_charWidth(*str++);
                break;
            default:
                return 0;
            }
        }
    return width;
}

/****************************************************************************
DESCRIPTION:
Compute the bounding box for a wide character string.

HEADER:
mgraph.h

PARAMETERS:
x       - x coordinate string would be drawn at
y       - y coordinate string would be drawn at
str     - Wide character string to measure
bounds  - Place to store the computed bounds

REMARKS:
This function computes the bounding box that fits tightly around a text
string drawn at a specified location on the current device context. This
routine correctly computes the bounding rectangle for the string given the
current text justification, size and direction settings.

This function is the same as MGL_textBounds, but provides support for
Unicode wide characters (for far-east languages).

Note: Wide character fonts are only supported for bitmap and TrueType fonts.
      Vector fonts are not supported via this function.

SEE ALSO:
MGL_textBounds, MGL_textHeight, MGL_textWidth, MGL_textWidth_W
****************************************************************************/
void MGLAPI MGL_textBounds_W(
    int x,
    int y,
    const wchar_t *str,
    rect_t *bounds)
{
    if (!str || !DC.a.ts.font)
        return;
    __MGL_findTextBounds(x,y,MGL_textWidth_W(str),bounds);
}

/****************************************************************************
DESCRIPTION:
Computes the character metrics for a specific wide character.

HEADER:
mgraph.h

PARAMETERS:
ch      - Wide character to measure
metrics - Place to store the resulting metrics

REMARKS:
This function computes the character metrics for a specific wide character.
The character metrics define specific characters width, height, ascent,
descent and other values. These values can then be used to correctly
position the character with pixel precise positioning. This function is the
same as MGL_getCharMetrics, but provides support for Unicode wide
characters (for far-east languages).

All values are defined in pixels and will be as accurate as possible given
the current fonts scaling factor (only vector fonts can be scaled).

SEE ALSO:
MGL_getFontMetrics, MGL_getCharMetrics
****************************************************************************/
void MGLAPI MGL_getCharMetrics_W(
    wchar_t ch,
    metrics_t *metrics)
{
    __MGL_getCharMetrics(ch,metrics);
}

/****************************************************************************
DESCRIPTION:
Returns the location to begin drawing an underscore for the font.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to be passed to MGL_drawStrXY
y   - y coordinate to be passed to MGL_drawStrXY
str - Wide character string to measure

REMARKS:
This function takes an (x,y) location that would normally be used to draw
a string with MGL_drawStrXY, and adjusts the coordinates to begin at the
under score location for the current font, in the current drawing attributes.
Thus the entire character string can be underlined by drawing a line
starting at the computed underscore location and extending for MGL_textWidth
pixels in length.

This function is the same as MGL_underScoreLocation, but provides support for
Unicode wide characters (for far-east languages).

Note: Wide character fonts are only supported for bitmap and TrueType fonts.
      Vector fonts are not supported via this function.

SEE ALSO:
MGL_underScoreLocation, MGL_drawStrXY, MGL_textWidth
****************************************************************************/
void MGLAPI MGL_underScoreLocation_W(
    int *x,
    int *y,
    const wchar_t *str)
{
    __MGL_findUnderScoreLocation(x,y,MGL_textWidth_W(str));
}

/****************************************************************************
DESCRIPTION:
Draws a wide character string at the specified position.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to begin rasterizing the string at
y   - y coordinate to begin rasterizing the string at
str - Wide character string to display

REMARKS:
Draws a string at the specified (x,y) position in the current drawing color,
write mode, font, text direction and justification.

This function is the same as MGL_drawStrXY, but provides support for
Unicode wide characters (for far-east languages).

Note: Wide character fonts are only supported for bitmap and TrueType fonts.
      Vector fonts are not supported via this function.

SEE ALSO:
MGL_drawStrXY, MGL_drawStr, MGL_drawStr_W, MGL_textHeight, MGL_textWidth,
MGL_useFont
****************************************************************************/
void MGLAPI MGL_drawStrXY_W(
    int x,
    int y,
    const wchar_t *str)
{
    if (DC.a.ts.font == NULL) return;

    BEGIN_VISIBLE_CLIP_LIST(&DC);
    switch (DC.a.ts.font->fontType & MGL_FONTTYPEMASK) {
        case MGL_FIXEDFONT:
        case MGL_PROPFONT:
            _MGL_drawStrBitmap_W(x+DC.viewPort.left,y+DC.viewPort.top,str);
            break;
        }
    END_VISIBLE_CLIP_LIST(&DC);
}

/****************************************************************************
DESCRIPTION:
Draws a wide character string at the current position.

HEADER:
mgraph.h

PARAMETERS:
str - Wide character string to display

REMARKS:
Draws a string at the current position (CP) in the current drawing color,
write mode, font, text direction and justification. The CP is moved so that
drawing will begin directly after the end of the string, only if the
horizontal justification is set to MGL_LEFT_TEXT, otherwise the CP is not
moved.

This function is the same as MGL_drawStr, but provides support for
Unicode wide characters (for far-east languages).

Note: Wide character fonts are only supported for bitmap and TrueType fonts.
      Vector fonts are not supported via this function.

SEE ALSO:
MGL_drawStrXY, MGL_drawStrXY_W, MGL_drawStr, MGL_textHeight, MGL_textWidth,
MGL_useFont
****************************************************************************/
void MGLAPI MGL_drawStr_W(
    const wchar_t *str)
{
    int width;

    if (DC.a.ts.font == NULL) return;
    MGL_drawStrXY_W(DC.a.CP.x,DC.a.CP.y,str);
    if (DC.a.ts.horizJust == MGL_LEFT_TEXT) {
        width = MGL_textWidth_W(str);
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

