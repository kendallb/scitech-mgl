/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Environment:  Any
*
* Description:  Simple Multi-Controller sample program showing how to run
*               an MGL application on the secondary graphics controller.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mgraph.h"
#include "ztimer.h"

/*----------------------------- Global Variables --------------------------*/

#define MAX_LINES   6

typedef struct {
    point_t p1,p2;
    point_t d1,d2;
    int     color;
    } vertex;

/* Array of vertices for the lines */
vertex  v[MAX_LINES];

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS;
v       - Array of vertices to initialise
maxx    - Maximum X coordinate for display mode
maxy    - Maximum Y coordinate for display mode

REMARKS:
Initialises the array of vertices.
****************************************************************************/
void initLines(
    vertex *v,
    int maxx,
    int maxy)
{
    int i;

    for (i = 0; i < MAX_LINES; i++) {
        v[i].p1.x = MGL_random(maxx);
        v[i].p1.y = MGL_random(maxy);
        v[i].p2.x = MGL_random(maxx);
        v[i].p2.y = MGL_random(maxy);
        v[i].d1.x = MGL_random(1) ? 1 : -1;
        v[i].d1.y = MGL_random(1) ? -1 : 1;
        v[i].d2.x = MGL_random(1) ? -1 : 1;
        v[i].d2.y = MGL_random(1) ? 1 : -1;
        v[i].color = MGL_randoml(0xFFFFFFFFUL);
        }
}

/****************************************************************************
PARAMETERS;
i       - Index of vertex to move
v       - Array of vertices
maxx    - Maximum X coordinate for display mode
maxy    - Maximum Y coordinate for display mode

REMARKS:
Moves the specific vertex around on the screen, bouncing off the edges.
****************************************************************************/
void moveCoords(
    int i,
    vertex *v,
    int maxx,
    int maxy)
{
    /* change the position of both vertices */
    v[i].p1.x += v[i].d1.x;
    v[i].p1.y += v[i].d1.y;
    v[i].p2.x += v[i].d2.x;
    v[i].p2.y += v[i].d2.y;

    /* change the direction of the first vertex */
    if (v[i].p1.x > maxx)   v[i].d1.x = -1;
    if (v[i].p1.x <= 0)     v[i].d1.x = 1;
    if (v[i].p1.y > maxy)   v[i].d1.y = -1;
    if (v[i].p1.y <= 0)     v[i].d1.y = 1;

    /* change the direction of the second vertex */
    if (v[i].p2.x > maxx)   v[i].d2.x = -1;
    if (v[i].p2.x <= 0)     v[i].d2.x = 1;
    if (v[i].p2.y > maxy)   v[i].d2.y = -1;
    if (v[i].p2.y <= 0)     v[i].d2.y = 1;
}

/****************************************************************************
PARAMETERS;
v       - Array of vertices
maxx    - Maximum X coordinate for display mode
maxy    - Maximum Y coordinate for display mode

REMARKS:
Draws the set of vertices are single pixel wide lines.
****************************************************************************/
void drawLines(
    vertex *v,
    int maxx,
    int maxy)
{
    int i;

    for(i = 0; i < MAX_LINES; i++) {
        moveCoords(i,v,maxx,maxy);
        MGL_setColor(v[i].color);
        MGL_line(v[i].p1,v[i].p2);
        }
}

/****************************************************************************
PARAMETERS;
v       - Array of vertices
maxx    - Maximum X coordinate for display mode
maxy    - Maximum Y coordinate for display mode

REMARKS:
Draws the set of vertices are solid rectangles.
****************************************************************************/
void drawRects(
    vertex *v,
    int maxx,
    int maxy)
{
    int i;

    for(i = 0; i < MAX_LINES; i++) {
        moveCoords(i,v,maxx,maxy);
        MGL_setColor(v[i].color);
        MGL_rectPt(v[i].p1,v[i].p2);
        }
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    MGLDC           *dc;
    int             numDevices,mode,refresh = MGL_DEFAULT_REFRESH;
    ulong           autoStepSize = 5000000UL;
    LZTimerObject   tm;

    /* Initialise MGL */
    if ((numDevices = MGL_init(".",NULL)) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    if (numDevices < 2)
        MGL_fatalError("This sample requires at least two devices");

    /* We don't need console support */
    MGL_enableConsoleSupport(false);

    /* Initialize the timer */
    ZTimerInit();

    /* Force output to second device */
    MGL_selectDisplayDevice(1 | MM_MODE_MIXED);

    /* Create a display device context at 640x480x16 and then 640x480x8 */
    if ((mode = MGL_findMode(640,480,16)) == -1)
        if ((mode = MGL_findMode(640,480,8)) == -1)
            MGL_fatalError(MGL_errorMsg(MGL_result()));

    if ((dc = MGL_createDisplayDC(mode,1,refresh)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    initLines(&v[0],MGL_maxx(),MGL_maxy());

    /* Now draw some lines continuously */
    LZTimerOnExt(&tm);
    do {
        MGL_makeCurrentDC(dc);
        drawLines(&v[0],MGL_maxx(),MGL_maxy());
        } while (LZTimerLapExt(&tm) < autoStepSize);
    LZTimerOffExt(&tm);

    /* Clear display device */
    MGL_makeCurrentDC(dc);
    MGL_clearDevice();

    /* Now draw some rectangles continuously */
    LZTimerOnExt(&tm);
    do {
        MGL_makeCurrentDC(dc);
        drawRects(&v[0],MGL_maxx(),MGL_maxy());
        } while (LZTimerLapExt(&tm) < autoStepSize);
    LZTimerOffExt(&tm);

    /* Clear display device */
    MGL_makeCurrentDC(dc);
    MGL_clearDevice();

    MGL_exit();
    return 0;
}

