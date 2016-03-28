/****************************************************************************
*
*                   Graphics Mode Console Output Library
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
* Description:  Header file for a graphics mode console manipulation
*               library. The library can also run in text mode similar
*               to the old text mode console library. Essentially this
*               library is new console library that adds support for
*               graphics modes based on SciTech SNAP Graphics, as well as
*               real VGA text modes based on the old console library.
*
*               The console library is written a direct replacement for
*               the old VGA text mode only console library, and hence it
*               can be used to port old text mode console programs to
*               run in graphics mode.
*
*               Note that when running in graphics mode, this library uses
*               the concept of a text mode shadow buffer such that all
*               operations are done on a fake text mode buffer, and the
*               contents of this buffer are draw on the graphics mode
*               screen. Hence you can still read and write blocks of text
*               data from the screen, but the values you read come from the
*               text mode shadow buffer.
*
****************************************************************************/

#ifndef __GCONSOLE_H
#define __GCONSOLE_H

#include "snap/graphics.h"
#include "event.h"

/*--------------------- Macros and type definitions -----------------------*/

#define GCAPI   _ASMAPI         /* C calling conventions always */

/****************************************************************************
REMARKS:
Defines the standard VGA compatible default text colors. These are the
values used to pass to the GC_makeAttr macro to create color values for
displaying text on the screen.

HEADER:
gconsole.h
****************************************************************************/
typedef enum {
    GC_BLACK,
    GC_BLUE,
    GC_GREEN,
    GC_CYAN,
    GC_RED,
    GC_MAGENTA,
    GC_BROWN,
    GC_LIGHTGRAY,
    GC_DARKGRAY,
    GC_LIGHTBLUE,
    GC_LIGHTGREEN,
    GC_LIGHTCYAN,
    GC_LIGHTRED,
    GC_LIGHTMAGENTA,
    GC_YELLOW,
    GC_WHITE,
    GC_BLINK = 128,
    } GC_COLORS;

/****************************************************************************
REMARKS:
Defines the console line wrap modes, which are passed to the GC_setLineWrap
function. The default line wrap mode is GC_LINE_WRAP. GC_WORD_WRAP is
used to wrap entire words at the right window boundary, and will cause any
words that do not fit entirely within the window to wrap to the next line
(unless the word is larger than the entire width, in which case the word
will be split across the two lines).

HEADER:
gconsole.h

MEMBERS:
GC_NO_WRAP      - Do not do any line wrapping at all
GC_LINE_WRAP    - Wrap lines at the end of the right window boundary
GC_WORD_WRAP    - Wrap words at the end of thr right window boundary
****************************************************************************/
typedef enum {
    GC_NO_WRAP,
    GC_LINE_WRAP,
    GC_WORD_WRAP,
    } GC_WRAP_MODES;

/****************************************************************************
REMARKS:
Defines the text mode cursor shapes.

HEADER:
gconsole.h

MEMBERS:
GC_CURSOR_NORMAL    - Normal underline cursor (insert cursor)
GC_CURSOR_FULL      - Full block cursor (overstrike cursor)
****************************************************************************/
typedef enum {
    GC_CURSOR_NORMAL,
    GC_CURSOR_FULL,
    } GC_CURSOR_TYPES;

/****************************************************************************
REMARKS:
Defines the console library scrolling directions. The values are passed in
the direction parameter to the GC_scroll function.

HEADER:
gconsole.h

MEMBERS:
GC_SCROLL_UP        - Scroll the window up
GC_SCROLL_DOWN      - Scroll the window down
GC_SCROLL_LEFT      - Scroll the window left
GC_SCROLL_RIGHT - Scroll the window right
****************************************************************************/
typedef enum {
    GC_SCROLL_UP,
    GC_SCROLL_DOWN,
    GC_SCROLL_LEFT,
    GC_SCROLL_RIGHT,
    } GC_SCROLL_DIRECTIONS;

/****************************************************************************
REMARKS:
Defines the text mode font to use.

HEADER:
gconsole.h

MEMBERS:
GC_FONT_8X8     - 8x8 character cell font
GC_FONT_8X14    - 8x14 character cell font
GC_FONT_8X16    - 8x16 character cell font
****************************************************************************/
typedef enum {
    GC_FONT_8X8 = 1,
    GC_FONT_8X14,
    GC_FONT_8X16,
    } GC_FONT_TYPES;

/****************************************************************************
REMARKS:
Structure maintaining the complete state of the graphics console library.

HEADER:
gconsole.h

MEMBERS:
dc              - SNAP Graphics device context
attr            - Current foreground text attribute
back_attr       - Current background text attribute
back_char       - Current background fill character
win_left        - Current output window left coordinate
win_top         - Current output window top coordinate
win_right       - Current output window right coordinate
win_bottom      - Current output window bottom coordinate
win_width       - Current output window width
win_height      - Current output window height
screen_width    - Current full screen width
screen_height   - Current full screen height
cell_height     - Current character cell height
screen_ptr      - Pointer to the screen data
cnt_x           - Current cursor position X coordinate
cnt_y           - Current cursor position Y coordinate
line_wrap       - Current line wrap mode
vga_state_saved - True if the VGA hardware state has been saved
old_mode        - Old SNAP Graphics display mode
is_graphics     - True if running in graphics mode
vga_state       - Place to store the saved VGA state
****************************************************************************/
typedef struct {
    GA_devCtx               *dc;
    int                     attr;
    int                     back_attr;
    int                     back_char;
    int                     win_left;
    int                     win_top;
    int                     win_right;
    int                     win_bottom;
    int                     win_width;
    int                     win_height;
    int                     screen_width;
    int                     screen_height;
    int                     cell_height;
    char                    *screen_ptr;
    char                    *mono_buf;
    void                    *text_font;
    int                     cnt_x;
    int                     cnt_y;
    ibool                   line_wrap;
    int                     cursor_scans;
    ibool                   cursor_visible;
    ibool                   cursor_excluded;
    ibool                   vga_state_saved;
    _EVT_heartBeatCallback  oldHeartBeat;
    void                    *oldHeartBeatParams;
    ibool                   active;
    ibool                   is_graphics;
    N_uint16                old_mode;
    N_uint16                cnt_mode;
    GA_modeInfo             mi;
    GA_palette              pal[16];
    LZTimerObject           timer;
    REF2D_driver            *ref2d;
    ibool                   unloadRef2d;
    N_int32                 (NAPIP SetMix)(N_int32 mix);
    void                    (NAPIP SetForeColor)(GA_color color);
    void                    (NAPIP SetBackColor)(GA_color color);
    void                    (NAPIP DrawRect)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
    void                    (NAPIP PutMonoImageMSBSys)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 transparent);
    void                    (NAPIP BitBlt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    uchar                   vga_state[1];
    } GC_devCtx;

/* Macro to build attribute bytes */

#define GC_makeAttr(f,b)    (char)( (((b) & 0x0F) << 4) | ((f) & 0x0F))

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

GC_devCtx * GCAPI GC_create(GA_devCtx *dc,int xchars,int ychars,int fontType);
GC_devCtx * GCAPI GC_createExt(GA_devCtx *dc,int xchars,int ychars,int fontType,int bitsPerPixel);
void        GCAPI GC_leave(GC_devCtx *gc);
void        GCAPI GC_leaveExt(GC_devCtx *gc,ibool restoreMode);
void        GCAPI GC_restore(GC_devCtx *gc);
void        GCAPI GC_destroy(GC_devCtx *gc);
void        GCAPI GC_heartBeat(GC_devCtx *gc);
void        GCAPI GC_setLineWrap(GC_devCtx *gc,int mode);
void        GCAPI GC_printf(GC_devCtx *gc,const char *format, ...);
void        GCAPI GC_puts(GC_devCtx *gc,const char *str);
void        GCAPI GC_putc(GC_devCtx *gc,int c);
void        GCAPI GC_writec(GC_devCtx *gc,int x,int y,int attr,int c);
void        GCAPI GC_write(GC_devCtx *gc,int x,int y,int attr,const char *str);
void        GCAPI GC_clreol(GC_devCtx *gc);
void        GCAPI GC_clrscr(GC_devCtx *gc);
void        GCAPI GC_gotoxy(GC_devCtx *gc,int x,int y);
int         GCAPI GC_wherex(GC_devCtx *gc);
int         GCAPI GC_wherey(GC_devCtx *gc);
void        GCAPI GC_delline(GC_devCtx *gc);
void        GCAPI GC_insline(GC_devCtx *gc);
void        GCAPI GC_moveText(GC_devCtx *gc,int left,int top,int right,int bottom,int destleft,int desttop);
int         GCAPI GC_bufSize(GC_devCtx *gc,int width,int height);
void        GCAPI GC_saveText(GC_devCtx *gc,int left,int top,int right,int bottom,void *dest);
void        GCAPI GC_restoreText(GC_devCtx *gc,int left,int top,int right,int bottom,void *source);
void        GCAPI GC_scroll(GC_devCtx *gc,int direction,int amt);
void        GCAPI GC_fillText(GC_devCtx *gc,int left,int top,int right,int bottom,int attr,int ch);
void        GCAPI GC_fillAttr(GC_devCtx *gc,int left,int top,int right,int bottom,int attr);
void        GCAPI GC_setWindow(GC_devCtx *gc,int left,int top,int right,int bottom);
void        GCAPI GC_getWindow(GC_devCtx *gc,int *left,int *top,int *right,int *bottom);
int         GCAPI GC_maxx(GC_devCtx *gc);
int         GCAPI GC_maxy(GC_devCtx *gc);
int         GCAPI GC_getAttr(GC_devCtx *gc);
void        GCAPI GC_setAttr(GC_devCtx *gc,int attr);
void        GCAPI GC_setBackground(GC_devCtx *gc,int attr,int ch);
void        GCAPI GC_getBackground(GC_devCtx *gc,int *attr,int *ch);
void        GCAPI GC_setForeColor(GC_devCtx *gc,int newcolor);
void        GCAPI GC_setBackColor(GC_devCtx *gc,int newcolor);
void        GCAPI GC_setCursor(GC_devCtx *gc,int type);
void        GCAPI GC_cursorOff(GC_devCtx *gc);
void        GCAPI GC_restoreCursor(GC_devCtx *gc,int scans);
int         GCAPI GC_getCursor(GC_devCtx *gc);
int         GCAPI GC_screenWidth(GC_devCtx *gc);
int         GCAPI GC_screenHeight(GC_devCtx *gc);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __GCONSOLE_H */

