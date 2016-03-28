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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Main program code for the demo program. This _must_ not be
*               compiled with anything but normal 8086 instructions, since
*               it must be able to detect an incorrect processor
*               configuration!
*
****************************************************************************/

#include "demo.hpp"
#include "pmapi.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

int     mode = -1,snowlevel = -1;
ibool   useVBE = true,useSNAP = true;

/* Path to find sample program data in standard SciTech source tree */

#ifdef  __UNIX__
#define MGL_INIT_PATH   "../../../"
#else
#define MGL_INIT_PATH   "..\\..\\..\\"
#endif

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
REMARKS:
Handles fatal errors
****************************************************************************/
void initFatalError(void)
{
    MGL_fatalError(MGL_errorMsg(MGL_result()));
}

/****************************************************************************
REMARKS:
Waits for an event and returns the ASCII code of the key that was pressed.
****************************************************************************/
int waitEvent(void)
{
    event_t evt;
    EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT | EVT_MOUSEDOWN);
    if (!(evt.what & EVT_MOUSEDOWN))
        return EVT_asciiCode(evt.message);
    return 0;
}

/****************************************************************************
REMARKS:
This function disables any of the display drivers as necessary to
enable and disable DirectDraw or SNAP.
****************************************************************************/
static void disableDrivers(void)
{
    MGL_enableAllDrivers();
    if (!useVBE)
        MGL_disableDriver(MGL_VBENAME);
    if (!useSNAP)
        MGL_disableDriver(MGL_SNAPNAME);
}

/****************************************************************************
PARAMETERS:
xRes    - X resolution for the mode to initialise
yRes    - Y resolution for the mode to initialise
bits    - Color depth for the mode to initialise

RETURNS:
Pointer to the MGL device context to use for the application

REMARKS:
Initialises the MGL and creates an appropriate display device context to
be used by the GUI. This creates and apropriate device context depending on
the system being compile for, and should be the only place where system
specific code is required.
****************************************************************************/
void initGraphics(
    int xRes,
    int yRes,
    int bits,
    MGL_HWND windowedWindow)
{
    // Initialise the MGL and attempt to set up 640x480x256 graphics as
    // the default video mode.
    if (MGL_init(MGL_INIT_PATH,NULL)== 0)
        initFatalError();
    disableDrivers();
    if (!windowedWindow) {
        if ((mode = MGL_findMode(xRes,yRes,bits)) == -1)
            initFatalError();
        }
}

/****************************************************************************
RETURNS:
Pointer to the MGL device context to use for the application

REMARKS:
Initialises the MGL and creates an appropriate display device context to be
used by the GUI. This creates and apropriate device context depending on the
system being compile for, and should be the only place where system specific
code is required.
****************************************************************************/
MGLDC *createDisplayDC(
    MGL_HWND windowedWindow)
{
    MGLDC   *dc;

    // Start the specified video mode and create the display DC
    if (!windowedWindow) {
        if ((dc = MGL_createDisplayDC(mode,MGL_availablePages(mode),MGL_DEFAULT_REFRESH)) == NULL)
            initFatalError();
        }
    else {
        if ((dc = MGL_createWindowedDC(windowedWindow)) == NULL)
            initFatalError();
        }
    MGL_makeCurrentDC(dc);
    if (snowlevel != -1)
        MGL_setPaletteSnowLevel(dc,snowlevel);
    return dc;
}

/****************************************************************************
PARAMETERS:
xRes            - X resolution for the mode to initialise
yRes            - Y resolution for the mode to initialise
bits            - Color depth for the mode to initialise
windowedWindow  - Handle to window to use if running in a window

REMARKS:
Initialised and runs the demo in the specified resolution and color depth.
****************************************************************************/
void runDemo(
    int xRes,
    int yRes,
    int bits,
    MGL_HWND windowedWindow)
{
    initGraphics(xRes,yRes,bits,windowedWindow);

    // Keep re-starting the application while we recieve the cmRestart
    // command code. This allows the application to change video modes
    // on the fly.
    uint endState = cmRestart;
    while (endState == cmRestart) {
        MGLDevCtx dc(createDisplayDC(windowedWindow));
        Demo *demo = new Demo(dc);
        endState = demo->run();
        delete demo;
        }
    MGL_exit();
}
#ifdef __CONSOLE__

#include <ctype.h>
#include "cmdline.h"

/****************************************************************************
REMARKS:
Provide usage information about the program.
****************************************************************************/
void help(void)
{
    int mode,xRes,yRes,bits;

    disableDrivers();
    if (MGL_init(MGL_INIT_PATH,NULL) == 0)
        initFatalError();
    printf("Usage: demo [-novbe -nonuc -ph] [<xRes> <yRes> <bitsPerPixel>]\n\n");
    printf("    -novbe  Don't use VESA VBE driver.\n");
    printf("    -nonuc  Don't use SciTech SNAP Graphics driver.\n");
    printf("    -p<arg> Set the palette snow level factor (defaults to 256).\n");
    printf("    -h      Provide this usage information.\n");
    printf("Press a key for list of video modes.");
    fflush(stdout);
    if (waitEvent() != ASCII_esc) {
        printf("\n\nAvailable modes are:\n");
        for (mode = 0; MGL_modeResolution(mode,&xRes,&yRes,&bits); mode++) {
            printf("  %4d x %4d x %2d %3d page (%s)\n",
                xRes,yRes,bits,
                MGL_availablePages(mode),
                MGL_modeDriverName(mode));
            if (mode != 0 && ((mode % 20) == 0)) {
                printf("Press a key to continue ... ");
                fflush(stdout);
                if (waitEvent() == ASCII_esc)
                    break;
                printf("\n");
                }
            }
        }
    MGL_exit();
    exit(1);
}

/****************************************************************************
PARAMETERS:
argc    - Number of command line arguments
argv    - Array of command line arguments

REMARKS:
Parses the command line.
****************************************************************************/
void parseArguments(
    int argc,
    char *argv[])
{
    int     option;
    char    *argument;

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"N:n:P:p:Hh",&argument);
        if (isascii(option))
            option = tolower(option);
        switch (option) {
            case 'n':
                if (stricmp(argument,"ovbe") == 0)
                    useVBE = false;
                if (stricmp(argument,"onuc") == 0)
                    useSNAP = false;
                break;
            case 'p':
                snowlevel = atoi(argument);
                break;
            case ALLDONE:
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
PARAMETERS:
argc    - Number of command line arguments
argv    - Array of command line arguments

REMARKS:
Main program entry point for console mode version.
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    parseArguments(argc,argv);
    if ((argc - nextargv) >= 3) {
        int xRes = atoi(argv[nextargv]);
        int yRes = atoi(argv[nextargv+1]);
        int bits = atoi(argv[nextargv+2]);
        runDemo(xRes,yRes,bits,NULL);
        }
    else
        runDemo(640,480,8,NULL);
    return 0;
}

#elif defined(__WINDOWS__)
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "demo.rh"

#define MAX_MODES 100

struct {
    int xRes;
    int yRes;
    int bits;
    } modeList[MAX_MODES];    /* List of modes to select    */

/****************************************************************************
PARAMETERS:
hWndCenter  - Window to center
parent      - Handle for parent window
repaint     - True if window should be re-painted

REMARKS:
Centers the specified window within the bounds of the specified parent
window. If the parent window is NULL, then we center it using the desktop
window.
****************************************************************************/
void CenterWindow(
    HWND hWndCenter,
    HWND parent,
    BOOL repaint)
{
    HWND    hWndParent = (parent ? parent : GetDesktopWindow());
    RECT    RectParent;
    RECT    RectCenter;
    int     CenterX,CenterY,Height,Width;

    GetWindowRect(hWndParent, &RectParent);
    GetWindowRect(hWndCenter, &RectCenter);
    Width = (RectCenter.right - RectCenter.left);
    Height = (RectCenter.bottom - RectCenter.top);
    CenterX = ((RectParent.right - RectParent.left) - Width) / 2;
    CenterY = ((RectParent.bottom - RectParent.top) - Height) / 2;
    if ((CenterX < 0) || (CenterY < 0)) {
        /* The Center Window is smaller than the parent window. */
        if (hWndParent != GetDesktopWindow()) {
            /* If the parent window is not the desktop use the desktop size. */
            CenterX = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
            CenterY = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;
            }
        CenterX = (CenterX < 0) ? 0: CenterX;
        CenterY = (CenterY < 0) ? 0: CenterY;
        }
    else {
        CenterX += RectParent.left;
        CenterY += RectParent.top;
        }

    /* Copy the values into RectCenter */
    RectCenter.left = CenterX;
    RectCenter.right = CenterX + Width;
    RectCenter.top = CenterY;
    RectCenter.bottom = CenterY + Height;

    /* Move the window to the new location */
    MoveWindow(hWndCenter, RectCenter.left, RectCenter.top,
            (RectCenter.right - RectCenter.left),
            (RectCenter.bottom - RectCenter.top), repaint);
}

/****************************************************************************
hwnd    - Handle to dialog box window

REMARKS:
Refreshes the list of available video modes in the video mode list box
given the newly selected pixel depth.
****************************************************************************/
void RefreshModeList(
    HWND hwnd)
{
    char    buf[PM_MAX_PATH];
    int     i,mode,xRes,yRes,bits,selectBits;
    HWND    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
    ibool   haveMode = false;

    /* Find out what color depth is selected */
    if (IsDlgButtonChecked(hwnd,IDC_WINDOWED))      selectBits = 0;
    else if (IsDlgButtonChecked(hwnd,IDC_8BIT))     selectBits = 8;
    else if (IsDlgButtonChecked(hwnd,IDC_16BIT))    selectBits = 16;
    else if (IsDlgButtonChecked(hwnd,IDC_24BIT))    selectBits = 24;
    else if (IsDlgButtonChecked(hwnd,IDC_32BIT))    selectBits = 32;

    /* Initialise the MGL so we can enumerate the available modes */
    if (MGL_init(MGL_INIT_PATH,NULL) == 0)
        initFatalError();
    disableDrivers();

    /* Build the list of available modes */
    SendMessage(hwndLst,LB_RESETCONTENT,0,0);
    if (selectBits == 0) {
        /* Populate with common window sizes */
        modeList[0].xRes = 320; modeList[0].yRes = 240; modeList[0].bits = 0;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"320 x 240 window");
        modeList[1].xRes = 400; modeList[1].yRes = 300; modeList[1].bits = 0;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"400 x 300 window");
        modeList[2].xRes = 512; modeList[2].yRes = 384; modeList[2].bits = 0;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"512 x 384 window");
        modeList[3].xRes = 640; modeList[3].yRes = 480; modeList[3].bits = 0;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"640 x 480 window");
        modeList[4].xRes = 800; modeList[4].yRes = 600; modeList[4].bits = 0;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"800 x 600 window");
        haveMode = true;
        }
    else {
        for (i = mode = 0; MGL_modeResolution(mode,&xRes,&yRes,&bits); mode++) {
            /* There will only ever be either 15bpp or 16bpp modes in Windows */
            if (bits == 15)
                bits = 16;
            if (bits != selectBits)
                continue;
            sprintf(buf,"%4d x %4d x %2d %3d page (%s)",
                xRes,yRes,bits,
                MGL_availablePages(mode),
                MGL_modeDriverName(mode));
            SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)buf);
            if (i == MAX_MODES)
                break;
            haveMode = true;
            modeList[i].xRes = xRes;
            modeList[i].yRes = yRes;
            modeList[i].bits = bits;
            i++;
            }
        }
    SendMessage(hwndLst,LB_SETCURSEL,0,0);

    /* Enable the window if we found any valid modes */
    if (!haveMode) {
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"No available modes");
        EnableWindow(hwndLst,FALSE);
        }
    else
        EnableWindow(hwndLst,TRUE);

    /* Clean up the MGL */
    MGL_exit();
}

/****************************************************************************
REMARKS:
Create the window to run the demo in.
****************************************************************************/
HWND CreateWindowedWindow(
    int xRes,
    int yRes)
{
    static ibool    classRegistered = false;
    static char     winClassName[] = "GameFrameworkWindow";
    WNDCLASS        cls;
    HWND            hwnd;

    /* Create a Window class for the fullscreen window in here, since we need
     * to register one that will do all our event handling for us.
     */
    if (!classRegistered) {
        cls.hCursor         = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon           = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));
        cls.lpszMenuName    = NULL;
        cls.lpszClassName   = winClassName;
        cls.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
        cls.hInstance       = GetModuleHandle(NULL);
        cls.style           = CS_DBLCLKS;
        cls.lpfnWndProc     = DefWindowProc;
        cls.cbWndExtra      = 0;
        cls.cbClsExtra      = 0;
        if (!RegisterClass(&cls))
            MGL_fatalError("Unable to register Window Class!");
        classRegistered = true;
        }
    hwnd = CreateWindow(winClassName,"MGL Sample Program",
        WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION,
        CW_USEDEFAULT,0,xRes,yRes,NULL,NULL,GetModuleHandle(NULL),NULL);
    ShowWindow(hwnd,SW_SHOW);
    return hwnd;
}

/****************************************************************************
PARAMETERS:
hwnd    - Handle to dialog box window

REMARKS:
Refreshes the list of available video modes in the video mode list box
given the newly selected pixel depth.
****************************************************************************/
BOOL WINAPI MainDlgProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    HWND    hwndLst,hwndWindowed;
    int     mode;

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CANCEL:
                    EndDialog(hwnd,IDC_CANCEL);
                    break;
                case IDC_MODELIST:
                    if (HIWORD(wParam) != LBN_DBLCLK)
                        break;
                case IDC_OK:
                    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
                    if (SendMessage(hwndLst,LB_GETCOUNT,0,0)) {
                        mode = SendMessage(hwndLst,LB_GETCURSEL,0,0);
                        ShowWindow(hwnd,false);
                        if (modeList[mode].bits == 0) {
                            hwndWindowed = CreateWindowedWindow(modeList[mode].xRes,modeList[mode].yRes);
                            runDemo(modeList[mode].xRes,modeList[mode].yRes,modeList[mode].bits,hwndWindowed);
                            DestroyWindow(hwndWindowed);
                            }
                        else
                            runDemo(modeList[mode].xRes,modeList[mode].yRes,modeList[mode].bits,NULL);
                        ShowWindow(hwnd,true);
                        }
                    break;
                case IDC_8BIT:
                case IDC_16BIT:
                case IDC_24BIT:
                case IDC_32BIT:
                case IDC_WINDOWED:
                    CheckDlgButton(hwnd,IDC_8BIT,LOWORD(wParam) == IDC_8BIT);
                    CheckDlgButton(hwnd,IDC_16BIT,LOWORD(wParam) == IDC_16BIT);
                    CheckDlgButton(hwnd,IDC_24BIT,LOWORD(wParam) == IDC_24BIT);
                    CheckDlgButton(hwnd,IDC_32BIT,LOWORD(wParam) == IDC_32BIT);
                    CheckDlgButton(hwnd,IDC_WINDOWED,LOWORD(wParam) == IDC_WINDOWED);
                    RefreshModeList(hwnd);
                    break;
                }
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd,NULL,FALSE);
            CheckDlgButton(hwnd,IDC_8BIT,TRUE);
            RefreshModeList(hwnd);
            return TRUE;
        }
    (void)lParam;
    return FALSE;
}

/****************************************************************************
PARAMETERS:
hInst       - Instance handle for the application
hPrev       - Previous instance handle
szCmdLine   - Command line for the application
sw          - Command to pass to ShowWindow

REMARKS:
Main program entry point for Win32 GUI versions of the MGL sample programs.
****************************************************************************/
int PASCAL WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrev,
    LPSTR szCmdLine,
    int sw)
{
    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    DialogBox(hInst,MAKEINTRESOURCE(IDD_MAINDLG),NULL,(DLGPROC)MainDlgProc);
    (void)hPrev;
    (void)szCmdLine;
    (void)sw;
    return 0;
}

#endif

