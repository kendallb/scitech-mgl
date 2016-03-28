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
* Description:  Common C module for Graphics Accelerator routines used
*               by all SciTech SNAP Graphics accelerated device drivers.
*
****************************************************************************/

#include "mgl.h"
#include "drivers/common/gsnap.h"

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
RETURNS:
Pointer to intialised instance data.

REMARKS:
This function initialises any necessary instance data for the device.
****************************************************************************/
void * MGLAPI SNAP_initInstance(
    void *inst)
{
    SNAP_data   *instance = (SNAP_data*)inst;

    if (instance) {
        /* Flag that this driver has not been inst->svga.detected yet */
        instance->svga.detected = -1;
        instance->svga.accelerated = false;
        }
    return inst;
}

/****************************************************************************
PARAMETERS:
dc  - Device context

REMARKS:
Destroys all internal memory allocated for the device context.
****************************************************************************/
static void MGLAPI SNAP_destroyDC(
    MGLDC *dc)
{
    SNAP_data   *inst = (SNAP_data*)dc->v;

    if (--dc->v->d.refCount == 0) {
        /* Unload SNAP display device driver if no more instances */
        SNAP_unload(inst);
        }
}

/****************************************************************************
PARAMETERS:
inst    - Pointer to the device instance data
id      - Driver id to store in table
mode    - Index into mode association table
modes   - Video mode/device driver association table
md      - SciTech SNAP Graphics mode information block to fill in

REMARKS:
Searches through the list of accelerated modes supported by the loaded
driver for one that matches the desired resolution and pixel depth.
****************************************************************************/
static ibool SNAP_useMode(
    SNAP_data *inst,
    int id,
    int mode,
    modetab modes,
    GA_mdEntry  *md,
    ibool accelerated)
{
    ulong   flags = 0;

    /* Fail the mode if we require acceleration or vice versa */
    if (accelerated != ((md->Attributes & gaHaveAccel2D) != 0))
        return false;

    /* Extract hardware flags for the mode */
    if (md->HaveRefresh)
        flags |= MGL_HAVE_REFRESH_CTRL;
    if (md->Attributes & gaHaveLinearBuffer)
        flags |= MGL_HAVE_LINEAR;
    if (md->Attributes & gaHaveInterlaced)
        flags |= MGL_HAVE_INTERLACED;
    if (md->Attributes & gaHaveDoubleScan)
        flags |= MGL_HAVE_DOUBLE_SCAN;
    if (md->Attributes & gaHaveTripleBuffer)
        flags |= MGL_HAVE_TRIPLEBUFFER;
    if (md->Attributes & gaHaveStereo)
        flags |= MGL_HAVE_STEREO | MGL_HAVE_STEREO_DUAL;
    if (md->Attributes & gaHaveHWStereoSync)
        flags |= MGL_HAVE_STEREO_HWSYNC;
    if (md->Attributes & gaHaveEVCStereoSync)
        flags |= MGL_HAVE_STEREO_EVCSYNC;
    if (md->Attributes & gaHaveHWCursor)
        flags |= MGL_HAVE_HWCURSOR;
    if (md->Attributes & gaHaveAccel2D)
        flags |= MGL_HAVE_ACCEL_2D;
    if (md->Attributes & gaHaveAccel3D)
        flags |= MGL_HAVE_ACCEL_3D;
    if (md->Attributes & gaHaveAccelVideo)
        flags |= MGL_HAVE_ACCEL_VIDEO;

    /* Ok, the mode is valid so enable it */
    DRV_useMode(modes,mode,id,md->XResolution,md->YResolution,
        md->BitsPerPixel,md->MaxBuffers,flags);

    (void)inst;
    return true;
}

/****************************************************************************
RETURNS:
True on success, false otherwise.

REMARKS:
Helper function to load the SciTech SNAP Graphics driver and enumerate all
available display modes.
****************************************************************************/
ibool MGLAPI SNAP_load(
    SNAP_data *inst)
{
    GA_modeInfo     modeInfo;
    int             i,oldDevice;
    N_uint16        *p;
    GA_mdEntry      *md;
    GA_initFuncs    *init;
    GA_driverFuncs  *drv;

    /* Grab the SNAP semaphore and switch the active device to the
     * device that we are constructing the device context for.
     */
    PM_lockSNAPAccess(0,true);
    oldDevice = GA_getActiveDevice();
    if (oldDevice != GET_CURRENT_DEVICE())
        GA_setActiveDevice(GET_CURRENT_DEVICE());   /* Don't want mixed mode here! */

    /* Load the SciTech SNAP Graphics driver if it has not already been loaded */
    if (!DEV.gaPtr) {
        if ((DEV.gaPtr = GA_loadDriver(GET_CURRENT_DEVICE(),false)) == NULL) {
            GA_setActiveDevice(oldDevice);
            PM_unlockSNAPAccess(0);
            return false;
            }
        }

    /* Initialise the driver if it has not already been initialised */
    if (!DEV.gaInfo) {
        /* Allocate memory for the SciTech SNAP Graphics dection info, and get functions */
        init = &inst->svga.init;
        init->dwSize = sizeof(*init);
        if (!GA_queryFunctions(DEV.gaPtr,GA_GET_INITFUNCS,init))
            MGL_fatalError("GA_queryFunctions(GA_GET_INITFUNCS) failed!");
        drv = &inst->svga.drv;
        drv->dwSize = sizeof(*drv);
        if (!GA_queryFunctions(DEV.gaPtr,GA_GET_DRIVERFUNCS,drv))
            MGL_fatalError("GA_queryFunctions(GA_GET_DRIVERFUNCS) failed!");

        /* Enumerate all display modes available in the driver */
        p = DEV.gaPtr->AvailableModes;
        for (i = 0,md = inst->svga.modeList; *p != 0xFFFF; p++) {
            modeInfo.dwSize = sizeof(modeInfo);
            if (init->GetVideoModeInfo(*p,&modeInfo) != 0)
                continue;
            if (modeInfo.Attributes & gaIsTextMode)
                continue;
            if (modeInfo.Attributes & gaIsGUIDesktop)
                continue;
            if (i >= MAX_DRIVER_MODES)
                break;
            if (modeInfo.Attributes & gaHaveAccel2D)
                inst->svga.accelerated = true;
            md->mode                    = *p;
            md->XResolution             = modeInfo.XResolution;
            md->YResolution             = modeInfo.YResolution;
            md->BitsPerPixel            = (uchar)modeInfo.BitsPerPixel;
            md->HaveRefresh             = modeInfo.RefreshRateList[1] != -1;
            md->Attributes              = modeInfo.Attributes;
            md->BytesPerScanLine        = (short)modeInfo.BytesPerScanLine;
            md->PixelFormat             = modeInfo.PixelFormat;
            md->MaxBuffers              = modeInfo.MaxBuffers;
            md->BitmapStartAlign        = modeInfo.BitmapStartAlign;
            md->BitmapStridePad         = modeInfo.BitmapStridePad;
            i++;
            md++;
            }
        md->mode = -1;

        /* Save the mode list so we can reuse it for other drivers */
        DEV.gaInfo = &inst->svga;
        }
    else {
        /* We have already detected the SciTech SNAP Graphics interface for a different
         * driver, so copy the cached mode information from the other driver.
         */
        inst->svga = *((SNAP_detectInfo*)DEV.gaInfo);
        }

    /* Restore the active device and release the semaphore */
    GA_setActiveDevice(oldDevice);
    PM_unlockSNAPAccess(0);
    return true;
}

/****************************************************************************
RETURNS:
True on success, false otherwise.

REMARKS:
Helper function to unload the SciTech SNAP Graphics driver.
****************************************************************************/
ibool MGLAPI SNAP_unload(
    SNAP_data *inst)
{
    int             oldDevice;

    /* Nothing to do if SNAP Graphics driver is not loaded. */
    if (!DEV.gaPtr)
        return false;

    /* Grab the SNAP semaphore and switch the active device to the
     * device that we are unloading the device context for.
     */
    PM_lockSNAPAccess(0,true);
    oldDevice = GA_getActiveDevice();
    if (oldDevice != GET_CURRENT_DEVICE())
        GA_setActiveDevice(GET_CURRENT_DEVICE());

    /* Unload the SNAP Graphics driver for the current device. */
    GA_unloadDriver(DEV.gaPtr);
    DEV.gaPtr = NULL;

    /* Restore the active device and release the semaphore */
    GA_setActiveDevice(oldDevice);
    PM_unlockSNAPAccess(0);
    return true;
}

/****************************************************************************
RETURNS:
True if the video card is a supported Accelerated SuperVGA

REMARKS:
Determines if a configured ACCEL.DRV driver is available to be loaded. If
the driver is found, we load the driver file into our applications code
space.
****************************************************************************/
ibool MGLAPI SNAP_hardwareDetect(
    SNAP_data *inst,
    ibool accelerated,
    int id,
    int *numModes,
    modetab availableModes)
{
    GA_mdEntry  *md;

    /* Check if we have already detected this driver */
    if (inst->svga.detected != -1)
        return inst->svga.detected;

    /* Load the SciTech SNAP Graphics driver if it has not already been loaded */
    if (!SNAP_load(inst))
        return inst->svga.detected = false;

    /* Now register all compatible display modes for this driver */
    for (md = inst->svga.modeList; md->mode != -1; md++) {
        if (*numModes >= MAX_DRIVER_MODES)
            break;
        if (SNAP_useMode(inst,id,*numModes,availableModes,md,accelerated))
            (*numModes)++;
        }
    return inst->svga.detected = true;
}

/****************************************************************************
REMARKS:
This function call the SciTech SNAP Graphics driver to set the graphics mode
given the passed in parameters. If the refresh rate is not set to the adapter
default setting, we attempt to find a suitable refresh rate using the GTF
timing formulas.
****************************************************************************/
static ibool GA_setMode(
    MGLDC *dc,
    int mode,
    N_int32 *virtualX,
    N_int32 *virtualY,
    N_int32 *bytesPerLine,
    N_int32 *maxMem,
    N_int32 refreshRate,
    GA_CRTCInfo *crtc)
{
    GA_initFuncs *init = &SVGA(dc).init;
    *bytesPerLine = -1;
    return (init->SetVideoMode(mode,virtualX,virtualY,bytesPerLine,maxMem,refreshRate,crtc) == 0);
}

/****************************************************************************
PARAMETERS:
dc          - Device context to initialise
mode        - Accelerated video mode to set
oldMode     - Place to store old BIOS mode number
virtualX    - Virtual X coordinate for the display mode
virtualY    - Virtual Y coordinate for the display mode
numBuffers  - Number of buffers to allocate for the mode
stereo      - True if stereo is enabled
refreshRate - Refresh rate for the display mode

RETURNS:
True on success, false on failure.

REMARKS:
Initialises the video mode by calling the loaded accelerated device driver
to initialise the video mode. We also save the state of the old video mode
in the specified parameters.
****************************************************************************/
ibool MGLAPI SNAP_setMode(
    MGLDC *dc,
    N_uint32 mode,
    N_uint32 *oldMode,
    N_int32 virtualX,
    N_int32 virtualY,
    N_int32 numBuffers,
    ibool stereo,
    N_int32 refreshRate,
    ibool useLinearBlits)
{
    display_vec         *d = &dc->v->d;
    int                 isStereo = false,scrolling = false,interlaced;
    float               maxRefresh;
    N_int32             maxMem,bytesPerLine = -1,gaRefreshRate = 0;
    GA_modeInfo         modeInfo;
    GA_CRTCInfo         crtc;
    GA_options          opt;
    SNAP_detectInfo     *svga = &SVGA(dc);
    GA_initFuncs        *init = &svga->init;
    GA_2DStateFuncs     drvState2d;
    GA_2DRenderFuncs    drvDraw2d;
    GA_cursorFuncs      drvCursor;
    GA_2DStateFuncs     state2d;
    GA_2DRenderFuncs    draw2d;
    GA_bufferFuncs      buf;
    GA_clipperFuncs     clip;
    int                 haveHWCursor,oldDevice;
    ibool               rc;
    N_int32             *r;
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

    /* Grab the SNAP semaphore and switch the active device to the
     * device that we are constructing the device context for.
     * NB: There must be only one device enabled, else we risk that
     * VGA memory access will go to the wrong adapter.
     */
    PM_lockSNAPAccess(0,true);
    oldDevice = GA_getActiveDevice();
    if (oldDevice != GET_CURRENT_DEVICE())
        GA_setActiveDevice(GET_CURRENT_DEVICE());

    /* Save the old video mode and get the current mode information */
    *oldMode = init->GetVideoMode();
    modeInfo.dwSize = sizeof(modeInfo);
    init->GetVideoModeInfo(mode & gaModeMask,&modeInfo);

    if (!(mode & gaWindowedMode)) {
        /* Get the driver options so we can determine what type of LC glasses
         * the user has connected to the computer.
         */
        opt.dwSize = sizeof(opt);
        init->GetOptions(&opt);

        /* If stereo mode is enabled, check what type of stereo mode we support
         * and set things up for stereo operation. If software stereo emulation
         * support is needed, the software stereo functions have to be passed to
         * the buffer manager at initialization time.
         */
        isStereo = _MGL_NO_STEREO;
        if (stereo) {
            isStereo = _MGL_SOFT_STEREO;
            if (dc->mi.modeFlags & MGL_HAVE_STEREO) {
                /* We have hardware stereo page flipping, so use it */
                isStereo = _MGL_HW_STEREO;
                }

            /* Configure the stereo type depending on the glasses attached.
             * Unless the user has glasses attached to the hardware sync port
             * we have to use either blue codes, keyframes or fall back on
             * pure software stereo flipping (for I/O port and VSync glasses).
             */
            switch (opt.glassesType) {
                case gaGlassesBlueCode:
                    isStereo |= _MGL_STEREO_BLUECODE;
                    break;
                case gaGlassesIOPort:
                case gaGlassesVSync:
                    isStereo = _MGL_SOFT_STEREO;
                    break;
                }
            numBuffers *= 2;
            dc->flags |= MGL_STEREO_ACCESS;
            }

        /* Check that we are withing the virtual dimensions */
        if (numBuffers == 0) {
            GA_setActiveDevice(oldDevice);
            PM_unlockSNAPAccess(0);
            return false;
            }
        if (virtualX != -1 && virtualY != -1) {
            if (virtualX <= dc->mi.xRes || virtualY <= dc->mi.yRes) {
                GA_setActiveDevice(oldDevice);
                PM_unlockSNAPAccess(0);
                return false;
                }
            scrolling = true;
            }
        else {
            virtualX = virtualY = -1;
            scrolling = false;
            }

        /* Determine if we are doing multi-buffering */
        if (numBuffers && ((numBuffers-1) <= dc->mi.maxPage) && dc->mi.maxPage > 0) {
            if (stereo)
                dc->mi.maxPage = (numBuffers/2)-1;
            else
                dc->mi.maxPage = numBuffers-1;
            }
        else
            dc->mi.maxPage = 0;

        /* Compute CRTC timings if refresh rate is specified */
        gaRefreshRate = refreshRate;
        if (gaRefreshRate != MGL_DEFAULT_REFRESH) {
            /* First see if the desired refresh rate is supported directly
             * by the selected display mode. If not then we have to use
             * GTF to create the desired refresh rate.
             */
            for (r = modeInfo.RefreshRateList; *r != -1; r++) {
                if (*r == gaRefreshRate)
                    break;
                }
            if (*r == -1) {
                /* Not a directly supported refresh rate, so try to use
                 * the VESA Generalised Timing Formula to set the
                 * refresh rate.
                 */
                GA_getMaxRefreshRate(DEV.gaPtr,0/*cntHead*/,&modeInfo,false,&maxRefresh);
                if (gaRefreshRate < 0) {
                    gaRefreshRate = -gaRefreshRate;
                    if (gaRefreshRate > (int)maxRefresh/2)
                        gaRefreshRate = (int)maxRefresh/2;
                    if (gaRefreshRate < 87)
                        gaRefreshRate = 87;
                    interlaced = true;
                    }
                else {
                    if (gaRefreshRate > (int)maxRefresh)
                        gaRefreshRate = (int)maxRefresh;
                    if (gaRefreshRate < 56)
                        gaRefreshRate = 56;
                    interlaced = false;
                    }
                if (!GA_computeCRTCTimings(DEV.gaPtr,0/*cntHead*/,&modeInfo,gaRefreshRate,interlaced,&crtc,false)) {
                    GA_setActiveDevice(oldDevice);
                    PM_unlockSNAPAccess(0);
                    return false;
                    }
                mode |= gaRefreshCtrl;
                }
            }
        else
            gaRefreshRate = 0;
        }

    /* Set the display mode */
    if (!GA_setMode(dc,mode,&virtualX,&virtualY,&bytesPerLine,&maxMem,gaRefreshRate,&crtc)) {
        GA_setActiveDevice(oldDevice);
        PM_unlockSNAPAccess(0);
        return false;
        }

    /* Now load the SciTech SNAP Graphics reference rasteriser */
    if (!GA_loadRef2d(DEV.gaPtr,false,&modeInfo,dc->mi.pageSize * (dc->mi.maxPage+1),&DEV.ref2d)) {
        GA_setActiveDevice(oldDevice);
        PM_unlockSNAPAccess(0);
        return false;
        }

    /* Initialise the buffer manager */
    buf.dwSize = sizeof(buf);
    if (!REF2D_queryFunctions(DEV.ref2d,GA_GET_BUFFERFUNCS,&buf)) {
        GA_setActiveDevice(oldDevice);
        PM_unlockSNAPAccess(0);
        return false;
        }

    rc = buf.InitBuffers(numBuffers,0,(isStereo) ? &stereoFuncs : NULL);

    /* Restore the old active device */
    GA_setActiveDevice(oldDevice);
    PM_unlockSNAPAccess(0);

    if (!rc)
        return false;

    dc->r.GetPrimaryBuffer      = buf.GetPrimaryBuffer;
    dc->r.GetFlippableBuffer    = buf.GetFlippableBuffer;
    dc->r.AllocBuffer           = buf.AllocBuffer;
    dc->r.FreeBuffer            = buf.FreeBuffer;
    dc->r.LockBuffer            = buf.LockBuffer;
    dc->r.UnlockBuffer          = buf.UnlockBuffer;
    dc->r.SetActiveBuffer       = buf.SetActiveBuffer;
    dc->r.FlipToBuffer          = buf.FlipToBuffer;
    dc->r.FlipToStereoBuffer    = buf.FlipToStereoBuffer;
    dc->r.GetFlipStatus         = buf.GetFlipStatus;
    dc->r.WaitTillFlipped       = buf.WaitTillFlipped;
    dc->r.UpdateCache           = buf.UpdateCache;
    dc->r.UpdateFromCache       = buf.UpdateFromCache;
    dc->r.BitBltBuf             = buf.BitBltBuf;
    dc->r.BitBltPattBuf         = buf.BitBltPattBuf;
    dc->r.BitBltColorPattBuf    = buf.BitBltColorPattBuf;
    dc->r.SrcTransBltBuf        = buf.SrcTransBltBuf;
    dc->r.DstTransBltBuf        = buf.DstTransBltBuf;
    dc->r.StretchBltBuf         = buf.StretchBltBuf;
    dc->r.BitBltPlaneMaskedBuf  = buf.BitBltPlaneMaskedBuf;
    dc->r.BitBltFxBuf           = buf.BitBltFxBuf;
    dc->r.DrawRectBuf           = buf.DrawRectBuf;
    dc->r.GetClipper            = buf.GetClipper;
    dc->r.SetClipper            = buf.SetClipper;
    dc->r.GetActiveBuffer       = buf.GetActiveBuffer;
    dc->r.GetVisibleBuffer      = buf.GetVisibleBuffer;
    dc->r.GetVisibleRightBuffer = buf.GetVisibleRightBuffer;

    /* Initialise the clip list manager */
    clip.dwSize = sizeof(clip);
    if (REF2D_queryFunctions(DEV.ref2d,GA_GET_CLIPPERFUNCS,&clip)) {
        dc->r.CreateClipper             = clip.CreateClipper;
        dc->r.IsClipListChanged         = clip.IsClipListChanged;
        dc->r.GetClipList               = clip.GetClipList;
        dc->r.DestroyClipper            = clip.DestroyClipper;
        }

    /* Initialise the internals for the mode */
    if (!(mode & gaWindowedMode)) {
        dc->mi.xRes = virtualX - 1;
        dc->mi.yRes = virtualY - 1;
        }
    dc->startX = dc->startY = 0;
    if (dc->mi.bitsPerPixel == 4)
        dc->mi.bytesPerLine = bytesPerLine * 8;
    else
        dc->mi.bytesPerLine = bytesPerLine;
    dc->mi.pageSize = virtualY * bytesPerLine;
    if (scrolling) {
        dc->mi.maxPage = (modeInfo.MaxScanLines / (dc->mi.yRes+1))-1;
        dc->virtualX = virtualX;
        dc->virtualY = virtualY;
        }
    else {
        dc->virtualX = -1;
        dc->virtualY = -1;
        }
    dc->numBuffers = numBuffers;
    dc->surface = NULL;
    dc->flags |= MGL_LINEAR_ACCESS;
    if (!(mode & gaLinearBuffer))
        dc->flags |= MGL_SHADOW_BUFFER;

    /* Initialise pointers to device driver vectors */
    svga->drv.dwSize = sizeof(svga->drv);
    REF2D_queryFunctions(DEV.ref2d,GA_GET_DRIVERFUNCS,&svga->drv);
    svga->cursor.dwSize = sizeof(svga->cursor);
    REF2D_queryFunctions(DEV.ref2d,GA_GET_CURSORFUNCS,&svga->cursor);
    svga->video.dwSize = sizeof(svga->video);
    GA_queryFunctions(DEV.gaPtr,GA_GET_VIDEOFUNCS,&svga->video);
    drvState2d.dwSize = sizeof(drvState2d);
    GA_queryFunctions(DEV.gaPtr,GA_GET_2DSTATEFUNCS,&drvState2d);
    drvDraw2d.dwSize = sizeof(drvDraw2d);
    GA_queryFunctions(DEV.gaPtr,GA_GET_2DRENDERFUNCS,&drvDraw2d);
    drvCursor.dwSize = sizeof(drvCursor);
    state2d.dwSize = sizeof(state2d);
    REF2D_queryFunctions(DEV.ref2d,GA_GET_2DSTATEFUNCS,&state2d);
    draw2d.dwSize = sizeof(draw2d);
    REF2D_queryFunctions(DEV.ref2d,GA_GET_2DRENDERFUNCS,&draw2d);
    haveHWCursor = GA_queryFunctions(DEV.gaPtr,GA_GET_CURSORFUNCS,&drvCursor);

    /* Now fill in device context information specific to display DC's */
    d->refCount++;
    d->device                       = GET_CURRENT_DEVICE();
    d->maxProgram                   = 256;
    d->widePalette                  = (dc->mi.scratch2 & gaHave8BitDAC) != 0;
    d->lockCount                    = 0;
    dc->activePage                  = 0;
    dc->visualPage                  = 0;
    if (haveHWCursor)
        dc->v->d.hardwareCursor     = svga->cursor.IsHardwareCursor();
    else
        d->hardwareCursor           = false;
    dc->v->d.setCursor              = SNAP_setCursor;
    dc->v->d.setCursorColor         = SNAP_setCursorColor;
    dc->v->d.setCursorPos           = SNAP_setCursorPos;
    dc->v->d.showCursor             = SNAP_showCursor;
    d->isStereo                     = isStereo;
    d->stereoRunning                = false;
    d->refreshRate                  = refreshRate;
    d->useLinearBlits               = useLinearBlits;
    d->restoreTextMode              = SNAP_restoreTextMode;
    d->restoreGraphMode             = SNAP_restoreGraphMode;
    d->setActivePage                = SNAP_setActivePage;
    d->setVisualPage                = SNAP_setVisualPage;
    if (svga->drv.WaitVSync)
        d->vSync                    = SNAP_vSync;
    if (svga->drv.IsVSync)
        d->isVSync                  = SNAP_isVSync;
    if (svga->drv.GetCurrentScanLine)
        d->getCurrentScanLine       = SNAP_getCurrentScanLine;
    d->setDisplayStart              = SNAP_setDisplayStart;
    d->startStereo                  = SNAP_startStereo;
    d->stopStereo                   = SNAP_stopStereo;
    if (dc->mi.scratch2 & gaIsVirtualMode) {
        /* In virtual display modes we don't have direct framebuffer
         * access, and we also never want to change the active device.
         */
        dc->flags                   &= ~MGL_LINEAR_ACCESS;
        d->setActiveDevice          = NULL;
        d->getActiveDevice          = NULL;
        }
    else {
        d->setActiveDevice          = GA_setActiveDevice;
        d->getActiveDevice          = GA_getActiveDevice;
        }
    if (svga->drv.SetGammaCorrectData) {
        dc->r.setGammaRamp          = SNAP_setGammaRamp;
        dc->r.getGammaRamp          = SNAP_getGammaRamp;
        }
    dc->r.getDefaultPalette         = DRV_getDefaultPalette;
    dc->r.realizePalette            = SNAP_realizePalette;
    dc->r.makeCurrent               = SNAP_makeCurrent;

    /* Setup all rendering vectors for SciTech SNAP Graphics functions */
    DRV_setSNAPRenderingVectors(dc,DEV.ref2d);

    /* Setup flags for accelerated display modes */
    if (dc->mi.scratch2 & gaHaveAccel2D) {
        if (drvDraw2d.DrawRect)
            dc->flags |= MGL_HW_RECT;
        if (drvDraw2d.DrawPattRect)
            dc->flags |= MGL_HW_PATT_RECT;
        if (drvDraw2d.DrawColorPattRect)
            dc->flags |= MGL_HW_CLRPATT_RECT;
        if (drvDraw2d.DrawLineInt)
            dc->flags |= MGL_HW_LINE;
        if (drvDraw2d.DrawStippleLineInt)
            dc->flags |= MGL_HW_STIPPLE_LINE;
        if (drvDraw2d.MonoBltSys || drvDraw2d.PutMonoImageMSBSys_Old)
            dc->flags |= MGL_HW_MONO_BLT;
        if (drvDraw2d.BitBlt)
            dc->flags |= MGL_HW_SCR_BLT;
        if (drvDraw2d.BitBltSys)
            dc->flags |= MGL_HW_SYS_BLT;
        if (drvDraw2d.SrcTransBlt)
            dc->flags |= MGL_HW_SRCTRANS_BLT;
        if (drvDraw2d.SrcTransBltSys)
            dc->flags |= MGL_HW_SRCTRANS_SYS_BLT;
        if (drvDraw2d.DstTransBlt)
            dc->flags |= MGL_HW_DSTTRANS_BLT;
        if (drvDraw2d.DstTransBltSys)
            dc->flags |= MGL_HW_DSTTRANS_SYS_BLT;
        if (drvDraw2d.StretchBlt)
            dc->flags |= MGL_HW_STRETCH_BLT;
        if (drvDraw2d.StretchBltSys)
            dc->flags |= MGL_HW_STRETCH_SYS_BLT;
        if (drvDraw2d.BitBltFx) {
            dc->flags |= MGL_HW_EFFECTS_BLT;
            dc->r.BitBltFxTest = drvDraw2d.BitBltFxTest;
            }
        if (drvDraw2d.BitBltFxSys)
            dc->flags |= MGL_HW_EFFECTS_SYS_BLT;
        }

    /* Setup the framebuffer access functions for shadow buffer operation */
    if (dc->flags & MGL_SHADOW_BUFFER) {
        }

    /* Ensure active and visual pages are set to 0 and set the software
     * rasteriser draw buffer.
     */
    dc->activePage = -1;
    dc->v->d.setActivePage(dc,0);
    MAKE_ACTIVE_DEVICE(dc);
    dc->v->d.setVisualPage(dc,0,MGL_dontWait);
    RESTORE_ACTIVE_DEVICE(dc);
    return true;
}

/****************************************************************************
RETURNS:
Number of bits in the value
****************************************************************************/
static int NumBits(
    uint value)
{
    int count = 0;
    while (value & 1) {
        count++;
        value >>= 1;
        }
    return count;
}

/****************************************************************************
RETURNS:
The aspect ratio for the mode when displayed on the monitor. We assume
all 16:9 modes are displayed with a 1:1 aspect ratio, and others are
computed for a standard 4:3 VGA monitor configuration.

Ideally we probably want to get the aspect ratio from SciTech SNAP Graphics
one day, where it can adjust the aspect ratio for modes based on the dimensions
of the actual display device.
****************************************************************************/
static int AspectRatio(
    GA_mdEntry *md)
{
    int ratio = (md->XResolution * 1000L * 3) / (md->YResolution * 4);
    if (ratio == 1333)
        ratio = 1000;       /* Special case for 16:9 modes          */
    if (ratio == 562 || ratio == 421)
        ratio = 1000;       /* Special case for portrait modes      */
    return ratio;
}

/****************************************************************************
RETURNS:
The maximum color value for the display mode.
****************************************************************************/
static color_t MaxColor(
    int bits)
{
    switch (bits) {
        case 4:     return 0xFUL;
        case 8:     return 0xFFUL;
        case 15:    return 0x7FFFUL;
        case 16:    return 0xFFFFUL;
        case 24:    return 0xFFFFFFUL;
        case 32:    return 0xFFFFFFFFUL;
        }
    return 0;
}

/****************************************************************************
PARAMETERS:
inst            - Instance data for the driver
dc              - Device context to initialise
mode            - Display mode to set
hwnd            - Handle to fullscreen console
virtualX        - Virtual X screen dimensions (-1 if not scrolling)
virtualY        - Virtual Y screen dimensions (-1 if not scrolling)
numBuffers      - Number of buffers to allocate
stereo          - True if stereo should be enabled
refreshRate     - Desired refresh rate for mode

RETURNS:
True on sucess, false on failure.

REMARKS:
This function initialise the device driver for use with the passed in
device context. We also switch the system into graphics mode when this
function is called.
****************************************************************************/
ibool MGLAPI SNAP_initHardware(
    SNAP_data *inst,
    MGLDC *dc,
    modeent *mode,
    ulong hwnd,
    int virtualX,
    int virtualY,
    int numBuffers,
    ibool stereo,
    int refreshRate,
    ibool useLinearBlits)
{
    SNAP_state          *state = &inst->state;
    display_vec         *d;
    gmode_t             *mi = &dc->mi;
    GA_mdEntry          *md;

    /* Find the mode in our internal mode list */
    for (md = inst->svga.modeList; md->mode != -1; md++) {
        if (md->XResolution == mode->xRes && md->YResolution == mode->yRes
                && md->BitsPerPixel == mode->bits)
            break;
        }
    if (md->mode == -1)
        return false;

    /* Fill in the device context mode information block */
    mi->xRes                    = md->XResolution-1;
    mi->yRes                    = md->YResolution-1;
    mi->bitsPerPixel            = md->BitsPerPixel;
    mi->maxColor                = MaxColor(md->BitsPerPixel);
    mi->maxPage                 = md->MaxBuffers-1;
    mi->bytesPerLine            = md->BytesPerScanLine;
    mi->aspectRatio             = AspectRatio(md);
    mi->pageSize                = md->YResolution * md->BytesPerScanLine;
    mi->scratch1                = md->mode;
    mi->scratch2                = md->Attributes;
    if (md->Attributes & gaHaveLinearBuffer)
        mi->scratch1            |= gaLinearBuffer;
    mi->modeFlags               = mode->flags;
    mi->bitmapStartAlign        = md->BitmapStartAlign;
    mi->bitmapStridePad         = md->BitmapStridePad;

    /* Copy DirectColor mask information to mi block */
    if (md->BitsPerPixel <= 8) {
        mi->modeFlags           |= MGL_IS_COLOR_INDEX;
        mi->redMaskSize         = 8;
        mi->redFieldPosition    = 0;
        mi->greenMaskSize       = 0;
        mi->greenFieldPosition  = 0;
        mi->blueMaskSize        = 0;
        mi->blueFieldPosition   = 0;
        mi->alphaMaskSize       = 0;
        mi->alphaFieldPosition  = 0;
        }
    else {
        mi->redMaskSize = NumBits(md->PixelFormat.RedMask);
        mi->redFieldPosition = md->PixelFormat.RedPosition;
        mi->greenMaskSize = NumBits(md->PixelFormat.GreenMask);
        mi->greenFieldPosition = md->PixelFormat.GreenPosition;
        mi->blueMaskSize = NumBits(md->PixelFormat.BlueMask);
        mi->blueFieldPosition = md->PixelFormat.BluePosition;
        mi->alphaMaskSize = NumBits(md->PixelFormat.AlphaMask);
        mi->alphaFieldPosition = md->PixelFormat.AlphaPosition;
        }

    /* Fill in remaing device context information */
    dc->deviceType          = MGL_DISPLAY_DEVICE;
    dc->xInch               = 9000;
    dc->yInch               = 7000;
    _MGL_computePF(dc);
    dc->v = (drv_vec*)state;
    d = &dc->v->d;
    d->destroy              = SNAP_destroyDC;
    d->hwnd                 = hwnd;

    /* Set the video mode and return status */
    return SNAP_setMode(dc,dc->mi.scratch1,&state->oldMode,virtualX,virtualY,
        numBuffers,stereo,refreshRate,useLinearBlits);
}

/****************************************************************************
PARAMETERS:
dc          - Device context to initialise

REMARKS:
Restore the previous display mode active when the graphics mode was
started.
****************************************************************************/
void MGLAPI SNAP_restoreTextMode(
    MGLDC *dc)
{
    N_int32     virtualX = -1,virtualY = -1,bpl = -1,maxMem;
    SNAP_data   *inst = (SNAP_data*)dc->v;
    SNAP_state  *state = &inst->state;
    ibool       rc;
    int         oldDevice;

    /* Grab the SNAP semaphore and switch the active device.
     * NB: There must be only one device enabled, else we risk that
     * VGA memory access will go to the wrong adapter.
     */
    PM_lockSNAPAccess(0,true);
    oldDevice = GA_getActiveDevice();
    if (oldDevice != GET_CURRENT_DEVICE())
        GA_setActiveDevice(GET_CURRENT_DEVICE());

    /* Restore text mode */
    rc = GA_setMode(dc,state->oldMode | gaNoAccel,&virtualX,&virtualY,&bpl,&maxMem,0,NULL);

    /* Restore active device */
    GA_setActiveDevice(oldDevice);
    PM_unlockSNAPAccess(0);

    /* Don't even think of calling into the OS if secondary device is active */
    if (rc && dc->v->d.device == 0)
        PM_setOSCursorLocation(0,0);

    /* Unload the graphics driver */
    GA_unloadRef2d(DEV.gaPtr);
    DEV.ref2d = NULL;
    DEV.hModRef2d = NULL;
}

/****************************************************************************
REMARKS:
Restore graphics mode again after resetting to text mode. This is used
by Windows to reset the display mode after coming back from an Alt-Tab
event.
****************************************************************************/
ibool MGLAPI SNAP_restoreGraphMode(
    MGLDC *dc)
{
    N_uint32    oldMode;

    if (!SNAP_setMode(dc,dc->mi.scratch1,&oldMode,dc->virtualX,dc->virtualY,
            dc->numBuffers,dc->v->d.isStereo != _MGL_NO_STEREO,
            dc->v->d.refreshRate,dc->v->d.useLinearBlits))
        return false;
    if (dc->v->d.stereoRunning)
        SNAP_startStereo(dc);
    return true;
}

/****************************************************************************
REMARKS:
Make the new device context the currently active device context for
rendering, which includes updating any necessary hardware state that
is cached in the hardware.
****************************************************************************/
void MGLAPI SNAP_makeCurrent(
    MGLDC *dc,
    MGLDC *oldDC,
    ibool partial)
{
#ifdef CHECKED
    /* Check that mixed mode is on for multi-controller support */
    if (!(_MGL_cntDevice & MM_MODE_MIXED) && (dc->v->d.device != 0)) {
        MGL_fatalError("Multi-controller drawing MUST use MM_MIXED_MODE!");
        }
#endif
    if (dc->v->d.device != _MGL_glDevice) {
        if (DEV.glLoaded && (DEV.cntOpenGL == MGL_GL_SNAP)) {
            /* Currently active device has changed and OpenGL is loaded,
             * so reload the OpenGL entry points from the device OpenGL
             * implementation.
             */
            _GA_glFuncs.dwSize = sizeof(_GA_glFuncs);
            GA_glQueryFunctions(DEV.glPtr,GA_GET_GLFUNCS,&_GA_glFuncs);
            _GA_gluFuncs.dwSize = sizeof(_GA_gluFuncs);
            GA_glQueryFunctions(DEV.glPtr,GA_GET_GLUFUNCS,&_GA_gluFuncs);
            }
        _MGL_glDevice = dc->v->d.device;
        }
    dc->r.SetActiveBuffer(dc->activeBuf);
    DRV_makeCurrent(dc,partial);
}

/****************************************************************************
REMARKS:
Enables free running stereo mode. We also re-program the blue color color
index to the correct value in here, so that the developer can enable and
disable support for blue codes at runtime with calls to MGL_setBlueCodeIndex
and MGL_startStereo.
****************************************************************************/
void MGLAPI SNAP_startStereo(
    MGLDC *dc)
{
    GA_driverFuncs *drv = &SVGA(dc).drv;

    if (dc->v->d.isStereo != _MGL_NO_STEREO) {
        MAKE_ACTIVE_DEVICE(dc);
        if (dc->v->d.isStereo & _MGL_STEREO_BLUECODE) {
            /* Program the blue code palette entry to pure blue */
            MGL_setPaletteEntry(dc,_MGL_blueCodeIndex,0,0,255);
            MGL_realizePalette(dc,256,0,false);
            }
        if (dc->v->d.isStereo & _MGL_HW_STEREO) {
            /* Enable hardware stereo page flipping */
            drv->EnableStereoMode(true);
            }
        if (dc->v->d.isStereo & _MGL_SOFT_STEREO) {
            /* Enable software page flip interrupt handler */
            GA_softStereoOn();
            }
        dc->v->d.stereoRunning = true;
        MGL_setVisualPage(dc,dc->visualPage,MGL_dontWait);
        RESTORE_ACTIVE_DEVICE(dc);
        }
}

/****************************************************************************
REMARKS:
Disables free running stereo mode, so the display controller always
displays from the left eye image.
****************************************************************************/
void MGLAPI SNAP_stopStereo(
    MGLDC *dc)
{
    GA_driverFuncs *drv = &SVGA(dc).drv;

    if (dc->v->d.isStereo != _MGL_NO_STEREO && dc->v->d.stereoRunning) {
        MAKE_ACTIVE_DEVICE(dc);
        if (dc->v->d.isStereo & _MGL_HW_STEREO) {
            /* Disable hardware stereo page flipping */
            drv->EnableStereoMode(false);
            }
        if (dc->v->d.isStereo & _MGL_SOFT_STEREO) {
            /* Disable software page flip interrupt handler */
            GA_softStereoOff();
            }
        dc->v->d.stereoRunning = false;
        RESTORE_ACTIVE_DEVICE(dc);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context
page    - Active page number

REMARKS:
Sets the current active page for the device context to the specified page
value.
****************************************************************************/
void MGLAPI SNAP_setActivePage(
    MGLDC *dc,
    int page)
{
    display_vec *d = &dc->v->d;
    int         ipage = (page & ~MGL_RIGHT_BUFFER);

    if (dc->activePage == page)
        return;
    if (ipage >= 0 && ipage <= dc->mi.maxPage) {
        dc->activePage = page;
        if (d->isStereo != _MGL_NO_STEREO) {
            if (page & MGL_RIGHT_BUFFER)
                ipage = ((page & ~MGL_RIGHT_BUFFER) * 2) + 1;
            else
                ipage *= 2;
            }
        dc->activeBuf = dc->r.GetFlippableBuffer(ipage);
        dc->r.SetActiveBuffer(dc->activeBuf);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context
page    - Active page number

REMARKS:
Sets the current visual page for the device context to the specified page
value.
****************************************************************************/
void MGLAPI SNAP_setVisualPage(
    MGLDC *dc,
    int page,
    int waitVRT)
{
    int             oldActivePage,effVisualPage;
    color_t         blueCodeColor;
    GA_buf          *leftBuf,*rightBuf;

    /* Although the MGL_RIGHT_BUFFER flag is documented only to be effective for
     * MGL_setActivePage(), it would be useful to check for it here for cases
     * where stereo display mode becomes disabled. The page parameter should be
     * masked off to be within valid range in any case.
     */
    effVisualPage = page;
    page &= ~MGL_RIGHT_BUFFER;

    if (page >= 0 && page <= dc->mi.maxPage) {
        dc->visualPage = page;
        if ((dc->v->d.isStereo & _MGL_STEREO_BLUECODE) && dc->v->d.stereoRunning) {
            MGLDC *olddc = MGL_makeCurrentDC(dc);

            /* Get the color for the blue code */
            if (dc->mi.maxColor > 255)
                blueCodeColor = MGL_packColor(&dc->pf,0,0,255);
            else
                blueCodeColor = _MGL_blueCodeIndex;

            /* Save the old color and active page for later */
            oldActivePage = dc->activePage;

            /* Draw the left image blue code (25% of width) */
            MGL_setActivePage(dc,page | MGL_LEFT_BUFFER);
            dc->r.SetForeColor(blueCodeColor);
            dc->r.solid.DrawRect(dc->mi.yRes,0,dc->mi.xRes/4,1);
            dc->r.SetForeColor(0);
            dc->r.solid.DrawRect(dc->mi.yRes,dc->mi.xRes/4,(dc->mi.xRes*3)/4,1);

            /* Draw the right image blue code (75% of width) */
            MGL_setActivePage(dc,page | MGL_RIGHT_BUFFER);
            dc->r.SetForeColor(blueCodeColor);
            dc->r.solid.DrawRect(dc->mi.yRes,0,(dc->mi.xRes*3)/4,1);
            dc->r.SetForeColor(0);
            dc->r.solid.DrawRect(dc->mi.yRes,(dc->mi.xRes*3)/4,dc->mi.xRes/4,1);

            /* Restore the old color and active page */
            dc->r.SetForeColor(dc->a.color);
            MGL_setActivePage(dc,oldActivePage);
            MGL_makeCurrentDC(olddc);
            }

        /* Disable triple buffering if only two pages */
        MAKE_ACTIVE_DEVICE(dc);
        if (dc->virtualX != -1) {
            /* Virtual scrolling is active, so use set display start */
            dc->CRTCBase = (dc->mi.pageSize * page);
            dc->v->d.setDisplayStart(dc,dc->startX,dc->startY,waitVRT);
            }
        else {
            /* Flip to the new visble buffer */
            if (dc->v->d.isStereo != _MGL_NO_STEREO) {
                leftBuf = dc->r.GetFlippableBuffer(page*2);
                rightBuf = dc->r.GetFlippableBuffer(page*2+1);
                /* Flip to stereo buffers if enabled, or separate buffers if not */
                if (dc->v->d.stereoRunning)
                    dc->r.FlipToStereoBuffer(leftBuf,rightBuf,waitVRT);
                else if (effVisualPage & MGL_RIGHT_BUFFER)
                    dc->r.FlipToBuffer(rightBuf,waitVRT);
                else
                    dc->r.FlipToBuffer(leftBuf,waitVRT);
                }
            else {
                leftBuf = dc->r.GetFlippableBuffer(page);
                dc->r.FlipToBuffer(leftBuf,waitVRT);
                }
            }
        RESTORE_ACTIVE_DEVICE(dc);
        }
}

/****************************************************************************
PARAMETERS:
dc          - Device context
x,y         - Display start coordinates
waitFlag    - Flag whether to wait for retrace.

REMARKS:
Sets the current display start address for the screen. The value of waitFlag
can be one of the following:

    -1  - Set coordinates but dont change hardware
    0   - Set coordinates and hardware; no wait for retrace
    1   - Set coordinates and hardware; wait for retrace

Passing a waitFlag of -1 is used to implement double buffering and virtual
scrolling at the same time. You call this function first to set the display
start X and Y coordinates, then you call SVGA_setVisualPage() to swap
display pages and the new start address takes hold with that call.
****************************************************************************/
void MGLAPI SNAP_setDisplayStart(
    MGLDC *dc,
    int x,
    int y,
    int waitFlag)
{
    ulong           start;
    GA_driverFuncs  *drv = &SVGA(dc).drv;

    if (dc->virtualX != -1) {
        /* Update current display start coordinates */
        dc->startX = x;
        dc->startY = y;
        if (waitFlag == -1)
            return;

        /* Compute starting address for display mode */
        MAKE_ACTIVE_DEVICE(dc);
        switch (dc->mi.bitsPerPixel) {
            case 8:
                start = dc->CRTCBase + dc->mi.bytesPerLine * y + x;
                break;
            case 15:
            case 16:
                start = dc->CRTCBase + dc->mi.bytesPerLine * y + x * 2;
                break;
            case 24:
                start = dc->CRTCBase + dc->mi.bytesPerLine * y + x * 3;
                break;
            default: /* case 32: */
                start = dc->CRTCBase + dc->mi.bytesPerLine * y + x * 4;
                break;
            }
        drv->SetDisplayStart(start,(waitFlag == MGL_waitVRT));
        RESTORE_ACTIVE_DEVICE(dc);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context
pal     - Palette of values to program
num     - Number of entries to program
index   - Index to start programming at
waitVRT - True to wait for the vertical retrace

REMARKS:
Program the hardware gamma ramp.
****************************************************************************/
void MGLAPI SNAP_setGammaRamp(
    MGLDC *dc,
    palette_ext_t *pal,
    int num,
    int index,
    int waitVRT)
{
    GA_driverFuncs *drv = &SVGA(dc).drv;

    MAKE_ACTIVE_DEVICE(dc);
    drv->SetGammaCorrectData((GA_palette*)pal,num,index,waitVRT);
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
pal     - Palette of values to program
num     - Number of entries to program
index   - Index to start programming at
waitVRT - True to wait for the vertical retrace

REMARKS:
Read the hardware gamma ramp.
****************************************************************************/
void MGLAPI SNAP_getGammaRamp(
    MGLDC *dc,
    palette_ext_t *pal,
    int num,
    int index)
{
    GA_driverFuncs *drv = &SVGA(dc).drv;

    MAKE_ACTIVE_DEVICE(dc);
    drv->GetGammaCorrectData((GA_palette*)pal,num,index);
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
pal     - Palette of values to program
num     - Number of entries to program
index   - Index to start programming at
waitVRT - True to wait for the vertical retrace

REMARKS:
Program the hardware palette.
****************************************************************************/
void MGLAPI SNAP_realizePalette(
    MGLDC *dc,
    palette_t *pal,
    int num,
    int index,
    int waitVRT)
{
    int             count;
    uchar           *p = (uchar*)&pal[index];
    int             maxProg = dc->v->d.maxProgram;
    GA_driverFuncs  *drv = &SVGA(dc).drv;

    /* Determine if we need to wait for the vertical retrace */
    MAKE_ACTIVE_DEVICE(dc);
    if (!waitVRT)
        count = num;
    else
        count = (num > maxProg) ? maxProg : num;
    while (num) {
        drv->SetPaletteData((GA_palette*)p,count,index,waitVRT);
        index += count;
        p += count*4;
        num -= count;
        count = (num > maxProg) ? maxProg : num;
        }
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context

REMARKS:
Syncs to a vertical interrupt.
****************************************************************************/
void MGLAPI SNAP_vSync(
    MGLDC *dc)
{
    GA_driverFuncs *drv = &SVGA(dc).drv;

    MAKE_ACTIVE_DEVICE(dc);
    drv->WaitVSync();
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context

RETURNS:
True if the vertical retrace is active, false if not.
****************************************************************************/
int MGLAPI SNAP_isVSync(
    MGLDC *dc)
{
    int             vSync;
    GA_driverFuncs  *drv = &SVGA(dc).drv;

    MAKE_ACTIVE_DEVICE(dc);
    vSync = drv->IsVSync();
    RESTORE_ACTIVE_DEVICE(dc);
    return vSync;
}

/****************************************************************************
PARAMETERS:
dc      - Device context

RETURNS:
Current scanline counter returned from the hardware.
****************************************************************************/
int MGLAPI SNAP_getCurrentScanLine(
    MGLDC *dc)
{
    int             line;
    GA_driverFuncs  *drv = &SVGA(dc).drv;

    MAKE_ACTIVE_DEVICE(dc);
    line = drv->GetCurrentScanLine();
    RESTORE_ACTIVE_DEVICE(dc);
    return line;
}

/****************************************************************************
PARAMETERS:
dc      - Device context
curs    - Cursor image to download

REMARKS:
This function downloads the new cursor image to the hardware. Note that
the MGL cursor images are 32x32, while SciTech SNAP Graphics supports 64x64
cursors so we have to convert the format of the image data before downloading
it.
****************************************************************************/
void MGLAPI SNAP_setCursor(
    MGLDC *dc,
    cursor_t *curs)
{
    GA_cursorFuncs *cursor = &SVGA(dc).cursor;

    MAKE_ACTIVE_DEVICE(dc);
    switch (curs->m.bitsPerPixel) {
        case 1:
            cursor->SetMonoCursor((GA_monoCursor*)curs->m.xorMask);
            break;
        case 4:
            cursor->SetColorCursor((GA_colorCursor*)curs->c16.colorData);
            break;
        case 8:
            cursor->SetColorCursor256((GA_colorCursor256*)curs->c256.colorData);
            break;
        case 24:
            cursor->SetColorCursorRGB((GA_colorCursorRGB*)curs->cRGB.colorData);
            break;
        case 32:
            cursor->SetColorCursorRGBA((GA_colorCursorRGBA*)curs->cRGBA.colorData);
            break;
        }
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
x,y     - New position for the mouse cursor

REMARKS:
This function moves the hardware cursor to the new location.
****************************************************************************/
void MGLAPI SNAP_setCursorPos(
    MGLDC *dc,
    int x,
    int y)
{
    GA_cursorFuncs *cursor = &SVGA(dc).cursor;

    MAKE_ACTIVE_DEVICE(dc);
    cursor->SetCursorPos(x,y);
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
visible - Flag to show or hide the cursor

REMARKS:
This function displays or hides the hardware mouse cursor.
****************************************************************************/
void MGLAPI SNAP_showCursor(
    MGLDC *dc,
    ibool visible)
{
    GA_cursorFuncs *cursor = &SVGA(dc).cursor;

    MAKE_ACTIVE_DEVICE(dc);
    cursor->ShowCursor(visible);
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
PARAMETERS:
dc          - Device context
cursorColor - Foreground color for the cursor

REMARKS:
This routine sets the cursor color as a color index or as the RGB values
depending on the display mode.
****************************************************************************/
void MGLAPI SNAP_setCursorColor(
    MGLDC *dc,
    color_t foreColor,
    color_t backColor)
{
    GA_palette      fg,bg;
    GA_cursorFuncs  *cursor = &SVGA(dc).cursor;

    if (dc->mi.modeFlags & MGL_IS_COLOR_INDEX) {
        if (dc->mi.scratch2 & gaHave8bppRGBCursor) {
            palette_t *pal = (palette_t*)&dc->colorTab[foreColor];
            fg.Red = pal->red;
            fg.Green = pal->green;
            fg.Blue =pal->blue;
            pal = (palette_t*)&dc->colorTab[backColor];
            bg.Red = pal->red;
            bg.Green = pal->green;
            bg.Blue =pal->blue;
            }
        else {
            fg.Red = (uchar)foreColor;
            bg.Red = (uchar)backColor;
            }
        }
    else {
        MGL_unpackColorFast(&dc->pf,foreColor,fg.Red,fg.Green,fg.Blue);
        MGL_unpackColorFast(&dc->pf,backColor,bg.Red,bg.Green,bg.Blue);
        }
    MAKE_ACTIVE_DEVICE(dc);
    cursor->SetMonoCursorColor(&fg,&bg);
    RESTORE_ACTIVE_DEVICE(dc);
}

/****************************************************************************
REMARKS:
Update the internal visible clip regions if they have changed for the window
that we are tracking. Unfortunately to date I have not found any way to
track the changes to a window outside of calling the SciTech SNAP Graphics
clipper to tell me if the clip info has changed or not other than polling before
every rendering function. There must be *some* way to determine this without
doing an expensive check on every rendering call...
****************************************************************************/
void MGLAPI SNAP_beginVisibleClipRegion(
    MGLDC *dc)
{
    rect_t      *rectList;
    N_int32     count;
    region_t    *rgn;
    ibool       complexClip;
    GA_buf      *buf = dc->activeBuf;

    /* First ensure that we lock the surface for the duration of our complex
     * clipping code as necessary. We need to do this to ensure that no
     * window movement events combined with task switching can cause the clip
     * list to change from the time we get it, to the time that we finish our
     * software rendering. If we end up doing accelerated rendering in the
     * driver, the driver is reponsible for unlocking the surface if it
     * is already locked.
     */
    if (dc->wm.gendc.clipper)
        dc->r.SetClipper(buf,dc->wm.gendc.clipper);
    dc->r.LockBuffer(buf);

    /* Now process window clip list changes */
    if (dc->r.IsClipListChanged(dc->wm.gendc.clipper)) {
        if (dc->visRegionWin) {
            MGL_freeRegion(dc->visRegionWin);
            dc->visRegionWin = NULL;
            }
        rectList = (rect_t*)dc->r.GetClipList(dc->wm.gendc.clipper,&count);
        if (count == 1) {
            /* There is only rectangle in the list, so set the clip
             * rectangle to the visible rectangle for the entire window.
             * This will make sure it is clipped correctly to a single
             * rectangle if a portion of the window is clipped (ie: task bar).
             */
            dc->visRectWin = rectList[0];
            if (MGL_equalRect(dc->visRectWin,dc->size))
                complexClip = false;
            else
                complexClip = true;
            }
        else
            complexClip = true;

        /* Construct the visible clip region from the list of rectangles.
         * Note that this is presently a simple way to do it, but not necessarily
         * all that fast. If we need a speedier way, we should be able to
         * construct a routine to convert the list to an MGL region given
         * that the rectangles in the list are sorted top to bottom, left to
         * right and that none of them overlap.
         */
        if (complexClip) {
            dc->visRegionWin = MGL_newRegion();
            if (count) {
                while (count--) {
                    _MGL_tmpRectRegion2(rgn,*rectList);
                    MGL_unionRegionOfs(dc->visRegionWin,rgn,0,0);
                    rectList++;
                    }
                }
            dc->visRectWin = dc->visRegionWin->rect;
            }

        /* Update window manager rects with same clip list data. */
        dc->visRectWM = dc->visRectWin;
        dc->visRegionWM = dc->visRegionWin;

        /* Make sure we flush to the appropriate DC's as necessary */
        if (dc == &DC) {
            _MGL_dcPtr->visRectWin      = dc->visRectWin;
            _MGL_dcPtr->visRegionWin    = dc->visRegionWin;
            _MGL_dcPtr->visRectWM       = dc->visRectWM;
            _MGL_dcPtr->visRegionWM     = dc->visRegionWM;
            }
        else if (dc == _MGL_dcPtr) {
            DC.visRectWin               = dc->visRectWin;
            DC.visRegionWin             = dc->visRegionWin;
            DC.visRectWM                = dc->visRectWM;
            DC.visRegionWM              = dc->visRegionWM;
            }

        /* Now re-compute all clipping information for the device context */
        __MGL_recomputeClipping(dc);
        }
}

/****************************************************************************
REMARKS:
Completes the internal visible clip region processing and cleans up.
****************************************************************************/
void MGLAPI SNAP_endVisibleClipRegion(
    MGLDC *dc)
{
    GA_buf      *buf = dc->activeBuf;

    /* Unlock the surface after doing all clipped rendering */
    dc->r.UnlockBuffer(buf);
    if (dc->wm.gendc.clipper)
        dc->r.SetClipper(buf,NULL);
}

