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
* Description:  Module containing code common to all platforms.
*
****************************************************************************/

#include "pmapi.h"
#include "clib/os/os.h"
#if defined(__WIN32_VXD__) || defined(__OS2_VDD__) || defined(__NT_DRIVER__)
#include "pm_help.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oshdr.h"
#endif

/*---------------------------- Global variables ---------------------------*/

/* {secret} */
long _VARAPI    ___drv_os_type = _OS_UNSUPPORTED;
static char     localBPDPath[PM_MAX_PATH] = "";
/* {secret} */
PM_int86Funcs   __PM_biosFuncs = { sizeof(__PM_biosFuncs) };

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
DESCRIPTION:
Function to override the SNAP BPD driver path

HEADER:
pmapi.h

PARAMETERS:
path    - Local path to the SciTech SNAP BPD driver files.

REMARKS:
This function is used by the application program to override the location
of the SciTech SNAP driver files that are loaded. Normally the loader code
will look in the system SciTech SNAP directories first, then in the 'drivers'
directory relative to the current working directory, and finally relative
to the MGL_ROOT environment variable. By default the local BPD path is
always set to the current directory if not initialised.

SEE ALSO:
PM_findBPD
****************************************************************************/
void PMAPI PM_setLocalBPDPath(
    const char *path)
{
    PM_init();
    strncpy(localBPDPath,path,sizeof(localBPDPath));
    localBPDPath[sizeof(localBPDPath)-1] = 0;
}

/****************************************************************************
PARAMETERS:
bpdpath     - Place to store the actual path to the file
cachedpath  - Place to store the cached BPD driver path
trypath     - Path to try to find the BPD file in
subpath     - Optional sub path to append to trypath
dllname     - Name of the Binary Portable DLL to load

RETURNS:
True if found, false if not.

REMARKS:
Trys the specified path to see if the BPD file can be found or not. If so,
the path used is returned in bpdpath and cachedpath.
****************************************************************************/
static ibool TryPath(
    char *bpdpath,
    char *cachedpath,
    const char *trypath,
    const char *subpath,
    const char *dllname)
{
    char    filename[256];
    FILE    *f;

    strcpy(bpdpath, trypath);
    PM_backslash(bpdpath);
    strcat(bpdpath,subpath);
    PM_backslash(bpdpath);
    strcpy(filename,bpdpath);
    strcat(filename,dllname);
    if ((f = fopen(filename,"rb")) == NULL)
        return false;
    if (cachedpath)
        strcpy(cachedpath,bpdpath);
    fclose(f);
    return true;
}

/****************************************************************************
RETURNS:
True if local override enabled, false if not.

REMARKS:
Tests to see if the local override option is enabled, and if so it will
look for the SciTech SNAP drivers in the local application directories in
preference to the SciTech SNAP system directories.
****************************************************************************/
static ibool GetLocalOverride(void)
{
    char            filename[256];
    FILE            *f;
    static ibool    local_override = -1;

    if (local_override == -1) {
        local_override = false;
        strcpy(filename,PM_getSNAPPath());
        PM_backslash(filename);
        strcat(filename,"graphics.ini");
        if ((f = fopen(filename,"r")) != NULL) {
            while (!feof(f) && fgets(filename,sizeof(filename),f)) {
                if (strnicmp(filename,"uselocal",8) == 0) {
                    local_override = ((*(filename+9) - '0') == 1);
                    break;
                    }
                }
            fclose(f);
            }
        }
    return local_override;
}

/****************************************************************************
DESCRIPTION:
Function to find a BPD file on the SNAP driver file path

HEADER:
pmapi.h

PARAMETERS:
dllname - Name of the Binary Portable DLL to load
bpdpath - Place to store the actual path to the file

RETURNS:
True if found, false if not.

REMARKS:
Finds the location of a specific Binary Portable DLL, by searching all
the standard SciTech SNAP driver locations. If the file is found,
we cache the SNAP driver location internally and search for all drivers
relativ to this path for subsequent calls. Hence the first call to this
function should be to find a parent BPD file that defines the root
of all the BPD files in the installation (ie: graphics.bpd for the SNAP
Graphics API).

SEE ALSO:
PM_setLocalBPDPath
****************************************************************************/
ibool PMAPI PM_findBPD(
    const char *dllname,
    char *bpdpath)
{
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__)
    char        *mgl_root;
#endif
    static char cachedpath[PM_MAX_PATH] = "";

    /* On the first call determine the path to the SciTech SNAP drivers */
    if (cachedpath[0] == 0) {
        /* First try in the global system SciTech SNAP driver path if
         * the local override setting is not enabled.
         */
        PM_init();
        if (localBPDPath[0] != 0) {
            if (TryPath(bpdpath,cachedpath,localBPDPath,"",dllname))
                return true;
            }
        else if (!GetLocalOverride()) {
            if (TryPath(bpdpath,cachedpath,PM_getSNAPPath(),"",dllname))
                return true;
            }

        /* Next try via the MGL_ROOT environment variable for developer
         * platforms. Note that the development binaries live in different
         * directories for different CPU architectures, so we make sure
         * we handle that here.
         */
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__)
        if ((mgl_root = getenv("MGL_ROOT")) != NULL) {
            char drivers[PM_MAX_PATH] = "drivers";
            PM_backslash(drivers);
            strcat(drivers,PM_CPU_SUBPATH);
            if (TryPath(bpdpath,cachedpath,mgl_root,drivers,dllname))
                return true;
            }
#endif

        /* Next try in the local application directory if available */
        PM_getCurrentPath(bpdpath,PM_MAX_PATH);
        if (TryPath(bpdpath,cachedpath,bpdpath,"drivers",dllname))
            return true;

        /* Finally try in the global system path again so that we
         * will still find the drivers in the global system path if
         * the local override option is on, but the application does
         * not have any local override drivers.
         */
        if (TryPath(bpdpath,cachedpath,PM_getSNAPPath(),"",dllname))
            return true;

        /* Whoops, we can't find the BPD file! */
        return false;
        }

    /* Always try in the previously discovered path */
    return TryPath(bpdpath,NULL,cachedpath,"",dllname);
}

/****************************************************************************
REMARKS:
Copies a string into another, and returns dest + strlen(src).
****************************************************************************/
static char *_stpcpy(
    char *_dest,
    const char *_src)
{
    if (!_dest || !_src)
        return 0;
    while ((*_dest++ = *_src++) != 0)
        ;
    return --_dest;
}

/****************************************************************************
REMARKS:
Copies a string into another, stopping at the maximum length. The string
is properly terminated (unlike strncpy).
****************************************************************************/
static void safe_strncpy(
    char *dst,
    const char *src,
    unsigned maxlen)
{
    if (dst) {
        if(strlen(src) >= maxlen) {
            strncpy(dst, src, maxlen);
            dst[maxlen] = 0;
            }
        else
            strcpy(dst, src);
        }
}

/****************************************************************************
REMARKS:
Determins if the dot separator is present in the string.
****************************************************************************/
static int findDot(
    char *p)
{
    if (*(p-1) == '.')
        p--;
    switch (*--p) {
        case ':':
            if (*(p-2) != '\0')
                break;
        case '/':
        case '\\':
        case '\0':
            return true;
        }
    return false;
}

/****************************************************************************
DESCRIPTION:
Make a full pathname from split components.

HEADER:
pmapi.h

PARAMETERS:
path    - Place to store full path
drive   - Drive component for path
dir     - Directory component for path
name    - Filename component for path
ext     - Extension component for path

REMARKS:
Function to make a full pathname from split components. Under Unix the
drive component will usually be empty. If the drive, dir, name, or ext
parameters are null or empty, they are not inserted in the path string.
Otherwise, if the drive doesn't end with a colon, one is inserted in the
path. If the dir doesn't end in a slash, one is inserted in the path.
If the ext doesn't start with a dot, one is inserted in the path.

The maximum sizes for the path string is given by the constant PM_MAX_PATH,
which includes space for the null-terminator.

SEE ALSO:
PM_splitPath
****************************************************************************/
void PMAPI PM_makepath(
    char *path,
    const char *drive,
    const char *dir,
    const char *name,
    const char *ext)
{
    if (drive && *drive) {
        *path++ = *drive;
        *path++ = ':';
        }
    if (dir && *dir) {
        path = _stpcpy(path,dir);
        if (*(path-1) != '\\' && *(path-1) != '/')
#ifdef  __UNIX__
            *path++ = '/';
#else
            *path++ = '\\';
#endif
        }
    if (name)
        path = _stpcpy(path,name);
    if (ext && *ext) {
        if (*ext != '.')
            *path++ = '.';
        path = _stpcpy(path,ext);
        }
    *path = 0;
}

/****************************************************************************
DESCRIPTION:
Split a full pathname into components.

HEADER:
pmapi.h

PARAMETERS:
path    - Full path to split
drive   - Drive component for path
dir     - Directory component for path
name    - Filename component for path
ext     - Extension component for path

RETURNS:
Flags indicating what components were parsed (PMSplitPathFlags)

REMARKS:
Function to split a full pathmame into separate components in the form

    X:\DIR\SUBDIR\NAME.EXT

and splits path into its four components. It then stores those components
in the strings pointed to by drive, dir, name and ext. (Each component is
required but can be a NULL, which means the corresponding component will be
parsed but not stored).

The maximum sizes for these strings are given by the constants PM_MAX_DRIVE
and PM_MAX_PATH. PM_MAX_DRIVE is always 4, and PM_MAX_PATH is usually at
least 256 characters. Under Unix the dir, name and ext components may be
up to the full path in length.

SEE ALSO:
PM_makePath
****************************************************************************/
int PMAPI PM_splitpath(
    const char *path,
    char *drive,
    char *dir,
    char *name,
    char *ext)
{
    char    *p;
    int     temp,ret;
    char    buf[PM_MAX_PATH+2];

    /* Set all string to default value zero */
    ret = 0;
    if (drive)  *drive = 0;
    if (dir)    *dir = 0;
    if (name)   *name = 0;
    if (ext)    *ext = 0;

    /* Copy filename into template up to PM_MAX_PATH characters */
    p = buf;
    if ((temp = strlen(path)) > PM_MAX_PATH)
        temp = PM_MAX_PATH;
    *p++ = 0;
    strncpy(p, path, temp);
    *(p += temp) = 0;

    /* Split the filename and fill corresponding nonzero pointers */
    temp = 0;
    for (;;) {
        switch (*--p) {
            case '.':
                if (!temp && (*(p+1) == '\0'))
                    temp = findDot(p);
                if ((!temp) && ((ret & PM_HAS_EXTENSION) == 0)) {
                    ret |= PM_HAS_EXTENSION;
                    safe_strncpy(ext, p, PM_MAX_PATH - 1);
                    *p = 0;
                    }
                continue;
            case ':':
                if (p != &buf[2])
                    continue;
            case '\0':
                if (temp) {
                    if (*++p)
                        ret |= PM_HAS_DIRECTORY;
                    safe_strncpy(dir, p, PM_MAX_PATH - 1);
                    *p-- = 0;
                    break;
                    }
            case '/':
            case '\\':
                if (!temp) {
                    temp++;
                    if (*++p)
                        ret |= PM_HAS_FILENAME;
                    safe_strncpy(name, p, PM_MAX_PATH - 1);
                    *p-- = 0;
                    if (*p == 0 || (*p == ':' && p == &buf[2]))
                        break;
                    }
                continue;
            case '*':
            case '?':
                if (!temp)
                    ret |= PM_HAS_WILDCARDS;
            default:
                continue;
            }
        break;
        }
    if (*p == ':') {
        if (buf[1])
            ret |= PM_HAS_DRIVE;
        safe_strncpy(drive, &buf[1], PM_MAX_DRIVE - 1);
        }
    return ret;
}

/****************************************************************************
DESCRIPTION:
Block until a specific time has elapsed since the last call

HEADER:
pmapi.h

PARAMETERS:
milliseconds    - Number of milliseconds for delay

REMARKS:
This function will block the calling thread or process until the specified
number of milliseconds have passed since the /last/ call to this function.
The first time this function is called, it will return immediately. On
subsquent calls it will block until the specified time has elapsed, or it
will return immediately if the time has already elapsed.

This function is useful to provide constant time functionality in a
program, such as a frame rate limiter for graphics applications etc.

SEE ALSO:
PM_sleep
****************************************************************************/
void PMAPI PM_blockUntilTimeout(
    ulong milliseconds)
{
    ulong                   microseconds = milliseconds * 1000L,msDelay;
    static LZTimerObject    tm;
    static ibool            firstTime = true;

    if (firstTime) {
        firstTime = false;
        LZTimerOnExt(&tm);
        }
    else {
        if ((msDelay = (microseconds - LZTimerLapExt(&tm)) / 1000L) > 0)
            PM_sleep(msDelay);
        while (LZTimerLapExt(&tm) < microseconds)
            ;
        LZTimerOffExt(&tm);
        LZTimerOnExt(&tm);
        }
}

/****************************************************************************
DESCRIPTION:
Registers the BIOS emulator fallback functions with the PM library

HEADER:
pmapi.h

REMARKS:
This function is used internally by the SciTech SNAP Graphics subsystem
to register the BIOS emulator functions with the PM library. This is used on
non-x86 machines so that the PM library may interface with the VESA BIOS
on the installed graphics card after the SNAP drivers have initialised
the BIOS emulator.
****************************************************************************/
void PMAPI PM_registerBIOSEmulator(
    PM_int86Funcs *biosFuncs)
{
    memset((uchar*)&__PM_biosFuncs + sizeof(__PM_biosFuncs.dwSize),0,__PM_biosFuncs.dwSize - sizeof(__PM_biosFuncs.dwSize));
    __PM_biosFuncs.dwSize = MIN(biosFuncs->dwSize,__PM_biosFuncs.dwSize);
    memcpy(&__PM_biosFuncs,biosFuncs,__PM_biosFuncs.dwSize);
}

#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__) && !defined(__WINCE__)
/****************************************************************************
DESCRIPTION:
Clear the TS flag and save the floating point processor state

HEADER:
pmapi.h

REMARKS:
This function is only relevant to Windows 9x VxD and Windows NT/2K/XP
ring 0 device drivers where floating point code is problematic. In order
to execute floating point code inside a VxD or NT device driver, you must
make sure the TS flag is cleared and that the floating point state is
completely saved and restored across any floating point code. Note also
that you *must* make sure you do not call any system services that may
cause a task switch or you will hose the system.

For platforms other than Windows 9x VxD's and Windows NT/2K/XP device
drivers this function will be stubbed out and do nothing.

SEE ALSO:
PM_restoreFPUState
****************************************************************************/
void PMAPI PM_saveFPUState(void)
{
}

/****************************************************************************
DESCRIPTION:
Restore the floating point processor state

HEADER:
pmapi.h

REMARKS:
This function is only relevant to Windows 9x VxD and Windows NT/2K/XP
ring 0 device drivers where floating point code is problematic. When you
are done executing floating point code, call this function to restore the
floating point processor state.

For platforms other than Windows 9x VxD's and Windows NT/2K/XP device
drivers this function will be stubbed out and do nothing.

SEE ALSO:
PM_saveFPUState
****************************************************************************/
void PMAPI PM_restoreFPUState(void)
{
}
#endif

