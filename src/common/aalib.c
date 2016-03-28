/****************************************************************************
*
*                      SciTech SNAP Audio Architecture
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
* Description:  C module for the SciTech SNAP Audio Driver API. Uses
*               the SciTech PM library for interfacing with DOS
*               extender specific functions.
*
****************************************************************************/

#include "snap/audio.h"
#if defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
#include "sdd/sddhelp.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

#ifndef  TEST_HARNESS
AA_exports  _VARAPI __AA_exports;
static int          loaded = false;
static MOD_MODULE   *hModBPD = NULL;
static uchar        *isvLicense = NULL;

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

static AA_imports _AA_imports = {
    sizeof(AA_imports),
    };
#endif

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "audio.bpd"

#ifndef TEST_HARNESS
/****************************************************************************
REMARKS:
Fatal error handler for non-exported AA_exports.
****************************************************************************/
static void _AA_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of SNAP Audio!\n");
}

/****************************************************************************
REMARKS:
Loads the SNAP binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    AA_initLibrary_t    AA_initLibrary;
    AA_exports          *aaExp;
    char                filename[PM_MAX_PATH];
    char                bpdpath[PM_MAX_PATH];
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
        return true;
    PM_init();
    __AA_exports.dwSize = sizeof(__AA_exports);

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((hModBPD = MOD_loadLibrary(filename,false)) == NULL)
        return false;
    if ((AA_initLibrary = (AA_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"AA_initLibrary")) == NULL)
        return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getSNAPPath()) == 0)
        strcpy(bpdpath,PM_getSNAPConfigPath());
    else {
        PM_backslash(bpdpath);
        strcat(bpdpath,"config");
        }
    if ((aaExp = AA_initLibrary(bpdpath,filename,&_PM_imports,&_N_imports,&_AA_imports)) == NULL)
        PM_fatalError("AA_initLibrary failed!\n");

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(__AA_exports)/sizeof(AA_initLibrary_t);
    for (i = 0,p = (ulong*)&__AA_exports; i < max; i++)
        *p++ = (ulong)_AA_fatalErrorHandler;
    memcpy(&__AA_exports,aaExp,MIN(sizeof(__AA_exports),aaExp->dwSize));
    loaded = true;

    /* Register ISV license file with SNAP if needed */
    if (isvLicense)
        __AA_exports.AA_registerLicense(isvLicense);
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the SNAP loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
int NAPI AA_status(void)
{
    if (!loaded)
        return nDriverNotFound;
    return __AA_exports.AA_status();
}

/* {secret} */
const char * NAPI AA_errorMsg(
    N_int32 status)
{
    if (!loaded)
        return "Unable to load SNAP device driver!";
    return __AA_exports.AA_errorMsg(status);
}

/* {secret} */
int NAPI AA_getDaysLeft(void)
{
    if (!LoadDriver())
        return -1;
    return __AA_exports.AA_getDaysLeft();
}

/* {secret} */
int NAPI AA_registerLicense(uchar *license)
{
    isvLicense = license;
    return true;
}

/* {secret} */
int NAPI AA_enumerateDevices(void)
{
    if (!LoadDriver())
        return 0;
    return __AA_exports.AA_enumerateDevices();
}

/* {secret} */
AA_devCtx * NAPI AA_loadDriver(N_int32 deviceIndex)
{
    if (!LoadDriver())
        return NULL;
    return __AA_exports.AA_loadDriver(deviceIndex);
}
#endif

