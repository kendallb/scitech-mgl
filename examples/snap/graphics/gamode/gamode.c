/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
* Description:  Simple program that allows the user to add and remove
*               display modes from the SciTech SNAP Graphics drivers.
*
****************************************************************************/

#include "snap/graphics.h"
#include "snap/copyrigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*--------------------------- Global variables ----------------------------*/

static GA_initFuncs     init;
static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("GAMode - %s Dial-A-Mode Utility\n", GRAPHICS_PRODUCT);
    printf("         %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
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
    printf("  gamode show [device]\n");
    printf("  gamode showvbe [device]\n");
    printf("     To show the list of display modes for the device. The showvbe variant\n");
    printf("     will show the list of modes reported to VESA VBE applications.\n");
    printf("\n");
    printf("  gamode add <xres> <yres> <bits> [device]\n");
    printf("     To add a new display mode (automatically enabled for VESA VBE apps)\n");
    printf("\n");
    printf("  gamode addref <xres> <yres> <refresh> [device]\n");
    printf("     To add a new refresh rate for a specific resolution. To specify an\n");
    printf("     interlaced refresh rate, use a negative value. i.e.: use -87 for 87Hz\n");
    printf("     interlaced modes.\n");
    printf("\n");
    printf("  gamode del <xres> <yres> <bits> [device]\n");
    printf("     To remove a specific display mode\n");
    printf("\n");
    printf("  gamode set <xres> <yres> <bits> <refresh> [head] [device]\n");
    printf("     To set the default refresh rate for a specific resolution\n");
    exit(-1);
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ShowSettings(
    GA_devCtx *dc)
{
    GA_configInfo   info;
    GA_modeProfile  modes;
    GA_modeInfo     mi;
    N_uint16        *m;
    int             i,j;

    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    modes.dwSize = sizeof(modes);
    GA_loadModeProfile(dc,&modes);
    printf("Display modes for %s %s (device %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        dc->DeviceIndex);
    for (m = dc->AvailableModes; *m != 0xFFFF; m++) {
        mi.dwSize = sizeof(mi);
        if (init.GetVideoModeInfo(*m,&mi) != 0)
            continue;
        if (mi.Attributes & (gaIsGUIDesktop | gaIsVirtualMode))
            continue;
        for (i = 0; i < modes.m.numModes; i++) {
            if (mi.Attributes & gaIsTextMode) {
                printf("  %4d x %4d Text @ ",
                    mi.XResolution / mi.XCharSize,
                    mi.YResolution / mi.YCharSize);
                if (mi.RefreshRateList[0] == -1) {
                    printf("Default Refresh\n");
                    }
                else {
                    for (j = 0; mi.RefreshRateList[j] != -1; j++) {
                        if (j > 0)
                            printf(",");
                        if (mi.RefreshRateList[j] < 0)
                            printf("%di", -mi.RefreshRateList[j]);
                        else
                            printf("%d", mi.RefreshRateList[j]);
                        }
                    printf(" Hz\n");
                    }
                break;
                }
            else {
                printf("  %4d x %4d x %2d @ ",
                    mi.XResolution,
                    mi.YResolution,
                    mi.BitsPerPixel);
                if (mi.RefreshRateList[0] == -1) {
                    printf("Default Refresh\n");
                    }
                else {
                    for (j = 0; mi.RefreshRateList[j] != -1; j++) {
                        if (j > 0)
                            printf(",");
                        if (mi.RefreshRateList[j] < 0)
                            printf("%di", -mi.RefreshRateList[j]);
                        else
                            printf("%d", mi.RefreshRateList[j]);
                        }
                    printf(" Hz\n");
                    }
                break;
                }
            }
        }
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ShowVBESettings(
    GA_devCtx *dc)
{
    GA_configInfo   info;
    GA_modeProfile  modes;
    GA_modeInfo     mi;
    N_uint16        *m;
    int             i;

    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    modes.dwSize = sizeof(modes);
    GA_loadModeProfile(dc,&modes);
    printf("Available VBE modes for %s %s (device %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        dc->DeviceIndex);
    for (m = dc->AvailableModes; *m != 0xFFFF; m++) {
        mi.dwSize = sizeof(mi);
        if (init.GetVideoModeInfo(*m,&mi) != 0)
            continue;
        if (mi.Attributes & gaIsGUIDesktop)
            continue;
        for (i = 0; i < modes.vbe.numModes; i++) {
            if (mi.Attributes & gaIsTextMode) {
                int xRes = mi.XResolution / mi.XCharSize;
                int yRes = mi.YResolution / mi.YCharSize;
                if (modes.vbe.modeList[i].xRes == xRes
                    && modes.vbe.modeList[i].yRes == yRes
                    && modes.vbe.modeList[i].bits == mi.BitsPerPixel) {
                    printf("  %4d x %4d Text @ ",
                        modes.vbe.modeList[i].xRes,
                        modes.vbe.modeList[i].yRes);
                    if (mi.DefaultRefreshRate < 0)
                        printf("%di", -mi.DefaultRefreshRate);
                    else
                        printf("%d", mi.DefaultRefreshRate);
                    printf(" Hz\n");
                    }
                }
            else if (modes.vbe.modeList[i].xRes == mi.XResolution
                && modes.vbe.modeList[i].yRes == mi.YResolution
                && modes.vbe.modeList[i].bits == mi.BitsPerPixel) {
                printf("  %4d x %4d x %2d @ ",
                    modes.vbe.modeList[i].xRes,
                    modes.vbe.modeList[i].yRes,
                    modes.vbe.modeList[i].bits);
                if (mi.DefaultRefreshRate < 0)
                    printf("%di", -mi.DefaultRefreshRate);
                else
                    printf("%d", mi.DefaultRefreshRate);
                printf(" Hz\n");
                }
            }
        }
}

/****************************************************************************
REMARKS:
Add a new mode to the driver.
****************************************************************************/
static void AddMode(
    GA_devCtx *dc,
    N_int32 xRes,
    N_int32 yRes,
    N_int32 bits)
{
    GA_enableVBEMode(dc,xRes,yRes,bits,true);
    if (GA_addMode(dc,xRes,yRes,bits,true))
        printf("Mode %d x %d x %d successfully added.\n",xRes,yRes,bits);
    else
        printf("Failed to add mode %d x %d x %d!\n",xRes,yRes,bits);
}

/****************************************************************************
REMARKS:
Add a new refresh rate to the display driver.
****************************************************************************/
static void AddRefresh(
    GA_devCtx *dc,
    N_int32 xRes,
    N_int32 yRes,
    N_int32 refresh)
{
    if (GA_addRefresh(dc,xRes,yRes,refresh,true)) {
        if (refresh < 0) {
            printf("Mode %d x %d @ %d Hz interlaced successfully added.\n",xRes,yRes,refresh);
            }
        else {
            printf("Mode %d x %d @ %d Hz successfully added.\n",xRes,yRes,refresh);
            }
        }
    else
        printf("Could not add refresh rate %d x %d @ %d Hz!\n",xRes,yRes,refresh);
}

/****************************************************************************
REMARKS:
Delete a mode for the driver.
****************************************************************************/
static void DelMode(
    GA_devCtx *dc,
    N_int32 xRes,
    N_int32 yRes,
    N_int32 bits)
{
    GA_disableVBEMode(dc,xRes,yRes,bits,true);
    if (GA_delMode(dc,xRes,yRes,bits,true))
        printf("Mode %d x %d x %d successfully deleted.\n",xRes,yRes,bits);
    else
        printf("Failed to delete mode %d x %d x %d!\n",xRes,yRes,bits);
}

/****************************************************************************
REMARKS:
Set the default refresh rate for a mode in the driver
****************************************************************************/
static void SetMode(
    GA_devCtx *dc,
    N_int32 xRes,
    N_int32 yRes,
    N_int32 bits,
    N_int32 refreshRate,
    N_int32 head)
{
    if (GA_setDefaultRefresh(dc,head,xRes,yRes,bits,refreshRate,true)) {
        if (refreshRate < 0)
            printf("Default set to %d x %d @ %dHz Interlaced successfully.\n",xRes,yRes,-refreshRate);
        else
            printf("Default set to %d x %d @ %dHz successfully.\n",xRes,yRes,refreshRate);
        }
    else
        printf("Failed to set default refresh for %d x %d @ %dHz!\n",xRes,yRes,refreshRate);
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
static GA_devCtx *LoadDriver(
    int deviceIndex)
{
    GA_devCtx   *dc;
    int         numDevices;

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

    /* Cleanup. We do this here because we know that we will not
     * be setting any graphics modes after loading the driver,
     * so we can properly restore the console state and make calls
     * to non-destructive functions in the driver before we unload
     * it.
     */
    Cleanup();
    return dc;
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    GA_devCtx *dc = NULL;

    /* Now handle the query */
    if (argc >= 2 && stricmp(argv[1],"show") == 0) {
        dc = LoadDriver((argc > 2) ? atoi(argv[2]) : 0);
        ShowSettings(dc);
        }
    else if (argc >= 2 && stricmp(argv[1],"showvbe") == 0) {
        dc = LoadDriver((argc > 2) ? atoi(argv[2]) : 0);
        ShowVBESettings(dc);
        }
    else if (argc >= 5 && stricmp(argv[1],"add") == 0) {
        dc = LoadDriver((argc > 5) ? atoi(argv[5]) : 0);
        AddMode(dc,atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
        }
    else if (argc >= 5 && stricmp(argv[1],"addref") == 0) {
        dc = LoadDriver((argc > 5) ? atoi(argv[5]) : 0);
        AddRefresh(dc,atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
        }
    else if (argc >= 5 && stricmp(argv[1],"del") == 0) {
        dc = LoadDriver((argc > 5) ? atoi(argv[5]) : 0);
        DelMode(dc,atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
        }
    else if (argc >= 6 && stricmp(argv[1],"set") == 0) {
        dc = LoadDriver((argc > 6) ? atoi(argv[6]) : 0);
        SetMode(dc,atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),
            (argc > 7) ? atoi(argv[7]) : 0);
        }
    else
        help();
    if (dc)
        GA_unloadDriver(dc);
    return 0;
}

