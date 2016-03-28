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
* Description:  Module to implement the gears OpenGL sample program.
*               Original code written by Brian Paul.
*
****************************************************************************/

#include "gatest.h"

/*---------------------------- Global Variables ---------------------------*/

#ifndef M_PI
#define M_PI 3.14159265
#endif

static GLfloat view_rotx, view_roty, view_rotz;
static GLint gear1, gear2, gear3;
static GLfloat angle;
static GLboolean stereo;            /* stereo vs mono view ? */
static GLfloat dxViewpt;            /* stereo viewpoint separation */
static GLfloat dxShift;             /* stereo image shift adjustment */
static GLfloat xfrmLeft[16];        /* left-eye projection transform */
static GLfloat xfrmRight[16];       /* right-eye projection transform */

/*----------------------------- Implementation ----------------------------*/

/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.

  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

static void gear(
    GLfloat inner_radius,
    GLfloat outer_radius,
    GLfloat width,
    GLint teeth,
    GLfloat tooth_depth)
{
    GLint i;
    GLfloat r0, r1, r2;
    GLfloat angle, da;
    GLfloat u, v, len;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0;
    r2 = outer_radius + tooth_depth / 2.0;

    da = 2.0 * M_PI / teeth / 4.0;

    glShadeModel(GL_FLAT);

    glNormal3f(0.0, 0.0, 1.0);

    /* draw front face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
        }
    glEnd();

    /* draw front sides of teeth */
    glBegin(GL_QUADS);
    da = 2.0 * M_PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
        }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    /* draw back face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        }
    glEnd();

    /* draw back sides of teeth */
    glBegin(GL_QUADS);
    da = 2.0 * M_PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;

        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        }
    glEnd();

    /* draw outward faces of teeth */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        u = r2 * cos(angle + da) - r1 * cos(angle);
        v = r2 * sin(angle + da) - r1 * sin(angle);
        len = sqrt(u * u + v * v);
        u /= len;
        v /= len;
        glNormal3f(v, -u, 0.0);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
        v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
        glNormal3f(v, -u, 0.0);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
        }

    glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
    glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

    glEnd();

    glShadeModel(GL_SMOOTH);

    /* draw inside radius cylinder */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * M_PI / teeth;
        glNormal3f(-cos(angle), -sin(angle), 0.0);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        }
    glEnd();
}

static void draw(void)
{
    /* Enclose original rendering code in 2-pass loop */
    int eye;
    for (eye = 0; eye < 2; eye++) {
        glDrawBuffer(stereo ? (eye ? GL_BACK_RIGHT : GL_BACK_LEFT) : GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Overide default projection view with stereo projection transforms */
        if (stereo) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadMatrixf(eye ? &xfrmRight[0] : &xfrmLeft[0]);
            glMatrixMode(GL_MODELVIEW);
            }

        glPushMatrix();
        glRotatef(view_rotx, 1.0, 0.0, 0.0);
        glRotatef(view_roty, 0.0, 1.0, 0.0);
        glRotatef(view_rotz, 0.0, 0.0, 1.0);

        glPushMatrix();
        glTranslatef(-3.0, -2.0, 0.0);
        glRotatef(angle, 0.0, 0.0, 1.0);
        glCallList(gear1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(3.1, -2.0, 0.0);
        glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
        glCallList(gear2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-3.1, 4.2, 0.0);
        glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
        glCallList(gear3);
        glPopMatrix();

        glPopMatrix();

        glFlush();
        /* Restore default projection view if stereo */
        if (stereo) {
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            /* Render only once if mono */
            }
        else
            break;
        }
    glFlush();
}

static void reshape(
    int width,
    int height)
{
    GLfloat h = (GLfloat) height / (GLfloat) width;

    glViewport(0, 0, (GLint) width, (GLint) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
    /* Calculate left-eye projection transform */
    glPushMatrix();
    glLoadIdentity();
    glFrustum(-1.0+dxShift, 1.0+dxShift, -h, h, 5.0, 60.0);
    glTranslatef(dxViewpt, 0.0, 0.0);
    glGetFloatv(GL_PROJECTION_MATRIX, &xfrmLeft[0]);
    glPopMatrix();
    /* Calculate right-eye projection transform */
    glPushMatrix();
    glLoadIdentity();
    glFrustum(-1.0-dxShift, 1.0-dxShift, -h, h, 5.0, 60.0);
    glTranslatef(-dxViewpt, 0.0, 0.0);
    glGetFloatv(GL_PROJECTION_MATRIX, &xfrmRight[0]);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -40.0);
}

static void myinit(void)
{
    static GLfloat pos[4] =
    {5.0, 5.0, 10.0, 0.0};
    static GLfloat red[4] =
    {0.8, 0.1, 0.0, 1.0};
    static GLfloat green[4] =
    {0.0, 0.8, 0.2, 1.0};
    static GLfloat blue[4] =
    {0.2, 0.2, 1.0, 1.0};

    /* Init global variables */
    view_rotx = 20.0;
    view_roty = 30.0;
    view_rotz = 0.0;
    angle = 0.0;
    stereo = GL_FALSE;
    dxViewpt = 0.950;
    dxShift = 0.115;

    /* Set up OpenGL rendering state */
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    /* make the gears */
    gear1 = glGenLists(1);
    glNewList(gear1, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    gear(1.0, 4.0, 1.0, 20, 0.7);
    glEndList();

    gear2 = glGenLists(1);
    glNewList(gear2, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    gear(0.5, 2.0, 2.0, 10, 0.7);
    glEndList();

    gear3 = glGenLists(1);
    glNewList(gear3, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    gear(1.3, 2.0, 0.5, 10, 0.7);
    glEndList();

    glEnable(GL_NORMALIZE);

    /* Do we have GL_STEREO quad buffered visual effective? */
    glGetBooleanv(GL_STEREO, &stereo);
}

int gearsTest(
    GA_HGLRC glrc,
    int width,
    int height)
{
    ibool   done = false;
    int     initPageCount = 20;
    int     fpsRate = 0,key = 0,waitVRT = gaWaitVRT,pageCount = initPageCount;
    ulong   lastCount = 0,newCount;
    event_t evt;

    myinit();
    reshape(width,height);
    LZTimerOn();
    while (!done) {
        draw();
        angle += 2.0;
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
            EVT_getNext(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
            key = EVT_asciiCode(evt.message);
            switch (key) {
                case 'z':
                    view_rotz += 5.0;
                    break;
                case 'Z':
                    view_rotz -= 5.0;
                    break;
                case 's':
                    stereo = (stereo) ? GL_FALSE : GL_TRUE;
                    break;
                case '+':
                    dxViewpt += 0.001;
                    reshape(width, height);
                    break;
                case '-':
                    dxViewpt -= 0.001;
                    reshape(width, height);
                    break;
                case '*':
                    dxShift += 0.001;
                    reshape(width, height);
                    break;
                case '/':
                    dxShift -= 0.001;
                    reshape(width, height);
                    break;
                case 'v':
                case 'V':
                    waitVRT -= 1;
                    if (modeInfo.Attributes & gaHaveTripleBuffer) {
                        if (waitVRT < gaTripleBuffer)
                            waitVRT = gaDontWait;
                        }
                    else {
                        if (waitVRT < gaWaitVRT)
                            waitVRT = gaDontWait;
                        }
                    break;
                case 0x1B:
                case 0x0D:
                case ' ':
                    done = true;
                    break;
                }
            switch (EVT_scanCode(evt.message)) {
                case KB_up:
                    view_rotx += 5.0;
                    break;
                case KB_down:
                    view_rotx -= 5.0;
                    break;
                case KB_left:
                    view_roty += 5.0;
                    break;
                case KB_right:
                    view_roty -= 5.0;
                    break;
                }
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

