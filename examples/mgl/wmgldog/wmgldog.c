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
* Description:  Windows version of MGLDOG sample app that runs in a window
*               on the Windows desktop. This shows how you can build a
*               normal windows application and use the MGL for high
*               performance rendering to a window.
*
****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "mgraph.h"
#include "wmgldog.rh"

/*--------------------------- Global Variables ----------------------------*/

typedef enum {
    DirectToScreen,
    MemoryBackBuffer,
    OffscreenBackBuffer,
    };

char        szAppName[] = "WMGLDOG: MGL Sprite Demo";
HINSTANCE   hInstApp;
HWND        hwndApp;
color_t     transparent;
int         sizex,sizey,width,height;
int         renderType = OffscreenBackBuffer;
MGLDC       *winDC = NULL,*dogDC = NULL,*backDC = NULL;

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
dc          - Display dc
bitmapName  - Name of bitmap file to load

RETURNS:
Pointer to valid memory DC with bitmap loaded

REMARKS:
Loads the specified bitmap into a memory DC with the same dimensions as the
bitmap on disk, but with the same pixel depth and format used by the display
DC (for maximum blt performance). The MGL automatically handles pixel format
conversion for us when we load the bitmap into our memory DC, and the
bitmap's palette is loaded into the memory DC's palette.
****************************************************************************/
MGLDC *LoadBitmapIntoMemDC(
    MGLDC *dc,
    char *bitmapName)
{
    int             width,height,bits;
    pixel_format_t  pf;
    MGLDC           *memDC;

    /* Get dimensions of bitmap image on disk */
    if (!MGL_getBitmapSize(bitmapName,&width,&height,&bits,&pf))
        MGL_fatalError("Cant locate bitmap file!");

    /* Get bitmap depth and pixel format for memory DC */
    bits = MGL_getBitsPerPixel(dc);
    MGL_getPixelFormat(dc,&pf);

    /* Create the memory DC and load the bitmap file into it */
    if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
        MGL_fatalError("Not enough memory to load bitmap!");
    if (!MGL_loadBitmapIntoDC(memDC,bitmapName,0,0,true))
        MGL_fatalError("Cant locate bitmap file!");
    return memDC;
}

/****************************************************************************
PARAMETERS:
dc  - Windowed DC to create memory back buffer for

REMARKS:
Creates a memory back buffer DC for a Windowed DC of the correct dimensions.
****************************************************************************/
MGLDC *CreateBackBufferDC(
    MGLDC *dc)
{
    int             bits;
    pixel_format_t  pf;
    palette_t       pal[256];
    MGLDC           *backDC = NULL;

    /* Create the main MGL Windowed DC for interfacing our main window */
    bits = MGL_getBitsPerPixel(dc);
    MGL_getPixelFormat(dc,&pf);
    sizex = MGL_sizex(dc)+1;
    sizey = MGL_sizey(dc)+1;

    /* Create the main backbuffer DC with same pixel format as Window DC */
    if (renderType == OffscreenBackBuffer) {
        if ((backDC = MGL_createOffscreenDC(dc,sizex,sizey)) == NULL)
            MGL_fatalError("Unable to create offscreen DC!");
        MGL_makeCurrentDC(backDC);
        }
    else if (renderType == MemoryBackBuffer) {
        if ((backDC = MGL_createMemoryDC(sizex,sizey,bits,&pf)) == NULL)
            MGL_fatalError("Unable to create Memory DC!");
        MGL_makeCurrentDC(backDC);
        }

    /* Copy the palette from the sprite and store in the offscreen bitmap
     * and the Window DC. The MGL automatically takes care of creating
     * identity palettes when we realize the palette for the memory DC and
     * the Window DC, but we must make sure that both of them get realized.
     */
    MGL_getPalette(dogDC,pal,MGL_getPaletteSize(dogDC),0);
    if (renderType != DirectToScreen) {
        MGL_setPalette(backDC,pal,MGL_getPaletteSize(backDC),0);
        MGL_realizePalette(backDC,MGL_getPaletteSize(backDC),0,false);
        }
    MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
    MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,false);

    /* Fill the back buffer DC with the bitmap transparent color. */
    if (backDC) {
        MGL_setBackColor(transparent);
        MGL_clearViewport();
        }
    return backDC;
}

/****************************************************************************
PARAMETERS:
hwnd    - Handle to Window used for output

REMARKS:
Sets up the MGL for output to the client region of the specified window.
****************************************************************************/
void CreateMGLDeviceContexts(
    HWND hwnd)
{
    /* Create the main MGL Windowed DC for interfacing our main window */
    if ((winDC = MGL_createWindowedDC(hwnd)) == NULL)
        MGL_fatalError("Unable to create Windowed DC!");

    /* Load the sprite bitmap with same pixel format as Window DC */
    dogDC = LoadBitmapIntoMemDC(winDC,"doggie2.bmp");

    /* Get transparent color from pixel (0,0) and dimensions of sprite */
    MGL_makeCurrentDC(dogDC);
    transparent = MGL_getPixelCoord(0,0);
    width = MGL_sizex(dogDC)+1;
    height = MGL_sizey(dogDC)+1;

    /* Create the main backbuffer DC with same pixel format as Window DC */
    backDC = CreateBackBufferDC(winDC);
}

/****************************************************************************
PARAMETERS:
hwnd    - Handle to Window used for output

REMARKS:
Handle window size events and if our window has changed size, we recreate
the back buffer
****************************************************************************/
void ResizeMGLDeviceContexts(
    HWND hwnd)
{
    /* By the time we get called here, the MGL has already adjusted the
     * dimensions of the windowed DC to match the new dimensions of the
     * window if the window size has changed. Hence we simply compare
     * the dimensions against our back buffer, and if they are
     * different we re-create our back buffer DC with the new dimensions.
     */
    if (winDC && backDC &&
            ((MGL_maxxDC(backDC) != MGL_maxxDC(winDC)) ||
             (MGL_maxyDC(backDC) != MGL_maxyDC(winDC)))) {
        MGL_destroyDC(backDC);
        backDC = CreateBackBufferDC(winDC);
        }
}

/****************************************************************************
PARAMETERS:
hwnd    - Handle to Window used for output

REMARKS:
Sets up the MGL for output to the client region of the specified window.
This function will destroy any previously existing MGL device contexts, and
will re-build all the DC's for the Window. Hence this routine is always
called from the Windows WM_SIZE message handler, which will always be
called before the first WM_PAINT message is received, allowing the MGL
device contexts to be created with the appropriate size information.
****************************************************************************/
void DestroyMGLDeviceContexts(
    HWND hwnd)
{
    MGL_makeCurrentDC(NULL);
    if (backDC)
        MGL_destroyDC(backDC);
    if (dogDC)
        MGL_destroyDC(dogDC);
    if (winDC)
        MGL_destroyDC(winDC);
}

/****************************************************************************
REMARKS:
Checks the menu items for the render type menu.
****************************************************************************/
void CheckMenuItems(
    HWND hwnd,
    int type)
{
    HMENU   hMenu = GetMenu(hwnd);

    CheckMenuItem(hMenu,MENU_DIRECT,type == DirectToScreen ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu,MENU_SYSMEM,type == MemoryBackBuffer ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu,MENU_OFFMEM,type == OffscreenBackBuffer ? MF_CHECKED : MF_UNCHECKED);
}

/****************************************************************************
REMARKS:
Function to change the current render type menu item.
****************************************************************************/
void ChangeRenderType(
    HWND hwnd,
    int type)
{
    CheckMenuItems(hwnd,renderType = type);
    MGL_destroyDC(backDC);
    backDC = CreateBackBufferDC(winDC);
    InvalidateRect(hwnd,NULL,TRUE);
}

/****************************************************************************
REMARKS:
Dialog procedure for About dialog box.
****************************************************************************/
BOOL CALLBACK AppAbout(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
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
    lParam = lParam;
    return FALSE;
}

/****************************************************************************
REMARKS:
Main window procedure for application.
****************************************************************************/
LONG CALLBACK AppWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC         hdc;
    BOOL        leftDown,rightDown;
    int         x,y;
    POINT       ofs;

    switch (msg) {
        case WM_CREATE:
            CheckMenuItems(hwnd,renderType);
            CreateMGLDeviceContexts(hwnd);
            if (backDC)
                MGL_bitBltCoord(backDC,dogDC,0,0,width,height,(sizex-width)/2,(sizey-height)/2,MGL_REPLACE_MODE);
            InvalidateRect(hwnd,NULL,TRUE);
            break;
        case WM_SIZE:
            sizex = LOWORD(lParam);
            sizey = HIWORD(lParam);
            ResizeMGLDeviceContexts(hwnd);
            if (backDC)
                MGL_bitBltCoord(backDC,dogDC,0,0,width,height,(sizex-width)/2,(sizey-height)/2,MGL_REPLACE_MODE);
            InvalidateRect(hwnd,NULL,TRUE);
            break;
        case WM_DESTROY:
            DestroyMGLDeviceContexts(hwnd);
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            switch (wParam) {
                case MENU_ABOUT:
                    DialogBox(hInstApp,"AppAbout",hwnd,(DLGPROC)AppAbout);
                    break;
                case MENU_EXIT:
                    PostMessage(hwnd,WM_CLOSE,0,0L);
                    break;
                case MENU_DIRECT:
                    ChangeRenderType(hwnd,DirectToScreen);
                    break;
                case MENU_SYSMEM:
                    ChangeRenderType(hwnd,MemoryBackBuffer);
                    MGL_bitBltCoord(backDC,dogDC,0,0,width,height,(sizex-width)/2,(sizey-height)/2,MGL_REPLACE_MODE);
                    break;
                case MENU_OFFMEM:
                    ChangeRenderType(hwnd,OffscreenBackBuffer);
                    MGL_bitBltCoord(backDC,dogDC,0,0,width,height,(sizex-width)/2,(sizey-height)/2,MGL_REPLACE_MODE);
                    break;
                }
            return 0;
        case WM_MOUSEMOVE:
            leftDown = (GetKeyState(VK_LBUTTON) < 0);
            rightDown = (GetKeyState(VK_RBUTTON) < 0);
            if (!leftDown && !rightDown)
                break;
            hdc = GetDC(hwnd);
            GetViewportOrgEx(hdc,&ofs);
            ReleaseDC(hwnd,hdc);
            x = LOWORD(lParam) - width/2 - ofs.x;
            y = HIWORD(lParam) - height/2 - ofs.y;
            if (renderType == DirectToScreen) {
                if (leftDown)
                    MGL_srcTransBltCoord(winDC,dogDC,0,0,width,height,x,y,transparent,MGL_REPLACE_MODE);
                else if (rightDown)
                    MGL_bitBltCoord(winDC,dogDC,0,0,width,height,x,y,MGL_REPLACE_MODE);
                }
            else {
                if (leftDown)
                    MGL_srcTransBltCoord(backDC,dogDC,0,0,width,height,x,y,transparent,MGL_REPLACE_MODE);
                else if (rightDown)
                    MGL_bitBltCoord(backDC,dogDC,0,0,width,height,x,y,MGL_REPLACE_MODE);
                MGL_bitBltCoord(winDC,backDC,x,y,x+width,y+height,x,y,MGL_REPLACE_MODE);
                }
            break;
        case WM_PAINT:
            MGL_beginPaint(winDC,BeginPaint(hwnd,&ps));
            if (renderType == DirectToScreen) {
                MGL_makeCurrentDC(winDC);
                MGL_clearDevice();
                MGL_bitBltCoord(winDC,dogDC,0,0,width,height,(sizex-width)/2,(sizey-height)/2,MGL_REPLACE_MODE);
                }
            else
                MGL_bitBltCoord(winDC,backDC,0,0,sizex,sizey,0,0,MGL_REPLACE_MODE);
            MGL_endPaint(winDC);
            EndPaint(hwnd,&ps);
            return 0;
        }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

/****************************************************************************
REMARKS:
Main application entry point.
****************************************************************************/
int PASCAL WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrev,
    LPSTR szCmdLine,
    int sw)
{
    MSG         msg;
    WNDCLASS    cls;

    /* Save instance handle for dialog boxes */
    hInstApp = hInst;
    szCmdLine = szCmdLine;

    if (!hPrev) {
        /* Register a class for the main application window */
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

    /* Initialise the MGL */
    if (MGL_init(".",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Create the main window and display it */
    hwndApp = CreateWindow(szAppName,szAppName,WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,0,400,400,NULL,NULL,hInst,NULL);
    ShowWindow(hwndApp,sw);

    /* Polling messages from event queue */
    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        else
            WaitMessage();
        }

    /* Clean up the MGL before exit */
    MGL_exit();
    return msg.wParam;
}

