/****************************************************************************
*
*                           SciTech SNAP Graphics
*
*               Copyright (C) 1991-2003 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Win32 DLL
*
* Description:  Test program to verify the functionality of the standard C
*               library.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <scitech.h>
#include <clib/os/os.h>
#include <clib/sys/cdecl.h>

/*--------------------------- Global variables ----------------------------*/

typedef struct {
    long    id;
    char    *name;
    } OS_ent;

OS_ent OS_names[] = {
    {_OS_PHARLAP,   "PharLap Embedded RTOS"},

    {_OS_WIN32VXD,  "Win32 VxD drivers"},
    {_OS_WIN95,     "Windows 95/98"},
    {_OS_WINNTDRV,  "Windows NT device driver"},
    {_OS_WIN2K,     "Windows 2000/XP"},
    {_OS_WINNT4,    "Windows NT 4"},

    {_OS_LINUX,     "Linux"},
    {_OS_FREEBSD,   "FreeBSD"},
    {_OS_SCO,       "SCO Unix"},
    {_OS_SOLARIS,   "Sun Solaris"},
    {_OS_QNX,       "QNX Realtime OS"},
    {_OS_BEOS,      "BeOS"},
    {_OS_OS2,       "IBM OS/2"},
    {-1,            "Unknown"},
    };

OS_ent OS_classes[] = {
    {_OS_CLASS_DOS,     "MSDOS Class OS"},
    {_OS_CLASS_WIN32,   "Win32 Class OS"},
    {_OS_CLASS_NETWARE, "Novell Netware Class OS"},
    {_OS_CLASS_UNIX,    "Unix Class OS"},
    {_OS_CLASS_OS2,     "OS/2 Class OS"},
    {-1,                "Unknown Class OS"},
    };

/*----------------------------- Glue Functions ----------------------------*/

void _CEXPORT HelloWorld(void)
{
    OS_ent  *os,*cls;

    for (os = OS_names; os->id != -1; os++) {
        if (_IS_OS(os->id))
            break;
        }
    for (cls = OS_classes; cls->id != -1; cls++) {
        if (_IS_OS_CLASS(cls->id))
            break;
        }
    printf("Hello World from %s, (%s)!\n", os->name, cls->name);
}

//#include "tests.c"
