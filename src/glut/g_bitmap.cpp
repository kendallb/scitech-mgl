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

void glutBitmapCharacter(GLUTbitmapFont font, int c)
{
  BitmapCharPtr ch;
  BitmapFontPtr fontinfo = (BitmapFontPtr) font;
  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;

  if (c < fontinfo->first ||
    c >= fontinfo->first + fontinfo->num_chars)
    return;
  ch = fontinfo->ch[c - fontinfo->first];
  if (ch) {
    /* save current modes */
    glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapbytes);
    glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbfirst);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowlength);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skiprows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skippixels);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    /* Little endian machines (DEC Alpha for example) could
       benefit from setting GL_UNPACK_LSB_FIRST to GL_TRUE
       instead of GL_FALSE, but this would require changing the
       generated bitmaps too. */
    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBitmap(ch->width, ch->height, ch->xorig, ch->yorig,
      ch->advance, 0, ch->bitmap);
    /* restore saved modes */
    glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
    glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
  }
}


