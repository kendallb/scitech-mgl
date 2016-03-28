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
* Environment:  Linux
*
* Description:  Include all the OS specific header files.
*
****************************************************************************/

#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/keyboard.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/kdev_t.h>
#include <linux/joystick.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sched.h>
#ifndef NO_PTHREAD
#include <pthread.h>
#endif

/* Global structure for BIOS emulator callbacks */

extern PM_int86Funcs __PM_biosFuncs;

/* Internal global variables */

extern int              _PM_console_fd,_PM_leds,_PM_modifiers;
extern volatile void    *_PM_ioBase;

/* Internal function prototypes */

void _PM_restore_kb_mode(void);
void _PM_keyboard_rawmode(void);

/* Internal functions to lock/unlock event semaphore */

int _PM_lockEventQueue(void);
void _PM_unlockEventQueue(void);

/* Size of stack for our local threads */

#define THREAD_STACK_SIZE   8192

/* Linux needs the generic joystick scaling code */

#define NEED_SCALE_JOY_AXIS

/* Linux has kernel provided joystick functions we can use */

#define USE_OS_JOYSTICK

