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
#include "gstroke.h"

/*------------------------- Implementation --------------------------------*/

void glutStrokeCharacter(GLUTstrokeFont font, int c)
{
  StrokeCharPtr ch;
  StrokePtr stroke;
  CoordPtr coord;
  StrokeFontPtr fontinfo = (StrokeFontPtr) font;
  int i, j;

  if (c < 0 || c >= fontinfo->num_chars)
    return;
  ch = &(fontinfo->ch[c]);
  if (ch) {
    for (i = ch->num_strokes, stroke = ch->stroke;
      i > 0; i--, stroke++) {
      glBegin(GL_LINE_STRIP);
      for (j = stroke->num_coords, coord = stroke->coord;
        j > 0; j--, coord++) {
        glVertex2f(coord->x, coord->y);
      }
      glEnd();
    }
    glTranslatef(ch->right, 0.0, 0.0);
  }
}


