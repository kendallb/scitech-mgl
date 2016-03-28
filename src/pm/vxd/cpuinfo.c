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
* Environment:  32-bit Windows VxD
*
* Description:  VxD specific code for the CPU detection module.
*
****************************************************************************/

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Initialise the counter and return the frequency of the counter.
****************************************************************************/
static void GetCounterFrequency(
    CPU_largeInteger *freq)
{
    freq->low = 1193180;
    freq->high = 0;
}

/****************************************************************************
REMARKS:
Read the counter and return the counter value.
****************************************************************************/
#define GetCounter(t)                           \
{                                               \
    CPU_largeInteger count;                     \
    VTD_Get_Real_Time(&count.high,&count.low);  \
    (t)->low = count.low;                       \
    (t)->high = count.high;                     \
}

