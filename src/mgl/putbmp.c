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
* Description:  MGL_putBitmap and related routines.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Draw a monochrome bitmap at the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to draw bitmap on
x           - x coordinate to draw bitmap at
y           - y coordinate to draw bitmap at
width       - Width of the bitmap to draw in pixels
byteWidth   - Width of the bitmap image in bytes
height      - Height of the bitmap in scanlines
image       - Pointer to the buffer holding the bitmap

REMARKS:
This function draws a monochrome bitmap in the current foreground color on the
current device context. Where a bit is a 1 in the bitmap definition, a pixel is plotted
in the foreground color, where a bit is a 0 the original pixels are left alone. This
function can be used to implement fast hardware pixel masking for drawing fast
transparent bitmaps on devices that do not have a native hardware transparent BitBlt
function.
****************************************************************************/
void MGLAPI MGL_putMonoImage(
    MGLDC *dc,
    int x,
    int y,
    int width,
    int byteWidth,
    int height,
    void *image)
{
    rect_t  d,r,clip;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = x;             d.top = y;
    d.right = x + width;    d.bottom = y + height;
    if (!MGL_sectRect(dc->clipRectView,d,&r)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Now draw the bitmap */
    MAKE_HARDWARE_CURRENT(dc,false);
    if (dc->clipRegionScreen) {
        /* Draw it clipped to a complex clip region */
        x += dc->viewPort.left;
        y += dc->viewPort.top;
        d.left = x;             d.top = y;
        d.right = x + width;    d.bottom = y + height;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (!MGL_equalRect(d,r)) {
                    /* Draw it clipped to the clip rectangle */
                    dc->r.ClipMonoImage_Old(x,y,width,height,byteWidth,image,dc->a.backMode,
                        r.left,r.top,
                        r.right,r.bottom);
                    }
                else {
                    /* Draw it unclipped for maximum speed */
                    dc->r.PutMonoImage_Old(x,y,width,height,byteWidth,image,dc->a.backMode);
                    }
                }
        END_CLIP_REGION();
        }
    else if (!MGL_equalRect(d,r)) {
        /* Draw it clipped to the clip rectangle */
        dc->r.ClipMonoImage_Old(x+dc->viewPort.left,y+dc->viewPort.top,
            width,height,byteWidth,image,dc->a.backMode,
            dc->clipRectScreen.left,dc->clipRectScreen.top,
            dc->clipRectScreen.right,dc->clipRectScreen.bottom);
        }
    else {
        /* Draw it unclipped for maximum speed */
        dc->r.PutMonoImage_Old(x+dc->viewPort.left,y+dc->viewPort.top,
            width,height,byteWidth,image,dc->a.backMode);
        }
    RESTORE_HARDWARE(dc,false);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a lightweight bitmap at the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to display bitmap on
x       - x coordinate to draw bitmap at
y       - y coordinate to draw bitmap at
bitmap  - Bitmap to display
op      - Write mode to use when drawing bitmap

REMARKS:
Draws a lightweight bitmap at the specified location. The bitmap can be
in any color format, and will be translated as necessary to the color format
required by the current device context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

Supported write modes are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmap(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *bitmap,
    int op)
{
    rect_t      d,r,clip;
    int         left,top,right,bottom,srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = x;                     d.top = y;
    d.right = x + bitmap->width;    d.bottom = y + bitmap->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Now blit the bitmap */
    if (bitmap->bitsPerPixel == 1) {
        /* Bitmap is a monochrome bitmap, so simply draw this using the
         * normal mono bitmap blitting code, but set the requested write
         * mode and colors first. Clipping is handled by the device driver
         * rendering.
         */
        int         oldop = dc->a.writeMode;
        color_t     oldColor = dc->a.color;
        color_t     oldBackColor = dc->a.backColor;
        palette_t   *pal = bitmap->pal;

        MAKE_HARDWARE_CURRENT(dc,false);
        dc->r.SetMix(op);
        if (pal) {
            dc->r.SetForeColor(MGL_rgbColor(dc,pal[1].red,pal[1].green,pal[1].blue));
            dc->r.SetBackColor(MGL_rgbColor(dc,pal[0].red,pal[0].green,pal[0].blue));
            }
        x += dc->viewPort.left;
        y += dc->viewPort.top;
        d.left = x;                     d.top = y;
        d.right = x + bitmap->width;    d.bottom = y + bitmap->height;
        if (dc->clipRegionScreen) {
            /* Draw it clipped to a complex clip region */
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    if (!MGL_equalRect(d,r)) {
                        /* Draw it clipped to the clip rectangle */
                        dc->r.ClipMonoImage_Old(x,y,bitmap->width,bitmap->height,
                            bitmap->bytesPerLine,bitmap->surface,dc->a.backMode,
                            r.left,r.top,
                            r.right,r.bottom);
                        }
                    else {
                        /* Draw it unclipped for maximum speed */
                        dc->r.PutMonoImage_Old(x,y,bitmap->width,bitmap->height,
                            bitmap->bytesPerLine,bitmap->surface,dc->a.backMode);
                        }
                    }
            END_CLIP_REGION();
            }
        else if (MGL_sectRect(dc->clipRectScreen,d,&r)) {
            if (!MGL_equalRect(d,r)) {
                /* Draw it clipped to the clip rectangle */
                dc->r.ClipMonoImage_Old(x,y,
                    bitmap->width,bitmap->height,bitmap->bytesPerLine,
                    bitmap->surface,dc->a.backMode,
                    dc->clipRectScreen.left,dc->clipRectScreen.top,
                    dc->clipRectScreen.right,dc->clipRectScreen.bottom);
                }
            else {
                /* Draw it unclipped for maximum speed */
                dc->r.PutMonoImage_Old(x,y,bitmap->width,bitmap->height,bitmap->bytesPerLine,bitmap->surface,dc->a.backMode);
                }
            }
        dc->r.SetMix(oldop);
        dc->r.SetForeColor(oldColor);
        dc->r.SetBackColor(oldBackColor);
        RESTORE_HARDWARE(dc,false);
        }
    else {
        /* Adjust to bitmap coordinates */
        MGL_offsetRect(d,-x,-y);
        x += d.left;
        y += d.top;
        MAKE_HARDWARE_CURRENT(dc,true);
        if (NEED_TRANSLATE_BM(bitmap,dc)) {
            /* Translate the pixel information when doing the Blt */
            gaFx.dwSize = sizeof(gaFx);
            gaFx.Flags = gaBltConvert | gaBltMixEnable;
            gaFx.Mix = op;
            gaFx.BitsPerPixel = bitmap->bitsPerPixel;
            gaFx.PixelFormat = GAPF(bitmap->pf);
            gaFx.SrcPalette = GAPAL(bitmap->pal);
            gaFx.DstPalette = GAPAL(dc->colorTab);
            SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                gaFx.TranslateVec = dstPal;
                }
            else if (ditherMode)
                gaFx.Flags |= gaBltDither;
            if (dc->clipRegionScreen) {
                left = d.left;
                top = d.top;
                right = d.right;
                bottom = d.bottom;
                d.left = x + dc->viewPort.left;
                d.top = y + dc->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                        }
                END_CLIP_REGION();
                }
            else {
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,
                    x + dc->viewPort.left,y + dc->viewPort.top,
                    d.right-d.left,d.bottom-d.top,&gaFx);
                }
            }
        else {
            if (dc->clipRegionScreen) {
                left = d.left;
                top = d.top;
                right = d.right;
                bottom = d.bottom;
                d.left = x + dc->viewPort.left;
                d.top = y + dc->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dc->r.BitBltSys(bitmap->surface,bitmap->bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,op,false);
                        }
                END_CLIP_REGION();
                }
            else {
                dc->r.BitBltSys(bitmap->surface,bitmap->bytesPerLine,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,
                    x + dc->viewPort.left,y + dc->viewPort.top,op,false);
                }
            }
        RESTORE_HARDWARE(dc,true);
        }
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a section of a lightweight bitmap at the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to display bitmap on
left    - Left coordinate of section to draw
top     - Top coordinate of section to draw
right   - Right coordinate of section to draw
bottom  - Bottom coordinate of section to draw
dstLeft - Left coordinate of destination of bitmap section
dstTop  - Right coordinate for destination of bitmap section
bitmap  - Bitmap to display
op      - Write mode to use when drawing bitmap

REMARKS:
Draws a section of a lightweight bitmap at the specified location. The bitmap
can be in any color format, and will be translated as necessary to the color
format required by the current device context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

Supported write modes are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    const bitmap_t *bitmap,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip the source rectangle to the bitmap dimensions */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (!MGL_sectRectCoord(0,0,bitmap->width,bitmap->height,left,top,right,bottom,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Blt the pixels to the device */
    if (bitmap->bitsPerPixel == 1) {
        int         oldop = dc->a.writeMode;
        color_t     oldColor = dc->a.color;
        color_t     oldBackColor = dc->a.backColor;
        palette_t   *pal = bitmap->pal;

        MAKE_HARDWARE_CURRENT(dc,false);
        dc->r.SetMix(op);
        if (pal) {
            dc->r.SetForeColor(MGL_rgbColor(dc,pal[1].red,pal[1].green,pal[1].blue));
            dc->r.SetBackColor(MGL_rgbColor(dc,pal[0].red,pal[0].green,pal[0].blue));
            }
        if (dc->clipRegionScreen) {
            /* Draw it clipped to a complex clip region */
            dstLeft += dc->viewPort.left - left;
            dstTop += dc->viewPort.top - top;
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    /* Draw it clipped to the clip rectangle */
                    dc->r.ClipMonoImage_Old(
                        dstLeft,dstTop,bitmap->width,bitmap->height,
                        bitmap->bytesPerLine,bitmap->surface,dc->a.backMode,
                        d.left,d.top,d.right,d.bottom);
                    }
            END_CLIP_REGION();
            }
        else {
            /* Draw it clipped to the clip rectangle */
            dc->r.ClipMonoImage_Old(
                dstLeft+dc->viewPort.left-left,dstTop+dc->viewPort.top-top,
                bitmap->width,bitmap->height,bitmap->bytesPerLine,
                bitmap->surface,dc->a.backMode,
                d.left+dc->viewPort.left,d.top+dc->viewPort.top,
                d.right+dc->viewPort.left,d.bottom+dc->viewPort.top);
            }
        dc->r.SetMix(oldop);
        dc->r.SetForeColor(oldColor);
        dc->r.SetBackColor(oldBackColor);
        RESTORE_HARDWARE(dc,false);
        }
    else {
        dstLeft = d.left;
        dstTop = d.top;
        MAKE_HARDWARE_CURRENT(dc,true);
        if (NEED_TRANSLATE_BM(bitmap,dc)) {
            /* Translate the pixel information when doing the Blt */
            gaFx.dwSize = sizeof(gaFx);
            gaFx.Flags = gaBltConvert | gaBltMixEnable;
            gaFx.Mix = op;
            gaFx.BitsPerPixel = bitmap->bitsPerPixel;
            gaFx.PixelFormat = GAPF(bitmap->pf);
            gaFx.SrcPalette = GAPAL(bitmap->pal);
            gaFx.DstPalette = GAPAL(dc->colorTab);
            SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                gaFx.TranslateVec = dstPal;
                }
            else if (ditherMode)
                gaFx.Flags |= gaBltDither;
            if (dc->clipRegionScreen) {
                d.left = dstLeft + dc->viewPort.left;
                d.top = dstTop + dc->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                        }
                END_CLIP_REGION();
                }
            else {
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    left,top,right-left,bottom-top,
                    dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                    right-left,bottom-top,&gaFx);
                }
            }
        else {
            if (dc->clipRegionScreen) {
                d.left = dstLeft + dc->viewPort.left;
                d.top = dstTop + dc->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dc->r.BitBltSys(bitmap->surface,bitmap->bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,op,false);
                        }
                END_CLIP_REGION();
                }
            else {
                dc->r.BitBltSys(bitmap->surface,bitmap->bytesPerLine,
                    left,top,right-left,bottom-top,
                    dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,op,false);
                }
            }
        RESTORE_HARDWARE(dc,true);
        }
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a lightweight bitmap mask in the specified color.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to display bitmap on
x       - x coordinate to draw bitmap at
y       - y coordinate to draw bitmap at
mask    - Monochrome bitmap mask to display
color   - Color to draw in
op      - Write mode to use when drawing bitmap

REMARKS:
Draws a lightweight monochrome bitmap at the specified location. This is
just a simply utility function that draws the monochrome bitmap in a specified
color and write mode as fast as possible.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapMask(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *mask,
    color_t color,
    int op)
{
    rect_t  d,r,clip;
    int     oldop = dc->a.writeMode;
    color_t oldColor = dc->a.color;
    color_t oldBackColor = dc->a.backColor;

    /* Clip to destination device context */
    if (mask->bitsPerPixel != 1)
        return;
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = x;                 d.top = y;
    d.right = x + mask->width;  d.bottom = y + mask->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to bitmap coordinates */
    MGL_offsetRect(d,-x,-y);
    x += d.left;
    y += d.top;
    MAKE_HARDWARE_CURRENT(dc,false);
    dc->r.SetMix(op);
    dc->r.SetForeColor(color);
    dc->r.SetBackColor(0);
    if (dc->clipRegionScreen) {
        /* Draw it clipped to a complex clip region */
        x += dc->viewPort.left;
        y += dc->viewPort.top;
        d.left = x;                 d.top = y;
        d.right = x + mask->width;  d.bottom = y + mask->height;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (!MGL_equalRect(d,r)) {
                    /* Draw it clipped to the clip rectangle */
                    dc->r.ClipMonoImage_Old(x,y,mask->width,mask->height,
                        mask->bytesPerLine,mask->surface,dc->a.backMode,
                        r.left,r.top,
                        r.right,r.bottom);
                    }
                else {
                    /* Draw it unclipped for maximum speed */
                    dc->r.PutMonoImage_Old(x,y,mask->width,mask->height,
                        mask->bytesPerLine,mask->surface,dc->a.backMode);
                    }
                }
        END_CLIP_REGION();
        }
    else if (MGL_sectRect(dc->clipRectScreen,d,&r)) {
        if (!MGL_equalRect(d,r)) {
            /* Draw it clipped to the clip rectangle */
            dc->r.ClipMonoImage_Old(x+dc->viewPort.left,y+dc->viewPort.top,
                mask->width,mask->height,mask->bytesPerLine,
                mask->surface,dc->a.backMode,
                dc->clipRectScreen.left,dc->clipRectScreen.top,
                dc->clipRectScreen.right,dc->clipRectScreen.bottom);
            }
        else {
            /* Draw it unclipped for maximum speed */
            dc->r.PutMonoImage_Old(x+dc->viewPort.left,y+dc->viewPort.top,
                mask->width,mask->height,mask->bytesPerLine,mask->surface,dc->a.backMode);
            }
        }
    dc->r.SetMix(oldop);
    dc->r.SetForeColor(oldColor);
    dc->r.SetForeColor(oldBackColor);
    RESTORE_HARDWARE(dc,false);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a transparent lightweight bitmap at the specified location with source
transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
x           - x coordinate to draw bitmap at
y           - y coordinate to draw bitmap at
bitmap      - Bitmap to display
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing bitmap

REMARKS:
Draws a transparent lightweight bitmap at the specified location with
source transparency. When transferring the data with source transparency,
for pixels in the source image that are equal to the specified transparent
color, the related pixel in the destination buffer will remain
untouched. This allows you to quickly transfer sprites between device
contexts with a single color being allocated as a transparent color.

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapSrcTrans(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *bitmap,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         left,top,right,bottom;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }
    d.left = x;                     d.top = y;
    d.right = x + bitmap->width;    d.bottom = y + bitmap->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to bitmap coordinates */
    MGL_offsetRect(d,-x,-y);
    x += d.left;
    y += d.top;
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeySrcSingle;
        gaFx.Mix = op;
        gaFx.ColorKeyLo = transparent;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            left = d.left;
            top = d.top;
            right = d.right;
            bottom = d.bottom;
            d.left = x + dc->viewPort.left;
            d.top = y + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,
                d.right-d.left,d.bottom-d.top,&gaFx);
            }
        }
    else {
        if (dc->clipRegionScreen) {
            left = d.left;
            top = d.top;
            right = d.right;
            bottom = d.bottom;
            d.left = x + dc->viewPort.left;
            d.top = y + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.SrcTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,op,transparent,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.SrcTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,
                op,transparent,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a section of a transparent lightweight bitmap at the specified location
with source transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
left        - Left coordinate of section to draw
top         - Top coordinate of section to draw
right       - Right coordinate of section to draw
bottom      - Bottom coordinate of section to draw
dstLeft     - Left coordinate of destination of bitmap section
dstTop      - Right coordinate for destination of bitmap section
bitmap      - Bitmap to display
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing bitmap

REMARKS:
Draws a section of a transparent lightweight bitmap at the specified location
with source transparency. When transferring the data with source transparency,
pixels in the source image that are equal to the specified transparent color,
the related pixel in the destination buffer will remain untouched. This allows
you to quickly transfer sprites between device contexts with a single color
being allocated as a transparent color.

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapSrcTransSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    const bitmap_t *bitmap,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for compatible bitmaps */
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Clip the source rectangle to the bitmap dimensions */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (!MGL_sectRectCoord(0,0,bitmap->width,bitmap->height,left,top,right,bottom,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;           dstTop = d.top;

    /* Perform the blit of the bitmap section */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeySrcSingle;
        gaFx.Mix = op;
        gaFx.ColorKeyLo = transparent;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                right-left,bottom-top,&gaFx);
            }
        }
    else {
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.SrcTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,op,transparent,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.SrcTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                op,transparent,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a transparent lightweight bitmap at the specified location with
destination transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
x           - x coordinate to draw bitmap at
y           - y coordinate to draw bitmap at
bitmap      - Bitmap to display
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing bitmap

REMARKS:
Draws a transparent lightweight bitmap at the specified location with
destination transparency. When transferring the data with
destination transparency, pixels in the destination image that are equal to
the specified transparent color will be updated, and those pixels that are
not the same will be skipped. This is effectively the operation performed
for 'blueScreen'ing or color keying and can also be used for drawing
transparent sprites. Note however that destination transparency is very
slow in software compared to source transparency!

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapDstTrans(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *bitmap,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         left,top,right,bottom;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }
    d.left = x;                     d.top = y;
    d.right = x + bitmap->width;    d.bottom = y + bitmap->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to bitmap coordinates */
    MGL_offsetRect(d,-x,-y);
    x += d.left;
    y += d.top;
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeyDstSingle;
        gaFx.Mix = op;
        gaFx.ColorKeyLo = transparent;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            left = d.left;
            top = d.top;
            right = d.right;
            bottom = d.bottom;
            d.left = x + dc->viewPort.left;
            d.top = y + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,
                d.right-d.left,d.bottom-d.top,&gaFx);
            }
        }
    else {
        if (dc->clipRegionScreen) {
            left = d.left;
            top = d.top;
            right = d.right;
            bottom = d.bottom;
            d.left = x + dc->viewPort.left;
            d.top = y + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.DstTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,op,transparent,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.DstTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,
                op,transparent,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a section of a transparent lightweight bitmap at the specified location
with destination transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
left        - Left coordinate of section to draw
top         - Top coordinate of section to draw
right       - Right coordinate of section to draw
bottom      - Bottom coordinate of section to draw
dstLeft     - Left coordinate of destination of bitmap section
dstTop      - Right coordinate for destination of bitmap section
bitmap      - Bitmap to display
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing bitmap

REMARKS:
Draws a section of a transparent lightweight bitmap at the specified location
with destination transparency. When transferring the data
with destination transparency, pixels in the destination image that are equal
to the specified transparent color will be updated, and those pixels that
are not the same will be skipped. This is effectively the operation performed
for 'blueScreen'ing or color keying and can also be used for drawing
transparent sprites. Note however that destination transparency is very slow
in software compared to source transparency!

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapDstTransSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    const bitmap_t *bitmap,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for compatible bitmaps */
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Clip the source rectangle to the bitmap dimensions */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (!MGL_sectRectCoord(0,0,bitmap->width,bitmap->height,left,top,right,bottom,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;           dstTop = d.top;

    /* Perform the blit of the bitmap section */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeyDstSingle;
        gaFx.Mix = op;
        gaFx.ColorKeyLo = transparent;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                right-left,bottom-top,&gaFx);
            }
        }
    else {
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dc->r.DstTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,op,transparent,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.DstTransBltSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                op,transparent,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a lightweight bitmap at the specified location while applying a
mono or color pattern.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
x           - x coordinate to draw bitmap at
y           - y coordinate to draw bitmap at
bitmap      - Bitmap to display
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
Draws a lightweight bitmap at the specified location, while applying
either a mono bitmap pattern or a color pixmap pattern to the data with a
ternary raster operation code (ROP3). If the usePixMap parameter is set to
true, the current pixmap pattern set by MGL_setPenPixmapPattern will be
applied as pattern data, otherwise the current monochrome bitmap pattern
set by MGL_setPenBitmapPattern will be applied.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapPatt(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *bitmap,
    int usePixMap,
    int rop3)
{
    rect_t  d,r,clip;
    int     left,top,right,bottom;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    if (bitmap->bitsPerPixel < 8 || (bitmap->bitsPerPixel != dc->mi.bitsPerPixel)) {
        SETERROR(grInvalidDevice);
        return;
        }
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = x;                     d.top = y;
    d.right = x + bitmap->width;    d.bottom = y + bitmap->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to bitmap coordinates */
    MGL_offsetRect(d,-x,-y);
    x += d.left;
    y += d.top;
    MAKE_HARDWARE_CURRENT(dc,true);
    if (dc->clipRegionScreen) {
        left = d.left;
        top = d.top;
        right = d.right;
        bottom = d.bottom;
        d.left = x + dc->viewPort.left;
        d.top = y + dc->viewPort.top;
        d.right = d.left + (right-left);
        d.bottom = d.top + (bottom-top);
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                srcLeft = left + (r.left - d.left);
                srcRight = srcLeft + (r.right - r.left);
                srcTop = top + (r.top - d.top);
                srcBottom = srcTop + (r.bottom - r.top);
                if (usePixMap) {
                    dc->r.BitBltColorPattSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3,false);
                    }
                else {
                    dc->r.BitBltPattSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3,false);
                    }
                }
        END_CLIP_REGION();
        }
    else {
        if (usePixMap) {
            dc->r.BitBltColorPattSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,rop3,false);
            }
        else {
            dc->r.BitBltPattSys(bitmap->surface,bitmap->bytesPerLine,
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,rop3,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a section of a lightweight bitmap at the specified location while
applying a mono or color pattern.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
left        - Left coordinate of section to draw
top         - Top coordinate of section to draw
right       - Right coordinate of section to draw
bottom      - Bottom coordinate of section to draw
dstLeft     - Left coordinate of destination of bitmap section
dstTop      - Right coordinate for destination of bitmap section
bitmap      - Bitmap to display
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
Draws a section of lightweight bitmap at the specified location, while
applying either a mono bitmap pattern or a color pixmap pattern to the data
with a ternary raster operation code (ROP3). If the usePixMap parameter is
set to true, the current pixmap pattern set by MGL_setPenPixmapPattern will be
applied as pattern data, otherwise the current monochrome bitmap pattern
set by MGL_setPenBitmapPattern will be applied.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putBitmapPattSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    const bitmap_t *bitmap,
    int usePixMap,
    int rop3)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check for compatible bitmaps */
    if (bitmap->bitsPerPixel < 8 || (bitmap->bitsPerPixel != dc->mi.bitsPerPixel)) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Clip the source rectangle to the bitmap dimensions */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (!MGL_sectRectCoord(0,0,bitmap->width,bitmap->height,left,top,right,bottom,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;           dstTop = d.top;

    /* Perform the blit of the bitmap section */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = d.left + (right-left);
        d.bottom = d.top + (bottom-top);
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                srcLeft = left + (r.left - d.left);
                srcRight = srcLeft + (r.right - r.left);
                srcTop = top + (r.top - d.top);
                srcBottom = srcTop + (r.bottom - r.top);
                if (usePixMap) {
                    dc->r.BitBltColorPattSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3,false);
                    }
                else {
                    dc->r.BitBltPattSys(bitmap->surface,bitmap->bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3,false);
                    }
                }
        END_CLIP_REGION();
        }
    else {
        if (usePixMap) {
            dc->r.BitBltColorPattSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                rop3,false);
            }
        else {
            dc->r.BitBltPattSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                rop3,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Determines if a set of effects for MGL_bitBltFx are supported in hardware

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
fx      - Information describing the effects to apply to the blit (bltfx_t)

RETURNS:
True if effect will run in hardware, false if in software

REMARKS:
This function is used to allow the calling application to determine if
the effects supplied in the fx field (bltfx_t structure) can be supported
in hardware, or if they will be implemented in software. This function
will test the set of flags as applied to the specific source and destination
device contexts, and return true if the effect will be executed in hardware
and false if it will be executed in software.

SEE ALSO:
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBltFx, MGL_stretchBltFxCoord
****************************************************************************/
ibool MGLAPI MGL_bitBltFxTestBitmap(
    MGLDC *dst,
    bitmap_t *src,
    bltfx_t *fx)
{
    GA_bltFx    gaFx;

    /* Fail immediately if we do not have hardware effects blits */
    if (!dst->r.BitBltFxTest)
        return false;

    /* Setup effect blit structure. The MGL flags are a subset of
     * the SciTech SNAP Graphics flags so we can simply copy them across and add
     * the extra flags that we need.
     */
    gaFx.dwSize = sizeof(gaFx);
    gaFx.Flags = fx->flags;
    gaFx.Mix = fx->writeMode;
    if (gaFx.Flags & gaBltAnyColorKey) {
        gaFx.ColorKeyLo = fx->colorKeyLo;
        gaFx.ColorKeyHi = fx->colorKeyHi;
        }
    if (gaFx.Flags & gaBltBlend) {
        gaFx.SrcBlendFunc = fx->srcBlendFunc;
        gaFx.DstBlendFunc = fx->dstBlendFunc;
        gaFx.ConstColor = fx->constColor;
        gaFx.ConstAlpha = fx->constAlpha;
        }
    if (NEED_TRANSLATE_BM(src,dst)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = src->bitsPerPixel;
        gaFx.PixelFormat = GAPF(src->pf);
        gaFx.SrcPalette = GAPAL(src->pal);
        }
    return dst->r.BitBltFxTest(&gaFx);
}

/****************************************************************************
DESCRIPTION:
Draw a lightweight bitmap at the specified location, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to display bitmap on
x       - x coordinate to draw bitmap at
y       - y coordinate to draw bitmap at
bitmap  - Bitmap to display
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Draws a lightweight bitmap at the specified location, with optional
effects applied. The effects applied to the blit operation range from X and
Y bitmap flipping to color transparency and blending. All of these operations
can be applied to data being copied between different device context of
different color depths and pixel formats if desired (ie: 32-bit ARGB
alpha blended bitmap to a 16-bit device context). Please refer to the
documentation for the bltfx_t structure and the MGL_bitBltFxFlagsType
enumeration that defines the flags passed to this function.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon, MGL_bitBltFxTestBitmap
****************************************************************************/
void MGLAPI MGL_putBitmapFx(
    MGLDC *dc,
    int x,
    int y,
    const bitmap_t *bitmap,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         left = 0,top = 0,right = bitmap->width,bottom = bitmap->height;
    int         dstLeft = x,dstTop = y;
    int         srcLeft,srcTop,srcRight,srcBottom,t;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = bitmap->height - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = bitmap->width - right;
        right = left + t;
        }

    /* Setup effect blit structure. The MGL flags are a subset of
     * the SciTech SNAP Graphics flags so we can simply copy them across and add
     * the extra flags that we need.
     */
    gaFx.dwSize = sizeof(gaFx);
    gaFx.Flags = fx->flags;
    gaFx.Mix = fx->writeMode;
    if (gaFx.Flags & gaBltAnyColorKey) {
        gaFx.ColorKeyLo = fx->colorKeyLo;
        gaFx.ColorKeyHi = fx->colorKeyHi;
        }
    if (gaFx.Flags & gaBltBlend) {
        gaFx.SrcBlendFunc = fx->srcBlendFunc;
        gaFx.DstBlendFunc = fx->dstBlendFunc;
        gaFx.ConstColor = fx->constColor;
        gaFx.ConstAlpha = fx->constAlpha;
        }

    dstLeft = d.left;
    dstTop = d.top;
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec;
            gaFx.TranslateVec = dstPal;
            }
        }
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = d.left + (right-left);
        d.bottom = d.top + (bottom-top);
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                srcLeft = left + (r.left - d.left);
                srcRight = srcLeft + (r.right - r.left);
                srcTop = top + (r.top - d.top);
                srcBottom = srcTop + (r.bottom - r.top);
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            right-left,bottom-top,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw a section of a lightweight bitmap at the specified location, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to display bitmap on
left    - Left coordinate of section to draw
top     - Top coordinate of section to draw
right   - Right coordinate of section to draw
bottom  - Bottom coordinate of section to draw
dstLeft - Left coordinate of destination of bitmap section
dstTop  - Right coordinate for destination of bitmap section
bitmap  - Bitmap to display
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Draws a section of a lightweight bitmap at the specified location, with optional
effects applied. The effects applied to the blit operation range from X and
Y bitmap flipping to color transparency and blending. All of these operations
can be applied to data being copied between different device context of
different color depths and pixel formats if desired (ie: 32-bit ARGB
alpha blended bitmap to a 16-bit device context). Please refer to the
documentation for the bltfx_t structure and the MGL_bitBltFxFlagsType
enumeration that defines the flags passed to this function.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon, MGL_bitBltFxTestBitmap
****************************************************************************/
void MGLAPI MGL_putBitmapFxSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    const bitmap_t *bitmap,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode,t;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Clip the source rectangle to the bitmap dimensions */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);
    if (!MGL_sectRectCoord(0,0,bitmap->width,bitmap->height,left,top,right,bottom,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = bitmap->height - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = bitmap->width - right;
        right = left + t;
        }

    /* Setup effect blit structure. The MGL flags are a subset of
     * the SciTech SNAP Graphics flags so we can simply copy them across and add
     * the extra flags that we need.
     */
    gaFx.dwSize = sizeof(gaFx);
    gaFx.Flags = fx->flags;
    gaFx.Mix = fx->writeMode;
    if (gaFx.Flags & gaBltAnyColorKey) {
        gaFx.ColorKeyLo = fx->colorKeyLo;
        gaFx.ColorKeyHi = fx->colorKeyHi;
        }
    if (gaFx.Flags & gaBltBlend) {
        gaFx.SrcBlendFunc = fx->srcBlendFunc;
        gaFx.DstBlendFunc = fx->dstBlendFunc;
        gaFx.ConstColor = fx->constColor;
        gaFx.ConstAlpha = fx->constAlpha;
        }

    dstLeft = d.left;
    dstTop = d.top;
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec;
            gaFx.TranslateVec = dstPal;
            }
        }
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = d.left + (right-left);
        d.bottom = d.top + (bottom-top);
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                srcLeft = left + (r.left - d.left);
                srcRight = srcLeft + (r.right - r.left);
                srcTop = top + (r.top - d.top);
                srcBottom = srcTop + (r.bottom - r.top);
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            right-left,bottom-top,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a lightweight bitmap to the specified rectangle.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
dstLeft     - Left coordinate to stretch bitmap to
dstTop      - Top coordinate to stretch bitmap to
dstRight    - Right coordinate to stretch bitmap to
dstBottom   - Bottom coordinate to stretch bitmap to
bitmap      - Bitmap to display
op          - Write mode to use when drawing bitmap

REMARKS:
Stretches a lightweight bitmap to the specified destination rectangle on the
device context. The bitmap can be in any color format, and will be
translated as necessary to the color format required by the current device
context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

Supported write modes are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_stretchBitmap(
    MGLDC *dc,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    const bitmap_t *bitmap,
    int op)
{
    rect_t      d,r,clip;
    int         left = 0,top = 0,right = bitmap->width,bottom = bitmap->height;
    ibool       clipIt = false;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBitmap(dc,dstLeft,dstTop,bitmap,op);
        return;
        }
    if (dc == _MGL_dcPtr)
        dc = &DC;
    if (bitmap->bitsPerPixel < 8) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Clip to destination device context */
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstRight;
    d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    if ((dc->clipRegionScreen) || (d.left != dstLeft) || (d.right  != dstRight)
            || (d.top != dstTop) || (d.bottom != dstBottom))
        clipIt = true;

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the stretch Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltStretchNearest;
        gaFx.Mix = op;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = dstRight + dc->viewPort.left;
            d.bottom = dstBottom + dc->viewPort.top;
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    if (MGL_equalRect(d,r))
                        gaFx.Flags &= ~gaBltClip;
                    else {
                        gaFx.Flags |= gaBltClip;
                        gaFx.ClipLeft = r.left;
                        gaFx.ClipTop = r.top;
                        gaFx.ClipRight = r.right;
                        gaFx.ClipBottom = r.bottom;
                        }
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            if (clipIt) {
                gaFx.Flags |= gaBltClip;
                gaFx.ClipLeft = dc->clipRectScreen.left;
                gaFx.ClipTop = dc->clipRectScreen.top;
                gaFx.ClipRight = dc->clipRectScreen.right;
                gaFx.ClipBottom = dc->clipRectScreen.bottom;
                }
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,&gaFx);
            }
        }
    else {
        /* Perform the stretch blit with no pixel format conversion */
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = dstRight + dc->viewPort.left;
            d.bottom = dstBottom + dc->viewPort.top;
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    dc->r.StretchBltSys(bitmap->surface,bitmap->bytesPerLine,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,clipIt,
                        r.left,r.top,r.right,r.bottom,op,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.StretchBltSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,clipIt,
                dc->clipRectScreen.left,dc->clipRectScreen.top,
                dc->clipRectScreen.right,dc->clipRectScreen.bottom,
                op,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a section of a lightweight bitmap to the specified device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
left        - Left coordinate of section to stretch
top         - Top coordinate of section to stretch
right       - Right coordinate of section to stretch
bottom      - Bottom coordinate of section to stretch
dstLeft     - Left coordinate to stretch bitmap to
dstTop      - Top coordinate to stretch bitmap to
dstRight    - Right coordinate to stretch bitmap to
dstBottom   - Bottom coordinate to stretch bitmap to
bitmap      - Bitmap to display
op          - Write mode to use when drawing bitmap

REMARKS:
Stretches a section of a lightweight bitmap to the specified destination
rectangle on the device context. The bitmap can be in any color format, and
will be translated as necessary to the color format required by the current
device context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

Supported write modes are enumerated in MGL_writeModeType.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_stretchBitmapSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    const bitmap_t *bitmap,
    int op)
{
    rect_t      d,r,clip;
    int         deltaSrc,deltaDst;
    fix32_t     zoomx,zoomy;
    ibool       clipIt = false;
    int         fdTop,fdLeft;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check incoming device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    if (bitmap->bitsPerPixel < 8) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Calculate the x zoom factor */
    deltaSrc = right - left;
    deltaDst = dstRight - dstLeft;
    if (deltaDst == deltaSrc)
        zoomx = MGL_FIX_1;
    else if (deltaDst == (deltaSrc * 2))
        zoomx = MGL_FIX_2;
    else
        zoomx = MGL_FixDiv(MGL_TOFIX(deltaDst),MGL_TOFIX(deltaSrc));

    /* Calculate the y zoom factor */
    deltaSrc = bottom - top;
    deltaDst = dstBottom - dstTop;
    if (deltaDst == deltaSrc)
        zoomy = MGL_FIX_1;
    else if (deltaDst == (deltaSrc * 2))
        zoomy = MGL_FIX_2;
    else
        zoomy = MGL_FixDiv(MGL_TOFIX(deltaDst),MGL_TOFIX(deltaSrc));

    /* Handle special case of 1:1 stretch */
    if (zoomx == MGL_FIX_1 && zoomy == MGL_FIX_1) {
        MGL_putBitmapSection(dc,left,top,right,bottom,dstLeft,dstTop,bitmap,op);
        return;
        }

    /* Clip the source rectangle to the bitmap dimensions */
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = left;  d.top = top;
    d.right = MIN(right, bitmap->width);
    d.bottom = MIN(bottom, bitmap->height);
    if (MGL_emptyRect(d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    fdLeft = MGL_TOFIX(dstLeft) + ((d.left - left) * zoomx);
    fdTop = MGL_TOFIX(dstTop) + ((d.top - top) * zoomy);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;
    dstLeft = MGL_FIXROUND(fdLeft);
    dstTop = MGL_FIXROUND(fdTop);
    dstRight = dstLeft+MGL_FIXTOINT((right-left)*zoomx);
    dstBottom = dstTop+MGL_FIXTOINT((bottom-top)*zoomy);

    /* Clip to destination device context */
    d.left = dstLeft;           d.top = dstTop;
    d.right = dstRight;         d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    if ((dc->clipRegionScreen) || (d.left != dstLeft) || (d.right  != dstRight)
            || (d.top != dstTop) || (d.bottom != dstBottom))
        clipIt = true;

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        /* Translate the pixel information when doing the stretch Blt */
        gaFx.dwSize = sizeof(gaFx);
        gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltStretchNearest;
        gaFx.Mix = op;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec | gaBltDither;
            gaFx.TranslateVec = dstPal;
            }
        else if (ditherMode)
            gaFx.Flags |= gaBltDither;
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = dstRight + dc->viewPort.left;
            d.bottom = dstBottom + dc->viewPort.top;
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    if (MGL_equalRect(d,r))
                        gaFx.Flags &= ~gaBltClip;
                    else {
                        gaFx.Flags |= gaBltClip;
                        gaFx.ClipLeft = r.left;
                        gaFx.ClipTop = r.top;
                        gaFx.ClipRight = r.right;
                        gaFx.ClipBottom = r.bottom;
                        }
                    dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            if (clipIt) {
                gaFx.Flags |= gaBltClip;
                gaFx.ClipLeft = dc->clipRectScreen.left;
                gaFx.ClipTop = dc->clipRectScreen.top;
                gaFx.ClipRight = dc->clipRectScreen.right;
                gaFx.ClipBottom = dc->clipRectScreen.bottom;
                }
            dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,&gaFx);
            }
        }
    else {
        /* Perform the stretch blit with no pixel format conversion */
        if (dc->clipRegionScreen) {
            d.left = dstLeft + dc->viewPort.left;
            d.top = dstTop + dc->viewPort.top;
            d.right = dstRight + dc->viewPort.left;
            d.bottom = dstBottom + dc->viewPort.top;
            BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    dc->r.StretchBltSys(bitmap->surface,bitmap->bytesPerLine,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,clipIt,
                        r.left,r.top,r.right,r.bottom,op,false);
                    }
            END_CLIP_REGION();
            }
        else {
            dc->r.StretchBltSys(bitmap->surface,bitmap->bytesPerLine,
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,clipIt,
                dc->clipRectScreen.left,dc->clipRectScreen.top,
                dc->clipRectScreen.right,dc->clipRectScreen.bottom,
                op,false);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a lightweight bitmap to the specified rectangle, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
dstLeft     - Left coordinate to stretch bitmap to
dstTop      - Top coordinate to stretch bitmap to
dstRight    - Right coordinate to stretch bitmap to
dstBottom   - Bottom coordinate to stretch bitmap to
bitmap      - Bitmap to display
fx          - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Stretches a lightweight bitmap to the specified destination rectangle on the
device context, while applying optional effects. This function behaves identically
to the MGL_putBitmapFx function, except it includes the ability to stretch the
bitmap at the same time. You can use the flags member of the bltfx_t structure
to define whether stretching is done using nearest color stretching or if pixel
interpolation will be used. If you don't specify any flags, nearest color
stretching will be used. The bitmap can be in any color format, and will be
translated as necessary to the color format required by the current device
context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon, MGL_bitBltFxTestBitmap
****************************************************************************/
void MGLAPI MGL_stretchBitmapFx(
    MGLDC *dc,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    const bitmap_t *bitmap,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         left = 0,top = 0,right = bitmap->width,bottom = bitmap->height;
    int         ditherMode,t;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBitmapFx(dc,dstLeft,dstTop,bitmap,fx);
        return;
        }
    if (dc == _MGL_dcPtr)
        dc = &DC;
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Clip to destination device context */
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstRight;
    d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = bitmap->height - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = bitmap->width - right;
        right = left + t;
        }

    /* Setup effect blit structure. The MGL flags are a subset of
     * the SciTech SNAP Graphics flags so we can simply copy them across and add
     * the extra flags that we need.
     */
    gaFx.dwSize = sizeof(gaFx);
    gaFx.Flags = fx->flags;
    gaFx.Mix = fx->writeMode;
    if (!(gaFx.Flags & (gaBltStretchXInterp | gaBltStretchYInterp)))
        gaFx.Flags |= gaBltStretchNearest;
    if (gaFx.Flags & gaBltAnyColorKey) {
        gaFx.ColorKeyLo = fx->colorKeyLo;
        gaFx.ColorKeyHi = fx->colorKeyHi;
        }
    if (gaFx.Flags & gaBltBlend) {
        gaFx.SrcBlendFunc = fx->srcBlendFunc;
        gaFx.DstBlendFunc = fx->dstBlendFunc;
        gaFx.ConstColor = fx->constColor;
        gaFx.ConstAlpha = fx->constAlpha;
        }
    if ((d.left != dstLeft) || (d.right  != dstRight) || (d.top != dstTop) || (d.bottom != dstBottom)) {
        gaFx.Flags |= gaBltClip;
        gaFx.ClipLeft = dc->clipRectScreen.left;
        gaFx.ClipTop = dc->clipRectScreen.top;
        gaFx.ClipRight = dc->clipRectScreen.right;
        gaFx.ClipBottom = dc->clipRectScreen.bottom;
        }

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec;
            gaFx.TranslateVec = dstPal;
            }
        }
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = dstRight + dc->viewPort.left;
        d.bottom = dstBottom + dc->viewPort.top;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (MGL_equalRect(d,r))
                    gaFx.Flags &= ~gaBltClip;
                else {
                    gaFx.Flags |= gaBltClip;
                    gaFx.ClipLeft = r.left;
                    gaFx.ClipTop = r.top;
                    gaFx.ClipRight = r.right;
                    gaFx.ClipBottom = r.bottom;
                    }
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a section of a lightweight bitmap to the specified device context,
while applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to display bitmap on
left        - Left coordinate of section to stretch
top         - Top coordinate of section to stretch
right       - Right coordinate of section to stretch
bottom      - Bottom coordinate of section to stretch
dstLeft     - Left coordinate to stretch bitmap to
dstTop      - Top coordinate to stretch bitmap to
dstRight    - Right coordinate to stretch bitmap to
dstBottom   - Bottom coordinate to stretch bitmap to
bitmap      - Bitmap to display
fx          - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Stretches a section of a lightweight bitmap to the specified destination rectangle
on the device context, while applying optional effects. This function behaves
identically to the MGL_putBitmapSectionFx function, except it includes the ability
to stretch the bitmap at the same time. You can use the flags member of the
bltfx_t structure to define whether stretching is done using nearest color
stretching or if pixel interpolation will be used. If you don't specify any
flags, nearest color stretching will be used. The bitmap can be in any color
format, and will be translated as necessary to the color format required by
the current device context.

When this function is called for 4 and 8 bit source bitmaps being copied to
either 4 or 8bpp destination device contexts, MGL first checks if the color
palettes for the source and destination bitmaps are the same. If they are
not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt段ng bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When this function is called for 4 and 8 bit source bitmaps being copied to
RGB destination device contexts, MGL will convert the pixels in the source
bitmap using the source bitmap palette to map them to the destination pixel
format. If however you know in advance that the palette for all source bitmaps
is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

Note also that if the source bitmap palette pointer is set to NULL, palette
translation is automatically avoided (ie: has the effect of forcing
MGL_checkIdentityPalette to false just for that bitmap).

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon, MGL_bitBltFxTestBitmap
****************************************************************************/
void MGLAPI MGL_stretchBitmapFxSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    const bitmap_t *bitmap,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         deltaSrc,deltaDst;
    fix32_t     zoomx,zoomy;
    int         fdTop,fdLeft;
    int         ditherMode,t;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check incoming device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    if (bitmap->bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* Calculate the x zoom factor */
    deltaSrc = right - left;
    deltaDst = dstRight - dstLeft;
    if (deltaDst == deltaSrc)
        zoomx = MGL_FIX_1;
    else if (deltaDst == (deltaSrc * 2))
        zoomx = MGL_FIX_2;
    else
        zoomx = MGL_FixDiv(MGL_TOFIX(deltaDst),MGL_TOFIX(deltaSrc));

    /* Calculate the y zoom factor */
    deltaSrc = bottom - top;
    deltaDst = dstBottom - dstTop;
    if (deltaDst == deltaSrc)
        zoomy = MGL_FIX_1;
    else if (deltaDst == (deltaSrc * 2))
        zoomy = MGL_FIX_2;
    else
        zoomy = MGL_FixDiv(MGL_TOFIX(deltaDst),MGL_TOFIX(deltaSrc));

    /* Handle special case of 1:1 stretch */
    if (zoomx == MGL_FIX_1 && zoomy == MGL_FIX_1) {
        MGL_putBitmapFxSection(dc,left,top,right,bottom,dstLeft,dstTop,bitmap,fx);
        return;
        }

    /* Clip the source rectangle to the bitmap dimensions */
    BEGIN_VISIBLE_CLIP_LIST(dc);
    d.left = left;  d.top = top;
    d.right = MIN(right, bitmap->width);
    d.bottom = MIN(bottom, bitmap->height);
    if (MGL_emptyRect(d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    fdLeft = MGL_TOFIX(dstLeft) + ((d.left - left) * zoomx);
    fdTop = MGL_TOFIX(dstTop) + ((d.top - top) * zoomy);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;
    dstLeft = MGL_FIXROUND(fdLeft);
    dstTop = MGL_FIXROUND(fdTop);
    dstRight = dstLeft+MGL_FIXTOINT((right-left)*zoomx);
    dstBottom = dstTop+MGL_FIXTOINT((bottom-top)*zoomy);

    /* Clip to destination device context */
    d.left = dstLeft;           d.top = dstTop;
    d.right = dstRight;         d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = bitmap->height - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = bitmap->width - right;
        right = left + t;
        }

    /* Setup effect blit structure. The MGL flags are a subset of
     * the SciTech SNAP Graphics flags so we can simply copy them across and add
     * the extra flags that we need.
     */
    gaFx.dwSize = sizeof(gaFx);
    gaFx.Flags = fx->flags;
    gaFx.Mix = fx->writeMode;
    if (!(gaFx.Flags & (gaBltStretchXInterp | gaBltStretchYInterp)))
        gaFx.Flags |= gaBltStretchNearest;
    if (gaFx.Flags & gaBltAnyColorKey) {
        gaFx.ColorKeyLo = fx->colorKeyLo;
        gaFx.ColorKeyHi = fx->colorKeyHi;
        }
    if (gaFx.Flags & gaBltBlend) {
        gaFx.SrcBlendFunc = fx->srcBlendFunc;
        gaFx.DstBlendFunc = fx->dstBlendFunc;
        gaFx.ConstColor = fx->constColor;
        gaFx.ConstAlpha = fx->constAlpha;
        }
     if ((d.left != dstLeft) || (d.right  != dstRight) || (d.top != dstTop) || (d.bottom != dstBottom)) {
        gaFx.Flags |= gaBltClip;
        gaFx.ClipLeft = dc->clipRectScreen.left;
        gaFx.ClipTop = dc->clipRectScreen.top;
        gaFx.ClipRight = dc->clipRectScreen.right;
        gaFx.ClipBottom = dc->clipRectScreen.bottom;
        }

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (NEED_TRANSLATE_BM(bitmap,dc)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = bitmap->bitsPerPixel;
        gaFx.PixelFormat = GAPF(bitmap->pf);
        gaFx.SrcPalette = GAPAL(bitmap->pal);
        gaFx.DstPalette = GAPAL(dc->colorTab);
        SETUP_DITHER_MODE(dc,IS_COLOR_INDEX_BMP(bitmap),bitmap->pal);
        if (ditherMode == -1) {
            gaFx.Flags |= gaBltTranslateVec;
            gaFx.TranslateVec = dstPal;
            }
        }
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = dstRight + dc->viewPort.left;
        d.bottom = dstBottom + dc->viewPort.top;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                if (MGL_equalRect(d,r))
                    gaFx.Flags &= ~gaBltClip;
                else {
                    gaFx.Flags |= gaBltClip;
                    gaFx.ClipLeft = r.left;
                    gaFx.ClipTop = r.top;
                    gaFx.ClipRight = r.right;
                    gaFx.ClipBottom = r.bottom;
                    }
                dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxSys(bitmap->surface,bitmap->bytesPerLine,
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Draw an icon at the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to draw icon on
x   - x coordinate to draw icon at
y   - y coordinate to draw icon at
icon    - Icon to display

REMARKS:
Draws an icon at the specified location on the current device context. The icon may
be in any color format, and will be translated as necessary to the color format of the
display device context. The icon is drawn by punching a black hole in the
background with the icon mask, and then OR'ing in the image bitmap for the icon.

SEE ALSO:
MGL_loadBitmap, MGL_putBitmap, MGL_putBitmapSection, MGL_putBitmapSrcTrans,
MGL_putBitmapSrcTransSection, MGL_putBitmapDstTrans,
MGL_putBitmapDstTransSection, MGL_putBitmapMask, MGL_putBitmapPatt,
MGL_putBitmapPattSection, MGL_putBitmapFx, MGL_putBitmapFxSection,
MGL_stretchBitmap, MGL_stretchBitmapSection, MGL_stretchBitmapFx,
MGL_stretchBitmapFxSection, MGL_putIcon
****************************************************************************/
void MGLAPI MGL_putIcon(
    MGLDC *dc,
    int x,
    int y,
    const icon_t *icon)
{
    int     oldop = dc->a.writeMode;
    color_t oldColor = dc->a.color;
    int     oldBackMode = dc->a.backMode;

    /* Punch a hole in the display with the icon's AND mask */
    MAKE_HARDWARE_CURRENT(dc,false);
    dc->r.SetMix(MGL_REPLACE_MODE);
    dc->r.SetForeColor(0);
    dc->a.backMode = MGL_TRANSPARENT_BACKGROUND;
    MGL_putMonoImage(dc,x,y,icon->xorMask.width,icon->byteWidth,icon->xorMask.height,icon->andMask);
    dc->r.SetMix(oldop);
    dc->r.SetForeColor(oldColor);
    dc->a.backMode = oldBackMode;

    /* Now XOR in the bitmap for the ICON */
    MGL_putBitmap(dc,x,y,&icon->xorMask,MGL_XOR_MODE);
    RESTORE_HARDWARE(dc,false);
}

