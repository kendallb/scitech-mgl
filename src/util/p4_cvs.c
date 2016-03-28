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
* Description:  Program to revert all CVS status files from a Perforce
*               checkin list.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include "pmapi.h"

/*------------------------- Global variables ------------------------------*/

#define CVS_DEPOT_ROOT  "//depot/cvs/"
#define CVS_DEPOT_LEN   (sizeof(CVS_DEPOT_ROOT)-1)
#define CVS_ROOT        "c:\\cvs\\"
#define logName "p4_cvs.log"

char    logPath[PM_MAX_PATH];
char    tempPath[PM_MAX_PATH];
char    tempPath2[PM_MAX_PATH];

/*-------------------------- Implementation -------------------------------*/

void recurseCollectFiles(
    FILE *log,
    const char *path,
    const char *fileMask,
    ibool logCVS)
{
    PM_findData findData;
    void        *hfile;
    int         done;
    char        files[PM_MAX_PATH];
    char        name[PM_MAX_PATH];

    // First dump all matching files
    findData.dwSize = sizeof(findData);
    strcpy(files,path);
    if (strlen(path) > 0)
        PM_backslash(files);
#ifndef __UNIX__
    if (strcmp(fileMask,"*") == 0)
        strcat(files,"*.*");
    else
#endif
        strcat(files,fileMask);
    done = (hfile = PM_findFirstFile(files,&findData)) == PM_FILE_INVALID;
    while (!done) {
        strcpy(name,path);
        if (strlen(path) > 0)
            PM_backslash(name);
        strcat(name,findData.name);
        if (!(findData.attrib & PM_FILE_DIRECTORY))
            fprintf(log,"%s\n", name);
        done = !PM_findNextFile(hfile,&findData);
        }
    if (hfile != PM_FILE_INVALID)
        PM_findClose(hfile);

    // Now recurse all subdirectories
    strcpy(files,path);
    if (strlen(path) > 0)
        PM_backslash(files);
#ifdef  __UNIX__
    strcat(files,"*");
#else
    strcat(files,"*.*");
#endif
    done = (hfile = PM_findFirstFile(files,&findData)) == PM_FILE_INVALID;
    while (!done) {
        if ((strcmp(findData.name,".") != 0) && (strcmp(findData.name,"..") != 0)) {
            if (findData.attrib & PM_FILE_DIRECTORY) {
                strcpy(name,path);
                if (strlen(path) > 0)
                    PM_backslash(name);
                strcat(name,findData.name);
                if (logCVS && (strcmp(findData.name,"CVS") == 0 ||
                        strcmp(findData.name,"CVSROOT") == 0)) {
                    recurseCollectFiles(log,name,"*",false);
                    }
                else
                    recurseCollectFiles(log,name,fileMask,logCVS);
                }
            }
        done = !PM_findNextFile(hfile,&findData);
        }
}

void collectFiles(
    const char *mask,
    ibool clear,
    ibool logCVS)
{
    FILE    *log;

    if ((log = fopen(tempPath,clear ? "w" : "a+")) == NULL)
        PM_fatalError("Unable to open temporary file");
    recurseCollectFiles(log,"",mask,logCVS);
    fclose(log);
}

void findRemovedFiles(
    const char *path)
{
    FILE    *log,*f;
    char    buf[PM_MAX_PATH];
    char    buf2[PM_MAX_PATH];
    char    *p;
    int     i,len;

    if ((f = fopen(path,"r")) == NULL)
        PM_fatalError("Unable to open temporary file");
    if ((log = fopen(tempPath,"w")) == NULL)
        PM_fatalError("Unable to open temporary file");
    while (!feof(f)) {
        if (fgets(buf,sizeof(buf),f) && (len = strlen(buf)) > CVS_DEPOT_LEN) {
            p = buf+CVS_DEPOT_LEN;  // Skip past //depot//cvs/
            len -= CVS_DEPOT_LEN;
            if (p[len-1] == '\n')
                p[--len] = 0;
#ifndef __UNIX__
            for (i = 0; i < len; i++) {
                if (p[i] == '/')
                    p[i] = '\\';
                }
#endif
            for (i = 0; i < len; i++) {
                if (p[i] == '#') {
                    p[i] = 0;
                    break;
                    }
                }
            strcpy(buf2,CVS_ROOT);
            PM_backslash(buf2);
            strcat(buf2,p);
            if (access(buf2,0) != 0)
                fprintf(log,"%s\n", buf2);
            }
        }
    fclose(log);
    fclose(f);
}

int main(int argc,char *argv[])
{
    char    *p,*branch;
    char    command[PM_MAX_PATH];
    char    drive[PM_MAX_DRIVE];
    char    dir[PM_MAX_PATH];
    char    name[PM_MAX_PATH];
    char    ext[PM_MAX_PATH];
    ibool   checkout = false;

    if (argc >= 2 && stricmp(argv[1],"-c") == 0) {
        checkout = true;
        argc--;
        argv++;
        }
    if (argc != 2 && argc != 3) {
        printf("Usage: p4_cvs [-c] <cvs command> <cvs branch>\n");
        return 0;
        }
    if (argc == 3)
        branch = argv[2];
    else
        branch = NULL;
    if ((p = getenv("TEMP")) == NULL) {
        if ((p = getenv("TMP")) == NULL)
            PM_fatalError("Unable to find temporary directory!");
        }
    PM_splitpath(p,drive,dir,name,ext);
    PM_backslash(dir);
    strcat(dir,name);
    PM_splitpath(tmpnam(NULL),NULL,NULL,name,ext);
    PM_makepath(tempPath,drive,dir,name,ext);
    PM_splitpath(tmpnam(NULL),NULL,NULL,name,ext);
    PM_makepath(tempPath2,drive,dir,name,ext);
    PM_splitpath(logName,NULL,NULL,name,ext);
    PM_makepath(logPath,drive,dir,name,ext);
#ifndef __UNIX__
    strlwr(tempPath);
    strlwr(tempPath2);
    strlwr(logPath);
#endif
    remove(logPath);

    printf("Opening CVS files in Perforce ... ");
    fflush(stdout);
    collectFiles("*",true,false);
    sprintf(command,"p4 -x %s open >>& %s",tempPath,logPath);
    system(command);
    printf("Done\n");
    fflush(stdout);

    if (checkout) {
        printf("Removing Perforce files ... ");
        fflush(stdout);
        sprintf(command,"k_rm -rf *",tempPath,logPath);
        system(command);
        printf("Done\n");
        fflush(stdout);
        }

    printf("Syncing to latest CVS files ... ");
    fflush(stdout);
    if (checkout) {
        if (branch)
            sprintf(command,"%s checkout -r %s >>& %s",argv[1],branch,logPath);
        else
            sprintf(command,"%s checkout . >>& %s",argv[1],logPath);
        }
    else {
        if (branch)
            sprintf(command,"%s update -r %s >>& %s",argv[1],branch,logPath);
        else
            sprintf(command,"%s update . >>& %s",argv[1],logPath);
        }
    system(command);
    printf("Done\n");
    fflush(stdout);

    printf("Adding new files to Perforce ... ");
    fflush(stdout);
    collectFiles("*",true,false);
    sprintf(command,"p4 -x %s add >>& %s",tempPath,logPath);
    system(command);
    printf("Done\n");
    fflush(stdout);

    printf("Reverting CVS special files ... ");
    fflush(stdout);
    collectFiles(".cvsignore",true,true);
    sprintf(command,"p4 -x %s revert >>& %s",tempPath,logPath);
    system(command);
    printf("Done\n");
    fflush(stdout);

    printf("Reverting unchanged files ... ");
    fflush(stdout);
    sprintf(command,"p4 diff -sr | p4 -x - revert >>& %s",logPath);
    system(command);
    printf("Done\n");
    fflush(stdout);
    remove(tempPath);

    printf("Deleting removed files ... ");
    fflush(stdout);
    sprintf(command,"p4 opened >>& %s",tempPath2);
    system(command);
    findRemovedFiles(tempPath2);
    sprintf(command,"p4 -x %s revert >>& %s",tempPath,logPath);
    system(command);
    sprintf(command,"p4 -x %s delete >>& %s",tempPath,logPath);
    system(command);
    printf("Done\n");
    fflush(stdout);

    // Clean up and exit
    remove(tempPath);
    remove(tempPath2);
    return 0;
}
