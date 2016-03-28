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
* Description:  Pixel manipulation routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*------------------------- Implementation --------------------------------*/

#undef  MGL_pixelFast
#undef  MGL_pixel
#undef  MGL_getPixelFast
#undef  MGL_getPixel

/****************************************************************************
DESCRIPTION:
Draws a pixel at the specified location as fast as possible.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to plot pixel at
y   - y coordinate to plot pixel at

REMARKS:
Plots a single pixel at the specified location in the current foreground color. This
routine is designed to allow plotting of multiple pixels as fast as possible.

Note that you must call MGL_beginPixel before calling this function, and you must
call MGL_endPixel after you have finished plotting a number of pixels.

SEE ALSO:
MGL_pixelFast, MGL_beginPixel, MGL_endPixel.
****************************************************************************/
void MGLAPI MGL_pixelCoordFast(
    int x,
    int y)
{
    x += DC.viewPort.left;
    y += DC.viewPort.top;
    if (DC.clipRegionScreen && !MGL_ptInRegionCoord(x,y,DC.clipRegionScreen))
        return;
    else if (!MGL_ptInRectCoord(x,y,DC.clipRectScreen))
        return;
    DC.r.PutPixel(x,y);
}

/****************************************************************************
DESCRIPTION:
Draws a pixel at the specified location as fast as possible.

HEADER:
mgraph.h

PARAMETERS:
p   - Point to plot pixel at

REMARKS:
This function is the same as MGL_pixelCoordFast, however it takes the coordinate
of the pixel to plot as a point.

SEE ALSO:
MGL_pixelCoordFast, MGL_beginPixel, MGL_endPixel.
****************************************************************************/
void MGL_pixelFast(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Returns the color of a specified pixel.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate of the pixel to read
y   - y coordinate of the pixel to read

RETURNS:
Color of the specified pixel.

REMARKS:
This function returns the color of the pixel at the specified coordinate. Note that you
must ensure that you call the routine MGL_beginPixel before reading any pixel
values and the routine MGL_endPixel after reading a bunch of pixels with these fast
pixel routines.

SEE ALSO:
MGL_getPixelFast, MGL_beginPixel, MGL_endPixel
****************************************************************************/
color_t MGLAPI MGL_getPixelCoordFast(
    int x,
    int y)
{
    x += DC.viewPort.left;
    y += DC.viewPort.top;
    if (DC.clipRegionScreen && !MGL_ptInRegionCoord(x,y,DC.clipRegionScreen))
        return (color_t)-1;
    else if (!MGL_ptInRectCoord(x,y,DC.clipRectScreen))
        return (color_t)-1;
    return DC.r.GetPixel(x,y);
}

/****************************************************************************
DESCRIPTION:
Returns the color of a specified pixel.

HEADER:
mgraph.h

PARAMETERS:
p   - Coordinate of the pixel to read

RETURNS:
Color of the specified pixel.

REMARKS:
This function is the same as MGL_getPixelCoordFast, however it takes the
coordinate of the pixel to read as a point.

SEE ALSO:
MGL_getPixelCoordFast, MGL_beginPixel, MGL_endPixel
****************************************************************************/
color_t MGL_getPixelFast(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Draws a pixel at the specified location.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate to plot pixel at
y   - y coordinate to plot pixel at

REMARKS:
Plots a single pixel at the specified location in the current foreground color.

SEE ALSO:
MGL_pixel
****************************************************************************/
void MGLAPI MGL_pixelCoord(
    int x,
    int y)
{
    MGL_beginPixel();
    MGL_pixelCoordFast(x,y);
    MGL_endPixel();
}

/****************************************************************************
DESCRIPTION:
Draws a pixel at the specified location.

HEADER:
mgraph.h

PARAMETERS:
p   - Point to plot pixel at

REMARKS:
This function is the same as MGL_pixelFast, however it takes the
coordinate of the pixel to plot as a point.

SEE ALSO:
MGL_pixelCoord
****************************************************************************/
void MGL_pixel(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Returns the color of a specified pixel.

HEADER:
mgraph.h

PARAMETERS:
x   - x coordinate of the pixel to read
y   - y coordinate of the pixel to read

RETURNS:
Color of the specified pixel.

REMARKS:
This function returns the color of the pixel at the specified coordinate.

SEE ALSO:
MGL_getPixel, MGL_beginPixel, MGL_endPixel
****************************************************************************/
color_t MGLAPI MGL_getPixelCoord(
    int x,
    int y)
{
    int v;
    MGL_beginPixel();
    v = MGL_getPixelCoordFast(x,y);
    MGL_endPixel();
    return v;
}

/****************************************************************************
DESCRIPTION:
Returns the color of a specified pixel.

HEADER:
mgraph.h

PARAMETERS:
p   - Coordinate of the pixel to read

RETURNS:
Color of the specified pixel.

REMARKS:
This function is the same as MGL_getPixelCoord, however it takes the
coordinate of the pixel to read as a point.

SEE ALSO:
MGL_getPixelCoord, MGL_beginPixel, MGL_endPixel
****************************************************************************/
color_t MGL_getPixel(
    point_t p);
/* Implemented as a macro */

/****************************************************************************
PARAMETERS:
x   - x coordinate to plot pixel at
y   - y coordinate to plot pixel at

REMARKS:
Internal routine to draw a pixel with coordinates passed in screen space.
This is used internally when we need to clip pixels correctly to the
screen but the pixel coordinates are already in screen space.
{secret}
****************************************************************************/
void MGLAPI _MGL_clipPixel(
    N_int32 x,
    N_int32 y)
{
    if (DC.clipRegionScreen && !MGL_ptInRegionCoord(x,y,DC.clipRegionScreen))
        return;
    else if (!MGL_ptInRectCoord(x,y,DC.clipRectScreen))
        return;
    DC.r.PutPixel(x,y);
}

