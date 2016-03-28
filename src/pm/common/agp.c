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
* Environment:  32-bit Ring 0 device driver
*
* Description:  Generic module to implement AGP support functions using the
*               SciTech SNAP AGP support drivers. If the OS provides
*               native AGP support, this module should *NOT* be used. Instead
*               wrappers should be placed around the OS support functions
*               to implement this functionality.
*
****************************************************************************/

#include "pmapi.h"
#ifndef REALMODE
#include "snap/agp.h"

/*--------------------------- Global variables ----------------------------*/

static AGP_devCtx       *agp;
static AGP_driverFuncs  driver;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
DESCRIPTION:
Initialise the AGP functions

HEADER:
pmapi.h

RETURNS:
Size of AGP aperture in MB on success, 0 on failure.

REMARKS:
This function initialises the AGP driver in the system and returns the
size of the available AGP aperture in megabytes if an AGP bus is found.
If there is no AGP bus or the driver could not be loaded, this function
returns 0.

SEE ALSO:
PM_agpExit, PM_agpReservePhysical
****************************************************************************/
ulong PMAPI PM_agpInit(void)
{
    if ((agp = AGP_loadDriver(0)) == NULL)
        return 0;
    driver.dwSize = sizeof(driver);
    if (!agp->QueryFunctions(AGP_GET_DRIVERFUNCS,&driver))
        return 0;
    switch (driver.GetApertureSize()) {
        case agpSize4MB:    return 4;
        case agpSize8MB:    return 8;
        case agpSize16MB:   return 16;
        case agpSize32MB:   return 32;
        case agpSize64MB:   return 64;
        case agpSize128MB:  return 128;
        case agpSize256MB:  return 256;
        case agpSize512MB:  return 512;
        case agpSize1GB:    return 1024;
        case agpSize2GB:    return 2048;
        }
    return 0;
}

/****************************************************************************
DESCRIPTION:
Close down the AGP functions

HEADER:
pmapi.h

REMARKS:
This function closes down the loaded AGP driver.

SEE ALSO:
PM_agpInit
****************************************************************************/
void PMAPI PM_agpExit(void)
{
    AGP_unloadDriver(agp);
}

/****************************************************************************
DESCRIPTION:
Reserves a rage of physical addresses of AGP memory

HEADER:
pmapi.h

PARAMETERS:
numPages    - Number of memory pages that should be reserved
type        - Type of memory to allocate
physContext - Returns the physical context handle for the mapping
physAddr    - Returns the physical address for the mapping

RETURNS:
True on success, false on failure.

REMARKS:
This function reserves a range of physical memory addresses on the system
bus which the AGP controller will respond to. If this function succeeds,
the AGP controller can respond to the reserved physical address range on
the bus. However you must first call AGP_commitPhysical to cause this memory
to actually be committed for use before it can be accessed.

SEE ALSO:
PM_agpReleasePhysical, PM_agpCommitPhysical
****************************************************************************/
ibool PMAPI PM_agpReservePhysical(
    ulong numPages,
    int type,
    void **physContext,
    PM_physAddr *physAddr)
{
    switch (type) {
        case PM_agpUncached:
            type = agpUncached;
            break;
        case PM_agpWriteCombine:
            type = agpWriteCombine;
            break;
        case PM_agpIntelDCACHE:
            type = agpIntelDCACHE;
            break;
        default:
            return false;
        }
    return driver.ReservePhysical(numPages,type,physContext,physAddr) == nOK;
}

/****************************************************************************
DESCRIPTION:
Releases a range of reserved physical AGP memory addresses

HEADER:
pmapi.h

PARAMETERS:
physContext - Physical AGP context to release

RETURNS:
True on success, false on failure.

REMARKS:
This function releases a range of physical memory addresses on the system
bus which the AGP controller will respond to. All committed memory for
the physical address range covered by the context will be released.

SEE ALSO:
PM_agpReservePhysical
****************************************************************************/
ibool PMAPI PM_agpReleasePhysical(
    void *physContext)
{
    return driver.ReleasePhysical(physContext) == nOK;
}

/****************************************************************************
DESCRIPTION:
Commits memory to a range of reserved physical AGP memory addresses

HEADER:
pmapi.h

PARAMETERS:
physContext - Physical AGP context to commit memory for
numPages    - Number of pages to be committed
startOffset - Offset in pages into the reserved physical context
physAddr    - Returns the physical address of the committed memory

RETURNS:
True on success, false on failure.

REMARKS:
This function commits AGP memory into the specified physical context that
was previously reserved by a call to PM_agpReservePhysical. You can use the
startOffset and numPages parameters to only commit portions of the reserved
memory range at a time.

SEE ALSO:
PM_agpReservePhysical, PM_agpFreePhysical
****************************************************************************/
ibool PMAPI PM_agpCommitPhysical(
    void *physContext,
    ulong numPages,
    ulong startOffset,
    PM_physAddr *physAddr)
{
    return driver.CommitPhysical(physContext,numPages,startOffset,physAddr) == nOK;
}

/****************************************************************************
DESCRIPTION:
Frees a range of commited physical AGP memory pages

HEADER:
pmapi.h

PARAMETERS:
physContext - Physical AGP context to free memory for
numPages    - Number of pages to be freed
startOffset - Offset in pages into the reserved physical context

RETURNS:
True on success, false on failure.

REMARKS:
This function frees memory previously committed by the PM_agpCommitPhysical
function. Note that you can free a portion of a memory range that was
previously committed if you wish.

SEE ALSO:
PM_agpCommitPhysical
****************************************************************************/
ibool PMAPI PM_agpFreePhysical(
    void *physContext,
    ulong numPages,
    ulong startOffset)
{
    return driver.FreePhysical(physContext,numPages,startOffset) == nOK;
}

#endif  /* !REALMODE */

