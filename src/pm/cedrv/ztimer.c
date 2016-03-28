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
* Environment:  WinCE
*
* Description:  OS specific implementation for the Zen Timer functions.
*
****************************************************************************/

/*---------------------------- Global variables ---------------------------*/

static CPU_largeInteger countFreq;
static ibool            havePerformanceCounter;
static ulong            start,finish;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Initialise the Zen Timer module internals.
****************************************************************************/
void __ZTimerInit(void)
{
    havePerformanceCounter = QueryPerformanceFrequency((LARGE_INTEGER*)&countFreq);
}

/****************************************************************************
REMARKS:
Start the Zen Timer counting.
****************************************************************************/
static void __LZTimerOn(
    LZTimerObject *tm)
{
    if (havePerformanceCounter)
        QueryPerformanceCounter((LARGE_INTEGER*)&tm->start);
    else
        tm->start.low = GetTickCount();
}

/****************************************************************************
REMARKS:
Compute the lap time since the timer was started.
****************************************************************************/
static ulong __LZTimerLap(
    LZTimerObject *tm)
{
    CPU_largeInteger    tmLap,tmCount;

    if (havePerformanceCounter) {
        QueryPerformanceCounter((LARGE_INTEGER*)&tmLap);
        _CPU_diffTime64(&tm->start,&tmLap,&tmCount);
        return _CPU_calcMicroSec(&tmCount,countFreq.low);
        }
    else {
        tmLap.low = GetTickCount();
        return (tmLap.low - tm->start.low) * 1000L;
        }
}

/****************************************************************************
REMARKS:
Stop the Zen Timer counting.
****************************************************************************/
static void __LZTimerOff(
    LZTimerObject *tm)
{
    if (havePerformanceCounter)
        QueryPerformanceCounter((LARGE_INTEGER*)&tm->end);
    else
        tm->end.low = GetTickCount();
}

/****************************************************************************
REMARKS:
Compute the elapsed time in microseconds between start and end timings.
****************************************************************************/
static ulong __LZTimerCount(
    LZTimerObject *tm)
{
    CPU_largeInteger    tmCount;

    if (havePerformanceCounter) {
        _CPU_diffTime64(&tm->start,&tm->end,&tmCount);
        return _CPU_calcMicroSec(&tmCount,countFreq.low);
        }
    else
        return (tm->end.low - tm->start.low) * 1000L;
}

/****************************************************************************
REMARKS:
Define the resolution of the long period timer as microseconds per timer tick.
****************************************************************************/
#define ULZTIMER_RESOLUTION     1000

/****************************************************************************
REMARKS:
Read the Long Period timer from the OS
****************************************************************************/
static ulong __ULZReadTime(void)
{
    return GetTickCount();
}

/****************************************************************************
REMARKS:
Compute the elapsed time from the BIOS timer tick. Note that we check to see
whether a midnight boundary has passed, and if so adjust the finish time to
account for this. We cannot detect if more that one midnight boundary has
passed, so if this happens we will be generating erronous results.
****************************************************************************/
ulong __ULZElapsedTime(
    ulong start,
    ulong finish)
{
    return finish - start;
}

