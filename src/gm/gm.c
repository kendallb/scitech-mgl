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
* Environment:  Any
*
* Description:  Game Framework for SciTech MGL. This library sprovides the
*               common functions that every game written with the MGL
*               will require, and automatically handles all of the 'polish'
*               functionality required of commercial games for the DOS
*               and Windows environments. This library provides support for
*               issues such as the following:
*
*               . Handling alt-tab'ing to GDI mode and back to fullscreen
*               . Static system color management in 8bpp modes
*               . Palette management for 8bpp modes windowed and fullscreen
*               . Switching between fullscreen and windowed modes on the fly
*               . Handling of system menu, maximise and minimise buttons
*               . System buffering in a window or with one hardware page
*               . Hardware multi-buffering for smooth animation fullscreen
*               . 1x2 and 2x2 stretching for both fullscreen and a window
*               . Event handling for mouse and keyboard
*               . Application activation notification
*               . Dirty rectangle management for system buffering
*               . OpenGL rendering support both windowed and fullscreen
*
****************************************************************************/

#include "gm/gm.h"
#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NO_GL_MACROS    /* Don't use OpenGL macros, but call functions  */
#include <GL/gl.h>

/*---------------------------- Global Variables ---------------------------*/

ibool                   GM_haveWin95;
ibool                   GM_haveWinNT;
ibool                   GM_exitMainLoop;
ibool                   GM_doDraw = true;
static ibool            GM_inited = false;
static GMDC             GM_dc = {0};
static GM_modeInfo      GM_cntMode;
static int              GM_requestPages;
static int              GM_requestRefreshRate;
static int              GM_requestForceSysMem;
static int              GM_requestStereo;
static ibool            GM_fullScreen;
static ibool            GM_sysPalNoStatic = false;
static char             GM_path[PM_MAX_PATH] = "";
static int              GM_activePage;
static int              GM_visualPage;
static ibool            GM_detected = false;
static ibool            GM_inModeSet = false;
static GM_driverOptions GM_driverOpt = {
                        true,true,false,
                        MGL_GL_AUTO,
                        };
static ibool                GM_oglActive = false;
static GM_modeFilterFunc    GM_modeFilter = NULL;
static GM_drawFunc          GM_draw = NULL;
static GM_eventFunc         GM_event = NULL;
static GM_keyDownFunc       GM_keyDown = NULL;
static GM_keyRepeatFunc     GM_keyRepeat = NULL;
static GM_keyUpFunc         GM_keyUp = NULL;
static GM_mouseDownFunc     GM_mouseDown = NULL;
static GM_mouseUpFunc       GM_mouseUp = NULL;
static GM_mouseMoveFunc     GM_mouseMove = NULL;
static GM_gameFunc          GM_gameLogic = NULL;
static GM_exitFunc          GM_doExit = NULL;
static GM_activateFunc      GM_activate = NULL;
static GM_preModeSwitchFunc GM_preModeSwitch = NULL;
static GM_modeSwitchFunc    GM_modeSwitch = NULL;
static MGL_suspend_cb_t     GM_suspendApp = NULL;
static int                  GM_sizeX,GM_sizeY;
static int                  GM_winSizeX,GM_winSizeY,GM_winStretch;
static int                  GM_winPosX = -1,GM_winPosY = -1;
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
static ibool                GM_autoSwitch = false;
static int                  GM_winAdjustX,GM_winAdjustY,GM_deskX,GM_deskY,GM_deskBits;
static ibool                GM_active = true,GM_minimized = false;
static char                 GM_winTitle[80];
static MGL_WNDPROC          GM_orgWinProc = NULL;
static MGL_WNDPROC          GM_userWndProc = NULL;
static MGL_HINSTANCE        GM_hInstApp = NULL;
static MGL_HWND             GM_hwndUser = NULL;
static ibool                GM_classRegistered = false;
static char                 GM_winClassName[] = "GameFrameworkWindow";
#endif

static pixel_format_t GM_pf15bpp = {        /* 15 bit (555 Format)  */
    0x1F,0x0A,3,0x1F,0x05,3,0x1F,0x00,3,0x01,0x0F,7,
    };

static pixel_format_t GM_pf24bpp = {        /* 24 bit (RGB Format)  */
    0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0,
    };

/*------------------- Game Framework Implementation -----------------------*/

/****************************************************************************
DESCRIPTION:
Disables support for static system palette colors

PARAMETERS:
flag    - True to disable static system colors

HEADER:
gm/gm.h

REMARKS:
This function is used to inform the Game Framework whether you wish to bypass
the static system palette in your game and use all available colors (except
0 and 255 which are always black and white respectively). By default the
static colors are left alone and the MGL will fix up the hardware palette to
use the proper static colors (by default this value is set to false and the
static system colors cannot be changed).

SEE ALSO:
GM_init
****************************************************************************/
void MGLAPI GM_initSysPalNoStatic(
    ibool flag)
{
    GM_sysPalNoStatic = flag;
}

/****************************************************************************
DESCRIPTION:
Tells the Game Framework where to find the MGL resources

PARAMETERS:
MGLPath - path to MGL resources

HEADER:
gm/gm.h

REMARKS:
This function tells the Game Framework where to find the MGL resources such
as bitmaps, fonts etc. By default the MGL always looks in the current
directory, however you can use this to point to a directory that contains the
resources such as on a CD-ROM drive. Functions such as MGL_loadBitmap and
MGL_loadFont will use this path to locate the files.

SEE ALSO:
GM_init
****************************************************************************/
void MGLAPI GM_initPath(
    const char *MGLPath)
{
    strcpy(GM_path,MGLPath);
}

/****************************************************************************
DESCRIPTION:
Sets the default window position for windowed modes

PARAMETERS:
x   - X coordinate for top left corner of window
y   - Y coordinate for top left corner of window

HEADER:
gm/gm.h

REMARKS:
This function tells the Game Framework where you want the top left corner
of the window to be positioned for windowed modes. By default the Game
Framework will center the window on the screen the first time the window
is created, and you can use this function to override the default
behavior.

SEE ALSO:
GM_init
****************************************************************************/
void MGLAPI GM_initWindowPos(
    int x,
    int y)
{
    GM_winPosX = x;
    GM_winPosY = y;
}

#if defined(__WINDOWS__) && !defined(__CONSOLE__)
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void ClearMessageQueue(void)
{
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
}

static ibool HaveWin95(void)
{
    int verMajor = GetVersion() & 0xFF;
    return (verMajor >= 4);
}
#endif

/****************************************************************************
REMARKS:
Detects the installed hardware if we have not already detected it. This
function is called from multiple places that require the hardware to be
detected.
****************************************************************************/
static void DetectHardware(void)
{
    if (!GM_detected) {
        MGL_exit();
        if (MGL_init(GM_path,NULL) == 0)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        MGL_enableAllDrivers();
        if (!GM_driverOpt.useSNAP)
            MGL_disableDriver(MGL_SNAPNAME);
        if (GM_driverOpt.useHWOpenGL)
            MGL_enableOpenGLDrivers();
        MGL_availablePages(0);
        GM_detected = true;
        }
}

/****************************************************************************
REMARKS:
Adds the mode to the global list of available modes at the end of the
current list. Note that if the mode is low resolution mode below 512x384,
we set the windowed stretch factor to be 2x2, otherwise we set it to be
1x1.
****************************************************************************/
static void AddMode(
    int mode,
    int xRes,
    int yRes,
    int bits,
    GM_stretchType stretch,
    ulong flags)
{
    GM_modeInfo *mi;

    mi = &GM_dc.modeList[GM_dc.numModes];
    mi->mode = mode;
    mi->xRes = xRes;
    mi->yRes = yRes;
    mi->bitsPerPixel = bits;
    if (mode == GM_WINDOWED_ONLY)
        mi->pages = 1;
    else
        mi->pages = MGL_availablePages(mode);
    mi->stretch = stretch;
    mi->flags = flags;
    if ((xRes < 512 && yRes < 384) || (xRes == 320 && (yRes == 400 || yRes == 480)))
        mi->windowedStretch = GM_STRETCH_2x2;
    else
        mi->windowedStretch = GM_STRETCH_1x1;
    if (mode == GM_WINDOWED_ONLY)
        strcpy(mi->driverName,"windowed");
    else
        strcpy(mi->driverName,MGL_modeDriverName(mode));
    GM_dc.numModes++;
    if (mode != GM_WINDOWED_ONLY)
        GM_dc.numFullscreenModes++;
}

/****************************************************************************
REMARKS:
Enumerates all the available native and psuedo graphics modes available
on the end users system, and builds a list in the GM_dc.modeList array.
The array is terminated with a -1 in the last mode in the list.
****************************************************************************/
static void EnumerateModes(void)
{
    int         xRes,yRes,bits,mode,lowestBits;
    ulong       flags;
    static int  recursive = false;

    /* Find the color depth of the first fullscreen mode */
    if (GM_detected)
        return;
    DetectHardware();
    GM_dc.numModes = GM_dc.numFullscreenModes = 0;
    if (!MGL_modeResolution(0,&xRes,&yRes,&lowestBits))
        lowestBits = -1;

    /* Search the available modes for all supported MGL modes in order from
     * lowest resolution and color depth to highest. If the native mode is
     * not available, we attempt to emulate the mode with stretching and if
     * that fails we create the mode as windowed only.
     */
    for (mode = 0; GM_dc.numModes <= GM_MAXMODE; mode++) {
        if (!MGL_modeResolution(mode,&xRes,&yRes,&bits))
            break;
        if (bits < 8)
            continue;
        flags = MGL_modeFlags(mode);
        if (GM_modeFilter && !GM_modeFilter(xRes,yRes,bits,flags))
            continue;
        if (!(GM_driverOpt.modeFlags & GM_MODE_8BPP) && bits == 8)
            continue;
        if (!(GM_driverOpt.modeFlags & GM_MODE_16BPP) && (bits == 15 || bits == 16))
            continue;
        if (!(GM_driverOpt.modeFlags & GM_MODE_24BPP) && bits == 24)
            continue;
        if (!(GM_driverOpt.modeFlags & GM_MODE_32BPP) && bits == 32)
            continue;
        if ((GM_driverOpt.modeFlags & GM_ONLY_2D_ACCEL) && !(flags & MGL_HAVE_ACCEL_2D))
            continue;
        if (!(GM_driverOpt.modeFlags & GM_ONLY_3D_ACCEL) || ((GM_driverOpt.modeFlags & GM_ONLY_3D_ACCEL) && (flags & MGL_HAVE_ACCEL_3D))) {
            /* The mode is valid, so add it to the list */
            AddMode(mode,xRes,yRes,bits,GM_STRETCH_1x1,flags);
            }
        }
    if (!recursive && GM_dc.numFullscreenModes == 0) {
        /* No fullscreen video modes were found at 8BPP, so relax the
         * color depth restriction so we can find fullscreen DIB modes
         * at the desktop color depth that we can run with a translated
         * blit. This can happen on Integraph systems as they dont support
         * ChangeDisplaySettings to different color depths.
         */
        int oldFlags = GM_driverOpt.modeFlags;
        GM_driverOpt.modeFlags = GM_MODE_ALLBPP | (GM_driverOpt.modeFlags & GM_ALLOW_WINDOWED);
        recursive = true;
        EnumerateModes();
        GM_driverOpt.modeFlags = oldFlags;
        }
    recursive = false;
}

/****************************************************************************
DESCRIPTION:
Sets the custom mode filter for mode enumeration

PARAMETERS:
filter  - New mode filter to set

HEADER:
gm/gm.h

REMARKS:
This function allows you to register a mode filter callback with the
Game Framework, which will be called during mode enumeration and will
allow you to apply your own custom filtering code to the list of
available video modes. Hence you can use this function to filter out all
non 1:1 aspect ratio modes for instance.

SEE ALSO:
GM_init
****************************************************************************/
void MGLAPI GM_setModeFilterFunc(
    GM_modeFilterFunc filter)
{
    GM_modeFilter = filter;
}

/****************************************************************************
DESCRIPTION:
Sets driver registration options for the Game Framework

PARAMETERS:
opt - Parameter block containing driver registration options

HEADER:
gm/gm.h

REMARKS:
This function tells the Game Framework which driver technologies you want
to support in your game. By default all of them are enabled, and you can
use this function to disable certain driver technologies at runtime for
compatibility in the field. You may call this function as many times as
you wish to change the driver options on the fly, and if the values
change the Game Framework will re-enumerate the list of available graphics
modes for you.

Note that the GM_driverOptions structure also contains the modeFlags field
which represents the color depths that you will be supporting in your
application, so that the Game Framework will only enumerate modes that your
game can support. For instance if you only support 8bpp modes, than pass a
value of GM_MODE_8BPP. If you support 8bpp and 15/16bpp then pass in a value
of GM_MODE_8BPP | GM_MODE_16BPP. Note also that you can change the supported
mode flags at any time, which is useful if your software renderer only
supports 8bpp modes, while in 3D hardware accelerated modes you want to
support all available color depths.

Note: The Game Framework enumerates both 15bpp (5:5:5) and 16bpp (5:6:5) modes
      when you pass in a value of GM_MODE_16BPP, since both of these modes
      will likely be available on end user systems. Also note that it is
      equally likely that in some cases only one of these formats and not the
      other is supported on the end user system, so your code will have to be
      able to support both formats for compatibility.

Note: If your game does not require hardware OpenGL support (ie: you are not
      using OpenGL in your game), then you should set the useOpenGLHW flag
      to false to make sure that the OpenGL drivers are not registered with
      the MGL (which would require the OpenGL runtime DLL's to be installed
      on your end users system).

Note: We recommend that you provide support for disabling both DirectDraw and
      WinDirect modes via a command line switch in your game, in case either
      of these two technologies have problems on your customer machines.
      Please see the Game Framework sample code for ideas on how to do this.

SEE ALSO:
GM_init
****************************************************************************/
void MGLAPI GM_setDriverOptions(
    GM_driverOptions *opt)
{
    if (memcmp(&GM_driverOpt,opt,sizeof(GM_driverOpt)) != 0) {
        GM_driverOpt = *opt;
        GM_detected = false;
        EnumerateModes();
        }
}

/****************************************************************************
REMARKS:
Resets all the relevant global variables that need to be reset each time
we begin a main loop.
****************************************************************************/
static void ResetGlobals(void)
{
    /* Reset global variables to defaults */
    GM_inited = false;
    GM_exitMainLoop = false;
    GM_doDraw = true;
    GM_dc.dc = NULL;
    GM_dc.dispdc = NULL;
    GM_dc.backdc = NULL;
    GM_inModeSet = false;
    GM_oglActive = false;
    GM_winPosX = -1;
    GM_winPosY = -1;
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    GM_dc.mainWindow = NULL;
    GM_orgWinProc = NULL;
    GM_active = true;
    GM_minimized = false;
#endif
}

/****************************************************************************
DESCRIPTION:
initializes the Game Framework

PARAMETERS:
windowTitle - Title for window in windowed modes and on task bar

HEADER:
gm/gm.h

RETURNS:
Pointer to the game framework context object

REMARKS:
This function initializes the Game Framework and must be called before you
attempt to set a graphics mode. Once this function has been called, the
Game Framework will have enumerated all the available graphics modes and
stored this information into the modeList field of the GMDC structure
returned from this function. It is then up to the application to find a
suitable mode and initialized it with a call to GM_setMode.

Before you can do anything useable with the Game Framework, after you
have called the GM_init function, you must then register a number of
function callbacks with the Game Framework that it will call to
implement the 'body' of the game (similar to C++ virtual functions, but
in C). Two of the most important are GM_setDrawFunc and
GM_setGameLogicFunc. If you want to respond to keyboard commands you
will probably also want to call GM_setKeyDownFunc as well.

Note: The Game Framework is responsible for creating the main window used by
      the game. Hence the value you pass in for windowTitle will be the main
      title for your games window in windows modes, as well as the title that
      the user will see when your game is minimised to the task bar in
      Windows 95 and Windows NT 4.0.

Note: The Game Framework only creates and maintains a single window for the
      life of the game, and on switches between windowed and fullscreen modes
      will automatically change the attributes of the main window for the
      appropriate mode. This way your game only needs to register a single
      main window with DirectSound and other DirectX components during
      initialization time, and avoids the problems of re-starting DirectSound
      during mode switches (and hence you sound can continue to play as you
      switch on the fly between resolutions and fullscreen and windowed
      modes).

See Also:
GM_setDriverOptions, GM_setMode, GM_setDrawFunc, GM_setGameLogicFunc
****************************************************************************/
GMDC * MGLAPI GM_init(
    const char *windowTitle)
{
    /* Reset global variables */
    ResetGlobals();

#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    /* Detect the version of Windows we are running on */
    GM_haveWin95 = HaveWin95();
    GM_haveWinNT = !GM_haveWin95;
    GM_deskX = GetSystemMetrics(SM_CXSCREEN);
    GM_deskY = GetSystemMetrics(SM_CYSCREEN);
    {
    HDC hdc = GetDC(NULL);
    GM_deskBits = GetDeviceCaps(hdc,BITSPIXEL);
    ReleaseDC(NULL,hdc);
    }

    /* Let the MGL know about our application instance handle */
    GM_hInstApp = GetModuleHandle(NULL);
    strncpy(GM_winTitle,windowTitle,sizeof(GM_winTitle));
    GM_winTitle[sizeof(GM_winTitle)-1] = 0;
#else
    (void)windowTitle;
#endif
    EnumerateModes();
    return (&GM_dc);
}

/****************************************************************************
DESCRIPTION:
Sets the draw callback function

PARAMETERS:
func    - Draw callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the draw callback function for your Game Framework game
and is called by the Game Framework main loop (GM_mainLoop) once
per frame to draw the current frame in the game. Note that the Game Framework
expects this function both draw the current frame and swap the buffers to
make them visible using GM_swapBuffers or GM_swapDirtyBuffers.

Note: In order to be able to continue running your games main logic loops
      while the user has switch away (ie: Alt-Tab) from your game in Windows,
      while the application is minimised we continue to process messages and
      call the registered game logic callback, however the draw callback will
      not be called until the application is restored to fullscreen mode.
      Hence your draw callback should not contain any game logic
      functionality, but only contain code to draw the current frame in the
      game.

See Also:
GM_init, GM_swapBuffers, GM_swapDirtyBuffers
****************************************************************************/
void MGLAPI GM_setDrawFunc(
    GM_drawFunc func)
{ GM_draw = func; }

/****************************************************************************
DESCRIPTION:
Sets the key down callback function

PARAMETERS:
func    - Key down callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the key down callback function for your Game Framework game
and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all key down events in the order that they are
entered by the user. Note that your callback is passed a copy of the event
that needs to be processed, so you can if you wish have a single handler for
all key events, or one that handles both key down and key repeat events and
register the same function with the Game Framework for both event types.

Note: The key down callback will not be called for key repeat events (ie: the
      user holds a key down). If you wish to capture key repeat events, use
      GM_setKeyRepeatFunc.

See Also:
GM_init, GM_setKeyRepeatFunc, GM_setKeyUpFunc
****************************************************************************/
void MGLAPI GM_setKeyDownFunc(
    GM_keyDownFunc func)
{ GM_keyDown = func; }

/****************************************************************************
DESCRIPTION:
Sets the key repeat callback function

PARAMETERS:
func    - Key repeat callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the key repeat callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all key repeat events in the order that they are
entered by the user. Note that your callback is passed a copy of the event
that needs to be processed, so you can if you wish have a single handler for
all key events, or one that handles both key down and key repeat events and
register the same function with the Game Framework for both event types.

See Also:
GM_setKeyDownFunc, GM_setKeyUpFunc
****************************************************************************/
void MGLAPI GM_setKeyRepeatFunc(
    GM_keyRepeatFunc func)
{ GM_keyRepeat = func; }

/****************************************************************************
DESCRIPTION:
Sets the key up callback function

PARAMETERS:
func    - Key up callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the key up callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all key up events in the order that they are
entered by the user. Note that your callback is passed a copy of the event
that needs to be processed, so you can if you wish have a single handler for
all key events if you wish.

See Also:
GM_setKeyDownFunc, GM_setKeyRepeatFunc
****************************************************************************/
void MGLAPI GM_setKeyUpFunc(
    GM_keyUpFunc func)
{ GM_keyUp = func; }

/****************************************************************************
DESCRIPTION:
Sets the event callback function

PARAMETERS:
func    - Event callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the general event callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all non key and non mouse related events in the
order that they are entered by the user (ie: timer or user events).
****************************************************************************/
void MGLAPI GM_setEventFunc(
    GM_eventFunc func)
{ GM_event = func; }

/****************************************************************************
DESCRIPTION:
Sets the mouse down callback function

PARAMETERS:
func    - Mouse down callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the mouse down callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all mouse down events in the order that they are
entered by the user. Note that your callback is passed a copy of the event
that needs to be processed, so you can if you wish have a single handler for
all mouse events, or one that handles both mouse down and mouse up events and
register the same function with the Game Framework for both event types.

See Also:
GM_init, GM_setMouseUpFunc, GM_setMouseMoveFunc
****************************************************************************/
void MGLAPI GM_setMouseDownFunc(
    GM_mouseDownFunc func)
{ GM_mouseDown = func; }

/****************************************************************************
DESCRIPTION:
Sets the mouse up callback function

PARAMETERS:
func    - Mouse up callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the mouse up callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all mouse up events in the order that they are
entered by the user. Note that your callback is passed a copy of the event
that needs to be processed, so you can if you wish have a single handler for
all mouse events, or one that handles both mouse down and mouse up events and
register the same function with the Game Framework for both event types.

See Also:
GM_init, GM_setMouseDownFunc, GM_setMouseMoveFunc
****************************************************************************/
void MGLAPI GM_setMouseUpFunc(
    GM_mouseUpFunc func)
{ GM_mouseUp = func; }

/****************************************************************************
DESCRIPTION:
Sets the mouse move callback function

PARAMETERS:
func    - Mouse move callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the mouse move callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) multiple
times per frame to process all mouse move events in the order that they are
entered by the user.

See Also:
GM_init, GM_setMouseDownFunc, GM_setMouseUpFunc
****************************************************************************/
void MGLAPI GM_setMouseMoveFunc(
    GM_mouseMoveFunc func)
{ GM_mouseMove = func; }

/****************************************************************************
DESCRIPTION:
Sets the game logic callback function

PARAMETERS:
func    - Game logic callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the game logic callback function for your Game Framework
game and is called by the Game Framework main loop (GM_mainLoop) once
per frame to update the game logic for the next frame after drawing the
current frame.

Note: In order to be able to continue running your games main logic loops
      while the user has switch away (ie: Alt-Tab) from your game in Windows,
      while the application is minimised we continue to process messages and
      call the registered game logic callback, however the draw callback will
      not be called until the application is restored to fullscreen mode.
      Hence your game logic callback should not contain /any/ code that
      performs drawing to the screen, as all that code should be located
      in your draw callback function.

See Also:
GM_init, GM_setDrawFunc
****************************************************************************/
void MGLAPI GM_setGameLogicFunc(
    GM_gameFunc func)
{ GM_gameLogic = func; }

/****************************************************************************
DESCRIPTION:
Sets the pre-mode switch callback function

PARAMETERS:
func    - Pre-mode switch callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the pre-mode switch callback function for your Game
Framework game and is called by the Game Framework when automatically switching
on the fly between windowed and fullscreen modes. If you have registered
a mode switch function with GM_setModeSwitchFunc, your pre-mode switch
function will be called before the existing mode is destroyed, giving your
code a change to destroy any internal data structures that might need to
be cleaned up before the mode is destroyed and the new one created.

See Also:
GM_init, GM_setMode, GM_setModeSwitchFunc
****************************************************************************/
void MGLAPI GM_setPreModeSwitchFunc(
    GM_preModeSwitchFunc func)
{ GM_preModeSwitch = func; }

/****************************************************************************
DESCRIPTION:
Sets the mode switch callback function

PARAMETERS:
func    - Mode switch callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the mode switch callback function for your Game Framework
game and is called by the Game Framework when automatically switching
on the fly between windowed and fullscreen modes. By default this handler
is set to NULL, and unless you call this function support for switching
on the fly between windowed and fullscreen modes is disabled. By default the
Game Framework contains code to provide two methods of switching to fullscreen
modes when running in windowed modes:

1   - When the user hits the /Alt-Enter/ key combination
2   - When the user clicks the /Maximise/ button on the games title bar

Likewise when the game is running in a fullscreen mode and the user hits the
/Alt-Enter/ key, the video mode will automatically be switched to windowed
mode. In order to support auto-switching between fullscreen and windowed
modes, all the MGL device contexts will be destroyed and re-created during
the switch, so you will have to include other code to re-initialize the MGL
to the state that the game is currently in (ie: setting the color palette
etc.) in your mode switch callback. You will also need to code your game in
such as way that it can handle dynamic resolution changes on the fly.

See Also:
GM_init, GM_setMode
****************************************************************************/
void MGLAPI GM_setModeSwitchFunc(
    GM_modeSwitchFunc func)
{ GM_modeSwitch = func; }

/****************************************************************************
DESCRIPTION:
Sets the exit callback function

PARAMETERS:
func    - Exit callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the exit callback function for your Game Framework
game and is called by the Game Framework main loop just before calling
MGL_exit to shut down the MGL and return to windowed mode before returning
to your code. If you have any code that must be called before exiting from
fullscreen mode, you should register it with this function.

Note: This function will /always/ be called /after/ the mode switch has
      occurred and the system is in the new graphics mode.

See Also:
GM_init, GM_mainLoop
****************************************************************************/
void MGLAPI GM_setExitFunc(
    GM_exitFunc func)
{ GM_doExit = func; }

/****************************************************************************
DESCRIPTION:
Sets the suspend app callback function

PARAMETERS:
func    - Suspend app callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the suspend app callback function for your Game Framework
game and is called by the Game Framework whenever the user switches away
from your game (suspends it, such as with Alt-Tab). The Game Framework
registers a default suspend application callback with the MGL to do most
of the handling for you. Note however that by default the Game Framework
suspend app callback passes a return value of MGL_SUSPEND_APP back to the
MGL which will suspend execution of your game until it has been restored.
The Game Framework has code to automatically ensure that the draw callback
is not called when the game is minimised and gets re-enabled when the
game is restored again if you return a value of MGL_NO_SUSPEND_APP from your
registered callback (this way you can continue to run networking code
in the background to keep other network players running if the server is
temporarily minimised).

You should register your own version of this function to handle extra things
during suspend and restores such as suspending CD-Audio sound playback or
other stuff not automatically handled by the underlying multi-media
libraries. A typical suspend application callback might be coded as follows:

    int _ASMAPI SuspendAppProc(MGLDC *dc,int flags)
    {
        if (flags == MGL_DEACTIVATE) {
            // Disable CD-Audio
            // Do other disabling stuff
            return MGL_NO_SUSPEND_APP;
            }
        else if (flags == MGL_REACTIVATE) {
            // Re-enable CD-Audio
            // Do other re-enabling stuff
            return MGL_NO_SUSPEND_APP;
            }
    }

See Also:
GM_init, GM_setSuspendAppProc
****************************************************************************/
void MGLAPI GM_setSuspendAppCallback(
    MGL_suspend_cb_t saveState)
{ GM_suspendApp = saveState; }

/****************************************************************************
DESCRIPTION:
Sets the application activate callback function

PARAMETERS:
func    - Application activate callback function to register

HEADER:
gm/gm.h

REMARKS:
This function sets the application activate callback function for your Game
Framework game. This function is called in windowed modes whenever the
activation status of your game changes, which can occur if the window is
minimised to the task bar or if the user switches away to another application
using /Alt-Tab/. Your callback is passed a flag that indicates whether your
game is now currently active or not, and should be used to enable and disable
support for things such as CD-Audio when your application loses
activation (or the current focus).

Note: The Game Framework contains built in support for enabling and disabling
      the static system palette colors when running in windowed modes, and
      will automatically switch back to static system color mode when your
      window loses the activation focus. Hence you should not attempt to
      change this in your application activate callback (if you want to be
      able to use the static system colors in a window, call
      GM_initSysPalNoStatic(true) before you initialize the Game Framework.

Note: This function is only called in windowed modes, and the equivalent
      function for fullscreen modes is set with the GM_setSuspendAppCallback
      function.

See Also:
GM_init, GM_setSuspendAppCallback, GM_initSysPalNoStatic
****************************************************************************/
void MGLAPI GM_setAppActivate(
    GM_activateFunc func)
{ GM_activate = func; }

#if defined(__WINDOWS__) && !defined(__CONSOLE__)
#include "gm/gmdlg.rh"

/****************************************************************************
DESCRIPTION:
Registers a user event procedure with the Game Framework

PARAMETERS:
winproc - User window procedure to register

HEADER:
gm/gm.h

REMARKS:
This function registers a user window procedure with the Game Framework.
The primary purpose of this function is to allow your game to process
regular windows messages (such as CD-Audio notification messages and other
messages not automatically handled by the Game Framework) in your game.
To use this function, simply write a regular window procedure as you would
normally for a real window, but instead of registering the window procedure
with in the RegisterClass function (the Game Framework calls RegisterClass
for you during initialization) call this function to register your window
procedure with the Game Framework.

Note: This function is only available in the Windows version of the Game
      Framework.
***************************************************************************/
void MGLAPI GM_registerEventProc(
    MGL_WNDPROC winproc)
{ GM_userWndProc = winproc; }

/****************************************************************************
DESCRIPTION:
Registers a user main window with the Game Framework

PARAMETERS:
hwndMain    - Handle to the main window for the application

HEADER:
gm/gm.h

REMARKS:
This function registers a user main window with the Game Framework.
The primary purpose of this function is to allow your main game code to
do the creation of the main window that is used by the Game Framework,
instead of letting the Game Framework libraries do it for you. This is mostly
to support integrating the Game Framework code with existing game code that
already does window creation and message handling.

Note that you should only call this function /after/ you have called GM_init
to initialize the Game Framework.

Note: If you use this function to register a main window, /do not/ use the
      GM_registerEventProc function to register your window procedure with
      the Game Framework!!

Note: This function is only available in the Windows version of the Game
      Framework.
***************************************************************************/
void MGLAPI GM_registerMainWindow(MGL_HWND
    hwndMain)
{ GM_hwndUser = hwndMain; }

/****************************************************************************
DESCRIPTION:
Centers the specified window within the bound of the parent window

PARAMETERS:
hWndCenter  - Window to center
parent      - Handle for parent window
repaint     - True if window should be re-painted

REMARKS:
Centers the specified window within the bounds of the specified parent window.
If the parent window is NULL, then we center it using the Desktop window.
***************************************************************************/
static void MGLAPI GM_centerWindow(
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

#define WM_DO_MODE_SWITCH   0x7FFF

/****************************************************************************
DESCRIPTION:
Main window procedure for the Game Framework.

PARAMETERS:
hwnd    - Handle to window recieving message
uMsg    - Message identifier
wParam  - Word parameter for message
lParam  - Long parameter for message

RETURNS:
Status of processing message

REMARKS:
Main window procedure for the Game Framework. This is where we handle all of
the common window messages on behalf of the game for issues like palette
activation, alt-tabbing etc.
***************************************************************************/
static LONG WINAPI GM_windowProc (
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg) {
        case WM_COMMAND:
            if (wParam == WM_DO_MODE_SWITCH) {
                /* Handle auto-switch between windowed and fullscreen */
                if (GM_modeSwitch) {
                    ibool windowed = (lParam == 2) ? true : GM_fullScreen;
                    if (GM_preModeSwitch) {
                        if (!GM_preModeSwitch(&GM_cntMode,windowed))
                            break;
                        }
                    GM_autoSwitch = true;
                    GM_setModeExt(&GM_cntMode,windowed,GM_requestPages,GM_requestRefreshRate,GM_requestForceSysMem,GM_requestStereo);
                    GM_modeSwitch(&GM_cntMode,windowed);
                    GM_autoSwitch = false;
                    }
                }
            break;
        case WM_SYSCHAR:
        case WM_SYSKEYUP:
            /* Stop Alt-Space from pausing our game */
            return 0;
        case WM_SYSKEYDOWN:
            /* Capture Alt-Enter's and toggle between fullscreen/windowed */
            if ((HIWORD(lParam) & KF_ALTDOWN) && wParam == VK_RETURN && GM_dc.dispdc)
                PostMessage(hwnd,WM_COMMAND,WM_DO_MODE_SWITCH,1);
            /* We ignore the remainder of the system keys to stop the
             * system menu from being activated from the keyboard and pausing
             * our app while fullscreen (ie: pressing the Alt key).
             */
            return 0;
        case WM_SYSCOMMAND:
            switch (wParam & ~0x0F) {
                case SC_MAXIMIZE:
                    /* Force the game to switch to fullscreen mode when the
                     * user clicks on the maximise button. Note that we dont
                     * let windows see this message so that the window is
                     * not actually maximised.
                     */
                    if (GM_dc.dispdc)
                        PostMessage(hwnd,WM_COMMAND,WM_DO_MODE_SWITCH,1);
                    return 0;
                }
            break;
        case WM_ACTIVATE:
            GM_active = LOWORD(wParam);
            GM_minimized = HIWORD(wParam);
            if (!GM_fullScreen) {
                if (GM_dc.dispdc) {
                    HDC hdc = GetDC(NULL);
                    if (GM_active && (GetSystemPaletteUse(hdc) == SYSPAL_STATIC)
                            && (GM_dc.dispdc->mi.bitsPerPixel == 8)
                            && GM_sysPalNoStatic && !GM_minimized)
                        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
                    else
                        SetSystemPaletteUse(hdc, SYSPAL_STATIC);
                    ReleaseDC(NULL,hdc);
                    }
                if (GM_activate)
                    GM_activate(GM_active);
                }
            break;
        case WM_SIZE:
            break;
        case WM_PAINT:
            ValidateRect(hwnd,NULL);
            break;
        case WM_DISPLAYCHANGE:
            if (!GM_inModeSet && !GM_fullScreen && GM_dc.dispdc) {
                /* The display mode has changed, so force a switch to
                 * windowed mode to properly update the system buffer.
                 */
                PostMessage(hwnd,WM_COMMAND,WM_DO_MODE_SWITCH,2);
                }
            break;
        case WM_CLOSE:
            if (!GM_fullScreen) {
                if (MessageBox (hwnd,"Are you sure you want to quit?", "Confirm Exit",
                        MB_YESNO | MB_SETFOREGROUND | MB_ICONQUESTION) == IDYES)
                    GM_exit();
                }
            return 1;
        case WM_DESTROY:
            if (!GM_fullScreen)
                GM_exit();
            break;
        }
    if (GM_userWndProc)
        return GM_userWndProc(hwnd,uMsg,wParam,lParam);
    if (GM_orgWinProc)
        return GM_orgWinProc(hwnd,uMsg,wParam,lParam);
    return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

/****************************************************************************
DESCRIPTION:
Processes all Windows events for the current iteration of the main loop

HEADER:
gm/gm.h

REMARKS:
This function is the message processing handler for the main event loop
for the Game Framework. This function basically processes all windows
messages and passes them to the window procedure for handling. Note that
this function does not use the MGL's event handling routines, and is
specific to Windows. Essentially this function implements the following:

    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

See Also:
GM_mainLoop, GM_processEvents, GM_cleanup
****************************************************************************/
void MGLAPI GM_processEventsWin(void)
{
    MSG         msg;

    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
}

static int      GM_modeNums[MAX_MODES],GM_chosenMode,GM_prevMode = -1;
static int      GM_windowedStretch = GM_STRETCH_1x1;
static int      GM_selectBits = -1;
static ibool    GM_startWindowed;

/****************************************************************************
PARMETERS:
hwnd    - Handle to dialog box window

REMARKS:
Refreshes the list of available video modes in the video mode list box given
the newly selected pixel depth.
****************************************************************************/
static void RefreshModeList(
    HWND hwnd)
{
    char        buf[MAX_STR];
    int         i,modes,selectIndex;
    GM_modeInfo *m;
    HWND        hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
    ibool       haveMode = false;

    /* Find out if we should use both WinDirect and DirectDraw or just one */
    MGL_glSetOpenGLType(GM_driverOpt.openGLType);

    SendMessage(hwndLst,LB_RESETCONTENT,0,0);
    if (IsDlgButtonChecked(hwnd,IDC_8BIT))          GM_selectBits = 8;
    else if (IsDlgButtonChecked(hwnd,IDC_16BIT))    GM_selectBits = 16;
    else if (IsDlgButtonChecked(hwnd,IDC_24BIT))    GM_selectBits = 24;
    else if (IsDlgButtonChecked(hwnd,IDC_32BIT))    GM_selectBits = 32;

    GM_detected = false;
    EnumerateModes();
    for (m = GM_dc.modeList,i = modes = 0; i < GM_dc.numModes; i++,m++) {
        if (m->bitsPerPixel != GM_selectBits)
            continue;
        sprintf(buf,"%dx%d %dbit %2d page (%s)",
            m->xRes,m->yRes,m->bitsPerPixel,m->pages,m->driverName);
        if (m->mode == GM_WINDOWED_ONLY)
            strcat(buf," - windowed only");
        else {
            if (m->stretch != GM_STRETCH_1x1) {
                if (m->stretch == GM_STRETCH_1x2)
                    strcat(buf," - 1x2 stretched");
                else
                    strcat(buf," - 2x2 stretched");
                }
            if (m->flags & MGL_HAVE_STEREO)
                strcat(buf," - stereo");
            if (m->flags & MGL_HAVE_ACCEL_2D)
                strcat(buf," - 2D accel");
            if (m->flags & MGL_HAVE_ACCEL_3D)
                strcat(buf," - 3D accel");
            }
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)buf);
        if (i == GM_prevMode)
            selectIndex = modes;
        if (modes == MAX_MODES)
            MGL_fatalError("Too many display modes!!");
        GM_modeNums[modes++] = i;
        haveMode = true;
        }
    SendMessage(hwndLst,LB_SETCURSEL,selectIndex,0);
    if (!haveMode) {
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"No available modes");
        EnableWindow(hwndLst,FALSE);
        }
    else {
        EnableWindow(hwndLst,TRUE);
        }
}

/****************************************************************************
REMARKS:
Main dialog box procedure for the mode selection dialog box, allowing for
interactive selection of graphics modes.
****************************************************************************/
static BOOL CALLBACK MainDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    HWND    hwndLst;

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CANCEL:
                    EndDialog(hwnd,IDC_CANCEL);
                    break;
                case IDC_MODELIST:
                    if (HIWORD(wParam) != LBN_DBLCLK) {
                        // TODO: Set 2x2 stretch button option in here!
                        break;
                        }
                case IDC_OK:
                    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
                    if (SendMessage(hwndLst,LB_GETCOUNT,0,0)) {
                        GM_chosenMode = GM_modeNums[SendMessage(hwndLst,LB_GETCURSEL,0,0)];
                        GM_windowedStretch = IsDlgButtonChecked(hwnd,IDC_STRETCH2x2)
                            ? GM_STRETCH_2x2 : GM_STRETCH_1x1;
                        GM_prevMode = GM_chosenMode;
                        GM_startWindowed = IsDlgButtonChecked(hwnd,IDC_WINDOWED);
                        EndDialog(hwnd,IDC_OK);
                        }
                    break;
                case IDC_8BIT:
                case IDC_16BIT:
                case IDC_24BIT:
                case IDC_32BIT:
                    CheckDlgButton(hwnd,IDC_8BIT,LOWORD(wParam) == IDC_8BIT);
                    CheckDlgButton(hwnd,IDC_16BIT,LOWORD(wParam) == IDC_16BIT);
                    CheckDlgButton(hwnd,IDC_24BIT,LOWORD(wParam) == IDC_24BIT);
                    CheckDlgButton(hwnd,IDC_32BIT,LOWORD(wParam) == IDC_32BIT);
                    RefreshModeList(hwnd);
                    break;
#if 0
                case IDC_AUTO:
                case IDC_MICROSOFT:
                case IDC_SGI_OPENGL:
                case IDC_MESA:
                    CheckDlgButton(hwnd,IDC_AUTO,LOWORD(wParam) == IDC_AUTO);
                    CheckDlgButton(hwnd,IDC_MICROSOFT,LOWORD(wParam) == IDC_MICROSOFT);
                    CheckDlgButton(hwnd,IDC_SGI_OPENGL,LOWORD(wParam) == IDC_SGI_OPENGL);
                    CheckDlgButton(hwnd,IDC_MESA,LOWORD(wParam) == IDC_MESA);
                    switch (LOWORD(wParam)) {
                        case IDC_AUTO:
                            GM_driverOpt.openGLType = MGL_GL_AUTO;
                            break;
                        case IDC_MICROSOFT:
                            GM_driverOpt.openGLType = MGL_GL_MICROSOFT;
                            break;
                        case IDC_SGI_OPENGL:
                            GM_driverOpt.openGLType = MGL_GL_SGI;
                            break;
                        case IDC_MESA:
                            GM_driverOpt.openGLType = MGL_GL_MESA;
                            break;
                        }
                    RefreshModeList(hwnd);
                    break;
#endif
                }
            break;
        case WM_INITDIALOG:
            GM_centerWindow(hwnd,NULL,FALSE);
            /* Disable buttons according to flags from GM_init */
            if (!(GM_driverOpt.modeFlags & GM_MODE_8BPP))
                EnableWindow(GetDlgItem(hwnd,IDC_8BIT),FALSE);
            if (!(GM_driverOpt.modeFlags & GM_MODE_16BPP))
                EnableWindow(GetDlgItem(hwnd,IDC_16BIT),FALSE);
            if (!(GM_driverOpt.modeFlags & GM_MODE_24BPP))
                EnableWindow(GetDlgItem(hwnd,IDC_24BIT),FALSE);
            if (!(GM_driverOpt.modeFlags & GM_MODE_32BPP))
                EnableWindow(GetDlgItem(hwnd,IDC_32BIT),FALSE);
            if (!GM_driverOpt.useHWOpenGL) {
#if 0
                EnableWindow(GetDlgItem(hwnd,IDC_AUTO),FALSE);
                EnableWindow(GetDlgItem(hwnd,IDC_MICROSOFT),FALSE);
                EnableWindow(GetDlgItem(hwnd,IDC_SGI_OPENGL),FALSE);
                EnableWindow(GetDlgItem(hwnd,IDC_MESA),FALSE);
                CheckDlgButton(hwnd,IDC_AUTO,TRUE);
#endif
                }

            /* Check the default buttons to start with */
            CheckDlgButton(hwnd,IDC_WINDOWED,GM_startWindowed);
            if ((GM_driverOpt.modeFlags & GM_MODE_8BPP) && (GM_selectBits < 0 || GM_selectBits == 8))
                CheckDlgButton(hwnd,IDC_8BIT,TRUE);
            else if ((GM_driverOpt.modeFlags & GM_MODE_16BPP) && (GM_selectBits < 0 || GM_selectBits == 16))
                CheckDlgButton(hwnd,IDC_16BIT,TRUE);
            else if ((GM_driverOpt.modeFlags & GM_MODE_24BPP) && (GM_selectBits < 0 || GM_selectBits == 24))
                CheckDlgButton(hwnd,IDC_24BIT,TRUE);
            else
                CheckDlgButton(hwnd,IDC_32BIT,TRUE);
            CheckDlgButton(hwnd,IDC_STRETCH2x2,GM_windowedStretch == GM_STRETCH_2x2);
            if (GM_driverOpt.useHWOpenGL) {
#if 0
                if (GM_driverOpt.openGLType == MGL_GL_AUTO)
                    CheckDlgButton(hwnd,IDC_AUTO,TRUE);
                else if (GM_driverOpt.openGLType == MGL_GL_MICROSOFT)
                    CheckDlgButton(hwnd,IDC_MICROSOFT,TRUE);
                else if (GM_driverOpt.openGLType == MGL_GL_SGI)
                    CheckDlgButton(hwnd,IDC_SGI_OPENGL,TRUE);
                else
                    CheckDlgButton(hwnd,IDC_MESA,TRUE);
#endif
                }
            RefreshModeList(hwnd);
            return TRUE;
        }
    return FALSE;
}
#endif

/****************************************************************************
DESCRIPTION:
Display a dialog box to choose a fullscreen graphics mode

HEADER:
gm/gm.h

PARAMETERS:
mode            - Place to return the selected mode information
startWindowed   - True if use wishes to start windowed, false if not

RETURNS:
True if a mode was chosen, false on error or if the user clicked cancel.

REMARKS:
This function will bring up a dialog box allowing the user to interactively
choose the graphics mode to be used for fullscreen modes, as well as allowing
then to change the OpenGL implementation, WinDirect and DirectDraw support
and also force the game to start in a window or fullscreen. This is mostly a
convenience function which is great for debugging, testing and demonstration
purposes. Note that if you do call this function, you must add the resources
for the dialog box used to your application, which are located in the
SCITECH\INCLUDE\GM\GMDLG.RC resource file.
***************************************************************************/
ibool MGLAPI GM_chooseMode(
    GM_modeInfo *mode,
    ibool *startWindowed)
{
#ifdef __CONSOLE__
    static ibool firstTime = true;
    if (firstTime) {
        firstTime = false;
        EnumerateModes();
        return GM_findMode(mode,mode->xRes,mode->yRes,mode->bitsPerPixel);
        }
    return false;
#elif defined(__WINDOWS__)
    EnumerateModes();
    GM_startWindowed = *startWindowed;
    if (DialogBox(GM_hInstApp,MAKEINTRESOURCE(IDD_MAINDLG),NULL,(DLGPROC)MainDlgProc) != IDC_OK)
        return false;
    *startWindowed = GM_startWindowed;
    *mode = GM_dc.modeList[GM_chosenMode];
    mode->windowedStretch = GM_windowedStretch;
    return true;
#else
    #error OS not supported yet!
#endif
}

/****************************************************************************
DESCRIPTION:
Suspend application callback registered with the MGL.

PARAMETERS:
dc      - Display device context
flags   - Flags indicating what to do

RETURNS:
Suspend application status flag

REMARKS:
This callback is called when the MGL detects that we have to switch back to
GDI mode when the user has Alt-Tab'ed away from the game. Because we want
to keep the main game logic running (so networking etc. still runs) we set
the GM_doDraw flag to false while minimised on the task bar so the main
loop will skip the draw function.
***************************************************************************/
static int _ASMAPI GM_suspendAppProc(
    MGLDC *dc,
    int flags)
{
    if (flags == MGL_DEACTIVATE) {
        GM_doDraw = false;
        if (GM_suspendApp)
            return GM_suspendApp(dc,flags);
        }
    else if (flags == MGL_REACTIVATE) {
        GM_doDraw = true;
        if (GM_suspendApp)
            return GM_suspendApp(dc,flags);
        }
    return MGL_SUSPEND_APP;
}

/****************************************************************************
DESCRIPTION:
Creates the main window

RETURNS:
True if we succeeded in creating the window

REMARKS:
Creates the main window for the game framework. Note that we only every have
one main window for all event handling whether we are in a fullscreen mode
or in a windowed mode. The MGL will re-use the same window for fullscreen
modes so that we only need to register the main window with DirectSound
and other DirectX components once at init time.
***************************************************************************/
static ibool createMainWindow(void)
{
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    WNDCLASS    cls;
    DWORD       windowStyle;
    RECT        client,window;
    int         dx = GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
    int         dy = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
#endif

    /* Destroy any existing device contexts */
    if (GM_oglActive) {
        MGL_glDeleteContext(GM_dc.dc);
        GM_oglActive = false;
        }
    MGL_makeCurrentDC(NULL);
    if (GM_dc.dispdc) {
        MGL_destroyDC(GM_dc.dispdc);
        GM_dc.dispdc = NULL;
        }
    if (GM_dc.backdc) {
        MGL_destroyDC(GM_dc.backdc);
        GM_dc.backdc = NULL;
        }
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    /* Create a Window class for the fullscreen window in here, since we need
     * to register one that will do all our event handling for us.
     */
    if (!GM_classRegistered) {
        cls.hCursor         = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon           = LoadIcon(GM_hInstApp,MAKEINTRESOURCE(1));
        cls.lpszMenuName    = NULL;
        cls.lpszClassName   = GM_winClassName;
        cls.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
        cls.hInstance       = GM_hInstApp;
        cls.style           = CS_DBLCLKS;
        cls.lpfnWndProc     = GM_windowProc;
        cls.cbWndExtra      = 0;
        cls.cbClsExtra      = 0;
        if (!RegisterClass(&cls))
            MGL_fatalError("Unable to register Window Class!");
        GM_classRegistered = true;
        }

    /* Create the hidden main window until we need it */
    if (!GM_inited) {
        EnumerateModes();
        if (!GM_hwndUser) {
            /* Create the main window here */
            windowStyle = WS_SYSMENU | WS_MINIMIZEBOX |
                          WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION;
            if (GM_modeSwitch)
                windowStyle |= WS_MAXIMIZEBOX;
            GM_dc.mainWindow = CreateWindow(GM_winClassName,GM_winTitle,windowStyle,
                0,0,320+dx,200+dy,NULL,NULL,GM_hInstApp,NULL);
            if (!GM_dc.mainWindow)
                return false;
            }
        else {
            /* The user has created the main window, so we now have to
             * subclass the main window and install our window handler.
             */
            GM_dc.mainWindow = GM_hwndUser;
            GM_orgWinProc = (MGL_WNDPROC)SetWindowLong(GM_dc.mainWindow,GWL_WNDPROC, (LPARAM)GM_windowProc);
            }
        GetClientRect(GM_dc.mainWindow,&client);
        GetWindowRect(GM_dc.mainWindow,&window);
        GM_winAdjustX = (window.right - window.left) - (client.right - client.left);
        GM_winAdjustY = (window.bottom - window.top) - (client.bottom - client.top);
        GM_inited = true;
        }
    MGL_registerFullScreenWindow(GM_dc.mainWindow);
#else
    EnumerateModes();
#endif
    return true;
}

/****************************************************************************
DESCRIPTION:
Creates the rendering device context

PARAMETERS:
dc  - Main display device context

RETURNS:
True if we succeeded in creating the rendering device context

REMARKS:
Sets up the game framework for rendering by creating a system memory back
buffer if necessary if we dont have direct access to the device context
surface or we can't do hardware page flipping.
****************************************************************************/
static ibool createRenderDC(
    MGLDC *dc,
    ibool forceSysMem)
{
    pixel_format_t  pf,dispPF;
    int             bits,dispBits;

    GM_dc.dispdc = dc;
    if (forceSysMem || (MGL_maxPage(dc) == 0) || (MGL_surfaceAccessType(dc) == MGL_NO_ACCESS)) {
        /* Determine the format for the system memory back buffer */
        bits = dispBits = MGL_getBitsPerPixel(dc);
        MGL_getPixelFormat(dc,&dispPF);
        pf = dispPF;
        switch (bits) {
            case 32:
                /* Fall back to 24bpp if we dont support 32bpp modes */
                if (!(GM_driverOpt.modeFlags & GM_MODE_32BPP)) {
                    pf = GM_pf24bpp;
                    bits = 24;
                    }
            case 24:
                /* Fall back to 5:5:5 if we dont support 24bpp modes */
                if (!(GM_driverOpt.modeFlags & GM_MODE_24BPP)) {
                    pf = GM_pf15bpp;
                    bits = 15;
                    }
            case 15:
            case 16:
                /* Fall back to 8bpp if we dont support 15/16bpp modes */
                if (!(GM_driverOpt.modeFlags & GM_MODE_16BPP))
                    bits = 8;
                break;
            }

        /* First try to allocate an offscreen device context if possible,
         * as this will allow us to do accelerated rendering and blitting when
         * running in a window. If a system memory buffer is forced, we skip this.
         */
        if (!forceSysMem && (bits == dispBits) && (memcmp(&pf,&dispPF,sizeof(pf)) == 0)) {
            if ((GM_dc.backdc = MGL_createOffscreenDC(dc,GM_sizeX,GM_sizeY)) == NULL)
                return false;
            }

        /* Create a system memory back buffer for drawing if we couldn't
         * create an offscreen buffer.
         */
        if (!GM_dc.backdc) {
            if ((GM_dc.backdc = MGL_createMemoryDC(GM_sizeX,GM_sizeY,bits,&pf)) == NULL)
                return false;
            }

        /* Make our rendering device context the current device context */
        GM_dc.dc = GM_dc.backdc;
        MGL_makeCurrentDC(GM_dc.backdc);
        }
    else {
        /* Hardware double buffering */
        GM_dc.dc = GM_dc.dispdc = dc;
        GM_dc.backdc = NULL;
        }

    /* Start multi buffered mode if more than 1 page */
    if (MGL_maxPage(dc) > 0) {
#ifdef  DEBUG_FLIP
        GM_activePage = 0;
#else
        GM_activePage = 1;
#endif
        GM_visualPage = 0;
        MGL_setActivePage(dc,GM_activePage);
        MGL_setVisualPage(dc,GM_visualPage,false);
        MGL_clearDevice();
        }
    return true;
}

#if defined(__WINDOWS__) && !defined(__CONSOLE__)
/****************************************************************************
DESCRIPTION:
Switches to a windowed graphics mode

PARAMETERS:
width       - Width of the window client area
height      - Height of the window client area
stretch     - Flags to determine the stretch mode to use
force8bit   - True to force memory back buffer to 8 bits per pixel

RETURNS:
True on success, false on error

REMARKS:
This function sets a windowed mode for the game with the memory back buffer
set to a size of (width,height).
****************************************************************************/
static ibool SetWindowedMode(
    int width,
    int height,
    int stretch,
    ibool forceSysMem)
{
    MGLDC   *dc;
    ibool   foundSize;
    RECT    window;

    /* Find the dimensions of the window */
    GM_inModeSet = true;
    GM_sizeX = width;
    GM_sizeY = height;
    if (GM_sizeX >= GM_deskX || GM_sizeY >= GM_deskY) {
        GM_sizeX = GM_deskX-GM_winAdjustX;
        GM_sizeY = GM_deskY-GM_winAdjustY;
        }
    foundSize = false;
    while (!foundSize) {
        switch (GM_winStretch = stretch) {
            case GM_STRETCH_1x1:
                GM_winSizeX = GM_sizeX;
                GM_winSizeY = GM_sizeY;
                break;
            case GM_STRETCH_1x2:
                GM_winSizeX = GM_sizeX;
                GM_winSizeY = GM_sizeY * 2;
                break;
            case GM_STRETCH_2x2:
                GM_winSizeX = GM_sizeX * 2;
                GM_winSizeY = GM_sizeY * 2;
                break;
            default:
                return false;
            }
        foundSize = true;
        if (GM_winSizeX >= GM_deskX || GM_winSizeY >= GM_deskY) {
            foundSize = false;
            if (stretch == GM_STRETCH_1x1)
                break;
            stretch = GM_STRETCH_1x1;
            }
        }

    /* Create the window and display it */
    GM_fullScreen = false;
    if (!createMainWindow())
        return false;

    /* Resize our window to the real size that we want and center it */
    if (GM_winPosX == -1) {
        /* This is the first time the window has been created in windowed
         * modes, so create it with the correct size and center it.
         */
        MoveWindow(GM_dc.mainWindow,0,0,GM_winSizeX+GM_winAdjustX,GM_winSizeY+GM_winAdjustY,TRUE);
        GM_centerWindow(GM_dc.mainWindow,NULL,TRUE);
        GetWindowRect(GM_dc.mainWindow,&window);
        GM_winPosX = window.left;
        GM_winPosY = window.top;
        }
    else {
        /* We have already been in windowed modes, so restore the previous
         * window position and size since the last time we went to fullscreen
         * modes.
         */
        MoveWindow(GM_dc.mainWindow,GM_winPosX,GM_winPosY,
            GM_winSizeX+GM_winAdjustX,GM_winSizeY+GM_winAdjustY,TRUE);
        }
    ShowWindow(GM_dc.mainWindow,SW_SHOW);
    SetFocus(GM_dc.mainWindow);
    SetActiveWindow(GM_dc.mainWindow);
    SetForegroundWindow(GM_dc.mainWindow);
    if ((dc = MGL_createWindowedDC(GM_dc.mainWindow)) == NULL)
        return false;
    if (MGL_glHaveHWOpenGL())
        GM_dc.modeFlags = MGL_HAVE_ACCEL_3D;
    else
        GM_dc.modeFlags = 0;
    if (GM_sysPalNoStatic && (dc->mi.bitsPerPixel == 8)) {
        HDC hdc = GetDC(NULL);
        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
        ReleaseDC(NULL,hdc);
        }
    if (!createRenderDC(dc,forceSysMem))
        return false;
    GM_inModeSet = false;
    return true;
}
#endif

/****************************************************************************
DESCRIPTION:
Switches to a fullscreen graphics mode

PARAMETERS:
mode        - MGL mode to initialize
pages       - Number of pages for hardware buffering
stretch     - Flags to determine the stretch mode to use
forceSysMem - Flag to force a system memory buffer for drawing
stereo      - True to enable stereo rendering

HEADER:
gm/gm.h

RETURNS:
True on success, false on error

REMARKS:
This function sets a fullscreen MGL mode for the game.
****************************************************************************/
static ibool SetFullScreenMode(
    int mode,
    int pages,
    int stretch,
    int refreshRate,
    ibool forceSysMem,
    ibool stereo)
{
    MGLDC   *dc;
    int     maxPages;

    /* Initialize the fullscreen mode */
    GM_inModeSet = true;
    GM_fullScreen = true;
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    if (GM_dc.mainWindow && GM_winPosX != -1) {
        /* Save position of current window so we can restore it when we
         * switch back to windowed mode from fullscreen modes. Note that
         * if we have not created our window yet (ie: going directly to
         * fullscreen on startup), we dont save the window position
         * since we want to create the initial window centered on the screen.
         */
        RECT    window;
        GetWindowRect(GM_dc.mainWindow,&window);
        GM_winPosX = window.left;
        GM_winPosY = window.top;
        }
#endif
    if (!createMainWindow())
        return false;
    if (stereo) {
        maxPages = MGL_availablePages(mode)/2;
        if (pages > maxPages)
            pages = maxPages;
        if ((dc = MGL_createStereoDisplayDC(mode,pages,refreshRate)) == NULL)
            return false;
        }
    else {
        maxPages = MGL_availablePages(mode);
        if (pages > maxPages)
            pages = maxPages;
        if ((dc = MGL_createDisplayDC(mode,pages,refreshRate)) == NULL)
            return false;
        }
    GM_dc.modeFlags = MGL_modeFlags(mode);
    MGL_makeCurrentDC(dc);
    GM_winSizeX = MGL_sizex(dc)+1;
    GM_winSizeY = MGL_sizey(dc)+1;
    switch (GM_winStretch = stretch) {
        case GM_STRETCH_1x1:
            GM_sizeX = GM_winSizeX;
            GM_sizeY = GM_winSizeY;
            break;
        case GM_STRETCH_1x2:
            GM_sizeX = GM_winSizeX;
            GM_sizeY = GM_winSizeY / 2;
            forceSysMem = true;
            break;
        case GM_STRETCH_2x2:
            GM_sizeX = GM_winSizeX / 2;
            GM_sizeY = GM_winSizeY / 2;
            forceSysMem = true;
            break;
        default:
            return false;
        }
    MGL_setSuspendAppCallback(GM_suspendAppProc);
    if (!createRenderDC(dc,forceSysMem))
        return false;
    GM_inModeSet = false;
    return true;
}

/****************************************************************************
DESCRIPTION:
Switches to a fullscreen or windowed graphics mode

PARAMETERS:
info        - Structure describing the mode to initialize
windowed    - True if the mode should be in a window
pages       - Number of pages for hardware buffering
refreshRate - Desired refresh rate for fullscreen display mode
forceSysMem - Flag to force a system memory buffer for all drawing
stereo      - True to enable stereo rendering

HEADER:
gm/gm.h

RETURNS:
True on success, false on error

REMARKS:
This function sets a graphics mode for the game given the passed in mode
information. If the windowed parameter is set to true, a windowed mode will
be set otherwise a fullscreen graphics mode will be set. You must pass in
one of the modes listed in the GMDC modeList returned from GM_init to this
function. When the Game Framework switches to the windowed
equivalent to a fullscreen graphics mode, we search for the closest 1:1
aspect ratio window size for the mode that corresponds to the fullscreen mode
chosen. Hence for a 320x200 fullscreen mode we choose a 320x240 window size,
for 320x400 and 320x480 we choose a 320x240 window size and for 640x350 and
640x400 we choose a 640x480 window size. This way we won't end up with a window
on the desktop that is not a 1:1 aspect ratio and desktop modes are generally
1:1 aspect ratio.

The number of pages passed in is used to allocate the specified number of
hardware video memory pages for multi-buffering and may be any value. The
Game Framework will however lower this value to the maximum that the hardware
supports, and if only 1 hardware page is available a system memory back buffer
will be created automatically. Hence if you want to always try and use
triple buffering if available, pass a value of 3 when you call this function.

The refresh rate parameter is used to select a desired refresh rate for the
fullscreen display mode. If that refresh rate cannot be achieved due to
hardware constraints, the next lowest available refresh rate will be used.
If you wish to use the user default setting for the refresh rate (always
a good idea unless you need refresh control, such as for stereo), simply
pass in a value of MGL_DEFAULT_REFRESH.

The stereo flag is used to enable support for either software or hardware
stereo modes in the MGL. This will fail if the underlying hardware device
does not support stereo capabilities. Stereo can be used with OpenGL
using Mesa, SGI OpenGL, SciTech DirectGL or Microsoft OpenGL (Microsoft
OpenGL requires a stereo ready OpenGL driver to be installed).

Note also that the Game Framework automatically handles switching between
windowed and fullscreen modes on the fly. By default the Game Framework
contains code to provide two methods of switching to fullscreen modes when
running in windowed modes:

1   - When the user hits the /Alt-Enter/ key combination
2   - When the user clicks the /Maximise/ button on the games title bar

Likewise when the game is running in a fullscreen mode and the user hits the
/Alt-Enter/ key, the video mode will automatically be switched to windowed
mode. Unless you have registered a mode switch callback with
GM_setModeSwitchFunc, on the fly switching between fullscreen and windowed
modes is disabled.

Note: You may call this function while already in a windowed or fullscreen
      graphics mode, which will cause the current graphics mode to be changed
      on the fly.

Note: If you wish to force the Game Framework to always create a system
      memory buffer for rendering, regardless of how many display pages
      are available, set the forceSysMem field to true when calling this
      function. Note also that if you have requested multiple buffers, all
      those buffers will still be used so you will see no tearing, however
      all rendering will be performed to a system buffer which will be copied
      to the screen when you swap the buffers with GM_swapBuffers.

Drawing directly to a hardware linear framebuffer is usually extremely
fast and will provide the most efficient method of rendering. However
if your rendering requires reads from the framebuffer (for effects such
as blending), this will be very slow over the PCI bus and your code will
be faster if you force a system member back buffer.

Note: If the user dynamically switches the resolution or color depth of the
      windows desktop while your application is running, if you have registered
      a mode switch callback with the Game Framework (via GM_setModeSwitchFunc)
      then the Game Framework will switch to a new windowed mode to cause the
      memory back buffers to be re-allocated for the most optimal format
      for the Windows display mode.

See Also:
GM_init, GM_setModeSwitchFunc
****************************************************************************/
ibool MGLAPI GM_setModeExt(
    GM_modeInfo *info,
    ibool windowed,
    int pages,
    int refreshRate,
    ibool forceSysMem,
    ibool stereo)
{
    GM_cntMode = *info;
    GM_requestPages = pages;
    GM_requestRefreshRate = refreshRate;
    GM_requestForceSysMem = forceSysMem;
    GM_requestStereo = stereo;
#if defined(__CONSOLE__)
    (void)windowed;
#elif defined(__WINDOWS__)
    if (windowed) {
        int xRes = info->xRes,yRes = info->yRes;

        /* We don't currently support stereo rendering in a window. We
         * *could* actually support this, if we enabled interleaved stereo
         * rendering to the memory back buffer which we then blit to the
         * screen. The desktop display mode must then be interlaced mode
         * for this to work (preferably with a high refresh rate).
         */
        if (stereo)
            return false;

        /* Instead of just setting the exact windowed equivalent to the
         * corresponding fullscreen mode, we search for the closest 1:1
         * aspect ratio window size for the mode that corresponds to the
         * fullscreen mode chosen. Ie: for 320x200 we choose 320x240,
         * for 320x400 and 320x480 we choose 320x240 and for 640x350 and
         * 640x400 we choose 640x480. This way we wont end up with a window
         * on the desktop with stretched graphics!
         */
        if ((xRes == 320 && yRes == 200) || (xRes == 320 && (yRes == 400 || yRes == 480))) {
            xRes = 320;
            yRes = 240;
            }
        if (xRes == 640) {
            xRes = 640;
            yRes = 480;
            }
        return SetWindowedMode(xRes,yRes,info->windowedStretch,forceSysMem);
        }
    else
#endif
        {
        if (info->mode == GM_WINDOWED_ONLY)
            return false;
        return SetFullScreenMode(info->mode,pages,info->stretch,refreshRate,forceSysMem,stereo);
        }
}

/****************************************************************************
DESCRIPTION:
Switches to a fullscreen or windowed graphics mode

PARAMETERS:
info        - Structure describing the mode to initialize
windowed    - True if the mode should be in a window
pages       - Number of pages for hardware buffering
forceSysMem - Flag to force a system memory buffer for all drawing

HEADER:
gm/gm.h

RETURNS:
True on success, false on error

REMARKS:
This function is now obsolete, and has been replaced by the GM_setModeExt
function. You should use the new function for all new application
development.

See Also:
GM_init, GM_setModeSwitchFunc, GM_setModeExt
****************************************************************************/
ibool MGLAPI GM_setMode(
    GM_modeInfo *info,
    ibool windowed,
    int pages,
    ibool forceSysMem)
{
    return GM_setModeExt(info,windowed,pages,MGL_DEFAULT_REFRESH,forceSysMem,false);
}

/****************************************************************************
DESCRIPTION:
Starts OpenGL graphics for the game

PARAMETERS:
flags   - OpenGL Rendering Context flags

HEADER:
gm/gm.h

RETURNS:
True on success, false on error

REMARKS:
This functions enables support for the OpenGL 3D API for the Game Framework,
and after this call you must do all rendering via calls to the OpenGL API.
The flags parameter (of type MGL_glContextFlagsType) is used to specify the
type of OpenGL rendering context that you want, such as if you want RGB or
color index mode, single or double buffering, an alpha buffer, an
accumulation buffer, a depth buffer (z-buffer) and a stencil buffer.

If you pass in a value of MGL_GL_VISUAL for the flags parameter, the MGL
will use the OpenGL visual that was set by a previous call to
MGL_glSetVisual. Hence if you require more control over the type of OpenGL
rendering context that is created, you can call MGL_glChooseVisual and
MGL_glSetVisual before calling this function. Note that you should *not* call
MGL_glCreateContext when using the Game Framework, but call this function
instead.

Note: After this function has been called, the current rendering context will
      have been made the current OpenGL rendering context with a call to
      MGL_glMakeCurrent, so you can simply start issuing OpenGL rendering
      commands to start drawing after calling this function.

See Also:
GM_setMode, MGL_glChooseVisual, MGL_glSetVisual
****************************************************************************/
ibool MGLAPI GM_startOpenGL(
    MGL_glContextFlagsType flags)
{
    MGL_makeCurrentDC(NULL);
    if (GM_dc.modeFlags & MGL_HAVE_ACCEL_3D) {
        /* We are running with hardware accelerated OpenGL drivers, so destroy
         * any existing memory back buffer before starting OpenGL. We also always
         * use the Microsoft OpenGL libraries if we are running on a DEC Alpha,
         * until such a time that SGI gets their OpenGL libraries ported to the DEC Alpha.
         */
        if (GM_dc.backdc) {
            MGL_destroyDC(GM_dc.backdc);
            GM_dc.dc = GM_dc.dispdc;
            GM_dc.backdc = NULL;
            }
        }
    else {
        /* We are running fullscreen or we are running in a window on
         * a system without hardware OpenGL drivers, in which case we
         * will render to a system memory buffer and do the blitting ourselves.
         * Doing this allows much tighter integration between OpenGL and
         * MGL rendering functions, since both rendering engines can acess
         * the surface memory directly.
         */
        if (GM_dc.backdc)
            flags &= ~MGL_GL_DOUBLE;
        }
    if (!MGL_glCreateContext(GM_dc.dc,flags))
        return false;
    MGL_glMakeCurrent(GM_dc.dc);
    GM_oglActive = true;

    /* If the device context is an 8bpp mode, then we also need to copy the
     * palette from our OpenGL device context to the main rendering device
     * context and then realize the palette. Note that we pass in a flag of
     * -1 to the GM_realizePalette function to force the MGL to re-build the
     * palette from scratch in windowed modes.
     */
    if (GM_dc.dc->mi.bitsPerPixel == 8) {
        palette_t pal[256];
        MGL_getPalette(GM_dc.dc,pal,256,0);
        GM_setPalette(pal,256,0);
        GM_realizePalette(256,0,-1);
        }
    return true;
}

/****************************************************************************
DESCRIPTION:
Set active rendering to the left stereo buffer.

HEADER:
gm/gm.h

REMARKS:
This function sets the active rendering buffer for all subsequent drawing
commands to the left stereo buffer.

See Also:
GM_setRightBuffer
****************************************************************************/
void MGLAPI GM_setLeftBuffer(void)
{
    MGL_setActivePage(GM_dc.dispdc,GM_activePage &= ~MGL_RIGHT_BUFFER);
}

/****************************************************************************
DESCRIPTION:
Set active rendering to the right stereo buffer.

HEADER:
gm/gm.h

REMARKS:
This function sets the active rendering buffer for all subsequent drawing
commands to the right stereo buffer.

See Also:
GM_setLeftBuffer
****************************************************************************/
void MGLAPI GM_setRightBuffer(void)
{
    MGL_setActivePage(GM_dc.dispdc,GM_activePage |= MGL_RIGHT_BUFFER);
}

/****************************************************************************
DESCRIPTION:
Start stereo display mode.

HEADER:
gm/gm.h

REMARKS:
This function starts stereo display mode for 3D stereo viewing. Stereo
display mode can be turned on and off, which is useful when displaying
static screens such as menus etc.

See Also:
GM_stopStereo
****************************************************************************/
void MGLAPI GM_startStereo(void)
{
    MGL_startStereo(GM_dc.dispdc);
}

/****************************************************************************
DESCRIPTION:
Stop stereo display mode.

HEADER:
gm/gm.h

REMARKS:
This function stops stereo display mode for 3D stereo viewing. Stereo
display mode can be turned on and off, which is useful when displaying
static screens such as menus etc.

See Also:
GM_startStereo
****************************************************************************/
void MGLAPI GM_stopStereo(void)
{
    MGL_stopStereo(GM_dc.dispdc);
}

/****************************************************************************
DESCRIPTION:
Swaps the hardware display buffers

PARAMETERS:
waitVRT - Wait for vertical retrace flag

REMARKS:
Swaps the hardware buffers if we have hardware double buffering enabled.
****************************************************************************/
static void swapHardwareBuffers(
    int waitVRT)
{
    if (GM_oglActive && !GM_dc.backdc) {
        MGL_glSwapBuffers(GM_dc.dc,waitVRT);
        }
    else {
        if (GM_dc.dispdc->mi.maxPage > 0) {
            GM_visualPage = GM_activePage;
            GM_activePage = (GM_activePage+1) % (GM_dc.dispdc->mi.maxPage+1);
#ifdef  DEBUG_FLIP
            GM_visualPage = GM_activePage;
#endif
            MGL_setActivePage(GM_dc.dispdc,GM_activePage);
            MGL_setVisualPage(GM_dc.dispdc,GM_visualPage & ~MGL_RIGHT_BUFFER,waitVRT);
            }
        else if (waitVRT != MGL_dontWait)
            MGL_vSync(GM_dc.dispdc);
        }
}

/****************************************************************************
DESCRIPTION:
Swaps the display buffers for the game

PARAMETERS:
waitVRT - Wait for vertical retrace flag

HEADER:
gm/gm.h

REMARKS:
Swaps the display buffers for the Game Framework game. If there are multiple
hardware display pages enabled for the game, the waitVRT flag (of
MGL_waitVRTFlagType) is used to determine if the MGL should wait for the
vertical retrace before swapping display pages or not.

If you started the Game Framework and requested a system memory back buffer,
enabled stretching or there was only one hardware display page available,
the Game Framework will blit the entire system memory back buffer to the
display device context and then perform the hardware page flip.

Note: If you are intentionally using a system memory back buffer, you may
      want to maintain a set of dirty rectangles for your display pages and
      call GM_swapDirtyBuffers to swap only the dirty portions of the frames
      to speed things up. This is most useful for games that don't update
      large portions of the screen very frequently.

See Also:
GM_swapDirtyBuffers, GM_setLeftBuffer, GM_setRightBuffer
****************************************************************************/
void MGLAPI GM_swapBuffers(
    MGL_waitVRTFlagType waitVRT)
{
    if (GM_dc.backdc) {
        if (GM_oglActive)
            glFlush();
        if (GM_winStretch != GM_STRETCH_1x1) {
            MGL_stretchBltCoord(GM_dc.dispdc,GM_dc.backdc,0,0,GM_sizeX,GM_sizeY,
                0,0,GM_winSizeX,GM_winSizeY,MGL_REPLACE_MODE);
            }
        else {
            MGL_bitBltCoord(GM_dc.dispdc,GM_dc.backdc,0,0,GM_sizeX,GM_sizeY,0,0,MGL_REPLACE_MODE);
            }
        }
    swapHardwareBuffers(waitVRT);
}

/****************************************************************************
DESCRIPTION:
Stretchs a rectangle from the back buffer to the display

PARAMETERS:
r   - Rectangle to stretch

REMARKS:
Stretches the rectangle from the system memory buffer to the display buffer.
This function is called by the MGL's region traversal routines to traverse
the region of dirty rectangles list.
****************************************************************************/
static void _ASMAPI stretchRect(
    const rect_t *r)
{
    if (GM_winStretch == GM_STRETCH_1x2) {
        MGL_stretchBltCoord(GM_dc.dispdc,GM_dc.backdc,
            r->left,r->top,r->right,r->bottom,
            r->left,r->top*2,r->right,r->bottom*2,MGL_REPLACE_MODE);
        }
    else {
        MGL_stretchBltCoord(GM_dc.dispdc,GM_dc.backdc,
            r->left,r->top,r->right,r->bottom,
            r->left*2,r->top*2,r->right*2,r->bottom*2,MGL_REPLACE_MODE);
        }
}

/****************************************************************************
DESCRIPTION:
Blits a rectangle from the back buffer to the display

PARAMETERS:
r   - Rectangle to blit

REMARKS:
Blits the rectangle from the system memory buffer to the display buffer.
This function is called by the MGL's region traversal routines to traverse
the region of dirty rectangles list.
****************************************************************************/
static void _ASMAPI blitRect(
    const rect_t *r)
{
    MGL_bitBlt(GM_dc.dispdc,GM_dc.backdc,*r,r->left,r->top,MGL_REPLACE_MODE);
}

//#define   DEBUG_DIRTY_RECT

/* Callback function to draw the rectangle for the rectangles in the
 * dirty rectangle region.
 */

#ifdef  DEBUG_DIRTY_RECT
static void _ASMAPI drawRect(const rect_t *r)
{
    MGL_setColorCI(255);
    MGL_rect(*r);
}
#endif

/****************************************************************************
DESCRIPTION:
Swaps the display buffers for the game with dirty rectangles

PARAMETERS:
dirty   - Region of dirty rectangles to blit
waitVRT - Wait for vertical retrace flag

HEADER:
gm/gm.h

REMARKS:
Swaps the display buffers for the Game Framework game by blitting the list of
dirty rectangles to the display. The list of dirty rectangles is passed in
as an MGL region, which you can construct using the MGL region manipulation
functions.

If there are multiple hardware display pages enabled for the game, the waitVRT
flag (of MGL_waitVRTFlagType) is used to determine if the MGL should wait for
the vertical retrace before swapping display pages or not.

Note: You should make sure you first call MGL_optimiseRegion before you call
      this function to minimise the number of rectangles in the dirty
      rectangle list. If you dont do this, the result will be the same but it
      may take longer to perform the blitting.

Note: If you did not specifically request a system memory back buffer, this
      function will behave identically to GM_swapBuffers and no blitting will
      occur.

See Also:
GM_swapBuffers
****************************************************************************/
void MGLAPI GM_swapDirtyBuffers(
    region_t *dirty,
    MGL_waitVRTFlagType waitVRT)
{
    if (GM_dc.backdc) {
        if (GM_oglActive)
            glFlush();
        if (GM_winStretch != GM_STRETCH_1x1)
            MGL_traverseRegion(dirty,stretchRect);
        else
            MGL_traverseRegion(dirty,blitRect);
#ifdef  DEBUG_DIRTY_RECT
        MGL_makeCurrentDC(GM_dc.dispdc);
        MGL_traverseRegion(dirty,drawRect);
        MGL_makeCurrentDC(GM_dc.dc);
#endif
        }
    swapHardwareBuffers(waitVRT);
}

/****************************************************************************
DESCRIPTION:
Sets the color palette entries

PARAMETERS:
pal         - Array of palette values to set
numColors   - Number of colors to set
startIndex  - Starting index in device context palette

HEADER:
gm/gm.h

REMARKS:
This function sets the palette values for the currently activate Game
Framework device context to the values passed in pal. Note that this
function does not program the hardware palette, but simply updates the
internal palette values for the MGL device context. Once you have set the
values in the palette, you should then call GM_realizePalette to program
the hardware palette entries from the values currently stored in the
MGL display device context.

Note: If we have a memory back buffer we also set the palette values for
      this to ensure we have an identity palette for blit operations for
      maximum speed.

See Also:
GM_realizePalette
****************************************************************************/
void MGLAPI GM_setPalette(
    palette_t *pal,
    int numColors,
    int startIndex)
{
    if (GM_oglActive && !GM_dc.backdc) {
        MGL_glSetPalette(GM_dc.dc,pal,numColors,startIndex);
        }
    else {
        MGL_setPalette(GM_dc.dispdc,pal,numColors,startIndex);
        if (GM_dc.backdc)
            MGL_setPalette(GM_dc.backdc,pal,numColors,startIndex);
        }
}

/****************************************************************************
DESCRIPTION:
Programs the hardware color palette

PARAMETERS:
numColors   - Number of colors to set
startIndex  - Starting index in device context palette
waitVRT     - True to wait for vertical retrace

HEADER:
gm/gm.h

REMARKS:
This function programs the hardware color palette from the current display
device context. You should first call GM_setPalette to set the color palette
entries to the values that you require before calling this function.

Note: If we have a memory back buffer we also realize the palette values for
      this to ensure we have an identity palette for blit operations for
      maximum speed.

See Also:
GM_setPalette
****************************************************************************/
void MGLAPI GM_realizePalette(
    int numColors,
    int startIndex,
    int waitVRT)
{
    if (GM_oglActive && !GM_dc.backdc) {
        /* Note that we pass a value of -1 for waitVRT to the realize
         * palette function to force the palette to be re-built rather
         * than animated with AnimatePalette in a window. If we use
         * AnimatePalette, OpenGL wont see the palette change and it's
         * memory back buffer will have an incorrect palette.
         */
        MGL_glRealizePalette(GM_dc.dc,numColors,startIndex,-1);
        }
    else {
        MGL_realizePalette(GM_dc.dispdc,numColors,startIndex,waitVRT);
        if (GM_dc.backdc)
            MGL_realizePalette(GM_dc.backdc,numColors,startIndex,waitVRT);
        }
}

/****************************************************************************
DESCRIPTION:
Processes all events for the current iteration of the main loop

HEADER:
gm/gm.h

REMARKS:
This function is the event processing handler for the main event loop
for the Game Framework. This function is responsible for farming out events
to the event handling callbacks registered by the game.

Note: If you wish to process messages in your game using a regular Windows
      window procedure, replace the GM_mainLoop function with your own and
      call GM_processEventsWin instead.

See Also:
GM_mainLoop, GM_processEventsWin, GM_cleanup
****************************************************************************/
void MGLAPI GM_processEvents(void)
{
    event_t evt;

    /* Handle any pending events */
    while (EVT_getNext(&evt,EVT_EVERYEVT)) {
        switch (evt.what) {
            case EVT_KEYDOWN:
                if (GM_keyDown)
                    GM_keyDown(&evt);
                break;
            case EVT_KEYREPEAT:
                if (GM_keyRepeat)
                    GM_keyRepeat(&evt);
                break;
            case EVT_KEYUP:
                if (GM_keyUp)
                    GM_keyUp(&evt);
                break;
            case EVT_MOUSEDOWN:
                if (GM_mouseDown)
                    GM_mouseDown(&evt);
                break;
            case EVT_MOUSEUP:
                if (GM_mouseUp)
                    GM_mouseUp(&evt);
                break;
            case EVT_MOUSEMOVE:
                if (GM_mouseMove)
                    GM_mouseMove(&evt);
                break;
            default:
                if (GM_event)
                    GM_event(&evt);
                break;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Cleans up the Game Framework and restores original mode

HEADER:
gm/gm.h

REMARKS:
This function calls the users registered exit function, exits the MGL
(which puts the system back into text mode for DOS or GDI mode for Windows)
and then does some final Game Framework cleanup. Normally you wont call
this function unless you have replaced the GM_mainLoop function with your
own custom version.

See Also:
GM_mainLoop, GM_processEvents, GM_processEventsWin
****************************************************************************/
void MGLAPI GM_cleanup(void)
{
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    if (GM_sysPalNoStatic) {
        HDC hdc = GetDC(NULL);
        SetSystemPaletteUse(hdc, SYSPAL_STATIC);
        ReleaseDC(NULL,hdc);
        if (GM_dc.dc && GM_dc.dispdc)
            GM_realizePalette(256,0,-1);
        }
#endif
    if (GM_doExit)
        GM_doExit();

    /* The following call to MGL_exit will destroy all our DC's, so clear our
     * internal pointers so that we dont call functions for destroyed
     * DC's in our windows message processing functions.
     */
    GM_dc.dc = NULL;
    GM_dc.dispdc = NULL;
    GM_dc.backdc = NULL;

    /* Exit the MGL */
    MGL_exit();

#if defined(__WINDOWS__) && !defined(__CONSOLE__)
    if (GM_orgWinProc) {
        /* Unsubclass the window if the app created it, and hide it */
        SetWindowLong(GM_dc.mainWindow,GWL_WNDPROC, (LPARAM)GM_orgWinProc);
        GM_orgWinProc = NULL;
        ShowWindow(GM_dc.mainWindow,SW_HIDE);
        }
    else {
        /* Destroy the window if we created it */
        DestroyWindow(GM_dc.mainWindow);
        }
    ClearMessageQueue();
#endif
    GM_detected = false;
    ResetGlobals();
}

/****************************************************************************
DESCRIPTION:
Runs the main loop for the Game Framework

HEADER:
gm/gm.h

REMARKS:
This function is the main event loop for the Game Framework, and controls
execution of the game until the game exists. This function is responsible
for farming out events to the event handling callbacks registered by the
game along with calling the game's gameLogic and draw callbacks. Note
that if we are suspended on the task bar we continue to process events and
call the gameLogic function (so networking can continue) however we skip
the call the to drawing function to avoid attempting to write to video memory
we no longer own.

The main loop is a simple loop constructed of the following steps (note that
MyGameLogic and MyDrawFrame are assumed to be your game logic and draw
functions that you would normally register with the Game Framework before
calling the GM_mainLoop function):

    GM_exitMainLoop = false;
    while (!GM_exitMainLoop) {
        GM_processEvents();
        MyGameLogic();
        if (GM_doDraw)
            MyDrawFrame();
        }
    GM_cleanup();

If you wish to replace the main loop with your own, you can take the
existing main loop code and replace it with your own variations. The
GM_processEvents functions processes events via the MGL event handling
functions and dispatches them to the registered event callbacks. If you wish
you can call GM_processEventsWin instead, which will simply flush the
windows message queue and you will be expected to handle all keyboard and
mouse events with the window procedure registered with GM_registerEventProc.
Hence an alternate main loop with all event handling done in regular window
procedure would be coded as follows:

    GM_exitMainLoop = false;
    GM_registerEventProc(MyWindowProc);
    while (!GM_exitMainLoop) {
        GM_processEventsWin();
        MyGameLogic();
        if (GM_doDraw)
            MyDrawFrame();
        }
    GM_cleanup();

Note: If you do replace the main loop with your own, /make absolutely sure/
      that you don't call your draw function if the global variable
      GM_doDraw is set to false, otherwise your game could lock the system
      when the user Alt-Tabs away and back to the desktop. Also make sure
      that you call GM_cleanup on the way out.

Note: To exit the main loop, call the GM_exit function which lets the main
      loop know that it should exit on the next iteration.

Note: You /must/ call GM_processEvents or GM_processEventsWin in your main
      loop to ensure that the Game Framework has a chance to process some
      internal functions every loop.

See Also:
GM_init, GM_setDrawFunc, GM_setGameLogicFunc, GM_exit, GM_processEvents,
GM_processEventsWin, GM_cleanup
****************************************************************************/
void MGLAPI GM_mainLoop(void)
{
    GM_exitMainLoop = false;
    while (!GM_exitMainLoop) {
        GM_processEvents();
        if (GM_gameLogic)
            GM_gameLogic();
        if (GM_doDraw && GM_draw)
            GM_draw();
        }
    GM_cleanup();
}

/****************************************************************************
DESCRIPTION:
Tells the Game Framework to exit the main loop

HEADER:
gm/gm.h

REMARKS:
This function simple lets the Game Framework know that the user wants to
exit and to clean up and exit from the main loop. After a call to this
function, the Game Framework will return from GM_mainLoop when it begins
to process the next frame.

See Also:
GM_mainLoop
****************************************************************************/
void MGLAPI GM_exit(void)
{ GM_exitMainLoop = true; }

/****************************************************************************
DESCRIPTION:
Returns true if we are running on Windows 95/98/Me

HEADER:
gm/gm.h

RETURNS:
True when running on Windows 95/98/Me

REMARKS:
This function returns the value of the global variable GM_haveWin95, and
is primarily intended as an interface function in the DLL version of the
Game Framework used for Borland Delphi.
****************************************************************************/
ibool MGLAPI GM_getHaveWin95(void)
{ return GM_haveWin95; }

/****************************************************************************
DESCRIPTION:
Returns true if we are running on Windows NT

HEADER:
gm/gm.h

RETURNS:
True when running on Windows NT

REMARKS:
This function returns the value of the global variable GM_haveWinNT, and
is primarily intended as an interface function in the DLL version of the
Game Framework used for Borland Delphi.
****************************************************************************/
ibool MGLAPI GM_getHaveWinNT(void)
{ return GM_haveWinNT; }

/****************************************************************************
DESCRIPTION:
Returns true if drawing is allowed

HEADER:
gm/gm.h

RETURNS:
True if drawing is allowed

REMARKS:
This function returns the value of the global variable GM_doDraw, and
is primarily intended as an interface function in the DLL version of the
Game Framework used for Borland Delphi.
****************************************************************************/
ibool MGLAPI GM_getDoDraw(void)
{ return GM_doDraw; }

/****************************************************************************
DESCRIPTION:
Returns true if the main loop should exit

HEADER:
gm/gm.h

RETURNS:
True if the main loop should exit

REMARKS:
This function returns the value of the global variable GM_exitMainLoop, and
is primarily intended as an interface function in the DLL version of the
Game Framework used for Borland Delphi.
****************************************************************************/
ibool MGLAPI GM_getExitMainLoop(void)
{ return GM_exitMainLoop; }

/****************************************************************************
DESCRIPTION:
Finds an available mode that has the desired resolution and color depth.

PARAMETERS:
mode    - Place to store the returned graphics mode information
xRes    - X resolution of the mode to find
yRes    - Y resolution of the mode to find
bits    - Color depth of the mode to find (-1 for don't care)

HEADER:
gm/gm.h

RETURNS:
True if a valid mode was found, false if not found.

REMARKS:
This function searches the list of available graphics modes for one that
matches the desired resolution and color depth. This is most useful for
finding a good default graphics mode to start your game in if the
user has not selected a default mode yet. Note that this function searches
for the mode from the top of the list backwards, so that we find the
highest performance 320x200 and 320x240 modes (i.e.: the Linear Framebuffer
modes rather than the VGA ModeX or Standard VGA modes).
****************************************************************************/
ibool MGLAPI GM_findMode(
    GM_modeInfo *mode,
    int xRes,
    int yRes,
    int bits)
{
    int i;

    for (i = 0; i < GM_dc.numModes; i++) {
        if (GM_dc.modeList[i].xRes == xRes && GM_dc.modeList[i].yRes == yRes) {
            if (bits != -1) {
                if (GM_dc.modeList[i].bitsPerPixel != bits)
                    continue;
                }
            *mode = GM_dc.modeList[i];
            return true;
            }
        }
    return false;
}

