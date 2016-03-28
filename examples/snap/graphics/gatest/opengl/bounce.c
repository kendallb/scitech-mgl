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
* Description:  Module to implement the bounce OpenGL sample program.
*               Original code written by Brian Paul.
*
****************************************************************************/

#include "gatest.h"

/*---------------------------- Global Variables ---------------------------*/

#define COS(X)  cos((X) * 3.14159/180.0)
#define SIN(X)  sin((X) * 3.14159/180.0)

static GLuint   Ball;
static GLfloat  Zrot, Zstep;
static GLfloat  Xpos, Ypos;
static GLfloat  Xvel, Yvel;
static GLfloat  Xmin, Xmax;
static GLfloat  Ymin;
static GLfloat  G;

/*----------------------------- Implementation ----------------------------*/

static void draw_ball(void)
{
    GLfloat a, b;
    GLfloat da = 18.0, db = 18.0;
    GLfloat radius = 1.0;
    GLuint color;
    GLfloat x, y, z;

    color = 0;
    for (a = -90.0; a+da <= 90.0; a += da) {
        glBegin(GL_QUAD_STRIP);
        for (b = 0.0; b <=360.0 ; b += db) {
            if (color)
                glIndexf(4);
            else
                glIndexf(15);
            x = COS(b) * COS(a);
            y = SIN(b) * COS(a);
            z = SIN(a);
            glVertex3f(x, y, z);
            x = radius * COS(b) * COS(a+da);
            y = radius * SIN(b) * COS(a+da);
            z = radius * SIN(a+da);
            glVertex3f(x, y, z);
            color = 1 - color;
            }
        glEnd();
        }
}

static GLuint make_ball(void)
{
    GLuint list;

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    draw_ball();
    glEndList();
    return list;
}

static void reshape(
    int width,
    int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -6.0, 6.0, -6.0, 6.0, -6.0, 6.0 );
    glMatrixMode(GL_MODELVIEW);
}

static void draw(void)
{
    GLint i;

    glClear(GL_COLOR_BUFFER_BIT);
    glIndexf(3);
    glBegin(GL_LINES);
    for (i = -5; i <= 5; i++) {
        glVertex2i(i, -5);
        glVertex2i(i, 5);
        }
    for (i = -5; i <= 5; i++) {
        glVertex2i(-5,i);
        glVertex2i(5,i);
        }
    for (i = -5; i <= 5; i++) {
        glVertex2i(i, -5);
        glVertex2f(i*1.15, -5.9);
        }
    glVertex2f(-5.3, -5.35);
    glVertex2f(5.3, -5.35);
    glVertex2f(-5.75, -5.9);
    glVertex2f(5.75, -5.9);
    glEnd();
    glFlush();

    glPushMatrix();
    glTranslatef(Xpos, Ypos, 0.0);
    glScalef(2.0, 2.0, 2.0);
    glRotatef(8.0, 0.0, 0.0, 1.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(Zrot, 0.0, 0.0, 1.0);
    glCallList(Ball);
    glPopMatrix();

    glFlush();
}

static void idle(void)
{
    static float vel0 = -100.0;

    Zrot += Zstep;
    Xpos += Xvel;
    if (Xpos >= Xmax) {
        Xpos = Xmax;
        Xvel = -Xvel;
        Zstep = -Zstep;
        }
    if (Xpos <= Xmin) {
        Xpos = Xmin;
        Xvel = -Xvel;
        Zstep = -Zstep;
        }
    Ypos += Yvel;
    Yvel += G;
    if (Ypos < Ymin) {
        Ypos = Ymin;
        if (vel0 == -100.0)
            vel0 = fabs(Yvel);
        Yvel = vel0;
        }
    draw();
}

int bounceTest(
    GA_HGLRC glrc,
    int width,
    int height)
{
    ibool   done = false;
    int     initPageCount = 20;
    int     fpsRate = 0,key = 0,waitVRT = gaWaitVRT,pageCount = initPageCount;
    ulong   lastCount = 0,newCount;

    /* Init global variables */
    Zrot = 0.0;     Zstep = 4.0;
    Xpos = 0.0;     Ypos = 1.0;
    Xvel = 0.1;     Yvel = 0.0;
    Xmin = -4.0;    Xmax=4.0;
    Ymin = -3.8;    G = -0.05;

    /* Init ball and OpenGL rendering state */
    Ball = make_ball();
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_FLAT);
    reshape(width,height);
    LZTimerOn();
    while (!done) {
        idle();
        gmoveto(0,0);
        gprintf("%d x %d %d bit %s (%d.%d fps)",(int)maxX+1,(int)maxY+1,
            (int)modeInfo.BitsPerPixel,
            (cntMode & gaLinearBuffer) ? "Linear" : "Banked",
            fpsRate / 10, fpsRate % 10);
        if (softwareOnly)
            gprintf("Rendering to system memory back buffer");
        switch (waitVRT) {
            case gaTripleBuffer:
                gprintf("Triple buffering - should be no flicker");
                gprintf("Frame rate *must* max at refresh rate");
                break;
            case gaWaitVRT:
                gprintf("Double buffering - should be no flicker");
                gprintf("Frame rate *must* lock to multiple of refresh");
                break;
            default:
                gprintf("Page flipping (no wait) - may flicker");
                gprintf("Frame rate *must* max at hardware limit");
                break;
            }
        glFuncs.SwapBuffers(glrc,waitVRT);
        if (EVT_kbhit()) {
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
            else
                break;
            }
        /* Compute the frames per second rate after going through a large
         * number of pages.
         */
        if (--pageCount == 0) {
            newCount = LZTimerLap();
            fpsRate = (int)(10000000L / (newCount - lastCount)) * initPageCount;
            lastCount = newCount;
            pageCount = initPageCount;
            }
        }
    LZTimerOff();
    return key;
}

