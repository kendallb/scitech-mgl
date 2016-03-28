/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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

#include "gatest.h"

/*---------------------------- Global Variables ---------------------------*/

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
us  - Number of microseconds to delay for

REMARKS:
Delays for the specified number of microseconds. We simply use the Zen Timer
routines to do this for us, since the delay() function is not normally
supported across all compilers.
****************************************************************************/
void us_delay(
    long us)
{
    LZTimerOn();
    while (LZTimerLap() < us)
        ;
    LZTimerOff();
}

/****************************************************************************
REMARKS:
Display the information about the video mode.
****************************************************************************/
void displaymodeInfo(void)
{
    gprintf("Video mode: %d x %d %d bit %s",
        modeInfo.XResolution,modeInfo.YResolution,
        modeInfo.BitsPerPixel,
        (cntMode & gaLinearBuffer) ? "Linear" : "Banked");
}

/****************************************************************************
REMARKS:
Draws a simple Moire pattern on the display screen using lines.
****************************************************************************/
void moire(
    GA_color defcolor)
{
    int     i,value;

    if (maxcolor >= 0x7FFFL) {
        for (i = 0; i < maxX; i++) {
            SetForeColor(rgbColor((uchar)((i*255L)/maxX),0,0));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,0,rgbColor((uchar)((i*255L)/maxX),0,0));
            SetForeColor(rgbColor(0,(uchar)((i*255L)/maxX),0));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,maxY,rgbColor(0,(uchar)((i*255L)/maxX),0));
            }
        for (i = 0; i < maxY; i++) {
            value = (int)((i*255L)/maxY);
            SetForeColor(rgbColor((uchar)value,0,(uchar)(255 - value)));
            draw2d.DrawLineInt(maxX/2,maxY/2,0,i,rgbColor((uchar)value,0,(uchar)(255 - value)));
            SetForeColor(rgbColor(0,(uchar)(255 - value),(uchar)value));
            draw2d.DrawLineInt(maxX/2,maxY/2,maxX,i,rgbColor(0,(uchar)(255 - value),(uchar)value));
            }
        }
    else {
        for (i = 0; i < maxX; i += 5) {
            SetForeColor(i % (maxcolor+1));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,0,true);
            SetForeColor((i+1) % (maxcolor+1));
            draw2d.DrawLineInt(maxX/2,maxY/2,i,maxY,true);
            }
        for (i = 0; i < maxY; i += 5) {
            SetForeColor((i+2) % (maxcolor+1));
            draw2d.DrawLineInt(maxX/2,maxY/2,0,i,true);
            SetForeColor((i+3) % (maxcolor+1));
            draw2d.DrawLineInt(maxX/2,maxY/2,maxX,i,true);
            }
        }
    SetForeColor(defcolor);
    draw2d.DrawLineInt(0,0,maxX,0,true);
    draw2d.DrawLineInt(0,0,0,maxY,true);
    draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
    draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
    CopyShadowBuffer();
}

/****************************************************************************
REMARKS:
Draws a simple Moire pattern on the display screen using lines, and waits
for a key press.
****************************************************************************/
int moireTest(void)
{
    moire(defcolor);
    if (maxX >= 479) {
        gmoveto(80,80);
        gprintf("Drawing test");  y += 16;
        displaymodeInfo();
        gprintf("Maximum x: %d, Maximum y: %d",maxX,maxY);
        gprintf("BytesPerLine %ld, Pages: %d",bytesPerLine,maxPage+1);
        y += 16;
        gprintf("You should see a colorful pattern of lines");
        gprintf("on the screen");
        }
    else {
        gmoveto(8,40);
        displaymodeInfo();
        }
    gprintf("Press any key to continue");
    y += 16;
    CopyShadowBuffer();
    return EVT_getch();
}

/****************************************************************************
REMARKS:
Animates a line on the display using page flipping if page flipping is
active.
****************************************************************************/
int pageFlipTest(void)
{
    int     i,j,istep,jstep,apage,vpage;
    int     initPageCount = 20, pageCount = initPageCount;
    int     fpsRate = 0,key = 0,waitVRT = gaWaitVRT;
    ibool   done = false,singleStep = false;
    ulong   color,lastCount = 0,newCount;
    char    buf[80],buf2[80],buf3[80];

    if (maxPage == 0)
        return 0;
    vpage = 0;
    apage = 1;
    SetActivePage(apage);
    SetVisualPage(vpage,waitVRT);
    i = 0;
    j = maxY;
    istep = 2;
    jstep = -2;
    color = 15;
    if (maxcolor > 255)
        color = defcolor;
    LZTimerOn();
    while (!done) {
        SetActivePage(apage);
        ClearPage(0);
        gmoveto(4,4);
        gprintf("%5d.%d fps", fpsRate / 10, fpsRate % 10);
        sprintf(buf,"%d x %d %d bit %s",(int)maxX+1,(int)maxY+1,
            (int)modeInfo.BitsPerPixel,
            (cntMode & gaLinearBuffer) ? "Linear" : "Banked");
        switch (waitVRT) {
            case gaTripleBuffer:
                strcpy(buf2,"Triple buffering - should be no flicker");
                strcpy(buf3,"Frame rate *must* max at refresh rate");
                break;
            case gaWaitVRT:
                strcpy(buf2,"Double buffering - should be no flicker");
                strcpy(buf3,"Frame rate *must* lock to multiple of refresh");
                break;
            default:
                strcpy(buf2,"Page flipping (no wait) - may flicker");
                strcpy(buf3,"Frame rate *must* max at hardware limit");
                break;
            }
        if (maxX <= 360) {
            WriteText(8,60,buf,defcolor);
            WriteText(8,80,buf2,defcolor);
            WriteText(8,96,buf3,defcolor);
            sprintf(buf,"Page %4d of %d", (int)apage+1, (int)maxPage+1);
            WriteText(8,116,buf,defcolor);
            }
        else {
            WriteText(80,60,buf,defcolor);
            WriteText(80,80,buf2,defcolor);
            WriteText(80,96,buf3,defcolor);
            sprintf(buf,"Page %4d of %d", (int)apage+1, (int)maxPage+1);
            WriteText(80,116,buf,defcolor);
            }
        SetForeColor(color);
        draw2d.DrawLineInt(i,0,maxX-i,maxY,true);
        draw2d.DrawLineInt(0,maxY-j,maxX,j,true);
        SetForeColor(defcolor);
        draw2d.DrawLineInt(0,0,maxX,0,true);
        draw2d.DrawLineInt(0,0,0,maxY,true);
        draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
        draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
        if (singleStep) {
            key = EVT_getch();                /* Swallow keypress */
            if (key == 's' || key == 'S') {
                singleStep ^= 1;
                }
            else if (key == 0x1B)
                break;
            }
        vpage = (vpage + 1) % (maxPage+1);
        SetVisualPage(vpage,waitVRT);
        apage = (apage + 1) % (maxPage+1);
        i += istep;
        if (i > maxX) {
            i = maxX-2;
            istep = -2;
            }
        if (i < 0)  i = istep = 2;
        j += jstep;
        if (j > maxY) {
            j = maxY-2;
            jstep = -2;
            }
        if (j < 0)  j = jstep = 2;

        /* Compute the frames per second rate after going through a large
         * number of pages.
         */
        if (--pageCount == 0) {
            newCount = LZTimerLap();
            fpsRate = (int)(10000000L / (newCount - lastCount)) * initPageCount;
            lastCount = newCount;
            pageCount = initPageCount;
            }

        if (!singleStep && EVT_kbhit()) {
            key = EVT_getch();                /* Swallow keypress */
            if (key == 'v' || key == 'V') {
                waitVRT -= 1;
                if (modeInfo.Attributes & gaHaveTripleBuffer) {
                    if (waitVRT < gaTripleBuffer)
                        waitVRT = gaDontWait;
                    }
                else {
                    if (waitVRT < gaWaitVRT)
                        waitVRT = gaDontWait;
                    }
                }
            else if (key == 's' || key == 'S') {
                singleStep ^= 1;
                }
            else
                break;
            }
        }
    LZTimerOff();

    /* Only reset the active page if we hadn't been put to background! */
    if (ref2d != NULL) {
        SetActivePage(0);
        SetVisualPage(0,gaDontWait);
        }
    return key;
}

/****************************************************************************
PARAMETERS:
pal             - Palette to fade
fullIntensity   - Palette of full intensity values
numColors       - Number of colors to fade
startIndex      - Starting index in palette
intensity       - Intensity value for entries (0-255)

REMARKS:
Fades each of the palette values in the palette by the specified intensity
value. The values to fade from are contained in the 'fullItensity' array,
which should be at least numColors in size.
****************************************************************************/
void fadePalette(
    GA_palette *pal,
    GA_palette *fullIntensity,
    int numColors,
    int startIndex,
    uchar intensity)
{
    uchar   *p,*fi;
    int     i;

    p = (uchar*)&pal[startIndex];
    fi = (uchar*)fullIntensity;
    for (i = 0; i < numColors; i++) {
        *p++ = (*fi++ * intensity) / (uchar)255;
        *p++ = (*fi++ * intensity) / (uchar)255;
        *p++ = (*fi++ * intensity) / (uchar)255;
        p++; fi++;
        }
}

/****************************************************************************
REMARKS:
Performs a palette programming test by displaying all the colors in the
palette and then quickly fading the values out then in again.
****************************************************************************/
int paletteTest(void)
{
    int         i,key;
    GA_palette  pal[256],tmp[256];
    char        buf[80];

    ClearPage(0);
    moire(63);
    if (maxX > 360) {
        x = 80; y = 80;
        }
    else {
        x = 40; y = 40;
        }
    sprintf(buf,"%d x %d %d bit %s",(int)maxX+1,(int)maxY+1,
        (int)modeInfo.BitsPerPixel,
        (cntMode & gaLinearBuffer) ? "Linear" : "Banked");
    WriteText(x,y,buf,63);
    y += 16;
    WriteText(x,y,"Palette programming test",63);
    y += 32;
    WriteText(x,y,"Hit a key to fade palette",63);

    memset(pal,0,256*sizeof(GA_palette));
    for (i = 0; i < 64; i++) {
        pal[i].Red = pal[i].Green = pal[i].Blue = i*4;
        pal[64 + i].Red = i*4;
        pal[128 + i].Green = i*4;
        pal[192 + i].Blue = i*4;
        }

    driver.SetPaletteData(pal,256,0,false);
    key = EVT_getch();
    if (key == 0x1B || key == 'N' || key == 'n' || key == 'F' || key == 'f')
        goto ResetPalette;

    /* Palette fade out */
    for (i = 63; i >= 0; i--) {
        fadePalette(tmp,pal,256,0,i*4);
        driver.SetPaletteData(tmp,256,0,true);
        if (EVT_kbhit())
            goto DoneFade;
        }

    /* Palette fade in */
    for (i = 0; i <= 63; i++) {
        fadePalette(tmp,pal,256,0,i*4);
        driver.SetPaletteData(tmp,256,0,true);
        if (EVT_kbhit())
            goto DoneFade;
        }

DoneFade:
    key = EVT_getch();
ResetPalette:
    driver.SetPaletteData(VGA8_defPal,256,0,false);
    return key;
}

/****************************************************************************
REMARKS:
Displays a set of color values using the wide DAC support if available.
***************************************************************************/
int wideDACTest(void)
{
    int         i,max,x,y,key;
    GA_palette  pal[256];
    char        buf[80];

    if (!(dc->Attributes & gaHave8BitDAC))
        return -1;
    memset(pal,0,256*sizeof(GA_palette));
    for (i = 0; i < 256; i++) {
        pal[i].Red = i;
        pal[i].Green = i;
        pal[i].Blue = i;
        }

    driver.SetPaletteData(pal,256,0,false);
    ClearPage(0);
    SetForeColor(255);
    draw2d.DrawLineInt(0,0,maxX,0,true);
    draw2d.DrawLineInt(0,0,0,maxY,true);
    draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
    draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
    if (maxX > 360) {
        x = 80;
        y = 80;
        }
    else {
        x = 40;
        y = 40;
        }
    sprintf(buf,"%d x %d %d bit %s",(int)maxX+1,(int)maxY+1,
        (int)modeInfo.BitsPerPixel,
        (cntMode & gaLinearBuffer) ? "Linear" : "Banked");
    WriteText(x,y,buf,255);
    y += 16;
    WriteText(x,y,"Wide DAC test",255);
    y += 32;
    if (maxX >= 639) {
        WriteText(x,y,"You should see a smooth transition of gray",255);
        y += 16;
        WriteText(x,y,"If you see lots of banding, the wide DAC is not working",255);
        y += 32;
        }
    max = maxX-1;
    for (i = 0; i < max; i++) {
        int intensity = (i*256)/max;
        SetForeColor(intensity);
        draw2d.DrawLineInt(i+1, y,    i+1, y+96, true);
        }

    key = (char)EVT_getch();
    driver.SetPaletteData(VGA8_defPal,256,0,false);
    return key;
}

/****************************************************************************
REMARKS:
Displays an image on the RGB display and uses the gamma correctin ramp
the fade the display.
***************************************************************************/
int gammaCorrectTest(void)
{
    int             i,key;
    GA_palette      pal[256];
    GA_palette      defPal[256];
    char            buf[80];

    if (!driver.SetGammaCorrectData)
        return -1;
    ClearPage(0);
    moire(defcolor);
    if (maxX > 360) {
        x = 80; y = 80;
        }
    else {
        x = 40; y = 40;
        }
    sprintf(buf,"%d x %d %d bit %s",(int)maxX+1,(int)maxY+1,
        (int)modeInfo.BitsPerPixel,
        (cntMode & gaLinearBuffer) ? "Linear" : "Banked");
    WriteText(x,y,buf,defcolor);
    y += 16;
    WriteText(x,y,"Gamma correction test",defcolor);
    y += 32;
    WriteText(x,y,"Hit a key to fade screen",defcolor);
    key = EVT_getch();
    if (key == 0x1B || key == 'N' || key == 'n' || key == 'F' || key == 'f')
        goto ResetGamma;

    /* gamma fade out */
    driver.GetGammaCorrectData(defPal,256,0);
    for (i = 63; i >= 0; i--) {
        fadePalette(pal,defPal,256,0,i*4);
        driver.SetGammaCorrectData(pal,256,0,true);
        if (EVT_kbhit())
            goto DoneFade;
        }

    /* gamma fade in */
    for (i = 0; i <= 63; i++) {
        fadePalette(pal,defPal,256,0,i*4);
        driver.SetGammaCorrectData(pal,256,0,true);
        if (EVT_kbhit())
            goto DoneFade;
        }

DoneFade:
    key = EVT_getch();
ResetGamma:
    driver.SetGammaCorrectData(defPal,256,0,false);
    return key;
}

/****************************************************************************
REMARKS:
Display a stereo test image on the screen and do hardware stereo page
flipping.
****************************************************************************/
int stereoTest(void)
{
    int         i,apage,vpage,buffer,key = 0,numBuffers;
    int         waitVRT = gaWaitVRT,dx = 0,dy = 0;
    ibool       done = false,bReverseX = false,bReverseY = false;
    char        buf[80],buf2[80];
    GA_color    red,blue;

    if (maxPage != 0) {
        /* If we have at least 4 pages, then we can do stereo with double
         * buffering. If we have at least two pages then we can do just
         * plain stereo without double buffering, otherwise we cant do
         * stereo so we skip this test.
         */
        numBuffers = maxPage+1;
        if (numBuffers >= 4)
            numBuffers = 2;
        else
            numBuffers = 1;
        if (numBuffers > 1) {
            /* Setup for double buffering */
            vpage = 0;
            apage = 1;
            }
        else {
            /* Setup for single buffered stereo */
            vpage = 0;
            apage = 0;
            }
        if (!StartStereo())
            return key;
        if (maxcolor >= 0x7FFFL) {
            red = rgbColor(0xFF,0,0);
            blue = rgbColor(0,0,0xFF);
            }
        else {
            red = 4;
            blue = 1;
            }
        SetActivePage(apage | gaLeftBuffer);
        SetVisualPage(vpage,waitVRT);
        while (!done) {
            buffer = gaLeftBuffer;
            for (i = 0; i < 2; i++) {
                SetActivePage(apage | buffer);
                ClearPage(0);
                SetForeColor(i ? red : blue);
                draw2d.DrawRect(dx,dy,maxX/2,maxY/2);
                gmoveto(4,4);
                if (dc->Attributes & gaHaveStereo)
                    sprintf(buf,"%d x %d %d bit Hardware Stereo",
                        (int)maxX+1,(int)maxY+1,(int)modeInfo.BitsPerPixel);
                else
                    sprintf(buf,"%d x %d %d bit Software Stereo",
                        (int)maxX+1,(int)maxY+1,(int)modeInfo.BitsPerPixel);
                switch (waitVRT) {
                    case gaWaitVRT:
                        strcpy(buf2,"Double buffering - should be no flicker");
                        break;
                    default:
                        strcpy(buf2,"Page flipping (no wait) - may flicker");
                        break;
                    }
                if (maxX <= 360) {
                    WriteText(8,60,buf,defcolor);
                    WriteText(8,80,buf2,defcolor);
                    sprintf(buf,"Page %d of %d", apage+1,numBuffers);
                    WriteText(8,100,buf,defcolor);
                    if (buffer == gaLeftBuffer)
                        WriteText(8,120,"LEFT",defcolor);
                    else
                        WriteText(68,120,"RIGHT",defcolor);
                    }
                else {
                    WriteText(80,60,buf,defcolor);
                    WriteText(80,80,buf2,defcolor);
                    sprintf(buf,"Page %d of %d", apage+1,numBuffers);
                    WriteText(80,100,buf,defcolor);
                    if (buffer == gaLeftBuffer)
                        WriteText(80,120,"LEFT",defcolor);
                    else
                        WriteText(120,120,"RIGHT",defcolor);
                    }
                buffer = gaRightBuffer;
                }
            if (numBuffers > 1) {
                /* Animate the rectangles pong style */
                if (bReverseX) {
                    if (--dx < 0) {
                        dx = 0; bReverseX = false;
                        }
                    }
                else {
                    if (++dx > maxX/2) {
                        dx = maxX/2; bReverseX = true;
                        }
                    }
                if (bReverseY) {
                    if (--dy < 0) {
                        dy = 0; bReverseY = false;
                        }
                    }
                else {
                    if (++dy > maxY/2) {
                        dy = maxY/2; bReverseY = true;
                        }
                    }

                /* Flip the display page */
                vpage ^= 1;
                SetVisualPage(vpage,waitVRT);
                apage ^= 1;
                }
            else {
                /* We only have one page, however we have to call
                 * SetVisualPage in order to display the blue code
                 * sync signal over the top for software stereo sync.
                 */
                SetVisualPage(vpage,gaDontWait);
                while (!EVT_kbhit())
                    ;
                }

            /* Check if a key has been hit */
            if (EVT_kbhit()) {
                key = EVT_getch();                /* Swallow keypress */
                if (key == 'v' || key == 'V') {
                    waitVRT -= 1;
                    if (waitVRT < gaWaitVRT)
                        waitVRT = gaDontWait;
                    }
                else
                    break;
                }
            }
        StopStereo();
        }
    /* Only reset the active page if we hadn't been put to background! */
    if (ref2d != NULL) {
        SetActivePage(0);
        SetVisualPage(0,gaDontWait);
        }
    return key;
}

/****************************************************************************
REMARKS:
Initialize cursor data in offscreen memory before running accel tests
to check for corruption/interaction between color/mono patterns, system
blit areas, etc.
****************************************************************************/
void doCursorInit(void)
{
    GA_cursorFuncs  curs;
    GA_palette      fore = {0xFF,0,0,0};
    GA_palette      back = {0,0xFF,0,0};
    int             x,y;

    curs.dwSize = sizeof(curs);
    if (!REF2D_queryFunctions(ref2d,GA_GET_CURSORFUNCS,&curs))
        return;
    if (modeInfo.BitsPerPixel <= 8 && !(modeInfo.Attributes & gaHave8bppRGBCursor)) {
        fore.Red = realColor(1);
        back.Red = realColor(2);
        fore.Green = fore.Blue = back.Green = back.Blue = 0;
        }
    curs.SetMonoCursor(&arrowCursor);
    curs.SetMonoCursorColor(&fore,&back);
    EVT_getMousePos(&x,&y);
    curs.SetCursorPos(x,y);
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doTest(
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
    if (!InitSoftwareRasterizer(cntDevice,modeInfo.MaxBuffers,false))
        PM_fatalError("Unable to initialise software rasteriser!");

    /* Initialize cursor data here to test for interaction w/other functions */
    doCursorInit();

    /* Set the hardware plane mask */
    SetPlaneMask(planeMask);

    /* Force a 1 bit per pixel shadow buffer for testing */
    if (force1bppShadow)
        EnableShadowBuffer(1,NULL,force1bppShadow == 2 ? gaBufferMonoLSB : 0);

    /* Run the tests */
    if (stricmp(accelTestName,"all") == 0) {
        if (moireTest() == 0x1B)
            goto DoneTests;
        if (!fullSpeed) {
            if (pageFlipTest() == 0x1B)
                goto DoneTests;
            if (maxcolor == 255) {
                if (paletteTest() == 0x1B)
                    goto DoneTests;
                if (wideDACTest() == 0x1B)
                    goto DoneTests;
                }
            else if (maxcolor > 255) {
                if (gammaCorrectTest() == 0x1B)
                    goto DoneTests;
                }
            }
        }
#ifdef PRO
    if (softwareEmulate || (modeInfo.Attributes & gaHaveAccel2D)) {
        if (!force1bppShadow) {
            /* Re-init the buffer manager to free up offscreen memory */
            if (!bufmgr.InitBuffers(MIN(modeInfo.MaxBuffers,2),0,NULL))
                return false;
            }
        if (fullSpeed) {
            while (!EVT_kbhit()) {
                if (doAccelTests(accelTestName) == 0x1B)
                    goto DoneTests;
                }
            EVT_getch();
            }
        else {
            if (doAccelTests(accelTestName) == 0x1B)
                goto DoneTests;
            }
        doCursorTest(accelTestName);
        }
#endif

    /* Return to text mode, restore the state of the console and exit */
DoneTests:
    ExitSoftwareRasterizer();
    GC_restore(gc);
    (void)gl;
    return true;
}

/****************************************************************************
REMARKS:
Round a value to the specified integer boundary
****************************************************************************/
static ulong RoundDown(
    ulong value,
    ulong boundary)
{
    return (value / boundary) * boundary;
}

/****************************************************************************
REMARKS:
Main function to do the interactive hardware panning tests.
****************************************************************************/
ibool doScrollTest(
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
    int             xpos,ypos,oldx = -1,oldy = -1;
    int             deskStartX,deskStartY,deskStartXMask;
    GA_cursorFuncs  curs;

    /* Bail if scroll function is not available */
    if (!(gc->dc->Attributes & gaHaveDisplayStart))
        return false;

    /* Find the maximum virtual X and Y resolution for the mode */
    GC_leave(gc);
    dc = gc->dc;
    modeInfo.dwSize = sizeof(modeInfo);
    if (xRes == -1) {
        if (init.GetVideoModeInfo(flags,&modeInfo) != 0)
            return false;
        }
    else {
        if (init.GetCustomVideoModeInfo(xRes,yRes,-1,-1,bitsPerPixel,&modeInfo) != 0)
            return false;
        }
    virtualX = modeInfo.XResolution * 2;
    virtualY = modeInfo.YResolution * 2;
    if (virtualX > modeInfo.MaxScanLineWidth)
        virtualX = modeInfo.MaxScanLineWidth;
    maxY = (modeInfo.XResolution * modeInfo.MaxScanLines) / virtualX;
    if (maxY < modeInfo.YResolution)
        maxY = modeInfo.YResolution;
    if (virtualY > maxY) {
        virtualY = maxY;
        virtualX = RoundDown((modeInfo.XResolution * modeInfo.MaxScanLines) / virtualY,16);
        }
    bytesPerLine = -1;

    /* Obtain the mode information and set the display mode */
    if (xRes == -1) {
        if (init.SetVideoMode(flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,refreshRate,crtc) != 0)
            return false;
        }
    else {
        if (init.SetCustomVideoMode(xRes,yRes,bitsPerPixel,flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,crtc) != 0)
            return false;
        }
    cntMode = flags;
    if (!InitSoftwareRasterizer(cntDevice,1,false))
        PM_fatalError("Unable to initialise software rasteriser!");

    /* Perform the scrolling tests */
    InitMouse(modeInfo.XResolution,modeInfo.YResolution);
    moire(defcolor);
    gmoveto(80,80);
    gprintf("Hardware scrolling test");  y += 16;
    displaymodeInfo();
    gprintf("Maximum x: %d, Maximum y: %d",virtualX,virtualY);
    gprintf("BytesPerLine: %ld",bytesPerLine);
    y += 16;
    gprintf("Use the mouse to scroll around within the virtual screen");
    gprintf("Press any key to exit");
    y += 16;
    doCursorInit();
    curs.dwSize = sizeof(curs);
    REF2D_queryFunctions(ref2d,GA_GET_CURSORFUNCS,&curs);

    /* Now pan around on the screen using the mouse */
    deskStartX = deskStartY = 0;
    deskStartXMask = modeInfo.DeskStartXMask;
    curs.ShowCursor(true);
    do {
        GetMousePos(&xpos,&ypos);
        if (xpos != oldx || ypos != oldy) {
            int scroll = false;
            oldx = xpos;
            oldy = ypos;
            if (xpos < deskStartX) {
                deskStartX = xpos;
                scroll = true;
                }
            else if (xpos >= (deskStartX + modeInfo.PhysicalXResolution)) {
                deskStartX = xpos - (modeInfo.PhysicalXResolution-1);
                scroll = true;
                }
            if (ypos < deskStartY) {
                deskStartY = ypos;
                scroll = true;
                }
            else if (ypos >= (deskStartY + modeInfo.PhysicalYResolution)) {
                deskStartY = ypos - (modeInfo.PhysicalYResolution-1);
                scroll = true;
                }
            if (scroll) {
                deskStartX &= deskStartXMask;
                SetVisualPageXY(0,deskStartX,deskStartY,gaWaitVRT);
                }

            // Now move the cursor to the new location
            curs.SetCursorPos(xpos - deskStartX,ypos - deskStartY);
            }
        } while (!EVT_kbhit());
    EVT_getch();

    /* NOTE: We do *not* hide the cursor here as we want to ensure that if
     *       if exit back to text mode, and then come back to graphics
     *       mode that the cursor is properly hidden!
     */

    /* Return to text mode, restore the state of the console and exit */
    ExitSoftwareRasterizer();
    GC_restore(gc);
    (void)gl;
    return true;
}

/****************************************************************************
REMARKS:
Function to draw the screen for the zooming test.
****************************************************************************/
void drawZoomScreen(void)
{
    ClearPage(0);
    moire(defcolor);
    gmoveto(80,80);
    gprintf("Zooming test");  y += 16;
    displaymodeInfo();
    gprintf("Maximum x: %d, Maximum y: %d",virtualX,virtualY);
    gprintf("BytesPerLine: %ld",bytesPerLine);
    y += 16;
    gprintf("Use the F1-F7 keys or mouse wheel to change resolution");
    gprintf("and the mouse to scroll around within the virtual screen");
    y += 16;
    gprintf("Press Esc to exit");
}

/****************************************************************************
REMARKS:
Main function to do the interactive zooming tests.
****************************************************************************/
ibool doZoomTest(
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
    int             key,done,deskStartX,deskStartY,zoomXRes,zoomYRes;
    int             lastZoom = 0,deskStartXMask;
    int             doZoom = false;
    event_t         evt;
    GA_cursorFuncs  curs;

    /* Bail if zooming function is not available */
    if (!init.SwitchPhysicalResolution)
        return false;
    if (!(gc->dc->Attributes & gaHaveDisplayStart))
        return false;

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
    if (!InitSoftwareRasterizer(cntDevice,modeInfo.MaxBuffers,false))
        PM_fatalError("Unable to initialise software rasteriser!");

    /* Perform the scrolling test */
    InitMouse(modeInfo.PhysicalXResolution,modeInfo.PhysicalYResolution);
    drawZoomScreen();
    doCursorInit();
    curs.dwSize = sizeof(curs);
    REF2D_queryFunctions(ref2d,GA_GET_CURSORFUNCS,&curs);

    /* Now pan around on the screen using the mouse */
    deskStartX = deskStartY = 0;
    deskStartXMask = modeInfo.DeskStartXMask;
    zoomXRes = modeInfo.PhysicalXResolution;
    zoomYRes = modeInfo.PhysicalYResolution;
    if (zoomXRes == 320 && zoomYRes <= 240)
        lastZoom = 0 ;
    else if (zoomXRes >= 320 && zoomXRes < 400)
        lastZoom = 1 ;
    else if (zoomXRes >= 400 && zoomXRes < 512)
        lastZoom = 2 ;
    else if (zoomXRes >= 512 && zoomXRes < 640)
        lastZoom = 3 ;
    else if (zoomXRes >= 640 && zoomXRes < 800)
        lastZoom = 4 ;
    else
        lastZoom = 5 ;

    curs.ShowCursor(true);
    done = false;
    while (!done) {
        doZoom = false;
        EVT_halt(&evt,EVT_KEYDOWN | EVT_MOUSEMOVE | EVT_MOUSEWHEEL | EVT_MOUSEDOWN);
        if (evt.what == EVT_MOUSEMOVE) {
            int scroll = false;
            if (evt.where_x < deskStartX) {
                deskStartX = evt.where_x;
                scroll = true;
                }
            else if (evt.where_x >= (deskStartX + zoomXRes)) {
                deskStartX = evt.where_x - (zoomXRes-1);
                scroll = true;
                }
            if (evt.where_y < deskStartY) {
                deskStartY = evt.where_y;
                scroll = true;
                }
            else if (evt.where_y >= (deskStartY + zoomYRes)) {
                deskStartY = evt.where_y - (zoomYRes-1);
                scroll = true;
                }
            if (scroll) {
                deskStartX &= deskStartXMask;
                SetVisualPageXY(0,deskStartX,deskStartY,gaWaitVRT);
                }

            // Now move the cursor to the new location
            curs.SetCursorPos(evt.where_x - deskStartX,evt.where_y - deskStartY);
            }
        else if (evt.what == EVT_MOUSEWHEEL) {
            if (EVT_wheelType(evt.message) == MW_VERTWHEEL) {
                if (EVT_wheelUp(evt.message)) {
                    if (lastZoom < 5) {
                        lastZoom += EVT_wheelDelta(evt.message) ;
                        doZoom = true ;
                        if (lastZoom > 5)
                            lastZoom = 5 ;
                        }
                    }
                else {
                    if (lastZoom > 0) {
                        lastZoom -= EVT_wheelDelta(evt.message) ;
                        doZoom = true ;
                        if (lastZoom < 0)
                            lastZoom = 0 ;
                        }
                    }
                }
                switch (lastZoom) {
                    case 0:
                        // Zoom to 320x240
                        zoomXRes = 320;
                        zoomYRes = 240;
                        break;
                    case 1:
                        // Zoom to 320x480
                        zoomXRes = 320;
                        zoomYRes = 480;
                        break;
                    case 2:
                        // Zoom to 400x300
                        zoomXRes = 400;
                        zoomYRes = 300;
                        break;
                    case 3:
                        // Zoom to 512x384
                        zoomXRes = 512;
                        zoomYRes = 384;
                        break;
                    case 4:
                        // Zoom to 640x480
                        zoomXRes = 640;
                        zoomYRes = 480;
                        break;
                    case 5:
                        // Zoom to 800x600
                        zoomXRes = 800;
                        zoomYRes = 600;
                        break;
                    }
            }
        else if (evt.what == EVT_MOUSEDOWN) {
            if ((evt.message & EVT_MIDDLEBMASK) != 0) {
                    // Go back to original mode
                    zoomXRes = modeInfo.XResolution;
                    zoomYRes = modeInfo.YResolution;
                    doZoom = true;
                }
            }
        else if (evt.what == EVT_KEYDOWN) {
            key = EVT_scanCode(evt.message);
            switch (key) {
                case KB_esc:
                    done = true;
                    break;
                case KB_F1:
                    // Go back to original mode
                    zoomXRes = modeInfo.XResolution;
                    zoomYRes = modeInfo.YResolution;
                    doZoom = true;
                    break;
                case KB_F2:
                    // Zoom to 320x240
                    zoomXRes = 320;
                    zoomYRes = 240;
                    doZoom = true;
                    break;
                case KB_F3:
                    // Zoom to 320x480
                    zoomXRes = 320;
                    zoomYRes = 480;
                    doZoom = true;
                    break;
                case KB_F4:
                    // Zoom to 400x300
                    zoomXRes = 400;
                    zoomYRes = 300;
                    doZoom = true;
                    break;
                case KB_F5:
                    // Zoom to 512x384
                    zoomXRes = 512;
                    zoomYRes = 384;
                    doZoom = true;
                    break;
                case KB_F6:
                    // Zoom to 640x480
                    zoomXRes = 640;
                    zoomYRes = 480;
                    doZoom = true;
                    break;
                case KB_F7:
                    // Zoom to 800x600
                    zoomXRes = 800;
                    zoomYRes = 600;
                    doZoom = true;
                    break;
                case KB_R:
                    drawZoomScreen();
                    break;
                }
            }

        if (doZoom) {
            if (zoomXRes <= modeInfo.XResolution && zoomYRes <= modeInfo.YResolution) {
                curs.ShowCursor(false);
                if (init.SwitchPhysicalResolution(zoomXRes,zoomYRes,refreshRate) == 0) {
                    ref2d->PostSwitchPhysicalResolution();

                    // Important: the mode actually set may not be exactly what we asked for,
                    // laptops for instance may not be able to do 320x480 on LCD. We need to
                    // requery the actual physical mode.
                    modeInfo.dwSize = sizeof(modeInfo);
                    init.GetCurrentVideoModeInfo(&modeInfo);
                    zoomXRes = modeInfo.PhysicalXResolution;
                    zoomYRes = modeInfo.PhysicalYResolution;
                    SetVisualPageXY(0,deskStartX = 0,deskStartY = 0,gaWaitVRT);
                    EVT_setMousePos(zoomXRes/2,zoomYRes/2);
                    doCursorInit();
                    drawZoomScreen();
                    }
                curs.ShowCursor(true);
                }
            }
        }

    /* NOTE: We do *not* hide the cursor here as we want to ensure that if
     *       if exit back to text mode, and then come back to graphics
     *       mode that the cursor is properly hidden.
     */

    /* Return to text mode, restore the state of the console and exit */
    ExitSoftwareRasterizer();
    GC_restore(gc);
    (void)gl;
    return true;
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doStereoTest(
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
    if (!InitSoftwareRasterizer(cntDevice,modeInfo.MaxBuffers,true))
        PM_fatalError("Unable to initialise software rasteriser!");

    /* Run the tests */
    stereoTest();

    /* Return to text mode, restore the state of the console and exit */
    ExitSoftwareRasterizer();
    GC_restore(gc);
    (void)gl;
    return true;
}

