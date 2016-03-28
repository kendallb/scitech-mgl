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
* Environment:  RTTarget-32
*
* Description:  RTOS-32 implementation for the SciTech cross platform
*               event library.
*
****************************************************************************/

#ifdef  USE_RTT32DLL
#define RTT32DLL
#endif
#include <rttarget.h>
#ifdef  USE_RTKERNEL32
#include <rtkernel.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

static ushort                   keyUpMsg[256] = {0};    /* Table of key up messages     */
static int                      rangeX = 0,rangeY = 0;  /* Range of mouse coordinates   */

/*---------------------------- Implementation -----------------------------*/

#define MOUSE_ZERO_POS      16384

/****************************************************************************
REMARKS:
The following macros/functions are used in the generic event handler code
for OS-specific functions. The following are implemented directly on RTOS-32.
****************************************************************************/
#define _EVT_disableInt()       RTSaveAndDisableInterrupts()
#define _EVT_restoreInt(flags)  RTRestoreInterrupts(flags)
#define Reboot()                RTReboot()

#define SUPPORT_CTRL_ALT_DEL
#include "common/keyboard.c"    /* Generic keyboard handler code */

/****************************************************************************
REMARKS:
This function is used to return the number of ticks since system
startup in milliseconds. This should be the same value that is placed into
the time stamp fields of events, and is used to implement auto mouse down
events.
****************************************************************************/
ulong _EVT_getTicks(void)
{
#ifdef USE_RTKERNEL32
    return RTKGetTime();
#else
    return GetTickCount();
#endif
}

/****************************************************************************
PARAMETERS:
what        - Event code
message     - Event message
x,y         - Mouse position at time of event
but_stat    - Mouse button status at time of event

REMARKS:
Adds a new mouse event to the event queue.
****************************************************************************/
static void addMouseEvent(
    uint what,
    uint message,
    int x,
    int y,
    int mickeyX,
    int mickeyY,
    uint but_stat)
{
    event_t evt;
    int     ps;

    if (EVT.count < EVENTQSIZE) {
        /* Save information in event record. */
        evt.when = GetTickCount();
        evt.what = what;
        evt.message = message;
        evt.modifiers = but_stat;
        evt.where_x = x;                /* Save mouse event position    */
        evt.where_y = y;
        evt.relative_x = mickeyX;
        evt.relative_y = mickeyY;
        evt.modifiers |= EVT.keyModifiers;
        ps = _EVT_disableInt();
        addEvent(&evt);                 /* Add to tail of event queue   */
        _EVT_restoreInt(ps);
        }
}

/****************************************************************************
REMARKS:
Keyboard interrupt handler function.

NOTE:   Interrupts are OFF when this routine is called by the keyboard ISR,
        and we leave them OFF the entire time.
****************************************************************************/
static void RTTAPI keyboardISR(void)
{
    char kbval;

    /* Process and translate the scan code */
    processRawScanCode(PM_inpb(0x60));

    /* Let the keyboard know we have processed the key press */
    kbval = PM_inpb(0x61);
    PM_outpb(0x61,kbval | 0x80);
    PM_outpb(0x61,kbval);

    /* Let RTKernel clear PIC and exit ISR */
    RTIRQEnd(1);
}

/****************************************************************************
REMARKS:
Note that unlike Win32 messages, there are no time stamps on console events,
so we have to add them ourselves. If the events are handled at interrupt
time, the time stamps would be accurate but interrupt latency would be
longer. If the events are handled outside of the interrupt handlers, the time
stamps would be delayed but interrupt latency would minimal. To solve this
we use a separate thread that is woken up from the interrupt handler so
the time stamps will be correct for the mouse events.
****************************************************************************/
void _EVT_pumpMessages(void)
{
    HANDLE          hstdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD    ir;
    DWORD           dwRead = 0;
    int             relX,relY;
    ulong           butstate,modifiers,buttons;
    static ulong    oldbutstate = 0xFFFFFFFF;

    /* Pump event messages */
    while (PeekConsoleInput(hstdin, &ir, 1, &dwRead) && (dwRead > 0)) {
        ReadConsoleInput(hstdin, &ir, 1, &dwRead);

        /* Read mouse console event, if any */
        if (ir.EventType != MOUSE_EVENT)
            continue;

        /* Get the base mouse event information. Since the mouse driver
         * wraps the x/y coordinates at 32,768, we reposition the mouse
         * to 16384,16384 here, after we figure out the relative mouse
         * coordinate change.
         */
        relX = (ir.Event.MouseEvent.dwMousePosition.X - MOUSE_ZERO_POS);
        relY = (ir.Event.MouseEvent.dwMousePosition.Y - MOUSE_ZERO_POS);
        RTSetMousePos(MOUSE_ZERO_POS,MOUSE_ZERO_POS);
        EVT.mx += relX;
        EVT.my += relY;
        if (EVT.mx < 0) EVT.mx = 0;
        if (EVT.mx > rangeX) EVT.mx = rangeX;
        if (EVT.my < 0) EVT.my = 0;
        if (EVT.my > rangeY) EVT.my = rangeY;

        /* Figure out the button states */
        butstate = modifiers = 0;
        if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
            butstate |= EVT_LEFTBMASK;
            modifiers |= EVT_LEFTBUT;
            }
        if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
            butstate |= EVT_MIDDLEBMASK;
            modifiers |= EVT_MIDDLEBUT;
            }
        if (ir.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
            butstate |= EVT_RIGHTBMASK;
            modifiers |= EVT_RIGHTBUT;
            }

        /* Add movement events if any */
        if (ir.Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
            /* If the last event was a movement event, then modify the last
             * event rather than post a new one, so that the queue will not
             * become saturated.
             */
            if (EVT.oldMove != -1) {
                EVT.evtq[EVT.oldMove].where_x = EVT.mx;
                EVT.evtq[EVT.oldMove].where_y = EVT.my;
                EVT.evtq[EVT.oldMove].relative_x += relX;
                EVT.evtq[EVT.oldMove].relative_y += relY;
                }
            else {
                EVT.oldMove = EVT.freeHead;
                addMouseEvent(EVT_MOUSEMOVE,0,EVT.mx,EVT.my,relX,relY,modifiers);
                }
            }

        /* Now check and see if the mouse state has transitioned, and if so
         * add the appropriate mouse up/down event.
         */
        if (oldbutstate != butstate) {
            /* See if one of the buttons transitioned to the down state */
            buttons = 0;
            if ((butstate & EVT_LEFTBMASK) && ((oldbutstate & EVT_LEFTBMASK) == 0))
                buttons |= EVT_LEFTBMASK;
            if ((butstate & EVT_RIGHTBMASK) && ((oldbutstate & EVT_RIGHTBMASK) == 0))
                buttons |= EVT_RIGHTBMASK;
            if ((butstate & EVT_MIDDLEBMASK) && ((oldbutstate & EVT_MIDDLEBMASK) == 0))
                buttons |= EVT_MIDDLEBMASK;
            if (buttons)
                addMouseEvent(EVT_MOUSEDOWN,buttons,EVT.mx,EVT.my,relX,relX,modifiers);

            /* See if one of the buttons transitioned to the up state */
            buttons = 0;
            if ((oldbutstate & EVT_LEFTBMASK) && ((butstate & EVT_LEFTBMASK) == 0))
                buttons |= EVT_LEFTBMASK;
            if ((oldbutstate & EVT_RIGHTBMASK) && ((butstate & EVT_RIGHTBMASK) == 0))
                buttons |= EVT_RIGHTBMASK;
            if ((oldbutstate & EVT_MIDDLEBMASK) && ((butstate & EVT_MIDDLEBMASK) == 0))
                buttons |= EVT_MIDDLEBMASK;
            if (buttons)
                addMouseEvent(EVT_MOUSEUP,buttons,EVT.mx,EVT.my,relX,relX,modifiers);

            /* Save current state */
            EVT.oldMove = -1;
            oldbutstate = butstate;
            }
        }
}

/****************************************************************************
REMARKS:
Safely abort the event module upon catching a fatal error.
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
mouseMove   - Callback function to call wheneve the mouse needs to be moved

REMARKS:
Initiliase the event handling module. Here we install our mouse handling ISR
to be called whenever any button's are pressed or released. We also build
the free list of events in the event queue.

We use handler number 2 of the mouse libraries interrupt handlers for our
event handling routines.
****************************************************************************/
void EVTAPI EVT_init(
    _EVT_mouseMoveHandler mouseMove)
{
    /* Initialise the event queue */
    EVT.mouseMove = mouseMove;
    initEventQueue();
    memset(keyUpMsg,0,sizeof(keyUpMsg));

    /* Catch program termination signals so we can clean up properly */
    signal(SIGABRT, _EVT_abort);
    signal(SIGINT, _EVT_abort);

    /* Init the PM library to ensure RTTarget kernel services are started */
    PM_init();

    /* Install keyboard handler */
    RTInstallSharedIRQHandler(1, keyboardISR);

    /* TODO: It would be nice to hook the mouse interrupt handler and
     *       signal a thread to do the pump messages processing, but no
     *       matter what we do at the moment, this does not work. Something
     *       is wrong with threads/semaphores in RTTarget when used with
     *       our code.
     */

    /* Init mouse driver with default scaling factors */
    RTInitMouse(-1, 12, 0, 1, 1);
    RTSetMousePos(MOUSE_ZERO_POS,MOUSE_ZERO_POS);

    /* Flush events which may have come in during setup */
    EVT_flush(EVT_EVERYEVT);
}

/****************************************************************************
REMARKS
Modifes the mouse coordinates as necessary if scaling to OS coordinates,
and sets the OS mouse cursor position.
****************************************************************************/
void _EVT_setMousePos(
    int *x,
    int *y)
{
    EVT.mx = *x;
    EVT.my = *y;
    if (EVT.mx < 0) EVT.mx = 0;
    if (EVT.mx > rangeX) EVT.mx = rangeX;
    if (EVT.my < 0) EVT.my = 0;
    if (EVT.my > rangeY) EVT.my = rangeY;
}

/****************************************************************************
REMARKS
Changes the range of coordinates returned by the mouse functions to the
specified range of values. This is used when changing between graphics
modes set the range of mouse coordinates for the new display mode.
****************************************************************************/
void EVTAPI EVT_setMouseRange(
    int xRes,
    int yRes)
{
    /* Save the mouse coordinate range for scaling later */
    rangeX = xRes;
    rangeY = yRes;

    /* Re-init the mouse driver with scaling factor based on resolution */
    RTMouseDone();
    RTInitMouse(-1, 12, 0,
        1 + (3 * rangeX + 400) / 1600,
        1 + (3 * rangeY + 300) / 1200);
}

/****************************************************************************
REMARKS:
Initiailises the internal event handling modules. The EVT_suspend function
can be called to suspend event handling (such as when shelling out to DOS),
and this function can be used to resume it again later.
****************************************************************************/
void EVT_resume(void)
{
    /* Not necessary on RTTarget */
}

/****************************************************************************
REMARKS
Suspends all of our event handling operations. This is also used to
de-install the event handling code.
****************************************************************************/
void EVT_suspend(void)
{
    /* Not necessary on RTTarget */
}

/****************************************************************************
REMARKS
Exits the event module for program terminatation.
****************************************************************************/
void EVT_exit(void)
{
    /* Restore signal handlers */
    signal(SIGABRT, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    /* Restore interrupt handlers which we installed */
    RTMouseDone();
    RTRemoveSharedIRQHandler(1, keyboardISR);
}

