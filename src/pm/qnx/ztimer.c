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
* Description:  QNX specific implementation for the Zen Timer functions.
*
****************************************************************************/

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Initialise the Zen Timer module internals.
****************************************************************************/
void __ZTimerInit(void)
{
}

/****************************************************************************
REMARKS:
Use the gettimeofday() function to get microsecond precision (probably less
though)
****************************************************************************/
static ulong __ULZReadTime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_nsec / 1000 + ts.tv_sec * 1000000);
}

/****************************************************************************
REMARKS:
Start the Zen Timer counting.
****************************************************************************/
#define __LZTimerOn(tm)     tm->start.low = __ULZReadTime()

/****************************************************************************
REMARKS:
Compute the lap time since the timer was started.
****************************************************************************/
#define __LZTimerLap(tm)        (__ULZReadTime() - tm->start.low)

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerOff(tm)        tm->end.low = __ULZReadTime()

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerCount(tm)  (tm->end.low - tm->start.low)

/****************************************************************************
REMARKS:
Define the resolution of the long period timer as microseconds per timer tick.
****************************************************************************/
#define ULZTIMER_RESOLUTION 1

/****************************************************************************
REMARKS:
Compute the elapsed time from the BIOS timer tick. Note that we check to see
whether a midnight boundary has passed, and if so adjust the finish time to
account for this. We cannot detect if more that one midnight boundary has
passed, so if this happens we will be generating erronous results.
****************************************************************************/
ulong __ULZElapsedTime(ulong start,ulong finish)
{ return finish - start; }
