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
* Environment:  QNX
*
* Description:  OS specific SciTech SNAP Graphics Graphics Architecture services for
*               the QNX operating system.
*
****************************************************************************/

#include "snap/graphics.h"
#include <time.h>

/*---------------------------- Global Variables ---------------------------*/

static ibool        haveRDTSC;

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
In order to support deploying new SciTech SNAP Graphics drivers that may require updated
PM library functions, we check here to see if there is a system wide version
of the PM functions available. If so we return those functions for use with
the system wide SciTech SNAP Graphics drivers, otherwise the compiled in version of the PM
library is used with the application local version of SciTech SNAP Graphics.
****************************************************************************/
PM_imports * NAPI GA_getSystemPMImports(void)
{
    // TODO: We may very well want to provide a system shared library
    //       that eports the PM functions required by the SciTech SNAP Graphics library
    //       for QNX here. That will eliminate fatal errors loading new
    //       drivers on QNX!
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
This function initialises the high precision timing functions for the
SciTech SNAP Graphics loader library.
****************************************************************************/
ibool NAPI GA_TimerInit(void)
{
    if (_GA_haveCPUID() && (_GA_getCPUIDFeatures() & CPU_HaveRDTSC) != 0)
        haveRDTSC = true;
    return true;
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
    else {
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
#ifdef __NATIVE_INT64__
        *value = (ts.tv_nsec / 1000 + ts.tv_sec * 1000000);
#else   
        value->low = (ts.tv_nsec / 1000 + ts.tv_sec * 1000000);
        value->high = 0;
#endif
        }
}

