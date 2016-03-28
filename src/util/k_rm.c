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
* Description:  Program to remove files and directories. This is similar
*               to the Unix rm program, except that internally it does
*               wildcard expansion so that it can be called from dmake
*               directly without needing to spawn a shell.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pmapi.h"

/*------------------------- Global variables ------------------------------*/

#define CLEAR_MASK  (PM_FILE_READONLY | PM_FILE_HIDDEN | PM_FILE_SYSTEM)

ibool   force = false;
ibool   recurse = false;

/*-------------------------- Implementation -------------------------------*/

void usage(void)
{
    printf("Usage: k_rm [-rf] <filename>\n");
    exit(0);
}

ibool removeFiles(
    const char *path,
    const char *fileMask)
{
    PM_findData findData;
    void        *hfile;
    int         done;
    char        files[PM_MAX_PATH];
    char        name[PM_MAX_PATH];

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
        if ((strcmp(findData.name,".") != 0) && (strcmp(findData.name,"..") != 0)) {
            strcpy(name,path);
            if (strlen(path) > 0)
                PM_backslash(name);
            strcat(name,findData.name);
            if (recurse && (findData.attrib & PM_FILE_DIRECTORY)) {
                /* This is a directory, so recursively delete it also */
                PM_findClose(hfile);
                if (stricmp(findData.name,fileMask) == 0) {
                    if (!removeFiles(name,"*"))
                        return false;
                    }
                else if (!removeFiles(name,fileMask))
                    return false;
                if (!PM_rmdir(name))
                    return false;
                done = (hfile = PM_findFirstFile(files,&findData)) == PM_FILE_INVALID;
                continue;
                }
            if (force && (findData.attrib & CLEAR_MASK))
                PM_setFileAttr(name,PM_FILE_NORMAL);
            remove(name);
            }
        done = !PM_findNextFile(hfile,&findData);
        }
    if (hfile != PM_FILE_INVALID)
        PM_findClose(hfile);
    return true;
}

int main(int argc,char *argv[])
{
    FILE    *f;
    char    drive[PM_MAX_DRIVE];
    char    dir[PM_MAX_PATH];
    char    name[PM_MAX_PATH];
    char    ext[PM_MAX_PATH];
    char    path[PM_MAX_PATH];
    char    mask[PM_MAX_PATH];
    char    buf[PM_MAX_PATH];

    if (argc == 1)
        usage();
    while (argc > 1) {
        if (stricmp(argv[1],"-f") == 0)
            force = true;
        else if (stricmp(argv[1],"-r") == 0)
            recurse = true;
        else if (stricmp(argv[1],"-rf") == 0) {
            force = true;
            recurse = true;
            }
        else if (stricmp(argv[1],"-h") == 0)
            usage();
        else if (stricmp(argv[1],"-S") == 0) {
            if ((f = fopen(argv[2],"r")) == NULL)
                return -1;
            while (!feof(f)) {
                if (fgets(buf,sizeof(buf),f)) {
                    if (buf[strlen(buf)-1] == '\n')
                        buf[strlen(buf)-1] = 0;
                    PM_splitpath(buf,drive,dir,name,ext);
                    PM_makepath(path,drive,dir,NULL,NULL);
                    PM_makepath(mask,NULL,NULL,name,ext);
                    removeFiles(path,mask);
                    }
                }
            fclose(f);
            argc--;
            argv++;
            }
        else {
            PM_splitpath(argv[1],drive,dir,name,ext);
            PM_makepath(path,drive,dir,NULL,NULL);
            PM_makepath(mask,NULL,NULL,name,ext);
            removeFiles(path,mask);
            }
        argc--;
        argv++;
        }
    return 0;
}
