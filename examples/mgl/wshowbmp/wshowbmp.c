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
* Environment:  Win32
*
* Description:  Windows version of SHOWBMP sample app that runs in a window
*               on the Windows desktop. Will load a bitmap of any format
*               into a memory DC with the same pixel depths as the main
*               Windows video mode. Hence if you load a 24 bit image while
*               in a HiColor or TrueColor video mode, the image will display
*               with maximum color resolution. In 8 bit modes the image will
*               be dithered if necessary.
*
*
****************************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <mmsystem.h>
#include "mgraph.h"
#include "wshowbmp.rh"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*--------------------------- Global Variables ----------------------------*/

char        szAppName[] = "SHOWBMP: MGL Bitmap Loading Demo";
char        szAppFilter[]="Bitmaps\0*.bmp;*.dib\0";
HINSTANCE   hInstApp;
HWND        hwndApp;
BOOL        fAppActive;
BOOL        haveTrueColor;
ibool        stretchBlt = false;
ibool        includeStatic = true;
ibool       animatePalette = false;
MGLDC       *winDC = NULL,*memDC = NULL;
char        bitmapName[256] = {0};

/*------------------------------ Implementation ---------------------------*/

void LoadBitmapPalette(MGLDC *winDC,MGLDC *memDC)
/****************************************************************************
*
* Function:     LoadBitmapPalette
* Parameters:   winDC   - Window DC to load palette into
*               memDC   - Memory DC to get palette values from
* Returns:      Pointer to valid memory DC with bitmap loaded
*
* Description:  Load the palette from the memory DC bitmap into the Window DC
*               and realize it to make it the hardware palette. Note that
*               we also realize the palette for the loaded bitmap in the
*               memory DC to re-build the DIB's color table when we switch
*               to and from SYSPAL_NOSTATIC mode.
*
****************************************************************************/
{
    palette_t       pal[256];

    MGL_getPalette(memDC,pal,MGL_getPaletteSize(memDC),0);
    MGL_setPalette(winDC,pal,MGL_getPaletteSize(winDC),0);
    MGL_realizePalette(memDC,MGL_getPaletteSize(memDC),0,false);
    MGL_realizePalette(winDC,MGL_getPaletteSize(winDC),0,false);
}

void GetMemoryBitmapDepth(MGLDC *dc,int *bits,pixel_format_t *pf)
/****************************************************************************
*
* Function:     GetMemoryBitmapDepth
* Parameters:   dc      - Window DC
*               bits    - Place to store pixel depth
*               pf      - Place to store pixel format
* Returns:      Pointer to valid memory DC with bitmap loaded
*
* Description:  Under Windows 3.1, the only bitmaps we can blt to the display
*               are 8 bit bitmaps. Under Windows NT and '95 we can blt
*               bitmaps of any color depth, so we create our memory DC with
*               the same depth and format as the current Windows display mode.
*
****************************************************************************/
{
    if (!haveTrueColor)
        *bits = 8;
    else {
        *bits = MGL_getBitsPerPixel(dc);
        MGL_getPixelFormat(dc,pf);
        }
}

MGLDC *LoadBitmapIntoMemDC(MGLDC *dc,char *bitmapName)
/****************************************************************************
*
* Function:     LoadBitmapIntoMemDC
* Parameters:   dc          - Display dc
*               bitmapName  - Name of bitmap file to load
* Returns:      Pointer to valid memory DC with bitmap loaded
*
* Description:  Loads the specified bitmap into a memory DC with the
*               same dimensions as the bitmap on disk, but with the same
*               pixel depth and format used by the display DC (for maximum
*               blt performance). The MGL automatically handles pixel
*               format conversion for us when we load the bitmap into our
*               memory DC, and the bitmap's palette is loaded into the
*               memory DC's palette.
*
****************************************************************************/
{
    int             width,height,bits,bitmapBits;
    pixel_format_t  pf;
    MGLDC           *memDC;

    /* Get dimensions of bitmap image on disk */
    if (!MGL_getBitmapSize(bitmapName,&width,&height,&bitmapBits,&pf))
        MGL_fatalError("Cant locate bitmap file!");

    /* Get bitmap depth and pixel format for memory DC */
    GetMemoryBitmapDepth(dc,&bits,&pf);

    /* Create the memory DC and load the bitmap file into it */
    if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
        MGL_fatalError("Not enough memory to load bitmap!");
    if (!MGL_loadBitmapIntoDC(memDC,bitmapName,0,0,true))
        MGL_fatalError("Cant locate bitmap file!");

    /* Load bitmap palette into Window DC and realize it */
    LoadBitmapPalette(winDC,memDC);
    return memDC;
}

void InitMGL(void)
/****************************************************************************
*
* Function:     InitMGL
*
* Description:  Initialise the MGL and load all bitmap resources required by
*               the application. This is where the application should allocate
*               space for all MGL memory DC's used for storing sprites etc
*               and load the sprites from disk.
*
****************************************************************************/
{
    /* Initialise the MGL with the current directory path */
    MGL_init(".",NULL);
}

void CreateMGLDeviceContexts(HWND hwnd)
/****************************************************************************
*
* Function:     CreateMGLDeviceContexts
* Parameters:   hwnd    - Handle to Window used for output
*
* Description:  Sets up the MGL for output to the client region of the
*               specified window. This function will destroy any previously
*               existing MGL device contexts, and will re-build all the
*               DC's for the Window. Hence this routine is always called
*               from the Windows WM_SIZE message handler, which will always
*               be called before the first WM_PAINT message is received,
*               allowing the MGL device contexts to be created with the
*               appropriate size information.
*
****************************************************************************/
{
    /* Destroy the previous Window DC if it existed */
    if (winDC)
        MGL_destroyDC(winDC);

    /* Create the main MGL Windowed DC for interfacing our main window */
    if ((winDC = MGL_createWindowedDC(hwnd)) == NULL)
        MGL_fatalError("Unable to create Windowed DC!");

    /* Re-load the bitmap palette if it existed */
    if (memDC)
        LoadBitmapPalette(winDC,memDC);
}

BOOL CALLBACK AppAbout(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
/****************************************************************************
*
* Function:     AppAbout
*
* Description:  Dialog procedure for About dialog box.
*
****************************************************************************/
{
    char    format[80],buf[80];

    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
                EndDialog(hwnd,TRUE);
            break;
        case WM_INITDIALOG:
            GetDlgItemText(hwnd,IDC_VERSION_STR,format,sizeof(format));
            sprintf(buf,format,MGL_VERSION_STR);
            SetDlgItemText(hwnd,IDC_VERSION_STR,buf);
            return TRUE;
        }
    return FALSE;
}

void OpenBitmapFile(HWND hwnd)
/****************************************************************************
*
* Function:     OpenBitmapFile
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

    /* Load the bitmap into a memory DC */
    if (GetOpenFileName(&ofn)) {
        if (memDC)
            MGL_destroyDC(memDC);
        memDC = LoadBitmapIntoMemDC(winDC, bitmapName);
        InvalidateRect(hwnd, NULL, TRUE);
        }
}

void CheckMenuItems(HWND hwnd)
/****************************************************************************
*
* Function:     CheckMenuItems
*
* Description:  Set the proper check marks for the stretch menu.
*
****************************************************************************/
{
    HMENU hMenu = GetMenu(hwnd);
    CheckMenuItem(hMenu, MENU_STRETCH_TOWINDOW, stretchBlt ? MF_CHECKED : MF_UNCHECKED);
    if (MGL_getBitsPerPixel(winDC) == 8) {
        CheckMenuItem(hMenu, MENU_STATIC_COLORS, includeStatic ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, MENU_ANIMATE_PALETTE, animatePalette ? MF_CHECKED : MF_UNCHECKED);
        }
    else {
        EnableMenuItem(hMenu, MENU_STATIC_COLORS, MF_DISABLED | MF_GRAYED);
        EnableMenuItem(hMenu, MENU_ANIMATE_PALETTE, MF_DISABLED | MF_GRAYED);
        }
}

void AppActivate(BOOL fActive)
/****************************************************************************
*
* Function:     AppActivate
* Parameters:   fActive - True if app is activating
*
* Description:  If the application is activating, then swap the system
*               into SYSPAL_NOSTATIC mode so that our palettes will display
*               correctly.
*
****************************************************************************/
{
    HDC hdc = GetDC(NULL);

    if (fActive && (GetSystemPaletteUse(hdc) == SYSPAL_STATIC)) {
        /* Switch to SYSPAL_NOSTATIC */
        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
        }
    else if (!fActive) {
        /* Always switch back to SYSPAL_STATIC */
        SetSystemPaletteUse(hdc, SYSPAL_STATIC);
        }
    ReleaseDC(NULL,hdc);
}

LONG CALLBACK AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
/****************************************************************************
*
* Function:     AppWndProc
*
* Description:  Main window proc for application.
*
****************************************************************************/
{
    PAINTSTRUCT ps;
    HDC         hdc;

    switch (msg) {
        case WM_ACTIVATEAPP:
            fAppActive = (BOOL)wParam;
            if (!includeStatic) {
                AppActivate(fAppActive);
                }
            break;
        case WM_COMMAND:
            switch (wParam) {
                case MENU_OPEN:
                    OpenBitmapFile(hwnd);
                    break;
                case MENU_ABOUT:
                    DialogBox(hInstApp,"AppAbout",hwnd,(DLGPROC)AppAbout);
                    break;
                case MENU_EXIT:
                    PostMessage(hwnd,WM_CLOSE,0,0L);
                    break;
                case MENU_STRETCH_TOWINDOW:
                    stretchBlt = !stretchBlt;
                    CheckMenuItems(hwnd);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case MENU_STATIC_COLORS:
                    if (includeStatic) {
                        /* Enable use of static colors */
                        includeStatic = false;
                        AppActivate(true);
                        }
                    else {
                        /* Remap the system colors to normal */
                        includeStatic = true;
                        AppActivate(false);
                        }
                    CheckMenuItems(hwnd);

                    /* Rebuild the MGL device context to re-build the
                     * palette for the loaded bitmap.
                     */
                    CreateMGLDeviceContexts(hwnd);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case MENU_ANIMATE_PALETTE:
                    animatePalette = !animatePalette;
                    CheckMenuItems(hwnd);
                    break;
                }
            return 0;
        case WM_CREATE:
            CreateMGLDeviceContexts(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PALETTECHANGED:
            if ((HWND)wParam == hwnd)
                break;
            /* Fall through to WM_QUERYNEWPALETTE */
        case WM_QUERYNEWPALETTE:
            if (winDC && memDC) {
                LoadBitmapPalette(winDC,memDC);
                InvalidateRect(hwnd,NULL,TRUE);
                return 1;
                }
            return 0;
        case WM_PAINT:
            if (!winDC)
                break;
            hdc = BeginPaint(hwnd,&ps);
            PatBlt(hdc,0,0,MGL_sizex(winDC)+1,MGL_sizey(winDC)+1,WHITENESS);
            if (memDC) {
                if (stretchBlt) {
                    MGL_stretchBltCoord(winDC,memDC,
                        0,0,MGL_sizex(memDC)+1,MGL_sizey(memDC)+1,
                        0,0,MGL_sizex(winDC)+1,MGL_sizey(winDC)+1,MGL_REPLACE_MODE);
                    }
                else {
                    MGL_bitBltCoord(winDC,memDC,
                        0,0,MGL_sizex(memDC)+1,MGL_sizey(memDC)+1,
                        0,0,MGL_REPLACE_MODE);
                    }
                }
            EndPaint(hwnd,&ps);
            return 0;
        }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

BOOL DetectTrueColor(void)
/****************************************************************************
*
* Function:     HaveTrueColor
* Returns       True if we can support TrueColor memory bitmaps. This
*               is possible when running Win32 code on Windows '95 or
*               Windows NT 3.5. For Windows 3.1 or any code running
*               Watcom C++ Win386 code, we can only support 8 bit memory
*               DC's.
*
****************************************************************************/
{
#ifdef  __WINDOWS32__
    int verMajor = GetVersion() & 0xFF;
    int verMinor = (GetVersion() >> 8) & 0xFF;
    if (verMajor > 3)
        return TRUE;
    if (verMinor > 0xA)
        return TRUE;
    return FALSE;
#else
    return FALSE;
#endif
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    MSG     msg;
    DWORD   lastTime,newTime;

    /* Detect if we have support for TrueColor */
    haveTrueColor = DetectTrueColor();

    /* Save instance handle for dialog boxes */
    hInstApp = hInst;

    if (!hPrev) {
        /* Register a class for the main application window */
        WNDCLASS cls;
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,"AppIcon");
        cls.lpszMenuName   = "AppMenu";
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = NULL;
        cls.hInstance      = hInst;
        cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
        cls.lpfnWndProc    = (LPVOID)AppWndProc;
        cls.cbWndExtra     = 0;
        cls.cbClsExtra     = 0;
        if (!RegisterClass(&cls))
            return FALSE;
        }

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Initialise the MGL */
    InitMGL();

    /* Create the main window and display it */
    hwndApp = CreateWindow(szAppName,szAppName,WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,0,400,400,NULL,NULL,hInst,NULL);
    ShowWindow(hwndApp,sw);
    CheckMenuItems(hwndApp);

    /* Polling messages from event queue */
    lastTime = timeGetTime();
    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        else if (animatePalette) {
            newTime = timeGetTime();
            if ((newTime - lastTime) > 100) {
                int palsize;

                /* Rotate the hardware palette */
                palsize = MGL_getPaletteSize(winDC);
                MGL_rotatePalette(winDC,palsize-2,1,MGL_ROTATE_UP);
                MGL_realizePalette(winDC,palsize,0,true);
                lastTime = newTime;
                }
            }
        }

    /* Clean up the MGL before exit */
    MGL_exit();
    return msg.wParam;
}

