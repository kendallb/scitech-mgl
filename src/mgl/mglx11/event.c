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
* Environment:  Unix/X11
*
* Description:  Routines to provide an event queue, which automatically
*               handles keyboard and mouse events for the MGL.
*
****************************************************************************/

#include "mgl.h"
#include "mglunix.h"
#include "mgldd.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <time.h>
#include <signal.h>

/*--------------------------- Global variables ----------------------------*/

#define EVENTQSIZE  100             /* Number of events in event queue  */

static int          head = -1;      /* Head of event queue              */
static int          tail = -1;      /* Tail of event queue              */
static int          freeHead = -1;  /* Head of free list                */
static int          count = 0;      /* No. of items currently in queue  */
static event_t      evtq[EVENTQSIZE];/* The queue structure itself      */
static int          timerReset;     /* Value to reset timer with        */
static int          timerCount = 0; /* Current timer countdown          */
static int      oldMove = -1;   /* Previous movement event          */
static int      oldKey = -1;    /* Previous key repeat event        */
static ibool         (*userEventCallback)(event_t *evt) = NULL;
static uint         mods = 0;
ibool       _EVT_installed = 0;/* Event handers installed?      */

extern int  MGLAPI _EVT_dataStart;
extern int  MGLAPI _EVT_dataEnd;

extern Display *dsp;
extern Window   wnd;


/*---------------------------- Implementation -----------------------------*/

static void addEvent(event_t *evt)
/****************************************************************************
*
* Function:     addEvent
* Parameters:   evt - Event to place onto event queue
*
* Description:  Adds an event to the event queue by tacking it onto the
*               tail of the event queue. This routine assumes that at least
*               one spot is available on the freeList for the event to
*               be inserted.
*
*               NOTE: Interrupts MUST be OFF while this routine is called
*                     to ensure we have mutually exclusive access to our
*                     internal data structures.
*
****************************************************************************/
{
    int         evtID;

    /* Call user supplied callback to modify the event if desired */
    if (userEventCallback) {
        if (!userEventCallback(evt))
            return;
    }

    /* Get spot to place the event from the free list */
    evtID = freeHead;
    freeHead = evtq[freeHead].next;

    /* Add to the tail of the event queue   */
    evt->next = -1;
    evt->prev = tail;
    if (tail != -1)
        evtq[tail].next = evtID;
    else
        head = evtID;
    tail = evtID;
    evtq[evtID] = *evt;
    count++;
}

static void addMouseEvent(uint what,uint message,int x,int y,uint but_stat)
/****************************************************************************
*
* Function:     addMouseEvent
* Parameters:   what        - Event code
*               message     - Event message
*               x,y         - Mouse position at time of event
*               but_stat    - Mouse button status at time of event
*
* Description:  Adds a new mouse event to the event queue. This routine is
*               called from within the mouse interrupt subroutine, so it
*               must be efficient.
*
*               NOTE: Interrupts are OFF when this routine is called by
*                     the mouse ISR, and we leave them off
*
****************************************************************************/
{
    event_t evt;

    if (count < EVENTQSIZE) {
        /* Save information in event record. */
        evt.when = MGL_getTicks();
        evt.what = what;
        evt.message = message;
        evt.modifiers = but_stat | mods;
        evt.where_x = x;                /* Save mouse event position    */
        evt.where_y = y;

        /* TODO: Determine and store keyboard shift modifiers */

        addEvent(&evt);                 /* Add to tail of event queue   */
        }
}

/* {secret} */
static void MGLAPI addKeyEvent(uint what,uint message)
/****************************************************************************
*
* Function:     addKeyEvent
* Parameters:   what        - Event code
*               message     - Event message (ASCII code and scan code)
*
* Description:  Adds a new keyboard event to the event queue.
*
****************************************************************************/
{
    event_t evt;

    if (count < EVENTQSIZE) {
        /* Save information in event record */
        evt.when = MGL_getTicks();
        evt.what = what;
        evt.message = (message & 0xff) | 0x10000UL;
        evt.where_x = evt.where_y = 0;

        switch(message){
        case XK_Shift_L:
        case XK_Shift_R:
            if(what == EVT_KEYDOWN)
                mods |= EVT_SHIFTKEY;
            else
                mods &= ~EVT_SHIFTKEY;
            break;
        case XK_Control_L:
        case XK_Control_R:
            if(what == EVT_KEYDOWN)
                mods |= EVT_CTRLSTATE;
            else
                mods &= ~EVT_CTRLSTATE;
            break;
        case XK_Alt_L:
        case XK_Alt_R:
            if(what == EVT_KEYDOWN)
                mods |= EVT_ALTSTATE;
            else
                mods &= ~EVT_ALTSTATE;
            break;
        default:
        }

        evt.modifiers = mods;

        if (evt.what == EVT_KEYREPEAT) {
            if (oldKey != -1)
                evtq[oldKey].message += 0x10000UL;
            else {
                oldKey = freeHead;
                addEvent(&evt);     /* Add to tail of event queue */
                }
            }
        else
            addEvent(&evt);         /* Add to tail of event queue */
        oldMove = -1;
    }
}

/*------------------------ Public interface routines ----------------------*/

/****************************************************************************
DESCRIPTION:
Posts a user defined event to the event queue

HEADER:
mgraph.h

RETURNS:
True if event was posted, false if event queue is full.

PARAMETERS:
what        - Type code for message to post
message     - Event specific message to post
modifiers   - Event specific modifier flags to post

REMARKS:
This routine is used to post user defined events to the event queue.

SEE ALSO:
EVT_flush, EVT_getNext, EVT_peekNext, EVT_halt
****************************************************************************/
ibool MGLAPI EVT_post(
    ulong which,
    uint what,
    ulong message,
    ulong modifiers)
{
    event_t evt;

    if (count < EVENTQSIZE) {
        /* Save information in event record */

        evt.which = which;
        evt.when = MGL_getTicks();
        evt.what = what;
        evt.message = message;
        evt.modifiers = modifiers;
        addEvent(&evt);             /* Add to tail of event queue   */
        return true;
    }else
        return false;
}

/****************************************************************************
DESCRIPTION:
Flushes all events of a specified type from the event queue.

PARAMETERS:
mask    - Mask specifying the types of events that should be removed

HEADER:
mgraph.h

REMARKS:
Flushes (removes) all pending events of the specified type from the event queue.
You may combine the masks for different event types with a simple logical OR.

SEE ALSO:
EVT_getNext, EVT_halt, EVT_peekNext
****************************************************************************/
void MGLAPI EVT_flush(
              uint mask)
{
    event_t evt;

    do {                            /* Flush all events */
        EVT_getNext(&evt,mask);
    } while (evt.what != EVT_NULLEVT);
}

/****************************************************************************
DESCRIPTION:
Flushes all events of a specified type from the event queue.

HEADER:
mgraph.h

PARAMETERS:
evt     - Pointer to
mask    - Mask specifying the types of events that should be removed

REMARKS:
Flushes (removes) all pending events of the specified type from the event queue.
You may combine the masks for different event types with a simple logical OR.

SEE ALSO:
EVT_getNext, EVT_halt, EVT_peekNext
****************************************************************************/
void MGLAPI EVT_halt(
    event_t *evt,
    uint mask)
{
    do {                            /* Wait for an event    */
        EVT_getNext(evt,EVT_EVERYEVT);
    } while (!(evt->what & mask));
}

static inline int mapX11Button(int but)
{
    switch(but){
    case 1:
        return EVT_LEFTBUT;
    case 3:
        return EVT_RIGHTBUT;
    }
    return but;
}

/****************************************************************************
REMARKS:
Retrieves all events from the mouse/keyboard event queue and stuffs them
into the MGL event queue for further processing.
****************************************************************************/
void getEvents(void)
{
    XEvent  ev;
    xwindc_vars *v = &DC.wm.xwindc;

    while (XPending(v->dpy) && XNextEvent(v->dpy,&ev)) {
        switch(ev.type){
        case KeyPress:
            addKeyEvent(EVT_KEYDOWN, XKeycodeToKeysym(v->dpy, ev.xkey.keycode, 0));
            break;
        case KeyRelease:
            addKeyEvent(EVT_KEYUP, XKeycodeToKeysym(v->dpy, ev.xkey.keycode, 0));
            break;
        case ButtonPress:
            addMouseEvent(EVT_MOUSEDOWN, 0, ev.xbutton.x, ev.xbutton.y, mapX11Button(ev.xbutton.button));
            break;
        case ButtonRelease:
            addMouseEvent(EVT_MOUSEUP, 0, ev.xbutton.x, ev.xbutton.y, mapX11Button(ev.xbutton.button));
            break;
        case MotionNotify:
            addMouseEvent(EVT_MOUSEMOVE, 0, ev.xmotion.x, ev.xmotion.y, 0);
            break;
        case FocusIn:
            break;
        case FocusOut:
            break;
        }

    }
}

/****************************************************************************
DESCRIPTION:
Retrieves the next pending event from the event queue.

PARAMETERS:
evt     - Pointer to structure to return the event info in
mask    - Mask specifying the types of events that should be removed

HEADER:
mgraph.h

RETURNS:
True if an event was pending, false if not.

REMARKS:
Retrieves the next pending event from the event queue, and stores it in a event_t
structure. The mask parameter is used to specify the type of events to be removed,
and can be any logical combination of any of the flags defined by the
MGL_eventType enumeration.

The what field of the event contains the event code of the event that was extracted.
All application specific events should begin with the EVT_USEREVT code and
build from there. Since the event code is stored in an integer, there is a maximum of
16 different event codes that can be distinguished (32 for the 32 bit version). You
can store extra information about the event in the message field to distinguish
between events of the same class (for instance the button used in a
EVT_MOUSEDOWN event).

If an event of the specified type was not in the event queue, the what field of the
event will be set to NULLEVT, and the return value will return false.

The EVT_TIMERTICK event is used to report that a specified time interval has
elapsed since the last EVT_TIMERTICK event occurred. See EVT_setTimerTick()
for information on how to enable timer tick events and to set the timer interval.

SEE ALSO:
EVT_flush, EVT_halt, EVT_peekNext, EVT_setTimerTick
****************************************************************************/
ibool MGLAPI EVT_getNext(
    event_t *evt,
    uint mask)
{
    int  evtID,next,prev;

    getEvents();    /* Read any pending events and place on event queue */

    evt->what = EVT_NULLEVT;                /* Default to null event    */

    if (count) {
        for (evtID = head; evtID != -1; evtID = evtq[evtID].next) {
            if (evtq[evtID].what & mask)
                break;                      /* Found an event           */
            }
        if (evtID == -1) {
            return false;                   /* Event was not found      */
            }

        next = evtq[evtID].next;
        prev = evtq[evtID].prev;

        if (prev != -1)
            evtq[prev].next = next;
        else
            head = next;

        if (next != -1)
            evtq[next].prev = prev;
        else
            tail = prev;

        *evt = evtq[evtID];                 /* Return the event         */
        evtq[evtID].next = freeHead;        /* and return to free list  */
        freeHead = evtID;
        count--;
        if (evt->what == EVT_MOUSEMOVE)
            oldMove = -1;
        if (evt->what == EVT_KEYREPEAT)
            oldKey = -1;

        //      if (evt->what & EVT_KEYEVT)
        //  _EVT_maskKeyCode(evt);
        }

    return evt->what != EVT_NULLEVT;
}

/****************************************************************************
DESCRIPTION:
Peeks at the next pending event in the event queue.

HEADER:
mgraph.h

RETURNS:
True if an event is pending, false if not.

PARAMETERS:
evt     - Pointer to structure to return the event info in
mask    - Mask specifying the types of events that should be removed

REMARKS:
Peeks at the next pending event of the specified type in the event queue. The mask
parameter is used to specify the type of events to be peeked at, and can be any
logical combination of any of the flags defined by the MGL_eventType enumeration.

In contrast to EVT_getNext, the event is not removed from the event queue. You
may combine the masks for different event types with a simple logical OR.

SEE ALSO:
EVT_flush, EVT_getNext, EVT_halt
****************************************************************************/
ibool MGLAPI EVT_peekNext(
    event_t *evt,
    uint mask)
{
    int     evtID;

    getEvents();    /* Read any pending events and place on event queue */

    evt->what = EVT_NULLEVT;                /* Default to null event    */

    if (count) {
        /* It is possible that an event be posted while we are trying
         * to access the event queue. This would create problems since
         * we may end up with invalid data for our event queue pointers. To
         * alleviate this, all interrupts are suspended while we manipulate
         * our pointers.
         */
    for (evtID = head; evtID != -1; evtID = evtq[evtID].next) {
        if (evtq[evtID].what & mask)
            break;                      /* Found an event           */
        }
    if (evtID == -1) {
        return false;                   /* Event was not found      */
        }

    *evt = evtq[evtID];                 /* Return the event         */

    //  if (evt->what & EVT_KEYEVT)
    //      _EVT_maskKeyCode(evt);
    }

    return evt->what != EVT_NULLEVT;
}

/* {secret} */
void _EVT_abort(int signo)
{
    char    buf[80];

    sprintf(buf, "Terminating on signal %d\n", signo);
    MGL_fatalError(buf);
}

/* {secret} */
void _EVT_resume(void)
/****************************************************************************
*
* Function:     _EVT_resume
*
* Description:  Installs the event handling code for the MGL.
*
****************************************************************************/
{
    int     i;

    /* Build free list, and initialize global data structures */
    for (i = 0; i < EVENTQSIZE; i++)
        evtq[i].next = i+1;
    evtq[EVENTQSIZE-1].next = -1;       /* Terminate list           */
    count = freeHead = 0;
    head = tail = -1;
    timerReset = timerCount = 0;
    oldMove = -1;
    oldKey = -1;

    /* Catch program termination signals so we can clean up properly */
    signal(SIGABRT, _EVT_abort);
    signal(SIGFPE, _EVT_abort);
    signal(SIGINT, _EVT_abort);
    signal(SIGTERM, _EVT_abort);
    _EVT_installed = true;
}

/* {secret} */
void _EVT_init(MGLDC *dc)
/****************************************************************************
*
* Function:     _EVT_init
*
* Description:  Initiliase the event handling module. Here we install our
*               mouse handling ISR to be called whenever any button's are
*               pressed or released. We also build the free list of events
*               in the event queue.
*
*               We use handler number 2 of the mouse libraries interrupt
*               handlers for our event handling routines.
*
****************************************************************************/
{
    /* TODO: X11 mouse initialization */
    XSelectInput(dc->wm.xwindc.dpy, dc->wm.xwindc.wnd,
                 ExposureMask|KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask);

    _EVT_resume();
}

/* {secret} */
void _EVT_suspend(void)
/****************************************************************************
*
* Function:     _EVT_suspend
*
* Description:  Suspends all of our event handling operations. This is
*               also used to de-install the event handling code.
*
****************************************************************************/
{
    if (_EVT_installed) {
        signal(SIGABRT, SIG_DFL);
        signal(SIGFPE, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        _EVT_installed = false;
    }
}

/****************************************************************************
DESCRIPTION:
Set the interval between EVT_TIMERTICK events.

HEADER:
mgraph.h

RETURNS:
Old value of timer tick interval

PARAMETERS:
ticks   - New value for timer tick interval (in milliseconds)

REMARKS:
This routine sets the number of ticks between each posting of the
EVT_TIMERTICK event to the event queue. The EVT_TIMERTICK event is off
by default. You can turn off the posting of EVT_TIMERTICK events by setting the
tick interval to 0.

Under Windows, one tick is approximately equal to 1/1000 of a second
(millisecond).

Under MSDOS, one tick is approximately equal to 1/18.2 of a second. To work out
a precise interval given in seconds, use the following expression:

    ticks = secs * (1193180.0 / 65536.0);

****************************************************************************/
int MGLAPI EVT_setTimerTick(
    int ticks)
{
#if 0
    int     oldticks;
    uint    ps;

    timerCount = ticks;
    oldticks = timerReset;
    timerReset = ticks;

    return oldticks;
#endif
    printf("Timer tick events not implemented yet :-( \n");
    return 0;
}

/* {secret} */
void MGLAPI _EVT_cCodeEnd(void) {}
