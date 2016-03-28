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
* Description:  Test program to check the ability to install an assembly
*               language mouse interrupt handler. We use assembly language
*               as it must be a far function and should swap to a local
*               32 bit stack if it is going to call any C based code (which
*               we do in this example).
*
*               Functions tested:   PM_installMouseHandler()
*                                   PM_int86()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pmapi.h"

volatile long count = 0;

#pragma off (check_stack)           /* No stack checking under Watcom   */

void PMAPI mouseHandler(
    uint mask,
    uint butstate,
    int x,
    int y,
    int mickeyX,
    int mickeyY)
{
    mask = mask;                /* We dont use any of the parameters    */
    butstate = butstate;
    x = x;
    y = y;
    mickeyX = mickeyX;
    mickeyY = mickeyY;
    count++;
}

int main(void)
{
    RMREGS          regs;
    PM_lockHandle   lh;

    regs.x.ax = 33;     /* Mouse function 33 - Software reset       */
    PM_int86(0x33,&regs,&regs);
    if (regs.x.bx == 0) {
        printf("No mouse installed.\n");
        exit(1);
        }

    /* Install our mouse handler and lock handler pages in memory. It is
     * difficult to get the size of a function in C, but we know our
     * function is well less than 100 bytes (and an entire 4k page will
     * need to be locked by the server anyway).
     */
    PM_lockCodePages((__codePtr)mouseHandler,100,&lh);
    PM_lockDataPages((void*)&count,sizeof(count),&lh);
    if (!PM_setMouseHandler(0xFFFF, mouseHandler)) {
        printf("Unable to install mouse handler!\n");
        exit(1);
        }
    printf("Mouse handler installed - Hit any key to exit\n");
    PM_getch();

    PM_restoreMouseHandler();
    PM_unlockDataPages((void*)&count,sizeof(count),&lh);
    PM_unlockCodePages((__codePtr)mouseHandler,100,&lh);
    printf("Mouse handler was called %ld times\n", count);
    return 0;
}
