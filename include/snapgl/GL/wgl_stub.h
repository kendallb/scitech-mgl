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
* Description:  Header describing the function stubs for calling OpenGL
*               wgl API functions.
*
****************************************************************************/

#ifndef __WGL_STUB_H
#define __WGL_STUB_H

/*--------------------- Macros and type definitions -----------------------*/

/* Structure containing all public Win32 wgl functions supported */

#pragma pack(1)
typedef struct {
    long        dwSize;

    /* OpenGL GDI32 functions */
    int     (APIENTRYP glsChoosePixelFormat)(HDC, CONST PIXELFORMATDESCRIPTOR *);
    int     (APIENTRYP glsDescribePixelFormat)(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
    int     (APIENTRYP glsGetPixelFormat)(HDC);
    BOOL    (APIENTRYP glsSetPixelFormat)(HDC, int, CONST PIXELFORMATDESCRIPTOR *);
    BOOL    (APIENTRYP glsSwapBuffers)(HDC);

    /* OpenGL wgl functions */
    HGLRC   (APIENTRYP glsCreateContext)(HDC);
    BOOL    (APIENTRYP glsDeleteContext)(HGLRC);
    HGLRC   (APIENTRYP glsGetCurrentContext)(VOID);
    HDC     (APIENTRYP glsGetCurrentDC)(VOID);
    PROC    (APIENTRYP glsGetProcAddress)(LPCSTR);
    BOOL    (APIENTRYP glsMakeCurrent)(HDC, HGLRC);
    BOOL    (APIENTRYP glsShareLists)(HGLRC, HGLRC);
    BOOL    (APIENTRYP glsUseFontBitmapsA)(HDC, DWORD, DWORD, DWORD);
    BOOL    (APIENTRYP glsUseFontBitmapsW)(HDC, DWORD, DWORD, DWORD);
    BOOL    (APIENTRYP glsUseFontOutlinesA)(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
    BOOL    (APIENTRYP glsUseFontOutlinesW)(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
    } GLS_wglFuncs;
#pragma pack()

/* Macros to override regular Win32 OpenGL bindings to call the gls
 * versions. Note that the macros must *always* be used, since we cannot
 * export functions named ChoosePixelFormat etc or there will be a link
 * time conflict with the exports from GDI32.DLL.
 */

#if !defined(NO_WGL_MACROS)

#define ChoosePixelFormat(a,b)                  _GLS_wglFuncs.glsChoosePixelFormat(a,b)
#define DescribePixelFormat(a,b,c,d)            _GLS_wglFuncs.glsDescribePixelFormat(a,b,c,d)
#define GetPixelFormat(a)                       _GLS_wglFuncs.glsGetPixelFormat(a)
#define SetPixelFormat(a,b,c)                   _GLS_wglFuncs.glsSetPixelFormat(a,b,c)
#define SwapBuffers(a)                          _GLS_wglFuncs.glsSwapBuffers(a)

#define wglCreateContext(a)                     _GLS_wglFuncs.glsCreateContext(a)
#define wglDeleteContext(a)                     _GLS_wglFuncs.glsDeleteContext(a)
#define wglGetCurrentContext()                  _GLS_wglFuncs.glsGetCurrentContext()
#define wglGetCurrentDC()                       _GLS_wglFuncs.glsGetCurrentDC()
#define wglGetProcAddress(a)                    _GLS_wglFuncs.glsGetProcAddress(a)
#define wglMakeCurrent(a,b)                     _GLS_wglFuncs.glsMakeCurrent(a,b)
#define wglShareLists(a,b)                      _GLS_wglFuncs.glsShareLists(a,b)
#define wglUseFontBitmapsA(a,b,c,d)             _GLS_wglFuncs.glsUseFontBitmapsA(a,b,c,d)
#define wglUseFontBitmapsW(a,b,c,d)             _GLS_wglFuncs.glsUseFontBitmapsW(a,b,c,d)
#define wglUseFontOutlinesA(a,b,c,d,e,f,g,h)    _GLS_wglFuncs.glsUseFontOutlinesA(a,b,c,d,e,f,g,h)
#define wglUseFontOutlinesW(a,b,c,d,e,f,g,h)    _GLS_wglFuncs.glsUseFontOutlinesW(a,b,c,d,e,f,g,h)

#endif

/*---------------------------- Global variables ---------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern GLS_wglFuncs  _VARAPI _GLS_wglFuncs;

#ifdef __cplusplus
}
#endif

#endif /* __WGL_STUB_H */
