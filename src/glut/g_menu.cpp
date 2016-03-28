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

/* Obsolete? added back so some applications will continue to compile */
void glutMenuStateFunc(_glutMenuStateFunc )
{
}

void glutMenuStatusFunc(_glutMenuStatusFunc )
{
}

int glutCreateMenu(GLUTselectCB)
{
    return 0;
}

void glutDestroyMenu(int)
{
}

int glutGetMenu(void)
{
    return 0;
}

void glutSetMenu(int)
{
}

void glutAddMenuEntry(char *, int)
{
}

void glutAddSubMenu(char *, int)
{
}

void glutChangeToMenuEntry(int, char *, int)
{
}

void glutChangeToSubMenu(int, char *, int)
{
}

void glutRemoveMenuItem(int)
{
}

void glutAttachMenu(int)
{
}

void glutDetachMenu(int)
{
}
