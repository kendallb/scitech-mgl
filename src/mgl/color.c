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
* Description:  Color manipulation routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Pack an RGB tuple into an MGL color.

HEADER:
mgraph.h

PARAMETERS:
pf  - Pixel format to get packing information from
R   - Red color component to pack (0 - 255)
G   - Green color component to pack (0 - 255)
B   - Blue color component to pack (0 - 255)

RETURNS:
MGL packed color value appropriate for the specified pixel format information.

REMARKS:
This function takes an RGB tuple of 8 bit color components and packs them
into an MGL packed color value. The color components are packed according
to the specified pixel format information, which can be obtained directly
from the mode information for a bitmap or an MGL device context.

SEE ALSO:
MGL_packColorFast, MGL_packColorExt, MGL_unpackColor, MGL_getPixelFormat
****************************************************************************/
color_t MGLAPI MGL_packColor(
    pixel_format_t *pf,
    uchar R,
    uchar G,
    uchar B)
{
    return MGL_packColorFast(pf,R,G,B);
}

/****************************************************************************
DESCRIPTION:
Pack an RGB with Alpha tuple into an MGL color.

HEADER:
mgraph.h

PARAMETERS:
pf  - Pixel format to get packing information from
A   - Alpha component to pack (0 - 255)
R   - Red color component to pack (0 - 255)
G   - Green color component to pack (0 - 255)
B   - Blue color component to pack (0 - 255)

RETURNS:
MGL packed color value appropriate for the specified pixel format information.

REMARKS:
This function takes an RGB with Alpha tuple of 8 bit color components and
packs them into an MGL packed color value. The color components are packed
according to the specified pixel format information, which can be obtained
directly from the mode information for a bitmap or an MGL device context.

SEE ALSO:
MGL_packColorFast, MGL_packColor, MGL_unpackColor, MGL_getPixelFormat
****************************************************************************/
color_t MGLAPI MGL_packColorExt(
    pixel_format_t *pf,
    uchar A,
    uchar R,
    uchar G,
    uchar B)
{
    return MGL_packColorFastExt(pf,A,R,G,B);
}

/****************************************************************************
DESCRIPTION:
Unpacks a packed MGL color value into RGB components.

HEADER:
mgraph.h

PARAMETERS:
pf      - Pixel format to use for unpacking
color   - Color to unpack
R       - Place to store extracted red component
G       - Place to store extracted green component
B       - Place to store extracted blue component

REMARKS:
This function takes a packed color value in the correct format for the
specified pixel format and extracts the red, green and blue components. Note
that the color values may not be the same as when you packed them with
MGL_packColor if the pixel format is a 15 or 16 bit format because of loss
of precision. The values are scaled back into the normal 24 bit RGB space.

SEE ALSO:
MGL_unpackColorFast, MGL_unpackColorExt, MGL_packColor, MGL_getPixelFormat
****************************************************************************/
void MGLAPI MGL_unpackColor(
    pixel_format_t *pf,
    color_t color,
    uchar *R,
    uchar *G,
    uchar *B)
{
    MGL_unpackColorFast(pf,color,*R,*G,*B);
}

/****************************************************************************
DESCRIPTION:
Unpacks a packed MGL color value into RGB components.

HEADER:
mgraph.h

PARAMETERS:
pf      - Pixel format to use for unpacking
color   - Color to unpack
A       - Place to store extracted alpha component
R       - Place to store extracted red component
G       - Place to store extracted green component
B       - Place to store extracted blue component

REMARKS:
This function takes a packed color value in the correct format for the
specified pixel format and extracts the red, green and blue components. Note
that the color values may not be the same as when you packed them with
MGL_packColor if the pixel format is a 15 or 16 bit format because of loss
of precision. The values are scaled back into the normal 24 bit RGB space.

SEE ALSO:
MGL_unpackColorFast, MGL_unpackColorExt, MGL_packColor, MGL_getPixelFormat
****************************************************************************/
void MGLAPI MGL_unpackColorExt(
    pixel_format_t *pf,
    color_t color,
    uchar *A,
    uchar *R,
    uchar *G,
    uchar *B)
{
    MGL_unpackColorFastExt(pf,color,*A,*R,*G,*B);
}

#undef MGL_packColorFast
#undef MGL_packColorFastExt
#undef MGL_unpackColorFast
#undef MGL_unpackColorFastExt

/****************************************************************************
DESCRIPTION:
Pack an RGB tuple into an MGL color.

HEADER:
mgraph.h

PARAMETERS:
pf  - Pixel format to get packing information from
R   - Red color component to pack (0 - 255)
G   - Green color component to pack (0 - 255)
B   - Blue color component to pack (0 - 255)

RETURNS:
MGL packed color value appropriate for the specified pixel format information.

REMARKS:
This function is the same as MGL_packColor, however it is implemented as a
macro and hence is more efficient.

SEE ALSO:
MGL_packColorFastExt, MGL_packColor, MGL_unpackColor
****************************************************************************/
color_t MGL_packColorFast(
    pixel_format_t *pf,
    uchar R,
    uchar G,
    uchar B);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Pack an RGB with Alpha tuple into an MGL color.

HEADER:
mgraph.h

PARAMETERS:
pf  - Pixel format to get packing information from
A   - Alpha component to pack (0 - 255)
R   - Red color component to pack (0 - 255)
G   - Green color component to pack (0 - 255)
B   - Blue color component to pack (0 - 255)

RETURNS:
MGL packed color value appropriate for the specified pixel format information.

REMARKS:
This function is the same as MGL_packColorExt, however it is implemented as a
macro and hence is more efficient.

SEE ALSO:
MGL_packColorFast, MGL_packColor, MGL_unpackColorExt
****************************************************************************/
color_t MGL_packColorFastExt(
    pixel_format_t *pf,
    uchar A,
    uchar R,
    uchar G,
    uchar B);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Unpacks a packed MGL color value into RGB components.

HEADER:
mgraph.h

PARAMETERS:
pf      - Pixel format to use for unpacking
color   - Color to unpack
R       - Place to store extracted red component
G       - Place to store extracted green component
B       - Place to store extracted blue component

REMARKS:
This function is the same as MGL_unpackColor, however it is implemented as
a macro and hence is more efficient.

SEE ALSO:
MGL_unpackColorExt, MGL_unpackColor, MGL_packColor, MGL_getPixelFormat
****************************************************************************/
void MGL_unpackColorFast(
    pixel_format_t *pf,
    color_t color,
    uchar R,
    uchar G,
    uchar B);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Unpacks a packed MGL color value into RGB components.

HEADER:
mgraph.h

PARAMETERS:
pf      - Pixel format to use for unpacking
color   - Color to unpack
A       - Place to store extracted alpha component
R       - Place to store extracted red component
G       - Place to store extracted green component
B       - Place to store extracted blue component

REMARKS:
This function is the same as MGL_unpackColor, however it is implemented as
a macro and hence is more efficient.

SEE ALSO:
MGL_unpackColorExt, MGL_unpackColor, MGL_packColor, MGL_getPixelFormat
****************************************************************************/
void MGL_unpackColorFastExt(
    pixel_format_t *pf,
    color_t color,
    uchar A,
    uchar R,
    uchar G,
    uchar B);
/* Implemented as a macro */
