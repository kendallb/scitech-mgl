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
* Environment:  RTTarget-32
*
* Description:  Module to implement OS specific services to measure the
*               CPU frequency.
*
****************************************************************************/

/*---------------------------- Global variables ---------------------------*/

static ibool havePerformanceCounter;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Initialise the counter and return the frequency of the counter.
****************************************************************************/
static void GetCounterFrequency(
    CPU_largeInteger *freq)
{
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)freq)) {
        havePerformanceCounter = false;
        freq->low = 100000;
        freq->high = 0;
        }
    else
        havePerformanceCounter = true;
}

/****************************************************************************
REMARKS:
Read the counter and return the counter value.
****************************************************************************/
#define GetCounter(t)                                       \
{                                                           \
    if (havePerformanceCounter)                             \
        QueryPerformanceCounter((LARGE_INTEGER*)t);         \
    else {                                                  \
        (t)->low = GetTickCount() * 100;                    \
        (t)->high = 0;                                      \
        }                                                   \
}
