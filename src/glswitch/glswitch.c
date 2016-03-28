/****************************************************************************
*
*                       SciTech OpenGL Switching Library
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
* Environment:  Win32
*
* Description:  Module for the OpenGL switching library. This
*               library allows application developers to link with the
*               glswitch library instead of opengl32.lib, and switch between
*               multiple OpenGL DLL's at runtime.
*
****************************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "GL/glswitch.h"

/*--------------------- Macros and type definitions -----------------------*/

typedef void (APIENTRYP SGIGLU_initLibrary_t)(GLS_glFuncs *glFuncs);
typedef int (APIENTRYP DGL_registerLicense_t)(const uchar*);

/*--------------------------- Global Variables ----------------------------*/

static ibool        IsGLDirect = false;
static HMODULE      _GLS_hModGL = NULL;
static HMODULE      _GLS_hModGLU = NULL;
static HMODULE      _GLS_hModGDI = NULL;
GLS_glFuncs         _VARAPI _GLS_glFuncs;
GLS_gluFuncs        _VARAPI _GLS_gluFuncs;
GLS_wglFuncs        _VARAPI _GLS_wglFuncs;

/*------------------------- Implementation --------------------------------*/

/* Define the names of DLL's that we dynamically load */

#define GDI32_DLL                       "gdi32.dll"
#define MSGL_DLL                        "opengl32.dll"
#define GLDIRECT_DLL                    "gldirect.dll"
#define MSGLU_DLL                       "glu32.dll"
#define SGIGLU_DLL                      "sgiglu.dll"
#define GLS_OPENGL                      "GLS_OPENGL"
#define _PFD_GENERIC_ACCELERATED        0x00001000

/* Macros to get the procedure addresses from loaded DLL's */

#define GL_PROC_ADDRESS(name)   \
        if ((funcPtr = (void*)GetProcAddress(_GLS_hModGL,#name)) != NULL) _GLS_glFuncs.name = funcPtr

#define GLU_PROC_ADDRESS(name)  \
        if ((funcPtr = (void*)GetProcAddress(_GLS_hModGLU,#name)) != NULL) _GLS_gluFuncs.name = funcPtr

#define GDI_PROC_ADDRESS(name)  \
        if ((funcPtr = (void*)GetProcAddress(_GLS_hModGDI,#name)) != NULL) _GLS_wglFuncs.gls##name = funcPtr

#define WGL_PROC_ADDRESS(name)  \
        if ((funcPtr = (void*)GetProcAddress(_GLS_hModGL,"wgl"#name)) != NULL) _GLS_wglFuncs.gls##name = funcPtr

#define WGL_GDI_PROC_ADDRESS(name)  \
        if ((funcPtr = (void*)GetProcAddress(_GLS_hModGL,"wgl"#name)) != NULL) _GLS_wglFuncs.gls##name = funcPtr

/****************************************************************************
REMARKS:
Report a fatal error condition and halt the program.
****************************************************************************/
static void FatalError(
    const char *msg)
{
    MessageBox(NULL,msg,"Fatal Error!", MB_ICONEXCLAMATION);
    exit(1);
}

/****************************************************************************
REMARKS:
Fatal error handler for non-exported GLS_glFuncs.
****************************************************************************/
static void _GL_fatalErrorHandler(void)
{
    FatalError("Unsupported OpenGL export function called!\n");
}

/****************************************************************************
REMARKS:
Fatal error handler for non-exported _GLS_gluFuncs.
****************************************************************************/
static void _GLU_fatalErrorHandler(void)
{
    FatalError("Unsupported OpenGL GLU export function called!\n");
}

/****************************************************************************
REMARKS:
Fatal error handler for non-exported _GLS_wglFuncs.
****************************************************************************/
static void _WGL_fatalErrorHandler(void)
{
    FatalError("Unsupported OpenGL WGL export function called!\n");
}

/****************************************************************************
REMARKS:
Function to initialise the exported OpenGL functions for the OpenGL
implementation we have connected to. The mechanism we use here ensures that
we can support forwards and backwards binary compatibility.
****************************************************************************/
static void _GLS_initGLFuncs(void)
{
    int     i,max;
    ulong   *p;

    _GLS_glFuncs.dwSize = sizeof(GLS_glFuncs);
    max = sizeof(GLS_glFuncs)/sizeof(SGIGLU_initLibrary_t   );
    for (i = 0,p = (ulong*)&_GLS_glFuncs; i < max; i++)
        *p++ = (ulong)_GL_fatalErrorHandler;
}

/****************************************************************************
REMARKS:
Function to initialise the exported OpenGL functions for the OpenGL
implementation we have connected to. The mechanism we use here ensures that
we can support forwards and backwards binary compatibility.
****************************************************************************/
static void _GLS_initGLUFuncs(void)
{
    int     i,max;
    ulong   *p;

    _GLS_gluFuncs.dwSize = sizeof(GLS_gluFuncs);
    max = sizeof(GLS_gluFuncs)/sizeof(SGIGLU_initLibrary_t  );
    for (i = 0,p = (ulong*)&_GLS_gluFuncs; i < max; i++)
        *p++ = (ulong)_GLU_fatalErrorHandler;
}

/****************************************************************************
REMARKS:
Function to initialise the exported OpenGL functions for the OpenGL
implementation we have connected to. The mechanism we use here ensures that
we can support forwards and backwards binary compatibility.
****************************************************************************/
static void _GLS_initWGLFuncs(void)
{
    int     i,max;
    ulong   *p;

    _GLS_wglFuncs.dwSize = sizeof(GLS_wglFuncs);
    max = sizeof(GLS_wglFuncs)/sizeof(SGIGLU_initLibrary_t  );
    for (i = 0,p = (ulong*)&_GLS_wglFuncs; i < max; i++)
        *p++ = (ulong)_WGL_fatalErrorHandler;
}

/****************************************************************************
RETURNS:
True if OpenGL is loaded, false if not.

REMARKS:
Attempts to dynamically load the OpenGL implementation DLL and set our
vectors to point to the corresponding OpenGL functions.
{secret}
****************************************************************************/
int GLSAPI GLS_initExt(
    const char *GLName,
    int GLUType)
{
    int                     status;
    char                    GLUName[PM_MAX_PATH] = "";
    SGIGLU_initLibrary_t    SGIGLU_initLibrary;
    void                    *funcPtr;

    if (!_GLS_hModGL) {
        /* Initialise the function pointer tables with defaults */
        _GLS_initGLFuncs();
        _GLS_initGLUFuncs();
        _GLS_initWGLFuncs();

        /* Load the OpenGL libraries from disk */
        if (((_GLS_hModGL = LoadLibrary(GLName)) == NULL)) {
            status = glsErrGLNotFound;
            goto Error;
            }
        _GLS_hModGDI = GetModuleHandle(GDI32_DLL);
        IsGLDirect = stricmp(GLName,GLDIRECT_DLL) == 0;

        /* Get the address of regular OpenGL functions */
        GL_PROC_ADDRESS(glAccum);
        GL_PROC_ADDRESS(glAlphaFunc);
        GL_PROC_ADDRESS(glAreTexturesResident);
        GL_PROC_ADDRESS(glArrayElement);
        GL_PROC_ADDRESS(glBegin);
        GL_PROC_ADDRESS(glBindTexture);
        GL_PROC_ADDRESS(glBitmap);
        GL_PROC_ADDRESS(glBlendFunc);
        GL_PROC_ADDRESS(glCallList);
        GL_PROC_ADDRESS(glCallLists);
        GL_PROC_ADDRESS(glClear);
        GL_PROC_ADDRESS(glClearAccum);
        GL_PROC_ADDRESS(glClearColor);
        GL_PROC_ADDRESS(glClearDepth);
        GL_PROC_ADDRESS(glClearIndex);
        GL_PROC_ADDRESS(glClearStencil);
        GL_PROC_ADDRESS(glClipPlane);
        GL_PROC_ADDRESS(glColor3b);
        GL_PROC_ADDRESS(glColor3bv);
        GL_PROC_ADDRESS(glColor3d);
        GL_PROC_ADDRESS(glColor3dv);
        GL_PROC_ADDRESS(glColor3f);
        GL_PROC_ADDRESS(glColor3fv);
        GL_PROC_ADDRESS(glColor3i);
        GL_PROC_ADDRESS(glColor3iv);
        GL_PROC_ADDRESS(glColor3s);
        GL_PROC_ADDRESS(glColor3sv);
        GL_PROC_ADDRESS(glColor3ub);
        GL_PROC_ADDRESS(glColor3ubv);
        GL_PROC_ADDRESS(glColor3ui);
        GL_PROC_ADDRESS(glColor3uiv);
        GL_PROC_ADDRESS(glColor3us);
        GL_PROC_ADDRESS(glColor3usv);
        GL_PROC_ADDRESS(glColor4b);
        GL_PROC_ADDRESS(glColor4bv);
        GL_PROC_ADDRESS(glColor4d);
        GL_PROC_ADDRESS(glColor4dv);
        GL_PROC_ADDRESS(glColor4f);
        GL_PROC_ADDRESS(glColor4fv);
        GL_PROC_ADDRESS(glColor4i);
        GL_PROC_ADDRESS(glColor4iv);
        GL_PROC_ADDRESS(glColor4s);
        GL_PROC_ADDRESS(glColor4sv);
        GL_PROC_ADDRESS(glColor4ub);
        GL_PROC_ADDRESS(glColor4ubv);
        GL_PROC_ADDRESS(glColor4ui);
        GL_PROC_ADDRESS(glColor4uiv);
        GL_PROC_ADDRESS(glColor4us);
        GL_PROC_ADDRESS(glColor4usv);
        GL_PROC_ADDRESS(glColorMask);
        GL_PROC_ADDRESS(glColorMaterial);
        GL_PROC_ADDRESS(glColorPointer);
        GL_PROC_ADDRESS(glCopyPixels);
        GL_PROC_ADDRESS(glCopyTexImage1D);
        GL_PROC_ADDRESS(glCopyTexImage2D);
        GL_PROC_ADDRESS(glCopyTexSubImage1D);
        GL_PROC_ADDRESS(glCopyTexSubImage2D);
        GL_PROC_ADDRESS(glCullFace);
        GL_PROC_ADDRESS(glDeleteLists);
        GL_PROC_ADDRESS(glDeleteTextures);
        GL_PROC_ADDRESS(glDepthFunc);
        GL_PROC_ADDRESS(glDepthMask);
        GL_PROC_ADDRESS(glDepthRange);
        GL_PROC_ADDRESS(glDisable);
        GL_PROC_ADDRESS(glDisableClientState);
        GL_PROC_ADDRESS(glDrawArrays);
        GL_PROC_ADDRESS(glDrawBuffer);
        GL_PROC_ADDRESS(glDrawElements);
        GL_PROC_ADDRESS(glDrawPixels);
        GL_PROC_ADDRESS(glEdgeFlag);
        GL_PROC_ADDRESS(glEdgeFlagPointer);
        GL_PROC_ADDRESS(glEdgeFlagv);
        GL_PROC_ADDRESS(glEnable);
        GL_PROC_ADDRESS(glEnableClientState);
        GL_PROC_ADDRESS(glEnd);
        GL_PROC_ADDRESS(glEndList);
        GL_PROC_ADDRESS(glEvalCoord1d);
        GL_PROC_ADDRESS(glEvalCoord1dv);
        GL_PROC_ADDRESS(glEvalCoord1f);
        GL_PROC_ADDRESS(glEvalCoord1fv);
        GL_PROC_ADDRESS(glEvalCoord2d);
        GL_PROC_ADDRESS(glEvalCoord2dv);
        GL_PROC_ADDRESS(glEvalCoord2f);
        GL_PROC_ADDRESS(glEvalCoord2fv);
        GL_PROC_ADDRESS(glEvalMesh1);
        GL_PROC_ADDRESS(glEvalMesh2);
        GL_PROC_ADDRESS(glEvalPoint1);
        GL_PROC_ADDRESS(glEvalPoint2);
        GL_PROC_ADDRESS(glFeedbackBuffer);
        GL_PROC_ADDRESS(glFinish);
        GL_PROC_ADDRESS(glFlush);
        GL_PROC_ADDRESS(glFogf);
        GL_PROC_ADDRESS(glFogfv);
        GL_PROC_ADDRESS(glFogi);
        GL_PROC_ADDRESS(glFogiv);
        GL_PROC_ADDRESS(glFrontFace);
        GL_PROC_ADDRESS(glFrustum);
        GL_PROC_ADDRESS(glGenLists);
        GL_PROC_ADDRESS(glGenTextures);
        GL_PROC_ADDRESS(glGetBooleanv);
        GL_PROC_ADDRESS(glGetClipPlane);
        GL_PROC_ADDRESS(glGetDoublev);
        GL_PROC_ADDRESS(glGetError);
        GL_PROC_ADDRESS(glGetFloatv);
        GL_PROC_ADDRESS(glGetIntegerv);
        GL_PROC_ADDRESS(glGetLightfv);
        GL_PROC_ADDRESS(glGetLightiv);
        GL_PROC_ADDRESS(glGetMapdv);
        GL_PROC_ADDRESS(glGetMapfv);
        GL_PROC_ADDRESS(glGetMapiv);
        GL_PROC_ADDRESS(glGetMaterialfv);
        GL_PROC_ADDRESS(glGetMaterialiv);
        GL_PROC_ADDRESS(glGetPixelMapfv);
        GL_PROC_ADDRESS(glGetPixelMapuiv);
        GL_PROC_ADDRESS(glGetPixelMapusv);
        GL_PROC_ADDRESS(glGetPointerv);
        GL_PROC_ADDRESS(glGetPolygonStipple);
        GL_PROC_ADDRESS(glGetString);
        GL_PROC_ADDRESS(glGetTexEnvfv);
        GL_PROC_ADDRESS(glGetTexEnviv);
        GL_PROC_ADDRESS(glGetTexGendv);
        GL_PROC_ADDRESS(glGetTexGenfv);
        GL_PROC_ADDRESS(glGetTexGeniv);
        GL_PROC_ADDRESS(glGetTexImage);
        GL_PROC_ADDRESS(glGetTexLevelParameterfv);
        GL_PROC_ADDRESS(glGetTexLevelParameteriv);
        GL_PROC_ADDRESS(glGetTexParameterfv);
        GL_PROC_ADDRESS(glGetTexParameteriv);
        GL_PROC_ADDRESS(glHint);
        GL_PROC_ADDRESS(glIndexMask);
        GL_PROC_ADDRESS(glIndexPointer);
        GL_PROC_ADDRESS(glIndexd);
        GL_PROC_ADDRESS(glIndexdv);
        GL_PROC_ADDRESS(glIndexf);
        GL_PROC_ADDRESS(glIndexfv);
        GL_PROC_ADDRESS(glIndexi);
        GL_PROC_ADDRESS(glIndexiv);
        GL_PROC_ADDRESS(glIndexs);
        GL_PROC_ADDRESS(glIndexsv);
        GL_PROC_ADDRESS(glIndexub);
        GL_PROC_ADDRESS(glIndexubv);
        GL_PROC_ADDRESS(glInitNames);
        GL_PROC_ADDRESS(glInterleavedArrays);
        GL_PROC_ADDRESS(glIsEnabled);
        GL_PROC_ADDRESS(glIsList);
        GL_PROC_ADDRESS(glIsTexture);
        GL_PROC_ADDRESS(glLightModelf);
        GL_PROC_ADDRESS(glLightModelfv);
        GL_PROC_ADDRESS(glLightModeli);
        GL_PROC_ADDRESS(glLightModeliv);
        GL_PROC_ADDRESS(glLightf);
        GL_PROC_ADDRESS(glLightfv);
        GL_PROC_ADDRESS(glLighti);
        GL_PROC_ADDRESS(glLightiv);
        GL_PROC_ADDRESS(glLineStipple);
        GL_PROC_ADDRESS(glLineWidth);
        GL_PROC_ADDRESS(glListBase);
        GL_PROC_ADDRESS(glLoadIdentity);
        GL_PROC_ADDRESS(glLoadMatrixd);
        GL_PROC_ADDRESS(glLoadMatrixf);
        GL_PROC_ADDRESS(glLoadName);
        GL_PROC_ADDRESS(glLogicOp);
        GL_PROC_ADDRESS(glMap1d);
        GL_PROC_ADDRESS(glMap1f);
        GL_PROC_ADDRESS(glMap2d);
        GL_PROC_ADDRESS(glMap2f);
        GL_PROC_ADDRESS(glMapGrid1d);
        GL_PROC_ADDRESS(glMapGrid1f);
        GL_PROC_ADDRESS(glMapGrid2d);
        GL_PROC_ADDRESS(glMapGrid2f);
        GL_PROC_ADDRESS(glMaterialf);
        GL_PROC_ADDRESS(glMaterialfv);
        GL_PROC_ADDRESS(glMateriali);
        GL_PROC_ADDRESS(glMaterialiv);
        GL_PROC_ADDRESS(glMatrixMode);
        GL_PROC_ADDRESS(glMultMatrixd);
        GL_PROC_ADDRESS(glMultMatrixf);
        GL_PROC_ADDRESS(glNewList);
        GL_PROC_ADDRESS(glNormal3b);
        GL_PROC_ADDRESS(glNormal3bv);
        GL_PROC_ADDRESS(glNormal3d);
        GL_PROC_ADDRESS(glNormal3dv);
        GL_PROC_ADDRESS(glNormal3f);
        GL_PROC_ADDRESS(glNormal3fv);
        GL_PROC_ADDRESS(glNormal3i);
        GL_PROC_ADDRESS(glNormal3iv);
        GL_PROC_ADDRESS(glNormal3s);
        GL_PROC_ADDRESS(glNormal3sv);
        GL_PROC_ADDRESS(glNormalPointer);
        GL_PROC_ADDRESS(glOrtho);
        GL_PROC_ADDRESS(glPassThrough);
        GL_PROC_ADDRESS(glPixelMapfv);
        GL_PROC_ADDRESS(glPixelMapuiv);
        GL_PROC_ADDRESS(glPixelMapusv);
        GL_PROC_ADDRESS(glPixelStoref);
        GL_PROC_ADDRESS(glPixelStorei);
        GL_PROC_ADDRESS(glPixelTransferf);
        GL_PROC_ADDRESS(glPixelTransferi);
        GL_PROC_ADDRESS(glPixelZoom);
        GL_PROC_ADDRESS(glPointSize);
        GL_PROC_ADDRESS(glPolygonMode);
        GL_PROC_ADDRESS(glPolygonOffset);
        GL_PROC_ADDRESS(glPolygonStipple);
        GL_PROC_ADDRESS(glPopAttrib);
        GL_PROC_ADDRESS(glPopClientAttrib);
        GL_PROC_ADDRESS(glPopMatrix);
        GL_PROC_ADDRESS(glPopName);
        GL_PROC_ADDRESS(glPrioritizeTextures);
        GL_PROC_ADDRESS(glPushAttrib);
        GL_PROC_ADDRESS(glPushClientAttrib);
        GL_PROC_ADDRESS(glPushMatrix);
        GL_PROC_ADDRESS(glPushName);
        GL_PROC_ADDRESS(glRasterPos2d);
        GL_PROC_ADDRESS(glRasterPos2dv);
        GL_PROC_ADDRESS(glRasterPos2f);
        GL_PROC_ADDRESS(glRasterPos2fv);
        GL_PROC_ADDRESS(glRasterPos2i);
        GL_PROC_ADDRESS(glRasterPos2iv);
        GL_PROC_ADDRESS(glRasterPos2s);
        GL_PROC_ADDRESS(glRasterPos2sv);
        GL_PROC_ADDRESS(glRasterPos3d);
        GL_PROC_ADDRESS(glRasterPos3dv);
        GL_PROC_ADDRESS(glRasterPos3f);
        GL_PROC_ADDRESS(glRasterPos3fv);
        GL_PROC_ADDRESS(glRasterPos3i);
        GL_PROC_ADDRESS(glRasterPos3iv);
        GL_PROC_ADDRESS(glRasterPos3s);
        GL_PROC_ADDRESS(glRasterPos3sv);
        GL_PROC_ADDRESS(glRasterPos4d);
        GL_PROC_ADDRESS(glRasterPos4dv);
        GL_PROC_ADDRESS(glRasterPos4f);
        GL_PROC_ADDRESS(glRasterPos4fv);
        GL_PROC_ADDRESS(glRasterPos4i);
        GL_PROC_ADDRESS(glRasterPos4iv);
        GL_PROC_ADDRESS(glRasterPos4s);
        GL_PROC_ADDRESS(glRasterPos4sv);
        GL_PROC_ADDRESS(glReadBuffer);
        GL_PROC_ADDRESS(glReadPixels);
        GL_PROC_ADDRESS(glRectd);
        GL_PROC_ADDRESS(glRectdv);
        GL_PROC_ADDRESS(glRectf);
        GL_PROC_ADDRESS(glRectfv);
        GL_PROC_ADDRESS(glRecti);
        GL_PROC_ADDRESS(glRectiv);
        GL_PROC_ADDRESS(glRects);
        GL_PROC_ADDRESS(glRectsv);
        GL_PROC_ADDRESS(glRenderMode);
        GL_PROC_ADDRESS(glRotated);
        GL_PROC_ADDRESS(glRotatef);
        GL_PROC_ADDRESS(glScaled);
        GL_PROC_ADDRESS(glScalef);
        GL_PROC_ADDRESS(glScissor);
        GL_PROC_ADDRESS(glSelectBuffer);
        GL_PROC_ADDRESS(glShadeModel);
        GL_PROC_ADDRESS(glStencilFunc);
        GL_PROC_ADDRESS(glStencilMask);
        GL_PROC_ADDRESS(glStencilOp);
        GL_PROC_ADDRESS(glTexCoord1d);
        GL_PROC_ADDRESS(glTexCoord1dv);
        GL_PROC_ADDRESS(glTexCoord1f);
        GL_PROC_ADDRESS(glTexCoord1fv);
        GL_PROC_ADDRESS(glTexCoord1i);
        GL_PROC_ADDRESS(glTexCoord1iv);
        GL_PROC_ADDRESS(glTexCoord1s);
        GL_PROC_ADDRESS(glTexCoord1sv);
        GL_PROC_ADDRESS(glTexCoord2d);
        GL_PROC_ADDRESS(glTexCoord2dv);
        GL_PROC_ADDRESS(glTexCoord2f);
        GL_PROC_ADDRESS(glTexCoord2fv);
        GL_PROC_ADDRESS(glTexCoord2i);
        GL_PROC_ADDRESS(glTexCoord2iv);
        GL_PROC_ADDRESS(glTexCoord2s);
        GL_PROC_ADDRESS(glTexCoord2sv);
        GL_PROC_ADDRESS(glTexCoord3d);
        GL_PROC_ADDRESS(glTexCoord3dv);
        GL_PROC_ADDRESS(glTexCoord3f);
        GL_PROC_ADDRESS(glTexCoord3fv);
        GL_PROC_ADDRESS(glTexCoord3i);
        GL_PROC_ADDRESS(glTexCoord3iv);
        GL_PROC_ADDRESS(glTexCoord3s);
        GL_PROC_ADDRESS(glTexCoord3sv);
        GL_PROC_ADDRESS(glTexCoord4d);
        GL_PROC_ADDRESS(glTexCoord4dv);
        GL_PROC_ADDRESS(glTexCoord4f);
        GL_PROC_ADDRESS(glTexCoord4fv);
        GL_PROC_ADDRESS(glTexCoord4i);
        GL_PROC_ADDRESS(glTexCoord4iv);
        GL_PROC_ADDRESS(glTexCoord4s);
        GL_PROC_ADDRESS(glTexCoord4sv);
        GL_PROC_ADDRESS(glTexCoordPointer);
        GL_PROC_ADDRESS(glTexEnvf);
        GL_PROC_ADDRESS(glTexEnvfv);
        GL_PROC_ADDRESS(glTexEnvi);
        GL_PROC_ADDRESS(glTexEnviv);
        GL_PROC_ADDRESS(glTexGend);
        GL_PROC_ADDRESS(glTexGendv);
        GL_PROC_ADDRESS(glTexGenf);
        GL_PROC_ADDRESS(glTexGenfv);
        GL_PROC_ADDRESS(glTexGeni);
        GL_PROC_ADDRESS(glTexGeniv);
        GL_PROC_ADDRESS(glTexImage1D);
        GL_PROC_ADDRESS(glTexImage2D);
        GL_PROC_ADDRESS(glTexParameterf);
        GL_PROC_ADDRESS(glTexParameterfv);
        GL_PROC_ADDRESS(glTexParameteri);
        GL_PROC_ADDRESS(glTexParameteriv);
        GL_PROC_ADDRESS(glTexSubImage1D);
        GL_PROC_ADDRESS(glTexSubImage2D);
        GL_PROC_ADDRESS(glTranslated);
        GL_PROC_ADDRESS(glTranslatef);
        GL_PROC_ADDRESS(glVertex2d);
        GL_PROC_ADDRESS(glVertex2dv);
        GL_PROC_ADDRESS(glVertex2f);
        GL_PROC_ADDRESS(glVertex2fv);
        GL_PROC_ADDRESS(glVertex2i);
        GL_PROC_ADDRESS(glVertex2iv);
        GL_PROC_ADDRESS(glVertex2s);
        GL_PROC_ADDRESS(glVertex2sv);
        GL_PROC_ADDRESS(glVertex3d);
        GL_PROC_ADDRESS(glVertex3dv);
        GL_PROC_ADDRESS(glVertex3f);
        GL_PROC_ADDRESS(glVertex3fv);
        GL_PROC_ADDRESS(glVertex3i);
        GL_PROC_ADDRESS(glVertex3iv);
        GL_PROC_ADDRESS(glVertex3s);
        GL_PROC_ADDRESS(glVertex3sv);
        GL_PROC_ADDRESS(glVertex4d);
        GL_PROC_ADDRESS(glVertex4dv);
        GL_PROC_ADDRESS(glVertex4f);
        GL_PROC_ADDRESS(glVertex4fv);
        GL_PROC_ADDRESS(glVertex4i);
        GL_PROC_ADDRESS(glVertex4iv);
        GL_PROC_ADDRESS(glVertex4s);
        GL_PROC_ADDRESS(glVertex4sv);
        GL_PROC_ADDRESS(glVertexPointer);
        GL_PROC_ADDRESS(glViewport);

        /* All GDI functions */
        if (stricmp(GLName,MSGL_DLL) == 0) {
            GDI_PROC_ADDRESS(ChoosePixelFormat);
            GDI_PROC_ADDRESS(DescribePixelFormat);
            GDI_PROC_ADDRESS(GetPixelFormat);
            GDI_PROC_ADDRESS(SetPixelFormat);
            GDI_PROC_ADDRESS(SwapBuffers);
            }
        else {
            WGL_GDI_PROC_ADDRESS(ChoosePixelFormat);
            WGL_GDI_PROC_ADDRESS(DescribePixelFormat);
            WGL_GDI_PROC_ADDRESS(GetPixelFormat);
            WGL_GDI_PROC_ADDRESS(SetPixelFormat);
            WGL_GDI_PROC_ADDRESS(SwapBuffers);
            }

        /* All regular wgl functions */
        WGL_PROC_ADDRESS(CreateContext);
        WGL_PROC_ADDRESS(DeleteContext);
        WGL_PROC_ADDRESS(GetCurrentContext);
        WGL_PROC_ADDRESS(GetCurrentDC);
        WGL_PROC_ADDRESS(GetProcAddress);
        WGL_PROC_ADDRESS(MakeCurrent);
        WGL_PROC_ADDRESS(ShareLists);
        WGL_PROC_ADDRESS(UseFontBitmapsA);
        WGL_PROC_ADDRESS(UseFontBitmapsW);
        WGL_PROC_ADDRESS(UseFontOutlinesA);
        WGL_PROC_ADDRESS(UseFontOutlinesW);

        /* Load the appropriate GLU library if requested */
        if (GLUType != glsGLUNone) {
            if (GLUType == glsGLUAuto && stricmp(GLName,MSGL_DLL) == 0)
                strcpy(GLUName,MSGLU_DLL);
            else
                strcpy(GLUName,SGIGLU_DLL);
            if (((_GLS_hModGLU = LoadLibrary(GLUName)) == NULL)) {
                status = glsErrGLUNotFound;
                goto Error;
                }

            /* Initialise the SGI GLU library as necessary, and pass down
             * a copy of all the OpenGL rendering vectors.
             */
            if (stricmp(GLUName,SGIGLU_DLL) == 0) {
                if ((SGIGLU_initLibrary = (SGIGLU_initLibrary_t)GetProcAddress(_GLS_hModGLU,"gluInitMGL")) == NULL)
                    FatalError("Unable to init SGIGLU!");
                SGIGLU_initLibrary(&_GLS_glFuncs);
                }

            /* OpenGL GLU functions */
            GLU_PROC_ADDRESS(gluBeginCurve);
            GLU_PROC_ADDRESS(gluBeginPolygon);
            GLU_PROC_ADDRESS(gluBeginSurface);
            GLU_PROC_ADDRESS(gluBeginTrim);
            GLU_PROC_ADDRESS(gluBuild1DMipmaps);
            GLU_PROC_ADDRESS(gluBuild2DMipmaps);
            GLU_PROC_ADDRESS(gluCylinder);
            GLU_PROC_ADDRESS(gluDeleteNurbsRenderer);
            GLU_PROC_ADDRESS(gluDeleteQuadric);
            GLU_PROC_ADDRESS(gluDeleteTess);
            GLU_PROC_ADDRESS(gluDisk);
            GLU_PROC_ADDRESS(gluEndCurve);
            GLU_PROC_ADDRESS(gluEndPolygon);
            GLU_PROC_ADDRESS(gluEndSurface);
            GLU_PROC_ADDRESS(gluEndTrim);
            GLU_PROC_ADDRESS(gluErrorString);
            GLU_PROC_ADDRESS(gluGetNurbsProperty);
            GLU_PROC_ADDRESS(gluGetString);
            GLU_PROC_ADDRESS(gluGetTessProperty);
            GLU_PROC_ADDRESS(gluLoadSamplingMatrices);
            GLU_PROC_ADDRESS(gluLookAt);
            GLU_PROC_ADDRESS(gluNewNurbsRenderer);
            GLU_PROC_ADDRESS(gluNewQuadric);
            GLU_PROC_ADDRESS(gluNewTess);
            GLU_PROC_ADDRESS(gluNextContour);
            GLU_PROC_ADDRESS(gluNurbsCallback);
            GLU_PROC_ADDRESS(gluNurbsCurve);
            GLU_PROC_ADDRESS(gluNurbsProperty);
            GLU_PROC_ADDRESS(gluNurbsSurface);
            GLU_PROC_ADDRESS(gluOrtho2D);
            GLU_PROC_ADDRESS(gluPartialDisk);
            GLU_PROC_ADDRESS(gluPerspective);
            GLU_PROC_ADDRESS(gluPickMatrix);
            GLU_PROC_ADDRESS(gluProject);
            GLU_PROC_ADDRESS(gluPwlCurve);
            GLU_PROC_ADDRESS(gluQuadricCallback);
            GLU_PROC_ADDRESS(gluQuadricDrawStyle);
            GLU_PROC_ADDRESS(gluQuadricNormals);
            GLU_PROC_ADDRESS(gluQuadricOrientation);
            GLU_PROC_ADDRESS(gluQuadricTexture);
            GLU_PROC_ADDRESS(gluScaleImage);
            GLU_PROC_ADDRESS(gluSphere);
            GLU_PROC_ADDRESS(gluTessBeginContour);
            GLU_PROC_ADDRESS(gluTessBeginPolygon);
            GLU_PROC_ADDRESS(gluTessCallback);
            GLU_PROC_ADDRESS(gluTessEndContour);
            GLU_PROC_ADDRESS(gluTessEndPolygon);
            GLU_PROC_ADDRESS(gluTessNormal);
            GLU_PROC_ADDRESS(gluTessProperty);
            GLU_PROC_ADDRESS(gluTessVertex);
            GLU_PROC_ADDRESS(gluUnProject);
            }
        }
    return glsOK;

Error:
    GLS_exit();
    return status;
}

/****************************************************************************
REMARKS:
Simple function to initialise the library based on environment variables.
****************************************************************************/
void GLSAPI GLS_init(
    const uchar *OemLicense)
{
    char    GLName[40] = GLDIRECT_DLL;
    char    *env;
    int     status;

    if ((env = getenv(GLS_OPENGL)) != NULL)
        strcpy(GLName,env);
    if (GLS_initExt(GLName,glsGLUAuto) != glsOK)
        FatalError("Unable to load OpenGL libraries!");
    status = GLS_registerLicense(OemLicense);
    if (status != glsOK && status != glsErrNoGLDirect)
        FatalError("Unable to register GLDirect license file!");
}

/****************************************************************************
REMARKS:
Registers the license file with GLDirect.
****************************************************************************/
int GLSAPI GLS_registerLicense(
    const uchar *OemLicense)
{
    DGL_registerLicense_t pDGL_registerLicense;

    /* Access the DGL_registerLicense export function by name */
    pDGL_registerLicense = (DGL_registerLicense_t)GetProcAddress(_GLS_hModGL, "DGL_registerLicense");
    if (pDGL_registerLicense == NULL)
        return glsErrNoGLDirect;

    /* Now try to register the license */
    if (!pDGL_registerLicense(OemLicense))
        return glsErrInvalidLicense;
    return glsOK;
}

/****************************************************************************
REMARKS:
Unloads the currently loaded OpenGL library.
****************************************************************************/
void GLS_exit(void)
{
    if (_GLS_hModGL) FreeLibrary(_GLS_hModGL);
    if (_GLS_hModGLU) FreeLibrary(_GLS_hModGLU);
    _GLS_hModGL = _GLS_hModGLU = NULL;
}

/****************************************************************************
DESCRIPTION:
Checks for Microsoft OpenGL hardware acceleration.

HEADER:
glswitch.h

RETURNS:
True if Microsoft OpenGL is hardware accelerated on the target platform.

REMARKS:
This function will load the Microsoft OpenGL libraries and attempt to
determine if hardware acceleration is available or not. This involves
loading the Microsoft OpenGL libraries and checking to see if OpenGL is
accelerated via an ICD or MCD hardware device driver.
****************************************************************************/
ibool GLSAPI GLS_haveHWOpenGL(void)
{
    HDC         hdc;
    int         (WINAPI *Describe)(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
    int         i,count;
    HMODULE     hModGL;
    static int  hwOpenGL = -1;

    if (hwOpenGL != -1)
        return hwOpenGL;
    hwOpenGL = false;
    if (((hModGL = LoadLibrary(MSGL_DLL)) != NULL) && (Describe = (void*)GetProcAddress(hModGL,"wglDescribePixelFormat")) != NULL) {
        hdc = GetDC(NULL);
        count = Describe(hdc, 0, 0, NULL);
        for (i = 1; i <= count; i++) {
            PIXELFORMATDESCRIPTOR pfd;
            if (Describe(hdc, i, sizeof(pfd), &pfd)) {
                BOOL genericFormat = (pfd.dwFlags & PFD_GENERIC_FORMAT) != 0;
                BOOL genericAccel = (pfd.dwFlags & _PFD_GENERIC_ACCELERATED) != 0;
                /* Determine hardware driver status by the combination of these
                 * two flags. Both FALSE means ICD, both TRUE means MCD.
                 */
                if (genericFormat == genericAccel) {
                    hwOpenGL = true;
                    break;
                    }
                }
            }
        ReleaseDC(NULL, hdc);
        FreeLibrary(hModGL);
        }
    return hwOpenGL;
}
