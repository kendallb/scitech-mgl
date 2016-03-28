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
* Description:  Test program to test the shared memory subsystem.
*
*               Functions tested:   PM_mallocShared(), PM_freeShared()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pmapi.h"

#define MAXALLOC    2000

int main(void)
{
    int     i;
    ulong   allocs;
    char    *p,*pa[MAXALLOC];
    PM_HWND hwndConsole;

    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    for (allocs = i = 0; i < MAXALLOC; i++) {
        if ((pa[i] = PM_mallocShared(10*1024)) != 0) {    /* in 10k blocks    */
            p = pa[allocs];
            memset(p, 0, 10*1024); /* touch every byte              */
            *p = 'x';           /* do something, anything with      */
            p[1023] = 'y';      /* the allocated memory             */
            allocs++;
            printf("Allocated %lu bytes\r", 10*(allocs << 10));
            }
        else break;
        if (PM_kbhit() && (PM_getch() == 0x1B))
            break;
        }
    PM_closeConsole(hwndConsole);

    printf("\n\nAllocated total of %lu bytes\n", 10 * (allocs << 10));

    for (i = allocs-1; i >= 0; i--)
        PM_freeShared(pa[i]);

    return 0;
}

