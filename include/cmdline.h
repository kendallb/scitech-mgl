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
* Description:  Header file for command line parsing module. This module
*               contains code to parse the command line, extracting options
*               and parameters in standard System V style.
*
****************************************************************************/

#ifndef __CMDLINE_H
#define __CMDLINE_H

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*---------------------------- Typedef's etc -----------------------------*/

#define ALLDONE     -1
#define PARAMETER   -2
#define INVALID     -3
#define HELP        -4

#define MAXARG      80

/* Option type sepecifiers */

#define OPT_INTEGER     'd'
#define OPT_HEX         'h'
#define OPT_OCTAL       'o'
#define OPT_UNSIGNED    'u'
#define OPT_LINTEGER    'D'
#define OPT_LHEX        'H'
#define OPT_LOCTAL      'O'
#define OPT_LUNSIGNED   'U'
#define OPT_FLOAT       'f'
#define OPT_DOUBLE      'F'
#define OPT_LDOUBLE     'L'
#define OPT_STRING      's'
#define OPT_SWITCH      '!'

typedef struct {
    uchar   opt;                /* The letter to describe the option    */
    uchar   type;               /* Type descriptor for the option       */
    void    *arg;               /* Place to store the argument          */
    char    *desc;              /* Description for this option          */
    } Option;

#define NUM_OPT(a)  sizeof(a) / sizeof(Option)

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------- Global variables ---------------------------*/

extern  int     nextargv;
extern  char    *nextchar;

/*------------------------- Function Prototypes --------------------------*/

int getcmdopt(int argc,char **argv,char *format,char **argument);
int getcmdargs(int argc,char *argv[],int num_opt,Option optarr[],
            int (*do_param)(char *param,int num));
void print_desc(int num_opt,Option optarr[]);
int parse_commandline(char *moduleName,char *cmdLine,int *pargc,
    char *pargv[],int maxArgv);

#ifdef __cplusplus
}
#endif

#endif

