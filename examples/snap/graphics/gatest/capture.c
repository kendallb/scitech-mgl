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
*               Graphics Architecture video overlay support.
*
****************************************************************************/

#include "gatest.h"

/*---------------------------- Global Variables ---------------------------*/

#define DEF_WIDTH   640
#define DEF_HEIGHT  480

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display an error message on the screen and wait for a key press
****************************************************************************/
static void displayError(
    char *msg)
{
    y += 16;
    gprintf(msg);
    y += 16;
    gprintf("Press any key to continue");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Handles key press for VSync and single-step in all video overlay tests
****************************************************************************/
static ibool handleKeypress(
    int *key,
    int *freezeVideo)
{
    if (EVT_kbhit()) {
        *key = EVT_getch();                /* Swallow keypress */
        if (*key == ' ')
            *freezeVideo ^= 1;
        else
            return true;
        }
    return false;
}

/****************************************************************************
REMARKS:
Decodes the capture input into displayable string
****************************************************************************/
static char* displayInput(
    GA_captureInputTypes input)
{
    switch (input) {
        case gaCaptureCompositeInput:
            return "Composite";
        case gaCaptureSVideoInput:
            return "S-Video";
        case gaCaptureRGBInput:
            return "Component RGB";
        case gaCaptureRS170Input:
            return "RS-170";
        }
    return "unknown input";
}

/****************************************************************************
REMARKS:
Decodes the YUV format into displayable string
****************************************************************************/
static char* displayFormat(
    GA_VideoFormatsType format)
{
    switch (format & gaVideoFormatMask) {
        case gaVideoYUV9:
            switch (format & gaVideoYUVLayoutMask) {
                case gaVideoYUV: return "YUV9 4:1:0 planar";
                case gaVideoYVU: return "YVU9 4:1:0 planar";
                }
            break;
        case gaVideoYUV12:
            switch (format & gaVideoYUVLayoutMask) {
                case gaVideoYUV: return "YUV12 4:2:0 planar";
                case gaVideoYVU: return "YVU12 4:2:0 planar";
                }
            break;
        case gaVideoYUV422:
            switch (format & gaVideoYUVLayoutMask) {
                case gaVideoYUYV: return "YUVU 4:2:2 packed";
                case gaVideoYVYU: return "YVYU 4:2:2 packed";
                case gaVideoUYVY: return "UYVY 4:2:2 packed";
                case gaVideoVYUY: return "VYUY 4:2:2 packed";
                }
            break;
        }
    return "unknown format";
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer static video capture test. The overlay will
show what is coming in via the video capture engine.
****************************************************************************/
static int staticTest(
    GA_videoFuncs *video,
    GA_videoCaptureFuncs *capture,
    GA_captureInputTypes input,
    GA_captureStandardTypes standard,
    GA_VideoCaptureInf *captureInfo)
{
    int             key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int             centerX, centerY;
    ibool           done = false,freezeInput = false,newFreezeInput = false;
    GA_captureBuf   *captureBuf = NULL;
    GA_buf          *primaryVideo = NULL;
    int             outputHead = init.GetActiveHead();
    char            str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video capture static test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Use larger overlay window for higher resolutions */
    if (modeInfo.XResolution >= 1280) {
        width *= 2;
        height *= 2;
        }
    else if (modeInfo.XResolution <= DEF_WIDTH) {
        width /= 2;
        height /= 2;
        }

    /* Allocate the source video buffer */
    if ((captureBuf = capture->AllocCaptureBuffer(width,height,captureInfo->VideoFormat,1)) == NULL) {
        displayError("Unable to allocate video capture buffer!");
        return 0x1B;
        }
    primaryVideo = captureBuf->VideoBuffers;
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    /* Set up the video capture and start it */
    capture->SetVideoInput(captureBuf,input,standard,0,false);
    capture->StartLiveVideo(captureBuf);

    /* Now loop around and display the video capture */
    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(primaryVideo->Format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(primaryVideo->Format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        if (handleKeypress(&key,&newFreezeInput))
            break;
        if (freezeInput != newFreezeInput) {
            freezeInput = newFreezeInput;
            if (freezeInput)
                capture->FreezeLiveVideo(captureBuf,gaCaptureFieldAny);
            else
                capture->StartLiveVideo(captureBuf);
            }
        }
    SetActivePage(0);
    video->DisableVideoOutput(0);
    capture->FreeCaptureBuffer(captureBuf);
    return key;
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doVideoCaptureTest(
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
    int                     i,choice;
    ibool                   success = true,all = true;
    GA_videoFuncs           video;
    GA_videoCaptureFuncs    capture;
    GA_VideoCaptureInf      captureInfo;
    GA_captureInputTypes    inputs[ ] = { gaCaptureCompositeInput,
                                          gaCaptureSVideoInput,
                                          gaCaptureRGBInput,
                                          gaCaptureRS170Input };
#define NUM_INPUTS (sizeof(inputs) / sizeof(inputs[0]))

    /* Obtain the mode information before setting the mode */
    dc = gc->dc;
    virtualX = virtualY = bytesPerLine = -1;
    modeInfo.dwSize = sizeof(modeInfo);
    if (xRes == -1) {
        if (init.GetVideoModeInfo(flags,&modeInfo) != 0)
            return false;
        }
    else {
        if (init.GetCustomVideoModeInfo(xRes,yRes,-1,-1,bitsPerPixel,&modeInfo) != 0)
            return false;
        }

    /* Check mode for video acceleration capabilities */
    if (!(modeInfo.Attributes & gaHaveAccelVideo)) {
        GC_printf(gc,"Video acceleration not available!\n\n");
        return false;
        }

    /* Check mode for video capture format list */
    captureInfo.dwSize = sizeof(captureInfo);
    if (!(modeInfo.VideoWindows)) {
        GC_printf(gc,"Video capture format list not available!\n\n");
        return false;
        }
    else
        memcpy(&captureInfo,*modeInfo.VideoCapture,captureInfo.dwSize);

    for (;;) {
        /* Display video format menu while in graphics console mode */
        GC_clrscr(gc);
        GC_printf(gc,"Video Capture Test\n\n");

        /* Display menu of available video overlay formats */
        GC_printf(gc,"Select Video Capture input to test:\n\n");
        for (i = 0; i < NUM_INPUTS; i++) {
            if ((captureInfo.InputType & inputs[i]) != inputs[i])
                continue;
            GC_printf(gc,"  [%i] - %s\n",i,displayInput(inputs[i]));
            }
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        for (;;) {
            choice = EVT_getch();
            if (tolower(choice) == 'q' || choice == 0x1B)
                return true;
            choice = (choice) - '0';
            if (choice >= 0 && choice < NUM_INPUTS) {
                if ((captureInfo.InputType & inputs[choice]) == inputs[choice])
                    break;
                }
            }

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
        if (!InitSoftwareRasterizer(cntDevice,1,false))
            PM_fatalError("Unable to initialise software rasteriser!");

        /* Get the hardware video overlay functions */
        video.dwSize = sizeof(video);
        if (!REF2D_queryFunctions(ref2d,GA_GET_VIDEOFUNCS,&video)) {
            displayError("Video functions not available!");
            success = false;
            goto DoneTests;
            }
        capture.dwSize = sizeof(capture);
        if (!REF2D_queryFunctions(ref2d,GA_GET_VIDEOCAPTUREFUNCS,&capture)) {
            displayError("Video capture functions not available!");
            success = false;
            goto DoneTests;
            }

        /* Run the tests for the selected capture input */
        i = choice;
        all = (stricmp(accelTestName,"all") == 0);
        if (all || stricmp(accelTestName,"capture") == 0)
            if (staticTest(&video,&capture,inputs[i],gaCaptureStandardNTSC,&captureInfo) == 0x1B)
                goto DoneTests;

DoneTests:
        /* Return to text mode, restore the state of the console and reloop */
        ExitSoftwareRasterizer();
        GC_restore(gc);
        if (!success)
            break;
        }

    (void)gl;
    (void)planeMask;
    (void)force1bppShadow;
    return success;
}

