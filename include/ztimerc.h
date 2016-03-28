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
* Description:  Header file for the Zen Timer library. Provides a number
*               of routines to accurately time segments of code. A long
*               period timer is provided to time code that takes up to
*               one hour to execute, with microsecond precision, and an
*               ultra long period timer for timing code that takes up to
*               24 hours to execute (raytracing etc).
*
****************************************************************************/

#ifndef __ZTIMERC_H
#define __ZTIMERC_H

#include "cpuinfo.h"

/*--------------------- Macros and type definitions -----------------------*/

/****************************************************************************
REMARKS:
Defines the structure for an LZTimerObject which contains the starting and
ending timestamps for the timer. By putting the timer information into a
structure the Zen Timer can be used for multiple timers running
simultaneously.

HEADER:
ztimer.h

MEMBERS:
start   - Starting 64-bit timer count
end     - Ending 64-bit timer count
****************************************************************************/
typedef struct {
    CPU_largeInteger    start;
    CPU_largeInteger    end;
    } LZTimerObject;

/*-------------------------- Function Prototypes --------------------------*/

#define LZTIMER_RES     1e-6            /* Seconds in a tick            */

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Routine to initialise the library - you MUST call this first! */

void    ZAPI ZTimerInit(void);
void    ZAPI ZTimerInitExt(ibool accurate);

/* Long period timing routines (times up to 1 hour) */

void    ZAPI LZTimerOn(void);
ulong   ZAPI LZTimerLap(void);
void    ZAPI LZTimerOff(void);
ulong   ZAPI LZTimerCount(void);

/* Long period timing routines with passed in timer object */

void    ZAPI LZTimerOnExt(LZTimerObject *tm);
ulong   ZAPI LZTimerLapExt(LZTimerObject *tm);
void    ZAPI LZTimerOffExt(LZTimerObject *tm);
ulong   ZAPI LZTimerCountExt(LZTimerObject *tm);

/* Ultra long period timing routines (times up to 65,000 hours) */

void    ZAPI ULZTimerOn(void);
ulong   ZAPI ULZTimerLap(void);
void    ZAPI ULZTimerOff(void);
ulong   ZAPI ULZTimerCount(void);
ulong   ZAPI ULZReadTime(void);
ulong   ZAPI ULZElapsedTime(ulong start,ulong finish);
void    ZAPI ULZTimerResolution(ulong *resolution);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __ZTIMERC_H */

