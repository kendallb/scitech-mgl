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
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture text mode support.
*
****************************************************************************/

#include "gatest.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the information about the video mode.
****************************************************************************/
static void displayModeInfo(
    GC_devCtx *gc,
    GA_modeInfo *mi)
{
    int     i,maxx,maxy,len,attr = GC_makeAttr(GC_WHITE, GC_BLUE);
    char    buf[80];

    maxx = (mi->XResolution/mi->XCharSize)-1;
    maxy = (mi->YResolution/mi->YCharSize)-1;
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
    sprintf(buf,"Text Mode: %d x %d (%dx%d character cell)",
        mi->XResolution / mi->XCharSize,
        mi->YResolution / mi->YCharSize,
        mi->XCharSize, mi->YCharSize);
    len = strlen(buf);
    GC_gotoxy(gc,(GC_maxx(gc)-len)/2,GC_maxy(gc)/2);
    GC_puts(gc,buf);
}

/****************************************************************************
REMARKS:
Main function to do the interactive tests.
****************************************************************************/
ibool doTextTest(
    GC_devCtx *_gc,
    GA_glCtx *gl,
    int xRes,
    int yRes,
    int bitsPerPixel,
    N_uint32 flags,
    int refreshRate,
    GA_CRTCInfo *crtc,
    N_uint32 planeMask,
    ibool useLSBMode)
{
    GC_devCtx   *gc;
    int         i;

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
    GC_setAttr(gc,GC_makeAttr(GC_YELLOW,GC_BLUE));
    GC_fillText(gc,0, 0, GC_maxx(gc), GC_maxy(gc),
        GC_makeAttr(GC_RED, GC_BLUE), '±');

    displayModeInfo(gc,&modeInfo);
    if (EVT_getch() == 0x1B)
        goto DoneTest;
    GC_setBackground(gc,GC_makeAttr(GC_CYAN, GC_BLUE), '°');
    for (i = 0; i < GC_maxy(gc) + 1; i++)
        GC_scroll(gc,GC_SCROLL_UP, 1);

    displayModeInfo(gc,&modeInfo);
    if (EVT_getch() == 0x1B)
        goto DoneTest;
    GC_setBackground(gc,GC_makeAttr(GC_RED, GC_BLUE), '²');
    for (i = 0; i < GC_maxy(gc) + 1; i++)
        GC_scroll(gc,GC_SCROLL_DOWN, 1);

    displayModeInfo(gc,&modeInfo);
    EVT_getch();

    /* Return to text mode, restore the state of the console and exit */
DoneTest:
    GC_destroy(gc);
    GC_restore(_gc);
    (void)gl;
    return true;
}

