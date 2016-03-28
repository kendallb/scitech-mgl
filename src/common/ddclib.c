/****************************************************************************
*
*                       SciTech SNAP DDC Architecture
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
* Description:  C module for the DDC Driver API. Uses the SciTech PM
*               library for interfacing with OS specific functions.
*
****************************************************************************/

#include "snap/ddclib.h"
#if defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
#include "sdd/sddhelp.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

DDC_exports  _VARAPI __DDC_exports;
static int          loaded = false;
static MOD_MODULE   *hModBPD = NULL;
static uchar        *isvLicense = NULL;

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "ddc.bpd"

/****************************************************************************
REMARKS:
Fatal error handler for non-exported DDC_exports.
****************************************************************************/
static void _DDC_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of SNAP DDC!\n");
}

/****************************************************************************
REMARKS:
Loads the SNAP binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    DDC_initLibrary_t   DDC_initLibrary;
    DDC_exports         *ddcExp;
    char                filename[PM_MAX_PATH];
    char                bpdpath[PM_MAX_PATH];
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
        return true;
    PM_init();

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((hModBPD = MOD_loadLibrary(filename,false)) == NULL)
        return false;
    if ((DDC_initLibrary = (DDC_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"DDC_initLibrary")) == NULL)
        return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getSNAPPath()) == 0)
        strcpy(bpdpath,PM_getSNAPConfigPath());
    else {
        PM_backslash(bpdpath);
        strcat(bpdpath,"config");
        }
    if ((ddcExp = DDC_initLibrary(bpdpath,filename,GA_getSystemPMImports(),&_N_imports)) == NULL)
        PM_fatalError("DDC_initLibrary failed!\n");

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(__DDC_exports)/sizeof(DDC_initLibrary_t);
    for (i = 0,p = (ulong*)&__DDC_exports; i < max; i++)
        *p++ = (ulong)_DDC_fatalErrorHandler;
    memcpy(&__DDC_exports,ddcExp,MIN(sizeof(__DDC_exports),ddcExp->dwSize));
    loaded = true;

    /* Register ISV license file with SNAP if needed */
    if (isvLicense)
        __DDC_exports.DDC_registerLicense(isvLicense);
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the SNAP loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
int NAPI DDC_status(void)
{
    if (!loaded)
        return nDriverNotFound;
    return __DDC_exports.DDC_status();
}

/* {secret} */
const char * NAPI DDC_errorMsg(
    N_int32 status)
{
    if (!loaded)
        return "Unable to load SNAP device driver!";
    return __DDC_exports.DDC_errorMsg(status);
}

/* {secret} */
int NAPI DDC_registerLicense(
    uchar *license)
{
    isvLicense = license;
    return true;
}

/* {secret} */
int NAPI DDC_enumerateDevices(void)
{
    if (!LoadDriver())
        return 0;
    return __DDC_exports.DDC_enumerateDevices();
}

/* {secret} */
GA_devCtx * NAPI DDC_loadDriver(
    N_int32 deviceIndex)
{
    if (!LoadDriver())
        return NULL;
#ifdef __WINDOWS32__
    PM_enableGlobalAddressMapping(true);
#endif
    return __DDC_exports.DDC_loadDriver(deviceIndex);
}

/* {secret} */
void NAPI DDC_unloadDriver(
    GA_devCtx *dc)
{
    if (loaded)
        __DDC_exports.DDC_unloadDriver(dc);
}

/* {secret} */
ibool NAPI DDC_queryFunctions(
    GA_devCtx *dc,
    N_uint32 id,
    void _FAR_ *funcs)
{
    if (loaded)
        return __DDC_exports.DDC_queryFunctions(dc,id,funcs);
    return false;
}

