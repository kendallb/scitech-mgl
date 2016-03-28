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
* Description:  Device context state manipulation routines.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

/* The standard bitmap patterns */

pattern_t   _MGL_empty_pat  = {{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
pattern_t   _MGL_gray_pat   = {{0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55}};
pattern_t   _MGL_solid_pat  = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};

/*------------------------- Implementation --------------------------------*/

pattern_t * MGLAPI _MGL_getEmptyPat(void)  { return &_MGL_empty_pat; }
pattern_t * MGLAPI _MGL_getGrayPat(void)   { return &_MGL_gray_pat; }
pattern_t * MGLAPI _MGL_getSolidPat(void)  { return &_MGL_solid_pat; }

/****************************************************************************
DESCRIPTION:
Sets the current foreground color.

HEADER:
mgraph.h

PARAMETERS:
color   - New foreground color value

REMARKS:
Sets the current foreground color values. The foreground color value is used to draw
all primitives.

Note that the value passed to this routine is either a color index or a color value in
the correct packed pixel format for the current video mode. Use the
MGL_packColor routine to pack 24 bit RGB values for direct color video modes.

SEE ALSO:
MGL_getColor, MGL_setBackColor, MGL_getBackColor, MGL_packColor
****************************************************************************/
void MGLAPI MGL_setColor(
    color_t color)
{
    DC.r.SetForeColor(DC.a.color = color);
}

/****************************************************************************
DESCRIPTION:
Returns the current foreground color.

HEADER:
mgraph.h

RETURNS:
Current foreground color.

SEE ALSO:
MGL_setColor, MGL_getBackColor, MGL_setBackColor
****************************************************************************/
color_t MGLAPI MGL_getColor(void)
{
    return DC.a.color;
}

/****************************************************************************
Description:
Sets the currently active background color.

HEADER:
mgraph.h

PARAMETERS:
color   - New background color value

REMARKS:
Sets the current background color value. The background color value is used to
clear the display and viewport with the MGL_clearDevice and MGL_clearViewport
routines, and is also used for filling solid primitives in the
MGL_BITMAP_OPAQUE fill mode.

Note that the value passed to this routine is either a color index or a color value in
the correct packed pixel format for the device context. Use the MGL_packColor
routine to pack 24 bit RGB values for RGB device contexts.

SEE ALSO:
MGL_getBackColor, MGL_setColor, MGL_getColor, MGL_packColor
****************************************************************************/
void MGLAPI MGL_setBackColor(
    color_t color)
{
    DC.r.SetBackColor(DC.a.backColor = color);
}

/****************************************************************************
DESCRIPTION:
Returns the current background color value.

HEADER:
mgraph.h

RETURNS:
Current background color value.

REMARKS:
Returns the current background color value. The background color value is used to
clear the display and viewport with the MGL_clearDevice and MGL_clearViewport
routines, and is also used for filling solid primitives in the
MGL_BITMAP_OPAQUE fill mode.

SEE ALSO:
MGL_setBackColor, MGL_getColor, MGL_setColor
****************************************************************************/
color_t MGLAPI MGL_getBackColor(void)
{
    return DC.a.backColor;
}

/****************************************************************************
DESCRIPTION:
Sets the current video mode's aspect ratio.

HEADER:
mgraph.h

PARAMETERS:
aspectRatio - New value for the aspect ratio

REMARKS:
This function sets the aspect ratio for the device context to a new value. This ratio is
equal to:

    pixel x size
    ------------  * 1000
    pixel y size

The device context aspect ratio can be used to display circles and squares on the
device by approximating them with ellipses and rectangles of the appropriate
dimensions. Thus in order to determine the number of pixels in the y direction for a
square with 100 pixels in the x direction, we can simply use the code:

    y_pixels = ((long)x_pixels * 1000) / aspectratio

Note the cast to a long to avoid arithmetic overflow, as the aspect ratio is returned
as an integer value with 1000 being a 1:1 aspect ratio.

SEE ALSO:
MGL_getAspectRatio
****************************************************************************/
void MGLAPI MGL_setAspectRatio(
    int aspectRatio)
{
    DC.mi.aspectRatio = aspectRatio;
}

/****************************************************************************
DESCRIPTION:
Returns the current device context aspect ratio.

HEADER:
mgraph.h

REMARKS:
This function returns the aspect ratio of the currently active output device's physical
pixels. This ratio is equal to:

    pixel x size
    ------------  x 1000
    pixel y size

The device context aspect ratio can be used to display circles and squares on the
device by approximating them with ellipses and rectangles of the appropriate
dimensions. Thus in order to determine the number of pixels in the y direction for a
square with 100 pixels in the x direction, we can simply use the code:

    y_pixels = ((long)x_pixels * 1000) / aspectratio

Note the cast to a long to avoid arithmetic overflow, as the aspect ratio is returned
as an integer value with 1000 being a 1:1 aspect ratio.

RETURNS:
Current video mode aspect ratio * 1000.

SEE ALSO:
MGL_setAspectRatio

****************************************************************************/
int MGLAPI MGL_getAspectRatio(void)
{
    return DC.mi.aspectRatio;
}

/****************************************************************************
DESCRIPTION:
Returns the value for current default color (always white but value may vary).

HEADER:
mgraph.h

RETURNS:
Default color value for current video mode (always white).

REMARKS:
Returns the default color value for the current video mode. This color value is
white if the palette has not been changed, and will always be white in direct
color modes. However, the numerical value for white will vary depending on the
color depth.

SEE ALSO:
MGL_setColor, MGL_getColor
****************************************************************************/
color_t MGLAPI MGL_defaultColor(void)
{
    return MGL_realColor(&DC, MGL_WHITE);
}

/****************************************************************************
DESCRIPTION:
Sets the current background mode for monochrome bitmap and stipple line rendering.

HEADER:
mgraph.h

PARAMETERS:
mode    - New background mode to set

REMARKS:
This function sets the current background mode for the device context.
Supported modes are enumerated in MGL_backModes. By default the MGL starts
up in transparent mode, however you can change the background mode to draw
monochrome bitmaps and stippled lines with all 0 bits draw in the background
color instead of being transparent. This affects all monochrome bitmap
functions (including text rendering) and stipple line functions.

SEE ALSO:
MGL_getBackMode
****************************************************************************/
void MGLAPI MGL_setBackMode(
    int mode)
{
    DC.a.backMode = mode;
}

/****************************************************************************
DESCRIPTION:
Returns the current background mode for monochrome bitmap rendering.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Current background mode.

REMARKS:
This function returns the current background mode for the device context.
Defined modes are enumerated in MGL_backModes.

SEE ALSO:
MGL_setBackMode
****************************************************************************/
int MGLAPI MGL_getBackMode(void)
{
    return DC.a.backMode;
}

/****************************************************************************
DESCRIPTION:
Sets the current write mode operation.

HEADER:
mgraph.h

PARAMETERS:
mode    - New write mode operation to use

REMARKS:
Sets the currently active write mode. Write mode operations supported by the
SciTech MGL for all output primitives are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_getWriteMode
****************************************************************************/
void MGLAPI MGL_setWriteMode(
    int mode)
{
    DC.r.SetMix(DC.a.writeMode = mode);
    _MGL_setRenderingVectors();
}

/****************************************************************************
DESCRIPTION:
Returns the current write mode operation.

HEADER:
mgraph.h

RETURNS:
Current write mode operation.

REMARKS:
Returns the currently active write mode. Write modes supported by the SciTech MGL
for all output primitives are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_setWriteMode

****************************************************************************/
int MGLAPI MGL_getWriteMode(void)
{
    return DC.a.writeMode;
}

/****************************************************************************
DESCRIPTION:
Sets the current pen style.

HEADER:
mgraph.h

PARAMETERS:
style   - New pen style to use

REMARKS:
Returns the currently active pen style.  Pen styles supported by the SciTech MGL
are enumerated in MGL_penStyleType.

When filling in the MGL_BITMAP_TRANSPARENT mode, the foreground color
is used to fill in all pixels in the bitmap pattern that are a 1. Where the pixels in the
bitmap pattern are a 0, the original background color is retained. In the
MGL_BITMAP_OPAQUE mode, the background color is used to fill in the pixels
in the bitmap that are set to a 0. When filling in MGL_PIXMAP mode, the
foreground and background color values are not used, and the  pixel colors are
obtained directly from the pixmap pattern colors.

SEE ALSO:
MGL_getPenStyle, MGL_setPenBitmapPattern
****************************************************************************/
void MGLAPI MGL_setPenStyle(
    int style)
{
    DC.a.penStyle = style;
    if (DC.a.penStyle == MGL_BITMAP_OPAQUE)
        DC.r.Use8x8MonoPattern(DC.a.cntPenPat);
    else if (DC.a.penStyle == MGL_BITMAP_TRANSPARENT)
        DC.r.Use8x8TransMonoPattern(DC.a.cntPenPat);
    else if (DC.a.penStyle == MGL_PIXMAP)
        DC.r.Use8x8ColorPattern(DC.a.cntPenPixPat);
    else if (DC.a.penStyle == MGL_PIXMAP_TRANSPARENT)
        DC.r.Use8x8TransColorPattern(DC.a.cntPenPixPat,DC.a.penPixPatTrans);
    _MGL_setRenderingVectors();
}

/****************************************************************************
DESCRIPTION:
Returns the current pen style.

HEADER:
mgraph.h

RETURNS:
Current pen style.

REMARKS:
This function returns the currently active pen style.  Pen styles supported by
the SciTech MGL are enumerated in MGL_penStyleType.

When filling in the MGL_BITMAP_ TRANSPARENT mode, the foreground color
is used to fill in all pixels in the bitmap pattern that are a 1. Where the pixels in the
bitmap pattern are a 0, the original background color is retained. In the
MGL_BITMAP_ OPAQUE mode, the background color is used to fill in the pixels
in the bitmap that are set to a 0. When filling in MGL_PIXMAP mode, the
foreground and background color values are not used, and the  pixel colors are
obtained directly from the pixmap pattern colors.

SEE ALSO:
MGL_setPenStyle, MGL_setPenBitmapPattern, MGL_setPenPixmapPattern
****************************************************************************/
int MGLAPI MGL_getPenStyle(void)
{
    return DC.a.penStyle;
}

/****************************************************************************
DESCRIPTION:
Sets the current line style.

HEADER:
mgraph.h

PARAMETERS:
style   - New line style to use

REMARKS:
Sets the current line style.  MGL supports two different line styles, either pen style
patterned lines (MGL_LINE_PENSTYLE) or stippled lines
(MGL_LINE_STIPPLE). Pen style patterned lines are similar to those provided by
QuickDraw for the Macintosh where lines are drawing using a rectangular pen that
can have an arbitrary size and can be filled with an arbitrary pattern. Pen style
patterned lines are the default. Stippled lines are similar to those used by CAD
programs on the PC, and are 1-pixel wide lines that can be drawn using a 16-bit
stipple mask. Stippled lines can be drawn very fast in hardware using the VBE/AF
accelerator drivers.

In stippled line mode the line stipple pattern is used to determine which pixels in the
line get drawn depending on which bits in the pattern are set. The stipple pattern is a
16-bit value, and everywhere that a bit is set to a 1 in the pattern, a pixel will be
drawn in the line. Everywhere that a bit is a 0, the pixel will be skipped in the line.
Note that bit 0 in the stipple pattern corresponds to pixel 0,16,32,... in the line, bit 1
is pixel 1,17,33 etc. To create a line that is drawn as a 'dot dot dash dash' you
would use the following value:

    0011100111001001b or 0x39C9

In stippled line mode the line stipple counter is used to count the number of pixels
that have been drawn in the line, and is updated after the line has been drawn. The
purpose of this counter is to allow you to draw connected lines using a stipple
pattern and the stippling will be continuous across the break in the lines. You can
use this function to reset the stipple counter to a known value before drawing lines
to force the stipple pattern to start at a specific bit position (usually resetting it to 0
before drawing a group of lines is sufficient).

Note that VBE/AF 1.0 accelerated devices do not support the line stipple counter, so
this counter is essentially reset to 0 every time that a line is drawn using the
hardware. VBE/AF 2.0 will rectify this problem in the future.

SEE ALSO:
MGL_setLineStipple, MGL_setLineStippleCount, MGL_getLineStyle
****************************************************************************/
void MGLAPI MGL_setLineStyle(
    int style)
{
    DC.a.lineStyle = style;
}

/****************************************************************************
DESCRIPTION:
Returns the current line style.

HEADER:
mgraph.h

REMARKS:
Returns the current line style. Refer to MGL_setLineStipple for more information
on stippled lines.

SEE ALSO:
MGL_setLineStyle, MGL_setLineStipple, MGL_setLineStippleCount
****************************************************************************/
int MGLAPI MGL_getLineStyle(void)
{
    return DC.a.lineStyle;
}

/****************************************************************************
DESCRIPTION:
Set the current line stipple pattern.

PARAMETERS:
stipple - New 16 - bit stipple pattern to set.

HEADER:
mgraph.h

REMARKS:
Sets the current line stipple pattern. The line stipple pattern is used to determine
which pixels in the line get drawn depending on which bits in the pattern are set.
The stipple pattern is a 16-bit value, and everywhere that a bit is set to a 1 in the
pattern, a pixel will be drawn in the line. Everywhere that a bit is a 0, the pixel will
be skipped in the line. Note that bit 0 in the stipple pattern corresponds to pixel
0,16,32,... in the line, bit 1 is pixel 1,17,33 etc. To create a line that is drawn as a
'dot dot dash dash' you would use the following value:

    0011100111001001b or 0x39C9

Note that to enable stippled line mode you must call MGL_setLineStyle, with the
MGL_LINE_STIPPLE parameter. Also note that stippled lines can only be 1 pixel
wide, and the pen size will be ignored when drawing a stippled line.

SEE ALSO:
MGL_setLineStyle, MGL_setLineStippleCount, MGL_getLineStipple
****************************************************************************/
void MGLAPI MGL_setLineStipple(
    ushort stipple)
{
    DC.r.SetLineStipple(DC.a.lineStipple = stipple);
}

/****************************************************************************
DESCRIPTION:
Returns the current line stipple pattern.

HEADER:
mgraph.h

REMARKS:
Return the current line stipple pattern. Refer to MGL_setLineStipple for more
information on stippled lines.

SEE ALSO:
MGL_setLineStyle, MGL_setLineStipple, MGL_setLineStippleCount
****************************************************************************/
ushort MGLAPI MGL_getLineStipple(void)
{
    return DC.a.lineStipple;
}

/****************************************************************************
DESCRIPTION:
Sets the current line stipple counter.

HEADER:
mgraph.h

PARAMETERS:
stippleCount    - New line stipple counter to use

REMARKS:
Sets the current line stipple counter to a specific value.  The line stipple counter is
used to count the number of pixels that have been drawn in the line, and is updated
after the line has been drawn. The purpose of this counter is to allow you to draw
connected lines using a stipple pattern and the stippling will be continuous across
the break in the lines. You can use this function to reset the stipple counter to a
known value before drawing lines to force the stipple pattern to start at a specific bit
position (usually resetting it to 0 before drawing a group of lines is sufficient).

SEE ALSO:
MGL_setLineStyle, MGL_setLineStipple, MGL_getLineStippleCount
****************************************************************************/
void MGLAPI MGL_setLineStippleCount(
    uint stippleCount)
{
    DC.r.SetLineStippleCount(DC.a.stippleCount = stippleCount);
}

/****************************************************************************
DESCRIPTION:
Returns the current line stipple counter.

HEADER:
mgraph.h

REMARKS:
Return the current line stipple counter. Refer to MGL_setLineStipple for more
information on stippled lines.

SEE ALSO:
MGL_setLineStyle, MGL_setLineStipple, MGL_setLineStippleCount
****************************************************************************/
uint MGLAPI MGL_getLineStippleCount(void)
{
    return DC.a.stippleCount;
}

/****************************************************************************
DESCRIPTION:
Sets the current polygon type.

HEADER:
mgraph.h

PARAMETERS:
type    - New polygon type

REMARKS:
Sets the current polygon type. You can change this value to force MGL to work
with a specific polygon type (and to avoid the default automatic polygon type
checking). Polygon types supported by the SciTech MGL are enumerated in MGL_polygonType.

If you expect to be drawing lots of complex or convex polygons, setting the polygon
type can result in faster polygon rasterizing. Note that this setting does not affect the
specialized triangle and quadrilateral rasterizing routines.

SEE ALSO:
MGL_getPolygonType, MGL_fillPolygon
****************************************************************************/
void MGLAPI MGL_setPolygonType(
    int type)
{
    DC.a.polyType = type;
}

/****************************************************************************
DESCRIPTION:
Returns the current polygon type.

HEADER:
mgraph.h

RETURNS:
Current polygon type code.

REMARKS:
Returns the current polygon type. You can change this value with the
MGL_setPolygonType to force MGL to work with a specific polygon type (and to
avoid the default automatic polygon type checking). Polygon types supported by
the SciTech MGL are enumerated in MGL_polygonType.

If you expect to be drawing lots of complex or convex polygons, setting the polygon
type can result in faster polygon rasterizing. Note that this setting does not affect the
specialized triangle and quadrilateral rasterizing routines.

SEE ALSO:
MGL_setPolygonType, MGL_fillPolygon
****************************************************************************/
int MGLAPI MGL_getPolygonType(void)
{
    return DC.a.polyType;
}

/****************************************************************************
DESCRIPTION:
Sets the font anti-aliasing palette for color index modes.

HEADER:
mgraph.h

PARAMETERS:
colorfg     - Color used to represent 100% of foreground
color75     - Color used to represent 75% blend of foreground 25% background.
color50     - Color used to represent 50% blend of foreground 50% background.
color25     - Color used to represent 25% blend of foreground 75% background.
colorbg     - Color used to represent 100% of background.

REMARKS:
This function sets the values of the anti-aliasing palette used to draw
anti-aliased fonts when in non-blended mode in color index display
modes. Anti-aliased fonts are rendered using a 5 level scheme, and the
five colors passed in to this function set up the palette entries to be
used for color index modes. It is up to the application program to ensure
that the palette is set up correctly with the appropriate colors.

In RGB display modes, the colors are automatically computed from the
foreground and background colors, so this function should not be called.

SEE ALSO:
MGL_setFontBlendMode, MGL_getFontAntiAliasPalette
****************************************************************************/
void MGLAPI MGL_setFontAntiAliasPalette(
    color_t colorfg,
    color_t color75,
    color_t color50,
    color_t color25,
    color_t colorbg)
{
    if (DC.mi.modeFlags & MGL_IS_COLOR_INDEX) {
        DC.a.aaColor[0] = colorfg;
        DC.a.aaColor[1] = color75;
        DC.a.aaColor[2] = color50;
        DC.a.aaColor[3] = color25;
        DC.a.aaColor[4] = colorbg;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the font anti-aliasing palette for color index modes.

HEADER:
mgraph.h

PARAMETERS:
colorfg     - Color used to represent 100% of foreground
color75     - Color used to represent 75% blend of foreground 25% background.
color50     - Color used to represent 50% blend of foreground 50% background.
color25     - Color used to represent 25% blend of foreground 75% background.
colorbg     - Color used to represent 100% of background.

REMARKS:
This function returns the values of the anti-aliasing palette used to draw
anti-aliased fonts when in non-blended mode in color index display
modes.

SEE ALSO:
MGL_setFontBlendMode, MGL_setFontAntiAliasPalette
****************************************************************************/
void MGLAPI MGL_getFontAntiAliasPalette(
    color_t *colorfg,
    color_t *color75,
    color_t *color50,
    color_t *color25,
    color_t *colorbg)
{
    *colorfg = DC.a.aaColor[0];
    *color75 = DC.a.aaColor[1];
    *color50 = DC.a.aaColor[2];
    *color25 = DC.a.aaColor[3];
    *colorbg = DC.a.aaColor[4];
}

/****************************************************************************
DESCRIPTION:
Sets the blending mode for anti-aliased fonts.

HEADER:
mgraph.h

PARAMETERS:
type    - Type of Blending mode, enumerated by MGL_fontBlendType.

REMARKS:
Sets the blending mode for anti-aliased fonts. The default blending mode
of MGL_AA_NORMAL does not combine source and destination pixels, but
chooses the colors from the anti-aliasing palette in 256 color modes, or
from value blended between the current foreground and background colors.

If the font blending mode is set to MGL_AA_RGBBLEND, then all anti-aliased
pixels in the font are blended both between the current foreground and
background colors, and the destination pixels on the screen.

Using MGL_AA_NORMAL provides the fastest font anti-aliasing support, but it
only works properly if you are drawing text over a solid background (such as
text in a word processor type application). Using MGL_AA_RGBBLEND provides
the highest quality anti-aliasing, and will correctly blend fonts over the
top of existing images in the framebuffer.

Note:   MGL_AA_RGBBLEND is only available in modes with > 8 bits per pixel.
        Blending cannot be done in 8 bits per pixel display modes.

SEE ALSO:
MGL_getFontBlendMode, MGL_setFontAntiAliasPalette
****************************************************************************/
void MGLAPI MGL_setFontBlendMode(
    int type)
{
    DC.a.fontBlendMode = type;
}

/****************************************************************************
DESCRIPTION:
Returns the blending mode for anti-aliased fonts.

HEADER:
mgraph.h

RETURNS:
Current font blending mode, enumerated by MGL_fontBlendType.

REMARKS:
This function returns the current font blending mode used to render
anti-aliased fonts.

SEE ALSO:
MGL_setFontBlendMode, MGL_setFontAntiAliasPalette
****************************************************************************/
int MGLAPI MGL_getFontBlendMode(void)
{
    return DC.a.fontBlendMode;
}

/****************************************************************************
DESCRIPTION:
Sets the current source and destination pixel blending function.

HEADER:
mgraph.h

PARAMETERS:
srcBlendFunc    - New source blending function, defined by MGL_blendFuncType.
dstBlendFunc    - New destination blending function, defined by MGL_blendFuncType.

REMARKS:
This function sets the current source and destination pixel blending function
used to enable pixel blending in the MGL. By default pixel blending is disabled,
and the blending function is set to MGL_BLEND_NONE. Essentially the source
and destination blend function define how to combine the source and
destination pixel colors together to get the resulting destination color
during rendering. For a more detailed description of the blending functions,
see the documentation for the MGL_blendFuncType enumeration.

Note:   Blending is only enabled when /both/ the source and destination
        blending functions are set to values other than MGL_BLEND_NONE.

Note:   Blending and logical write modes are not supported at the same time.
        When you enable blending modes, the logical write mode set by
        MGL_setWriteMode is ignored.

Note:   No hardware currently supports arbitrary pixel blending operations
        for 2D operations, so enabling blending modes causes the MGL to run
        entirely in software. Keep this in mind since if performance is
        important!

SEE ALSO:
MGL_getBlendFunc, MGL_setAlphaValue
****************************************************************************/
void MGLAPI MGL_setBlendFunc(
    int srcBlendFunc,
    int dstBlendFunc)
{
    if (DC.r.SetBlendFunc)
        DC.r.SetBlendFunc(DC.a.srcBlendFunc = srcBlendFunc,DC.a.dstBlendFunc = dstBlendFunc);
    else {
        /* Translate parameters and call older, obsolete interface */
        if (srcBlendFunc == MGL_BLEND_SRCALPHAFAST) {
            srcBlendFunc = MGL_BLEND_SRCALPHA;
            dstBlendFunc = MGL_BLEND_ONEMINUSSRCALPHA;
            }
        if (srcBlendFunc == MGL_BLEND_CONSTANTALPHAFAST) {
            srcBlendFunc = MGL_BLEND_CONSTANTALPHA;
            dstBlendFunc = MGL_BLEND_ONEMINUSCONSTANTALPHA;
            }
        DC.r.SetSrcBlendFunc_Old(DC.a.srcBlendFunc = srcBlendFunc);
        DC.r.SetDstBlendFunc_Old(DC.a.dstBlendFunc = dstBlendFunc);
        }
}

/****************************************************************************
DESCRIPTION:
Returns the current source and destination pixel blending functions.

HEADER:
mgraph.h

PARAMETERS:
srcBlendFunc    - Place to store source blending function
dstBlendFunc    - Place to store destination blending function

REMARKS:
This function returns the current source and destination pixel blending functions
used for pixel blending in the MGL. By default pixel blending is disabled, and
the blending function is set to MGL_BLEND_NONE. For a more detailed
description of the blending functions, see the documentation for the
MGL_blendFuncType enumeration.

SEE ALSO:
MGL_setBlendFunc, MGL_setAlphaValue
****************************************************************************/
void MGLAPI MGL_getBlendFunc(
    int *srcBlendFunc,
    int *dstBlendFunc)
{
    *srcBlendFunc = DC.a.srcBlendFunc;
    *dstBlendFunc = DC.a.dstBlendFunc;
}

/****************************************************************************
DESCRIPTION:
Sets the current constant alpha value for blending functions.

HEADER:
mgraph.h

PARAMETERS:
alpha   - New constant alpha value for blending function, between 0 and 255

REMARKS:
This function sets the current constant alpha value used for pixel blending
functions in the MGL. The constant alpha value is only used if one of the
source or destination blending functions is set to include constant alpha.
The constant alpha value should be in the range of 0 through 255.

SEE ALSO:
MGL_setSrcBlendFunc, MGL_setDstBlendFunc, MGL_getAlphaValue
****************************************************************************/
void MGLAPI MGL_setAlphaValue(
    uchar alpha)
{
    DC.r.SetAlphaValue(DC.a.alphaValue = alpha);
}

/****************************************************************************
DESCRIPTION:
Returns the current constant alpha value for blending functions.

HEADER:
mgraph.h

RETURNS:
Current constant alpha value for blending functions.

REMARKS:
This function returns the current constant alpha value for pixel blending
function in the MGL.

SEE ALSO:
MGL_setSrcBlendFunc, MGL_setDstBlendFunc, MGL_setAlphaValue
****************************************************************************/
uchar MGLAPI MGL_getAlphaValue(void)
{
    return DC.a.alphaValue;
}

/****************************************************************************
DESCRIPTION:
Sets the current plane mask for all rendering operations.

HEADER:
mgraph.h

PARAMETERS:
mask    - New plane mask to make active

REMARKS:
This function sets the current plane mask for all rendering operations, which
is used to mask out specific bits from being affected during writes to the
framebuffer. The mask passed in should be a packed color value for the
currently active display mode.

Note:   A plane mask of 0xFFFFFFFF will disable plane masking operation,
        and this is the default plane mask enabled by the MGL when it starts
        up.

Note:   Not all hardware supports plane masking in hardware, and if hardware
        plane masking is not available, enabling this will cause the rendering
        to be done entirely in software.

SEE ALSO:
MGL_getPlaneMask
****************************************************************************/
void MGLAPI MGL_setPlaneMask(
    ulong mask)
{
    DC.r.SetPlaneMask(DC.a.planeMask = mask);
}

/****************************************************************************
DESCRIPTION:
Returns the current plane mask for all rendering operations.

HEADER:
mgraph.h

RETURNS:
Current plane mask.

REMARKS:
This function returns the current plane mask for all rendering operations,
which is used to mask out specific bits from being affected during writes
to the framebuffer.

SEE ALSO:
MGL_setPlaneMask
****************************************************************************/
ulong MGLAPI MGL_getPlaneMask(void)
{
    return DC.a.planeMask;
}

/****************************************************************************
DESCRIPTION:
Sets the current dithering mode for all blitting operations.

HEADER:
mgraph.h

PARAMETERS:
mode    - New dithering mode to make active

REMARKS:
This function sets the current dithering mode used when blitting RGB images
to 8, 15 and 16 bits per pixel device context. If dithering is enabled, the
blit operation will dither the resulting image to produce the best quality.
When dithering is disabled, the blit operation uses the closest color which
has less quality but is faster.

Note:   The closest color method is fastest when the destination device
        context is a 15 or 16 bits per pixel bitmap. However when the
        destination is an 8 bits per pixel device context, the dithering
        mode will usually be faster.

Note:   Dithering is /on/ by default in the MGL.

SEE ALSO:
MGL_getDitherMode
****************************************************************************/
void MGLAPI MGL_setDitherMode(
    int mode)
{
    DC.a.ditherMode = mode;
}

/****************************************************************************
DESCRIPTION:
Returns the current dithering mode for all blitting operations.

HEADER:
mgraph.h

RETURNS:
Current dithering mode mask.

REMARKS:
This function returns the current dithering mode used when blitting RGB
images to 8, 15 and 16 bits per pixel device context.

SEE ALSO:
MGL_setDitherMode
****************************************************************************/
int MGLAPI MGL_getDitherMode(void)
{
    return DC.a.ditherMode;
}

/****************************************************************************
DESCRIPTION:
Sets the current pen size.

HEADER:
mgraph.h

PARAMETERS:
h   - Height of the pen in pixels
w   - Width of the pen in pixels

REMARKS:
Sets the size of the current pen size in pixels. The default pen is 1 pixel by 1 pixel in
dimensions, however you can change this to whatever value you like. When
primitives are rasterized with a pen other than the default, the pixels in the pen
always lie to the right and below the current pen position.

SEE ALSO:
MGL_getPenSize
****************************************************************************/
void MGLAPI MGL_setPenSize(
    int h,
    int w)
{
    CHECK(w >= 1);
    CHECK(h >= 1);
    DC.a.penHeight = h-1;
    DC.a.penWidth = w-1;
    _MGL_setRenderingVectors();
}

/****************************************************************************
DESCRIPTION:
Returns the current pen size.

HEADER:
mgraph.h

PARAMETERS:
height  - Place to store the current pen height
width   - Place to store the current pen width

REMARKS:
Return the size of the current pen in pixels. The default pen is 1 pixel by 1 pixel in
dimensions, however you can change this to whatever value you like. When
primitives are rasterized with a pen other than the default, the pixels in the pen
always lie to the right and below the current pen position.

SEE ALSO:
MGL_setPenSize
****************************************************************************/
void MGLAPI MGL_getPenSize(
    int *height,
    int *width)
{
    *height = DC.a.penHeight+1;
    *width = DC.a.penWidth+1;
}

/****************************************************************************
DESCRIPTION:
Downloads a new bitmap pattern into the driver.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the bitmap pattern slot to download into
pat     - New bitmap pattern to download

REMARKS:
This function downloads a new bitmap pattern into the hardware on the
graphics device. The MGL supports 8 patterns cached in the driver, allowing
you to select one of them to be active at a time using the
MGL_usePenBitmapPattern function. Bitmap patterns are used used when
rendering patterned primitives in the MGL_BITMAP_TRANSPARENT and
MGL_BITMAP_OPQAUE pen styles. A bitmap pattern is defined as an 8 x 8 array
of monochrome pixels, stored as an array of 8 bytes.

When filling in the MGL_BITMAP_TRANSPARENT mode, the foreground color
is used to fill in all pixels in the bitmap pattern that are a 1. Where the
pixels in the bitmap pattern are a 0, the original background color is
retained. In the MGL_BITMAP_OPAQUE mode, the background color is used to
fill in the pixels in the bitmap that are set to a 0.

Note:   After calling MGL_setPenBitmapPattern, the pattern will not become
        active until *after* you also call MGL_usePenBitmapPattern. This
        function only downloads the pattern into the pattern cache, and does
        not actually select the pattern for use (even if the old selected
        pattern was the same index).

SEE ALSO:
MGL_usePenBitmapPattern, MGL_getPenBitmapPattern, MGL_setPenPixmapPattern,
MGL_setPenStyle
****************************************************************************/
void MGLAPI MGL_setPenBitmapPattern(
    int index,
    const pattern_t *pat)
{
    memcpy(&DC.a.penPat[index],pat,sizeof(pattern_t));
    DC.r.Set8x8MonoPattern(index,GAMONOPAT(pat));
}

/****************************************************************************
DESCRIPTION:
Sets the currently active bitmap pattern.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the bitmap pattern to make active

REMARKS:
This function sets the currently active bitmap pattern used when rendering
patterned primitives in the MGL_BITMAP_TRANSPARENT and MGL_BITMAP_OPQAUE pen
styles. The pattern must already have been downloaded with a call to the
MGL_setPenBitmapPattern function.

SEE ALSO:
MGL_setPenBitmapPattern, MGL_getPenBitmapPattern, MGL_setPenPixmapPattern,
MGL_setPenStyle
****************************************************************************/
void MGLAPI MGL_usePenBitmapPattern(
    int index)
{
    DC.a.cntPenPat = index;
    if (DC.a.penStyle == MGL_BITMAP_OPAQUE)
        DC.r.Use8x8MonoPattern(index);
    else if (DC.a.penStyle == MGL_BITMAP_TRANSPARENT)
        DC.r.Use8x8TransMonoPattern(index);
}

/****************************************************************************
DESCRIPTION:
Returns the currently active bitmap pattern index, and pattern data.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the bitmap pattern slot to read from
pat     - Place to store the bitmap pattern

RETURNS
Currently active bitmap pattern index.

REMARKS:
This function returns a copy of the specified bitmap pattern used when
rendering patterned primitives in the MGL_BITMAP_TRANSPARENT and
MGL_BITMAP_OPQAUE pen styles. It also returns the currently active bitmap
pattern index, since the MGL supports 8 patterns cached internally in the
driver.

Note:   You can pass a value of NULL in the pat parameter, and the pattern
        data will not be copied. Ie: MGL_getPenBitmapPattern(0,NULL) returns
        just the currently active bitmap pattern index.

SEE ALSO:
MGL_setPenBitmapPattern, MGL_usePenBitmapPattern, MGL_setPenPixmapPattern,
MGL_setPenStyle
****************************************************************************/
int MGLAPI MGL_getPenBitmapPattern(
    int index,
    pattern_t *pat)
{
    if (pat)
        memcpy(pat,&DC.a.penPat[index],sizeof(pattern_t));
    return DC.a.cntPenPat;
}

/****************************************************************************
DESCRIPTION:
Downloads a new pixmap pattern into the driver.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the pixmap pattern slot to download into
pat     - New pixmap pattern to download

REMARKS:
This function downloads a new pixmap pattern into the hardware on the
graphics device. The MGL supports 8 patterns cached in the driver, allowing
you to select one of them to be active at a time using the
MGL_usePenPixmapPattern function. Pixmap patterns are used used when
rendering patterned primitives in the MGL_PIXMAP pen style. A pixmap pattern
is defined as an 8 x 8 array of color pixels, stored differently depending
on the color depth for the pattern. When filling in MGL_PIXMAP mode, the
foreground and background color values are not used, and the pixel colors
are obtained directly from the pixmap pattern colors.

Note:   After calling MGL_setPenPixmapPattern, the pattern will not become
        active until *after* you also call MGL_usePenPixmapPattern. This
        function only downloads the pattern into the pattern cache, and does
        not actually select the pattern for use (even if the old selected
        pattern was the same index).

SEE ALSO:
MGL_usePenPixmapPattern, MGL_getPenPixmapPattern, MGL_setPenBitmapPattern,
MGL_setPenStyle
****************************************************************************/
void MGLAPI MGL_setPenPixmapPattern(
    int index,
    const pixpattern_t *pat)
{
    memcpy(&DC.a.penPixPat[index],pat,sizeof(pixpattern_t));
    DC.r.Set8x8ColorPattern(index,GACLRPAT(pat));
}

/****************************************************************************
DESCRIPTION:
Sets the currently active pixmap pattern.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the pixmap pattern to make active

REMARKS:
This function sets the currently active bitmap pattern used when rendering
patterned primitives in the MGL_PIXMAP pen style. The pattern must already
have been downloaded with a call to the MGL_setPenPixmapPattern function.

SEE ALSO:
MGL_setPenPixmapPattern, MGL_getPenPixmapPattern, MGL_setPenBitmapPattern,
MGL_setPenStyle
****************************************************************************/
void MGLAPI MGL_usePenPixmapPattern(
    int index)
{
    DC.a.cntPenPat = index;
    if (DC.a.penStyle == MGL_PIXMAP)
        DC.r.Use8x8ColorPattern(index);
    else if (DC.a.penStyle == MGL_PIXMAP_TRANSPARENT)
        DC.r.Use8x8TransColorPattern(index,DC.a.penPixPatTrans);
}

/****************************************************************************
DESCRIPTION:
Sets the currently active pixmap pattern transparent color.

HEADER:
mgraph.h

PARAMETERS:
color   - New transparent color for pixmap patterns

REMARKS:
This function sets the currently active pixmap pattern transparent color.
This is used to determine which pixels in the pixmap pattern are transparent
when the device context has the MGL_PIXMAP_TRANSPARENT pen style active.

SEE ALSO:
MGL_setPenPixmapPattern, MGL_getPenPixmapTransparent
****************************************************************************/
void MGLAPI MGL_setPenPixmapTransparent(
    color_t color)
{
    DC.a.penPixPatTrans = color;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active pixmap pattern transparent color.

HEADER:
mgraph.h

RETURNS:
Returns the currently active pixmap pattern transparent color.

REMARKS:
This function returns the currently active pixmap pattern transparent color.
This is used to determine which pixels in the pixmap pattern are transparent
when the device context has the MGL_PIXMAP_TRANSPARENT pen style active.

SEE ALSO:
MGL_setPenPixmapPattern, MGL_setPenPixmapTransparent
****************************************************************************/
color_t MGLAPI MGL_getPenPixmapTransparent(void)
{
    return DC.a.penPixPatTrans;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active pixmap pattern index, and pattern data.

HEADER:
mgraph.h

PARAMETERS:
index   - Index of the bitmap pattern slot to read from
pat     - Place to store the pixmap pattern

RETURNS
Currently active bitmap pattern index.

REMARKS:
This function returns a copy of the specified pixmap pattern used when
rendering patterned primitives in the MGL_PIXMAP pen style. It also returns
the currently active pixmap pattern index, since the MGL supports 8
patterns cached internally in the driver.

Note:   You can pass a value of NULL in the pat parameter, and the pattern
        data will not be copied. Ie: MGL_getPenPixmapPattern(0,NULL) returns
        just the currently active pixmap pattern index.

SEE ALSO:
MGL_setPenPixmapPattern, MGL_usePenPixmapPattern, MGL_setPenBitmapPattern,
MGL_setPenStyle
****************************************************************************/
int MGLAPI MGL_getPenPixmapPattern(
    int index,
    pixpattern_t *pat)
{
    if (pat)
        memcpy(pat,&DC.a.penPixPat[index],sizeof(pixpattern_t));
    return DC.a.cntPenPixPat;
}

/****************************************************************************
DESCRIPTION:
Sets the current text horizontal and vertical justification.

HEADER:
mgraph.h

PARAMETERS:
horiz   - New horizontal text justification value
vert    - New vertical text justification value

REMARKS:
Sets the current text justification values. Horizontal and vertical justification
type supported by the SciTech MGL are enumerated in MGL_textJustType.

SEE ALSO:
MGL_getTextJustify
****************************************************************************/
void MGLAPI MGL_setTextJustify(
    int horiz,
    int vert)
{
    DC.a.ts.horizJust = horiz;
    DC.a.ts.vertJust = vert;
}

/****************************************************************************
DESCRIPTION:
Returns the current text justification.

HEADER:
mgraph.h

PARAMETERS:
horiz   - Place to store horizontal justification
vert    - Place to store vertical justification

REMARKS:
Returns the current text justification values. Justification types supported by
the SciTech MGL are enumerated in MGL_textJustType.

SEE ALSO:
MGL_setTextJustify
****************************************************************************/
void MGLAPI MGL_getTextJustify(
    int *horiz,
    int *vert)
{
    *horiz = DC.a.ts.horizJust;
    *vert = DC.a.ts.vertJust;
}

/****************************************************************************
DESCRIPTION:
Sets the current text direction.

HEADER:
mgraph.h

PARAMETERS:
direction   - New text direction value

REMARKS:
Sets the current text direction.  Directions supported by the SciTech MGL are
enumerated in MGL_textJustType.


SEE ALSO:
MGL_getTextDirection, MGL_drawStr
****************************************************************************/
void MGLAPI MGL_setTextDirection(
    int direction)
{
    DC.a.ts.dir = direction;
}

/****************************************************************************
DESCRIPTION:
Returns the current text direction.

HEADER:
mgraph.h

RETURNS:
Current text direction.

REMARKS:
Returns the current text direction.  Directions supported by the SciTech MGL are
enumerated in MGL_textJustType.

SEE ALSO:
MGL_setTextDirection, MGL_drawStr
****************************************************************************/
int MGLAPI MGL_getTextDirection(void)
{
    return DC.a.ts.dir;
}

/****************************************************************************
DESCRIPTION:
Sets the current text scaling factors

HEADER:
mgraph.h

PARAMETERS:
numerx  - x scaling numerator value
denomx  - x scaling denominator value
numery  - y scaling numerator value
denomy  - y scaling denominator value

REMARKS:
Sets the current text scaling factors used by MGL. The text size values define an
integer scaling factor to be used, where the actual values will be computed using the
following formula:

             unscaled * numer
    scaled = ----------------
                  denom

Note:MGL can only scale vectored fonts.

SEE ALSO:
MGL_getTextSize
****************************************************************************/
void MGLAPI MGL_setTextSize(
    int numerx,
    int denomx,
    int numery,
    int denomy)
{
    DC.a.ts.szNumerx = numerx;
    DC.a.ts.szNumery = numery;
    DC.a.ts.szDenomx = denomx;
    DC.a.ts.szDenomy = denomy;
}

/****************************************************************************
DESCRIPTION:
Returns the current text scaling factors.

HEADER:
mgraph.h

PARAMETERS:
numerx  - Place to store the x numerator value
denomx  - Place to store the x denominator value
numery  - Place to store the y numerator value
denomy  - Place to store the y denominator value

REMARKS:
Returns the current text scaling factors used by MGL. The text size values define an
integer scaling factor to be used, where the actual values will be computed using the
following formula:

Note:MGL can only scale vector fonts. Bitmap fonts cannot be scaled.

SEE ALSO:
MGL_setTextSize

****************************************************************************/
void MGLAPI MGL_getTextSize(
    int *numerx,
    int *denomx,
    int *numery,
    int *denomy)
{
    *numerx = DC.a.ts.szNumerx;
    *numery = DC.a.ts.szNumery;
    *denomx = DC.a.ts.szDenomx;
    *denomy = DC.a.ts.szDenomy;
}

/****************************************************************************
DESCRIPTION:
Sets the current space extra value.

HEADER:
mgraph.h

PARAMETERS:
extra   - New space extra value

REMARKS:
Sets the current space extra value used when drawing text in the current font. The
space extra value is normally zero, but can be a positive or negative value. When
this value is positive, it will insert extra space between the characters in a font and
making this value negative will make the characters run on top of each other.

SEE ALSO:
MGL_getSpaceExtra, MGL_drawStr
****************************************************************************/
void MGLAPI MGL_setSpaceExtra(
    int extra)
{
    DC.a.ts.spaceExtra = extra;
}

/****************************************************************************
DESCRIPTION:
Returns the current space extra value.

HEADER:
mgraph.h

RETURNS:
Current space extra value.

REMARKS:
Returns the current space extra value used when drawing text in the current font.
The space extra value is normally zero, but can be a positive or negative value. This
value can be used to insert extra space between the characters in a font (making this
value a large negative value will make the characters run on top of each other).

SEE ALSO:
MGL_setSpaceExtra, MGL_drawStr
****************************************************************************/
int MGLAPI MGL_getSpaceExtra(void)
{
    return DC.a.ts.spaceExtra;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active font.

HEADER:
mgraph.h

RETURNS:
Pointer to currently active font.

REMARKS:
Returns a pointer to the currently active font. The currently active font is used to
perform all text output by MGL.

SEE ALSO:
MGL_useFont, MGL_loadFont, MGL_unloadFont
****************************************************************************/
font_t * MGLAPI MGL_getFont(void)
{
    return DC.a.ts.font;
}

/****************************************************************************
DESCRIPTION:
Restores the current text settings.

HEADER:
mgraph.h

PARAMETERS:
settings    - Text settings to restore

REMARKS:
Restores a set of previously saved text settings. This routine provides a way to save
and restore all the values relating to the rasterizing of text in MGL with a single
function call.

SEE ALSO:
MGL_getTextSettings
****************************************************************************/
void MGLAPI MGL_setTextSettings(
    text_settings_t *settings)
{
    DC.a.ts = *settings;
    MGL_useFont(settings->font);
    MGL_setTextDirection(settings->dir);
}

/****************************************************************************
DESCRIPTION:

Returns the current text settings.

HEADER:
mgraph.h

PARAMETERS:
settings    - Place to store the current text settings

REMARKS:
Returns a copy of the currently active text settings. This routine provides a way to
save and restore all the values relating to the rasterizing of text in MGL with a
single function call.

SEE ALSO:
MGL_setTextSettings
****************************************************************************/
void MGLAPI MGL_getTextSettings(
    text_settings_t *settings)
{
    *settings = DC.a.ts;
}

/*******************************************************************************
DESCRIPTION:
Sets the currently active font.

HEADER:
mgraph.h

PARAMETERS:
font    - New font to use

RETURNS:
True if the font was valid and selected, false if not.

REMARKS:
Selects the specified font as the currently active font for the active device context. If
the font data is invalid, the MGL result flag is set and the routine will return false.

Do not unload a font file if it is currently in use by MGL!

SEE ALSO:
MGL_drawStr,MGL_loadFont, MGL_unloadFont
*******************************************************************************/
ibool MGLAPI MGL_useFont(
    font_t *font)
{
    int fontType;

    __MGL_result = grOK;
    if (font == NULL)
        return true;

    fontType = (font->fontType & MGL_FONTTYPEMASK);
    if (fontType < 0 || fontType > MGL_PROPFONT) {
        __MGL_result = grInvalidFont;
        return false;
        }

    /* Ensure that the font is rotated back the default direction before
     * swapping to the new font.
     */
    MGL_setTextDirection(MGL_RIGHT_TEXT);
    DC.a.ts.font = font;

    /* Set ts.useEncoding to reflect current font type. */
    if (((font->fontType & MGL_FONTTYPEMASK) != MGL_VECTORFONT) &&
        (BITFONT(font)->lib && BITFONT(font)->lib->fontLibType != MGL_BITMAPFONT_LIB))
        DC.a.ts.useEncoding = true;
    else
        DC.a.ts.useEncoding = false;

    return true;
}

/****************************************************************************
DESCRIPTION:
Returns the starting and ending arc coordinates.

HEADER:
mgraph.h

PARAMETERS:
coords  - Pointer to structure to store coordinates

REMARKS:
This function returns the center coordinate, and starting and ending points on the
ellipse that define the last elliptical arc that was rasterized. You can then use these
coordinates to draw a line from the center of the ellipse to the starting and ending
points to complete the outline of an elliptical wedge.

Note that you must call this routine immediately after calling the MGL_ellipseArc
family of routines.

SEE ALSO:
MGL_ellipseArc, MGL_fillEllipseArc
****************************************************************************/
void MGLAPI MGL_getArcCoords(
    arc_coords_t *coords)
{
    *coords = DC.ac;
}

/****************************************************************************
REMARKS:
Sets up all the rendering vectors depending on the current
rendering options so that we can eliminate all switching
code in the high level MGL functions.
{secret}
****************************************************************************/
void MGLAPI _MGL_setRenderingVectors(void)
{
    switch (DC.a.penStyle) {
        case MGL_BITMAP_SOLID:
            DC.r.cur = DC.r.solid;
            break;
        case MGL_BITMAP_OPAQUE:
        case MGL_BITMAP_TRANSPARENT:
            DC.r.cur = DC.r.patt;
            break;
        case MGL_PIXMAP:
        case MGL_PIXMAP_TRANSPARENT:
            DC.r.cur = DC.r.colorPatt;
            break;
        }
}

/****************************************************************************
DESCRIPTION:
Returns a copy of the current rasterizing attributes.

HEADER:
mgraph.h

PARAMETERS:
attr    - Pointer to structure to store attribute values in

REMARKS:
This function returns a copy of the currently active attributes. You can use
this routine to save the state of MGL and later restore this state with the
MGL_restoreAttributes routine.

SEE ALSO:
MGL_restoreAttributes
****************************************************************************/
void MGLAPI MGL_getAttributes(
    attributes_t *attr)
{
    *attr = DC.a;
    attr->penHeight++;
    attr->penWidth++;
}

/****************************************************************************
DESCRIPTION:
Restores a previously saved set of rasterizing attributes.

HEADER:
mgraph.h

PARAMETERS:
attr    - Pointer to the attribute list to restore

REMARKS:
This function restores a set of attributes that were saved with the
MGL_getAttributes routine. The attributes list represents the current state of
the MGL. The value of the color palette is not changed by this routine.

Note:   Calling this function sets the viewport and clip rectangle for the
        device context to the values that were saved by

SEE ALSO:
MGL_getAttributes
****************************************************************************/
void MGLAPI MGL_restoreAttributes(
    attributes_t *attr)
{
    int i;

    DC.a = *attr;
    DC.a.penHeight--;
    DC.a.penWidth--;
    MGL_setColor(attr->color);
    MGL_setBackColor(attr->backColor);
    MGL_setWriteMode(attr->writeMode);
    MGL_setPenStyle(attr->penStyle);
    MGL_setLineStipple((ushort)attr->lineStipple);
    MGL_setLineStippleCount(attr->stippleCount);
    for (i = 0; i < 8; i++) {
        MGL_setPenBitmapPattern(i,&attr->penPat[i]);
        MGL_setPenPixmapPattern(i,&attr->penPixPat[i]);
        }
    MGL_usePenBitmapPattern(attr->cntPenPat);
    MGL_usePenPixmapPattern(attr->cntPenPixPat);
    MGL_setTextSettings(&attr->ts);
    MGL_setBlendFunc(attr->srcBlendFunc,attr->dstBlendFunc);
    MGL_setAlphaValue((uchar)attr->alphaValue);
    MGL_setPlaneMask(attr->planeMask);
}

/****************************************************************************
DESCRIPTION:
Reset all rasterizing attributes to their default values.

HEADER:
mgraph.h

PARAMETERS:
dc  - device context to be reset

REMARKS:
This function resets all of the device context attributes to their default
values. Note that this function also sets the current viewport and clip
rectangle for the device context to cover the entire device context surface.

SEE ALSO:
MGL_getAttributes, MGL_restoreAttributes, MGL_getDefaultPalette
****************************************************************************/
void MGLAPI MGL_defaultAttributes(
    MGLDC *dc)
{
    MGLDC   *oldDC = _MGL_dcPtr;
    rect_t  view;

    /* Set default attributes for the device context */
    dc->a.color             = MGL_realColor(dc, MGL_WHITE);
    dc->a.backColor         = MGL_BLACK;
    dc->a.aaColor[0]        = 0;
    dc->a.aaColor[1]        = 0;
    dc->a.aaColor[2]        = 0;
    dc->a.aaColor[3]        = 0;
    dc->a.aaColor[4]        = 0;
    dc->a.backMode          = MGL_TRANSPARENT_BACKGROUND;
    dc->a.CP.x              = 0;
    dc->a.CP.y              = 0;
    dc->a.writeMode         = MGL_REPLACE_MODE;
    dc->a.penStyle          = MGL_BITMAP_SOLID;
    dc->a.penHeight         = 1;
    dc->a.penWidth          = 1;
    dc->a.cntPenPat         = 0;
    dc->a.cntPenPixPat      = 0;
    dc->a.penPixPatTrans    = 0;
    dc->a.lineStyle         = MGL_LINE_PENSTYLE;
    dc->a.lineStipple       = 0xFFFF;
    dc->a.stippleCount      = 0;
    dc->a.polyType          = MGL_AUTO_POLYGON;
    dc->a.fontBlendMode     = MGL_AA_NORMAL;
    dc->a.srcBlendFunc      = MGL_BLEND_NONE;
    dc->a.dstBlendFunc      = MGL_BLEND_NONE;
    dc->a.alphaValue        = 0xFF;
    dc->a.planeMask         = 0xFFFFFFFFUL;
    dc->a.ditherMode        = MGL_DITHER_ON;
    dc->a.ts.horizJust      = MGL_LEFT_TEXT;
    dc->a.ts.vertJust       = MGL_TOP_TEXT;
    dc->a.ts.dir            = MGL_RIGHT_TEXT;
    dc->a.ts.szNumerx       = 1;
    dc->a.ts.szNumery       = 1;
    dc->a.ts.szDenomx       = 1;
    dc->a.ts.szDenomy       = 1;
    dc->a.ts.spaceExtra     = 0;
    dc->a.ts.font           = NULL;
    dc->a.ts.encoding       = NULL;
    dc->a.ts.useEncoding    = false;

    /* Make the device context current so we can restore the attributes */
    MGL_makeCurrentDC(dc);
    _MGL_updateCurrentDC(dc);
    MGL_restoreAttributes(&DC.a);

    /* Set default viewport and clip rectangle information (setting the
     * viewport by definition sets the default clip rectangle).
     */
    view.left     = 0;
    view.top      = 0;
    view.right    = dc->size.right - dc->size.left;
    view.bottom   = dc->size.bottom - dc->size.top;
    MGL_setViewport(view);

    /* Restore the old current device context */
    MGL_makeCurrentDC(oldDC);
}

