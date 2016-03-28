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
* Description:  Simple program that allows a monitor to be selected and
*               made the active monitor for SciTech SNAP Graphics programs.
*
****************************************************************************/

#include "scitech.h"
#include "snap/graphics.h"
#include "pmapi.h"
#include "snap/copyrigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*--------------------------- Global variables ----------------------------*/

static GA_initFuncs     init;
static GA_driverFuncs   driver;
static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

#define DLL_NAME        "graphics.bpd"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("GAMonitor - %s Monitor Selection Program\n", GRAPHICS_PRODUCT);
    printf("            %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
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
    printf("  gamon show [head] [device]\n");
    printf("\n");
    printf("     To list the monitors for the device (default is head 0, device 0)\n");
    printf("\n");
    printf("  gamon mfr [mfrname]\n");
    printf("\n");
    printf("     To list all known monitor manufacturer names. You can optionally include\n");
    printf("     a partial manufacturer name that will be used to narrow the search.\n");
    printf("\n");
    printf("  gamon model <mfr>\n");
    printf("\n");
    printf("     To list all known monitor models for a specific manufacturer.\n");
    printf("\n");
    printf("  gamon select <mfr> <model> [head] [device]\n");
    printf("\n");
    printf("     To select a monitor for the specified device. If the device number\n");
    printf("     is not included, we assume the default monitor (ie: device 0). Note also\n");
    printf("     that the passed in manufacturer and model strings can be partial matches.\n");
    printf("\n");
    printf("  gamon enable|disable <DPMS | GTF | Wide | 4:3> [head] [device]\n");
    printf("\n");
    printf("     To enable or disable specific features for the selected monitor.\n");
    printf("\n");
    printf("  gamon import [filename.INF]\n");
    printf("\n");
    printf("     To import a Windows INF monitor file.\n");
    exit(-1);
}

/****************************************************************************
REMARKS:
Open the monitor database on disk.
****************************************************************************/
static void OpenDB(
    GA_devCtx *dc)
{
    char    filename[PM_MAX_PATH];

    /* Find path to monitor database */
    if (!dc)
        PM_fatalError("Load driver first before opening monitor database!");
    strcpy(filename,GA_getSNAPConfigPath());
    PM_backslash(filename);
    strcat(filename,"monitor.dbx");

    /* Open the monitor database */
    if (!MDBX_open(filename)) {
        printf("Unable to open monitor database!\n");
        exit(-1);
        }
}

/****************************************************************************
PARAMETERS:
mfrFilter   - String to filter manufacturers on

REMARKS:
Display the list of all manufacturers in the database for a given string.
****************************************************************************/
static void ListMFR(
    GA_devCtx *dc,
    const char *mfrFilter)
{
    int         len = strlen(mfrFilter);
    char        mfr[MONITOR_MFR_LEN+1] = "";
    GA_monitor  monitor;

    printf("List of monitor manufacturer names:\n\n");
    OpenDB(dc);
    if (MDBX_first(&monitor) == MDBX_ok) {
        do {
            if (strcmp(monitor.mfr,mfr) != 0) {
                strcpy(mfr,monitor.mfr);
                if (strnicmp(mfr,mfrFilter,len) == 0)
                    printf("%s\n", mfr);
                }
            } while (MDBX_next(&monitor) == MDBX_ok);
        }
    MDBX_close();
}

/****************************************************************************
PARAMETERS:
mfrFilter   - String to filter manufacturers on

REMARKS:
Display the list of all models in the database for a given string.
****************************************************************************/
static void ListModel(
    GA_devCtx *dc,
    const char *mfrFilter)
{
    int         len = strlen(mfrFilter);
    GA_monitor  monitor;

    printf("List of monitor model names:\n\n");
    OpenDB(dc);
    if (MDBX_first(&monitor) == MDBX_ok) {
        do {
            if (len == 0) {
                printf("%-7s %3dkHz, %3dHz, %4s %s: %s\n",
                    monitor.PNPID,
                    monitor.maxHScan,
                    monitor.maxVScan,
                    (monitor.flags & Monitor_DPMSEnabled) ? "DPMS" : "",
                    monitor.mfr,
                    monitor.model);
                }
            else if (strnicmp(monitor.mfr,mfrFilter,len) == 0) {
                printf("%s: %s\n", monitor.mfr, monitor.model);
                }
            } while (MDBX_next(&monitor) == MDBX_ok);
        }
    MDBX_close();
}

/****************************************************************************
REMARKS:
Compatibility function to convert a string to lowercase for Unix.
****************************************************************************/
static void my_strlwr(char *s)
{
    while (*s) {
        *s = (char)tolower(*s); s++;
        }
}

/****************************************************************************
PARAMETERS:
mfrFilter   - String to filter manufacturers on
modelFilter - String to filter models on

REMARKS:
Select a particular model and write the monitor information to disk.
****************************************************************************/
static void SelectModel(
    GA_devCtx *dc,
    const char *mfrFilter,
    const char *modelFilter,
    int head,
    int device)
{
    int             mfrLen = strlen(mfrFilter),found = false;
    char            model[80],filter[80];
    GA_monitor      monitor;
    GA_configInfo   info;

    /* Display monitor header info */
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    printf("Monitor for %s %s (device %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        dc->DeviceIndex);

    /* Find the monitor in the database */
    strcpy(filter,modelFilter);
    my_strlwr(filter);
    OpenDB(dc);
    if (MDBX_first(&monitor) == MDBX_ok) {
        do {
            strcpy(model,monitor.model);
            my_strlwr(model);
            if (strnicmp(monitor.mfr,mfrFilter,mfrLen) == 0 && strstr(model,filter) != NULL) {
                found = true;
                break;
                }
            } while (MDBX_next(&monitor) == MDBX_ok);
        }
    MDBX_close();

    /* Now write the selected record to disk */
    if (found) {
        /* Change the active monitor in the loaded driver */
        init.SetMonitorInfo(&monitor,head);

        /* Save the monitor to disk */
        if (!GA_saveMonitorInfo(dc,head,&monitor)) {
            printf("Unable to write monitor configuration file!\n");
            exit(-1);
            }
        printf("Selected monitor (device %d):\n\n", device);
        printf("%s: %s\n", monitor.mfr, monitor.model);
        }
    else
        printf("Monitor not found!\n");
}

/****************************************************************************
REMARKS:
Return the string for the maximum resolution for the monitor.
****************************************************************************/
const char *GetMaxRes(
    int maxRes)
{
    switch (maxRes) {
        case MaxRes_640x480:    return "640 x 480";     break;
        case MaxRes_800x600:    return "800 x 600";     break;
        case MaxRes_1024x768:   return "1024 x 768";    break;
        case MaxRes_1152x864:   return "1152 x 864";    break;
        case MaxRes_1280x1024:  return "1280 x 1024";   break;
        case MaxRes_1600x1200:  return "1600 x 1200";   break;
        case MaxRes_1800x1350:  return "1800 x 1350";   break;
        case MaxRes_1920x1440:  return "1920 x 1440";   break;
        case MaxRes_2048x1536:  return "2048 x 1536";   break;
        }
    return "Unknown!";
}

/****************************************************************************
REMARKS:
Displays the currently select monitor information.
****************************************************************************/
void DisplayMonitor(
    GA_devCtx *dc,
    int head)
{
    GA_monitor      monitor;
    GA_configInfo   info;
    ibool           comma;

    /* Display monitor header info */
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    printf("Monitor for %s %s (head %d, device %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        head, dc->DeviceIndex);

    /* Display monitor information */
    init.GetMonitorInfo(&monitor,head);
    printf("  Manufacturer:    %s\n", monitor.mfr);
    printf("  Model:           %s\n",monitor.model);
    printf("  Max Resolution:  %s\n",GetMaxRes(monitor.maxResolution));
    printf("  Horizontal Scan: %dkHz - %dkHz\n",monitor.minHScan,monitor.maxHScan);
    printf("  Vertical Scan:   %dHz - %dHz\n",monitor.minVScan,monitor.maxVScan);
    if (monitor.flags & (Monitor_DPMSEnabled | Monitor_GTFEnabled | Monitor_Widescreen)) {
        printf("  Capabilities:    ");
        comma = false;
        if (monitor.flags & Monitor_DPMSEnabled) {
            printf("DPMS");
            comma = true;
            }
        if (monitor.flags & Monitor_GTFEnabled) {
            if (comma)
                printf(", ");
            printf("GTF");
            comma = true;
            }
        if (monitor.flags & Monitor_Widescreen) {
            if (comma)
                printf(", ");
            printf("Wide");
            comma = true;
            }
        if (monitor.flags & Monitor_Exclude4to3) {
            if (comma)
                printf(", ");
            printf("Exclude 4:3");
            }
        printf("\n");
        }
}

/****************************************************************************
REMARKS:
Enable a specific feature for the monitor
****************************************************************************/
void EnableFeature(
    GA_devCtx *dc,
    char *feature,
    int enable,
    int head,
    int device)
{
    GA_monitor  monitor;

    /* Now enable the specific feature */
    init.GetMonitorInfo(&monitor,head);
    if (stricmp(feature,"DPMS") == 0) {
        if (enable)
            monitor.flags |= Monitor_DPMSEnabled;
        else
            monitor.flags &= ~Monitor_DPMSEnabled;
        }
    if (stricmp(feature,"GTF") == 0) {
        if (enable)
            monitor.flags |= Monitor_GTFEnabled;
        else
            monitor.flags &= ~Monitor_GTFEnabled;
        }
    if (stricmp(feature,"Wide") == 0) {
        if (enable)
            monitor.flags |= Monitor_Widescreen;
        else
            monitor.flags &= ~Monitor_Widescreen;
        }
    if (stricmp(feature,"4:3") == 0) {
        if (enable)
            monitor.flags |= Monitor_Exclude4to3;
        else
            monitor.flags &= ~Monitor_Exclude4to3;
        }

    /* Change the active monitor in the loaded driver */
    init.SetMonitorInfo(&monitor,head);

    /* Save the monitor to disk */
    if (!GA_saveMonitorInfo(dc,head,&monitor)) {
        printf("Unable to write monitor configuration file!\n");
        exit(-1);
        }
    DisplayMonitor(dc,head);
}

/****************************************************************************
REMARKS:
Creates an empty database if one does not exist.
****************************************************************************/
void CreateMonitorDBX(
    const char *filename)
{
    FILE        *fDB;
    MDBX_header h;
    GA_monitor  rec;

    /* Write the header to disk */
    if ((fDB = fopen(filename,"wb")) == NULL)
        PM_fatalError("Unable to create monitor.dbx");
    strcpy(h.signature,MDBX_SIGNATURE);
    h.entries = 0;
    fwrite(&h,1,sizeof(h),fDB);
    fclose(fDB);
    if (!MDBX_open(filename))
        PM_fatalError("Unable to open database");

    /* Insert an entry for the unknown monitor */
    strcpy(rec.mfr,"(Standard monitor types)");
    strcpy(rec.model,"Unknown monitor");
    rec.PNPID[0] = 0;
    rec.maxResolution = 0xFF;
    rec.minHScan = 0;
    rec.maxHScan = 0;
    rec.minVScan = 0;
    rec.maxVScan = 0;
    rec.flags = 0;
    if (MDBX_insert(&rec) != MDBX_ok)
        PM_fatalError("Failure to insert record!");
    MDBX_flush();
    MDBX_close();
}

/****************************************************************************
REMARKS:
Imports a Windows INF file.
****************************************************************************/
void ImportINF(
    char *INFPath)
{
    FILE    *f;
    char    buf[PM_MAX_PATH];
    char    filename[PM_MAX_PATH];
    int     len;

    strcpy(filename,GA_getSNAPConfigPath());
    PM_backslash(filename);
    strcat(filename,"monitor.dbx");
    if (!MDBX_open(filename)) {
        if (INFPath[0] == '@') {
            /* Create the monitor database if it does not exist when
             * importing multiple INF files.
             */
            CreateMonitorDBX(filename);
            }
        if (!MDBX_open(filename))
            PM_fatalError(MDBX_getErrorMsg());
        }
    if (INFPath[0] == '@') {
        if ((f = fopen(INFPath+1,"r")) == NULL)
            PM_fatalError("Unable to open input file!");
        printf("Importing monitors .");
        fflush(stdout);
        while (fgets(buf,sizeof(buf),f)) {
            len = strlen(buf);
            if (buf[len-1] == '\n')
                buf[--len] = '\0';
            if (len > 0) {
                if (MDBX_importINF(buf,NULL) != MDBX_ok) {
                    printf("\n\nFILE: %s - %s\n\n", buf, MDBX_getErrorMsg());
                    }
                printf(".");
                fflush(stdout);
                }
            }
        fclose(f);
        printf("\nDone!\n");
        }
    else {
        if (MDBX_importINF(INFPath,NULL) != MDBX_ok) {
            MDBX_close();
            PM_fatalError(MDBX_getErrorMsg());
            }
        }
    if (MDBX_flush() != MDBX_ok) {
        MDBX_close();
        PM_fatalError(MDBX_getErrorMsg());
        }
    MDBX_close();
    if (INFPath[0] != '@')
        printf("INF file successfully imported!");
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
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
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
    GA_devCtx   *dc = NULL;
    int         head,device;

    /* Now handle the query */
    if (argc >= 2 && stricmp(argv[1],"mfr") == 0) {
        dc = LoadDriver(0);
        if (argc > 2)
            ListMFR(dc,argv[2]);
        else
            ListMFR(dc,"");
        }
    else if (argc == 3 && stricmp(argv[1],"model") == 0) {
        dc = LoadDriver(0);
        if (stricmp(argv[2],"all") == 0)
            ListModel(dc,"");
        else
            ListModel(dc,argv[2]);
        }
    else if (argc >= 4 && stricmp(argv[1],"select") == 0) {
        head = (argc > 4) ? atoi(argv[4]) : 0;
        device = (argc > 5) ? atoi(argv[5]) : 0;
        dc = LoadDriver(device);
        SelectModel(dc,argv[2],argv[3],head,device);
        }
    else if (argc >= 3 && stricmp(argv[1],"enable") == 0) {
        head = (argc > 3) ? atoi(argv[3]) : 0;
        device = (argc > 4) ? atoi(argv[4]) : 0;
        dc = LoadDriver(device);
        EnableFeature(dc,argv[2],true,head,device);
        }
    else if (argc >= 3 && stricmp(argv[1],"disable") == 0) {
        head = (argc > 3) ? atoi(argv[3]) : 0;
        device = (argc > 4) ? atoi(argv[4]) : 0;
        dc = LoadDriver(device);
        EnableFeature(dc,argv[2],false,head,device);
        }
    else if (argc >= 2 && stricmp(argv[1],"show") == 0) {
        head = (argc > 2) ? atoi(argv[2]) : 0;
        device = (argc > 3) ? atoi(argv[3]) : 0;
        dc = LoadDriver(device);
        DisplayMonitor(dc,head);
        }
    else if (argc == 3 && stricmp(argv[1],"import") == 0) {
        dc = LoadDriver(0);
        ImportINF(argv[2]);
        }
    else
        help();

    if (dc)
        GA_unloadDriver(dc);
    return 0;
}

