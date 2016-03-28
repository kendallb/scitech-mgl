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
* Description:  Simple program to profile the speed of certain operations
*               for the SciTech SNAP Graphics Architecture. This is
*               a great way to test the performance of different SuperVGA
*               cards and different compiler configurations.
*
*               Note, this library uses the Zen Timer Library for
*               microsecond accuracy timing of the routines.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cmdline.h"
#include "profile.h"
#include "gconsole.h"
#include "ztimer.h"

/*--------------------------- Global variables ----------------------------*/

static ibool    thrashCache = true;
static int      numDevices = 0;
static PM_HWND  hwndConsole;
static void     *stateBuf;
static char     testName[80] = "All";
GA_initFuncs    init;
GA_driverFuncs  driver;
GC_devCtx       *primaryGC = NULL;

#define NO_BUILD
#include "snap/graphics/snapver.c"

const char *gaGetReleaseDate(void)
{ return release_date; };

const char *gaGetReleaseDate2(void)
{ return release_date2; };

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    if (numDevices > 0)
        GA_setActiveDevice(0);
    if (primaryGC)
        GC_destroy(primaryGC);
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

/****************************************************************************
REMARKS:
Display the program banner
****************************************************************************/
void banner(void)
{
    printf("GAPerf - %s Performance Profiler\n", GRAPHICS_PRODUCT);
    printf("         Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    printf("%s\n",copyright_str);
    printf("\n");
}

/****************************************************************************
REMARKS:
Display the command line help.
****************************************************************************/
void help(void)
{
    GA_unloadDriver(dc);
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    banner();
    printf("Options are:\n");
    printf("    -l       - List available graphics modes\n");
    printf("    -t<name> - Run test 'name' (-tlist to display list of all tests)\n");
    printf("    -c       - Thrash the system memory cache during BitBlt's\n");
    printf("    -s       - Force software only rendering (no acceleration)\n");
    printf("\n");
    printf("Usage: profile [-lt] <xres> <yres> <bits>\n");
    exit(1);
}

/****************************************************************************
REMARKS:
Display a list of available resolutions
****************************************************************************/
void AvailableModes(void)
{
    N_uint16    *modes;

    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    printf("Available graphics modes:\n");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (modeInfo.BitsPerPixel < 4)
            continue;
        printf(" %4d x %4d %d bits per pixel\n",
            modeInfo.XResolution, modeInfo.YResolution, modeInfo.BitsPerPixel);
        }
    GA_unloadDriver(dc);
    exit(1);
}

/****************************************************************************
REMARKS:
Parse the command line arguments.
****************************************************************************/
void parseArguments(
    int argc,
    char *argv[])
{
    int     option;
    char    *argument;

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"lcst:",&argument);
        switch (option) {
            case 'l':
                AvailableModes();
                break;
            case 't':
                if (stricmp(argument,"list") == 0) {
                        GA_unloadDriver(dc);
                        PM_restoreConsoleState(stateBuf,hwndConsole);
                        PM_closeConsole(hwndConsole);
                    banner();
                    printf("Available tests:\n");
                    printf("  - BaseLine\n");
                    printf("  - Lines\n");
                    printf("  - BresenhamLines\n");
                    printf("  - StippleLines\n");
                    printf("  - BresenhamStippleLines\n");
                    printf("  - CPUSysBlt\n");
                    printf("  - CPUClears\n");
                    printf("  - BitBltSys\n");
                    printf("  - Clears\n");
                    printf("  - MonoPatt\n");
                    printf("  - TransMonoPatt\n");
                    printf("  - ColorPatt\n");
                    printf("  - TransColorPatt\n");
                    printf("  - BitBlt\n");
                    printf("  - PutMonoImage\n");
                    exit(-1);
                    }
                strcpy(testName,argument);
                break;
            case 'c':
                thrashCache = false;
                break;
            case 's':
                softwareOnly = true;
                break;
            case ALLDONE:
                break;
            case PARAMETER:
                break;
            case 'h':
            case INVALID:
            default:
                help();
            }
        } while (option != ALLDONE && option != PARAMETER);
}

/****************************************************************************
REMARKS:
Main program entry point.
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    ProfileRecord   R;
    int             x,y,bits,success;
    GC_devCtx       *gc;

    /* Check to see if we are running in a window */
    if (PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        return -1;
        }

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Load the device driver for this device */
    if ((numDevices = GA_enumerateDevices(false)) == 0)
        PM_fatalError(GA_errorMsg(GA_status()));
    if ((dc = GA_loadDriver(0,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

    /* Display available modes for invalid command line */
    memset(&R,0,sizeof(R));
    parseArguments(argc,argv);
    R.thrashCache = thrashCache;
    argc -= (nextargv-1);
    if (argc != 4)
        help();
    x = atoi(argv[nextargv]);
    y = atoi(argv[nextargv+1]);
    bits = atoi(argv[nextargv+2]);

    /* Create the graphics console */
    if ((gc = GC_create(dc,80,50,GC_FONT_8X8)) == NULL)
        if ((gc = GC_create(dc,80,25,GC_FONT_8X8)) == NULL) {
            PM_fatalError("Unable to find 80x25 text mode!");
            }
    primaryGC = gc;

    ZTimerInit();

    /* Now do the performance profiling */
    success = ProfileIt(x,y,bits,testName,&R);

    /* Unload the device driver */
        GC_destroy(gc);
    GA_unloadDriver(dc);

    /* Restore the console */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    if (success) {
        printf("Profiling results for mode %dx%d %d color.\n",
            maxX+1,maxY+1,maxcolor+1);
        printf("\n");
        printf("Hardware accelerated functions:\n\n");
        printf(" Integer lines (solid)              => %10.2f lines/s\n",   R.intLinesPerSec);
        printf(" Bresenham lines (solid)            => %10.2f lines/s\n",   R.bresLinesPerSec);
        printf(" Integer lines (patterned)          => %10.2f lines/s\n",   R.intStippleLinesPerSec);
        printf(" Bresenham lines (patterned)        => %10.2f lines/s\n",   R.bresStippleLinesPerSec);
        printf(" Clears (solid pattern)             => %10.2f MB/s\n",      R.clearsMbPerSec);
        printf(" Clears (mono pattern)              => %10.2f MB/s\n",      R.monoPattMbPerSec);
        printf(" Clears (transparent mono pattern)  => %10.2f MB/s\n",      R.transMonoPattMbPerSec);
        printf(" Clears (color pattern)             => %10.2f MB/s\n",      R.colorPattMbPerSec);
        printf(" Clears (transparent color pattern) => %10.2f MB/s\n",      R.transColorPattMbPerSec);
        printf(" BitBlt screen->screen (scrolls)    => %10.2f MB/s\n",      R.bitBltMbPerSec);
        printf(" BitBlt memory->screen              => %10.2f MB/s\n",      R.bitBltSysMbPerSec);
        printf(" Mono image memory->screen (text)   => %10.2f MB/s\n",      R.putMonoImageMbPerSec);
        printf("\n");
        printf("CPU Direct to Video Memory functions:\n\n");
        printf(" CPU copies to VRAM                 => %10.2f MB/s\n",      R.putImagesCPUMbPerSec);
        printf(" CPU copies from VRAM               => %10.2f MB/s\n",      R.getImagesCPUMbPerSec);
        printf(" CPU reads from VRAM                => %10.2f MB/s\n",      R.readsCPUMbPerSec);
        printf(" CPU clears                         => %10.2f MB/s\n",      R.clearCPUMbPerSec);
        printf(" CPU reverse clears                 => %10.2f MB/s\n",      R.reverseClearCPUMbPerSec);
        printf("\n");
        printf("Baseline values for system memory:\n\n");
        printf(" clears (REP STOSD)     => %10.2f MB/s\n",      R.clearBaseLineMbPerSec);
        printf(" reverse clears         => %10.2f MB/s\n",      R.reverseClearBaseLineMbPerSec);
        printf(" reads  (MOV EAX,[EDI]) => %10.2f MB/s\n",      R.readBaseLineMbPerSec);
        printf(" copies (REP MOVSD)     => %10.2f MB/s\n",      R.copyBaseLineMbPerSec);
        printf("\n");
        printf("Overall GA-Mark: %.2f\n", ComputeGAMark(&R));
        }
    else {
        printf("Unable to set desired graphics mode\n");
        }
    return 0;
}
