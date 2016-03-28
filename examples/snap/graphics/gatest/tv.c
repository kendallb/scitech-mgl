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
*               Graphics Architecture functions for TV output modes.
*
****************************************************************************/

#include "gatest.h"
#include "ztimer.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Draw the background image for the centering and refresh display
****************************************************************************/
static void drawBackground(void)
{
    char        buf[80];
    int         i,x,y,min,max;
    long        range;
    GA_palette  pal[256],*p;

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
    draw2d.DrawLineInt(maxX/2,0,maxX/2,maxY,true);
    draw2d.DrawLineInt(0,maxY/2,maxX,maxY/2,true);

    x = maxX/2 - 240;
    y = maxY/2 - 146;
    sprintf(buf,"Video mode: %d x %d %d bits per pixel",maxX+1,maxY+1,modeInfo.BitsPerPixel);
    WriteText(x,y,buf,defcolor);    y += 32;
    WriteText(x,y,"Adjust mode parameters with the following keys:",defcolor); y += 32;
    WriteText(x,y,"      \x1B Move image left",defcolor); y += 16;
    WriteText(x,y,"      \x1A Move image right",defcolor); y += 16;
    WriteText(x,y,"      \x18 Move image up",defcolor); y += 16;
    WriteText(x,y,"      \x19 Move image down",defcolor); y += 16;
    WriteText(x,y,"      + Increase brightness",defcolor); y += 16;
    WriteText(x,y,"      - Decrease brightness",defcolor); y += 16;
    WriteText(x,y,"      r Restore original values",defcolor); y += 16;
    y += 16;
    WriteText(x,y,"Press <Enter> to accept changes, ESC to quit without saving",defcolor);
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool CenterTVMode(
    GC_devCtx *gc,
    N_uint32 mode)
{
    GA_options  opt,defOpt;
    GA_TVParams *tv;
    int         ch;
    event_t     evt;

    /* Obtain the mode information and set the display mode */
    GC_leave(gc);
    dc = gc->dc;
    virtualX = virtualY = bytesPerLine = -1;
    modeInfo.dwSize = sizeof(modeInfo);
    if (init.GetVideoModeInfo(mode,&modeInfo) != 0)
        return false;
    if (init.SetVideoMode(mode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0)
        return false;
    cntMode = mode;
    if (!InitSoftwareRasterizer(cntDevice,1,false))
        PM_fatalError("Unable to initialize reference rasteriser!");
    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    defOpt = opt;

    /* Find the appropriate TV parameters block depending on current mode */
    if (modeInfo.XResolution > 640) {
        switch (opt.outputDevice & gaOUTPUT_TVCOLORMASK) {
            case gaOUTPUT_TVPAL:
            case gaOUTPUT_TVPAL_M:
            case gaOUTPUT_TVPAL_60:
            case gaOUTPUT_TVPAL_CN:
            case gaOUTPUT_TVSCART_PAL:
                if (opt.outputDevice & gaOUTPUT_TVOVERSCAN)
                    tv = &opt.TV800PALOver;
                else
                    tv = &opt.TV800PALUnder;
                break;
            case gaOUTPUT_TVNTSC:
            case gaOUTPUT_TVNTSC_J:
            default:
                if (opt.outputDevice & gaOUTPUT_TVOVERSCAN)
                    tv = &opt.TV800NTSCOver;
                else
                    tv = &opt.TV800NTSCUnder;
                break;
            }
        }
    else {
        switch (opt.outputDevice & gaOUTPUT_TVCOLORMASK) {
            case gaOUTPUT_TVPAL:
            case gaOUTPUT_TVPAL_M:
            case gaOUTPUT_TVPAL_60:
            case gaOUTPUT_TVPAL_CN:
            case gaOUTPUT_TVSCART_PAL:
                if (opt.outputDevice & gaOUTPUT_TVOVERSCAN)
                    tv = &opt.TV640PALOver;
                else
                    tv = &opt.TV640PALUnder;
                break;
            case gaOUTPUT_TVNTSC:
            case gaOUTPUT_TVNTSC_J:
            default:
                if (opt.outputDevice & gaOUTPUT_TVOVERSCAN)
                    tv = &opt.TV640NTSCOver;
                else
                    tv = &opt.TV640NTSCUnder;
                break;
            }
        }

    /* Draw the background image */
    drawBackground();

    /* Now process key events */
    for (;;) {
        EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
        ch = EVT_scanCode(evt.message);
        if (ch == KB_esc || ch == KB_Q) {
            opt = defOpt;
            init.SetOptions(&opt);
            break;
            }
        if (ch == KB_enter) {
            /* Save the timing changes */
            GA_saveOptions(dc,&opt);
            break;
            }

        /* Now handle the key event */
        switch (ch) {
            case KB_R:          /* Reset to initial values */
                opt = defOpt;
                init.SetOptions(&opt);
                break;
            case KB_equals:
            case KB_padPlus:    /* Increase brightness */
                tv->brightness++;
                init.SetOptions(&opt);
                break;
            case KB_minus:
            case KB_padMinus:   /* Decrease brightness */
                tv->brightness--;
                init.SetOptions(&opt);
                break;
            case KB_up:         /* Move the image up */
                tv->vPos--;
                init.SetOptions(&opt);
                break;
            case KB_down:       /* Move the image down */
                tv->vPos++;
                init.SetOptions(&opt);
                break;
            case KB_left:       /* Move the image left */
                tv->hPos--;
                init.SetOptions(&opt);
                break;
            case KB_right:      /* Move the image right */
                tv->hPos++;
                init.SetOptions(&opt);
                break;
            }
        }

    /* Return to text mode, restore the state of the console and exit */
    ExitSoftwareRasterizer();
    GC_restore(gc);
    return true;
}

