/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture.
*
****************************************************************************/

#include "snap/gasdk.h"
#ifndef __WIN32_VXD__
#include <stdarg.h>
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

static ibool        forceSoftwareCount = 0;
ibool               softwareOnly = false;
N_uint16            cntMode;
N_int32             x,y,maxX,maxY,maxPage,activePage,visualPage,maxMem;
N_int32             virtualX,virtualY,bytesPerLine,bytesPerPixel;
N_int32             isStereo,useBlueCode,blueCodeIndex;
GA_color            defcolor,maxcolor,foreColor,backColor;
GA_modeInfo         modeInfo;
REF2D_driver        *ref2d;
ibool               unloadRef2d;
GA_devCtx           *dc;
GA_2DStateFuncs     drv_state2d,state2d;
GA_2DRenderFuncs    drv_draw2d,draw2d;
GA_bufferFuncs      bufmgr;
GA_buf              *primaryBuf;
GA_buf              *activeBuf;
GA_buf              *shadowBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColorEx(
    uchar r,
    uchar g,
    uchar b,
    GA_pixelFormat *pf)
{
    return ((ulong)((r >> pf->RedAdjust)   & pf->RedMask)   << pf->RedPosition)
         | ((ulong)((g >> pf->GreenAdjust) & pf->GreenMask) << pf->GreenPosition)
         | ((ulong)((b >> pf->BlueAdjust)  & pf->BlueMask)  << pf->BluePosition);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColor(
    uchar r,
    uchar g,
    uchar b)
{
    return rgbColorEx(r,g,b,&activeBuf->PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorEx(
    int index,
    int bitsPerPixel,
    uint colorMask,
    GA_pixelFormat *pf)
{
    if (bitsPerPixel <= 8)
        return index & colorMask;
    return rgbColorEx(VGA8_defPal[index].Red,
                      VGA8_defPal[index].Green,
                      VGA8_defPal[index].Blue,
                      pf);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColor(
    int index)
{
    return realColorEx(index,modeInfo.BitsPerPixel,maxcolor,&activeBuf->PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.

NOTE: This version padded all the reserved values with 1's instead of
      zeroes. This is only intended for conformance testing the new WHQL
      style source transparent blit tests.
****************************************************************************/
GA_color rgbColorPaddedEx(
    uchar r,
    uchar g,
    uchar b,
    GA_pixelFormat *pf)
{
    if (dc->WorkArounds & gaNoWHQLTransparentBlit)
        return rgbColorEx(r,g,b,pf);
    return ((ulong)((r >> pf->RedAdjust)   & pf->RedMask)   << pf->RedPosition)
         | ((ulong)((g >> pf->GreenAdjust) & pf->GreenMask) << pf->GreenPosition)
         | ((ulong)((b >> pf->BlueAdjust)  & pf->BlueMask)  << pf->BluePosition)
         | ((ulong)((0xFF >> pf->AlphaAdjust)  & pf->AlphaMask)  << pf->AlphaPosition);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColorPadded(
    uchar r,
    uchar g,
    uchar b)
{
    return rgbColorPaddedEx(r,g,b,&activeBuf->PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorPaddedEx(
    int index,
    int bitsPerPixel,
    uint colorMask,
    GA_pixelFormat *pf)
{
    if (bitsPerPixel <= 8)
        return index & colorMask;
    return rgbColorPaddedEx(VGA8_defPal[index].Red,
                            VGA8_defPal[index].Green,
                            VGA8_defPal[index].Blue,
                            pf);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorPadded(
    int index)
{
    return realColorPaddedEx(index,modeInfo.BitsPerPixel,maxcolor,&activeBuf->PixelFormat);
}

/****************************************************************************
REMARKS:
Set the current foreground color for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetForeColor(
    GA_color color)
{
    state2d.SetForeColor(color);
    foreColor = color;
}

/****************************************************************************
REMARKS:
Set the current background color for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetBackColor(
    GA_color color)
{
    state2d.SetBackColor(color);
    backColor = color;
}

/****************************************************************************
REMARKS:
Set the current mix for both the hardware driver and the reference rasteriser.
****************************************************************************/
void SetMix(
    N_int32 mix)
{
    state2d.SetMix(mix);
}

/****************************************************************************
REMARKS:
Set the 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Set8x8MonoPattern(
    N_int32 index,
    GA_pattern *pattern)
{
    state2d.Set8x8MonoPattern(index,pattern);
}

/****************************************************************************
REMARKS:
Select an 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8MonoPattern(
    N_int32 index)
{
    state2d.Use8x8MonoPattern(index);
}

/****************************************************************************
REMARKS:
Select an 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8TransMonoPattern(
    N_int32 index)
{
    state2d.Use8x8TransMonoPattern(index);
}

/****************************************************************************
REMARKS:
Set the 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Set8x8ColorPattern(
    N_int32 index,
    GA_colorPattern *pattern)
{
    state2d.Set8x8ColorPattern(index,pattern);
}

/****************************************************************************
REMARKS:
Select an 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8ColorPattern(
    N_int32 index)
{
    state2d.Use8x8ColorPattern(index);
}

/****************************************************************************
REMARKS:
Select an 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8TransColorPattern(
    N_int32 index,
    GA_color transparent)
{
    state2d.Use8x8TransColorPattern(index,transparent);
}

/****************************************************************************
REMARKS:
Set the line stipple pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetLineStipple(
    GA_stipple stipple)
{
    state2d.SetLineStipple(stipple);
}

/****************************************************************************
REMARKS:
Set the line stipple count for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetLineStippleCount(
    N_uint32 count)
{
    state2d.SetLineStippleCount(count);
}

/****************************************************************************
REMARKS:
Set the plane mask for both the hardware driver and the reference rasteriser.
****************************************************************************/
void SetPlaneMask(N_uint32 planeMask)
{
    state2d.SetPlaneMask(planeMask);
}

/****************************************************************************
REMARKS:
Enables the use of a system memory shadow buffer for testing purposes. The
shadow buffer will always be copied to the primary buffer.
****************************************************************************/
void EnableShadowBuffer(
    int bitsPerPixel,
    GA_pixelFormat *pf,
    int flags)
{
    maxPage = 0;
    maxcolor = 1 << (bitsPerPixel-1);
    modeInfo.BitsPerPixel = 1;
    modeInfo.Attributes &= ~gaHaveAccel2D;
    if ((shadowBuf = bufmgr.AllocBufferExt(modeInfo.XResolution,modeInfo.YResolution,
            bitsPerPixel,pf,flags | gaBufferSysMem)) == NULL)
        PM_fatalError("Attempt to allocate shadow buffer failed!\n");
    bufmgr.SetActiveBuffer(activeBuf = shadowBuf);
}

/****************************************************************************
REMARKS:
Copies the shadow buffer if enabled to the primary buffer.
****************************************************************************/
void CopyShadowBuffer(void)
{
    GA_bltFx    fx;

    if (shadowBuf) {
        bufmgr.SetActiveBuffer(primaryBuf);
        fx.dwSize = sizeof(fx);
        fx.Flags = gaBltConvert;
        if (shadowBuf->Flags & gaBufferMonoLSB)
            fx.Flags |= gaBltMonoLSB;
        fx.BitsPerPixel = 1;
        fx.ForeColor = 0xFFFFFFFF;
        fx.BackColor = 0;
        bufmgr.BitBltFxBuf(shadowBuf,
            0,0,modeInfo.XResolution,modeInfo.YResolution,
            0,0,modeInfo.XResolution,modeInfo.YResolution,&fx);
        bufmgr.SetActiveBuffer(shadowBuf);
        }
}

/****************************************************************************
REMARKS:
Set the currently active rendering buffer to the passed in buffer
****************************************************************************/
void SetActiveBuffer(
    GA_buf *buf)
{
    /* Now switch the active rendering buffer */
    bufmgr.SetActiveBuffer(activeBuf = buf);

    /* Set resolution values */
    maxX = buf->Width-1;
    maxY = buf->Height-1;
    bytesPerLine = buf->Stride;

    /* Now initialise color depth specific values */
    switch (buf->BitsPerPixel) {
        case 1:
            bytesPerPixel = 1;
            defcolor = maxcolor = 0x1;
            break;
        case 4:
            bytesPerPixel = 1;
            defcolor = maxcolor = 0xF;
            break;
        case 8:
            bytesPerPixel = 1;
            defcolor = (buf->Flags & gaBufferVideo) ? 0xFF : 0xF;
            maxcolor = 0xFF;
            break;
        case 15:
        case 16:
            bytesPerPixel = 2;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            break;
        case 24:
            bytesPerPixel = 3;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            break;
        case 32:
            bytesPerPixel = 4;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            break;
        }
}

/****************************************************************************
REMARKS:
Set the currently active drawing page for both the hardware driver and
the reference rasteriser. We use this to implement hardware page flipping.
****************************************************************************/
void SetActivePage(
    int page)
{
    GA_buf  *buf;

    activePage = page;
    if (isStereo != gaNoStereo) {
        if (page & gaRightBuffer)
            page = ((page & ~gaRightBuffer) * 2) + 1;
        else
            page *= 2;
        }
    if ((buf = bufmgr.GetFlippableBuffer(page)) == NULL)
        PM_fatalError("Attempt to draw to invalid page!\n");
    SetActiveBuffer(buf);
}

/****************************************************************************
REMARKS:
Set the currently visible page for the hardware driver. We use this to
implement hardware page flipping.
****************************************************************************/
void SetVisualPage(
    int page,
    int waitVRT)
{
    int     oldActivePage = activePage;
    ulong   blueCodeColor;
    GA_buf  *leftBuf,*rightBuf;

    visualPage = page;
    if (isStereo != gaNoStereo && useBlueCode) {
        /* Get the color for the blue code */
        if (maxcolor > 255)
            blueCodeColor = rgbColor(0,0,255);
        else
            blueCodeColor = blueCodeIndex;

        /* Draw the left image blue code (25% of width) */
        SetActivePage(page | gaLeftBuffer);
        SetForeColor(blueCodeColor);
        draw2d.DrawLineInt(0,maxY,maxX / 4,maxY,true);
        SetForeColor(0);
        draw2d.DrawLineInt(maxX / 4,maxY,maxX,maxY,true);

        /* Draw the right image blue code (75% of width) */
        SetActivePage(page | gaRightBuffer);
        SetForeColor(blueCodeColor);
        draw2d.DrawLineInt(0,maxY,(maxX * 3) / 4,maxY,true);
        SetForeColor(0);
        draw2d.DrawLineInt((maxX * 3) / 4,maxY,maxX,maxY,true);
        SetActivePage(oldActivePage);
        }

    /* Flip to the visible buffer */
    if (isStereo != gaNoStereo) {
        leftBuf = bufmgr.GetFlippableBuffer(visualPage*2);
        rightBuf = bufmgr.GetFlippableBuffer(visualPage*2+1);
        if (!leftBuf || !rightBuf)
            PM_fatalError("Attempt to flip to invalid page!\n");
        bufmgr.FlipToStereoBuffer(leftBuf,rightBuf,waitVRT);
        }
    else {
        leftBuf = bufmgr.GetFlippableBuffer(visualPage);
        if (!leftBuf)
            PM_fatalError("Attempt to flip to invalid page!\n");
        bufmgr.FlipToBuffer(leftBuf,waitVRT);
        }
}

/****************************************************************************
REMARKS:
Set the currently visible page for the hardware driver. We use this to
implement hardware page flipping.
****************************************************************************/
void SetVisualPageXY(
    int page,
    int x,
    int y,
    int waitVRT)
{
    GA_buf  *buf;

    visualPage = page;
    buf = bufmgr.GetFlippableBuffer(visualPage);
    if (!buf)
        PM_fatalError("Attempt to flip to invalid page!\n");
    bufmgr.FlipToBufferXY(buf,x,y,waitVRT);
}

/****************************************************************************
REMARKS:
Start stereo page flipping mode.
****************************************************************************/
ibool StartStereo(void)
{
    if (isStereo == gaNoStereo) {
        /* We always use the blue codes in the SciTech SNAP Graphics SDK for simplicity */
        useBlueCode = true;
        if (useBlueCode && modeInfo.BitsPerPixel == 8) {
            /* Enable support for blue code software stereo sync signal */
            VGA8_defPal[blueCodeIndex].Red = 0;
            VGA8_defPal[blueCodeIndex].Green = 0;
            VGA8_defPal[blueCodeIndex].Blue = 0xFF;
            driver.SetPaletteData(VGA8_defPal,256,0,false);
            }
        isStereo = gaStereoOn;
        SetVisualPage(visualPage,gaDontWait);
        }
    return true;
}

/****************************************************************************
REMARKS:
Stops stereo page flipping mode.
****************************************************************************/
void StopStereo(void)
{
    if (isStereo != gaNoStereo) {
        isStereo = gaStereoOff;
        SetVisualPage(visualPage,gaDontWait);
        }
}

/****************************************************************************
REMARKS:
Clears the current display page.
****************************************************************************/
void ClearPage(
    GA_color color)
{
    SetForeColor(color);
    draw2d.DrawRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Clips a mono bitmap to the passed in clip rectangle and draws on the screen.
****************************************************************************/
void ClipMonoBltSys(
    int x,
    int y,
    int width,
    int height,
    int byteWidth,
    uchar *image,
    GA_color foreColor,
    GA_color backColor,
    int mix,
    int flags,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    int     bottom,right;

    // Check if image needs to be clipped at all
    right = x + width;
    bottom = y + height;
    if ((bottom <= clipTop) || (y >= clipBottom) ||
        (right <= clipLeft) || (x >= clipRight))
        return;

    // Clip the top and bottom of the bitmap image from being drawn
    if (y < clipTop) {
        int skip = (clipTop - y);
        y = clipTop;
        image += skip * byteWidth;
        height -= skip;
        }
    if (bottom > clipBottom)
        height -= (bottom - clipBottom);

    // Now clip the left and right edges so that the clip rectangle lies
    // within the first and last bytes of the image.
    if (x < clipLeft) {
        int skip = (clipLeft - x) / 8;
        image += skip;
        x += skip * 8;
        width -= skip * 8;
        }
    if (right > clipRight)
        width -= (right - clipRight);

    // Now draw the bitmap, either clipped or unclipped
    draw2d.MonoBltSys(x,y,width,height,byteWidth,image,
        (x < clipLeft) ? (clipLeft - x) : 0,
        foreColor,backColor,mix,flags);
}

/****************************************************************************
REMARKS:
Writes the string to the display
****************************************************************************/
void WriteText(
    int x,
    int y,
    char *s,
    GA_color color)
{
    static  uchar *monobuf = NULL;
    static  int bufsize = 0;
    uchar   *font,*image,*buf,*p;
    int     i,length,width,byteWidth,charsize = 16;

    /* Allocate a block of memory for building the mono string to blit */
    length = strlen(s);
    if (bufsize < length * charsize) {
        free(monobuf);
        bufsize = length * charsize;
        if ((monobuf = malloc(bufsize)) == NULL)
            PM_fatalError("Out of memory!");
        }

    /* Now build up the single mono blit string in the buffer */
    SetForeColor(color);
    font = dc->TextFont8x16;
    buf = monobuf;
    byteWidth = length;
    width = length * 8;
    while (length--) {
        p = buf++;
        image = (font + (*s++) * 16);
        for (i = 0; i < 16; i++) {
            *p = *image++;
            p += byteWidth;
            }
        }

    /* Finally draw the entire string as a single mono blit, clipped
     * if necessary
     */
    if (draw2d.MonoBltSys) {
        /* Newer MonoBltSys function for newer binary drivers */
        if (x <= 0 || y <= 0 || x+width >= modeInfo.XResolution || y+charsize >= modeInfo.YResolution) {
            ClipMonoBltSys(x,y,width,charsize,byteWidth,monobuf,color,0,GA_REPLACE_MIX,gaBltMonoTransparent,
                0,0,modeInfo.XResolution,modeInfo.YResolution);
            }
        else
            draw2d.MonoBltSys(x,y,width,charsize,byteWidth,monobuf,0,color,0,GA_REPLACE_MIX,gaBltMonoTransparent);
        }
    else {
        /* Backward compatibility for older binary drivers */
        if (x <= 0 || y <= 0 || x+width >= modeInfo.XResolution || y+charsize >= modeInfo.YResolution) {
            draw2d.ClipMonoImageMSBSys_Old(x,y,width,charsize,byteWidth,monobuf,true,
                0,0,modeInfo.XResolution,modeInfo.YResolution);
            }
        else
            draw2d.PutMonoImageMSBSys_Old(x,y,width,charsize,byteWidth,monobuf,true);
        }
}

/****************************************************************************
REMARKS:
Writes the string to the display. This is a slower version that writes
each character bitmap individually. We mostly use this for conformance
testing to ensure that single characters draw correctly with the hardware.
****************************************************************************/
void WriteTextSlow(
    int x,
    int y,
    char *s,
    GA_color color)
{
    uchar   *font,*image;
    int     length;

    SetForeColor(color);
    font = dc->TextFont8x16;
    length = strlen(s);
    while (length--) {
        image = (font + (*s++) * 16);
        if (draw2d.MonoBltSys) {
            /* Newer MonoBltSys function for newer binary drivers */
            if (x <= 0 || y <= 0 || x+8 >= modeInfo.XResolution || y+16 >= modeInfo.YResolution) {
                ClipMonoBltSys(x,y,8,16,1,image,color,0,GA_REPLACE_MIX,gaBltMonoTransparent,
                    0,0,modeInfo.XResolution,modeInfo.YResolution);
                }
            else
                draw2d.MonoBltSys(x,y,8,16,1,image,0,color,0,GA_REPLACE_MIX,gaBltMonoTransparent);
            }
        else {
            /* Backward compatibility for older binary drivers */
            if (x <= 0 || y <= 0 || x+8 >= modeInfo.XResolution || y+16 >= modeInfo.YResolution) {
                draw2d.ClipMonoImageMSBSys_Old(x,y,8,16,1,image,true,
                    0,0,modeInfo.XResolution,modeInfo.YResolution);
                }
            else
                draw2d.PutMonoImageMSBSys_Old(x,y,8,16,1,image,true);
            }
        x += 8;
        }
}

/****************************************************************************
PARAMETERS:
x,y - Location to move text cursor to

REMARKS:
Moves the current text location to the specified position.
****************************************************************************/
void gmoveto(
    int _x,
    int _y)
{
    x = _x;
    y = _y;
}

/****************************************************************************
REMARKS:
Moves the current text location to the next line down
****************************************************************************/
void gnewline(void)
{ y += 16; }

/****************************************************************************
REMARKS:
Returns the current x position
****************************************************************************/
int ggetx(void)
{ return x; }

/****************************************************************************
REMARKS:
Returns the current y position
****************************************************************************/
int ggety(void)
{ return y; }

/****************************************************************************
PARAMETERS:
fmt     - Format string
...     - Standard printf style parameters

REMARKS:
Simple printf style output routine for sending text to the current graphics
modes. It begins drawing the string at the current location, and moves to
the start of the next logical line.
****************************************************************************/
int gprintf(
    char *fmt,
    ...)
{
    va_list argptr;
    char    buf[255];
    int     cnt;

    va_start(argptr,fmt);
    cnt = vsprintf(buf,fmt,argptr);
    WriteText(x,y,buf,defcolor);
    y += 16;
    va_end(argptr);
    (void)argptr;
    return cnt;
}

/****************************************************************************
REMARKS:
This function forces the SciTech SNAP Graphics reference rasteriser functions to be
used for all drawing functions, overiding any hardware functions.
****************************************************************************/
void ForceSoftwareOnly(void)
{
    ref2d->ForceSoftwareOnly(true);
    forceSoftwareCount++;
}

/****************************************************************************
REMARKS:
Function to revert the software only rendering state back to the previous
setting.
****************************************************************************/
void RevertSoftwareOnly(void)
{
    if (drv_state2d.WaitTillIdle)
        drv_state2d.WaitTillIdle();
    if (forceSoftwareCount)
        forceSoftwareCount--;
    if (forceSoftwareCount)
        ref2d->ForceSoftwareOnly(true);
    else
        ref2d->ForceSoftwareOnly(softwareOnly);
}

/****************************************************************************
REMARKS:
Main function to initialise the software rasteriser module and the internal
library helper functions.
****************************************************************************/
ibool InitSoftwareRasterizer(
    int deviceIndex,
    int reservePages,
    ibool stereo)
{
    GA_initFuncs        init;
    GA_softStereoFuncs  stereoFuncs = {
        sizeof(stereoFuncs),
        GA_softStereoInit,
        GA_softStereoOn,
        GA_softStereoScheduleFlip,
        GA_softStereoGetFlipStatus,
        GA_softStereoWaitTillFlipped,
        GA_softStereoOff,
        GA_softStereoExit,
        };

    /* Initialise the dimensions for the screen and setup drawing buffer */
    maxPage = modeInfo.MaxBuffers-1;
    activePage = 0;
    isStereo = gaNoStereo;
    useBlueCode = true;
    blueCodeIndex = 255;

    /* Load the SciTech SNAP Graphics reference rasteriser for the color depth */
    memset(&state2d,0,sizeof(state2d));
    memset(&draw2d,0,sizeof(draw2d));
    memset(&bufmgr,0,sizeof(bufmgr));
    unloadRef2d = false;
    if ((ref2d = GA_getCurrentRef2d(dc->DeviceIndex)) == NULL) {
        modeInfo.dwSize = sizeof(modeInfo);
        init.dwSize = sizeof(init);
        GA_queryFunctions(dc,GA_GET_INITFUNCS,&init);
        init.GetCurrentVideoModeInfo(&modeInfo);
        unloadRef2d = true;
        if (!GA_loadRef2d(dc,false,&modeInfo,-1,&ref2d))
            return false;
        }
    drv_state2d.dwSize = sizeof(drv_state2d);
    GA_queryFunctions(dc,GA_GET_2DSTATEFUNCS,&drv_state2d);
    drv_draw2d.dwSize = sizeof(drv_draw2d);
    GA_queryFunctions(dc,GA_GET_2DRENDERFUNCS,&drv_draw2d);
    driver.dwSize = sizeof(driver);
    REF2D_queryFunctions(ref2d,GA_GET_DRIVERFUNCS,&driver);
    state2d.dwSize = sizeof(state2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DSTATEFUNCS,&state2d);
    draw2d.dwSize = sizeof(draw2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DRENDERFUNCS,&draw2d);

    /* Initialise the buffer manager and find the primary buffer */
    bufmgr.dwSize = sizeof(bufmgr);
    if (!REF2D_queryFunctions(ref2d,GA_GET_BUFFERFUNCS,&bufmgr))
        return false;
    if (!bufmgr.InitBuffers(reservePages,0,stereo ? &stereoFuncs : NULL))
        return false;
    activeBuf = primaryBuf = bufmgr.GetPrimaryBuffer();

    /* Setup for hardware rendering to the framebuffer */
    forceSoftwareCount = 0;
    RevertSoftwareOnly();

    /* Set the default mixes and state... */
    SetActivePage(0);
    visualPage = 0;
    SetMix(GA_REPLACE_MIX);
    SetForeColor(defcolor);
    SetBackColor(0);

    /* Program the default color palette */
    if (modeInfo.BitsPerPixel == 8)
        driver.SetPaletteData(VGA8_defPal,256,0,false);
    else if (modeInfo.BitsPerPixel == 4)
        driver.SetPaletteData(VGA4_defPal,16,0,false);
    (void)deviceIndex;
    return true;
}

/****************************************************************************
REMARKS:
Function to exit the software rasteriser module and clean up
****************************************************************************/
void ExitSoftwareRasterizer(void)
{
    /* Release shadow buffer, if any */
    if (shadowBuf) {
        if (bufmgr.FreeBuffer)
            bufmgr.FreeBuffer(shadowBuf);
        shadowBuf = NULL;
        }
    if (unloadRef2d)
        GA_unloadRef2d(dc);
    memset(&drv_state2d, 0, sizeof(drv_state2d));
    memset(&drv_draw2d, 0, sizeof(drv_draw2d));
}
