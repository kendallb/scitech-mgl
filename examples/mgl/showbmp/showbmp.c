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
* Description:  Program to demonstrate loading bitmaps of any size and
*               color depth directly into the display DC. Automatic color
*               conversion is done on the bitmap to convert it to the
*               current display DC's pixel format.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mgraph.h"
#include "pmapi.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

#define PROG_NAME   "showbmp"

/*---------------------------- Global Variables ---------------------------*/

static ibool    useSNAP = true;
static ibool    useVBE = true;
#ifdef __CONSOLE__
static int      xRes = 640;
static int      yRes = 480;
static int      bits = 8;
static int      refreshRate = MGL_DEFAULT_REFRESH;
#endif

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
REMARKS:
This function displays fatal error message and exits the app.
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

MGLDC *initGraphics(int xRes,int yRes,int bits,int refreshRate)
/****************************************************************************
*
* Function:     initGraphics
* Parameters:   modeNum - Video mode number to start
* Returns:      Pointer to the MGL device context to use for the application
*
* Description:  Initialises the MGL and creates an appropriate display
*               device context to be used by the GUI. This creates and
*               apropriate device context depending on the system being
*               compile for, and should be the only place where system
*               specific code is required.
*
****************************************************************************/
{
    MGLDC   *dc;
    int     mode,numPages;

    /* Start the MGL and create a display device context */
    if (!MGL_init(".",NULL))
        initFatalError();
    disableDrivers();
    if ((mode = MGL_findMode(xRes,yRes,bits)) == -1)
        initFatalError();
    numPages = MIN(MGL_availablePages(mode),2);
    if ((dc = MGL_createDisplayDC(mode,numPages,refreshRate)) == NULL)
        initFatalError();
    MGL_makeCurrentDC(dc);
    return dc;
}

void waitEvent(void)
/****************************************************************************
*
* Function:     waitEvent
*
* Description:  Waits for either a keyboard or mouse event before returning.
*
****************************************************************************/
{
    event_t evt;
    EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT | EVT_MOUSEDOWN);
}

void showBitmap(int xRes,int yRes,int bits,int refreshRate,char *bitmapName)
{
    MGLDC       *dc;

    /* Initialise the graphics mode */
    dc = initGraphics(xRes,yRes,bits,refreshRate);

    /* Load the bitmap into the display DC. In 8 bit modes the palette is
     * automatically handled by this routine for display DC's.
     */
    if (!MGL_loadBitmapIntoDC(dc,bitmapName,0,0,true))
        initFatalError();
    waitEvent();

    MGL_exit();
}

#ifdef __CONSOLE__

#include "cmdline.h"

void help(char *progName)
/****************************************************************************
*
* Function:     help
* Parameters:   progName    - Name of demo program
*
* Description:  Provide usage information about the program.
*
****************************************************************************/
{
    printf("Usage: %s [-novbe -nonuc] [<xRes> <yRes> <bitsPerPixel> [-r<Hz>]]\n\n", progName);
    printf("    -novbe  Don't use VESA VBE driver.\n");
    printf("    -nonuc  Don't use SciTech SNAP Graphics driver.\n");
    printf("    -r<Hz>  Use a specific refresh rate (- for interlaced).\n");
    printf("    -h      Provide this usage information.\n\n");

    exit(1);
}

void parseArguments(int argc,char *argv[],char *progName)
/****************************************************************************
*
* Function:     parseArguments
* Parameters:   argc,argv   - Command line arguments from main()
*
* Description:  Parse the initial command line before starting the demo.
*
****************************************************************************/
{
    int     option;
    char    *argument;

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"N:n:R:r:Hh",&argument);
        if (isascii(option))
            option = tolower(option);
        switch (option) {
            case 'n':
                if (stricmp(argument,"ovbe") == 0)
                    useVBE = false;
                if (stricmp(argument,"onuc") == 0)
                    useSNAP = false;
                break;
            case 'r':
                refreshRate = atoi(argument);
                if (refreshRate < 0)
                    refreshRate = (-refreshRate) | MGL_INTERLACED_MODE;
                break;
            case ALLDONE:
            case PARAMETER:
                break;
            case 'h':
            case INVALID:
            default:
                help(progName);
            }
        } while (option != ALLDONE && option != PARAMETER);
}

int main(int argc,char *argv[])
{
    char    bitmapName[PM_MAX_PATH];

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Parse command line */
    parseArguments(argc,argv,PROG_NAME);
    if (nextargv <= argc-3) {
        xRes = atoi(argv[nextargv]);
        yRes = atoi(argv[nextargv+1]);
        bits = atoi(argv[nextargv+2]);
        }

    /* Initialise desired graphics mode */
    printf("Bitmap to load: ");
    gets(bitmapName);
    showBitmap(xRes,yRes,bits,refreshRate,bitmapName);
    return 0;
}

#elif defined(__OS2_PM__)

// TODO: Add code to support OS/2 Presentation Manager!

#elif   defined(__WINDOWS__)
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include "showbmp.rh"

struct {
    int xRes;
    int yRes;
    int bits;
    } modeList[MAX_MODES];    /* List of modes to select    */

static char    szAppFilter[] = "Bitmaps\0*.bmp;*.dib\0";
static char    bitmapName[PM_MAX_PATH] = "";

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

ibool OpenBitmapFile(HWND hwnd)
/****************************************************************************
*
* Function:     OpenBitmapFile
* Parameters:   hwnd    - Handle to parent window
* Returns:      True for valid bitmap name, false otherwise
*
* Description:  Open the bitmap file and load it into the memory DC.
*
****************************************************************************/
{
    OPENFILENAME    ofn;

    bitmapName[0] = 0;

    /* Prompt user for file to open */
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szAppFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = bitmapName;
    ofn.nMaxFile = sizeof(bitmapName);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    return GetOpenFileName(&ofn);
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
                case IDC_SELECT_BITMAP:
                    OpenBitmapFile(hwnd);
                    break;
                case IDC_MODELIST:
                    if (HIWORD(wParam) != LBN_DBLCLK)
                        break;
                case IDC_OK:
                    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
                    if (SendMessage(hwndLst,LB_GETCOUNT,0,0)) {
                        if (bitmapName[0] == 0 && !OpenBitmapFile(hwnd))
                            break;
                        mode = SendMessage(hwndLst,LB_GETCURSEL,0,0);
                        xres = modeList[mode].xRes;
                        yres = modeList[mode].yRes;
                        bits = modeList[mode].bits;
                        showBitmap(xres,yres,bits,MGL_DEFAULT_REFRESH,bitmapName);
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
