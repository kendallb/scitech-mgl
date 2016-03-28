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
* Description:  Simple program that allows the memory clock settings to
*               be changed for SciTech SNAP Graphics program.
*
****************************************************************************/

#include "snap/graphics.h"
#include "snap/copyrigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------- Global Variables ---------------------------*/

static GA_devCtx        *dc = NULL;
static GA_initFuncs     init;
static GA_configInfo    info;
static GA_options       options;
static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("GAMemClk - %s Memory Clock Selection Program\n", GRAPHICS_PRODUCT);
    printf("           %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
    printf("\n");
}

/****************************************************************************
REMARKS:
Display the help screen.
****************************************************************************/
static void help(void)
{
    banner();
    printf("Valid commands are:\n");
    printf("\n");
    printf("  gamemclk show [device]\n");
    printf("\n");
    printf("     To show the current settings for the device.\n");
    printf("\n");
    printf("  gamemclk set <memory clock> [device]\n");
    printf("\n");
    printf("     To change the current memory clock for the device.\n");
    printf("\n");
    printf("  gamemclk eset <engine clock> [device]\n");
    printf("\n");
    printf("     To change the current engine clock for the device (if available).\n");
    exit(-1);
}

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

/****************************************************************************
REMARKS:
Load the SNAP driver and intialise it.
****************************************************************************/
static void LoadDriver(
    int deviceIndex)
{
    int numDevices;

    if (!dc) {
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

        /* Load the driver */
        PM_lockSNAPAccess(0, true);
        GA_setActiveDevice(deviceIndex);
        if ((dc = GA_loadDriver(deviceIndex,false)) == NULL)
            PM_fatalError(GA_errorMsg(GA_status()));
        GA_setActiveDevice(0);
        PM_unlockSNAPAccess(0);

        init.dwSize = sizeof(init);
        if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
            PM_fatalError("Unable to get device driver functions!");
        info.dwSize = sizeof(info);
        init.GetConfigInfo(&info);
        options.dwSize = sizeof(options);
        init.GetOptions(&options);

        /* Cleanup. We do this here because we know that we will not
         * be setting any graphics modes after loading the driver,
         * so we can properly restore the console state and make calls
         * to non-destructive functions in the driver before we unload
         * it.
         */
        Cleanup();
        }
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ShowSettings(
    int deviceIndex)
{
    LoadDriver(deviceIndex);
    printf(
        "Memory clock settings for %s %s (device %d):\n\n"
        "  Default: %.2f MHz\n"
        "  Current: %.2f MHz\n"
        "  Maximum: %.2f MHz\n",
        info.ManufacturerName, info.ChipsetName,
        deviceIndex,
        options.defaultMemoryClock / 65536.0,
        options.memoryClock / 65536.0,
        options.maxMemoryClock / 65536.0);
    if (dc->Attributes & gaHaveEngineClock) {
        printf(
            "\n"
            "Engine clock settings for %s %s (device %d):\n\n"
            "  Default: %.2f MHz\n"
            "  Current: %.2f MHz\n"
            "  Maximum: %.2f MHz\n",
            info.ManufacturerName, info.ChipsetName,
            deviceIndex,
            options.defaultEngineClock / 65536.0,
            options.engineClock / 65536.0,
            options.maxEngineClock / 65536.0);
        }
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void SetMemClock(
    float memClock,
    int deviceIndex)
{
    N_fix32 fxMemClock = N_FLTTOFIX(memClock);

    LoadDriver(deviceIndex);
    if (fxMemClock > options.maxMemoryClock)
        printf("Memory clock too high!\n");
    else {
        options.memoryClock = fxMemClock;
        init.SetOptions(&options);
        GA_saveOptions(dc,&options);
        ShowSettings(deviceIndex);
        }
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void SetEngClock(
    float engClock,
    int deviceIndex)
{
    N_fix32 fxMemClock = N_FLTTOFIX(engClock);

    LoadDriver(deviceIndex);
    if (dc->Attributes & gaHaveEngineClock) {
        if (fxMemClock > options.maxEngineClock)
            printf("Engine clock too high!\n");
        else {
            options.engineClock = fxMemClock;
            init.SetOptions(&options);
            GA_saveOptions(dc,&options);
            ShowSettings(deviceIndex);
            }
        }
    else
        printf("Device does not support a programmable engine clock.\n");
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    /* Now handle the query */
    if (argc >= 2 && stricmp(argv[1],"show") == 0) {
        if (argc == 2)
            ShowSettings(0);
        else
            ShowSettings(atoi(argv[2]));
        }
    else if (argc >= 3 && stricmp(argv[1],"set") == 0) {
        if (argc == 3)
            SetMemClock(atof(argv[2]),0);
        else
            SetMemClock(atof(argv[2]),atoi(argv[3]));
        }
    else if (argc >= 3 && stricmp(argv[1],"eset") == 0) {
        if (argc == 3)
            SetEngClock(atof(argv[2]),0);
        else
            SetEngClock(atof(argv[2]),atoi(argv[3]));
        }
    else
        help();
    if (dc)
        GA_unloadDriver(dc);
    return 0;
}
