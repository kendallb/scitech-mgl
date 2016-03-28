/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any
*
* Description:  Header describing the function stubs for calling OpenGL
*               core API functions.
*
****************************************************************************/

#ifndef __GL_STUB_H
#define __GL_STUB_H

/*--------------------- Macros and type definitions -----------------------*/

#pragma pack(1)

/****************************************************************************
REMARKS:
Main structure containing all public OpenGL compatible functions supported.
{secret}
****************************************************************************/
typedef struct GA_glFuncs {
    unsigned long dwSize;

    /* OpenGL 1.0/1.1 compatible functions */
    void        (APIENTRYP glAccum) (GLenum op, GLfloat value);
    void        (APIENTRYP glAlphaFunc) (GLenum func, GLclampf ref);
    GLboolean   (APIENTRYP glAreTexturesResident) (GLsizei n, const GLuint *textures, GLboolean *residences);
    void        (APIENTRYP glArrayElement) (GLint i);
    void        (APIENTRYP glBegin) (GLenum mode);
    void        (APIENTRYP glBindTexture) (GLenum target, GLuint texture);
    void        (APIENTRYP glBitmap) (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
    void        (APIENTRYP glBlendFunc) (GLenum sfactor, GLenum dfactor);
    void        (APIENTRYP glCallList) (GLuint list);
    void        (APIENTRYP glCallLists) (GLsizei n, GLenum type, const GLvoid *lists);
    void        (APIENTRYP glClear) (GLbitfield mask);
    void        (APIENTRYP glClearAccum) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void        (APIENTRYP glClearColor) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void        (APIENTRYP glClearDepth) (GLclampd depth);
    void        (APIENTRYP glClearIndex) (GLfloat c);
    void        (APIENTRYP glClearStencil) (GLint s);
    void        (APIENTRYP glClipPlane) (GLenum plane, const GLdouble *equation);
    void        (APIENTRYP glColor3b) (GLbyte red, GLbyte green, GLbyte blue);
    void        (APIENTRYP glColor3bv) (const GLbyte *v);
    void        (APIENTRYP glColor3d) (GLdouble red, GLdouble green, GLdouble blue);
    void        (APIENTRYP glColor3dv) (const GLdouble *v);
    void        (APIENTRYP glColor3f) (GLfloat red, GLfloat green, GLfloat blue);
    void        (APIENTRYP glColor3fv) (const GLfloat *v);
    void        (APIENTRYP glColor3i) (GLint red, GLint green, GLint blue);
    void        (APIENTRYP glColor3iv) (const GLint *v);
    void        (APIENTRYP glColor3s) (GLshort red, GLshort green, GLshort blue);
    void        (APIENTRYP glColor3sv) (const GLshort *v);
    void        (APIENTRYP glColor3ub) (GLubyte red, GLubyte green, GLubyte blue);
    void        (APIENTRYP glColor3ubv) (const GLubyte *v);
    void        (APIENTRYP glColor3ui) (GLuint red, GLuint green, GLuint blue);
    void        (APIENTRYP glColor3uiv) (const GLuint *v);
    void        (APIENTRYP glColor3us) (GLushort red, GLushort green, GLushort blue);
    void        (APIENTRYP glColor3usv) (const GLushort *v);
    void        (APIENTRYP glColor4b) (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
    void        (APIENTRYP glColor4bv) (const GLbyte *v);
    void        (APIENTRYP glColor4d) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
    void        (APIENTRYP glColor4dv) (const GLdouble *v);
    void        (APIENTRYP glColor4f) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void        (APIENTRYP glColor4fv) (const GLfloat *v);
    void        (APIENTRYP glColor4i) (GLint red, GLint green, GLint blue, GLint alpha);
    void        (APIENTRYP glColor4iv) (const GLint *v);
    void        (APIENTRYP glColor4s) (GLshort red, GLshort green, GLshort blue, GLshort alpha);
    void        (APIENTRYP glColor4sv) (const GLshort *v);
    void        (APIENTRYP glColor4ub) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
    void        (APIENTRYP glColor4ubv) (const GLubyte *v);
    void        (APIENTRYP glColor4ui) (GLuint red, GLuint green, GLuint blue, GLuint alpha);
    void        (APIENTRYP glColor4uiv) (const GLuint *v);
    void        (APIENTRYP glColor4us) (GLushort red, GLushort green, GLushort blue, GLushort alpha);
    void        (APIENTRYP glColor4usv) (const GLushort *v);
    void        (APIENTRYP glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    void        (APIENTRYP glColorMaterial) (GLenum face, GLenum mode);
    void        (APIENTRYP glColorPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glCopyPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
    void        (APIENTRYP glCopyTexImage1D) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
    void        (APIENTRYP glCopyTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    void        (APIENTRYP glCopyTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    void        (APIENTRYP glCopyTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    void        (APIENTRYP glCullFace) (GLenum mode);
    void        (APIENTRYP glDeleteLists) (GLuint list, GLsizei range);
    void        (APIENTRYP glDeleteTextures) (GLsizei n, const GLuint *textures);
    void        (APIENTRYP glDepthFunc) (GLenum func);
    void        (APIENTRYP glDepthMask) (GLboolean flag);
    void        (APIENTRYP glDepthRange) (GLclampd zNear, GLclampd zFar);
    void        (APIENTRYP glDisable) (GLenum cap);
    void        (APIENTRYP glDisableClientState) (GLenum array);
    void        (APIENTRYP glDrawArrays) (GLenum mode, GLint first, GLsizei count);
    void        (APIENTRYP glDrawBuffer) (GLenum mode);
    void        (APIENTRYP glDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
    void        (APIENTRYP glDrawPixels) (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void        (APIENTRYP glEdgeFlag) (GLboolean flag);
    void        (APIENTRYP glEdgeFlagPointer) (GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glEdgeFlagv) (const GLboolean *flag);
    void        (APIENTRYP glEnable) (GLenum cap);
    void        (APIENTRYP glEnableClientState) (GLenum array);
    void        (APIENTRYP glEnd) (void);
    void        (APIENTRYP glEndList) (void);
    void        (APIENTRYP glEvalCoord1d) (GLdouble u);
    void        (APIENTRYP glEvalCoord1dv) (const GLdouble *u);
    void        (APIENTRYP glEvalCoord1f) (GLfloat u);
    void        (APIENTRYP glEvalCoord1fv) (const GLfloat *u);
    void        (APIENTRYP glEvalCoord2d) (GLdouble u, GLdouble v);
    void        (APIENTRYP glEvalCoord2dv) (const GLdouble *u);
    void        (APIENTRYP glEvalCoord2f) (GLfloat u, GLfloat v);
    void        (APIENTRYP glEvalCoord2fv) (const GLfloat *u);
    void        (APIENTRYP glEvalMesh1) (GLenum mode, GLint i1, GLint i2);
    void        (APIENTRYP glEvalMesh2) (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
    void        (APIENTRYP glEvalPoint1) (GLint i);
    void        (APIENTRYP glEvalPoint2) (GLint i, GLint j);
    void        (APIENTRYP glFeedbackBuffer) (GLsizei size, GLenum type, GLfloat *buffer);
    void        (APIENTRYP glFinish) (void);
    void        (APIENTRYP glFlush) (void);
    void        (APIENTRYP glFogf) (GLenum pname, GLfloat param);
    void        (APIENTRYP glFogfv) (GLenum pname, const GLfloat *params);
    void        (APIENTRYP glFogi) (GLenum pname, GLint param);
    void        (APIENTRYP glFogiv) (GLenum pname, const GLint *params);
    void        (APIENTRYP glFrontFace) (GLenum mode);
    void        (APIENTRYP glFrustum) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
    GLuint      (APIENTRYP glGenLists) (GLsizei range);
    void        (APIENTRYP glGenTextures) (GLsizei n, GLuint *textures);
    void        (APIENTRYP glGetBooleanv) (GLenum pname, GLboolean *params);
    void        (APIENTRYP glGetClipPlane) (GLenum plane, GLdouble *equation);
    void        (APIENTRYP glGetDoublev) (GLenum pname, GLdouble *params);
    GLenum      (APIENTRYP glGetError) (void);
    void        (APIENTRYP glGetFloatv) (GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetIntegerv) (GLenum pname, GLint *params);
    void        (APIENTRYP glGetLightfv) (GLenum light, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetLightiv) (GLenum light, GLenum pname, GLint *params);
    void        (APIENTRYP glGetMapdv) (GLenum target, GLenum query, GLdouble *v);
    void        (APIENTRYP glGetMapfv) (GLenum target, GLenum query, GLfloat *v);
    void        (APIENTRYP glGetMapiv) (GLenum target, GLenum query, GLint *v);
    void        (APIENTRYP glGetMaterialfv) (GLenum face, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetMaterialiv) (GLenum face, GLenum pname, GLint *params);
    void        (APIENTRYP glGetPixelMapfv) (GLenum map, GLfloat *values);
    void        (APIENTRYP glGetPixelMapuiv) (GLenum map, GLuint *values);
    void        (APIENTRYP glGetPixelMapusv) (GLenum map, GLushort *values);
    void        (APIENTRYP glGetPointerv) (GLenum pname, GLvoid* *params);
    void        (APIENTRYP glGetPolygonStipple) (GLubyte *mask);
    const GLubyte * (APIENTRYP glGetString) (GLenum name);
    void        (APIENTRYP glGetTexEnvfv) (GLenum target, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetTexEnviv) (GLenum target, GLenum pname, GLint *params);
    void        (APIENTRYP glGetTexGendv) (GLenum coord, GLenum pname, GLdouble *params);
    void        (APIENTRYP glGetTexGenfv) (GLenum coord, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetTexGeniv) (GLenum coord, GLenum pname, GLint *params);
    void        (APIENTRYP glGetTexImage) (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
    void        (APIENTRYP glGetTexLevelParameterfv) (GLenum target, GLint level, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetTexLevelParameteriv) (GLenum target, GLint level, GLenum pname, GLint *params);
    void        (APIENTRYP glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params);
    void        (APIENTRYP glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params);
    void        (APIENTRYP glHint) (GLenum target, GLenum mode);
    void        (APIENTRYP glIndexMask) (GLuint mask);
    void        (APIENTRYP glIndexPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glIndexd) (GLdouble c);
    void        (APIENTRYP glIndexdv) (const GLdouble *c);
    void        (APIENTRYP glIndexf) (GLfloat c);
    void        (APIENTRYP glIndexfv) (const GLfloat *c);
    void        (APIENTRYP glIndexi) (GLint c);
    void        (APIENTRYP glIndexiv) (const GLint *c);
    void        (APIENTRYP glIndexs) (GLshort c);
    void        (APIENTRYP glIndexsv) (const GLshort *c);
    void        (APIENTRYP glIndexub) (GLubyte c);
    void        (APIENTRYP glIndexubv) (const GLubyte *c);
    void        (APIENTRYP glInitNames) (void);
    void        (APIENTRYP glInterleavedArrays) (GLenum format, GLsizei stride, const GLvoid *pointer);
    GLboolean   (APIENTRYP glIsEnabled) (GLenum cap);
    GLboolean   (APIENTRYP glIsList) (GLuint list);
    GLboolean   (APIENTRYP glIsTexture) (GLuint texture);
    void        (APIENTRYP glLightModelf) (GLenum pname, GLfloat param);
    void        (APIENTRYP glLightModelfv) (GLenum pname, const GLfloat *params);
    void        (APIENTRYP glLightModeli) (GLenum pname, GLint param);
    void        (APIENTRYP glLightModeliv) (GLenum pname, const GLint *params);
    void        (APIENTRYP glLightf) (GLenum light, GLenum pname, GLfloat param);
    void        (APIENTRYP glLightfv) (GLenum light, GLenum pname, const GLfloat *params);
    void        (APIENTRYP glLighti) (GLenum light, GLenum pname, GLint param);
    void        (APIENTRYP glLightiv) (GLenum light, GLenum pname, const GLint *params);
    void        (APIENTRYP glLineStipple) (GLint factor, GLushort pattern);
    void        (APIENTRYP glLineWidth) (GLfloat width);
    void        (APIENTRYP glListBase) (GLuint base);
    void        (APIENTRYP glLoadIdentity) (void);
    void        (APIENTRYP glLoadMatrixd) (const GLdouble *m);
    void        (APIENTRYP glLoadMatrixf) (const GLfloat *m);
    void        (APIENTRYP glLoadName) (GLuint name);
    void        (APIENTRYP glLogicOp) (GLenum opcode);
    void        (APIENTRYP glMap1d) (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
    void        (APIENTRYP glMap1f) (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
    void        (APIENTRYP glMap2d) (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
    void        (APIENTRYP glMap2f) (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
    void        (APIENTRYP glMapGrid1d) (GLint un, GLdouble u1, GLdouble u2);
    void        (APIENTRYP glMapGrid1f) (GLint un, GLfloat u1, GLfloat u2);
    void        (APIENTRYP glMapGrid2d) (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
    void        (APIENTRYP glMapGrid2f) (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
    void        (APIENTRYP glMaterialf) (GLenum face, GLenum pname, GLfloat param);
    void        (APIENTRYP glMaterialfv) (GLenum face, GLenum pname, const GLfloat *params);
    void        (APIENTRYP glMateriali) (GLenum face, GLenum pname, GLint param);
    void        (APIENTRYP glMaterialiv) (GLenum face, GLenum pname, const GLint *params);
    void        (APIENTRYP glMatrixMode) (GLenum mode);
    void        (APIENTRYP glMultMatrixd) (const GLdouble *m);
    void        (APIENTRYP glMultMatrixf) (const GLfloat *m);
    void        (APIENTRYP glNewList) (GLuint list, GLenum mode);
    void        (APIENTRYP glNormal3b) (GLbyte nx, GLbyte ny, GLbyte nz);
    void        (APIENTRYP glNormal3bv) (const GLbyte *v);
    void        (APIENTRYP glNormal3d) (GLdouble nx, GLdouble ny, GLdouble nz);
    void        (APIENTRYP glNormal3dv) (const GLdouble *v);
    void        (APIENTRYP glNormal3f) (GLfloat nx, GLfloat ny, GLfloat nz);
    void        (APIENTRYP glNormal3fv) (const GLfloat *v);
    void        (APIENTRYP glNormal3i) (GLint nx, GLint ny, GLint nz);
    void        (APIENTRYP glNormal3iv) (const GLint *v);
    void        (APIENTRYP glNormal3s) (GLshort nx, GLshort ny, GLshort nz);
    void        (APIENTRYP glNormal3sv) (const GLshort *v);
    void        (APIENTRYP glNormalPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glOrtho) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
    void        (APIENTRYP glPassThrough) (GLfloat token);
    void        (APIENTRYP glPixelMapfv) (GLenum map, GLint mapsize, const GLfloat *values);
    void        (APIENTRYP glPixelMapuiv) (GLenum map, GLint mapsize, const GLuint *values);
    void        (APIENTRYP glPixelMapusv) (GLenum map, GLint mapsize, const GLushort *values);
    void        (APIENTRYP glPixelStoref) (GLenum pname, GLfloat param);
    void        (APIENTRYP glPixelStorei) (GLenum pname, GLint param);
    void        (APIENTRYP glPixelTransferf) (GLenum pname, GLfloat param);
    void        (APIENTRYP glPixelTransferi) (GLenum pname, GLint param);
    void        (APIENTRYP glPixelZoom) (GLfloat xfactor, GLfloat yfactor);
    void        (APIENTRYP glPointSize) (GLfloat size);
    void        (APIENTRYP glPolygonMode) (GLenum face, GLenum mode);
    void        (APIENTRYP glPolygonOffset) (GLfloat factor, GLfloat units);
    void        (APIENTRYP glPolygonStipple) (const GLubyte *mask);
    void        (APIENTRYP glPopAttrib) (void);
    void        (APIENTRYP glPopClientAttrib) (void);
    void        (APIENTRYP glPopMatrix) (void);
    void        (APIENTRYP glPopName) (void);
    void        (APIENTRYP glPrioritizeTextures) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
    void        (APIENTRYP glPushAttrib) (GLbitfield mask);
    void        (APIENTRYP glPushClientAttrib) (GLbitfield mask);
    void        (APIENTRYP glPushMatrix) (void);
    void        (APIENTRYP glPushName) (GLuint name);
    void        (APIENTRYP glRasterPos2d) (GLdouble x, GLdouble y);
    void        (APIENTRYP glRasterPos2dv) (const GLdouble *v);
    void        (APIENTRYP glRasterPos2f) (GLfloat x, GLfloat y);
    void        (APIENTRYP glRasterPos2fv) (const GLfloat *v);
    void        (APIENTRYP glRasterPos2i) (GLint x, GLint y);
    void        (APIENTRYP glRasterPos2iv) (const GLint *v);
    void        (APIENTRYP glRasterPos2s) (GLshort x, GLshort y);
    void        (APIENTRYP glRasterPos2sv) (const GLshort *v);
    void        (APIENTRYP glRasterPos3d) (GLdouble x, GLdouble y, GLdouble z);
    void        (APIENTRYP glRasterPos3dv) (const GLdouble *v);
    void        (APIENTRYP glRasterPos3f) (GLfloat x, GLfloat y, GLfloat z);
    void        (APIENTRYP glRasterPos3fv) (const GLfloat *v);
    void        (APIENTRYP glRasterPos3i) (GLint x, GLint y, GLint z);
    void        (APIENTRYP glRasterPos3iv) (const GLint *v);
    void        (APIENTRYP glRasterPos3s) (GLshort x, GLshort y, GLshort z);
    void        (APIENTRYP glRasterPos3sv) (const GLshort *v);
    void        (APIENTRYP glRasterPos4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    void        (APIENTRYP glRasterPos4dv) (const GLdouble *v);
    void        (APIENTRYP glRasterPos4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void        (APIENTRYP glRasterPos4fv) (const GLfloat *v);
    void        (APIENTRYP glRasterPos4i) (GLint x, GLint y, GLint z, GLint w);
    void        (APIENTRYP glRasterPos4iv) (const GLint *v);
    void        (APIENTRYP glRasterPos4s) (GLshort x, GLshort y, GLshort z, GLshort w);
    void        (APIENTRYP glRasterPos4sv) (const GLshort *v);
    void        (APIENTRYP glReadBuffer) (GLenum mode);
    void        (APIENTRYP glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
    void        (APIENTRYP glRectd) (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
    void        (APIENTRYP glRectdv) (const GLdouble *v1, const GLdouble *v2);
    void        (APIENTRYP glRectf) (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void        (APIENTRYP glRectfv) (const GLfloat *v1, const GLfloat *v2);
    void        (APIENTRYP glRecti) (GLint x1, GLint y1, GLint x2, GLint y2);
    void        (APIENTRYP glRectiv) (const GLint *v1, const GLint *v2);
    void        (APIENTRYP glRects) (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
    void        (APIENTRYP glRectsv) (const GLshort *v1, const GLshort *v2);
    GLint       (APIENTRYP glRenderMode) (GLenum mode);
    void        (APIENTRYP glRotated) (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
    void        (APIENTRYP glRotatef) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void        (APIENTRYP glScaled) (GLdouble x, GLdouble y, GLdouble z);
    void        (APIENTRYP glScalef) (GLfloat x, GLfloat y, GLfloat z);
    void        (APIENTRYP glScissor) (GLint x, GLint y, GLsizei width, GLsizei height);
    void        (APIENTRYP glSelectBuffer) (GLsizei size, GLuint *buffer);
    void        (APIENTRYP glShadeModel) (GLenum mode);
    void        (APIENTRYP glStencilFunc) (GLenum func, GLint ref, GLuint mask);
    void        (APIENTRYP glStencilMask) (GLuint mask);
    void        (APIENTRYP glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass);
    void        (APIENTRYP glTexCoord1d) (GLdouble s);
    void        (APIENTRYP glTexCoord1dv) (const GLdouble *v);
    void        (APIENTRYP glTexCoord1f) (GLfloat s);
    void        (APIENTRYP glTexCoord1fv) (const GLfloat *v);
    void        (APIENTRYP glTexCoord1i) (GLint s);
    void        (APIENTRYP glTexCoord1iv) (const GLint *v);
    void        (APIENTRYP glTexCoord1s) (GLshort s);
    void        (APIENTRYP glTexCoord1sv) (const GLshort *v);
    void        (APIENTRYP glTexCoord2d) (GLdouble s, GLdouble t);
    void        (APIENTRYP glTexCoord2dv) (const GLdouble *v);
    void        (APIENTRYP glTexCoord2f) (GLfloat s, GLfloat t);
    void        (APIENTRYP glTexCoord2fv) (const GLfloat *v);
    void        (APIENTRYP glTexCoord2i) (GLint s, GLint t);
    void        (APIENTRYP glTexCoord2iv) (const GLint *v);
    void        (APIENTRYP glTexCoord2s) (GLshort s, GLshort t);
    void        (APIENTRYP glTexCoord2sv) (const GLshort *v);
    void        (APIENTRYP glTexCoord3d) (GLdouble s, GLdouble t, GLdouble r);
    void        (APIENTRYP glTexCoord3dv) (const GLdouble *v);
    void        (APIENTRYP glTexCoord3f) (GLfloat s, GLfloat t, GLfloat r);
    void        (APIENTRYP glTexCoord3fv) (const GLfloat *v);
    void        (APIENTRYP glTexCoord3i) (GLint s, GLint t, GLint r);
    void        (APIENTRYP glTexCoord3iv) (const GLint *v);
    void        (APIENTRYP glTexCoord3s) (GLshort s, GLshort t, GLshort r);
    void        (APIENTRYP glTexCoord3sv) (const GLshort *v);
    void        (APIENTRYP glTexCoord4d) (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
    void        (APIENTRYP glTexCoord4dv) (const GLdouble *v);
    void        (APIENTRYP glTexCoord4f) (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
    void        (APIENTRYP glTexCoord4fv) (const GLfloat *v);
    void        (APIENTRYP glTexCoord4i) (GLint s, GLint t, GLint r, GLint q);
    void        (APIENTRYP glTexCoord4iv) (const GLint *v);
    void        (APIENTRYP glTexCoord4s) (GLshort s, GLshort t, GLshort r, GLshort q);
    void        (APIENTRYP glTexCoord4sv) (const GLshort *v);
    void        (APIENTRYP glTexCoordPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glTexEnvf) (GLenum target, GLenum pname, GLfloat param);
    void        (APIENTRYP glTexEnvfv) (GLenum target, GLenum pname, const GLfloat *params);
    void        (APIENTRYP glTexEnvi) (GLenum target, GLenum pname, GLint param);
    void        (APIENTRYP glTexEnviv) (GLenum target, GLenum pname, const GLint *params);
    void        (APIENTRYP glTexGend) (GLenum coord, GLenum pname, GLdouble param);
    void        (APIENTRYP glTexGendv) (GLenum coord, GLenum pname, const GLdouble *params);
    void        (APIENTRYP glTexGenf) (GLenum coord, GLenum pname, GLfloat param);
    void        (APIENTRYP glTexGenfv) (GLenum coord, GLenum pname, const GLfloat *params);
    void        (APIENTRYP glTexGeni) (GLenum coord, GLenum pname, GLint param);
    void        (APIENTRYP glTexGeniv) (GLenum coord, GLenum pname, const GLint *params);
    void        (APIENTRYP glTexImage1D) (GLenum target, GLint level, GLint components, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void        (APIENTRYP glTexImage2D) (GLenum target, GLint level, GLint components, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void        (APIENTRYP glTexParameterf) (GLenum target, GLenum pname, GLfloat param);
    void        (APIENTRYP glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params);
    void        (APIENTRYP glTexParameteri) (GLenum target, GLenum pname, GLint param);
    void        (APIENTRYP glTexParameteriv) (GLenum target, GLenum pname, const GLint *params);
    void        (APIENTRYP glTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
    void        (APIENTRYP glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void        (APIENTRYP glTranslated) (GLdouble x, GLdouble y, GLdouble z);
    void        (APIENTRYP glTranslatef) (GLfloat x, GLfloat y, GLfloat z);
    void        (APIENTRYP glVertex2d) (GLdouble x, GLdouble y);
    void        (APIENTRYP glVertex2dv) (const GLdouble *v);
    void        (APIENTRYP glVertex2f) (GLfloat x, GLfloat y);
    void        (APIENTRYP glVertex2fv) (const GLfloat *v);
    void        (APIENTRYP glVertex2i) (GLint x, GLint y);
    void        (APIENTRYP glVertex2iv) (const GLint *v);
    void        (APIENTRYP glVertex2s) (GLshort x, GLshort y);
    void        (APIENTRYP glVertex2sv) (const GLshort *v);
    void        (APIENTRYP glVertex3d) (GLdouble x, GLdouble y, GLdouble z);
    void        (APIENTRYP glVertex3dv) (const GLdouble *v);
    void        (APIENTRYP glVertex3f) (GLfloat x, GLfloat y, GLfloat z);
    void        (APIENTRYP glVertex3fv) (const GLfloat *v);
    void        (APIENTRYP glVertex3i) (GLint x, GLint y, GLint z);
    void        (APIENTRYP glVertex3iv) (const GLint *v);
    void        (APIENTRYP glVertex3s) (GLshort x, GLshort y, GLshort z);
    void        (APIENTRYP glVertex3sv) (const GLshort *v);
    void        (APIENTRYP glVertex4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    void        (APIENTRYP glVertex4dv) (const GLdouble *v);
    void        (APIENTRYP glVertex4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void        (APIENTRYP glVertex4fv) (const GLfloat *v);
    void        (APIENTRYP glVertex4i) (GLint x, GLint y, GLint z, GLint w);
    void        (APIENTRYP glVertex4iv) (const GLint *v);
    void        (APIENTRYP glVertex4s) (GLshort x, GLshort y, GLshort z, GLshort w);
    void        (APIENTRYP glVertex4sv) (const GLshort *v);
    void        (APIENTRYP glVertexPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void        (APIENTRYP glViewport) (GLint x, GLint y, GLsizei width, GLsizei height);

    /* OpenGL 1.2/1.2.1 compatible functions */
    void        (APIENTRYP glBlendColor) (GLclampf, GLclampf, GLclampf, GLclampf);
    void        (APIENTRYP glBlendEquation) (GLenum);
    void        (APIENTRYP glDrawRangeElements) (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
    void        (APIENTRYP glColorTable) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glColorTableParameterfv) (GLenum, GLenum, const GLfloat *);
    void        (APIENTRYP glColorTableParameteriv) (GLenum, GLenum, const GLint *);
    void        (APIENTRYP glCopyColorTable) (GLenum, GLenum, GLint, GLint, GLsizei);
    void        (APIENTRYP glGetColorTable) (GLenum, GLenum, GLenum, GLvoid *);
    void        (APIENTRYP glGetColorTableParameterfv) (GLenum, GLenum, GLfloat *);
    void        (APIENTRYP glGetColorTableParameteriv) (GLenum, GLenum, GLint *);
    void        (APIENTRYP glColorSubTable) (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glCopyColorSubTable) (GLenum, GLsizei, GLint, GLint, GLsizei);
    void        (APIENTRYP glConvolutionFilter1D) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glConvolutionFilter2D) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glConvolutionParameterf) (GLenum, GLenum, GLfloat);
    void        (APIENTRYP glConvolutionParameterfv) (GLenum, GLenum, const GLfloat *);
    void        (APIENTRYP glConvolutionParameteri) (GLenum, GLenum, GLint);
    void        (APIENTRYP glConvolutionParameteriv) (GLenum, GLenum, const GLint *);
    void        (APIENTRYP glCopyConvolutionFilter1D) (GLenum, GLenum, GLint, GLint, GLsizei);
    void        (APIENTRYP glCopyConvolutionFilter2D) (GLenum, GLenum, GLint, GLint, GLsizei, GLsizei);
    void        (APIENTRYP glGetConvolutionFilter) (GLenum, GLenum, GLenum, GLvoid *);
    void        (APIENTRYP glGetConvolutionParameterfv) (GLenum, GLenum, GLfloat *);
    void        (APIENTRYP glGetConvolutionParameteriv) (GLenum, GLenum, GLint *);
    void        (APIENTRYP glGetSeparableFilter) (GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid *);
    void        (APIENTRYP glSeparableFilter2D) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid *);
    void        (APIENTRYP glGetHistogram) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
    void        (APIENTRYP glGetHistogramParameterfv) (GLenum, GLenum, GLfloat *);
    void        (APIENTRYP glGetHistogramParameteriv) (GLenum, GLenum, GLint *);
    void        (APIENTRYP glGetMinmax) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
    void        (APIENTRYP glGetMinmaxParameterfv) (GLenum, GLenum, GLfloat *);
    void        (APIENTRYP glGetMinmaxParameteriv) (GLenum, GLenum, GLint *);
    void        (APIENTRYP glHistogram) (GLenum, GLsizei, GLenum, GLboolean);
    void        (APIENTRYP glMinmax) (GLenum, GLenum, GLboolean);
    void        (APIENTRYP glResetHistogram) (GLenum);
    void        (APIENTRYP glResetMinmax) (GLenum);
    void        (APIENTRYP glTexImage3D) (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glTexSubImage3D) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
    void        (APIENTRYP glCopyTexSubImage3D) (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);

    /* OpenGL 1.3 compatible functions */
    void        (APIENTRYP glActiveTexture) (GLenum);
    void        (APIENTRYP glClientActiveTexture) (GLenum);
    void        (APIENTRYP glMultiTexCoord1d) (GLenum, GLdouble);
    void        (APIENTRYP glMultiTexCoord1dv) (GLenum, const GLdouble *);
    void        (APIENTRYP glMultiTexCoord1f) (GLenum, GLfloat);
    void        (APIENTRYP glMultiTexCoord1fv) (GLenum, const GLfloat *);
    void        (APIENTRYP glMultiTexCoord1i) (GLenum, GLint);
    void        (APIENTRYP glMultiTexCoord1iv) (GLenum, const GLint *);
    void        (APIENTRYP glMultiTexCoord1s) (GLenum, GLshort);
    void        (APIENTRYP glMultiTexCoord1sv) (GLenum, const GLshort *);
    void        (APIENTRYP glMultiTexCoord2d) (GLenum, GLdouble, GLdouble);
    void        (APIENTRYP glMultiTexCoord2dv) (GLenum, const GLdouble *);
    void        (APIENTRYP glMultiTexCoord2f) (GLenum, GLfloat, GLfloat);
    void        (APIENTRYP glMultiTexCoord2fv) (GLenum, const GLfloat *);
    void        (APIENTRYP glMultiTexCoord2i) (GLenum, GLint, GLint);
    void        (APIENTRYP glMultiTexCoord2iv) (GLenum, const GLint *);
    void        (APIENTRYP glMultiTexCoord2s) (GLenum, GLshort, GLshort);
    void        (APIENTRYP glMultiTexCoord2sv) (GLenum, const GLshort *);
    void        (APIENTRYP glMultiTexCoord3d) (GLenum, GLdouble, GLdouble, GLdouble);
    void        (APIENTRYP glMultiTexCoord3dv) (GLenum, const GLdouble *);
    void        (APIENTRYP glMultiTexCoord3f) (GLenum, GLfloat, GLfloat, GLfloat);
    void        (APIENTRYP glMultiTexCoord3fv) (GLenum, const GLfloat *);
    void        (APIENTRYP glMultiTexCoord3i) (GLenum, GLint, GLint, GLint);
    void        (APIENTRYP glMultiTexCoord3iv) (GLenum, const GLint *);
    void        (APIENTRYP glMultiTexCoord3s) (GLenum, GLshort, GLshort, GLshort);
    void        (APIENTRYP glMultiTexCoord3sv) (GLenum, const GLshort *);
    void        (APIENTRYP glMultiTexCoord4d) (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
    void        (APIENTRYP glMultiTexCoord4dv) (GLenum, const GLdouble *);
    void        (APIENTRYP glMultiTexCoord4f) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
    void        (APIENTRYP glMultiTexCoord4fv) (GLenum, const GLfloat *);
    void        (APIENTRYP glMultiTexCoord4i) (GLenum, GLint, GLint, GLint, GLint);
    void        (APIENTRYP glMultiTexCoord4iv) (GLenum, const GLint *);
    void        (APIENTRYP glMultiTexCoord4s) (GLenum, GLshort, GLshort, GLshort, GLshort);
    void        (APIENTRYP glMultiTexCoord4sv) (GLenum, const GLshort *);
    void        (APIENTRYP glLoadTransposeMatrixf) (const GLfloat *);
    void        (APIENTRYP glLoadTransposeMatrixd) (const GLdouble *);
    void        (APIENTRYP glMultTransposeMatrixf) (const GLfloat *);
    void        (APIENTRYP glMultTransposeMatrixd) (const GLdouble *);
    void        (APIENTRYP glSampleCoverage) (GLclampf, GLboolean);
    void        (APIENTRYP glCompressedTexImage3D) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
    void        (APIENTRYP glCompressedTexImage2D) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
    void        (APIENTRYP glCompressedTexImage1D) (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
    void        (APIENTRYP glCompressedTexSubImage3D) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
    void        (APIENTRYP glCompressedTexSubImage2D) (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
    void        (APIENTRYP glCompressedTexSubImage1D) (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
    void        (APIENTRYP glGetCompressedTexImage) (GLenum, GLint, GLvoid *);

    /* OpenGL 1.4 compatible functions */
    void        (APIENTRYP glBlendFuncSeparate) (GLenum, GLenum, GLenum, GLenum);
    void        (APIENTRYP glFogCoordf) (GLfloat);
    void        (APIENTRYP glFogCoordfv) (const GLfloat *);
    void        (APIENTRYP glFogCoordd) (GLdouble);
    void        (APIENTRYP glFogCoorddv) (const GLdouble *);
    void        (APIENTRYP glFogCoordPointer) (GLenum, GLsizei, const GLvoid *);
    void        (APIENTRYP glMultiDrawArrays) (GLenum, GLint *, GLsizei *, GLsizei);
    void        (APIENTRYP glMultiDrawElements) (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);
    void        (APIENTRYP glPointParameterf) (GLenum, GLfloat);
    void        (APIENTRYP glPointParameterfv) (GLenum, const GLfloat *);
    void        (APIENTRYP glPointParameteri) (GLenum, GLint);
    void        (APIENTRYP glPointParameteriv) (GLenum, const GLint *);
    void        (APIENTRYP glSecondaryColor3b) (GLbyte, GLbyte, GLbyte);
    void        (APIENTRYP glSecondaryColor3bv) (const GLbyte *);
    void        (APIENTRYP glSecondaryColor3d) (GLdouble, GLdouble, GLdouble);
    void        (APIENTRYP glSecondaryColor3dv) (const GLdouble *);
    void        (APIENTRYP glSecondaryColor3f) (GLfloat, GLfloat, GLfloat);
    void        (APIENTRYP glSecondaryColor3fv) (const GLfloat *);
    void        (APIENTRYP glSecondaryColor3i) (GLint, GLint, GLint);
    void        (APIENTRYP glSecondaryColor3iv) (const GLint *);
    void        (APIENTRYP glSecondaryColor3s) (GLshort, GLshort, GLshort);
    void        (APIENTRYP glSecondaryColor3sv) (const GLshort *);
    void        (APIENTRYP glSecondaryColor3ub) (GLubyte, GLubyte, GLubyte);
    void        (APIENTRYP glSecondaryColor3ubv) (const GLubyte *);
    void        (APIENTRYP glSecondaryColor3ui) (GLuint, GLuint, GLuint);
    void        (APIENTRYP glSecondaryColor3uiv) (const GLuint *);
    void        (APIENTRYP glSecondaryColor3us) (GLushort, GLushort, GLushort);
    void        (APIENTRYP glSecondaryColor3usv) (const GLushort *);
    void        (APIENTRYP glSecondaryColorPointer) (GLint, GLenum, GLsizei, const GLvoid *);
    void        (APIENTRYP glWindowPos2d) (GLdouble, GLdouble);
    void        (APIENTRYP glWindowPos2dv) (const GLdouble *);
    void        (APIENTRYP glWindowPos2f) (GLfloat, GLfloat);
    void        (APIENTRYP glWindowPos2fv) (const GLfloat *);
    void        (APIENTRYP glWindowPos2i) (GLint, GLint);
    void        (APIENTRYP glWindowPos2iv) (const GLint *);
    void        (APIENTRYP glWindowPos2s) (GLshort, GLshort);
    void        (APIENTRYP glWindowPos2sv) (const GLshort *);
    void        (APIENTRYP glWindowPos3d) (GLdouble, GLdouble, GLdouble);
    void        (APIENTRYP glWindowPos3dv) (const GLdouble *);
    void        (APIENTRYP glWindowPos3f) (GLfloat, GLfloat, GLfloat);
    void        (APIENTRYP glWindowPos3fv) (const GLfloat *);
    void        (APIENTRYP glWindowPos3i) (GLint, GLint, GLint);
    void        (APIENTRYP glWindowPos3iv) (const GLint *);
    void        (APIENTRYP glWindowPos3s) (GLshort, GLshort, GLshort);
    void        (APIENTRYP glWindowPos3sv) (const GLshort *);
    } GA_glFuncs;

#pragma pack()

#if !defined(NO_GL_MACROS)

/* Macros to call the OpenGL functions via a function pointer */

#define glAccum(op,value)                                                               _GA_glFuncs.glAccum(op,value)
#define glAlphaFunc(func,ref)                                                           _GA_glFuncs.glAlphaFunc(func,ref)
#define glAreTexturesResident(n,textures,residences)                                    _GA_glFuncs.glAreTexturesResident(n,textures,residences)
#define glArrayElement(i)                                                               _GA_glFuncs.glArrayElement(i)
#define glBegin(mode)                                                                   _GA_glFuncs.glBegin(mode)
#define glBindTexture(target,texture)                                                   _GA_glFuncs.glBindTexture(target,texture)
#define glBitmap(width,height,xorig,yorig,xmove,ymove,bitmap)                           _GA_glFuncs.glBitmap(width,height,xorig,yorig,xmove,ymove,bitmap)
#define glBlendFunc(sfactor,dfactor)                                                    _GA_glFuncs.glBlendFunc(sfactor,dfactor)
#define glCallList(list)                                                                _GA_glFuncs.glCallList(list)
#define glCallLists(n,type,lists)                                                       _GA_glFuncs.glCallLists(n,type,lists)
#define glClear(mask)                                                                   _GA_glFuncs.glClear(mask)
#define glClearAccum(red,green,blue,alpha)                                              _GA_glFuncs.glClearAccum(red,green,blue,alpha)
#define glClearColor(red,green,blue,alpha)                                              _GA_glFuncs.glClearColor(red,green,blue,alpha)
#define glClearDepth(depth)                                                             _GA_glFuncs.glClearDepth(depth)
#define glClearIndex(c)                                                                 _GA_glFuncs.glClearIndex(c)
#define glClearStencil(s)                                                               _GA_glFuncs.glClearStencil(s)
#define glClipPlane(plane,equation)                                                     _GA_glFuncs.glClipPlane(plane,equation)
#define glColor3b(red,green,blue)                                                       _GA_glFuncs.glColor3b(red,green,blue)
#define glColor3bv(v)                                                                   _GA_glFuncs.glColor3bv(v)
#define glColor3d(red,green,blue)                                                       _GA_glFuncs.glColor3d(red,green,blue)
#define glColor3dv(v)                                                                   _GA_glFuncs.glColor3dv(v)
#define glColor3f(red,green,blue)                                                       _GA_glFuncs.glColor3f(red,green,blue)
#define glColor3fv(v)                                                                   _GA_glFuncs.glColor3fv(v)
#define glColor3i(red,green,blue)                                                       _GA_glFuncs.glColor3i(red,green,blue)
#define glColor3iv(v)                                                                   _GA_glFuncs.glColor3iv(v)
#define glColor3s(red,green,blue)                                                       _GA_glFuncs.glColor3s(red,green,blue)
#define glColor3sv(v)                                                                   _GA_glFuncs.glColor3sv(v)
#define glColor3ub(red,green,blue)                                                      _GA_glFuncs.glColor3ub(red,green,blue)
#define glColor3ubv(v)                                                                  _GA_glFuncs.glColor3ubv(v)
#define glColor3ui(red,green,blue)                                                      _GA_glFuncs.glColor3ui(red,green,blue)
#define glColor3uiv(v)                                                                  _GA_glFuncs.glColor3uiv(v)
#define glColor3us(red,green,blue)                                                      _GA_glFuncs.glColor3us(red,green,blue)
#define glColor3usv(v)                                                                  _GA_glFuncs.glColor3usv(v)
#define glColor4b(red,green,blue,alpha)                                                 _GA_glFuncs.glColor4b(red,green,blue,alpha)
#define glColor4bv(v)                                                                   _GA_glFuncs.glColor4bv(v)
#define glColor4d(red,green,blue,alpha)                                                 _GA_glFuncs.glColor4d(red,green,blue,alpha)
#define glColor4dv(v)                                                                   _GA_glFuncs.glColor4dv(v)
#define glColor4f(red,green,blue,alpha)                                                 _GA_glFuncs.glColor4f(red,green,blue,alpha)
#define glColor4fv(v)                                                                   _GA_glFuncs.glColor4fv(v)
#define glColor4i(red,green,blue,alpha)                                                 _GA_glFuncs.glColor4i(red,green,blue,alpha)
#define glColor4iv(v)                                                                   _GA_glFuncs.glColor4iv(v)
#define glColor4s(red,green,blue,alpha)                                                 _GA_glFuncs.glColor4s(red,green,blue,alpha)
#define glColor4sv(v)                                                                   _GA_glFuncs.glColor4sv(v)
#define glColor4ub(red,green,blue,alpha)                                                _GA_glFuncs.glColor4ub(red,green,blue,alpha)
#define glColor4ubv(v)                                                                  _GA_glFuncs.glColor4ubv(v)
#define glColor4ui(red,green,blue,alpha)                                                _GA_glFuncs.glColor4ui(red,green,blue,alpha)
#define glColor4uiv(v)                                                                  _GA_glFuncs.glColor4uiv(v)
#define glColor4us(red,green,blue,alpha)                                                _GA_glFuncs.glColor4us(red,green,blue,alpha)
#define glColor4usv(v)                                                                  _GA_glFuncs.glColor4usv(v)
#define glColorMask(red,green,blue,alpha)                                               _GA_glFuncs.glColorMask(red,green,blue,alpha)
#define glColorMaterial(face,mode)                                                      _GA_glFuncs.glColorMaterial(face,mode)
#define glColorPointer(size,type,stride,pointer)                                        _GA_glFuncs.glColorPointer(size,type,stride,pointer)
#define glCopyPixels(x,y,width,height,type)                                             _GA_glFuncs.glCopyPixels(x,y,width,height,type)
#define glCopyTexImage1D(target,level,internalformat,x,y,width,border)                  _GA_glFuncs.glCopyTexImage1D(target,level,internalformat,x,y,width,border)
#define glCopyTexImage2D(target,level,internalformat,x,y,width,height,border)           _GA_glFuncs.glCopyTexImage2D(target,level,internalformat,x,y,width,height,border)
#define glCopyTexSubImage1D(target,level,xoffset,x,y,width)                             _GA_glFuncs.glCopyTexSubImage1D(target,level,xoffset,x,y,width)
#define glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height)              _GA_glFuncs.glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height)
#define glCullFace(mode)                                                                _GA_glFuncs.glCullFace(mode)
#define glDeleteLists(list,range)                                                       _GA_glFuncs.glDeleteLists(list,range)
#define glDeleteTextures(n,textures)                                                    _GA_glFuncs.glDeleteTextures(n,textures)
#define glDepthFunc(func)                                                               _GA_glFuncs.glDepthFunc(func)
#define glDepthMask(flag)                                                               _GA_glFuncs.glDepthMask(flag)
#define glDepthRange(zNear,zFar)                                                        _GA_glFuncs.glDepthRange(zNear,zFar)
#define glDisable(cap)                                                                  _GA_glFuncs.glDisable(cap)
#define glDisableClientState(array)                                                     _GA_glFuncs.glDisableClientState(array)
#define glDrawArrays(mode,first,count)                                                  _GA_glFuncs.glDrawArrays(mode,first,count)
#define glDrawBuffer(mode)                                                              _GA_glFuncs.glDrawBuffer(mode)
#define glDrawElements(mode,count,type,indices)                                         _GA_glFuncs.glDrawElements(mode,count,type,indices)
#define glDrawPixels(width,height,format,type,pixels)                                   _GA_glFuncs.glDrawPixels(width,height,format,type,pixels)
#define glEdgeFlag(flag)                                                                _GA_glFuncs.glEdgeFlag(flag)
#define glEdgeFlagPointer(stride,pointer)                                               _GA_glFuncs.glEdgeFlagPointer(stride,pointer)
#define glEdgeFlagv(flag)                                                               _GA_glFuncs.glEdgeFlagv(flag)
#define glEnable(cap)                                                                   _GA_glFuncs.glEnable(cap)
#define glEnableClientState(array)                                                      _GA_glFuncs.glEnableClientState(array)
#define glEnd()                                                                         _GA_glFuncs.glEnd()
#define glEndList()                                                                     _GA_glFuncs.glEndList()
#define glEvalCoord1d(u)                                                                _GA_glFuncs.glEvalCoord1d(u)
#define glEvalCoord1dv(u)                                                               _GA_glFuncs.glEvalCoord1dv(u)
#define glEvalCoord1f(u)                                                                _GA_glFuncs.glEvalCoord1f(u)
#define glEvalCoord1fv(u)                                                               _GA_glFuncs.glEvalCoord1fv(u)
#define glEvalCoord2d(u,v)                                                              _GA_glFuncs.glEvalCoord2d(u,v)
#define glEvalCoord2dv(u)                                                               _GA_glFuncs.glEvalCoord2dv(u)
#define glEvalCoord2f(u,v)                                                              _GA_glFuncs.glEvalCoord2f(u,v)
#define glEvalCoord2fv(u)                                                               _GA_glFuncs.glEvalCoord2fv(u)
#define glEvalMesh1(mode,i1,i2)                                                         _GA_glFuncs.glEvalMesh1(mode,i1,i2)
#define glEvalMesh2(mode,i1,i2,j1,j2)                                                   _GA_glFuncs.glEvalMesh2(mode,i1,i2,j1,j2)
#define glEvalPoint1(i)                                                                 _GA_glFuncs.glEvalPoint1(i)
#define glEvalPoint2(i,j)                                                               _GA_glFuncs.glEvalPoint2(i,j)
#define glFeedbackBuffer(size,type,buffer)                                              _GA_glFuncs.glFeedbackBuffer(size,type,buffer)
#define glFinish()                                                                      _GA_glFuncs.glFinish()
#define glFlush()                                                                       _GA_glFuncs.glFlush()
#define glFogf(pname,param)                                                             _GA_glFuncs.glFogf(pname,param)
#define glFogfv(pname,params)                                                           _GA_glFuncs.glFogfv(pname,params)
#define glFogi(pname,param)                                                             _GA_glFuncs.glFogi(pname,param)
#define glFogiv(pname,params)                                                           _GA_glFuncs.glFogiv(pname,params)
#define glFrontFace(mode)                                                               _GA_glFuncs.glFrontFace(mode)
#define glFrustum(left,right,bottom,top,zNear,zFar)                                     _GA_glFuncs.glFrustum(left,right,bottom,top,zNear,zFar)
#define glGenLists(range)                                                               _GA_glFuncs.glGenLists(range)
#define glGenTextures(n,textures)                                                       _GA_glFuncs.glGenTextures(n,textures)
#define glGetBooleanv(pname,params)                                                     _GA_glFuncs.glGetBooleanv(pname,params)
#define glGetClipPlane(plane,equation)                                                  _GA_glFuncs.glGetClipPlane(plane,equation)
#define glGetDoublev(pname,params)                                                      _GA_glFuncs.glGetDoublev(pname,params)
#define glGetError()                                                                    _GA_glFuncs.glGetError()
#define glGetFloatv(pname,params)                                                       _GA_glFuncs.glGetFloatv(pname,params)
#define glGetIntegerv(pname,params)                                                     _GA_glFuncs.glGetIntegerv(pname,params)
#define glGetLightfv(light,pname,params)                                                _GA_glFuncs.glGetLightfv(light,pname,params)
#define glGetLightiv(light,pname,params)                                                _GA_glFuncs.glGetLightiv(light,pname,params)
#define glGetMapdv(target,query,v)                                                      _GA_glFuncs.glGetMapdv(target,query,v)
#define glGetMapfv(target,query,v)                                                      _GA_glFuncs.glGetMapfv(target,query,v)
#define glGetMapiv(target,query,v)                                                      _GA_glFuncs.glGetMapiv(target,query,v)
#define glGetMaterialfv(face,pname,params)                                              _GA_glFuncs.glGetMaterialfv(face,pname,params)
#define glGetMaterialiv(face,pname,params)                                              _GA_glFuncs.glGetMaterialiv(face,pname,params)
#define glGetPixelMapfv(map,values)                                                     _GA_glFuncs.glGetPixelMapfv(map,values)
#define glGetPixelMapuiv(map,values)                                                    _GA_glFuncs.glGetPixelMapuiv(map,values)
#define glGetPixelMapusv(map,values)                                                    _GA_glFuncs.glGetPixelMapusv(map,values)
#define glGetPointerv(pname,params)                                                     _GA_glFuncs.glGetPointerv(pname,params)
#define glGetPolygonStipple(mask)                                                       _GA_glFuncs.glGetPolygonStipple(mask)
#define glGetString(name)                                                               _GA_glFuncs.glGetString(name)
#define glGetTexEnvfv(target,pname,params)                                              _GA_glFuncs.glGetTexEnvfv(target,pname,params)
#define glGetTexEnviv(target,pname,params)                                              _GA_glFuncs.glGetTexEnviv(target,pname,params)
#define glGetTexGendv(coord,pname,params)                                               _GA_glFuncs.glGetTexGendv(coord,pname,params)
#define glGetTexGenfv(coord,pname,params)                                               _GA_glFuncs.glGetTexGenfv(coord,pname,params)
#define glGetTexGeniv(coord,pname,params)                                               _GA_glFuncs.glGetTexGeniv(coord,pname,params)
#define glGetTexImage(target,level,format,type,pixels)                                  _GA_glFuncs.glGetTexImage(target,level,format,type,pixels)
#define glGetTexLevelParameterfv(target,level,pname,params)                             _GA_glFuncs.glGetTexLevelParameterfv(target,level,pname,params)
#define glGetTexLevelParameteriv(target,level,pname,params)                             _GA_glFuncs.glGetTexLevelParameteriv(target,level,pname,params)
#define glGetTexParameterfv(target,pname,params)                                        _GA_glFuncs.glGetTexParameterfv(target,pname,params)
#define glGetTexParameteriv(target,pname,params)                                        _GA_glFuncs.glGetTexParameteriv(target,pname,params)
#define glHint(target,mode)                                                             _GA_glFuncs.glHint(target,mode)
#define glIndexMask(mask)                                                               _GA_glFuncs.glIndexMask(mask)
#define glIndexPointer(type,stride,pointer)                                             _GA_glFuncs.glIndexPointer(type,stride,pointer)
#define glIndexd(c)                                                                     _GA_glFuncs.glIndexd(c)
#define glIndexdv(c)                                                                    _GA_glFuncs.glIndexdv(c)
#define glIndexf(c)                                                                     _GA_glFuncs.glIndexf(c)
#define glIndexfv(c)                                                                    _GA_glFuncs.glIndexfv(c)
#define glIndexi(c)                                                                     _GA_glFuncs.glIndexi(c)
#define glIndexiv(c)                                                                    _GA_glFuncs.glIndexiv(c)
#define glIndexs(c)                                                                     _GA_glFuncs.glIndexs(c)
#define glIndexsv(c)                                                                    _GA_glFuncs.glIndexsv(c)
#define glIndexub(c)                                                                    _GA_glFuncs.glIndexub(c)
#define glIndexubv(c)                                                                   _GA_glFuncs.glIndexubv(c)
#define glInitNames()                                                                   _GA_glFuncs.glInitNames()
#define glInterleavedArrays(format,stride,pointer)                                      _GA_glFuncs.glInterleavedArrays(format,stride,pointer)
#define glIsEnabled(cap)                                                                _GA_glFuncs.glIsEnabled(cap)
#define glIsList(list)                                                                  _GA_glFuncs.glIsList(list)
#define glIsTexture(texture)                                                            _GA_glFuncs.glIsTexture(texture)
#define glLightModelf(pname,param)                                                      _GA_glFuncs.glLightModelf(pname,param)
#define glLightModelfv(pname,params)                                                    _GA_glFuncs.glLightModelfv(pname,params)
#define glLightModeli(pname,param)                                                      _GA_glFuncs.glLightModeli(pname,param)
#define glLightModeliv(pname,params)                                                    _GA_glFuncs.glLightModeliv(pname,params)
#define glLightf(light,pname,param)                                                     _GA_glFuncs.glLightf(light,pname,param)
#define glLightfv(light,pname,params)                                                   _GA_glFuncs.glLightfv(light,pname,params)
#define glLighti(light,pname,param)                                                     _GA_glFuncs.glLighti(light,pname,param)
#define glLightiv(light,pname,params)                                                   _GA_glFuncs.glLightiv(light,pname,params)
#define glLineStipple(factor,pattern)                                                   _GA_glFuncs.glLineStipple(factor,pattern)
#define glLineWidth(width)                                                              _GA_glFuncs.glLineWidth(width)
#define glListBase(base)                                                                _GA_glFuncs.glListBase(base)
#define glLoadIdentity()                                                                _GA_glFuncs.glLoadIdentity()
#define glLoadMatrixd(m)                                                                _GA_glFuncs.glLoadMatrixd(m)
#define glLoadMatrixf(m)                                                                _GA_glFuncs.glLoadMatrixf(m)
#define glLoadName(name)                                                                _GA_glFuncs.glLoadName(name)
#define glLogicOp(opcode)                                                               _GA_glFuncs.glLogicOp(opcode)
#define glMap1d(target,u1,u2,stride,order,points)                                       _GA_glFuncs.glMap1d(target,u1,u2,stride,order,points)
#define glMap1f(target,u1,u2,stride,order,points)                                       _GA_glFuncs.glMap1f(target,u1,u2,stride,order,points)
#define glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points)                _GA_glFuncs.glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points)
#define glMap2f(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points)                _GA_glFuncs.glMap2f(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points)
#define glMapGrid1d(un,u1,u2)                                                           _GA_glFuncs.glMapGrid1d(un,u1,u2)
#define glMapGrid1f(un,u1,u2)                                                           _GA_glFuncs.glMapGrid1f(un,u1,u2)
#define glMapGrid2d(un,u1,u2,vn,v1,v2)                                                  _GA_glFuncs.glMapGrid2d(un,u1,u2,vn,v1,v2)
#define glMapGrid2f(un,u1,u2,vn,v1,v2)                                                  _GA_glFuncs.glMapGrid2f(un,u1,u2,vn,v1,v2)
#define glMaterialf(face,pname,param)                                                   _GA_glFuncs.glMaterialf(face,pname,param)
#define glMaterialfv(face,pname,params)                                                 _GA_glFuncs.glMaterialfv(face,pname,params)
#define glMateriali(face,pname,param)                                                   _GA_glFuncs.glMateriali(face,pname,param)
#define glMaterialiv(face,pname,params)                                                 _GA_glFuncs.glMaterialiv(face,pname,params)
#define glMatrixMode(mode)                                                              _GA_glFuncs.glMatrixMode(mode)
#define glMultMatrixd(m)                                                                _GA_glFuncs.glMultMatrixd(m)
#define glMultMatrixf(m)                                                                _GA_glFuncs.glMultMatrixf(m)
#define glNewList(list,mode)                                                            _GA_glFuncs.glNewList(list,mode)
#define glNormal3b(nx,ny,nz)                                                            _GA_glFuncs.glNormal3b(nx,ny,nz)
#define glNormal3bv(v)                                                                  _GA_glFuncs.glNormal3bv(v)
#define glNormal3d(nx,ny,nz)                                                            _GA_glFuncs.glNormal3d(nx,ny,nz)
#define glNormal3dv(v)                                                                  _GA_glFuncs.glNormal3dv(v)
#define glNormal3f(nx,ny,nz)                                                            _GA_glFuncs.glNormal3f(nx,ny,nz)
#define glNormal3fv(v)                                                                  _GA_glFuncs.glNormal3fv(v)
#define glNormal3i(nx,ny,nz)                                                            _GA_glFuncs.glNormal3i(nx,ny,nz)
#define glNormal3iv(v)                                                                  _GA_glFuncs.glNormal3iv(v)
#define glNormal3s(nx,ny,nz)                                                            _GA_glFuncs.glNormal3s(nx,ny,nz)
#define glNormal3sv(v)                                                                  _GA_glFuncs.glNormal3sv(v)
#define glNormalPointer(type,stride,pointer)                                            _GA_glFuncs.glNormalPointer(type,stride,pointer)
#define glOrtho(left,right,bottom,top,zNear,zFar)                                       _GA_glFuncs.glOrtho(left,right,bottom,top,zNear,zFar)
#define glPassThrough(token)                                                            _GA_glFuncs.glPassThrough(token)
#define glPixelMapfv(map,mapsize,values)                                                _GA_glFuncs.glPixelMapfv(map,mapsize,values)
#define glPixelMapuiv(map,mapsize,values)                                               _GA_glFuncs.glPixelMapuiv(map,mapsize,values)
#define glPixelMapusv(map,mapsize,values)                                               _GA_glFuncs.glPixelMapusv(map,mapsize,values)
#define glPixelStoref(pname,param)                                                      _GA_glFuncs.glPixelStoref(pname,param)
#define glPixelStorei(pname,param)                                                      _GA_glFuncs.glPixelStorei(pname,param)
#define glPixelTransferf(pname,param)                                                   _GA_glFuncs.glPixelTransferf(pname,param)
#define glPixelTransferi(pname,param)                                                   _GA_glFuncs.glPixelTransferi(pname,param)
#define glPixelZoom(xfactor,yfactor)                                                    _GA_glFuncs.glPixelZoom(xfactor,yfactor)
#define glPointSize(size)                                                               _GA_glFuncs.glPointSize(size)
#define glPolygonMode(face,mode)                                                        _GA_glFuncs.glPolygonMode(face,mode)
#define glPolygonOffset(factor,units)                                                   _GA_glFuncs.glPolygonOffset(factor,units)
#define glPolygonStipple(mask)                                                          _GA_glFuncs.glPolygonStipple(mask)
#define glPopAttrib()                                                                   _GA_glFuncs.glPopAttrib()
#define glPopClientAttrib()                                                             _GA_glFuncs.glPopClientAttrib()
#define glPopMatrix()                                                                   _GA_glFuncs.glPopMatrix()
#define glPopName()                                                                     _GA_glFuncs.glPopName()
#define glPrioritizeTextures(n,textures,priorities)                                     _GA_glFuncs.glPrioritizeTextures(n,textures,priorities)
#define glPushAttrib(mask)                                                              _GA_glFuncs.glPushAttrib(mask)
#define glPushClientAttrib(mask)                                                        _GA_glFuncs.glPushClientAttrib(mask)
#define glPushMatrix()                                                                  _GA_glFuncs.glPushMatrix()
#define glPushName(name)                                                                _GA_glFuncs.glPushName(name)
#define glRasterPos2d(x,y)                                                              _GA_glFuncs.glRasterPos2d(x,y)
#define glRasterPos2dv(v)                                                               _GA_glFuncs.glRasterPos2dv(v)
#define glRasterPos2f(x,y)                                                              _GA_glFuncs.glRasterPos2f(x,y)
#define glRasterPos2fv(v)                                                               _GA_glFuncs.glRasterPos2fv(v)
#define glRasterPos2i(x,y)                                                              _GA_glFuncs.glRasterPos2i(x,y)
#define glRasterPos2iv(v)                                                               _GA_glFuncs.glRasterPos2iv(v)
#define glRasterPos2s(x,y)                                                              _GA_glFuncs.glRasterPos2s(x,y)
#define glRasterPos2sv(v)                                                               _GA_glFuncs.glRasterPos2sv(v)
#define glRasterPos3d(x,y,z)                                                            _GA_glFuncs.glRasterPos3d(x,y,z)
#define glRasterPos3dv(v)                                                               _GA_glFuncs.glRasterPos3dv(v)
#define glRasterPos3f(x,y,z)                                                            _GA_glFuncs.glRasterPos3f(x,y,z)
#define glRasterPos3fv(v)                                                               _GA_glFuncs.glRasterPos3fv(v)
#define glRasterPos3i(x,y,z)                                                            _GA_glFuncs.glRasterPos3i(x,y,z)
#define glRasterPos3iv(v)                                                               _GA_glFuncs.glRasterPos3iv(v)
#define glRasterPos3s(x,y,z)                                                            _GA_glFuncs.glRasterPos3s(x,y,z)
#define glRasterPos3sv(v)                                                               _GA_glFuncs.glRasterPos3sv(v)
#define glRasterPos4d(x,y,z,w)                                                          _GA_glFuncs.glRasterPos4d(x,y,z,w)
#define glRasterPos4dv(v)                                                               _GA_glFuncs.glRasterPos4dv(v)
#define glRasterPos4f(x,y,z,w)                                                          _GA_glFuncs.glRasterPos4f(x,y,z,w)
#define glRasterPos4fv(v)                                                               _GA_glFuncs.glRasterPos4fv(v)
#define glRasterPos4i(x,y,z,w)                                                          _GA_glFuncs.glRasterPos4i(x,y,z,w)
#define glRasterPos4iv(v)                                                               _GA_glFuncs.glRasterPos4iv(v)
#define glRasterPos4s(x,y,z,w)                                                          _GA_glFuncs.glRasterPos4s(x,y,z,w)
#define glRasterPos4sv(v)                                                               _GA_glFuncs.glRasterPos4sv(v)
#define glReadBuffer(mode)                                                              _GA_glFuncs.glReadBuffer(mode)
#define glReadPixels(x,y,width,height,format,type,pixels)                               _GA_glFuncs.glReadPixels(x,y,width,height,format,type,pixels)
#define glRectd(x1,y1,x2,y2)                                                            _GA_glFuncs.glRectd(x1,y1,x2,y2)
#define glRectdv(v1,v2)                                                                 _GA_glFuncs.glRectdv(v1,v2)
#define glRectf(x1,y1,x2,y2)                                                            _GA_glFuncs.glRectf(x1,y1,x2,y2)
#define glRectfv(v1,v2)                                                                 _GA_glFuncs.glRectfv(v1,v2)
#define glRecti(x1,y1,x2,y2)                                                            _GA_glFuncs.glRecti(x1,y1,x2,y2)
#define glRectiv(v1,v2)                                                                 _GA_glFuncs.glRectiv(v1,v2)
#define glRects(x1,y1,x2,y2)                                                            _GA_glFuncs.glRects(x1,y1,x2,y2)
#define glRectsv(v1,v2)                                                                 _GA_glFuncs.glRectsv(v1,v2)
#define glRenderMode(mode)                                                              _GA_glFuncs.glRenderMode(mode)
#define glRotated(angle,x,y,z)                                                          _GA_glFuncs.glRotated(angle,x,y,z)
#define glRotatef(angle,x,y,z)                                                          _GA_glFuncs.glRotatef(angle,x,y,z)
#define glScaled(x,y,z)                                                                 _GA_glFuncs.glScaled(x,y,z)
#define glScalef(x,y,z)                                                                 _GA_glFuncs.glScalef(x,y,z)
#define glScissor(x,y,width,height)                                                     _GA_glFuncs.glScissor(x,y,width,height)
#define glSelectBuffer(size,buffer)                                                     _GA_glFuncs.glSelectBuffer(size,buffer)
#define glShadeModel(mode)                                                              _GA_glFuncs.glShadeModel(mode)
#define glStencilFunc(func,ref,mask)                                                    _GA_glFuncs.glStencilFunc(func,ref,mask)
#define glStencilMask(mask)                                                             _GA_glFuncs.glStencilMask(mask)
#define glStencilOp(fail,zfail,zpass)                                                   _GA_glFuncs.glStencilOp(fail,zfail,zpass)
#define glTexCoord1d(s)                                                                 _GA_glFuncs.glTexCoord1d(s)
#define glTexCoord1dv(v)                                                                _GA_glFuncs.glTexCoord1dv(v)
#define glTexCoord1f(s)                                                                 _GA_glFuncs.glTexCoord1f(s)
#define glTexCoord1fv(v)                                                                _GA_glFuncs.glTexCoord1fv(v)
#define glTexCoord1i(s)                                                                 _GA_glFuncs.glTexCoord1i(s)
#define glTexCoord1iv(v)                                                                _GA_glFuncs.glTexCoord1iv(v)
#define glTexCoord1s(s)                                                                 _GA_glFuncs.glTexCoord1s(s)
#define glTexCoord1sv(v)                                                                _GA_glFuncs.glTexCoord1sv(v)
#define glTexCoord2d(s,t)                                                               _GA_glFuncs.glTexCoord2d(s,t)
#define glTexCoord2dv(v)                                                                _GA_glFuncs.glTexCoord2dv(v)
#define glTexCoord2f(s,t)                                                               _GA_glFuncs.glTexCoord2f(s,t)
#define glTexCoord2fv(v)                                                                _GA_glFuncs.glTexCoord2fv(v)
#define glTexCoord2i(s,t)                                                               _GA_glFuncs.glTexCoord2i(s,t)
#define glTexCoord2iv(v)                                                                _GA_glFuncs.glTexCoord2iv(v)
#define glTexCoord2s(s,t)                                                               _GA_glFuncs.glTexCoord2s(s,t)
#define glTexCoord2sv(v)                                                                _GA_glFuncs.glTexCoord2sv(v)
#define glTexCoord3d(s,t,r)                                                             _GA_glFuncs.glTexCoord3d(s,t,r)
#define glTexCoord3dv(v)                                                                _GA_glFuncs.glTexCoord3dv(v)
#define glTexCoord3f(s,t,r)                                                             _GA_glFuncs.glTexCoord3f(s,t,r)
#define glTexCoord3fv(v)                                                                _GA_glFuncs.glTexCoord3fv(v)
#define glTexCoord3i(s,t,r)                                                             _GA_glFuncs.glTexCoord3i(s,t,r)
#define glTexCoord3iv(v)                                                                _GA_glFuncs.glTexCoord3iv(v)
#define glTexCoord3s(s,t,r)                                                             _GA_glFuncs.glTexCoord3s(s,t,r)
#define glTexCoord3sv(v)                                                                _GA_glFuncs.glTexCoord3sv(v)
#define glTexCoord4d(s,t,r,q)                                                           _GA_glFuncs.glTexCoord4d(s,t,r,q)
#define glTexCoord4dv(v)                                                                _GA_glFuncs.glTexCoord4dv(v)
#define glTexCoord4f(s,t,r,q)                                                           _GA_glFuncs.glTexCoord4f(s,t,r,q)
#define glTexCoord4fv(v)                                                                _GA_glFuncs.glTexCoord4fv(v)
#define glTexCoord4i(s,t,r,q)                                                           _GA_glFuncs.glTexCoord4i(s,t,r,q)
#define glTexCoord4iv(v)                                                                _GA_glFuncs.glTexCoord4iv(v)
#define glTexCoord4s(s,t,r,q)                                                           _GA_glFuncs.glTexCoord4s(s,t,r,q)
#define glTexCoord4sv(v)                                                                _GA_glFuncs.glTexCoord4sv(v)
#define glTexCoordPointer(size,type,stride,pointer)                                     _GA_glFuncs.glTexCoordPointer(size,type,stride,pointer)
#define glTexEnvf(target,pname,param)                                                   _GA_glFuncs.glTexEnvf(target,pname,param)
#define glTexEnvfv(target,pname,params)                                                 _GA_glFuncs.glTexEnvfv(target,pname,params)
#define glTexEnvi(target,pname,param)                                                   _GA_glFuncs.glTexEnvi(target,pname,param)
#define glTexEnviv(target,pname,params)                                                 _GA_glFuncs.glTexEnviv(target,pname,params)
#define glTexGend(coord,pname,param)                                                    _GA_glFuncs.glTexGend(coord,pname,param)
#define glTexGendv(coord,pname,params)                                                  _GA_glFuncs.glTexGendv(coord,pname,params)
#define glTexGenf(coord,pname,param)                                                    _GA_glFuncs.glTexGenf(coord,pname,param)
#define glTexGenfv(coord,pname,params)                                                  _GA_glFuncs.glTexGenfv(coord,pname,params)
#define glTexGeni(coord,pname,param)                                                    _GA_glFuncs.glTexGeni(coord,pname,param)
#define glTexGeniv(coord,pname,params)                                                  _GA_glFuncs.glTexGeniv(coord,pname,params)
#define glTexImage1D(target,level,components,width,border,format,type,pixels)           _GA_glFuncs.glTexImage1D(target,level,components,width,border,format,type,pixels)
#define glTexImage2D(target,level,components,width,height,border,format,type,pixels)    _GA_glFuncs.glTexImage2D(target,level,components,width,height,border,format,type,pixels)
#define glTexParameterf(target,pname,param)                                             _GA_glFuncs.glTexParameterf(target,pname,param)
#define glTexParameterfv(target,pname,params)                                           _GA_glFuncs.glTexParameterfv(target,pname,params)
#define glTexParameteri(target,pname,param)                                             _GA_glFuncs.glTexParameteri(target,pname,param)
#define glTexParameteriv(target,pname,params)                                           _GA_glFuncs.glTexParameteriv(target,pname,params)
#define glTexSubImage1D(target,level,xoffset,width,format,type,pixels)                  _GA_glFuncs.glTexSubImage1D(target,level,xoffset,width,format,type,pixels)
#define glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels)   _GA_glFuncs.glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels)
#define glTranslated(x,y,z)                                                             _GA_glFuncs.glTranslated(x,y,z)
#define glTranslatef(x,y,z)                                                             _GA_glFuncs.glTranslatef(x,y,z)
#define glVertex2d(x,y)                                                                 _GA_glFuncs.glVertex2d(x,y)
#define glVertex2dv(v)                                                                  _GA_glFuncs.glVertex2dv(v)
#define glVertex2f(x,y)                                                                 _GA_glFuncs.glVertex2f(x,y)
#define glVertex2fv(v)                                                                  _GA_glFuncs.glVertex2fv(v)
#define glVertex2i(x,y)                                                                 _GA_glFuncs.glVertex2i(x,y)
#define glVertex2iv(v)                                                                  _GA_glFuncs.glVertex2iv(v)
#define glVertex2s(x,y)                                                                 _GA_glFuncs.glVertex2s(x,y)
#define glVertex2sv(v)                                                                  _GA_glFuncs.glVertex2sv(v)
#define glVertex3d(x,y,z)                                                               _GA_glFuncs.glVertex3d(x,y,z)
#define glVertex3dv(v)                                                                  _GA_glFuncs.glVertex3dv(v)
#define glVertex3f(x,y,z)                                                               _GA_glFuncs.glVertex3f(x,y,z)
#define glVertex3fv(v)                                                                  _GA_glFuncs.glVertex3fv(v)
#define glVertex3i(x,y,z)                                                               _GA_glFuncs.glVertex3i(x,y,z)
#define glVertex3iv(v)                                                                  _GA_glFuncs.glVertex3iv(v)
#define glVertex3s(x,y,z)                                                               _GA_glFuncs.glVertex3s(x,y,z)
#define glVertex3sv(v)                                                                  _GA_glFuncs.glVertex3sv(v)
#define glVertex4d(x,y,z,w)                                                             _GA_glFuncs.glVertex4d(x,y,z,w)
#define glVertex4dv(v)                                                                  _GA_glFuncs.glVertex4dv(v)
#define glVertex4f(x,y,z,w)                                                             _GA_glFuncs.glVertex4f(x,y,z,w)
#define glVertex4fv(v)                                                                  _GA_glFuncs.glVertex4fv(v)
#define glVertex4i(x,y,z,w)                                                             _GA_glFuncs.glVertex4i(x,y,z,w)
#define glVertex4iv(v)                                                                  _GA_glFuncs.glVertex4iv(v)
#define glVertex4s(x,y,z,w)                                                             _GA_glFuncs.glVertex4s(x,y,z,w)
#define glVertex4sv(v)                                                                  _GA_glFuncs.glVertex4sv(v)
#define glVertexPointer(size,type,stride,pointer)                                       _GA_glFuncs.glVertexPointer(size,type,stride,pointer)
#define glViewport(x,y,width,height)                                                    _GA_glFuncs.glViewport(x,y,width,height)

/* OpenGL 1.2/1.2.1 compatible functions */
#define glBlendColor(a,b,c,d)                                                           _GA_glFuncs.glBlendColor(a,b,c,d)
#define glBlendEquation(a)                                                              _GA_glFuncs.glBlendEquation(a)
#define glDrawRangeElements(a,b,c,d,e,f)                                                _GA_glFuncs.glDrawRangeElements(a,b,c,d,e,f)
#define glColorTable(a,b,c,d,e,f)                                                       _GA_glFuncs.glColorTable(a,b,c,d,e,f)
#define glColorTableParameterfv(a,b,c)                                                  _GA_glFuncs.glColorTableParameterfv(a,b,c)
#define glColorTableParameteriv(a,b,c)                                                  _GA_glFuncs.glColorTableParameteriv(a,b,c)
#define glCopyColorTable(a,b,c,d,e)                                                     _GA_glFuncs.glCopyColorTable(a,b,c,d,e)
#define glGetColorTable(a,b,c,d)                                                        _GA_glFuncs.glGetColorTable(a,b,c,d)
#define glGetColorTableParameterfv(a,b,c)                                               _GA_glFuncs.glGetColorTableParameterfv(a,b,c)
#define glGetColorTableParameteriv(a,b,c,d)                                             _GA_glFuncs.glGetColorTableParameteriv(a,b,c,d)
#define glColorSubTable(a,b,c,d,e,f)                                                    _GA_glFuncs.glColorSubTable(a,b,c,d,e,f)
#define glCopyColorSubTable(a,b,c,d,e)                                                  _GA_glFuncs.glCopyColorSubTable(a,b,c,d,e)
#define glConvolutionFilter1D(a,b,c,d,e,f)                                              _GA_glFuncs.glConvolutionFilter1D(a,b,c,d,e,f)
#define glConvolutionFilter2D(a,b,c,d,e,f,g)                                            _GA_glFuncs.glConvolutionFilter2D(a,b,c,d,e,f,g)
#define glConvolutionParameterf(a,b,c)                                                  _GA_glFuncs.glConvolutionParameterf(a,b,c)
#define glConvolutionParameterfv(a,b,c)                                                 _GA_glFuncs.glConvolutionParameterfv(a,b,c)
#define glConvolutionParameteri(a,b,c)                                                  _GA_glFuncs.glConvolutionParameteri(a,b,c)
#define glConvolutionParameteriv(a,b,c)                                                 _GA_glFuncs.glConvolutionParameteriv(a,b,c)
#define glCopyConvolutionFilter1D(a,b,c,d,e)                                            _GA_glFuncs.glCopyConvolutionFilter1D(a,b,c,d,e)
#define glCopyConvolutionFilter2D(a,b,c,d,e,f)                                          _GA_glFuncs.glCopyConvolutionFilter2D(a,b,c,d,e,f)
#define glGetConvolutionFilter(a,b,c,d)                                                 _GA_glFuncs.glGetConvolutionFilter(a,b,c,d)
#define glGetConvolutionParameterfv(a,b,c)                                              _GA_glFuncs.glGetConvolutionParameterfv(a,b,c)
#define glGetConvolutionParameteriv(a,b,c)                                              _GA_glFuncs.glGetConvolutionParameteriv(a,b,c)
#define glGetSeparableFilter(a,b,c,d,e,f)                                               _GA_glFuncs.glGetSeparableFilter(a,b,c,d,e,f)
#define glSeparableFilter2D(a,b,c,d,e,f,g,h)                                            _GA_glFuncs.glSeparableFilter2D(a,b,c,d,e,f,g,h)
#define glGetHistogram(a,b,c,d,e)                                                       _GA_glFuncs.glGetHistogram(a,b,c,d,e)
#define glGetHistogramParameterfv(a,b,c)                                                _GA_glFuncs.glGetHistogramParameterfv(a,b,c)
#define glGetHistogramParameteriv(a,b,c)                                                _GA_glFuncs.glGetHistogramParameteriv(a,b,c)
#define glGetMinmax(a,b,c,d,e)                                                          _GA_glFuncs.glGetMinmax(a,b,c,d,e)
#define glGetMinmaxParameterfv(a,b,c)                                                   _GA_glFuncs.glGetMinmaxParameterfv(a,b,c)
#define glGetMinmaxParameteriv(a,b,c)                                                   _GA_glFuncs.glGetMinmaxParameteriv(a,b,c)
#define glHistogram(a,b,c,d)                                                            _GA_glFuncs.glHistogram(a,b,c,d)
#define glMinmax(a,b,c)                                                                 _GA_glFuncs.glMinmax(a,b,c)
#define glResetHistogram(a)                                                             _GA_glFuncs.glResetHistogram(a)
#define glResetMinmax(a)                                                                _GA_glFuncs.glResetMinmax(a)
#define glTexImage3D(a,b,c,d,e,f,g,h,i,j)                                               _GA_glFuncs.glTexImage3D(a,b,c,d,e,f,g,h,i,j)
#define glTexSubImage3D(a,b,c,d,e,f,g,h,i,j,k)                                          _GA_glFuncs.glTexSubImage3D(a,b,c,d,e,f,g,h,i,j,k)
#define glCopyTexSubImage3D(a,b,c,d,e,f,g,h,i)                                          _GA_glFuncs.glCopyTexSubImage3D(a,b,c,d,e,f,g,h,i)

/* OpenGL 1.3 compatible functions */
#define glActiveTexture(a)                                                              _GA_glFuncs.glActiveTexture(a)
#define glClientActiveTexture()                                                         _GA_glFuncs.glClientActiveTexture()
#define glMultiTexCoord1d(a,b)                                                          _GA_glFuncs.glMultiTexCoord1d(a,b)
#define glMultiTexCoord1dv(a,b)                                                         _GA_glFuncs.glMultiTexCoord1dv(a,b)
#define glMultiTexCoord1f(a,b)                                                          _GA_glFuncs.glMultiTexCoord1f(a,b)
#define glMultiTexCoord1fv(a,b)                                                         _GA_glFuncs.glMultiTexCoord1fv(a,b)
#define glMultiTexCoord1i(a,b)                                                          _GA_glFuncs.glMultiTexCoord1i(a,b)
#define glMultiTexCoord1iv(a,b)                                                         _GA_glFuncs.glMultiTexCoord1iv(a,b)
#define glMultiTexCoord1s(a,b)                                                          _GA_glFuncs.glMultiTexCoord1s(a,b)
#define glMultiTexCoord1sv(a,b)                                                         _GA_glFuncs.glMultiTexCoord1sv(a,b)
#define glMultiTexCoord2d(a,b,c)                                                        _GA_glFuncs.glMultiTexCoord2d(a,b,c)
#define glMultiTexCoord2dv(a,b)                                                         _GA_glFuncs.glMultiTexCoord2dv(a,b)
#define glMultiTexCoord2f(a,b,c)                                                        _GA_glFuncs.glMultiTexCoord2f(a,b,c)
#define glMultiTexCoord2fv(a,b)                                                         _GA_glFuncs.glMultiTexCoord2fv(a,b)
#define glMultiTexCoord2i(a,b,c)                                                        _GA_glFuncs.glMultiTexCoord2i(a,b,c)
#define glMultiTexCoord2iv(a,b)                                                         _GA_glFuncs.glMultiTexCoord2iv(a,b)
#define glMultiTexCoord2s(a,b,c)                                                        _GA_glFuncs.glMultiTexCoord2s(a,b,c)
#define glMultiTexCoord2sv(a,b)                                                         _GA_glFuncs.glMultiTexCoord2sv(a,b)
#define glMultiTexCoord3d(a,b,c,d)                                                      _GA_glFuncs.glMultiTexCoord3d(a,b,c,d)
#define glMultiTexCoord3dv(a,b)                                                         _GA_glFuncs.glMultiTexCoord3dv(a,b)
#define glMultiTexCoord3f(a,b,c,d)                                                      _GA_glFuncs.glMultiTexCoord3f(a,b,c,d)
#define glMultiTexCoord3fv(a,b)                                                         _GA_glFuncs.glMultiTexCoord3fv(a,b)
#define glMultiTexCoord3i(a,b,c,d)                                                      _GA_glFuncs.glMultiTexCoord3i(a,b,c,d)
#define glMultiTexCoord3iv(a,b)                                                         _GA_glFuncs.glMultiTexCoord3iv(a,b)
#define glMultiTexCoord3s(a,b,c,d)                                                      _GA_glFuncs.glMultiTexCoord3s(a,b,c,d)
#define glMultiTexCoord3sv(a,b)                                                         _GA_glFuncs.glMultiTexCoord3sv(a,b)
#define glMultiTexCoord4d(a,b,c,d,e)                                                    _GA_glFuncs.glMultiTexCoord4d(a,b,c,d,e)
#define glMultiTexCoord4dv(a,b)                                                         _GA_glFuncs.glMultiTexCoord4dv(a,b)
#define glMultiTexCoord4f(a,b,c,d,e)                                                    _GA_glFuncs.glMultiTexCoord4f(a,b,c,d,e)
#define glMultiTexCoord4fv(a,b)                                                         _GA_glFuncs.glMultiTexCoord4fv(a,b)
#define glMultiTexCoord4i(a,b,c,d,e)                                                    _GA_glFuncs.glMultiTexCoord4i(a,b,c,d,e)
#define glMultiTexCoord4iv(a,b)                                                         _GA_glFuncs.glMultiTexCoord4iv(a,b)
#define glMultiTexCoord4s(a,b,c,d,e)                                                    _GA_glFuncs.glMultiTexCoord4s(a,b,c,d,e)
#define glMultiTexCoord4sv(a,b)                                                         _GA_glFuncs.glMultiTexCoord4sv(a,b)
#define glLoadTransposeMatrixf(a)                                                       _GA_glFuncs.glLoadTransposeMatrixf(a)
#define glLoadTransposeMatrixd(a)                                                       _GA_glFuncs.glLoadTransposeMatrixd(a)
#define glMultTransposeMatrixf(a)                                                       _GA_glFuncs.glMultTransposeMatrixf(a)
#define glMultTransposeMatrixd(a)                                                       _GA_glFuncs.glMultTransposeMatrixd(a)
#define glSampleCoverage(a,b)                                                           _GA_glFuncs.glSampleCoverage(a,b)
#define glCompressedTexImage3D(a,b,c,d,e,f,g,h,i)                                       _GA_glFuncs.glCompressedTexImage3D(a,b,c,d,e,f,g,h,i)
#define glCompressedTexImage2D(a,b,c,d,e,f,g,h)                                         _GA_glFuncs.glCompressedTexImage2D(a,b,c,d,e,f,g,h)
#define glCompressedTexImage1D(a,b,c,d,e,f,g)                                           _GA_glFuncs.glCompressedTexImage1D(a,b,c,d,e,f,g)
#define glCompressedTexSubImage3D(a,b,c,d,e,f,g,h,i,j,k)                                _GA_glFuncs.glCompressedTexSubImage3D(a,b,c,d,e,f,g,h,i,j,k)
#define glCompressedTexSubImage2D(a,b,c,d,e,f,g,h,i)                                    _GA_glFuncs.glCompressedTexSubImage2D(a,b,c,d,e,f,g,h,i)
#define glCompressedTexSubImage1D(a,b,c,d,e,f,g)                                        _GA_glFuncs.glCompressedTexSubImage1D(a,b,c,d,e,f,g)
#define glGetCompressedTexImage(a,b,c)                                                  _GA_glFuncs.glGetCompressedTexImage(a,b,c)

/* OpenGL 1.4 compatible functions */
#define glBlendFuncSeparate(a,b,c,d)                                                    _GA_glFuncs.glBlendFuncSeparate(a,b,c,d)
#define glFogCoordf(a)                                                                  _GA_glFuncs.glFogCoordf(a)
#define glFogCoordfv(a)                                                                 _GA_glFuncs.glFogCoordfv(a)
#define glFogCoordd(a)                                                                  _GA_glFuncs.glFogCoordd(a)
#define glFogCoorddv(a)                                                                 _GA_glFuncs.glFogCoorddv(a)
#define glFogCoordPointer(a,b,c)                                                        _GA_glFuncs.glFogCoordPointer(a,b,c)
#define glMultiDrawArrays(a,b,c,d)                                                      _GA_glFuncs.glMultiDrawArrays(a,b,c,d)
#define glMultiDrawElements(a,b,c,d,e)                                                  _GA_glFuncs.glMultiDrawElements(a,b,c,d,e)
#define glPointParameterf(a,b)                                                          _GA_glFuncs.glPointParameterf(a,b)
#define glPointParameterfv(a,b)                                                         _GA_glFuncs.glPointParameterfv(a,b)
#define glPointParameteri(a,b)                                                          _GA_glFuncs.glPointParameteri(a,b)
#define glPointParameteriv(a,b)                                                         _GA_glFuncs.glPointParameteriv(a,b)
#define glSecondaryColor3b(a,b,c)                                                       _GA_glFuncs.glSecondaryColor3b(a,b,c)
#define glSecondaryColor3bv(a)                                                          _GA_glFuncs.glSecondaryColor3bv(a)
#define glSecondaryColor3d(a,b,c)                                                       _GA_glFuncs.glSecondaryColor3d(a,b,c)
#define glSecondaryColor3dv(a)                                                          _GA_glFuncs.glSecondaryColor3dv(a)
#define glSecondaryColor3f(a,b,c)                                                       _GA_glFuncs.glSecondaryColor3f(a,b,c)
#define glSecondaryColor3fv(a)                                                          _GA_glFuncs.glSecondaryColor3fv(a)
#define glSecondaryColor3i(a,b,c)                                                       _GA_glFuncs.glSecondaryColor3i(a,b,c)
#define glSecondaryColor3iv(a)                                                          _GA_glFuncs.glSecondaryColor3iv(a)
#define glSecondaryColor3s(a,b,c)                                                       _GA_glFuncs.glSecondaryColor3s(a,b,c)
#define glSecondaryColor3sv(a)                                                          _GA_glFuncs.glSecondaryColor3sv(a)
#define glSecondaryColor3ub(a,b,c)                                                      _GA_glFuncs.glSecondaryColor3ub(a,b,c)
#define glSecondaryColor3ubv(a)                                                         _GA_glFuncs.glSecondaryColor3ubv(a)
#define glSecondaryColor3ui(a,b,c)                                                      _GA_glFuncs.glSecondaryColor3ui(a,b,c)
#define glSecondaryColor3uiv(a)                                                         _GA_glFuncs.glSecondaryColor3uiv(a)
#define glSecondaryColor3us(a,b,c)                                                      _GA_glFuncs.glSecondaryColor3us(a,b,c)
#define glSecondaryColor3usv(a)                                                         _GA_glFuncs.glSecondaryColor3usv(a)
#define glSecondaryColorPointer(a,b,c,d)                                                _GA_glFuncs.glSecondaryColorPointer(a,b,c,d)
#define glWindowPos2d(a,b)                                                              _GA_glFuncs.glWindowPos2d(a,b)
#define glWindowPos2dv(a)                                                               _GA_glFuncs.glWindowPos2dv(a)
#define glWindowPos2f(a,b)                                                              _GA_glFuncs.glWindowPos2f(a,b)
#define glWindowPos2fv(a)                                                               _GA_glFuncs.glWindowPos2fv(a)
#define glWindowPos2i(a,b)                                                              _GA_glFuncs.glWindowPos2i(a,b)
#define glWindowPos2iv(a)                                                               _GA_glFuncs.glWindowPos2iv(a)
#define glWindowPos2s(a,b)                                                              _GA_glFuncs.glWindowPos2s(a,b)
#define glWindowPos2sv(a)                                                               _GA_glFuncs.glWindowPos2sv(a)
#define glWindowPos3d(a,b,c)                                                            _GA_glFuncs.glWindowPos3d(a,b,c)
#define glWindowPos3dv(a)                                                               _GA_glFuncs.glWindowPos3dv(a)
#define glWindowPos3f(a,b,c)                                                            _GA_glFuncs.glWindowPos3f(a,b,c)
#define glWindowPos3fv(a)                                                               _GA_glFuncs.glWindowPos3fv(a)
#define glWindowPos3i(a,b,c)                                                            _GA_glFuncs.glWindowPos3i(a,b,c)
#define glWindowPos3iv(a)                                                               _GA_glFuncs.glWindowPos3iv(a)
#define glWindowPos3s(a,b,c)                                                            _GA_glFuncs.glWindowPos3s(a,b,c)
#define glWindowPos3sv(a)                                                               _GA_glFuncs.glWindowPos3sv(a)

#endif

/*---------------------------- Global variables ---------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern GA_glFuncs _VARAPI _GA_glFuncs;

#ifdef __cplusplus
}
#endif

#endif /* __GL_STUB_H */

