/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Simple program that allows the SciTech SNAP Graphics
*               option for log file generation to be changed or queried.
*
*               SNAP Graphics log file generation performs file I/O and
*               formatted printing operations which are only useful for
*               output to read/write file systems. Disabling the log file
*               option will bypass these time-consuming steps, and
*               effectively reduce loading time in embedded systems
*               or other environments with read-only file systems.
*
* Note:         This particular program sets certain global options
*               which should only be performed by advanced users!
*
****************************************************************************/

#include "snap/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------- Global Variables ---------------------------*/

static GA_globalOptions     glb;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the help screen.
****************************************************************************/
static void help(void)
{
    printf("\nUsage:\n\n"
           "  setlog show\n"
           "  setlog on|off\n");
    exit(-1);
}

/****************************************************************************
REMARKS:
Show the current setting for the global log file option.
Global options do not require that the SNAP Graphics driver be loaded.
****************************************************************************/
static void ShowSettings(
    void)
{
    glb.dwSize = sizeof(glb);
    GA_getGlobalOptions(&glb,false);
    printf("Disable Log File Option = %s\n\n", glb.bDisableLogFile ? "on" : "off");
}

/****************************************************************************
REMARKS:
Change the current setting for the global log file option.
Global options do not require that the SNAP Graphics driver be loaded.
****************************************************************************/
static void ChangeSettings(
    ibool enable)
{
    glb.dwSize = sizeof(glb);
    GA_getGlobalOptions(&glb,false);
    glb.bDisableLogFile = enable;
    GA_setGlobalOptions(&glb);
    GA_saveGlobalOptions(&glb);
    ShowSettings();
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    /* Now handle the query */
    if (argc >= 2) {
        if (stricmp(argv[1],"show") == 0)
            ShowSettings();
        else if (stricmp(argv[1],"on") == 0)
            ChangeSettings(true);
        else if (stricmp(argv[1],"off") == 0)
            ChangeSettings(false);
        else
            help();
        }
    else
        help();
    return 0;
}

