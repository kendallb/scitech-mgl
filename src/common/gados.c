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
* Environment:  MSDOS
*
* Description:  OS specific SciTech SNAP Graphics Architecture services for
*               the MSDOS operating system.
*
****************************************************************************/

#include "pm_help.h"
#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
PARAMETERS:
path    - Local path to the SciTech SNAP Graphics driver files.

REMARKS:
This function is used by the application program to override the location
of the SciTech SNAP Graphics driver files that are loaded. Normally the loader code
will look in the system SciTech SNAP Graphics directories first, then in the 'drivers'
directory relative to the current working directory, and finally relative
to the MGL_ROOT environment variable.
****************************************************************************/
void NAPI GA_setLocalPath(
    const char *path)
{
    PM_setLocalBPDPath(path);
}

/****************************************************************************
RETURNS:
Pointer to the system wide PM library imports, or the internal version if none

REMARKS:
Nothing to do here for DOS. Basically since DOS has no system wide shared
library mechanism we are essentially screwed if the binary API changes.
By default for 32-bit DOS apps the local SciTech SNAP Graphics drivers should always be
used in preference to the system wide SciTech SNAP Graphics drivers.
****************************************************************************/
PM_imports * NAPI GA_getSystemPMImports(void)
{
    return &_PM_imports;
}

/****************************************************************************
REMARKS:
Not used.
****************************************************************************/
GA_sharedInfo * NAPI GA_getSharedInfo(
    int device)
{
    VXD_regs    r;

    if (_PM_VXD_sel != 0 || _PM_VXD_off != 0) {
        memset(&r,0,sizeof(r));
        r.eax = API_NUM(PMHELP_GAGETSHAREDINFO);
        _PM_VxDCall(&r,_PM_VXD_off,_PM_VXD_sel);
        if (r.eax)
            return (GA_sharedInfo*)r.eax;
        }
    return NULL;
}

/****************************************************************************
REMARKS:
Nothing special for this OS.
****************************************************************************/
ibool NAPI GA_getSharedExports(
    GA_exports *gaExp,
    ibool shared)
{
    (void)gaExp;
    (void)shared;
    return false;
}

#if !defined(TEST_HARNESS) && !defined(VBETEST)
/****************************************************************************
REMARKS:
Nothing special for this OS
****************************************************************************/
ibool NAPI GA_queryFunctions(
    GA_devCtx *dc,
    N_uint32 id,
    void _FAR_ *funcs)
{
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
    return __GA_exports.REF2D_queryFunctions(ref2d,id,funcs);
}
#endif

/****************************************************************************
REMARKS:
This function initialises the high precision timing functions for the DOS
SciTech SNAP Graphics loader library.
****************************************************************************/
ibool NAPI GA_TimerInit(void)
{
    if (_GA_haveCPUID() && (_GA_getCPUIDFeatures() & CPU_HaveRDTSC) != 0)
        return true;
    return false;
}

/****************************************************************************
REMARKS:
This function reads the high resolution timer.
****************************************************************************/
void NAPI GA_TimerRead(
    GA_largeInteger *value)
{
    _GA_readTimeStamp(value);
}

