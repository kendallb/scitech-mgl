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
* Environment:  Linux
*
* Description:  Linux specific code for the CPU detection module.
*
****************************************************************************/

#include <sys/utsname.h>

#include <ztimer.h>

/*----------------------------- Implementation ----------------------------*/

#ifdef __INTEL__
/****************************************************************************
REMARKS:
Initialise the counter and return the frequency of the counter.
****************************************************************************/
static void GetCounterFrequency(
    CPU_largeInteger *freq)
{
    freq->low = 1000000;
    freq->high = 0;
}
#endif

/****************************************************************************
REMARKS:
Read the counter and return the counter value.
****************************************************************************/
#define GetCounter(t)                           \
{                                               \
    struct timeval tv;                          \
    gettimeofday(&tv,NULL);                     \
    (t)->low = tv.tv_sec*1000000 + tv.tv_usec;  \
    (t)->high = 0;                              \
}

#ifndef __INTEL__
/****************************************************************************
REMARKS:
uname() sounds more reliable than /proc/cpuinfo but does not provide
detailed CPU information
this is a temporary solution
****************************************************************************/
static char * _CPU_getProcessorName(void)
{
    struct utsname buf;
    static char    name[80] = "";

    if (name[0] != '\0')
        return name;
    if (uname(&buf) == -1)
        return "Unknown";
    strncpy(name,buf.machine,sizeof(name));
    return name;
}
#endif

