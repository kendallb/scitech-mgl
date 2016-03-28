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
* Description:  Main header file for the OpenGL Utility Toolkit library.
*               This minimal implementation of GLUT has been ported to run
*               on top of SciTech MGL's OpenGL implementation in fullscreen
*               and windowed modes. We dont support menus, overlays or
*               input devices other than the keyboard and mouse. This could
*               be extended as time goes by. The primary purpose of this
*               version of GLUT is to provide a simple framework so we
*               can compile and run the OpenGL sample programs, as well
*               as providing a tool to get MGL programmers up and running
*               quickly with OpenGL support.
*
*               Note that we also only support a single window for output
*               as there is only one output display when running fullscreen.
*
****************************************************************************/

#ifdef __USE_GLUTW32__
#include <GL/glutw32.h>
#else
#ifndef __glut_h__
#define __glut_h__

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GLUT API revision history:
 *
 * GLUT_API_VERSION is updated to reflect incompatible GLUT
 * API changes (interface changes, semantic changes, deletions,
 * or additions).
 *
 * GLUT_API_VERSION=1  First public release of GLUT.  11/29/94
 *
 * GLUT_API_VERSION=2  Added support for OpenGL/GLX multisampling,
 * extension.  Supports new input devices like tablet, dial and button
 * box, and Spaceball.  Easy to query OpenGL extensions.
 *
 * GLUT_API_VERSION=3  glutMenuStatus added.
 *
 */
#ifndef GLUT_API_VERSION  /* allow this to be overriden */
#define GLUT_API_VERSION                3
#endif

/* display mode bit masks */
#define GLUT_RGB                        0
#define GLUT_RGBA                       GLUT_RGB
#define GLUT_INDEX                      1
#define GLUT_SINGLE                     0
#define GLUT_DOUBLE                     2
#define GLUT_ACCUM                      4
#define GLUT_ALPHA                      8
#define GLUT_DEPTH                      16
#define GLUT_STENCIL                    32
#define GLUT_MULTISAMPLE                128
#define GLUT_STEREO                     256
#define GLUT_LUMINANCE                  512

/* mouse buttons */
#define GLUT_LEFT_BUTTON                0
#define GLUT_MIDDLE_BUTTON              1
#define GLUT_RIGHT_BUTTON               2

/* mouse button callback state */
#define GLUT_DOWN                       0
#define GLUT_UP                         1

/* function keys */
#define GLUT_KEY_F1                     1
#define GLUT_KEY_F2                     2
#define GLUT_KEY_F3                     3
#define GLUT_KEY_F4                     4
#define GLUT_KEY_F5                     5
#define GLUT_KEY_F6                     6
#define GLUT_KEY_F7                     7
#define GLUT_KEY_F8                     8
#define GLUT_KEY_F9                     9
#define GLUT_KEY_F10                    10
#define GLUT_KEY_F11                    11
#define GLUT_KEY_F12                    12
/* directional keys */
#define GLUT_KEY_LEFT                   100
#define GLUT_KEY_UP                     101
#define GLUT_KEY_RIGHT                  102
#define GLUT_KEY_DOWN                   103
#define GLUT_KEY_PAGE_UP                104
#define GLUT_KEY_PAGE_DOWN              105
#define GLUT_KEY_HOME                   106
#define GLUT_KEY_END                    107
#define GLUT_KEY_INSERT                 108

/* entry/exit callback state */
#define GLUT_LEFT                       0
#define GLUT_ENTERED                    1

/* menu usage callback state */
#define GLUT_MENU_NOT_IN_USE            0
#define GLUT_MENU_IN_USE                1

/* visibility callback state */
#define GLUT_NOT_VISIBLE                0
#define GLUT_VISIBLE                    1

/* color index component selection values */
#define GLUT_RED                        0
#define GLUT_GREEN                      1
#define GLUT_BLUE                       2

/* layers for use */
#define GLUT_NORMAL                     0
#define GLUT_OVERLAY                    1

/* stroke font opaque addresses (use constants instead in source code) */
extern void *glutStrokeRoman(void);
extern void *glutStrokeMonoRoman(void);

/* stroke font constants (use these in GLUT program) */
#define GLUT_STROKE_ROMAN               glutStrokeRoman()
#define GLUT_STROKE_MONO_ROMAN          glutStrokeMonoRoman()

/* bitmap font opaque addresses (use constants instead in source code) */
extern void *glutBitmap9By15(void);
extern void *glutBitmap8By13(void);
extern void *glutBitmapTimesRoman10(void);
extern void *glutBitmapTimesRoman24(void);
extern void *glutBitmapHelvetica10(void);
extern void *glutBitmapHelvetica12(void);
extern void *glutBitmapHelvetica18(void);

/* bitmap font constants (use these in GLUT program) */
#define GLUT_BITMAP_9_BY_15             glutBitmap9By15()
#define GLUT_BITMAP_8_BY_13             glutBitmap8By13()
#define GLUT_BITMAP_TIMES_ROMAN_10      glutBitmapTimesRoman10()
#define GLUT_BITMAP_TIMES_ROMAN_24      glutBitmapTimesRoman24()
#define GLUT_BITMAP_HELVETICA_10        glutBitmapHelvetica10()
#define GLUT_BITMAP_HELVETICA_12        glutBitmapHelvetica12()
#define GLUT_BITMAP_HELVETICA_18        glutBitmapHelvetica18()

/* glutGet parameters */
#define GLUT_WINDOW_X                   100
#define GLUT_WINDOW_Y                   101
#define GLUT_WINDOW_WIDTH               102
#define GLUT_WINDOW_HEIGHT              103
#define GLUT_WINDOW_BUFFER_SIZE         104
#define GLUT_WINDOW_STENCIL_SIZE        105
#define GLUT_WINDOW_DEPTH_SIZE          106
#define GLUT_WINDOW_RED_SIZE            107
#define GLUT_WINDOW_GREEN_SIZE          108
#define GLUT_WINDOW_BLUE_SIZE           109
#define GLUT_WINDOW_ALPHA_SIZE          110
#define GLUT_WINDOW_ACCUM_RED_SIZE      111
#define GLUT_WINDOW_ACCUM_GREEN_SIZE    112
#define GLUT_WINDOW_ACCUM_BLUE_SIZE     113
#define GLUT_WINDOW_ACCUM_ALPHA_SIZE    114
#define GLUT_WINDOW_DOUBLEBUFFER        115
#define GLUT_WINDOW_RGBA                116
#define GLUT_WINDOW_PARENT              117
#define GLUT_WINDOW_NUM_CHILDREN        118
#define GLUT_WINDOW_COLORMAP_SIZE       119
#define GLUT_WINDOW_NUM_SAMPLES         120
#define GLUT_WINDOW_STEREO              121
#define GLUT_WINDOW_CURSOR              122
#define GLUT_SCREEN_WIDTH               200
#define GLUT_SCREEN_HEIGHT              201
#define GLUT_SCREEN_WIDTH_MM            202
#define GLUT_SCREEN_HEIGHT_MM           203
#define GLUT_MENU_NUM_ITEMS             300
#define GLUT_DISPLAY_MODE_POSSIBLE      400
#define GLUT_INIT_WINDOW_X              500
#define GLUT_INIT_WINDOW_Y              501
#define GLUT_INIT_WINDOW_WIDTH          502
#define GLUT_INIT_WINDOW_HEIGHT         503
#define GLUT_INIT_DISPLAY_MODE          504
#define GLUT_ELAPSED_TIME               700

/* glutDeviceGet parameters */
#define GLUT_HAS_KEYBOARD               600
#define GLUT_HAS_MOUSE                  601
#define GLUT_HAS_SPACEBALL              602
#define GLUT_HAS_DIAL_AND_BUTTON_BOX    603
#define GLUT_HAS_TABLET                 604
#define GLUT_NUM_MOUSE_BUTTONS          605
#define GLUT_NUM_SPACEBALL_BUTTONS      606
#define GLUT_NUM_BUTTON_BOX_BUTTONS     607
#define GLUT_NUM_DIALS                  608
#define GLUT_NUM_TABLET_BUTTONS         609

/* glutLayerGet parameters */
#define GLUT_OVERLAY_POSSIBLE           800
#define GLUT_LAYER_IN_USE               801
#define GLUT_HAS_OVERLAY                802
#define GLUT_TRANSPARENT_INDEX          803
#define GLUT_NORMAL_DAMAGED             804
#define GLUT_OVERLAY_DAMAGED            805

/* glutUseLayer parameters */
#define GLUT_NORMAL                     0
#define GLUT_OVERLAY                    1

/* glutGetModifiers return mask */
#define GLUT_ACTIVE_SHIFT               1
#define GLUT_ACTIVE_CTRL                2
#define GLUT_ACTIVE_ALT                 4

/* glutSetCursor parameters */
/* Basic arrows */
#define GLUT_CURSOR_RIGHT_ARROW         0
#define GLUT_CURSOR_LEFT_ARROW          1
/* Symbolic cursor shapees */
#define GLUT_CURSOR_INFO                2
#define GLUT_CURSOR_DESTROY             3
#define GLUT_CURSOR_HELP                4
#define GLUT_CURSOR_CYCLE               5
#define GLUT_CURSOR_SPRAY               6
#define GLUT_CURSOR_WAIT                7
#define GLUT_CURSOR_TEXT                8
#define GLUT_CURSOR_CROSSHAIR           9
/* Directional cursors */
#define GLUT_CURSOR_UP_DOWN             10
#define GLUT_CURSOR_LEFT_RIGHT          11
/* Sizing cursors */
#define GLUT_CURSOR_TOP_SIDE            12
#define GLUT_CURSOR_BOTTOM_SIDE         13
#define GLUT_CURSOR_LEFT_SIDE           14
#define GLUT_CURSOR_RIGHT_SIDE          15
#define GLUT_CURSOR_TOP_LEFT_CORNER     16
#define GLUT_CURSOR_TOP_RIGHT_CORNER    17
#define GLUT_CURSOR_BOTTOM_RIGHT_CORNER 18
#define GLUT_CURSOR_BOTTOM_LEFT_CORNER  19
/* Inherit from parent window */
#define GLUT_CURSOR_INHERIT             100
/* Blank cursor */
#define GLUT_CURSOR_NONE                101
/* Fullscreen crosshair (if available) */
#define GLUT_CURSOR_FULL_CROSSHAIR      102

/* GLUT MGL API extensions */
#define GLUT_MGL_WINDOWED               -1
#define GLUT_MGL_WINDOWED_320x240       -2
#define GLUT_MGL_WINDOWED_512x384       -3
#define GLUT_MGL_WINDOWED_640x480       -4
#define GLUT_MGL_WINDOWED_800x600       -5

typedef struct {
    long    tv_sec;
    long    tv_usec;
    } GLUTtimeval;

/* GLUT initialization sub-API */
extern void glutInit(int *argcp, char **argv);
extern void glutInitDisplayMode(unsigned int mode);
extern void glutInitWindowPosition(int x, int y);
extern void glutInitWindowSize(int width, int height);
extern void glutMainLoop(void);

/* GLUT MGL extensions sub-API */
extern void glutInitMGLOpenGLType(int type);
extern void glutInitMGLSysPalNoStatic(ibool flag);
extern void glutInitMGLDrivers(ibool useVBE,ibool useSNAP);
extern void glutInitMGLPath(const char *MGLPath);
extern void glutInitMGLMode(int xRes,int yRes,int bitsPerPixel);
extern void glutExit(void);

/* GLUT window sub-api */
extern int glutCreateWindow(char *title);
extern int glutCreateSubWindow(int win, int x, int y, int width, int height);
extern void glutDestroyWindow(int win);
extern void glutPostRedisplay(void);
extern void glutSwapBuffers(void);
extern int glutGetWindow(void);
extern void glutSetWindow(int win);
extern void glutSetWindowTitle(char *title);
extern void glutSetIconTitle(char *title);
extern void glutPositionWindow(int x, int y);
extern void glutReshapeWindow(int width, int height);
extern void glutPopWindow(void);
extern void glutPushWindow(void);
extern void glutIconifyWindow(void);
extern void glutShowWindow(void);
extern void glutHideWindow(void);
extern void glutFullScreen(void);
extern void glutSetCursor(int cursor);

/* GLUT overlay sub-API */
extern void glutEstablishOverlay(void);
extern void glutRemoveOverlay(void);
extern void glutUseLayer(GLenum layer);
extern void glutPostOverlayRedisplay(void);
extern void glutShowOverlay(void);
extern void glutHideOverlay(void);

typedef void (*_glutMenuFunc)(int);

/* GLUT menu sub-API */
extern int glutCreateMenu(_glutMenuFunc menu);
extern void glutDestroyMenu(int menu);
extern int glutGetMenu(void);
extern void glutSetMenu(int menu);
extern void glutAddMenuEntry(char *label, int value);
extern void glutAddSubMenu(char *label, int submenu);
extern void glutChangeToMenuEntry(int item, char *label, int value);
extern void glutChangeToSubMenu(int item, char *label, int submenu);
extern void glutRemoveMenuItem(int item);
extern void glutAttachMenu(int button);
extern void glutDetachMenu(int button);

typedef void (*_glutDisplayFunc)(void);
typedef void (*_glutReshapeFunc)(int width, int height);
typedef void (*_glutKeyboardFunc)(unsigned char key, int x, int y);
typedef void (*_glutMouseFunc)(int button, int state, int x, int y);
typedef void (*_glutMotionFunc)(int x, int y);
typedef void (*_glutPassiveMotionFunc)(int x, int y);
typedef void (*_glutEntryFunc)(int state);
typedef void (*_glutVisibilityFunc)(int state);
typedef void (*_glutIdleFunc)(void);
typedef void (*_glutTimerFunc)(int value);
typedef void (*_glutMenuStateFunc)(int state);
typedef void (*_glutSpecialFunc)(int key, int x, int y);
typedef void (*_glutSpaceballMotionFunc)(int x, int y, int z);
typedef void (*_glutSpaceballRotateFunc)(int x, int y, int z);
typedef void (*_glutSpaceballButtonFunc)(int button, int state);
typedef void (*_glutButtonBoxFunc)(int button, int state);
typedef void (*_glutDialsFunc)(int dial, int value);
typedef void (*_glutTabletMotionFunc)(int x, int y);
typedef void (*_glutTabletButtonFunc)(int button, int state, int x, int y);
typedef void (*_glutMenuStatusFunc)(int status, int x, int y);
typedef void (*_glutOverlayDisplayFunc)(void);

/* GLUT callback sub-api */
extern void glutDisplayFunc(_glutDisplayFunc f);
extern void glutReshapeFunc(_glutReshapeFunc f);
extern void glutKeyboardFunc(_glutKeyboardFunc f);
extern void glutMouseFunc(_glutMouseFunc f);
extern void glutMotionFunc(_glutMotionFunc f);
extern void glutPassiveMotionFunc(_glutPassiveMotionFunc f);
extern void glutEntryFunc(_glutEntryFunc f);
extern void glutVisibilityFunc(_glutVisibilityFunc f);
extern void glutIdleFunc(_glutIdleFunc f);
extern void glutTimerFunc(unsigned int millis,_glutTimerFunc f,int value);
extern void glutMenuStateFunc(_glutMenuStateFunc f);
extern void glutSpecialFunc(_glutSpecialFunc f);
extern void glutSpaceballMotionFunc(_glutSpaceballMotionFunc f);
extern void glutSpaceballRotateFunc(_glutSpaceballRotateFunc f);
extern void glutSpaceballButtonFunc(_glutSpaceballButtonFunc f);
extern void glutButtonBoxFunc(_glutButtonBoxFunc f);
extern void glutDialsFunc(_glutDialsFunc f);
extern void glutTabletMotionFunc(_glutTabletMotionFunc f);
extern void glutTabletButtonFunc(_glutTabletButtonFunc f);
extern void glutMenuStatusFunc(_glutMenuStatusFunc f);
extern void glutOverlayDisplayFunc(_glutOverlayDisplayFunc f);

/* GLUT color index sub-api */
extern void glutSetColor(int, GLfloat red, GLfloat green, GLfloat blue);
extern GLfloat glutGetColor(int ndx, int component);
extern void glutCopyColormap(int win);

/* GLUT state retrieval sub-api */
extern int glutGet(GLenum type);
extern int glutDeviceGet(GLenum type);
/* GLUT extension support sub-API */
extern int glutExtensionSupported(char *name);
extern int glutGetModifiers(void);
extern int glutLayerGet(GLenum type);

/* GLUT font sub-API */
extern void glutBitmapCharacter(void *font, int character);
extern int glutBitmapWidth(void *font, int character);
extern void glutStrokeCharacter(void *font, int character);
extern int glutStrokeWidth(void *font, int character);

/* GLUT pre-built models sub-API */
extern void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
extern void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
extern void glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
extern void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
extern void glutWireCube(GLdouble size);
extern void glutSolidCube(GLdouble size);
extern void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
extern void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
extern void glutWireDodecahedron(void);
extern void glutSolidDodecahedron(void);
extern void glutWireTeapot(GLdouble size);
extern void glutSolidTeapot(GLdouble size);
extern void glutWireOctahedron(void);
extern void glutSolidOctahedron(void);
extern void glutWireTetrahedron(void);
extern void glutSolidTetrahedron(void);
extern void glutWireIcosahedron(void);
extern void glutSolidIcosahedron(void);

extern void __glutWarning       (char *format,...);
extern void __glutFatalError(char *format,...);
extern void __glutFatalUsage(char *format,...);

#ifdef __cplusplus
}
#endif

#endif  /* __glut_h__ */
#endif  /* __USE_GLUTW32__ */

