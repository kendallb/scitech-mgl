/****************************************************************************
*
*                     SciTech SNAP HDA Bus Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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
* Description:  C module for the HDA Bus Driver API. Uses the SciTech PM
*               library for interfacing with OS specific functions.
*
****************************************************************************/

#include "snap/hdabus.h"
#include "clib/modloadr.h"
#if defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
#include "sdd/sddhelp.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

HDA_exports _VARAPI     __HDA_exports;
static int              loaded = false;
static MOD_MODULE       *hModBPD = NULL;
static PM_lockHandle    hModLockHandle;

#ifdef __DRIVER__
extern N_imports _VARAPI _N_imports;
#else
static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };
#endif

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "hdabus.bpd"

/****************************************************************************
REMARKS:
Fatal error handler for non-exported HDA_exports.
****************************************************************************/
static void _HDA_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of SNAP HDA Bus!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the SNAP binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    HDA_initLibrary_t   HDA_initLibrary;
    HDA_exports         *hdaExp;
    char                filename[PM_MAX_PATH];
    char                bpdpath[PM_MAX_PATH];
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
        return true;
#ifndef __DRIVER__
    PM_init();
#endif

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((hModBPD = MOD_loadLibrary(filename,true)) == NULL)
        return false;
    if ((HDA_initLibrary = (HDA_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"HDA_initLibrary")) == NULL)
        return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getSNAPPath()) == 0)
        strcpy(bpdpath,PM_getSNAPConfigPath());
    else {
        PM_backslash(bpdpath);
        strcat(bpdpath,"config");
        }
    if ((hdaExp = HDA_initLibrary(bpdpath,filename,&_PM_imports,&_N_imports)) == NULL)
        PM_fatalError("HDA_initLibrary failed!\n");

    /* Lock the entire driver in memory */
    PM_lockDataPages(hModBPD,hModBPD->size,&hModLockHandle);

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(__HDA_exports)/sizeof(HDA_initLibrary_t);
    for (i = 0,p = (ulong*)&__HDA_exports; i < max; i++)
        *p++ = (ulong)_HDA_fatalErrorHandler;
    memcpy(&__HDA_exports,hdaExp,MIN(sizeof(__HDA_exports),hdaExp->dwSize));
    loaded = true;
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the SNAP loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
N_int32 NAPI HDA_init(
    N_uint16 vendorID,
    N_uint16 deviceID,
    N_uint32 *codecMask)
{
    if (!LoadDriver())
        return nDriverNotFound;
    return __HDA_exports.HDA_init(vendorID,deviceID,codecMask);
}

