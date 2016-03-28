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
* Environment:  MSDOS
*
* Description:  OS specific implementation for the Zen Timer functions.
*
****************************************************************************/


/*---------------------------- Global variables ---------------------------*/

uchar * _VARAPI _ZTimerBIOSPtr;

/*----------------------------- Implementation ----------------------------*/

/* External assembler functions */

void    _ASMAPI LZ_timerOn(void);
ulong   _ASMAPI LZ_timerLap(void);
void    _ASMAPI LZ_timerOff(void);
ulong   _ASMAPI LZ_timerCount(void);
void    _ASMAPI LZ_disable(void);
void    _ASMAPI LZ_enable(void);

/****************************************************************************
REMARKS:
Initialise the Zen Timer module internals.
****************************************************************************/
void __ZTimerInit(void)
{
    _ZTimerBIOSPtr = PM_getBIOSPointer();
}

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerOn(tm)     LZ_timerOn()

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerLap(tm)        LZ_timerLap()

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerOff(tm)        LZ_timerOff()

/****************************************************************************
REMARKS:
Call the assembler Zen Timer functions to do the timing.
****************************************************************************/
#define __LZTimerCount(tm)  LZ_timerCount()

/****************************************************************************
REMARKS:
Define the resolution of the long period timer as microseconds per timer tick.
****************************************************************************/
#define ULZTIMER_RESOLUTION     54925

/****************************************************************************
REMARKS:
Read the Long Period timer value from the BIOS timer tick.
****************************************************************************/
static ulong __ULZReadTime(void)
{
    ulong   ticks;
    LZ_disable();            /* Turn of interrupts               */
    ticks = PM_getLong(_ZTimerBIOSPtr+0x6C);
    LZ_enable();             /* Turn on interrupts again         */
    return ticks;
}

/****************************************************************************
REMARKS:
Compute the elapsed time from the BIOS timer tick. Note that we check to see
whether a midnight boundary has passed, and if so adjust the finish time to
account for this. We cannot detect if more that one midnight boundary has
passed, so if this happens we will be generating erronous results.
****************************************************************************/
ulong __ULZElapsedTime(ulong start,ulong finish)
{
    if (finish < start)
        finish += 1573040L;         /* Number of ticks in 24 hours      */
    return finish - start;
}

