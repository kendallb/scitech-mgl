/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Description:  SciTech SNAP Graphics accelerated device driver file. This
*               driver only supports linear framebuffer, accelerated
*               devices and will ignore any other type device.
*
****************************************************************************/

#include "mgl.h"
#include "drivers/snap/snap.h"

/*---------------------------- Driver Tables -----------------------------*/

drivertype SNAP_driver = {
    MGL_SNAPNAME,
    "SciTech SNAP Graphics Accelerated Driver",
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1991-2004 SciTech Software, Inc.\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    SNAP_createInstance,
    SNAP_detect,
    SNAP_initDriver,
    SNAP_destroyInstance,
    };

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
Pointer to intialised instance data.

REMARKS:
This function initialises any necessary instance data for the device.
****************************************************************************/
void * MGLAPI SNAP_createInstance(void)
{
    return SNAP_initInstance(PM_calloc(1,sizeof(SNAP_data)));
}

/****************************************************************************
PARAMETERS:
data            - Instance data for the driver
id              - Id of this driver for storing in mode table
numModes        - Pointer to the number of modes already detected
availableModes  - Table to fill in with available mode information

RETURNS:
True if the device was found, false if not.

REMARKS:
Detects if a driver with the desired capabilities is present, and if so
register all the modes reported by the driver with the MGL.
****************************************************************************/
ibool MGLAPI SNAP_detect(
    void *data,
    int id,
    int *numModes,
    modetab availableModes)
{
    return SNAP_hardwareDetect((SNAP_data*)data,true,id,numModes,availableModes);
}

/****************************************************************************
PARAMETERS:
data            - Instance data for the driver
dc              - Device context to initialise
mode            - Display mode to set
hwnd            - Handle to OS window for the mode
virtualX        - Virtual X screen dimensions (-1 if not scrolling)
virtualY        - Virtual Y screen dimensions (-1 if not scrolling)
numBuffers      - Number of buffers to allocate
stereo          - True if stereo should be enabled
refreshRate     - Desired refresh rate for mode

RETURNS:
True on sucess, false on failure.

REMARKS:
This function initialise the device driver for use with the passed in
device context. We also switch the system into graphics mode when this
function is called.
****************************************************************************/
ibool MGLAPI SNAP_initDriver(
    void *data,
    MGLDC *dc,
    modeent *mode,
    ulong hwnd,
    int virtualX,
    int virtualY,
    int numBuffers,
    ibool stereo,
    int refreshRate,
    ibool useLinearBlits)
{
    return SNAP_initHardware((SNAP_data*)data,dc,mode,hwnd,virtualX,virtualY,numBuffers,stereo,refreshRate,useLinearBlits);
}

/****************************************************************************
REMARKS:
Frees the internal data structures allocated during the detection process
****************************************************************************/
void MGLAPI SNAP_destroyInstance(
    void *data)
{
    if (data) {
        SNAP_unload((SNAP_data*)data);
        PM_free(data);
        }
}

