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
* Description:  Test program for the Zen Timer Library.
*
****************************************************************************/

#include <stdio.h>
#include "pmapi.h"
#include "ztimer.h"

#define DELAY_SECS  10

/*-------------------------- Implementation -------------------------------*/

/* The following routine takes a long count in microseconds and outputs
 * a string representing the count in seconds. It could be modified to
 * return a pointer to a static string representing the count rather
 * than printing it out.
 */

void ReportTime(ulong count)
{
    ulong   secs;

    secs = count / 1000000L;
    count = count - secs * 1000000L;
    printf("Time taken: %lu.%06lu seconds\n",secs,count);
}

int     i,j;                                /* NON register variables! */

int main(void)
{
#ifdef  LONG_TEST
    ulong   start,finish;
#endif

    printf("Processor type: %d %ld MHz\n", CPU_getProcessorType(), CPU_getProcessorSpeed(true));

    ZTimerInit();

    /* Test the long period Zen Timer (we don't check for overflow coz
     * it would take tooooo long!)
     */

    LZTimerOn();
    for (j = 0; j < 10; j++)
        for (i = 0; i < 20000; i++)
            i = i;
    LZTimerOff();
    ReportTime(LZTimerCount());

    /* Test the ultra long period Zen Timer */
#ifdef LONG_TEST
    start = ULZReadTime();
    delay(DELAY_SECS * 1000);
    finish = ULZReadTime();
    printf("Delay of %d secs took %d 1/10ths of a second\n",
        DELAY_SECS,ULZElapsedTime(start,finish));
#endif

    return 0;
}
