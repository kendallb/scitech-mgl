/****************************************************************************
*
*                               Makedef
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
* Environment:  MS DOS
*
* Description:  Program to take an import library definition file called
*               <myfile>.REF and output a decorated <myfile>.DEF file.
*               It decorates all the functions with explicit ordinals one
*               after the other so that we can get Visual C++ to work
*               with DLL's built with other compilers. We also remove any
*               leading underscores (if present) for the function names
*               so that Visual C++ will be able to link properly to our
*               libraries (fucking compiler!).
*
****************************************************************************/

#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXPORTS "EXPORTS"

int main(int argc,char *argv[])
{
    char    buf[PM_MAX_PATH];
    char    filename[PM_MAX_PATH];
    char    name[PM_MAX_PATH],stdcallName[PM_MAX_PATH];
    char    *p,*q;
    ibool   visual,foundExports;
    int     ordinal,len;
    FILE    *in,*out;

    if (argc != 2 && argc != 3) {
        printf("Usage: MAKEDEF [-v] <myfile>.REF\n");
        printf("\n");
        printf("where -v outputs a file for Visual C++. Note the .REF is not present.\n");
        exit(1);
        }
    if (argc == 2) {
        visual = false;
        strcpy(filename,argv[1]);
        }
    else if (argc == 3) {
        visual = true;
        strcpy(filename,argv[2]);
        }

    strcpy(buf,filename);
    strcat(buf,".ref");
    if ((in = fopen(buf,"r")) == NULL) {
        printf("Unable to open input file %s\n", buf);
        exit(1);
        }
    strcpy(buf,filename);
    strcat(buf,".def");
    if ((out = fopen(buf,"w")) == NULL) {
        printf("Unable to open output file %s\n", buf);
        exit(1);
        }

    /* Search for start of EXPORT's */
    foundExports = false;
    while (!feof(in)) {
        if (fgets(buf,sizeof(buf),in)) {
            fputs(buf,out);
            if (strncmp(buf,EXPORTS,sizeof(EXPORTS)-1) == 0) {
                foundExports = true;
                break;
                }
            }
        }
    if (!foundExports) {
        printf("Did not find EXPORTS keyword!\n");
        exit(1);
        }

    /* Now copy the decorated functions */
    ordinal = 1;
    while (!feof(in)) {
        if (fgets(buf,sizeof(buf),in)) {
            len = strlen(buf);
            if (buf[len-1] == '\n') {
                buf[len-1] = '\0';
                len--;
                }
            if (buf[0] == ';')
                continue;
            if (len > 0) {
                /* Extract the name to be exported */
                p = buf;
                while (isspace(*p) && *p != '\n' && *p != '\0')
                    p++;
                q = stdcallName;
                while (!isspace(*p) && *p != '\n' && *p != '\0')
                    *q++ = *p++;
                *q = '\0';
                strcpy(name,stdcallName);
                if ((p = strchr(name,'@')) != NULL)
                    *p = '\0';
                else
                    stdcallName[0] = '\0';

                /* Now output the export lines as necessary for each compiler */
                if (visual) {
                    if (stdcallName[0])
                        fprintf(out,"    %s=_%s @%d\n", name,stdcallName,ordinal);
                    else
                        fprintf(out,"    %s=%s @%d\n", name,name,ordinal);
                    }
                else {
                    if (stdcallName[0])
                        fprintf(out,"    %s=%s @%d\n", name,name,ordinal);
                    else
                        fprintf(out,"    %s=_%s @%d\n", name,name,ordinal);
                    }
                ordinal++;
                }
            }
        }

    fclose(in);
    fclose(out);
    return 0;
}

