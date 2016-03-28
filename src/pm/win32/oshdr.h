/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Description:  Include file to include all OS specific header files.
*
****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <float.h>
#define NONAMELESSUNION
#include "pm/ddraw.h"

/* Macros to save and restore the default control word. Windows 9x has
 * some bugs in it such that calls to load any DLL's which load 16-bit
 * DLL's cause the floating point control word to get trashed. We fix
 * this by saving and restoring the control word across problematic
 * calls.
 */

#if defined(__INTEL__) && !defined(__GNUC__)
#define GET_DEFAULT_CW()                    \
{                                           \
    if (_PM_cw_default == 0)                \
        _PM_cw_default = _control87(0,0);   \
}
#define RESET_DEFAULT_CW()                  \
    _control87(_PM_cw_default,0xFFFFFFFF)
#else
#define GET_DEFAULT_CW()
#define RESET_DEFAULT_CW()
#endif

/* Custom window messages */

#define	WM_DO_SUSPEND_APP			WM_USER
#define	WM_PM_LEAVE_FULLSCREEN	    0
#define	WM_PM_RESTORE_FULLSCREEN	1

/* Macro for disabling AutoPlay on a use system */

#define AUTOPLAY_DRIVE_CDROM    0x20

/*--------------------------- Global Variables ----------------------------*/

#ifdef  __INTEL__
extern uint     _PM_cw_default;         /* Default FPU control word     */
#endif
extern int      _PM_deskX,_PM_deskY;    /* Desktop dimensions           */
extern HWND     _PM_hwndConsole;        /* Window handle for console    */
extern ibool    _PM_backInGDI;          /* Flags if GDI desktop         */

/*-------------------------- Internal Functions ---------------------------*/

void _EVT_pumpMessages(void);

