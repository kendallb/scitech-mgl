/****************************************************************************
*
*                       MegaGraph Graphics Library
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
* Environment:  IBM PC (MS DOS)
*
* Description:  Header file for the MGLUNIX bindings for UNIX-like systems.
*
****************************************************************************/

#ifndef __MGLUNIX_H
#define __MGLUNIX_H

#ifndef MGLLINUX
#define MGLLINUX
#endif

#ifdef __X11__

#ifndef MGLX11
#define MGLX11
#endif

#include <X11/Xlib.h>
/* {secret} */
typedef GC           MGL_HDC;
/* {secret} */
typedef Window      MGL_HWND;

typedef XImage      *MGL_HBITMAP;

/* {secret} */
typedef Colormap    MGL_HPALETTE;

#else

/* {secret} */
typedef void*       MGL_HDC;
/* {secret} */
typedef void*       MGL_HWND;

typedef void*       MGL_HBITMAP;

/* {secret} */
typedef void*       MGL_HPALETTE;

#endif

/* {secret} */
typedef void        *MGL_HINSTANCE;
/* {secret} */
typedef void        *MGL_HGLRC;
/* {secret} */
typedef long        (* MGL_WNDPROC)(MGL_HWND, uint, uint, long);

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

MGLDC * MGLAPI MGL_createWindowedDC(
    MGL_HWND hwnd);

#ifdef __X11__
void MGLAPI MGL_setX11Display(Display *dpy);
#endif

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __MGLUNIX_H */
