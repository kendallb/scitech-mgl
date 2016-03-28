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

void glutEstablishOverlay(void)
{
}

void glutRemoveOverlay(void)
{
}

void glutUseLayer(GLenum)
{
}

void glutPostOverlayRedisplay(void)
{
}

void glutOverlayDisplayFunc(GLUTdisplayCB)
{
}

void glutHideOverlay(void)
{
}

void glutShowOverlay(void)
{
}

int glutLayerGet(GLenum param)
{
    switch (param) {
        case GLUT_OVERLAY_POSSIBLE:
            return false;
        case GLUT_LAYER_IN_USE:
            return 0;
        case GLUT_HAS_OVERLAY:
            return 0;
        case GLUT_TRANSPARENT_INDEX:
            return 0;
        case GLUT_NORMAL_DAMAGED:
            return 0;
        case GLUT_OVERLAY_DAMAGED:
            return 0;
        default:
            __glutWarning("invalid glutLayerGet param: %d", param);
            return -1;
        }
}
