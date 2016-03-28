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
* Environment:  Any
*
* Description:  Module to implement the code to perform fullscreen
*               centering and refresh rate control using the SciTech SNAP
*               Graphics Architecture functions.
*
****************************************************************************/

#include "gatest.h"
#include "ztimer.h"

/*---------------------------- Global Variables ---------------------------*/

#define HSTEP           8
#define MIN_HBLANK_TIME (10 * HSTEP)
#define MIN_VBLANK_TIME 10
#define MIN_HSYNC_START (0 * HSTEP)
#define MIN_HSYNC_END   (1 * HSTEP)
#define MIN_VSYNC_START 0
#define MIN_VSYNC_END   5
#define SYNC_COUNT      5

static int              vRefresh;       /* Vertical refresh rate        */

#ifdef DEBUG_BLANKS
extern int hBlankStart,hBlankEnd,vBlankStart,vBlankEnd;
#endif

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Function to check if the vSync line is active
****************************************************************************/
static ibool checkForVSync(void)
{
    ibool   active = false;
    ibool   vSync = driver.IsVSync();

    /* Check for changing vSync signal */
    LZTimerOn();
    while (LZTimerLap() < 100000UL) {
        if (driver.IsVSync() != vSync) {
            active = true;
            break;
            }
        }
    LZTimerOff();
    return active;
}

/****************************************************************************
REMARKS:
Function to measure the refresh rate from the hardware
****************************************************************************/
static void ReadRefresh(void)
{
    int     i;
#ifdef  __WINDOWS32___TODO__
    DWORD   oldclass;
    HANDLE  hprocess;

    // TODO: We should abstract this into the PM library!

    /* Set the priority of the process to maximum for accurate timing */
    hprocess = GetCurrentProcess();
    oldclass = GetPriorityClass(hprocess);
    SetPriorityClass(hprocess, REALTIME_PRIORITY_CLASS);
#endif

    if (checkForVSync()) {
        driver.WaitVSync();
        driver.WaitVSync();
        LZTimerOn();
        for (i = 0; i < SYNC_COUNT; i++)
            driver.WaitVSync();
        LZTimerOff();
        vRefresh = (int)((SYNC_COUNT * 10000000.0) / LZTimerCount());
        }
    else {
        vRefresh = 0;
        }

#ifdef  __WINDOWS32___TODO__
    SetPriorityClass(hprocess, oldclass);
#endif
}

/****************************************************************************
REMARKS:
Dumps the CRTC timings to a file.
****************************************************************************/
static void dumpCRTCValues(
    GA_CRTCInfo *crtc)
{
    FILE    *f = fopen("center.log", "at+");

    fprintf(f,"\nCRTC values for %dx%d %d bit @%d.%dHz\n",
        modeInfo.XResolution,modeInfo.YResolution, modeInfo.BitsPerPixel,
        vRefresh / 10, vRefresh % 10);
    fprintf(f,"\n");
    fprintf(f,"  hTotal      = %d\n", crtc->HorizontalTotal);
    fprintf(f,"  hSyncStart  = %d\n", crtc->HorizontalSyncStart);
    fprintf(f,"  hSyncEnd    = %d\n", crtc->HorizontalSyncEnd);
#ifdef DEBUG_BLANKS
    fprintf(f,"  hBlankStart  = %d\n", hBlankStart);
    fprintf(f,"  hBlankEnd    = %d\n", hBlankEnd);
#endif
    fprintf(f,"\n");
    fprintf(f,"  vTotal      = %d\n", crtc->VerticalTotal);
    fprintf(f,"  vSyncStart  = %d\n", crtc->VerticalSyncStart);
    fprintf(f,"  vSyncEnd    = %d\n", crtc->VerticalSyncEnd);
#ifdef DEBUG_BLANKS
    fprintf(f,"  vBlankStart  = %d\n", vBlankStart);
    fprintf(f,"  vBlankEnd    = %d\n", vBlankEnd);
#endif
    fprintf(f,"\n");
    fprintf(f,"  Interlaced  = %s\n", (crtc->Flags & gaInterlaced) ? "Yes" : "No");
    fprintf(f,"  Double scan = %s\n", (crtc->Flags & gaDoubleScan) ? "Yes" : "No");
    fprintf(f,"  H sync pol  = %s\n", (crtc->Flags & gaHSyncNeg) ? "-" : "+");
    fprintf(f,"  V sync pol  = %s\n", (crtc->Flags & gaVSyncNeg) ? "-" : "+");
    fprintf(f,"\n");
    fprintf(f,"  Dot Clock   = %d.%02dMhz\n",   (int)crtc->PixelClock / 1000000, (int)crtc->PixelClock / 10000);
    fclose(f);
}

/****************************************************************************
REMARKS:
Draw the background image for the centering and refresh display
****************************************************************************/
static void drawBackground(
    GC_devCtx *gc,
    GA_CRTCInfo *crtc,
    ibool readRefresh)
{
    char        buf[80];
    int         i,x,y,min,max,maxx,maxy,attr;
    long        range;
    GA_palette  pal[256],*p;

    if (modeInfo.BitsPerPixel == 0) {
        GC_setAttr(gc,GC_makeAttr(GC_YELLOW,GC_BLUE));
        GC_fillText(gc,0, 0, GC_maxx(gc), GC_maxy(gc), GC_makeAttr(GC_RED, GC_BLUE), '±');
        attr = GC_makeAttr(GC_WHITE, GC_BLUE);
        maxx = GC_maxx(gc);
        maxy = GC_maxy(gc);
        GC_writec(gc,0,0,attr,'É');
        GC_writec(gc,maxx,0,attr,'»');
        GC_writec(gc,maxx,maxy,attr,'¼');
        GC_writec(gc,0,maxy,attr,'È');
        for (i = 1; i < maxx; i++) {
            GC_writec(gc,i,0,attr,'Í');
            GC_writec(gc,i,maxy,attr,'Í');
            }
        for (i = 1; i < maxy; i++) {
            GC_writec(gc,0,i,attr,'º');
            GC_writec(gc,maxx,i,attr,'º');
            }
        x = GC_maxx(gc)/2 - 30;
        y = GC_maxy(gc)/2 - 9;
        sprintf(buf,"Text Mode: %d x %d (%dx%d character cell)",
            modeInfo.XResolution / modeInfo.XCharSize,
            modeInfo.YResolution / modeInfo.YCharSize,
            modeInfo.XCharSize, modeInfo.YCharSize);
        GC_gotoxy(gc,x,y);
        GC_puts(gc,buf); GC_gotoxy(gc,x,y+=2);
        GC_puts(gc,"Adjust mode parameters with the following keys:"); GC_gotoxy(gc,x,y+=2);
        GC_puts(gc,"      \x1B Move image left"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      \x1A Move image right"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      \x18 Move image up"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      \x19 Move image down"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc," Ctrl \x1B Decrease image size horizontally"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc," Ctrl \x1A Increase image size horizontally"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      h Toggle horizontal sync polarity"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      v Toggle vertical sync polarity"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      + Increase vertical refresh"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      - Decrease vertical refresh"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"      r Restore original values"); GC_gotoxy(gc,x,y+=1);
        GC_puts(gc,"Press <Enter> to accept changes, ESC to quit without saving"); GC_gotoxy(gc,x,y+=1);
        sprintf(buf,"hSync '%s', vSync '%s'",
            (crtc->Flags & gaHSyncNeg) ? "-" : "+",
            (crtc->Flags & gaVSyncNeg) ? "-" : "+");
        GC_gotoxy(gc,GC_maxx(gc)-20,2);
        GC_puts(gc,buf);
        if (readRefresh)
            ReadRefresh();
        sprintf(buf,"Refresh Rate: %d.%dHz", vRefresh / 10, vRefresh % 10);
        GC_gotoxy(gc,GC_maxx(gc) - 20,1);
        GC_puts(gc,buf);
        }
    else {
        if (modeInfo.BitsPerPixel == 4) {
            ClearPage(0);
            moire(defcolor);
            }
        else {
            min = 32;
            max = 253;
            range = max - min;
            for (i = 0; i < 254; i++) {
                pal[i].Red = 0;
                pal[i].Blue = (((i*range)/254)+min);
                pal[i].Green = 0;
                }
            pal[254].Red = pal[254].Green = pal[254].Blue = 128;
            pal[255].Red = pal[255].Green = pal[255].Blue = 255;
            if (modeInfo.BitsPerPixel > 8) {
                for (i = 0; i < maxY; i++) {
                    p = &pal[(i * 254L) / maxY];
                    SetForeColor(rgbColor(p->Red,p->Green,p->Blue));
                    draw2d.DrawLineInt(0,i,maxX,i,true);
                    }
                }
            else {
                driver.SetPaletteData(pal,256,0,false);
                for (i = 0; i < maxY; i++) {
                    SetForeColor((i * 254L) / maxY);
                    draw2d.DrawLineInt(0,i,maxX,i,true);
                    }
                defcolor = 255;
                }
            SetForeColor(defcolor);
            draw2d.DrawLineInt(0,0,maxX,0,true);
            draw2d.DrawLineInt(0,0,0,maxY,true);
            draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
            draw2d.DrawLineInt(0,maxY,maxX,maxY,true);
            }
        if (maxX < 639 || maxY < 479) {
            x = maxX/2 - 145;
            y = maxY/2 - 65;
            sprintf(buf,"Mode: %d x %d %d bits per pixel",(int)maxX+1,(int)maxY+1,(int)modeInfo.BitsPerPixel);
            WriteText(x,y,buf,defcolor);    y += 32;
            WriteText(x,y,"     \x1B Move image left",defcolor); y += 16;
            WriteText(x,y,"     \x1A Move image right",defcolor); y += 16;
            WriteText(x,y,"     \x18 Move image up",defcolor); y += 16;
            WriteText(x,y,"     \x19 Move image down",defcolor); y += 16;
            WriteText(x,y,"     r Restore original values",defcolor); y += 16;
            y += 8;
            WriteText(x,y,"Press <Enter> to accept, ESC to quit",defcolor);
            }
        else {
            x = maxX/2 - 240;
            y = maxY/2 - 146;
            sprintf(buf,"Video mode: %d x %d %d bits per pixel",(int)maxX+1,(int)maxY+1,(int)modeInfo.BitsPerPixel);
            WriteText(x,y,buf,defcolor);    y += 32;
            WriteText(x,y,"Adjust mode parameters with the following keys:",defcolor); y += 32;
            WriteText(x,y,"      \x1B Move image left",defcolor); y += 16;
            WriteText(x,y,"      \x1A Move image right",defcolor); y += 16;
            WriteText(x,y,"      \x18 Move image up",defcolor); y += 16;
            WriteText(x,y,"      \x19 Move image down",defcolor); y += 16;
            WriteText(x,y," Ctrl \x1B Decrease image size horizontally",defcolor); y += 16;
            WriteText(x,y," Ctrl \x1A Increase image size horizontally",defcolor); y += 16;
            WriteText(x,y,"      h Toggle horizontal sync polarity",defcolor); y += 16;
            WriteText(x,y,"      v Toggle vertical sync polarity",defcolor); y += 16;
            WriteText(x,y,"      + Increase vertical refresh",defcolor); y += 16;
            WriteText(x,y,"      - Decrease vertical refresh",defcolor); y += 16;
            WriteText(x,y,"      r Restore original values",defcolor); y += 16;
#ifdef DEBUG_BLANKS
            WriteText(x,y,"     [] Adjust horizontal blank start",defcolor); y += 16;
            WriteText(x,y,"     {} Adjust horizontal blank end",defcolor); y += 16;
            WriteText(x,y,"     ,. Adjust vertical blank start",defcolor); y += 16;
            WriteText(x,y,"     <> Adjust vertical blank end",defcolor); y += 16;
#endif
            y += 16;
            WriteText(x,y,"Press <Enter> to accept changes, ESC to quit without saving",defcolor);
            sprintf(buf,"hSync '%s', vSync '%s'",
                (crtc->Flags & gaHSyncNeg) ? "-" : "+",
                (crtc->Flags & gaVSyncNeg) ? "-" : "+");
            WriteText(maxX-180,24,buf,defcolor);
            }
        if (readRefresh)
            ReadRefresh();
        sprintf(buf,"Refresh Rate: %d.%dHz", vRefresh / 10, vRefresh % 10);
        WriteText(maxX-180,8,buf,defcolor);
        }
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doCenter(
    GC_devCtx *_gc,
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
    ibool       updateTimings,updateRefresh;
    int         ch;
    GA_CRTCInfo defCRTC;
    event_t     evt;
    GC_devCtx   *gc;

    /* Obtain the mode information and set the display mode */
    GC_leave(_gc);
    dc = _gc->dc;
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
    gc = GC_create(dc,-1,-1,GC_FONT_8X8);
    GC_cursorOff(gc);
    cntMode = flags;
    if (modeInfo.BitsPerPixel >= 4) {
        if (!InitSoftwareRasterizer(cntDevice,1,false))
            PM_fatalError("Unable to initialize reference rasteriser!");
        }

    /* Draw the background image */
    init.GetCRTCTimings(crtc);
    init.SaveCRTCTimings(crtc);
    defCRTC = *crtc;
    refreshRate = crtc->RefreshRate / 100;
    drawBackground(gc,crtc,true);

    /* Now process key events */
    for (;;) {
        EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
        ch = EVT_scanCode(evt.message);
        if (ch == KB_esc || ch == KB_Q) {
            /* Restore the original timings */
            GA_restoreCRTCTimings(dc,init.GetActiveHead());
            break;
            }
        if (ch == KB_enter) {
            /* Save the timing changes */
            GA_saveCRTCTimings(dc,init.GetActiveHead());
            break;
            }

        /* Now handle the key event */
        updateTimings = false;
        updateRefresh = false;
        switch (ch) {
            case KB_H:          // Toggle hsync polarity
                crtc->Flags ^= gaHSyncNeg;
                drawBackground(gc,crtc,false);
                updateTimings = true;
                break;
            case KB_V:          // Toggle vsync polarity
                crtc->Flags ^= gaVSyncNeg;
                drawBackground(gc,crtc,false);
                updateTimings = true;
                break;
            case KB_R:          // reset to initial values
                *crtc = defCRTC;
                updateTimings = true;
                updateRefresh = true;
                break;
            case KB_D:          // Dump CRTC values to file
                dumpCRTCValues(crtc);
                break;
            case KB_equals:
            case KB_padPlus:
                if (useGTF) {
                    refreshRate++;
                    if (GA_computeCRTCTimings(dc,init.GetActiveHead(),&modeInfo,refreshRate,crtc->Flags & gaInterlaced,crtc,false)) {
                        updateTimings = true;
                        updateRefresh = true;
                        }
                    }
                else if (crtc->PixelClock + 250000 <= modeInfo.MaxPixelClock) {
                    crtc->PixelClock += 250000;
                    updateTimings = true;
                    updateRefresh = true;
                    }
                break;
            case KB_minus:
            case KB_padMinus:
                if (useGTF) {
                    refreshRate--;
                    if (GA_computeCRTCTimings(dc,init.GetActiveHead(),&modeInfo,refreshRate,crtc->Flags & gaInterlaced,crtc,false)) {
                        updateTimings = true;
                        updateRefresh = true;
                        }
                    }
                else {
                    crtc->PixelClock -= 250000;
                    updateTimings = true;
                    updateRefresh = true;
                    }
                break;
            case KB_up:
                if (evt.modifiers & EVT_CTRLSTATE) {
                    // Increase image size vertically
                    if (crtc->VerticalTotal > (modeInfo.YResolution + MIN_VBLANK_TIME)) {
                        crtc->VerticalTotal--;
                        updateTimings = true;
                        }
                    }
                else {
                    // Move the image up
                    if (crtc->VerticalSyncEnd < (crtc->VerticalTotal - MIN_VSYNC_END)) {
                        crtc->VerticalSyncStart++;
                        crtc->VerticalSyncEnd++;
                        updateTimings = true;
                        }
                    }
                break;
            case KB_down:
                if (evt.modifiers & EVT_CTRLSTATE) {
                    // Decrease image size vertically
                    crtc->VerticalTotal++;
                    updateTimings = true;
                    }
                else {
                    // Move the image down
                    if (crtc->VerticalSyncStart > (modeInfo.YResolution + MIN_VSYNC_START)) {
                        crtc->VerticalSyncStart--;
                        crtc->VerticalSyncEnd--;
                        updateTimings = true;
                        }
                    }
                break;
            case KB_left:
                if (evt.modifiers & EVT_CTRLSTATE) {
                    // Decrease image size horizontally
                    crtc->HorizontalTotal += HSTEP;
                    updateTimings = true;
                    }
                else {
                    // Move the image left
                    if (crtc->HorizontalSyncEnd < (crtc->HorizontalTotal - MIN_HSYNC_END)) {
                        crtc->HorizontalSyncStart += HSTEP;
                        crtc->HorizontalSyncEnd += HSTEP;
                        updateTimings = true;
                        }
                    }
                break;
            case KB_right:
                if (evt.modifiers & EVT_CTRLSTATE) {
                    // Increase image size horizontally
                    if (crtc->HorizontalTotal > (modeInfo.XResolution + MIN_HBLANK_TIME)) {
                        crtc->HorizontalTotal -= HSTEP;
                        updateTimings = true;
                        }
                    }
                else {
                    // Move the image right
                    if (crtc->HorizontalSyncStart > (modeInfo.XResolution + MIN_HSYNC_START)) {
                        crtc->HorizontalSyncStart -= HSTEP;
                        crtc->HorizontalSyncEnd -= HSTEP;
                        updateTimings = true;
                        }
                    }
                break;
#ifdef DEBUG_BLANKS
            case KB_leftSquareBrace:
                if (evt.modifiers & EVT_SHIFTKEY)
                    hBlankEnd -= 8;
                else
                    hBlankStart -= 8;
                updateTimings = true;
                break;
            case KB_rightSquareBrace:
                if (evt.modifiers & EVT_SHIFTKEY)
                    hBlankEnd += 8;
                else
                    hBlankStart += 8;
                updateTimings = true;
                break;
            case KB_comma:
                if (evt.modifiers & EVT_SHIFTKEY)
                    vBlankEnd -= 8;
                else
                    vBlankStart -= 8;
                updateTimings = true;
                break;
            case KB_period:
                if (evt.modifiers & EVT_SHIFTKEY)
                    vBlankEnd += 8;
                else
                    vBlankStart += 8;
                updateTimings = true;
                break;
#endif
            }

        /* Update the timings if they have changed */
        if (updateTimings) {
            init.SaveCRTCTimings(crtc);
            if (updateRefresh)
                drawBackground(gc,crtc,true);
            }
        }

    /* Return to text mode, restore the state of the console and exit */
    if (modeInfo.BitsPerPixel >= 4)
        ExitSoftwareRasterizer();
    GC_destroy(gc);
    GC_restore(_gc);
    (void)gl;
    return true;
}

