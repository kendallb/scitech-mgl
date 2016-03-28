/****************************************************************************
*
*                              Expand Include
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
* Environment:  MSDOS
*
* Description:  Program to expand all assembler include files in the
*               assembler source file into the resulting target
*               source file. Because of a bug in the way that 32 bit
*               linkers handle TASM's debug info (including BCC32 when
*               debugging under TD32) the debuggers cannot correctly
*               handle stepping into assembler source that has been
*               included from a separate file. In order to get around
*               this we use this program to expand all the source modules
*               that we are currently debugging into the main source
*               module so that we can properly step into the resulting
*               code.
*
*               This program is very simple and will only search for
*               include files relative to the current directory.
*
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#ifdef  __MSDOS__
#include <dir.h>
#endif
#include "scitech.h"

#define BEGIN_STR       ";BEGIN"
#define BEGIN_LEN       6
#define BEGIN_FULL_STR  ";BEGIN EXPAND INCLUDE"
#define BEGIN_FULL_LEN  21
#define END_STR         ";END"
#define END_LEN         4
#define END_FULL_STR    ";END EXPAND INCLUDE"
#define END_FULL_LEN    19
#define ASM_INC         "include"
#define ASM_INC_LEN     7

/*-------------------------- Implementation -------------------------------*/

char    line[255];

void error(char *msg)
{
    fprintf(stderr,"FATAL: %s\n", msg);
    exit(1);
}

/* Open a file returning true if successful */

FILE *openfile(char *filename,char *mode)
{
    FILE *in;
    if ((in = fopen(filename,mode) ) == NULL)
        error("Could not open input file");
    return in;
}

char *skipwhite(char *s)
{
    while (*s && isspace(*s))
        s++;
    return *s ? s : NULL;
}

char *skiptowhite(char *s)
{
    while (*s && !isspace(*s))
        s++;
    return *s ? s : NULL;
}

void help(void)
{
    printf("Usage: expndinc <srcfile> <dstfile>\n");
    exit(1);
}

void expandIncludes(FILE *in,FILE *out)
{
    FILE    *inc;
    char    incName[30],*p,*p2;
    ibool    expanding = false;

    while (fgets(line,sizeof(line),in)) {
        if (expanding) {
            if (strnicmp(line,ASM_INC,ASM_INC_LEN) == 0) {
                p = skipwhite(line+ASM_INC_LEN)+1;
                p2 = incName;
                while (*p != '\"')
                    *p2++ = *p++;
                *p2 = '\0';
                inc = openfile(incName,"r");
                fprintf(out,";---- BEGIN INCLUDED FILE %s ----\n",incName);
                fprintf(out,";%s",line);
                expandIncludes(inc,out);
                fprintf(out,";---- END INCLUDED FILE %s ----\n",incName);
                fclose(inc);
                }
            else fputs(line,out);
            if (strnicmp(line,END_STR,END_LEN) == 0)
                if (strnicmp(line,END_FULL_STR,END_FULL_LEN) == 0)
                    expanding = false;
            }
        else {
            fputs(line,out);
            if (strnicmp(line,BEGIN_STR,BEGIN_LEN) == 0)
                if (strnicmp(line,BEGIN_FULL_STR,BEGIN_FULL_LEN) == 0)
                    expanding = true;
            }
        }
}

int main(int argc,char *argv[])
{
    FILE    *in,*out;

    if (argc != 3)
        help();

    in = openfile(argv[1],"r");
    out = openfile(argv[2],"w");

    expandIncludes(in,out);

    fclose(out);
    fclose(in);
    return 0;
}
