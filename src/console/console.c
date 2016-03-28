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
* Description:  Main module for console manipulation module. This is a
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "console.h"
#include "pmapi.h"

/*---------------------------- Global variables ---------------------------*/

#define LINES_43    0x2000
#define LINES_50    0x4000

static int      _attr = 0x07;           /* Current global attribute     */
static int      _backAttr = 0x07;       /* Background attribute         */
static int      _backChar = ' ';        /* Background character         */
static int      _win_left;              /* Left edge of window          */
static int      _win_top;               /* Top edge of window           */
static int      _win_right;             /* Right edge of window         */
static int      _win_bottom;            /* Bottom edge of window        */
static int      _win_width;             /* Width of window              */
static int      _win_height;            /* Height of window             */
static int      _screenWidth;           /* Width of entire screen       */
static int      _screenHeight;          /* Height of entire screen      */
static int      _cellHeight;            /* Character cell height        */
static char     *_screenPtr;            /* Pointer to video display     */
static int      _cnt_x = 0,_cnt_y = 0;  /* Current cursor coordinates   */
static ibool    _lineWrap = CON_LINE_WRAP;/* Line wrap mode             */
static int      _inited = 0;            /* Has CON_init been called?    */
static int      _old_width;
static int      _old_height;
static int      _no_disp;

#include "vgadata.c"

/*---------------------------- Implementation -----------------------------*/

/****************************************************************************
PARAMETERS:
port    - I/O port to read value from
index   - Port index to read

RETURNS:
Byte read from 'port' register 'index'.
****************************************************************************/
static ushort CON_rdinx(
    ushort port,
    ushort index)
{
    PM_outpb(port,index);
    return PM_inpb(port+1);
}

/****************************************************************************
PARAMETERS:
port    - I/O port to write to
index   - Port index to write
value   - Byte to write to port

REMARKS:
Writes a byte value to the 'port' register 'index'.
****************************************************************************/
static void CON_wrinx(
    ushort port,
    ushort index,
    ushort value)
{
    PM_outpb(port,index);
    PM_outpb(port+1,value);
}

/****************************************************************************
PARAMETERS:
port    - I/O port to set
index   - Index register to program
mask    - Mask of bits to clear

REMARKS:
Clears the bits specified by the mask to a logical zero for the I/O port
index register.
****************************************************************************/
static void CON_clrinx(
    ushort port,
    ushort index,
    ushort mask)
{
    CON_wrinx(port,index,CON_rdinx(port,index) & ~mask);
}

#define bufferAddr(x,y) (_screenPtr + (((_screenWidth * (y)) + (x)) * 2))

#define writeChar(x,y,ch,attr)      \
{                                   \
    char *p = bufferAddr((x),(y));  \
    *p++ = (ch);                    \
    *p = attr;                      \
}

#define writeStr(x,y,attr,str,len)  \
{                                   \
    char *p = bufferAddr(x,y);      \
    while ((len)--) {               \
        *p++ = *str++;              \
        *p++ = attr;                \
        }                           \
}

/****************************************************************************
PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
destleft    - Destination region left edge
desttop     - Destination region top edge

REMARKS:
Moves a block of video memory to another location in video memory, using
direct video memory access.
****************************************************************************/
static void moveText(
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop)
{
    int     i,increment,len;
    char    *source,*dest;

    increment = _screenWidth * 2;
    len = (right - left + 1) * 2;
    if (top > dstTop) {
        source = bufferAddr(left,top);
        dest = bufferAddr(dstLeft,dstTop);
        for (i = top; i <= bottom; i++) {
            memmove(dest,source,len);
            source += increment;
            dest += increment;
            }
        }
    else {
        source = bufferAddr(left,bottom);
        dest = bufferAddr(dstLeft,(dstTop + bottom - top));
        for (i = top; i <= bottom; i++) {
            memmove(dest,source,len);
            source -= increment;
            dest -= increment;
            }
        }
}

/****************************************************************************
PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
source      - Place to store the memory region

REMARKS:
Moves a block of video memory from a buffer directly to the screen.
****************************************************************************/
static void moveToScreen(
    int left,
    int top,
    int right,
    int bottom,
    char *source)
{
    int     i,increment,len;
    char    *dest = bufferAddr(left,top);

    increment = _screenWidth * 2;
    len = (right - left + 1) * 2;
    for (i = top; i <= bottom; i++) {
        memcpy(dest,source,len);
        source += len;
        dest += increment;
        }
}

/****************************************************************************
PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
dest        - Place to store the memory region

REMARKS:
Moves a block of video memory from the screen into a buffer.
****************************************************************************/
static void moveFromScreen(
    int left,
    int top,
    int right,
    int bottom,
    char *dest)
{
    int     i,increment,len;
    char    *source = bufferAddr(left,top);

    increment = _screenWidth * 2;
    len = (right - left + 1) * 2;
    for (i = top; i <= bottom; i++) {
        memcpy(dest,source,len);
        source += increment;
        dest += len;
        }
}

/****************************************************************************
PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
attr        - Attribute to fill with
ch          - Character to fill with

REMARKS:
Fills a block of memory with the specified character and attribute.
****************************************************************************/
static void fillText(
    int left,
    int top,
    int right,
    int bottom,
    int attr,
    int ch)
{
    int     i,j,increment,len;
    char    *dest = bufferAddr(left,top);

    len = (right - left + 1);
    increment = (_screenWidth - len) * 2;
    for (i = top; i <= bottom; i++) {
        for (j = 0; j < len; j++) {
            *dest++ = ch;
            *dest++ = attr;
            }
        dest += increment;
        }
}

/****************************************************************************
PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
attr        - Attribute to fill with

REMARKS:
Fills a block of memory with the specified attribute, leaving the
characters unchanged.
****************************************************************************/
static void fillAttr(
    int left,
    int top,
    int right,
    int bottom,
    int attr)
{
    int     i,j,increment,len;
    char    *dest = bufferAddr(left,top);

    len = (right - left + 1);
    increment = (_screenWidth - len) * 2;
    for (i = top; i <= bottom; i++) {
        for (j = 0; j < len; j++) {
            dest++;
            *dest++ = attr;
            }
        dest += increment;
        }
}

/****************************************************************************
PARAMETERS:
buf - Buffer text to word wrap
len - Length of the buffer

RETURNS:
Number of characters to back up for word wrap (0 for no wrap)

REMARKS:
Goes through the current buffer string to determine if we need to word wrap
the current string. If the last character in the buffer is a space, then we
dont need to wrap and return 0. We also return 0 if the word is too large
to fit on a line.
****************************************************************************/
static int wordWrap(
    const char *buf,
    int len)
{
    const char  *p = &buf[len];

    while (p > buf && !isspace(*p))
        p--;
    return len - (p - buf);
}

/****************************************************************************
PARAMETERS:
buf - Buffer text to output
len - Length of the buffer

REMARKS:
Copies the buffer of text and displays it on the screen at the current
cursor location.
****************************************************************************/
static void outputBuf(
    const char *buf,
    int len)
{
    writeStr(_cnt_x + _win_left,_cnt_y + _win_top,_attr,buf,len);
}

/****************************************************************************
REMARKS:
Short delay for programming VGA registers
****************************************************************************/
static void vga_delay(void)
{
    int i;
    for (i = 0; i < 10; i++)
        ;
}

/****************************************************************************
REMARKS:
Ensure the VGA screen is turned on
****************************************************************************/
static void vga_screenon(void)
{
    CON_clrinx(SEQ_I,0x01,0x20);
    PM_inpb(IS1_R);
    vga_delay();
    PM_outpb(ATT_IW,0x20);
}

/****************************************************************************
REMARKS:
Saves the VGA registers into an array
****************************************************************************/
static void _CON_saveVGARegisters(
    uchar *regs)
{
    int i;

    /* Save state of VGA registers */
    for (i = 0; i < CRT_C; i++)
        regs[CRT + i] = CON_rdinx(CRT_I,i);
    for (i = 0; i < ATT_C; i++) {
        PM_inpb(IS1_R);
        vga_delay();
        PM_outpb(ATT_IW,i);
        vga_delay();
        regs[ATT + i] = PM_inpb(ATT_R);
        vga_delay();
        }
    for (i = 0; i < GRA_C; i++)
        regs[GRA + i] = CON_rdinx(GRA_I,i);
    for (i = 0; i < SEQ_C; i++)
        regs[SEQ + i] = CON_rdinx(SEQ_I,i);
    regs[MIS] = PM_inpb(MIS_R);
    vga_screenon();
}

/****************************************************************************
REMARKS:
Restores the VGA registers from an array
****************************************************************************/
static void _CON_restoreVGARegisters(
    const uchar *regs)
{
    int i;

    /* Restore the state of the VGA sequencer registers */
    PM_outpb(MIS_W,regs[MIS]);
    CON_wrinx(SEQ_I,0x00,0x01);
    CON_wrinx(SEQ_I,0x01,regs[SEQ + 1] | 0x20);
    for (i = 2; i < SEQ_C; i++)
        CON_wrinx(SEQ_I,i,regs[SEQ + i]);
    CON_wrinx(SEQ_I,0x00,0x03);

    /* Deprotect CRT registers 0-7 and write CRTC */
    CON_clrinx(CRT_I,0x11,0x80);
    for (i = 0; i < CRT_C; i++)
        CON_wrinx(CRT_I,i,regs[CRT + i]);
    for (i = 0; i < GRA_C; i++)
        CON_wrinx(GRA_I,i,regs[GRA + i]);
    for (i = 0; i < ATT_C; i++) {
        PM_inpb(IS1_R);     /* reset flip-flop */
        vga_delay();
        PM_outpb(ATT_IW,i);
        vga_delay();
        PM_outpb(ATT_IW,regs[ATT + i]);
        vga_delay();
        }
    vga_screenon();
}

/****************************************************************************
REMARKS:
Read the font data from the VGA character generator RAM
****************************************************************************/
static void _CON_getFont(
    uchar *data)
{
    uchar   *A0000Ptr = PM_getA0000Pointer();
    uchar   save[7];

    /* Enable access to character generator RAM */
    save[0] = CON_rdinx(SEQ_I,0x00);
    save[1] = CON_rdinx(SEQ_I,0x02);
    save[2] = CON_rdinx(SEQ_I,0x04);
    save[3] = CON_rdinx(SEQ_I,0x00);
    save[4] = CON_rdinx(GRA_I,0x04);
    save[5] = CON_rdinx(GRA_I,0x05);
    save[6] = CON_rdinx(GRA_I,0x06);
    CON_wrinx(SEQ_I,0x00,0x01);
    CON_wrinx(SEQ_I,0x02,0x04);
    CON_wrinx(SEQ_I,0x04,0x07);
    CON_wrinx(SEQ_I,0x00,0x03);
    CON_wrinx(GRA_I,0x04,0x02);
    CON_wrinx(GRA_I,0x05,0x00);
    CON_wrinx(GRA_I,0x06,0x00);

    /* Copy character generator RAM */
    data[0] = 8;
    data[1] = 32;
    memcpy(data+2,A0000Ptr,CHAR_SIZE * 256);

    CON_wrinx(SEQ_I,0x00,save[0]);
    CON_wrinx(SEQ_I,0x02,save[1]);
    CON_wrinx(SEQ_I,0x04,save[2]);
    CON_wrinx(SEQ_I,0x00,save[3]);
    CON_wrinx(GRA_I,0x04,save[4]);
    CON_wrinx(GRA_I,0x05,save[5]);
    CON_wrinx(GRA_I,0x06,save[6]);
}

/****************************************************************************
REMARKS:
Downloads the font data to the VGA character generator RAM
****************************************************************************/
static void _CON_setFont(
    uchar *data)
{
    uchar   *A0000Ptr = PM_getA0000Pointer();
    int     i,cellHeight = data[1];

    /* Enable access to character generator RAM */
    CON_wrinx(SEQ_I,0x00,0x01);
    CON_wrinx(SEQ_I,0x02,0x04);
    CON_wrinx(SEQ_I,0x04,0x07);
    CON_wrinx(SEQ_I,0x00,0x03);
    CON_wrinx(GRA_I,0x04,0x02);
    CON_wrinx(GRA_I,0x05,0x00);
    CON_wrinx(GRA_I,0x06,0x00);

    /* Zero character generator RAM */
    memset(A0000Ptr,0,CHAR_SIZE * 256);
    for (i = 0; i < 256; i++)
        memcpy(A0000Ptr+(i*CHAR_SIZE),data+2+(i*cellHeight),cellHeight);
}

/****************************************************************************
DESCRIPTION:
Initialise the text mode console library.

HEADER:
console.h

REMARKS:
Initialise the console library subsystem. This should be called at program
inception, and also after any changes to the current video mode that are
not done with the console library (such as if you are using SciTech SNAP
Graphics to switch to an extended VGA text mode such as 100x50 or 132x50).

When you are finished, use the CON_restoreMode function to restore the
state of the text mode console back to it's original state. This will
restore the original size (ie: 80x25, 80x43 or 80x50) as well as the
original text mode font.

NOTE:   You /must/ ensure that you first open a console for output before
        initialising the console library, using the PM_openConsole function.

SEE ALSO:
CON_restoreMode, PM_openConsole
****************************************************************************/
void CONAPI CON_init(void)
{
    int vDisp,dispStart;

    /* Read the screen width and height directly from the hardware */
    PM_init();
    vDisp = (CON_rdinx(CRT_I,0x12)
        | ((CON_rdinx(CRT_I,0x7) << 7) & 0x100)
        | ((CON_rdinx(CRT_I,0x7) << 3) & 0x200)) + 1;
    if (vDisp == 1024) {
        dispStart = 0;
        _cellHeight = 16;
        _win_width = 80;
        _win_height = 25;
        _screenWidth = _win_width;
        _screenHeight =  _win_height;
        _win_left = _win_top = 0;
        _win_right = _win_width - 1;
        _win_bottom = _win_height - 1;
        _screenPtr = PM_malloc(_win_width * _win_height * 2);
        _no_disp = true;
        }
    else {
        dispStart = CON_rdinx(CRT_I,0x0D) | (CON_rdinx(CRT_I,0x0C) << 8);
        _cellHeight = (CON_rdinx(CRT_I,0x9) & 0x1F) + 1;
        _win_width = CON_rdinx(CRT_I,0x01)+1;
        _win_height = vDisp / _cellHeight;
        _screenWidth = _win_width;
        _screenHeight =  _win_height;
        _win_left = _win_top = 0;
        _win_right = _win_width - 1;
        _win_bottom = _win_height - 1;
        _screenPtr = PM_mapPhysicalAddr(0xB8000,0xFFFF,true);
        _screenPtr += dispStart * 2;
        _no_disp = false;
        }
    if (!_inited) {
        _old_width = _screenWidth;
        _old_height = _screenHeight;
        if(!_no_disp) {
            _CON_getFont(FONT_save);
            _CON_saveVGARegisters(VGA_save);
            }
        _inited = true;
        }
    CON_setCursor(CON_CURSOR_NORMAL);
    CON_setLineWrap(CON_LINE_WRAP);
}

/****************************************************************************
DESCRIPTION:
Switch the display to 25 line text mode.

HEADER:
console.h

REMARKS:
Set the standard VGA 80x25 line text mode.

SEE ALSO:
CON_set43LineMode, CON_set50LineMode
****************************************************************************/
void CONAPI CON_set25LineMode(void)
{
    if (!_no_disp) {
        _CON_setFont(VGA_font8x16);
        _CON_restoreVGARegisters(VGA_80x25);
        PM_setOSScreenWidth(80,25);
        CON_init();
        }
}

/****************************************************************************
DESCRIPTION:
Switch the display to 43 line text mode.

HEADER:
console.h

REMARKS:
Set the standard VGA 80x43 line text mode.

SEE ALSO:
CON_set25LineMode, CON_set50LineMode
****************************************************************************/
void CONAPI CON_set43LineMode(void)
{
    if (!_no_disp) {
        _CON_setFont(VGA_font8x8);
        _CON_restoreVGARegisters(VGA_80x43);
        PM_setOSScreenWidth(80,43);
        CON_init();
        }
}

/****************************************************************************
DESCRIPTION:
Switch the display to 50 line text mode.

HEADER:
console.h

REMARKS:
Set the standard VGA 80x50 line text mode.

SEE ALSO:
CON_set25LineMode, CON_set43LineMode
****************************************************************************/
void CONAPI CON_set50LineMode(void)
{
    if (!_no_disp) {
        _CON_setFont(VGA_font8x8);
        _CON_restoreVGARegisters(VGA_80x50);
        PM_setOSScreenWidth(80,50);
        CON_init();
        }
}

/****************************************************************************
DESCRIPTION:
Restore the original text mode and shut down the console library.

HEADER:
console.h

REMARKS:
Restore the original text mode active when CON_init was called, and shut
down the console library. Will correctly restore 25, 43 and 50 line VGA
video modes. You must call CON_init again if you wish to use the console
library to display anything on the screen.

NOTE:   Don't forget to call PM_closeConsole to close the console when you
        are done!

SEE ALSO:
CON_init, PM_closeConsole
****************************************************************************/
void CONAPI CON_restoreMode(void)
{
    if (_inited) {
        if (!_no_disp) {
            _CON_setFont(FONT_save);
            _CON_restoreVGARegisters(VGA_save);
            PM_setOSScreenWidth(_old_width,_old_height);
            }
        CON_gotoxy(_cnt_x,_cnt_y);
        _inited = false;
        }
}

/****************************************************************************
DESCRIPTION:
Set the line wrap mode for the console library.

HEADER:
console.h

PARAMETERS:
mode    - New line wrap mode (CONSOLE_WRAP_MODES)

REMARKS:
This function changes the line wrap mode for the console library. The default
line wrap mode is CON_LINE_WRAP, and will cause lines to be split at the
window boundary. CON_NO_WRAP will do no wrapping at all. CON_WORD_WRAP is
used to wrap entire words at the right window boundary, and will cause any
words that do not fit entirely within the window to wrap to the next line
(unless the word is larger than the entire width, in which case the word
will be split across the two lines).
****************************************************************************/
void CONAPI CON_setLineWrap(
    int mode)
{ _lineWrap = mode; }

/****************************************************************************
DESCRIPTION:
Function to print formatted text to the console.

HEADER:
console.h

PARAMETERS:
format  - Format string
...     - Extra parameters based on format string

REMARKS:
This function prints formatted text to the current window on console.
Formatting information is handled similarly to the regular C library
printf function, with the addition of the following formatting characters:

    \f  Clear window and home cursor
    \r  Go back to the start of the current line
    \b  Move one character left (non-destructive)

The screen will scroll up if you go past the bottom line of the window.
Characters that go past the end of the current line wrap depending on the
setting of wrapmode. If wrapmode is on (default), characters wrap to the
beginning of the next line, otherwise they wrap to the beginning of
the same line.

SEE ALSO:
CON_puts, CON_putc, CON_write, CON_writec
****************************************************************************/
void CONAPI CON_printf(
    const char *format,
    ...)
{
    va_list     args;
    static char buf[2048];

    va_start(args,format);
    vsprintf(buf,format,args);
    CON_puts(buf);
    va_end(args);
}

/****************************************************************************
DESCRIPTION:
Function to print text to the console.

HEADER:
console.h

PARAMETERS:
str - String to output

REMARKS:
Outputs a string to the text console. This function is similar to the C
library puts function, with the addition of the following formatting
characters:

    \f  Clear window and home cursor
    \r  Go back to the start of the current line
    \b  Move one character left (non-destructive)

The screen will scroll up if you go past the bottom line of the window.
Characters that go past the end of the current line wrap depending on the
setting of wrapmode. If wrapmode is on (default), characters wrap to the
beginning of the next line, otherwise they wrap to the beginning of
the same line.

SEE ALSO:
CON_printf, CON_putc, CON_write, CON_writec
****************************************************************************/
void CONAPI CON_puts(
    const char *str)
{
    static char buf[256];
    char        *p;
    int         len,diffLen;

    len = *(p = buf) = 0;
    while (*str) {
        switch (*str) {
            case '\f':
                CON_clrscr();
                len = *(p = buf) = 0;       /* Discard buffer contents  */
                break;
            case '\r':
                outputBuf(buf,len);
                len = *(p = buf) = 0;
                _cnt_x = 0;
                CON_gotoxy(_cnt_x,_cnt_y);
                break;
            case '\n':
                outputBuf(buf,len);
                len = *(p = buf) = 0;
                _cnt_x = 0;
                if (++_cnt_y >= _win_height) {
                    _cnt_y = _win_height-1;
                    CON_scroll(CON_SCROLL_UP,1);
                    }
                CON_gotoxy(_cnt_x,_cnt_y);
                break;
            case '\b':
                if (_cnt_x + --len < 0)
                    len = 0;
                else
                    --p;
                break;
            default:
                *p++ = *str;
                len++;
                if (_cnt_x + len >= _win_width) {
                    if (_lineWrap == CON_WORD_WRAP) {
                        diffLen = wordWrap(buf,len);
                        if (diffLen != len || _cnt_x != 0) {
                            p -= diffLen;
                            len -= diffLen;
                            outputBuf(buf,len);
                            _cnt_x = 0;
                            if (++_cnt_y >= _win_height) {
                                _cnt_y = _win_height-1;
                                CON_scroll(CON_SCROLL_UP,1);
                                }
                            while (diffLen > 0 && isspace(*p)) {
                                p++;
                                diffLen--;
                                }
                            if (diffLen) {
                                outputBuf(p,diffLen);
                                }
                            len = *(p = buf) = 0;
                            _cnt_x = diffLen;
                            }
                        }
                    if (len != 0) {
                        outputBuf(buf,len);
                        len = *(p = buf) = 0;
                        _cnt_x = 0;
                        if (_lineWrap != CON_NO_WRAP) {
                            if (++_cnt_y >= _win_height) {
                                _cnt_y = _win_height-1;
                                CON_scroll(CON_SCROLL_UP,1);
                                }
                            }
                        }
                    CON_gotoxy(_cnt_x,_cnt_y);
                    }
                break;
            }
        str++;
        }
    outputBuf(buf,len);
    _cnt_x += len;
    CON_gotoxy(_cnt_x,_cnt_y);
}

/****************************************************************************
DESCRIPTION:
Function to print a single character to the console.

HEADER:
console.h

PARAMETERS:
c   - Characters to output

REMARKS:
Outputs a single character to the text console. This function is similar to
the C library putc function, with the addition of the following formatting
characters:

    \f  Clear window and home cursor
    \r  Go back to the start of the current line
    \b  Move one character left (non-destructive)

The screen will scroll up if you go past the bottom line of the window.
Characters that go past the end of the current line wrap depending on the
setting of wrapmode. If wrapmode is on (default), characters wrap to the
beginning of the next line, otherwise they wrap to the beginning of
the same line.

SEE ALSO:
CON_printf, CON_puts, CON_write, CON_writec
****************************************************************************/
void CONAPI CON_putc(
    int c)
{
    switch (c) {
        case 0:
            break;                      /* Ignore ASCII NULL's          */
        case '\f':
            CON_clrscr();
            break;
        case '\n':
            if (++_cnt_y >= _win_height) {
                _cnt_y = _win_height-1;
                CON_scroll(CON_SCROLL_UP,1);
                }
            break;
        case '\r':
            _cnt_x = 0;
            break;
        case '\b':
            if (--_cnt_x < 0)
                _cnt_x = 0;
            break;
        default:
            writeChar(_cnt_x + _win_left,_cnt_y + _win_top,c,_attr);
            if (++_cnt_x >= _win_width) {
                _cnt_x = 0;
                if (_lineWrap) {
                    if (++_cnt_y >= _win_height) {
                        _cnt_y = _win_height-1;
                        CON_scroll(CON_SCROLL_UP,1);
                        }
                    }
                }
            break;
        }
    CON_gotoxy(_cnt_x,_cnt_y);
}

/****************************************************************************
DESCRIPTION:
Clears from the current position to the end of the line.

HEADER:
console.h

REMARKS:
Clears from the current position to the end of the line.

SEE ALSO:
CON_clrscr, CON_delline, CON_insline
****************************************************************************/
void CONAPI CON_clreol(void)
{
    CON_fillText(_cnt_x,_cnt_y,_win_width-1,_cnt_y,_backAttr,_backChar);
}

/****************************************************************************
DESCRIPTION:
Clears the current window

HEADER:
console.h

REMARKS:
Clears the window and moves the cursor to the top left hand corner of the
window (0,0). The window is cleared with the current background character
and in the current background attribute.

SEE ALSO:
CON_clreol, CON_delline, CON_insline
****************************************************************************/
void CONAPI CON_clrscr(void)
{
    CON_fillText(0,0,_win_width-1,_win_height-1,_backAttr,_backChar);
    _cnt_x = _cnt_y = 0;
    CON_gotoxy(0,0);
}

/****************************************************************************
DESCRIPTION:
Deletes the current line of text.

HEADER:
console.h

REMARKS:
Deletes the current line and moves all text below it up one line.

SEE ALSO:
CON_clrscr, CON_clreol, CON_insline
****************************************************************************/
void CONAPI CON_delline(void)
{
    int     top;

    top = _win_top;
    _win_top += _cnt_y;
    CON_scroll(CON_SCROLL_UP,1);
    _win_top = top;
}

/****************************************************************************
DESCRIPTION:
Inserts a new line at the current cursor position.

HEADER:
console.h

REMARKS:
Inserts a new line at the current cursor position. Lines below the line
are moved down, and the bottom line is lost.

SEE ALSO:
CON_clrscr, CON_clreol, CON_delline
****************************************************************************/
void CONAPI CON_insline(void)
{
    int     top;

    top = _win_top;
    _win_top += _cnt_y;
    CON_scroll(CON_SCROLL_DOWN,1);
    _win_top = top;
}

/****************************************************************************
DESCRIPTION:
Function to print text to the console.

HEADER:
console.h

PARAMETERS:
x       - X coordinate to write string at
y       - Y coordinate to write string at
attr    - Color attribute to write string in
str     - String to write

REMARKS:
This function outputs a string to the text console. Formatting info is
/not/ handled (using CON_printf for that). The string is clipped at the
window boundary. This function is a /lot/ faster than CON_printf and
CON_puts since formatting information is not handled. The cursor is not
moved.

SEE ALSO:
CON_printf, CON_puts, CON_putc, CON_writec
****************************************************************************/
void CONAPI CON_write(
    int x,
    int y,
    int attr,
    const char *str)
{
    int     len;

    if (y < 0 || y >= _win_height || x < 0)
        return;
    len = strlen(str);
    if (x + len - 1 >= _win_width)
        len = _win_width - x;
    if (len <= 0)
        return;
    writeStr(x + _win_left,y + _win_top,attr,str,len);
}

/****************************************************************************
DESCRIPTION:
Function to print single character to the console.

HEADER:
console.h

PARAMETERS:
x       - X coordinate to write string at
y       - Y coordinate to write string at
attr    - Color attribute to write string in
c       - Character to write

REMARKS:
Displays a single character on the console. Formatting info is /not/ handled
(use CON_putc for that), and the character is clipped at the window boundary.
This function is a /lot/ faster than CON_putc and since formatting
information is not handled. The cursor is not moved.

SEE ALSO:
CON_printf, CON_puts, CON_putc, CON_write
****************************************************************************/
void CONAPI CON_writec(
    int x,
    int y,
    int attr,
    int c)
{
    if (y < 0 || y >= _win_height || x < 0)
        return;
    if (x >= _win_width)
        return;
    writeChar(x + _win_left,y + _win_top,c,attr);
}

/****************************************************************************
DESCRIPTION:
Moves a block of text to another location.

HEADER:
console.h

PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
destleft    - Destination region left edge
desttop     - Destination region top edge

REMARKS:
This function moves a block of text to another location on the console.
This function properly handles overlapping blocks of text, to ensure
that screen corruption does not occur.

SEE ALSO:
CON_saveText, CON_restoreText, CON_scroll
****************************************************************************/
void CONAPI CON_moveText(
    int left,
    int top,
    int right,
    int bottom,
    int destleft,
    int desttop)
{
    moveText(_win_left + left,_win_top + top,_win_left + right,
        _win_top + bottom,_win_left + destleft,_win_top + desttop);
}

/****************************************************************************
DESCRIPTION:
Saves a block of text to a system memory buffer.

HEADER:
console.h

PARAMETERS:
left    - Left edge of screen region
top     - Top edge of screen region
right   - Right edge of screen region
bottom  - Bottom edge of screen region
dest    - Buffer to hold data

REMARKS:
This function saves a block of text from the console to a system memory
buffer. The buffer of text can be restored to the same or a different
location on the screen with the CON_restoreText function.

SEE ALSO:
CON_moveText, CON_restoreText, CON_scroll, CON_bufSize
****************************************************************************/
void CONAPI CON_saveText(
    int left,
    int top,
    int right,
    int bottom,
    void *dest)
{
    moveFromScreen(_win_left + left,_win_top + top,_win_left + right,
        _win_top + bottom,dest);
}

/****************************************************************************
DESCRIPTION:
Restore a block of text from a system memory buffer.

HEADER:
console.h

PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
source      - Buffer holding data to use

REMARKS:
This function restores a block of text to the console from a system memory
buffer. The buffer of text can be saves from the same or a different
location on the screen with the CON_saveText function.

SEE ALSO:
CON_moveText, CON_saveText, CON_scroll, CON_bufSize
****************************************************************************/
void CONAPI CON_restoreText(
    int left,
    int top,
    int right,
    int bottom,
    void *source)
{
    moveToScreen(_win_left + left,_win_top + top,_win_left + right,
        _win_top + bottom,source);
}

/****************************************************************************
DESCRIPTION:
Fill a block of text on the console.

HEADER:
console.h

PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
attr        - Attribute to fill with
ch          - Character to fill with

REMARKS:
Fills a region of the screen with the specified character and color
attribute.

SEE ALSO:
CON_fillAttr
****************************************************************************/
void CONAPI CON_fillText(
    int left,
    int top,
    int right,
    int bottom,
    int attr,
    int ch)
{
    fillText(_win_left + left,_win_top + top,_win_left + right,
        _win_top + bottom,attr,ch);
}

/****************************************************************************
DESCRIPTION:
Change a block of text on the console to a specific color attribute.

HEADER:
console.h

PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
attr        - Attribute to fill with

REMARKS:
Fills a region of the screen with the specified color attribute. The
original text on the screen is not changed, only the color attributes.

SEE ALSO:
CON_fillText
****************************************************************************/
void CONAPI CON_fillAttr(
    int left,
    int top,
    int right,
    int bottom,
    int attr)
{
    fillAttr(_win_left + left,_win_top + top,_win_left + right,
        _win_top + bottom,attr);
}

/****************************************************************************
DESCRIPTION:
Restore a block of text from a system memory buffer.

HEADER:
console.h

PARAMETERS:
direction   - Direction to scroll area in (CONSOLE_SCROLL_DIRECTIONS)
amt         - Amount to scroll area by (lines or characters)

REMARKS:
Scrolls the current window in the indicated direction by the indicated
amount, filling in the exposed area with the current background character
and color attribute.

SEE ALSO:
CON_moveText, CON_saveText, CON_restoreText
****************************************************************************/
void CONAPI CON_scroll(
    int direction,
    int amt)
{
    switch (direction) {
        case CON_SCROLL_UP:
            if (amt <= 0 || amt >= _win_height) {
                CON_clrscr();
                return;
                }
            moveText(_win_left,_win_top + amt,_win_right,
                _win_bottom,_win_left,_win_top);
            fillText(_win_left,_win_bottom - (amt-1),_win_right,
                _win_bottom,_backAttr,_backChar);
            break;
        case CON_SCROLL_DOWN:
            if (amt <= 0 || amt >= _win_height) {
                CON_clrscr();
                return;
                }
            moveText(_win_left,_win_top,_win_right,_win_bottom - amt,
                _win_left,_win_top + amt);
            fillText(_win_left,_win_top,_win_right,_win_top+(amt-1),
                _backAttr,_backChar);
            break;
        case CON_SCROLL_RIGHT:
            if (amt <= 0 || amt > _win_width) {
                CON_clrscr();
                return;
                }
            moveText(_win_left,_win_top,_win_right - amt,_win_bottom,
                _win_left + amt,_win_top);
            fillText(_win_left,_win_top,_win_left + (amt-1),_win_bottom,
                _backAttr,_backChar);
            break;
        case CON_SCROLL_LEFT:
            if (amt <= 0 || amt > _win_width) {
                CON_clrscr();
                return;
                }
            moveText(_win_left + amt,_win_top,_win_right,_win_bottom,
                _win_left,_win_top);
            fillText(_win_right-(amt-1),_win_top,_win_right,_win_bottom,
                _backAttr,_backChar);
            break;
        }
}

/****************************************************************************
DESCRIPTION:
Set the text window for console output.

HEADER:
console.h

PARAMETERS:
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region

REMARKS:
Sets the current window to the specified location on the screen, and moves
the cursor to the top left hand corner. All subsequent text output will
be relative to this window, and clipped to the window boundary (for
functions that do clipping).

SEE ALSO:
CON_getWindow, CON_wherex, CON_wherey, CON_gotoxy, CON_maxx, CON_maxyy,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
void CONAPI CON_setWindow(
    int left,
    int top,
    int right,
    int bottom)
{
    _win_left = MAX(left,0);
    _win_right = MIN(right,_screenWidth-1);
    _win_top = MAX(top,0);
    _win_bottom = MIN(bottom,_screenHeight-1);
    _win_width = _win_right - _win_left + 1;
    _win_height = _win_bottom - _win_top + 1;
    CON_gotoxy(0,0);
}

/****************************************************************************
DESCRIPTION:
Set the text window for console output.

HEADER:
console.h

PARAMETERS:
left        - Place to store left edge of window
top         - Place to store top edge of window
right       - Place to store right edge of window
bottom      - Place to store bottom edge of window

REMARKS:
Returns the current window to the specified location on the screen.

SEE ALSO:
CON_setWindow, CON_wherex, CON_wherey, CON_gotoxy, CON_maxx, CON_maxyy,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
void CONAPI CON_getWindow(
    int *left,
    int *top,
    int *right,
    int *bottom)
{
    *left = _win_left;
    *top = _win_top;
    *right = _win_right;
    *bottom = _win_bottom;
}

/****************************************************************************
DESCRIPTION:
Returns the current cursor X coordinate

HEADER:
console.h

RETURNS:
Current cursor X coordinate

REMARKS:
Returns the current cursor X coordinate

SEE ALSO:
CON_setWindow, CON_getWindow, CON_wherey, CON_gotoxy, CON_maxx, CON_maxyy,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
int CONAPI CON_wherex(void)
{
    return _cnt_x;
}

/****************************************************************************
DESCRIPTION:
Returns the current cursor Y coordinate

HEADER:
console.h

RETURNS:
Current cursor Y coordinate

REMARKS:
Returns the current cursor Y coordinate

SEE ALSO:
CON_setWindow, CON_getWindow, CON_wherex, CON_gotoxy, CON_maxx, CON_maxyy,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
int CONAPI CON_wherey(void)
{
    return _cnt_y;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium X coordinate for current window

HEADER:
console.h

RETURNS:
Maximum X coordinate for current window (inclusive)

REMARKS:
Returns the maxium X coordinate for current window

SEE ALSO:
CON_setWindow, CON_getWindow, CON_wherex, CON_wherey, CON_gotoxy, CON_maxyy,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
int CONAPI CON_maxx(void)
{
    return _win_width - 1;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium Y coordinate for current window

HEADER:
console.h

RETURNS:
Maximum Y coordinate for current window (inclusive)

REMARKS:
Returns the maxium Y coordinate for current window

SEE ALSO:
CON_setWindow, CON_getWindow, CON_wherex, CON_wherey, CON_gotoxy, CON_maxx,
CON_getScreenWidth, CON_getScreenHeight
****************************************************************************/
int CONAPI CON_maxy(void)
{
    return _win_height - 1;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium Y coordinate for current window

HEADER:
console.h

PARAMETERS:
x   - New cursor X coordinate
y   - New cursor Y coordinate

REMARKS:
Moves the cursor location to (x,y) on the console. If the location lies
outside of the current window, the cursor is hidden by moving it off the
entire screen. Functions such as CON_puts, CON_printf and CON_putc all
output to the current cursor location.

SEE ALSO:
CON_setWindow, CON_getWindow, CON_wherex, CON_wherey, CON_maxx, CON_maxy
****************************************************************************/
void CONAPI CON_gotoxy(
    int x,
    int y)
{
    int offset;

    if (x >= _win_width || y >= _win_height) {
        x = _screenWidth;
        y = _screenHeight;
        }
    PM_setOSCursorLocation(_win_left+x,_win_top+y);
    offset = (_win_top + y) * _screenWidth + _win_left + x;
    CON_wrinx(CRT_I,0x0E,(offset >> 8) & 0xFF);
    CON_wrinx(CRT_I,0x0F,offset & 0xFF);
    _cnt_x = x;
    _cnt_y = y;
}

/****************************************************************************
DESCRIPTION:
Sets the current background character and attribute.

HEADER:
console.h

PARAMETERS:
attr    - New background attribute
ch      - New background character

REMARKS:
Sets the current background character and attribute. The current background
character and attribute are used to fill in any empty space created by
the CON_scroll, CON_delline and CON_clrscr functions.

SEE ALSO:
CON_getBackground, CON_setAttr, CON_setForeColor, CON_setBackColor
****************************************************************************/
void CONAPI CON_setBackground(
    int attr,
    int ch)
{
    _backAttr = attr;
    _backChar = ch;
}

/****************************************************************************
DESCRIPTION:
Returns the current background character and attribute.

HEADER:
console.h

PARAMETERS:
attr    - Place to store background attribute
ch      - Place to store background character

REMARKS:
Returns the current background character and attribute. The current background
character and attribute are used to fill in any empty space created by
the CON_scroll, CON_delline and CON_clrscr functions.

SEE ALSO:
CON_setBackground, CON_setAttr, CON_setForeColor, CON_setBackColor
****************************************************************************/
void CONAPI CON_getBackground(
    int *attr,
    int *ch)
{
    *attr = _backAttr;
    *ch = _backChar;
}

/****************************************************************************
DESCRIPTION:
Sets the current text color attribute.

HEADER:
console.h

PARAMETERS:
attr    - New text color attribute

REMARKS:
Sets the current text color attribute. The current text color
attribute is used to display all text on the screen for all text output
functions. The text attribute consists of both the foreground and
background colors used by the text characters.

You can use the CON_makeAttr macro to create a text attribute from separate
foreground and background colors.

SEE ALSO:
CON_setBackground, CON_getBackground, CON_setForeColor, CON_setBackColor,
CON_makeAttr
****************************************************************************/
void CONAPI CON_setAttr(
    int attr)
{
    _attr = attr;
}

/****************************************************************************
DESCRIPTION:
Returns the current text color attribute.

HEADER:
console.h

RETURNS:
Current text color attribute

REMARKS:
Returns the current text color attribute. The current text color
attribute is used to display all text on the screen for all text output
functions. The text attribute consists of both the foreground and
background colors used by the text characters.

SEE ALSO:
CON_setBackground, CON_getBackground, CON_getAttr, CON_setForeColor,
CON_setBackColor
****************************************************************************/
int CONAPI CON_getAttr(void)
{
    return _attr;
}

/****************************************************************************
DESCRIPTION:
Sets the current text background color.

HEADER:
console.h

PARAMETERS:
newcolor    - New background text color (CONSOLE_COLORS)

REMARKS:
Sets the background color for the current text attribute.

SEE ALSO:
CON_setBackground, CON_getBackground, CON_setAttr, CON_setForeColor,
****************************************************************************/
void CONAPI CON_setBackColor(
    int newcolor)
{
    _attr = (_attr & 0x0F) | ((newcolor & 0x0F) << 4);
}

/****************************************************************************
DESCRIPTION:
Sets the current text foreground color.

HEADER:
console.h

PARAMETERS:
newcolor    - New foreground text color (CONSOLE_COLORS)

REMARKS:
Sets the foreground color for the current text attribute.

SEE ALSO:
CON_setBackground, CON_getBackground, CON_setAttr, CON_setBackColor,
****************************************************************************/
void CONAPI CON_setForeColor(
    int newcolor)
{
    _attr = (_attr & 0xF0) | (newcolor & 0x0F);
}

/****************************************************************************
DESCRIPTION:
Returns the current full text screen width

HEADER:
console.h

RETURNS:
Current full text screen width

REMARKS:
Returns the current full text screen width

SEE ALSO:
CON_getScreenHeight
****************************************************************************/
int CONAPI CON_screenWidth(void)
{
    return _screenWidth;
}

/****************************************************************************
DESCRIPTION:
Returns the current full text screen height

HEADER:
console.h

RETURNS:
Current full text screen height

REMARKS:
Returns the current full text screen height

SEE ALSO:
CON_getScreenHeight
****************************************************************************/
int CONAPI CON_screenHeight(void)
{
    return _screenHeight;
}

/****************************************************************************
DESCRIPTION:
Sets the text mode cursor to type

HEADER:
console.h

PARAMETERS:
type    - New cursor type to set (CONSOLE_CURSOR_TYPES)

REMARKS:
Sets the text mode cursor type.

SEE ALSO:
CON_cursorOff, CON_getCursor, CON_restoreCursor
****************************************************************************/
void CONAPI CON_setCursor(
    int type)
{
    int scans = 0;

    switch (type) {
        case CON_CURSOR_NORMAL:
            scans = ((_cellHeight-2) << 8) | (_cellHeight-1);
            break;
        case CON_CURSOR_FULL:
            scans = _cellHeight-1;
            break;
        }
    if (_cellHeight >= 16)
        scans -= 0x101;
    CON_restoreCursor(scans);
}

/****************************************************************************
DESCRIPTION:
Hides the text cursor.

HEADER:
console.h

REMARKS:
Hides the text cursor.

SEE ALSO:
CON_setCursor, CON_getCursor, CON_restoreCursor
****************************************************************************/
void CONAPI CON_cursorOff(void)
{
    CON_restoreCursor(0x2000);
}

/****************************************************************************
DESCRIPTION:
Returns the current text cursor

HEADER:
console.h

RETURNS:
Current text cursor cursor

REMARKS:
Returns the current text cursor. The high byte contains the top
scanline and the low byte contains the bottom scanline.

SEE ALSO:
CON_setCursor, CON_cursorOff, CON_restoreCursor
****************************************************************************/
int CONAPI CON_getCursor(void)
{
    return ((CON_rdinx(CRT_I,0x0A) << 8)-1) | CON_rdinx(CRT_I,0x0B);
}

/****************************************************************************
DESCRIPTION:
Restores a previous cursor setting

HEADER:
console.h

PARAMETERS:
scans   - Cursor scan lines

REMARKS:
Restores a previously saved cursor value. The high byte contains the top
scanline and the low byte contains the bottom scanline.

SEE ALSO:
CON_setCursor, CON_cursorOff, CON_getCursor
****************************************************************************/
void CONAPI CON_restoreCursor(
    int scans)
{
    CON_wrinx(CRT_I,0x0A,((scans >> 8) & 0xFF));
    CON_wrinx(CRT_I,0x0B,scans & 0xFF);
}

/****************************************************************************
DESCRIPTION:
Return the code for the next pending key.

HEADER:
console.h

RETURNS:
Character code for the key, or 0 if no keypress is pending.

REMARKS:
This function returns the character code for the next pending key in the
keyboard queue, or 0 if no keypress is pending. This function will not
block until a key is pressed.

NOTE:   This function is obsolete. You shoud try to use the new PM library
        event functions instead of this function if possible.

SEE ALSO:
CON_getch
****************************************************************************/
int CONAPI CON_kbhit(void)
{
    return PM_kbhit();
}

/****************************************************************************
DESCRIPTION:
Return the code for the next pending key.

HEADER:
console.h

RETURNS:
Character code for the key.

REMARKS:
This function returns the character code for the next pending key in the
keyboard queue, and will block until a key is pressed.

NOTE:   This function is obsolete. You shoud try to use the new PM library
        event functions instead of this function if possible.

SEE ALSO:
CON_kbhit
****************************************************************************/
int CONAPI CON_getch(void)
{
    return PM_getch();
}

/* C comment documentation for the console library macros */

#if 0

/****************************************************************************
DESCRIPTION:
Make a color attribute from a foreground and background color pair.

HEADER:
console.h

PARAMETERS:
f   - Foreground color (CONSOLE_COLORS)
b   - Background color (CONSOLE_COLORS)

RETURNS:
New text color attribute

REMARKS:
This macro creates a color attribute from a foreground and background
color pair.
****************************************************************************/
uchar CON_makeAttr(
    uchar f,
    uchar b);
/* Macro definition */

/****************************************************************************
DESCRIPTION:
Return the size of the buffer to hold a width x height text block.

HEADER:
console.h

PARAMETERS:
width   - Width of the text block
height  - Heigth of the text block

RETURNS:
Size needed to save the text block in memory, in bytes.

REMARKS:
This macro is used to determine the amount of memory needed to store a block
of text that is width x height in size. You should use this macro to
determine how much memory you need for the CON_saveText and CON_restoreText
functions.

SEE ALSO:
CON_saveText, CON_restoreText
****************************************************************************/
int CON_bufSize(
    int width,
    int height);
/* Macro definition */

#endif

