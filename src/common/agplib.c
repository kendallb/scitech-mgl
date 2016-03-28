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
#include "snap/agp.h"
#if !defined(__WIN32_VXD__)
#include <string.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

#if !defined(DEBUG_AGP_DRIVER) && !defined(AGP_TEST_HARNESS)
static AGP_exports  _AGP_exports;
static int          loaded = false;
static MOD_MODULE   *hModBPD = NULL;

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

static AGP_imports _AGP_imports = {
    sizeof(AGP_imports),
    };
#endif

#include "pmimp.h"

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "agp.bpd"

#if !defined(DEBUG_AGP_DRIVER) && !defined(AGP_TEST_HARNESS)
/****************************************************************************
REMARKS:
Fatal error handler for non-exported GA_exports.
****************************************************************************/
static void _AGP_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported AGP export function called! Please upgrade your copy of AGP!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the SciTech SNAP binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    AGP_initLibrary_t   AGP_initLibrary;
    AGP_exports         *agpExp;
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
    if ((AGP_initLibrary = (AGP_initLibrary_t)MOD_getProcAddress(hModBPD,"_AGP_initLibrary")) == NULL)
        return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getSNAPPath()) == 0)
        strcpy(bpdpath,PM_getSNAPConfigPath());
    else {
        PM_backslash(bpdpath);
        strcat(bpdpath,"config");
        }
    if ((agpExp = AGP_initLibrary(bpdpath,filename,GA_getSystemPMImports(),&_N_imports,&_AGP_imports)) == NULL)
        PM_fatalError("AGP_initLibrary failed!\n");
    _AGP_exports.dwSize = sizeof(_AGP_exports);
    max = sizeof(_AGP_exports)/sizeof(AGP_initLibrary_t);
    for (i = 0,p = (ulong*)&_AGP_exports; i < max; i++)
        *p++ = (ulong)_AGP_fatalErrorHandler;
    memcpy(&_AGP_exports,agpExp,MIN(sizeof(_AGP_exports),agpExp->dwSize));
    loaded = true;
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the SciTech SNAP loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
int NAPI AGP_status(void)
{
    if (!loaded)
        return nDriverNotFound;
    return _AGP_exports.AGP_status();
}

/* {secret} */
const char * NAPI AGP_errorMsg(
    N_int32 status)
{
    if (!loaded)
        return "Unable to load SNAP device driver!";
    return _AGP_exports.AGP_errorMsg(status);
}

/* {secret} */
AGP_devCtx * NAPI AGP_loadDriver(N_int32 deviceIndex)
{
    if (!LoadDriver())
        return NULL;
    return _AGP_exports.AGP_loadDriver(deviceIndex);
}

/* {secret} */
void NAPI AGP_unloadDriver(
    AGP_devCtx *dc)
{
    if (loaded)
        _AGP_exports.AGP_unloadDriver(dc);
}

/* {secret} */
void NAPI AGP_getGlobalOptions(
    AGP_globalOptions *options)
{
    if (LoadDriver())
        _AGP_exports.AGP_getGlobalOptions(options);
}

/* {secret} */
void NAPI AGP_setGlobalOptions(
    AGP_globalOptions *options)
{
    if (LoadDriver())
        _AGP_exports.AGP_setGlobalOptions(options);
}

/* {secret} */
void NAPI AGP_saveGlobalOptions(
    AGP_globalOptions *options)
{
    if (loaded)
        _AGP_exports.AGP_saveGlobalOptions(options);
}
#endif

#ifdef __INTEL__
/* {secret} */
void NAPI _OS_delay8253(N_uint32 microSeconds);
#endif

/****************************************************************************
REMARKS:
This function delays for the specified number of microseconds
****************************************************************************/
void NAPI _OS_delay(
    N_uint32 microSeconds)
{
    LZTimerObject   tm;
#ifdef __INTEL__
    static ibool    inited = false;
    static ibool    haveRDTSC;

    if (!inited) {
        // This has been causing problems in VxD's for some reason, so for now
        // we avoid using it.
        if (_GA_haveCPUID() && (_GA_getCPUIDFeatures() & CPU_HaveRDTSC) != 0) {
            ZTimerInit();
            haveRDTSC = true;
            }
        else
            haveRDTSC = false;
        inited = true;
        }
    if (haveRDTSC) {
        LZTimerOnExt(&tm);
        while (LZTimerLapExt(&tm) < microSeconds)
            ;
        LZTimerOnExt(&tm);
        }
    else
        _OS_delay8253(microSeconds);
#else
    LZTimerOnExt(&tm);
    while (LZTimerLapExt(&tm) < microSeconds)
        ;
    LZTimerOnExt(&tm);
#endif
}

