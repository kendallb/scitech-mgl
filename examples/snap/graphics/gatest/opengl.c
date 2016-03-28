/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
*               Graphics OpenGL compatible 3D rendering engine.
*
****************************************************************************/

#include "gatest.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Function to create the OpenGL rendering context
****************************************************************************/
static ibool CreateGLRC(
    GA_glCtx *gl,
    GA_HGLRC *glrc,
    ibool colorIndex)
{
    GA_glVisual visual;
    GA_palette  pal[256];

    if (*glrc)
        glFuncs.DeleteContext(*glrc);
    visual.dwSize       = sizeof(visual);
    visual.DoubleBuffer = (doubleBuffer != 0);
    visual.Stereo       = false;
    visual.ColorIndex   = colorIndex;
    visual.DepthBits    = 16;
    visual.AlphaBits    = 0;
    visual.StencilBits  = 0;
    visual.AccumBits    = 0;
    visual.SoftwareOnly = softwareOnly;
    glFuncs.ChooseVisual(primaryBuf,&visual);
    if ((*glrc = glFuncs.CreateContext(primaryBuf,&visual,NULL)) == NULL)
        PM_fatalError("Unable to create rendering context!");
    if (!glFuncs.MakeCurrent(*glrc))
        PM_fatalError("Unable to set current rendering context!");
    if (noDither)
        glDisable(GL_DITHER);
    else
        glEnable(GL_DITHER);

    /* Set the halfTone palette for 8-bit dithering modes */
    if (!colorIndex && primaryBuf->BitsPerPixel <= 8) {
        driver.GetPaletteData(pal,256,0);
        glFuncs.GetHalfTonePalette(pal+10,236);
        pal[0xFF].Red = 0xFF;
        pal[0xFF].Green = 0xFF;
        pal[0xFF].Blue = 0xFF;
        defcolor = 0xFF;
        driver.SetPaletteData(pal,256,0,false);
        glFuncs.SetPaletteData(*glrc,(GA_palette*)&pal,256,0);
        }
    return true;
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doOpenGLTest(
    GC_devCtx *gc,
    GA_glCtx *gl,
    int xRes,
    int yRes,
    int bitsPerPixel,
    N_uint32 flags,
    int refreshRate,
    GA_CRTCInfo *crtc,
    N_uint32 planeMask,
    ibool force1bppShadow)
{
    ibool       doAll = false;
    GA_HGLRC    glrc = NULL;
    int         numBuffers;

    /* Obtain the mode information and set the display mode */
    GC_leave(gc);
    dc = gc->dc;
    virtualX = virtualY = bytesPerLine = -1;
    modeInfo.dwSize = sizeof(modeInfo);
    if (xRes == -1) {
        if (init.GetVideoModeInfo(flags,&modeInfo) != 0)
            return false;
        if (init.SetVideoMode(flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,refreshRate,crtc) != 0)
            return false;
        }
    else {
        if (init.GetCustomVideoModeInfo(xRes,yRes,-1,-1,bitsPerPixel,&modeInfo) != 0)
            return false;
        if (init.SetCustomVideoMode(xRes,yRes,bitsPerPixel,flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,crtc) != 0)
            return false;
        }
    cntMode = flags;
    if (doubleBuffer == 2)
        numBuffers = modeInfo.MaxBuffers >= 2 ? 2 : 1;
    else
        numBuffers = 1;
    if (!InitSoftwareRasterizer(cntDevice,numBuffers,false))
        PM_fatalError("Unable to initialise software rasteriser!");

    /* Init the OpenGL rendering engine */
    if (!glFuncs.Init(ref2d))
        PM_fatalError("Unable to initialise OpenGL engine!");

    /* Run the tests */
    if (stricmp(accelTestName,"all") == 0)
        doAll = true;
    if (modeInfo.BitsPerPixel <= 8) {
        /* Color index specific tests */
        if (doAll || stricmp(accelTestName,"bounce") == 0) {
            CreateGLRC(gl,&glrc,true);
            if (bounceTest(glrc,modeInfo.XResolution,modeInfo.YResolution) == 0x1B)
                goto DoneTests;
            }
        }

    /* RGB tests */
    if (doAll || stricmp(accelTestName,"gears") == 0) {
        CreateGLRC(gl,&glrc,false);
        if (gearsTest(glrc,modeInfo.XResolution,modeInfo.YResolution) == 0x1B)
            goto DoneTests;
        }
    if (doAll || stricmp(accelTestName,"gears2") == 0) {
        CreateGLRC(gl,&glrc,false);
        if (gears2Test(glrc,modeInfo.XResolution,modeInfo.YResolution) == 0x1B)
            goto DoneTests;
        }
    if (doAll || stricmp(accelTestName,"moth") == 0) {
        CreateGLRC(gl,&glrc,false);
        if (mothTest(glrc,modeInfo.XResolution,modeInfo.YResolution) == 0x1B)
            goto DoneTests;
        }
    if (doAll || stricmp(accelTestName,"mech") == 0) {
        CreateGLRC(gl,&glrc,false);
        if (mechTest(glrc,modeInfo.XResolution,modeInfo.YResolution) == 0x1B)
            goto DoneTests;
        }

    /* Return to text mode, restore the state of the console and exit */
DoneTests:
    glFuncs.DeleteContext(glrc);
    ExitSoftwareRasterizer();
    GC_restore(gc);
    (void)planeMask;
    return true;
}

