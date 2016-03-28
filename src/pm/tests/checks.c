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
* Description:  Main module for building checked builds of products with
*               assertions and trace code.
*
****************************************************************************/

#include "scitech.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef  __WINDOWS__
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#endif

#ifdef  CHECKED

/*---------------------------- Global variables ---------------------------*/

#define LOGFILE "\\scitech.log"

void (*_CHK_fail)(int fatal,const char *msg,const char *cond,const char *file,int line) = _CHK_defaultFail;

/*---------------------------- Implementation -----------------------------*/

/****************************************************************************
DESCRIPTION:
Handles fatal error and warning conditions for checked builds.

HEADER:
scitech.h

REMARKS:
This function is called whenever an inline check or warning fails in any
of the SciTech runtime libraries. Warning conditions simply cause the
condition to be logged to the log file and send to the system debugger
under Window. Fatal error conditions do all of the above, and then
terminate the program with a fatal error conditions.

This handler may be overriden by the user code if necessary to replace it
with a different handler (the MGL for instance overrides this and replaces
it with a handler that does an MGL_exit() before terminating the application
so that it will clean up correctly.
****************************************************************************/
void _CHK_defaultFail(
    int fatal,
    const char *msg,
    const char *cond,
    const char *file,
    int line)
{
    char    buf[256];
    FILE    *log = fopen(LOGFILE, "at+");

    sprintf(buf,msg,cond,file,line);
    if (log) {
        fputs(buf,log);
        fflush(log);
        fclose(log);
#ifdef  __WINDOWS__
        OutputDebugStr(buf);
#endif
        }
    if (fatal) {
#ifdef  __WINDOWS__
        MessageBox(NULL, buf,"Fatal Error!",MB_ICONEXCLAMATION);
#else
        fputs(buf,stderr);
#endif
        exit(-1);
        }
}

#endif  /* CHECKED */
