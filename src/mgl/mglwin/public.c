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
* Environment:  Windows
*
* Description:  Public Windows specific functions for the MGL.
*
****************************************************************************/

#include "mgl.h"
#include "mglwin/internal.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Associate a window manager device context with an MGL device context.

HEADER:
mglwin.h

PARAMETERS:
dc  - MGL windowed device context to use
hdc - Handle to window manager device context to associate

RETURNS:
True if the application's palette has changed, false if not.

REMARKS:
This function and its and the corresponding function MGL_endPaint() should
be called between the windows BeginPaint and EndPaint messages. This
function allows MGL to use the newest clipping regions and viewport settings.

MGL_beginPaint() and MGL_endPaint() must bracket drawing functions that draw
to a window type with a style of CS_PARENTDC or CS_CLASSDC. Such as dialog
box controls. These types of windows allocate device handles on the fly so
the HDC may change between calls to GetDC() or BeginPaint(). Therefore MGL
cannot draw to these types of windows without knowing the new HDC after
every BeginPaint() or GetDC() call.

OpenGL windows should NOT use MGL_beginPaint and MGL_endPaint.

A typical Windows WM_PAINT handler would be coded as follows:

    case WM_PAINT:
        hdc = BeginPaint(hwnd,&ps);
        MGL_beginPaint(dc,hdc);
        // Do rasterizing code in here //
        MGL_bitBlt(dc,memDC,r,0,0,MGL_REPLACE_MODE);
        MGL_endPaint(dc);
        EndPaint(hwnd,&ps);
        return 0;

SEE ALSO:
MGL_EndPaint
****************************************************************************/
ibool MGLAPI MGL_beginPaint(
    MGLDC *dc,
    MGL_HDC hdc)
{
    ibool   palChanged = false;

    if (dc->deviceType == MGL_WINDOWED_DEVICE) {
        /* We trade our internal HDC for the one passed in from windows
         * this will cause drawing to occur with the correct viewport and
         * clipping settings (as passed in from the windows BeginPaint
         * function)
         */
        dc->wm.windc.hdc = hdc;

        /* If we have a palette for the DC, then select & realize it,
         * returning true if the palette changed.
         */
        if (dc->wm.windc.hpal) {
            SelectPalette(dc->wm.windc.hdc,dc->wm.windc.hpal,false);
            palChanged = RealizePalette(dc->wm.windc.hdc);
            }
        }
    return palChanged;
}

/****************************************************************************
DESCRIPTION:
Cleans up after a previous call to MGL_beginPaint.

HEADER:
mglwin.h

PARAMETERS:
dc  - MGL windowed device context to use

REMARKS:
This function and its and the corresponding function MGL_beginPaint() should
be called between the windows BeginPaint and EndPaint messages.

MGL_beginPaint() and MGL_endPaint() must bracket drawing functions that draw
to a window type with a style of CS_PARENTDC or CS_CLASSDC. Such as dialog
box controls. These types of windows allocate device handles on the fly so
the HDC may change between calls to GetDC() or BeginPaint(). Therefore MGL
cannot draw to these types of windows without knowing the new HDC after
every BeginPaint() or GetDC() call.

A typical Windows WM_PAINT handler would be coded as follows:

    case WM_PAINT:
        hdc = BeginPaint(hwnd,&ps);
        MGL_beginPaint(dc,hdc);
        // Do rasterizing code in here //
        MGL_bitBlt(dc,memDC,r,0,0,MGL_REPLACE_MODE);
        MGL_endPaint(dc);
        EndPaint(hwnd,&ps);
        return 0;

SEE ALSO:
MGL_beginPaint
****************************************************************************/
void MGLAPI MGL_endPaint(
    MGLDC *dc)
{
    if (dc->deviceType == MGL_WINDOWED_DEVICE) {
        /* Here we reset the MGLDC's HDC to the internal one.
         * If the window has no private DC the HDC will be set to NULL
         * and drawing operations will not work until a new
         * MGL_beginPaint/MGL_endPaint bracket.
         */
        dc->wm.windc.hdc = dc->wm.windc.hdcOwned;
        }
}

/****************************************************************************
PARAMETERS:
dc      - Currently active windowed DC
active  - True if app is active, false if not

REMARKS:
Internal function to handle application activation messages so that we
can properly handle palette management for the window. We force a repaint
if the system palette usage is set to no-static mode.
****************************************************************************/
static void _MGL_appActivate(
    MGLDC *dc,
    ibool active)
{
    HDC hdc = GetDC(NULL);

    _MGL_isBackground = !active;
    if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC && MGL_getBitsPerPixel(dc) <= 8) {
        /* Re-realize the hardware palette and repaint on activate */
        MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,-1);
        InvalidateRect(dc->wm.windc.hwnd,NULL,TRUE);
        }
    ReleaseDC(NULL, hdc);
}

/****************************************************************************
PARAMETERS:
dc  - MGL device context to track window resize and movement for

REMARKS:
This internal function is used by the MGL to track the window size and
position on the screen, so that we can adjust the internal MGL rendering
information for the window so that it is correctly drawn on the screen.
****************************************************************************/
static void _MGL_trackWindowDimensions(
    MGLDC *dc)
{
    POINT       pt;
    RECT        r;
    rect_t      view,newSize;
    int         xResNew,yResNew,adjustX,adjustY;
    region_t    *clipRgn = NULL;

    pt.x = pt.y = 0;
    ClientToScreen(dc->wm.windc.hwnd, &pt);
    GetClientRect(dc->wm.windc.hwnd,&r);
    xResNew = r.right - r.left - 1;
    yResNew = r.bottom - r.top - 1;
    if (xResNew < 0 || yResNew < 0) {
        /* This will happen when the app has been minimised to the taskbar
         * so we don't do the actual resize until the app gains the focus
         * again.
         */
        return;
        }

    /* Check if complex clip region is active */
    if ((clipRgn = MGL_newRegion()) == NULL)
        return;
    MGL_getClipRegionDC(dc,clipRgn);

    /* Need to adjust clip rects based on new client area size */
    MGL_getViewportDC(dc,&view);
    dc->mi.xRes = xResNew;
    dc->mi.yRes = yResNew;
    newSize.left = pt.x;
    newSize.top = pt.y;
    newSize.right = xResNew + 1 + pt.x;
    newSize.bottom = yResNew + 1 + pt.y;
    adjustX = newSize.left - dc->size.left;
    adjustY = newSize.top - dc->size.top;
    dc->size = newSize;
    dc->visRectWM = newSize;
    dc->visRectWin = newSize;
    view.right = xResNew + 1;
    view.bottom = yResNew + 1;
    MGL_setViewportDC(dc,view);

    /* Re-set previously active complex clip region to new screen coordinates */
    if (!MGL_isSimpleRegion(clipRgn))
        MGL_setClipRegionDC(dc,clipRgn);
    else
        MGL_freeRegion(clipRgn);

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC) {
        _MGL_dcPtr->mi.xRes             = dc->mi.xRes;
        _MGL_dcPtr->mi.yRes             = dc->mi.yRes;
        _MGL_dcPtr->visRectWM           = dc->visRectWM;
        _MGL_dcPtr->visRectWin          = dc->visRectWin;
        _MGL_dcPtr->size                = dc->size;
        }
    else if (dc == _MGL_dcPtr) {
        DC.mi.xRes                      = dc->mi.xRes;
        DC.mi.yRes                      = dc->mi.yRes;
        DC.visRectWM                    = dc->visRectWM;
        DC.visRectWin                   = dc->visRectWin;
        DC.size                         = dc->size;
        }
}

/****************************************************************************
PARAMETERS:
dc  - Device context to activate the palette for

REMARKS:
Activates the Windows palette for a windowed device context.
****************************************************************************/
static ibool _MGL_activatePalette(
    MGLDC *dc)
{
    ibool       palChanged = false;
    HPALETTE    hPalOld;

    if (dc->wm.windc.hpal) {
        HDC hdc = GetDC(dc->wm.windc.hwnd);
        hPalOld = SelectPalette(hdc,dc->wm.windc.hpal, false);
        if ((palChanged = RealizePalette(hdc)) != false)
            InvalidateRect(dc->wm.windc.hwnd,NULL,TRUE);
        SelectPalette(hdc,hPalOld, false);
        ReleaseDC(dc->wm.windc.hwnd, hdc);
        }
    return palChanged;
}

/****************************************************************************
DESCRIPTION:
Window procedure hook for windowed modes.

PARAMETERS:
dc  - Device context to hook the window procedure for

REMARKS:
Main Window proc for the full screen WinDirect Window that we create while
running in full screen mode. Here we capture all mouse and keyboard events
for the window and plug them into our event queue.
****************************************************************************/
static LONG WINAPI _MGL_subWinProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LONG lParam)
{
    MGLDC   *dc;

    /* Find the MGL device context associated with this window handle.
     * We use a linked list for now since the list of windowed DC's
     * is not likely to be that large.
     */
    for (dc = _LST_first(_MGL_winDCList); dc; dc = _LST_next(dc)) {
        if (dc->wm.windc.hwnd == (MGL_HWND)hwnd)
            break;
        }
    if (!dc)
        return DefWindowProc(hwnd,msg,wParam,lParam);

    /* Now handle the window messages as appropriate */
	switch (msg) {
        case WM_ACTIVATEAPP:
			_MGL_appActivate(dc,wParam);
            break;
        case WM_MOVE:
        case WM_SIZE:
            _MGL_trackWindowDimensions(dc);
			break;
		case WM_PALETTECHANGED:
			if ((HWND)wParam == hwnd)
				break;
			/* Fall through to WM_QUERYNEWPALETTE */
		case WM_QUERYNEWPALETTE:
			return _MGL_activatePalette(dc);
		}
    return dc->wm.windc.orgWinProc(hwnd,msg,wParam,lParam);
}

/****************************************************************************
DESCRIPTION:
Function to hook the window procedure for a windowed DC

PARAMETERS:
dc  - Device context to hook the window procedure for

REMARKS:
This is an internal function that allows the OS specific code to hook the
window event handling procedure for the passed in windowed device context,
so that we can directly manage and handle common events on behalf of the
application program. We especially need to hook the window procedure to
hook into the windowing system to maintain a complex clip list for the
window as necessary.
{secret}
****************************************************************************/
void _MGL_hookWindowProc(
    MGLDC *dc)
{
	dc->wm.windc.orgWinProc = (MGL_WNDPROC )SetWindowLong(dc->wm.windc.hwnd,
        GWL_WNDPROC,(LPARAM)_MGL_subWinProc);
}

/****************************************************************************
DESCRIPTION:
Function to unhook the window procedure for a windowed DC

PARAMETERS:
dc  - Device context to unhook the window procedure for

REMARKS:
This is an internal function that allows the OS specific code to unhook the
window event handling procedure for the passed in windowed device context
to properly clean up when a windowed DC is destroyed.
{secret}
****************************************************************************/
void _MGL_unhookWindowProc(
    MGLDC *dc)
{
    SetWindowLong(dc->wm.windc.hwnd,GWL_WNDPROC,(LPARAM)dc->wm.windc.orgWinProc);
	dc->wm.windc.orgWinProc = NULL;
}

/*******************************************************************************
DESCRIPTION:
Registers a user window with the MGL to be used for fullscreen modes

HEADER:
mglwin.c

PARAMETERS:
hwnd	- Handle to user window to use for fullscreen modes.

REMARKS:
This function allows the application to create the window used for fullscreen
modes, and let the MGL know about the window so it will use that window
instead of creating it's own fullscreen window. By default when you create
a fullscreen device context, the MGL will create a fullscreen window that
covers the entire desktop that is used to capture Windows events such as
keyboard events, mouse events and activation events. However in some situations
it is beneficial to have only a single window that is used for all fullscreen
graphics modes, as well as windows modes (primarily to be able to properly
support DirectSound via a single window).

If you have registered a fullscreen window with the MGL, the MGL will take
that window, zoom it fullscreen and modify the window styles and attributes
to remove the title bar and other window decorations. When the MGL then
returns from fullscreen mode back to GDI mode, it will restore the window
back to the original position, style and state it was in before the fullscreen
mode was created. This allows you to create a single window with the MGL and
use it for both windowed modes and fullscreen modes.

Note:   This function is also useful if you wish to bypass the MGL event
        handling code in Windows and do all your own event handling for
        your own Window.

Note:	When the MGL exits via a call to MGL exit, the fullscreen window that
		you passed in will be destroyed (necessary to work around bugs in
		DirectDraw).
*******************************************************************************/
void MGLAPI MGL_registerFullScreenWindow(
    MGL_HWND hwndFullScreen)
{
    _MGL_hwndFullScreen = hwndFullScreen;
}

/*******************************************************************************
DESCRIPTION:
Returns the current fullscreen window handle.

HEADER:
mglwin.h

RETURNS:
Current fullscreen window handle.

REMARKS:
This function returns the handle to the current fullscreen window. When you are
running in fullscreen modes under Windows, MGL always maintains a fullscreen,
topmost window that is used for event handling.

If you are using the DirectSound libraries for sound output, you will need to inform
DirectSound what your fullscreen window is so that it can correctly mute the sound
for your application when the focus is lost to another application. If you do not do
this, when you switch to fullscreen modes all sound output via DirectSound will be
muted (assuming your are requesting exclusive mode).

SEE ALSO:
MGL_registerEventProc
*******************************************************************************/
MGL_HWND MGLAPI MGL_getFullScreenWindow(void)
{
    return _MGL_hwndFullScreen;
}

/*******************************************************************************
DESCRIPTION:
Registers a user supplied window procedure for event handling.

HEADER:
mglwin.h

PARAMETERS:
userWndProc 	- Point to user supplied Window Procedure

REMARKS:
This function registers a user supplied window procedure with MGL that will be
used for event handling purposes. By default MGL applications can simply use the
EVT_* event handling functions that are common to both the DOS and Windows
versions of MGL. However developers porting Windows specific code from
DirectDraw may wish to use their existing window specific event handling code.
This function allows you to do this by telling MGL to use your window procedure
for all event processing.

Note:   This function is only used to hook the window procedure for fullscreen
        windows, since for Windowed modes you need to pass in your own window
        handle anyway. Also note that an alternate method of hooking the
        Windows event procedures is to register your own fullscreen window
        handle using the MGL_registerFullScreenWindow function, in which case
        your window will get all window messages.

SEE ALSO:
EVT_getNext
*******************************************************************************/
void MGLAPI MGL_registerEventProc(
	MGL_WNDPROC userWndProc)
{
    PM_registerEventProc(userWndProc);
}

