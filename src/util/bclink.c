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
* Environment:  MSDOS, Win32 Console
*
* Description:  Simple program to process a linker command line, strip out
*               the reference to the .RES file, run the linker and then
*               run the resource compile to bind the resource file to the
*               resulting .EXE file.
*
*               Specific to Borland C++ 4.5 and later
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <io.h>

//#define   DEBUG

#define MAX_LINES   300
#define LD_CMD      argv[1]
#define RSP_FILE    argv[2]
#define RES_EXT     ".res"
#define RES_LEN     4
#define DEF_EXT     ".def"
#define DEF_LEN     4
#define BUF_SIZE    2048

char    *lines[MAX_LINES];
char    buf[BUF_SIZE];
char    resFile[_MAX_DIR];
char    defFile[_MAX_DIR];

int findstr(char *p,char *resFile,int *resLen,char *str,int len)
{
    char *start = p;

    while (*p != '0' && *p != ' ' && *p != ',' && *p != '\n') {
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

/*
 * Sample linker response file for a Win16 app. The .res file will be
 * listed in the object file list, and needs to be moved from there and
 * added to the end of the linker response file.
 *
 * /v -Twe -c -C -x+
 * c0wl.obj win16.obj win16.res
 * win16.exe
 * win16.map
 * import.lib mathwl.lib cwl.lib ztimer.lib
 * win16.def
 */

int main(int argc, char *argv[])
{
    int     i,bufLen,resLen,numLines,foundRes = 0;
    int     defLen,foundDef = 0;
    char    *p,*p2;
    FILE    *f;

    if (argc != 3) {
        fprintf(stderr, "Usage: bclink <tlink.exe> <link_rsp_file>\n");
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
     * file name and adding it to the end of the response file.
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
                p += resLen;
                if (*p == ' ')
                    p++;
                foundRes = 1;
#ifdef  DEBUG
                printf("Found resource file: %s\n", resFile);
#endif
                }
            else {
                if (!foundDef && findstr(p, defFile, &defLen, DEF_EXT, DEF_LEN)) {
                    foundDef = 1;
#ifdef  DEBUG
                    printf("Found DEF file: %s\n", defFile);
#endif
                    }
                *p2++ = *p++;
                bufLen++;
                }
            }
        *p2 = '\0';
        fputs(buf,f);
        }
    fprintf(f, "\n%s", resFile);
    fclose(f);

    /* If we found a DEF file in the linker script, check if one is on disk
     * and if not create a default one. The default one we create is for
     * Win32 apps so all 16-bit apps *must* have a .DEF file included.
     */
    if (foundDef && access(defFile,0) == -1) {
#ifdef  DEBUG
        printf("Creating default def file %s\n", defFile);
#endif
        if ((f = fopen(defFile, "w")) == NULL) {
            printf("Unable to create default .DEF file!\n");
            exit(1);
            }
        fputs("EXETYPE WINDOWS\n", f);
        fputs("CODE PRELOAD MOVEABLE DISCARDABLE\n", f);
        fputs("DATA PRELOAD MOVEABLE\n", f);
        fclose(f);
        foundDef = 2;
        }

    /* Now run the linker to link the final output file */
    sprintf(buf,"@%s", RSP_FILE);
#ifdef  DEBUG
    printf("%s %s\n", LD_CMD, buf);
#endif
    if ((i = spawnlp(P_WAIT, LD_CMD, LD_CMD, buf, NULL)) != 0)
        return i;

    /* Now delete the .DEF file if we created it */
    if (foundDef == 2)
        unlink(defFile);
    return 0;
}
