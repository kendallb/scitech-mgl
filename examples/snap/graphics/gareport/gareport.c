/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Description:  Simple program that reports the detected hardware
*               configuration from SciTech SNAP Graphics (to be used in
*               scripts).
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/gasdk.h"
#ifdef  ISV_LICENSE
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
static void PMAPI Cleanup(void)
{
    /* Reset active device and release semaphore to be sure */
    if (GA_getActiveDevice() != 0) {
        GA_setActiveDevice(0);
        PM_unlockSNAPAccess(0);
        }
    if (stateBuf) {
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        stateBuf = NULL;
        }
}

int main(int argc,char *argv[])
{
    int                 deviceIndex = 0, numDevices;
    GA_devCtx           *dc;
    GA_configInfo       info;
    GA_initFuncs        init;
    GA_globalOptions    gOpt;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Determine if we should disable logging. This shows how you
     * can turn off logging with SNAP in your own programs.
     */
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
        gOpt.dwSize = sizeof(gOpt);
        GA_getGlobalOptions(&gOpt, false);
        gOpt.bDisableLogFile = 1;
        gOpt.bNoDDCDetect = 1;
        GA_setGlobalOptions(&gOpt);
        argc--;
        argv++;
        }

    /* Modified to overide CPU calibration step too. */
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'c') {
        gOpt.dwSize = sizeof(gOpt);
        GA_getGlobalOptions(&gOpt, false);
        gOpt.bNoCPUCalibration = true;
        gOpt.dwCPUClockSpeed = atoi(&argv[1][2]);
        GA_setGlobalOptions(&gOpt);
        argc--;
        argv++;
        }

    /* Get device index from command line */
    if (argc > 1)
        deviceIndex = atoi(argv[1]);

    /* Save the state of the console. If we are connecting to the shared
     * driver, we don't need to save/restore the console state because
     * we are just getting information from the driver.
     */
    if (!GA_isSharedDriverLoaded()) {
        hwndConsole = PM_openConsole(0,0,640,480,8,true);
        if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
            PM_fatalError("Out of memory!");
        PM_saveConsoleState(stateBuf,hwndConsole);
        }

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(Cleanup);

    /* Check if the specified device number is present */
    numDevices = GA_enumerateDevices(false);
    if (deviceIndex >= numDevices)
        PM_fatalError("Device not found!");

    /* Load the device driver for this device */
    PM_lockSNAPAccess(0, true);
    GA_setActiveDevice(deviceIndex);
    if ((dc = GA_loadDriver(deviceIndex,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    GA_setActiveDevice(0);
    PM_unlockSNAPAccess(0);

    /* Cleanup. We do this here because we know that we will not
     * be setting any graphics modes after loading the driver,
     * so we can properly restore the console state and make calls
     * to non-destructive functions in the driver before we unload
     * it.
     */
    Cleanup();

    /* Get configuration information and report it */
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    printf(
        "Manufacturer:    %s\n"
        "Chipset:         %s\n"
        "Memory:          %d Kb\n"
        "DAC:             %s\n"
        "Clock:           %s\n"
        "Driver Revision: %d.%d, %s\n"
        "Driver Build:    %s\n",
        info.ManufacturerName, info.ChipsetName, dc->TotalMemory,
        info.DACName, info.ClockName, (int)dc->Version >> 8,
        (int)dc->Version & 0xFF, info.VersionInfo, info.BuildDate
        );
    GA_unloadDriver(dc);
    return 0;
}
