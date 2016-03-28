/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Description:  Simple Game Framework OpenGL sample program. This is a
*               port of Brian Paul's Gears demo from Mesa to the MGL
*               Game Framework.
*
*               Adapted for stereoscopic display support by David Milici.
*
****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ztimer.h"
#include "gm/gm.h"
#include "GL/gl.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

#ifndef M_PI
#  define M_PI 3.14159265
#endif

static GLfloat view_rotx=20.0, view_roty=30.0, view_rotz=0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;
static GLuint count = 1;
static GMDC *gm;
static ibool waitForRetrace = MGL_waitVRT;
static font_t *font8x8 = NULL;

GM_driverOptions driverOpt = {
    true,           /* UseSNAP          */
    true,           /* UseHWOpenGL      */
    MGL_GL_AUTO,    /* OpenGLType       */
    GM_MODE_ALLBPP, /* modeFlags        */
    };

/* Control vars for stereo3d display */
GLboolean bStereoEnabled = false;

/* Stereo3d parallax parameters */
GLfloat fDxViewpoint;
GLfloat fDxImageShift;

/* Stereo3d frustum parameters */
GLfloat fFrustumWidth2;
GLfloat fFrustumHeight2;
GLfloat fFrustumZnear;
GLfloat fFrustumZfar;

/* Stereo3d projection matrixes */
GLfloat mProjectionLeft[16];
GLfloat mProjectionRight[16];

/*------------------------------ Implementation ---------------------------*/

/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
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
    r1 = outer_radius - tooth_depth/2.0;
    r2 = outer_radius + tooth_depth/2.0;

    da = 2.0*M_PI / teeth / 4.0;

    glShadeModel( GL_FLAT );

    glNormal3f( 0.0, 0.0, 1.0 );

    /* draw front face */
    glBegin( GL_QUAD_STRIP );
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0*M_PI / teeth;
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
        glVertex3f( r1*cos(angle), r1*sin(angle), width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
        }
    glEnd();

    /* draw front sides of teeth */
    glBegin( GL_QUADS );
    da = 2.0*M_PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
        }
    glEnd();

    glNormal3f( 0.0, 0.0, -1.0 );

    /* draw back face */
    glBegin( GL_QUAD_STRIP );
    for (i = 0;i <= teeth; i++) {
        angle = i * 2.0*M_PI / teeth;
        glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
        }
    glEnd();

    /* draw back sides of teeth */
    glBegin( GL_QUADS );
    da = 2.0*M_PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
        glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
        }
    glEnd();


    /* draw outward faces of teeth */
    glBegin( GL_QUAD_STRIP );
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0*M_PI / teeth;

        glVertex3f( r1*cos(angle),      r1*sin(angle),       width*0.5 );
        glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
        u = r2*cos(angle+da) - r1*cos(angle);
        v = r2*sin(angle+da) - r1*sin(angle);
        len = sqrt( u*u + v*v );
        u /= len;
        v /= len;
        glNormal3f( v, -u, 0.0 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),    width*0.5 );
        glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
        glNormal3f( cos(angle), sin(angle), 0.0 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 );
        glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
        u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
        v = r1*sin(angle+3*da) - r2*sin(angle+2*da);
        glNormal3f( v, -u, 0.0 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 );
        glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
        glNormal3f( cos(angle), sin(angle), 0.0 );
        }

    glVertex3f( r1*cos(0), r1*sin(0), width*0.5 );
    glVertex3f( r1*cos(0), r1*sin(0), -width*0.5 );

    glEnd();


    glShadeModel( GL_SMOOTH );

    /* draw inside radius cylinder */
    glBegin( GL_QUAD_STRIP );
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0*M_PI / teeth;
        glNormal3f( -cos(angle), -sin(angle), 0.0 );
        glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
        glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
        }
    glEnd();
}

ulong               lastCount;      // Timer count for last fps update
int                 frameCount;     // Number of frames for timing
int                 fpsRate;        // Current frames per second rate

static void drawFrame(void)
{
    ulong   newCount;
    char    buf[20];
    int     i;

    /* enclose original rendering code in 2-pass loop */
    for (i = 0; i < 2; i++) {
        /* adjust projection matrix for stereo viewpoint */
        if (bStereoEnabled) {
            glDrawBuffer((i==0) ? GL_BACK_LEFT : GL_BACK_RIGHT);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadMatrixf((i==0) ? (float*)&mProjectionLeft : (float*)&mProjectionRight);
            glMatrixMode(GL_MODELVIEW);
            }

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glPushMatrix();
        glRotatef( view_rotx, 1.0, 0.0, 0.0 );
        glRotatef( view_roty, 0.0, 1.0, 0.0 );
        glRotatef( view_rotz, 0.0, 0.0, 1.0 );

        glPushMatrix();
        glTranslatef( -3.0, -2.0, 0.0 );
        glRotatef( angle, 0.0, 0.0, 1.0 );
        glCallList(gear1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef( 3.1, -2.0, 0.0 );
        glRotatef( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
        glCallList(gear2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef( -3.1, 4.2, 0.0 );
        glRotatef( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
        glCallList(gear3);
        glPopMatrix();

        glPopMatrix();

        sprintf(buf,"%3d.%d fps", fpsRate / 10, fpsRate % 10);
        MGL_setColorRGB(0xFF,0xFF,0xFF);
        MGL_setTextJustify(MGL_RIGHT_TEXT,MGL_BOTTOM_TEXT);
        MGL_drawStrXY(MGL_maxx(),MGL_maxy(),buf);

        /* render twice for stereo, or once for mono */
        if (bStereoEnabled) {
            glFlush();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            }
        else
            break;
        } /* for loop */

    GM_swapBuffers(waitForRetrace);

    // Update the frames per second count if we have gone past at least
    // a quarter of a second since the last update.
    newCount = LZTimerLap();
    frameCount++;
    if ((newCount - lastCount) > 100000L) {
        fpsRate = (int)(10000000L / (newCount - lastCount)) * frameCount;
        lastCount = newCount;
        frameCount = 0;
        }

    count++;
}

// Additional routine for building stereo projection matrixes
static void calcStereoMatrix(void)
{
    glMatrixMode(GL_PROJECTION);

    /* rebuild frustum with rightward image shift for left eye view */
    glPushMatrix();
    glLoadIdentity();
    glFrustum(-fFrustumWidth2+fDxImageShift, fFrustumWidth2+fDxImageShift,
              -fFrustumHeight2, fFrustumHeight2,
              fFrustumZnear, fFrustumZfar);

    /* Offset viewpoint for left eye perspective */
    glTranslatef(fDxViewpoint, 0.0, 0.0);
    glGetFloatv(GL_PROJECTION_MATRIX, (float*)&mProjectionLeft);
    glPopMatrix();

    /* Rebuild frustum with leftward image shift for right eye view */
    glPushMatrix();
    glLoadIdentity();
    glFrustum(-fFrustumWidth2-fDxImageShift, fFrustumWidth2-fDxImageShift,
              -fFrustumHeight2, fFrustumHeight2,
              fFrustumZnear, fFrustumZfar);

    /* Offset viewpoint for right eye perspective */
    glTranslatef(-fDxViewpoint, 0.0, 0.0);
    glGetFloatv(GL_PROJECTION_MATRIX, (float*)&mProjectionRight);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/****************************************************************************
REMARKS:
Sets up the correct viewing parameters after the size of our rendering
context has changed. This occurs whenever as mode change has happened. We
also create our display lists in here, as these need to be re-built anytime
that we change resolutions.
****************************************************************************/
static void init(void)
{
    static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0 };
    static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0 };
    static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0 };
    static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0 };
    GLint   width = MGL_sizex(gm->dispdc)+1;
    GLint   height = MGL_sizey(gm->dispdc)+1;
    GLfloat  h = (GLfloat) height / (GLfloat) width;

    /* Start OpenGL rendering. Note that we just use some simple flags to
     * do this, however you can call the MGL_glChooseVisual/MGL_glSetVisual
     * functions on the gm->dispdc device context if you need more control
     * over the type of OpenGL visual created.
     */
    if (bStereoEnabled) {
        if (!GM_startOpenGL(MGL_GL_RGB | MGL_GL_DEPTH | MGL_GL_DOUBLE | MGL_GL_STEREO))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        glGetBooleanv(GL_STEREO, &bStereoEnabled);
        if (bStereoEnabled == false)
            MGL_fatalError("GL_STEREO display visual not supported.");
        }
    else {
        if (!GM_startOpenGL(MGL_GL_RGB | MGL_GL_DEPTH | MGL_GL_DOUBLE))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }

    /* Initialize the viewing transforms */
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );

    /* Remember viewing frustum parameters for building stereo matrixes */
    if (bStereoEnabled) {
        fFrustumWidth2 = 1.0;
        fFrustumHeight2 = h;
        fFrustumZnear = 5.0;
        fFrustumZfar = 60.0;
        fDxImageShift = 0.1 * fFrustumWidth2;
        fDxViewpoint = 0.90;
        calcStereoMatrix();

        /* Lower contrast to reduce ghosting in LC shutter glasses */
        glClearColor(0.2, 0.2, 0.2, 1.0);
        }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -40.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Initialize default state information */
    glLightfv( GL_LIGHT0, GL_POSITION, pos );
    glEnable( GL_CULL_FACE );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_DEPTH_TEST );

    /* Make the gears */
    gear1 = glGenLists(1);
    glNewList(gear1, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
    gear( 1.0, 4.0, 1.0, 20, 0.7 );
    glEndList();

    gear2 = glGenLists(1);
    glNewList(gear2, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
    gear( 0.5, 2.0, 2.0, 10, 0.7 );
    glEndList();

    gear3 = glGenLists(1);
    glNewList(gear3, GL_COMPILE);
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
    gear( 1.3, 2.0, 0.5, 10, 0.7 );
    glEndList();

    glEnable( GL_NORMALIZE );

    if (!font8x8)
        font8x8 = MGL_loadFont("pc8x8.fnt");
    MGL_useFont(font8x8);
}

/****************************************************************************
PARAMETERS:
evt - Keyboard event to handle

REMARKS:
Handles keyboard events and passes them to ProcessInput.
****************************************************************************/
static void keyDown(
    event_t *evt)
{
    switch (EVT_scanCode(evt->message)) {
        case KB_left:
            view_roty += 5.0;
            break;
        case KB_right:
            view_roty -= 5.0;
            break;
        case KB_up:
            view_rotx += 5.0;
            break;
        case KB_down:
            view_rotx -= 5.0;
            break;
        default:
            switch (EVT_asciiCode(evt->message)) {
                case 'z':
                    view_rotz += 5.0;
                    break;
                case 'Z':
                    view_rotz -= 5.0;
                    break;
                case 0x1B:
                    GM_exit();
                    break;
                case 'v':
                    if (!MGL_isWindowedDC(gm->dispdc)) {
                        if (waitForRetrace == MGL_waitVRT)
                            waitForRetrace = MGL_dontWait;
                        else
                            waitForRetrace = MGL_waitVRT;
                        }
                    break;
                /* Adjust stereo3d parallax parameters */
                case '+':
                    fDxViewpoint += 0.10;
                    calcStereoMatrix();
                    break;
                case '-':
                    fDxViewpoint -= 0.10;
                    if (fDxViewpoint < 0.0)
                        fDxViewpoint = 0.0;
                    calcStereoMatrix();
                    break;
                case '*':
                    fDxImageShift += 0.01;
                    calcStereoMatrix();
                    break;
                case '/':
                    fDxImageShift -= 0.01;
                    calcStereoMatrix();
                    break;
                }
            break;
        }
}

/****************************************************************************
REMARKS:
Update all object positions for the next frame
****************************************************************************/
static void gameLogic(void)
{
    angle += 2.0;
}

/****************************************************************************
REMARKS:
Main callback to switch between fullscreen and windowed graphics modes. This
function is called by the game framework when the user requests to switch
to fullscreen modes either by typing 'Alt-Enter' or by clicking on the
maximize button when in windowed modes.
****************************************************************************/
void switchModes(
    GM_modeInfo *mode,
    ibool windowed)
{
    init();
}

/****************************************************************************
PARAMETER:
dc      - Active display DC to save/restore state from
flags   - Flags indicating what to do

RETURNS:
Switch status flag

REMARKS:
This function is where you would put your own code to handle switching to
and from fullscreen modes, such as pausing CD-Audio and other stuff. The
main game loop will continue to run as we return MGL_NO_SUSPEND_APP (but no
drawFrame will not be called, only gameLogic), and you can optionally return
MGL_NO_DEACTIVATE to forcibly ignore the switch.
****************************************************************************/
int MGLAPI suspendApp(
    MGLDC *dc,
    int flags)
{
    if (flags == MGL_DEACTIVATE) {
        /* We are switching back to GDI mode, so put code in here to disable
         * stuff when switched away from fullscreen mode. Note that this
         * function may get once to determine if the switch should occur,
         * and again when the switch actually happens.
         */
        }
    else if (flags == MGL_REACTIVATE) {
        /* We are now back in fullscreen mode, so put code in here to renable
         * stuff for fullscreen modes.
         */
        }
    return MGL_NO_SUSPEND_APP;
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    int         xRes = 640,yRes = 480,bits = 16,refresh = MGL_DEFAULT_REFRESH;
    int         startWindowed = true;
    GM_modeInfo info;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    while (argc > 1) {
        if (stricmp(argv[1],"-nosnap") == 0)
            driverOpt.useSNAP = false;
        else if (stricmp(argv[1],"-nohwopengl") == 0)
            driverOpt.useHWOpenGL = false;
        else if (stricmp(argv[1],"-usesnapgl") == 0)
            driverOpt.openGLType = MGL_GL_SNAP;
        else if (argc >= 3 && stricmp(argv[1],"-stereo") == 0) {
            bStereoEnabled = true;
            startWindowed = false;
            refresh = atoi(argv[2]);
            argc -= 1;
            argv += 1;
            }
        else if (argc >= 5 && stricmp(argv[1],"-mode") == 0) {
            xRes = atoi(argv[2]);
            yRes = atoi(argv[3]);
            bits = atoi(argv[4]);
            argc -= 3;
            argv += 3;
            }
#ifndef __WINDOWS__
        else {
            printf("Valid arguments are:\n");
            printf("    -nosnap\n");
            printf("    -nodirectdraw\n");
            printf("    -nohwopengl\n");
            printf("    -usemesa\n");
            printf("    -usesgi\n");
            printf("    -stereo [refresh]\n");
            printf("    -mode [xres] [yres] [bits]\n");
            exit(1);
            }
#endif
        argc--;
        argv++;
        }
    GM_setDriverOptions(&driverOpt);
    ZTimerInit();
    LZTimerOn();
    GM_initSysPalNoStatic(true);
    if ((gm = GM_init("Game Framework OpenGL Demo")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    GM_setGameLogicFunc(gameLogic);
    GM_setDrawFunc(drawFrame);
    GM_setKeyDownFunc(keyDown);
    GM_setKeyRepeatFunc(keyDown);
    GM_setModeSwitchFunc(switchModes);
    GM_setSuspendAppCallback(suspendApp);

    /* Search for 640x480x64k as the default mode */
    info.xRes = xRes;
    info.yRes = yRes;
    info.bitsPerPixel = bits;
    while (GM_chooseMode(&info,&startWindowed)) {
        if (!GM_setModeExt(&info,startWindowed,3,refresh,false,bStereoEnabled))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        if (bStereoEnabled)
            GM_startStereo();
        init();
        GM_mainLoop();
        }
    return 0;
}
