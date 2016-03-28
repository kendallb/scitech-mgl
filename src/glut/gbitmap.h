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

#ifndef __glutbitmap_h__
#define __glutbitmap_h__

/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GLsizei width;
  GLsizei height;
  GLfloat xorig;
  GLfloat yorig;
  GLfloat advance;
  GLubyte *bitmap;
} BitmapCharRec, *BitmapCharPtr;

typedef struct {
  char *name;
  int num_chars;
  int first;
  BitmapCharPtr *ch;
} BitmapFontRec, *BitmapFontPtr;

typedef void *GLUTbitmapFont;

#ifdef __cplusplus
}
#endif

#endif /* __glutbitmap_h__ */


