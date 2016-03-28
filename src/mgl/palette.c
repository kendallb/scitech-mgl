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
* Description:  Palette programming routines. Provides full support for
*               palette rotation and palette fading. If the current
*               video mode is a 15, 16 or 24 bit mode, we still support
*               the palette functions in order to provide color lookup
*               capabilities for programs in these modes using the
*               MGL_realColor() routine. Obviously there is no hardware
*               support, so palette rotations and palette fades will not
*               have an effect until the displays is redrawn :-)
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

GA_color _MGL_translate[256];           /* Palette translation vector       */

GA_color _MGL_halfToneTranslate[256] = {
    0x00,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,
    0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x32,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,
    0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,
    0x54,0x55,0x32,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,
    0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,
    0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,0x82,0x83,
    0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,
    0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,
    0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,
    0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,
    0xC4,0xC5,0xC6,0xC7,0xC8,0xC8,0xCA,0xCB,0xCC,0xCD,0xC8,0xCF,0xD0,0xD1,0xD2,0xD3,
    0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,
    0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,
    0xF4,0xF5,0xF6,0xF7,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    };

GA_color _MGL_linearTranslate[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
    };

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Returns the real packed MGL color for a color index.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to map color with
color   - Color to map

RETURNS:
Real packed MGL color value for the color index.

REMARKS:
This function returns a packed MGL color value appropriate for the specified device
context given a color index. This routine works with all device contexts, including
RGB device contexts. For the color index devices, the value is simply returned
unchanged. However for RGB devices, the color index is translated via the current
color palette for the device to find the appropriate packed MGL color value for that
device. Thus you can still write code for RGB devices that works with color indexes
(although you cannot do things like hardware palette fades and rotates as the palette
is implemented in software).

SEE ALSO:
MGL_setColorCI, MGL_setColorRGB, MGL_setPalette, MGL_getPalette
****************************************************************************/
color_t MGLAPI MGL_realColor(
    MGLDC *dc,
    int color)
{
    if (dc->mi.maxColor > 255)
        return dc->colorTab[color];
    else
        return color;
}

/****************************************************************************
DESCRIPTION:
Computes a packed MGL color from a 24 bit RGB tuple.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to map color with
R   - Red component of color to map (0 - 255)
G   - Green component of color to map (0 - 255)
B   - Blue component of color to map (0 - 255)

RETURNS:
Packed MGL color closest to specified RGB tuple.

REMARKS:
This function computes the packed MGL color value from a specific 24 bit RGB
tuple for a device context. If the device context is an RGB device context or an 8 bit
device in RGB dithered mode, this value simply returns the proper packed MGL
pixel value representing this color (the same as MGL_packColor would). However
if the device context is a color index device, the color palette is searched for the
color value that is the closest to the specified color. This function allows you to
specify a color given an RGB tuple, and will work in color index modes as well
with any color palette.

SEE ALSO:
MGL_realColor, MGL_setColorCI, MGL_setColorRGB
****************************************************************************/
color_t MGLAPI MGL_rgbColor(
    MGLDC *dc,
    uint R,
    uint G,
    uint B)
{
    if (dc->mi.maxColor > 255)
        return MGL_packColorFast(&dc->pf,R,G,B);
    else {
        palette_t   *pal = TO_PAL(dc->colorTab);
        uint        i;
        int         closest = 0;
        int         d,distance = 0x7FFFL;

        /* Look for closest match */
        for (i = 0; i <= dc->mi.maxColor; i++,pal++) {
            if ((d = ABS((int)R - (int)pal->red)
                   + ABS((int)G - (int)pal->green)
                   + ABS((int)B - (int)pal->blue)) < distance) {
                distance = d;
                closest = i;
                }
            }
        return closest;
        }
}

/****************************************************************************
DESCRIPTION:
Sets the current foreground color given a 24 bit RGB tuple.

HEADER:
mgraph.h

PARAMETERS:
R   - Red component of color (0 - 255)
G   - Green component of color (0 - 255)
B   - Blue component of color (0 - 255)

REMARKS:
This function sets the foreground color to a specific 24 bit RGB tuple. If the device
context is an RGB device context or an 8 bit device in RGB dithered mode, this
value simply sets the proper packed MGL pixel value representing this color (the
same as MGL_packColor would). However if the device context is a color index
device, the color palette is searched for the color value that is the closest to the
specified color. This function allows you to specify a color given an RGB tuple, and
will work in color index modes as well with any color palette.

SEE ALSO:
MGL_setColor, MGL_setColorCI, MGL_rgbColor
****************************************************************************/
void MGLAPI MGL_setColorRGB(
    uchar R,
    uchar G,
    uchar B)
{
    if (DC.mi.maxColor > 255)
        DC.r.SetForeColor(MGL_packColorFast(&DC.pf,R,G,B));
    else
        DC.r.SetForeColor(MGL_rgbColor(&DC,R,G,B));
}

/****************************************************************************
DESCRIPTION:
Sets the current foreground color given a color index.

HEADER:
mgraph.h

PARAMETERS:
index   - Color index of color to set

REMARKS:
Sets the current foreground color value given a color index. This routine works with
all device contexts, including RGB device contexts. For the color index devices, the
value for the foreground color is simply set unchanged. However for RGB devices,
the color index is translated via the current color palette for the device to find the
appropriate packed MGL color value for that device. Thus you can still write code
for RGB devices that works with color indexes.

SEE ALSO:
MGL_setColor, MGL_setColorRGB, MGL_realColor
****************************************************************************/
void MGLAPI MGL_setColorCI(
    int index)
{
    if (DC.mi.maxColor > 255)
        DC.r.SetForeColor(DC.colorTab[index]);
    else
        DC.r.SetForeColor(index);
}

/****************************************************************************
DESCRIPTION:
Sets a single palette entry.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to set palette entry in
entry   - Palette index to program
red     - Red component for palette entry
green   - Green component for palette entry
blue    - Blue component for palette entry

REMARKS:
Sets the color values of a single palette entry. If you wish to set more than a single
palette index you should use the

MGL_setPalette routine which is faster for multiple entries. Note that this routine
does not actually change the value of the hardware palette, and if you wish to
change the hardware palette to reflect the new values, you will need to call the
MGL_realizePalette function to update the hardware palette.

This function is also valid for RGB device contexts, and will simply set the color
translation tables for these devices (used for drawing color index bitmaps and
translating color index color values to RGB values).

SEE ALSO:
MGL_getPaletteEntry, MGL_setPalette, MGL_getPalette, MGL_realizePalette
****************************************************************************/
void MGLAPI MGL_setPaletteEntry(
    MGLDC *dc,
    int entry,
    uchar red,
    uchar green,
    uchar blue)
{
    if (dc->mi.maxColor > 255)
        dc->colorTab[entry] = MGL_packColor(&dc->pf,red,green,blue);
    else {
        palette_t *pal = TO_PAL(dc->colorTab) + entry;
        pal->red = red; pal->green = green; pal->blue = blue; pal->alpha = 0;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the value of a single palette entry.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context of interest
entry   - Palette index to read
red     - Place to store the red component
green   - Place to store the green component
blue    - Place to store the blue component

REMARKS:
This function returns the value of a single color palette entry. If you wish to obtain
more than a single palette entry you should use the MGL_getPalette routine which
is faster for multiple entries.

SEE ALSO:
MGL_setPaletteEntry, MGL_getPalette, MGL_setPalette
****************************************************************************/
void MGLAPI MGL_getPaletteEntry(
    MGLDC *dc,
    int entry,
    uchar *red,
    uchar *green,
    uchar *blue)
{
    if (dc->mi.maxColor > 255)
        MGL_unpackColor(&dc->pf,dc->colorTab[entry],red,green,blue);
    else {
        palette_t   *pal = TO_PAL(dc->colorTab) + entry;
        *red = pal->red;
        *green = pal->green;
        *blue = pal->blue;
        }
}

/****************************************************************************
DESCRIPTION:
Sets the palette values for a device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to set palette values for
pal         - Pointer to array of palette values to set
numColors   - Number of color values to set
startIndex  - Starting index of first color value to set

REMARKS:
This function sets part or all of the color palette for the device context. You can
specify only a subset of the palette values to be modified with the startIndex and
numColors arguments. Thus:

    MGL_setPalette(dc,pal,10,50);

will program the 10 color indices from 50-59 with the values stored in the
palette buffer 'pal'.

Note:   This routine does not actually change the value of
        the hardware palette.  If you wish to change the hardware palette
        to reflect the new values, you will need to call the
        MGL_realizePalette function to update the hardware palette.

Note:   You must ensure that you do not attempt to program invalid color
        indices! Use MGL_maxColor() to find the largest color index in
        color index modes.

Note:   This function is also valid for RGB device contexts, and will
        simply set the color translation tables for these devices (used
        for drawing color index bitmaps and translating color index color values to RGB values).

SEE ALSO:
MGL_getPalette, MGL_setPaletteEntry, MGL_realizePalette
****************************************************************************/
void MGLAPI MGL_setPalette(
    MGLDC *dc,
    palette_t *pal,
    int numColors,
    int startIndex)
{
    int         i;
    palette_t   *p;

    if (dc->mi.maxColor > 255) {
        for (i = startIndex,pal += startIndex; i < startIndex + numColors; i++,pal++)
            dc->colorTab[i] = MGL_packColor(&dc->pf,pal->red,pal->green,pal->blue);
        }
    else {
        /* We cant do a memcpy() as we must ensure that the alpha values are
         * always zero so that our palette searching code can run as
         * efficiently as possible.
         */
        p = TO_PAL(dc->colorTab) + startIndex;
        for (i = startIndex,pal += startIndex; i < startIndex + numColors; i++,pal++) {
            *p++ = *pal;
            pal->alpha = 0;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Returns the currently active palette values.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context of interest
pal         - Place to store the retrieved values
numColors   - Number of color values to retrieve
startIndex  - Starting palette index value to retrieve

REMARKS:
This function copies part or all of the currently active palette values and stores it in
the array pal. You can specify only a subset of the palette values to be obtained with
the startIndex and numColors arguments.

Thus to save the entire palette in a 256 color video mode, you would use (assuming
enough space for the palette has been allocated):

    MGL_getPalette(pal,255,0);

or to get the top half of the palette you would use:

    MGL_getPalette(pal,128,128);

You should ensure that you have allocated enough memory to hold all of the palette
values that you wish to read. You can use MGL_getPaletteSize to determine the
size required to save the entire palette.

SEE ALSO:
MGL_getPaletteEntry, MGL_setPalette, MGL_getDefaultPalette
****************************************************************************/
void MGLAPI MGL_getPalette(
    MGLDC *dc,
    palette_t *pal,
    int numColors,
    int startIndex)
{
    int i;

    if (dc->mi.maxColor > 255) {
        for (i = startIndex; i < startIndex + numColors; i++)
            MGL_unpackColor(&dc->pf,dc->colorTab[i],&pal[i].red,&pal[i].green,&pal[i].blue);
        }
    else
        memcpy(pal,TO_PAL(dc->colorTab) + startIndex,numColors * sizeof(palette_t));
}

/****************************************************************************
DESCRIPTION:
Returns the number of entries in the entire palette.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Number of entries in entire palette.

REMARKS:
This function returns the number of entries in the entire palette. You should use this
function to determine the size of the entire palette, since the palette is still available
in HiColor and TrueColor video modes. For RGB modes the palette is implemented
in software rather than hardware, and is used for translating color index values to
RGB color values, such as when displaying color index bitmaps in RGB modes.

SEE ALSO:
MGL_getPalette
****************************************************************************/
int MGLAPI MGL_getPaletteSize(
    MGLDC *dc)
{
    if (dc->mi.maxColor > 255)
        return 256;
    else
        return dc->mi.maxColor + 1;
}

/****************************************************************************
DESCRIPTION:
Returns the default palette for the device.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest
pal - Place to store the default palette values

REMARKS:
Copies the default palette for the specified device context into the passed palette
structure.

Note:   The size of the default palette can be found with a call to
        MGL_getPaletteSize().

SEE ALSO:
MGL_setPalette, MGL_getPalette
****************************************************************************/
void MGLAPI MGL_getDefaultPalette(
    MGLDC *dc,
    palette_t *pal)
{
    void *p = dc->r.getDefaultPalette(dc);
    if (p)
        memcpy(pal,p,MGL_getPaletteSize(dc) * sizeof(palette_t));
}

/****************************************************************************
DESCRIPTION:
Realizes the hardware color palette for the display device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to realize palette for
numColors   - Number of colors to realize
startIndex  - Starting index of first color to realize
waitVRT     - True if routine should sync to vertical retrace, false if not.

REMARKS:
This function realizes the hardware palette associated with a display device
context. Calls to MGL_setPalette only update the palette values in the color
palette for the device context structure, but do not actually program the
hardware palette for display device contexts in 4 and 8 bits per pixel modes.
In order to program the hardware palette you must call this routine.

When the hardware palette is realized, you normally need to sync to the
vertical retrace to ensure that the palette values are programmed without
the onset of snow (see MGL_setPaletteSnowLevel to adjust the number of
colors programmed per retrace period). If however you wish to perform double
buffered animation and change the hardware color palette at the same time,
you should call this routine immediately after calling either
MGL_setVisualPage or MGL_swapBuffers with the waitVRT flag set to false.

SEE ALSO:
MGL_setPalette, MGL_setVisualPage, MGL_swapBuffers, MGL_setPaletteSnowLevel
****************************************************************************/
void MGLAPI MGL_realizePalette(
    MGLDC *dc,
    int numColors,
    int startIndex,
    ibool waitVRT)
{
    if (dc->mi.maxColor <= 255)
        dc->r.realizePalette(dc,TO_PAL(dc->colorTab),numColors,startIndex,waitVRT);
}

/****************************************************************************
DESCRIPTION:
Resets the palette to the default values.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

REMARKS:
Sets the palette to the current MGL default values for the device context. This can
be used to reset the palette to the original default values that the palette is
programmed with when MGL is initialized.

SEE ALSO:
MGL_getDefaultPalette, MGL_setPalette, MGL_getPalette
****************************************************************************/
void MGLAPI MGL_setDefaultPalette(
    MGLDC *dc)
{
    palette_t   pal[256];

    MGL_getDefaultPalette(dc,pal);
    MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
}

/****************************************************************************
DESCRIPTION:
Map the colors of a memory device context to match a new palette.

HEADER:
mgraph.h

PARAMETERS:
dc  - Memory device context to map (8 bits per pixel only)
pal - New palette to map to

REMARKS:
This function maps the pixels of an 8 bits per pixel memory device context to the
specified palette, and then sets the palette for the device context to the new palette.
This function actually translates every pixel in the device context's surface to the
new palette, by looking for the entry in the new palette that is the closest to color of
the original pixel in the old palette (the one currently active before this routine was
called).

SEE ALSO:
MGL_setPalette
****************************************************************************/
void MGLAPI MGL_mapToPalette(
    MGLDC *dc,
    palette_t *pal)
{
    palette_t   devPal[256];
    int         i,count = dc->mi.xRes+1,height = dc->mi.yRes+1;
    uchar       *s,*s1;
    uchar       *p,*buf = _MGL_buf,*endp = buf + count;

    /* Only 8bpp device contexts are supported by this function */
    if (!(dc->mi.modeFlags & MGL_IS_COLOR_INDEX))
        return;

    /* Read the current palette and use as source palette for translate */
    MGL_getPalette(dc,devPal,256,0);

    /* Set the final color palette in the bitmap */
    MGL_setPalette(dc,pal,256,0);
    MGL_realizePalette(dc,256,0,false);

    /* Build translation vector to map between the palettes */
    _MGL_buildTranslateVector(_MGL_translate,256,devPal,256,(palette_t*)dc->colorTab);

    /* Translate the bitmap data in place */
    MGL_beginDirectAccessDC(dc);
    s = (uchar*)dc->surface;
    for (i = 0; i < height; i++, s += dc->mi.bytesPerLine) {
        s1 = (uchar*)s; p = buf;
        while (p != endp)
            *p++ = (uchar)_MGL_translate[*s1++];
        MGL_memcpy(s,buf,count);
        }
    MGL_endDirectAccessDC(dc);
}

/****************************************************************************
DESCRIPTION:
Sets the hardware gamma correction ramp.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to set gamma ramp values for
gamma       - Pointer to array of gamma ramp values to set
num         - Number of gamma entries to set
index       - Starting index of first gamma entry to set
waitVRT     - True if routine should sync to vertical retrace, false if not.

REMARKS:
This function sets part or all of the hardware gamma correction ramp for
a device context. You can specify only a subset of the gamma ramp entried to
be modified with the startIndex and numColors arguments. The changes to
the gamma ramp take place immediately.

Note:   This function is only valid for RGB display modes, and will do
        nothing in color index modes.

SEE ALSO:
MGL_getGammaRamp
****************************************************************************/
ibool MGLAPI MGL_setGammaRamp(
    MGLDC *dc,
    palette_ext_t *gamma,
    int num,
    int index,
    ibool waitVRT)
{
    if (dc->r.setGammaRamp) {
        dc->r.setGammaRamp(dc,gamma,num,index,waitVRT);
        return true;
        }
    return false;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active hardware gamma correction ramp.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context of interest
gamma   - Place to store the retrieved values
num     - Number of gamma entries to retrieve
index   - Starting gamma entry index to retrieve

REMARKS:
This function copies part or all of the currently active hardware gamma
correction ramp and stores it in the gamma array. You can specify only a
subset of the gamma entries to be retrieved with the startIndex and
numColors arguments.

SEE ALSO:
MGL_setGammaRamp
****************************************************************************/
ibool MGLAPI MGL_getGammaRamp(
    MGLDC *dc,
    palette_ext_t *gamma,
    int num,
    int index)
{
    if (dc->r.getGammaRamp) {
        dc->r.getGammaRamp(dc,gamma,num,index);
        return true;
        }
    return false;
}

/****************************************************************************
PARAMETERS:
color   - RGB color palette value to search for
pal     - Pointer to color lookup table to scan
palSize - Size of the color lookup table

RETURNS:
Color index of the closest color to the specified RGB value

REMARKS:
This routine searches a color palette for the color that is the closest to
the specified RGB color value.
{secret}
****************************************************************************/
int _MGL_closestColor(
    palette_t *color,
    palette_t *pal,
    int palSize)
{
    int         i,closest = 0;
    int         d,distance = 0x7FFFL;

    /* Look for closest match */
    for (i = 0; i < palSize; i++,pal++) {
        if ((d = ABS((int)color->red - (int)pal->red)
               + ABS((int)color->green - (int)pal->green)
               + ABS((int)color->blue - (int)pal->blue)) < distance) {
            distance = d;
            closest = i;
            }
        }
    return closest;
}

/****************************************************************************
DESCRIPTION:
Rotates the palette values for a device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context containing palette
numColors   - Number of colors to rotate
startIndex  - Starting index for colors to rotate
direction   - Direction to rotate the palette entries

REMARKS:
This function rotates the palette values in the device context in the specified
direction. Note that this routine does not effect the currently active hardware
palette, and you must call MGL_realizePalette in order to make the program the
rotated palette to the hardware.

Supported directions of rotation are enumerated in MGL_palRotateType.

When the direction specified is MGL_ROTATE_UP, the first entry in the palette is
moved to the last position in the palette, and all the remaining entries are moved
one position up in the array. If the direction specified is MGL_ROTATE_DOWN,
the last entry is moved into the first entry of the palette, and the remaining entries
are all moved one position down in the array.

SEE ALSO:
MGL_setPalette, MGL_getPalette, MGL_fadePalette
****************************************************************************/
void MGLAPI MGL_rotatePalette(
    MGLDC *dc,
    int numColors,
    int startIndex,
    int direction)
{
    palette_t   *p,t;

    numColors--;
    if (direction == MGL_ROTATE_DOWN) {
        p = &TO_PAL(dc->colorTab)[startIndex];
        t = p[numColors];
        memmove(p+1,p,numColors*sizeof(palette_t));
        p[0] = t;
        }
    else {
        p = &TO_PAL(dc->colorTab)[startIndex];
        t = p[0];
        memmove(p,p+1,numColors*sizeof(palette_t));
        p[numColors] = t;
        }
}

/****************************************************************************
DESCRIPTION:
Fades the values for a color palette.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context with palette to fade
fullIntensity   - Pointer to full intensity palette to fade from
numColors   - Number of colors in palette to fade
startIndex  - Starting index of first color to fade
intensity   - Intensity for the final output palette (0 - 255)

RETURNS:
True if the entire output palette is black, false if not.

REMARKS:
This routine will take the values from a full intensity palette_t structure, fade the
values and store them into a device context palette. The actual hardware palette will
not be programmed at this stage, so you will then need to make a call to
MGL_realizePalette to make the changes visible.

The intensity value is a number between 0 and 255 that defines the intensity of the
output values. An intensity of 255 will produce the same output values as the input
values. An intensity of 128 will product values in the output palette that are half the
intensity of the input palette and an intensity of 0 produces an all black palette.

If the entire output palette is black, then the routine will return true, otherwise it will
return false.

SEE ALSO:
MGL_setPalette, MGL_getPalette, MGL_rotatePalette, MGL_realizePalette
****************************************************************************/
ibool MGLAPI MGL_fadePalette(
    MGLDC *dc,
    palette_t *fullIntensity,
    int numColors,
    int startIndex,
    uchar intensity)
{
    uchar   allBlack = 0,*p,*fi;
    int     i;

    p = (uchar*)&TO_PAL(dc->colorTab)[startIndex];
    fi = (uchar*)fullIntensity;
    for (i = 0; i < numColors; i++) {
        *p = (*fi++ * intensity) / (uchar)255;
        allBlack |= *p++;
        *p = (*fi++ * intensity) / (uchar)255;
        allBlack |= *p++;
        *p = (*fi++ * intensity) / (uchar)255;
        allBlack |= *p++;
        fi++; p++;
        }
    return !allBlack;
}
