/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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

#define DEF_WIDTH   360
#define DEF_HEIGHT  288

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
    int *waitVRT,
    int *singleStep)
{
    /* Handle single-step case by always waiting for key press */
    if (*singleStep) {
        *key = EVT_getch();                /* Swallow keypress */
        if (*key == 's' || *key == 'S')
            *singleStep ^= 1;
        else if (*key == 0x1B)
            return true;
        }
    /* Otherwise handle key only if pressed */
    else if (EVT_kbhit()) {
        *key = EVT_getch();                /* Swallow keypress */
        if (*key == 'v' || *key == 'V')
            *waitVRT ^= 1;
        else if (*key == 's' || *key == 'S')
            *singleStep ^= 1;
        else
            return true;
        }
    return false;
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
Does a simple overlay buffer static test
****************************************************************************/
static int staticTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay static test");  y += 16;
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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
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
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if (!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        }
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

#ifndef LITE
/****************************************************************************
REMARKS:
Does a simple page flipping test for the video overlay buffer
****************************************************************************/
static int videoFlipTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     i,j,istep,jstep,apage,vpage,fpsRate = 0,key = 0;
    ibool   done = false,waitVRT = true,singleStep = false;
    ulong   lastCount = 0,newCount;
    char    buf[80],buf2[80];
    GA_buf  *primaryVideo = NULL,*visibleBuf,*activeBuf;
    GA_buf  *videoBuffers[3];
    int     numBuffers = 3;
    int     outputHead = init.GetActiveHead();
    int     width = DEF_WIDTH, height = DEF_HEIGHT;
    int     centerX, centerY;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay flip test");  y += 16;
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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,numBuffers)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for double buffering the video overlay */
    videoBuffers[0] = video->GetAttachedVideoBuffer(primaryVideo,0);
    videoBuffers[1] = video->GetAttachedVideoBuffer(primaryVideo,1);
    videoBuffers[2] = video->GetAttachedVideoBuffer(primaryVideo,2);
    vpage = 0;
    apage = 1;
    visibleBuf = videoBuffers[vpage];
    activeBuf = videoBuffers[apage];
    SetActiveBuffer(visibleBuf);
    ClearVideoBuffer(visibleBuf);
    SetActiveBuffer(activeBuf);

    /* Set the video output window */
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            centerX,centerY,
            primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    i = 0;
    j = maxY;
    istep = 2;
    jstep = -2;
    LZTimerOn();
    while (!done) {
        activeBuf = videoBuffers[apage];
        SetActiveBuffer(activeBuf);
        ClearVideoBuffer(activeBuf);
        gmoveto(4,4);
        gprintf("%3d.%d fps", fpsRate / 10, fpsRate % 10);
        sprintf(buf,"%d x %d %d bit %s overlay",activeBuf->Width,activeBuf->Height,
            activeBuf->BitsPerPixel,displayFormat(format));
        if (waitVRT) {
            strcpy(buf2,"Double buffering (wait sync)");
            }
        else {
            strcpy(buf2,"Double buffering (no wait)");
            }
        if (maxX <= 360) {
            if (width < DEF_WIDTH)
                WriteText(8,60,displayFormat(format),defcolor);
            else
                WriteText(8,60,buf,defcolor);
            if (width < DEF_WIDTH)
                WriteText(8,80,&buf2[17],defcolor);
            else
                WriteText(8,80,buf2,defcolor);
            sprintf(buf,"Page %d of %d", (int)apage+1, (int)numBuffers);
            WriteText(8,116,buf,defcolor);
            }
        else {
            WriteText(80,60,buf,defcolor);
            WriteText(80,80,buf2,defcolor);
            sprintf(buf,"Page %d of %d", (int)apage+1, (int)numBuffers);
            WriteText(80,116,buf,defcolor);
            }
        SetForeColor(defcolor);
        draw2d.DrawLineInt(i,0,maxX-i,maxY,true);
        draw2d.DrawLineInt(0,maxY-j,maxX,j,true);
        SetForeColor(defcolor);
        draw2d.DrawLineInt(0,0,maxX,0,true);
        draw2d.DrawLineInt(0,0,0,maxY,true);
        draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
        draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        vpage = (vpage + 1) % numBuffers;
        visibleBuf = videoBuffers[vpage];
        video->FlipVideoOutput(0,visibleBuf,waitVRT);
        apage = (apage + 1) % numBuffers;
        i += istep;
        if (i > maxX) {
            i = maxX-2;
            istep = -2;
            }
        if (i < 0)
            i = istep = 2;
        j += jstep;
        if (j > maxY) {
            j = maxY-2;
            jstep = -2;
            }
        if (j < 0)
            j = jstep = 2;

        /* Compute the frames per second rate after going through an entire
         * set of display pages.
         */
        if (apage == 0) {
            newCount = LZTimerLap();
            fpsRate = (int)(10000000L / (newCount - lastCount)) * numBuffers;
            lastCount = newCount;
            }
        }
    LZTimerOff();
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does the overlay buffer test for increasing source overlay size
****************************************************************************/
static int videoSourceTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key=0,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     srcTop = 0, srcLeft = 0;
    ibool   loop = true, done = false, waitVRT = true, singleStep = false;
    ibool   srcClipX = false, srcClipY = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    int     centerX, centerY;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay source image test");      y += 8;
    displaymodeInfo();                               y += 8;
    gprintf("Keys:  '<' height-1   '>' height+1");   y += 8;
    gprintf("       ',' width-1    '.' width+1");    y += 8;
    gprintf("       '-' -10 both   '+' +10 both");   y += 8;
    gprintf("       'x' max width  'y' max height"); y += 8;
    gprintf("       'l' toggle letterbox clipping"); y += 8;
    gprintf("       'w' toggle width clipping");     y += 8;
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Use larger overlay window for higher resolutions */
    if (modeInfo.XResolution >= 1280)
        width *= 2, height *= 2;
    if (modeInfo.XResolution <= DEF_WIDTH)
        width /= 2, height /= 2;

    while (!done) {
        if (height < 1) height = 1;
        if (width  < 1) width  = 1;

        /* Allocate the source video buffer */
        if ((primaryVideo = video->AllocVideoBuffers(width+(srcLeft*2),height+(srcTop*2),format,1)) == NULL) {
            width /= 2;
            height /= 2;
            if ((primaryVideo = video->AllocVideoBuffers(width+(srcLeft*2),height+(srcTop*2),format,1)) == NULL) {
                displayError("Unable to allocate primary video buffer!");
                return true;
                }
            }

        /* Centering based on background max XY */
        centerX = (maxX-width)/2;
        centerY = (maxY-height)/2;

        /* Set up for single buffer video overlay */
        SetActiveBuffer(primaryVideo);
        ClearVideoBuffer(primaryVideo);
        LoadVideoBuffer(primaryVideo);

        /* Set the video output window */
        if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            srcLeft,srcTop,width,height,centerX,centerY,width,height,0)) {
            SetActivePage(0);
            displayError("Unable to set video output window!");
            return false;
            }

        if (width > 200 && height > 80 ) {
            gmoveto(8,60);
            gprintf("%s %dx%d",displayFormat(format), width, height);
            }

        loop = true;

        while (loop) {
            if (waitVRT)
                driver.WaitVSync();

            if (handleKeypress(&key,&waitVRT,&singleStep)) {
                loop = false;
                switch (key) {
                    case '<':
                        height -= 1;
                        break;
                    case '>':
                        height += 1;
                        break;
                    case ',':
                        width -= 1;
                        break;
                    case '.':
                        width += 1;
                        break;
                    case '=':
                    case '+':
                        height += 10, width += 10;
                        break;
                    case '-':
                    case '_':
                        height -= 10, width -= 10;
                        break;
                    case 'x':
                    case 'X':
                        width = videoInfo->VideoMaxXSource;
                        break;
                    case 'y':
                    case 'Y':
                        height = videoInfo->VideoMaxYSource;
                        break;
                    case 'l':
                    case 'L':
                        if (!srcClipY)
                            srcTop = 50, centerY += 50, height -= 100, srcClipY = true;
                        else
                            srcTop = 0,  centerY -= 50, height += 100, srcClipY = false;
                        video->MoveVideoOutput(0,srcLeft,srcTop,width,height,centerX,centerY,width,height,waitVRT);
                        loop = true;
                        break;
                    case 'w':
                    case 'W':
                        if (!srcClipX)
                            srcLeft = 50, centerX += 50, width -= 100, srcClipX = true;
                        else
                            srcLeft = 0,  centerX -= 50, width += 100, srcClipX = false;
                        video->MoveVideoOutput(0,srcLeft,srcTop,width,height,centerX,centerY,width,height,waitVRT);
                        loop = true;
                        break;
                    default:
                        done = true;
                        break;
                    }
                }
            }

        SetActivePage(0);
        video->DisableVideoOutput(0);
        video->FreeVideoBuffers(primaryVideo);
        }
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer move test
****************************************************************************/
static int moveTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     key,x,y,incx,incy,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay move test");  y += 16;
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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    incx = 1;
    incy = -1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Move the video window to new location */
        x += incx;
        y += incy;
        if ((x >= (modeInfo.XResolution - width)) || (x <= 0))
            incx = -incx;
        if ((y >= (modeInfo.YResolution - height)) || (y <= 0))
            incy = -incy;
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->MoveVideoOutput(0,0,0,primaryVideo->Width,primaryVideo->Height,x,y,width,height,waitVRT);
        }
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}
#endif

/****************************************************************************
REMARKS:
Does a simple overlay buffer scale test
****************************************************************************/
static int scaleTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     key,x,y,incx,incy,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    int     incstretchx,incstretchy;
    int     smallWidth = 4,smallHeight = 4;
    int     bigWidth = modeInfo.XResolution - 50,bigHeight = modeInfo.YResolution - 50;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay scale test");  y += 16;
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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    incx = 1;
    incy = -1;
    incstretchx = 1;
    incstretchy = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,
            gaVideoXInterp | gaVideoYInterp)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Stretch the video window */
        width += incstretchx;
        height += incstretchy;
        if ((width >= bigWidth) || (width <= smallWidth))
            incstretchx = -incstretchx;
        if ((height >= bigHeight) || (height <= smallHeight))
            incstretchy = -incstretchy;

        /* Move the video window to new location */
        x += incx;
        y += incy;
        if (x > (modeInfo.XResolution - width - 2))
            incx = -1;
        if (x < 2)
            incx = 1;
        if (y > (modeInfo.YResolution - height - 3))
            incy = -1;
        if (y < 3)
            incy = 1;
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->MoveVideoOutput(0,0,0,primaryVideo->Width,primaryVideo->Height,x,y,width,height,waitVRT);
        }
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

#ifndef LITE
/****************************************************************************
REMARKS:
Does a simple overlay buffer clip test at edges and corners
****************************************************************************/
static int clipTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     key,x,y,x0,y0,incx,incy,width = DEF_WIDTH,height = DEF_HEIGHT;
    ibool   done = false,waitVRT = true,singleStep = false,xorOutline = true;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay clip test");  y += 16;
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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    incx = 1;
    incy = 0;
    width = primaryVideo->Width;
    height = primaryVideo->Height;
    x = -width/4;
    y = -height/4;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);

    /* Draw 1-pixel XOR border around video window to see clipping */
    if (xorOutline) {
        SetActiveBuffer(primaryBuf);
        SetMix(GA_XOR_MIX);
        SetForeColor(maxcolor);
        draw2d.DrawClippedLineInt(x-1,y-1,x+width+0,y-1,true,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x-1,y-0,x-1,y+height+0,false,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x+width+0,y-0,x+width+0,y+height+0,false,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x-1,y+height+0,x+width+0,y+height+0,true,0,0,maxX,maxY);
        x0 = x;
        y0 = y;
        }
    while (!done) {
        /* Update video window coordinates to new location */
        x += incx;
        y += incy;
        if (x > (modeInfo.XResolution - width*3/4)) {
            incx = 0;
            incy = 1;
            x--;
            }
        else if (y > (modeInfo.YResolution - height*3/4)) {
            incx = -1;
            incy = 0;
            y--;
            }
        else if (x < -width/4) {
            incx = 0;
            incy = -1;
            x++;
            }
        else if (y < -height/4) {
            incx = 1;
            incy = 0;
            y++;
            }
        /* Move the video window to new location */
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->MoveVideoOutput(0,0,0,primaryVideo->Width,primaryVideo->Height,x,y,width,height,waitVRT);
        /* Redraw 1-pixel XOR border around video window to remove outline */
        if (xorOutline) {
            draw2d.DrawClippedLineInt(x0-1,y0-1,x0+width+0,y0-1,true,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0-1,y0-0,x0-1,y0+height+0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0+width+0,y0-0,x0+width+0,y0+height+0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0-1,y0+height+0,x0+width+0,y0+height+0,true,0,0,maxX,maxY);
            }
        if (key == 'x' || key == 'X')
            xorOutline ^= 1;
        /* Redraw 1-pixel XOR border around video window to update outline */
        if (xorOutline) {
            draw2d.DrawClippedLineInt(x-1,y-1,x+width+0,y-1,true,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x-1,y-0,x-1,y+height+0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x+width+0,y-0,x+width+0,y+height+0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x-1,y+height+0,x+width+0,y+height+0,true,0,0,maxX,maxY);
            x0 = x;
            y0 = y;
            }
        }
    SetMix(GA_REPLACE_MIX);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer clip test at edges with scaling
****************************************************************************/
static int clipScaleTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     key,x,y,x0,y0,width = DEF_WIDTH,height = DEF_HEIGHT,w0,h0;
    int     incstretchx,incstretchy;
    int     smallWidth = DEF_WIDTH,smallHeight = DEF_HEIGHT;
    int     bigWidth = 2*DEF_WIDTH,bigHeight = 2*DEF_HEIGHT;
    ibool   done = false,waitVRT = true,singleStep = false,xorOutline = true;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay clip test with scaling");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Use larger overlay window for higher resolutions */
    if (modeInfo.XResolution >= 1280) {
        width *= 2;
        height *= 2;
        smallWidth *= 2;
        smallHeight *= 2;
        bigWidth *= 2;
        bigHeight *= 2;
        }
    else if (modeInfo.XResolution <= DEF_WIDTH) {
        width /= 2;
        height /= 2;
        smallWidth /= 2;
        smallHeight /= 2;
        bigWidth /= 2;
        bigHeight /= 2;
        }

    /* Allocate the source video buffer */
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    x = 0;
    y = 0;
    width = primaryVideo->Width;
    height = primaryVideo->Height;
    incstretchx = 1;
    incstretchy = 0;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,
            gaVideoXInterp | gaVideoYInterp)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);

    /* Draw 1-pixel XOR border around video window to see clipping */
    if (xorOutline) {
        SetActiveBuffer(primaryBuf);
        SetMix(GA_XOR_MIX);
        SetForeColor(maxcolor);
        draw2d.DrawClippedLineInt(x-1,y-1,x+width+0,y-1,true,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x-1,y-0,x-1,y+height+0,false,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x+width+0,y-0,x+width+0,y+height+0,false,0,0,maxX,maxY);
        draw2d.DrawClippedLineInt(x-1,y+height+0,x+width+0,y+height+0,true,0,0,maxX,maxY);
        x0 = x;
        y0 = y;
        w0 = width;
        h0 = height;
        }
    while (!done) {
        /* Stretch the video window */
        width += incstretchx;
        height += incstretchy;
        if (width >= bigWidth) {
            incstretchx = -1;
            }
        else if (width < smallWidth) {
            incstretchx = 0;
            incstretchy = 1;
            width = smallWidth;
            }
        if (height >= bigHeight) {
            incstretchy = -1;
            }
        else if (height < smallHeight) {
            incstretchy = 0;
            incstretchx = 1;
            height = smallHeight;
            }
        /* Update video window coordinates for new size */
        x = primaryVideo->Width - width;
        y = primaryVideo->Height - height;
        /* Move the video window to new location */
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->MoveVideoOutput(0,0,0,primaryVideo->Width,primaryVideo->Height,x,y,width,height,waitVRT);
        /* Redraw 1-pixel XOR border around video window to remove outline */
        if (xorOutline) {
            draw2d.DrawClippedLineInt(x0-1,y0-1,x0+w0,y0-1,true,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0-1,y0-0,x0-1,y0+h0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0+w0,y0-0,x0+w0,y0+h0,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x0-1,y0+h0,x0+w0,y0+h0,true,0,0,maxX,maxY);
            }
        if (key == 'x' || key == 'X')
            xorOutline ^= 1;
        /* Redraw 1-pixel XOR border around video window to update outline */
        if (xorOutline) {
            draw2d.DrawClippedLineInt(x-1,y-1,x+width,y-1,true,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x-1,y-0,x-1,y+height,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x+width,y-0,x+width,y+height,false,0,0,maxX,maxY);
            draw2d.DrawClippedLineInt(x-1,y+height,x+width,y+height,true,0,0,maxX,maxY);
            x0 = x;
            y0 = y;
            w0 = width;
            h0 = height;
            }
        }
    SetMix(GA_REPLACE_MIX);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}
#endif

/****************************************************************************
REMARKS:
Does a simple overlay buffer brightness value test
****************************************************************************/
static int brightnessTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];
    N_int32 brightness,minval,maxval,defval,incval;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay brightness test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video parameter adjustment supported */
    if (video->SetVideoBrightness == NULL) {
        displayError("Video driver has no SetVideoBrightness support");
        return 0;
        }

    /* Get min, max, and default video parameter values */
    minval = videoInfo->VideoBrightnessMin;
    maxval = videoInfo->VideoBrightnessMax;
    defval = videoInfo->VideoBrightnessDefault;

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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    brightness = defval;
    incval = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Adjust video brightness throughout reported range */
        brightness += incval;
        if (brightness > maxval) {
            brightness = maxval;
            incval = -1;
            }
        else if (brightness < minval) {
            brightness = minval;
            incval = 1;
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->SetVideoBrightness(0,brightness);
        if (waitVRT)
            driver.WaitVSync();
        }
    video->SetVideoBrightness(0,defval);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer contrast value test
****************************************************************************/
static int contrastTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];
    N_int32 contrast,minval,maxval,defval,incval;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay contrast test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video parameter adjustment supported */
    if (video->SetVideoContrast == NULL) {
        displayError("Video driver has no SetVideoContrast support");
        return 0;
        }

    /* Get min, max, and default video parameter values */
    minval = videoInfo->VideoContrastMin;
    maxval = videoInfo->VideoContrastMax;
    defval = videoInfo->VideoContrastDefault;

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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    contrast = defval;
    incval = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Adjust video contrast throughout reported range */
        contrast += incval;
        if (contrast > maxval) {
            contrast = maxval;
            incval = -1;
            }
        else if (contrast < minval) {
            contrast = minval;
            incval = 1;
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->SetVideoContrast(0,contrast);
        if (waitVRT)
            driver.WaitVSync();
        }
    video->SetVideoContrast(0,defval);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer saturation value test
****************************************************************************/
static int saturationTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];
    N_int32 saturation,minval,maxval,defval,incval;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay saturation test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video parameter adjustment supported */
    if (video->SetVideoSaturation == NULL) {
        displayError("Video driver has no SetVideoSaturation support");
        return 0;
        }

    /* Get min, max, and default video parameter values */
    minval = videoInfo->VideoSaturationMin;
    maxval = videoInfo->VideoSaturationMax;
    defval = videoInfo->VideoSaturationDefault;

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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    saturation = defval;
    incval = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Adjust video saturation throughout reported range */
        saturation += incval;
        if (saturation > maxval) {
            saturation = maxval;
            incval = -1;
            }
        else if (saturation < minval) {
            saturation = minval;
            incval = 1;
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->SetVideoSaturation(0,saturation);
        if (waitVRT)
            driver.WaitVSync();
        }
    video->SetVideoSaturation(0,defval);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer hue value test
****************************************************************************/
static int hueTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];
    N_int32 hue,minval,maxval,defval,incval;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay hue test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video parameter adjustment supported */
    if (video->SetVideoHue == NULL) {
        displayError("Video driver has no SetVideoHue support");
        return 0;
        }

    /* Get min, max, and default video parameter values */
    minval = videoInfo->VideoHueMin;
    maxval = videoInfo->VideoHueMax;
    defval = videoInfo->VideoHueDefault;

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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    hue = defval;
    incval = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Adjust video hue throughout reported range */
        hue += incval;
        if (hue > maxval) {
            hue = maxval;
            incval = -1;
            }
        else if (hue < minval) {
            hue = minval;
            incval = 1;
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->SetVideoHue(0,hue);
        if (waitVRT)
            driver.WaitVSync();
        }
    video->SetVideoHue(0,defval);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer gamma value test
****************************************************************************/
static int gammaTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int     key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    GA_buf  *primaryVideo = NULL;
    int     outputHead = init.GetActiveHead();
    char    str[80];
    N_int32 gamma,minval,maxval,defval,incval;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay gamma test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video parameter adjustment supported */
    if (video->SetVideoGammaCorrect == NULL) {
        displayError("Video driver has no SetVideoGammaCorrect support");
        return 0;
        }
    /* Get min, max, and default video parameter values */
    minval = 0;
    maxval = 255;   /* gamma range = 0.01 to 2.55 */
    defval = 0;     /* no gamma correction */

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
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window */
    gamma = defval;
    incval = 1;
    x = centerX;
    y = centerY;
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);
    while (!done) {
        /* Adjust video gamma throughout reported range */
        gamma += incval;
        if (gamma > maxval) {
            gamma = maxval;
            incval = -1;
            }
        else if (gamma < minval) {
            gamma = minval;
            incval = 1;
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->SetVideoGammaCorrect(0,gamma);
        if (waitVRT)
            driver.WaitVSync();
        }
    video->SetVideoGammaCorrect(0,defval);
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

/****************************************************************************
REMARKS:
Does a simple overlay buffer destination colorkey value test
****************************************************************************/
static int dstColorKeyTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format,
    GA_videoInf *videoInfo)
{
    int         key,x,y,width = DEF_WIDTH,height = DEF_HEIGHT;
    int         centerX, centerY;
    int         incx,incy;
    ibool       done = false,waitVRT = true,singleStep = false;
    GA_buf      *primaryVideo = NULL;
    int         outputHead = init.GetActiveHead();
    char        str[80];
    GA_color    colorkey = (modeInfo.BitsPerPixel <= 8) ? GC_MAGENTA : rgbColor(0x7F,0x00,0x7F);

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);

    /* Use larger overlay window for higher resolutions */
    if (modeInfo.XResolution >= 1280) {
        width *= 2;
        height *= 2;
        }
    else if (modeInfo.XResolution <= DEF_WIDTH) {
        width /= 2;
        height /= 2;
        }

    /* Draw black hole in background with destination color key value */
    SetForeColor(colorkey);
    x = (maxX-width)/2;
    y = (maxY-height)/2;
    draw2d.DrawRect(x,y,width,height);

    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay destination colorkey test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Check if video colorkey supported */
    if ((videoInfo->VideoOutputFlags & gaVideoColorKeyDstSingle) == 0) {
        displayError("Video driver has no destination colorkey capability");
        return 0;
        }

    if (video->SetDstVideoColorKey == NULL) {
        displayError("Video driver has no SetDstVideoColorKey support");
        return 0;
        }

    /* Allocate the source video buffer */
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,1)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for single buffer video overlay */
    SetActiveBuffer(primaryVideo);
    ClearVideoBuffer(primaryVideo);

    /* Set the video output window with destination colorkey */
    incx = 1;
    incy = -1;
    x = centerX;
    y = centerY;
    video->SetDstVideoColorKey(0,colorkey,colorkey);
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            x,y,primaryVideo->Width,primaryVideo->Height,
            gaVideoColorKeyDstSingle)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    if(!LoadVideoBuffer(primaryVideo)) {
        displayError("Unable to load overlay bitmaps!");
        return 0x1B;
        }

    sprintf(str,"%d x %d %d bit %s overlay",primaryVideo->Width,primaryVideo->Height,
        primaryVideo->BitsPerPixel,displayFormat(format));
    if (width < DEF_WIDTH)
        WriteText(8,60,displayFormat(format),defcolor);
    else
        WriteText(8,80,str,defcolor);

    while (!done) {
        /* Move the video window to new location over background destination colorkey */
        x += incx;
        y += incy;
        if ((x >= (modeInfo.XResolution - width)) || (x <= 0))
            incx = -incx;
        if ((y >= (modeInfo.YResolution - height)) || (y <= 0))
            incy = -incy;
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        video->MoveVideoOutput(0,0,0,primaryVideo->Width,primaryVideo->Height,x,y,width,height,waitVRT);
        }
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    return key;
}

#ifndef LITE
/****************************************************************************
REMARKS:
Performs streaming video playback into video overlay buffer
****************************************************************************/
static int streamTest(
    GA_videoFuncs *video,
    GA_VideoFormatsType format)
{
    int     apage,vpage,fpsRate = 0,key = 0;
    int     centerX, centerY;
    ibool   done = false,waitVRT = true,singleStep = false;
    ulong   lastCount = 0,newCount;
    char    buf[80],buf2[80];
    GA_buf  *primaryVideo = NULL,*visibleBuf,*activeBuf;
    GA_buf  *videoBuffers[4];
    int     numBuffers = 4;
    int     outputHead = init.GetActiveHead();
    int     i,width,height,blocksize,headersize,markersize;
    FILE    *stream;
    N_uint8 *data,*d,*s;

    /* Draw background for video overlay */
    SetActivePage(0);
    ClearPage(0);
    moire(defcolor);
    if (maxX >= 479)
        gmoveto(80,80);
    else
        gmoveto(8,40);
    gprintf("Video overlay stream test");  y += 16;
    displaymodeInfo();
    gprintf("Press any key to continue");
    if (EVT_getch() == 0x1B)
        return 0x1B;

    /* Raw video stream is only available in YUV12 planar format */
    if ((format & gaVideoYUV12) != gaVideoYUV12) {
        displayError("Stream test is only available for YUV12 format...");
        return 0;
        }

    /* Attempt to open MPlayer YUV video stream file */
    if ((stream = fopen("stream.yuv","rb")) == NULL) {
        displayError("Unable to load video stream file 'stream.yuv'...");
        return 0;
        }

    /* Read ASCII header until line feed */
    i = 0;
    while (!feof(stream)) {
        if (fread(&buf[i],1,1,stream) == 0)
            break;
        if (buf[i] == '\n')
            break;
        i++;
        }
    headersize = i+1;

    /* Verify MPlayer YUV file signature */
    if (strncmp(buf,"YUV4MPEG2",sizeof("YUV4MPEG2")-1) != 0) {
        displayError("Invalid YUV stream file...");
        return 0;
        }
    sscanf(buf,"YUV4MPEG2 W%d H%d", &width, &height);
    blocksize = width * height * 3 / 2;
    data = malloc(blocksize);

    /* Skip past first frame = MPlayer dead frame */
    markersize = sizeof("FRAME\n") - 1;
    fseek(stream,markersize+blocksize,SEEK_CUR);

    /* Allocate the source video buffer */
    if ((primaryVideo = video->AllocVideoBuffers(width,height,format,numBuffers)) == NULL) {
        displayError("Unable to allocate primary video buffer!");
        return 0x1B;
        }
    centerX = (maxX - primaryVideo->Width)/2;
    centerY = (maxY - primaryVideo->Height)/2;

    /* Set up for double buffering the video overlay */
    videoBuffers[0] = video->GetAttachedVideoBuffer(primaryVideo,0);
    videoBuffers[1] = video->GetAttachedVideoBuffer(primaryVideo,1);
    videoBuffers[2] = video->GetAttachedVideoBuffer(primaryVideo,2);
    videoBuffers[3] = video->GetAttachedVideoBuffer(primaryVideo,3);
    vpage = 0;
    apage = 1;
    visibleBuf = videoBuffers[vpage];
    activeBuf = videoBuffers[apage];
    SetActiveBuffer(visibleBuf);
    ClearVideoBuffer(visibleBuf);
    SetActiveBuffer(activeBuf);

    /* Set the video output window */
    if (!video->SetVideoOutput(0,outputHead,primaryVideo,
            0,0,primaryVideo->Width,primaryVideo->Height,
            centerX,centerY,
            primaryVideo->Width,primaryVideo->Height,0)) {
        SetActivePage(0);
        displayError("Unable to set video output window!");
        return 0x1B;
        }

    LZTimerOn();
    while (!done) {
        activeBuf = videoBuffers[apage];
        SetActiveBuffer(activeBuf);

        /* Reloop past first frame if end of file */
        if (feof(stream))
            fseek(stream,headersize+markersize+blocksize,SEEK_SET);
        /* Read binary block past "FRAME" marker */
        fread(data,markersize,1,stream);
        fread(data,blocksize,1,stream);

        /* Copy binary block to Y buffer region */
        bufmgr.LockBuffer(activeBuf);
        s = data;
        d = (N_uint8*)activeBuf->Surface;
        for (i = 0; i < height; i++) {
            memcpy(d,s,width);
            s += width;
            d += activeBuf->Stride;
            }
        /* Copy remaining binary block to 1/2 size U and V buffer regions */
        if (format & gaVideoYUV)
            d = (N_uint8*)activeBuf->Surface + activeBuf->Stride * activeBuf->Height;
        else
            d = (N_uint8*)activeBuf->Surface + activeBuf->Stride * activeBuf->Height * 5 / 4;
        for (i = 0; i < height/2; i++) {
            memcpy(d,s,width/2);
            s += width/2;
            d += activeBuf->Stride/2;
            }
        if (format & gaVideoYUV)
            d = (N_uint8*)activeBuf->Surface + activeBuf->Stride * activeBuf->Height * 5 / 4;
        else
            d = (N_uint8*)activeBuf->Surface + activeBuf->Stride * activeBuf->Height;
        for (i = 0; i < height/2; i++) {
            memcpy(d,s,width/2);
            s += width/2;
            d += activeBuf->Stride/2;
            }
        bufmgr.UnlockBuffer(activeBuf);

        /* Add text to current video buffer */
        gmoveto(4,4);
        gprintf("%3d.%d fps", fpsRate / 10, fpsRate % 10);
        sprintf(buf,"%d x %d %d bit %s overlay",activeBuf->Width,activeBuf->Height,
            activeBuf->BitsPerPixel,displayFormat(format));
        if (waitVRT) {
            strcpy(buf2,"Double buffering (wait sync)");
            }
        else {
            strcpy(buf2,"Double buffering (no wait)");
            }
        if (maxX <= 360) {
            WriteText(8,60,buf,defcolor);
            WriteText(8,80,buf2,defcolor);
            sprintf(buf,"Page %d of %d", (int)apage+1, (int)numBuffers);
            WriteText(8,116,buf,defcolor);
            }
        else {
            WriteText(80,60,buf,defcolor);
            WriteText(80,80,buf2,defcolor);
            sprintf(buf,"Page %d of %d", (int)apage+1, (int)numBuffers);
            WriteText(80,116,buf,defcolor);
            }
        if (handleKeypress(&key,&waitVRT,&singleStep))
            break;
        vpage = (vpage + 1) % numBuffers;
        visibleBuf = videoBuffers[vpage];
        video->FlipVideoOutput(0,visibleBuf,waitVRT);
        apage = (apage + 1) % numBuffers;

        /* Compute the frames per second rate after going through an entire
         * set of display pages.
         */
        if (apage == 0) {
            newCount = LZTimerLap();
            fpsRate = (int)(10000000L / (newCount - lastCount)) * numBuffers;
            lastCount = newCount;
            }
        }
    LZTimerOff();
    SetActivePage(0);
    video->DisableVideoOutput(0);
    video->FreeVideoBuffers(primaryVideo);
    free(data);
    fclose(stream);
    return key;
}
#endif

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doVideoOverlayTest(
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
    int                 i,choice;
    ibool               success = true,all = true;
    GA_videoFuncs       video;
    GA_videoInf         videoInfo;
    GA_VideoFormatsType formats[ ] = {  gaVideoYUV9|gaVideoYUV,
                                        gaVideoYUV9|gaVideoYVU,
                                        gaVideoYUV12|gaVideoYUV,
                                        gaVideoYUV12|gaVideoYVU,
                                        gaVideoYUV422|gaVideoYUYV,
                                        gaVideoYUV422|gaVideoYVYU,
                                        gaVideoYUV422|gaVideoUYVY,
                                        gaVideoYUV422|gaVideoVYUY };

#define NUM_FORMATS     (sizeof(formats) / sizeof(formats[0]))

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

    /* Check mode for video format list */
    videoInfo.dwSize = sizeof(videoInfo);
    if (!(modeInfo.VideoWindows)) {
        GC_printf(gc,"Video format list not available!\n\n");
        return false;
        }
    else {
        memcpy(&videoInfo,*modeInfo.VideoWindows,videoInfo.dwSize);
        }

    for (;;) {

        /* Display video format menu while in graphics console mode */
        GC_clrscr(gc);
        GC_printf(gc,"Video Overlay Test\n\n");

        /* Display menu of available video overlay formats */
        GC_printf(gc,"Select Video Overlay format to test:\n\n");
        for (i = 0; i < NUM_FORMATS; i++) {
            if ((videoInfo.VideoInputFormats & formats[i]) != formats[i])
                continue;
            GC_printf(gc,"  [%i] - %s\n",i,displayFormat(formats[i]));
            }
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        for (;;) {
            choice = EVT_getch();
            if (tolower(choice) == 'q' || choice == 0x1B)
                return true;
            choice = (choice) - '0';
            if (choice >= 0 && choice < NUM_FORMATS)
                if ((videoInfo.VideoInputFormats & formats[choice]) == formats[choice])
                    break;
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

        /* Run the tests for the selected video format */
        i = choice;
        all = (stricmp(accelTestName,"all") == 0);
        if (all || stricmp(accelTestName,"video") == 0)
            if (staticTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"brightness") == 0)
            if (brightnessTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"contrast") == 0)
            if (contrastTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"saturation") == 0)
            if (saturationTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"hue") == 0)
            if (hueTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"gamma") == 0)
            if (gammaTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"colorkey") == 0)
            if (dstColorKeyTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"scale") == 0)
            if (scaleTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
#ifndef LITE
        if (all || stricmp(accelTestName,"move") == 0)
            if (moveTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"clip") == 0)
            if (clipTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"clipscale") == 0)
            if (clipScaleTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"flip") == 0)
            if (videoFlipTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"srcimg") == 0)
            if (videoSourceTest(&video,formats[i],&videoInfo) == 0x1B)
                goto DoneTests;
        if (all || stricmp(accelTestName,"stream") == 0)
            if (streamTest(&video,formats[i]) == 0x1B)
                goto DoneTests;
#endif

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

