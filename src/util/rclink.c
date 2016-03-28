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
* Environment:  Command line console
*
* Description:  Simple program to process a linker command line, strip out
*               the reference to the .RES file, run the linker and then
*               run the resource compile to bind the resource file to the
*               resulting .EXE file.
*
*               Note, this program is a *real* simple hack, but it works.
*
****************************************************************************/

#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __UNIX__
#include <unistd.h>
#include "spawn.h"
#else
#include <process.h>
#endif

#define MAX_LINES   1000
#define LD_CMD      argv[1]
#define RC_CMD      argv[2]
#define EXE_FILE    argv[3]
#define RSP_FILE    argv[4]
#define RES_EXT     ".res"
#define RES_LEN     4
#define BUF_SIZE    2048

char    *lines[MAX_LINES];
char    buf[BUF_SIZE];
char    resFile[PM_MAX_PATH];

int findstr(char *p,char *resFile,int *resLen,char *str,int len)
{
    char *start = p;

    while (*p != '\0' && *p != ' ' && *p != ',' && *p != '\n') {
        if (strncmp(p,str,len) == 0) {
            *resLen = p - start + len;
            memcpy(resFile,start,*resLen);
            resFile[*resLen] = '\0';
            return 1;
            }
        p++;
        }
    return 0;
}

int main(int argc, char *argv[])
{
    int     i,bufLen,resLen,numLines,foundRes = 0;
    char    *p,*p2;
    FILE    *f;

    if (argc != 5) {
        fprintf(stderr, "Usage: rclink <link> <rc> <exe_file> <link_rsp_file>\n");
        return -1;
        }

    /* Read all lines from input file. Input file is small enough that we
     * can read the entire file in.
     */
    if ((f = fopen(RSP_FILE, "r")) == NULL) {
        printf("Unable to open response file!\n");
        exit(1);
        }
    numLines = 0;
    while (fgets(buf,BUF_SIZE,f)) {
        if ((lines[numLines] = strdup(buf)) == NULL) {
            printf("Out of memory reading input file!\n");
            exit(1);
            }
        if (++numLines == MAX_LINES) {
            printf("Too many lines in input file!\n");
            exit(1);
            }
        }
    fclose(f);

    /* Dump all lines back to the response file, stripping out the .res
     * file name.
     */
    if ((f = fopen(RSP_FILE, "w")) == NULL) {
        printf("Unable to update response file!\n");
        exit(1);
        }
    for (i = 0; i < numLines; i++) {
        p = lines[i];
        p2 = buf;
        bufLen = 0;
        while (*p) {
            if (!foundRes && findstr(p, resFile, &resLen, RES_EXT, RES_LEN)) {
                if (stricmp(LD_CMD,"ilink") == 0 && i > 0) {
                    int len = strlen(lines[i-1]);
                    if (lines[i-1][len-2] == '+') {
                        lines[i-1][len-2] = '\n';
                        lines[i-1][len-1] = 0;
                        }
                    }
                if (stricmp(LD_CMD,"wlink") == 0 && i > 0) {
                    int len = strlen(lines[i-1]);
                    if (lines[i-1][len-2] == ',') {
                        lines[i-1][len-2] = '\n';
                        lines[i-1][len-1] = 0;
                        }
                    }
                if (bufLen > 1 && *(p-1) == ',')
                    p2 -= 1;
                else if (bufLen > 2 && *(p-2) == ',')
                    p2 -= 2;
                p += resLen;
                if (*p == ' ')
                    p++;
                if (bufLen == 0) {
                    if (*p == '\n')
                        p++;
                    else if (*p == ',' && *(p+1) == '\n')
                        p += 2;
                    }
                foundRes = 1;
#ifdef  DEBUG
                printf("Found resource file: %s\n", resFile);
#endif
                }
            else {
                *p2++ = *p++;
                bufLen++;
                }
            }
        *p2 = '\0';
        strcpy(lines[i],buf);
        }
    for (i = 0; i < numLines; i++) {
        if (strlen(lines[i]) > 0)
            fputs(lines[i],f);
        }
    fclose(f);

    /* Now run the linker to link the final output file */
    if (stricmp(LD_CMD,"ilink") == 0) {
        /* Special case code for linking with IBM VisualAge C++ */
        sprintf(buf,"@%s", RSP_FILE);
#ifdef  DEBUG
        printf("%s %s\n", LD_CMD, buf);
#endif
        if ((i = spawnlp(P_WAIT, LD_CMD, LD_CMD, "/nofree", "/nol", buf, NULL)) != 0)
            return i;
        }
    else {
        sprintf(buf,"@%s", RSP_FILE);
#ifdef  DEBUG
        printf("%s %s\n", LD_CMD, buf);
#endif
        if ((i = spawnlp(P_WAIT, LD_CMD, LD_CMD, buf, NULL)) != 0)
            return i;
        }

    if (stricmp(RC_CMD,"wbind") == 0) {
        /* Special case code for building Watcom Win386 extended Windows
         * applications to call the wbind utility with the correct
         * format. wbind automatically calls the resource compiler
         */
        p = EXE_FILE; p2 = buf;
        while (*p != 0 && *p != '.')
            *p2++ = *p++;
        *p2 = 0;
        strcat(buf,".exe");
        if (foundRes) {
#ifdef  DEBUG
            printf("%s %s -q -R -q %s %s\n", RC_CMD, buf, resFile, buf);
#endif
            if ((i = spawnlp(P_WAIT, RC_CMD, RC_CMD, buf, "-q", "-R", "-q", resFile, buf, NULL)) != 0)
                return i;
            }
        else {
#ifdef  DEBUG
            printf("%s %s -q -n\n", RC_CMD, buf);
#endif
            spawnlp(P_WAIT, RC_CMD, RC_CMD, buf, "-n", "-q", NULL);
            }
        }
    else {
        /* Now run the resource compiler to bind the resources if we found the
         * .res file in the list of objects
         */
        if (foundRes) {
#ifdef  DEBUG
            printf("%s %s %s\n", RC_CMD, resFile, EXE_FILE);
#endif
            if (stricmp(LD_CMD,"wlink") == 0) {
                if ((i = spawnlp(P_WAIT, RC_CMD, RC_CMD, "-q", resFile, EXE_FILE, NULL)) != 0)
                    return i;
                }
            else {
                if ((i = spawnlp(P_WAIT, RC_CMD, RC_CMD, resFile, EXE_FILE, NULL)) != 0)
                    return i;
                }                    
            }
        }
    return 0;
}

