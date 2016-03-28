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
* Description:  Generic utility funcitons used by the SciTech SNAP
*               Graphics test programs.
*
****************************************************************************/

#include "gatest.h"
#include "pmapi.h"

extern ibool isPortrait;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Sets a 640x480x256 linear framebuffer SVGA graphics mode, or higher bpp if
8bpp mode is not available.
****************************************************************************/
void SetGraphicsMode(
    GC_devCtx *gc)
{
    GA_CRTCInfo     crtc;
    N_uint16        *modes;
    int             oldMode,mode = -1;
    int             depth, depths[] = {8, 16, 15, 32, 24};

    /* Search for the 640x480x256 graphics mode */
    GC_leave(gc);
    dc = gc->dc;
    for (depth = 0; depth < (sizeof(depths) / sizeof(depths[0])); depth++) {
        for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
            modeInfo.dwSize = sizeof(modeInfo);
            if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
                continue;
            if (modeInfo.Attributes & gaIsTextMode)
                continue;
            if ((modeInfo.XResolution == 640 && modeInfo.YResolution == 480) ||
                (isPortrait && modeInfo.XResolution == 480 && modeInfo.YResolution == 640) &&
                modeInfo.BitsPerPixel == depths[depth]) {
                mode = *modes;
                if (modeInfo.Attributes & gaHaveLinearBuffer)
                    mode |= gaLinearBuffer;
                break;
                }
            }
            if (mode != -1)
                break;
        }
    if (mode == -1)
        PM_fatalError("Unable to set 640x480 graphics mode!");

    /* Set text mode and initialise console library */
    virtualX = virtualY = bytesPerLine = -1;
    oldMode = init.GetVideoMode();
    init.SetVideoMode(mode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc);
    cntMode = mode;
    if (!InitSoftwareRasterizer(cntDevice,1,false))
        PM_fatalError("Unable to initialize reference rasteriser!");
}

/****************************************************************************
REMARKS:
Restore the old display mode active before graphics mode was started.
****************************************************************************/
void RestoreMode(
    GC_devCtx *gc)
{
    /* Clean up the software rasteriser on exit */
    if (modeInfo.BitsPerPixel >= 4)
        ExitSoftwareRasterizer();
    GC_restore(gc);
}

/****************************************************************************
REMARKS:
Function to find the name of a display mode from the mode information.
****************************************************************************/
int GetModeName(
    char *buf,
    GA_modeInfo *mi)
{
    char    buf1[80];
    int     attr = mi->Attributes;
    int     pages = mi->MaxBuffers;
    int     xChars,yChars;

    /* Build the name of the mode */
    if (attr & gaIsTextMode) {
        xChars = mi->XResolution / mi->XCharSize;
        yChars = mi->YResolution / mi->YCharSize;
        sprintf(buf,"%dx%d Text Mode (%dx%d character cell)",
            xChars,yChars,mi->XCharSize,mi->YCharSize);
        }
    else {
        if (!(attr & gaHaveLinearBuffer))
            strcpy(buf1,", Banked");
        else
            strcpy(buf1,"");
        sprintf(buf,"%dx%dx%d, %d page%s",mi->XResolution,mi->YResolution,
            mi->BitsPerPixel,pages,buf1);
        if (attr & gaHaveAccel2D)
            strcat(buf,", Accel");
        if (attr & gaHaveTripleBuffer)
            strcat(buf,", TripleBuf");
        if (attr & gaHaveStereo)
            strcat(buf,", Stereo");
        if (attr & gaHaveNonVGAMode)
            strcat(buf,", NonVGA");
        }
    return true;
}

/****************************************************************************
REMARKS:
Initialise the mouse driver
****************************************************************************/
void InitMouse(
    int xRes,
    int yRes)
{
    EVT_setMouseRange(xRes-1,yRes-1);
    EVT_setMousePos(xRes/2,yRes/2);
}

/****************************************************************************
REMARKS:
Gets the current mouse cursor position from the mouse driver.
****************************************************************************/
void GetMousePos(
    int *x,
    int *y)
{
    EVT_getMousePos(x,y);
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to check if a key has
been hit. We check for keydown and keyrepeat events, and we also flush the
event queue of all non keydown events to avoid it filling up.
****************************************************************************/
int EVT_kbhit(void)
{
    int     hit;
    event_t evt;

    hit = EVT_peekNext(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    EVT_flush(~(EVT_KEYDOWN | EVT_KEYREPEAT));
    return hit;
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to read an ASCII keypress
and return it. This function will block until a key is hit.
****************************************************************************/
int EVT_getch(void)
{
    event_t evt;

    do {
        EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
        } while (EVT_asciiCode(evt.message) == 0);
    return EVT_asciiCode(evt.message);
}

/****************************************************************************
REMARKS:
Function to read a string from the keyboard, using the console library
for output.
****************************************************************************/
int GetString(
    GC_devCtx *gc,
    char *str,
    int maxLen)
{
    int x,y,c,len = 0;

    x = GC_wherex(gc);
    y = GC_wherey(gc);
    for (;;) {
        if ((c = EVT_getch()) == 0xD) {
            str[len] = 0;
            GC_printf(gc,"\n");
            return 1;
            }
        if (c == 0x1B)
            return -1;
        if (c == 0x08) {
            if (len > 0) {
                len--;
                x--;
                GC_gotoxy(gc,x,y);
                GC_putc(gc,' ');
                GC_gotoxy(gc,x,y);
                }
            }
        else if (isprint(c)) {
            if (len < maxLen) {
                str[len++] = c;
                GC_gotoxy(gc,x,y);
                GC_putc(gc,c & 0xFF);
                x++;
                }
            }
        }
}

