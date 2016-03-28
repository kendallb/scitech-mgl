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

int glutGetModifiers(void)
{
    int modifiers = 0;

    if (__glutModifierMask == (uint)~0) {
        __glutWarning("glutGetModifiers: do not call outside core input callback.");
        return 0;
        }
    /* Convert from MGL modifiers to GLUT modifiers. The __glutModifierMask
     * contains the modifiers field from the last MGL input event.
     */
    if (__glutModifierMask & EVT_SHIFTKEY)
        modifiers |= GLUT_ACTIVE_SHIFT;
    if (__glutModifierMask & EVT_CTRLSTATE)
        modifiers |= GLUT_ACTIVE_CTRL;
    if (__glutModifierMask & EVT_ALTSTATE)
        modifiers |= GLUT_ACTIVE_ALT;
    return modifiers;
}


