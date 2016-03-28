/****************************************************************************
*
*                       SciTech MGL Graphics Library
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
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
*
* Language:     ANSI C
* Environment:  IBM PC (MS DOS)
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
*
****************************************************************************/

#include "glutint.h"

/*------------------------- Implementation --------------------------------*/

int glutDeviceGet(GLenum param)
{
    switch (param) {
        case GLUT_HAS_KEYBOARD:
            return true;
        case GLUT_HAS_MOUSE:
            return true;
        case GLUT_HAS_SPACEBALL:
            return false;
        case GLUT_HAS_DIAL_AND_BUTTON_BOX:
            return false;
        case GLUT_HAS_TABLET:
            return false;
        case GLUT_NUM_MOUSE_BUTTONS:
            return 2;
        case GLUT_NUM_SPACEBALL_BUTTONS:
            return 0;
        case GLUT_NUM_BUTTON_BOX_BUTTONS:
            return 0;
        case GLUT_NUM_DIALS:
            return 0;
        case GLUT_NUM_TABLET_BUTTONS:
            return 0;
        default:
            __glutWarning("invalid glutDeviceGet parameter: %d", param);
            return -1;
        }
}


