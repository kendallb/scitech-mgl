/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  Any
*
* Description:  Module containing Unix I/O functions.
*
****************************************************************************/

#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

/*----------------------------- Implementation ----------------------------*/

/* {secret} */
typedef struct {
    DIR     *d;
    char    path[PM_MAX_PATH];
    char    mask[PM_MAX_PATH];
    } PM_findHandle;

/****************************************************************************
REMARKS:
Internal function to convert the find data to the generic interface.
****************************************************************************/
static void convertFindData(
    PM_findData *findData,
    struct dirent *blk,
    const char *path)
{
    ulong       dwSize = findData->dwSize;
    struct stat st;
    char        filename[PM_MAX_PATH];

    memset(findData,0,findData->dwSize);
    findData->dwSize = dwSize;
    strcpy(filename,path);
    PM_backslash(filename);
    strcat(filename,blk->d_name);
    stat(filename,&st);
    if (!(st.st_mode & S_IWRITE))
        findData->attrib |= PM_FILE_READONLY;
    if (st.st_mode & S_IFDIR)
        findData->attrib |= PM_FILE_DIRECTORY;
    findData->sizeLo = st.st_size;
    findData->sizeHi = 0;
    strncpy(findData->name,blk->d_name,PM_MAX_PATH);
    findData->name[PM_MAX_PATH-1] = 0;
}

/****************************************************************************
REMARKS:
Determines if a file name matches the passed in pattern.
****************************************************************************/
static ibool filematch(
    char *pattern,
    char *dirpath,
    struct dirent *dire)
{
    struct stat st;
    int         i = 0,j = 0,lastchar = '\0';
    char        fullpath[PM_MAX_PATH];

    strcpy(fullpath,dirpath);
    PM_backslash(fullpath);
    strcat(fullpath, dire->d_name);
    if (stat(fullpath, &st) != 0)
        return false;
    for (; i < (int)strlen(dire->d_name) && j < (int)strlen(pattern); i++, j++) {
        if (pattern[j] == '*' && lastchar != '\\') {
            if (pattern[j+1] == '\0')
                return true;
            while (dire->d_name[i++] != pattern[j+1]) {
                if (dire->d_name[i] == '\0')
                    return false;
                }
            i -= 2;
            }
        else if (dire->d_name[i] != pattern[j] &&
                !(pattern[j] == '?' && lastchar != '\\'))
            return false;
        lastchar = pattern[i];
        }
    if (j == (int)strlen(pattern) && i == (int)strlen(dire->d_name))
        return true;
    return false;
}

/****************************************************************************
REMARKS:
Function to find the first file matching a search criteria in a directory.
****************************************************************************/
void * PMAPI PM_findFirstFile(
    const char *filename,
    PM_findData *findData)
{
    PM_findHandle   *d;
    struct dirent   *dire;
    char            name[PM_MAX_PATH];
    char            ext[PM_MAX_PATH];

    if ((d = PM_malloc(sizeof(*d))) == NULL)
        return PM_FILE_INVALID;
    PM_splitpath(filename,NULL,d->path,name,ext);
    strcpy(d->mask,name);
    strcat(d->mask,ext);
    if (strlen(d->path) == 0)
        strcpy(d->path, ".");
    if (d->path[strlen(d->path)-1] == '/')
        d->path[strlen(d->path)-1] = 0;
    if ((d->d = opendir(d->path)) != NULL) {
        while ((dire = readdir(d->d)) != NULL) {
            if (filematch(d->mask,d->path,dire)) {
                convertFindData(findData,dire,d->path);
                return d;
                }
            }
        closedir(d->d);
        }
    PM_free(d);
    return PM_FILE_INVALID;
}

/****************************************************************************
REMARKS:
Function to find the next file matching a search criteria in a directory.
****************************************************************************/
ibool PMAPI PM_findNextFile(
    void *handle,
    PM_findData *findData)
{
    PM_findHandle   *d = handle;
    struct dirent   *dire;

    while ((dire = readdir(d->d)) != NULL) {
        if (filematch(d->mask,d->path,dire)) {
            convertFindData(findData,dire,d->path);
            return true;
            }
        }
    return false;
}

/****************************************************************************
REMARKS:
Function to close the find process
****************************************************************************/
void PMAPI PM_findClose(
    void *handle)
{
    PM_findHandle   *d = handle;

    closedir(d->d);
    free(d);
}

/****************************************************************************
REMARKS:
Function to get the current working directory for the specififed drive.
Under Unix this will always return the current working directory regardless
of what the value of 'drive' is.
****************************************************************************/
void PMAPI PM_getdcwd(
    int drive,
    char *dir,
    int len)
{
    (void)drive;
    getcwd(dir,len);
}

/****************************************************************************
REMARKS:
Function to change the file attributes for a specific file.
****************************************************************************/
void PMAPI PM_setFileAttr(
    const char *filename,
    uint attrib)
{
    struct stat st;
    mode_t      mode;

    stat(filename,&st);
    mode = st.st_mode;
    if (attrib & PM_FILE_READONLY)
        mode &= ~S_IWRITE;
    else
        mode |= S_IWRITE;
    chmod(filename,mode);
}

/****************************************************************************
REMARKS:
Function to get the file attributes for a specific file.
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    struct stat st;

    stat(filename,&st);
    if (st.st_mode & S_IWRITE)
        return 0;
    return PM_FILE_READONLY;
}

/****************************************************************************
REMARKS:
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
    return mkdir(filename,0x1FF) == 0;
}

/****************************************************************************
REMARKS:
Function to remove a directory.
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
    return rmdir(filename) == 0;
}

/****************************************************************************
REMARKS:
Function to get the file time and date for a specific file.
****************************************************************************/
ibool PMAPI PM_getFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    // TODO: Implement this!
    (void)filename;
    (void)gmTime;
    (void)time;
    PM_fatalError("PM_getFileTime not implemented yet!");
    return false;
}

/****************************************************************************
REMARKS:
Function to set the file time and date for a specific file.
****************************************************************************/
ibool PMAPI PM_setFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    // TODO: Implement this!
    (void)filename;
    (void)gmTime;
    (void)time;
    PM_fatalError("PM_setFileTime not implemented yet!");
    return false;
}
