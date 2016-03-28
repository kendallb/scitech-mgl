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
* Environment:  Any 32-bit protected mode environment
*
* Description:  C module for the Graphics Accelerator Driver API. Uses
*               the SciTech PM library for interfacing with DOS
*               extender specific functions.
*
****************************************************************************/

#include "snap/graphics.h"
#if defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
#include "sdd/sddhelp.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

#ifndef TEST_HARNESS
GA_exports  _VARAPI __GA_exports;
static int          loaded = false;
static MOD_MODULE   *hModBPD = NULL;
static uchar        *isvLicense = NULL;

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

static GA_imports _GA_imports = {
    sizeof(GA_imports),
    GA_getSharedInfo,
    GA_TimerInit,
    GA_TimerRead,
    GA_TimerDifference,
    };
#endif

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "graphics.bpd"

#ifndef TEST_HARNESS
/****************************************************************************
REMARKS:
Fatal error handler for non-exported GA_exports.
****************************************************************************/
static void _GA_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of SNAP Graphics!\n");
}

/****************************************************************************
DESCRIPTION:
Checks to see if the OS shared SNAP driver is loaded

HEADER:
snap/graphics.h

RETURNS:
True if the shared driver is loaded, false if not.

REMARKS:
This function checks to see if the operating system shared SNAP driver is
already loaded. This function is mostly useful to determine if the shared
driver is already loaded, such that a call to GA_loadDriver() will end
up getting a reference counted copy of the operating system shared driver.
If this function returns false, this is the first instance of SNAP so if
the shared flag is set to true when calling GA_loadDriver(), the shared
SNAP driver will be loaded (and other applications can connect to it).

SEE ALSO:
GA_loadDriver
****************************************************************************/
ibool NAPI GA_isSharedDriverLoaded(void)
{
    /* Check if we have already loaded the driver */
    if (loaded)
        return true;
    PM_init();

    /* First try to see if we can find the system wide shared exports
     * if they are available. Under OS/2 this connects to our global
     * shared SNAP loader in SDDPMI.DLL.
     */
    __GA_exports.dwSize = sizeof(__GA_exports);
    if (GA_getSharedExports(&__GA_exports,true)) {
        if (isvLicense)
            __GA_exports.GA_registerLicense(isvLicense,true);
        return loaded = true;
        }
    return false;
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the SNAP binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(
    ibool shared)
{
    GA_initLibrary_t    GA_initLibrary;
    GA_exports          *gaExp;
    char                filename[PM_MAX_PATH];
    char                bpdpath[PM_MAX_PATH];
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
        return true;
    PM_init();

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(__GA_exports)/sizeof(GA_initLibrary_t);
    for (i = 0,p = (ulong*)&__GA_exports; i < max; i++)
        *p++ = (ulong)_GA_fatalErrorHandler;

#if !defined(__LINUX__) || defined(__SHARED__)
    /* First try to see if we can find the system wide shared exports
     * if they are available. Under OS/2 this connects to our global
     * shared SNAP loader in SDDPMI.DLL. For Linux we *cannot* do this
     * if the PM library is static linked!
     */
    __GA_exports.dwSize = sizeof(__GA_exports);
    if (GA_getSharedExports(&__GA_exports,shared)) {
        if (isvLicense)
            __GA_exports.GA_registerLicense(isvLicense,shared);
        return loaded = true;
        }
#elif defined(__LINUX__)
    shared = false;
#endif

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((hModBPD = MOD_loadLibrary(filename,shared)) == NULL)
        return false;
    if ((GA_initLibrary = (GA_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"GA_initLibrary")) == NULL)
        return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getSNAPPath()) == 0)
        strcpy(bpdpath,PM_getSNAPConfigPath());
    else {
        PM_backslash(bpdpath);
        strcat(bpdpath,"config");
        }
    if ((gaExp = GA_initLibrary(shared,bpdpath,filename,GA_getSystemPMImports(),&_N_imports,&_GA_imports)) == NULL)
        PM_fatalError("GA_initLibrary failed!\n");
    memcpy(&__GA_exports,gaExp,MIN(sizeof(__GA_exports),gaExp->dwSize));
    loaded = true;

    /* Allow the loaded driver to change the exports table after it has
     * been loaded if necessary. We only support this in newer versions
     * of graphics.bpd, so we have to check if this is available first.
     */
    if ((ulong)__GA_exports.GA_setExportTable != (ulong)_GA_fatalErrorHandler)
        GA_setExportTable(&__GA_exports);

#if defined(__LINUX__) && defined(__SHARED__)
    /* TODO: Should implement this for other platforms */
    /* Now set the shared SNAP loader exports so that all SNAP
     * apps from this point forward we will end up referencing the
     * code loaded in the SDDPMI driver.
     */
    if (shared)
        PM_setGASharedExports(&__GA_exports,sizeof(__GA_exports));
#endif

    /* Register ISV license file with SNAP if needed */
    if (isvLicense)
        __GA_exports.GA_registerLicense(isvLicense,shared);
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the SNAP loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
int NAPI GA_status(void)
{
    if (!loaded)
        return nDriverNotFound;
    return __GA_exports.GA_status();
}

/* {secret} */
const char * NAPI GA_errorMsg(
    N_int32 status)
{
    if (!loaded)
        return "Unable to load SNAP device driver!";
    return __GA_exports.GA_errorMsg(status);
}

/* {secret} */
int NAPI GA_getDaysLeft(N_int32 shared)
{
    if (!LoadDriver(shared))
        return -1;
    return __GA_exports.GA_getDaysLeft(shared);
}

/* {secret} */
int NAPI GA_registerLicense(uchar *license,N_int32 shared)
{
    (void)shared;
    isvLicense = license;
    return true;
}

/* {secret} */
ibool NAPI GA_loadInGUI(N_int32 shared)
{
    if (!LoadDriver(shared))
        return false;
    return __GA_exports.GA_loadInGUI(shared);
}

/* {secret} */
int NAPI GA_enumerateDevices(N_int32 shared)
{
    if (!LoadDriver(shared))
        return 0;
    return __GA_exports.GA_enumerateDevices(shared);
}

/* {secret} */
PCIDeviceInfo * NAPI GA_getPCIDeviceInfo(N_int32 deviceIndex,N_int32 shared)
{
    if (!LoadDriver(shared))
        return 0;
    if (__GA_exports.GA_getPCIDeviceInfo)
        return __GA_exports.GA_getPCIDeviceInfo(deviceIndex,shared);
    return NULL;
}

/* {secret} */
GA_devCtx * NAPI GA_loadDriver(N_int32 deviceIndex,N_int32 shared)
{
    if (!LoadDriver(shared))
        return NULL;
    return __GA_exports.GA_loadDriver(deviceIndex,shared);
}

/* {secret} */
void NAPI GA_getGlobalOptions(
    GA_globalOptions *options,
    ibool shared)
{
    if (LoadDriver(shared))
        __GA_exports.GA_getGlobalOptions(options,shared);
}

/* {secret} */
void NAPI GA_setGlobalOptions(
    GA_globalOptions *options)
{
    if (loaded)
        __GA_exports.GA_setGlobalOptions(options);
}

/* {secret} */
ibool NAPI GA_saveGlobalOptions(
    GA_globalOptions *options)
{
    if (loaded)
        return __GA_exports.GA_saveGlobalOptions(options);
    return false;
}

/* {secret} */
MOD_MODULE * NAPI GA_loadLibrary(
    const char *szBPDName,
    ulong *size,
    ibool shared)
{
    if (!LoadDriver(shared))
        return NULL;
    return __GA_exports.GA_loadLibrary(szBPDName,size,shared);
}

/* {secret} */
REF2D_driver * NAPI GA_getCurrentRef2d(
    N_int32 deviceIndex)
{
    /* Bail for older drivers that didn't export this function! */
    if (!__GA_exports.GA_getCurrentRef2d)
        return NULL;
    return __GA_exports.GA_getCurrentRef2d(deviceIndex);
}

/* {secret} */
int NAPI GA_isOEMVersion(ibool shared)
{
    if (!LoadDriver(shared))
        return 0;
    return __GA_exports.GA_isOEMVersion(shared);
}

/* {secret} */
N_uint32 * NAPI GA_getLicensedDevices(ibool shared)
{
    if (!LoadDriver(shared))
        return 0;
    return __GA_exports.GA_getLicensedDevices(shared);
}

/* {secret} */
void NAPI GA_setMinimumDriverVersion(
    N_uint32 version,
    N_int32 allowFallback,
    N_int32 shared)
{
    if (LoadDriver(shared))
        __GA_exports.GA_setMinimumDriverVersion(version,allowFallback,shared);
}

#else

ibool NAPI GA_isSharedDriverLoaded(void)
{
    return false;
}

#endif

