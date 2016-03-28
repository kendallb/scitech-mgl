/****************************************************************************
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
* Environment:  any
*
* Description:  Simple program for Unix systems to spawn a C or C++
*               compiler with arguments passed in a response file. We do
*               this so we can keep the build command lines clean under
*               Unix systems using dmake, so that we can more easily see
*               when errors show up.
*
*               If you need to see the command line being used, run dmake
*               with the -vt command line switch to leave the response file
*               behind.
*
****************************************************************************/

#include "pmapi.h"
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __UNIX__
#include <unistd.h>
#else
#include <process.h>
#endif

/*------------------------- Global variables ------------------------------*/

#define MAX_CMD_LINE    (10 * 1024)
#define MAX_ARGS        1024

char    cmdLine[MAX_CMD_LINE];
char    *ccArgv[MAX_ARGS];
char    moduleName[PM_MAX_PATH];

/*-------------------------- Implementation -------------------------------*/

void usage(void)
{
    printf("Usage: k_cc <cc> @<rspfile> <source_file>\n");
    printf("\n");
    printf("Simple C/C++ compiler wrapper program to hide the command line. This\n");
    printf("program is used to keep the command line clean so it is easy to spot\n");
    printf("warnings and messages in build log files when using dmake.\n");
    printf("\n");
    printf("If you need to see the command line being used, run dmake with the -vt\n");
    printf("command line switch to leave the response file behind.\n");
    exit(-1);
}

int main(int argc,char *argv[])
{
    FILE    *f;

    if ((argc != 3 && argc != 4) || argv[2][0] != '@')
        usage();
    if ((f = fopen(&argv[2][1],"r")) == NULL) {
        printf("Unable to open response file '%s'\n", &argv[2][1]);
        exit(-1);
        }
    if (fgets(cmdLine,sizeof(cmdLine),f) == NULL) {
        printf("I/O error reading response file '%s'\n", &argv[2][1]);
        exit(-1);
        }
    fclose(f);
    if (argc == 4) {
        strcat(cmdLine," ");
        strcat(cmdLine,argv[3]);
        printf("%s %s %s\n", argv[1], argv[2], argv[3]);
        }
    else
        printf("%s %s\n", argv[1], argv[2]);
    parse_commandline(moduleName,cmdLine,&argc,ccArgv,MAX_ARGS);
    ccArgv[argc] = NULL;
    return execvp(argv[1],ccArgv);
}

