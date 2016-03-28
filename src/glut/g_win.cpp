/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*    The contents of this file are subject to the SciTech MGL Public
*    License Version 1.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of
*    the License at http://www.scitechsoft.com/mgl-license.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*    The Original Code is Copyright (C) 1991-2004 SciTech Software, Inc.
*
*    The Initial Developer of the Original Code is SciTech Software, Inc.
*    All Rights Reserved.
*
*  ========================================================================
*
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
****************************************************************************/

#include "glutint.h"

/*------------------------- Implementation --------------------------------*/

int glutGetWindow(void)
{
    return 0;
}

void glutSetWindow(int)
{
}

void __glutDefaultDisplay(void)
{
    __glutFatalError("DefaultDisplay callback called...this shouldn't happen\n");
}

void __glutDefaultReshape(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
}

int glutCreateSubWindow(int, int, int, int, int)
{
    return 0;
}

void glutDestroyWindow(int)
{
}

void glutSwapBuffers(void)
{
    static int      frameCount = 0,fpsRate = 0;
    static GLUTtimeval prev = {0};
    GLUTtimeval     now,delta;
    char            buf[20];

    if (__glutCurrentWindow.fpsCounter) {
        /* Show the frames per second over the top of the animation */
        frameCount++;
        GETTIMEOFDAY(&now);
        TIMEDELTA(delta,now,prev);
        if (delta.tv_sec >= 1) {
            fpsRate = (int)(10000000L / (delta.tv_sec * 1000000L + delta.tv_usec)) * frameCount;
            frameCount = 0;
            prev = now;
            }
        sprintf(buf,"%3d.%d fps", fpsRate / 10, fpsRate % 10);
        MGL_setColorRGB(0xFF,0xFF,0xFF);
        MGL_setTextJustify(MGL_RIGHT_TEXT,MGL_BOTTOM_TEXT);
        MGL_drawStrXY(MGL_maxx(),MGL_maxy(),buf);
        }
    /* Update window directly from offscreen rendering context if not fullscreen mode */
    if (__glutCurrentWindow.windc)
        MGL_bitBltCoord(__glutCurrentWindow.windc,__glutCurrentWindow.dc,0,0,__glutCurrentWindow.windc->mi.xRes+1,__glutCurrentWindow.windc->mi.yRes+1,0,0,MGL_REPLACE_MODE);
    else
        MGL_glSwapBuffers(__glutCurrentWindow.dc,__glutCurrentWindow.waitVRT);
}

void glutDisplayFunc(GLUTdisplayCB displayFunc)
{
    __glutCurrentWindow.display = displayFunc;
}

void glutKeyboardFunc(GLUTkeyboardCB keyboardFunc)
{
    __glutCurrentWindow.keyboard = keyboardFunc;
}

void glutSpecialFunc(GLUTspecialCB specialFunc)
{
    __glutCurrentWindow.special = specialFunc;
}

void glutMouseFunc(GLUTmouseCB mouseFunc)
{
    __glutCurrentWindow.mouse = mouseFunc;
}

void glutMotionFunc(GLUTmotionCB motionFunc)
{
    __glutCurrentWindow.motion = motionFunc;
}

void glutPassiveMotionFunc(GLUTpassiveCB passiveMotionFunc)
{
    __glutCurrentWindow.passive = passiveMotionFunc;
}

void glutEntryFunc(GLUTentryCB entryFunc)
{
    __glutCurrentWindow.entry = entryFunc;
}

void glutVisibilityFunc(GLUTvisibilityCB visibilityFunc)
{
    __glutCurrentWindow.visibility = visibilityFunc;
}

void glutReshapeFunc(GLUTreshapeCB reshapeFunc)
{
    if (reshapeFunc) {
        __glutCurrentWindow.reshape = reshapeFunc;
        }
    else {
        __glutCurrentWindow.reshape = __glutDefaultReshape;
        }
}
