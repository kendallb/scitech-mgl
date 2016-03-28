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
* Description:  VESA GTF test program using SciTech SNAP Graphics.
*
****************************************************************************/

#include "snap/gasdk.h"
#include "event.h"
#include "cmdline.h"
#include <ctype.h>
#include <math.h>

/*--------------------------- Global variables ----------------------------*/

#define SYNC_COUNT      5

static PM_HWND  hwndConsole;
static void     *stateBuf;
float           settleTime = 2.0;
int             startX = 640;
int             startHZ = 60;
int             bitsPerPixel = 8;
int             maxHZ = 120;
int             resolutionIncr = -1;
int             autostep = 0;
int             animate = 1;
int             refreshIncr = -1;
int             aspectX = 4;
int             aspectY = 3;
int             vRefresh = 0;
GA_initFuncs    init;
GA_driverFuncs  driver;

#define MAX_LINES 35

typedef struct {
    int x1,y1;
    int x2,y2;
    int dx1,dy1;
    int dx2,dy2;
    int color;
    } vector;

vector          *lines;
int             numLines,lineMaxX,lineMaxY;
int             lineOffX,lineOffY;

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
    EVT_exit();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to check if a key has
been hit. We check for keydown and keyrepeat events, and we also flush the
event queue of all non keydown events to avoid it filling up.
****************************************************************************/
int EVT_kbhit(void)
{
    int     hit;
    event_t evt;

    hit = EVT_peekNext(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    EVT_flush(~(EVT_KEYDOWN | EVT_KEYREPEAT));
    return hit;
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to read an ASCII keypress
and return it. This function will block until a key is hit.
****************************************************************************/
int EVT_getch(void)
{
    event_t evt;

    EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    return EVT_asciiCode(evt.message);
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
REMARKS:
Displays the sign on banner on the screen.
****************************************************************************/
void banner(void)
{
    printf("GTFTest - VESA Generalised Timing Formula Compliance Test\n");
    printf("          Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    printf("%s\n",copyright_str);
    printf("\n");
}

/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
    banner();
    printf("Options are:\n");
    printf("    -s<sec>  - Settle time between mode sets (default 2 seconds)\n");
    printf("    -x<xres> - Set starting X resolution\n");
    printf("    -b<bits> - Set the color depth\n");
    printf("    -e<Hz>   - Set starting refresh rate (default 60Hz)\n");
    printf("    -m<Hz>   - Set maxium refresh rate (default 120Hz)\n");
    printf("    -r<step> - Increment resolution in 'step' pixel increments\n");
    printf("    -f<incr> - Increment refresh rate in 'incr' Hz increments\n");
    printf("    -a<X:Y>  - Set aspect ratio (default 4:3, ie: -a16:9)\n");
    exit(1);
}

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
        option = getcmdopt(argc,argv,"s:x:e:m:r:f:a:b:",&argument);
        switch (option) {
            case 's':
                settleTime = atof(argument);
                break;
            case 'x':
                startX = atoi(argument);
                break;
            case 'e':
                startHZ = atoi(argument);
                break;
            case 'm':
                maxHZ = atoi(argument);
                break;
            case 'r':
                resolutionIncr = atoi(argument);
                break;
            case 'f':
                refreshIncr = atoi(argument);
                break;
            case 'a':
                sscanf(argument,"%d:%d", &aspectX, &aspectY);
                break;
            case 'b':
                bitsPerPixel = atoi(argument);
                break;
            case ALLDONE:
                break;
            case 'h':
            case PARAMETER:
            case INVALID:
            default:
                help();
            }
        } while (option != ALLDONE);
}

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
Function to find the maximum X resolution at 8bpp and 60Hz non-interlaced
for the graphics card.
****************************************************************************/
static int findMaxResolution(void)
{
    int         xResMax,yResMax;
    GA_CRTCInfo crtcMax;
    GA_modeInfo miMax;

    /* Find the maximum resolution at this color depth */
    miMax.dwSize = sizeof(miMax);
    if (init.GetCustomVideoModeInfo(640,480,-1,-1,bitsPerPixel,&miMax) != 0)
        PM_fatalError("Unable to find maximum resolution!");
    for (xResMax = RoundDown(miMax.MaxScanLineWidth,bitsPerPixel); xResMax > 640; xResMax -= 8) {
        yResMax = (xResMax * aspectY) / aspectX;
        if (init.GetCustomVideoModeInfo(xResMax,yResMax,-1,-1,bitsPerPixel,&miMax) != 0)
            continue;
        if (!GA_computeCRTCTimings(dc,0,&miMax,60,false,&crtcMax,true))
            continue;
        break;
        }
    return xResMax;
}

/****************************************************************************
REMARKS:
Function to check if the vSync line is active
****************************************************************************/
static ibool checkForVSync(void)
{
    ibool   active = false;
    ibool   vSync = driver.IsVSync();

    /* Check for changing vSync signal */
    LZTimerOn();
    while (LZTimerLap() < 100000UL) {
        if (driver.IsVSync() != vSync) {
            active = true;
            break;
            }
        }
    LZTimerOff();
    return active;
}

/****************************************************************************
REMARKS:
Function to measure the refresh rate from the hardware
****************************************************************************/
static void ReadRefresh(void)
{
    int     i;
#ifdef  __WINDOWS32__
    DWORD   oldclass;
    HANDLE  hprocess;

    /* Set the priority of the process to maximum for accurate timing */
    hprocess = GetCurrentProcess();
    oldclass = GetPriorityClass(hprocess);
    SetPriorityClass(hprocess, REALTIME_PRIORITY_CLASS);
#endif

    if (checkForVSync()) {
        driver.WaitVSync();
        driver.WaitVSync();
        LZTimerOn();
        for (i = 0; i < SYNC_COUNT; i++)
            driver.WaitVSync();
        LZTimerOff();
        vRefresh = (int)((SYNC_COUNT * 10000000.0) / LZTimerCount());
        }
    else {
        vRefresh = 0;
        }

#ifdef  __WINDOWS32__
    SetPriorityClass(hprocess, oldclass);
#endif
}

/****************************************************************************
REMARKS:
Draw the background image for the centering and refresh display
****************************************************************************/
static void drawBackground(
    GA_CRTCInfo *crtc,
    int HZ)
{
    char        buf[80];
    int         i,min,max;
    long        range;
    GA_palette  pal[256],*p;

    min = 32;
    max = 253;
    range = max - min;
    for (i = 0; i < 254; i++) {
        pal[i].Red = 0;
        pal[i].Blue = (((i*range)/254)+min);
        pal[i].Green = 0;
        }
    pal[254].Red = pal[254].Green = pal[254].Blue = 128;
    pal[255].Red = pal[255].Green = pal[255].Blue = 255;
    if (modeInfo.BitsPerPixel > 8) {
        for (i = 0; i < maxY; i++) {
            p = &pal[(i * 254L) / maxY];
            SetForeColor(rgbColor(p->Red,p->Green,p->Blue));
            draw2d.DrawLineInt(0,i,maxX,i,true);
            }
        }
    else {
        driver.SetPaletteData(pal,256,0,false);
        for (i = 0; i < maxY; i++) {
            SetForeColor((i * 254L) / maxY);
            draw2d.DrawLineInt(0,i,maxX,i,true);
            }
        defcolor = 255;
        }
    SetForeColor(defcolor);
    draw2d.DrawLineInt(0,0,maxX,0,true);
    draw2d.DrawLineInt(0,0,0,maxY,true);
    draw2d.DrawLineInt(maxX,0,maxX,maxY,true);
    draw2d.DrawLineInt(0,maxY,maxX,maxY,true);

    sprintf(buf,"%d x %d %d bpp",(int)maxX+1,(int)maxY+1,(int)modeInfo.BitsPerPixel);
    WriteText(8,8,buf,defcolor);
    ReadRefresh();
    sprintf(buf,"Refresh: %d (%d.%d) Hz", HZ,vRefresh / 10, vRefresh % 10);
    WriteText(maxX-180,8,buf,defcolor);
}

/****************************************************************************
REMARKS:
Computes a random color value and returns it. To do this so it works
properly in RGB modes with all pixel formats, we need to generate a random
32 bit number and then mask it to the current pixel format. In some systems
the alpha channel information in 16 bit and 32 bit pixel modes is active,
and hence *must* be set to zero for proper operation (the ATI Mach64 is
one such card).
****************************************************************************/
GA_color randomColor(void)
{
    if (modeInfo.BitsPerPixel == 8) {
        return rand()%254+1;
        }
    else {
        return rgbColor(rand()%256,rand()%256,rand()%256);
        }
}

/****************************************************************************
REMARKS:
Animate solid numLines on the screen to take up memory bandwidth. Lines are
the best type of primitive to draw, as they suck up major bandwidth due to
the random access nature of their drawing.
****************************************************************************/
void initLines(void)
{
    int i;

    lineMaxX = (maxX * 7) / 8;
    lineMaxY = (maxY * 7) / 8;
    lineOffX = maxX / 16;
    lineOffY = maxY / 16;
    numLines = ((float)MAX_LINES/lineMaxY)*lineMaxY;
    lines = malloc(sizeof(vector) * numLines);
    for (i = 0; i < numLines;i++) {
        lines[i].x1 = i*(lineMaxX/MAX_LINES);
        lines[i].y1 = i*(lineMaxY/MAX_LINES);
        lines[i].x2 = lineMaxX - i*(lineMaxX/MAX_LINES);
        lines[i].y2 = i*(lineMaxY/MAX_LINES);
        lines[i].dx1 = 1;
        lines[i].dy1 = -1;
        lines[i].dx2 = -1;
        lines[i].dy2 = 1;
        lines[i].color = randomColor();
        }
}

/****************************************************************************
REMARKS:
Animate solid numLines on the screen to take up memory bandwidth. Lines are
the best type of primitive to draw, as they suck up major bandwidth due to
the random access nature of their drawing.
****************************************************************************/
int animateLines(
    int i)
{
    if (i >= numLines)
        i = 0;

    /* Change the position of both vertices */
    lines[i].x1 += lines[i].dx1;
    lines[i].y1 += lines[i].dy1;
    lines[i].x2 += lines[i].dx2;
    lines[i].y2 += lines[i].dy2;

    /* Change the direction of the first vertice */
    if (lines[i].x1 > lineMaxX) lines[i].dx1 = -1;
    if (lines[i].x1 <= 0)   lines[i].dx1 = 1;
    if (lines[i].y1 > lineMaxY) lines[i].dy1 = -1;
    if (lines[i].y1 <= 0)   lines[i].dy1 = 1;

    /* Change the direction of the second vertice */
    if (lines[i].x2 > lineMaxX) lines[i].dx2 = -1;
    if (lines[i].x2 <= 0)   lines[i].dx2 = 1;
    if (lines[i].y2 > lineMaxY) lines[i].dy2 = -1;
    if (lines[i].y2 <= 0)   lines[i].dy2 = 1;

    SetForeColor(lines[i].color);
    draw2d.DrawLineInt(
        lines[i].x1 + lineOffX,
        lines[i].y1 + lineOffY,
        lines[i].x2 + lineOffX,
        lines[i].y2 + lineOffY,
        true);
    return ++i;
}

/****************************************************************************
REMARKS:
Exit the line animation code.
****************************************************************************/
void exitLines(void)
{
    free(lines);
}

/****************************************************************************
REMARKS:
Dumps the CRTC timings to a file.
****************************************************************************/
static void dumpCRTCValues(
    GA_CRTCInfo *crtc,
    int HZ)
{
    FILE    *f = fopen("gtftest.log", "at+");

    fprintf(f,"\nCRTC values for %dx%d %d bit @%dHz (%d.%dHz actual)\n",
        modeInfo.XResolution,modeInfo.YResolution, modeInfo.BitsPerPixel,
        HZ,vRefresh / 10, vRefresh % 10);
    fprintf(f,"\n");
    fprintf(f,"  hTotal      = %d\n", crtc->HorizontalTotal);
    fprintf(f,"  hSyncStart  = %d\n", crtc->HorizontalSyncStart);
    fprintf(f,"  hSyncEnd    = %d\n", crtc->HorizontalSyncEnd);
    fprintf(f,"\n");
    fprintf(f,"  vTotal      = %d\n", crtc->VerticalTotal);
    fprintf(f,"  vSyncStart  = %d\n", crtc->VerticalSyncStart);
    fprintf(f,"  vSyncEnd    = %d\n", crtc->VerticalSyncEnd);
    fprintf(f,"\n");
    fprintf(f,"  H sync pol  = %s\n", (crtc->Flags & gaHSyncNeg) ? "-" : "+");
    fprintf(f,"  V sync pol  = %s\n", (crtc->Flags & gaVSyncNeg) ? "-" : "+");
    fprintf(f,"\n");
    fprintf(f,"  Dot Clock   = %d.%02dMhz\n",   (int)crtc->PixelClock / 1000000, (int)crtc->PixelClock / 10000);
    fclose(f);
}

/****************************************************************************
REMARKS:
Function to test a specific resolution and refresh rate at 8bpp
****************************************************************************/
static int TestMode(
    GA_devCtx *_dc,
    int xRes,
    int HZ)
{
    int             ch,yRes,flags = gaLinearBuffer | gaRefreshCtrl;
    int             status = 0,line = 0;
    ulong           maxTimer = settleTime * 1000000L;
    GA_CRTCInfo     crtc;
    LZTimerObject   tm;

    /* Obtain the mode information and set the display mode */
    dc = _dc;
    virtualX = virtualY = bytesPerLine = -1;
    modeInfo.dwSize = sizeof(modeInfo);
    yRes = (xRes * aspectY) / aspectX;
    if (init.GetCustomVideoModeInfo(xRes,yRes,-1,-1,bitsPerPixel,&modeInfo) != 0)
        return 0x1B;
    if (!GA_computeCRTCTimings(dc,0,&modeInfo,HZ,false,&crtc,true))
        return 0x1B;
    if (init.SetCustomVideoMode(xRes,yRes,bitsPerPixel,flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,&crtc) != 0)
        return 0x1B;
    cntMode = flags;
    InitSoftwareRasterizer(0,1,false);
    drawBackground(&crtc,HZ);
    EVT_flush(EVT_EVERYEVT);
    initLines();
    LZTimerOnExt(&tm);
    for (;;) {
        if (autostep && (LZTimerLapExt(&tm) >= maxTimer))
            break;
        if (EVT_kbhit()) {
            if ((ch = EVT_getch()) == 0x1B) {
                status = 0x1B;
                break;
                }
            ch = tolower(ch);
            if (ch == 'f') {
                dumpCRTCValues(&crtc,HZ);
                }
            else if (ch == 'a') {
                animate ^= 1;
                }
            else if (ch == 'q') {
                autostep ^= 1;
                }
            else if (ch == '+' || ch == '-') {
                status = ch;
                break;
                }
            }
        if (animate)
            line = animateLines(line);
        }
    LZTimerOffExt(&tm);
    ExitSoftwareRasterizer();
    exitLines();
    return status;
}

/****************************************************************************
REMARKS:
Main program loop
****************************************************************************/
static void MainLoop(void)
{
    int             choice,xRes,HZ,maxX;
    GA_configInfo   info;
    N_uint32        oldMode;

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(0,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");

    /* Display information about the detected hardware */
    oldMode = init.GetVideoMode();
    banner();
    printf("Vendor Name:  %s\n",dc->OemVendorName);
    printf("Copyright:    %s\n",dc->OemCopyright);
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    info.dwSize = sizeof(info);
    printf("Version:      %d.%d with %d Kb memory\n",
        (int)dc->Version >> 8,(int)dc->Version & 0xFF, (int)dc->TotalMemory);
    printf("\n");
    printf("Manufacturer: %s\n", info.ManufacturerName);
    printf("Chipset:      %s\n", info.ChipsetName);
    printf("RAMDAC:       %s\n", info.DACName);
    printf("Clock:        %s\n", info.ClockName);
    printf("Version:      %s\n", info.VersionInfo);
    printf("Build:        %s\n", info.BuildDate);
    printf("\n");
    printf("Press a key to start test. During the test hit the 'f' key to fail a mode\n");
    printf("and dump the CRTC timings to the GTFTEST.LOG file. Hit 'Esc' to exit test.\n");
    fflush(stdout);
    choice = tolower(EVT_getch());
    if (choice != 0x1B) {
        maxX = findMaxResolution();
        for (xRes = startX; xRes <= maxX; xRes += resolutionIncr) {
            for (HZ = startHZ; HZ <= maxHZ; HZ += refreshIncr) {
                switch (TestMode(dc,xRes,HZ)) {
                    case 0x1B:
                        refreshIncr = resolutionIncr = -1;
                        break;
                    case '-':
                        xRes -= resolutionIncr*2;
                        if (xRes < (startX-resolutionIncr))
                            xRes = (startX-resolutionIncr);
                        HZ = maxHZ;
                        break;
                    }
                if (refreshIncr == -1)
                    break;
                }
            if (resolutionIncr == -1)
                break;
            }
        }

    /* Restore the original text mode */
    virtualX = virtualY = bytesPerLine = -1;
    init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);

    /* Unload the device driver for this device */
    GA_unloadDriver(dc);
}

int main(int argc,char *argv[])
{
    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Initialise the event library */
    EVT_init(moveMouse);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Find the number of display devices attached */
    parseArguments(argc,argv);
    ZTimerInit();
    PM_init();
    MainLoop();

    /* Exit the event library */
    EVT_exit();

    /* Restore the console */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    return 0;
}

