/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Windows
*
* Description:  Stubs binding OpenGL functions by calling the appropriate
*               internal function for calls not made via the entry points
*               table.
*
****************************************************************************/

#define NO_GL_MACROS
#include "gl\gl.h"
#include "gl\glu.h"

/*------------------------- Implementation --------------------------------*/

#ifdef  NO_ASSEMBLER

#define MKSTUB(name,proto,args)             \
void APIENTRY name##proto                   \
{ _MGL_glFuncs.name##args; }

#define MKSTUB_RET(type,name,proto,args)    \
type APIENTRY name##proto                   \
{ return _MGL_glFuncs.name##args; }

MKSTUB(glAccum,(GLenum op, GLfloat value),(op,value));
MKSTUB(glAlphaFunc,(GLenum func, GLclampf ref),(func,ref));
MKSTUB_RET(GLboolean,glAreTexturesResident,(GLsizei n, const GLuint *textures, GLboolean *residences),(n,textures,residences));
MKSTUB(glArrayElement,(GLint i),(i));
MKSTUB(glBegin,(GLenum mode),(mode));
MKSTUB(glBindTexture,(GLenum target, GLuint texture),(target,texture));
MKSTUB(glBitmap,(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap),(width, height, xorig, yorig, xmove,ymove,bitmap));
MKSTUB(glBlendFunc,(GLenum sfactor, GLenum dfactor),(sfactor,dfactor));
MKSTUB(glCallList,(GLuint list),(list));
MKSTUB(glCallLists,(GLsizei n, GLenum type, const GLvoid *lists),(n, type, lists));
MKSTUB(glClear,(GLbitfield mask),(mask));
MKSTUB(glClearAccum,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha),(red, green, blue, alpha));
MKSTUB(glClearColor,(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha),(red, green, blue, alpha));
MKSTUB(glClearDepth,(GLclampd depth),(depth));
MKSTUB(glClearIndex,(GLfloat c),(c));
MKSTUB(glClearStencil,(GLint s),(s));
MKSTUB(glClipPlane,(GLenum plane, const GLdouble *equation),(plane, equation));
MKSTUB(glColor3b,(GLbyte red, GLbyte green, GLbyte blue),(red, green, blue));
MKSTUB(glColor3bv,(const GLbyte *v),(v));
MKSTUB(glColor3d,(GLdouble red, GLdouble green, GLdouble blue),(red, green, blue));
MKSTUB(glColor3dv,(const GLdouble *v),(v));
MKSTUB(glColor3f,(GLfloat red, GLfloat green, GLfloat blue),(red, green, blue));
MKSTUB(glColor3fv,(const GLfloat *v),(v));
MKSTUB(glColor3i,(GLint red, GLint green, GLint blue),(red, green, blue));
MKSTUB(glColor3iv,(const GLint *v),(v));
MKSTUB(glColor3s,(GLshort red, GLshort green, GLshort blue),(red, green, blue));
MKSTUB(glColor3sv,(const GLshort *v),(v));
MKSTUB(glColor3ub,(GLubyte red, GLubyte green, GLubyte blue),(red, green, blue));
MKSTUB(glColor3ubv,(const GLubyte *v),(v));
MKSTUB(glColor3ui,(GLuint red, GLuint green, GLuint blue),(red, green, blue));
MKSTUB(glColor3uiv,(const GLuint *v),(v));
MKSTUB(glColor3us,(GLushort red, GLushort green, GLushort blue),(red, green, blue));
MKSTUB(glColor3usv,(const GLushort *v),(v));
MKSTUB(glColor4b,(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha),(red, green, blue, alpha));
MKSTUB(glColor4bv,(const GLbyte *v),(v));
MKSTUB(glColor4d,(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha),(red, green, blue, alpha));
MKSTUB(glColor4dv,(const GLdouble *v),(v));
MKSTUB(glColor4f,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha),(red, green, blue, alpha));
MKSTUB(glColor4fv,(const GLfloat *v),(v));
MKSTUB(glColor4i,(GLint red, GLint green, GLint blue, GLint alpha),(red, green, blue, alpha));
MKSTUB(glColor4iv,(const GLint *v),(v));
MKSTUB(glColor4s,(GLshort red, GLshort green, GLshort blue, GLshort alpha),(red, green, blue, alpha));
MKSTUB(glColor4sv,(const GLshort *v),(v));
MKSTUB(glColor4ub,(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha),(red, green, blue, alpha));
MKSTUB(glColor4ubv,(const GLubyte *v),(v));
MKSTUB(glColor4ui,(GLuint red, GLuint green, GLuint blue, GLuint alpha),(red, green, blue, alpha));
MKSTUB(glColor4uiv,(const GLuint *v),(v));
MKSTUB(glColor4us,(GLushort red, GLushort green, GLushort blue, GLushort alpha),(red, green, blue, alpha));
MKSTUB(glColor4usv,(const GLushort *v),(v));
MKSTUB(glColorMask,(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha),(red, green, blue, alpha));
MKSTUB(glColorMaterial,(GLenum face, GLenum mode),(face, mode));
MKSTUB(glColorPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size, type, stride, pointer));
MKSTUB(glCopyPixels,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type),(x, y, width, height, type));
MKSTUB(glCopyTexImage1D,(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border),(target, level, internalformat, x, y, width, border));
MKSTUB(glCopyTexImage2D,(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border),(target, level, internalformat, x, y, width, height, border));
MKSTUB(glCopyTexSubImage1D,(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width),(target, level, xoffset, x, y, width));
MKSTUB(glCopyTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height),(target, level, xoffset, yoffset, x, y, width, height));
MKSTUB(glCullFace,(GLenum mode),(mode));
MKSTUB(glDeleteLists,(GLuint list, GLsizei range),(list, range));
MKSTUB(glDeleteTextures,(GLsizei n, const GLuint *textures),(n, textures));
MKSTUB(glDepthFunc,(GLenum func),(func));
MKSTUB(glDepthMask,(GLboolean flag),(flag));
MKSTUB(glDepthRange,(GLclampd zNear, GLclampd zFar),(zNear, zFar));
MKSTUB(glDisable,(GLenum cap),(cap));
MKSTUB(glDisableClientState,(GLenum array),(array));
MKSTUB(glDrawArrays,(GLenum mode, GLint first, GLsizei count),(mode, first, count));
//MKSTUB(glDrawBuffer,(GLenum mode),(mode));
MKSTUB(glDrawElements,(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices),(mode, count, type, indices));
MKSTUB(glDrawPixels,(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels),(width, height, format, type, pixels));
MKSTUB(glEdgeFlag,(GLboolean flag),(flag));
MKSTUB(glEdgeFlagPointer,(GLsizei stride, const GLboolean *pointer),(stride, pointer));
MKSTUB(glEdgeFlagv,(const GLboolean *flag),(flag));
MKSTUB(glEnable,(GLenum cap),(cap));
MKSTUB(glEnableClientState,(GLenum array),(array));
MKSTUB(glEnd,(void),());
MKSTUB(glEndList,(void),());
MKSTUB(glEvalCoord1d,(GLdouble u),(u));
MKSTUB(glEvalCoord1dv,(const GLdouble *u),(u));
MKSTUB(glEvalCoord1f,(GLfloat u),(u));
MKSTUB(glEvalCoord1fv,(const GLfloat *u),(u));
MKSTUB(glEvalCoord2d,(GLdouble u, GLdouble v),(u, v));
MKSTUB(glEvalCoord2dv,(const GLdouble *u),(u));
MKSTUB(glEvalCoord2f,(GLfloat u, GLfloat v),(u, v));
MKSTUB(glEvalCoord2fv,(const GLfloat *u),(u));
MKSTUB(glEvalMesh1,(GLenum mode, GLint i1, GLint i2),(mode, i1, i2));
MKSTUB(glEvalMesh2,(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2),(mode, i1, i2, j1, j2));
MKSTUB(glEvalPoint1,(GLint i),(i));
MKSTUB(glEvalPoint2,(GLint i, GLint j),(i, j));
MKSTUB(glFeedbackBuffer,(GLsizei size, GLenum type, GLfloat *buffer),(size, type, buffer));
MKSTUB(glFinish,(void),());
MKSTUB(glFlush,(void),());
MKSTUB(glFogf,(GLenum pname, GLfloat param),(pname, param));
MKSTUB(glFogfv,(GLenum pname, const GLfloat *params),(pname, params));
MKSTUB(glFogi,(GLenum pname, GLint param),(pname, param));
MKSTUB(glFogiv,(GLenum pname, const GLint *params),(pname, params));
MKSTUB(glFrontFace,(GLenum mode),(mode));
MKSTUB(glFrustum,(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar),(left, right, bottom, top, zNear, zFar));
MKSTUB_RET(GLuint,glGenLists,(GLsizei range),(range));
MKSTUB(glGenTextures,(GLsizei n, GLuint *textures),(n, textures));
MKSTUB(glGetBooleanv,(GLenum pname, GLboolean *params),(pname, params));
MKSTUB(glGetClipPlane,(GLenum plane, GLdouble *equation),(plane, equation));
MKSTUB(glGetDoublev,(GLenum pname, GLdouble *params),(pname, params));
MKSTUB_RET(GLenum,glGetError,(void),());
MKSTUB(glGetFloatv,(GLenum pname, GLfloat *params),(pname, params));
MKSTUB(glGetIntegerv,(GLenum pname, GLint *params),(pname, params));
MKSTUB(glGetLightfv,(GLenum light, GLenum pname, GLfloat *params),(light, pname, params));
MKSTUB(glGetLightiv,(GLenum light, GLenum pname, GLint *params),(light, pname, params));
MKSTUB(glGetMapdv,(GLenum target, GLenum query, GLdouble *v),(target, query, v));
MKSTUB(glGetMapfv,(GLenum target, GLenum query, GLfloat *v),(target, query, v));
MKSTUB(glGetMapiv,(GLenum target, GLenum query, GLint *v),(target, query, v));
MKSTUB(glGetMaterialfv,(GLenum face, GLenum pname, GLfloat *params),(face, pname, params));
MKSTUB(glGetMaterialiv,(GLenum face, GLenum pname, GLint *params),(face, pname, params));
MKSTUB(glGetPixelMapfv,(GLenum map, GLfloat *values),(map, values));
MKSTUB(glGetPixelMapuiv,(GLenum map, GLuint *values),(map, values));
MKSTUB(glGetPixelMapusv,(GLenum map, GLushort *values),(map, values));
MKSTUB(glGetPointerv,(GLenum pname, GLvoid* *params),(pname, params));
MKSTUB(glGetPolygonStipple,(GLubyte *mask),(mask));
MKSTUB_RET(const GLubyte *,glGetString,(GLenum name),(name));
MKSTUB(glGetTexEnvfv,(GLenum target, GLenum pname, GLfloat *params),(target, pname, params));
MKSTUB(glGetTexEnviv,(GLenum target, GLenum pname, GLint *params),(target, pname, params));
MKSTUB(glGetTexGendv,(GLenum coord, GLenum pname, GLdouble *params),(coord, pname, params));
MKSTUB(glGetTexGenfv,(GLenum coord, GLenum pname, GLfloat *params),(coord, pname, params));
MKSTUB(glGetTexGeniv,(GLenum coord, GLenum pname, GLint *params),(coord, pname, params));
MKSTUB(glGetTexImage,(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels),(target, level, format, type, pixels));
MKSTUB(glGetTexLevelParameterfv,(GLenum target, GLint level, GLenum pname, GLfloat *params),(target, level, pname, params));
MKSTUB(glGetTexLevelParameteriv,(GLenum target, GLint level, GLenum pname, GLint *params),(target, level, pname, params));
MKSTUB(glGetTexParameterfv,(GLenum target, GLenum pname, GLfloat *params),(target, pname, params));
MKSTUB(glGetTexParameteriv,(GLenum target, GLenum pname, GLint *params),(target, pname, params));
MKSTUB(glHint,(GLenum target, GLenum mode),(target, mode));
MKSTUB(glIndexMask,(GLuint mask),(mask));
MKSTUB(glIndexPointer,(GLenum type, GLsizei stride, const GLvoid *pointer),(type, stride, pointer));
MKSTUB(glIndexd,(GLdouble c),(c));
MKSTUB(glIndexdv,(const GLdouble *c),(c));
MKSTUB(glIndexf,(GLfloat c),(c));
MKSTUB(glIndexfv,(const GLfloat *c),(c));
MKSTUB(glIndexi,(GLint c),(c));
MKSTUB(glIndexiv,(const GLint *c),(c));
MKSTUB(glIndexs,(GLshort c),(c));
MKSTUB(glIndexsv,(const GLshort *c),(c));
MKSTUB(glIndexub,(GLubyte c),(c));
MKSTUB(glIndexubv,(const GLubyte *c),(c));
MKSTUB(glInitNames,(void),());
MKSTUB(glInterleavedArrays,(GLenum format, GLsizei stride, const GLvoid *pointer),(format, stride, pointer));
MKSTUB_RET(GLboolean,glIsEnabled,(GLenum cap),(cap));
MKSTUB_RET(GLboolean,glIsList,(GLuint list),(list));
MKSTUB_RET(GLboolean,glIsTexture,(GLuint texture),(texture));
MKSTUB(glLightModelf,(GLenum pname, GLfloat param),(pname, param));
MKSTUB(glLightModelfv,(GLenum pname, const GLfloat *params),(pname, params));
MKSTUB(glLightModeli,(GLenum pname, GLint param),(pname, param));
MKSTUB(glLightModeliv,(GLenum pname, const GLint *params),(pname, params));
MKSTUB(glLightf,(GLenum light, GLenum pname, GLfloat param),(light, pname, param));
MKSTUB(glLightfv,(GLenum light, GLenum pname, const GLfloat *params),(light, pname, params));
MKSTUB(glLighti,(GLenum light, GLenum pname, GLint param),(light, pname, param));
MKSTUB(glLightiv,(GLenum light, GLenum pname, const GLint *params),(light, pname, params));
MKSTUB(glLineStipple,(GLint factor, GLushort pattern),(factor, pattern));
MKSTUB(glLineWidth,(GLfloat width),(width));
MKSTUB(glListBase,(GLuint base),(base));
MKSTUB(glLoadIdentity,(void),());
MKSTUB(glLoadMatrixd,(const GLdouble *m),(m));
MKSTUB(glLoadMatrixf,(const GLfloat *m),(m));
MKSTUB(glLoadName,(GLuint name),(name));
MKSTUB(glLogicOp,(GLenum opcode),(opcode));
MKSTUB(glMap1d,(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points),(target, u1, u2, stride, order, points));
MKSTUB(glMap1f,(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points),(target, u1, u2, stride, order, points));
MKSTUB(glMap2d,(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points),(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points));
MKSTUB(glMap2f,(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points),(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points));
MKSTUB(glMapGrid1d,(GLint un, GLdouble u1, GLdouble u2),(un, u1, u2));
MKSTUB(glMapGrid1f,(GLint un, GLfloat u1, GLfloat u2),(un, u1, u2));
MKSTUB(glMapGrid2d,(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2),(un, u1, u2, vn, v1, v2));
MKSTUB(glMapGrid2f,(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2),(un, u1, u2, vn, v1, v2));
MKSTUB(glMaterialf,(GLenum face, GLenum pname, GLfloat param),(face, pname, param));
MKSTUB(glMaterialfv,(GLenum face, GLenum pname, const GLfloat *params),(face, pname, params));
MKSTUB(glMateriali,(GLenum face, GLenum pname, GLint param),(face, pname, param));
MKSTUB(glMaterialiv,(GLenum face, GLenum pname, const GLint *params),(face, pname, params));
MKSTUB(glMatrixMode,(GLenum mode),(mode));
MKSTUB(glMultMatrixd,(const GLdouble *m),(m));
MKSTUB(glMultMatrixf,(const GLfloat *m),(m));
MKSTUB(glNewList,(GLuint list, GLenum mode),(list, mode));
MKSTUB(glNormal3b,(GLbyte nx, GLbyte ny, GLbyte nz),(nx, ny, nz));
MKSTUB(glNormal3bv,(const GLbyte *v),(v));
MKSTUB(glNormal3d,(GLdouble nx, GLdouble ny, GLdouble nz),(nx, ny, nz));
MKSTUB(glNormal3dv,(const GLdouble *v),(v));
MKSTUB(glNormal3f,(GLfloat nx, GLfloat ny, GLfloat nz),(nx, ny, nz));
MKSTUB(glNormal3fv,(const GLfloat *v),(v));
MKSTUB(glNormal3i,(GLint nx, GLint ny, GLint nz),(nx, ny, nz));
MKSTUB(glNormal3iv,(const GLint *v),(v));
MKSTUB(glNormal3s,(GLshort nx, GLshort ny, GLshort nz),(nx, ny, nz));
MKSTUB(glNormal3sv,(const GLshort *v),(v));
MKSTUB(glNormalPointer,(GLenum type, GLsizei stride, const GLvoid *pointer),(type, stride, pointer));
MKSTUB(glOrtho,(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar),(left, right, bottom, top, zNear, zFar));
MKSTUB(glPassThrough,(GLfloat token),(token));
MKSTUB(glPixelMapfv,(GLenum map, GLint mapsize, const GLfloat *values),(map, mapsize, values));
MKSTUB(glPixelMapuiv,(GLenum map, GLint mapsize, const GLuint *values),(map, mapsize, values));
MKSTUB(glPixelMapusv,(GLenum map, GLint mapsize, const GLushort *values),(map, mapsize, values));
MKSTUB(glPixelStoref,(GLenum pname, GLfloat param),(pname, param));
MKSTUB(glPixelStorei,(GLenum pname, GLint param),(pname, param));
MKSTUB(glPixelTransferf,(GLenum pname, GLfloat param),(pname, param));
MKSTUB(glPixelTransferi,(GLenum pname, GLint param),(pname, param));
MKSTUB(glPixelZoom,(GLfloat xfactor, GLfloat yfactor),(xfactor, yfactor));
MKSTUB(glPointSize,(GLfloat size),(size));
MKSTUB(glPolygonMode,(GLenum face, GLenum mode),(face, mode));
MKSTUB(glPolygonOffset,(GLfloat factor, GLfloat units),(factor, units));
MKSTUB(glPolygonStipple,(const GLubyte *mask),(mask));
MKSTUB(glPopAttrib,(void),());
MKSTUB(glPopClientAttrib,(void),());
MKSTUB(glPopMatrix,(void),());
MKSTUB(glPopName,(void),());
MKSTUB(glPrioritizeTextures,(GLsizei n, const GLuint *textures, const GLclampf *priorities),(n, textures, priorities));
MKSTUB(glPushAttrib,(GLbitfield mask),(mask));
MKSTUB(glPushClientAttrib,(GLbitfield mask),(mask));
MKSTUB(glPushMatrix,(void),());
MKSTUB(glPushName,(GLuint name),(name));
MKSTUB(glRasterPos2d,(GLdouble x, GLdouble y),(x, y));
MKSTUB(glRasterPos2dv,(const GLdouble *v),(v));
MKSTUB(glRasterPos2f,(GLfloat x, GLfloat y),(x, y));
MKSTUB(glRasterPos2fv,(const GLfloat *v),(v));
MKSTUB(glRasterPos2i,(GLint x, GLint y),(x, y));
MKSTUB(glRasterPos2iv,(const GLint *v),(v));
MKSTUB(glRasterPos2s,(GLshort x, GLshort y),(x, y));
MKSTUB(glRasterPos2sv,(const GLshort *v),(v));
MKSTUB(glRasterPos3d,(GLdouble x, GLdouble y, GLdouble z),(x, y, z));
MKSTUB(glRasterPos3dv,(const GLdouble *v),(v));
MKSTUB(glRasterPos3f,(GLfloat x, GLfloat y, GLfloat z),(x, y, z));
MKSTUB(glRasterPos3fv,(const GLfloat *v),(v));
MKSTUB(glRasterPos3i,(GLint x, GLint y, GLint z),(x, y, z));
MKSTUB(glRasterPos3iv,(const GLint *v),(v));
MKSTUB(glRasterPos3s,(GLshort x, GLshort y, GLshort z),(x, y, z));
MKSTUB(glRasterPos3sv,(const GLshort *v),(v));
MKSTUB(glRasterPos4d,(GLdouble x, GLdouble y, GLdouble z, GLdouble w),(x, y, z, w));
MKSTUB(glRasterPos4dv,(const GLdouble *v),(v));
MKSTUB(glRasterPos4f,(GLfloat x, GLfloat y, GLfloat z, GLfloat w),(x, y, z, w));
MKSTUB(glRasterPos4fv,(const GLfloat *v),(v));
MKSTUB(glRasterPos4i,(GLint x, GLint y, GLint z, GLint w),(x, y, z, w));
MKSTUB(glRasterPos4iv,(const GLint *v),(v));
MKSTUB(glRasterPos4s,(GLshort x, GLshort y, GLshort z, GLshort w),(x, y, z, w));
MKSTUB(glRasterPos4sv,(const GLshort *v),(v));
MKSTUB(glReadBuffer,(GLenum mode),(mode));
MKSTUB(glReadPixels,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels),(x, y, width, height, format, type, pixels));
MKSTUB(glRectd,(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2),(x1, y1, x2, y2));
MKSTUB(glRectdv,(const GLdouble *v1, const GLdouble *v2),(v1, v2));
MKSTUB(glRectf,(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2),(x1, y1, x2, y2));
MKSTUB(glRectfv,(const GLfloat *v1, const GLfloat *v2),(v1, v2));
MKSTUB(glRecti,(GLint x1, GLint y1, GLint x2, GLint y2),(x1, y1, x2, y2));
MKSTUB(glRectiv,(const GLint *v1, const GLint *v2),(v1, v2));
MKSTUB(glRects,(GLshort x1, GLshort y1, GLshort x2, GLshort y2),(x1, y1, x2, y2));
MKSTUB(glRectsv,(const GLshort *v1, const GLshort *v2),(v1, v2));
MKSTUB_RET(GLint,glRenderMode,(GLenum mode),(mode));
MKSTUB(glRotated,(GLdouble angle, GLdouble x, GLdouble y, GLdouble z),(angle, x, y, z));
MKSTUB(glRotatef,(GLfloat angle, GLfloat x, GLfloat y, GLfloat z),(angle, x, y, z));
MKSTUB(glScaled,(GLdouble x, GLdouble y, GLdouble z),(x, y, z));
MKSTUB(glScalef,(GLfloat x, GLfloat y, GLfloat z),(x, y, z));
MKSTUB(glScissor,(GLint x, GLint y, GLsizei width, GLsizei height),(x, y, width, height));
MKSTUB(glSelectBuffer,(GLsizei size, GLuint *buffer),(size, buffer));
MKSTUB(glShadeModel,(GLenum mode),(mode));
MKSTUB(glStencilFunc,(GLenum func, GLint ref, GLuint mask),(func, ref, mask));
MKSTUB(glStencilMask,(GLuint mask),(mask));
MKSTUB(glStencilOp,(GLenum fail, GLenum zfail, GLenum zpass),(fail, zfail, zpass));
MKSTUB(glTexCoord1d,(GLdouble s),(s));
MKSTUB(glTexCoord1dv,(const GLdouble *v),(v));
MKSTUB(glTexCoord1f,(GLfloat s),(s));
MKSTUB(glTexCoord1fv,(const GLfloat *v),(v));
MKSTUB(glTexCoord1i,(GLint s),(s));
MKSTUB(glTexCoord1iv,(const GLint *v),(v));
MKSTUB(glTexCoord1s,(GLshort s),(s));
MKSTUB(glTexCoord1sv,(const GLshort *v),(v));
MKSTUB(glTexCoord2d,(GLdouble s, GLdouble t),(s, t));
MKSTUB(glTexCoord2dv,(const GLdouble *v),(v));
MKSTUB(glTexCoord2f,(GLfloat s, GLfloat t),(s, t));
MKSTUB(glTexCoord2fv,(const GLfloat *v),(v));
MKSTUB(glTexCoord2i,(GLint s, GLint t),(s, t));
MKSTUB(glTexCoord2iv,(const GLint *v),(v));
MKSTUB(glTexCoord2s,(GLshort s, GLshort t),(s, t));
MKSTUB(glTexCoord2sv,(const GLshort *v),(v));
MKSTUB(glTexCoord3d,(GLdouble s, GLdouble t, GLdouble r),(s, t, r));
MKSTUB(glTexCoord3dv,(const GLdouble *v),(v));
MKSTUB(glTexCoord3f,(GLfloat s, GLfloat t, GLfloat r),(s, t, r));
MKSTUB(glTexCoord3fv,(const GLfloat *v),(v));
MKSTUB(glTexCoord3i,(GLint s, GLint t, GLint r),(s, t, r));
MKSTUB(glTexCoord3iv,(const GLint *v),(v));
MKSTUB(glTexCoord3s,(GLshort s, GLshort t, GLshort r),(s, t, r));
MKSTUB(glTexCoord3sv,(const GLshort *v),(v));
MKSTUB(glTexCoord4d,(GLdouble s, GLdouble t, GLdouble r, GLdouble q),(s, t, r, q));
MKSTUB(glTexCoord4dv,(const GLdouble *v),(v));
MKSTUB(glTexCoord4f,(GLfloat s, GLfloat t, GLfloat r, GLfloat q),(s, t, r, q));
MKSTUB(glTexCoord4fv,(const GLfloat *v),(v));
MKSTUB(glTexCoord4i,(GLint s, GLint t, GLint r, GLint q),(s, t, r, q));
MKSTUB(glTexCoord4iv,(const GLint *v),(v));
MKSTUB(glTexCoord4s,(GLshort s, GLshort t, GLshort r, GLshort q),(s, t, r, q));
MKSTUB(glTexCoord4sv,(const GLshort *v),(v));
MKSTUB(glTexCoordPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size, type, stride, pointer));
MKSTUB(glTexEnvf,(GLenum target, GLenum pname, GLfloat param),(target, pname, param));
MKSTUB(glTexEnvfv,(GLenum target, GLenum pname, const GLfloat *params),(target, pname, params));
MKSTUB(glTexEnvi,(GLenum target, GLenum pname, GLint param),(target, pname, param));
MKSTUB(glTexEnviv,(GLenum target, GLenum pname, const GLint *params),(target, pname, params));
MKSTUB(glTexGend,(GLenum coord, GLenum pname, GLdouble param),(coord, pname, param));
MKSTUB(glTexGendv,(GLenum coord, GLenum pname, const GLdouble *params),(coord, pname, params));
MKSTUB(glTexGenf,(GLenum coord, GLenum pname, GLfloat param),(coord, pname, param));
MKSTUB(glTexGenfv,(GLenum coord, GLenum pname, const GLfloat *params),(coord, pname, params));
MKSTUB(glTexGeni,(GLenum coord, GLenum pname, GLint param),(coord, pname, param));
MKSTUB(glTexGeniv,(GLenum coord, GLenum pname, const GLint *params),(coord, pname, params));
MKSTUB(glTexImage1D,(GLenum target, GLint level, GLint components, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels),(target, level, components, width, border, format, type, pixels));
MKSTUB(glTexImage2D,(GLenum target, GLint level, GLint components, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels),(target, level, components, width, height, border, format, type, pixels));
MKSTUB(glTexParameterf,(GLenum target, GLenum pname, GLfloat param),(target, pname, param));
MKSTUB(glTexParameterfv,(GLenum target, GLenum pname, const GLfloat *params),(target, pname, params));
MKSTUB(glTexParameteri,(GLenum target, GLenum pname, GLint param),(target, pname, param));
MKSTUB(glTexParameteriv,(GLenum target, GLenum pname, const GLint *params),(target, pname, params));
MKSTUB(glTexSubImage1D,(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels),(target, level, xoffset, width, format, type, pixels));
MKSTUB(glTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels),(target, level, xoffset, yoffset, width, height, format, type, pixels));
MKSTUB(glTranslated,(GLdouble x, GLdouble y, GLdouble z),(x, y, z));
MKSTUB(glTranslatef,(GLfloat x, GLfloat y, GLfloat z),(x, y, z));
MKSTUB(glVertex2d,(GLdouble x, GLdouble y),(x, y));
MKSTUB(glVertex2dv,(const GLdouble *v),(v));
MKSTUB(glVertex2f,(GLfloat x, GLfloat y),(x, y));
MKSTUB(glVertex2fv,(const GLfloat *v),(v));
MKSTUB(glVertex2i,(GLint x, GLint y),(x, y));
MKSTUB(glVertex2iv,(const GLint *v),(v));
MKSTUB(glVertex2s,(GLshort x, GLshort y),(x, y));
MKSTUB(glVertex2sv,(const GLshort *v),(v));
MKSTUB(glVertex3d,(GLdouble x, GLdouble y, GLdouble z),(x, y, z));
MKSTUB(glVertex3dv,(const GLdouble *v),(v));
MKSTUB(glVertex3f,(GLfloat x, GLfloat y, GLfloat z),(x, y, z));
MKSTUB(glVertex3fv,(const GLfloat *v),(v));
MKSTUB(glVertex3i,(GLint x, GLint y, GLint z),(x, y, z));
MKSTUB(glVertex3iv,(const GLint *v),(v));
MKSTUB(glVertex3s,(GLshort x, GLshort y, GLshort z),(x, y, z));
MKSTUB(glVertex3sv,(const GLshort *v),(v));
MKSTUB(glVertex4d,(GLdouble x, GLdouble y, GLdouble z, GLdouble w),(x, y, z, w));
MKSTUB(glVertex4dv,(const GLdouble *v),(v));
MKSTUB(glVertex4f,(GLfloat x, GLfloat y, GLfloat z, GLfloat w),(x, y, z, w));
MKSTUB(glVertex4fv,(const GLfloat *v),(v));
MKSTUB(glVertex4i,(GLint x, GLint y, GLint z, GLint w),(x, y, z, w));
MKSTUB(glVertex4iv,(const GLint *v),(v));
MKSTUB(glVertex4s,(GLshort x, GLshort y, GLshort z, GLshort w),(x, y, z, w));
MKSTUB(glVertex4sv,(const GLshort *v),(v));
MKSTUB(glVertexPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer),(size, type, stride, pointer));
MKSTUB(glViewport,(GLint x, GLint y, GLsizei width, GLsizei height),(x, y, width, height));
MKSTUB(glWindowPos4fMESA,(GLfloat x, GLfloat y, GLfloat z, GLfloat w ),(x, y, z, w ));
MKSTUB(glResizeBuffersMESA,(void),());

/* OpenGL 1.2 functions */

MKSTUB(glDrawRangeElements,(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices),(mode, start, end, count, type, indices));
MKSTUB(glTexImage3D,(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels),(target, level, internalFormat, width, height, depth, border, format, type, pixels));
MKSTUB(glTexSubImage3D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels),(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels));
MKSTUB(glCopyTexSubImage3D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height),(target, level, xoffset, yoffset, zoffset, x, y, width, height));

MKSTUB(gluBeginCurve,(GLUnurbs* nurb),(nurb));
MKSTUB(gluBeginPolygon,(GLUtesselator* tess),(tess));
MKSTUB(gluBeginSurface,(GLUnurbs* nurb),(nurb));
MKSTUB(gluBeginTrim,(GLUnurbs* nurb),(nurb));
MKSTUB_RET(GLint,gluBuild1DMipmaps,(GLenum target, GLint component, GLsizei width, GLenum format, GLenum type, const void *data),(target, component, width, format, type, data));
MKSTUB_RET(GLint,gluBuild2DMipmaps,(GLenum target, GLint component, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data),(target, component, width, height, format, type, data));
MKSTUB(gluCylinder,(GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks),(quad, base, top, height, slices, stacks));
MKSTUB(gluDeleteNurbsRenderer,(GLUnurbs* nurb),(nurb));
MKSTUB(gluDeleteQuadric,(GLUquadric* quad),(quad));
MKSTUB(gluDeleteTess,(GLUtesselator* tess),(tess));
MKSTUB(gluDisk,(GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops),(quad, inner, outer, slices, loops));
MKSTUB(gluEndCurve,(GLUnurbs* nurb),(nurb));
MKSTUB(gluEndPolygon,(GLUtesselator* tess),(tess));
MKSTUB(gluEndSurface,(GLUnurbs* nurb),(nurb));
MKSTUB(gluEndTrim,(GLUnurbs* nurb),(nurb));
MKSTUB_RET(const GLubyte *,gluErrorString,(GLenum error),(error));
MKSTUB(gluGetNurbsProperty,(GLUnurbs* nurb, GLenum property, GLfloat* data),(nurb, property, data));
MKSTUB_RET(const GLubyte *,gluGetString,(GLenum name),(name));
MKSTUB(gluGetTessProperty,(GLUtesselator* tess, GLenum which, GLdouble* data),(tess, which, data));
MKSTUB(gluLoadSamplingMatrices,(GLUnurbs* nurb, const GLfloat *model, const GLfloat *perspective, const GLint *view),(nurb, model, perspective, view));
MKSTUB(gluLookAt,(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ),(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ));
MKSTUB_RET(GLUnurbs*,gluNewNurbsRenderer,(void),());
MKSTUB_RET(GLUquadric*,gluNewQuadric,(void),());
MKSTUB_RET(GLUtesselator*,gluNewTess,(void),());
MKSTUB(gluNextContour,(GLUtesselator* tess, GLenum type),(tess, type));
MKSTUB(gluNurbsCallback,(GLUnurbs* nurb, GLenum which, GLvoid (CALLBACK *CallBackFunc)()),(nurb, which, CallBackFunc));
MKSTUB(gluNurbsCurve,(GLUnurbs* nurb, GLint knotCount, GLfloat *knots, GLint stride, GLfloat *control, GLint order, GLenum type),(nurb, knotCount, knots, stride, control, order, type));
MKSTUB(gluNurbsProperty,(GLUnurbs* nurb, GLenum property, GLfloat value),(nurb, property, value));
MKSTUB(gluNurbsSurface,(GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type),(nurb, sKnotCount, sKnots, tKnotCount, tKnots, sStride, tStride, control, sOrder, tOrder, type));
MKSTUB(gluOrtho2D,(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top),(left, right, bottom, top));
MKSTUB(gluPartialDisk,(GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep),(quad, inner, outer, slices, loops, start, sweep));
MKSTUB(gluPerspective,(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar),(fovy, aspect, zNear, zFar));
MKSTUB(gluPickMatrix,(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint *viewport),(x, y, delX, delY, viewport));
MKSTUB_RET(GLint,gluProject,(GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* winX, GLdouble* winY, GLdouble* winZ),(objX, objY, objZ, model, proj, view, winX, winY, winZ));
MKSTUB(gluPwlCurve,(GLUnurbs* nurb, GLint count, GLfloat* data, GLint stride, GLenum type),(nurb, count, data, stride, type));
MKSTUB(gluQuadricCallback,(GLUquadric* quad, GLenum which, GLvoid (CALLBACK *CallBackFunc)()),(quad, which, CallBackFunc));
MKSTUB(gluQuadricDrawStyle,(GLUquadric* quad, GLenum draw),(quad, draw));
MKSTUB(gluQuadricNormals,(GLUquadric* quad, GLenum normal),(quad, normal));
MKSTUB(gluQuadricOrientation,(GLUquadric* quad, GLenum orientation),(quad, orientation));
MKSTUB(gluQuadricTexture,(GLUquadric* quad, GLboolean texture),(quad, texture));
MKSTUB_RET(GLint,gluScaleImage,(GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut),(format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut, dataOut));
MKSTUB(gluSphere,(GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks),(quad, radius, slices, stacks));
MKSTUB(gluTessBeginContour,(GLUtesselator* tess),(tess));
MKSTUB(gluTessBeginPolygon,(GLUtesselator* tess, GLvoid* data),(tess, data));
MKSTUB(gluTessCallback,(GLUtesselator* tess, GLenum which, GLvoid (CALLBACK *CallBackFunc)()),(tess, which, CallBackFunc));
MKSTUB(gluTessEndContour,(GLUtesselator* tess),(tess));
MKSTUB(gluTessEndPolygon,(GLUtesselator* tess),(tess));
MKSTUB(gluTessNormal,(GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ),(tess, valueX, valueY, valueZ));
MKSTUB(gluTessProperty,(GLUtesselator* tess, GLenum which, GLdouble data),(tess, which, data));
MKSTUB(gluTessVertex,(GLUtesselator* tess, GLdouble *location, GLvoid* data),(tess, location, data));
MKSTUB_RET(GLint,gluUnProject,(GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* objX, GLdouble* objY, GLdouble* objZ),(winX, winY, winZ, model, proj, view, objX, objY, objZ));

#endif

