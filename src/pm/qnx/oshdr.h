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
* Environment:  QNX
*
* Description:  Include file to include all OS specific header files.
*
****************************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <process.h>
#include <time.h>
#ifndef __QNXNTO__
#include <sys/mouse.h>
#include <sys/keyboard.h>
#include <sys/fd.h>
#include <conio.h>
#else
#include <sys/neutrino.h>
#include <sys/dcmd_input.h>

/* 
 * Original QNX Neutrino port does not work with v6.3.0 SDK. Some event handling routines
 * will not run properly due to device name changes, or in some cases do not even compile. 
 * We use the following conditional define to skip compiling these routines, however
 * the actual Neutrino version number of the originally ported code is unknown.
 */
 
#define __OLD_NTO__     (_NTO_VERSION < 630)
 
/* Things 'borrowed' from photon/keycodes.h */

/*
 * Keyboard modifiers
 */
#define KEYMODBIT_SHIFT                                         0
#define KEYMODBIT_CTRL                                          1
#define KEYMODBIT_ALT                                           2
#define KEYMODBIT_ALTGR                                         3
#define KEYMODBIT_SHL3                                          4
#define KEYMODBIT_MOD6                                          5
#define KEYMODBIT_MOD7                                          6
#define KEYMODBIT_MOD8                                          7

#define KEYMODBIT_SHIFT_LOCK                                    8
#define KEYMODBIT_CTRL_LOCK                                     9
#define KEYMODBIT_ALT_LOCK                                      10
#define KEYMODBIT_ALTGR_LOCK                                    11
#define KEYMODBIT_SHL3_LOCK                                     12
#define KEYMODBIT_MOD6_LOCK                                     13
#define KEYMODBIT_MOD7_LOCK                                     14
#define KEYMODBIT_MOD8_LOCK                                     15

#define KEYMODBIT_CAPS_LOCK                                     16
#define KEYMODBIT_NUM_LOCK                                      17
#define KEYMODBIT_SCROLL_LOCK                                   18

#define KEYMOD_SHIFT                                            (1 << KEYMODBIT_SHIFT)
#define KEYMOD_CTRL                                             (1 << KEYMODBIT_CTRL)
#define KEYMOD_ALT                                              (1 << KEYMODBIT_ALT)
#define KEYMOD_ALTGR                                            (1 << KEYMODBIT_ALTGR)
#define KEYMOD_SHL3                                             (1 << KEYMODBIT_SHL3)
#define KEYMOD_MOD6                                             (1 << KEYMODBIT_MOD6)
#define KEYMOD_MOD7                                             (1 << KEYMODBIT_MOD7)
#define KEYMOD_MOD8                                             (1 << KEYMODBIT_MOD8)

#define KEYMOD_SHIFT_LOCK                                       (1 << KEYMODBIT_SHIFT_LOCK)
#define KEYMOD_CTRL_LOCK                                        (1 << KEYMODBIT_CTRL_LOCK)
#define KEYMOD_ALT_LOCK                                         (1 << KEYMODBIT_ALT_LOCK)
#define KEYMOD_ALTGR_LOCK                                       (1 << KEYMODBIT_ALTGR_LOCK)
#define KEYMOD_SHL3_LOCK                                        (1 << KEYMODBIT_SHL3_LOCK)
#define KEYMOD_MOD6_LOCK                                        (1 << KEYMODBIT_MOD6_LOCK)
#define KEYMOD_MOD7_LOCK                                        (1 << KEYMODBIT_MOD7_LOCK)
#define KEYMOD_MOD8_LOCK                                        (1 << KEYMODBIT_MOD8_LOCK)

#define KEYMOD_CAPS_LOCK                                        (1 << KEYMODBIT_CAPS_LOCK)
#define KEYMOD_NUM_LOCK                                         (1 << KEYMODBIT_NUM_LOCK)
#define KEYMOD_SCROLL_LOCK                                      (1 << KEYMODBIT_SCROLL_LOCK)

/*
 * Keyboard flags
 */
#define KEY_DOWN                                                0x00000001      /* Key was pressed down */
#define KEY_REPEAT                                              0x00000002      /* Key was repeated */
#define KEY_SCAN_VALID                                          0x00000020      /* Scancode is valid */
#define KEY_SYM_VALID                                           0x00000040      /* Key symbol is valid */
#define KEY_CAP_VALID                                           0x00000080      /* Key cap is valid */
#define KEY_DEAD                                                0x40000000      /* Key symbol is a DEAD key */
#define KEY_OEM_CAP                                             0x80000000      /* Key cap is an OEM scan code from keyboard */

#endif  /* __QNXNTO__ */
