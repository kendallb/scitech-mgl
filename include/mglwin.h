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
* Description:  Header file for the MGL bindings for Microsoft Windows using
*               DirectX 6.0 for windowed and for full screen modes.
*
****************************************************************************/

#ifndef __MGLWIN_H
#define __MGLWIN_H

#ifndef MGLWIN
#define MGLWIN
#endif

/*---------------------- Macros and type definitions ----------------------*/

#ifdef  DECLARE_HANDLE
/* {secret} */
typedef HDC         MGL_HDC;
/* {secret} */
typedef HINSTANCE   MGL_HINSTANCE;
/* {secret} */
typedef HPALETTE    MGL_HPALETTE;
/* {secret} */
typedef HGLRC       MGL_HGLRC;
/* {secret} */
typedef WNDPROC     MGL_WNDPROC;
#else
/* {secret} */
typedef void        *MGL_HDC;
/* {secret} */
typedef void        *MGL_HINSTANCE;
/* {secret} */
typedef void        *MGL_HPALETTE;
/* {secret} */
typedef void        *MGL_HGLRC;
/* {secret} */
typedef long        (__stdcall* MGL_WNDPROC)(MGL_HWND, uint, uint, long);
#endif
#ifdef  __DDRAW_INCLUDED__
/* {secret} */
typedef LPDIRECTDRAW4           MGL_LPDD;
/* {secret} */
typedef LPDIRECTDRAWSURFACE4    MGL_LPDDSURF;
/* {secret} */
typedef LPDIRECTDRAWPALETTE     MGL_LPDDPAL;
/* {secret} */
typedef LPDIRECTDRAWCLIPPER     MGL_LPDDCLIP;
#else
/* {secret} */
typedef void        *MGL_LPDD;
/* {secret} */
typedef void        *MGL_LPDDSURF;
/* {secret} */
typedef void        *MGL_LPDDPAL;
/* {secret} */
typedef void        *MGL_LPDDCLIP;
#endif

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Function to register a fullscreen window with the MGL. If you wish
 * for the MGL to use your own window for fullscreen modes, you can
 * register it with this function. Note that when the MGL goes into
 * fullscreen modes, the attributes, size and position of the window are
 * modified to make it into a fullscreen Window necessary to cover the
 * entire desktop, and the state of the window will be restore to the
 * original format on return to normal GDI mode.
 *
 * Note that if you are using a common window for Windowed mode and
 * fullscreen modes of your application, you will need to ensure that
 * certain messages that you window normally handles in windowed modes are
 * ignored when in fullscreen modes.
 */

void    MGLAPI MGL_registerFullScreenWindow(MGL_HWND hwndFullScreen);

/* Obtain the handle to the MGL fullscreen window when in fullscreen modes */

MGL_HWND MGLAPI MGL_getFullScreenWindow(void);

/* Function to register a fullscreen event handling window procedure.
 * If you wish to do your own event handling, you can register your window
 * procedure with the MGL using this function and it will be called
 * when there are messages to be handled. You can still call the MGL_event()
 * functions even if you have registered an event handling procedure.
 */

void    MGLAPI MGL_registerEventProc(MGL_WNDPROC userWndProc);

/* Associate a Window manager DC with the MGLDC for painting */

ibool   MGLAPI MGL_beginPaint(MGLDC *dc,MGL_HDC hdc);

/* Clean up after painting */

void    MGLAPI MGL_endPaint(MGLDC *dc);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __MGLWIN_H */

