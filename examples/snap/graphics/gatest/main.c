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
* Description:  Command line driven, text based front end for the SciTech
*               SNAP Graphics test program.
*
****************************************************************************/

#include "gatest.h"
#include "cmdline.h"
#include "clib/os/os.h"

/*--------------------------- Global variables ----------------------------*/

static int      textModeX = 80;
static int      textModeY = 50;
int             numDevices = 0,cntDevice = -1;
ibool           fullSpeed = false;
ibool           breakPoint = false;
ibool           softwareEmulate = false;
ibool           useGTF = false;
ibool           doSmallTest = true;
ibool           doubleBuffer = 2;
ibool           noDither = false;
int             xResMax,yResMax;
ulong           planeMask = 0xFFFFFFFF;
ibool           locked = false;
GA_devCtx       *primaryDC = NULL;
GC_devCtx       *primaryGC = NULL;
GA_modeInfo     miMax;
GA_CRTCInfo     crtcMax;
GA_initFuncs    init;
GA_driverFuncs  driver;
GA_glInitFuncs  glFuncs;
GA_glFuncs      _VARAPI _GA_glFuncs;
GA_gluFuncs     _VARAPI _GA_gluFuncs;
char            accelTestName[PM_MAX_PATH] = "all";
char            exePathName[PM_MAX_PATH];
static PM_HWND  hwndConsole;
static void     *stateBuf;
int             multiModeX = 0;
int             multiModeY = 0;
int             fontSize = GC_FONT_8X8;
ibool           isPortrait = false;
ibool           isFlipped = false;
ibool           isInverted = false;
ibool           isFiltered = false;

#define NO_BUILD
#include "snap/graphics/snapver.c"

const char *gaGetReleaseDate(void)
{ return release_date; };

const char *gaGetReleaseDate2(void)
{ return release_date2; };

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

#define MAX_MENU    40

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Displays the copyright signon banner on the screen
****************************************************************************/
static void banner(
    GC_devCtx *gc)
{
#ifdef  LITE
    GC_printf(gc,"GACtrl - %s Driver Control Center\n", GRAPHICS_PRODUCT);
#else
    GC_printf(gc,"GATest - %s Driver Compliance Test\n", GRAPHICS_PRODUCT);
#endif
    GC_printf(gc,"         Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    GC_printf(gc,"%s\n",copyright_str);
    GC_printf(gc,"\n");
}

/****************************************************************************
REMARKS:
Handles going into/out of background
****************************************************************************/
int PMAPI SuspendCallbackFunc(
        int flags)
{
    if (flags == PM_DEACTIVATE) {
        ExitSoftwareRasterizer();
        return PM_SUSPEND_APP;
        }

    /* We're coming back to foreground and we need to restore some
     * reasonable defaults - unfortunately this is not a GUI app
     * and it doesn't know how to repaint itself. Not terribly good but
     * works.
     */
    EVT_post(0, EVT_KEYDOWN, 0x1B, 0);  /* Fake an Esc keypress        */
    EVT_post(0, EVT_KEYUP, 0x1B, 0);
    return PM_SUSPEND_APP;
}

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
    if (primaryDC)
        GA_unloadDriver(primaryDC);
    if (locked)
        PM_unlockSNAPAccess(0);
    EVT_exit();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

/****************************************************************************
REMARKS:
Handles the choice of display mode common to all display modes.
****************************************************************************/
ibool doChoice(
    GC_devCtx *gc,
    GA_glCtx *gl,
    int *menu,
    int maxmenu,
    int bitsPerPixel,
    ulong planeMask,
    doTestType doTest,
    ibool useLSBMode)
{
    int         xRes,yRes,mode,choice,maxChoice,refreshRate = 0;
    GA_modeInfo mi;
    char        buf[40];
    float       maxRefresh,refresh = 0;
    ibool       interlaced;
    GA_CRTCInfo crtc;

    GC_printf(gc,"  [Esc] - Quit\n\n");
    GC_printf(gc,"Choice: ");
    choice = EVT_getch();
    choice = tolower(choice);
    if (choice == 0x1B)
        return true;
    if (choice >= 'a')
        choice = choice - 'a' + 10;
    else
        choice -= '0';
    if (0 <= choice && choice < maxmenu) {
        mode = menu[choice];
        if (mode == -1) {
            xRes = xResMax;
            yRes = yResMax;
            mi = miMax;
            crtc = crtcMax;
            mode = 0;
            }
        else if (mode == -2) {
            /* Select a custom display mode and refresh rate */
            GC_clrscr(gc);
            banner(gc);
            GC_printf(gc,"Enter Custom X Resolution: ");
            if (GetString(gc,buf,sizeof(buf)) == -1)
                return false;
            xRes = atoi(buf);
            GC_printf(gc,"Enter Custom Y Resolution: ");
            if (GetString(gc,buf,sizeof(buf)) == -1)
                return false;
            yRes = atoi(buf);
            GC_printf(gc,"Enter Custom Refresh Rate: ");
            if (GetString(gc,buf,sizeof(buf)) == -1)
                return false;
            refresh = atof(buf);
            interlaced = false;
            if (refresh < 0) {
                refresh = -refresh;
                interlaced = true;
                }
            mi.dwSize = sizeof(mi);
            if (!yRes || init.GetCustomVideoModeInfo(xRes,yRes,-1,-1,bitsPerPixel,&mi) != 0) {
                GC_printf(gc,"\n");
                GC_printf(gc,"ERROR: Hardware device not capable of requested mode!\n\n");
                GC_printf(gc,"\nPress any key to continue...\n");
                EVT_getch();
                return false;
                }
            if (!GA_computeCRTCTimings(gc->dc,init.GetActiveHead(),&mi,refresh,interlaced,&crtc,false)) {
                GC_printf(gc,"\n");
                GC_printf(gc,"ERROR: Could not compute CRTC timings!\n\n");
                GC_printf(gc,"\nPress any key to continue...\n");
                EVT_getch();
                return false;
                }
            mode = 0;
            }
        else {
            /* Select a pre-defined display mode and refresh rate */
            xRes = yRes = -1;
            mi.dwSize = sizeof(mi);
            init.GetVideoModeInfo(mode,&mi);
            crtc.RefreshRate = 0;
            GC_clrscr(gc);
            banner(gc);
            if (useGTF) {
                GA_getMaxRefreshRate(gc->dc,init.GetActiveHead(),&mi,false,&maxRefresh);
                GC_printf(gc,"Choose refresh rate for %d x %d %d bits per pixel (max %3.2fHz):\n\n",
                    mi.XResolution, mi.YResolution, mi.BitsPerPixel, maxRefresh);
                GC_printf(gc,"  [0] - User Default\n");
                if (maxRefresh >= 60)   GC_printf(gc,"  [1] - 60 Hz\n");
                if (maxRefresh >= 65)   GC_printf(gc,"  [2] - 65 Hz\n");
                if (maxRefresh >= 70)   GC_printf(gc,"  [3] - 70 Hz\n");
                if (maxRefresh >= 75)   GC_printf(gc,"  [4] - 75 Hz\n");
                if (maxRefresh >= 80)   GC_printf(gc,"  [5] - 80 Hz\n");
                if (maxRefresh >= 85)   GC_printf(gc,"  [6] - 85 Hz\n");
                if (maxRefresh >= 90)   GC_printf(gc,"  [7] - 90 Hz\n");
                if (maxRefresh >= 95)   GC_printf(gc,"  [8] - 95 Hz\n");
                if (maxRefresh >= 100)  GC_printf(gc,"  [9] - 100 Hz\n");
                if (maxRefresh >= 105)  GC_printf(gc,"  [A] - 105 Hz\n");
                if (maxRefresh >= 110)  GC_printf(gc,"  [B] - 110 Hz\n");
                if (maxRefresh >= 115)  GC_printf(gc,"  [C] - 115 Hz\n");
                if (maxRefresh >= 120)  GC_printf(gc,"  [D] - 120 Hz\n");
                GC_printf(gc,"  [E] - Custom\n");
                GC_printf(gc,"\nChoice: ");
                choice = EVT_getch();
                if (tolower(choice) == 'q' || choice == 0x1B)
                    return false;
                interlaced = false;
                switch (tolower(choice)) {
                    case '1':   refresh = 60;   break;
                    case '2':   refresh = 65;   break;
                    case '3':   refresh = 70;   break;
                    case '4':   refresh = 75;   break;
                    case '5':   refresh = 80;   break;
                    case '6':   refresh = 85;   break;
                    case '7':   refresh = 90;   break;
                    case '8':   refresh = 95;   break;
                    case '9':   refresh = 100;  break;
                    case 'a':   refresh = 105;  break;
                    case 'b':   refresh = 110;  break;
                    case 'c':   refresh = 115;  break;
                    case 'd':   refresh = 120;  break;
                    case 'e':
                        GC_printf(gc,"\n\nEnter Refresh Rate (- for interlaced): ");
                        if (GetString(gc,buf,sizeof(buf)) == -1)
                            return false;
                        refresh = atof(buf);
                        if (refresh < 0) {
                            refresh = -refresh;
                            interlaced = true;
                            }
                        if (refresh < 40 || refresh > 200)
                            refresh = 0;
                        break;
                    }
                if (refresh != 0) {
                    if (!GA_computeCRTCTimings(gc->dc,init.GetActiveHead(),&mi,refresh,interlaced,&crtc,false)) {
                        GC_printf(gc,"\n");
                        GC_printf(gc,"ERROR: Could not compute CRTC timings!\n\n");
                        GC_printf(gc,"\nPress any key to continue...\n");
                        EVT_getch();
                        return false;
                        }
                    mode |= gaRefreshCtrl;
                    }
                }
            else {
                GC_printf(gc,"Choose refresh rate for %d x %d %d bits per pixel:\n\n",
                    mi.XResolution, mi.YResolution, mi.BitsPerPixel);
                if (mi.DefaultRefreshRate < 0)
                    GC_printf(gc,"  [0] - User Default (%d Hz Interlaced)\n", -mi.DefaultRefreshRate);
                else
                    GC_printf(gc,"  [0] - User Default (%d Hz)\n", mi.DefaultRefreshRate);
                for (maxChoice = 0; mi.RefreshRateList[maxChoice] != -1; maxChoice++) {
                    choice = maxChoice+1;
                    if (choice < 10)
                        choice = '0' + choice;
                    else
                        choice = 'A' + choice - 10;
                    if (mi.RefreshRateList[maxChoice] < 0)
                        GC_printf(gc,"  [%c] - %ld Hz Interlaced%s\n", choice,-mi.RefreshRateList[maxChoice],
                            mi.RefreshRateList[maxChoice] == mi.DefaultRefreshRate ? " *" : "");
                    else
                        GC_printf(gc,"  [%c] - %ld Hz%s\n", choice,mi.RefreshRateList[maxChoice],
                            mi.RefreshRateList[maxChoice] == mi.DefaultRefreshRate ? " *" : "");
                    }
                GC_printf(gc,"\nChoice: ");
                choice = -1;
                while (choice < 0 || choice > maxChoice) {
                    choice = EVT_getch();
                    if (tolower(choice) == 'q' || choice == 0x1B)
                        return false;
                    if (choice == 0x0D)
                        choice = 0;
                    else if (choice >= 'a')
                        choice = choice - 'a' + 10;
                    else
                        choice -= '0';
                    }
                if (choice == 0)
                    refreshRate = 0;
                else
                    refreshRate = mi.RefreshRateList[choice-1];
                }
            }
        if (!(mi.Attributes & gaIsTextMode) && (mi.Attributes & gaHaveLinearBuffer))
            mode |= gaLinearBuffer;
        if (!doTest(gc,gl,xRes,yRes,bitsPerPixel,mode,refreshRate,&crtc,planeMask,bitsPerPixel == 1 ? useLSBMode ? 2 : 1 : 0)) {
            GC_printf(gc,"\n");
            GC_printf(gc,"ERROR: Video mode did not set correctly!\n\n");
            GC_printf(gc,"\nPress any key to continue...\n");
            EVT_getch();
            }
        }
    return false;
}

/****************************************************************************
REMARKS:
Function to add a mode to the menu list and generate the name for the mode.
****************************************************************************/
static int addMode(
    GC_devCtx *gc,
    int *menu,
    int maxmenu,
    GA_modeInfo *mi,
    int mode)
{
    int     num;
    char    buf[80];

    /* Get name for mode and mode number for initialising it */
    if (maxmenu < 10)
        num = '0' + maxmenu;
    else
        num = 'A' + maxmenu - 10;
    if (GetModeName(buf,mi) == 0)
        return maxmenu;
    GC_printf(gc,"  [%c] - %s\n",num,buf);
    menu[maxmenu++] = mode;
    return maxmenu;
}

#ifdef PRO
/****************************************************************************
REMARKS:
Round a value to the specified integer boundary
****************************************************************************/
static ulong RoundDown(
    ulong value,
    ulong boundary)
{
    return (value / boundary) * boundary;
}

/****************************************************************************
REMARKS:
Function to add a mode to the menu list and generate the name for the mode.
****************************************************************************/
static int addMaxMode(
    GC_devCtx *gc,
    int bitsPerPixel,
    int *menu,
    int maxmenu)
{
    int     num;
    char    buf[80];

    /* Find the maximum resolution at this color depth */
    miMax.dwSize = sizeof(miMax);
    if (init.GetCustomVideoModeInfo(640,480,-1,-1,bitsPerPixel,&miMax) != 0)
        return maxmenu;
    for (xResMax = RoundDown(miMax.MaxScanLineWidth,8); xResMax > 640; xResMax -= 8) {
        yResMax = (xResMax * 3) / 4;
        if (init.GetCustomVideoModeInfo(xResMax,yResMax,-1,-1,bitsPerPixel,&miMax) != 0)
            continue;
        if (!GA_computeCRTCTimings(gc->dc,init.GetActiveHead(),&miMax,60,false,&crtcMax,false))
            continue;
        break;
        }
    if (xResMax == 640)
        return maxmenu;

    /* Get name for mode and mode number for initialising it */
    if (maxmenu < 10)
        num = '0' + maxmenu;
    else
        num = 'A' + maxmenu - 10;
    if (GetModeName(buf,&miMax) == 0)
        return maxmenu;
    GC_printf(gc,"  [%c] - %s (Max @ 60Hz)\n",num,buf);
    menu[maxmenu++] = -1;
    return maxmenu;
}
#endif

/****************************************************************************
REMARKS:
Function to add a mode to the menu list and generate the name for the mode.
****************************************************************************/
static int addCustomMode(
    GC_devCtx *gc,
    int *menu,
    int maxmenu)
{
    int num;

    /* Get name for mode and mode number for initialising it */
    if (maxmenu == 0)
        return 0;
    if (maxmenu < 10)
        num = '0' + maxmenu;
    else
        num = 'A' + maxmenu - 10;
    GC_printf(gc,"  [%c] - Custom\n",num);
    menu[maxmenu++] = -2;
    return maxmenu;
}

/****************************************************************************
REMARKS:
Main menu for testing all 16 color modes
****************************************************************************/
static void testText(
    GC_devCtx *gc,
    doTestType doTest)
{
    int         maxmenu,menu[MAX_MENU];
    N_uint16    *modes;
    GA_modeInfo mi;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Which text mode to test:\n\n");
        maxmenu = 0;
        for (modes = gc->dc->AvailableModes; *modes != 0xFFFF; modes++) {
            mi.dwSize = sizeof(mi);
            if (init.GetVideoModeInfo(*modes,&mi) != 0)
                continue;
            if (!(mi.Attributes & gaIsTextMode))
                continue;
            if (mi.Attributes & gaIsGUIDesktop)
                continue;
            maxmenu = addMode(gc,menu,maxmenu,&mi,*modes);
            }
        maxmenu = addCustomMode(gc,menu,maxmenu);
        if (doChoice(gc,NULL,menu,maxmenu,0,0xFFFFFFFF,doTest,false))
            break;
        }
}

#ifdef PRO
/****************************************************************************
REMARKS:
Main menu for testing all 16 color modes
****************************************************************************/
static void test2(
    GC_devCtx *gc,
    doTestType doTest,
    ibool useLSBMode)
{
    int         maxmenu,menu[MAX_MENU];
    int         activeHead = init.GetActiveHead();
    N_uint16    *modes;
    GA_modeInfo mi;
    int         oldSoftwareEmulate = softwareEmulate;
    int         oldSmallTest = doSmallTest;

    softwareEmulate = true;
    doSmallTest = false;
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Which 1 bit shadow buffer mode to test:\n\n");
        maxmenu = 0;
        if (activeHead == gaActiveHeadClone || activeHead == gaActiveHeadPrimary) {
            for (modes = gc->dc->AvailableModes; *modes != 0xFFFF; modes++) {
                mi.dwSize = sizeof(mi);
                if (init.GetVideoModeInfo(*modes,&mi) != 0)
                    continue;
                if (mi.BitsPerPixel != 8)
                    continue;
                if (mi.Attributes & gaIsGUIDesktop)
                    continue;
                mi.BitsPerPixel = 1;
                maxmenu = addMode(gc,menu,maxmenu,&mi,*modes);
                }
            }
        if (doChoice(gc,NULL,menu,maxmenu,1,0xFFFFFFFF,doTest,useLSBMode))
            break;
        }
    softwareEmulate = oldSoftwareEmulate;
    doSmallTest = oldSmallTest;
}
#endif

/****************************************************************************
REMARKS:
Main menu for testing all 16 color modes
****************************************************************************/
static void test16(
    GC_devCtx *gc,
    doTestType doTest)
{
    int         maxmenu,menu[MAX_MENU];
    N_uint16    *modes;
    GA_modeInfo mi;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Which 4 bit video mode to test:\n\n");
        maxmenu = 0;
        for (modes = gc->dc->AvailableModes; *modes != 0xFFFF; modes++) {
            mi.dwSize = sizeof(mi);
            if (init.GetVideoModeInfo(*modes,&mi) != 0)
                continue;
            if (mi.BitsPerPixel != 4)
                continue;
            if (mi.Attributes & gaIsGUIDesktop)
                continue;
            maxmenu = addMode(gc,menu,maxmenu,&mi,*modes);
            }
#ifdef PRO
        maxmenu = addMaxMode(gc,4,menu,maxmenu);
#endif
        maxmenu = addCustomMode(gc,menu,maxmenu);
        if (doChoice(gc,NULL,menu,maxmenu,4,0xFFFFFFFF,doTest,false))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all 256 color modes
****************************************************************************/
static void test256(
    GC_devCtx *gc,
    GA_glCtx *gl,
    doTestType doTest)
{
    int         maxmenu,menu[MAX_MENU];
    N_uint16    *modes;
    GA_modeInfo mi;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Which 8 bit video mode to test:\n\n");
        maxmenu = 0;
        for (modes = gc->dc->AvailableModes; *modes != 0xFFFF; modes++) {
            mi.dwSize = sizeof(mi);
            if (init.GetVideoModeInfo(*modes,&mi) != 0)
                continue;
            if (mi.BitsPerPixel != 8)
                continue;
            if (mi.Attributes & gaIsGUIDesktop)
                continue;
            maxmenu = addMode(gc,menu,maxmenu,&mi,*modes);
            }
#ifdef PRO
        maxmenu = addMaxMode(gc,8,menu,maxmenu);
#endif
        maxmenu = addCustomMode(gc,menu,maxmenu);
        if (doChoice(gc,gl,menu,maxmenu,8,planeMask,doTest,false))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all direct color modes
****************************************************************************/
static void testDirectColor(
    GC_devCtx *gc,
    GA_glCtx *gl,
    long colors,
    doTestType doTest)
{
    int         maxmenu,numbits,menu[MAX_MENU];
    N_uint16    *modes;
    GA_modeInfo mi;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        if (colors == 0x7FFFL)
            numbits = 15;
        else if (colors == 0xFFFFL)
            numbits = 16;
        else if (colors == 0xFFFFFFL)
            numbits = 24;
        else
            numbits = 32;
        GC_printf(gc,"Which %d bit video mode to test:\n\n", numbits);
        maxmenu = 0;
        for (modes = gc->dc->AvailableModes; *modes != 0xFFFF; modes++) {
            mi.dwSize = sizeof(mi);
            if (init.GetVideoModeInfo(*modes,&mi) != 0)
                continue;
            if (mi.Attributes & gaIsGUIDesktop)
                continue;
            /* filter out modes that Mesa can't handle - TODO: handle them */
            if (doTest == doOpenGLTest && (mi.XResolution > 4096 || mi.YResolution > 4096))
                continue;
            if (mi.BitsPerPixel == numbits) {
                maxmenu = addMode(gc,menu,maxmenu,&mi,*modes);
                }
            }
#ifdef PRO
        maxmenu = addMaxMode(gc,numbits,menu,maxmenu);
#endif
        maxmenu = addCustomMode(gc,menu,maxmenu);
        if (doChoice(gc,gl,menu,maxmenu,numbits,planeMask,doTest,false))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void InteractiveTests(
    GC_devCtx *gc)
{
    int choice;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        GC_printf(gc,". 2D Acceleration:        %-3s     ", gc->dc->Attributes & gaHaveAccel2D ? "Yes" : "No");
        GC_printf(gc,". Hardware Cursor:        %-3s\n", gc->dc->Attributes & gaHaveHWCursor ? "Yes" : "No");
        GC_printf(gc,". Multi Buffering:        %-3s     ", maxPage > 0 ? "Yes" : "No");
        GC_printf(gc,". Virtual Scrolling:      %-3s\n", gc->dc->Attributes & gaHaveDisplayStart ? "Yes" : "No");
        GC_printf(gc,". HW Triple Buffering:    %-3s     ", gc->dc->Attributes & gaHaveTripleBuffer ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Flipping:     %-3s\n", gc->dc->Attributes & gaHaveStereo ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Sync:         %-3s     ", gc->dc->Attributes & gaHaveHWStereoSync ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Sync (EVC):   %-3s\n", gc->dc->Attributes & gaHaveEVCStereoSync ? "Yes" : "No");
        GC_printf(gc,". 8 bit wide DAC support: %-3s     ", gc->dc->Attributes & gaHave8BitDAC ? "Yes" : "No");
        GC_printf(gc,". NonVGA Controller:      %-3s\n", gc->dc->Attributes & gaHaveNonVGAMode ? "Yes" : "No");
        GC_printf(gc,". Linear framebuffer:     ");
        if (gc->dc->LinearBasePtr) {
            GC_printf(gc,"%ldMb\n", (ulong)gc->dc->LinearBasePtr >> 20);
            }
        else
            GC_printf(gc,"No\n");
        GC_printf(gc,"\n");
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 4 bits per pixel modes      [3] - 16 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 8 bits per pixel modes      [4] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 15 bits per pixel modes     [5] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [6] - Text modes\n");
#ifdef PRO
        GC_printf(gc,"  [7] - 1 bit MSB tests             [8] - 1 bit LSB tests\n");
#endif
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test16(gc,doTest);                              break;
            case '1':   test256(gc,NULL,doTest);                        break;
            case '2':   testDirectColor(gc,NULL,0x7FFFL,doTest);        break;
            case '3':   testDirectColor(gc,NULL,0xFFFFL,doTest);        break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFL,doTest);      break;
            case '5':   testDirectColor(gc,NULL,0xFFFFFFFFL,doTest);    break;
            case '6':   testText(gc,doTextTest);                        break;
#ifdef PRO
            case '7':   test2(gc,doTest,false);                         break;
            case '8':   test2(gc,doTest,true);                          break;
#endif
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void CenteringAndRefresh(
    GC_devCtx *gc)
{
    int choice;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (init.GetDisplayOutput() & (gaOUTPUT_LCD | gaOUTPUT_DFP | gaOUTPUT_TV)) {
            GC_printf(gc,"Centering and refresh only available on CRT display...");
            EVT_getch();
            return;
            }
        GC_printf(gc,"Select color mode to center:\n\n");
        GC_printf(gc,"  [0] - 4 bits per pixel modes      [3] - 16 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 8 bits per pixel modes      [4] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 15 bits per pixel modes     [5] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [6] - Text modes\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test16(gc,doCenter);                            break;
            case '1':   test256(gc,NULL,doCenter);                      break;
            case '2':   testDirectColor(gc,NULL,0x7FFFL,doCenter);      break;
            case '3':   testDirectColor(gc,NULL,0xFFFFL,doCenter);      break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFL,doCenter);    break;
            case '5':   testDirectColor(gc,NULL,0xFFFFFFFFL,doCenter);  break;
            case '6':   testText(gc,doCenter);                          break;
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive OpenGL testing functions
****************************************************************************/
static void OpenGLTests(
    GC_devCtx *gc)
{
    int         choice;
    GA_glCtx    *gl = NULL;

    /* Try to load the OpenGL driver */
    if ((gl = GA_glLoadDriver(gc->dc)) == NULL) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Could not load OpenGL engine...");
        EVT_getch();
        return;
        }

    /* Handle the OpenGL menu */
    for (;;) {
        GC_clrscr(gc);
        banner(gc);

        /* Get the OpenGL init and API function pointers */
        glFuncs.dwSize = sizeof(glFuncs);
        if (!GA_glQueryFunctions(gl,GA_GET_GLINITFUNCS,&glFuncs))
            PM_fatalError("Unable to get OpenGL init functions");
        _GA_glFuncs.dwSize = sizeof(_GA_glFuncs);
        if (!GA_glQueryFunctions(gl,GA_GET_GLFUNCS,&_GA_glFuncs))
            PM_fatalError("Unable to get OpenGL API functions");
        _GA_gluFuncs.dwSize = sizeof(_GA_gluFuncs);
        if (!GA_glQueryFunctions(gl,GA_GET_GLUFUNCS,&_GA_gluFuncs))
            PM_fatalError("Unable to get OpenGL Utility API functions");
        GC_printf(gc,"OpenGL Vendor Name:    %s\n",gl->OemVendorName);
        GC_printf(gc,"OpenGL Copyright:      %s\n",gl->OemCopyright);
        GC_printf(gc,"OpenGL Engine Version: %d.%d.%d\n",
            ((int)gl->DriverRev >> 16) & 0xFF,((int)gl->DriverRev >> 8) & 0xFF,(int)gl->DriverRev & 0xFF);
        GC_printf(gc,"OpenGL API Version:    %d.%d\n",
            ((int)gl->OpenGLRev >> 16) & 0xFF,((int)gl->OpenGLRev >> 8) & 0xFF);
        GC_printf(gc,"\n");
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 8 bits per pixel modes      [3] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 15 bits per pixel modes     [4] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 16 bits per pixel modes                                  \n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test256(gc,gl,doOpenGLTest);                        break;
            case '1':   testDirectColor(gc,gl,0x7FFFL,doOpenGLTest);        break;
            case '2':   testDirectColor(gc,gl,0xFFFFL,doOpenGLTest);        break;
            case '3':   testDirectColor(gc,gl,0x00FFFFFFL,doOpenGLTest);    break;
            case '4':   testDirectColor(gc,gl,0xFFFFFFFFL,doOpenGLTest);    break;
            }
        }

    /* Unload the OpenGL engine if loaded */
    if (gl)
        GA_glUnloadDriver(gl);
}

/****************************************************************************
REMARKS:
Main menu for all interactive hardware video overlay testing functions
****************************************************************************/
static void VideoOverlayTests(
    GC_devCtx *gc)
{
    int choice;

    /* Check if we have hardware video overlay support */
    if (!(gc->dc->Attributes & gaHaveAccelVideo)) {
        GC_printf(gc,"Hardware video overlay support not available...");
        EVT_getch();
        return;
        }

    /* Handle the video overlay menu */
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 8 bits per pixel modes      [3] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 15 bits per pixel modes     [4] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 16 bits per pixel modes                                  \n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test256(gc,NULL,doVideoOverlayTest);                        break;
            case '1':   testDirectColor(gc,NULL,0x7FFFL,doVideoOverlayTest);        break;
            case '2':   testDirectColor(gc,NULL,0xFFFFL,doVideoOverlayTest);        break;
            case '3':   testDirectColor(gc,NULL,0x00FFFFFFL,doVideoOverlayTest);    break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFFFL,doVideoOverlayTest);    break;
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive hardware video overlay testing functions
****************************************************************************/
static void VideoCaptureTests(
    GC_devCtx *gc)
{
    int choice;

    /* Check if we have hardware video overlay support, which we need for
     * this test to run
     */
    if (!(gc->dc->Attributes & gaHaveAccelVideo)) {
        GC_printf(gc,"Hardware video overlay support not available...");
        EVT_getch();
        return;
        }

    /* Check if we have hardware video overlay support */
    if (!(gc->dc->Attributes & gaHaveVideoCapture)) {
        GC_printf(gc,"Hardware video capture support not available...");
        EVT_getch();
        return;
        }

    /* Handle the video overlay menu */
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 8 bits per pixel modes      [3] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 15 bits per pixel modes     [4] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 16 bits per pixel modes                                  \n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test256(gc,NULL,doVideoCaptureTest);                        break;
            case '1':   testDirectColor(gc,NULL,0x7FFFL,doVideoCaptureTest);        break;
            case '2':   testDirectColor(gc,NULL,0xFFFFL,doVideoCaptureTest);        break;
            case '3':   testDirectColor(gc,NULL,0x00FFFFFFL,doVideoCaptureTest);    break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFFFL,doVideoCaptureTest);    break;
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void GlobalRefresh(
    GC_devCtx *gc)
{
    char    buf[40];
    int     choice,refresh = 0;

    GC_clrscr(gc);
    banner(gc);
    GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
    GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
    GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
        (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
    GC_printf(gc,"\n");
    GC_printf(gc,"Select new global refresh rate:\n\n");
    GC_printf(gc,"  [0] - 60 Hz\n");
    GC_printf(gc,"  [1] - 65 Hz\n");
    GC_printf(gc,"  [2] - 70 Hz\n");
    GC_printf(gc,"  [3] - 75 Hz\n");
    GC_printf(gc,"  [4] - 80 Hz\n");
    GC_printf(gc,"  [5] - 85 Hz\n");
    GC_printf(gc,"  [6] - 90 Hz\n");
    GC_printf(gc,"  [7] - 95 Hz\n");
    GC_printf(gc,"  [8] - 100 Hz\n");
    GC_printf(gc,"  [9] - Custom\n");
    GC_printf(gc,"  [Q] - Quit\n\n");
    GC_printf(gc,"Choice: ");
    choice = EVT_getch();
    if (tolower(choice) == 'q' || choice == 0x1B)
        return;
    switch (tolower(choice)) {
        case '0':   refresh = 60;   break;
        case '1':   refresh = 65;   break;
        case '2':   refresh = 70;   break;
        case '3':   refresh = 75;   break;
        case '4':   refresh = 80;   break;
        case '5':   refresh = 85;   break;
        case '6':   refresh = 90;   break;
        case '7':   refresh = 95;   break;
        case '8':   refresh = 100;  break;
        case '9':
            GC_printf(gc,"\n\nEnter Refresh Rate: ");
            if (GetString(gc,buf,sizeof(buf)) == -1)
                return;
            refresh = atof(buf);
            if (refresh < 40 || refresh > 200)
                refresh = 0;
            break;
        }
    if (refresh != 0) {
        init.SetGlobalRefresh(refresh,init.GetActiveHead());
        GA_saveCRTCTimings(gc->dc,init.GetActiveHead());
        }
}

/****************************************************************************
REMARKS:
Sets the text mode with the specified dimensions
****************************************************************************/
static int FindMode(
    GA_devCtx *dc,
    int xRes,
    int yRes,
    int bits)
{
    GA_modeInfo     modeInfo;
    N_uint16        *modes;

    /* Search for the text mode to use with the same resolution */
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (xRes == modeInfo.XResolution && yRes == modeInfo.YResolution && bits == modeInfo.BitsPerPixel) {
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                return *modes | gaLinearBuffer;
            return *modes;
            }
        }
    return -1;
}

/****************************************************************************
REMARKS:
Main menu for changing the active display head
****************************************************************************/
static void MultiHeadControl(
    GC_devCtx *gcOrg)
{
    int         choice;
    int         numHeads = init.GetNumberOfHeads();
    int         activeHead = init.GetActiveHead();
    int         displayOutput = init.GetDisplayOutput();
    GC_devCtx   *gc;

    if (gcOrg->dc->Attributes & gaHaveMultiHead) {
        /* If supporting multi-head with non-CRT outputs, make sure they are enabled. */
        if ((gcOrg->dc->Attributes & gaHaveDFPOutput) && !(displayOutput & gaOUTPUT_DFP))
            displayOutput |= gaOUTPUT_DFP;
        if ((gcOrg->dc->Attributes & gaHaveLCDOutput) && !(displayOutput & gaOUTPUT_LCD))
            displayOutput |= gaOUTPUT_LCD;
        if (!(displayOutput & gaOUTPUT_CRT))
            displayOutput |= gaOUTPUT_CRT;
        if (init.GetDisplayOutput() != displayOutput)
            init.SetDisplayOutput(displayOutput);
        }

    /* For NonVGA controllers we need to run this test in graphics modes
     * as some controllers only support multi-head output in graphics mode.
     * Also some controllers do not support multi-head in 8bpp modes, so
     * we force the color depth to 15bpp or higher.
     */
    GC_leave(gcOrg);
    if ((gc = GC_createExt(gcOrg->dc,80,50,GC_FONT_8X8,15)) == NULL)
        PM_fatalError("Unable to set 80x50 graphical text mode!");

    /* Now handle the multi-head switching */
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & gaHaveMultiHead)) {
            GC_printf(gc,"Multi-Head support not available...");
            EVT_getch();
            GC_destroy(gc);
            GC_restore(gcOrg);
            return;
            }
        GC_printf(gc,"Select display head to make active:\n\n");
        GC_printf(gc,"  [0] - Clone mode (all heads display same image)\n");
        GC_printf(gc,"  [1] - Primary Head\n");
        GC_printf(gc,"  [2] - Secondary Head\n");
        if (numHeads >= 3)
            GC_printf(gc,"  [3] - Ternary Head\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (tolower(choice) == 'q' || choice == 0x1B)
            break;
        switch (tolower(choice)) {
            case '0':
                activeHead = gaActiveHeadClone;
                break;
            case '1':
                activeHead = gaActiveHeadPrimary;
                break;
            case '2':
                activeHead = gaActiveHeadSecondary;
                break;
            case '3':
                if (numHeads >= 3)
                    activeHead = gaActiveHeadTernary;
                break;
            }

        /* Reset current display mode if the active head changed */
        if (activeHead != init.GetActiveHead()) {
            init.SetActiveHead(activeHead);
            GC_leave(gc);
            GC_restore(gc);
            }
        }

    /* Restore original display mode */
    GC_destroy(gc);
    GC_restore(gcOrg);
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void TVOutputControl(
    GC_devCtx *gc)
{
    int         choice,mode,changed = false,cntMode = init.GetVideoMode();
    GA_options  opt,orgOpt;
    GA_CRTCInfo crtc;

    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    orgOpt = opt;
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & gaHaveTVOutput)) {
            GC_printf(gc,"TV support not available...");
            EVT_getch();
            return;
            }
        GC_printf(gc,"Select TV value to control:\n\n");
        GC_printf(gc,"  [0] - Select TV only mode\n");
        GC_printf(gc,"  [1] - Select Simultaneous mode\n");
        GC_printf(gc,"  [2] - Select CRT only mode\n");
        GC_printf(gc,"  [3] - Center 640x480 TV image and adjust brightness\n");
        GC_printf(gc,"  [4] - Center 800x600 TV image and adjust brightness\n");
        GC_printf(gc,"  [5] - Set output mode to underscan\n");
        GC_printf(gc,"  [6] - Set output mode to overscan\n");
        GC_printf(gc,"  [7] - Set output mode to NTSC\n");
        GC_printf(gc,"  [8] - Set output mode to NTSC-J\n");
        GC_printf(gc,"  [9] - Set output mode to PAL\n");
        GC_printf(gc,"  [A] - Set output mode to PAL-M\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (tolower(choice) == 'q' || choice == 0x1B)
            break;
        switch (tolower(choice)) {
            case '0':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_SELECTMASK) | gaOUTPUT_TV);
                changed = true;
                break;
            case '1':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_SELECTMASK) | gaOUTPUT_TV | gaOUTPUT_CRT);
                changed = true;
                break;
            case '2':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_SELECTMASK) | gaOUTPUT_CRT);
                changed = true;
                break;
            case '3':
                if ((mode = FindMode(gc->dc,640,480,8)) != -1)
                    CenterTVMode(gc,mode);
                break;
            case '4':
                if ((mode = FindMode(gc->dc,800,600,8)) != -1)
                    CenterTVMode(gc,mode);
                break;
            case '5':
                init.SetDisplayOutput(opt.outputDevice & ~gaOUTPUT_TVOVERSCAN);
                changed = true;
                break;
            case '6':
                init.SetDisplayOutput(opt.outputDevice | gaOUTPUT_TVOVERSCAN);
                changed = true;
                break;
            case '7':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_TVCOLORMASK) | gaOUTPUT_TVNTSC);
                changed = true;
                break;
            case '8':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_TVCOLORMASK) | gaOUTPUT_TVNTSC_J);
                changed = true;
                break;
            case '9':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_TVCOLORMASK) | gaOUTPUT_TVPAL);
                changed = true;
                break;
            case 'a':
                init.SetDisplayOutput((opt.outputDevice & ~gaOUTPUT_TVCOLORMASK) | gaOUTPUT_TVPAL_M);
                changed = true;
                break;
            }

        /* Reset current display mode */
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(cntMode | gaDontClear,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc);
        }

    /* Check if the settings should be saved */
    if (changed) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Save settings (Y/N)?");
        choice = EVT_getch();
        if (tolower(choice) == 'y') {
            init.GetOptions(&opt);
            GA_saveOptions(gc->dc,&opt);
            }
        else
            init.SetOptions(&orgOpt);
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void LCDOutputControl(
    GC_devCtx *gc)
{
    int         choice,changed = false,cntMode = init.GetVideoMode();
    GA_options  opt,orgOpt;
    GA_CRTCInfo crtc;

    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    orgOpt = opt;
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & (gaHaveLCDOutput | gaHaveDFPOutput))) {
            GC_printf(gc,"LCD/DFP panel support not available...");
            EVT_getch();
            return;
            }
        GC_printf(gc,"Select LCD value to control:\n\n");
        GC_printf(gc,"  [0] - Select CRT mode\n");
        if (gc->dc->Attributes & gaHaveLCDOutput)
            GC_printf(gc,"  [1] - Select LCD mode\n");
        if (gc->dc->Attributes & gaHaveDFPOutput)
            GC_printf(gc,"  [2] - Select DFP mode\n");
        if (gc->dc->Attributes & gaHaveLCDOutput)
            GC_printf(gc,"  [3] - Select LCD+CRT mode\n");
        if ((gc->dc->Attributes & (gaHaveDFPOutput | gaHaveLCDOutput)) == (gaHaveDFPOutput | gaHaveLCDOutput))
            GC_printf(gc,"  [4] - Select LCD+DFP mode\n");
        if (gc->dc->Attributes & gaHaveDFPOutput)
            GC_printf(gc,"  [5] - Select CRT+DFP mode\n");
        if ((gc->dc->Attributes & (gaHaveDFPOutput | gaHaveLCDOutput)) == (gaHaveDFPOutput | gaHaveLCDOutput))
            GC_printf(gc,"  [6] - Select LCD+CRT+DFP mode\n");
        GC_printf(gc,"  [7] - Enable expansion to LCD panel size\n");
        GC_printf(gc,"  [8] - Disable expansion to LCD panel size\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (tolower(choice) == 'q' || choice == 0x1B)
            break;
        switch (tolower(choice)) {
            case '0':
                init.SetDisplayOutput(gaOUTPUT_CRT);
                changed = true;
                break;
            case '1':
                init.SetDisplayOutput(gaOUTPUT_LCD);
                changed = true;
                break;
            case '2':
                init.SetDisplayOutput(gaOUTPUT_DFP);
                changed = true;
                break;
            case '3':
                init.SetDisplayOutput(gaOUTPUT_LCD | gaOUTPUT_CRT);
                changed = true;
                break;
            case '4':
                init.SetDisplayOutput(gaOUTPUT_LCD | gaOUTPUT_DFP);
                changed = true;
                break;
            case '5':
                init.SetDisplayOutput(gaOUTPUT_CRT | gaOUTPUT_DFP);
                changed = true;
                break;
            case '6':
                init.SetDisplayOutput(gaOUTPUT_LCD | gaOUTPUT_CRT | gaOUTPUT_DFP);
                changed = true;
                break;
            case '7':
                opt.bLCDExpand = true;
                init.SetOptions(&opt);
                changed = true;
                break;
            case '8':
                opt.bLCDExpand = false;
                init.SetOptions(&opt);
                changed = true;
                break;
            }

        /* Retrieve the current options after the driver has changed them */
        init.GetOptions(&opt);

        /* Reset current display mode. On some hardware the above SetDisplayOutput
         * function will not take effect until the next mode set.
         */
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(cntMode | gaDontClear,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc);
        }

    /* Check if the settings should be saved */
    if (changed) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Save settings (Y/N)?");
        choice = EVT_getch();
        if (tolower(choice) == 'y') {
            init.GetOptions(&opt);
            GA_saveOptions(gc->dc,&opt);
            }
        else
            init.SetOptions(&orgOpt);
        }
}

/****************************************************************************
REMARKS:
Function to build a gamma correction color ramp
****************************************************************************/
static void BuildGammaRamp(
    GA_paletteExt *pal,
    float gamma)
{
    int     i;
    float   igamma = 1.0 / gamma;

    for (i = 0; i < 256; i++) {
        pal[i].Red = pal[i].Green = pal[i].Blue =
            (ushort)((pow(i / 255.0,igamma) * 65535.0) + 0.5);
        }
}

/****************************************************************************
REMARKS:
Main menu for gamma corrcetion control
****************************************************************************/
static void GammaControl(
    GC_devCtx *gc)
{
    char            buf[40];
    float           gamma;
    GA_options      opt;

    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    GC_clrscr(gc);
    banner(gc);
    GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
    GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
    GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
        (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
    GC_printf(gc,"\n");
    if (driver.SetGammaCorrectData) {
        GC_printf(gc,"Enter gamma correction factor (ie: 2.3): ");
        if (GetString(gc,buf,sizeof(buf)) == -1)
            return;
        if ((gamma = atof(buf)) == 0.0)
            return;
        BuildGammaRamp(opt.gammaRamp,gamma);
        init.SetOptions(&opt);
        GA_saveOptions(gc->dc,&opt);
        }
    else {
        GC_printf(gc,"Gamma correction not available...");
        EVT_getch();
        return;
        }
}

#ifndef LITE
/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void ScrollingTest(
    GC_devCtx *gc)
{
    int choice;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & gaHaveDisplayStart)) {
            GC_printf(gc,"Scrolling not available...");
            EVT_getch();
            return;
            }
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 4 bits per pixel modes      [3] - 16 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 8 bits per pixel modes      [4] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 15 bits per pixel modes     [5] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test16(gc,doScrollTest);                            break;
            case '1':   test256(gc,NULL,doScrollTest);                      break;
            case '2':   testDirectColor(gc,NULL,0x7FFFL,doScrollTest);      break;
            case '3':   testDirectColor(gc,NULL,0xFFFFL,doScrollTest);      break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFL,doScrollTest);    break;
            case '5':   testDirectColor(gc,NULL,0xFFFFFFFFL,doScrollTest);  break;
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void ZoomingTest(
    GC_devCtx *gc)
{
    int choice;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & gaHaveDisplayStart)) {
            GC_printf(gc,"Zooming not available...");
            EVT_getch();
            return;
            }
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 4 bits per pixel modes      [3] - 16 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 8 bits per pixel modes      [4] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 15 bits per pixel modes     [5] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test16(gc,doZoomTest);                              break;
            case '1':   test256(gc,NULL,doZoomTest);                        break;
            case '2':   testDirectColor(gc,NULL,0x7FFFL,doZoomTest);        break;
            case '3':   testDirectColor(gc,NULL,0xFFFFL,doZoomTest);        break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFL,doZoomTest);      break;
            case '5':   testDirectColor(gc,NULL,0xFFFFFFFFL,doZoomTest);    break;
            }
        }
}

/****************************************************************************
REMARKS:
Main menu for all interactive testing functions
****************************************************************************/
static void StereoTest(
    GC_devCtx *gc)
{
    int choice;

    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)gc->dc->Version >> 8,(int)gc->dc->Version & 0xFF,(int)gc->dc->TotalMemory);
        GC_printf(gc,"\n");
        if (!(gc->dc->Attributes & gaHaveStereo) && !(PM_getOSType() & (_OS_DOS | _OS_RTTARGET))) {
            GC_printf(gc,"Stereo emulation support not available (%s)...",PM_getOSName());
            EVT_getch();
            return;
            }
        GC_printf(gc,". HW Triple Buffering:    %-3s     ", gc->dc->Attributes & gaHaveTripleBuffer ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Flipping:     %-3s\n", gc->dc->Attributes & gaHaveStereo ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Sync:         %-3s     ", gc->dc->Attributes & gaHaveHWStereoSync ? "Yes" : "No");
        GC_printf(gc,". HW Stereo Sync (EVC):   %-3s\n", gc->dc->Attributes & gaHaveEVCStereoSync ? "Yes" : "No");
        GC_printf(gc,"\n");
        GC_printf(gc,"Select color mode to test:\n\n");
        GC_printf(gc,"  [0] - 4 bits per pixel modes      [3] - 16 bits per pixel modes\n");
        GC_printf(gc,"  [1] - 8 bits per pixel modes      [4] - 24 bits per pixel modes\n");
        GC_printf(gc,"  [2] - 15 bits per pixel modes     [5] - 32 bits per pixel modes\n");
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");
        choice = EVT_getch();
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test16(gc,doStereoTest);                            break;
            case '1':   test256(gc,NULL,doStereoTest);                      break;
            case '2':   testDirectColor(gc,NULL,0x7FFFL,doStereoTest);      break;
            case '3':   testDirectColor(gc,NULL,0xFFFFL,doStereoTest);      break;
            case '4':   testDirectColor(gc,NULL,0xFFFFFFL,doStereoTest);    break;
            case '5':   testDirectColor(gc,NULL,0xFFFFFFFFL,doStereoTest);  break;
            }
        }
}
#endif

/****************************************************************************
REMARKS:
Writes the currently configured options to disk.
****************************************************************************/
void SaveOptions(
    GA_devCtx *dc)
{
    GA_options  opt;

    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    GA_saveOptions(dc,&opt);
}

/****************************************************************************
REMARKS:
Sets the global options according to command line parameters.
****************************************************************************/
static void SetGlobalOptions(void)
{
    GA_globalOptions    opt;

    opt.dwSize = sizeof(opt);
    GA_getGlobalOptions(&opt, false);

    if (multiModeX) {
        /* Setup a simple side/side virtual multi-controller mode */
        opt.bVirtualDisplay = true;
        opt.virtualSize.left = opt.virtualSize.top = 0;
        opt.virtualSize.right = multiModeX * 2;
        opt.virtualSize.bottom = multiModeY;
        opt.resolutions[0].left = opt.resolutions[0].top = 0;
        opt.resolutions[0].right = multiModeX;
        opt.resolutions[0].bottom = multiModeY;
        opt.resolutions[1].left = opt.resolutions[1].top = 0;
        opt.resolutions[1].right = multiModeX;
        opt.resolutions[1].bottom = multiModeY;
        opt.bounds[0].left = opt.bounds[0].top = 0;
        opt.bounds[0].right = multiModeX;
        opt.bounds[0].bottom = multiModeY;
        opt.bounds[1].left = multiModeX;
        opt.bounds[1].top = 0;
        opt.bounds[1].right = multiModeX * 2;
        opt.bounds[1].bottom = multiModeY;
        }
    GA_setGlobalOptions(&opt);
}

/****************************************************************************
REMARKS:
Displays the main menu screen
****************************************************************************/
void MainScreen(
    int deviceIndex)
{
    int                 choice;
    ibool               doNothing;
    GA_devCtx           *dc;
    GC_devCtx           *gc;
    GA_configInfo       info;
    GA_options          opt;

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(deviceIndex,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

    /* Find out if any filter options are set */
    opt.dwSize = sizeof(opt);
    init.GetOptions(&opt);
    isPortrait = opt.bPortrait;
    isFlipped  = opt.bFlipped;
    isInverted = opt.bInvertColors;
    isFiltered = isPortrait || isFlipped || isInverted;

#ifdef PRO
    /* Set the gaSlowBltSys flag so that we fully test the BitBltSys
     * functions using the hardware versions and not the software ones.
     */
    dc->WorkArounds |= gaSlowBltSys;
#endif

    /* Leave the primary console so we can create a new one (potentially
     * on a secondary display device)
     */
    if (primaryGC && deviceIndex == primaryGC->dc->DeviceIndex) {
        /* Reuse the primary GC for the menus */
        gc = primaryGC;
        }
    else {
        /* Set the text mode and initialise the console library */
        if (primaryGC)
            GC_leave(primaryGC);
        if ((gc = GC_create(dc,textModeX,textModeY,fontSize)) == NULL)
            if ((gc = GC_create(dc,80,25,fontSize)) == NULL) {
                PM_fatalError("Unable to find 80x25 text mode!");
                }
        }
    for (;;) {
        GC_clrscr(gc);
        banner(gc);
        GC_printf(gc,"Vendor Name:  %s\n",gc->dc->OemVendorName);
        GC_printf(gc,"Copyright:    %s\n",gc->dc->OemCopyright);
        info.dwSize = sizeof(info);
        init.GetConfigInfo(&info);
        info.dwSize = sizeof(info);
        GC_printf(gc,"Version:      %d.%02d with %d Kb memory\n",
            (int)dc->Version >> 8,(int)dc->Version & 0xFF, (int)dc->TotalMemory);
        GC_printf(gc,"\n");
        GC_printf(gc,"Manufacturer: %s\n", info.ManufacturerName);
        GC_printf(gc,"Chipset:      %s\n", info.ChipsetName);
        GC_printf(gc,"RAMDAC:       %s\n", info.DACName);
        GC_printf(gc,"Clock:        %s\n", info.ClockName);
        GC_printf(gc,"Version:      %s\n", info.VersionInfo);
        GC_printf(gc,"Build:        %s", info.BuildDate);
        if (info.Certified)
            GC_printf(gc," (Certified %s)\n", info.CertifiedDate);
        else
            GC_printf(gc,"\n");
        GC_printf(gc,"\n");
        GC_printf(gc,"Select test to perform:\n\n");
        GC_printf(gc,"  [0] - Interactive Tests\n");
        GC_printf(gc,"  [1] - Centering and Refresh Control\n");
        GC_printf(gc,"  [2] - Global Refresh Control\n");
        GC_printf(gc,"  [3] - Multi-Head Control\n");
        GC_printf(gc,"  [4] - TV Output Control\n");
        GC_printf(gc,"  [5] - LCD Panel Output Control\n");
        GC_printf(gc,"  [6] - Color Correction (Gamma) Control\n");
        GC_printf(gc,"  [7] - VESA DPMS Power Management Tests\n");
        GC_printf(gc,"  [8] - VESA DDC Display Data Channel Tests\n");
        GC_printf(gc,"  [9] - 3D OpenGL Compatible Rendering Tests\n");
        GC_printf(gc,"  [A] - Video Overlay Tests\n");
#ifndef LITE
        GC_printf(gc,"  [B] - Video Capture Tests\n");
        GC_printf(gc,"  [C] - VESA DDC/CI Interactive Monitor Control Tests\n");
        GC_printf(gc,"  [D] - Hardware Scrolling Test\n");
        GC_printf(gc,"  [E] - Zooming Test\n");
        GC_printf(gc,"  [F] - Stereo LC glasses Test\n");
#endif
        GC_printf(gc,"  [Q] - Quit\n\n");
        GC_printf(gc,"Choice: ");

        do {
            doNothing = false;
            choice = tolower(EVT_getch());
            if (choice == 'q' || choice == 'Q' || choice == 0x1B)
                break;

            /* Install our suspend callback handler */
            PM_setSuspendAppCallback(SuspendCallbackFunc);

            switch (choice) {
                case '0':   InteractiveTests(gc);       break;
                case '1':   CenteringAndRefresh(gc);    break;
                case '2':   GlobalRefresh(gc);          break;
                case '3':   MultiHeadControl(gc);       break;
                case '4':   TVOutputControl(gc);        break;
                case '5':   LCDOutputControl(gc);       break;
                case '6':   GammaControl(gc);           break;
                case '7':   DPMSTests(gc);              break;
                case '8':   DDCTests(gc);               break;
                case '9':   OpenGLTests(gc);            break;
                case 'a':   VideoOverlayTests(gc);      break;
                case 'b':   VideoCaptureTests(gc);      break;
#ifndef LITE
                case 'c':   DDCCITests(gc);             break;
                case 'd':   ScrollingTest(gc);          break;
                case 'e':   ZoomingTest(gc);            break;
                case 'f':   StereoTest(gc);             break;
#endif
                default :   doNothing = true;
                            EVT_flush(EVT_KEYUP | EVT_KEYDOWN | EVT_KEYREPEAT);
                            break;
                }

            /* Remove our suspend callback handler */
            PM_setSuspendAppCallback(NULL);

            } while (doNothing == true);

        if (choice == 'q' || choice == 'Q' || choice == 0x1B) break;
        GC_printf(gc,"\n");
        }

    /* Destroy console if it's not the primary one */
    if (gc != primaryGC)
        GC_destroy(gc);

    /* Unload the device driver for this device */
    GA_unloadDriver(dc);
}

/****************************************************************************
PARAMETERS:
x   - X coordinate of the mouse cursor position (screen coordinates)
y   - Y coordinate of the mouse cursor position (screen coordinates)

REMARKS:
This gets called periodically to move the mouse. It will get called when
the mouse may not have actually moved, so check if it has before redrawing
it.
****************************************************************************/
void EVTAPI moveMouse(
    int x,
    int y)
{
}

/****************************************************************************
PARAMETERS:
params  - Optional parameters passed to function

REMARKS:
This gets called periodically during event handling and we use this to
handle the polled screen switching code for the command line control
center.
****************************************************************************/
static void EVTAPI HeartBeatCallback(
    void *params)
{
    if (drv_state2d.Flush)
        drv_state2d.Flush();
    if (init.PollForDisplaySwitch && init.PollForDisplaySwitch())
        init.PerformDisplaySwitch();
}

#ifdef __CONSOLE__
/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
#ifdef  LITE
    printf("GACtrl - %s Driver Control Center\n", GRAPHICS_PRODUCT);
#else
    printf("GATest - %s Driver Compliance Test\n", GRAPHICS_PRODUCT);
#endif
    printf("         Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    printf("%s\n",copyright_str);
    printf("\n");
    printf("Options are:\n");
    printf("    -s       - Run in software only mode\n");
    printf("    -e       - Emulate missing hardware features in software\n");
    printf("    -g       - Use VESA GTF for refresh rate control\n");
    printf("    -m<XxY>  - Use XxY text mode (ie: -m80x60 for 80x60 text mode)\n");
    printf("    -x       - Render to front buffer for OpenGL tests\n");
    printf("    -X       - Disable hardware page flipping for OpenGL tests\n");
    printf("    -d       - Disable dithering for 15/16-bit OpenGL tests\n");
#if defined(PRO)
    printf("    -t<name> - Skip to acceleration test 'name'\n");
    printf("    -T       - Display list of all tests\n");
    printf("    -b       - Execute debug break point at start of test\n");
    printf("    -f       - Run tests at full speed (no wait for keypress)\n");
    printf("    -a       - Skip small tests\n");
    printf("    -k<mask> - Set plane mask for write operations\n");
    printf("    -c<XxY>  - Enable side/side multi-controller mode 2XxY\n");
    printf("    -n<n>    - Force a graphics font size (1 = 8x8, 2 = 8x14, 3 = 8x6)\n");
    printf("    -o<n>    - Use output device number 'n'\n");
#endif
    exit(1);
}
#endif

/****************************************************************************
REMARKS:
Parses the command line arguments.
****************************************************************************/
static void parseArguments(
    int argc,
    char *argv[])
{
    int     option;
    char    *argument;

    /* Parse command line options */

    do {
        option = getcmdopt(argc,argv,"segft:Tk:bac:n:m:xXdo:",&argument);
        switch (option) {
            case 'a':
                doSmallTest = false;
                break;
            case 'b':
                breakPoint = true;
                break;
            case 'c':
                sscanf(argument,"%dx%d", &multiModeX, &multiModeY);
                break;
            case 'd':
                noDither = true;
                break;
            case 'e':
                softwareEmulate = true;
                break;
            case 'f':
                fullSpeed = true;
                break;
            case 'g':
                useGTF = true;
                break;
            case 'k':
                sscanf(argument,"%lx", &planeMask);
                break;
            case 'm':
                sscanf(argument,"%dx%d", &textModeX, &textModeY);
                break;
            case 'n':
                /* A negative value will force graphics console mode */
                sscanf(argument,"%d", &fontSize);
                fontSize = -fontSize;
                break;
            case 'o':
                sscanf(argument,"%d", &cntDevice);
                break;
            case 's':
                softwareOnly = true;
                break;
#ifdef PRO
            case 't':
                strcpy(accelTestName,argument);
                break;
            case 'T':
                displayTestNames();
                exit(1);
                break;
#endif
            case 'x':
                doubleBuffer = 0;
                break;
            case 'X':
                doubleBuffer = 1;
                break;
            case ALLDONE:
                break;
#ifdef __CONSOLE__
            case 'h':
            case PARAMETER:
            case INVALID:
            default:
                help();
                /* won't return */
#endif
            }
        } while (option != ALLDONE);
}

int main(int argc,char *argv[])
{
    int         i,choice;

    /* Locate the executable path for local bitmap resources */
#ifdef __UNIX__
    memset(exePathName, 0, PM_MAX_PATH);
    if (readlink("/proc/self/exe", exePathName, PM_MAX_PATH) == -1)
#endif
        strcpy(exePathName,argv[0]);

    /* Find the number of display devices attached
     * Note: this should be done _before_ opening the console
     */
    parseArguments(argc,argv);

    /* Check to see if we are running in a window and primary device is requested */
    if ((cntDevice < 1) && PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        return -1;
        }

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,640,480,8,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Initialise the event library */
    EVT_init(moveMouse);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Register our handler to handle polled mode switch changes */
    EVT_setHeartBeatCallback(HeartBeatCallback,NULL);

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    ZTimerInit();
    PM_init();
    SetGlobalOptions();

    if (cntDevice < 0) {
        /* Load the device driver for the primary display and set text mode */
        if ((primaryDC = GA_loadDriver(0,false)) == NULL)
            PM_fatalError(GA_errorMsg(GA_status()));
        if ((primaryGC = GC_create(primaryDC,textModeX,textModeY,fontSize)) == NULL)
            if ((primaryGC = GC_create(primaryDC,80,25,fontSize)) == NULL) {
                PM_fatalError("Unable to find 80x25 text mode!");
                }
        if ((numDevices = GA_enumerateDevices(false)) > 1) {
            for (;;) {
                GC_clrscr(primaryGC);
                banner(primaryGC);
                GC_printf(primaryGC,"Select display adapter:\n\n");
                for (i = 0; i < numDevices; i++) {
                    GC_printf(primaryGC,"  [%d] - Display Adapter %d\n", i, i);
                    }
                GC_printf(primaryGC,"  [Q] - Quit\n\n");
                GC_printf(primaryGC,"Choice: ");
                choice = EVT_getch();
                if (choice == 'q' || choice == 'Q' || choice == 0x1B)
                    break;
                choice -= '0';
                if (choice >= 0 && choice < numDevices) {
                    cntDevice = choice;
                    GA_setActiveDevice(cntDevice);
                    MainScreen(cntDevice);
                    GA_setActiveDevice(0);
                    /* Restore the primary console if needed */
                    if (cntDevice != primaryGC->dc->DeviceIndex)
                        GC_restore(primaryGC);
                    }
                }
            }
        else {
            MainScreen(0);
            }
        GC_destroy(primaryGC);
        GA_unloadDriver(primaryDC);
        }
    else {
        /* Need to always load driver for primary device */
        PM_lockSNAPAccess(0, true);
        locked = true;
        if ((primaryDC = GA_loadDriver(0,false)) == NULL)
            PM_fatalError(GA_errorMsg(GA_status()));

        /* Use graphics modes for console */
        if (fontSize > 0)
            fontSize = -fontSize;

        /* Use designated device and skip device choice */
        GA_setActiveDevice(cntDevice | GA_DEVICE_MIXED_MODE);
        MainScreen(cntDevice);
        GA_setActiveDevice(0);
        GA_unloadDriver(primaryDC);
        PM_unlockSNAPAccess(0);
        locked = false;
        }

    /* Exit the event library */
    EVT_exit();

    /* Restore the console */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    return 0;
}
