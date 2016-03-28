/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*    The contents of this file are subject to the SciTech MGL Public
*    License Version 1.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of
*    the License at http://www.scitechsoft.com/mgl-license.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*    The Original Code is Copyright (C) 1991-2004 SciTech Software, Inc.
*
*    The Initial Developer of the Original Code is SciTech Software, Inc.
*    All Rights Reserved.
*
*  ========================================================================
*
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
****************************************************************************/

#include "glutint.h"
#ifdef  __WINDOWS__
#undef  WINGDIAPI
#undef  APIENTRY
#undef  CALLBACK
//#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*--------------------------- Global Variables ----------------------------*/

uint                __glutModifierMask = ~0;
GLUTidleCB          __glutIdleFunc = NULL;
GLUTtimer           *__glutTimerList = NULL;
static GLUTtimer    *freeTimerList = NULL;
static ibool            exitMainLoop;

/*------------------------- Implementation --------------------------------*/

void glutIdleFunc(GLUTidleCB idleFunc)
{
    __glutIdleFunc = idleFunc;
}

void glutTimerFunc(unsigned int, GLUTtimerCB timerFunc, int value)
{
    GLUTtimer   *timer;

    if (!timerFunc)
        return;
    if (freeTimerList) {
        /* Grab a timer from the free list */
        timer = freeTimerList;
        freeTimerList = freeTimerList->next;
        }
    else {
        /* Allocate space for a new timer */
        timer = new GLUTtimer;
        if (!timer)
            __glutFatalError("out of memory.");
        }
    timer->value = value;
    timer->next = __glutTimerList;
    timer->func = timerFunc;
    __glutTimerList = timer;
}

static void handleTimeouts(void)
{
    GLUTtimeval now;
    GLUTtimer   *timer;

    if (__glutTimerList) {
        GETTIMEOFDAY(&now);
        while (IS_AT_OR_AFTER(__glutTimerList->timeout, now)) {
            timer = __glutTimerList;
            timer->func(timer->value);
            __glutTimerList = timer->next;
            timer->next = freeTimerList;
            freeTimerList = timer;
            if (!__glutTimerList)
                break;
            }
        }
}

void glutPostRedisplay(void)
{
#ifdef  __WINDOWS__
    PostMessage((HWND)__glutMainWindow,WM_GLUT_REDISPLAY,0,0);
#else
    EVT_post(0,EVT_USEREVT,WM_GLUT_REDISPLAY,0);
#endif
}

static void idleWait(void)
{
    if (__glutTimerList)
        handleTimeouts();
    if (__glutIdleFunc)
        __glutIdleFunc();
}

#ifdef __WINDOWS__

static void getModifiers(DWORD lParam)
{
    __glutModifierMask = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        __glutModifierMask |= EVT_CTRLSTATE;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        __glutModifierMask |= EVT_SHIFTKEY;
    if (HIWORD(lParam) & KF_ALTDOWN)
        __glutModifierMask |= EVT_ALTSTATE;
}

static void AppActivate(int fActive)
{
    HDC hdc = GetDC(NULL);

    if (fActive && (GetSystemPaletteUse(hdc) == SYSPAL_STATIC))
        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
    else if (!fActive)
        SetSystemPaletteUse(hdc, SYSPAL_STATIC);
    ReleaseDC(NULL,hdc);
}

LONG WINAPI __glutWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int     key;
    RECT    rect;
    POINT   point;

    switch (uMsg) {
        case WM_GLUT_REDISPLAY:
            if (__glutCurrentWindow.display)
                __glutCurrentWindow.display();
            break;
        case WM_ACTIVATEAPP:
            if (!__glutFullscreen && __glutCurrentWindow.dc) {
                int fAppActive = wParam;
                if (__glutStaticPalette)
                    AppActivate(fAppActive);
                }
            break;
        case WM_SYSCOMMAND:
            if (!__glutFullscreen) {
                switch(wParam) {
                    case SC_MAXIMIZE:
                    case SC_RESTORE:
                        if (__glutCurrentWindow.visibility)
                            __glutCurrentWindow.visibility(GL_TRUE);
                        break;
                    case SC_MINIMIZE:
                        if (__glutCurrentWindow.visibility)
                            __glutCurrentWindow.visibility(GL_FALSE);
                        break;
                    }
                }
            break;
        case WM_SHOWWINDOW:
            if (!__glutFullscreen) {
                if (__glutCurrentWindow.visibility)
                    __glutCurrentWindow.visibility((BOOL)wParam);
                }
            break;
        case WM_PAINT:
            if (!__glutFullscreen && __glutCurrentWindow.dc) {
                if (__glutCurrentWindow.display) {
                    __glutCurrentWindow.display();
                    ValidateRect(hwnd,NULL);
                    }
                return 0;
                }
            break;
        case WM_SIZE:
            if (!__glutFullscreen && __glutCurrentWindow.dc) {
                MGL_glResizeBuffers(__glutCurrentWindow.dc);
                if (__glutCurrentWindow.reshape) {
                    __glutCurrentWindow.reshape(__glutCurrentWindow.dc->mi.xRes+1,
                                                __glutCurrentWindow.dc->mi.yRes+1);
                    }
                InvalidateRect(hwnd,NULL,false);
                return 0;
                }
            break;
        case WM_KEYDOWN:
            getModifiers(lParam);
            switch (wParam) {
                case VK_F1:     key = GLUT_KEY_F1;          break;
                case VK_F2:     key = GLUT_KEY_F2;          break;
                case VK_F3:     key = GLUT_KEY_F3;          break;
                case VK_F4:     key = GLUT_KEY_F4;          break;
                case VK_F5:     key = GLUT_KEY_F5;          break;
                case VK_F6:     key = GLUT_KEY_F6;          break;
                case VK_F7:     key = GLUT_KEY_F7;          break;
                case VK_F8:     key = GLUT_KEY_F8;          break;
                case VK_F9:     key = GLUT_KEY_F9;          break;
                case VK_F10:    key = GLUT_KEY_F10;         break;
                case VK_F11:    key = GLUT_KEY_F11;         break;
                case VK_F12:    key = GLUT_KEY_F12;         break;
                case VK_LEFT:   key = GLUT_KEY_LEFT;        break;
                case VK_UP:     key = GLUT_KEY_UP;          break;
                case VK_RIGHT:  key = GLUT_KEY_RIGHT;       break;
                case VK_DOWN:   key = GLUT_KEY_DOWN;        break;
                case VK_PRIOR:  key = GLUT_KEY_PAGE_UP;     break;
                case VK_NEXT:   key = GLUT_KEY_PAGE_DOWN;   break;
                case VK_HOME:   key = GLUT_KEY_HOME;        break;
                case VK_END:    key = GLUT_KEY_END;         break;
                case VK_INSERT: key = GLUT_KEY_INSERT;      break;
                default:        key = FALSE;                break;
                }
            if (key && __glutCurrentWindow.special) {
                GetWindowRect(hwnd, &rect);
                GetCursorPos(&point);
                __glutCurrentWindow.special(key, point.x - rect.left, point.y - rect.top);
                }
            __glutModifierMask = ~0;
            return 1;
        case WM_CHAR:
            getModifiers(lParam);
            if (wParam == 'F')
                __glutCurrentWindow.fpsCounter = !__glutCurrentWindow.fpsCounter;
            else if (wParam == 'V')
                __glutCurrentWindow.waitVRT = !__glutCurrentWindow.waitVRT;
            else if (wParam == '\x1B' && __glutFullscreen)
                glutExit();
            else if (__glutCurrentWindow.keyboard) {
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                __glutCurrentWindow.keyboard((unsigned char)wParam, (int)point.x, (int)point.y);
                }
            __glutModifierMask = ~0;
            return 1;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            // TODO: Need to handle mouse coordinates (need to scale to
            //       the coordinates of the display device context for fullscreen
            break;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            // TODO: Need to handle mouse coordinates (need to scale to
            //       the coordinates of the display device context for fullscreen
            break;
        case WM_MOUSEMOVE:
            // TODO: Need to handle mouse movements (need to scale to the
            //       coordinates of the display device context for fullscreen
#if 0
            if(wParam & MK_LBUTTON || wParam & MK_MBUTTON || wParam & MK_RBUTTON) {
                if(__glutCurrentWindow.motion) {
                    __glutSetWindow(info);
                    (*__glutCurrentWindow.motion)(x, y);
                    }
                }
            else if(__glutCurrentWindow.passive) {
                __glutSetWindow(info);
                (*__glutCurrentWindow.passive)(x, y);
                }
            return 0;
#endif
            break;
        case WM_SETFOCUS:
            if (!__glutFullscreen && __glutCurrentWindow.dc && __glutCurrentWindow.entry)
                __glutCurrentWindow.entry(GLUT_ENTERED);
            return 0;
        case WM_KILLFOCUS:
            if(!__glutFullscreen && __glutCurrentWindow.dc && __glutCurrentWindow.entry)
                __glutCurrentWindow.entry(GLUT_LEFT);
            return 0;
        case WM_CLOSE:
            if (!__glutFullscreen)
                DestroyWindow(hwnd);
            return 1;
        case WM_DESTROY:
            if (!__glutFullscreen) {
                PostQuitMessage(0);
                __glutMainWindow = NULL;
                }
            return 1;
        }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void glutMainLoop(void)
{
    MSG msg;

    /* Force some initial states */
    if (__glutCurrentWindow.entry)
        __glutCurrentWindow.entry(GLUT_ENTERED);
    if(__glutCurrentWindow.visibility)
        __glutCurrentWindow.visibility(true);
    __glutCurrentWindow.reshape(__glutCurrentWindow.dc->mi.xRes+1,
                                __glutCurrentWindow.dc->mi.yRes+1);
    if (!MGL_isWindowedDC(__glutCurrentWindow.dc))
        __glutCurrentWindow.display();

    /* Process events until we are done */
    exitMainLoop = false;
    while (!exitMainLoop) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        else
            idleWait();
        }
    __glutExitFullscreen();
}

#else

/* DOS based main event loop. We could use the same code here for fullscreen,
 * but it makes it cleaner to use a real windows procedure for Windows code
 * as we can then support both windowed and fullscreen modes with the same
 * main event loop.
 */

const ushort
    kbF1    = 0x3B, kbF2    = 0x3C, kbF3    = 0x3D,
    kbF4    = 0x3E, kbF5    = 0x3F, kbF6    = 0x40,
    kbF7    = 0x41, kbF8    = 0x42, kbF9    = 0x43,
    kbF10   = 0x44, kbF11   = 0x85, kbF12   = 0x86,
    kbLeft  = 0x4B, kbRight = 0x4D, kbUp    = 0x48,
    kbDown  = 0x50, kbIns   = 0x52, kbDel   = 0x53,
    kbHome  = 0x47, kbEnd   = 0x4F, kbPgUp  = 0x49,
    kbPgDn  = 0x51, kbNoKey = 0x00;

void glutMainLoop(void)
{
    event_t evt;
    int     ch,button;

    /* Force some initial states */
    if (__glutCurrentWindow.entry)
        __glutCurrentWindow.entry(GLUT_ENTERED);
    if(__glutCurrentWindow.visibility)
        __glutCurrentWindow.visibility(true);
    __glutCurrentWindow.reshape(__glutCurrentWindow.dc->mi.xRes+1,
                                __glutCurrentWindow.dc->mi.yRes+1);
    if (__glutCurrentWindow.display)
        __glutCurrentWindow.display();

    /* Process events until we are done */
    exitMainLoop = false;
    while (!exitMainLoop) {
        if (EVT_getNext(&evt,EVT_EVERYEVT)) {
            switch (evt.what) {
                case EVT_KEYDOWN:
                case EVT_KEYREPEAT:
                    __glutModifierMask = evt.modifiers;
                    if ((ch = EVT_asciiCode(evt.message)) != 0) {
                        /* Handle regular ASCII codes */
                        if (ch == 'F')
                            __glutCurrentWindow.fpsCounter = !__glutCurrentWindow.fpsCounter;
                        else if (ch == 'V')
                            __glutCurrentWindow.waitVRT = !__glutCurrentWindow.waitVRT;
                        else if (ch == '\x1B')
                            glutExit();
                        else if (__glutCurrentWindow.keyboard)
                            __glutCurrentWindow.keyboard((char)ch, evt.where_x, evt.where_y);
                        }
                    else {
                        switch (EVT_scanCode(evt.message)) {
                            case kbF1:      ch = GLUT_KEY_F1;           break;
                            case kbF2:      ch = GLUT_KEY_F2;           break;
                            case kbF3:      ch = GLUT_KEY_F3;           break;
                            case kbF4:      ch = GLUT_KEY_F4;           break;
                            case kbF5:      ch = GLUT_KEY_F5;           break;
                            case kbF6:      ch = GLUT_KEY_F6;           break;
                            case kbF7:      ch = GLUT_KEY_F7;           break;
                            case kbF8:      ch = GLUT_KEY_F8;           break;
                            case kbF9:      ch = GLUT_KEY_F9;           break;
                            case kbF10:     ch = GLUT_KEY_F10;          break;
                            case kbF11:     ch = GLUT_KEY_F11;          break;
                            case kbF12:     ch = GLUT_KEY_F12;          break;
                            case kbLeft:    ch = GLUT_KEY_LEFT;         break;
                            case kbUp:      ch = GLUT_KEY_UP;           break;
                            case kbRight:   ch = GLUT_KEY_RIGHT;        break;
                            case kbDown:    ch = GLUT_KEY_DOWN;         break;
                            case kbPgUp:    ch = GLUT_KEY_PAGE_UP;      break;
                            case kbPgDn:    ch = GLUT_KEY_PAGE_DOWN;    break;
                            case kbHome:    ch = GLUT_KEY_HOME;         break;
                            case kbEnd:     ch = GLUT_KEY_END;          break;
                            case kbIns:     ch = GLUT_KEY_INSERT;       break;
                            default:        ch = FALSE;                 break;
                            }
                        if (ch && __glutCurrentWindow.special)
                            __glutCurrentWindow.special(ch, evt.where_x, evt.where_y);
                        }
                    __glutModifierMask = ~0;
                    break;
                case EVT_MOUSEDOWN:
                case EVT_MOUSEUP:
                    __glutModifierMask = evt.modifiers;
                    if (evt.message & EVT_LEFTBMASK)
                        button = GLUT_LEFT_BUTTON;
                    else
                        button = GLUT_RIGHT_BUTTON;
                    if (__glutCurrentWindow.mouse) {
                        __glutCurrentWindow.mouse(button,
                            evt.what == EVT_MOUSEDOWN ? GLUT_DOWN : GLUT_UP,
                            evt.where_x,evt.where_y);
                        }
                    __glutModifierMask = ~0;
                    break;
                case EVT_MOUSEMOVE:
                    if ((evt.message & EVT_ALLBMASK) != 0) {
                        if(__glutCurrentWindow.motion)
                            __glutCurrentWindow.motion(evt.where_x, evt.where_y);
                        }
                    else {
                        if(__glutCurrentWindow.passive)
                            __glutCurrentWindow.passive(evt.where_x, evt.where_y);
                        }
                    break;
                case EVT_USEREVT:
                    if (evt.message == WM_GLUT_REDISPLAY) {
                        if (__glutCurrentWindow.display)
                            __glutCurrentWindow.display();
                        }
                    break;
                }
            }
        else
            idleWait();
        }
    __glutExitFullscreen();
}

#endif

void glutExit(void)
{
    exitMainLoop = true;
}
