/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Description:  Win32 implementation for the SciTech cross platform
*               event library.
*
****************************************************************************/

#include "event.h"
#include "pmapi.h"
#include "win32/oshdr.h"
#include "snap/graphics.h"

/*---------------------------- Global Variables ---------------------------*/

/* Maximum number of DirectDraw drivers we will support */

#define MAX_DD_DRIVERS  4

/* Publicly accessible variables */
/* {secret} */
int                 _PM_deskX = 0;      /* Desktop X dimension          */
/* {secret} */
int                 _PM_deskY = 0;      /* Desktop Y dimension          */
/* {secret} */
HWND                _PM_hwndConsole = 0;/* Window handle for console    */
/* {secret} */
ibool               _PM_backInGDI = false;/* Flags if GDI desktop       */
#ifdef  __INTEL__
/* {secret} */
uint                _PM_cw_default;     /* Default FPU control word     */
#endif

/* Private internal variables */

static HINSTANCE        hInstApp = NULL;/* Application instance handle      */
static HWND             hwndUser = NULL;/* User window handle               */
static HINSTANCE        hInstDD = NULL; /* Handle to DirectDraw DLL         */
static LPDIRECTDRAW7    lpDD = NULL;    /* DirectDraw object                */
static LONG             oldWndStyle;    /* Info about old user window       */
static LONG             oldExWndStyle;  /* Info about old user window       */
static int              oldWinPosX;     /* Old window position X coordinate */
static int              oldWinPosY;     /* Old window pisition Y coordinate */
static int              oldWinSizeX;    /* Old window size X                */
static int              oldWinSizeY;    /* Old window size Y                */
static WNDPROC          oldWinProc = NULL;
static PM_WNDPROC       filterWinProc = NULL;
static PM_suspendApp_cb suspendApp = NULL;
static ibool            waitActive = false;
static ibool            isFullScreen = false;
static ibool            disableAutoPlay = false;
static GUID             GUIDList[MAX_DD_DRIVERS] = {0};
static int              numDDDevices = 0;

/* Internal strings */

static char *szWinClassName     = "SciTechDirectDrawWindow";

/* Dynalinks to DirectDraw functions */

static HRESULT (WINAPI *pDirectDrawCreateEx)(GUID FAR *lpGuid, LPDIRECTDRAW7 FAR *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter);
static HRESULT (WINAPI *pDirectDrawEnumerateEx)(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);

/* Declare all DirectDraw GUID's as static variables within this module */

#undef  DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        static const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUID( IID_IDirectDraw7_local,            0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );

/*---------------------------- Implementation -----------------------------*/

/****************************************************************************
REMARKS:
Suspends the application by switching back to the GDI desktop, allowing
normal application code to be processed, and then waiting for the
application activate command to bring us back to fullscreen mode with our
window minimised.
****************************************************************************/
static void LeaveFullScreen(void)
{
    int retCode = PM_SUSPEND_APP;

    if (_PM_backInGDI)
        return;
    if (suspendApp)
        retCode = suspendApp(PM_DEACTIVATE);
    disableAutoPlay = false;
    _PM_backInGDI = true;

    /* Now process messages normally until we are re-activated */
    waitActive = true;
    if (retCode != PM_NO_SUSPEND_APP) {
        while (waitActive) {
            _EVT_pumpMessages();
            Sleep(200);
            }
        }
}

/****************************************************************************
REMARKS:
Reactivate all the surfaces for DirectDraw and set the system back up for
fullscreen rendering.
****************************************************************************/
static void RestoreFullScreen(void)
{
    static ibool    firstTime = true;

    if (firstTime) {
        /* Clear the message queue while waiting for the surfaces to be
         * restored.
         */
        firstTime = false;
        while (1) {
            /* Continue looping until out application has been restored
             * and we have reset the display mode.
             */
            _EVT_pumpMessages();
            if (GetActiveWindow() == _PM_hwndConsole) {
                if (suspendApp)
                    suspendApp(PM_REACTIVATE);
                disableAutoPlay = true;
                _PM_backInGDI = false;
                waitActive = false;
                firstTime = true;
                return;
                }
            Sleep(200);
            }
        }
}

/****************************************************************************
DESCRIPTION:
Suspends the application by switch back to the OS desktop

HEADER:
pmapi.h

REMARKS:
This function suspends the application by switching back to the
regular OS desktop, allowing normal application code to be processed and
then waiting for the application activate command to bring us back to
fullscreen mode.

This version only gets called if we have not captured the screen switch in
our activate message loops and will occur if the DirectDraw drivers lose a
surface for some reason while rendering. This should not normally happen,
but it is included just to be sure (it can happen on WinNT/2000/XP if the
user hits the Ctrl-Alt-Del key combination). Note that this code will always
spin loop, and we cannot disable the spin looping from this version (ie:
if the user hits Ctrl-Alt-Del under WinNT/2000 the application main loop
will cease to be executed until the user switches back to the application).
****************************************************************************/
void PMAPI PM_doSuspendApp(void)
{
    static  ibool firstTime = true;

    /* Call system DLL version if found */
    if (_PM_imports.PM_doSuspendApp != PM_doSuspendApp) {
        _PM_imports.PM_doSuspendApp();
        return;
        }

    if (firstTime) {
        if (suspendApp)
            suspendApp(PM_DEACTIVATE);
        disableAutoPlay = false;
        firstTime = false;
        _PM_backInGDI = true;
        }
    RestoreFullScreen();
    firstTime = true;
}

/****************************************************************************
REMARKS:
Main Window proc for the full screen DirectDraw Window that we create while
running in full screen mode. Here we capture all mouse and keyboard events
for the window and plug them into our event queue.
****************************************************************************/
static LONG CALLBACK PM_winProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LONG lParam)
{
    switch (msg) {
        case WM_SYSCHAR:
            /* Stop Alt-Space from pausing our application */
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (HIWORD(lParam) & KF_REPEAT) {
                if (msg == WM_SYSKEYDOWN)
                    return 0;
                break;
                }
            /* Fall through for keydown events */
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP) {
                if ((HIWORD(lParam) & KF_ALTDOWN) && wParam == VK_RETURN)
                    break;
                /* We ignore the remainder of the system keys to stop the
                 * system menu from being activated from the keyboard and pausing
                 * our app while fullscreen (ie: pressing the Alt key).
                 */
                return 0;
                }
            break;
        case WM_SYSCOMMAND:
            switch (wParam & ~0x0F) {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    /* Ignore screensaver requests in fullscreen modes */
                    return 0;
                }
            break;
        case WM_SIZE:
            if (waitActive && _PM_backInGDI && (wParam != SIZE_MINIMIZED)) {
                /* Start the re-activation process */
                PostMessage(hwnd,WM_DO_SUSPEND_APP,WM_PM_RESTORE_FULLSCREEN,0);
                }
            else if (!waitActive && isFullScreen && !_PM_backInGDI && (wParam == SIZE_MINIMIZED)) {
                /* Start the de-activation process */
                PostMessage(hwnd,WM_DO_SUSPEND_APP,WM_PM_LEAVE_FULLSCREEN,0);
                }
            break;
        case WM_DO_SUSPEND_APP:
            switch (wParam) {
				case WM_PM_RESTORE_FULLSCREEN:
					RestoreFullScreen();
					break;
				case WM_PM_LEAVE_FULLSCREEN:
					LeaveFullScreen();
					break;
                }
            return 0;
        default:
             if (disableAutoPlay) {
                UINT query_cancel_autoplay = RegisterWindowMessage(TEXT("QueryCancelAutoPlay"));
                if (msg == query_cancel_autoplay)
                   return 1;
                }
            break;
        }
    if (oldWinProc)
        return oldWinProc(hwnd,msg,wParam,lParam);
    if (filterWinProc)
        return filterWinProc(hwnd,msg,wParam,lParam);
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

/****************************************************************************
PARAMETERS:
hwnd    - User window to convert
width   - Window of the fullscreen window
height  - Height of the fullscreen window

RETURNS:
Handle to converted fullscreen Window.

REMARKS:
This function takes the original user window handle and modifies the size,
position and attributes for the window to convert it into a fullscreen
window that we can use.
****************************************************************************/
static PM_HWND _PM_convertUserWindow(
    HWND hwnd,
    int width,
    int height)
{
    RECT    window;

    GetWindowRect(hwnd,&window);
    oldWinPosX = window.left;
    oldWinPosY = window.top;
    oldWinSizeX = window.right - window.left;
    oldWinSizeY = window.bottom - window.top;
    oldWndStyle = SetWindowLong(hwnd,GWL_STYLE,WS_POPUP | WS_SYSMENU);
    oldExWndStyle = SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
    ShowWindow(hwnd,SW_SHOW);
    MoveWindow(hwnd,0,0,width,height,TRUE);
    SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
    oldWinProc = (WNDPROC)SetWindowLong(hwnd,GWL_WNDPROC, (LPARAM)PM_winProc);
    return hwnd;
}

/****************************************************************************
PARAMETERS:
hwnd    - User window to restore

REMARKS:
This function restores the original attributes of the user window and put's
it back into it's original state before it was converted to a fullscreen
window.
****************************************************************************/
static void _PM_restoreUserWindow(
    HWND hwnd)
{
    SetWindowLong(hwnd,GWL_WNDPROC, (LPARAM)oldWinProc);
    SetWindowLong(hwnd,GWL_EXSTYLE,oldExWndStyle);
    SetWindowLong(hwnd,GWL_STYLE,oldWndStyle);
    SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hwnd,SW_SHOW);
    MoveWindow(hwnd,oldWinPosX,oldWinPosY,oldWinSizeX,oldWinSizeY,TRUE);
    oldWinProc = NULL;
}

/****************************************************************************
REMARKS:
DirectDraw Multi Monitor Enumeration callback
****************************************************************************/
static BOOL WINAPI DDEnumCallbackEx(
	GUID FAR *lpGUID,
	LPSTR lpDriverDescription,
	LPSTR lpDriverName,
	LPVOID lpContext,
	HMONITOR hm)
{
	if (numDDDevices >= MAX_DD_DRIVERS)
		return DDENUMRET_CANCEL;
    if (lpGUID)
        GUIDList[numDDDevices] = *lpGUID;
    numDDDevices++;
	(void)lpDriverDescription;
	(void)lpDriverName;
	(void)lpContext;
	(void)hm;
	return DDENUMRET_OK;
}

/****************************************************************************
DESCRIPTION:
Loads the DirectDraw libraries

HEADER:
pmapi.h

PARAMETERS:
device  - Index of the device to load DirectDraw for (0 for primary)

RETURNS:
Pointer to the loaded DirectDraw library (number of devices for device == -1)

REMARKS:
Attempts to dynamically load the DirectDraw DLL's and create the DirectDraw
objects that we need. This function is generally never called by application
code, but is called by the DirectX SNAP drivers when the DirectX libraries
need to be loaded.

Note:   If you pass a device value of -1, we will simply enumerate the
        number of DirectDraw devices available and return the number of
        devices cast to a (void*) rather than a pointer to the DirectDraw
        object.

Note:   This function is Windows specific

SEE ALSO:
PM_unloadDirectDraw, PM_getDirectDrawWindow
****************************************************************************/
void * PMAPI PM_loadDirectDraw(
    int device)
{
    HDC     hdc;
    int     bits;
    void    *ret = NULL;

    /* Call system DLL version if found */
    if (_PM_imports.PM_loadDirectDraw != PM_loadDirectDraw)
        return _PM_imports.PM_loadDirectDraw(device);

    /* Load the DirectDraw DLL if not presently loaded */
    GET_DEFAULT_CW();
    if (!hInstDD) {
        hdc = GetDC(NULL);
        bits = GetDeviceCaps(hdc,BITSPIXEL);
        ReleaseDC(NULL,hdc);
        if (bits < 8)
            goto Done;
        if ((hInstDD = LoadLibrary("ddraw.dll")) == NULL)
            goto Done;
        pDirectDrawCreateEx = (void*)GetProcAddress(hInstDD,"DirectDrawCreateEx");
        if (!pDirectDrawCreateEx)
            goto Done;
        pDirectDrawEnumerateEx = (void*)GetProcAddress(hInstDD,"DirectDrawEnumerateExA");
        if (!pDirectDrawEnumerateEx)
            goto Done;
        }

    /* Enumerate all available DirectDraw devices. Note that we *must* do this
     * before trying to create a DirectDraw object for DX9, or this will fail
     * for cards with multiple heads!.
     */
    if (numDDDevices == 0) {
        pDirectDrawEnumerateEx(DDEnumCallbackEx, NULL,
    	    DDENUM_ATTACHEDSECONDARYDEVICES);
        }

    if (device == -1) {
        /* Do not create the DirectDraw object, just return the number
         * of devices that we have found.
         *
         * TODO: Add support for multi-controller/head with DirectX eventually
         */
#if 0
        ret = (void*)numDDDevices;
#else
        ret = (void*)1;
#endif
        }
    else {
        /* Create the DX7 DirectDraw object */
        if (!lpDD && pDirectDrawCreateEx(device ? &GUIDList[device] : NULL, &lpDD, &IID_IDirectDraw7_local, NULL) != DD_OK)
            goto Done;
        ret = lpDD;
        }
Done:
    RESET_DEFAULT_CW();
    return ret;
}

/****************************************************************************
DESCRIPTION:
Unloads the DirectDraw libraries

HEADER:
pmapi.h

PARAMETERS:
device  - Index of the device to unload DirectDraw for (0 for primary)

REMARKS:
Frees any DirectDraw objects for the device. We never actually explicitly
unload the ddraw.dll library, since unloading and reloading it is
unnecessary since we only want to unload it when the application exits and
that happens automatically.

Note:   This function is Windows specific

SEE ALSO:
PM_loadDirectDraw
****************************************************************************/
void PMAPI PM_unloadDirectDraw(
    int device)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_unloadDirectDraw != PM_unloadDirectDraw) {
        _PM_imports.PM_unloadDirectDraw(device);
        return;
        }
    if (lpDD) {
        IDirectDraw_Release(lpDD);
        lpDD = NULL;
        }
    (void)device;
}


/****************************************************************************
DESCRIPTION:
Returns a pointer to the DirectDraw window

HEADER:
pmapi.h

RETURNS:
Pointer to the DirectDraw application window.

REMARKS:
Return the DirectDraw window handle used by the application. This is used
by the SNAP DirectX driver to find the proper window handle registered
by the application for the DirectX fullscreen application.

Note:   This function is Windows specific

SEE ALSO:
PM_loadDirectDraw, PM_unloadDirectDraw
****************************************************************************/
PM_HWND PMAPI PM_getDirectDrawWindow(void)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_getDirectDrawWindow != PM_getDirectDrawWindow)
        return _PM_imports.PM_getDirectDrawWindow();
    return _PM_hwndConsole;
}

/****************************************************************************
DESCRIPTION:
Opens a console for windowed or fullscreen operation

HEADER:
pmapi.h

PARAMETERS:
hWndUser    - Pointer to use application window (NULL if none)
device      - Index of the device to control (0 for primary)
xRes        - X resolution planned for the fullscreen console mode
yRes        - Y resolution planned for the fullscreen console mode
bpp         - Color depth planned for the fullscreen console mode
fullScreen  - True if the console is fullscreen, false if windowed

RETURNS:
Pointer to the console window handle

REMARKS:
This function open a console for output to the screen, creating the main
event handling window if necessary when the hWndUser parameter is set to
NULL.

SEE ALSO:
PM_getConsoleStateSize, PM_saveConsoleState, PM_setSuspendAppCallback,
PM_restoreConsoleState, PM_closeConsole
****************************************************************************/
PM_HWND PMAPI PM_openConsole(
    PM_HWND hWndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
    WNDCLASS        cls;
    static ibool    classRegistered = false;

    /* Call system DLL version if found */
    GA_getSystemPMImports();
    if (_PM_imports.PM_openConsole != PM_openConsole) {
        if (fullScreen) {
            _PM_deskX = xRes;
            _PM_deskY = yRes;
            }
        return _PM_imports.PM_openConsole(hWndUser,device,xRes,yRes,bpp,fullScreen);
        }

    /* Create the fullscreen window if necessary */
    hwndUser = hWndUser;
    if (fullScreen) {
        if (!classRegistered) {
            /* Create a Window class for the fullscreen window in here, since
             * we need to register one that will do all our event handling for
             * us.
             */
            hInstApp            = GetModuleHandle(NULL);
            cls.hCursor         = LoadCursor(NULL,IDC_ARROW);
            cls.hIcon           = LoadIcon(hInstApp,MAKEINTRESOURCE(1));
            cls.lpszMenuName    = NULL;
            cls.lpszClassName   = szWinClassName;
            cls.hbrBackground   = GetStockObject(BLACK_BRUSH);
            cls.hInstance       = hInstApp;
            cls.style           = CS_DBLCLKS;
            cls.lpfnWndProc     = PM_winProc;
            cls.cbWndExtra      = 0;
            cls.cbClsExtra      = 0;
            if (!RegisterClass(&cls))
                return NULL;
            classRegistered = true;
            }
        _PM_deskX = xRes;
        _PM_deskY = yRes;
        if (!hwndUser) {
            char windowTitle[80];
            if (LoadString(hInstApp,1,windowTitle,sizeof(windowTitle)) == 0)
                strcpy(windowTitle,"MGL Fullscreen Application");
            _PM_hwndConsole = CreateWindowEx(WS_EX_APPWINDOW,szWinClassName,
                windowTitle,WS_POPUP | WS_SYSMENU,0,0,xRes,yRes,
                NULL,NULL,hInstApp,NULL);
            }
        else {
            _PM_hwndConsole = _PM_convertUserWindow(hwndUser,xRes,yRes);
            }
        ShowCursor(false);
        isFullScreen = true;
        disableAutoPlay = true;
        }
    else {
        _PM_hwndConsole = hwndUser;
        isFullScreen = false;
        }
    SetFocus(_PM_hwndConsole);
    SetForegroundWindow(_PM_hwndConsole);
    return _PM_hwndConsole;
}

/****************************************************************************
DESCRIPTION:
Find the size of the console state buffer.

HEADER:
pmapi.h

RETURNS:
Size of the console state save buffer in bytes

REMARKS:
This function returns the size of the console state save buffer in bytes.
This buffer can be used to save and restore the state of the OS console.

SEE ALSO:
PM_openConsole, PM_saveConsoleState, PM_setSuspendAppCallback,
PM_restoreConsoleState, PM_closeConsole
****************************************************************************/
int PMAPI PM_getConsoleStateSize(void)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_getConsoleStateSize != PM_getConsoleStateSize)
        return _PM_imports.PM_getConsoleStateSize();

    /* Not used in Windows */
    return 1;
}

/****************************************************************************
DESCRIPTION:
Save the state of the OS console.

HEADER:
pmapi.h

PARAMETERS:
stateBuf    - State buffer to save state to
hwndConsole - Console window handle

REMARKS:
This function saves the state of the OS console, so that it can be later
restored by the PM_restoreConsoleState. This function must be called to
save the console state so that it can go into graphics mode. On many
OS'es this doesn't do much, but on Linux for instance this properly
enabled the console for graphics output, and allows us to properly
restore it later.

SEE ALSO:
PM_openConsole, PM_getConsoleStateSize, PM_setSuspendAppCallback,
PM_restoreConsoleState, PM_closeConsole
****************************************************************************/
void PMAPI PM_saveConsoleState(
    void *stateBuf,
    PM_HWND hwndConsole)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_saveConsoleState != PM_saveConsoleState) {
        _PM_imports.PM_saveConsoleState(stateBuf,hwndConsole);
        return;
        }

    /* Not used in Windows */
    (void)stateBuf;
    (void)hwndConsole;
}

/****************************************************************************
DESCRIPTION:
Set the suspend application callback for the fullscreen console.

HEADER:
pmapi.h

PARAMETERS:
stateBuf    - State buffer to restore state from
hwndConsole - Console window handle

REMARKS:
This function set the suspend application callback for the fullscreen
console. This callback is used to allow the application to properly save
and restore it's own state when the fullscreen console is being switch
away from or being switched back to.

SEE ALSO:
PM_openConsole, PM_closeConsole
****************************************************************************/
void PMAPI PM_setSuspendAppCallback(
    PM_suspendApp_cb saveState)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_setSuspendAppCallback != PM_setSuspendAppCallback) {
        _PM_imports.PM_setSuspendAppCallback(saveState);
        return;
        }
    suspendApp = saveState;
}

/****************************************************************************
DESCRIPTION:
Restores the state of the OS console.

HEADER:
pmapi.h

PARAMETERS:
stateBuf    - State buffer to restore state from
hwndConsole - Console window handle

REMARKS:
This function restore the state of the OS console that was previously
saved with the PM_saveConsoleState function.

SEE ALSO:
PM_openConsole, PM_getConsoleStateSize, PM_setSuspendAppCallback,
PM_saveConsoleState, PM_closeConsole
****************************************************************************/
void PMAPI PM_restoreConsoleState(
    const void *stateBuf,
    PM_HWND hwndConsole)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_restoreConsoleState != PM_restoreConsoleState) {
        _PM_imports.PM_restoreConsoleState(stateBuf,hwndConsole);
        return;
        }

    /* Not used in Windows */
    (void)stateBuf;
    (void)hwndConsole;
}

/****************************************************************************
DESCRIPTION:
Closes the OS console.

HEADER:
pmapi.h

PARAMETERS:
hwndConsole - Console window handle to close

REMARKS:
This function closes the OS console, given the console window handle
passed back from the PM_openConsole function.

SEE ALSO:
PM_openConsole, PM_getConsoleStateSize, PM_saveConsoleState,
PM_restoreConsoleState
****************************************************************************/
void PMAPI PM_closeConsole(
    PM_HWND hwndConsole)
{
    /* Call system DLL version if found */
    if (_PM_imports.PM_closeConsole != PM_closeConsole) {
        _PM_deskX = _PM_deskY = 0;
        _PM_imports.PM_closeConsole(hwndConsole);
        return;
        }
    if (isFullScreen) {
        ShowCursor(true);
        disableAutoPlay = false;
        if (hwndUser)
            _PM_restoreUserWindow(hwndConsole);
        else
            DestroyWindow(hwndConsole);
        }
    hwndUser = NULL;
    _PM_hwndConsole = NULL;
    _PM_deskX = _PM_deskY = 0;
}

/****************************************************************************
DESCRIPTION:
Register a user event filter procedure with the PM library

HEADER:
pmapi.h

PARAMETERS:
userWndProc - User window procedure

REMARKS:
This function registers a user window procedure with the PM library
event subsgstem. This filter is only called for fullscreen modes, and is
called before instead of the default window procedure for the fullscreen
window handle only if the window procedure was not already overridden.
****************************************************************************/
void PMAPI PM_registerEventProc(
    PM_WNDPROC userWndProc)
{
    filterWinProc = userWndProc;
}

