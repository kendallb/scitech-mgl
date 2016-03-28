/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Language:     C++ 3.0
* Environment:  Any
*
* Description:  Module to precompile all of the header file required by
*               the library. This is pointless during development, but
*               can significantly speed up compilation times when using
*               the library.
*
*               You should include this file as the first file in your
*               project or makefile, to ensure that it gets compiled first,
*               with the same set of macros and compilation settings.
*
****************************************************************************/

#include "mvis/mvision.hpp"

/*--------------------------- Global Variables ----------------------------*/

MVHotKey    _MV_noHotKey(kbNoKey,0);
MVRect      _MV_emptyRect(0,0,0,0);
MVRect      _MV_pointRect(0,0,1,1);
int         _MV_systemFont;
int         _MV_systemFontSize;
int         _MV_sysScrollBarWidth;
int         _MV_iconOffset;
static int  bdrDark,bdrBright;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
left    - Left edge of rectangle
top - Top edge of rectangle
right   - Right edge of rectangle
bottom  - Bottom edge of rectangle
style   - Style of border to draw (INSET, OUTSET etc)
thickness   - Thickness of the border in pixels

REMARKS:
If the style is set to BDR_INSET, the border is colored so that it look
like the interior of the rectangle is inset into the display. If style is set
to BDR_OUTSET, the border is colored so that the interior looks like it is
raised. If style is set to OUTLINE, the border is drawn as a 3d 2 pixels thick
outline.
****************************************************************************/
void MV_drawBorderCoord(
    int left,
    int top,
    int right,
    int bottom,
    int style,
    int thickness)
{
    int     i;
    color_t color = MGL_getColor();
    color_t bright,dark;

    if (bottom <= top+1 || right <= left+1)
        return;

    switch (style) {
        case MV_BDR_INSET:
            bright = bdrDark;
            dark = bdrBright;
            break;
        case MV_BDR_OUTSET:
            bright = bdrBright;
            dark = bdrDark;
            break;
        default:    /* MV_BDR_OUTLINE: */
            MGL_setColor(bdrBright);
            MGL_rectCoord(left+1,top+1,right,bottom);
            MGL_setColor(bdrDark);
            MGL_rectCoord(left,top,right-1,bottom-1);
            MGL_setColor(color);
            return;
        }

    MGL_setColor(bright);
    for (i = 0; i < thickness; i++) {
        MGL_lineCoord(left, top+i, right-i-1, top+i);
        MGL_lineCoord(left+i, top, left+i, bottom-i-1);
        }

    MGL_setColor(dark);
    for (i = 0; i < thickness; i++) {
        MGL_lineCoord(right-i-1, top+i+1, right-i-1, bottom-1);
        MGL_lineCoord(left+i+1, bottom-i-1, right-1, bottom-i-1);
        }
    MGL_setColor(color);
}

/****************************************************************************
PARAMETERS:
bright  - Color for the bright component of the border color
dark    - Color for the dark component of the border color

REMARKS:
The border colors are the two colors used to draw horizontal, vertical
and rectangular borders with the MGL_drawBorder function.
****************************************************************************/
void MV_setBorderColors(
    color_t bright,
    color_t dark)
{
    bdrBright = bright;
    bdrDark = dark;
}
