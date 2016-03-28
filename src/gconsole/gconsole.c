/****************************************************************************
*
*                     Graphics Mode Console Output Library
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
* Description:  Main module for a graphics mode console manipulation
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "gconsole.h"
#include "pmapi.h"
#include "event.h"

/*---------------------------- Global variables ---------------------------*/

static GA_palette GC_defPal[16] = {
    {0x00,0x00,0x00,0},
    {0xA8,0x00,0x00,0},
    {0x00,0xA8,0x00,0},
    {0xA8,0xA8,0x00,0},
    {0x00,0x00,0xA8,0},
    {0xA8,0x00,0xA8,0},
    {0x00,0xA8,0xA8,0},
    {0xA8,0xA8,0xA8,0},
    {0x54,0x00,0x00,0},
    {0xFC,0x00,0x00,0},
    {0x54,0xA8,0x00,0},
    {0xFC,0xA8,0x00,0},
    {0x54,0x00,0xA8,0},
    {0xFC,0x00,0xA8,0},
    {0x54,0xA8,0xA8,0},
    {0xFC,0xA8,0xA8,0},
    };

/*---------------------------- Implementation -----------------------------*/

/* Number of microseconds between cursor toggles */

#define CURSOR_TOGGLE_TIME (1000000 / 4)

/****************************************************************************
PARAMETERS:
port    - I/O port to read value from
index   - Port index to read

RETURNS:
Byte read from 'port' register 'index'.
****************************************************************************/
static ushort GC_rdinx(
    ushort port,
    ushort index)
{
    PM_outpb(port,(uchar)index);
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
static void GC_wrinx(
    ushort port,
    ushort index,
    ushort value)
{
    PM_outpb(port,(uchar)index);
    PM_outpb(port+1,(uchar)value);
}

#define bufferAddr(x,y) (gc->screen_ptr + (((gc->screen_width * (y)) + (x)) * 2))

#define writeChar(x,y,ch,attr)          \
{                                       \
    char *p = bufferAddr((x),(y));      \
    *p++ = (ch);                        \
    *p = attr;                          \
    if (gc->is_graphics)                \
        _updateScreen(gc,x,y,1,1);      \
}

#define writeStr(x,y,attr,str,len)      \
{                                       \
    char *p = bufferAddr(x,y);          \
    int i = (len);                      \
    while (i--) {                       \
        *p++ = *str++;                  \
        *p++ = attr;                    \
        }                               \
    if (gc->is_graphics)                \
        _updateScreen(gc,x,y,len,1);    \
}

#define updateScreen(left,top,right,bottom)                     \
{                                                               \
    if (gc->is_graphics)                                        \
        _updateScreen(gc,left,top,right-left+1,bottom-top+1);   \
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
#define rgbColor(r,g,b,pf)                                                          \
    ((u32)(((r) >> (pf)->RedAdjust)   & (pf)->RedMask)   << (pf)->RedPosition)    \
  | ((u32)(((g) >> (pf)->GreenAdjust) & (pf)->GreenMask) << (pf)->GreenPosition)  \
  | ((u32)(((b) >> (pf)->BlueAdjust)  & (pf)->BlueMask)  << (pf)->BluePosition)

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
static GA_color realColor(
    GC_devCtx *gc,
    int index)
{
    if (gc->mi.BitsPerPixel <= 8)
        return index & 15;
    return rgbColor(gc->pal[index].Red,
                    gc->pal[index].Green,
                    gc->pal[index].Blue,
                    &gc->mi.PixelFormat);
}

/****************************************************************************
REMARKS:
Draws the cursor at it's current location
****************************************************************************/
static void drawCursor(
    GC_devCtx *gc)
{
    uchar   *src;

    // TODO: We may need to change this; the cursor should not be XOR'ed
    //       on the screen but rather drawn over the character block!

    if (gc->cursor_scans != 0x2000) {
        // Find the character position in the buffer that the cursor is over
        // so that we can find the color to draw the cursor in.
        src = (uchar*)gc->screen_ptr + (gc->cnt_y * gc->screen_width + gc->cnt_x) * 2;

        // Now XOR the cursor onto the screen at the appropriate location.
        gc->SetMix(GA_XOR_MIX);
        gc->SetForeColor(realColor(gc,src[1] & 0xF));
        gc->DrawRect(
            gc->cnt_x * 8,
            gc->cnt_y * gc->cell_height + (gc->cursor_scans & 0xFF),
            8,
            (gc->cursor_scans & 0xFF) - (gc->cursor_scans >> 8) + 1);
        }
}

/****************************************************************************
REMARKS:
Toggles the cursor from being visible on the screen and vice versa
****************************************************************************/
static void toggleCursor(
    GC_devCtx *gc)
{
    gc->cursor_visible ^= 1;
    drawCursor(gc);
}

/****************************************************************************
REMARKS:
Excludes the cursor from the screen if it is presently visible at it's
current location.
****************************************************************************/
static void excludeCursor(
    GC_devCtx *gc)
{
    if (gc->is_graphics && gc->cursor_visible && gc->cursor_excluded++ == 0)
        drawCursor(gc);
}

/****************************************************************************
REMARKS:
Unexcludes the cursor from the screen if it is presently visible and
re-draws it at the current location (which may be different from the
location it was excluded at!).
****************************************************************************/
static void unexcludeCursor(
    GC_devCtx *gc)
{
    if (gc->is_graphics && gc->cursor_visible && --gc->cursor_excluded == 0)
        drawCursor(gc);
}

/****************************************************************************
PARAMETERS:
left    - Left edge of screen region
top     - Top edge of screen region
width   - Width of the text block to update
height  - Height of the text block to update

REMARKS:
Updates the graphics screen from the text mode shadow buffer.
****************************************************************************/
static void _updateScreen(
    GC_devCtx *gc,
    int left,
    int top,
    int width,
    int height)
{
    int     x,y;
    int     cell_height = gc->cell_height;
    int     stride = gc->screen_width * 2;
    uchar   *font,*src;

    // TODO: We may want to optimise this code to coalesce data that has
    //       the same attributes to do larger fills with PutMonoImage

    /* Now construct the mono bitmap buffer and draw it */
    font = gc->text_font;
    src = (uchar*)gc->screen_ptr + top * stride + left * 2;
    gc->SetMix(GA_REPLACE_MIX);
    for (y = top; y < top+height; y++) {
        uchar *s = src;
        for (x = left; x < left+width; x++, s += 2) {
            uchar ch = s[0];
            uchar attr = s[1];
            uchar *image = (font + ch * cell_height);
            gc->SetBackColor(realColor(gc,attr >> 16));
            gc->SetForeColor(realColor(gc,attr & 0xF));
            gc->PutMonoImageMSBSys(
                x * 8,y * cell_height,8,cell_height,
                1,image,false);
            }
        src += stride;
        }
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
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    int dstLeft,
    int dstTop)
{
    int     i,increment,len,cell_height = gc->cell_height;
    char    *source,*dest;

    increment = gc->screen_width * 2;
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

    /* Now blit the contents of the graphics screen */
    if (gc->is_graphics) {
        left *= 8;      top *= cell_height;
        right *= 8;     bottom *= cell_height;
        dstLeft *= 8;   dstTop *= cell_height;
        gc->BitBlt(left,top,right-left,bottom-top,dstLeft,dstTop,GA_REPLACE_MIX);
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
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    char *source)
{
    int     i,increment,len;
    char    *dest = bufferAddr(left,top);

    increment = gc->screen_width * 2;
    len = (right - left + 1) * 2;
    for (i = top; i <= bottom; i++) {
        memcpy(dest,source,len);
        source += len;
        dest += increment;
        }
    updateScreen(left,top,right,bottom);
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
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    char *dest)
{
    int     i,increment,len;
    char    *source = bufferAddr(left,top);

    increment = gc->screen_width * 2;
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
    GC_devCtx *gc,
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
    increment = (gc->screen_width - len) * 2;
    for (i = top; i <= bottom; i++) {
        for (j = 0; j < len; j++) {
            *dest++ = ch;
            *dest++ = attr;
            }
        dest += increment;
        }
    updateScreen(left,top,right,bottom);
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
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    int attr)
{
    int     i,j,increment,len;
    char    *dest = bufferAddr(left,top);

    len = (right - left + 1);
    increment = (gc->screen_width - len) * 2;
    for (i = top; i <= bottom; i++) {
        for (j = 0; j < len; j++) {
            dest++;
            *dest++ = attr;
            }
        dest += increment;
        }
    updateScreen(left,top,right,bottom);
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
    GC_devCtx *gc,
    const char *buf,
    int len)
{
    writeStr(gc->cnt_x + gc->win_left,gc->cnt_y + gc->win_top,gc->attr,buf,len);
}

/****************************************************************************
REMARKS:
Searches for a suitable text mode to use in the list of available modes
****************************************************************************/
static N_uint16 FindTextMode(
    GA_devCtx *dc,
    GA_initFuncs *init,
    GA_modeInfo *mi,
    int x,
    int y)
{
    N_uint16    *modes;
    int         xChars,yChars;

    /* Search for the display mode to use */
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        mi->dwSize = sizeof(*mi);
        if (init->GetVideoModeInfo(*modes,mi) != 0)
            continue;
        if (!(mi->Attributes & gaIsTextMode))
            continue;
        xChars = mi->XResolution / mi->XCharSize;
        yChars = mi->YResolution / mi->YCharSize;
        if (xChars == x && yChars == y) {
            return *modes;
            }
        }
    return 0xFFFF;
}

/****************************************************************************
REMARKS:
Searches for a suitable text mode to use in the list of available modes
****************************************************************************/
static N_uint16 FindGraphicsMode(
    GA_devCtx *dc,
    GA_initFuncs *init,
    GA_modeInfo *mi,
    int x,
    int y,
    int cellHeight,
    int minBpp)
{
    N_uint16    *modes;
    int         xChars,yChars;

    // TODO: Change this to use custom modes if available and we can't
    //       find an exact match in the mode tables.

    /* Search for the display mode to use */
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        mi->dwSize = sizeof(*mi);
        if (init->GetVideoModeInfo(*modes,mi) != 0)
            continue;
        if (mi->Attributes & (gaIsTextMode | gaIsGUIDesktop))
            continue;
        if (mi->BitsPerPixel < minBpp)
            continue;
        xChars = mi->XResolution / 8;
        yChars = mi->YResolution / cellHeight;
        if (xChars >= x && yChars >= y) {
            if (mi->Attributes & gaHaveLinearBuffer)
                return *modes | gaLinearBuffer;
            return *modes;
            }
        }
    return 0xFFFF;
}

/****************************************************************************
DESCRIPTION:
Heart beat function to handle the text mode flashing cursor

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

REMARKS:
This function should be called periodically if the text mode cursor should
flash on the screen. This function is called automatically inside the
GC_kbhit and GC_getch function, but you should call it in the main loop
for any code that relies on the text cursor to flash.
****************************************************************************/
void GCAPI GC_heartBeat(
    GC_devCtx *gc)
{
    if (gc->active && gc->is_graphics && (LZTimerLapExt(&gc->timer) > CURSOR_TOGGLE_TIME)) {
        LZTimerOffExt(&gc->timer);
        LZTimerOnExt(&gc->timer);
        toggleCursor(gc);
        }
}

/****************************************************************************
REMARKS:
Callback function from the event library to call the heartbeat function.
****************************************************************************/
static void GCAPI GC_heartBeatCallback(
    void *params)
{
    GC_devCtx   *gc = (GC_devCtx*)params;

    if (gc->oldHeartBeat)
        gc->oldHeartBeat(gc->oldHeartBeatParams);
    GC_heartBeat(gc);
}

/****************************************************************************
DESCRIPTION:
Initialise the graphics mode console library and create a device context.

HEADER:
gconsole.h

PARAMETERS:
dc          - SNAP Graphics device context to use
xchars      - Number of characters in the X direction
ychars      - Number of characters in the Y direction
fontType    - Font type to load

RETURNS:
Pointer to device context, or NULL on failure

REMARKS:
Initialise the console library subsystem. This should be called at program
inception and you should pass this function the size of the text mode you
would like to support. If the desired text mode is not found in the driver
the console library will find the first lowest resolution graphics mode
that can support the desired text resolution and leave blank space on the
right and bottom edges of the screen as necessary.

Note that the fontType parameter only determines the font size that will
be used in graphics modes. The font type parameter is ignored if real
text modes are used. You can force graphics mode operation with this
function by setting the fontType parameter to be negative, although
that method is now obsolete and you should use GC_createExt() instead.

When you are finished, use the GC_exit function to restore the
state of the console back to it's original state. This will
restore the original size (ie: 80x25, 80x43 or 80x50) as well as the
original text mode font.

NOTE:   You /must/ ensure that you first open a console for output before
        initialising the console library, using the PM_openConsole function.

SEE ALSO:
GC_createExt, GC_destroy, PM_openConsole
****************************************************************************/
GC_devCtx * GCAPI GC_create(
    GA_devCtx *dc,
    int xchars,
    int ychars,
    int fontType)
{
    GC_devCtx   *gc;

    if (fontType < 0) {
        if ((gc = GC_createExt(dc,xchars,ychars,-fontType,8)) != NULL)
            return gc;
        return GC_createExt(dc,xchars,ychars,-fontType,4);
        }
    return GC_createExt(dc,xchars,ychars,fontType,0);
}

/****************************************************************************
DESCRIPTION:
Initialise the graphics mode console library and create a device context.

HEADER:
gconsole.h

PARAMETERS:
dc              - SNAP Graphics device context to use
xchars          - Number of characters in the X direction
ychars          - Number of characters in the Y direction
fontType        - Font type to load
bitsPerPixel    - Minimum color depth for graphics mode operation (0 for text)

RETURNS:
Pointer to device context, or NULL on failure

REMARKS:
Initialise the console library subsystem. This should be called at program
inception and you should pass this function the size of the text mode you
would like to support. If the desired text mode is not found in the driver
the console library will find the first lowest resolution graphics mode
that can support the desired text resolution and leave blank space on the
right and bottom edges of the screen as necessary.

Note that the fontType parameter only determines the font size that will
be used in graphics modes. The font type parameter is ignored if real
text modes are used.

The bitsPerPixel value is used to force the console to use graphics mode
rather than text mode, and sets the minimum color depth for graphics mode
operation. If you set this value to 0, text mode will be used. Any value
other than zero will force graphics mode operation.

When you are finished, use the GC_exit function to restore the
state of the console back to it's original state. This will
restore the original size (ie: 80x25, 80x43 or 80x50) as well as the
original text mode font.

NOTE:   You /must/ ensure that you first open a console for output before
        initialising the console library, using the PM_openConsole function.

SEE ALSO:
GC_create, GC_destroy, PM_openConsole
****************************************************************************/
GC_devCtx * GCAPI GC_createExt(
    GA_devCtx *dc,
    int xchars,
    int ychars,
    int fontType,
    int bitsPerPixel)
{
    int                 vDisp,dispStart,size;
    GC_devCtx           *gc;
    GA_initFuncs        init;

    /* Allocate memory for the device context */
    PM_init();
    size = sizeof(*gc) + PM_getVGAStateSize();
    if ((gc = calloc(1,size)) == NULL)
        return NULL;

    /* Connect with SNAP Graphics and determine if we are in graphics mode */
    gc->is_graphics = false;
    gc->dc = dc;
    if (dc) {
        /* Get SNAP Graphics init functions */
        init.dwSize = sizeof(init);
        if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
            PM_fatalError("Unable to get device driver functions!");
        if (xchars == -1 || ychars == -1) {
            /* The mode was set externally by the program before calling GC_create() */
            gc->old_mode = 0xFFFF;
            gc->cnt_mode = 0xFFFF;
            gc->cell_height = 8;
            gc->mi.dwSize = sizeof(gc->mi);
            init.GetCurrentVideoModeInfo(&gc->mi);
            gc->text_font = dc->TextFont8x8;
            }
        else {
            /* Find the appropriate display mode to use. If you set the font
             * type to be negative this will force a graphics mode to be used
             * instead (mainly for testing purposes so it is undocumented).
             */
            gc->old_mode = (N_uint16)init.GetVideoMode();
            if (bitsPerPixel > 0 || (gc->cnt_mode = FindTextMode(dc,&init,&gc->mi,xchars,ychars)) == 0xFFFF) {
                switch (ABS(fontType)) {
                    case GC_FONT_8X14:
                        gc->cell_height = 14;
                        gc->text_font = dc->TextFont8x14;
                        break;
                    case GC_FONT_8X16:
                        gc->cell_height = 16;
                        gc->text_font = dc->TextFont8x16;
                        break;
                    case GC_FONT_8X8:
                    default:
                        gc->cell_height = 8;
                        gc->text_font = dc->TextFont8x8;
                        break;
                    }
                if ((gc->cnt_mode = FindGraphicsMode(dc,&init,&gc->mi,xchars,ychars,gc->cell_height,bitsPerPixel)) == 0xFFFF)
                    goto Error;
                }
            }
        if (!(gc->mi.Attributes & gaIsTextMode)) {
            /* Setup for graphics mode operation */
            gc->is_graphics = true;
            gc->screen_width = gc->mi.XResolution / 8;
            gc->screen_height = gc->mi.YResolution / gc->cell_height;
            gc->screen_ptr = malloc(gc->screen_width * gc->screen_height * 2);
            gc->mono_buf = malloc(gc->screen_width / 8 * gc->screen_height);
            memcpy(gc->pal,GC_defPal,sizeof(GC_defPal));
            }
        }

    /* Set the console mode by restoring the graphics console */
    GC_restore(gc);

    /* Read the screen width and height directly from the hardware if
     * we are running in VGA mode
     */
    if (!gc->is_graphics) {
        vDisp = (GC_rdinx(0x3D4,0x12)
            | ((GC_rdinx(0x3D4,0x7) << 7) & 0x100)
            | ((GC_rdinx(0x3D4,0x7) << 3) & 0x200)) + 1;
        dispStart = GC_rdinx(0x3D4,0x0D) | (GC_rdinx(0x3D4,0x0C) << 8);
        gc->cell_height = (GC_rdinx(0x3D4,0x9) & 0x1F) + 1;
        gc->screen_width = GC_rdinx(0x3D4,0x01)+1;
        gc->screen_height =  vDisp / gc->cell_height;
        gc->screen_ptr = PM_mapPhysicalAddr(0xB8000,0xFFFF,true);
        gc->screen_ptr += dispStart * 2;
        }

    /* Initialise the internals of the library */
    gc->win_width = gc->screen_width;
    gc->win_height = gc->screen_height;
    gc->win_left = gc->win_top = 0;
    gc->win_right = gc->win_width - 1;
    gc->win_bottom = gc->win_height - 1;
    gc->cnt_x = 0;
    gc->cnt_y = 0;
    GC_setAttr(gc,0x07);
    GC_setBackground(gc,0x07,' ');
    GC_setCursor(gc,GC_CURSOR_NORMAL);
    GC_setLineWrap(gc,GC_LINE_WRAP);

    /* Setup the cursor heartbeat function */
    EVT_getHeartBeatCallback(&gc->oldHeartBeat,&gc->oldHeartBeatParams);
    EVT_setHeartBeatCallback(GC_heartBeatCallback,gc);
    LZTimerOnExt(&gc->timer);
    return gc;

Error:
    if (gc)
        free(gc);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Restore the console to the state active prior to calling GC_leave

HEADER:
gconsole.h

PARAMETERS:
gc  - Device context to restore

REMARKS:
This function restores the console for operation by setting the necessary
display mode after direct control over the console was relinquished to
the application programmer.

SEE ALSO:
GC_leave, GC_create, GC_destroy
****************************************************************************/
void GCAPI GC_restore(
    GC_devCtx *gc)
{
    GA_initFuncs        init;
    GA_2DStateFuncs     state2d;
    GA_2DRenderFuncs    draw2d;
    GA_bufferFuncs      bufmgr;
    GA_buf              *activeBuf;
    GA_CRTCInfo         crtc;
    N_int32             virtualX,virtualY,bytesPerLine,maxMem;

    /* Save the VGA hardware state if necessary */
    if (!gc->is_graphics) {
        if (!gc->vga_state_saved) {
            PM_saveVGAState(gc->vga_state);
            gc->vga_state_saved = true;
            }
        }
    if (gc->dc) {
        /* Get SNAP Graphics init functions */
        init.dwSize = sizeof(init);
        if (!GA_queryFunctions(gc->dc,GA_GET_INITFUNCS,&init))
            PM_fatalError("Unable to get device driver functions!");

        /* Set the display mode for the console library */
        if (gc->cnt_mode != 0xFFFF) {
            virtualX = virtualY = bytesPerLine = -1;
            if (init.SetVideoMode(gc->cnt_mode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc) == -1)
                PM_fatalError("Unable to initialise display mode!");
            gc->mi.dwSize = sizeof(gc->mi);
            init.GetCurrentVideoModeInfo(&gc->mi);
            }
        if (!(gc->mi.Attributes & gaIsTextMode)) {
            /* Load the SNAP Graphics reference rasteriser for the color depth */
            if ((gc->ref2d = GA_getCurrentRef2d(gc->dc->DeviceIndex)) == NULL) {
                /* Must unload Ref2d when exiting console mode to avoid conflicts with other instances */
                gc->unloadRef2d = true;
                if (!GA_loadRef2d(gc->dc,false,&gc->mi,gc->mi.BytesPerScanLine * gc->mi.YResolution,&gc->ref2d))
                    PM_fatalError("Unable to load SNAP Graphics Reference Rasteriser!");
                }
            state2d.dwSize = sizeof(state2d);
            REF2D_queryFunctions(gc->ref2d,GA_GET_2DSTATEFUNCS,&state2d);
            draw2d.dwSize = sizeof(draw2d);
            REF2D_queryFunctions(gc->ref2d,GA_GET_2DRENDERFUNCS,&draw2d);

            /* Initialise the buffer manager and find the primary buffer */
            bufmgr.dwSize = sizeof(bufmgr);
            if (!REF2D_queryFunctions(gc->ref2d,GA_GET_BUFFERFUNCS,&bufmgr))
                PM_fatalError("GetBufferFuncs failed!");
            if (!bufmgr.InitBuffers(1,0,NULL))
                PM_fatalError("InitBuffers failed!");
            activeBuf = bufmgr.GetPrimaryBuffer();
            bufmgr.SetActiveBuffer(activeBuf);

            /* Cache the rendering vectors we use for drawing */
            gc->SetMix              = state2d.SetMix;
            gc->SetForeColor        = state2d.SetForeColor;
            gc->SetBackColor        = state2d.SetBackColor;
            gc->DrawRect            = draw2d.DrawRect;
            gc->PutMonoImageMSBSys  = draw2d.PutMonoImageMSBSys_Old;
            gc->BitBlt              = draw2d.BitBlt;
            }
        }

    // TODO: Program the hardware VGA palette for text modes!!
    gc->active = true;
}

/****************************************************************************
DESCRIPTION:
Leaves the console and optionally restoring the prior active display mode

HEADER:
gconsole.h

PARAMETERS:
gc          - Device context to leave
restoreMode - True to restore previous display mode, false to leave alone

REMARKS:
This function is used to relinquish control over the display for an active
console so that you can do something else with the display (such as
go into graphics mode or start a different display mode and create a new
console for that mode). You should call GC_restore() when you are ready
to restore the console for operation.

If you pass true for the restoreMode parameter, this function will restore
the display mode to the original mode that was active when the GC was
created. If you just plan to set another display mode immediately after
leaving the console, you can pass false for this parameter.

SEE ALSO:
GA_leave, GC_restore, GC_create, GC_destroy
****************************************************************************/
void GCAPI GC_leaveExt(
    GC_devCtx *gc,
    ibool restoreMode)
{
    GA_initFuncs    init;
    GA_CRTCInfo     crtc;
    N_int32         virtualX,virtualY,bytesPerLine,maxMem;

    if (gc->cursor_visible)
        toggleCursor(gc);
    gc->active = false;
    if (restoreMode) {
        if (gc->dc) {
            init.dwSize = sizeof(init);
            if (!GA_queryFunctions(gc->dc,GA_GET_INITFUNCS,&init))
                PM_fatalError("Unable to get device driver functions!");
            if (gc->old_mode != 0xFFFF) {
                virtualX = virtualY = bytesPerLine = -1;
                if (init.SetVideoMode(gc->old_mode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc) == -1)
                    PM_fatalError("Unable to initialise display mode!");
                }
            if (gc->is_graphics && gc->unloadRef2d)
                GA_unloadRef2d(gc->dc);
            }
        if (gc->vga_state_saved)
            PM_restoreVGAState(gc->vga_state);
        }
}

/****************************************************************************
DESCRIPTION:
Leaves the console and make it inactive

HEADER:
gconsole.h

PARAMETERS:
gc  - Device context to leave

REMARKS:
This function is used to relinquish control over the display for an active
console so that you can do something else with the display (such as
go into graphics mode or start a different display mode and create a new
console for that mode). You should call GC_restore() when you are ready
to restore the console for operation.

SEE ALSO:
GA_leaveExt, GC_restore, GC_create, GC_destroy
****************************************************************************/
void GCAPI GC_leave(
    GC_devCtx *gc)
{
    GC_leaveExt(gc,false);
}

/****************************************************************************
DESCRIPTION:
Destroy the console device context

HEADER:
gconsole.h

PARAMETERS:
gc  - Device context to destroy

REMARKS:
Restore the original text mode active when GC_create was called, and shut
down the console library. Will correctly restore 25, 43 and 50 line VGA
video modes. You must call GC_create again if you wish to use the console
library to display anything on the screen.

NOTE:   Don't forget to call PM_closeConsole to close the console when you
        are done!

SEE ALSO:
GC_create, PM_closeConsole
****************************************************************************/
void GCAPI GC_destroy(
    GC_devCtx *gc)
{
    if (gc) {
        /* Restore the old heartbeat function */
        EVT_setHeartBeatCallback(gc->oldHeartBeat,gc->oldHeartBeatParams);

        /* Restore the old display mode */
        GC_leaveExt(gc,true);
        if (gc->is_graphics) {
            free(gc->mono_buf);
            free(gc->screen_ptr);
            }
        free(gc);
        }
}

/****************************************************************************
DESCRIPTION:
Set the line wrap mode for the console library.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
mode    - New line wrap mode (CONSOLE_WRAP_MODES)

REMARKS:
This function changes the line wrap mode for the console library. The default
line wrap mode is GC_LINE_WRAP, and will cause lines to be split at the
window boundary. GC_NO_WRAP will do no wrapping at all. GC_WORD_WRAP is
used to wrap entire words at the right window boundary, and will cause any
words that do not fit entirely within the window to wrap to the next line
(unless the word is larger than the entire width, in which case the word
will be split across the two lines).
****************************************************************************/
void GCAPI GC_setLineWrap(
    GC_devCtx *gc,
    int mode)
{
    gc->line_wrap = mode;
}

/****************************************************************************
DESCRIPTION:
Function to print formatted text to the console.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
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
GC_puts, GC_putc, GC_write, GC_writec
****************************************************************************/
void GCAPI GC_printf(
    GC_devCtx *gc,
    const char *format,
    ...)
{
    va_list     args;
    static char buf[2048];

    va_start(args,format);
    vsprintf(buf,format,args);
    GC_puts(gc,buf);
    va_end(args);
}

/****************************************************************************
DESCRIPTION:
Function to print text to the console.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
str     - String to output

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
GC_printf, GC_putc, GC_write, GC_writec
****************************************************************************/
void GCAPI GC_puts(
    GC_devCtx *gc,
    const char *str)
{
    static char buf[256];
    char        *p;
    int         len,diffLen;

    excludeCursor(gc);
    len = *(p = buf) = 0;
    while (*str) {
        switch (*str) {
            case '\f':
                GC_clrscr(gc);
                len = *(p = buf) = 0;       /* Discard buffer contents  */
                break;
            case '\r':
                outputBuf(gc,buf,len);
                len = *(p = buf) = 0;
                gc->cnt_x = 0;
                GC_gotoxy(gc,gc->cnt_x,gc->cnt_y);
                break;
            case '\n':
                outputBuf(gc,buf,len);
                len = *(p = buf) = 0;
                gc->cnt_x = 0;
                if (++gc->cnt_y >= gc->win_height) {
                    gc->cnt_y = gc->win_height-1;
                    GC_scroll(gc,GC_SCROLL_UP,1);
                    }
                GC_gotoxy(gc,gc->cnt_x,gc->cnt_y);
                break;
            case '\b':
                if (gc->cnt_x + --len < 0)
                    len = 0;
                else
                    --p;
                break;
            default:
                *p++ = *str;
                len++;
                if (gc->cnt_x + len >= gc->win_width) {
                    if (gc->line_wrap == GC_WORD_WRAP) {
                        diffLen = wordWrap(buf,len);
                        if (diffLen != len || gc->cnt_x != 0) {
                            p -= diffLen;
                            len -= diffLen;
                            outputBuf(gc,buf,len);
                            gc->cnt_x = 0;
                            if (++gc->cnt_y >= gc->win_height) {
                                gc->cnt_y = gc->win_height-1;
                                GC_scroll(gc,GC_SCROLL_UP,1);
                                }
                            while (diffLen > 0 && isspace(*p)) {
                                p++;
                                diffLen--;
                                }
                            if (diffLen) {
                                outputBuf(gc,p,diffLen);
                                }
                            len = *(p = buf) = 0;
                            gc->cnt_x = diffLen;
                            }
                        }
                    if (len != 0) {
                        outputBuf(gc,buf,len);
                        len = *(p = buf) = 0;
                        gc->cnt_x = 0;
                        if (gc->line_wrap != GC_NO_WRAP) {
                            if (++gc->cnt_y >= gc->win_height) {
                                gc->cnt_y = gc->win_height-1;
                                GC_scroll(gc,GC_SCROLL_UP,1);
                                }
                            }
                        }
                    GC_gotoxy(gc,gc->cnt_x,gc->cnt_y);
                    }
                break;
            }
        str++;
        }
    outputBuf(gc,buf,len);
    gc->cnt_x += len;
    GC_gotoxy(gc,gc->cnt_x,gc->cnt_y);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Function to print a single character to the console.

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context
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
GC_printf, GC_puts, GC_write, GC_writec
****************************************************************************/
void GCAPI GC_putc(
    GC_devCtx *gc,
    int c)
{
    excludeCursor(gc);
    switch (c) {
        case 0:
            break;                      /* Ignore ASCII NULL's          */
        case '\f':
            GC_clrscr(gc);
            break;
        case '\n':
            if (++gc->cnt_y >= gc->win_height) {
                gc->cnt_y = gc->win_height-1;
                GC_scroll(gc,GC_SCROLL_UP,1);
                }
            break;
        case '\r':
            gc->cnt_x = 0;
            break;
        case '\b':
            if (--gc->cnt_x < 0)
                gc->cnt_x = 0;
            break;
        default:
            writeChar(gc->cnt_x + gc->win_left,gc->cnt_y + gc->win_top,c,gc->attr);
            if (++gc->cnt_x >= gc->win_width) {
                gc->cnt_x = 0;
                if (gc->line_wrap) {
                    if (++gc->cnt_y >= gc->win_height) {
                        gc->cnt_y = gc->win_height-1;
                        GC_scroll(gc,GC_SCROLL_UP,1);
                        }
                    }
                }
            break;
        }
    GC_gotoxy(gc,gc->cnt_x,gc->cnt_y);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Clears from the current position to the end of the line.

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

REMARKS:
Clears from the current position to the end of the line.

SEE ALSO:
GC_clrscr, GC_delline, GC_insline
****************************************************************************/
void GCAPI GC_clreol(
    GC_devCtx *gc)
{
    GC_fillText(gc,gc->cnt_x,gc->cnt_y,gc->win_width-1,gc->cnt_y,gc->back_attr,gc->back_char);
}

/****************************************************************************
DESCRIPTION:
Clears the current window

HEADER:
gconsole.h

REMARKS:
Clears the window and moves the cursor to the top left hand corner of the
window (0,0). The window is cleared with the current background character
and in the current background attribute.

SEE ALSO:
GC_clreol, GC_delline, GC_insline
****************************************************************************/
void GCAPI GC_clrscr(
    GC_devCtx *gc)
{
    GC_fillText(gc,0,0,gc->win_width-1,gc->win_height-1,gc->back_attr,gc->back_char);
    GC_gotoxy(gc,0,0);
}

/****************************************************************************
DESCRIPTION:
Deletes the current line of text.

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

REMARKS:
Deletes the current line and moves all text below it up one line.

SEE ALSO:
GC_clrscr, GC_clreol, GC_insline
****************************************************************************/
void GCAPI GC_delline(
    GC_devCtx *gc)
{
    int     top;

    top = gc->win_top;
    gc->win_top += gc->cnt_y;
    GC_scroll(gc,GC_SCROLL_UP,1);
    gc->win_top = top;
}

/****************************************************************************
DESCRIPTION:
Inserts a new line at the current cursor position.

HEADER:
gconsole.h

REMARKS:
Inserts a new line at the current cursor position. Lines below the line
are moved down, and the bottom line is lost.

SEE ALSO:
GC_clrscr, GC_clreol, GC_delline
****************************************************************************/
void GCAPI GC_insline(
    GC_devCtx *gc)
{
    int     top;

    top = gc->win_top;
    gc->win_top += gc->cnt_y;
    GC_scroll(gc,GC_SCROLL_DOWN,1);
    gc->win_top = top;
}

/****************************************************************************
DESCRIPTION:
Function to print text to the console.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
x       - X coordinate to write string at
y       - Y coordinate to write string at
attr    - Color attribute to write string in
str     - String to write

REMARKS:
This function outputs a string to the text console. Formatting info is
/not/ handled (using GC_printf for that). The string is clipped at the
window boundary. This function is a /lot/ faster than GC_printf and
GC_puts since formatting information is not handled. The cursor is not
moved.

SEE ALSO:
GC_printf, GC_puts, GC_putc, GC_writec
****************************************************************************/
void GCAPI GC_write(
    GC_devCtx *gc,
    int x,
    int y,
    int attr,
    const char *str)
{
    int     len;

    if (y < 0 || y >= gc->win_height || x < 0)
        return;
    excludeCursor(gc);
    len = strlen(str);
    if (x + len - 1 >= gc->win_width)
        len = gc->win_width - x;
    if (len <= 0)
        return;
    writeStr(x + gc->win_left,y + gc->win_top,attr,str,len);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Function to print single character to the console.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
x       - X coordinate to write string at
y       - Y coordinate to write string at
attr    - Color attribute to write string in
c       - Character to write

REMARKS:
Displays a single character on the console. Formatting info is /not/ handled
(use GC_putc for that), and the character is clipped at the window boundary.
This function is a /lot/ faster than GC_putc and since formatting
information is not handled. The cursor is not moved.

SEE ALSO:
GC_printf, GC_puts, GC_putc, GC_write
****************************************************************************/
void GCAPI GC_writec(
    GC_devCtx *gc,
    int x,
    int y,
    int attr,
    int c)
{
    if (y < 0 || y >= gc->win_height || x < 0)
        return;
    if (x >= gc->win_width)
        return;
    excludeCursor(gc);
    writeChar(x + gc->win_left,y + gc->win_top,c,attr);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Moves a block of text to another location.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
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
GC_saveText, GC_restoreText, GC_scroll
****************************************************************************/
void GCAPI GC_moveText(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    int destleft,
    int desttop)
{
    excludeCursor(gc);
    moveText(gc,gc->win_left + left,gc->win_top + top,gc->win_left + right,
        gc->win_top + bottom,gc->win_left + destleft,gc->win_top + desttop);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Determine the amount of memory required to hold a save buffer

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
width   - Width of the save buffer in characters
height  - Height of the save buffer in characters

REMARKS:
This function should be used to determine the amount of memory needed to
save a portion of the console with the GC_saveText function.

SEE ALSO:
GC_saveText
****************************************************************************/
int GCAPI GC_bufSize(
    GC_devCtx *gc,
    int width,
    int height)
{
    (void)gc;
    return width * height * 2;
}

/****************************************************************************
DESCRIPTION:
Saves a block of text to a system memory buffer.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
left    - Left edge of screen region
top     - Top edge of screen region
right   - Right edge of screen region
bottom  - Bottom edge of screen region
dest    - Buffer to hold data

REMARKS:
This function saves a block of text from the console to a system memory
buffer. The buffer of text can be restored to the same or a different
location on the screen with the GC_restoreText function.

SEE ALSO:
GC_moveText, GC_restoreText, GC_scroll, GC_bufSize
****************************************************************************/
void GCAPI GC_saveText(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    void *dest)
{
    moveFromScreen(gc,gc->win_left + left,gc->win_top + top,gc->win_left + right,
        gc->win_top + bottom,dest);
}

/****************************************************************************
DESCRIPTION:
Restore a block of text from a system memory buffer.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
source      - Buffer holding data to use

REMARKS:
This function restores a block of text to the console from a system memory
buffer. The buffer of text can be saves from the same or a different
location on the screen with the GC_saveText function.

SEE ALSO:
GC_moveText, GC_saveText, GC_scroll, GC_bufSize
****************************************************************************/
void GCAPI GC_restoreText(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    void *source)
{
    excludeCursor(gc);
    moveToScreen(gc,gc->win_left + left,gc->win_top + top,gc->win_left + right,
        gc->win_top + bottom,source);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Fill a block of text on the console.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
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
GC_fillAttr
****************************************************************************/
void GCAPI GC_fillText(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    int attr,
    int ch)
{
    excludeCursor(gc);
    fillText(gc,gc->win_left + left,gc->win_top + top,gc->win_left + right,
        gc->win_top + bottom,attr,ch);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Change a block of text on the console to a specific color attribute.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
left        - Left edge of screen region
top         - Top edge of screen region
right       - Right edge of screen region
bottom      - Bottom edge of screen region
attr        - Attribute to fill with

REMARKS:
Fills a region of the screen with the specified color attribute. The
original text on the screen is not changed, only the color attributes.

SEE ALSO:
GC_fillText
****************************************************************************/
void GCAPI GC_fillAttr(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom,
    int attr)
{
    excludeCursor(gc);
    fillAttr(gc,gc->win_left + left,gc->win_top + top,gc->win_left + right,
        gc->win_top + bottom,attr);
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Restore a block of text from a system memory buffer.

HEADER:
gconsole.h

PARAMETERS:
direction   - Direction to scroll area in (CONSOLE_SCROLL_DIRECTIONS)
amt         - Amount to scroll area by (lines or characters)

REMARKS:
Scrolls the current window in the indicated direction by the indicated
amount, filling in the exposed area with the current background character
and color attribute.

SEE ALSO:
GC_moveText, GC_saveText, GC_restoreText
****************************************************************************/
void GCAPI GC_scroll(
    GC_devCtx *gc,
    int direction,
    int amt)
{
    excludeCursor(gc);
    switch (direction) {
        case GC_SCROLL_UP:
            if (amt <= 0 || amt >= gc->win_height) {
                GC_clrscr(gc);
                return;
                }
            moveText(gc,gc->win_left,gc->win_top + amt,gc->win_right,
                gc->win_bottom,gc->win_left,gc->win_top);
            fillText(gc,gc->win_left,gc->win_bottom - (amt-1),gc->win_right,
                gc->win_bottom,gc->back_attr,gc->back_char);
            break;
        case GC_SCROLL_DOWN:
            if (amt <= 0 || amt >= gc->win_height) {
                GC_clrscr(gc);
                return;
                }
            moveText(gc,gc->win_left,gc->win_top,gc->win_right,gc->win_bottom - amt,
                gc->win_left,gc->win_top + amt);
            fillText(gc,gc->win_left,gc->win_top,gc->win_right,gc->win_top+(amt-1),
                gc->back_attr,gc->back_char);
            break;
        case GC_SCROLL_RIGHT:
            if (amt <= 0 || amt > gc->win_width) {
                GC_clrscr(gc);
                return;
                }
            moveText(gc,gc->win_left,gc->win_top,gc->win_right - amt,gc->win_bottom,
                gc->win_left + amt,gc->win_top);
            fillText(gc,gc->win_left,gc->win_top,gc->win_left + (amt-1),gc->win_bottom,
                gc->back_attr,gc->back_char);
            break;
        case GC_SCROLL_LEFT:
            if (amt <= 0 || amt > gc->win_width) {
                GC_clrscr(gc);
                return;
                }
            moveText(gc,gc->win_left + amt,gc->win_top,gc->win_right,gc->win_bottom,
                gc->win_left,gc->win_top);
            fillText(gc,gc->win_right-(amt-1),gc->win_top,gc->win_right,gc->win_bottom,
                gc->back_attr,gc->back_char);
            break;
        }
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Set the text window for console output.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
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
GC_getWindow, GC_wherex, GC_wherey, GC_gotoxy, GC_maxx, GC_maxyy,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
void GCAPI GC_setWindow(
    GC_devCtx *gc,
    int left,
    int top,
    int right,
    int bottom)
{
    gc->win_left = MAX(left,0);
    gc->win_right = MIN(right,gc->screen_width-1);
    gc->win_top = MAX(top,0);
    gc->win_bottom = MIN(bottom,gc->screen_height-1);
    gc->win_width = gc->win_right - gc->win_left + 1;
    gc->win_height = gc->win_bottom - gc->win_top + 1;
    GC_gotoxy(gc,0,0);
}

/****************************************************************************
DESCRIPTION:
Set the text window for console output.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
left        - Place to store left edge of window
top         - Place to store top edge of window
right       - Place to store right edge of window
bottom      - Place to store bottom edge of window

REMARKS:
Returns the current window to the specified location on the screen.

SEE ALSO:
GC_setWindow, GC_wherex, GC_wherey, GC_gotoxy, GC_maxx, GC_maxyy,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
void GCAPI GC_getWindow(
    GC_devCtx *gc,
    int *left,
    int *top,
    int *right,
    int *bottom)
{
    *left = gc->win_left;
    *top = gc->win_top;
    *right = gc->win_right;
    *bottom = gc->win_bottom;
}

/****************************************************************************
DESCRIPTION:
Returns the current cursor X coordinate

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current cursor X coordinate

REMARKS:
Returns the current cursor X coordinate

SEE ALSO:
GC_setWindow, GC_getWindow, GC_wherey, GC_gotoxy, GC_maxx, GC_maxyy,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
int GCAPI GC_wherex(
    GC_devCtx *gc)
{
    return gc->cnt_x;
}

/****************************************************************************
DESCRIPTION:
Returns the current cursor Y coordinate

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current cursor Y coordinate

REMARKS:
Returns the current cursor Y coordinate

SEE ALSO:
GC_setWindow, GC_getWindow, GC_wherex, GC_gotoxy, GC_maxx, GC_maxyy,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
int GCAPI GC_wherey(
    GC_devCtx *gc)
{
    return gc->cnt_y;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium X coordinate for current window

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Maximum X coordinate for current window (inclusive)

REMARKS:
Returns the maxium X coordinate for current window

SEE ALSO:
GC_setWindow, GC_getWindow, GC_wherex, GC_wherey, GC_gotoxy, GC_maxyy,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
int GCAPI GC_maxx(
    GC_devCtx *gc)
{
    return gc->win_width - 1;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium Y coordinate for current window

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Maximum Y coordinate for current window (inclusive)

REMARKS:
Returns the maxium Y coordinate for current window

SEE ALSO:
GC_setWindow, GC_getWindow, GC_wherex, GC_wherey, GC_gotoxy, GC_maxx,
GC_getScreenWidth, GC_getScreenHeight
****************************************************************************/
int GCAPI GC_maxy(
    GC_devCtx *gc)
{
    return gc->win_height - 1;
}

/****************************************************************************
DESCRIPTION:
Returns the maxium Y coordinate for current window

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context
x   - New cursor X coordinate
y   - New cursor Y coordinate

REMARKS:
Moves the cursor location to (x,y) on the console. If the location lies
outside of the current window, the cursor is hidden by moving it off the
entire screen. Functions such as GC_puts, GC_printf and GC_putc all
output to the current cursor location.

SEE ALSO:
GC_setWindow, GC_getWindow, GC_wherex, GC_wherey, GC_maxx, GC_maxy
****************************************************************************/
void GCAPI GC_gotoxy(
    GC_devCtx *gc,
    int x,
    int y)
{
    int offset;

    excludeCursor(gc);
    if (x >= gc->win_width || y >= gc->win_height) {
        x = gc->screen_width;
        y = gc->screen_height;
        }
    if (!gc->is_graphics) {
        offset = (gc->win_top + y) * gc->screen_width + gc->win_left + x;
        GC_wrinx(0x3D4,0x0E,(uchar)((offset >> 8) & 0xFF));
        GC_wrinx(0x3D4,0x0F,(uchar)(offset & 0xFF));
        }
    gc->cnt_x = x;
    gc->cnt_y = y;
    unexcludeCursor(gc);
}

/****************************************************************************
DESCRIPTION:
Sets the current background character and attribute.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
attr    - New background attribute
ch      - New background character

REMARKS:
Sets the current background character and attribute. The current background
character and attribute are used to fill in any empty space created by
the GC_scroll, GC_delline and GC_clrscr functions.

SEE ALSO:
GC_getBackground, GC_setAttr, GC_setForeColor, GC_setBackColor
****************************************************************************/
void GCAPI GC_setBackground(
    GC_devCtx *gc,
    int attr,
    int ch)
{
    gc->back_attr = attr;
    gc->back_char = ch;
}

/****************************************************************************
DESCRIPTION:
Returns the current background character and attribute.

HEADER:
gconsole.h

PARAMETERS:
attr    - Place to store background attribute
ch      - Place to store background character

REMARKS:
Returns the current background character and attribute. The current background
character and attribute are used to fill in any empty space created by
the GC_scroll, GC_delline and GC_clrscr functions.

SEE ALSO:
GC_setBackground, GC_setAttr, GC_setForeColor, GC_setBackColor
****************************************************************************/
void GCAPI GC_getBackground(
    GC_devCtx *gc,
    int *attr,
    int *ch)
{
    *attr = gc->back_attr;
    *ch = gc->back_char;
}

/****************************************************************************
DESCRIPTION:
Sets the current text color attribute.

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
attr    - New text color attribute

REMARKS:
Sets the current text color attribute. The current text color
attribute is used to display all text on the screen for all text output
functions. The text attribute consists of both the foreground and
background colors used by the text characters.

You can use the GC_makeAttr macro to create a text attribute from separate
foreground and background colors.

SEE ALSO:
GC_setBackground, GC_getBackground, GC_setForeColor, GC_setBackColor,
GC_makeAttr
****************************************************************************/
void GCAPI GC_setAttr(
    GC_devCtx *gc,
    int attr)
{
    gc->attr = attr;
}

/****************************************************************************
DESCRIPTION:
Returns the current text color attribute.

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current text color attribute

REMARKS:
Returns the current text color attribute. The current text color
attribute is used to display all text on the screen for all text output
functions. The text attribute consists of both the foreground and
background colors used by the text characters.

SEE ALSO:
GC_setBackground, GC_getBackground, GC_getAttr, GC_setForeColor,
GC_setBackColor
****************************************************************************/
int GCAPI GC_getAttr(
    GC_devCtx *gc)
{
    return gc->attr;
}

/****************************************************************************
DESCRIPTION:
Sets the current text background color.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
newcolor    - New background text color (CONSOLE_COLORS)

REMARKS:
Sets the background color for the current text attribute.

SEE ALSO:
GC_setBackground, GC_getBackground, GC_setAttr, GC_setForeColor,
****************************************************************************/
void GCAPI GC_setBackColor(
    GC_devCtx *gc,
    int newcolor)
{
    gc->attr = (gc->attr & 0x0F) | ((newcolor & 0x0F) << 4);
}

/****************************************************************************
DESCRIPTION:
Sets the current text foreground color.

HEADER:
gconsole.h

PARAMETERS:
gc          - Console device context
newcolor    - New foreground text color (CONSOLE_COLORS)

REMARKS:
Sets the foreground color for the current text attribute.

SEE ALSO:
GC_setBackground, GC_getBackground, GC_setAttr, GC_setBackColor,
****************************************************************************/
void GCAPI GC_setForeColor(
    GC_devCtx *gc,
    int newcolor)
{
    gc->attr = (gc->attr & 0xF0) | (newcolor & 0x0F);
}

/****************************************************************************
DESCRIPTION:
Returns the current full text screen width

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current full text screen width

REMARKS:
Returns the current full text screen width

SEE ALSO:
GC_getScreenHeight
****************************************************************************/
int GCAPI GC_screenWidth(
    GC_devCtx *gc)
{
    return gc->screen_width;
}

/****************************************************************************
DESCRIPTION:
Returns the current full text screen height

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current full text screen height

REMARKS:
Returns the current full text screen height

SEE ALSO:
GC_getScreenHeight
****************************************************************************/
int GCAPI GC_screenHeight(
    GC_devCtx *gc)
{
    return gc->screen_height;
}

/****************************************************************************
DESCRIPTION:
Sets the text mode cursor to type

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
type    - New cursor type to set (CONSOLE_CURSOR_TYPES)

REMARKS:
Sets the text mode cursor type.

SEE ALSO:
GC_cursorOff, GC_getCursor, GC_restoreCursor
****************************************************************************/
void GCAPI GC_setCursor(
    GC_devCtx *gc,
    int type)
{
    int scans = 0;

    switch (type) {
        case GC_CURSOR_NORMAL:
            scans = ((gc->cell_height-2) << 8) | (gc->cell_height-1);
            break;
        case GC_CURSOR_FULL:
            scans = gc->cell_height-1;
            break;
        }
    if (gc->cell_height >= 16)
        scans -= 0x101;
    GC_restoreCursor(gc,scans);
}

/****************************************************************************
DESCRIPTION:
Hides the text cursor.

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

REMARKS:
Hides the text cursor.

SEE ALSO:
GC_setCursor, GC_getCursor, GC_restoreCursor
****************************************************************************/
void GCAPI GC_cursorOff(
    GC_devCtx *gc)
{
    GC_restoreCursor(gc,0x2000);
}

/****************************************************************************
DESCRIPTION:
Returns the current text cursor

HEADER:
gconsole.h

PARAMETERS:
gc  - Console device context

RETURNS:
Current text cursor cursor

REMARKS:
Returns the current text cursor. The high byte contains the top
scanline and the low byte contains the bottom scanline.

SEE ALSO:
GC_setCursor, GC_cursorOff, GC_restoreCursor
****************************************************************************/
int GCAPI GC_getCursor(
    GC_devCtx *gc)
{
    if (gc->is_graphics)
        return gc->cursor_scans;
    else
        return ((GC_rdinx(0x3D4,0x0A) << 8)-1) | GC_rdinx(0x3D4,0x0B);
}

/****************************************************************************
DESCRIPTION:
Restores a previous cursor setting

HEADER:
gconsole.h

PARAMETERS:
gc      - Console device context
scans   - Cursor scan lines

REMARKS:
Restores a previously saved cursor value. The high byte contains the top
scanline and the low byte contains the bottom scanline.

SEE ALSO:
GC_setCursor, GC_cursorOff, GC_getCursor
****************************************************************************/
void GCAPI GC_restoreCursor(
    GC_devCtx *gc,
    int scans)
{
    excludeCursor(gc);
    if (gc->is_graphics) {
        gc->cursor_scans = scans;
        }
    else {
        GC_wrinx(0x3D4,0x0A,(uchar)((scans >> 8) & 0xFF));
        GC_wrinx(0x3D4,0x0B,(uchar)(scans & 0xFF));
        }
    unexcludeCursor(gc);
}

/* C comment documentation for the console library macros */

#if 0
/****************************************************************************
DESCRIPTION:
Make a color attribute from a foreground and background color pair.

HEADER:
gconsole.h

PARAMETERS:
f   - Foreground color (CONSOLE_COLORS)
b   - Background color (CONSOLE_COLORS)

RETURNS:
New text color attribute

REMARKS:
This macro creates a color attribute from a foreground and background
color pair.
****************************************************************************/
uchar GC_makeAttr(
    uchar f,
    uchar b);
/* Macro definition */
#endif

