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

int glutGet(GLenum param)
{
    MGLVisual   visual;

    switch (param) {
        case GLUT_INIT_WINDOW_X:
            return __glutInitX;
        case GLUT_INIT_WINDOW_Y:
            return __glutInitY;
        case GLUT_INIT_WINDOW_WIDTH:
            return __glutInitWidth;
        case GLUT_INIT_WINDOW_HEIGHT:
            return __glutInitHeight;
        case GLUT_INIT_DISPLAY_MODE:
            return __glutDisplayMode;
        case GLUT_WINDOW_X:
            return 0;
        case GLUT_WINDOW_Y:
            return 0;
        case GLUT_WINDOW_WIDTH:
            return __glutCurrentWindow.dc->mi.xRes+1;
        case GLUT_WINDOW_HEIGHT:
            return __glutCurrentWindow.dc->mi.yRes+1;
        case GLUT_WINDOW_BUFFER_SIZE:
            return __glutCurrentWindow.dc->mi.bitsPerPixel;
        case GLUT_WINDOW_STENCIL_SIZE:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.stencil_size;
        case GLUT_WINDOW_DEPTH_SIZE:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.depth_size;
        case GLUT_WINDOW_RED_SIZE:
            return __glutCurrentWindow.dc->pf.redPos;
        case GLUT_WINDOW_GREEN_SIZE:
            return __glutCurrentWindow.dc->pf.greenPos;
        case GLUT_WINDOW_BLUE_SIZE:
            return __glutCurrentWindow.dc->pf.bluePos;
        case GLUT_WINDOW_ALPHA_SIZE:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.alpha_size;
        case GLUT_WINDOW_ACCUM_RED_SIZE:
            return 0;   // TODO: What do we return for this?
        case GLUT_WINDOW_ACCUM_GREEN_SIZE:
            return 0;   // TODO: What do we return for this?
        case GLUT_WINDOW_ACCUM_BLUE_SIZE:
            return 0;   // TODO: What do we return for this?
        case GLUT_WINDOW_ACCUM_ALPHA_SIZE:
            return 0;   // TODO: What do we return for this?
        case GLUT_WINDOW_DOUBLEBUFFER:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.db_flag;
        case GLUT_WINDOW_RGBA:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.rgb_flag;
        case GLUT_WINDOW_COLORMAP_SIZE:
            if (__glutCurrentWindow.dc->mi.bitsPerPixel == 8)
                return 256;
            return 0;
        case GLUT_WINDOW_PARENT:
            return 0;
        case GLUT_WINDOW_NUM_CHILDREN:
            return 0;
        case GLUT_WINDOW_NUM_SAMPLES:
            return 0;
        case GLUT_WINDOW_STEREO:
            MGL_glGetVisual(__glutCurrentWindow.dc,&visual);
            return visual.stereo_flag;
        case GLUT_WINDOW_CURSOR:
            return 0;
        case GLUT_SCREEN_WIDTH:
            return __glutCurrentWindow.dc->mi.xRes+1;
        case GLUT_SCREEN_HEIGHT:
            return __glutCurrentWindow.dc->mi.xRes+1;
        case GLUT_SCREEN_WIDTH_MM:
            __glutWarning("SCREEN_WIDTH_MM unimplemented in GLUT for MGL");
            return 0;
        case GLUT_SCREEN_HEIGHT_MM:
            __glutWarning("SCREEN_HEIGHT_MM unimplemented in GLUT for MGL");
            return 0;
        case GLUT_MENU_NUM_ITEMS:
            return 0;
        case GLUT_DISPLAY_MODE_POSSIBLE:
            // TODO: We should implenent this but it may get complicated
            //       if they call this before the MGL has initializes the
            //       display device context.
            return 0;
        case GLUT_ELAPSED_TIME: {
            GLUTtimeval elapsed, beginning, now;

            __glutInitTime(&beginning);
            GETTIMEOFDAY(&now);
            TIMEDELTA(elapsed, now, beginning);
            return (int) ((elapsed.tv_sec * 1000) + (elapsed.tv_usec / 1000));
            }
        default:
            __glutWarning("invalid glutGet parameter: %d", param);
            return -1;
        }
}
