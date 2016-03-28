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
* Environment:  OS/2 32-bit
*
* Description:  Header file for the OS/2 bindings for IBM OS/2 Warp using
*               DIVE in a windowed and full screen modes. Note that you need
*               to link with a different library to support drawing in a
*               window and for fullscreen console programs. Running in a
*               Window requires DIVE, while running in a fullscreen console
*               simply requires any VGA/VESA compatible display adapter
*               and the GENPMI.DLL driver (from IBM's GENGRADD display
*               drivers)
*
****************************************************************************/

#ifndef __MGLOS2_H
#define __MGLOS2_H

#ifndef MGLOS2
#define MGLOS2
#endif

/*---------------------- Macros and type definitions ----------------------*/

/* {secret} */
typedef void        *MGL_HDC;
/* {secret} */
typedef void        *MGL_HINSTANCE;
/* {secret} */
typedef void        *MGL_HPALETTE;
/* {secret} */
typedef void        *MGL_HGLRC;
/* {secret} */
typedef long        (*MGL_WNDPROC)(MGL_HWND, uint, uint, long);
/* {secret} */
typedef ibool       (*_MGL_userEventFilter)(event_t *evt);

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Function to register a fullscreen window with the MGL. If you wish
 * for the MGL to use your own window for fullscreen modes, you can
 * register it with this function. Note that when the MGL goes into
 * fullscreen modes, the attributes, size and position of the window are
 * modified to make it into a fullscreen Window necessary to cover the
 * entire desktop, and the state of the window will be restore to the original
 * format on return to normal GDI mode.
 *
 * Note that if you are using a common window for Windowed mode and fullscreen
 * modes of your application, you will need to ensure that certain messages
 * that you window normally handles in windowed modes are ignored when in
 * fullscreen modes.
 */

void    MGLAPI MGL_registerFullScreenWindow(MGL_HWND hwndFullScreen);

/* Obtain the handle to the MGL fullscreen window when in fullscreen modes */

MGL_HWND MGLAPI MGL_getFullScreenWindow(void);

/* Tell the MGL what your applications main window is */

void    MGLAPI MGL_setMainWindow(MGL_HWND hwnd);

/* Tell the MGL your applications instance handle (call before all funcs!) */

void    MGLAPI MGL_setAppInstance(MGL_HINSTANCE hInstApp);

/* Function to register a fullscreen event handling window procedure.
 * If you wish to do your own event handling, you can register your window
 * procedure with the MGL using this function and it will be called
 * when there are messages to be handled. You can still call the MGL_event()
 * functions even if you have registered an event handling procedure.
 */

void    MGLAPI MGL_registerEventProc(MGL_WNDPROC userWndProc);

/* Device context management */

MGLDC   * MGLAPI MGL_createWindowedDC(MGL_HWND hwnd);

/* Activate the WindowDC's palette */

ibool   MGLAPI MGL_activatePalette(MGLDC *dc,ibool unrealize);

/* Let the MGL know when your application is being activated or deactivated.
 * This function only needs to be called when running in Windowed modes and
 * you have set the system palette to SYSPAL_NOSTATIC mode, to ensure
 * that the MGL can properly re-map your application palette when your
 * app is not active and allow Windows to re-map your bitmap colors on the
 * fly. This function should be passed a pointer to the currently active
 * MGL Windowed DC and a flag to indicate whether the app is in the background
 * or not.
 */

void    MGLAPI MGL_appActivate(MGLDC *winDC,ibool active);

/* Install user supplied event filter callback */

void    MGLAPI MGL_setUserEventFilter(_MGL_userEventFilter filter);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __MGLOS2_H */
