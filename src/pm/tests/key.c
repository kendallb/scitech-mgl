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
* Description:  Test program to check the ability to install a C based
*               keyboard interrupt handler.
*
*               Functions tested:   PM_setKeyHandler()
*                                   PM_chainPrevKey()
*                                   PM_restoreKeyHandler()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pmapi.h"

volatile long count = 0;

#pragma off (check_stack)           /* No stack checking under Watcom   */

void PMAPI keyHandler(void)
{
    count++;
    PM_chainPrevKey();  /* Chain to previous handler    */
}

int main(void)
{
    int             ch;
    PM_lockHandle   lh;

    /* Install our timer handler and lock handler pages in memory. It is
     * difficult to get the size of a function in C, but we know our
     * function is well less than 100 bytes (and an entire 4k page will
     * need to be locked by the server anyway).
     */
    PM_lockCodePages((__codePtr)keyHandler,100,&lh);
    PM_lockDataPages((void*)&count,sizeof(count),&lh);
    PM_installBreakHandler();           /* We *DONT* want Ctrl-Breaks! */
    PM_setKeyHandler(keyHandler);
    printf("Keyboard interrupt handler installed - Type some characters and\n");
    printf("hit ESC to exit\n");
    while ((ch = PM_getch()) != 0x1B) {
        printf("%c", ch);
        fflush(stdout);
        }

    PM_restoreKeyHandler();
    PM_restoreBreakHandler();
    PM_unlockDataPages((void*)&count,sizeof(count),&lh);
    PM_unlockCodePages((__codePtr)keyHandler,100,&lh);
    printf("\n\nKeyboard handler was called %ld times\n", count);
    return 0;
}
