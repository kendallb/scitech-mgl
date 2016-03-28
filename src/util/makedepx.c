/****************************************************************************
*
*                                makedep
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
* Description:  Program to build dependencies lists for expanded assembler
*               files.
*
*               Written specifically for use with the DMAKE program.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pmapi.h"
#include "cmdline.h"
#ifdef  __LINUX__
#include <unistd.h>
#endif

#define MAX_SRC_DIR     50
#define MAX_INC_DIR     50
#define MAX_FILES       1000
#define MAX_DEPEND      1000
#define LINESIZE        256
#define INCLUDE_VAR     "INCLUDE"
#define ASM_INC         "include"
#define ASM_INC_LEN     7
#define C_INC           "#include"
#define C_INC_LEN       8

/*------------------------- Global variables ------------------------------*/

char    sourceFile[_MAX_PATH];
char    expandedFile[_MAX_PATH];
int     incnum = 0;
char    incdir[MAX_SRC_DIR][_MAX_PATH];
int     srcnum = 1;
char    srcdir[MAX_SRC_DIR][_MAX_PATH] = {""};
int     numDepend;
char    dependList[MAX_DEPEND][_MAX_PATH];
char    *srcexts[] = {".c",".cc",".cpp",".cxx",".asm"};
int     type;
ibool   ignoreSys;
ibool   relativePaths;
ibool   debug = false;

#define NUMEXT  (sizeof(srcexts) / sizeof(char*))

/*-------------------------- Implementation -------------------------------*/

#ifdef  __UNIX__
/* Unix file systems are case sensitive */
#define stricmp     strcasecmp
#define strnicmp    strncasecmp
#define LWR_FILENAME(s)
#define UPR_FILENAME(s)
#define SEPARATOR   '/'
#else
#define LWR_FILENAME(s) strlwr(s)
#define UPR_FILENAME(s) strupr(s)
#define SEPARATOR   '\\'
#endif

void convertSeparator(char *s)
{
    while (*s) {
#ifdef  __UNIX__
        if (*s == '\\')
            *s = '/';
#else
        if (*s == '/')
            *s = '\\';
#endif
        s++;
        }
}

int openfile(FILE **in,char *filename,char *mode)
{
    if ((*in = fopen(filename,mode) ) == NULL)
        return false;
    else
        return true;
}

void error(char *msg)
{
    fprintf(stderr,"FATAL: %s\n", msg);
    exit(1);
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

void readfilenames(
    char *name,
    char filenames[MAX_FILES][PM_MAX_PATH],
    int *numfiles)
{
    char    buf[PM_MAX_PATH];
    FILE    *f;

    *numfiles = 0;
    if (!openfile(&f,name,"r")) {
        /* Unable to open the file - simply return 0 for number of files
         * to process
         */
        return;
        }
    while (!feof(f) && (fscanf(f," %s ",buf) == 1)) {
        strlwr(buf);
        strcpy(filenames[*numfiles],buf);
        (*numfiles)++;
        }
    fclose(f);
}

char *readrsp(char *name)
{
    FILE        *f;
    static char rspbuf[512];

    if (!openfile(&f,name,"r"))
        error("Unable to open response file!\n");
    fgets(rspbuf,512,f);
    fclose(f);
    return rspbuf;
}

void help(void)
{
    printf("Usage: makedepx [-arsSIu @rspfile] <expfile> <asmfile>\n\n");
    printf("\n");
    printf("Options are:\n");
    printf("\n");
    printf("  -a<file> Append the output to the specified file (stdout is default)\n");
    printf("  -r       Generate relative pathnames for dependencies (not system includes)\n");
    printf("  -s       Ignore all system include files (eg: <stdio.h>)\n");
    printf("  -S<dir>  List of directories to search for source files (-S@rsp also valid)\n");
    printf("  -I<inc>  List of directories to search for include files (-I@rsp also valid)\n");
    printf("  -u       Generate dependencies in all uppercase (lowercase is default)\n");
    exit(1);
}

int getSrcName(char *srcname,char *dependname)
/****************************************************************************
*
* Function:     getSrcName
* Parameters:   srcname     - Place to store the source file name
*               dependname  - Dependent file name (.obj) being used
* Returns:      0 for C files, 1 for assembler files, -1 if not found.
*
* Description:  Attempts to locate the source file for the specified
*               object file dependency and return the pathname. If one
*               cannot be found we bomb out.
*
****************************************************************************/
{
    int         i,j;
    char        tmp[PM_MAX_PATH],drive[PM_MAX_DRIVE],dir[PM_MAX_PATH];
    char        name[PM_MAX_PATH],ext[PM_MAX_PATH];
    PM_findData findData;
    void        *hfile;

    findData.dwSize = sizeof(findData);
    PM_splitpath(dependname,drive,dir,name,ext);
    if (stricmp(ext,".res") == 0) {
        for (i = 0; i < srcnum; i++) {
            PM_makepath(tmp,"",srcdir[i],name,".rc");
            if (debug)
                fprintf(stderr,"Searching for %s ... \n", tmp);
            if ((hfile = PM_findFirstFile(tmp,&findData)) != PM_FILE_INVALID) {
                PM_findClose(hfile);
                strcpy(srcname, tmp);
                if (debug)
                    fprintf(stderr,"found\n");
                return 0;
                }
            if (debug)
                fprintf(stderr,"not found\n");
            }
        }
    else {
        for (i = 0; i < srcnum; i++) {
            for (j = 0; j < NUMEXT; j++) {
                PM_makepath(tmp,"",srcdir[i],name,srcexts[j]);
                if (debug)
                    fprintf(stderr,"Searching for %s ... \n", tmp);
                if ((hfile = PM_findFirstFile(tmp,&findData)) != PM_FILE_INVALID) {
                    PM_findClose(hfile);
                    strcpy(srcname, tmp);
                    if (debug)
                        fprintf(stderr,"found\n");
                    return (srcexts[j][1] == 'a');
                    }
                if (debug)
                    fprintf(stderr,"not found\n");
                }
            }
        }
    fprintf(stderr,"Unable to locate dependencies for %s.\n", dependname);
    return -1;
}

void addIncName(char *incname,ibool sysInclude)
/****************************************************************************
*
* Function:     addIncName
* Parameters:   incname     - Name of include file to add
*               sysInclude  - True if it is a system include file
*
* Description:  Attempts to search for the specified include file and
*               builds the full pathname to it.
*
****************************************************************************/
{
    int         i,j;
    char        tmp[PM_MAX_PATH],cwd[PM_MAX_PATH],drive[PM_MAX_DRIVE];
    char        dir[PM_MAX_PATH],name[PM_MAX_PATH],ext[PM_MAX_PATH];
    char        cdrive[PM_MAX_DRIVE],cdir[PM_MAX_PATH],cname[PM_MAX_PATH];
    char        cext[PM_MAX_PATH],tdir[PM_MAX_PATH],*p,*cp;
    PM_findData findData;
    void        *hfile;

    findData.dwSize = sizeof(findData);
    strlwr(incname);
    for (i = (sysInclude ? 1 : 0); i < incnum; i++) {
        strcpy(tmp,incdir[i]);
        strcat(tmp,incname);
        if (debug)
            fprintf(stderr,"Searching for %s ... \n", tmp);
        if ((hfile = PM_findFirstFile(tmp,&findData)) != PM_FILE_INVALID) {
            PM_findClose(hfile);
            if (debug)
                fprintf(stderr,"found\n");
            if (relativePaths && !sysInclude) {
                PM_splitpath(tmp,drive,dir,name,ext);
                PM_getCurrentPath(cwd,_MAX_PATH);
                strlwr(cwd);
                PM_backslash(cwd);
                PM_splitpath(cwd,cdrive,cdir,cname,cext);
                if (drive[0] == cdrive[0]) {
                    p = dir;
                    cp = cdir;
                    /* Skip common directory paths */
                    while (*p && *cp  && (*p == *cp)) {
                        p++; cp++;
                        }
                    /* Skip back to start of uncommon directory name */
                    while (*(p-1) != '\\') {
                        p--;
                        cp--;
                        }
                    strcpy(tdir,p);
                    p = cdir;
                    while ((cp = strchr(cp,'\\')) != NULL) {
                        *p++ = '.';
                        *p++ = '.';
                        *p++ = '\\';
                        cp++;
                        }
                    *p = 0;
                    strcat(cdir,tdir);
                    PM_makepath(tmp,NULL,cdir,name,ext);
                    }
                }
            for (j = 0; j < numDepend; j++)
                if (strcmp(dependList[j],tmp) == 0)
                    return;
            if (numDepend == MAX_DEPEND)
                error("Too many dependant files!");
            strcpy(dependList[numDepend++],tmp);
            return;
            }
        if (debug)
            fprintf(stderr,"not found\n");
        }

    fprintf(stderr,"Unable to locate include file %s.\n", incname);
}

void buildDependList(char *srcname)
/****************************************************************************
*
* Function:     buildDependList
* Parameters:   srcname         - Name of source file to build dependencies
*
* Description:  Builds the list of dependencies for the source file.
*
****************************************************************************/
{
    FILE    *f;
    int     i,first,last,sysInclude;
    char    line[LINESIZE],*s,*e;

    if (!openfile(&f,srcname,"r")) {
        printf("Unable to open source file %s!!", srcname);
        exit(1);
        }

    first = numDepend;
    while (fgets(line,LINESIZE,f)) {
        if ((s = skipwhite(line)) == NULL)
            continue;

        if (type) {             /* Assembler source file        */
            if (strnicmp(s,ASM_INC,ASM_INC_LEN) == 0) {
                if ((s = skipwhite(s + ASM_INC_LEN)) == NULL)
                    continue;
                if (*s == '"')
                    s++;
                if ((e = skiptowhite(s)) == NULL)
                    continue;
                if (*(e-1) == '"')
                    e--;
                *e = 0;
                addIncName(s,false);
                }
            }
        else {                  /* C style source file          */
            if (strnicmp(s,C_INC,C_INC_LEN) == 0) {
                if ((s = skipwhite(s + C_INC_LEN)) == NULL)
                    continue;
                if (*s == '<') {
                    if (ignoreSys)
                        continue;
                    sysInclude = true;
                    }
                else sysInclude = false;
                s++;
                if ((e = skiptowhite(s)) == NULL)
                    continue;
                e--;
                *e = 0;
                addIncName(s,sysInclude);
                }
            }
        }
    last = numDepend;
    fclose(f);

    /* Recursively parse each include file found */
    if (first < last) {
        for (i = first; i < last; i++)
            buildDependList(dependList[i]);
        }
}

void processFiles(FILE *out,char sourceFile[],char expandedFile[],
    ibool uppercase)
/****************************************************************************
*
* Function:     processFiles
* Parameters:   out             - File to send output list to
*
* Description:  Processes the list of files generating the list of
*               dependencies in the output list.
*
****************************************************************************/
{
    int     j;
    char    srcname[_MAX_PATH];

    if ((type = getSrcName(srcname,sourceFile)) == -1)
        return;
    numDepend = 0;
    buildDependList(srcname);

    /* Last one in list is the original source file */
    strcpy(dependList[numDepend++],srcname);

    /* Convert to uppercase or lower case as needed */
    if (uppercase) {
        strupr(sourceFile);
        for (j = 0; j < numDepend; j++)
            strupr(dependList[j]);
        }

    /* Write the list of dependant files to the output file */
    for (j = 0; j < numDepend; j++) {
        convertSeparator(dependList[j]);
        fprintf(out, "%s: %s\n", expandedFile,dependList[j]);
        }
}

int main(int argc,char *argv[])
{
    int     option;
    char    *argument,*s,*d;
    char    tmp[255];
    FILE    *out = stdout;
    ibool   uppercase;

    uppercase = ignoreSys = relativePaths = false;

    /* Read include file directories from the INCLUDE evironment var */

    PM_getCurrentPath(tmp,_MAX_PATH);
    strlwr(tmp);
    strcpy(incdir[incnum],tmp);
    PM_backslash(incdir[incnum++]);
    if (getenv(INCLUDE_VAR)) {
        strcpy(tmp,getenv(INCLUDE_VAR));
        strlwr(tmp);
        s = tmp;
        while ((d = strchr(s,';')) != NULL) {
            *d++ = '\0';
            strcpy(incdir[incnum],s);
            PM_backslash(incdir[incnum++]);
            s = d;
            }
        strcpy(incdir[incnum],s);
        PM_backslash(incdir[incnum++]);
        }

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"a:rsS:I:uD",&argument);
        switch(option) {
            case 'a':
                if (!openfile(&out,argument,"at"))
                    error("Unable to open output file");
                break;
            case 'r':   relativePaths = true;       break;
            case 's':   ignoreSys = true;           break;
            case 'u':   uppercase = true;           break;
            case 'D':   debug = true;               break;
            case 'S':
                if (argument[0] == '@')
                    argument = readrsp(argument+1);
                strlwr(argument);
                s = argument;
                while ((d = strchr(s,';')) != NULL) {
                    *d++ = '\0';
                    strcpy(srcdir[srcnum],s);
                    PM_backslash(srcdir[srcnum++]);
                    s = d;
                    }
                strcpy(srcdir[srcnum],s);
                PM_backslash(srcdir[srcnum++]);
                break;
            case 'I':
                if (argument[0] == '@')
                    argument = readrsp(argument+1);
                strlwr(argument);
                s = argument;
                while ((d = strchr(s,';')) != NULL) {
                    *d++ = '\0';
                    strcpy(incdir[incnum],s);
                    PM_backslash(incdir[incnum++]);
                    s = d;
                    }
                strcpy(incdir[incnum],s);
                PM_backslash(incdir[incnum++]);
                break;
            case INVALID:
                help();
            }
        } while (option != ALLDONE && option != PARAMETER);

    if ((argc - nextargv) < 2)
        help();

    /* Read files to work with. The first file is the expanded source
     * filename and the second file is the non-expanded one.
     */
    strcpy(sourceFile,argv[nextargv+1]);
    strlwr(sourceFile);
    strcpy(expandedFile,argv[nextargv]);
    strlwr(expandedFile);

    processFiles(out,sourceFile,expandedFile,uppercase);

    if (out != stdout)
        fclose(out);
    return 0;
}
