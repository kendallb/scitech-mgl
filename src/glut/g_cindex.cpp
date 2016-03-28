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

void glutSetColor(int ndx, GLfloat red, GLfloat green, GLfloat blue)
{
    palette_t pal;

    pal.red = (uchar)(red * 255.0);
    pal.green = (uchar)(green * 255.0),
    pal.blue = (uchar)(blue * 255.0),
    MGL_glSetPalette(__glutCurrentWindow.dc,&pal,1,ndx);
}

GLfloat glutGetColor(int ndx, int comp)
{
    uchar   red,green,blue;

    MGL_getPaletteEntry(__glutCurrentWindow.dc,ndx,&red,&green,&blue);
    switch (comp) {
        case GLUT_RED:
            return (float)red / 255.0;
        case GLUT_BLUE:
            return (float)blue / 255.0;
        case GLUT_GREEN:
            return (float)green / 255.0;
        }
    __glutWarning("invalid component in glutGetColor()!");
    return -1.0;
}

void glutCopyColormap(int)
{
}
