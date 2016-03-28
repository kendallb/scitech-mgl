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
* Description:  Offscreen buffer management code.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Creates a new MGL memory buffer for storing bitmaps

HEADER:
mgraph.h

PARAMETERS:
dc      - MGL display or windowed device context to allocate buffer from
width   - Width of the buffer in pixels
height  - Height of the buffer in scanlines
flags   - Flags to use when creating the buffer

RETURNS:
Pointer to the allocated buffer, NULL on failure.

REMARKS:
This function allocates a new lightweight bitmap buffer in offscreen display
memory. Lighweight bitmap buffers are used to store bitmap and sprite
information in the offscreen display memory in the hardware, but are not
full device contexts. Hence buffers have much less memory overhead than a
full offscreen device context. Buffers can only be used for storing bitmaps
and blitting them around on the screen. You can copy the contents to a MGL
device context using the MGL_putBuffer, MGL_stretchBuffer and
MGL_putBufferSrcTrans functions. You can also copy the contents of an
MGL device context to a buffer using the MGL_copyToBuffer function.

If you need to draw on a buffer in offscreen memory, create a full
offscreen device context instead. Then you can call any of the standard
MGL drawing functions and BitBlt operations for the offscreen memory buffer.
The primary disadvantage of doing this is that a full offscreen device
context has a lot more memory overhead involved in maintaining the
device context state information than a simple offscreen buffer.

If you flag the MGL_BUF_CACHED flag, MGL will always keep a cached copy of
the surface memory for the buffer in system memory. If your offscreen
buffers get lost (ie: on a fullscreen mode switch), they can be automatically
be restored when the application regains the active focus. This also allows
the MGL to compact the offscreen memory heap when necessary. If you don't
set the cacheIt parameter to true, then it is your applications
responsibility to reload the bitmaps when the focus is lost and regained.
Note however that the MGL is /not/ responsible for maintaining the contents
of the buffer cache memory, so if you draw directly on the buffer surface
you should ensure that the cache is kept up to date if you wish for your
buffers to be cached. Note that the MGL_copyToBuffer and
MGL_copyBitmapToBuffer functions will ensure that both the buffer cache
and video memory buffer contents are updated if the buffer is cached. Hence
if you are only using the buffers to store static sprite information,
you can use MGL_copyToBuffer() or MGL_copyBitmapToBuffer() and never
have to worry about keeping the buffer cache in sync.

Note:   The MGL automatically manages offscreen display memory, and if you
        run out of offscreen display memory it will place the buffer
        surfaces in system memory (unless you pass the MGL_BUF_NOSYSMEM
        flag). Hence you should allocate your important buffers first,
        to ensure they end up in offscreen memory for speedy drawing.

Note:   This function is only valid for display device contexts, and
        will fail if you call it for a different device context type
        with an error code of grInvalidDC.

SEE ALSO:
MGL_createBuffer, MGL_lockBuffer, MGL_unlockBuffer, MGL_destroyBuffer,
MGL_createOffscreenDC, MGL_putBuffer, MGL_stretchBuffer, MGL_putBufferSrcTrans,
MGL_putBufferDstTrans, MGL_copyToBuffer, MGL_copyBitmapToBuffer,
MGL_updateBufferCache, MGL_updateFromBufferCache
****************************************************************************/
MGLBUF * MGLAPI MGL_createBuffer(
    MGLDC *dc,
    int width,
    int height,
    M_uint32 flags)
{
    MGLBUF  *buf;

    /* Check that the DC is valid, and create the buffer */
    CHECK(dc != NULL);
    if (dc->r.AllocBuffer) {
        if ((buf = (MGLBUF*)dc->r.AllocBuffer(width,height,flags)) != NULL)
            buf->dc = dc;
        return buf;
        }
    FATALERROR(grInvalidDC);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Lock a buffer to begin direct surface access

HEADER:
mgraph.h

PARAMETERS:
buf - MGL buffer to lock

RETURNS:
Physical address of locked buffer in memory, 0 if a system memory buffer.

REMARKS:
This function locks a buffer so that an application can begin drawing
directly on the surface memory. You /must/ call this function before you
draw directly on the bitmap surface!

The return value from this function is the physical start address of the
buffer in memory, which can be used to program DMA operations from other
hardware devices directly into the video memory for the buffer. This is
useful for frame grabber devices so that the resulting frame from the
frame grabber device can be blitted to the visual display as quickly as
possible (much quicker than if it was DMA'ed into system memory).

If the buffer is a system memory buffer, the return value from this
function will be 0, since we cannot obtain the physical starting address
of a system memory buffer.

SEE ALSO:
MGL_unlockBuffer
****************************************************************************/
ulong MGLAPI MGL_lockBuffer(
    MGLBUF *buf)
{
    CHECK(buf != NULL);
    return buf->dc->r.LockBuffer(TO_BUF(buf));
}

/****************************************************************************
DESCRIPTION:
Unlock a buffer after direct surface access

HEADER:
mgraph.h

PARAMETERS:
buf - MGL buffer to unlock

REMARKS:
This function unlocks a buffer after the application has completed direct
surface access on the buffer.

SEE ALSO:
MGL_lockBuffer
****************************************************************************/
void MGLAPI MGL_unlockBuffer(
    MGLBUF *buf)
{
    CHECK(buf != NULL);
    buf->dc->r.UnlockBuffer(TO_BUF(buf));
}

/****************************************************************************
DESCRIPTION:
Destroys an existing MGL buffer

HEADER:
mgraph.h

PARAMETERS:
buf         - MGL buffer to destroy

REMARKS:
This function destroys an MGL buffer, and frees all resources associated
with the buffer.

SEE ALSO:
MGL_createBuffer
****************************************************************************/
void MGLAPI MGL_destroyBuffer(
    MGLBUF *buf)
{
    if (!buf)
        return;
    CHECK(buf->dc->r.FreeBuffer);
    buf->dc->r.FreeBuffer(TO_BUF(buf));
}

/****************************************************************************
PARAMETERS:
width               - Width of the source bitmap
height              - Height of the source bitmap
bytesPerLine        - Pitch of the source bitmap
bitsPerPixel        - Color depth of the source bitmap
surface             - Surface of the source bitmap
left                - Left coordinate in device context to blit from
top                 - Top coordinate in device context to blit from
right               - Right coordinate in device context to blit from
bottom              - Bottom coordinate in device context to blit from
dstLeft             - Left coordinate in buffer to blit to
dstTop              - Top coordinate in buffer to blit to
buf                 - MGL buffer to blit to
cacheBytesPerLine   - Pitch of the surface cache
surfaceCache        - Surface cache bitmap to copy to (NULL to ignore)

REMARKS:
Internal function to do the work of coping data to a buffer.
****************************************************************************/
static void __MGL_copyToBuffer(
    int width,
    int height,
    int bytesPerLine,
    int bitsPerPixel,
    void *surface,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf)
{
    rect_t  d;
    int     widthBytes,cached = false;
    uchar   *src,*dst,*cache = NULL;

    /* Clip the source rectangle to the bitmap dimensions */
    if (!MGL_sectRectCoord(0,0,width,height,left,top,right,bottom,&d))
        return;
    dstLeft += (d.left - left); dstTop += (d.top - top);
    left = d.left;              right = d.right;
    top = d.top;                bottom = d.bottom;

    /* Clip to buffer dimensions */
    d.left = dstLeft;                   d.top = dstTop;
    d.right = dstLeft + (right-left);   d.bottom = dstTop + (bottom-top);
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,d.left,d.top,d.right,d.bottom,&d))
        return;
    left += (d.left - dstLeft);
    top += (d.top - dstTop);

    /* Lock the buffer so we can access the surface directly */
    if (!(buf->flags & MGL_BUF_SYSMEM))
        buf->dc->r.LockBuffer(TO_BUF(buf));

    /* Now copy the bitmap information to both the surface and the surface
     * cache if necessary.
     */
    src = PIXEL_ADDR(left,top,surface,bytesPerLine,bitsPerPixel);
    dst = PIXEL_ADDR(dstLeft,dstTop,buf->surface,buf->bytesPerLine,bitsPerPixel);
    if (buf->flags & MGL_BUF_CACHED) {
        cache = PIXEL_ADDR(left,top,buf->surfaceCache,buf->cacheBytesPerLine,bitsPerPixel);
        cached = true;
        }
    widthBytes = BYTESPERLINE((d.right-d.left),bitsPerPixel);
    while (height--) {
        MGL_memcpy(dst,src,widthBytes);
        if (cached)
            MGL_memcpy(cache,src,widthBytes);
        src += bytesPerLine;
        dst += buf->bytesPerLine;
        cache += buf->cacheBytesPerLine;
        }
    if (!(buf->flags & MGL_BUF_SYSMEM))
        buf->dc->r.UnlockBuffer(TO_BUF(buf));
}

/****************************************************************************
DESCRIPTION:
Copies a section of a device context to an offscreen buffer

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to blit from
left    - Left coordinate in device context to blit from
top     - Top coordinate in device context to blit from
right   - Right coordinate in device context to blit from
bottom  - Bottom coordinate in device context to blit from
dstLeft - Left coordinate in buffer to blit to
dstTop  - Top coordinate in buffer to blit to
buf     - MGL buffer to blit to

REMARKS:
This function is used to copy bitmap data from an MGL device context into
an offscreen buffer. The information copied is clipped to the full dimensions
of the buffer, and the source clipping rectangle in the source device context
is ignored.

SEE ALSO:
MGL_copyBitmapToBuffer, MGL_putBuffer
****************************************************************************/
void MGLAPI MGL_copyToBuffer(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf)
{
    CHECK(dc);
    CHECK(buf);
    CHECK(dc->mi.bitsPerPixel == buf->dc->mi.bitsPerPixel);
    __MGL_copyToBuffer(dc->mi.xRes+1,dc->mi.yRes+1,dc->mi.bytesPerLine,
        dc->mi.bitsPerPixel,dc->surface,left,top,right,bottom,
        dstLeft,dstTop,buf);
}

/****************************************************************************
DESCRIPTION:
Copies a section of a bitmap to an offscreen buffer

HEADER:
mgraph.h

PARAMETERS:
bitmap  - Bitmap to blit from
left    - Left coordinate in device context to blit from
top     - Top coordinate in device context to blit from
right   - Right coordinate in device context to blit from
bottom  - Bottom coordinate in device context to blit from
dstLeft - Left coordinate in buffer to blit to
dstTop  - Top coordinate in buffer to blit to
buf     - MGL buffer to blit to

REMARKS:
This function is used to copy bitmap data from a lightweight system memory
bitmap into an offscreen buffer. The information copied is clipped to the
full dimensions of the buffer.

SEE ALSO:
MGL_copyToBuffer, MGL_putBuffer
****************************************************************************/
void MGLAPI MGL_copyBitmapToBuffer(
    bitmap_t *bitmap,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf)
{
    CHECK(bitmap);
    CHECK(buf);
    CHECK(buf->dc);
    CHECK(bitmap->bitsPerPixel == buf->dc->mi.bitsPerPixel);
    __MGL_copyToBuffer(bitmap->width,bitmap->height,bitmap->bytesPerLine,
        bitmap->bitsPerPixel,bitmap->surface,left,top,right,bottom,
        dstLeft,dstTop,buf);
}

/****************************************************************************
DESCRIPTION:
Updates the system cache from the video memory buffer contents

HEADER:
mgraph.h

PARAMETERS:
buf - MGL buffer to copy into system memory cache

REMARKS:
This function is used to update the system memory buffer cache by copying the
contents of the video memory buffer into the system memory cache. This is
mostly useful if you are using cached buffers with a system memory shadow,
and you have updated the video memory buffer and need to flush the changes
to the system memory cache. This operation is not particularly fast (video
memory reads are always slow), but if you need to keep the system memory
cache up to date this is the way to do it.

SEE ALSO:
MGL_updateFromBufferCache, MGL_putBuffer
****************************************************************************/
void MGLAPI MGL_updateBufferCache(
    MGLBUF *buf)
{
    CHECK(buf);
    CHECK(buf->dc);
    CHECK(buf->dc->r.UpdateCache);
    buf->dc->r.UpdateCache(TO_BUF(buf));
}

/****************************************************************************
DESCRIPTION:
Updates the video memory buffer contents from system memory cache

HEADER:
mgraph.h

PARAMETERS:
buf - MGL buffer to copy from system memory cache

REMARKS:
This function updates the video memory buffer by copying the contents of
the system memory cache buffer into the video memory buffer. This is useful
if you need to replace the buffer contents with new values, or you need to
do software rendering on the buffer. Doing the rendering on the system
memory buffer will be faster, and when you are done this function can be
used to update the video memory copy of the buffer. Unless you need to
specifically do some drawing in hardware, updating the system memory
cache and using this function will be faster than updating video memory
and using MGL_updateBufferCache.

SEE ALSO:
MGL_updateBufferCache, MGL_putBuffer
****************************************************************************/
void MGLAPI MGL_updateFromBufferCache(
    MGLBUF *buf)
{
    CHECK(buf);
    CHECK(buf->dc);
    CHECK(buf->dc->r.UpdateFromCache);
    buf->dc->r.UpdateFromCache(TO_BUF(buf));
}

/****************************************************************************
DESCRIPTION:
Copy an offscreen buffer to the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to copy buffer to
x   - x coordinate to copy buffer to
y   - y coordinate to copy buffer to
buf - Buffer to copy
op  - Write mode to use when drawing buffer

REMARKS:
Copies an offscreen buffer to the specified location on the device context.
Supported write modes are enumerated in MGL_writeModeType.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBuffer(
    MGLDC *dc,
    int x,
    int y,
    MGLBUF *buf,
    int op)
{
    rect_t  d,r,clip;
    int     left,top,right,bottom,srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = x;                 d.top = y;
    d.right = x + buf->width;   d.bottom = y + buf->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to buffer coordinates */
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
                dc->r.BitBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltBuf(TO_BUF(buf),
            d.left,d.top,d.right-d.left,d.bottom-d.top,
            x + dc->viewPort.left,y + dc->viewPort.top,op);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy a section of an offscreen buffer to the specified location.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to copy buffer to
left    - Left coordinate of section to copy
top     - Top coordinate of section to copy
right   - Right coordinate of section to copy
bottom  - Bottom coordinate of section to copy
dstLeft - Left coordinate of destination to copy buffer to
dstTop  - Right coordinate of destination to copy buffer to
buf     - Buffer to copy
op      - Write mode to use when drawing buffer

REMARKS:
Copies a section of an offscreen buffer to the specified location on the
device context.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf,
    int op)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip the source rectangle to the buffer dimensions */
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,left,top,right,bottom,&d)) {
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
                dc->r.BitBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,op);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy an offscreen buffer to the specified location with source transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
x           - x coordinate to copy buffer to
y           - y coordinate to copy buffer to
buf         - Buffer to copy
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing buffer

REMARKS:
Copies an offscreen buffer to the specified location on the device context.
Supported write modes are enumerated in MGL_writeModeType.

REMARKS:
Copies a offscreen buffer to the specified location on the device context
with either source or destination transparency.

When transferring the data with source transparency, for pixels in the source
image that are equal to the specified transparent color, the related pixel
in the destination buffer will remain untouched. This allows you to quickly
transfer sprites between device contexts with a single color being allocated
as a transparent color.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferSrcTrans(
    MGLDC *dc,
    int x,
    int y,
    MGLBUF *buf,
    color_t transparent,
    int op)
{
    rect_t  d,r,clip;
    int     left,top,right,bottom;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = x;                 d.top = y;
    d.right = x + buf->width;   d.bottom = y + buf->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to buffer coordinates */
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
                dc->r.SrcTransBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op,transparent);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.SrcTransBltBuf(TO_BUF(buf),
            d.left,d.top,d.right-d.left,d.bottom-d.top,
            x + dc->viewPort.left,y + dc->viewPort.top,
            op,transparent);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy a section of an offscreen buffer to the specified location with
source transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
left        - Left coordinate of section to copy
top         - Top coordinate of section to copy
right       - Right coordinate of section to copy
bottom      - Bottom coordinate of section to copy
dstLeft     - Left coordinate of destination to copy buffer to
dstTop      - Right coordinate of destination to copy buffer to
buf         - Buffer to copy
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing buffer

REMARKS:
Copies a section of an offscreen buffer to the specified location on the
device context with either source or destination transparency.

When transferring the data with source transparency, pixels in the source
image that are equal to the specified transparent color, the related pixel
in the destination buffer will remain untouched. This allows you to quickly
transfer sprites between device contexts with a single color being allocated
as a transparent color.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferSrcTransSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf,
    color_t transparent,
    int op)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip the source rectangle to the bitmap dimensions */
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,left,top,right,bottom,&d)) {
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
                dc->r.SrcTransBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op,transparent);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.SrcTransBltBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            op,transparent);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy an offscreen buffer to the specified location with destination
transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
x           - x coordinate to copy buffer to
y           - y coordinate to copy buffer to
buf         - Buffer to copy
transparent - Transparent color for the bitmap
op          - Write mode to use when drawing buffer

REMARKS:
Copies an offscreen buffer to the specified location on the device context.
Supported write modes are enumerated in MGL_writeModeType.

REMARKS:
Copies a offscreen buffer to the specified location on the device context
with either source or destination transparency.

When transferring the data with destination transparency, pixels in the
destination image that are equal to the specified transparent color will be
updated, and those pixels that are not the same will be skipped. This is
effectively the operation performed for 'blueScreen'ing or color keying and
can also be used for drawing transparent sprites. Note however that
destination transparency is very slow in software compared to source
transparency!

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferDstTrans(
    MGLDC *dc,
    int x,
    int y,
    MGLBUF *buf,
    color_t transparent,
    int op)
{
    rect_t  d,r,clip;
    int     left,top,right,bottom;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = x;                 d.top = y;
    d.right = x + buf->width;   d.bottom = y + buf->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to buffer coordinates */
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
                dc->r.DstTransBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op,transparent);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.DstTransBltBuf(TO_BUF(buf),
            d.left,d.top,d.right-d.left,d.bottom-d.top,
            x + dc->viewPort.left,y + dc->viewPort.top,
            op,transparent);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy a section of an offscreen buffer to the specified location with
destination transparency.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
left        - Left coordinate of section to copy
top         - Top coordinate of section to copy
right       - Right coordinate of section to copy
bottom      - Bottom coordinate of section to copy
dstLeft     - Left coordinate of destination to copy buffer to
dstTop      - Right coordinate of destination to copy buffer to
buf         - Buffer to copy
transparent - Transparent color for the bitmap
sourceTrans - True for source transparency, false for destination transparency
op          - Write mode to use when drawing buffer

REMARKS:
Copies a section of an offscreen buffer to the specified location on the
device context with either source or destination transparency.

When transferring the data with destination transparency, pixels in the
destination image that are equal to the specified transparent color will be
updated, and those pixels that are not the same will be skipped. This is
effectively the operation performed for 'blueScreen'ing or color keying and
can also be used for drawing transparent sprites. Note however that
destination transparency is very slow in software compared to source
transparency!

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferDstTransSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf,
    color_t transparent,
    int op)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip the source rectangle to the bitmap dimensions */
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,left,top,right,bottom,&d)) {
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
                dc->r.DstTransBltBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,op,transparent);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.DstTransBltBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            op,transparent);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy an offscreen buffer to the specified location with an optional pattern.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
x           - x coordinate to copy buffer to
y           - y coordinate to copy buffer to
buf         - Buffer to copy
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
Copies an offscreen buffer to the specified location on the device context,
while applying either a mono bitmap pattern or a color pixmap pattern to the
data with a ternary raster operation code (ROP3). If the usePixMap parameter
is set to true, the current pixmap pattern set by MGL_setPenPixmapPattern
will be applied as pattern data, otherwise the current monochrome bitmap
pattern set by MGL_setPenBitmapPattern will be applied.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferPatt(
    MGLDC *dc,
    int x,
    int y,
    MGLBUF *buf,
    int usePixMap,
    int rop3)
{
    rect_t  d,r,clip;
    int     left,top,right,bottom,srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = x;                 d.top = y;
    d.right = x + buf->width;   d.bottom = y + buf->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }

    /* Adjust to buffer coordinates */
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
                    dc->r.BitBltColorPattBuf(TO_BUF(buf),
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3);
                    }
                else {
                    dc->r.BitBltPattBuf(TO_BUF(buf),
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3);
                    }
                }
        END_CLIP_REGION();
        }
    else {
        if (usePixMap) {
            dc->r.BitBltColorPattBuf(TO_BUF(buf),
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,rop3);
            }
        else {
            dc->r.BitBltPattBuf(TO_BUF(buf),
                d.left,d.top,d.right-d.left,d.bottom-d.top,
                x + dc->viewPort.left,y + dc->viewPort.top,rop3);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy a section of an offscreen buffer to the specified location with an
optional pattern.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
left        - Left coordinate of section to copy
top         - Top coordinate of section to copy
right       - Right coordinate of section to copy
bottom      - Bottom coordinate of section to copy
dstLeft     - Left coordinate of destination to copy buffer to
dstTop      - Right coordinate of destination to copy buffer to
buf         - Buffer to copy
usePixMap   - True to use color pixmap pattern, false for mono bitmap pattern
rop3        - ROP3 raster operation code to use during Blt (MGL_rop3CodesType)

REMARKS:
Copies a section of an offscreen buffer to the specified location on the
device context, while applying either a mono bitmap pattern or a color pixmap
pattern to the data with a ternary raster operation code (ROP3). If the
usePixMap parameter is set to true, the current pixmap pattern set by
MGL_setPenPixmapPattern will be applied as pattern data, otherwise the current
monochrome bitmap pattern set by MGL_setPenBitmapPattern will be applied.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_putBufferPattSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf,
    int usePixMap,
    int rop3)
{
    rect_t  d,r,clip;
    int     srcLeft,srcTop,srcRight,srcBottom;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip the source rectangle to the buffer dimensions */
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,left,top,right,bottom,&d)) {
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
                    dc->r.BitBltColorPattBuf(TO_BUF(buf),
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3);
                    }
                else {
                    dc->r.BitBltPattBuf(TO_BUF(buf),
                        srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                        r.left,r.top,rop3);
                    }
                }
        END_CLIP_REGION();
        }
    else {
        if (usePixMap) {
            dc->r.BitBltColorPattBuf(TO_BUF(buf),
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,rop3);
            }
        else {
            dc->r.BitBltPattBuf(TO_BUF(buf),
                left,top,right-left,bottom-top,
                dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,rop3);
            }
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Determines if a set of effects for MGL_putBufferFx are supported in hardware

HEADER:
mgraph.h

PARAMETERS:
dst     - Destination device context
src     - Source buffer
fx      - Information describing the effects to apply to the blit (bltfx_t)

RETURNS:
True if effect will run in hardware, false if in software

REMARKS:
This function is used to allow the calling application to determine if
the effects supplied in the fx field (bltfx_t structure) can be supported
in hardware, or if they will be implemented in software. This function
will test the set of flags as applied to the specific source buffer and
destination device context, and return true if the effect will be executed
in hardware and false if it will be executed in software.

SEE ALSO:
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBufferFx,
MGL_stretchBufferFxSection
****************************************************************************/
ibool MGLAPI MGL_bitBltFxTestBuf(
    MGLDC *dst,
    MGLBUF *src,
    bltfx_t *fx)
{
    GA_bltFx    gaFx;

    /* Fail immediately if we do not have hardware effects blits */
    CHECK(dst == src->dc);
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
    return dst->r.BitBltFxTest(&gaFx);
}

/****************************************************************************
DESCRIPTION:
Copy an offscreen buffer to the specified location, while applying different
effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to copy buffer to
x       - x coordinate to copy buffer to
y       - y coordinate to copy buffer to
buf     - Buffer to copy
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Copies an offscreen buffer to the specified location on the device context,
with optional effects applied. The effects applied to the blit operation range
from X and Y bitmap flipping to color transparency and blending.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection,
MGL_bitBltFxTestBuf
****************************************************************************/
void MGLAPI MGL_putBufferFx(
    MGLDC *dc,
    int x,
    int y,
    MGLBUF *buf,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         left,top,right,bottom,srcLeft,srcTop,srcRight,srcBottom;
    GA_bltFx    gaFx;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = x;                 d.top = y;
    d.right = x + buf->width;   d.bottom = y + buf->height;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
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

    /* Adjust to buffer coordinates */
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
                dc->r.BitBltFxBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxBuf(TO_BUF(buf),
            d.left,d.top,d.right-d.left,d.bottom-d.top,
            x + dc->viewPort.left,y + dc->viewPort.top,
            d.right-d.left,d.bottom-d.top,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Copy a section of an offscreen buffer to the specified location, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to copy buffer to
left    - Left coordinate of section to copy
top     - Top coordinate of section to copy
right   - Right coordinate of section to copy
bottom  - Bottom coordinate of section to copy
dstLeft - Left coordinate of destination to copy buffer to
dstTop  - Right coordinate of destination to copy buffer to
buf     - Buffer to copy
fx      - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Copies a section of an offscreen buffer to the specified location on the
device context, with optional effects applied. The effects applied to the
blit operation range from X and Y bitmap flipping to color transparency
and blending.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection,
MGL_bitBltFxTestBuf
****************************************************************************/
void MGLAPI MGL_putBufferFxSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    MGLBUF *buf,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         srcLeft,srcTop,srcRight,srcBottom;
    GA_bltFx    gaFx;

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip the source rectangle to the buffer dimensions */
    if (!MGL_sectRectCoord(0,0,buf->width,buf->height,left,top,right,bottom,&d)) {
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
                dc->r.BitBltFxBuf(TO_BUF(buf),
                    srcLeft,srcTop,srcRight-srcLeft,srcBottom-srcTop,
                    r.left,r.top,srcRight-srcLeft,srcBottom-srcTop,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            right-left,bottom-top,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches an offscreen buffer to the specified rectangle.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy bufer to
dstLeft     - Left coordinate to stretch buffer to
dstTop      - Top coordinate to stretch buffer to
dstRight    - Right coordinate to stretch buffer to
dstBottom   - Bottom coordinate to stretch buffer to
buf         - Buffer to display
op          - Write mode to use when drawing buffer

REMARKS:
Stretches an offscreen buffer to the destination rectangle on the specififed
device context.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_stretchBuffer(
    MGLDC *dc,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    MGLBUF *buf,
    int op)
{
    rect_t  d,r,clip;
    int     left = 0,top = 0,right = buf->width,bottom = buf->height;
    ibool   clipIt = false;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBuffer(dc,dstLeft,dstTop,buf,op);
        return;
        }

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstRight;
    d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
        }
    if ((d.left != dstLeft) || (d.right  != dstRight) || (d.top != dstTop) || (d.bottom != dstBottom))
        clipIt = true;

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = dstRight + dc->viewPort.left;
        d.bottom = dstBottom + dc->viewPort.top;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                dc->r.StretchBltBuf(TO_BUF(buf),
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,!MGL_equalRect(d,r),
                    r.left,r.top,r.right,r.bottom,op);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.StretchBltBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,clipIt,
            dc->clipRectScreen.left,dc->clipRectScreen.top,
            dc->clipRectScreen.right,dc->clipRectScreen.bottom,op);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a section of an offscreen buffer to the specified device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
left        - Left coordinate of section to stretch
top         - Top coordinate of section to stretch
right       - Right coordinate of section to stretch
bottom      - Bottom coordinate of section to stretch
dstLeft     - Left coordinate to stretch buffer to
dstTop      - Top coordinate to stretch buffer to
dstRight    - Right coordinate to stretch buffer to
dstBottom   - Bottom coordinate to stretch buffer to
buf         - Buffer to display
op          - Write mode to use when drawing buffer

REMARKS:
Stretches a section of an offscreen buffer to the destination rectangle on
the specified device context.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection
****************************************************************************/
void MGLAPI MGL_stretchBufferSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    MGLBUF *buf,
    int op)
{
    rect_t  d,r,clip;
    int     deltaSrc,deltaDst;
    fix32_t zoomx,zoomy;
    ibool   clipIt = false;
    int     fdTop,fdLeft;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBufferSection(dc,left,top,right,bottom,dstLeft,dstTop,buf,op);
        return;
        }

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

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

    /* Clip the source rectangle to the bitmap dimensions */
    d.left = left;  d.top = top;
    d.right = MIN(right, buf->width);
    d.bottom = MIN(bottom, buf->height);
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
    if ((d.left != dstLeft) || (d.right  != dstRight) || (d.top != dstTop) || (d.bottom != dstBottom))
        clipIt = true;

    /* Perform the blit operation */
    MAKE_HARDWARE_CURRENT(dc,true);
    if (dc->clipRegionScreen) {
        d.left = dstLeft + dc->viewPort.left;
        d.top = dstTop + dc->viewPort.top;
        d.right = dstRight + dc->viewPort.left;
        d.bottom = dstBottom + dc->viewPort.top;
        BEGIN_CLIP_REGION(clip,dc->clipRegionScreen);
            if (MGL_sectRect(clip,d,&r)) {
                dc->r.StretchBltBuf(TO_BUF(buf),
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,!MGL_equalRect(d,r),
                    r.left,r.top,r.right,r.bottom,op);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.StretchBltBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,clipIt,
            dc->clipRectScreen.left,dc->clipRectScreen.top,
            dc->clipRectScreen.right,dc->clipRectScreen.bottom,
            op);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches an offscreen buffer to the specified rectangle, while
applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy bufer to
dstLeft     - Left coordinate to stretch buffer to
dstTop      - Top coordinate to stretch buffer to
dstRight    - Right coordinate to stretch buffer to
dstBottom   - Bottom coordinate to stretch buffer to
buf         - Buffer to display
fx          - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Stretches an offscreen buffer to the destination rectangle on the specififed
device context, while applying optional effects. You can use the flags member
of the bltfx_t structure to define whether stretching is done using nearest
color stretching or if pixel interpolation will be used. If you don't specify
any flags, nearest color stretching will be used.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection,
MGL_bitBltFxTestBuf
****************************************************************************/
void MGLAPI MGL_stretchBufferFx(
    MGLDC *dc,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    MGLBUF *buf,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         left = 0,top = 0,right = buf->width,bottom = buf->height;
    GA_bltFx    gaFx;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBufferFx(dc,dstLeft,dstTop,buf,fx);
        return;
        }

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

    /* Clip to destination device context */
    d.left = dstLeft;
    d.top = dstTop;
    d.right = dstRight;
    d.bottom = dstBottom;
    if (!MGL_sectRect(dc->clipRectView,d,&d)) {
        END_VISIBLE_CLIP_LIST(dc);
        return;
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
                dc->r.BitBltFxBuf(TO_BUF(buf),
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

/****************************************************************************
DESCRIPTION:
Stretches a section of an offscreen buffer to the specified device context,
while applying different effects in the process.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to copy buffer to
left        - Left coordinate of section to stretch
top         - Top coordinate of section to stretch
right       - Right coordinate of section to stretch
bottom      - Bottom coordinate of section to stretch
dstLeft     - Left coordinate to stretch buffer to
dstTop      - Top coordinate to stretch buffer to
dstRight    - Right coordinate to stretch buffer to
dstBottom   - Bottom coordinate to stretch buffer to
buf         - Buffer to display
fx          - Information describing the effects to apply to the blit (bltfx_t)

REMARKS:
Stretches a section of an offscreen buffer to the destination rectangle on
the specified device context, while applying optional effects. You can use the
flags member of the bltfx_t structure to define whether stretching is done using
nearest color stretching or if pixel interpolation will be used. If you don't
specify any flags, nearest color stretching will be used.

Note:   This function will fail if you attempt to copy a buffer to a
        device context that is not the device context that the original
        buffer was allocated for.

SEE ALSO:
MGL_copyToBuffer, MGL_copyBitmapToBuffer, MGL_updateBufferCache,
MGL_updateFromBufferCache, MGL_putBuffer, MGL_putBufferSection,
MGL_putBufferSrcTrans, MGL_putBufferSrcTransSection, MGL_putBufferDstTrans,
MGL_putBufferDstTransSection, MGL_putBufferPatt, MGL_putBufferPattSection,
MGL_putBufferFx, MGL_putBufferFxSection, MGL_stretchBuffer,
MGL_stretchBufferSection, MGL_stretchBufferFx, MGL_stretchBufferFxSection,
MGL_bitBltFxTestBuf
****************************************************************************/
void MGLAPI MGL_stretchBufferFxSection(
    MGLDC *dc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop,
    int dstRight,
    int dstBottom,
    MGLBUF *buf,
    bltfx_t *fx)
{
    rect_t      d,r,clip;
    int         deltaSrc,deltaDst;
    fix32_t     zoomx,zoomy;
    int         fdTop,fdLeft;
    GA_bltFx    gaFx;

    /* Handle special case of 1:1 stretch */
    if (((right - left) == (dstRight - dstLeft)) &&
        ((bottom - top) == (dstBottom - dstTop))) {
        MGL_putBufferFxSection(dc,left,top,right,bottom,dstLeft,dstTop,buf,fx);
        return;
        }

    /* Check that the device context pointer is valid */
    CHECK(dc == buf->dc);
    if (dc == _MGL_dcPtr)
        dc = &DC;
    BEGIN_VISIBLE_CLIP_LIST(dc);

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

    /* Clip the source rectangle to the bitmap dimensions */
    d.left = left;  d.top = top;
    d.right = MIN(right, buf->width);
    d.bottom = MIN(bottom, buf->height);
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
                dc->r.BitBltFxBuf(TO_BUF(buf),
                    left,top,right-left,bottom-top,
                    d.left,d.top,d.right-d.left,d.bottom-d.top,&gaFx);
                }
        END_CLIP_REGION();
        }
    else {
        dc->r.BitBltFxBuf(TO_BUF(buf),
            left,top,right-left,bottom-top,
            dstLeft + dc->viewPort.left,dstTop + dc->viewPort.top,
            dstRight-dstLeft,dstBottom-dstTop,&gaFx);
        }
    RESTORE_HARDWARE(dc,true);
    END_VISIBLE_CLIP_LIST(dc);
}

// TODO: Should add support for drawing rectangles directly to a buffer for
//       clearing parts of the buffer etc as we can support that in SNAP

