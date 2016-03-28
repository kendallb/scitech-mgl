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

/*---------------------------- Global Variables ---------------------------*/

static ushort                   keyUpMsg[256] = {0};    /* Table of key up messages     */
static int                      rangeX,rangeY;          /* Range of mouse coordinates   */
static _EVT_mouseMoveHandler    _EVT_mouseMove;         /* Internal mouse move pointer  */

/*---------------------------- Implementation -----------------------------*/

/* These are not used under Win32 */
#define _EVT_disableInt()       1
#define _EVT_restoreInt(flags)  (void)(flags)

/****************************************************************************
PARAMETERS:
scanCode    - Scan code to test

REMARKS:
This macro determines if a specified key is currently down at the
time that the call is made.
****************************************************************************/
#define _EVT_isKeyDown(scanCode)    (keyUpMsg[scanCode] != 0)

/****************************************************************************
REMARKS:
This function is used to return the number of ticks since system
startup in milliseconds. This should be the same value that is placed into
the time stamp fields of events, and is used to implement auto mouse down
events.
{secret}
****************************************************************************/
ulong _EVT_getTicks(void)
{ return timeGetTime(); }

/****************************************************************************
REMARKS:
Pumps all messages in the message queue from Win32 into our event queue.
{secret}
****************************************************************************/
void _EVT_pumpMessages(void)
{
    MSG     msg;
    MSG     charMsg;
    event_t evt;

    // TODO: Add support for DirectInput! We can't support relative mouse
    //       movement motion counters without DirectInput ;-(.
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        memset(&evt,0,sizeof(evt));
        switch (msg.message) {
            case WM_MOUSEMOVE:
                /* Ignore mouse movement events when in the background */
                if (!_PM_backInGDI)
                    evt.what = EVT_MOUSEMOVE;
                break;
            case WM_LBUTTONDBLCLK:
                evt.what = EVT_MOUSEDOWN;
                evt.message = EVT_LEFTBMASK | EVT_DBLCLICK;
                break;
            case WM_LBUTTONDOWN:
                evt.what = EVT_MOUSEDOWN;
                evt.message = EVT_LEFTBMASK;
                break;
            case WM_LBUTTONUP:
                evt.what = EVT_MOUSEUP;
                evt.message = EVT_LEFTBMASK;
                break;
            case WM_RBUTTONDBLCLK:
                evt.what = EVT_MOUSEDOWN | EVT_DBLCLICK;
                evt.message = EVT_RIGHTBMASK;
                break;
            case WM_RBUTTONDOWN:
                evt.what = EVT_MOUSEDOWN;
                evt.message = EVT_RIGHTBMASK;
                break;
            case WM_RBUTTONUP:
                evt.what = EVT_MOUSEUP;
                evt.message = EVT_RIGHTBMASK;
                break;
            case WM_MBUTTONDBLCLK:
                evt.what = EVT_MOUSEDOWN | EVT_DBLCLICK;
                evt.message = EVT_MIDDLEBMASK;
                break;
            case WM_MBUTTONDOWN:
                evt.what = EVT_MOUSEDOWN;
                evt.message = EVT_MIDDLEBMASK;
                break;
            case WM_MBUTTONUP:
                evt.what = EVT_MOUSEUP;
                evt.message = EVT_MIDDLEBMASK;
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if (HIWORD(msg.lParam) & KF_REPEAT) {
                    evt.what = EVT_KEYREPEAT;
                    }
                else {
                    evt.what = EVT_KEYDOWN;
                    }
                break;
            case WM_KEYUP:
            case WM_SYSKEYUP:
                evt.what = EVT_KEYUP;
                break;
            case WM_PAINT:
                evt.what = EVT_REPAINT;
                evt.which = (ulong)msg.hwnd;
                break;
            }

        /* Convert mouse event modifier flags */
        if (evt.what & EVT_MOUSEEVT) {
            if (_PM_deskX) {
                evt.where_x = ((long)msg.pt.x * rangeX) / _PM_deskX;
                evt.where_y = ((long)msg.pt.y * rangeY) / _PM_deskY;
                }
            else {
                ScreenToClient(_PM_hwndConsole, &msg.pt);
                evt.where_x = msg.pt.x;
                evt.where_y = msg.pt.y;
                }
            if (evt.what == EVT_MOUSEMOVE) {
                /* Save the current mouse position */
                EVT.mx = evt.where_x;
                EVT.my = evt.where_y;
                if (EVT.oldMove != -1) {
                    EVT.evtq[EVT.oldMove].where_x = evt.where_x;/* Modify existing one  */
                    EVT.evtq[EVT.oldMove].where_y = evt.where_y;
//                  EVT.evtq[EVT.oldMove].relative_x += mickeyX;    // TODO!
//                  EVT.evtq[EVT.oldMove].relative_y += mickeyY;    // TODO!
                    evt.what = 0;
                    }
                else {
                    EVT.oldMove = EVT.freeHead; /* Save id of this move event   */
//                  evt.relative_x = mickeyX;    // TODO!
//                  evt.relative_y = mickeyY;    // TODO!
                    }
                }
            else
                EVT.oldMove = -1;
            if (msg.wParam & MK_LBUTTON)
                evt.modifiers |= EVT_LEFTBUT;
            if (msg.wParam & MK_RBUTTON)
                evt.modifiers |= EVT_RIGHTBUT;
            if (msg.wParam & MK_MBUTTON)
                evt.modifiers |= EVT_MIDDLEBUT;
            if (msg.wParam & MK_SHIFT)
                evt.modifiers |= EVT_SHIFTKEY;
            if (msg.wParam & MK_CONTROL)
                evt.modifiers |= EVT_CTRLSTATE;
            }

        /* Convert keyboard codes */
        TranslateMessage(&msg);
        if (evt.what & EVT_KEYEVT) {
            int scanCode = (msg.lParam >> 16) & 0xFF;
            if (evt.what == EVT_KEYUP) {
                /* Get message for keyup code from table of cached down values */
                evt.message = keyUpMsg[scanCode];
                keyUpMsg[scanCode] = 0;
                }
            else {
                if (PeekMessage(&charMsg,NULL,WM_CHAR,WM_CHAR,PM_REMOVE))
                    evt.message = charMsg.wParam;
                if (PeekMessage(&charMsg,NULL,WM_SYSCHAR,WM_SYSCHAR,PM_REMOVE))
                    evt.message = charMsg.wParam;
                evt.message |= ((msg.lParam >> 8) & 0xFF00);
                keyUpMsg[scanCode] = (ushort)evt.message;
                }
            if (evt.what == EVT_KEYREPEAT)
                evt.message |= (msg.lParam << 16);
            if (HIWORD(msg.lParam) & KF_ALTDOWN)
                evt.modifiers |= EVT_ALTSTATE;
            if (GetKeyState(VK_SHIFT) & 0x8000U)
                evt.modifiers |= EVT_SHIFTKEY;
            if (GetKeyState(VK_CONTROL) & 0x8000U)
                evt.modifiers |= EVT_CTRLSTATE;
            EVT.oldMove = -1;
            }

        if (evt.what != 0) {
            /* Add time stamp and add the event to the queue */
            evt.when = msg.time;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }
        DispatchMessage(&msg);
        }
}

/****************************************************************************
REMARKS:
This macro/function is used to converts the scan codes reported by the
keyboard to our event libraries normalised format. We only have one scan
code for the 'A' key, and use shift modifiers to determine if it is a
Ctrl-F1, Alt-F1 etc. The raw scan codes from the keyboard work this way,
but the OS gives us 'cooked' scan codes, we have to translate them back
to the raw format.
****************************************************************************/
#define _EVT_maskKeyCode(evt)

/****************************************************************************
REMARKS:
Safely abort the event module upon catching a fatal error.
{secret}
****************************************************************************/
void _EVT_abort(
    int signal)
{
    (void)signal;
    EVT_exit();
    PM_fatalError("Unhandled exception!");
}

/****************************************************************************
PARAMETERS:
x   - New X coordinate to move mouse cursor to
y   - New Y coordinate to move mouse cursor to

REMARKS:
Move the mouse cursor on the screen to the specified X and Y locations
as necessary. In here we adjust the coordinates from client rectangle
coordinates to screen coordinates ensure that the cursor is drawn in the
correct location.
{secret}
****************************************************************************/
static void EVTAPI _mouseMoveHandler(
    int x,
    int y)
{
    POINT   pt;

    pt.x = x;
    pt.y = y;
    ClientToScreen(_PM_hwndConsole, &pt);
    _EVT_mouseMove(pt.x,pt.y);
}

/****************************************************************************
PARAMETERS:
mouseMove   - Callback function to call wheneve the mouse needs to be moved

REMARKS:
Initiliase the event handling module. Here we install our mouse handling ISR
to be called whenever any button's are pressed or released. We also build
the free list of events in the event queue.

We use handler number 2 of the mouse libraries interrupt handlers for our
event handling routines.
{secret}
****************************************************************************/
void EVTAPI EVT_init(
    _EVT_mouseMoveHandler mouseMove)
{
    /* Initialise the event queue */
    _EVT_mouseMove = mouseMove;
    EVT.mouseMove = _mouseMoveHandler;
    initEventQueue();
    memset(keyUpMsg,0,sizeof(keyUpMsg));

    /* Catch program termination signals so we can clean up properly */
    signal(SIGABRT, _EVT_abort);
    signal(SIGINT, _EVT_abort);
}

/****************************************************************************
REMARKS
Changes the range of coordinates returned by the mouse functions to the
specified range of values. This is used when changing between graphics
modes set the range of mouse coordinates for the new display mode.
{secret}
****************************************************************************/
void EVTAPI EVT_setMouseRange(
    int xRes,
    int yRes)
{
    rangeX = xRes;
    rangeY = yRes;
}

/****************************************************************************
REMARKS
Modifies the mouse coordinates as necessary if scaling to OS coordinates,
and sets the OS mouse cursor position.
{secret}
****************************************************************************/
void _EVT_setMousePos(
    int *x,
    int *y)
{
    if (_PM_deskX) {
        /* Scale coordinates up to desktop coordinates first */
        int scaledX = (*x * _PM_deskX) / rangeX;
        int scaledY = (*y * _PM_deskY) / rangeY;

        /* Scale coordinates back to screen coordinates again */
        *x = (scaledX * rangeX) / _PM_deskX;
        *y = (scaledY * rangeY) / _PM_deskY;
        SetCursorPos(scaledX,scaledY);
        }
    else {
        POINT   pt;

        pt.x = *x;
        pt.y = *y;
        ClientToScreen(_PM_hwndConsole, &pt);
        *x = pt.x;
        *y = pt.y;
        SetCursorPos(pt.x,pt.y);
        }
}

/****************************************************************************
REMARKS:
Initiailises the internal event handling modules. The EVT_suspend function
can be called to suspend event handling (such as when shelling out to DOS),
and this function can be used to resume it again later.
{secret}
****************************************************************************/
void EVT_resume(void)
{
    // Do nothing for Win32
}

/****************************************************************************
REMARKS
Suspends all of our event handling operations. This is also used to
de-install the event handling code.
{secret}
****************************************************************************/
void EVT_suspend(void)
{
    // Do nothing for Win32
}

/****************************************************************************
REMARKS
Exits the event module for program terminatation.
{secret}
****************************************************************************/
void EVT_exit(void)
{
    /* Restore signal handlers */
    signal(SIGABRT, SIG_DFL);
    signal(SIGINT, SIG_DFL);
}

/****************************************************************************
REMARKS:
This function is used to detect the attached joysticks, and determine
what axes are present and functioning. This function will re-detect any
attached joysticks when it is called, so if the user forgot to attach
the joystick when the application started, you can call this function to
re-detect any newly attached joysticks.
****************************************************************************/
int EVTAPI EVT_joyIsPresent(void)
{
    // TODO: Implement joystick code based on DirectX!
    return 0;
}

/****************************************************************************
REMARKS:
This routine is used to poll analogue joysticks for button and position
information. It should be called once for each main loop of the user
application, just before processing all pending events via EVT_getNext.
All information polled from the joystick will be posted to the event
queue for later retrieval.

Note:   Most analogue joysticks will provide readings that change even
        though the joystick has not moved. Hence if you call this routine
        you will likely get an EVT_JOYMOVE event every time through your
        event loop.
****************************************************************************/
void EVTAPI EVT_pollJoystick(void)
{
}

/****************************************************************************
REMARKS:
This function can be used to zero in on better joystick calibration factors,
which may work better than the default simplistic calibration (which assumes
the joystick is centered when the event library is initialised).
To use this function, ask the user to hold the stick in the upper left
position and then have them press a key or button. and then call this
function. This function will then read the joystick and update the
calibration factors.

Usually, assuming that the stick was centered when the event library was
initialized, you really only need to call EVT_joySetLowerRight since the
upper left position is usually always 0,0 on most joysticks. However, the
safest procedure is to call all three calibration functions.
****************************************************************************/
void EVTAPI EVT_joySetUpperLeft(void)
{
}

/****************************************************************************
REMARKS:
This function can be used to zero in on better joystick calibration factors,
which may work better than the default simplistic calibration (which assumes
the joystick is centered when the event library is initialised).
To use this function, ask the user to hold the stick in the lower right
position and then have them press a key or button. and then call this
function. This function will then read the joystick and update the
calibration factors.

Usually, assuming that the stick was centered when the event library was
initialized, you really only need to call EVT_joySetLowerRight since the
upper left position is usually always 0,0 on most joysticks. However, the
safest procedure is to call all three calibration functions.
****************************************************************************/
void EVTAPI EVT_joySetLowerRight(void)
{
}

/****************************************************************************
REMARKS:
This function can be used to zero in on better joystick calibration factors,
which may work better than the default simplistic calibration (which assumes
the joystick is centered when the event library is initialised).
To use this function, ask the user to hold the stick in the center
position and then have them press a key or button. and then call this
function. This function will then read the joystick and update the
calibration factors.

Usually, assuming that the stick was centered when the event library was
initialized, you really only need to call EVT_joySetLowerRight since the
upper left position is usually always 0,0 on most joysticks. However, the
safest procedure is to call all three calibration functions.
****************************************************************************/
void EVTAPI EVT_joySetCenter(void)
{
}

