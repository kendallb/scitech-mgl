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
* Language:     Borland C++ 3.1 (not tested with anything else)
* Environment:  MSDOS
*
* Description:  Simple program to generate a .EXE file with GO32 bound to
*               it directly. You call it with the following syntax:
*
*                   dj_ld [options] <mainfile>.exe [objects/libs | @<rspfile>]
*
*               It will issue the following commands to build the resulting
*               .exe file:
*
*                   gcc [options] [objects] [libs] -o mainfile
*                *  strip mainfile
*                   coff2exe mainfile
*                *  rm mainfile
*
*               Notice that it generates the final file <mainfile>.exe as
*               output. If the -g debugging flag is specified on the
*               command line, then the 'strip mainfile' command will not
*               be executed, and neither will the del mainfile command.
*
*               Also note that libraries are specified on the command
*               in the normal DOS style format (mylib.a) rather than the
*               Unix format (-lmylib, which looks for the file libmylib.a).
*               Since GCC will only search the current directory for a
*               library named 'mylib.a', we handle searching the path
*               specified in the LIB environment variable and
*               provide the full pathname to the library file to link with.
*
*               We will also get the values to
*
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <dir.h>
#include <io.h>
#include <process.h>

#define true        1
#define false       0
#define BUFSIZE     4096
#define MAXLIBPATH  20
#define MAXLIBS     50

int     debug = false,foundmain = false,numlibs = 0;
char    mainfile[40];
char    libenv[255];
char    *libpath[MAXLIBPATH];
char    *libs[MAXLIBS];

void addCommand(FILE *rspfile, char *command, int len)
{
    int     i;
    char    path[255];

    if (command[len-1] == 'a' && command[len-2] == '.') {
        i = 0;
        while (libpath[i] != NULL) {
            strcpy(path,libpath[i]);
            strcat(path,"/");
            strcat(path,command);
            if (access(path,4) == 0)
                break;
            i++;
            }
        /* Add to list of libraries to be processed */
        if (libpath[i])
            libs[numlibs++] = strdup(path);
        else libs[numlibs++] = strdup(command);
        }
    else {
        fprintf(rspfile, command);
        fprintf(rspfile, " ");
        }
}

void processCommand(FILE *rspfile, char *command)
{
    int len = strlen(command);

    if (!foundmain && command[0] != '-') {
        strcpy(mainfile, command);
        if (len < 3) {
            fprintf(stderr, "Expecting name of .exe file!\n");
            exit(-1);
            }
        if (mainfile[len-4] == '.')
            mainfile[len-4] = '\0';
        foundmain = true;
        }
    else if (foundmain)
        addCommand(rspfile, command, len);
    else {
        fprintf(rspfile, command);
        fprintf(rspfile, " ");
        }
    if (command[0] == '-' && command[1] == 'g')
        debug = true;
}

void processRspFile(FILE *rspfile, const char *filename)
{
    FILE    *localrsp = fopen(filename+1, "r");
    char    *buffer = malloc(BUFSIZE), *p;

    if (!buffer) {
        printf("Out of memory!\n");
        exit(-1);
        }
    if (!localrsp) {
        printf("Unable to open response file %s!\n", filename);
        exit(-1);
        }

    while (fgets(buffer, BUFSIZE, localrsp)) {
        p = strtok(buffer, " ");
        while (p) {
            processCommand(rspfile, p);
            p = strtok(NULL, " ");
            }
        }

    fclose(localrsp);
    free(buffer);
}

int main(int argc, char *argv[])
{
    int     status,i;
    char    *rspfilename,command[80],*p;
    FILE    *rspfile;

    if (argc < 2) {
        fprintf(stderr, "Usage: dj_ld [options] <mainfile>.exe [objects/libs | @<rspfile>]\n");
        return -1;
        }

    /* Build the response file to call GCC with */
    rspfilename = tmpnam(NULL);
    if ((rspfile = fopen(rspfilename, "w")) == NULL) {
        printf("Unable to open temporary file!\n");
        exit(1);
        }

    /* Build the list of library directories from the environment variable */

    i = 0;
    if ((p = getenv("LIB")) != NULL) {
        strcpy(libenv,p);
        p = strtok(libenv,";");
        while (p) {
            libpath[i++] = p;
            p = strtok(NULL, ";");
            }
        }
    libpath[i] = NULL;

    /* Process all commands on command line */

    for (i = 1; i < argc; i++) {
        if (foundmain && argv[i][0] == '@')
            processRspFile(rspfile, argv[i]);
        else processCommand(rspfile, argv[i]);
        }

    /* Dump list of libraries after all object files */
    for (i = 0; i < numlibs; i++) {
        fprintf(rspfile,"%s ", libs[i]);
        }

    /* Dump list of libraries after all object files AGAIN! We need to
     * do this to get around problems with the DJGPP single pass linker,
     * since some of the MGL libraries have interdependecies.
     */
    for (i = 0; i < numlibs; i++) {
        fprintf(rspfile,"%s ", libs[i]);
        }

    /* Set the name of the output file */
    fprintf(rspfile, "-o %s\n", mainfile);
    fclose(rspfile);

#ifdef  DEBUG
    printf("gcc @%s\n", rspfilename);
#endif
    strcpy(command, "@");
    strcat(command, rspfilename);
    status = spawnlp(P_WAIT, "gcc", "gcc", command, NULL);
    unlink(rspfilename);

    if (status != 0) return status;
    if (!debug) {
#ifdef  DEBUG
        printf("strip %s\n", mainfile);
#endif
        spawnlp(P_WAIT, "strip", "strip", mainfile, NULL);
        }
#ifdef  DEBUG
    printf("coff2exe %s\n", mainfile);
#endif
    spawnlp(P_WAIT, "coff2exe", "coff2exe", mainfile, NULL);
    if (!debug) {
#ifdef  DEBUG
        printf("rm %s\n", mainfile);
#endif
        spawnlp(P_WAIT, "rm", "rm", mainfile, NULL);
        }

    return status;
}
