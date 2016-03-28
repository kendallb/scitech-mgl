/****************************************************************************
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
* Description:  Game Framework main implementation code for SGI's SkyFly
*               demonstration program.
*
*               Modified for stereoscopic displays implementing
*               GL_STEREO quad-buffered page-flipping by David Milici.
*
****************************************************************************/

#include "gm/gm.h"
#include "GL/gl.h"
#include "skyfly.h"
#include "ztimer.h"
#include <string.h>
#include <stdlib.h>
#include "stereo.h"             /* stereo3d code additions */
#ifdef  __WINDOWS__
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

int         _autopilot = false;
GMDC        *gm;
ibool       waitForRetrace = MGL_waitVRT;
ibool       forcergbmode = false;
int         startWindowed = false;
GM_modeInfo modeInfo;

GM_driverOptions driverOpt = {
    true,           /* UseSNAP          */
    true,           /* UseHWOpenGL      */
    MGL_GL_AUTO,    /* OpenGLType       */
    GM_MODE_ALLBPP, /* modeFlags        */
    };

palette_t   pal[256];

#ifdef STEREO
/* control vars for stereo3d display */
GLboolean bStereoRequested = GL_FALSE;
GLboolean bStereoEnabled = GL_FALSE;
/* stereo3d parallax parameters */
GLfloat fDxViewpoint;
GLfloat fDxImageShift;
/* stereo3d frustum parameters */
GLfloat fFrustumWidth2;
GLfloat fFrustumHeight2;
GLfloat fFrustumZnear;
GLfloat fFrustumZfar;
/* stereo3d projection matrixes */
GLfloat mProjectionLeft[16];
GLfloat mProjectionRight[16];
#endif

/* Event handling globals */

static int buttons[BUTCOUNT];
static int mouse_x, mouse_y;

/* Functions in skyfly.c */

void init_misc(void);
void init_skyfly(void);
void sim_singlechannel(void);
void cull_proc(void);
void draw_proc(void);
void set_fog(int enable);
void set_dither(int enable);

#define SKY_R 0.23f
#define SKY_G 0.35f
#define SKY_B 0.78f

#define TERR_DARK_R 0.27f
#define TERR_DARK_G 0.18f
#define TERR_DARK_B 0.00f

#define TERR_LITE_R 0.24f
#define TERR_LITE_G 0.53f
#define TERR_LITE_B 0.05f

/*------------------------------ Implementation ---------------------------*/

int Xgetbutton(int button)
{
    int b;
    if (button < 0 || button >= BUTCOUNT)
        return -1;
    b = buttons[button];
    if (button < LEFTMOUSE)
        buttons[button] = 0;
    return b;
}

int Xgetvaluator(int val)
{
    switch (val) {
        case MOUSEX:
            return mouse_x;
        case MOUSEY:
            return mouse_y;
        default:
            return -1;
        }
}

void setPaletteIndex(int i, GLfloat r, GLfloat g, GLfloat b)
{
    pal[i].red = (uchar)(255.0F * r);
    pal[i].green = (uchar) (255.0F * g);
    pal[i].blue = (uchar) (255.0F * b);
}

void init_cmap(void)
{
    int         ii, jj, color;
    GLfloat     r0, g0, b0, r1, g1, b1;

    /* Set up color map */
    color = 10;
    memset(pal,0,sizeof(pal));

    /* Sky colors */
    sky_base = color;
    r0 = SKY_R; r1 = 1.0f;
    g0 = SKY_G; g1 = 1.0f;
    b0 = SKY_B; b1 = 1.0f;
    for (ii = 0; ii < SKY_COLORS; ii++) {
        GLfloat p, r, g, b;
        p = (GLfloat) ii / (SKY_COLORS-1);
        r = r0 + p * (r1 - r0);
        g = g0 + p * (g1 - g0);
        b = b0 + p * (b1 - b0);
        for (jj = 0; jj < FOG_LEVELS; jj++) {
            GLfloat fp, fr, fg, fb;
            fp = (FOG_LEVELS > 1) ? (GLfloat) jj / (FOG_LEVELS-1) : 0.0f;
            fr = r + fp * (fog_params[0] - r);
            fg = g + fp * (fog_params[1] - g);
            fb = b + fp * (fog_params[2] - b);
            setPaletteIndex(sky_base + (ii*FOG_LEVELS) + jj, fr, fg, fb);
        }
    }
    color += (SKY_COLORS * FOG_LEVELS);

    /* Terrain colors */
    terr_base = color;
    r0 = TERR_DARK_R;   r1 = TERR_LITE_R;
    g0 = TERR_DARK_G;   g1 = TERR_LITE_G;
    b0 = TERR_DARK_B;   b1 = TERR_LITE_B;
    for (ii = 0; ii < TERR_COLORS; ii++) {
        GLfloat p, r, g, b;
        p = (GLfloat) ii / (TERR_COLORS-1);
        r = r0 + p * (r1 - r0);
        g = g0 + p * (g1 - g0);
        b = b0 + p * (b1 - b0);
        for (jj = 0; jj < FOG_LEVELS; jj++) {
            GLfloat fp, fr, fg, fb;
            fp = (FOG_LEVELS > 1) ? (GLfloat) jj / (FOG_LEVELS-1) : 0.0f;
            fr = r + fp * (fog_params[0] - r);
            fg = g + fp * (fog_params[1] - g);
            fb = b + fp * (fog_params[2] - b);
            setPaletteIndex(terr_base + (ii*FOG_LEVELS) + jj, fr, fg, fb);
        }
    }
    color += (TERR_COLORS * FOG_LEVELS);

    /* Plane colors */
    plane_colors[0] = color;
    plane_colors[1] = color + (PLANE_COLORS/2);
    plane_colors[2] = color + (PLANE_COLORS-1);
    r0 = 0.4; r1 = 0.8;
    g0 = 0.4; g1 = 0.8;
    b0 = 0.1; b1 = 0.1;
    for (ii = 0; ii < PLANE_COLORS; ii++) {
        GLfloat p, r, g, b;
        p = (GLfloat) ii / (PLANE_COLORS);
        r = r0 + p * (r1 - r0);
        g = g0 + p * (g1 - g0);
        b = b0 + p * (b1 - b0);
        setPaletteIndex(plane_colors[0] + ii, r, g, b);
    }
    color += PLANE_COLORS;
    GM_setPalette(pal,256,0);
    GM_realizePalette(256,0,true);
}

static GLboolean checkExtension(const char *name)
{
    const char *p = (const char *) glGetString(GL_EXTENSIONS);

    while (p = strstr(p, name)) {
        const char *q = p + strlen(name);

        if (*q == ' ' || *q == '\0') {
            return GL_TRUE;
        }
        p = q;
    }
    return GL_FALSE;
}

font_t *font8x8 = NULL;

void reinit(void)
{
    char    title[256];
    int     i,flags = MGL_GL_DEPTH | MGL_GL_DOUBLE;

    rgbmode = forcergbmode || (gm->dc->mi.bitsPerPixel > 8);
    if (!rgbmode) {
        flags |= MGL_GL_INDEX;
        rgbmode = false;
        }
    fog = !rgbmode;
    dither = !rgbmode;
    mipmap = false;
    if ((!startWindowed && (modeInfo.flags & MGL_HAVE_ACCEL_3D)) || (startWindowed && MGL_glHaveHWOpenGL())) {
        /* Enable dithering and mipmapping for hardware 3D modes */
        dither = true;
        mipmap = true;
        fog = true;
        }

#ifdef  STEREO
    /* request GL_STEREO compatible display visual */
    if (bStereoRequested)
        flags |= MGL_GL_STEREO;
#endif
    if (!GM_startOpenGL(flags))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if (!rgbmode && !checkExtension("SGI_index_texture")) {
        MGL_fatalError("SGI_index_texture is required for this application to\n"
                       "work correctly in color index mode.\n");
        }
#ifdef  STEREO
    /* check if GL_STEREO quad-buffered page-flipping enabled ? */
    if (bStereoRequested)
        glGetBooleanv(GL_STEREO, &bStereoEnabled);
    if (bStereoRequested && (bStereoEnabled != GL_TRUE))
        MGL_fatalError("GL_STEREO display visual not enabled.");
#endif

    sprintf(title, "OpenGL SkyFly (%s %s %s)",
        glGetString(GL_VENDOR),
        glGetString(GL_RENDERER),
        glGetString(GL_VERSION));
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    SetWindowText(gm->mainWindow, title);
#endif
    Wxsize = gm->dc->mi.xRes+1;
    Wysize = gm->dc->mi.yRes+1;
    glViewport(0,0,Wxsize,Wysize);
    for(i = 0; i < BUTCOUNT; i++)
        buttons[i] = 0;
    mouse_x = Wxsize * 0.4;
    mouse_y = Wysize/2;
    init_misc();
    if (!rgbmode)
        init_cmap();
    init_skyfly();

    if (!font8x8)
        font8x8 = MGL_loadFont("pc8x8.fnt");
    MGL_useFont(font8x8);

    /* Set default mouse position */
    mouse_x = Wxsize * 0.1;
    mouse_y = Wysize/2;

    /* Start the Zen Timer counting */
//  LZTimerOn();
}

void gameLogic(void)
{
    sim_singlechannel();
}

ulong               lastCount;      // Timer count for last fps update
int                 frameCount;     // Number of frames for timing
int                 fpsRate;        // Current frames per second rate

void draw(void)
{
    ulong   newCount;
    char    buf[20];

#ifdef  STEREO
    int     i;

    /* enclose original rendering code in 2-pass loop */
    for (i=0; i<2; i++)
    {
    /* adjust projection matrix for stereo viewpoint */
    if (bStereoEnabled) {
        glDrawBuffer((i==0) ? GL_BACK_LEFT : GL_BACK_RIGHT);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf((i==0) ? &mProjectionLeft : &mProjectionRight);
        glMatrixMode(GL_MODELVIEW);
        }
#endif

    // Draw the frame
    cull_proc();
    draw_proc();

#ifdef  STEREO
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
#endif

    // Update the frames per second count if we have gone past at least
    // a quarter of a second since the last update.
    newCount = LZTimerLap();
    frameCount++;
    if ((newCount - lastCount) > 100000L) {
        fpsRate = (int)(10000000L / (newCount - lastCount)) * frameCount;
        lastCount = newCount;
        frameCount = 0;
        }
    sprintf(buf,"%3d.%d fps", fpsRate / 10, fpsRate % 10);
    MGL_setColorRGB(0xFF,0xFF,0xFF);
    MGL_setTextJustify(MGL_RIGHT_TEXT,MGL_BOTTOM_TEXT);
    MGL_drawStrXY(MGL_maxx(),MGL_maxy(),buf);

    // Swap the display buffers
    GM_swapBuffers(waitForRetrace);
}

// CGDC Demo
extern int _frame;
extern float Speed;
void init_positions(void);

void keydown(event_t *evt)
{
    switch ((ushort)evt->message) {
        case KB_left:
            buttons[LEFTARROWKEY] = 1;
            break;
        case KB_right:
            buttons[RIGHTARROWKEY] = 1;
            break;
        case KB_up:
            buttons[UPARROWKEY] = 1;
            break;
        case KB_down:
            buttons[DOWNARROWKEY] = 1;
            break;
        case KB_pageDown:
            buttons[PAGEDOWNKEY] = 1;
            break;
        case KB_pageUp:
            buttons[PAGEUPKEY] = 1;
            break;
        default:
            switch (EVT_asciiCode(evt->message)) {
                case 0x1B:
                    GM_exit();
                    break;
                case 'r':
                    buttons[RKEY] = 1;
                    break;
                case '.':
                    buttons[PERIODKEY] = 1;
                    break;
                case ' ':
                    buttons[SPACEKEY] = 1;
                    break;
                case 'f':
                    set_fog(!fog);
                    break;
                case 'd':
                    set_dither(!dither);
                    break;
                case 'v':
                    if (!MGL_isWindowedDC(gm->dispdc)) {
                        if (waitForRetrace == MGL_waitVRT)
                            waitForRetrace = MGL_dontWait;
                        else
                            waitForRetrace = MGL_waitVRT;
                        }
                    break;
                case 'a':
                    _autopilot = !_autopilot;
                    _frame = 0;
                    mouse_x = Wxsize * 0.1;
                    mouse_y = Wysize/2;
                    init_positions();
                    if (_autopilot) {
                        Speed = 1.0f;
                        }
                    else {
                        Speed = 0.3f;
                        }
                    break;
#ifdef  STEREO  /* adjust stereo3d parallax settings */
                case '+':
                    fDxViewpoint += 0.01;
                    calcStereoMatrix();
                    break;
                case '-':
                    fDxViewpoint -= 0.01;
                    if (fDxViewpoint < 0.0)
                        fDxViewpoint = 0.0;
                    calcStereoMatrix();
                    break;
                case '*':
                    fDxImageShift += 0.0005;
                    calcStereoMatrix();
                    break;
                case '/':
                    fDxImageShift -= 0.0005;
                    calcStereoMatrix();
                    break;
#endif
                }
        }
}

void mousedown(event_t *evt)
{
    if (_autopilot) {
        mouse_x = Wxsize * 0.1;
        mouse_y = Wysize/2;
        }
    else {
        mouse_x = evt->where_x;
        mouse_y = evt->where_y;
        }
    if (evt->message & EVT_LEFTBMASK)
        buttons[LEFTMOUSE] = 1;
    else
        buttons[RIGHTMOUSE] = 1;
}

void mouseup(event_t *evt)
{
    if (_autopilot) {
        mouse_x = Wxsize * 0.1;
        mouse_y = Wysize/2;
        }
    else {
        mouse_x = evt->where_x;
        mouse_y = evt->where_y;
        }
    if (evt->message & EVT_LEFTBMASK)
        buttons[LEFTMOUSE] = 0;
    else
        buttons[RIGHTMOUSE] = 0;
}

void mousemove(event_t *evt)
{
    if (_autopilot) {
        mouse_x = Wxsize * 0.1;
        mouse_y = Wysize/2;
        }
    else {
        mouse_x = evt->where_x;
        mouse_y = evt->where_y;
        }
}

/* Main callback to switch between fullscreen and windowed graphics modes. This
 * function is called by the game framework when the user requests to switch to
 * fullscreen modes either by typing 'Alt-Enter' or by clicking on the maximum
 * button when in windowed modes.
 */

void switchModes(GM_modeInfo *mode,ibool windowed)
{
    reinit();
}

void main(int argc, char *argv[])
{
    int xRes = 640,yRes = 480,bits = 16;

    if (argc > 1) {
#if 0
        else if (stricmp(argv[1],"-novbeaf") == 0)
            driverOpt.useVBEAF = false;
#endif
        if (stricmp(argv[1],"-nohwopengl") == 0)
            driverOpt.useHWOpenGL = false;
        else if (stricmp(argv[1],"-usesnapgl") == 0)
            driverOpt.openGLType = MGL_GL_SNAP;
        else if (stricmp(argv[1],"-rgb") == 0)
            forcergbmode = true;
#ifdef STEREO
        else if (stricmp(argv[1],"-stereo") == 0)
            bStereoRequested = GL_TRUE;
#endif
        else if (argc >= 5 && stricmp(argv[1],"-mode") == 0) {
            xRes = atoi(argv[2]);
            yRes = atoi(argv[3]);
            bits = atoi(argv[4]);
            }
#ifndef __WINDOWS__
        else {
            printf("Valid arguments are:\n");
            printf("    -nowindirect\n");
            printf("    -novbe\n");
            printf("    -nolinear\n");
            printf("    -novbeaf\n");
            printf("    -nodirectdraw\n");
            printf("    -norle\n");
            printf("    -usesysmem\n");
            printf("    -stereo\n");
            printf("    -mode [xres] [yres] [bits]\n");
            exit(1);
            }
#endif
        }
    GM_setDriverOptions(&driverOpt);
    ZTimerInit();
    LZTimerOn();
    if ((gm = GM_init("OpenGL SkyFly")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    GM_setDrawFunc(draw);
    GM_setGameLogicFunc(gameLogic);
    GM_setKeyDownFunc(keydown);
    GM_setKeyRepeatFunc(keydown);
    GM_setMouseDownFunc(mousedown);
    GM_setMouseMoveFunc(mousemove);
    GM_setMouseUpFunc(mouseup);
    GM_setModeSwitchFunc(switchModes);

#ifndef __WINDOWS__
    /* Search for 640x480x64k as the default mode */
    if (!GM_findMode(&modeInfo,xRes,yRes,bits))
        MGL_fatalError("Unable to find default graphics mode!");
#else
    /* Search for 640x480x64k as the default mode */
    modeInfo.xRes = xRes;
    modeInfo.yRes = yRes;
    modeInfo.bitsPerPixel = bits;
    while (GM_chooseMode(&modeInfo,&startWindowed))
#endif
        {
        if (!GM_setMode(&modeInfo,startWindowed,3,false))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        reinit();
        GM_mainLoop();
        }
}
