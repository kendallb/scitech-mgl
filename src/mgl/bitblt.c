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
* Description:  Bit Block Transfer routines (BitBlt's).
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

#undef  MGL_bitBlt
#undef  MGL_srcTransBlt
#undef  MGL_dstTransBlt
#undef  MGL_bitBltPatt
#undef  MGL_bitBltFx
#undef  MGL_stretchBlt
#undef  MGL_stretchBltFx
#undef  MGL_copyPage
#undef  MGL_getDivot
#undef  MGL_divotSize

/****************************************************************************
DESCRIPTION:
Saves a divot of video memory into system RAM.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to save divot from
left    - Left coordinate of area to save
top     - Top coordinate of area to save
right   - Right coordinate of area to save
bottom  - Bottom coordinate of area to save
divot   - Pointer to area to store the video memory in

REMARKS:
This function copies a block of video memory from the active page of the
current device context into a system RAM buffer. A divot is defined as being
a rectangular area of video memory that you wish to save, however the
bounding rectangle for the divot is expanded slightly to properly aligned
boundaries for the absolute maximum performance with the current device
context. This function is generally used to store the video memory behind
pull down menus and pop up dialog boxes, and the memory can only be restored
to exactly the same position that it was saved from.

You must pre-allocate enough space to hold the entire divot in system RAM.
Use the MGL_divotSize routine to determine the size of the memory block
required to store the divot.

SEE ALSO:
MGL_getDivot, MGL_putDivot, MGL_divotSize, PM_malloc
****************************************************************************/
void MGLAPI MGL_getDivotCoord(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    void *divot)
{
    rect_t      d;
    uchar       *p;
    divotHeader hdr;

    /* Clip divot to destination device context */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    d.left = left + dc->viewPort.left;
    d.top = top + dc->viewPort.top;
    d.right = right + dc->viewPort.left;
    d.bottom = bottom + dc->viewPort.top;
    if (!MGL_sectRect(dc->clipRectScreen,d,&d))
        return;

    /* Store header information */
    hdr.left = d.left - dc->size.left;      hdr.top = d.top - dc->size.top;
    hdr.right = d.right - dc->size.left;    hdr.bottom = d.bottom - dc->size.top;
    switch (dc->mi.bitsPerPixel) {
        case 4:
            hdr.bytesPerLine = ((d.right - d.left)+1)/2;
            break;
        case 8:
            hdr.bytesPerLine = (d.right - d.left);
            break;
        case 15:
        case 16:
            hdr.bytesPerLine = (d.right - d.left)*2;
            break;
        case 24:
            hdr.bytesPerLine = (d.right - d.left)*3;
            break;
        default: /* case 32: */
            hdr.bytesPerLine = (d.right - d.left)*4;
            break;
        }
    p = divot;
    memcpy(p,&hdr,sizeof(hdr));
    p += sizeof(hdr);
    MAKE_HARDWARE_CURRENT(dc,true);
    dc->r.GetBitmapSys(p,hdr.bytesPerLine,d.left,d.top,d.right-d.left,d.bottom-d.top,0,0,GA_REPLACE_MIX);
    RESTORE_HARDWARE(dc,true);
}

/****************************************************************************
DESCRIPTION:
Saves a divot of video memory into system RAM.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to save divot from
r       - Rectangle containing coordinates of divot to save
divot   - Pointer to area to store the video memory in

REMARKS:
This function is the same as MGL_getDivotCoord however it takes entire
rectangles as arguments instead of coordinates.

SEE ALSO:
MGL_putDivot, MGL_divotSize, PM_malloc
****************************************************************************/
void MGL_getDivot(
    MGLDC dc,
    rect_t,
    void *divot);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Replaces a divot of video memory to the location from which it was copied.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to restore the divot to
divot   - Pointer to the divot to replace

REMARKS:
This function replaces a rectangle of video memory that was saved previously
with the MGL_getDivot function. The divot is replaced at the same location
that is was taken from on the current device context.

A divot is defined as being a rectangular area of video memory that you wish
to save, however the bounding rectangle for the divot is expanded slightly
to properly aligned boundaries for the absolute maximum performance with the
current device context. This function is generally used to store the video
memory behind pull down menus and pop up dialog boxes, and the memory can
only be restored to exactly the same position that it was saved from.

SEE ALSO:
MGL_divotSize
****************************************************************************/
void MGLAPI MGL_putDivot(
    MGLDC *dc,
    void *divot)
{
    uchar       *s;
    divotHeader hdr;

    /* Extract header information */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    s = divot;
    memcpy(&hdr,s,sizeof(hdr));
    hdr.left += dc->size.left;  hdr.top += dc->size.top;
    hdr.right += dc->size.left; hdr.bottom += dc->size.top;
    s += sizeof(hdr);
    MAKE_HARDWARE_CURRENT(dc,true);
    dc->r.BitBltSys(s,hdr.bytesPerLine,0,0,hdr.right-hdr.left,hdr.bottom-hdr.top,hdr.left,hdr.top,GA_REPLACE_MIX,false);
    RESTORE_HARDWARE(dc,true);
}

/****************************************************************************
DESCRIPTION:
Number of bytes required to store a divot of specified size.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to measure divot size from
left    - Left coordinate of divot area
top     - Top coordinate of divot area
right   - Right coordinate of divot area
bottom  - Bottom coordinate of divot area

RETURNS:
Size of the specified divot in bytes.

REMARKS:
Determines the number of bytes required to store a divot of the specified
size taken from the current device context. A divot is a portion of video
memory that needs to be temporarily saved and restored, such as implementing
pull down menus and pop up dialog boxes. A divot must always be saved and
restored to the same area, and will extend the dimensions of the area
covered to obtain the maximum possible performance for saving and restoring
the memory.

SEE ALSO:
MGL_divotSize, MGL_getDivot, MGL_putDivot
****************************************************************************/
long MGLAPI MGL_divotSizeCoord(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom)
{
    int bytesPerLine;

    if (dc == _MGL_dcPtr)
        dc = &DC;
    switch (dc->mi.bitsPerPixel) {
        case 4:
        case 8:
            bytesPerLine = (right - left);
            break;
        case 15:
        case 16:
            bytesPerLine = (right - left)*2;
            break;
        case 24:
            bytesPerLine = (right - left)*3;
            break;
        default: /* case 32: */
            bytesPerLine = (right - left)*4;
            break;
        }
    return sizeof(divotHeader) + bytesPerLine * (bottom-top);
}

/****************************************************************************
DESCRIPTION:
Number of bytes required to store a divot of specified size.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to measure divot size from
r   - Bounding rectangle of the divot area

RETURNS:
Size of the specified divot in bytes.

REMARKS:
This function is the same as MGL_divotSizeCoord however it takes entire
rectangles as arguments instead of coordinates.

SEE ALSO:
MGL_divotSizeCoord, MGL_getDivot, MGL_putDivot
****************************************************************************/
long MGL_divotSize(
    MGLDC *dc,
    rect_t r);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one device context into another.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
left    - Left coordinate of image to Blt from
top     - Top coordinate of image to Blt from
right   - Right coordinate of image to Blt from
bottom  - Bottom coordinate of image to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
op      - Write mode to use during Blt

REMARKS:
Copies a block of bitmap data from one device context to another. The source
and destination rectangles may overlap even if the source and destination
device contexts are the same, and MGL will correctly handle the overlapping
regions. This routine has been highly optimized for absolute maximum performance,
so it will provide the fastest method of copying bitmap data between device
contexts. To obtain absolute maximum performance, you should align the source
and destination bitmaps on DWORD boundaries (4 pixels for 8 bit, 2 pixels for
15/16 bit) and the low level device driver code will special case this for
maximum performance.

This function will correctly handle Blt's across device contexts with
differing pixel depths, and will perform the necessary pixel format translation
to convert from the source device to the destination device. Note that although
the code to implement this is highly optimized, this can be a time consuming
operation so you should attempt to pre-convert all bitmaps to the current
display device pixel format for maximum performance if using this routine for
sprite animation.

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

This routine can also be used to perform hardware accelerated Blt's between
offscreen memory devices and the display device when running in fullscreen
modes, providing the hardware accelerator (if present) can support this
operation.

The write mode operation specifies how the source image data should be combined
with the destination image data.  Write modes supported by the SciTech MGL are
enumerated in MGL_writeModeType.

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_bitBltCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dst);

    /* Clip to source rectangle */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination rectangle */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Now perform the blit operation */
    if (src->mi.bitsPerPixel == 1) {
        int         oldop = dst->a.writeMode;
        color_t     oldColor = dst->a.color;
        color_t     oldBackColor = dst->a.backColor;
        palette_t   *pal = TO_PAL(src->colorTab);

        MAKE_HARDWARE_CURRENT(dst,false);
        dst->r.SetMix(op);
        dst->r.SetForeColor(MGL_rgbColor(dst,pal[1].red,pal[1].green,pal[1].blue));
        dst->r.SetBackColor(MGL_rgbColor(dst,pal[0].red,pal[0].green,pal[0].blue));
        if (dst->clipRegionScreen) {
            /* Draw it clipped to a complex clip region */
            dstLeft += dst->viewPort.left;
            dstTop += dst->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    /* Draw it clipped to the clip rectangle */
                    dst->r.ClipMonoImage_Old(
                        dstLeft,dstTop,src->mi.xRes+1,src->mi.yRes+1,
                        src->mi.bytesPerLine,src->surface,dst->a.backMode,
                        d.left,d.top,d.right,d.bottom);
                    }
            END_CLIP_REGION();
            }
        else {
            /* Draw it clipped to the clip rectangle */
            dst->r.ClipMonoImage_Old(
                dstLeft+dst->viewPort.left,dstTop+dst->viewPort.top,
                src->mi.xRes+1,src->mi.yRes+1,src->mi.bytesPerLine,
                src->surface,dst->a.backMode,
                d.left+dst->viewPort.left,d.top+dst->viewPort.top,
                d.right+dst->viewPort.left,d.bottom+dst->viewPort.top);
            }
        dst->r.SetMix(oldop);
        dst->r.SetForeColor(oldColor);
        dst->r.SetBackColor(oldBackColor);
        RESTORE_HARDWARE(dst,false);
        }
    else {
        dstLeft = d.left;   dstTop = d.top;
        if (src == dst) {
            /* BitBlt between areas of the same device context */
            MAKE_HARDWARE_CURRENT(dst,true);
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dstLeft = r.left;
                        dstTop = r.top;
                        src->r.BitBlt(
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,op);
                        }
                END_CLIP_REGION();
                }
            else {
                src->r.BitBlt(
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,op);
                }
            RESTORE_HARDWARE(dst,true);
            }
        else if (src->deviceType == MGL_MEMORY_DEVICE) {
            /* Copy from one DC to any other DC when one is in system memory */
            MAKE_HARDWARE_CURRENT(dst,true);
            if (NEED_TRANSLATE_DC(src,dst)) {
                gaFx.dwSize = sizeof(gaFx);
                gaFx.Flags = gaBltConvert | gaBltMixEnable;
                gaFx.Mix = op;
                gaFx.BitsPerPixel = src->mi.bitsPerPixel;
                gaFx.PixelFormat = GAPF(&src->pf);
                gaFx.SrcPalette = GAPAL(src->colorTab);
                gaFx.DstPalette = GAPAL(dst->colorTab);
                SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
                if (ditherMode == -1) {
                    gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                    gaFx.TranslateVec = dstPal;
                    }
                else if (ditherMode)
                    gaFx.Flags |= gaBltDither;
                if (dst->clipRegionScreen) {
                    left += src->viewPort.left;
                    top += src->viewPort.top;
                    right += src->viewPort.left;
                    bottom += src->viewPort.top;
                    d.left = dstLeft + dst->viewPort.left;
                    d.top = dstTop + dst->viewPort.top;
                    d.right = d.left + (right-left);
                    d.bottom = d.top + (bottom-top);
                    srcLeft = left - (dst->viewPort.left - dst->viewPortOrg.x);
                    srcTop = top - (dst->viewPort.top - dst->viewPortOrg.y);
                    BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                        if (MGL_sectRect(clip,d,&r)) {
                            srcLeft += (r.left - dstLeft);
                            srcRight = srcLeft + (r.right - r.left);
                            srcTop += (r.top - dstTop);
                            srcBottom = srcTop + (r.bottom - r.top);
                            dstLeft = r.left;
                            dstTop = r.top;
                            dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                                srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                                dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                            }
                    END_CLIP_REGION();
                    }
                else {
                    dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                        left + src->viewPort.left,top + src->viewPort.top,
                        right - left,bottom - top,
                        dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                        right - left,bottom - top,&gaFx);
                    }
                }
            else {
                if (dst->clipRegionScreen) {
                    left += src->viewPort.left;
                    top += src->viewPort.top;
                    right += src->viewPort.left;
                    bottom += src->viewPort.top;
                    d.left = dstLeft + dst->viewPort.left;
                    d.top = dstTop + dst->viewPort.top;
                    d.right = d.left + (right-left);
                    d.bottom = d.top + (bottom-top);
                    BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                        if (MGL_sectRect(clip,d,&r)) {
                            srcLeft = left + (r.left - d.left);
                            srcRight = srcLeft + (r.right - r.left);
                            srcTop = top + (r.top - d.top);
                            srcBottom = srcTop + (r.bottom - r.top);
                            dst->r.BitBltSys(src->surface,src->mi.bytesPerLine,
                                srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                                r.left,r.top,op,false);
                            }
                    END_CLIP_REGION();
                    }
                else {
                    dst->r.BitBltSys(src->surface,src->mi.bytesPerLine,
                        left + src->viewPort.left,top + src->viewPort.top,
                        right - left,bottom - top,
                        dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                        op,false);
                    }
                }
            RESTORE_HARDWARE(dst,true);
            }
        else if (dst->deviceType == MGL_MEMORY_DEVICE) {
            /* Copy from one DC to any other DC when one is in system memory */
            if (NEED_TRANSLATE_DC(src,dst)) {
                MAKE_HARDWARE_CURRENT(dst,true);
                /* TODO: We may need to deal with Convert blt reads from
                 *       the framebuffer differently.
                 */
                gaFx.dwSize = sizeof(gaFx);
                gaFx.Flags = gaBltConvert | gaBltMixEnable;
                gaFx.Mix = op;
                gaFx.BitsPerPixel = src->mi.bitsPerPixel;
                gaFx.PixelFormat = GAPF(&src->pf);
                gaFx.SrcPalette = GAPAL(src->colorTab);
                gaFx.DstPalette = GAPAL(dst->colorTab);
                SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
                if (ditherMode == -1) {
                    gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                    gaFx.TranslateVec = dstPal;
                    }
                else if (ditherMode)
                    gaFx.Flags |= gaBltDither;
                if (src->r.LockBuffer) {
                    /* Lock the source buffer if it is in hardware video memory */
                    src->r.LockBuffer(src->activeBuf);
                    src->surface = src->activeBuf->Surface;
                    }
                if (dst->clipRegionScreen) {
                    left += src->viewPort.left;
                    top += src->viewPort.top;
                    right += src->viewPort.left;
                    bottom += src->viewPort.top;
                    d.left = dstLeft + dst->viewPort.left;
                    d.top = dstTop + dst->viewPort.top;
                    d.right = d.left + (right-left);
                    d.bottom = d.top + (bottom-top);
                    srcLeft = left - (dst->viewPort.left - dst->viewPortOrg.x);
                    srcTop = top - (dst->viewPort.top - dst->viewPortOrg.y);
                    BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                        if (MGL_sectRect(clip,d,&r)) {
                            srcLeft += (r.left - dstLeft);
                            srcRight = srcLeft + (r.right - r.left);
                            srcTop += (r.top - dstTop);
                            srcBottom = srcTop + (r.bottom - r.top);
                            dstLeft = r.left;
                            dstTop = r.top;
                            dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                                srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                                dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                            }
                    END_CLIP_REGION();
                    }
                else {
                    dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                        left + src->viewPort.left,top + src->viewPort.top,
                        right - left,bottom - top,
                        dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                        right - left,bottom - top,&gaFx);
                    }
                if (src->r.UnlockBuffer) {
                    /* Unlock the source buffer if it is in hardware video memory */
                    src->r.UnlockBuffer(src->activeBuf);
                    src->surface = NULL;
                    }
                RESTORE_HARDWARE(dst,true);
                }
            else {
                MAKE_HARDWARE_CURRENT(src,true);
                if (dst->clipRegionScreen) {
                    left += src->viewPort.left;
                    top += src->viewPort.top;
                    right += src->viewPort.left;
                    bottom += src->viewPort.top;
                    d.left = dstLeft + dst->viewPort.left;
                    d.top = dstTop + dst->viewPort.top;
                    d.right = d.left + (right-left);
                    d.bottom = d.top + (bottom-top);
                    BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                        if (MGL_sectRect(clip,d,&r)) {
                            srcLeft = left + (r.left - d.left);
                            srcRight = srcLeft + (r.right - r.left);
                            srcTop = top + (r.top - d.top);
                            srcBottom = srcTop + (r.bottom - r.top);
                            src->r.GetBitmapSys(dst->surface,dst->mi.bytesPerLine,
                                srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                                r.left,r.top,op);
                            }
                    END_CLIP_REGION();
                    }
                else {
                    src->r.GetBitmapSys(dst->surface,dst->mi.bytesPerLine,
                        left + src->viewPort.left,top + src->viewPort.top,
                        right - left,bottom - top,
                        dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,op);
                    }
                RESTORE_HARDWARE(src,true);
                }
            }
        else {
            /* BitBlt between two device contexts which may both be in
             * hardware. We allow the MGL device driver determine how to
             * handle the case, so that it can be done in hardware if
             * available (such as blitting from offscreen DC or back buffers).
             */
            MAKE_HARDWARE_CURRENT(dst,true);
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);

                // TODO: This is incorrect for complex clip regions! We need to sort
                //       the clip rectangle list as per the mechanism we used in the Qt
                //       SNAP drivers!!
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dstLeft = r.left;
                        dstTop = r.top;
                        dst->r.BitBltBuf(src->activeBuf,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,op);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.BitBltBuf(src->activeBuf,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,op);
                }
            RESTORE_HARDWARE(dst,true);
            }
        }
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one device context into another.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
r       - Rectangle defining are to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
op      - Write mode to use during Blt

REMARKS:
This function is the same as MGL_bitBltCoord, however it takes entire
rectangles as parameters instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_bitBlt(
    MGLDC *dst,
    MGLDC *src,
    rect_t r,
    int dstLeft,
    int dstTop,
    int op);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one page in a display device context
to another page within the same device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Destination device context
srcPage - Page in DC to get the source data from
left    - Left coordinate of image to Blt from
top     - Top coordinate of image to Blt from
right   - Right coordinate of image to Blt from
bottom  - Bottom coordinate of image to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
op      - Write mode to use during Blt

REMARKS:
Copies a block of bitmap data from the source page in the destination device
context to the currently active page. This routine has been highly optimized
for absolute maximum performance, so it will provide the fastest method of
copying bitmap data between two different pages in a display device context.

The write mode operation specifies how the source image data should be combined
with the destination image data.  Write modes supported by the SciTech MGL are
enumerated in MGL_writeModeType.

The destination rectangle is clipped according to the current clipping
rectangles for the destination device context. However the source rectangle
is only clipped to the bounds of the source page dimensions and is not
clipped to the source clip rectangle. Also the source coordinates are
global screen coordinates and are not viewport translated. You will need to
do your own viewport translation as necessary.

Note:   This function will utilise the hardware whenever available to
        speed up the blitting of data between display pages. However if
        the end user system does not have hardware screen to screen blit
        operations (or the underlying device driver does not support this),
        then the copy operation will be quite slow. It would be faster in
        this case to do all rendering to a system memory DC and blit
        that to the screen instead of doing software blit's between two
        display pages.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_copyPageCoord(
    MGLDC *dc,
    int srcPage,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int op)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that this is a fullscreen display device context */
    if (!_MGL_isFullscreenDevice(dc)) {
        FATALERROR(grInvalidDC);
        return;
        }

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to source bounds */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(dc->bounds,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination rectangle */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;   dstTop = d.top;

    /* Now perform the blit operation */
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
                dstLeft = r.left;
                dstTop = r.top;
                dc->r.BitBltBuf(dc->r.GetFlippableBuffer(srcPage),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    dstLeft,dstTop,op);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltBuf(dc->r.GetFlippableBuffer(srcPage),
            left,top,right - left,bottom - top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,op);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one page in a display device context
to another page within the same device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Destination device context
srcPage - Page in DC to get the source data from
r       - Rectangle defining are to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
op      - Write mode to use during Blt

REMARKS:
This function is the same as MGL_copyPageCoord, however it takes entire
rectangles as parameters instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_copyPage(
    MGLDC *dc,
    int srcPage,
    rect_t r,
    int dstLeft,
    int dstTop,
    int op);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Copies a block of image data with source transparency.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
left        - Left coordinate of source image
top         - Top coordinate of source image
right       - Right coordinate of source image
bottom      - Bottom coordinate of source image
dstLeft     - Left coordinate of destination
dstTop      - Top coordinate of destination
transparent - Transparent color to skip in source image
op          - Write mode to use during Blt

REMARKS:
Copies a block of bitmap data form one device context to another with either
source or destination transparency. When transferring the data with source
transparency, for pixels in the source image that are equal to the specified
transparent color, the related pixel in the destination buffer will remain
untouched. This allows you to quickly transfer sprites between device
contexts with a single color being allocated as a transparent color.

This routine has been highly optimized for maximum performance in all pixel
depths, so will provide a very fast method for performing transparent sprite
animation. However you may find that if you can use alternative techniques to
pre-compile the sprites (like using run length encoding etc.) you will be
able to build faster software based sprite animation code that can directly
access the device context surface. However this routine can also be used to
perform hardware accelerated Blt's between offscreen memory device's and the
display device when running in fullscreen modes, providing the hardware
accelerator (if present) can support this operation. If you have a hardware
accelerator capable of this, this will provide the ultimate performance for
transparent sprite animation.

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_srcTransBltCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom,ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for valid contexts */
    if ((src == dst) || src->mi.bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dst);

    /* Clip to source device context */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;           dstTop = d.top;

    /* Now perform the blit operation */
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        /* Copy from one DC to any other DC when one is in system memory, optionally
         * doing color conversion on the fly.
         */
        if (NEED_TRANSLATE_DC(src,dst)) {
            gaFx.dwSize = sizeof(gaFx);
            gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeySrcSingle;
            gaFx.Mix = op;
            gaFx.ColorKeyLo = transparent;
            gaFx.BitsPerPixel = src->mi.bitsPerPixel;
            gaFx.PixelFormat = GAPF(&src->pf);
            gaFx.SrcPalette = GAPAL(src->colorTab);
            gaFx.DstPalette = GAPAL(dst->colorTab);
            SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                gaFx.TranslateVec = dstPal;
                }
            else if (ditherMode)
                gaFx.Flags |= gaBltDither;
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                srcLeft = left - (dst->viewPort.left - dst->viewPortOrg.x);
                srcTop = top - (dst->viewPort.top - dst->viewPortOrg.y);
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft += (r.left - dstLeft);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop += (r.top - dstTop);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dstLeft = r.left;
                        dstTop = r.top;
                        dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    right - left,bottom - top,&gaFx);
                }
            }
        else {
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dst->r.SrcTransBltSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,op,transparent,false);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.SrcTransBltSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    op,transparent,false);
                }
            }
        }
    else {
        /* BitBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dstLeft = r.left;
                    dstTop = r.top;
                    dst->r.SrcTransBltBuf(src->activeBuf,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        dstLeft,dstTop,op,transparent);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.SrcTransBltBuf(src->activeBuf,
                left + src->viewPort.left,top + src->viewPort.top,
                right - left,bottom - top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,op,transparent);
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Copies a block of image data with destination transparency.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
left        - Left coordinate of source image
top         - Top coordinate of source image
right       - Right coordinate of source image
bottom      - Bottom coordinate of source image
dstLeft     - Left coordinate of destination
dstTop      - Top coordinate of destination
transparent - Transparent color to skip in source image
op          - Write mode to use during Blt

REMARKS:
Copies a block of bitmap data form one device context to another with either
source or destination transparency. When transferring the data with
destination transparency, pixels in the destination image that are equal to
the specified transparent color will be updated, and those pixels that are
not the same will be skipped. This is effectively the operation performed
for 'blueScreen'ing or color keying and can also be used for drawing
transparent sprites. Note however that destination transparency is very
slow in software compared to source transparency!

This routine has been highly optimized for maximum performance in all pixel
depths, so will provide a very fast method for performing transparent sprite
animation. However you may find that if you can use alternative techniques to
pre-compile the sprites (like using run length encoding etc.) you will be
able to build faster software based sprite animation code that can directly
access the device context surface. However this routine can also be used to
perform hardware accelerated Blt's between offscreen memory device's and the
display device when running in fullscreen modes, providing the hardware
accelerator (if present) can support this operation. If you have a hardware
accelerator capable of this, this will provide the ultimate performance for
transparent sprite animation.

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

Note:   If you are doing pixel format conversion at the same time (ie:
        color depth for source bitmap is different to the destination
        bitmap), then the transparent color value must be set to the
        traslated destination pixel format. Ie: if you are blitting an
        8bpp bitmap to a 32bpp device context, the transparent color must
        be a 32bpp value.

Note:   This routine also only works with pixel depths that are at least
        4 bits deep.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_dstTransBltCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    color_t transparent,
    int op)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom,ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for valid contexts */
    if ((src == dst) || src->mi.bitsPerPixel < 4) {
        SETERROR(grInvalidDevice);
        return;
        }

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dst);

    /* Clip to source device context */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination device context */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);
    dstLeft = d.left;           dstTop = d.top;

    /* Now perform the blit operation */
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        /* Copy from one DC to any other DC when one is in system memory, optionally
         * doing color conversion on the fly.
         */
        if (NEED_TRANSLATE_DC(src,dst)) {
            gaFx.dwSize = sizeof(gaFx);
            gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltColorKeyDstSingle;
            gaFx.Mix = op;
            gaFx.ColorKeyLo = transparent;
            gaFx.BitsPerPixel = src->mi.bitsPerPixel;
            gaFx.PixelFormat = GAPF(&src->pf);
            gaFx.SrcPalette = GAPAL(src->colorTab);
            gaFx.DstPalette = GAPAL(dst->colorTab);
            SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                gaFx.TranslateVec = dstPal;
                }
            else if (ditherMode)
                gaFx.Flags |= gaBltDither;
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                srcLeft = left - (dst->viewPort.left - dst->viewPortOrg.x);
                srcTop = top - (dst->viewPort.top - dst->viewPortOrg.y);
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft += (r.left - dstLeft);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop += (r.top - dstTop);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dstLeft = r.left;
                        dstTop = r.top;
                        dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    right - left,bottom - top,&gaFx);
                }
            }
        else {
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = d.left + (right-left);
                d.bottom = d.top + (bottom-top);
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        srcLeft = left + (r.left - d.left);
                        srcRight = srcLeft + (r.right - r.left);
                        srcTop = top + (r.top - d.top);
                        srcBottom = srcTop + (r.bottom - r.top);
                        dst->r.DstTransBltSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,op,transparent,false);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.DstTransBltSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    op,transparent,false);
                }
            }
        }
    else {
        /* BitBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dstLeft = r.left;
                    dstTop = r.top;
                    dst->r.DstTransBltBuf(src->activeBuf,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        dstLeft,dstTop,op,transparent);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.DstTransBltBuf(src->activeBuf,
                left + src->viewPort.left,top + src->viewPort.top,
                right - left,bottom - top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,op,transparent);
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Copies a block of image data with source transparency.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
srcRect     - Rectangle defining source image
dstLeft     - Left coordinate of destination
dstTop      - Top coordinate of destination
transparent - Transparent color to skip in source image
op          - Write mode to use during Blt

REMARKS:
This function is the same as MGL_srcTransBltCoord, however it takes a
rectangle as a parameter instead of the four coordinates of a rectangle.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_srcTransBlt(
    MGLDC *dst,
    MGLDC *src,
    rect_t srcRect,
    int dstLeft,
    int dstTop,
    color_t transparent,
    int op);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Copies a block of image data with destination transparency.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
srcRect     - Rectangle defining source image
dstLeft     - Left coordinate of destination
dstTop      - Top coordinate of destination
transparent - Transparent color to skip in source image
op          - Write mode to use during Blt

REMARKS:
This function is the same as MGL_dstTransBltCoord, however it takes a
rectangle as a parameter instead of the four coordinates of a rectangle.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_dstTransBlt(
    MGLDC *dst,
    MGLDC *src,
    rect_t srcRect,
    int dstLeft,
    int dstTop,
    color_t transparent,
    int op);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one device context into another while
applying a mono or color pattern.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
left        - Left coordinate of image to Blt from
top         - Top coordinate of image to Blt from
right       - Right coordinate of image to Blt from
bottom      - Bottom coordinate of image to Blt from
dstLeft     - Left coordinate to Blt to
dstTop      - Right coordinate to Blt to
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
Copies a block of bitmap data from one device context to another, while applying
either a mono bitmap pattern or a color pixmap pattern to the data with a
ternary raster operation code (ROP3). If the usePixMap parameter is set to
true, the current pixmap pattern set by MGL_setPenPixmapPattern will be
applied as pattern data, otherwise the current monochrome bitmap pattern
set by MGL_setPenBitmapPattern will be applied.

The source and destination rectangles may overlap even if the source and
destination device contexts are the same, and MGL will correctly handle the
overlapping regions.

This function will only work with Blt's between device contexts that have
identical pixel formats. If the color depth or pixel formats are different,
this function will produce undefined results.

The ROP3 code specifies how the source, pattern and destination image data
should be combined to produce the final result. SciTech MGL supports all 256
ROP3 codes, and they are enumerated in MGL_rop3CodesType.

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

Note:   This function is not designed to support overlapping source
        and destination rectangles on the same device context so if
        the source and destination rectangles overlap on the same
        device context, the results are undefined.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_bitBltPattCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int usePixMap,
    int rop3)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dst);

    /* Clip to source rectangle */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination rectangle */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Now perform the blit operation */
    dstLeft = d.left;   dstTop = d.top;
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        /* Copy from one DC to any other DC when one is in system memory */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    if (usePixMap) {
                        dst->r.BitBltColorPattSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,rop3,false);
                        }
                    else {
                        dst->r.BitBltPattSys(src->surface,src->mi.bytesPerLine,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            r.left,r.top,rop3,false);
                        }
                    }
            END_CLIP_REGION();
            }
        else {
            if (usePixMap) {
                dst->r.BitBltColorPattSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    rop3,false);
                }
            else {
                dst->r.BitBltPattSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    rop3,false);
                }
            }
        }
    else {
        /* BitBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dstLeft = r.left;
                    dstTop = r.top;
                    if (usePixMap) {
                        dst->r.BitBltColorPattBuf(src->activeBuf,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,rop3);
                        }
                    else {
                        dst->r.BitBltPattBuf(src->activeBuf,
                            srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                            dstLeft,dstTop,rop3);
                        }
                    }
            END_CLIP_REGION();
            }
        else {
            if (usePixMap) {
                dst->r.BitBltColorPattBuf(src->activeBuf,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,rop3);
                }
            else {
                dst->r.BitBltPattBuf(src->activeBuf,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right - left,bottom - top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,rop3);
                }
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Blts a block of image data from one device context into another while
applying a mono or color pattern.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
r           - Rectangle defining are to Blt from
dstLeft     - Left coordinate to Blt to
dstTop      - Right coordinate to Blt to
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
This function is the same as MGL_bitBltPattCoord, however it takes entire
rectangles as parameters instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_bitBltPatt(
    MGLDC *dst,
    MGLDC *src,
    rect_t r,
    int dstLeft,
    int dstTop,
    int usePixMap,
    int rop3);
/* Implemented as a macro */

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
ibool MGLAPI MGL_bitBltFxTest(
    MGLDC *dst,
    MGLDC *src,
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
    if (NEED_TRANSLATE_DC(src,dst)) {
        gaFx.Flags |= gaBltConvert;
        gaFx.BitsPerPixel = src->mi.bitsPerPixel;
        gaFx.PixelFormat = GAPF(&src->pf);
        gaFx.SrcPalette = GAPAL(src->colorTab);
        }
    return dst->r.BitBltFxTest(&gaFx);
}

/****************************************************************************
DESCRIPTION:
Copies a block of image data from one device context into another, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
left    - Left coordinate of image to Blt from
top     - Top coordinate of image to Blt from
right   - Right coordinate of image to Blt from
bottom  - Bottom coordinate of image to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Copies a block of bitmap data from one device context to another, with optional
effects applied. The effects applied to the blit operation range from X and
Y bitmap flipping to color transparency and blending. All of these operations
can be applied to data being copied between different device context of
different color depths and pixel formats if desired (ie: 32-bit ARGB
alpha blended bitmap to a 16-bit device context). Please refer to the
documentation for the bltfx_t structure and the MGL_bitBltFxFlagsType
enumeration that defines the flags passed to this function.

This function will correctly handle effects Blt's across device contexts with
differing pixel depths, and will perform the necessary pixel format translation
to convert from the source device to the destination device. Note that although
the code to implement this is highly optimized, this can be a time consuming
operation so you should attempt to pre-convert all bitmaps to the current
display device pixel format for maximum performance if using this routine for
sprite animation.

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

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

Note:   This function is not designed to support overlapping source
        and destination rectangles on the same device context so if
        the source and destination rectangles overlap on the same
        device context, the results are undefined.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_bitBltFxCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom,t;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* In order to clip the results, we first clip the source rectangle to
     * the source device context, and then clip the destination rectangle to
     * the destination device context.
     */
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dst);

    /* Clip to source rectangle */
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to destination rectangle */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstLeft + (right-left);
    d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    left += (d.left - dstLeft); right = left + (d.right - d.left);
    top += (d.top - dstTop);    bottom = top + (d.bottom - d.top);

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = src->size.bottom - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = src->size.right - right;
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

    /* Now perform the blit */
    dstLeft = d.left;   dstTop = d.top;
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        /* Copy from one DC to any other DC when one is in system memory */
        if (NEED_TRANSLATE_DC(src,dst)) {
            gaFx.Flags |= gaBltConvert;
            gaFx.BitsPerPixel = src->mi.bitsPerPixel;
            gaFx.PixelFormat = GAPF(&src->pf);
            gaFx.SrcPalette = GAPAL(src->colorTab);
            gaFx.DstPalette = GAPAL(dst->colorTab);
            SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec;
                gaFx.TranslateVec = dstPal;
                }
            }
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            srcLeft = left - (dst->viewPort.left - dst->viewPortOrg.x);
            srcTop = top - (dst->viewPort.top - dst->viewPortOrg.y);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft += (r.left - dstLeft);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop += (r.top - dstTop);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dstLeft = r.left;
                    dstTop = r.top;
                    dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                left + src->viewPort.left,top + src->viewPort.top,
                right - left,bottom - top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                right - left,bottom - top,&gaFx);
            }
        }
    else {
        /* BitBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = d.left + (right-left);
            d.bottom = d.top + (bottom-top);
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    srcLeft = left + (r.left - d.left);
                    srcRight = srcLeft + (r.right - r.left);
                    srcTop = top + (r.top - d.top);
                    srcBottom = srcTop + (r.bottom - r.top);
                    dstLeft = r.left;
                    dstTop = r.top;
                    dst->r.BitBltFxBuf(src->activeBuf,
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        dstLeft,dstTop,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.BitBltFxBuf(src->activeBuf,
                left + src->viewPort.left,top + src->viewPort.top,
                right - left,bottom - top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                right - left,bottom - top,&gaFx);
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Copies a block of image data from one device context into another, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
r       - Rectangle defining are to Blt from
dstLeft - Left coordinate to Blt to
dstTop  - Right coordinate to Blt to
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
This function is the same as MGL_bitBltFxCoord, however it takes entire
rectangles as parameters instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_bitBltFx(
    MGLDC *dst,
    MGLDC *src,
    rect_t r,
    int dstLeft,
    int dstTop,
    bltfx_t *fx);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Stretches a block of image data from one device context to another.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
left        - Left coordinate of source image
top         - Top coordinate of source image
right       - Right coordinate of source image
bottom      - Bottom coordinate of source image
dstLeft     - Left coordinate of destination image
dstTop      - Top coordinate of destination image
dstRight    - Right coordinate of destination image
dstBottom   - Bottom coordinate of destination image
op          - Write mode to use during Blt

REMARKS:
Copies a block of bitmap data form one device context to another, stretching or
shrinking the image as necessary to fit the destination rectangle for the destination
device context.

The source and destination device context may be the same, however the
source and destination rectangles may not overlap. This routine
has been highly optimized for absolute maximum performance, so it will
provide the fastest method of stretching bitmap data between device contexts,
and can also be used to stretch bitmap data from a memory device context
to a windowed device context.

This function will correctly handle StretchBlt's across device contexts with
differing pixel depths, and will perform the necessary pixel format
translation to convert from the source device to the destination device.
Note that although the code to implement this is highly optimized, this can
be a time consuming operation so you should attempt to pre-convert all
bitmaps to the current display device pixel format for maximum performance
if possible.

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

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively, however the zoom factor is determined using the unclipped
source and destination rectangles.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_stretchBltCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    int op)
{
    rect_t      d,r,clip;
    int         deltaSrc,deltaDst;
    int         fdTop, fdLeft;
    fix32_t     zoomx,zoomy;
    ibool       clipIt = false;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for valid device contexts */
    if ((src == dst) || src->mi.bitsPerPixel < 8) {
        SETERROR(grInvalidDevice);
        return;
        }
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;

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
        MGL_bitBltCoord(dst,src,left,top,right,bottom,dstLeft,dstTop,op);
        return;
        }

    /* Clip to the source device context */
    BEGIN_VISIBLE_CLIP_LIST(dst);
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
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

    /* Clip the destination device context */
    d.left = dstLeft;           d.top = dstTop;
    d.right = dstRight;         d.bottom = dstBottom;
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }
    if ((d.left != dstLeft) || (d.right  != dstRight) || (d.top != dstTop) || (d.bottom != dstBottom))
        clipIt = true;

    /* Now perform the blit operation */
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        /* Copy from one DC to any other DC when one is in system memory */
        if (NEED_TRANSLATE_DC(src,dst)) {
            /* Translate the pixel information when doing the stretch Blt */
            gaFx.dwSize = sizeof(gaFx);
            gaFx.Flags = gaBltConvert | gaBltMixEnable | gaBltStretchNearest;
            gaFx.Mix = op;
            gaFx.BitsPerPixel = src->mi.bitsPerPixel;
            gaFx.PixelFormat = GAPF(&src->pf);
            gaFx.SrcPalette = GAPAL(src->colorTab);
            gaFx.DstPalette = GAPAL(dst->colorTab);
            SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec | gaBltDither;
                gaFx.TranslateVec = dstPal;
                }
            else if (ditherMode)
                gaFx.Flags |= gaBltDither;
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = dstRight + dst->viewPort.left;
                d.bottom = dstBottom + dst->viewPort.top;
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
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
                        dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                            left,top,right-left,bottom-top,
                            d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                        }
                END_CLIP_REGION();
                }
            else {
                if (clipIt) {
                    gaFx.Flags |= gaBltClip;
                    gaFx.ClipLeft = dst->clipRectScreen.left;
                    gaFx.ClipTop = dst->clipRectScreen.top;
                    gaFx.ClipRight = dst->clipRectScreen.right;
                    gaFx.ClipBottom = dst->clipRectScreen.bottom;
                    }
                dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right-left,bottom-top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    dstRight-dstLeft,dstBottom-dstTop,&gaFx);
                }
            }
        else {
            /* Perform the stretch blit with no pixel format conversion */
            if (dst->clipRegionScreen) {
                left += src->viewPort.left;
                top += src->viewPort.top;
                right += src->viewPort.left;
                bottom += src->viewPort.top;
                d.left = dstLeft + dst->viewPort.left;
                d.top = dstTop + dst->viewPort.top;
                d.right = dstRight + dst->viewPort.left;
                d.bottom = dstBottom + dst->viewPort.top;
                BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                    if (MGL_sectRect(clip,d,&r)) {
                        dst->r.StretchBltSys(src->surface,src->mi.bytesPerLine,
                            left,top,right-left,bottom-top,
                            d.left,d.top,d.right-d.left,d.bottom-d.top,!MGL_equalRect(d,r),
                            r.left,r.top,r.right,r.bottom,op,false);
                        }
                END_CLIP_REGION();
                }
            else {
                dst->r.StretchBltSys(src->surface,src->mi.bytesPerLine,
                    left + src->viewPort.left,top + src->viewPort.top,
                    right-left,bottom-top,
                    dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                    dstRight-dstLeft,dstBottom-dstTop,clipIt,
                    dst->clipRectScreen.left,dst->clipRectScreen.top,
                    dst->clipRectScreen.right,dst->clipRectScreen.bottom,
                    op,false);
                }
            }
        }
    else {
        /* StretchBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = dstRight + dst->viewPort.left;
            d.bottom = dstBottom + dst->viewPort.top;
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
                if (MGL_sectRect(clip,d,&r)) {
                    dst->r.StretchBltBuf(src->activeBuf,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,!MGL_equalRect(d,r),
                        r.left,r.top,r.right,r.bottom,op);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.StretchBltBuf(src->activeBuf,
                left + src->viewPort.left,top + src->viewPort.top,
                right-left,bottom-top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,clipIt,
                dst->clipRectScreen.left,dst->clipRectScreen.top,
                dst->clipRectScreen.right,dst->clipRectScreen.bottom,op);
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Stretches a block of image data from one device context to another.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
srcRect - Rectangle defining source image
dstRect - Rectangle defining destination image
op      - Write mode to use during Blt

REMARKS:
This function is the same as MGL_stretchBltCoord, however it takes entire
rectangles as arguments instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_stretchBlt(
    MGLDC dst,
    MGLDC src,
    rect_t srcRect,
    rect_t dstRect,
    int op);
/* Implemented as a macro */

/****************************************************************************
DESCRIPTION:
Stretches a block of image data from one device context to another, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dst         - Destination device context
src         - Source device context
left        - Left coordinate of source image
top         - Top coordinate of source image
right       - Right coordinate of source image
bottom      - Bottom coordinate of source image
dstLeft     - Left coordinate of destination image
dstTop      - Top coordinate of destination image
dstRight    - Right coordinate of destination image
dstBottom   - Bottom coordinate of destination image
fx          - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Copies a block of bitmap data form one device context to another, stretching or
shrinking the image as necessary to fit the destination rectangle for the destination
device context, while applying optional effects. This function behaves identically
to the MGL_bitBltFxCoord function, except it includes the ability to stretch the
bitmap at the same time. You can use the flags member of the bltfx_t structure
to define whether stretching is done using nearest color stretching or if pixel
interpolation will be used. If you don't specify any flags, nearest color
stretching will be used.

This function will correctly handle effects Blt's across device contexts with
differing pixel depths, and will perform the necessary pixel format translation
to convert from the source device to the destination device. Note that although
the code to implement this is highly optimized, this can be a time consuming
operation so you should attempt to pre-convert all bitmaps to the current
display device pixel format for maximum performance if using this routine for
sprite animation.

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

The source and destination rectangles are clipped according to the current
clipping rectangles for the source and destination device contexts
respectively.

Note:   This function is not designed to support overlapping source
        and destination rectangles on the same device context so if
        the source and destination rectangles overlap on the same
        device context, the results are undefined.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGLAPI MGL_stretchBltFxCoord(
    MGLDC *dst,
    MGLDC *src,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         deltaSrc,deltaDst,t;
    int         fdTop, fdLeft;
    fix32_t     zoomx,zoomy;
    int         ditherMode;
    void        *dstPal;
    GA_bltFx    gaFx;

    /* Check for valid device contexts */
    if ((src == dst) || src->mi.bitsPerPixel < 8) {
        SETERROR(grInvalidDevice);
        return;
        }
    if (src == _MGL_dcPtr)
        src = &DC;
    if (dst == _MGL_dcPtr)
        dst = &DC;

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
        MGL_bitBltFxCoord(dst,src,left,top,right,bottom,dstLeft,dstTop,fx);
        return;
        }

    /* Clip to the source device context */
    BEGIN_VISIBLE_CLIP_LIST(dst);
    d.left = left;              d.top = top;
    d.right = right;            d.bottom = bottom;
    if (!MGL_sectRect(src->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
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

    /* Clip the destination device context */
    d.left = dstLeft;           d.top = dstTop;
    d.right = dstRight;         d.bottom = dstBottom;
    if (!MGL_sectRect(dst->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dst);
        return;
        }

    /* Adjust clipped coordinates if we are doing X or Y flipping */
    if (fx->flags & MGL_BLT_FLIPY) {
        t = (bottom - top);
        top = src->size.bottom - bottom;
        bottom = top + t;
        }
    if (fx->flags & MGL_BLT_FLIPX) {
        t = (right - left);
        left = src->size.right - right;
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
        gaFx.ClipLeft = dst->clipRectScreen.left;
        gaFx.ClipTop = dst->clipRectScreen.top;
        gaFx.ClipRight = dst->clipRectScreen.right;
        gaFx.ClipBottom = dst->clipRectScreen.bottom;
        }

    /* Now perform the blit operation */
    MAKE_HARDWARE_CURRENT(dst,true);
    if (src->deviceType == MGL_MEMORY_DEVICE || dst->deviceType == MGL_MEMORY_DEVICE) {
        if (NEED_TRANSLATE_DC(src,dst)) {
            gaFx.Flags |= gaBltConvert;
            gaFx.BitsPerPixel = src->mi.bitsPerPixel;
            gaFx.PixelFormat = GAPF(&src->pf);
            gaFx.SrcPalette = GAPAL(src->colorTab);
            gaFx.DstPalette = GAPAL(dst->colorTab);
            SETUP_DITHER_MODE(dst,(src->mi.modeFlags & MGL_IS_COLOR_INDEX),true);
            if (ditherMode == -1) {
                gaFx.Flags |= gaBltTranslateVec;
                gaFx.TranslateVec = dstPal;
                }
            }
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = dstRight + dst->viewPort.left;
            d.bottom = dstBottom + dst->viewPort.top;
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
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
                    dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.BitBltFxSys(src->surface,src->mi.bytesPerLine,
                left + src->viewPort.left,top + src->viewPort.top,
                right-left,bottom-top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,&gaFx);
            }
        }
    else {
        /* StretchBlt between two device contexts which may both be in
         * hardware. We allow the MGL device driver determine how to
         * handle the case, so that it can be done in hardware if
         * available (such as blitting from offscreen DC or back buffers).
         */
        if (dst->clipRegionScreen) {
            left += src->viewPort.left;
            top += src->viewPort.top;
            right += src->viewPort.left;
            bottom += src->viewPort.top;
            d.left = dstLeft + dst->viewPort.left;
            d.top = dstTop + dst->viewPort.top;
            d.right = dstRight + dst->viewPort.left;
            d.bottom = dstBottom + dst->viewPort.top;
            BEGIN_CLIP_REGION(clip,dst->clipRegionScreen);
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
                    dst->r.BitBltFxBuf(src->activeBuf,
                        left,top,right-left,bottom-top,
                        d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                    }
            END_CLIP_REGION();
            }
        else {
            dst->r.BitBltFxBuf(src->activeBuf,
                left + src->viewPort.left,top + src->viewPort.top,
                right-left,bottom-top,
                dstLeft + dst->viewPort.left,dstTop + dst->viewPort.top,
                dstRight-dstLeft,dstBottom-dstTop,&gaFx);
            }
        }
    RESTORE_HARDWARE(dst,true);
    END_VISIBLE_CLIP_LIST(dst);
}

/****************************************************************************
DESCRIPTION:
Stretches a block of image data from one device context to another, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source device context
srcRect - Rectangle defining source image
dstRect - Rectangle defining destination image
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
This function is the same as MGL_stretchBltFxCoord, however it takes entire
rectangles as arguments instead of coordinates.

SEE ALSO:
MGL_bitBlt, MGL_bitBltCoord, MGL_srcTransBlt, MGL_srcTransBltCoord,
MGL_dstTransBlt, MGL_dstTransBltCoord, MGL_bitBltPatt, MGL_bitBltPattCoord,
MGL_bitBltFx, MGL_bitBltFxCoord, MGL_stretchBlt, MGL_stretchBltCoord,
MGL_stretchBltFx, MGL_stretchBltFxCoord, MGL_copyPage, MGL_copyPageCoord
****************************************************************************/
void MGL_stretchBltFx(
    MGLDC dst,
    MGLDC src,
    rect_t srcRect,
    rect_t dstRect,
    bltfx_t *fx);
/* Implemented as a macro */

