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
* Environment:  Linux/QNX
*
* Description:  Program to save the console state state so that it can
*               be later restored if the program crashed while the console
*               was in graphics mode.
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pmapi.h"

int main(void)
{
    PM_HWND hwndConsole;
    ulong   stateSize;
    void    *stateBuf;
    FILE    *f;

    /* Allocate a buffer to save console state and save the state */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    stateSize = PM_getConsoleStateSize();
    if ((stateBuf = PM_malloc(stateSize)) == NULL) {
        PM_closeConsole(hwndConsole);
        printf("Unable to allocate console state buffer!\n");
        return -1;
        }
    PM_saveConsoleState(stateBuf,0);

    /* Restore the console state on exit */
    PM_restoreConsoleState(stateBuf,0);
    PM_closeConsole(hwndConsole);

    /* Write the saved console state buffer to disk */
    if ((f = fopen("/etc/pmsave.dat","wb")) == NULL)
        printf("Unable to open /etc/pmsave.dat for writing!\n");
    else {
        fwrite(&stateSize,1,sizeof(stateSize),f);
        fwrite(stateBuf,1,stateSize,f);
        fclose(f);
        printf("Console state successfully saved to /etc/pmsave.dat\n");
        }
    PM_free(stateBuf);
    return 0;
}

