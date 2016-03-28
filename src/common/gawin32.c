/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  OS specific SNAP Graphics Architecture services for
*               the Win32 operating system environments.
*
****************************************************************************/

#include "pm_help.h"
#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*------------------------- Global Variables ------------------------------*/

#define BPD_NAME        "graphics.bpd"
#define NGA_W32_NAME    "nga_w32.dll"

extern HANDLE           _PM_hDevice;
static HMODULE          hModDLL = NULL;
static ibool            useRing0Driver = false;
static ibool            haveRDTSC;
static GA_largeInteger  countFreq;

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
REMARKS:
Loads the shared "nga_w32.dll" library from disk and connects to it. This
library is *always* located in the same directory as the SNAP
graphics.bpd file.
****************************************************************************/
static ibool LoadSharedDLL(void)
{
    char    filename[PM_MAX_PATH];
    char    bpdpath[PM_MAX_PATH];

    /* Check if we have already loaded the DLL */
    if (hModDLL)
        return true;
    PM_init();

    /* Find the path to the BPD file. Note that we do this first
     * *before* calling PM_findBPD() for the nga_w32.dll library
     * because PM_findBPD() will cache the first directory it finds
     * for finding subsequent files. It is important that we always
     * use the directory where the graphics.bpd file we need is
     * located, so we must always search for that file first.
     */
    if (!PM_findBPD(BPD_NAME,bpdpath))
        return false;

    /* Open the DLL file */
    if (!PM_findBPD(NGA_W32_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,NGA_W32_NAME);
    if ((hModDLL = LoadLibrary(filename)) == NULL)
        return false;
    return true;
}

/****************************************************************************
PARAMETERS:
path    - Local path to the SciTech SNAP Graphics driver files.

REMARKS:
This function is used by the application program to override the location
of the SciTech SNAP Graphics driver files that are loaded. Normally the loader code
will look in the system SciTech SNAP Graphics directories first, then in the 'drivers'
directory relative to the current working directory, and finally relative
to the MGL_ROOT environment variable.

Note that for Win32 we also call into the loaded PMHELP device driver
as necessary to change the local SciTech SNAP Graphics path for system wide
SciTech SNAP Graphics drivers.
****************************************************************************/
void NAPI GA_setLocalPath(
    const char *path)
{
    DWORD   inBuf[1];
    DWORD   outBuf[1],outCnt;

    PM_setLocalBPDPath(path);
    if (_PM_hDevice != INVALID_HANDLE_VALUE) {
        inBuf[0] = (DWORD)path;
        DeviceIoControl(_PM_hDevice, PMHELP_GASETLOCALPATH32,
            inBuf, sizeof(inBuf), outBuf, sizeof(outBuf), &outCnt, NULL);
        }
}

/****************************************************************************
RETURNS:
Pointer to the system wide PM library imports, or the internal version if none

REMARKS:
In order to support deploying new SciTech SNAP Graphics drivers that may
require updated PM library functions, we check here to see if there is a
system wide version of the PM functions available. If so we return those
functions for use with the system wide SciTech SNAP Graphics drivers, otherwise
the compiled in version of the PM library is used with the application local
version of SciTech SNAP Graphics.
****************************************************************************/
PM_imports * NAPI GA_getSystemPMImports(void)
{
    PM_imports * pmImp;
    PM_imports * (NAPIP _GA_getSystemPMImports)(void);

    if (LoadSharedDLL()) {
        if ((_GA_getSystemPMImports = (void*)GetProcAddress(hModDLL,"_GA_getSystemPMImports")) != NULL) {
            pmImp = _GA_getSystemPMImports();
            memcpy(&_PM_imports,pmImp,MIN(_PM_imports.dwSize,pmImp->dwSize));
            return pmImp;
            }
        }
    return &_PM_imports;
}

/****************************************************************************
REMARKS:
Not used.
****************************************************************************/
GA_sharedInfo * NAPI GA_getSharedInfo(
    int device)
{
    return NULL;
}

/****************************************************************************
PARAMETERS:
gaExp   - Place to store the exported functions
shared  - True if connecting to the shared, global SciTech SNAP Graphics driver

REMARKS:
For Win32 if we are connecting to the shared, global SciTech SNAP Graphics driver (loaded
at ring 0) then we need to load a special nga_w32.dll library which contains
thunks to call down into the Ring 0 device driver as necessary. If we are
connecting to the application local SciTech SNAP Graphics drivers (ie:
SciTech SNAP Graphics on DirectDraw emulation layer) then we do nothing here.
****************************************************************************/
ibool NAPI GA_getSharedExports(
    GA_exports *gaExp,
    ibool shared)
{
    GA_exports * exp;
    GA_exports * (NAPIP _GA_getSystemGAExports)(void);

    useRing0Driver = false;
    if (!LoadSharedDLL())
        PM_fatalError("Unable to load " NGA_W32_NAME "!");
    if ((_GA_getSystemGAExports = (void*)GetProcAddress(hModDLL,"_GA_getSystemGAExports")) == NULL)
        PM_fatalError("Unable to load " NGA_W32_NAME "!");
    if ((exp = _GA_getSystemGAExports()) != NULL) {
        memcpy(gaExp,exp,MIN(gaExp->dwSize,exp->dwSize));
        useRing0Driver = true;
        return true;
        }
    (void)shared;
    return false;
}

#ifndef TEST_HARNESS
/****************************************************************************
REMARKS:
Nothing special for this OS
****************************************************************************/
ibool NAPI GA_queryFunctions(
    GA_devCtx *dc,
    N_uint32 id,
    void _FAR_ *funcs)
{
    static ibool (NAPIP _GA_queryFunctions)(GA_devCtx *dc,N_uint32 id,void _FAR_ *funcs) = NULL;

    if (useRing0Driver) {
        // Call the version in nga_w32.dll if it is loaded
        if (!_GA_queryFunctions) {
            if ((_GA_queryFunctions = (void*)GetProcAddress(hModDLL,"_GA_queryFunctions")) == NULL)
                PM_fatalError("Unable to get exports from " NGA_W32_NAME "!");
            }
        return _GA_queryFunctions(dc,id,funcs);
        }
    return __GA_exports.GA_queryFunctions(dc,id,funcs);
}

/****************************************************************************
REMARKS:
Nothing special for this OS
****************************************************************************/
ibool NAPI REF2D_queryFunctions(
    REF2D_driver *ref2d,
    N_uint32 id,
    void _FAR_ *funcs)
{
    static ibool (NAPIP _REF2D_queryFunctions)(REF2D_driver *ref2d,N_uint32 id,void _FAR_ *funcs) = NULL;

    if (useRing0Driver) {
        // Call the version in nga_w32.dll if it is loaded
        if (!_REF2D_queryFunctions) {
            if ((_REF2D_queryFunctions = (void*)GetProcAddress(hModDLL,"_REF2D_queryFunctions")) == NULL)
                PM_fatalError("Unable to get exports from " NGA_W32_NAME "!");
            }
        return _REF2D_queryFunctions(ref2d,id,funcs);
        }
    return __GA_exports.REF2D_queryFunctions(ref2d,id,funcs);
}
#endif

/****************************************************************************
REMARKS:
This function initialises the high precision timing functions for the
SciTech SNAP Graphics loader library.
****************************************************************************/
ibool NAPI GA_TimerInit(void)
{
    if (_GA_haveCPUID() && (_GA_getCPUIDFeatures() & CPU_HaveRDTSC) != 0) {
        haveRDTSC = true;
        return true;
        }
    else if (QueryPerformanceFrequency((LARGE_INTEGER*)&countFreq)) {
        haveRDTSC = false;
        return true;
        }
    return false;
}

/****************************************************************************
REMARKS:
This function reads the high resolution timer.
****************************************************************************/
void NAPI GA_TimerRead(
    GA_largeInteger *value)
{
    if (haveRDTSC)
        _GA_readTimeStamp(value);
    else
        QueryPerformanceCounter((LARGE_INTEGER*)value);
}

