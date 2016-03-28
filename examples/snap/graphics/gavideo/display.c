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
#include "config.h"
#include "global.h"
#include "snap/gasdk.h"

/*---------------------------- Global Variables ---------------------------*/

GA_initFuncs     init;
GA_driverFuncs   driver;
static GA_videoFuncs    video;
static PM_HWND          hwndConsole;
static void             *stateBuf;
static int              oldMode = -1;
static GA_buf *         videoBuf;
static GA_buf *         videoBackBuf;
static ibool            displayVideo;

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
    if (oldMode != -1) {
        virtualX = virtualY = bytesPerLine = -1;
        ExitSoftwareRasterizer();
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        }
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
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
    /* Check to see if we are running in a window */
    if (PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        exit(-1);
        }

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,640,480,8,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

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

    /* Get requested resolution, start graphics and draw pattern */
    if ((oldMode = InitGraphics(mode_xres,mode_yres,mode_bits)) == -1) {
        /* Restore the console */
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        AvailableModes();
        GA_unloadDriver(dc);
        exit(-1);
        }

    /* Get the video functions and check for YUV12 overlays */
    video.dwSize = sizeof(video);
    if (!REF2D_queryFunctions(ref2d,GA_GET_VIDEOFUNCS,&video))
        PM_fatalError("Hardware video overlays not supported!");
    if (!(modeInfo.VideoWindows[0]->VideoInputFormats & gaVideoYUV12))
        PM_fatalError("YUV12 video format not supported!");

    /* Now create the YUV video overlay buffer */
    if ((videoBuf = video.AllocVideoBuffers(coded_picture_width,coded_picture_height,gaVideoYUV12,2)) != NULL) {
        videoBackBuf = video.GetAttachedVideoBuffer(videoBuf,1);
        }
    else {
        videoBuf = video.AllocVideoBuffers(coded_picture_width,coded_picture_height,gaVideoYUV12,1);
        videoBackBuf = NULL;
        }
    if (!videoBuf)
        PM_fatalError("Unable to allocate offscreen video buffer!");

    /* Indicate that we should display the video as soon as the first frame is ready */
    displayVideo = true;
}

/****************************************************************************
REMARKS:
Clean up and exit
****************************************************************************/
void exit_display()
{
    PM_getch();
    FatalErrorCleanup();
}

extern int quit_flag;

/****************************************************************************
REMARKS:
Display the frame of video data
****************************************************************************/
void display_frame(
    unsigned char *src[])
{
    uchar   *s,*d;
    int     count,s1offset,s2offset,s3offset;
    int     d1offset,d2offset,d3offset;
    int     srcPitch,srcPitch23,dstPitch,dstPitch23;
    int     width,height;

    /* Swap the buffers if double buffering */
    if (videoBackBuf) {
        GA_buf *temp = videoBackBuf;
        videoBackBuf = videoBuf;
        videoBuf = temp;
        }

    /* Get video buffer buffer dimensions */
    bufmgr.LockBuffer(videoBuf);
    width = videoBuf->Width;
    height = videoBuf->Height;

    /* Compute pitches and offsets for source Y, U and V planes */
    srcPitch = (width + 3) & ~3;
    srcPitch23 = ((width / 2) + 3) & ~3;
    s1offset = 0;
    s2offset = srcPitch * height + s1offset;
    s3offset = (srcPitch23 * (height / 2)) + s2offset;

    /* Compute pitches and offsets for destination Y, U and V planes */
    dstPitch = videoBuf->HwStride;
    dstPitch23 = dstPitch / 2;
    d1offset = 0;
    d2offset = dstPitch * height + d1offset;
    d3offset = (dstPitch23 * (height / 2)) + d2offset;

    /* Copy Y plane data */
    count = height;
    s = src[0] + s1offset;
    d = (uchar*)videoBuf->Surface + d1offset;
    while (count--) {
        memcpy(d, s, width);
        s += srcPitch;
        d += dstPitch;
        }

    /* Adjust width/height for U/V planes */
    width /= 2;
    height /= 2;

    /* Copy U plane data */
    count = height;
    s = src[0] + s2offset;
    d = (uchar*)videoBuf->Surface + d2offset;
    while (count--) {
        memcpy(d, s, width);
        s += srcPitch23;
        d += dstPitch23;
        }

    /* Copy V plane data */
    count = height;
    s = src[0] + s3offset;
    d = (uchar*)videoBuf->Surface + d3offset;
    while (count--) {
        memcpy(d, s, width);
        s += srcPitch23;
        d += dstPitch23;
        }

    /* Unlock the video buffer after the copy */
    bufmgr.UnlockBuffer(videoBuf);

    /* Now display the video buffer */
    if (displayVideo) {
        /* Display the video overlay surface if this is the first time */
        video.SetVideoOutput(0,0,videoBuf,0,0,videoBuf->Width,videoBuf->Height,0,0,modeInfo.XResolution,modeInfo.YResolution,gaVideoXInterp | gaVideoYInterp);
        displayVideo = false;
        }
    else if (videoBackBuf) {
        /* Flip to the back buffer */
        video.FlipVideoOutput(0,videoBuf,false);
        }

    /* Check for ESC so you can exit early */
    if (PM_kbhit() && (PM_getch() == 0x1B)) {
        quit_flag = 1;    
        }
}

