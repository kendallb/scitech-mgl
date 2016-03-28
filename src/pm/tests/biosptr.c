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
* Environment:  any
*
* Description:  Test program to check the ability to manipulate the
*               BIOS data area from protected mode using the PM
*               library. Compile and link with the appropriate command
*               line for your DOS extender.
*
*               Functions tested:   PM_getBIOSSelector()
*                                   PM_getLong()
*                                   PM_getByte()
*                                   PM_getWord()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pmapi.h"

/* Macros to obtain values from the BIOS data area */

#define TICKS()     PM_getLong(bios+0x6C)
#define KB_STAT     PM_getByte(bios+0x17)
#define KB_HEAD     PM_getWord(bios+0x1A)
#define KB_TAIL     PM_getWord(bios+0x1C)

/* Macros for working with the keyboard buffer */

#define KB_HIT()    (KB_HEAD != KB_TAIL)
#define CTRL()      (KB_STAT & 4)
#define SHIFT()     (KB_STAT & 2)
#define ESC         0x1B

/* Selector for BIOS data area */

uchar *bios;

int main(void)
{
    int c,done = 0;

    bios = PM_getBIOSPointer();
    printf("Hit any key to test, Ctrl-Shift-Esc to quit\n");
    while (!done) {
        if (KB_HIT()) {
            c = PM_getch();
            if (c == 0) PM_getch();
            printf("TIME=%-8lX ST=%02X CHAR=%02X ", TICKS(), KB_STAT, c);
            printf("\n");
            if ((c == ESC) && SHIFT() && CTRL())/* Ctrl-Shift-Esc */
                break;
            }
        }

    return 0;
}
