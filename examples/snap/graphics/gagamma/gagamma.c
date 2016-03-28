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
* Description:  Simple program that allows the SciTech SNAP Graphics gamma
*               ramp to be changed from the command line.
*
****************************************************************************/

#include "snap/graphics.h"
#include "snap/copyrigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*---------------------------- Global Variables ---------------------------*/

static GA_devCtx        *dc = NULL;
static GA_initFuncs     init;
static GA_configInfo    info;
static GA_options       opt;
static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("GAGamma - %s Gamma Ramp Selection Program\n", GRAPHICS_PRODUCT);
    printf("          %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
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
    printf("  gagamma export [device]\n");
    printf("  gagamma import filename [device]\n");
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
        opt.dwSize = sizeof(opt);
        init.GetOptions(&opt);

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
static void ExportGamma(
    int deviceIndex)
{
    int i;

    LoadDriver(deviceIndex);
    printf(
        "Gamma ramp for %s %s (device %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        deviceIndex);
    for (i = 0; i < 256; i++) {
        printf("%03d: %04X, %04X, %04X\n",
            i,
            opt.gammaRamp[i].Red,
            opt.gammaRamp[i].Green,
            opt.gammaRamp[i].Blue);
        }
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ImportGamma(
    const char *filename,
    int deviceIndex)
{
    FILE    *f;
    char    buf[256];
    int     i,red,green,blue;

    if ((f = fopen(filename,"r")) == NULL) {
        printf("Unable to open file '%s'\n", filename);
        exit(-1);
        }
    LoadDriver(deviceIndex);
    while (!feof(f) && fgets(buf,sizeof(buf),f)) {
        if (sscanf(buf,"%d: %x,%x,%x",&i,&red,&green,&blue) == 4) {
            opt.gammaRamp[i].Red = red;
            opt.gammaRamp[i].Green = green;
            opt.gammaRamp[i].Blue = blue;
            }
        }
    fclose(f);
    init.SetOptions(&opt);
    GA_saveOptions(dc,&opt);
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
    if (argc >= 2 && stricmp(argv[1],"export") == 0) {
        if (argc == 2)
            ExportGamma(0);
        else
            ExportGamma(atoi(argv[2]));
        }
    else if (argc >= 3 && stricmp(argv[1],"import") == 0) {
        int device = (argc == 3) ? 0 : atoi(argv[3]);
        ImportGamma(argv[2],device);
        }
    else
        help();
    if (dc)
        GA_unloadDriver(dc);
    return 0;
}

