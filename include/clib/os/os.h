/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  General header file to define the global OS variable,
*               the the types of OS'es we recognise.
*
****************************************************************************/

#ifndef __OS_OS_H
#define __OS_OS_H

/* Defines for the runtime operating system. We define this
 * so that our OS neutral binaries can include code that is OS dependant
 * that will switch at runtime as necessary.
 */

#define _OS_DOS         0x00000001  /* MS-DOS environments              */
#define _OS_WIN32VXD    0x00000002  /* Win32 VxD drivers                */
#define _OS_WIN95       0x00000004  /* Windows 95/98/Me                 */
#define _OS_WIN2K       0x00000008  /* Windows 2000/XP                  */
#define _OS_OS2         0x00000010  /* Class for OS/2                   */
#define _OS_NETWARE     0x00000020  /* Class for Novell Netware         */
#define _OS_LINUX       0x00000040  /* Linux                            */
#define _OS_FREEBSD     0x00000080  /* FreeBSD                          */
#define _OS_SCO         0x00000100  /* SCO Unix                         */
#define _OS_SOLARIS     0x00000200  /* Sun Solaris                      */
#define _OS_QNX         0x00000400  /* QNX Realtime OS                  */
#define _OS_BEOS        0x00000800  /* BeOS                             */
#define _OS_SMX         0x00001000  /* SMX Embedded OS                  */
#define _OS_WINNT4      0x00002000  /* Windows NT 4.0                   */
#define _OS_PHARLAP     0x00004000  /* PharLap Embedded Real Time OS    */
#define _OS_OSE         0x00008000  /* Enea OSE Embedded OS             */
#define _OS_RTTARGET    0x00010000  /* RTTarget32 Embedded OS           */
#define _OS_WINCE       0x00020000  /* Windows CE Embedded OS           */
#define _OS_WINNTDRV    0x00040000  /* Windows NT device drivers        */
#define _OS_UNUNUNIUM   0x00080000  /* Unununium                        */
#define _OS_AMIGAOS4	0x00100000  /* AmigaOS4 PowerPC                 */
#define _OS_VXWORKS     0x00200000  /* VxWorks Embedded OS              */
#define _OS_ALL         0x7FFFFFFF  /* All OS'es                        */
#define _OS_UNSUPPORTED -1          /* Environment not supported        */

/* Defines for classes of operating systems */

#define _OS_CLASS_DOS       (_OS_DOS)
#define _OS_CLASS_WIN32     (_OS_WIN95 | _OS_WINNT4 | _OS_WIN2K)
#define _OS_CLASS_RING0     (_OS_WIN32VXD | _OS_WINNTDRV)
#define _OS_CLASS_OS2       (_OS_OS2)
#define _OS_CLASS_NETWARE   (_OS_NETWARE)
#define _OS_CLASS_UNIX      (_OS_LINUX | _OS_FREEBSD | _OS_SCO | _OS_SOLARIS | _OS_QNX | _OS_BEOS)
#define _OS_CLASS_EMBEDDED  (_OS_SMX | _OS_PHARLAP | _OS_OSE | _OS_RTTARGET | _OS_UNUNUNIUM | _OS_WINCE | _OS_VXWORKS)
#define _OS_CLASS_AMIGAOS   (_OS_AMIGAOS4)

/* Define a macro to test for a specific OS */

#define _IS_OS(f)       (___drv_os_type == (f))

/* Define a macro to test for a class of OS */

#define _IS_OS_CLASS(c) (___drv_os_type & (c))

#ifdef __cplusplus
extern "C" long     _VARAPI ___drv_os_type;
#else
extern long         _VARAPI ___drv_os_type;
#endif

#endif  /* __OS_OS_H */

