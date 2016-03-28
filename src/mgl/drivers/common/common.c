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
* Description:  Device driver helper routines common to all display
*               device drivers.
*
****************************************************************************/

#include "mgldd.h"
#include "drivers/common/common.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
dc  - MGL device context to get HDC for

RETURNS:
Pointer to the default palette for the DC, NULL if none available.
****************************************************************************/
void * MGLAPI DRV_getDefaultPalette(
    MGLDC *dc)
{
    if (dc->mi.bitsPerPixel >= 8)
        return _VGA8_defPal;
    else if (dc->mi.bitsPerPixel == 4)
        return _VGA4_defPal;
    return NULL;
}

/****************************************************************************
REMARKS:
Renders a clipped mono bitmap by clipping of as much of the bitmap in
software, and then calling the MonoBltSys function.
****************************************************************************/
void MGLAPI DRV_ClipMonoBltSys(
    MGLDC *dc,
    N_int32 x,
    N_int32 y,
    N_int32 width,
    N_int32 height,
    N_int32 byteWidth,
    N_uint8 *image,
    GA_color foreColor,
    GA_color backColor,
    N_int32 mix,
    N_int32 flags,
    N_int32 clipLeft,
    N_int32 clipTop,
    N_int32 clipRight,
    N_int32 clipBottom)
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
    dc->r.MonoBltSys(x,y,width,height,byteWidth,image,
        (x < clipLeft) ? (clipLeft - x) : 0,
        foreColor,backColor,mix,flags);
}

/****************************************************************************
PARAMETERS:
dc      - MGL device context to get HDC for
state2d - Pointer to SNAP Graphics 2D state functions
draw2d  - Pointer to SNAP Graphics 2D drawing functions
ref2d   - Pointer to SNAP Graphics 2D reference driver

RETURNS:
Pointer to the default palette for the DC, NULL if none available.
****************************************************************************/
void DRV_setSNAPRenderingVectors(
    MGLDC *dc,
    REF2D_driver *ref2d)
{
    GA_2DStateFuncs     state2d;
    GA_2DRenderFuncs    draw2d;

    /* Get the function group from the reference rasteriser */
    state2d.dwSize = sizeof(state2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DSTATEFUNCS,&state2d);
    draw2d.dwSize = sizeof(draw2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DRENDERFUNCS,&draw2d);

    /* SciTech SNAP Graphics compatible 2D state management vectors */
    dc->r.SetDrawSurface            = ref2d->SetDrawSurface;
    dc->r.SetDrawBuffer             = state2d.SetDrawBuffer;
    dc->r.SetMix                    = state2d.SetMix;
    dc->r.SetForeColor              = state2d.SetForeColor;
    dc->r.SetBackColor              = state2d.SetBackColor;
    dc->r.Set8x8MonoPattern         = state2d.Set8x8MonoPattern;
    dc->r.Use8x8MonoPattern         = state2d.Use8x8MonoPattern;
    dc->r.Use8x8TransMonoPattern    = state2d.Use8x8TransMonoPattern;
    dc->r.Set8x8ColorPattern        = state2d.Set8x8ColorPattern;
    dc->r.Use8x8ColorPattern        = state2d.Use8x8ColorPattern;
    dc->r.Use8x8TransColorPattern   = state2d.Use8x8TransColorPattern;
    dc->r.SetLineStipple            = state2d.SetLineStipple;
    dc->r.SetLineStippleCount       = state2d.SetLineStippleCount;
    dc->r.SetPlaneMask              = state2d.SetPlaneMask;
    dc->r.SetBlendFunc              = state2d.SetBlendFunc;
    dc->r.SetAlphaValue             = state2d.SetAlphaValue;
    dc->r.SetLineStyle              = state2d.SetLineStyle;
    dc->r.BuildTranslateVector      = state2d.BuildTranslateVector;

    /* Older drivers may not have SetBlendFunc, but the older deprecated
     * SetSrcBlendFunc and SetDstBlendFunc functions. We will in this
     * case fall back on the older functions.
     */
    if (!state2d.SetBlendFunc) {
        dc->r.SetSrcBlendFunc_Old  = state2d.SetSrcBlendFunc_Old;
        dc->r.SetDstBlendFunc_Old  = state2d.SetDstBlendFunc_Old;
        }

    /* SciTech SNAP Graphics compatible rendering vectors */
    dc->r.GetPixel                  = draw2d.GetPixel;
    dc->r.PutPixel                  = draw2d.PutPixel;
    dc->r.DrawLineInt               = draw2d.DrawLineInt;
    dc->r.DrawBresenhamLine         = draw2d.DrawBresenhamLine;
    dc->r.DrawStippleLineInt        = draw2d.DrawStippleLineInt;
    dc->r.DrawBresenhamStippleLine  = draw2d.DrawBresenhamStippleLine;
    dc->r.DrawEllipse               = draw2d.DrawEllipse;
    dc->r.ClipEllipse               = draw2d.ClipEllipse;
    dc->r.PutMonoImage_Old          = draw2d.PutMonoImageMSBSys_Old;
    dc->r.ClipMonoImage_Old         = draw2d.ClipMonoImageMSBSys_Old;
    dc->r.MonoBltSys                = draw2d.MonoBltSys;
    dc->r.ClipMonoBltSys            = DRV_ClipMonoBltSys;
    dc->r.BitBlt                    = draw2d.BitBlt;
    dc->r.BitBltLin                 = draw2d.BitBltLin;
    dc->r.BitBltSys                 = draw2d.BitBltSys;
    dc->r.BitBltPatt                = draw2d.BitBltPatt;
    dc->r.BitBltPattLin             = draw2d.BitBltPattLin;
    dc->r.BitBltPattSys             = draw2d.BitBltPattSys;
    dc->r.BitBltColorPatt           = draw2d.BitBltColorPatt;
    dc->r.BitBltColorPattLin        = draw2d.BitBltColorPattLin;
    dc->r.BitBltColorPattSys        = draw2d.BitBltColorPattSys;
    dc->r.SrcTransBlt               = draw2d.SrcTransBlt;
    dc->r.SrcTransBltLin            = draw2d.SrcTransBltLin;
    dc->r.SrcTransBltSys            = draw2d.SrcTransBltSys;
    dc->r.DstTransBlt               = draw2d.DstTransBlt;
    dc->r.DstTransBltLin            = draw2d.DstTransBltLin;
    dc->r.DstTransBltSys            = draw2d.DstTransBltSys;
    dc->r.StretchBlt                = draw2d.StretchBlt;
    dc->r.StretchBltLin             = draw2d.StretchBltLin;
    dc->r.StretchBltSys             = draw2d.StretchBltSys;
    dc->r.BitBltFxTest              = NULL;
    dc->r.BitBltFx                  = draw2d.BitBltFx;
    dc->r.BitBltFxLin               = draw2d.BitBltFxLin;
    dc->r.BitBltFxSys               = draw2d.BitBltFxSys;
    dc->r.GetBitmapSys              = draw2d.GetBitmapSys;
    dc->r.solid.DrawScanList        = draw2d.DrawScanList;
    dc->r.patt.DrawScanList         = draw2d.DrawPattScanList;
    dc->r.colorPatt.DrawScanList    = draw2d.DrawColorPattScanList;
    dc->r.solid.DrawEllipseList     = draw2d.DrawEllipseList;
    dc->r.patt.DrawEllipseList      = draw2d.DrawPattEllipseList;
    dc->r.colorPatt.DrawEllipseList = draw2d.DrawColorPattEllipseList;
    dc->r.solid.DrawFatEllipseList  = draw2d.DrawFatEllipseList;
    dc->r.patt.DrawFatEllipseList   = draw2d.DrawPattFatEllipseList;
    dc->r.colorPatt.DrawFatEllipseList = draw2d.DrawColorPattFatEllipseList;
    dc->r.solid.DrawRect            = draw2d.DrawRect;
    dc->r.patt.DrawRect             = draw2d.DrawPattRect;
    dc->r.colorPatt.DrawRect        = draw2d.DrawColorPattRect;
    dc->r.solid.DrawTrap            = draw2d.DrawTrap;
    dc->r.patt.DrawTrap             = draw2d.DrawPattTrap;
    dc->r.colorPatt.DrawTrap        = draw2d.DrawColorPattTrap;
}

/****************************************************************************
REMARKS:
Make the new device context the currently active device context for
rendering, which includes updating any necessary hardware state that
is cached in the SciTech SNAP Graphics drivers.
****************************************************************************/
void MGLAPI DRV_makeCurrent(
    MGLDC *dc,
    ibool partial)
{
    int i;

    /* Now update those attributes that are cached in SciTech SNAP Graphics */
    if (!partial) {
        dc->r.SetMix(dc->a.writeMode);
        dc->r.SetForeColor(dc->a.color);
        dc->r.SetBackColor(dc->a.backColor);
        for (i = 0; i < 8; i++) {
            dc->r.Set8x8MonoPattern(i,GAMONOPAT(&dc->a.penPat[i]));
            dc->r.Set8x8ColorPattern(i,GACLRPAT(&dc->a.penPixPat[i]));
            }
        if (dc->a.penStyle == MGL_BITMAP_OPAQUE)
            dc->r.Use8x8MonoPattern(dc->a.cntPenPat);
        else if (dc->a.penStyle == MGL_BITMAP_TRANSPARENT)
            dc->r.Use8x8TransMonoPattern(dc->a.cntPenPat);
        else if (dc->a.penStyle == MGL_PIXMAP)
            dc->r.Use8x8ColorPattern(dc->a.cntPenPixPat);
        else if (dc->a.penStyle == MGL_PIXMAP_TRANSPARENT)
            dc->r.Use8x8TransColorPattern(dc->a.cntPenPixPat,dc->a.penPixPatTrans);
        dc->r.SetLineStipple(dc->a.lineStipple);
        dc->r.SetLineStippleCount(dc->a.stippleCount);
        }
    dc->r.SetPlaneMask(dc->a.planeMask);
    if (DC.r.SetBlendFunc)
        dc->r.SetBlendFunc(dc->a.srcBlendFunc,dc->a.dstBlendFunc);
    else {
        DC.r.SetSrcBlendFunc_Old(DC.a.srcBlendFunc);
        DC.r.SetDstBlendFunc_Old(DC.a.dstBlendFunc);
        }
    dc->r.SetAlphaValue(dc->a.alphaValue);
}

