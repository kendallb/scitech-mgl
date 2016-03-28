/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  any
*
* Description:  Test program to check the ability to generate real mode
*               interrupts and to be able to obtain direct access to the
*               video memory from protected mode. Compile and link with
*               the appropriate command line for your DOS extender.
*
*               Functions tested:   PM_getBIOSSelector()
*                                   PM_mapPhysicalAddr()
*                                   PM_int86()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pmapi.h"

uchar   *bios;          /* Pointer to BIOS data area        */
uchar   *videoPtr;      /* Pointer to VGA framebuffer       */
void    *stateBuf;      /* Console state save buffer        */

/* Routine to return the current video mode number */

int getVideoMode(void)
{
    return PM_getByte(bios+0x49);
}

/* Routine to set a specified video mode */

void setVideoMode(int mode)
{
    RMREGS r;

    r.x.ax = mode;
    PM_int86(0x10, &r, &r);
}

/* Routine to clear a rectangular region on the display by calling the
 * video BIOS.
 */

void clearScreen(int startx, int starty, int endx, int endy, unsigned char attr)
{
    RMREGS r;

    r.x.ax = 0x0600;
    r.h.bh = attr;
    r.h.cl = startx;
    r.h.ch = starty;
    r.h.dl = endx;
    r.h.dh = endy;
    PM_int86(0x10, &r, &r);
}

/* Routine to fill a rectangular region on the display using direct
 * video writes.
 */

#define SCREEN(x,y) (videoPtr + ((y) * 160) + ((x) << 1))

void fill(int startx, int starty, int endx, int endy, unsigned char c,
    unsigned char attr)
{
    unsigned char   *v;
    int             x,y;

    for (y = starty; y <= endy; y++) {
        v = SCREEN(startx,y);
        for (x = startx; x <= endx; x++) {
            *v++ = c;
            *v++ = attr;
            }
        }
}

/* Routine to display a single character using direct video writes */

void writeChar(int x, int y, unsigned char c, unsigned char attr)
{
    unsigned char *v = SCREEN(x,y);
    *v++ = c;
    *v = attr;
}

/* Routine to draw a border around a rectangular area using direct video
 * writes.
 */

static unsigned char border_chars[] = {
    186, 205, 201, 187, 200, 188        /* double box chars */
    };

void border(int startx, int starty, int endx, int endy, unsigned char attr)
{
    unsigned char   *v;
    unsigned char   *b;
    int             i;

    b = border_chars;

    for (i = starty+1; i < endy; i++) {
        writeChar(startx, i, *b, attr);
        writeChar(endx, i, *b, attr);
        }
    b++;
    for (i = startx+1, v = SCREEN(startx+1, starty); i < endx; i++) {
        *v++ = *b;
        *v++ = attr;
        }
    for (i = startx+1, v = SCREEN(startx+1, endy); i < endx; i++) {
        *v++ = *b;
        *v++ = attr;
        }
    b++;
    writeChar(startx, starty, *b++, attr);
    writeChar(endx, starty, *b++, attr);
    writeChar(startx, endy, *b++, attr);
    writeChar(endx, endy, *b++, attr);
}

int main(void)
{
    int     orgMode;
    PM_HWND hwndConsole;

    /* Open the console */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    printf("Hit any key to start 80x25 text mode and perform some direct video output.\n");
    PM_getch();

    /* Allocate a buffer to save console state and save the state */
    if ((stateBuf = PM_malloc(PM_getConsoleStateSize())) == NULL) {
        printf("Unable to allocate console state buffer!\n");
        exit(1);
        }
    PM_saveConsoleState(stateBuf,hwndConsole);
    bios = PM_getBIOSPointer();
    orgMode = getVideoMode();
    setVideoMode(0x3);
    if ((videoPtr = PM_mapPhysicalAddr(0xB8000,0xFFFF,true)) == NULL) {
        printf("Unable to obtain pointer to framebuffer!\n");
        exit(1);
        }

    /* Draw some text on the screen */
    fill(0, 0, 79, 24, 176, 0x1E);
    border(0, 0, 79, 24, 0x1F);
    PM_getch();
    clearScreen(0, 0, 79, 24, 0x7);

    /* Restore the console state on exit */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_free(stateBuf);
    PM_closeConsole(hwndConsole);

    /* Display useful status information */
    printf("\n");
    printf("Original Video Mode = 0x%02X\n", orgMode);
    printf("BIOS Pointer = 0x%08X\n", (int)bios);
    printf("Video Memory = 0x%08X\n", (int)videoPtr);
    return 0;
}

