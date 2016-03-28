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
* Description:  Main module containing debug checking features.
*
****************************************************************************/

#include "pmapi.h"
#ifdef  __WIN32_VXD__
#include "vxdfile.h"
#elif defined(__NT_DRIVER__)
#include "ntdriver.h"
#elif defined(__OS2_VDD__)
#include "vddfile.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if defined(__WINDOWS32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif

/*---------------------------- Global variables ---------------------------*/

#define DEBUG_LOG "scitech.log"

#if !defined(__NT_DRIVER__) && !defined(__WIN32_VXD__) && !defined(__DRIVER__)
/* {secret} */
void (_ASMAPIP _CHK_fail)(int fatal,const char *msg,const char *cond,const char *file,int line) = _CHK_defaultFail;
#endif
static char logFile[256] = "";

/*----------------------------- Implementation ----------------------------*/

#ifndef __NT_DRIVER__
/****************************************************************************
DESCRIPTION:
Function to log information to the log file and debugger device

HEADER:
scitech.h

PARAMETERS:
fmt     - Formatted message to display
...     - Variable list of arguments to display

REMARKS:
Logs the information to the log file, and optionally displays the message on
any attached debugger device.
{secret}
****************************************************************************/
void _ASMAPI _CHK_debugLog(
    const char *fmt,
    ...)
{
    char            buf[256];
    va_list         argptr;
    FILE            *f;
    static ibool    firstTime = true;

    /* Format the message to log */
    va_start(argptr, fmt);
    vsprintf(buf, fmt, argptr);
    va_end(argptr);

    /* Find the name of the log file to use */
    if (logFile[0] == 0) {
        strcpy(logFile,PM_getSNAPPath());
        PM_backslash(logFile);
        strcat(logFile,DEBUG_LOG);
        }

    /* Create the log file if this is the first time through */
    if (firstTime) {
        if ((f = fopen(logFile,"w")) != NULL)
            fclose(f);
        firstTime = false;
        }

    /* Log the message to the log file */
    if ((f = fopen(logFile,"a+")) != NULL) {
        fwrite(buf,1,strlen(buf),f);
        fclose(f);
        }
}
#endif

/****************************************************************************
DESCRIPTION:
Default check failure handling function

HEADER:
scitech.h

PARAMETERS:
fatal   - Determine whether the condition was fatal or not
msg     - Formatted message to display (format "%s %s %d")
cond    - String describing the condition that failed
file    - String describing the file that failed
line    - Line number in the file for the failure

REMARKS:
Logs the information to the log file, and optionally displays the message on
any attached debugger device.
{secret}
****************************************************************************/
void _ASMAPI _CHK_defaultFail(
    int fatal,
    const char *msg,
    const char *cond,
    const char *file,
    int line)
{
    char    buf[256];

    /* First log the message */
    _CHK_debugLog((char*)msg,cond,file,line);

    /* Finally handle fatal error conditions and exit */
    if (fatal) {
        sprintf(buf,"Check failed: check '%s' for details", logFile);
        PM_fatalError(buf);
        }
}

/****************************************************************************
DESCRIPTION:
Sets the location of the debug log file.

HEADER:
pmapi.h

PARAMETERS:
logFilePath - Full file and path name to debug log file.

REMARKS:
Sets the name and location of the debug log file. The debug log file is
created and written to when runtime checks, warnings and failure conditions
are logged to disk when code is compiled in CHECKED mode. By default the
log file is called 'scitech.log' and goes into the current SciTech SNAP
path for the application. You can use this function to set the filename
and location of the debug log file to your own application specific
directory.
****************************************************************************/
void PMAPI PM_setDebugLog(
    const char *logFilePath)
{
    strcpy(logFile,logFilePath);
}

