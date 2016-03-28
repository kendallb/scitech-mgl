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
* Environment:  Any
*
* Description:  Test program for PM_getOSName.
*
****************************************************************************/

#include "ztimer.h"
#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>

/*----------------------------- Implementation ----------------------------*/

int function1(
    int x,
    int y)
{
    return x + y;
}

int function2(
    int x,
    int y)
{
    return x - y;
}

int main(int
    argc)
{
    int i,z;
    int (*fp)(int x,int y);

    ZTimerInit();

    if (argc != 1)
        fp = function2;
    else
        fp = function1;

    LZTimerOn();
    z = 0;
    for (i = 0; i < 100000000; i++) {
        z += fp(i,10);
        }
    LZTimerOff();
    printf("func   z = %d (time = %d)\n", z, LZTimerCount());

    LZTimerOn();
    z = 0;
    for (i = 0; i < 100000000; i++) {
        if (argc != 1)
            z += i - 10;
        else
            z += i + 10;
        }
    LZTimerOff();
    printf("inline z = %d (time = %d)\n", z, LZTimerCount());

    return 0;
}

