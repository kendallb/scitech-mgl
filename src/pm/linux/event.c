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
* Environment:  Linux
*
* Description:  Linux fullscreen console implementation for the SciTech
*               cross platform event library.
*
****************************************************************************/

/*---------------------------- Global Variables ---------------------------*/

extern int              _PM_haveConsole;
extern int              _PM_console_fd;
static ushort           keyUpMsg[256] = {0};
static int              range_x, range_y;
static short            *axis0 = NULL, *axis1 = NULL;
static uchar            *buts0 = NULL, *buts1 = NULL;
static int              joystick0_fd = 0, joystick1_fd = 0;
static int              js_version = 0;
static char             js0_axes = 0, js0_buttons = 0;
static char             js1_axes = 0, js1_buttons = 0;
static char             joystick0_dev[20] = "/dev/js0";
static char             joystick1_dev[20] = "/dev/js1";
#ifndef NO_PTHREAD
static pthread_t        tid_eventThread;
#else
static pid_t            pid_eventThread;
static char             thread_stack[THREAD_STACK_SIZE];
#endif

#define STD_FLG (CREAD | CLOCAL | HUPCL)

#define MOUSE_RBUTTON   0x01
#define MOUSE_MBUTTON   0x02
#define MOUSE_LBUTTON   0x04

/* The name of the environment variables used by event code */

#define ENV_JOYDEV0  "SNAP_JOYDEV1"
#define ENV_JOYDEV1  "SNAP_JOYDEV2"

/* Scancode mappings on Linux for special keys */

typedef struct {
    int scan;
    int map;
    } keymap;

// TODO: Fix this and set it up so we can do a binary search!

keymap keymaps[] = {
    {96, KB_padEnter},
    {74, KB_padMinus},
    {78, KB_padPlus},
    {55, KB_padTimes},
    {98, KB_padDivide},
    {71, KB_padHome},
    {72, KB_padUp},
    {73, KB_padPageUp},
    {75, KB_padLeft},
    {76, KB_padCenter},
    {77, KB_padRight},
    {79, KB_padEnd},
    {80, KB_padDown},
    {81, KB_padPageDown},
    {82, KB_padInsert},
    {83, KB_padDelete},
    {105,KB_left},
    {108,KB_down},
    {106,KB_right},
    {103,KB_up},
    {110,KB_insert},
    {102,KB_home},
    {104,KB_pageUp},
    {111,KB_delete},
    {107,KB_end},
    {109,KB_pageDown},
    {125,KB_leftWindows},
    {126,KB_rightWindows},
    {127,KB_menu},
    {100,KB_rightAlt},
    {97,KB_rightCtrl},
    };

/* And the keypad with num lock turned on (changes the ASCII code only) */

keymap keypad[] = {
    {71, ASCII_7},
    {72, ASCII_8},
    {73, ASCII_9},
    {75, ASCII_4},
    {76, ASCII_5},
    {77, ASCII_6},
    {79, ASCII_1},
    {80, ASCII_2},
    {81, ASCII_3},
    {82, ASCII_0},
    {83, ASCII_period},
    };

#define NB_KEYMAPS (sizeof(keymaps)/sizeof(keymaps[0]))
#define NB_KEYPAD (sizeof(keypad)/sizeof(keypad[0]))

/*---------------------------- Implementation -----------------------------*/

/* Functions defined into linux/mouse.c */
extern ibool SetupMice (void) ;
extern void ShutdownMice (void) ;
ibool GetMouseUpdate (uchar *buttons, uchar *lastButtons, int *dx, int *dy, int *dz) ;

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
Macros to lock and unlock exclusive access to the event queue data by
the keyboard and mouse processing threads.
****************************************************************************/
#define ENTER_CRITICAL_SECTION()    _PM_lockEventQueue()
#define LEAVE_CRITICAL_SECTION()    _PM_unlockEventQueue()

/****************************************************************************
REMARKS:
Macro used by OS generic code to get exclusive access to the event queue
****************************************************************************/
#define _EVT_disableInt()           ENTER_CRITICAL_SECTION()
#define _EVT_restoreInt(flags)      LEAVE_CRITICAL_SECTION()

/****************************************************************************
REMARKS:
This function is used to return the number of ticks since system
startup in milliseconds. This should be the same value that is placed into
the time stamp fields of events, and is used to implement auto mouse down
events.
****************************************************************************/
ulong _EVT_getTicks(void)
{
    static uint     starttime = 0;
    struct timeval  t;

    gettimeofday(&t, NULL);
    if (starttime == 0)
        starttime = t.tv_sec * 1000 + (t.tv_usec/1000);
    return ((t.tv_sec * 1000 + (t.tv_usec/1000)) - starttime);
}

/****************************************************************************
REMARKS:
Small Unix function that checks for availability on a file using select()
****************************************************************************/
static ibool dataReady(
    int fd)
{
    static struct timeval   t = { 0L, 0L };
    fd_set                  fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    return select(fd+1, &fds, NULL, NULL, &t) > 0;
}

/****************************************************************************
REMARKS:
Map a keypress via the key mapping table
****************************************************************************/
static int getKeyMapping(
    keymap *tab,
    int nb,
    int key)
{
    int i;

    for(i = 0; i < nb; i++) {
        if (tab[i].scan == key)
            return tab[i].map;
        }
    return key;
}

/****************************************************************************
REMARKS:
Create a joystick event from the joystick data
****************************************************************************/
static void makeJoyEvent(
    event_t *evt)
{
    evt->message = 0;
    if (buts0 && axis0) {
        if (buts0[0]) evt->message |= EVT_JOY1_BUTTONA;
        if (buts0[1]) evt->message |= EVT_JOY1_BUTTONB;
        evt->where_x = axis0[0];
        evt->where_y = axis0[1];
        }
    else
        evt->where_x = evt->where_y = 0;
    if (buts1 && axis1) {
        if (buts1[0]) evt->message |= EVT_JOY2_BUTTONA;
        if (buts1[1]) evt->message |= EVT_JOY2_BUTTONB;
        evt->where_x = axis1[0];
        evt->where_y = axis1[1];
        }
    else
        evt->where_x = evt->where_y = 0;
}

/****************************************************************************
REMARKS:
Read the joystick axis data
****************************************************************************/
int EVTAPI _EVT_readJoyAxis(
    int jmask,
    int *axis)
{
    int mask = 0;

    if ((js_version & ~0xffff) == 0) {
        /* Old 0.x driver */
        struct JS_DATA_TYPE js;
        if (joystick0_fd && read(joystick0_fd, &js, JS_RETURN) == JS_RETURN) {
            if (jmask & EVT_JOY_AXIS_X1)
                axis[0] = js.x;
            if (jmask & EVT_JOY_AXIS_Y1)
                axis[1] = js.y;
            mask |= EVT_JOY_AXIS_X1|EVT_JOY_AXIS_Y1;
            }
        if (joystick1_fd && read(joystick1_fd, &js, JS_RETURN) == JS_RETURN) {
            if (jmask & EVT_JOY_AXIS_X2)
                axis[2] = js.x;
            if (jmask & EVT_JOY_AXIS_Y2)
                axis[3] = js.y;
            mask |= EVT_JOY_AXIS_X2|EVT_JOY_AXIS_Y2;
            }
        }
    else {
        if (axis0) {
            if (jmask & EVT_JOY_AXIS_X1)
                axis[0] = axis0[0];
            if (jmask & EVT_JOY_AXIS_Y1)
                axis[1] = axis0[1];
            mask |= EVT_JOY_AXIS_X1 | EVT_JOY_AXIS_Y1;
            }
        if (axis1) {
            if (jmask & EVT_JOY_AXIS_X2)
                axis[2] = axis1[0];
            if (jmask & EVT_JOY_AXIS_Y2)
                axis[3] = axis1[1];
            mask |= EVT_JOY_AXIS_X2 | EVT_JOY_AXIS_Y2;
            }
        }
    return mask;
}

/****************************************************************************
REMARKS:
Read the joystick button data
****************************************************************************/
int EVTAPI _EVT_readJoyButtons(void)
{
    int buts = 0;

    if ((js_version & ~0xffff) == 0) {
        /* Old 0.x driver */
        struct JS_DATA_TYPE js;
        if (joystick0_fd && read(joystick0_fd, &js, JS_RETURN) == JS_RETURN)
            buts = js.buttons;
        if (joystick1_fd && read(joystick1_fd, &js, JS_RETURN) == JS_RETURN)
            buts |= js.buttons << 2;
        }
    else {
        if (buts0)
            buts |= EVT_JOY1_BUTTONA*buts0[0] + EVT_JOY1_BUTTONB*buts0[1];
        if (buts1)
            buts |= EVT_JOY2_BUTTONA*buts1[0] + EVT_JOY2_BUTTONB*buts1[1];
        }
    return buts;
}

/****************************************************************************
DESCRIPTION:
Returns the mask indicating what joystick axes are attached.

HEADER:
event.h

REMARKS:
This function is used to detect the attached joysticks, and determine
what axes are present and functioning. This function will re-detect any
attached joysticks when it is called, so if the user forgot to attach
the joystick when the application started, you can call this function to
re-detect any newly attached joysticks.

SEE ALSO:
EVT_joySetLowerRight, EVT_joySetCenter, EVT_joyIsPresent
****************************************************************************/
int EVTAPI EVT_joyIsPresent(void)
{
    static int      mask = 0;
    int             i;
    char            *tmp, name0[128], name1[128];
    static ibool    inited = false;

    if (inited)
        return mask;
    memset(EVT.joyMin,0,sizeof(EVT.joyMin));
    memset(EVT.joyCenter,0,sizeof(EVT.joyCenter));
    memset(EVT.joyMax,0,sizeof(EVT.joyMax));
    memset(EVT.joyPrev,0,sizeof(EVT.joyPrev));
    EVT.joyButState = 0;
    if ((tmp = getenv(ENV_JOYDEV0)) != NULL)
        strcpy(joystick0_dev,tmp);
    if ((tmp = getenv(ENV_JOYDEV1)) != NULL)
        strcpy(joystick1_dev,tmp);
    if ((joystick0_fd = open(joystick0_dev, O_RDONLY)) < 0)
        joystick0_fd = 0;
    if ((joystick1_fd = open(joystick1_dev, O_RDONLY)) < 0)
        joystick1_fd = 0;
    if (!joystick0_fd && !joystick1_fd) // No joysticks detected
        return 0;
    inited = true;
    if (ioctl(joystick0_fd ? joystick0_fd : joystick1_fd, JSIOCGVERSION, &js_version) < 0)
        return 0;

    /* Initialise joystick 0 */
    if (joystick0_fd) {
        ioctl(joystick0_fd, JSIOCGNAME(sizeof(name0)), name0);
        if (js_version & ~0xffff) {
            struct js_event js;

            ioctl(joystick0_fd, JSIOCGAXES, &js0_axes);
            ioctl(joystick0_fd, JSIOCGBUTTONS, &js0_buttons);
            axis0 = PM_calloc((int)js0_axes, sizeof(short));
            buts0 = PM_malloc((int)js0_buttons);
            /* Read the initial events */
            while(dataReady(joystick0_fd)
                  && read(joystick0_fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)
                  && (js.type & JS_EVENT_INIT)
                  ) {
                if (js.type & JS_EVENT_BUTTON)
                    buts0[js.number] = js.value;
                else if (js.type & JS_EVENT_AXIS)
                    axis0[js.number] = scaleJoyAxis(js.value,js.number);
                }
            }
        else {
            js0_axes = 2;
            js0_buttons = 2;
            axis0 = PM_calloc((int)js0_axes, sizeof(short));
            buts0 = PM_malloc((int)js0_buttons);
            }
        }

    /* Initialise joystick 1 */
    if (joystick1_fd) {
        ioctl(joystick1_fd, JSIOCGNAME(sizeof(name1)), name1);
        if (js_version & ~0xffff) {
            struct js_event js;

            ioctl(joystick1_fd, JSIOCGAXES, &js1_axes);
            ioctl(joystick1_fd, JSIOCGBUTTONS, &js1_buttons);
            axis1 = PM_calloc((int)js1_axes, sizeof(short));
            buts1 = PM_malloc((int)js1_buttons);
            /* Read the initial events */
            while(dataReady(joystick1_fd)
                  && read(joystick1_fd, &js, sizeof(struct js_event))==sizeof(struct js_event)
                  && (js.type & JS_EVENT_INIT)
                  ) {
                if (js.type & JS_EVENT_BUTTON)
                    buts1[js.number] = js.value;
                else if (js.type & JS_EVENT_AXIS)
                    axis1[js.number] = scaleJoyAxis(js.value,js.number<<2);
                }
            }
        else {
            js1_axes = 2;
            js1_buttons = 2;
            axis1 = PM_calloc((int)js1_axes, sizeof(short));
            buts1 = PM_malloc((int)js1_buttons);
            }
        }

#ifdef  CHECKED
    fprintf(stderr,"Using joystick driver version %d.%d.%d\n",
            js_version >> 16, (js_version >> 8) & 0xff, js_version & 0xff);
    if (joystick0_fd)
        fprintf(stderr,"Joystick 1 (%s): %s\n", joystick0_dev, name0);
    if (joystick1_fd)
        fprintf(stderr,"Joystick 2 (%s): %s\n", joystick1_dev, name1);
#endif
    mask = _EVT_readJoyAxis(EVT_JOY_AXIS_ALL,EVT.joyCenter);
    if (mask) {
        for (i = 0; i < JOY_NUM_AXES; i++)
            EVT.joyMax[i] = EVT.joyCenter[i]*2;
        }
    return mask;
}

/****************************************************************************
DESCRIPTION:
Polls the joystick for position and button information.

HEADER:
event.h

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

SEE ALSO:
EVT_getNext, EVT_peekNext, EVT_joySetUpperLeft, EVT_joySetLowerRight,
EVT_joySetCenter, EVT_joyIsPresent
****************************************************************************/
void EVTAPI EVT_pollJoystick(void)
{
    event_t evt;
    int     i,axis[JOY_NUM_AXES],newButState,mask,moved,ps;

    /* TODO: Can this be removed and/or put in a separate thread eventually? */

    if ((js_version & ~0xFFFF) == 0 && EVT.joyMask) {
        /* Read joystick axes and post movement events if they have
         * changed since the last time we polled. Until the events are
         * actually flushed, we keep modifying the same joystick movement
         * event, so you won't get multiple movement event
         */
        mask = _EVT_readJoyAxis(EVT.joyMask,axis);
        newButState = _EVT_readJoyButtons();
        moved = false;
        for (i = 0; i < JOY_NUM_AXES; i++) {
            if (mask & (EVT_JOY_AXIS_X1 << i))
                axis[i] = scaleJoyAxis(axis[i],i);
            else
                axis[i] = EVT.joyPrev[i];
            if (axis[i] != EVT.joyPrev[i])
                moved = true;
            }
        if (moved) {
            ps = _EVT_disableInt();
            memcpy(EVT.joyPrev,axis,sizeof(EVT.joyPrev));
            if (EVT.oldJoyMove != -1) {
                /* Modify the existing joystick movement event */
                EVT.evtq[EVT.oldJoyMove].message = newButState;
                EVT.evtq[EVT.oldJoyMove].where_x = EVT.joyPrev[0];
                EVT.evtq[EVT.oldJoyMove].where_y = EVT.joyPrev[1];
                EVT.evtq[EVT.oldJoyMove].relative_x = EVT.joyPrev[2];
                EVT.evtq[EVT.oldJoyMove].relative_y = EVT.joyPrev[3];
                }
            else if (EVT.count < EVENTQSIZE) {
                /* Add a new joystick movement event */
                EVT.oldJoyMove = EVT.freeHead;
                memset(&evt,0,sizeof(evt));
                evt.what = EVT_JOYMOVE;
                evt.message = EVT.joyButState;
                evt.where_x = EVT.joyPrev[0];
                evt.where_y = EVT.joyPrev[1];
                evt.relative_x = EVT.joyPrev[2];
                evt.relative_y = EVT.joyPrev[3];
                addEvent(&evt);
                }
            _EVT_restoreInt(ps);
            }

        /* Read the joystick buttons, and post events to reflect the change
         * in state for the joystick buttons.
         */
        if (newButState != EVT.joyButState) {
            ps = _EVT_disableInt();
            if (EVT.count < EVENTQSIZE) {
                /* Add a new joystick movement event */
                memset(&evt,0,sizeof(evt));
                evt.what = EVT_JOYCLICK;
                evt.message = newButState;
                EVT.evtq[EVT.oldJoyMove].where_x = EVT.joyPrev[0];
                EVT.evtq[EVT.oldJoyMove].where_y = EVT.joyPrev[1];
                EVT.evtq[EVT.oldJoyMove].relative_x = EVT.joyPrev[2];
                EVT.evtq[EVT.oldJoyMove].relative_y = EVT.joyPrev[3];
                addEvent(&evt);
                }
            EVT.joyButState = newButState;
            _EVT_restoreInt(ps);
            }
        }
}

/****************************************************************************
DESCRIPTION:
Calibrates the joystick upper left position

HEADER:
event.h

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

SEE ALSO:
EVT_joySetUpperLeft, EVT_joySetLowerRight, EVT_joyIsPresent
****************************************************************************/
void EVTAPI EVT_joySetUpperLeft(void)
{
    _EVT_readJoyAxis(EVT_JOY_AXIS_ALL,EVT.joyMin);
}

/****************************************************************************
DESCRIPTION:
Calibrates the joystick lower right position

HEADER:
event.h

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

SEE ALSO:
EVT_joySetUpperLeft, EVT_joySetCenter, EVT_joyIsPresent
****************************************************************************/
void EVTAPI EVT_joySetLowerRight(void)
{
    _EVT_readJoyAxis(EVT_JOY_AXIS_ALL,EVT.joyMax);
}

/****************************************************************************
DESCRIPTION:
Calibrates the joystick center position

HEADER:
event.h

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

SEE ALSO:
EVT_joySetUpperLeft, EVT_joySetLowerRight, EVT_joySetCenter
****************************************************************************/
void EVTAPI EVT_joySetCenter(void)
{
    _EVT_readJoyAxis(EVT_JOY_AXIS_ALL,EVT.joyCenter);
}

/****************************************************************************
PARAMETERS:
arg - Argument passed to the thread when it starts

REMARKS:
This is the keyboard event handling thread that processes all keyboard
data and posts the resulting events to the event queue.
****************************************************************************/
#ifndef NO_PTHREAD
static void *_EVT_eventThread(
#else
static int _EVT_eventThread(
#endif
    void *arg)
{
    event_t                 evt;
    int                     c;
    char                    ch;
    static struct kbentry   ke;
    static ushort           repeatKey[128] = {0};
    fd_set                  s;
    struct timeval          *ptv = NULL;

    for (;;) {
        FD_ZERO(&s);
        FD_SET(_PM_console_fd, &s);
        while (select(_PM_console_fd+1, &s, NULL, NULL, ptv) > 0) {
            if (read(_PM_console_fd, &ch, 1) > 0) {
                evt.when = _EVT_getTicks();
                ENTER_CRITICAL_SECTION();
                if (_PM_haveConsole) {
                    /* Handle keyboard for real consoles */
                    c = ch & 0x7F;
                    if (ch & 0x80) {
                        /* Key released */
                        evt.what = EVT_KEYUP;
                        switch (c) {
                            case KB_leftShift:
                                _PM_modifiers &= ~EVT_LEFTSHIFT;
                                break;
                            case KB_rightShift:
                                _PM_modifiers &= ~EVT_RIGHTSHIFT;
                                break;
                            case 29:
                                _PM_modifiers &= ~(EVT_LEFTCTRL|EVT_CTRLSTATE);
                                break;
                            case 97:            /* Control */
                                _PM_modifiers &= ~EVT_CTRLSTATE;
                                break;
                            case 56:
                                _PM_modifiers &= ~(EVT_LEFTALT|EVT_ALTSTATE);
                                break;
                            case 100:
                                _PM_modifiers &= ~EVT_ALTSTATE;
                                break;
                            }
                        evt.modifiers = _PM_modifiers;
                        evt.message = keyUpMsg[c];
                        if (EVT.count < EVENTQSIZE)
                            addEvent(&evt);
                        keyUpMsg[c] = 0;
                        repeatKey[c] = 0;
                        }
                    else {
                        /* Key pressed */
                        evt.what = EVT_KEYDOWN;
                        switch (c) {
                            case KB_leftShift:
                                _PM_modifiers |= EVT_LEFTSHIFT;
                                break;
                            case KB_rightShift:
                                _PM_modifiers |= EVT_RIGHTSHIFT;
                                break;
                            case 29:
                                _PM_modifiers |= EVT_LEFTCTRL|EVT_CTRLSTATE;
                                break;
                            case 97:            /* Control */
                                _PM_modifiers |= EVT_CTRLSTATE;
                                break;
                            case 56:
                                _PM_modifiers |= EVT_LEFTALT|EVT_ALTSTATE;
                                break;
                            case 100:
                                _PM_modifiers |= EVT_ALTSTATE;
                                break;
                            case KB_capsLock:   /* Caps Lock */
                                _PM_leds ^= LED_CAP;
                                ioctl(_PM_console_fd, KDSETLED, _PM_leds);
                                break;
                            case KB_numLock:    /* Num Lock */
                                _PM_leds ^= LED_NUM;
                                ioctl(_PM_console_fd, KDSETLED, _PM_leds);
                                break;
                            case KB_scrollLock: /* Scroll Lock */
                                _PM_leds ^= LED_SCR;
                                ioctl(_PM_console_fd, KDSETLED, _PM_leds);
                                break;
                            }
                        evt.modifiers = _PM_modifiers;
                        if (keyUpMsg[c]) {
                            evt.what = EVT_KEYREPEAT;
                            evt.message = keyUpMsg[c] | (repeatKey[c]++ << 16);
                            }
                        else {
                            int asc;
    
                            evt.message = getKeyMapping(keymaps, NB_KEYMAPS, c) << 8;
                            if ((_PM_leds & LED_NUM) && (getKeyMapping(keypad, NB_KEYPAD, c) != c))
                                asc = getKeyMapping(keypad, NB_KEYPAD, c);
                            else {
                                ke.kb_index = c;
                                ke.kb_table = 0;
                                if ((_PM_modifiers & EVT_SHIFTKEY) || (_PM_leds & LED_CAP))
                                    ke.kb_table |= K_SHIFTTAB;
                                if (_PM_modifiers & (EVT_LEFTALT | EVT_ALTSTATE))
                                    ke.kb_table |= K_ALTTAB;
                                if (ioctl(_PM_console_fd, KDGKBENT, (unsigned long)&ke) < 0)
                                    perror("ioctl(KDGKBENT)");
                                switch (c) {
                                    case 14:
                                        asc = ASCII_backspace;
                                        break;
                                    case 15:
                                        asc = ASCII_tab;
                                        break;
                                    case 28:
                                    case 96:
                                        asc = ASCII_enter;
                                        break;
                                    case 1:
                                        asc = ASCII_esc;
                                    default:
                                        asc = ke.kb_value & 0xFF;
                                        if (asc < 0x1B)
                                            asc = 0;
                                        break;
                                    }
                                }
                            if ((_PM_modifiers & (EVT_CTRLSTATE|EVT_LEFTCTRL)) && isalpha(asc))
                                evt.message |= toupper(asc) - 'A' + 1;
                            else
                                evt.message |= asc;
                            keyUpMsg[c] = evt.message;
                            repeatKey[c]++;
                            }
                        if (EVT.count < EVENTQSIZE)
                            addEvent(&evt);
                        }
                    }
                else {                                            
                    /* Handle serial/SSH console support with the keyboard */
                    // TODO: Add some basic translation for arrow keys etc!
                    evt.what = EVT_KEYDOWN;
                    evt.modifiers = 0;
                    evt.message = ch;
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    evt.what = EVT_KEYUP;
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }                                
                LEAVE_CRITICAL_SECTION();
                }
            FD_ZERO(&s);
            FD_SET(_PM_console_fd, &s);
            }
        }
    return 0;
}

/****************************************************************************
REMARKS:
Obtains and pumps mouse and joystick messages into our event queue.
****************************************************************************/
static void _EVT_pumpMessages(void)
{
    uchar           buttons;
    uchar           oldButtons;
    int             dx, dy, dz;
    event_t         evt;

    /* Poll mouse events */
    while (GetMouseUpdate (&buttons, &oldButtons, &dx, &dy, &dz)) {
        EVT.mx += dx;
        EVT.my += dy;
        if (EVT.mx < 0)
            EVT.mx = 0;
        if (EVT.my < 0)
            EVT.my = 0;
        if (EVT.mx > range_x)
            EVT.mx = range_x;
        if (EVT.my > range_y)
            EVT.my = range_y;
        evt.when = _EVT_getTicks();
        evt.where_x = EVT.mx;
        evt.where_y = EVT.my;
        evt.relative_x = dx;
        evt.relative_y = dy;

        evt.modifiers = _PM_modifiers;
        if (buttons & MOUSE_RBUTTON)
            evt.modifiers |= EVT_RIGHTBUT;
        if (buttons & MOUSE_MBUTTON)
            evt.modifiers |= EVT_MIDDLEBUT;
        if (buttons & MOUSE_LBUTTON)
            evt.modifiers |= EVT_LEFTBUT;

        /* Left click events */
        if ((buttons & MOUSE_LBUTTON) != (oldButtons & MOUSE_LBUTTON)) {
            evt.what = buttons & MOUSE_LBUTTON ? EVT_MOUSEDOWN : EVT_MOUSEUP ;
            evt.message = EVT_LEFTBMASK;
            EVT.oldMove = -1;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }

        /* Right click events */
        if ((buttons & MOUSE_RBUTTON) != (oldButtons & MOUSE_RBUTTON)) {
            evt.what = buttons & MOUSE_RBUTTON ? EVT_MOUSEDOWN : EVT_MOUSEUP ;
            evt.message = EVT_RIGHTBMASK;
            EVT.oldMove = -1;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }

        /* Middle click events */
        if ((buttons & MOUSE_MBUTTON) != (oldButtons & MOUSE_MBUTTON)) {
            evt.what = buttons & MOUSE_MBUTTON ? EVT_MOUSEDOWN : EVT_MOUSEUP ;
            evt.message = EVT_MIDDLEBMASK;
            EVT.oldMove = -1;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }

        /* Mouse movement event */
        if (dx || dy) {
            evt.what = EVT_MOUSEMOVE;
            evt.message = 0;
            if (EVT.oldMove != -1) {
                /* Modify existing movement event */
                EVT.evtq[EVT.oldMove].where_x = evt.where_x;
                EVT.evtq[EVT.oldMove].where_y = evt.where_y;
                }
            else {
                /* Save id of this movement event */
                EVT.oldMove = EVT.freeHead;
                if (EVT.count < EVENTQSIZE)
                    addEvent(&evt);
                }
            }

        /* Mouse wheel event */
        if (dz) {
            evt.what = EVT_MOUSEWHEEL;
            evt.message = MW_HORZWHEEL ;
            if (dz > 0)
                evt.message |= EVT_WHEELDIRMASK ;
            else
                dz = -dz ;
            evt.message |= dz & 0x0f ;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }
        }
    
    /* Poll joystick events using the 1.x joystick driver API in the 2.2 kernels */
    if (js_version & ~0xffff) {
        static struct js_event  js;

        /* Read joystick axis 0 */
        evt.when = 0;
        evt.modifiers = _PM_modifiers;
        if (joystick0_fd && dataReady(joystick0_fd) &&
                read(joystick0_fd, &js, sizeof(js)) == sizeof(js)) {
            if (js.type & JS_EVENT_BUTTON) {
                if (js.number < 2) { /* Only 2 buttons for now :( */
                    buts0[js.number] = js.value;
                    evt.what = EVT_JOYCLICK;
                    makeJoyEvent(&evt);
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }
                }
            else if (js.type & JS_EVENT_AXIS) {
                axis0[js.number] = scaleJoyAxis(js.value,js.number);
                evt.what = EVT_JOYMOVE;
                if (EVT.oldJoyMove != -1) {
                    makeJoyEvent(&EVT.evtq[EVT.oldJoyMove]);
                    }
                else if (EVT.count < EVENTQSIZE) {
                    EVT.oldJoyMove = EVT.freeHead;
                    makeJoyEvent(&evt);
                    addEvent(&evt);
                    }
                }
            }

        /* Read joystick axis 1 */
        if (joystick1_fd && dataReady(joystick1_fd) &&
                read(joystick1_fd, &js, sizeof(js))==sizeof(js)) {
            if (js.type & JS_EVENT_BUTTON) {
                if (js.number < 2) { /* Only 2 buttons for now :( */
                    buts1[js.number] = js.value;
                    evt.what = EVT_JOYCLICK;
                    makeJoyEvent(&evt);
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }
                }
            else if (js.type & JS_EVENT_AXIS) {
                axis1[js.number] = scaleJoyAxis(js.value,js.number<<2);
                evt.what = EVT_JOYMOVE;
                if (EVT.oldJoyMove != -1) {
                    makeJoyEvent(&EVT.evtq[EVT.oldJoyMove]);
                    }
                else if (EVT.count < EVENTQSIZE) {
                    EVT.oldJoyMove = EVT.freeHead;
                    makeJoyEvent(&evt);
                    addEvent(&evt);
                    }
                }
            }
        }
}

/****************************************************************************
REMARKS:
This macro/function is used to converts the scan codes reported by the
keyboard to our event libraries normalised format. We only have one scan
code for the 'A' key, and use shift _PM_modifiers to determine if it is a
Ctrl-F1, Alt-F1 etc. The raw scan codes from the keyboard work this way,
but the OS gives us 'cooked' scan codes, we have to translate them back
to the raw format.
****************************************************************************/
#define _EVT_maskKeyCode(evt)

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
    int             i;
#ifndef NO_PTHREAD
    pthread_attr_t  attr;
#endif    

    /* Initialise the event queue */
    EVT.mouseMove = mouseMove;
    initEventQueue();
    for (i = 0; i < 256; i++)
        keyUpMsg[i] = 0;

    /* Keyboard initialization */
    if (_PM_console_fd == -1)
        PM_fatalError("You must first call PM_openConsole to use the EVT functions!");
    _PM_keyboard_rawmode();
        
    /* Initialise the mouse module */
    SetupMice();

    /* Create the thread used to process events */
#ifndef NO_PTHREAD
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid_eventThread, &attr, _EVT_eventThread, NULL);
#else    
    pid_eventThread = clone(_EVT_eventThread, &thread_stack[THREAD_STACK_SIZE], CLONE_FS | CLONE_FILES | CLONE_VM, NULL);
#endif
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
    range_x = xRes;
    range_y = yRes;
}

/****************************************************************************
REMARKS
Modifes the mouse coordinates as necessary if scaling to OS coordinates,
and sets the OS mouse cursor position.
****************************************************************************/
#define _EVT_setMousePos(x,y)

/****************************************************************************
REMARKS:
Initiailises the internal event handling modules. The EVT_suspend function
can be called to suspend event handling (such as when shelling out to DOS),
and this function can be used to resume it again later.
****************************************************************************/
void EVT_resume(void)
{
    // Do nothing for Linux
}

/****************************************************************************
REMARKS
Suspends all of our event handling operations. This is also used to
de-install the event handling code.
****************************************************************************/
void EVT_suspend(void)
{
    // Do nothing for Linux
}

/****************************************************************************
REMARKS
Exits the event module for program terminatation.
****************************************************************************/
void EVT_exit(void)
{
#if defined(NO_PTHREAD)
    kill(pid_eventThread,SIGKILL);
#endif
    _PM_restore_kb_mode();
    ShutdownMice();
    if (joystick0_fd) {
        close(joystick0_fd);
        free(axis0);
        free(buts0);
        joystick0_fd = 0;
        }
    if (joystick1_fd) {
        close(joystick1_fd);
        free(axis1);
        free(buts1);
        joystick1_fd = 0;
        }
}

