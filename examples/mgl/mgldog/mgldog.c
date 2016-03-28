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
* Language:     ANSI C
* Environment:  Full screen environment
*
* Description:  Program to demonstrate solid and transparent bitBlt code.
*
*               We also set up a suspend application callback so that the
*               user can switch between our application and the normal
*               GDI desktop. The MGL takes care of most of the work, and
*               all we need to do is save and retore the bitmap for the
*               entire screen.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mgraph.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

#define PROG_NAME   "mgldog"

ibool   useVBE = true;
ibool   useSNAP = true;

/*------------------------------ Implementation ---------------------------*/

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
Handles a fatal error condition during initialisation.
****************************************************************************/
static void initFatalError(void)
{
    char    buf[80];
    sprintf(buf,"Graphics error: %s\n",MGL_errorMsg(MGL_result()));
    MGL_fatalError(buf);
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
dc      - Active display DC to save/restore state from
flags   - Flags indicating what to do

RETURNS:
Switch status flag.

REMARKS:
Restores the display to a clear image when the application is restored.
Under DirectX we don't get a chance to try restoring our data, so we have
to simply restore back to decent state when the reactivate message comes
along.
****************************************************************************/
int _ASMAPI doSuspendApp(
    MGLDC *dc,
    int flags)
{
    if (flags == MGL_REACTIVATE) {
        MGLDC *oldDC = MGL_makeCurrentDC(dc);
        MGL_clearDevice();
        MGL_makeCurrentDC(oldDC);
        }
    return MGL_SUSPEND_APP;
}

/****************************************************************************
PARAMETERS:
xRes    - X resolution of mode to use
yRes    - Y resolution of mode to use
bits    - Color depth of mode to use

RETURNS:
Pointer to the MGL device context to use for the application

REMARKS:
Initialises the MGL and creates an appropriate display device context to be
used by the program.
****************************************************************************/
MGLDC *initGraphics(
    int xRes,
    int yRes,
    int bits)
{
    MGLDC   *dc;

    /* Start the MGL and create a display device context */
    dc = MGL_quickInit(xRes,yRes,bits,1);

    /* Register our suspend application callback */
    MGL_setSuspendAppCallback(doSuspendApp);

    /* Turn off identity palette checking for maximum speed */
    MGL_checkIdentityPalette(false);
    return dc;
}

/****************************************************************************
PARAMETERS:
dc          - Display dc
bitmapName  - Name of bitmap file to load

RETURNS:
Pointer to valid memory DC with bitmap loaded

REMARKS:
Loads the specified bitmap into a memory DC with the same dimensions as the
bitmap on disk, but with the same pixel depth and format used by the display
DC (for maximum blt performance). The MGL automatically handles pixel
format conversion for us when we load the bitmap into our memory DC.
****************************************************************************/
MGLDC *loadBitmapIntoMemDC(
    MGLDC *dc,
    char *bitmapName)
{
    palette_t       pal[256];
    int             width,height,bitsPerPixel;
    pixel_format_t  pf;
    MGLDC           *memDC;

    /* Get dimensions of bitmap image on disk */
    if (!MGL_getBitmapSize(bitmapName,&width,&height,&bitsPerPixel,&pf))
        initFatalError();

    /* Load into Memory DC with same pixel depth and format as the display */
    bitsPerPixel = MGL_getBitsPerPixel(dc);
    MGL_getPixelFormat(dc,&pf);

    /* Create the memory DC and load the bitmap file into it */
    if ((memDC = MGL_createMemoryDC(width,height,bitsPerPixel,&pf)) == NULL)
        MGL_fatalError("Not enough memory to load bitmap!\n");
    if (!MGL_loadBitmapIntoDC(memDC,bitmapName,0,0,true))
        initFatalError();

    /* If this is an 8 bits per pixel video mode, then set the hardware
     * palette to that used by the bitmap we just loaded
     */
    if (MGL_getBitsPerPixel(dc) == 8) {
        MGL_getPalette(memDC,pal,MGL_getPaletteSize(memDC),0);
        MGL_setPalette(dc,pal,MGL_getPaletteSize(memDC),0);
        MGL_realizePalette(dc,MGL_getPaletteSize(memDC),0,true);
        }
    return memDC;
}

/****************************************************************************
PARAMETERS:
xRes    - X resolution of mode to use
yRes    - Y resolution of mode to use
bits    - Color depth of mode to use

REMARKS:
Performs the sprite animation in the specified full screen video mode.
****************************************************************************/
void doGraphics(
    int xRes,
    int yRes,
    int bits)
{
    MGLDC   *dc;
    MGLDC   *memDC;
    color_t transparent;
    int     x,y,width,height,done = false;
    event_t evt;

    /* Initialise the MGL */
    dc = initGraphics(xRes,yRes,bits);

    /* Load bitmap file into memory DC with same pixel depth as display */
    memDC = loadBitmapIntoMemDC(dc,"doggie2.bmp");

    /* Get transparent color from pixel (0,0) */
    MGL_makeCurrentDC(memDC);
    transparent = MGL_getPixelCoord(0,0);
    MGL_makeCurrentDC(dc);

    /* Draw the bitmap initially in the middle of the display */
    width = MGL_sizex(memDC)+1;
    height = MGL_sizey(memDC)+1;
    MGL_srcTransBltCoord(dc,memDC,0,0,width,height,(MGL_sizex(dc) - width)/2,
        (MGL_sizey(dc) - height)/2,transparent,MGL_REPLACE_MODE);

    /* Show the mouse cursor */
    if (MGL_getBitsPerPixel(dc) == 8)
        MS_setCursorColor(0xFF);
    MS_show();

    /* Continue drawing doggies while the mouse buttons are pressed */
    while (!done) {
        if (EVT_getNext(&evt,EVT_EVERYEVT)) {
            switch (evt.what) {
                case EVT_KEYDOWN:
                    if (EVT_asciiCode(evt.message) == 0x1B)
                        done = true;
                    break;
                case EVT_MOUSEMOVE:
                    x = evt.where_x - width/2;
                    y = evt.where_y - height/2;
                    MS_obscure();
                    if (evt.modifiers & EVT_LEFTBUT) {
                        MGL_srcTransBltCoord(dc,memDC,0,0,width,height,x,y,
                            transparent,MGL_REPLACE_MODE);
                        }
                    else if (evt.modifiers & EVT_RIGHTBUT) {
                        MGL_bitBltCoord(dc,memDC,0,0,width,height,x,y,
                            MGL_REPLACE_MODE);
                        }
                    MS_show();
                    break;
                }
            }
        }
    MGL_exit();
}

#ifdef __CONSOLE__

#include "cmdline.h"

void help(void)

/****************************************************************************
*
* Function:     help
*
* Description:  Provide usage information about the program.
*
****************************************************************************/
{
    int mode,xRes,yRes,bits;

    disableDrivers();
    if (MGL_init(".",NULL) == 0)
        initFatalError();
    printf("Usage: %s [-novbe -nonuc -ph] [<xRes> <yRes> <bitsPerPixel>]\n\n", PROG_NAME);
    printf("    -novbe  Don't use VESA VBE driver.\n");
    printf("    -nonuc  Don't use SciTech SNAP Graphics driver.\n");
    printf("    -h      Provide this usage information.\n\n");
    printf("Press a key for list of video modes.");
    fflush(stdout);
    if (waitEvent() != 0x1B) {
        printf("\n\nAvailable modes are:\n");
        for (mode = 0; MGL_modeResolution(mode,&xRes,&yRes,&bits); mode++) {
            printf("  %3d - %4d x %4d x %2d %3d page (%s)\n",
                mode,xRes,yRes,bits,
                MGL_availablePages(mode),
                MGL_modeDriverName(mode));
            if (mode != 0 && ((mode % 20) == 0)) {
                printf("Press a key to continue ... ");
                fflush(stdout);
                if (waitEvent() == 0x1B)
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
        option = getcmdopt(argc,argv,"N:n:Hh",&argument);
        if (isascii(option))
            option = tolower(option);
        switch (option) {
            case 'n':
                if (stricmp(argument,"ovbe") == 0)
                    useVBE = false;
                if (stricmp(argument,"onuc") == 0)
                    useSNAP = false;
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
Main console mode program entry point.
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Parse command line arguments */
    parseArguments(argc,argv);

    /* Run the demo */
    if (nextargv >= argc)
        doGraphics(640,480,8);
    else if (nextargv <= argc-3)
        doGraphics(atoi(argv[nextargv]),atoi(argv[nextargv+1]),atoi(argv[nextargv+2]));
    printf("Type '%s -h' for a list of available modes\n", PROG_NAME);
    return 0;
}

#elif defined(__OS2_PM__)

// TODO: Add code to support OS/2 Presentation Manager!

#elif defined(__WINDOWS__)
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "mgldog.rh"

struct {
    int xRes;
    int yRes;
    int bits;
    } modeList[MAX_MODES];    /* List of modes to select    */


void CenterWindow(HWND hWndCenter, HWND parent, BOOL repaint)
/****************************************************************************
*
* Function:     CenterWindow
* Parameters:   hWndCenter  - Window to center
*               parent      - Handle for parent window
*               repaint     - True if window should be re-painted
*
* Description:  Centers the specified window within the bounds of the
*               specified parent window. If the parent window is NULL, then
*               we center it using the Desktop window.
*
****************************************************************************/
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

void RefreshModeList(HWND hwnd)
/****************************************************************************
*
* Function:     RefreshModeList
* Parameters:   hwnd    - Handle to dialog box window
*
* Description:  Refreshes the list of available video modes in the video
*               mode list box given the newly selected pixel depth.
*
****************************************************************************/
{
    char    buf[MAX_STR];
    int     i,x,y,bits,selectBits,mode;
    HWND    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
    ibool   haveMode = false;

    /* Find out if we should use both SNAP and VBE or just one */
#if 0
    useSNAP = IsDlgButtonChecked(hwnd,IDC_USEDDRAW);
    useVBE = IsDlgButtonChecked(hwnd,IDC_USEWDIRECT);
    if (IsDlgButtonChecked(hwnd,IDC_USEBOTH))
        useSNAP = useVBE = true;
#endif

    if (IsDlgButtonChecked(hwnd,IDC_4BIT))          selectBits = 4;
    else if (IsDlgButtonChecked(hwnd,IDC_8BIT))     selectBits = 8;
    else if (IsDlgButtonChecked(hwnd,IDC_15BIT))    selectBits = 15;
    else if (IsDlgButtonChecked(hwnd,IDC_16BIT))    selectBits = 16;
    else if (IsDlgButtonChecked(hwnd,IDC_24BIT))    selectBits = 24;
    else if (IsDlgButtonChecked(hwnd,IDC_32BIT))    selectBits = 32;

    /* Initialise the MGL so we can enumerate the available modes */
    if (MGL_init(".",NULL) == 0)
        initFatalError();
    disableDrivers();

    SendMessage(hwndLst,LB_RESETCONTENT,0,0);
    for (i = mode = 0; MGL_modeResolution(mode,&x,&y,&bits); mode++) {
        if (bits != selectBits)
            continue;
        sprintf(buf,"%4d x %4d x %2d %2d page (%s)",
            x,y,bits,
            MGL_availablePages(mode),
            MGL_modeDriverName(mode));
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)buf);
        modeList[i].xRes = x;
        modeList[i].yRes = y;
        modeList[i].bits = bits;
        haveMode = true;
        i++;
        if (i > MAX_MODES)
            break;
        }
    SendMessage(hwndLst,LB_SETCURSEL,0,0);

    if (!haveMode) {
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"No available modes");
        EnableWindow(hwndLst,FALSE);
        }
    else {
        EnableWindow(hwndLst,TRUE);
        }

    /* Exit this instance of MGL now that we have a mode list */
    MGL_exit();
}

BOOL CALLBACK MainDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
/****************************************************************************
*
* Function:     MainDlgProc
*
* Description:  Dialog procedure for front end dialog box.
*
****************************************************************************/
{
    HWND    hwndLst;
    int     mode,xres,yres,bits;
    char    buf[MAX_STR];
    char    format[MAX_STR];

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
                        xres = modeList[mode].xRes;
                        yres = modeList[mode].yRes;
                        bits = modeList[mode].bits;
                        doGraphics(xres,yres,bits);
                        }
                    break;
                case IDC_4BIT:
                case IDC_8BIT:
                case IDC_15BIT:
                case IDC_16BIT:
                case IDC_24BIT:
                case IDC_32BIT:
                    CheckDlgButton(hwnd,IDC_4BIT,LOWORD(wParam) == IDC_4BIT);
                    CheckDlgButton(hwnd,IDC_8BIT,LOWORD(wParam) == IDC_8BIT);
                    CheckDlgButton(hwnd,IDC_15BIT,LOWORD(wParam) == IDC_15BIT);
                    CheckDlgButton(hwnd,IDC_16BIT,LOWORD(wParam) == IDC_16BIT);
                    CheckDlgButton(hwnd,IDC_24BIT,LOWORD(wParam) == IDC_24BIT);
                    CheckDlgButton(hwnd,IDC_32BIT,LOWORD(wParam) == IDC_32BIT);
                    RefreshModeList(hwnd);
                    break;
                case IDC_USEDDRAW:
                case IDC_USEWDIRECT:
                case IDC_USEBOTH:
                    CheckDlgButton(hwnd,IDC_USEDDRAW,LOWORD(wParam) == IDC_USEDDRAW);
                    CheckDlgButton(hwnd,IDC_USEWDIRECT,LOWORD(wParam) == IDC_USEWDIRECT);
                    CheckDlgButton(hwnd,IDC_USEBOTH,LOWORD(wParam) == IDC_USEBOTH);
                    RefreshModeList(hwnd);
                    break;
                }
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd,NULL,FALSE);
            GetDlgItemText(hwnd,IDC_VERSIONSTR,format,sizeof(format));
            sprintf(buf,format,"Win32");
            SetDlgItemText(hwnd,IDC_VERSIONSTR,buf);
            EnableWindow(GetDlgItem(hwnd,IDC_4BIT),FALSE);
            CheckDlgButton(hwnd,IDC_8BIT,TRUE);
            CheckDlgButton(hwnd,IDC_USEBOTH,TRUE);
            RefreshModeList(hwnd);
            return TRUE;
        }
    lParam = lParam;
    return FALSE;
}

ibool HaveWin95(void)
{
    int verMajor = GetVersion() & 0xFF;
    return (verMajor >= 4);
}

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
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
