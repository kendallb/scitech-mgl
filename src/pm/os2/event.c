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
* Environment:  IBM PC (OS/2)
*
* Description:  OS/2 implementation for the SciTech cross platform
*               event library.
*
****************************************************************************/

/*---------------------------- Global Variables ---------------------------*/

/* Define generous keyboard monitor circular buffer size to minimize
 * the danger of losing keystrokes
 */
#define KEYBUFSIZE  (EVENTQSIZE + 10)

static int      oldMouseState;          /* Old mouse state               */
static ulong    oldKeyMessage;          /* Old keyboard state            */
static ushort   keyUpMsg[256] = {0};    /* Table of key up messages      */
static int      rangeX,rangeY;          /* Range of mouse coordinates    */
HMOU            _EVT_hMouse;            /* Handle to the mouse driver    */
HMONITOR        _EVT_hKbdMon;           /* Handle to the keyboard driver */
TID             kbdMonTID = 0;          /* Keyboard monitor thread ID    */
HEV             hevStart;               /* Start event semaphore handle  */
BOOL            bMonRunning;            /* Flag set if monitor thread OK */
HMTX            hmtxKeyBuf;             /* Mutex protecting key buffer   */
KEYPACKET       keyMonPkts[KEYBUFSIZE]; /* Array of monitor key packets  */
int             kpHead = 0;             /* Key packet buffer head        */
int             kpTail = 0;             /* Key packet buffer tail        */

// Uncomment to enable debugging messages in this code
//#define DEBUG_MSGS

/*---------------------------- Implementation -----------------------------*/

/* These are not used under OS/2 */
#define _EVT_disableInt()       1
#define _EVT_restoreInt(flags)

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
****************************************************************************/
ulong _EVT_getTicks(void)
{
    ULONG   count;
    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &count, sizeof(ULONG) );
    return count;
}

/****************************************************************************
REMARKS:
Converts a mickey movement value to a pixel adjustment value.
****************************************************************************/
static int MickeyToPixel(
    int mickey)
{
    // TODO: We can add some code in here to handle 'acceleration' for
    //       the mouse cursor. For now just use the mickeys.
    return mickey;
}

/* Some useful defines any typedefs used in the keyboard handling */
#define KEY_RELEASE             0x40

/****************************************************************************
REMARKS:
Pumps all messages in the message queue from OS/2 into our event queue.
****************************************************************************/
static void _EVT_pumpMessages(void)
{
    KBDINFO         keyInfo;        /* Must not cross a 64K boundary */
    KBDKEYINFO      key;            /* Must not cross a 64K boundary */
    MOUQUEINFO      mqueue;         /* Must not cross a 64K boundary */
    MOUEVENTINFO    mouse;          /* Must not cross a 64K boundary */
    ushort          mWait;          /* Must not cross a 64K boundary */
    KEYPACKET       kp;             /* Must not cross a 64K boundary */
    event_t         evt;
    int             scan;
    ibool           noInput = TRUE; /* Flag to determine if any input was available */

    /* First of all, check if we should do any session switch work */
    __PM_checkConsoleSwitch();

    /* Pump all keyboard messages */
    if (bMonRunning) {
        /* Extract messages from our circular buffer */
        for (;;) {
            /* Check that the monitor thread is still running */
            if (!bMonRunning)
                PM_fatalError("Keyboard monitor thread died!");

            /* Protect keypacket buffer with mutex */
            DosRequestMutexSem(hmtxKeyBuf, SEM_INDEFINITE_WAIT);
            if (kpHead == kpTail) {
                DosReleaseMutexSem(hmtxKeyBuf);
                break;
                }

            noInput = FALSE;

            /* Read packet from circular buffer and remove it */
            memcpy(&kp, &keyMonPkts[kpTail], sizeof(KEYPACKET));
            if (++kpTail == KEYBUFSIZE)
                kpTail = 0;
            DosReleaseMutexSem(hmtxKeyBuf);

            /* Compensate for the 0xE0 character */
            if (kp.XlatedScan && kp.XlatedChar == 0xE0)
                kp.XlatedChar = 0;

            /* Determine type of keyboard event */
            memset(&evt,0,sizeof(evt));
            if (kp.KbdDDFlagWord & KEY_RELEASE)
                evt.what = EVT_KEYUP;
            else
                evt.what = EVT_KEYDOWN;

            /* Convert keyboard codes */
            scan = kp.MonFlagWord >> 8;
            if (evt.what == EVT_KEYUP) {
                /* Get message for keyup code from table of cached down values */
                evt.message = keyUpMsg[scan];
                keyUpMsg[scan] = 0;
                oldKeyMessage = -1;
                }
            else {
                evt.message = ((ulong)scan << 8) | kp.XlatedChar;
                if (evt.message == keyUpMsg[scan]) {
                    evt.what = EVT_KEYREPEAT;
                    evt.message |= 0x10000;
                    }
                oldKeyMessage = evt.message & 0x0FFFF;
                keyUpMsg[scan] = (ushort)evt.message;
                }

            /* Convert shift state modifiers */
            if (kp.u.ShiftState & 0x0001)
                evt.modifiers |= EVT_RIGHTSHIFT;
            if (kp.u.ShiftState & 0x0002)
                evt.modifiers |= EVT_LEFTSHIFT;
            if (kp.u.ShiftState & 0x0100)
                evt.modifiers |= EVT_LEFTCTRL;
            if (kp.u.ShiftState & 0x0200)
                evt.modifiers |= EVT_LEFTALT;
            if (kp.u.ShiftState & 0x0400)
                evt.modifiers |= EVT_RIGHTCTRL;
            if (kp.u.ShiftState & 0x0800)
                evt.modifiers |= EVT_RIGHTALT;
            EVT.oldMove = -1;

            /* Add time stamp and add the event to the queue */
            evt.when = key.time;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);
            }

        /* Don't just flush because that terminally confuses the monitor */
        do {
            KbdCharIn(&key, IO_NOWAIT, 0);
            } while (key.fbStatus & KBDTRF_FINAL_CHAR_IN);
        }
    else {
        while (KbdCharIn(&key, IO_NOWAIT, 0) == NO_ERROR && key.fbStatus) {

            /* Determine type of keyboard event */
            memset(&evt,0,sizeof(evt));
            evt.what = EVT_KEYDOWN;
            evt.message = (key.chScan << 8) | key.chChar;

            /* Convert shift state modifiers */
            if (key.fsState & 0x0001)
                evt.modifiers |= EVT_RIGHTSHIFT;
            if (key.fsState & 0x0002)
                evt.modifiers |= EVT_LEFTSHIFT;
            if (key.fsState & 0x0100)
                evt.modifiers |= EVT_LEFTCTRL;
            if (key.fsState & 0x0200)
                evt.modifiers |= EVT_LEFTALT;
            if (key.fsState & 0x0400)
                evt.modifiers |= EVT_RIGHTCTRL;
            if (key.fsState & 0x0800)
                evt.modifiers |= EVT_RIGHTALT;
            EVT.oldMove = -1;

            /* Add time stamp and add the event to the queue */
            evt.when = key.time;
            if (EVT.count < EVENTQSIZE)
                addEvent(&evt);

            if (key.fbStatus & KBDTRF_FINAL_CHAR_IN)
                break;
            }
        }

    /* Pump all mouse messages */
    KbdGetStatus(&keyInfo,0);
    /* Check return code - mouse may not be operational!! */
    if (MouGetNumQueEl(&mqueue,_EVT_hMouse) == NO_ERROR) {
        while (mqueue.cEvents) {
            while (mqueue.cEvents--) {
                memset(&evt,0,sizeof(evt));
                mWait = MOU_NOWAIT;
                MouReadEventQue(&mouse,&mWait,_EVT_hMouse);

                /* Update the mouse position. We get the mouse coordinates
                 * in mickeys so we have to translate these into pixels and
                 * move our mouse position. If we don't do this, OS/2 gives
                 * us the coordinates in character positions since it still
                 * thinks we are in text mode!
                 */
                EVT.mx += MickeyToPixel(mouse.col);
                EVT.my += MickeyToPixel(mouse.row);
                if (EVT.mx < 0) EVT.mx = 0;
                if (EVT.my < 0) EVT.my = 0;
                if (EVT.mx > rangeX)    EVT.mx = rangeX;
                if (EVT.my > rangeY)    EVT.my = rangeY;
                evt.where_x = EVT.mx;
                evt.where_y = EVT.my;
                evt.relative_x = mouse.col;
                evt.relative_y = mouse.row;
                evt.when = key.time;
                if (mouse.fs & (MOUSE_BN1_DOWN | MOUSE_MOTION_WITH_BN1_DOWN))
                    evt.modifiers |= EVT_LEFTBUT;
                if (mouse.fs & (MOUSE_BN2_DOWN | MOUSE_MOTION_WITH_BN2_DOWN))
                    evt.modifiers |= EVT_RIGHTBUT;
                if (mouse.fs & (MOUSE_BN3_DOWN | MOUSE_MOTION_WITH_BN3_DOWN))
                    evt.modifiers |= EVT_MIDDLEBUT;
                if (keyInfo.fsState & 0x0001)
                    evt.modifiers |= EVT_RIGHTSHIFT;
                if (keyInfo.fsState & 0x0002)
                    evt.modifiers |= EVT_LEFTSHIFT;
                if (keyInfo.fsState & 0x0100)
                    evt.modifiers |= EVT_LEFTCTRL;
                if (keyInfo.fsState & 0x0200)
                    evt.modifiers |= EVT_LEFTALT;
                if (keyInfo.fsState & 0x0400)
                    evt.modifiers |= EVT_RIGHTCTRL;
                if (keyInfo.fsState & 0x0800)
                    evt.modifiers |= EVT_RIGHTALT;

                /* Check for left mouse click events */
                /* 0x06 == (MOUSE_BN1_DOWN | MOUSE_MOTION_WITH_BN1_DOWN) */
                if (((mouse.fs & 0x0006) && !(oldMouseState & 0x0006))
                        || (!(mouse.fs & 0x0006) && (oldMouseState & 0x0006))) {
                    if (mouse.fs & 0x0006)
                        evt.what = EVT_MOUSEDOWN;
                    else
                        evt.what = EVT_MOUSEUP;
                    evt.message = EVT_LEFTBMASK;
                    EVT.oldMove = -1;
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }

                /* Check for right mouse click events */
                /* 0x0018 == (MOUSE_BN2_DOWN | MOUSE_MOTION_WITH_BN2_DOWN) */
                if (((mouse.fs & 0x0018) && !(oldMouseState & 0x0018))
                        || (!(mouse.fs & 0x0018) && (oldMouseState & 0x0018))) {
                    if (mouse.fs & 0x0018)
                        evt.what = EVT_MOUSEDOWN;
                    else
                        evt.what = EVT_MOUSEUP;
                    evt.message = EVT_RIGHTBMASK;
                    EVT.oldMove = -1;
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }

                /* Check for middle mouse click events */
                /* 0x0060 == (MOUSE_BN3_DOWN | MOUSE_MOTION_WITH_BN3_DOWN) */
                if (((mouse.fs & 0x0060) && !(oldMouseState & 0x0060))
                        || (!(mouse.fs & 0x0060) && (oldMouseState & 0x0060))) {
                    if (mouse.fs & 0x0060)
                        evt.what = EVT_MOUSEDOWN;
                    else
                        evt.what = EVT_MOUSEUP;
                    evt.message = EVT_MIDDLEBMASK;
                    EVT.oldMove = -1;
                    if (EVT.count < EVENTQSIZE)
                        addEvent(&evt);
                    }

                /* Check for mouse movement event */
                if (mouse.fs & 0x002B) {
                    evt.what = EVT_MOUSEMOVE;
                    if (EVT.oldMove != -1) {
                        EVT.evtq[EVT.oldMove].where_x = evt.where_x;/* Modify existing one  */
                        EVT.evtq[EVT.oldMove].where_y = evt.where_y;
                        }
                    else {
                        EVT.oldMove = EVT.freeHead; /* Save id of this move event   */
                        if (EVT.count < EVENTQSIZE)
                            addEvent(&evt);
                        }
                    }

                /* Save current mouse state */
                oldMouseState = mouse.fs;
                }
            MouGetNumQueEl(&mqueue,_EVT_hMouse);
            }
            noInput = FALSE;
        }

    /* If there was no input available, give up the current timeslice
     * Note: DosSleep(0) will effectively do nothing if no other thread is ready. Hence
     * DosSleep(0) will still use 100% CPU _but_ should not interfere with other programs.
     */
    if (noInput)
        DosSleep(0);
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
Keyboard monitor thread. Needed to catch both keyup and keydown events.
****************************************************************************/
static void _kbdMonThread(
    void *params)
{
    APIRET       rc;
    KEYPACKET    kp;
    USHORT       count = sizeof(KEYPACKET);
    MONBUF       monInbuf;
    MONBUF       monOutbuf;
    int          kpNew;

    /* Raise thread priority for higher responsiveness */
    DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0);
    monInbuf.cb  = sizeof(monInbuf) - sizeof(monInbuf.cb);
    monOutbuf.cb = sizeof(monOutbuf) - sizeof(monOutbuf.cb);
    bMonRunning = FALSE;

//    printf("monInbuf = %X,   kp = %X,  count = %X\n", *((PULONG)&monInbuf), *((PULONG)&kp), (ULONG)count);
//    printf("&monInbuf = %P, &kp = %P, &count = %P\n", &monInbuf, &kp, &count);

    /* Register the buffers to be used for monitoring for current session */
    rc = DosMonReg(_EVT_hKbdMon, (PBYTE)&monInbuf, (PBYTE)&monOutbuf, MONITOR_END, -1);
    if (rc) {
#ifdef DEBUG_MSGS
        printf("Error in DosMonReg, rc = %ld\n", rc);
#endif
        DosPostEventSem(hevStart);  /* unblock the main thread */
        return;
        }

    rc = DosMonRead((PBYTE)&monInbuf, IO_NOWAIT, (PBYTE)&kp, (PUSHORT)&count);
    if (rc && (rc != ERROR_MON_BUFFER_EMPTY)) {
#ifdef DEBUG_MSGS
        printf("Error in first DosMonRead, rc = %ld\n", rc);
#endif
        DosPostEventSem(hevStart);
        return;
        }

    /* Unblock the main thread and tell it we're OK*/
    bMonRunning = TRUE;
    DosPostEventSem(hevStart);
    while (bMonRunning) {  /* Start an endless loop */
        /* Read data from keyboard driver */
        rc = DosMonRead((PBYTE)&monInbuf, IO_WAIT, (PBYTE)&kp, (PUSHORT)&count);
        if (rc) {
#ifdef DEBUG_MSGS
            if (bMonRunning)
                printf("Error in DosMonRead, rc = %ld\n", rc);
#endif
            bMonRunning = FALSE;
            return;
            }

        /* Pass FLUSH packets immediately */
        if (kp.MonFlagWord & 4) {
#ifdef DEBUG_MSGS
            printf("Flush packet!\n");
#endif
            DosMonWrite((PBYTE)&monOutbuf, (PBYTE)&kp, count);
            continue;
            }

        //TODO: to be removed
        /* Skip extended scancodes & some others */
        if (((kp.MonFlagWord >> 8) == 0xE0) || ((kp.KbdDDFlagWord & 0x0F) == 0x0F)) {
            DosMonWrite((PBYTE)&monOutbuf, (PBYTE)&kp, count);
            continue;
            }

//      printf("RawScan = %X, XlatedScan = %X, fbStatus = %X, KbdDDFlags = %X\n",
//          kp.MonFlagWord >> 8, kp.XlatedScan, kp.u.ShiftState, kp.KbdDDFlagWord);

        /* Protect access to buffer with mutex semaphore */
        rc = DosRequestMutexSem(hmtxKeyBuf, 1000);
        if (rc) {
#ifdef DEBUG_MSGS
            printf("Can't get access to mutex, rc = %ld\n", rc);
#endif
            bMonRunning = FALSE;
            return;
            }

        /* Store packet in circular buffer, drop it if it's full */
        kpNew = kpHead + 1;
        if (kpNew == KEYBUFSIZE)
            kpNew = 0;
        if (kpNew != kpTail) {
            memcpy(&keyMonPkts[kpHead], &kp, sizeof(KEYPACKET));
            // TODO: fix this!
            /* Convert break to make code */
            keyMonPkts[kpHead].MonFlagWord &= 0x7FFF;
            kpHead = kpNew;
            }
        DosReleaseMutexSem(hmtxKeyBuf);

        /* Finally write the packet */
        rc = DosMonWrite((PBYTE)&monOutbuf, (PBYTE)&kp, count);
        if (rc) {
#ifdef DEBUG_MSGS
            if (bMonRunning)
                printf("Error in DosMonWrite, rc = %ld\n", rc);
#endif
            bMonRunning = FALSE;
            return;
            }
        }
    (void)params;
}

/****************************************************************************
REMARKS:
Safely abort the event module upon catching a fatal error.
****************************************************************************/
void _EVT_abort(
    int signal)
{
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
    ushort  stat;

    /* Initialise the event queue */
    PM_init();
    EVT.mouseMove = mouseMove;
    initEventQueue();
    oldMouseState = 0;
    oldKeyMessage = 0;
    memset(keyUpMsg,0,sizeof(keyUpMsg));

    /* Open the mouse driver, and set it up to report events in mickeys */
    MouOpen(NULL,&_EVT_hMouse);
    stat = 0x7F;
    MouSetEventMask(&stat,_EVT_hMouse);
    stat = (MOU_NODRAW | MOU_MICKEYS) << 8;
    MouSetDevStatus(&stat,_EVT_hMouse);

    /* Open the keyboard monitor  */
    if (DosMonOpen((PSZ)"KBD$", &_EVT_hKbdMon))
        PM_fatalError("Unable to open keyboard monitor!");

    /* Create event semaphore, the monitor will post it when it's initalized */
    if (DosCreateEventSem(NULL, &hevStart, 0, FALSE))
        PM_fatalError("Unable to create event semaphore!");

    /* Create mutex semaphore protecting the keypacket buffer */
    if (DosCreateMutexSem(NULL, &hmtxKeyBuf, 0, FALSE))
        PM_fatalError("Unable to create mutex semaphore!");

    /* Start keyboard monitor thread, use 32K stack */
    kbdMonTID = _beginthread(_kbdMonThread, NULL, 0x8000, NULL);

    /* Now block until the monitor thread is up and running */
    /* Give the thread one second */
    DosWaitEventSem(hevStart, 1000);
    if (!bMonRunning) {
        /* If thread didn't start, we'll fall back to plain old Kbd API */
        DosMonClose(_EVT_hKbdMon);
        }

    /* Catch program termination signals so we can clean up properly */
    signal(SIGABRT, _EVT_abort);
    signal(SIGINT, _EVT_abort);
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
    rangeX = xRes;
    rangeY = yRes;
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
    // Do nothing for OS/2
}

/****************************************************************************
REMARKS
Suspends all of our event handling operations. This is also used to
de-install the event handling code.
****************************************************************************/
void EVT_suspend(void)
{
    // Do nothing for OS/2
}

/****************************************************************************
REMARKS
Exits the event module for program terminatation.
****************************************************************************/
void EVT_exit(void)
{
    APIRET  rc;

    /* Restore signal handlers */
    signal(SIGABRT, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    /* Close the mouse driver */
    MouClose(_EVT_hMouse);

    /* Stop the keyboard monitor thread and close the monitor */
    if (bMonRunning) {
        bMonRunning = FALSE;
        KbdFlushBuffer(0);
        DosWaitThread(&kbdMonTID, DCWW_WAIT);
        rc = DosMonClose(_EVT_hKbdMon);
#ifdef DEBUG_MSGS
        if (rc) {
            printf("DosMonClose failed, rc = %ld\n", rc);
            }
#endif
        }
    DosCloseEventSem(hevStart);
    DosCloseMutexSem(hmtxKeyBuf);
}

