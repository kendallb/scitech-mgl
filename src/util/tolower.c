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
* Environment:  Win32
*
* Description:  Simple program to rename all files and directories to
*               lowercase.
*
****************************************************************************/

#include "scitech.h"
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

/*------------------------- Global variables ------------------------------*/

int recursive = false;

/*-------------------------- Implementation -------------------------------*/

void help(void)
/****************************************************************************
*
* Function:     help
*
* Description:  Help provides usage information about our program if the
*               options do make any sense.
*
****************************************************************************/
{
    printf("Usage: tolower -s <files>]\n\n");
    printf("This program converst all filenames to lowercase. It can recurse all\n");
    printf("sub-directories if you specify the -s command line option.\n");
    exit(1);
}

void backslash(char *s)
/****************************************************************************
*
* Function:     backslash
* Parameters:   s   - String to add backslash to
*
* Description:  Appends a trailing '\' pathname separator if the string
*               currently does not have one appended.
*
****************************************************************************/
{
    uint pos = strlen(s);
    if (s[pos-1] != '\\') {
        s[pos] = '\\';
        s[pos+1] = '\0';
        }
}

void renamefiles(const char *path,const char *filename)
{
    int             done;
    char            files[_MAX_PATH];
    char            name[_MAX_PATH];
    char            lowername[_MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE          hfile;

    strcpy(files,path);
    strcat(files,filename);
    done = (hfile = FindFirstFile(files,&findData)) == INVALID_HANDLE_VALUE;
    while (!done) {
        if ((strcmp(findData.cFileName,".") != 0) && (strcmp(findData.cFileName,"..") != 0)) {
            /* Rename the file */
            strcpy(name,path);
            strcat(name,findData.cFileName);
            strcpy(lowername,name);
            strlwr(lowername);
            printf("%s -> %s\n", name, lowername);
            rename(name,lowername);

            /* If this is a directory, so recursively delete it also */
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (recursive) {
                    backslash(name);
                    renamefiles(name,filename);
                    }
                }
            }
        done = !FindNextFile(hfile,&findData);
        }
    if (hfile != INVALID_HANDLE_VALUE)
        FindClose(hfile);
}

void main(int argc,char *argv[])
{
    int     option;
    char    *argument;
    char    cwd[_MAX_PATH];

    /* Parse the command line for format specifier options */
    do {
        option = getcmdopt(argc,argv,"sS",&argument);
        if (option >= 0)
            option = tolower(option);
        switch(option) {
            case 's':
                recursive = true;
                break;
            case INVALID:
                fputs("Invalid option\a\n",stderr);
                exit(1);
                break;
            }
        } while (option != ALLDONE && option != PARAMETER);

    /* Ensure that we at least have on file to translate */
    if ((argc - nextargv) < 1)
        help();

    /* If the user has not provided the file names using the -f option,
     * we must get them from the command line...
     */
    GetCurrentDirectory(sizeof(cwd),cwd);
    backslash(cwd);
    while (nextargv < argc)
        renamefiles(cwd,argv[nextargv++]);
}
