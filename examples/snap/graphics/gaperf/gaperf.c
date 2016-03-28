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
* Description:  Simple program to profile the speed of certain operations
*               for the SciTech SNAP Graphics Architecture. This is
*               a great way to test the performance of different SuperVGA
*               cards and different compiler configurations.
*
*               Note, this library uses the Zen Timer Library for
*               microsecond accuracy timing of the routines.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "profile.h"
#include "pmapi.h"
#include "ztimer.h"

/*--------------------------- Global variables ----------------------------*/

#define MAXLINES        3000
#define LINES_PER_GROUP 200
#define MAX_IMAGES      20

static int             x_arr[MAXLINES];
static int             y_arr[MAXLINES];

/* Simple test patterns */

static GA_pattern   testPat[8] = {
    {{0x7F, 0xFF, 0xF7, 0xFF, 0x7F, 0xFF, 0xF7, 0xFF}},
    {{0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77}},
    {{0xB1, 0x30, 0x03, 0x1B, 0xD8, 0xC0, 0x0C, 0x8D}},
    {{0x80, 0x10, 0x02, 0x20, 0x01, 0x08, 0x40, 0x04}},
    {{0x80, 0x40, 0x20, 0x00, 0x02, 0x04, 0x08, 0x00}},
    {{0x82, 0x44, 0x39, 0x44, 0x82, 0x01, 0x01, 0x01}},
    {{0xF8, 0x74, 0x22, 0x47, 0x8F, 0x17, 0x22, 0x71}},
    {{0x55, 0xA0, 0x40, 0x40, 0x55, 0x0A, 0x04, 0x04}},
    };

static GA_colorPattern  testColorPat[8];
static GA_color         testColorTrans[8];

/* Simple mono test image */

#define imageByteWidth  4
static uchar imageMSB[] = {
    0xFF, 0xDE, 0x7B, 0xFE,  /* 11111111 11011110 01111011 11111110 */
    0xFF, 0xCC, 0x31, 0xFE,  /* 11111111 11001100 00110001 11111110 */
    0x8C, 0x4C, 0x31, 0x82,  /* 10001100 01001100 00110001 10000010 */
    0x0C, 0x0C, 0x31, 0x80,  /* 00001100 00001100 00110001 10000000 */
    0x0C, 0x0C, 0x31, 0x90,  /* 00001100 00001100 00110001 10010000 */
    0x0C, 0x0F, 0xF1, 0xF0,  /* 00001100 00001111 11110001 11110000 */
    0x0C, 0x0F, 0xF1, 0xF0,  /* 00001100 00001111 11110001 11110000 */
    0x0C, 0x0C, 0x31, 0x90,  /* 00001100 00001100 00110001 10010000 */
    0x0C, 0x0C, 0x31, 0x80,  /* 00001100 00001100 00110001 10000000 */
    0x0C, 0x0C, 0x31, 0x82,  /* 00001100 00001100 00110001 10000010 */
    0x0C, 0x0C, 0x31, 0xFE,  /* 00001100 00001100 00110001 11111110 */
    0x1E, 0x1E, 0x7B, 0xFE,  /* 00011110 00011110 01111011 11111110 */
    0x00, 0x00, 0x00, 0x00,  /* 00000000 00000000 00000000 00000000 */
    };
static int imageWidth = imageByteWidth * 8;
static int imageHeight = sizeof(imageMSB)/imageByteWidth;

/*----------------------------- Implementation ----------------------------*/

/* External assembly language routine to perform a full screen Blt from
 * system memory, using fast REP MOVSD strings instructions for entire
 * 64k banks - about as optimal as you will ever get for a full screen
 * Blt.
 */

void _ASMAPI readBufLin(void *buffer,uint size);
void _ASMAPI clearBufLin(void *buffer,N_int32 value,uint size);
void _ASMAPI clearBufDownLin(void *buffer,N_int32 value,uint size);
void _ASMAPI copyBufLin(void *buffer,char *image,uint size);

#define PROFILE_MBS(func,name)              \
{                                           \
    int count = 0;                          \
    float time;                             \
    LZTimerOn();                            \
    do {                                    \
        func;                               \
        count++;                            \
        } while (LZTimerLap() < 2000000);   \
    if (drv_state2d.WaitTillIdle)           \
        drv_state2d.WaitTillIdle();         \
    LZTimerOff();                           \
    time = LZTimerCount() * LZTIMER_RES;    \
    R->name##MbPerSec = ((float)count * size) / (1048576.0 * time); \
}

static int _random(int max)
{
    return (rand() % (max+1));
}

/****************************************************************************
REMARKS:
Draws a bunch of colored accelerated rectangles on the display.
****************************************************************************/
static void drawRects(void)
{
    int x,y,i,j,width,height;
    int color;

    SetMix(GA_REPLACE_MIX);
    ClearPage(realColor(7));
    width = 2 * ((maxX+1) / 46)-1;
    height = 2 * ((maxY-10)  / 47)-1;
    x = width / 2;
    y = height / 2;
    color = 1;
    for (j = 0; j < 16; j++) {
        for (i = 0; i < 16; i++) {
            SetForeColor(realColor(color++));
            draw2d.DrawRect(x,y,width,height);
            x += (width/2) * 3;
            }
        y += (height / 2) * 3;
        x = width / 2;
        }
}

/****************************************************************************
REMARKS:
Test the speed of line drawing in the specified video mode. We blast out a
whole bunch of random lines as fast as possible.
****************************************************************************/
static void profileIntLines(
    ProfileRecord *R)
{
    int     i,j,count = 0;
    float   time;

    srand(1000);
    for (i = 0; i < MAXLINES; i++) {
        x_arr[i] = _random(maxX);
        y_arr[i] = _random(maxY);
        }

    i = 0;
    LZTimerOn();
    do {
        for (j = 0; j < LINES_PER_GROUP; j++) {
            SetForeColor(count);
            draw2d.DrawLineInt(x_arr[i],y_arr[i],x_arr[i+1],y_arr[i+1],true);
            count++;
            if (++i >= (MAXLINES-1))
                i = 0;
            }
        } while (LZTimerLap() < 2000000);
    if (drv_state2d.WaitTillIdle)
        drv_state2d.WaitTillIdle();
    LZTimerOff();
    time = LZTimerCount() * LZTIMER_RES;
    R->intLinesPerSec = count / time;
}

// Macro to swap the value of two integer variables

#define SWAP(a,b)       { a^=b; b^=a; a^=b; }

/****************************************************************************
PARAMETERS:
x1          - X1 coordinate
y1          - Y1 coordinate
x2          - X2 coordinate
y2          - Y2 coordinate
drawFirst   - True to draw first pixel
drawLast    - True to draw last pixel

REMARKS:
Draws a line using the bresenham line engine in SNAP. We just do a
full integer based line here.
****************************************************************************/
static void DrawBresenhamLine(
    N_int32 x1,
    N_int32 y1,
    N_int32 x2,
    N_int32 y2,
    ibool drawFirst,
    ibool drawLast)
{
    int absDeltaX,absDeltaY,majorInc,diagInc;
    int initialError,flags;

    /* Compute bresenham line engine parameters */
    flags = gaLineXPositive | gaLineYPositive | gaLineXMajor;
    if (drawLast)
        flags |= gaLineDoLastPel;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        flags &= ~gaLineXPositive;
        }
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        flags &= ~gaLineYPositive;
        }
    if (absDeltaY > absDeltaX) {
        SWAP(absDeltaX,absDeltaY);
        flags &= ~gaLineXMajor;
        }
    majorInc = 2 * absDeltaY;               // 2 * dy
    initialError = majorInc - absDeltaX;    // 2 * dy - dx
    diagInc = initialError - absDeltaX;     // 2 * (dy - dx)

    // Skip first pixel if desired
    if (!drawFirst) {
        if (flags & gaLineXMajor) {
            if (flags & gaLineXPositive)
                x1++;
            else
                x1--;
            }
        else {
            if (flags & gaLineYPositive)
                y1++;
            else
                y1--;
            }
        if (initialError <= 0) {
            initialError += majorInc;
            }
        else {
            initialError += diagInc;
            if (flags & gaLineXMajor) {
                if (flags & gaLineYPositive)
                    y1++;
                else
                    y1--;
                }
            else {
                if (flags & gaLineXPositive)
                    x1++;
                else
                    x1--;
                }
            }
        absDeltaX--;
        }

    /* Now draw the line */
    draw2d.DrawBresenhamLine(x1,y1,initialError,majorInc,diagInc,absDeltaX+1,flags);
}

/****************************************************************************
REMARKS:
Test the speed of line drawing in the specified video mode. We blast out a
whole bunch of random lines as fast as possible.
****************************************************************************/
static void profileBresenhamLines(
    ProfileRecord *R)
{
    int     i,j,count = 0;
    float   time;

    srand(1000);
    for (i = 0; i < MAXLINES; i++) {
        x_arr[i] = _random(maxX);
        y_arr[i] = _random(maxY);
        }

    i = 0;
    LZTimerOn();
    do {
        for (j = 0; j < LINES_PER_GROUP; j++) {
            SetForeColor(count);
            DrawBresenhamLine(x_arr[i],y_arr[i],x_arr[i+1],y_arr[i+1],true,true);
            count++;
            if (++i >= (MAXLINES-1))
                i = 0;
            }
        } while (LZTimerLap() < 2000000);
    if (drv_state2d.WaitTillIdle)
        drv_state2d.WaitTillIdle();
    LZTimerOff();
    time = LZTimerCount() * LZTIMER_RES;
    R->bresLinesPerSec = count / time;
}

/****************************************************************************
REMARKS:
Test the speed of line drawing in the specified video mode. We blast out a
whole bunch of random lines as fast as possible.
****************************************************************************/
static void profileIntStippleLines(
    ProfileRecord *R)
{
    int     i,j,count = 0;
    float   time;

    srand(1000);
    for (i = 0; i < MAXLINES; i++) {
        x_arr[i] = _random(maxX);
        y_arr[i] = _random(maxY);
        }

    SetLineStipple(0x0C3F);
    i = 0;
    LZTimerOn();
    do {
        for (j = 0; j < LINES_PER_GROUP; j++) {
            SetForeColor(count);
            draw2d.DrawStippleLineInt(x_arr[i],y_arr[i],x_arr[i+1],y_arr[i+1],true,false);
            count++;
            if (++i >= (MAXLINES-1))
                i = 0;
            }
        } while (LZTimerLap() < 2000000);
    if (drv_state2d.WaitTillIdle)
        drv_state2d.WaitTillIdle();
    LZTimerOff();
    time = LZTimerCount() * LZTIMER_RES;
    R->intStippleLinesPerSec = count / time;
}

/****************************************************************************
PARAMETERS:
x1          - X1 coordinate
y1          - Y1 coordinate
x2          - X2 coordinate
y2          - Y2 coordinate
drawFirst   - True to draw first pixel
drawLast    - True to draw last pixel

REMARKS:
Draws a line using the bresenham line engine in SNAP. We just do a
full integer based line here.
****************************************************************************/
static void DrawBresenhamStippleLine(
    N_int32 x1,
    N_int32 y1,
    N_int32 x2,
    N_int32 y2,
    ibool drawFirst,
    ibool drawLast,
    ibool transparent)
{
    int absDeltaX,absDeltaY,majorInc,diagInc;
    int initialError,flags;

    /* Compute bresenham line engine parameters */
    flags = gaLineXPositive | gaLineYPositive | gaLineXMajor;
    if (drawLast)
        flags |= gaLineDoLastPel;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        flags &= ~gaLineXPositive;
        }
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        flags &= ~gaLineYPositive;
        }
    if (absDeltaY > absDeltaX) {
        SWAP(absDeltaX,absDeltaY);
        flags &= ~gaLineXMajor;
        }
    majorInc = 2 * absDeltaY;               // 2 * dy
    initialError = majorInc - absDeltaX;    // 2 * dy - dx
    diagInc = initialError - absDeltaX;     // 2 * (dy - dx)

    // Skip first pixel if desired
    if (!drawFirst) {
        if (flags & gaLineXMajor) {
            if (flags & gaLineXPositive)
                x1++;
            else
                x1--;
            }
        else {
            if (flags & gaLineYPositive)
                y1++;
            else
                y1--;
            }
        if (initialError <= 0) {
            initialError += majorInc;
            }
        else {
            initialError += diagInc;
            if (flags & gaLineXMajor) {
                if (flags & gaLineYPositive)
                    y1++;
                else
                    y1--;
                }
            else {
                if (flags & gaLineXPositive)
                    x1++;
                else
                    x1--;
                }
            }
        absDeltaX--;
        }

    /* Now draw the line */
    draw2d.DrawBresenhamStippleLine(x1,y1,initialError,majorInc,diagInc,absDeltaX+1,flags,transparent);
}

/****************************************************************************
REMARKS:
Test the speed of line drawing in the specified video mode. We blast out a
whole bunch of random lines as fast as possible.
****************************************************************************/
static void profileBresenhamStippleLines(
    ProfileRecord *R)
{
    int     i,j,count = 0;
    float   time;

    srand(1000);
    for (i = 0; i < MAXLINES; i++) {
        x_arr[i] = _random(maxX);
        y_arr[i] = _random(maxY);
        }

    SetLineStipple(0x0C3F);
    i = 0;
    LZTimerOn();
    do {
        for (j = 0; j < LINES_PER_GROUP; j++) {
            SetForeColor(count);
            DrawBresenhamStippleLine(x_arr[i],y_arr[i],x_arr[i+1],y_arr[i+1],true,true,false);
            count++;
            if (++i >= (MAXLINES-1))
                i = 0;
            }
        } while (LZTimerLap() < 2000000);
    if (drv_state2d.WaitTillIdle)
        drv_state2d.WaitTillIdle();
    LZTimerOff();
    time = LZTimerCount() * LZTIMER_RES;
    R->bresStippleLinesPerSec = count / time;
}

/****************************************************************************
REMARKS:
Copys a linear buffer and increments the index of the buffer to be copied
to the next buffer. We wrap around at the maximum buffer limit.
****************************************************************************/
static void copyBuf(
    int *i,
    int maxImages,
    char *dst,
    char *src,
    uint size)
{
    copyBufLin(dst,src,size);
    if (++*i >= maxImages)
        *i = 0;
}

/****************************************************************************
REMARKS:
Test the speed of blitting full size image from system RAM to video RAM.

NOTE: The bitBlt'ing routine used blt's and entire display memory frame at
a time, which is as optimal as you can get. Thus the results of this
profiling test will give you a good idea of what you can expect as the
absolute best case in real world performance.

NOTE: In order to thrash the system RAM cache, so that we can determine the
RAW blitting performance we allocate a number of system memory buffers and
cycle through each one (only in 32 bit PM version however)
****************************************************************************/
static void profileCPUSysBlt(
    ProfileRecord *R)
{
    int     i,maxImages;
    char    *image[MAX_IMAGES];
    int     size = modeInfo.BytesPerScanLine * (maxY+1);

    maxImages = ((1024 * 1024U) / size) + 1;
    for (i = 0; i < maxImages; i++)
        image[i] = malloc(size);

    drawRects();
    PROFILE_MBS(readBufLin(dc->LinearMem,size),                                 readsCPU);
    PROFILE_MBS(copyBuf(&i,maxImages,image[i % maxImages],dc->LinearMem,size),  getImagesCPU);
    ClearPage(0);
    PROFILE_MBS(copyBuf(&i,maxImages,dc->LinearMem,image[i % maxImages],size),  putImagesCPU);

    for (i = 0; i < maxImages; i++)
        free(image[i]);
}

/****************************************************************************
REMARKS:
Copys a linear buffer and increments the index of the buffer to be copied
to the next buffer. We wrap around at the maximum buffer limit.
****************************************************************************/
static void bitBltSys(
    int *i,
    int maxImages,
    char *src,
    uint size)
{
    draw2d.BitBltSys(src,modeInfo.BytesPerScanLine,0,0,maxX+1,maxY+1,0,0,GA_REPLACE_MIX,false);
    if (++*i >= maxImages)
        *i = 0;
}

/****************************************************************************
REMARKS:
Test the speed of blitting full size image from system RAM to video RAM.

NOTE: The bitBlt'ing routine used blt's and entire display memory frame at
a time, which is as optimal as you can get. Thus the results of this
profiling test will give you a good idea of what you can expect as the
absolute best case in real world performance.

NOTE: In order to thrash the system RAM cache, so that we can determine the
RAW blitting performance we allocate a number of system memory buffers and
cycle through each one (only in 32 bit PM version however)
****************************************************************************/
static void profileSysBlt(
    ProfileRecord *R)
{
    int     i,maxImages;
    char    *image[MAX_IMAGES];
    int     size = modeInfo.BytesPerScanLine * (maxY+1);

    maxImages = ((1024 * 1024U) / size) + 1;
    for (i = 0; i < maxImages; i++)
        image[i] = malloc(size);

    drawRects();
    for (i = 0; i < maxImages; i++)
        copyBufLin(image[i],dc->LinearMem,size);
    ClearPage(0);
    PROFILE_MBS(bitBltSys(&i,maxImages,image[i % maxImages],size),bitBltSys);

    for (i = 0; i < maxImages; i++)
        free(image[i]);
}

/****************************************************************************
REMARKS:
Test the speed of screen clearing to a specific color directly over the
PCI/AGP bus using the CPU.
****************************************************************************/
static void profileCPUClears(
    ProfileRecord *R)
{
    int size = modeInfo.BytesPerScanLine * (maxY+1);

    /* Profile standard clears with increasing memory addresses */
    PROFILE_MBS(clearBufLin(dc->LinearMem,count,size),      clearCPU);
    PROFILE_MBS(clearBufDownLin(dc->LinearMem,count,size),  reverseClearCPU);
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void ClearScreen(
    GA_color color)
{
    SetForeColor(color);
    draw2d.DrawRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileClears(
    ProfileRecord *R)
{
    int size = modeInfo.BytesPerScanLine * (maxY+1);

    PROFILE_MBS(ClearScreen(count), clears);
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void ClearMonoPatt(
    GA_color color)
{
    SetForeColor(color);
    SetBackColor(color+1);
    Use8x8MonoPattern(color & 7);
    draw2d.DrawPattRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileMonoPatt(
    ProfileRecord *R)
{
    int i,size = modeInfo.BytesPerScanLine * (maxY+1);

    for (i = 0; i < 8; i++)
        Set8x8MonoPattern(i,&testPat[i]);
    PROFILE_MBS(ClearMonoPatt(count),   monoPatt);
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void ClearTransMonoPatt(
    GA_color color)
{
    SetForeColor(color);
    SetBackColor(color+1);
    Use8x8TransMonoPattern(color & 7);
    draw2d.DrawPattRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileTransMonoPatt(
    ProfileRecord *R)
{
    int i,size = modeInfo.BytesPerScanLine * (maxY+1);

    for (i = 0; i < 8; i++)
        Set8x8MonoPattern(i,&testPat[i]);
    PROFILE_MBS(ClearTransMonoPatt(count),   transMonoPatt);
}

/****************************************************************************
REMARKS:
Sets up the color test pattern
****************************************************************************/
static void setupColorPatterns(void)
{
    int         i,j,index;
    GA_color    c,fore,back;
    uchar       mask;
    GA_pattern  *pat;

    for (index = 0; index < 8; index++) {
        fore = realColor(index+2);
        back = realColor(index+4);
        if (index & 1)
            testColorTrans[index] = fore;
        else
            testColorTrans[index] = back;
        pat = &testPat[index];
        for (i = 0; i < 8; i++) {
            mask = pat->p[i];
            for (j = 0; j < 8; j++) {
                switch (modeInfo.BitsPerPixel) {
                    case 4:
                    case 8:
                        if (mask & 0x1)
                            testColorPat[index].b8.p[j][i] = fore;
                        else
                            testColorPat[index].b8.p[j][i] = back;
                        break;
                    case 15:
                    case 16:
                        if (mask & 0x1)
                            testColorPat[index].b16.p[j][i] = fore;
                        else
                            testColorPat[index].b16.p[j][i] = back;
                        break;
                    case 24:
                        if (mask & 0x1)
                            c = fore;
                        else
                            c = back;
                        testColorPat[index].b24.p[j][i][0] = c;
                        testColorPat[index].b24.p[j][i][1] = c >> 8;
                        testColorPat[index].b24.p[j][i][2] = c >> 16;
                        break;
                    case 32:
                        if (mask & 0x1)
                            testColorPat[index].b32.p[j][i] = fore;
                        else
                            testColorPat[index].b32.p[j][i] = back;
                        break;
                    }
                mask >>= 1;
                }
            }
        }
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void ClearColorPatt(
    int count)
{
    Use8x8ColorPattern(count & 7);
    draw2d.DrawColorPattRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileColorPatt(
    ProfileRecord *R)
{
    int i,size = modeInfo.BytesPerScanLine * (maxY+1);

    setupColorPatterns();
    for (i = 0; i < 8; i++)
        Set8x8ColorPattern(i,&testColorPat[i]);
    PROFILE_MBS(ClearColorPatt(count),   colorPatt);
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void ClearTransColorPatt(
    int count)
{
    Use8x8TransColorPattern(count & 7,testColorTrans[count & 7]);
    draw2d.DrawColorPattRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileTransColorPatt(
    ProfileRecord *R)
{
    int i,size = modeInfo.BytesPerScanLine * (maxY+1);

    setupColorPatterns();
    for (i = 0; i < 8; i++)
        Set8x8ColorPattern(i,&testColorPat[i]);
    PROFILE_MBS(ClearTransColorPatt(count),   transColorPatt);
}

/****************************************************************************
REMARKS:
Clears the framebuffer using the SNAP driver.
****************************************************************************/
static void BltScreen(void)
{
    static int  flipFlop = 0;
    int         width = maxX+1-10;
    int         height = maxY+1-10;

    if (flipFlop)
        draw2d.BitBlt(10,10,width,height,5,5,GA_REPLACE_MIX);
    else
        draw2d.BitBlt(5,5,width,height,10,10,GA_REPLACE_MIX);
    flipFlop ^= 1;
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profileBitBlt(
    ProfileRecord *R)
{
    int width = maxX+1-10;
    int height = maxY+1-10;
    int size = width * bytesPerPixel * height;

    drawRects();
    PROFILE_MBS(BltScreen(), bitBlt);
}

/****************************************************************************
REMARKS:
Display a bunch of monochrome images
****************************************************************************/
static void PutMonoImage(
    int count)
{
    int         x,y;
    GA_color    color,backColor;

    if (draw2d.MonoBltSys) {
        /* New mono blt function in newer drivers */
        backColor = realColor(count % 16);
        for (x = 0; x < maxX - imageWidth; x += imageWidth)
            for (y = 0; y < maxY - imageHeight; y += imageHeight) {
                color = realColor((x + y) % 16);
                draw2d.MonoBltSys(x,y,imageWidth,imageHeight,imageByteWidth,imageMSB,0,color,backColor,GA_REPLACE_MIX,0);
                }
        }
    else {
        /* Older obsolete functions in older drivers */
        SetBackColor(realColor(count % 16));
        for (x = 0; x < maxX - imageWidth; x += imageWidth)
            for (y = 0; y < maxY - imageHeight; y += imageHeight) {
                SetForeColor(realColor((x + y) % 16));
                draw2d.PutMonoImageMSBSys_Old(x,y,imageWidth,imageHeight,imageByteWidth,imageMSB,false);
                }
        }
}

/****************************************************************************
REMARKS:
Test the speed of clearing the screen with the hardware accelerator.
****************************************************************************/
static void profilePutMonoImage(
    ProfileRecord *R)
{
    int width = maxX+1-imageWidth;
    int height = maxY+1-imageHeight;
    int size = width * bytesPerPixel * height;

    PROFILE_MBS(PutMonoImage(count), putMonoImage);
}

/****************************************************************************
REMARKS:
Finds the baseline values for clearing and moving system memory buffers for
comparison purposes.
****************************************************************************/
static void profileBaseLine(
    ProfileRecord *R)
{
    void    *buffer;
    char    *image;
    uint    size;

    if (R->thrashCache)
        size = 1024 * 1024U;     /* Large memory buffer to thrash cache */
    else
        size = 64000U;
    buffer = malloc(size);
    image = malloc(size);

    PROFILE_MBS(clearBufLin(buffer,count,size),     clearBaseLine);
    PROFILE_MBS(clearBufDownLin(buffer,count,size), reverseClearBaseLine);
    PROFILE_MBS(readBufLin(buffer,size),            readBaseLine);
    PROFILE_MBS(copyBufLin(buffer,image,size),      copyBaseLine);

    free(buffer);
    free(image);
}

/****************************************************************************
REMARKS:
Delay for one second.
****************************************************************************/
static void delayOneSecond(void)
{
    LZTimerOn();
    while (LZTimerLap() < 1000000)
        ;
    LZTimerOff();
}

/****************************************************************************
REMARKS:
Initialise the graphics mode with the specified resolution and
color depth.
****************************************************************************/
static int InitGraphics(
    int x,
    int y,
    int bits)
{
    int         oldMode;
    N_uint16    *modes;

    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (modeInfo.BitsPerPixel < 4)
            continue;
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits) {
            cntMode = *modes;
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                cntMode |= gaLinearBuffer;
            oldMode = init.GetVideoMode();
            virtualX = virtualY = bytesPerLine = -1;
            if (init.SetVideoMode(cntMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0)
                return -1;
            InitSoftwareRasterizer(0,1,false);
            return oldMode;
            }
        }
    return -1;
}

#define CHECK_TEST(name)    (stricmp(testName,"All") == 0 || stricmp(testName,name) == 0)

/****************************************************************************
REMARKS:
Main function to do the performance profiling.
****************************************************************************/
ibool ProfileIt(
    int x,
    int y,
    int bits,
    char *testName,
    ProfileRecord *R)
{
    int oldMode;
    int thrashCache = R->thrashCache;

    /* Clear the profile record results */
    memset(R,0,sizeof(*R));
    R->thrashCache = thrashCache;

    /* Get requested resolution, start graphics and draw pattern */
    if ((oldMode = InitGraphics(x,y,bits)) != -1) {
        delayOneSecond();
        if (CHECK_TEST("Lines"))
            profileIntLines(R);
        if (CHECK_TEST("BresenhamLines"))
            profileBresenhamLines(R);
        if (CHECK_TEST("StippleLines"))
            profileIntStippleLines(R);
        if (CHECK_TEST("BresenhamStippleLines"))
            profileBresenhamStippleLines(R);
        if (modeInfo.BitsPerPixel > 4) {
            // TODO: Make this work for 4bit mode!
            if (CHECK_TEST("CPUSysBlt"))
                profileCPUSysBlt(R);
            if (CHECK_TEST("CPUClears"))
                profileCPUClears(R);
            if (CHECK_TEST("BitBltSys"))
                profileSysBlt(R);
            }
        if (CHECK_TEST("Clears"))
            profileClears(R);
        if (CHECK_TEST("MonoPatt"))
            profileMonoPatt(R);
        if (CHECK_TEST("TransMonoPatt"))
            profileTransMonoPatt(R);
        if (CHECK_TEST("ColorPatt"))
            profileColorPatt(R);
        if (CHECK_TEST("TransColorPatt"))
            profileTransColorPatt(R);
        if (CHECK_TEST("BitBlt"))
            profileBitBlt(R);
        if (CHECK_TEST("PutMonoImage"))
            profilePutMonoImage(R);
        if (CHECK_TEST("BaseLine"))
            profileBaseLine(R);
        ExitSoftwareRasterizer();
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        return true;
        }
    return false;
}

/****************************************************************************
REMARKS:
Function to compute an overall GA-Mark for the performance results.
****************************************************************************/
float ComputeGAMark(
    ProfileRecord *R)
{
    float   GAMark = 0;

    GAMark += R->intLinesPerSec                                     / 200000;
    GAMark += R->bresLinesPerSec                                    / 200000;
    GAMark += R->intStippleLinesPerSec                              / 900000;
    GAMark += R->bresStippleLinesPerSec                             / 900000;
    GAMark += R->clearsMbPerSec                                     / 200;
    GAMark += R->monoPattMbPerSec                                   / 400;
    GAMark += R->transMonoPattMbPerSec                              / 400;
    GAMark += R->colorPattMbPerSec                                  / 400;
    GAMark += R->putMonoImageMbPerSec                               / 100;
    GAMark += R->bitBltMbPerSec                                     / 100;
    GAMark += R->bitBltSysMbPerSec                                  / 100;

    return GAMark * 5;
}

