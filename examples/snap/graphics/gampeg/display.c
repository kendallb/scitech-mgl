/****************************************************************************
*
*                    SciTech SNAP Graphics MPEG Player!
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
* Description:  Back end display module for the SNAP video player that uses
*               hardware video overlays.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "global.h"
#include "snap/gasdk.h"

/*---------------------------- Global Variables ---------------------------*/

#define MAX_SURFACES    5

typedef struct {
    int     reference;
    ulong   sequenceNumber;
    } surface_rec;

GA_initFuncs                init;
GA_driverFuncs              driver;
static GA_videoFuncs        video;
static GA_HwMCFuncs         hwmc;
#ifndef PLAY_UNDER_X
static PM_HWND              hwndConsole;
static void *               stateBuf;
static int                  oldMode = -1;
#endif
static ibool                displayVideo;
static GA_buf *             target = NULL;
static GA_buf *             past = NULL;
static GA_buf *             future = NULL;
static GA_buf *             nextDisplayed = NULL;
static GA_buf *             lastDisplayed = NULL;
static ulong                sequenceNumber = 1;
static int                  numSurfaces = MAX_SURFACES;
static GA_buf *             surfaces[MAX_SURFACES];
static surface_rec          surfaceInfo[MAX_SURFACES];
static int                  width;
static int                  height;
static int                  numBlocks;
static short *              _block;
static GA_HwMCMacroBlock *  _mb;
static int                  _nextIndex;
static int                  maxBlocks;
static short *              blocks;
static int                  maxMacroBlocks;
static GA_HwMCMacroBlock *  macroBlocks;
static int                  unsignedIntra = 0;
static int                  useIDCT = 0;
static uchar                uiclip[1024];
static uchar *              uiclp;
static char                 iclip[1024];
static char *               iclp;
static short                niclip[1024];
static short *              niclp;
static short                preallocBlock[64];

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    int i;

    if (hwmc.FreeSurface != NULL) {
        for (i = 0; i < numSurfaces; i++)
            hwmc.FreeSurface(surfaces[i]);
        }
    if (video.DisableVideoOutput != NULL)
        video.DisableVideoOutput(0);
#ifndef PLAY_UNDER_X
    if (oldMode != -1) {
        virtualX = virtualY = bytesPerLine = -1;
        ExitSoftwareRasterizer();
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        }
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
#endif
    GA_unloadDriver(dc);
}

/****************************************************************************
REMARKS:
Display a list of available resolutions
****************************************************************************/
void AvailableModes(void)
{
    N_uint16    *modes;

    printf("Available graphics modes:\n");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        printf("    %4d x %4d %d bits per pixel\n",
            modeInfo.XResolution, modeInfo.YResolution, modeInfo.BitsPerPixel);
        }
}

/****************************************************************************
REMARKS:
Initialise the graphics mode with the specified resolution and
color depth.
****************************************************************************/
int InitGraphics(
    int x,
    int y,
    int bits)
{
    int         oldMode;
    N_uint16    *modes;

    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits) {
            cntMode = *modes;
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                cntMode |= gaLinearBuffer;
            oldMode = init.GetVideoMode();
            virtualX = virtualY = bytesPerLine = -1;
            if (init.SetVideoMode(cntMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0)
                return -1;
            if (!InitSoftwareRasterizer(0,1,false))
                PM_fatalError("Unable to initialise software rasteriser!");
            return oldMode;
            }
        }
    return -1;
}

/****************************************************************************
REMARKS:
Start the graphics mode and initialise the display modeul
****************************************************************************/
void init_display(void)
{
    int                 i;
    GA_HwMCSurfaceInf   **HwMCSurface;
    N_uint32            mcFlags;

    /* Check to see if we are running in a window */
    if (PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        exit(-1);
        }

    /* Save the state of the console */
#ifndef PLAY_UNDER_X
    hwndConsole = PM_openConsole(0,0,640,480,8,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);
#endif

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(0,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

#ifndef PLAY_UNDER_X
    /* Get requested resolution, start graphics and draw pattern */
    if ((oldMode = InitGraphics(mode_xres,mode_yres,mode_bits)) == -1) {
        /* Restore the console */
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        AvailableModes();
        GA_unloadDriver(dc);
        exit(-1);
        }
#else
    ref2d = GA_getCurrentRef2d(0);
    modeInfo.dwSize = sizeof(modeInfo);
    init.GetCurrentVideoModeInfo(&modeInfo);
#endif

    /* Get the video functions and check for YUV12 overlays */
    video.dwSize = sizeof(video);
    if (!REF2D_queryFunctions(ref2d,GA_GET_VIDEOFUNCS,&video))
        PM_fatalError("Hardware video overlays not supported!");
    hwmc.dwSize = sizeof(hwmc);
    if (!REF2D_queryFunctions(ref2d,GA_GET_HWMCFUNCS,&hwmc))
        PM_fatalError("Hardware motion compensation not supported!");

    /* Determine if the surfaces we want are supported */
    HwMCSurface = modeInfo.HwMCSurfaces;
    mcFlags = gaVideoMPEG2;
    if (use_idct != -1)
        mcFlags |= gaVideoIDCT;
    while (((*HwMCSurface)->VideoFormat & mcFlags) != mcFlags) {
        HwMCSurface++;
        if (*HwMCSurface == NULL)
            PM_fatalError("Unable to find hardware motion compensation surface!");
        }
    if ((*HwMCSurface)->VideoFormat & gaVideoIDCT)
        useIDCT = 1;
    if ((*HwMCSurface)->Flags & gaHwMC_IntraUnsigned)
        unsignedIntra = 1;

    /* Now create the motion compensation surfaces */
    width = horizontal_size;
    height = vertical_size;
    for (i = 0; i < numSurfaces; i++) {
        if ((surfaces[i] = hwmc.AllocSurface((*HwMCSurface)->VideoFormat,coded_picture_width,coded_picture_height,0)) == NULL) {
            if (i < 4) {
                PM_fatalError("Couldn't create enough HwMCSurfaces\n");
                }
            else {
                numSurfaces = i;
                printf("Could only allocate %d surfaces\n", numSurfaces);
                }
            }
        surfaceInfo[i].reference = 0;
        surfaceInfo[i].sequenceNumber = 0;
        }

    /* Indicate that we should display the video as soon as the first frame is ready */
    displayVideo = true;

    /* Allocate memory for the hardware macro blocks */
    slices = slices * mb_width;
    maxBlocks = slices * 6;
    blocks = malloc(maxBlocks * 130 * sizeof(short));
    maxMacroBlocks = slices;
    macroBlocks = malloc(maxMacroBlocks * sizeof(GA_HwMCMacroBlock));
    if (!blocks || !macroBlocks)
        PM_fatalError("Out of memory!");

    /* Init these - not sure what they are for yet */
    uiclp = uiclip+512;
    for (i= -512; i<512; i++)
        uiclp[i] = (i<-128) ? 0 : ((i>127) ? 255 : i+128);

    iclp = iclip+512;
    for (i= -512; i<512; i++)
        iclp[i] = (i<-128) ? -128 : ((i>127) ? 127 : i);

    niclp = niclip+512;
    for (i= -512; i<512; i++)
        niclp[i] = (i<-256) ? -256 : ((i>255) ? 255 : i);
}

/****************************************************************************
REMARKS:
Clean up and exit
****************************************************************************/
void exit_display()
{
#ifndef PLAY_UNDER_X
    PM_getch();
#endif
    FatalErrorCleanup();
}

/****************************************************************************
REMARKS:
???
****************************************************************************/
void assign_surfaces(void)
{
    int     i;
    int     oldest_nonref = -1;
    ulong   age;

    _block = blocks;
    _nextIndex = 0;
    _mb = macroBlocks;
    numBlocks = 0;

    for (age = ~0, i = 0; i < numSurfaces; i++) {
        if (!surfaceInfo[i].reference && (surfaceInfo[i].sequenceNumber < age)) {
            age = surfaceInfo[i].sequenceNumber;
            oldest_nonref = i;
            }
        }

    if (pict_type == B_TYPE) {
        nextDisplayed = target = surfaces[oldest_nonref];
        }
    else {
        int oldest_ref = -1;

        for (age = ~0, i = 0; i < numSurfaces; i++) {
            if (surfaceInfo[i].reference && (surfaceInfo[i].sequenceNumber < age)) {
                age = surfaceInfo[i].sequenceNumber;
                oldest_ref = i;
                }
            }

        if (oldest_ref != -1) {
            int newest_ref = -1;

            for (age = 0, i = 0; i < numSurfaces; i++) {
                if (surfaceInfo[i].reference && (surfaceInfo[i].sequenceNumber > age)) {
                    age = surfaceInfo[i].sequenceNumber;
                    newest_ref = i;
                    }
                }

            if (oldest_ref != newest_ref)
                surfaceInfo[oldest_ref].reference = 0;
            past = nextDisplayed = surfaces[newest_ref];
            }
        else {
            past = surfaces[oldest_nonref];
            }
        future = target = surfaces[oldest_nonref];
        surfaceInfo[oldest_nonref].reference = 1;
        }
    surfaceInfo[oldest_nonref].sequenceNumber = sequenceNumber++;
}

/****************************************************************************
REMARKS:
Displays the surface. If this is the first frame, enable the video overlay
surface, otherwise just flip to the new surface to display.
****************************************************************************/
void DisplaySurface(
    GA_buf *surface,
    int field)
{
    // TODO: What to do with the field parameter!?

    if (displayVideo) {
        video.SetVideoOutput(0,0,surface,0,0,horizontal_size, vertical_size,0,0,mode_xres,mode_yres,gaVideoXInterp | gaVideoYInterp);
        displayVideo = false;
        }
    else
        video.FlipVideoOutput(0,surface,false);
}

/****************************************************************************
REMARKS:
Display the frame on the video overlay surface
****************************************************************************/
int display_frame(
    int field)
{
    hwmc.FlushSurface(target);
    if (nextDisplayed) {
        hwmc.SyncSurface(nextDisplayed);
        DisplaySurface(nextDisplayed,field);
        lastDisplayed = nextDisplayed;
        }
#ifndef PLAY_UNDER_X
    if (PM_kbhit())
        return 0;
#endif
    return 1;
}

/****************************************************************************
REMARKS:
Display the field on the video overlay surface
****************************************************************************/
void display_field(
    int field)
{
    hwmc.FlushSurface(target);
    if (nextDisplayed) {
        hwmc.SyncSurface(nextDisplayed);
        DisplaySurface(nextDisplayed,field);
        }
}

/****************************************************************************
REMARKS:
Display the second field on the video overlay surface
****************************************************************************/
void display_second_field(
    int field)
{
    hwmc.FlushSurface(target);
    if (lastDisplayed) {
        DisplaySurface(lastDisplayed,field);
        }
}

/****************************************************************************
REMARKS:
Display the last frame in a sequence
****************************************************************************/
void putlast(
    int framenum)
{
    hwmc.FlushSurface(target);
    if (secondfield) {
        /* We could if we were bobbing */
        printf("last frame incomplete, not displayed\n");
        }
    else if (nextDisplayed) {
        hwmc.SyncSurface(nextDisplayed);
        DisplaySurface(nextDisplayed,gaHwMC_FramePicture);
        }
}

/****************************************************************************
REMARKS:
Add a macro block and render it in hardware
****************************************************************************/
void add_macroblock(
    int x,
    int y,
    int mb_type,
    int motion_type,
    int (*PMV)[2][2],
    int (*mv_field_sel)[2],
    int *dmvector,
    int cbp,
    int dct_type)
{
    int     skip = 0;

    _mb->X = x;
    _mb->Y = y;

    if (mb_type & MB_INTRA) {
        _mb->MacroBlockType = gaHwMC_TypeIntra;
        }
    else {
        /* SNAP HwMC doesn't support skips */
        if (!(mb_type & (MB_BACKWARD | MB_FORWARD))) {
            _mb->MacroBlockType = gaHwMC_TypeMotionForward;
            motion_type = (pict_struct == FRAME_PICTURE) ? MC_FRAME : MC_FIELD;
            _mb->PMV[0][0][0] = 0;
            _mb->PMV[0][0][1] = 0;
            skip = 1;
            }
        else {
            _mb->MacroBlockType = 0;
            if (mb_type & MB_FORWARD) {
                _mb->MacroBlockType = gaHwMC_TypeMotionForward;
                _mb->PMV[0][0][0] = PMV[0][0][0];
                _mb->PMV[0][0][1] = PMV[0][0][1];
                _mb->PMV[1][0][0] = PMV[1][0][0];
                _mb->PMV[1][0][1] = PMV[1][0][1];
                }
            if (mb_type & MB_BACKWARD) {
                _mb->MacroBlockType |= gaHwMC_TypeMotionBackward;
                _mb->PMV[0][1][0] = PMV[0][1][0];
                _mb->PMV[0][1][1] = PMV[0][1][1];
                _mb->PMV[1][1][0] = PMV[1][1][0];
                _mb->PMV[1][1][1] = PMV[1][1][1];
                }
            }
        if ((mb_type & MB_PATTERN) && cbp)
            _mb->MacroBlockType |= gaHwMC_TypePattern;

        _mb->MotionType = motion_type;

        if (motion_type == MC_DMV) {
            int DMV[2][2];

            if (pict_struct == FRAME_PICTURE) {
                calc_DMV(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);
                _mb->PMV[0][1][0] = PMV[0][0][0];
                _mb->PMV[0][1][1] = PMV[0][0][1];
                _mb->PMV[1][0][0] = DMV[0][0];
                _mb->PMV[1][0][1] = DMV[0][1] << 1;
                _mb->PMV[1][1][0] = DMV[1][0];
                _mb->PMV[1][1][1] = DMV[1][1] << 1;
                }
            else {
                calc_DMV(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]);
                _mb->PMV[0][1][0] = DMV[0][0];
                _mb->PMV[0][1][1] = DMV[0][1];
                }
            }

        if ((motion_type == MC_FIELD) || (motion_type == MC_16X8)) {
            if (skip) {
                _mb->MotionVerticalFieldSelect = (pict_struct == BOTTOM_FIELD) ? gaHwMC_SelectFirstForward : 0;
                }
            else {
                _mb->MotionVerticalFieldSelect = 0;
                if (mv_field_sel[0][0])
                    _mb->MotionVerticalFieldSelect |= gaHwMC_SelectFirstForward;
                if (mv_field_sel[0][1])
                    _mb->MotionVerticalFieldSelect |= gaHwMC_SelectFirstBackward;
                if (mv_field_sel[1][0])
                    _mb->MotionVerticalFieldSelect |= gaHwMC_SelectSecondForward;
                if (mv_field_sel[1][1])
                    _mb->MotionVerticalFieldSelect |= gaHwMC_SelectSecondBackward;
                }
            }
        }
    if (alt_block_format) {
        _mb->Index = ((ulong)_block - (ulong)blocks) / (130 * 2);
        if (ld->altscan)
            _mb->MacroBlockType |= gaHwMC_TypeAltZigZagOrder;
        }
    else
        _mb->Index = ((ulong)_block - (ulong)blocks) >> 7;
    _mb->DCTType = dct_type;
    _mb->CodedBlockPattern = cbp;

    _block += _nextIndex;
    _nextIndex = 0;
    numBlocks++;
    _mb++;

    if (numBlocks == slices) {
        GA_buf  *_past = NULL, *_future = NULL;
        ulong   flags;

        switch (pict_type) {
            case I_TYPE:
                break;
            case P_TYPE:
                if (!(_past = past))
                    _past = target;
                break;
            case B_TYPE:
                _past = past;
                _future = future;
                break;
            }

        flags = secondfield ? gaHwMC_SecondField : 0;
        if (alt_block_format)
            flags |= gaHwMC_AltBlockFormat;

        hwmc.RenderSurface(pict_struct, target, _past, _future,
            flags, slices, 0, macroBlocks, blocks);
        numBlocks = 0;
        _mb = macroBlocks;
        _block = blocks;
        }
}

/****************************************************************************
REMARKS:
Get a fresh macro block
****************************************************************************/
void get_block(void)
{
    ld->block = useIDCT ? (_block + _nextIndex) : preallocBlock;
    if (!alt_block_format)
        memset(ld->block, 0, 64 * sizeof(short));
}

/****************************************************************************
REMARKS:
Add a macro block
****************************************************************************/
void add_block(
    int intra)
{
    if (!useIDCT) {
        short * blk = ld->block;
        short * _blk = _block + _nextIndex;
        int     i;

        idct(blk);
        if (intra) {
            if (unsignedIntra) {
                for (i = 0; i < 64; i++)
                    _blk[i] = uiclp[blk[i]];
                }
            else {
                for (i = 0; i < 64; i++)
                    _blk[i] = iclp[blk[i]];
                }
            }
        else {
            for (i = 0; i < 64; i++)
                _blk[i] = niclp[blk[i]];
            }
        }
    if (alt_block_format)
        _nextIndex += 130;
    else
        _nextIndex += 64;
}

