/****************************************************************************
*
*                       SciTech MGL Graphics Library
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
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
*
* Language:     ANSI C
* Environment:  IBM PC (MS DOS)
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
*
****************************************************************************/

#ifndef __glutint_h__
#define __glutint_h__

#include "mgraph.h"
#include <GL/glut.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbitmap.h"

#define GETTIMEOFDAY(_x)                \
{                                       \
  struct timeb _t;                      \
  ftime(&_t);                           \
  (_x)->tv_sec = _t.time;               \
  (_x)->tv_usec = _t.millitm * 1000;    \
}

#define ADD_TIME(dest, src1, src2) {                    \
  if(((dest).tv_usec =                                  \
    (src1).tv_usec + (src2).tv_usec) >= 1000000) {      \
    (dest).tv_usec -= 1000000;                          \
    (dest).tv_sec = (src1).tv_sec + (src2).tv_sec + 1;  \
  } else {                                              \
    (dest).tv_sec = (src1).tv_sec + (src2).tv_sec;      \
    if(((dest).tv_sec >= 1) && (((dest).tv_usec <0))) { \
      (dest).tv_sec --;(dest).tv_usec += 1000000;       \
    }                                                   \
  }                                                     \
}

#define TIMEDELTA(dest, src1, src2) {                           \
  if(((dest).tv_usec = (src1).tv_usec - (src2).tv_usec) < 0) {  \
    (dest).tv_usec += 1000000;                                  \
    (dest).tv_sec = (src1).tv_sec - (src2).tv_sec - 1;          \
  } else {                                                      \
     (dest).tv_sec = (src1).tv_sec - (src2).tv_sec;             \
  }                                                             \
}

#define IS_AFTER(t1, t2)            \
  (((t2).tv_sec > (t1).tv_sec) ||   \
  (((t2).tv_sec == (t1).tv_sec) &&  \
  ((t2).tv_usec > (t1).tv_usec)))

#define IS_AT_OR_AFTER(t1, t2)      \
  (((t2).tv_sec > (t1).tv_sec) ||   \
  (((t2).tv_sec == (t1).tv_sec) &&  \
  ((t2).tv_usec >= (t1).tv_usec)))

#define GLUT_WIND_IS_RGB(x)         (((x) & GLUT_INDEX) == 0)
#define GLUT_WIND_IS_INDEX(x)       (((x) & GLUT_INDEX) != 0)
#define GLUT_WIND_IS_SINGLE(x)      (((x) & GLUT_DOUBLE) == 0)
#define GLUT_WIND_IS_DOUBLE(x)      (((x) & GLUT_DOUBLE) != 0)
#define GLUT_WIND_HAS_ACCUM(x)      (((x) & GLUT_ACCUM) != 0)
#define GLUT_WIND_HAS_ALPHA(x)      (((x) & GLUT_ALPHA) != 0)
#define GLUT_WIND_HAS_DEPTH(x)      (((x) & GLUT_DEPTH) != 0)
#define GLUT_WIND_HAS_STENCIL(x)    (((x) & GLUT_STENCIL) != 0)
#define GLUT_WIND_IS_MULTISAMPLE(x) (((x) & GLUT_MULTISAMPLE) != 0)
#define GLUT_WIND_IS_STEREO(x)      (((x) & GLUT_STEREO) != 0)
#define GLUT_WIND_IS_LUMINANCE(x)   (((x) & GLUT_LUMINANCE) != 0)
#define GLUT_MAP_WORK               (1 << 0)
#define GLUT_EVENT_MASK_WORK        (1 << 1)
#define GLUT_REDISPLAY_WORK         (1 << 2)
#define GLUT_CONFIGURE_WORK         (1 << 3)
#define GLUT_COLORMAP_WORK          (1 << 4)
#define GLUT_DEVICE_MASK_WORK       (1 << 5)
#define GLUT_FINISH_WORK            (1 << 6)
#define GLUT_DEBUG_WORK             (1 << 7)
#define GLUT_DUMMY_WORK             (1 << 8)
#define GLUT_FULL_SCREEN_WORK       (1 << 9)
#define GLUT_OVERLAY_REDISPLAY_WORK (1 << 10)

/* Internal windows messages */
#ifdef  __WINDOWS__
#define WM_GLUT_REDISPLAY       WM_USER+1
#else
#define WM_GLUT_REDISPLAY       0
#endif

/* GLUT CALLBACK FUNCTION TYPES */
typedef void (*GLUTdisplayCB) (void);
typedef void (*GLUTreshapeCB) (int, int);
typedef void (*GLUTkeyboardCB) (unsigned char, int, int);
typedef void (*GLUTmouseCB) (int, int, int, int);
typedef void (*GLUTmotionCB) (int, int);
typedef void (*GLUTpassiveCB) (int, int);
typedef void (*GLUTentryCB) (int);
typedef void (*GLUTvisibilityCB) (int);
typedef void (*GLUTidleCB) (void);
typedef void (*GLUTtimerCB) (int);
typedef void (*GLUTmenuStateCB) (int);  /* DEPRICATED. */
typedef void (*GLUTmenuStatusCB) (int, int, int);
typedef void (*GLUTselectCB) (int);
typedef void (*GLUTspecialCB) (int, int, int);
typedef void (*GLUTspaceMotionCB) (int, int, int);
typedef void (*GLUTspaceRotateCB) (int, int, int);
typedef void (*GLUTspaceButtonCB) (int, int);
typedef void (*GLUTdialsCB) (int, int);
typedef void (*GLUTbuttonBoxCB) (int, int);
typedef void (*GLUTtabletMotionCB) (int, int);
typedef void (*GLUTtabletButtonCB) (int, int, int, int);

struct GLUTwindow {
    MGLDC   *dc;            /* MGL device context                           */
    MGLDC   *windc;         /* MGL device context for window                */
    MGLDC   *backdc;        /* MGL device context for window back buffer    */
    int     waitVRT;        /* True to wait for vertical retrace            */
    ibool   fpsCounter;     /* True to display fps counter                  */
    ibool   fakeSingle;     /* faking single buffer with double             */

    /* Callbacks */
    GLUTdisplayCB       display;        /* redraw callback                  */
    GLUTreshapeCB       reshape;        /* resize callback (width,height)   */
    GLUTmouseCB         mouse;          /* mouse callback (button,state,x,y)*/
    GLUTmotionCB        motion;         /* motion callback (x,y)            */
    GLUTpassiveCB       passive;        /* passive motion callback (x,y)    */
    GLUTentryCB         entry;          /* window entry/exit callback (state)*/
    GLUTkeyboardCB      keyboard;       /* keyboard callback (ASCII,x,y)    */
    GLUTvisibilityCB    visibility;     /* visibility callback              */
    GLUTspecialCB       special;        /* special key callback             */
    GLUTbuttonBoxCB     buttonBox;      /* button box callback              */
    GLUTdialsCB         dials;          /* dials callback                   */
    GLUTspaceMotionCB   spaceMotion;    /* Spaceball motion callback        */
    GLUTspaceRotateCB   spaceRotate;    /* Spaceball rotate callback        */
    GLUTspaceButtonCB   spaceButton;    /* Spaceball button callback        */
    GLUTtabletMotionCB  tabletMotion;   /* tablet motion callback           */
    GLUTtabletButtonCB  tabletButton;   /* tablet button callback           */
    };

struct GLUTtimer {
    GLUTtimer           *next;          /* list of timers                   */
    GLUTtimeval         timeout;        /* time to be called                */
    GLUTtimerCB         func;           /* timer callback (value)           */
    int                 value;          /* callback return value            */
    };

/* private variables from g_init.c */
extern uint __glutDisplayMode;
extern char *__glutProgramName;
extern int __glutInitHeight;
extern int __glutInitWidth;
extern int __glutInitX;
extern int __glutInitY;
extern int __glutMGLDriver,__glutMGLMode;
extern GLUTwindow __glutCurrentWindow;
#ifdef  __WINDOWS__
extern "C" MGL_HWND __glutMainDialog;
extern "C" MGL_HWND __glutMainWindow;
extern "C" ibool    __glutDone;
extern "C" ibool        __glutFullscreen;
extern "C" ibool        __glutStaticPalette;
#endif

/* private variables from g_event.c */
extern uint __glutModifierMask;
#ifdef  __WINDOWS__
extern "C" long GLAPIENTRY __glutWindowProc(MGL_HWND hWnd, uint uMsg, uint wParam, long lParam);
#endif

/* private routines from g_init.c */
void __glutInitTime(GLUTtimeval *beginning);
void __glutExitFullscreen(void);

#endif /* __glutint_h__ */

