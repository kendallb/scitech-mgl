/****************************************************************************
*
*                          Console Output Library
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
* Description:  Header file for console manipulation module. This is a
*               small module for fast, compiler independant console
*               output routines. It has been modified to for use with
*               32 bit flat model compilers and the code is being made
*               freely available for use in the POVRay Ray Tracer.
*
*               Has also been completely re-written to only use direct
*               video output code, and to directly re-program the the VGA
*               graphics hardware rather than make use of the BIOS. Hence
*               this code is portable and will run on any system with
*               VGA compatible hardware.
*
****************************************************************************/

#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "scitech.h"

/*--------------------- Macros and type definitions -----------------------*/

#define CONAPI  _ASMAPI         /* C calling conventions always */

/****************************************************************************
REMARKS:
Defines the standard VGA compatible default text colors. These are the
values used to pass to the CON_makeAttr macro to create color values for
displaying text on the screen.

HEADER:
console.h
****************************************************************************/
typedef enum {
    CON_BLACK,
    CON_BLUE,
    CON_GREEN,
    CON_CYAN,
    CON_RED,
    CON_MAGENTA,
    CON_BROWN,
    CON_LIGHTGRAY,
    CON_DARKGRAY,
    CON_LIGHTBLUE,
    CON_LIGHTGREEN,
    CON_LIGHTCYAN,
    CON_LIGHTRED,
    CON_LIGHTMAGENTA,
    CON_YELLOW,
    CON_WHITE,
    CON_BLINK = 128,
    } CONSOLE_COLORS;

/****************************************************************************
REMARKS:
Defines the console line wrap modes, which are passed to the CON_setLineWrap
function. The default line wrap mode is CON_LINE_WRAP. CON_WORD_WRAP is
used to wrap entire words at the right window boundary, and will cause any
words that do not fit entirely within the window to wrap to the next line
(unless the word is larger than the entire width, in which case the word
will be split across the two lines).

HEADER:
console.h

MEMBERS:
CON_NO_WRAP     - Do not do any line wrapping at all
CON_LINE_WRAP   - Wrap lines at the end of the right window boundary
CON_WORD_WRAP   - Wrap words at the end of thr right window boundary
****************************************************************************/
typedef enum {
    CON_NO_WRAP,
    CON_LINE_WRAP,
    CON_WORD_WRAP,
    } CONSOLE_WRAP_MODES;

/****************************************************************************
REMARKS:
Defines the text mode cursor shapes.

HEADER:
console.h

MEMBERS:
CON_CURSOR_NORMAL   - Normal underline cursor (insert cursor)
CON_CURSOR_FULL     - Full block cursor (overstrike cursor)
****************************************************************************/
typedef enum {
    CON_CURSOR_NORMAL,
    CON_CURSOR_FULL,
    } CONSOLE_CURSOR_TYPES;

/****************************************************************************
REMARKS:
Defines the console library scrolling directions. The values are passed in
the direction parameter to the CON_scroll function.

HEADER:
console.h

MEMBERS:
CON_SCROLL_UP       - Scroll the window up
CON_SCROLL_DOWN     - Scroll the window down
CON_SCROLL_LEFT     - Scroll the window left
CON_SCROLL_RIGHT    - Scroll the window right
****************************************************************************/
typedef enum {
    CON_SCROLL_UP,
    CON_SCROLL_DOWN,
    CON_SCROLL_LEFT,
    CON_SCROLL_RIGHT,
    } CONSOLE_SCROLL_DIRECTIONS;

/* Macro to build attribute bytes */

#define CON_makeAttr(f,b)   (char)( (((b) & 0x0F) << 4) | ((f) & 0x0F))

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

void    CONAPI CON_init(void);
void    CONAPI CON_set25LineMode(void);
void    CONAPI CON_set43LineMode(void);
void    CONAPI CON_set50LineMode(void);
void    CONAPI CON_restoreMode(void);
void    CONAPI CON_setLineWrap(int mode);
void    CONAPI CON_printf(const char *format, ...);
void    CONAPI CON_puts(const char *str);
void    CONAPI CON_putc(int c);
void    CONAPI CON_writec(int x,int y,int attr,int c);
void    CONAPI CON_write(int x,int y,int attr,const char *str);
void    CONAPI CON_clreol(void);
void    CONAPI CON_clrscr(void);
void    CONAPI CON_gotoxy(int x,int y);
int     CONAPI CON_wherex(void);
int     CONAPI CON_wherey(void);
void    CONAPI CON_delline(void);
void    CONAPI CON_insline(void);
void    CONAPI CON_moveText(int left,int top,int right,int bottom,int destleft,int desttop);
#define CON_bufSize(width,height) ((width) * (height) * 4)
void    CONAPI CON_saveText(int left,int top,int right,int bottom,void *dest);
void    CONAPI CON_restoreText(int left,int top,int right,int bottom,void *source);
void    CONAPI CON_scroll(int direction,int amt);
void    CONAPI CON_fillText(int left,int top,int right,int bottom,int attr,int ch);
void    CONAPI CON_fillAttr(int left,int top,int right,int bottom,int attr);
void    CONAPI CON_setWindow(int left,int top,int right,int bottom);
void    CONAPI CON_getWindow(int *left,int *top,int *right,int *bottom);
int     CONAPI CON_maxx(void);
int     CONAPI CON_maxy(void);
int     CONAPI CON_getAttr(void);
void    CONAPI CON_setAttr(int attr);
void    CONAPI CON_setBackground(int attr,int ch);
void    CONAPI CON_getBackground(int *attr,int *ch);
void    CONAPI CON_setForeColor(int newcolor);
void    CONAPI CON_setBackColor(int newcolor);
void    CONAPI CON_setCursor(int type);
void    CONAPI CON_cursorOff(void);
void    CONAPI CON_restoreCursor(int scans);
int     CONAPI CON_getCursor(void);
int     CONAPI CON_screenWidth(void);
int     CONAPI CON_screenHeight(void);
int     CONAPI CON_kbhit(void);
int     CONAPI CON_getch(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __CONSOLE_H */
