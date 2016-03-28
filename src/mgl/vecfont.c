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
* Description:  Vector font text output routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Generates the commands to draw a vectored font.

HEADER:
mgraph.h

PARAMETERS:
x       - x coordinate to start drawing text at
y       - y coordinate to start drawing text at
str     - Character string to draw
move    - Routine to call to perform a move operation
draw    - Routine to call to perform a draw operation

RETURNS:
True if the string is correctly rasterized, false if font is not a vector font.

REMARKS:
This function calls a set of user supplied routines to rasterize the characters in a
vector font. This allows the vector fonts to be drawn in 2D or 3D floating point
coordinate systems by transforming each of the coordinates required to draw each
character by any arbitrary transformation, or in any coordinate system that the users
desires.

The move routine is called to move the cursor to a new location, and the draw
routine is used to perform a draw operation from the current location to the
specified location. Each character in the vector font is started with a move
operation.

Note that the coordinates passed to the move and draw routines will be offset from
the point (x,y), where the point (x,y) is the origin of the first character (i.e. it lies on
its baseline). Note also that the coordinates will be relative to the origin with the
origin at the lower left corner of each character (i.e. inverse of normal device
coordinate y-axis values).

This routine does not honor the standard scaling factors, but simply draws the
characters with a size of (1,1,1,1) (because scaling will be done by the user supplied
move and draw routines).

If the passed font is not a valid vector font, this routine returns false.

SEE ALSO:
MGL_drawStr, MGL_useFont
****************************************************************************/
ibool MGLAPI MGL_vecFontEngine(
    int x,
    int y,
    const char *str,
    void (MGLAPIP move)(int x,int y),
    void (MGLAPIP draw)(int x,int y))
{
    int             offset,dx,dy,missing;
    vector          *vec;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;

    /* Ensure that the font is a valid vector font */
    if ((font->fontType & MGL_FONTTYPEMASK) != MGL_VECTORFONT)
        return false;

    /* Set scaling to (1,1,1,1) */
    DC.a.ts.szNumerx = DC.a.ts.szNumery = 1;
    DC.a.ts.szDenomx = DC.a.ts.szDenomy = 1;

    /* Adjust the first characters origin depending on the vertical
     * justification setting.
     */
    y -= (ts.vertJust == MGL_CENTER_TEXT ? (font->ascent + font->descent)/2 :
         ((ts.vertJust == MGL_TOP_TEXT ? font->ascent :
          (ts.vertJust == MGL_BOTTOM_TEXT ? font->descent : 0))));
    x -= (ts.horizJust == MGL_CENTER_TEXT ? MGL_textWidth(str)/2 :
         (ts.horizJust == MGL_RIGHT_TEXT ? MGL_textWidth(str) : 0));

    while (*str) {
        missing = false;
        if ((offset = VECFONT(font)->offset[(uchar)*str]) == -1) {
            offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
            missing = true;
            }
        vec = (vector*)&VECFONT(font)->def[offset];

        /* Perform the sequence of operations to draw the character */
        while (VEC_OP1(*vec)) {
            dx = VEC_DX(*vec);
            dy = VEC_DY(*vec);
            if (VEC_OP2(*vec))
                move(x + dx,y + dy);
            else
                draw(x + dx,y + dy);
            vec++;
            }

        /* Move to the start of the next character */
        if (missing)
            x += VECFONT(font)->width[VEC_MISSINGSYMBOL(font)];
        else
            x += VECFONT(font)->width[(int)*str];
        str++;
        }

    /* Restore original text settings */
    DC.a.ts = ts;
    return true;
}

/****************************************************************************
PARAMETERS:
x,y     - Coordinate to start drawing text at
vec     - Vector font glyph to draw
z_nx    - X scale numerator
sz_ny   - Y scale numrtator
sz_dx   - X scale denominator
sz_dy   - Y scale denominator

REMARKS:
Routine to draw a single vector glyph using the unclipped device driver
functions in the MGL.
{secret}
****************************************************************************/
static void drawCharVec(
    int x,
    int y,
    vector *vec,
    int sz_nx,
    int sz_ny,
    int sz_dx,
    int sz_dy)
{
    int oldx,oldy,newx,newy;

    /* Perform the sequence of operations to draw the character */
    newx = newy = 0;
    while (VEC_OP1(*vec)) {
        oldx = newx;
        oldy = newy;
        newx = (sz_nx * VEC_DX(*vec)) / sz_dx;
        newy = (sz_ny * VEC_DY(*vec)) / sz_dy;
        if (!VEC_OP2(*vec))
            DC.r.DrawLineInt(x+oldx,y-oldy,x+newx,y-newy,!VEC_OP1(*(vec+1)));
        vec++;
        }
}

/****************************************************************************
PARAMETERS:
x,y     - Coordinate to start drawing text at
vec     - Vector font glyph to draw
z_nx    - X scale numerator
sz_ny   - Y scale numrtator
sz_dx   - X scale denominator
sz_dy   - Y scale denominator

REMARKS:
Routine to draw a single vector glyph slowly be calling the MGL_lineCoord
routine so that it will be correctly clipped and drawn with normal
attributes.
{secret}
****************************************************************************/
static void drawCharVecSlow(
    int x,
    int y,
    vector *vec,
    int sz_nx,
    int sz_ny,
    int sz_dx,
    int sz_dy)
{
    int oldx,oldy,newx,newy;

    /* Convert back to viewport coordinates */
    x -= DC.viewPort.left;
    y -= DC.viewPort.top;

    /* Perform the sequence of operations to draw the character */
    newx = newy = 0;
    while (VEC_OP1(*vec)) {
        oldx = newx;
        oldy = newy;
        newx = (sz_nx * VEC_DX(*vec)) / sz_dx;
        newy = (sz_ny * VEC_DY(*vec)) / sz_dy;
        if (!VEC_OP2(*vec))
            MGL_lineCoordExt(x+oldx,y-oldy,x+newx,y-newy,!VEC_OP1(*(vec+1)));
        vec++;
        }
}

/****************************************************************************
PARAMETERS:
vec - Pointer to vector glyph to rotate
dir - Direction rotation in

REMARKS:
Rotates the representation of the vector in the specified direction.
{secret}
****************************************************************************/
static void rotateVector(
    vector *vec,
    int dir)
{
    int dx,dy;

    if (dir == MGL_UP_DIR) {
        /* Perform an upwards rotation of 90 degrees */
        while (VEC_OP1(*vec)) {
            dx = -VEC_DY(*vec);         /* dx = -dy */
            dy = VEC_DX(*vec);          /* dy = dx  */
            *vec = REPLACE_VEC(*vec,dx,dy);
            vec++;
            }
        }
    else if (dir == MGL_LEFT_DIR) {
        /* Perform a leftwards rotation of 180 degrees */
        while (VEC_OP1(*vec)) {
            dx = -VEC_DX(*vec);         /* dx = -dx */
            dy = -VEC_DY(*vec);         /* dy = -dy */
            *vec = REPLACE_VEC(*vec,dx,dy);
            vec++;
            }
        }
    else {
        /* Perform a downwards rotation of 90 degrees */
        while (VEC_OP1(*vec)) {
            dx = VEC_DY(*vec);          /* dx = dy  */
            dy = -VEC_DX(*vec);         /* dy = -dx */
            *vec = REPLACE_VEC(*vec,dx,dy);
            vec++;
            }
        }
}

/****************************************************************************
PARAMETERS:
x   - X coordinate to draw at
y   - Y coordinate to draw at
str - String to draw

REMARKS:
Draws the specified string at the position (x,y) in the current drawing
color, write mode, text direction and justification using the current
vector font.

In order to draw the vector font in a different direction we rotate the
glyphs on the fly.
{secret}
****************************************************************************/
void _MGL_drawStrVec(
    int x,
    int y,
    const char *str)
{
    int             i,width,height,ascent,descent,offset,maxWidth;
    int             maxKern,fontAscent,sz_nx,sz_ny,sz_dx,sz_dy;
    vector          *vec;
    text_settings_t ts = DC.a.ts;
    font_t          *font = ts.font;
    ibool           useSlow;
    rect_t          charRect,d,clipRect;

    /* Compute the scaled ascent, descent, maxKern and maxWidth values */
    ascent = (ts.szNumery * font->ascent) / ts.szDenomy;
    descent = (ts.szNumery * font->descent) / ts.szDenomy;
    maxKern = (ts.szNumerx * font->maxKern) / ts.szDenomx;
    maxWidth = (ts.szNumerx * font->maxWidth) / ts.szDenomx;
    fontAscent = font->fontHeight + font->descent - 1;
    fontAscent = (ts.szNumery * fontAscent) / ts.szDenomy;

    /* If we are drawing with patterned lines, or with a wide pen, then
     * call the slow drawing routine, which works properly in these
     * cases. Ditto with complex clip region active, so the vectors are
     * properly clipped per clip list entry.
     */
    useSlow = (DC.clipRegionScreen || DC.a.penStyle != MGL_BITMAP_SOLID ||
               DC.a.penHeight != 0 || DC.a.penWidth != 0);

    /* Save the clipping rectangle in local variable */
    clipRect = DC.clipRectScreen;

    /* Determine the current scaling factors to use for scan converting
     * each character
     */
    if (ts.dir == MGL_LEFT_DIR || ts.dir == MGL_RIGHT_DIR) {
        sz_nx = ts.szNumerx;        sz_dx = ts.szDenomx;
        sz_ny = ts.szNumery;        sz_dy = ts.szDenomy;
        }
    else {
        /* Swap them over for vertical chars    */
        sz_nx = ts.szNumery;        sz_dx = ts.szDenomy;
        sz_ny = ts.szNumerx;        sz_dy = ts.szDenomx;
        }

    /* Adjust the first characters origin depending on the vertical
     * justification setting.
     */
    switch (ts.vertJust) {
        case MGL_CENTER_TEXT:
            height = (ascent + descent + 1)/2;
            y += (ts.dir == MGL_RIGHT_DIR ? height :
                 (ts.dir == MGL_LEFT_DIR ? -height : 0));
            x += (ts.dir == MGL_UP_DIR ? height :
                 (ts.dir == MGL_DOWN_DIR ? -height : 0));
            break;
        case MGL_TOP_TEXT:
            y += (ts.dir == MGL_RIGHT_DIR ? ascent :
                 (ts.dir == MGL_LEFT_DIR ? -ascent : 0));
            x += (ts.dir == MGL_UP_DIR ? ascent :
                 (ts.dir == MGL_DOWN_DIR ? -ascent : 0));
            break;
        case MGL_BOTTOM_TEXT:
            y += (ts.dir == MGL_RIGHT_DIR ? descent :
                 (ts.dir == MGL_LEFT_DIR ? -descent : 0));
            x += (ts.dir == MGL_UP_DIR ? descent :
                 (ts.dir == MGL_DOWN_DIR ? -descent : 0));
            break;

        /* We dont need to adjust the coordinates for BASELINE_TEXT */
        }

    /* Now draw the string using a different loop for each of the four
     * text drawing directions. Note that we maintain a character clipping
     * rectangle through each loop, and check to see if the character must
     * be clipped, calling a slower clipping routine if need be.
     */
    width = MGL_textWidth(str);

    switch (ts.dir) {
        case MGL_RIGHT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x - width/2 :
                    x - width));
            if (!useSlow) {
                charRect.left = i + maxKern;
                charRect.right = i + maxWidth + 1 + DC.a.penWidth;
                charRect.top = y - fontAscent;
                charRect.bottom = y - descent + 1 + DC.a.penHeight;
                }
            while (*str) {
                if ((offset = VECFONT(font)->offset[(uchar)*str]) == -1)
                    offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
                vec = (vector *)&VECFONT(font)->def[offset];
                MGL_unionRect(charRect,clipRect,&d);
                if (useSlow)
                    drawCharVecSlow(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                else if (!MGL_equalRect(clipRect,d)) {
                    if (!MGL_disjointRect(charRect,clipRect))
                        drawCharVecSlow(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                    }
                else
                    drawCharVec(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);

                i += (width = __MGL_charWidth((uchar)*str++));
                charRect.left += width;
                charRect.right += width;
                }
            break;
        case MGL_UP_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y + width/2 :
                    y + width));
            if (!useSlow) {
                charRect.left = x - fontAscent;
                charRect.right = x - descent + 1 + DC.a.penWidth;
                charRect.top = i - maxWidth;
                charRect.bottom = i - maxKern + 1 + DC.a.penHeight;
                }
            while (*str) {
                if ((offset = VECFONT(font)->offset[(uchar)*str]) == -1)
                    offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
                vec = (vector *)&VECFONT(font)->def[offset];
                rotateVector(vec,MGL_UP_DIR);
                MGL_unionRect(charRect,clipRect,&d);
                if (useSlow)
                    drawCharVecSlow(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                else if (!MGL_equalRect(clipRect,d)) {
                    if (!MGL_disjointRect(charRect,clipRect))
                        drawCharVecSlow(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                    }
                else
                    drawCharVec(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                rotateVector(vec,MGL_DOWN_DIR);

                i -= (width = __MGL_charWidth((uchar)*str++));
                charRect.top -= width;
                charRect.bottom -= width;
                }
            break;
        case MGL_LEFT_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? x :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    x + width/2 :
                    x + width));
            if (!useSlow) {
                charRect.left = i - maxWidth;
                charRect.right = i - maxKern + 1 + DC.a.penWidth;
                charRect.top = y + descent;
                charRect.bottom = y + fontAscent + 1 + DC.a.penHeight;
                }
            while (*str) {
                if ((offset = VECFONT(font)->offset[(uchar)*str]) == -1)
                    offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
                vec = (vector *)&VECFONT(font)->def[offset];
                rotateVector(vec,MGL_LEFT_DIR);
                MGL_unionRect(charRect,clipRect,&d);
                if (useSlow)
                    drawCharVecSlow(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                else if (!MGL_equalRect(clipRect,d)) {
                    if (!MGL_disjointRect(charRect,clipRect))
                        drawCharVecSlow(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                    }
                else
                    drawCharVec(i,y,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                rotateVector(vec,MGL_LEFT_DIR);

                i -= (width = __MGL_charWidth((uchar)*str++));
                charRect.left -= width;
                charRect.right -= width;
                }
            break;
        case MGL_DOWN_DIR:
            i = (ts.horizJust == MGL_LEFT_TEXT ? y :
                (ts.horizJust == MGL_CENTER_TEXT ?
                    y - width/2 :
                    y - width));
            if (!useSlow) {
                charRect.left = x + descent;
                charRect.right = x + fontAscent + 1 + DC.a.penWidth;
                charRect.top = i + maxKern;
                charRect.bottom = i + maxWidth + 1 + DC.a.penHeight;
                }
            while (*str) {
                if ((offset = VECFONT(font)->offset[(uchar)*str]) == -1)
                    offset = VECFONT(font)->offset[VEC_MISSINGSYMBOL(font)];
                vec = (vector *)&VECFONT(font)->def[offset];
                rotateVector(vec,MGL_DOWN_DIR);
                MGL_unionRect(charRect,clipRect,&d);
                if (useSlow)
                    drawCharVecSlow(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                else if (!MGL_equalRect(clipRect,d)) {
                    if (!MGL_disjointRect(charRect,clipRect))
                        drawCharVecSlow(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                    }
                else
                    drawCharVec(x,i,vec,sz_nx,sz_ny,sz_dx,sz_dy);
                rotateVector(vec,MGL_UP_DIR);

                i += (width = __MGL_charWidth((uchar)*str++));
                charRect.top += width;
                charRect.bottom += width;
                }
            break;
        }
}
