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
* Description:  Simple program that allows the SciTech SNAP Graphics
*               options to be changed from the command line.
*
****************************************************************************/

#include "snap/graphics.h"
#include "snap/copyrigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*---------------------------- Global Variables ---------------------------*/

#ifdef ISV_LICENSE
#include "isv.c"
#endif

static GA_devCtx        *dc = NULL;
static GA_initFuncs     init;
static GA_configInfo    info;
static GA_options       opt;
static GA_globalOptions gOpt;
GA_driverFuncs          driver;
static PM_HWND          hwndConsole;
static void             *stateBuf = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Unloads the SNAP Graphics driver when accessed from shared memory. (Windows)
****************************************************************************/
static void Unload(void)
{
    if (dc) {
        GA_unloadDriver(dc);
        dc = NULL;
        }
}

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("GAOption - %s Option Selection Program\n", GRAPHICS_PRODUCT);
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
    printf("  gaoption show       <global | accel> [device]\n");
    printf("  gaoption accel      <function> <on | off> [device]\n");
    printf("  gaoption accel      <none | basic | most | full> [device]\n");
    printf("  gaoption vbe        <on | off>\n");
    printf("  gaoption vga        <on | off>\n");
    printf("  gaoption noncert    <on | off>\n");
    printf("  gaoption nowcomb    <on | off>\n");
    printf("  gaoption noddc      <on | off>\n");
    printf("  gaoption agpfwrite  <on | off>\n");
    printf("  gaoption agprate    <1 | 2 | 4 | 8>\n");
    printf("  gaoption lcdusebios <auto | on | off>\n");
    printf("  gaoption vidmem     <memsize | off>\n");
    printf("  gaoption agpmem     <memsize>\n");
    printf("  gaoption usesysmem  <memsize | off>\n");
    printf("  gaoption dvireduce  <on | off> [device]\n");
    printf("  gaoption prefer16   <on | off> [device]\n");
    printf("  gaoption prefer32   <on | off> [device]\n");
    printf("  gaoption compress   <on | off> [device]\n");
    printf("  gaoption ddcbios    <on | off> [device]\n");
    printf("  gaoption busmaster  <on | off> [device]\n");
    printf("  gaoption invert     <on | off> [device]\n");
    printf("  gaoption rotation   <on | off> [device]\n");
    printf("  gaoption flipped    <on | off> [device]\n");
    printf("  gaoption virtual    <on | off> <xRes> <yRes> <horz count> <vert count>\n");
    printf("  gaoption multihead  <on | off> <xRes> <yRes> <horz count> <vert count> [device]\n");
    printf("  gaoption dpvl       <on | off> <xRes> <yRes> <horz count> <vert count> [device]\n");

    Unload();
    exit(-1);
}

/****************************************************************************
REMARKS:
Load the global device driver options.
****************************************************************************/
static void LoadGlobalOptions(void)
{
    gOpt.dwSize = sizeof(gOpt);
    GA_getGlobalOptions(&gOpt, false);
}

/****************************************************************************
REMARKS:
Closes the console and restores the previous console state.
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
Fatal error cleanup handler when called from PM_fatalError().
****************************************************************************/
static void PMAPI FatalErrorCleanup(void)
{
    Cleanup();
    Unload();
}

/****************************************************************************
REMARKS:
Fatal error cleanup handler when called from exit().
****************************************************************************/
static void ExitErrorCleanup(void)
{
    FatalErrorCleanup();
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
        PM_setFatalErrorCleanup(FatalErrorCleanup);
        atexit(ExitErrorCleanup);

#ifdef ISV_LICENSE
        GA_registerLicense(OemLicense,false);
#endif

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
        LoadGlobalOptions();

        /* Turn off multi-head support if the controller itself does
         * not support multi-head modes
         */
        if (!(dc->Attributes & gaHaveMultiHead))
            opt.bMultiHead = false;

        /* Turn off DPVL support if the driver itself does not support
         * DPVL enablement.
         */
        if (!(dc->AttributesExt & gaHaveDPVLMode))
            opt.bDPVLMode = false;

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
Function to enable virtual display mode. This is a simple function that
can enable any number of displays in a rectangular layout with each
display using the same resolution.

NOTE: Virtual mode is to split a display across multiple physical graphics
      cards, not a single card with multiple heads.
****************************************************************************/
static void EnableVirtualMode(
    int argc,
    char *argv[])
{
    int x,y,i,xRes,yRes,numX,numY;

    if (argc != 7)
        help();
    xRes = atoi(argv[3]);
    yRes = atoi(argv[4]);
    numX = atoi(argv[5]);
    numY = atoi(argv[6]);
    gOpt.virtualSize.left = gOpt.virtualSize.top = 0;
    gOpt.virtualSize.right = xRes * numX;
    gOpt.virtualSize.bottom = yRes * numY;
    memset(&gOpt.resolutions,0,sizeof(gOpt.resolutions));
    memset(&gOpt.bounds,0,sizeof(gOpt.bounds));
    for (i = x = 0; x < numX; x++) {
        for (y = 0; y < numY; y++, i++) {
            gOpt.resolutions[i].left = gOpt.resolutions[i].top = 0;
            gOpt.resolutions[i].right = xRes;
            gOpt.resolutions[i].bottom = yRes;
            gOpt.bounds[i].left = x * xRes;
            gOpt.bounds[i].top = y * yRes;
            gOpt.bounds[i].right = (x+1) * xRes;
            gOpt.bounds[i].bottom = (y+1) * yRes;
            }
        }
    gOpt.bVirtualDisplay = true;
}

/****************************************************************************
REMARKS:
Function to enable virtual display mode. This is a simple function that
can enable any number of displays in a rectangular layout with each
display using the same resolution.

NOTE: Multi-head mode is to split a display across a single graphics card
      that has multiple video connectors on it.
****************************************************************************/
static void EnableMultiHeadMode(
    int argc,
    char *argv[])
{
    int x,y,i,xRes,yRes,numX,numY;

    if (!(dc->Attributes & gaHaveMultiHead)){
        printf("MultiHead is not supported on this driver.\n");
        exit(-1);
        }

    if (argc != 7 && argc != 8)
        help();
    xRes = atoi(argv[3]);
    yRes = atoi(argv[4]);
    numX = atoi(argv[5]);
    numY = atoi(argv[6]);
    if (numX * numY > GA_MAX_HEADS){
        printf("Requested layout exceeds maximum number of heads.\n");
        exit(-1);
        }
    else if (numX * numY < 2){
        printf("Requested heads must be greater than or equal to 2.\n");
        exit(-1);
        }
    opt.multiHeadSize.left = opt.multiHeadSize.top = 0;
    opt.multiHeadSize.right = xRes * numX;
    opt.multiHeadSize.bottom = yRes * numY;
    memset(&opt.multiHeadRes,0,sizeof(opt.multiHeadRes));
    memset(&opt.multiHeadBounds,0,sizeof(opt.multiHeadBounds));
    for (i = x = 0; x < numX; x++) {
        for (y = 0; y < numY; y++, i++) {
            opt.multiHeadRes[i].left = opt.multiHeadRes[i].top = 0;
            opt.multiHeadRes[i].right = xRes;
            opt.multiHeadRes[i].bottom = yRes;
            opt.multiHeadBounds[i].left = x * xRes;
            opt.multiHeadBounds[i].top = y * yRes;
            opt.multiHeadBounds[i].right = (x+1) * xRes;
            opt.multiHeadBounds[i].bottom = (y+1) * yRes;
            }
        }
    opt.bMultiHead = true;

    /* VESA DPVL mode and multi-head modes are mutually exclusive! */
    opt.bDPVLMode = false;
}

/****************************************************************************
REMARKS:
Function to enable the VESA Digital Panel Video Link mode for very large
displaus. This is a simple function that can enable any number of displays
in a rectangular layout with each display using the same resolution. The
VESA DPVL standard is used to communicate the update regions to the monitor.

NOTE: DPVL mode splits a display across a single graphics card using a very
      large desktop mode but multiplexes the output onto a single DVI
      flat panel connector.
****************************************************************************/
static void EnableDPVLMode(
    int argc,
    char *argv[])
{
    int x,y,i,xRes,yRes,numX,numY;

    if (!(dc->AttributesExt & gaHaveDPVLMode)){
        printf("DPVL mode is not supported on this driver.\n");
        exit(-1);
        }

    if (argc != 7 && argc != 8)
        help();
    xRes = atoi(argv[3]);
    yRes = atoi(argv[4]);
    numX = atoi(argv[5]);
    numY = atoi(argv[6]);
    if (numX * numY > GA_MAX_DPVL_HEADS){
        printf("Requested layout exceeds maximum number of heads.\n");
        exit(-1);
        }
    else if (numX * numY < 2){
        printf("Requested heads must be greater than or equal to 2.\n");
        exit(-1);
        }
    opt.DPVLHeadSize.left = opt.DPVLHeadSize.top = 0;
    opt.DPVLHeadSize.right = xRes * numX;
    opt.DPVLHeadSize.bottom = yRes * numY;
    memset(&opt.DPVLHeadRes,0,sizeof(opt.DPVLHeadRes));
    memset(&opt.DPVLHeadBounds,0,sizeof(opt.DPVLHeadBounds));
    for (i = x = 0; x < numX; x++) {
        for (y = 0; y < numY; y++, i++) {
            opt.DPVLHeadRes[i].left = opt.DPVLHeadRes[i].top = 0;
            opt.DPVLHeadRes[i].right = xRes;
            opt.DPVLHeadRes[i].bottom = yRes;
            opt.DPVLHeadBounds[i].left = x * xRes;
            opt.DPVLHeadBounds[i].top = y * yRes;
            opt.DPVLHeadBounds[i].right = (x+1) * xRes;
            opt.DPVLHeadBounds[i].bottom = (y+1) * yRes;
            }
        }
    opt.bDPVLMode = true;

    /* VESA DPVL mode and multi-head modes are mutually exclusive! */
    opt.bMultiHead = false;
}

/****************************************************************************
REMARKS:
Function to enable the hardware acceleration features of the drivers
depending on a sliding scale.
****************************************************************************/
static void SetHardwareAccel(
    int argc,
    char *argv[])
{
        ibool   enable;

    if (argc < 3 || argc > 5) help();
    if (stricmp(argv[2],"full") == 0) {
        opt.bHardwareCursor         = true;
        opt.bHardwareColorCursor    = true;
        opt.bHardwareVideo          = true;
        opt.bHardwareAccel2D        = true;
        opt.bHardwareAccel3D        = true;
        opt.bMonoPattern            = true;
        opt.bTransMonoPattern       = true;
        opt.bColorPattern           = true;
        opt.bTransColorPattern      = true;
        opt.bSysMem                 = true;
        opt.bLinear                 = true;
        opt.bDrawScanList           = true;
        opt.bDrawEllipseList        = true;
        opt.bDrawFatEllipseList     = true;
        opt.bDrawRect               = true;
        opt.bDrawRectLin            = true;
        opt.bDrawTrap               = true;
        opt.bDrawLine               = true;
        opt.bDrawStippleLine        = true;
        opt.bMonoBlt                = true;
        opt.bBitBlt                 = true;
        opt.bBitBltPatt             = true;
        opt.bBitBltColorPatt        = true;
        opt.bSrcTransBlt            = true;
        opt.bDstTransBlt            = true;
        opt.bStretchBlt             = true;
        opt.bConvertBlt             = true;
        opt.bStretchConvertBlt      = true;
        opt.bBitBltFx               = true;
        opt.bGetBitmap              = true;
        opt.accelType               = gaAccelType_Full;
        }
    else if (stricmp(argv[2],"most") == 0) {
        opt.bHardwareCursor         = false;
        opt.bHardwareColorCursor    = false;
        opt.bHardwareVideo          = true;
        opt.bHardwareAccel2D        = true;
        opt.bHardwareAccel3D        = true;
        opt.bMonoPattern            = true;
        opt.bTransMonoPattern       = true;
        opt.bColorPattern           = true;
        opt.bTransColorPattern      = true;
        opt.bSysMem                 = true;
        opt.bLinear                 = true;
        opt.bDrawScanList           = true;
        opt.bDrawEllipseList        = true;
        opt.bDrawFatEllipseList     = true;
        opt.bDrawRect               = true;
        opt.bDrawRectLin            = true;
        opt.bDrawTrap               = true;
        opt.bDrawLine               = true;
        opt.bDrawStippleLine        = true;
        opt.bMonoBlt                = true;
        opt.bBitBlt                 = true;
        opt.bBitBltPatt             = true;
        opt.bBitBltColorPatt        = true;
        opt.bSrcTransBlt            = true;
        opt.bDstTransBlt            = true;
        opt.bStretchBlt             = true;
        opt.bConvertBlt             = true;
        opt.bStretchConvertBlt      = true;
        opt.bBitBltFx               = true;
        opt.bGetBitmap              = true;
        opt.accelType               = gaAccelType_Most;
        }
    else if (stricmp(argv[2],"basic") == 0) {
        opt.bHardwareCursor         = false;
        opt.bHardwareColorCursor    = false;
        opt.bHardwareVideo          = false;
        opt.bHardwareAccel2D        = true;
        opt.bHardwareAccel3D        = false;
        opt.bMonoPattern            = true;
        opt.bTransMonoPattern       = true;
        opt.bColorPattern           = true;
        opt.bTransColorPattern      = true;
        opt.bSysMem                 = false;
        opt.bLinear                 = false;
        opt.bDrawScanList           = true;
        opt.bDrawEllipseList        = true;
        opt.bDrawFatEllipseList     = true;
        opt.bDrawRect               = true;
        opt.bDrawRectLin            = false;
        opt.bDrawTrap               = true;
        opt.bDrawLine               = false;
        opt.bDrawStippleLine        = false;
        opt.bMonoBlt                = false;
        opt.bBitBlt                 = true;
        opt.bBitBltPatt             = false;
        opt.bBitBltColorPatt        = false;
        opt.bSrcTransBlt            = false;
        opt.bDstTransBlt            = false;
        opt.bStretchBlt             = false;
        opt.bConvertBlt             = false;
        opt.bStretchConvertBlt      = false;
        opt.bBitBltFx               = false;
        opt.bGetBitmap              = false;
        opt.accelType               = gaAccelType_Basic;
        }
    else if (stricmp(argv[2],"none") == 0) {
        opt.bHardwareCursor         = false;
        opt.bHardwareColorCursor    = false;
        opt.bHardwareVideo          = false;
        opt.bHardwareAccel2D        = false;
        opt.bHardwareAccel3D        = false;
        opt.bMonoPattern            = false;
        opt.bTransMonoPattern       = false;
        opt.bColorPattern           = false;
        opt.bTransColorPattern      = false;
        opt.bSysMem                 = false;
        opt.bLinear                 = false;
        opt.bDrawScanList           = false;
        opt.bDrawEllipseList        = false;
        opt.bDrawFatEllipseList     = false;
        opt.bDrawRect               = false;
        opt.bDrawRectLin            = false;
        opt.bDrawTrap               = false;
        opt.bDrawLine               = false;
        opt.bDrawStippleLine        = false;
        opt.bMonoBlt                = false;
        opt.bBitBlt                 = false;
        opt.bBitBltPatt             = false;
        opt.bBitBltColorPatt        = false;
        opt.bSrcTransBlt            = false;
        opt.bDstTransBlt            = false;
        opt.bStretchBlt             = false;
        opt.bConvertBlt             = false;
        opt.bStretchConvertBlt      = false;
        opt.bBitBltFx               = false;
        opt.bGetBitmap              = false;
        opt.accelType               = gaAccelType_None;
        }
    else {
        if (argc < 4) help();
        enable = stricmp(argv[3],"on") == 0;

        if (stricmp(argv[2],"HardwareCursor") == 0)
            opt.bHardwareCursor         = enable;
        else if (stricmp(argv[2],"HardwareColorCursor") == 0)
            opt.bHardwareColorCursor    = enable;
        else if (stricmp(argv[2],"HardwareVideo") == 0)
            opt.bHardwareVideo          = enable;
        else if (stricmp(argv[2],"HardwareAccel2D") == 0)
            opt.bHardwareAccel2D        = enable;
        else if (stricmp(argv[2],"HardwareAccel3D") == 0)
            opt.bHardwareAccel3D        = enable;
        else if (stricmp(argv[2],"MonoPattern") == 0)
            opt.bMonoPattern            = enable;
        else if (stricmp(argv[2],"TransMonoPattern") == 0)
            opt.bTransMonoPattern       = enable;
        else if (stricmp(argv[2],"ColorPattern") == 0)
            opt.bColorPattern           = enable;
        else if (stricmp(argv[2],"TransColorPattern") == 0)
            opt.bTransColorPattern      = enable;
        else if (stricmp(argv[2],"SysMem") == 0)
            opt.bSysMem                 = enable;
        else if (stricmp(argv[2],"Linear") == 0)
            opt.bLinear                 = enable;
        else if (stricmp(argv[2],"DrawScanList") == 0)
            opt.bDrawScanList           = enable;
        else if (stricmp(argv[2],"DrawEllipseList") == 0)
            opt.bDrawEllipseList        = enable;
        else if (stricmp(argv[2],"DrawFatEllipseList") == 0)
            opt.bDrawFatEllipseList     = enable;
        else if (stricmp(argv[2],"DrawRect") == 0)
            opt.bDrawRect               = enable;
        else if (stricmp(argv[2],"DrawRectLin") == 0)
            opt.bDrawRectLin            = enable;
        else if (stricmp(argv[2],"DrawTrap") == 0)
            opt.bDrawTrap               = enable;
        else if (stricmp(argv[2],"DrawLine") == 0)
            opt.bDrawLine               = enable;
        else if (stricmp(argv[2],"DrawStippleLine") == 0)
            opt.bDrawStippleLine        = enable;
        else if (stricmp(argv[2],"MonoBlt") == 0)
            opt.bMonoBlt                = enable;
        else if (stricmp(argv[2],"BitBlt") == 0)
            opt.bBitBlt                 = enable;
        else if (stricmp(argv[2],"BitBltPatt") == 0)
            opt.bBitBltPatt             = enable;
        else if (stricmp(argv[2],"BitBltColorPatt") == 0)
            opt.bBitBltColorPatt        = enable;
        else if (stricmp(argv[2],"SrcTransBlt") == 0)
            opt.bSrcTransBlt            = enable;
        else if (stricmp(argv[2],"DstTransBlt") == 0)
            opt.bDstTransBlt            = enable;
        else if (stricmp(argv[2],"StretchBlt") == 0)
            opt.bStretchBlt             = enable;
        else if (stricmp(argv[2],"ConvertBlt") == 0)
            opt.bConvertBlt             = enable;
        else if (stricmp(argv[2],"StretchConvertBlt") == 0)
            opt.bStretchConvertBlt      = enable;
        else if (stricmp(argv[2],"BitBltFx") == 0)
            opt.bBitBltFx               = enable;
        else if (stricmp(argv[2],"GetBitmap") == 0)
            opt.bGetBitmap              = enable;
        else
            help();
        opt.accelType               = gaAccelType_Custom;
        }
}

/****************************************************************************
REMARKS:
Returns the LCDUseBIOS setting.
****************************************************************************/
static const char *GetLCDMode(void)
{
    switch (gOpt.bLCDUseBIOS) {
        case gaLCDUseBIOS_Off:  return "Off";
        case gaLCDUseBIOS_Auto: return "Auto";
        case gaLCDUseBIOS_On:   return "On";
        }
    return "Unknown!";
}

/****************************************************************************
REMARKS:
Returns the AGP shared memory size
****************************************************************************/
static const char *GetMemoryLimit(void)
{
    static char buf[20];
    if (gOpt.dwVideoMemLimit) {
        sprintf(buf,"%d Mb", gOpt.dwVideoMemLimit / (1024 * 1024));
        return buf;
        }
    return "Off";
}

/****************************************************************************
REMARKS:
Returns the AGP shared memory size
****************************************************************************/
static const char *GetAGPMemSize(void)
{
    static char buf[20];
    sprintf(buf,"%d Kb", gOpt.dwSharedAGPMemSize);
    return buf;
}

/****************************************************************************
REMARKS:
Returns the hardware acceleration type.
****************************************************************************/
static const char *GetSysMemMode(void)
{
    static char buf[20];

    if (gOpt.bUseMemoryDriver) {
        sprintf(buf,"On (%d Kb)", gOpt.wSysMemSize);
        return buf;
        }
    return "Off";
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ShowGlobalSettings(void)
{
    int     i;

    LoadGlobalOptions();
    printf(
        "Global options for all devices:\n\n"
        "  Force VBE Fallback ...... %s\n"
        "  Force VGA Fallback ...... %s\n"
        "  Allow non-certified ..... %s\n"
        "  Disable write combining . %s\n"
        "  Use BIOS for LCD panel... %s\n"
        "  Video Memory Limit....... %s\n"
        "  Shared AGP memory size... %s\n"
        "  Use system memory driver. %s\n"
        "  Disable DDC detection.... %s\n"
        "  Enable AGP FastWrite..... %s\n"
        "  Maximum AGP data rate.... %dX\n",
        gOpt.bVBEOnly           ? "On" : "Off",
        gOpt.bVGAOnly           ? "On" : "Off",
        gOpt.bAllowNonCertified ? "On" : "Off",
        gOpt.bNoWriteCombine    ? "On" : "Off",
        GetLCDMode(),
        GetMemoryLimit(),
        GetAGPMemSize(),
        GetSysMemMode(),
        gOpt.bNoDDCDetect    ? "On" : "Off",
        gOpt.bAGPFastWrite   ? "On" : "Off",
        gOpt.bMaxAGPRate);
    if (gOpt.bVirtualDisplay) {
        printf("  Virtual Display.......... Yes\n");
        for (i = 0; i < GA_MAX_VIRTUAL_DISPLAYS; i++) {
            if (gOpt.bounds[i].right == 0)
                break;
            printf("    %2d: %4d x %4d (%4d,%4d,%4d,%4d)\n",
                i,
                gOpt.resolutions[i].right,
                gOpt.resolutions[i].bottom,
                gOpt.bounds[i].left,
                gOpt.bounds[i].top,
                gOpt.bounds[i].right,
                gOpt.bounds[i].bottom);
            }
        }
    else
        printf("  Virtual Display.......... Off\n");
}

/****************************************************************************
REMARKS:
Returns the hardware acceleration type.
****************************************************************************/
static const char *GetHardwareAccelType(void)
{
    switch (opt.accelType) {
        case gaAccelType_Custom: return "Custom";
        case gaAccelType_Full:   return "Full";
        case gaAccelType_Most:   return "Most";
        case gaAccelType_Basic:  return "Basic";
        case gaAccelType_None:   return "None";
        }
    return "Unknown!";
}

/****************************************************************************
REMARKS:
Show the current acceleration settings for the device.
****************************************************************************/
static void ShowAccelSettings(
    int deviceIndex)
{
    LoadDriver(deviceIndex);
    printf(
        "Acceleration Settings for %s %s (device %d):\n\n"
        "  HardwareCursor        : %s\n"
        "  HardwareColorCursor   : %s\n"
        "  HardwareVideo         : %s\n"
        "  HardwareAccel2D       : %s\n"
        "  HardwareAccel3D       : %s\n"
        "  MonoPattern           : %s\n"
        "  TransMonoPattern      : %s\n"
        "  ColorPattern          : %s\n"
        "  TransColorPattern     : %s\n"
        "  SysMem                : %s\n"
        "  Linear                : %s\n"
        "  DrawScanList          : %s\n"
        "  DrawEllipseList       : %s\n"
        "  DrawFatEllipseList    : %s\n"
        "  DrawRect              : %s\n"
        "  DrawRectLin           : %s\n"
        "  DrawTrap              : %s\n"
        "  DrawLine              : %s\n"
        "  DrawStippleLine       : %s\n"
        "  MonoBlt               : %s\n"
        "  BitBlt                : %s\n"
        "  BitBltPatt            : %s\n"
        "  BitBltColorPatt       : %s\n"
        "  SrcTransBlt           : %s\n"
        "  DstTransBlt           : %s\n"
        "  StretchBlt            : %s\n"
        "  ConvertBlt            : %s\n"
        "  StretchConvertBlt     : %s\n"
        "  BitBltFx              : %s\n"
        "  GetBitmap             : %s\n"
        "  Hardware acceleration : %s\n\n",
        info.ManufacturerName, info.ChipsetName,
        deviceIndex,
        opt.bHardwareCursor         ? "On" : "Off",
        opt.bHardwareColorCursor    ? "On" : "Off",
        opt.bHardwareVideo          ? "On" : "Off",
        opt.bHardwareAccel2D        ? "On" : "Off",
        opt.bHardwareAccel3D        ? "On" : "Off",
        opt.bMonoPattern            ? "On" : "Off",
        opt.bTransMonoPattern       ? "On" : "Off",
        opt.bColorPattern           ? "On" : "Off",
        opt.bTransColorPattern      ? "On" : "Off",
        opt.bSysMem                 ? "On" : "Off",
        opt.bLinear                 ? "On" : "Off",
        opt.bDrawScanList           ? "On" : "Off",
        opt.bDrawEllipseList        ? "On" : "Off",
        opt.bDrawFatEllipseList     ? "On" : "Off",
        opt.bDrawRect               ? "On" : "Off",
        opt.bDrawRectLin            ? "On" : "Off",
        opt.bDrawTrap               ? "On" : "Off",
        opt.bDrawLine               ? "On" : "Off",
        opt.bDrawStippleLine        ? "On" : "Off",
        opt.bMonoBlt                ? "On" : "Off",
        opt.bBitBlt                 ? "On" : "Off",
        opt.bBitBltPatt             ? "On" : "Off",
        opt.bBitBltColorPatt        ? "On" : "Off",
        opt.bSrcTransBlt            ? "On" : "Off",
        opt.bDstTransBlt            ? "On" : "Off",
        opt.bStretchBlt             ? "On" : "Off",
        opt.bConvertBlt             ? "On" : "Off",
        opt.bStretchConvertBlt      ? "On" : "Off",
        opt.bBitBltFx               ? "On" : "Off",
        opt.bGetBitmap              ? "On" : "Off",
        GetHardwareAccelType());
}

/****************************************************************************
REMARKS:
Show the current settings for the device.
****************************************************************************/
static void ShowSettings(
    int deviceIndex)
{
    int i;

    LoadDriver(deviceIndex);
    printf(
        "Options for %s %s (device %d):\n\n"
        "  Invert .................. %s\n"
        "  Rotation ................ %s\n"
        "  Flipped ................. %s\n"
        "  Reduced DVI Timings...... %s\n"
        "  Prefer 16 bit per pixel.. %s\n"
        "  Prefer 32 bit per pixel.. %s\n"
        "  Compressed Framebuffer... %s\n"
        "  Allow DDC BIOS........... %s\n"
        "  PCI bus mastering........ %s\n"
        "  Video memory packets..... %s\n"
        "  Hardware acceleration.... %s\n",
        info.ManufacturerName, info.ChipsetName,
        deviceIndex,
        opt.bInvertColors       ? "On" : "Off",
        opt.bPortrait           ? "On" : "Off",
        opt.bFlipped            ? "On" : "Off",
        opt.bDVIReducedTimings  ? "On" : "Off",
        opt.bPrefer16bpp        ? "On" : "Off",
        opt.bPrefer32bpp        ? "On" : "Off",
        opt.bCompressedFB       ? "On" : "Off",
        opt.bAllowDDCBIOS       ? "On" : "Off",
        opt.bBusMaster          ? "On" : "Off",
        opt.bVidMemPackets      ? "On" : "Off",
        GetHardwareAccelType());
    if (opt.bMultiHead) {
        printf("  Multi Head Display....... Yes\n");
        for (i = 0; i < GA_MAX_HEADS; i++) {
            if (opt.multiHeadBounds[i].right == 0)
                break;
            printf("    %2d: %4d x %4d (%4d,%4d,%4d,%4d)\n",
                i,
                opt.multiHeadRes[i].right,
                opt.multiHeadRes[i].bottom,
                opt.multiHeadBounds[i].left,
                opt.multiHeadBounds[i].top,
                opt.multiHeadBounds[i].right,
                opt.multiHeadBounds[i].bottom);
            }
        }
    else
        printf("  Multi Head Display....... Off\n");
    if (opt.bDPVLMode) {
        printf("  VESA DPVL Mode........... Yes\n");
        for (i = 0; i < GA_MAX_DPVL_HEADS; i++) {
            if (opt.DPVLHeadBounds[i].right == 0)
                break;
            printf("    %2d: %4d x %4d (%4d,%4d,%4d,%4d)\n",
                i,
                opt.DPVLHeadRes[i].right,
                opt.DPVLHeadRes[i].bottom,
                opt.DPVLHeadBounds[i].left,
                opt.DPVLHeadBounds[i].top,
                opt.DPVLHeadBounds[i].right,
                opt.DPVLHeadBounds[i].bottom);
            }
        }
    else
        printf("  VESA DPVL Mode........... Off\n");
    printf("\n");
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
        if (argc == 2) {
            ShowSettings(0);
            ShowGlobalSettings();
            }
        else if (tolower(argv[2][0]) == 'g')
            ShowGlobalSettings();
        else if (tolower(argv[2][0]) == 'a') {
            int device = (argc == 3) ? 0 : atoi(argv[3]);
            ShowAccelSettings(device);
            }
        else {
            ShowSettings(atoi(argv[2]));
            ShowGlobalSettings();
            }
        }
    else if (argc >= 3) {
        int     device;
        ibool   enable = stricmp(argv[2],"on") == 0;

        LoadGlobalOptions();
        if (stricmp(argv[1],"vbe") == 0)
            gOpt.bVBEOnly = enable;
        else if (stricmp(argv[1],"vga") == 0)
            gOpt.bVGAOnly = enable;
        else if (stricmp(argv[1],"noncert") == 0)
            gOpt.bAllowNonCertified = enable;
        else if (stricmp(argv[1],"nowcomb") == 0)
            gOpt.bNoWriteCombine = enable;
        else if (stricmp(argv[1],"noddc") == 0)
            gOpt.bNoDDCDetect = enable;
        else if (stricmp(argv[1],"agpfwrite") == 0)
            gOpt.bAGPFastWrite = enable;
        else if (stricmp(argv[1],"agprate") == 0) {
            gOpt.bMaxAGPRate = atoi(argv[2]);
            if (gOpt.bMaxAGPRate < 1)
                gOpt.bMaxAGPRate = 1;
            else if (gOpt.bMaxAGPRate > 8)
                gOpt.bMaxAGPRate = 8;
            }
        else if (stricmp(argv[1],"virtual") == 0) {
            if (enable)
                EnableVirtualMode(argc,argv);
            else
                gOpt.bVirtualDisplay = false;
            }
        else if (stricmp(argv[1],"lcdusebios") == 0) {
            if (stricmp(argv[2],"auto") == 0)
                gOpt.bLCDUseBIOS = gaLCDUseBIOS_Auto;
            else if (enable)
                gOpt.bLCDUseBIOS = gaLCDUseBIOS_On;
            else
                gOpt.bLCDUseBIOS = gaLCDUseBIOS_Off;
            }
        else if (stricmp(argv[1],"agpmem") == 0) {
            gOpt.dwSharedAGPMemSize = atoi(argv[2]);
            if (gOpt.dwSharedAGPMemSize < 512 || gOpt.dwSharedAGPMemSize > 65536)
                gOpt.dwSharedAGPMemSize = 0;
            }
        else if (stricmp(argv[1],"vidmem") == 0) {
            if (stricmp(argv[2],"off") == 0)
                gOpt.dwVideoMemLimit = 0;
            else
                gOpt.dwVideoMemLimit = atoi(argv[2]) * 1024 * 1024;
            }
        else if (stricmp(argv[1],"usesysmem") == 0) {
            gOpt.bUseMemoryDriver = (stricmp(argv[2],"off") != 0);
            gOpt.wSysMemSize = atoi(argv[2]);
            }
        else {
            if (stricmp(argv[1],"accel") == 0) {
                /* Device parameter is always last, and if it is not present
                 * then the device should be zero which is what atoi()
                 * returns for non-number values.
                 */
                device = atoi(argv[argc-1]);
                }
            else if ((stricmp(argv[1],"multihead") == 0) || (stricmp(argv[1],"dpvl") == 0)) {
                if (argc == 8)
                    device = atoi(argv[7]);
                else
                    device = 0;
                }
            else
                device = (argc == 3) ? 0 : atoi(argv[3]);
            LoadDriver(device);
            if (stricmp(argv[1],"invert") == 0)
                opt.bInvertColors = enable;
            else if (stricmp(argv[1],"rotation") == 0)
                opt.bPortrait = enable;
            else if (stricmp(argv[1],"flipped") == 0)
                opt.bFlipped = enable;
            else if (stricmp(argv[1],"dvireduce") == 0)
                opt.bDVIReducedTimings = enable;
            else if (stricmp(argv[1],"prefer16") == 0)
                opt.bPrefer16bpp = enable;
            else if (stricmp(argv[1],"prefer32") == 0)
                opt.bPrefer32bpp = enable;
            else if (stricmp(argv[1],"compress") == 0)
                opt.bCompressedFB = enable;
            else if (stricmp(argv[1],"ddcbios") == 0)
                opt.bAllowDDCBIOS = enable;
            else if (stricmp(argv[1],"busmaster") == 0)
                opt.bBusMaster = enable;
            else if (stricmp(argv[1],"vidmempackets") == 0)
                opt.bVidMemPackets = enable;
            else if (stricmp(argv[1],"accel") == 0)
                SetHardwareAccel(argc,argv);
            else if (stricmp(argv[1],"multihead") == 0) {
                if (enable)
                    EnableMultiHeadMode(argc,argv);
                else
                    opt.bMultiHead = false;
                }
            else if (stricmp(argv[1],"dpvl") == 0) {
                if (enable)
                    EnableDPVLMode(argc,argv);
                else
                    opt.bDPVLMode = false;
                }
            init.SetOptions(&opt);
            GA_saveOptions(dc,&opt);
            ShowSettings(device);
            }
        GA_setGlobalOptions(&gOpt);
        GA_saveGlobalOptions(&gOpt);
        ShowGlobalSettings();
        }
    else
        help();
    Unload();
    return 0;
}

