/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Language:     C++ 3.0
* Environment:  Any
*
* Description:  Implementation for the miscellaneous functions used by
*               the library.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mprogram.hpp"
#include "pmapi.h"
#include <string.h>
#include <ctype.h>

/*---------------------------- Global Variables ---------------------------*/

// Global mouse cursor resources (loaded at program startup)

MGLCursor   *MV_ARROW_CURSOR = NULL;
MGLCursor   *MV_WAIT_CURSOR;
MGLCursor   *MV_IBEAM_CURSOR;
MGLCursor   *MV_MOVE_CURSOR;
MGLCursor   *MV_SIZENESW_CURSOR;
MGLCursor   *MV_SIZENS_CURSOR;
MGLCursor   *MV_SIZENWSE_CURSOR;
MGLCursor   *MV_SIZEWE_CURSOR;
MGLCursor   *MV_currentCursor;
MGLIcon     *MV_ICON_WARNING;
MGLIcon     *MV_ICON_ERROR;
MGLIcon     *MV_ICON_INFORMATION;
MGLIcon     *MV_ICON_CONFIRM;
font_t      *MV_SYS_GLYPHS = NULL;
bitmap_t    *MV_SYS_BITMAPS = NULL;
MVRect      *MV_SYS_GLYPH_RECTS = NULL;
MVRect      *MV_SYS_BITMAP_RECTS = NULL;
uint        scWinDarkGray       = 8;
uint        scWinLightRed       = 9;
uint        scWinLightGreen     = 10;
uint        scWinLightYellow    = 11;
uint        scWinLightBlue      = 12;
uint        scWinLightMagenta   = 13;
uint        scWinLightCyan      = 14;
uint        scWinWhite          = 15;
static int  *sysColors;

// Normal system color palette RGB values
static palette_t normalPalette[] = {
    {0x00,0x00,0x00,0x00},          // scBlack
    {0xFF,0xFF,0xFF,0x00},          // scWhite
    {0xC0,0xC0,0xC0,0x00},          // scLightGray
    {0x80,0x80,0x80,0x00},          // scDarkGray
    {0x80,0x00,0x00,0x00},          // scBlue
    {0x80,0x80,0x00,0x00},          // scJade
    {0x80,0xFF,0xFF,0x00},          // scYellow
    {0xCC,0xFF,0xFF,0x00},          // scLightYellow
    };

#define NUM_NORMPAL (sizeof(normalPalette)/sizeof(palette_t))

// Windows system color palette RGB values
static palette_t winPalette[] = {
    {0x00,0x00,0x00,0x00},          // scWinBlack
    {0x00,0x00,0x80,0x00},          // scWinDarkRed
    {0x00,0x80,0x00,0x00},          // scWinDarkGreen
    {0x00,0x80,0x80,0x00},          // scWinDarkYellow
    {0x80,0x00,0x00,0x00},          // scWinDarkBlue
    {0x80,0x00,0x80,0x00},          // scWinDarkMagenta
    {0x80,0x80,0x00,0x00},          // scWinDarkCyan
    {0xC0,0xC0,0xC0,0x00},          // scWinLightGray
    {0x80,0x80,0x80,0x00},          // scWinDarkGray
    {0x00,0x00,0xFF,0x00},          // scWinLightRed
    {0x00,0xFF,0x00,0x00},          // scWinLightGreen
    {0x00,0xFF,0xFF,0x00},          // scWinLightYellow
    {0xFF,0x00,0x00,0x00},          // scWinLightBlue
    {0xFF,0x00,0xFF,0x00},          // scWinLightMagenta
    {0xFF,0xFF,0x00,0x00},          // scWinLightCyan
    {0xFF,0xFF,0xFF,0x00},          // scWinWhite
    };

#define NUM_WINPAL (sizeof(winPalette)/sizeof(palette_t))

// System color mappings for normal color palette
static int normalSysColors[scMaxSystemColor] = {
    scBlack,                        // Invalid system color index
    scBlack,                        // Desktop pattern background color
    scJade,                         // Desktop pattern foregound color
    scLightGray,                    // Menu interior color
    scBlue,                         // Menu selected interior color
    scBlack,                        // Menu normal text
    scDarkGray,                     // Menu disabled text
    scWhite,                        // Menu selected text
    scWhite,                        // System highlight color
    scLightGray,                    // System medium light color
    scDarkGray,                     // System shadow color
    scBlack,                        // System dark shadow color
    scBlack,                        // System focus rectangle color
    scLightGray,                    // Window frame color
    scLightGray,                    // Window background color
    scWhite,                        // Window client background color
    scBlue,                         // Active title bar background
    scDarkGray,                     // Inactive title bar background
    scWhite,                        // Active title bar text
    scWhite,                        // Inactive title bar text
    scBlack,                        // Title bar icon colors
    scLightGray,                    // Scroll bar interior color
    scBlack,                        // Active scroll bar arrow color
    scWhite,                        // Inactive scroll bar arrow color
    scDarkGray,                     // Unselected scroll interior color
    scBlack,                        // Selected scroll bar interior color
    scLightGray,                    // Normal button face color
    scLightGray,                    // Pressed button face color
    scBlack,                        // Normal button text color
    scBlack,                        // Focused button text color
    scDarkGray,                     // Disabled button text color
    scBlack,                        // Default button border color
    scBlack,                        // Default dialog static text color
    scWhite,                        // Input line interior color
    scBlack,                        // Input line normal text color
    scWhite,                        // Input line selected text color
    scBlue,                         // Input line selection color
    scBlack,                        // Input line cursor color
    scYellow,                       // Input line cursor color (selected)
    scWhite,                        // List box interior color
    scBlack,                        // List box normal cell color
    scWhite,                        // List box selected cell color
    scBlue,                         // List box highlighted cell color
    scYellow,                       // List box cursor color
    };                              // Maximum system color index+1

// System color mappings for Windows style system color palette
static int winSysColors[scMaxSystemColor] = {
    scWinBlack,                     // Invalid system color index
    scWinBlack,                     // Desktop pattern background color
    scWinDarkCyan,                  // Desktop pattern foregound color
    scWinLightGray,                 // Menu interior color
    scWinDarkBlue,                  // Menu selected interior color
    scWinBlack,                     // Menu normal text
    scWinDarkGray,                  // Menu disabled text
    scWinWhite,                     // Menu selected text
    scWinWhite,                     // System highlight color
    scWinLightGray,                 // System medium light color
    scWinDarkGray,                  // System shadow color
    scWinBlack,                     // System dark shadow color
    scWinBlack,                     // System focus rectangle color
    scWinLightGray,                 // Window frame color
    scWinLightGray,                 // Window background color
    scWinWhite,                     // Window client background color
    scWinDarkBlue,                  // Active title bar background
    scWinDarkGray,                  // Inactive title bar background
    scWinWhite,                     // Active title bar text
    scWinWhite,                     // Inactive title bar text
    scWinBlack,                     // Title bar icon colors
    scWinLightGray,                 // Scroll bar interior color
    scWinBlack,                     // Active scroll bar arrow color
    scWinWhite,                     // Inactive scroll bar arrow color
    scWinDarkGray,                  // Unselected scroll interior color
    scWinBlack,                     // Selected scroll bar interior color
    scWinLightGray,                 // Normal button face color
    scWinLightGray,                 // Pressed button face color
    scWinBlack,                     // Normal button text color
    scWinBlack,                     // Focused button text color
    scWinDarkGray,                  // Disabled button text color
    scWinBlack,                     // Default button border color
    scWinBlack,                     // Default dialog static text color
    scWinWhite,                     // Input line interior color
    scWinBlack,                     // Input line normal text color
    scWinWhite,                     // Input line selected text color
    scWinDarkBlue,                  // Input line selection color
    scWinBlack,                     // Input line cursor color
    scWinLightYellow,               // Input line cursor color (selected)
    scWinWhite,                     // List box interior color
    scWinBlack,                     // List box normal cell color
    scWinWhite,                     // List box selected cell color
    scWinDarkBlue,                  // List box highlighted cell color
    scWinLightYellow,               // List box cursor color
    };

// Place to find MGL system bitmaps
#ifdef __UNIX__
#define MGL_BITMAPS "bitmaps/"
#else
#define MGL_BITMAPS "bitmaps\\"
#endif

/*----------------------------- Implementation ----------------------------*/

static MVRect *LoadSysRects(const char *filename,int max)
/****************************************************************************
*
* Function:     LoadSysRects
* Parameters:   filename    - Name of rectangle list to load
*               max         - Maximum number of rectangles to load
*
* Description:  Loads a set of rectangle definitions from the specified
*               file for system bitmaps and glyphs.
*
****************************************************************************/
{
    int     i,v,left,top,right,bottom;
    char    buf[PM_MAX_PATH],validpath[PM_MAX_PATH];
    FILE    *f;
    MVRect  *r;
    size_t  fileSize;
    char    *data,*ptr;

    // Try to open the system rectangle list
    sprintf(buf,"%s.dat",filename);
    _MGL_findFile(validpath,MGL_BITMAPS,buf,"r");
    if ((f = MGL_fopen(validpath,"r")) == NULL) {
        sprintf(validpath,"Unable to open system rectangle list (%s)!", buf);
        MGL_fatalError(validpath);
        }
    if ((r = new MVRect[max]) == NULL) {
        MGL_fclose(f);
        MGL_fatalError("No memory to open system rectangle list!");
        }

    // Read in the entire file and append a \0 to guarantee string terminates
    MGL_fseek(f, 0, SEEK_END);
    fileSize = MGL_ftell(f);
    if ((data = new char[fileSize + 1]) == NULL) {
        MGL_fclose(f);
        delete r;
        MGL_fatalError("No memory to read system rectangle list!");
        }
    MGL_fseek(f, 0, SEEK_SET);
    MGL_fread(data, 1, fileSize, f);
    MGL_fclose(f);
    data[fileSize] = 0;

    // Now parse the system rectangle list from the loaded data file
    ptr = data;
    for (i = 0; i < max; i++) {
        if ((v = sscanf(ptr,"%d %d %d %d", &left,&top,&right,&bottom)) != 4) {
            sprintf(buf,"Too few rectangles in system rectangle list (%d) line %d!", v, i+1);
            MGL_fatalError(buf);
            }
        // Bump pointer to end of line or file
        while (*ptr && *ptr != 0x0A) ptr++;
        if (*ptr) ptr++;
        r[i].left() = left;
        r[i].top() = top;
        r[i].right() = right;
        r[i].bottom() = bottom;
        }
    delete [] data;
    return r;
}

void MV_loadSystemResources(MGLDevCtx& dc,int systemFont,int paletteType)
/****************************************************************************
*
* Function:     MV_loadSystemResources
* Parameters:   dc          - Display device context used for the MegaVision
*               systemFont  - Default system font family to use
*               paletteType - Default palette type to use
*
* Description:  Load all of the system resources used by the MegaVision
*               such as default system cursors, font, palette and bitmaps.
*
****************************************************************************/
{
    if (!MV_ARROW_CURSOR) {
        MV_ARROW_CURSOR     = new MGLCursor(MGL_DEF_CURSOR);
        MV_WAIT_CURSOR      = new MGLCursor("wait.cur");
        MV_IBEAM_CURSOR     = new MGLCursor("ibeam.cur");
        MV_MOVE_CURSOR      = new MGLCursor("move.cur");
        MV_SIZENESW_CURSOR  = new MGLCursor("sizenesw.cur");
        MV_SIZENS_CURSOR    = new MGLCursor("sizens.cur");
        MV_SIZENWSE_CURSOR  = new MGLCursor("sizenwse.cur");
        MV_SIZEWE_CURSOR    = new MGLCursor("sizewe.cur");
        MV_ICON_WARNING     = new MGLIcon("warning.ico");
        MV_ICON_ERROR       = new MGLIcon("error.ico");
        MV_ICON_INFORMATION = new MGLIcon("inform.ico");
        MV_ICON_CONFIRM     = new MGLIcon("confirm.ico");
        }
    MV_currentCursor = MV_ARROW_CURSOR;

    // Set the default system palette and color mappings
    if (paletteType == (int)ptNormalPalette) {
        // Use the default MegaVision palette colors
        dc.setPalette(normalPalette,NUM_NORMPAL,0);
        dc.realizePalette(NUM_NORMPAL,0,false);
        sysColors = normalSysColors;
        MS_setCursorColor(dc.realColor(scWhite));
        }
    else {
        // Use the standard Windows palette colors. For 16 color modes
        // we use all the palette entries, but for 256 color and higher
        // modes we use the bottom 8 and top 8 palette entries (Windows
        // actually uses the bottom 10 and top 10 entries, but we can survive
        // with 16 colors and this make us compatible with Windows bitmaps.
        if (dc.getBitsPerPixel() >= 8) {
            palette_t   pal[256];

            // Load the halftone palette and set the first and last 8
            // entries to our Window system palette colors.
            MGL_getHalfTonePalette(pal);
            memcpy(&pal[0],&winPalette[0],8 * sizeof(palette_t));
            memcpy(&pal[248],&winPalette[8],8 * sizeof(palette_t));
            dc.setPalette(pal,256,0);
            dc.realizePalette(256,0,false);

            // Now remap the palette indices to point to the top 8 entries
            scWinDarkGray       = 248;
            scWinLightRed       = 249;
            scWinLightGreen     = 250;
            scWinLightYellow    = 251;
            scWinLightBlue      = 252;
            scWinLightMagenta   = 253;
            scWinLightCyan      = 254;
            scWinWhite          = 255;
            for (int i = 0; i < scMaxSystemColor; i++) {
                if (winSysColors[i] >= 8 && winSysColors[i] < 16)
                    winSysColors[i] += 240;
                }
            }
        else {
            dc.setPalette(winPalette,NUM_WINPAL,0);
            }
        dc.realizePalette(dc.getPaletteSize(),0,false);
        sysColors = winSysColors;
        MS_setCursorColor(dc.realColor(scWinWhite));
        }

    // Load the default system font depending on the video mode that has
    // been selected. We always use fonts from the same family, but different
    // sizes for different resolutions. We also load the appropriate sized
    // system bitmaps and glyphs for the user interface components.
    int             maxy = dc.sizey()+1;
    int             maxx = dc.sizex()+1;
    MVFontRecord    fontRec(_MV_systemFont = systemFont);
    char            *bitmapName,*glyphName;
    char            buf[PM_MAX_PATH];

    if (maxx <= 400) {
        //320x200, 320x240, 320x400, 320x480, 400x300
        fontRec.sizey = 11;
        _MV_iconOffset = 6;
        bitmapName = "mvctrl0";
        glyphName = "mvglyph0";
        _MV_sysScrollBarWidth = 16;
        }
    else if (maxy <= 350) {
        // 640x350
        fontRec.sizey = 13;
        _MV_iconOffset = 10;
        bitmapName = "mvctrl1";
        glyphName = "mvglyph1";
        _MV_sysScrollBarWidth = 16;
        }
    else if (maxy <= 400) {
        // 512x384, 640x400
        fontRec.sizey = 13;
        _MV_iconOffset = 10;
        bitmapName = "mvctrl1";
        glyphName = "mvglyph1";
        _MV_sysScrollBarWidth = 16;
        }
    else if (maxy <= 480) {
        // 640x480
        fontRec.sizey = 15;
        _MV_iconOffset = 12;
        bitmapName = "mvctrl2";
        glyphName = "mvglyph2";
        _MV_sysScrollBarWidth = 16;
        }
    else if (maxy <= 600) {
        // 800x600
        fontRec.sizey = 15;
        _MV_iconOffset = 12;
        bitmapName = "mvctrl2";
        glyphName = "mvglyph2";
        _MV_sysScrollBarWidth = 16;
        }
    else if (maxy <= 768) {
        // 1024x768
        fontRec.sizey = 22;
        _MV_iconOffset = 12;
        bitmapName = "mvctrl2";
        glyphName = "mvglyph2";
        _MV_sysScrollBarWidth = 16;
        }
    else {
        // 1152x864, 1280x960, 1280x1024, 1600x1200
        fontRec.sizey = 29;
        _MV_iconOffset = 12;
        bitmapName = "mvctrl2";
        glyphName = "mvglyph2";
        _MV_sysScrollBarWidth = 16;
        }
    _MV_systemFontSize = fontRec.sizey;
    if (!MV_fontManager.setSystemFont(fontRec)) {
        char msg[80];
        sprintf(msg, "Unable to load system font: %s",
                MGL_errorMsg(dc.result()));
        MGL_fatalError(msg);
        }

    // Delete old system bitmaps if present
    MGL_unloadFont(MV_SYS_GLYPHS);
    MGL_unloadBitmap(MV_SYS_BITMAPS);
    delete MV_SYS_GLYPH_RECTS;
    delete MV_SYS_BITMAP_RECTS;

    // Load the system bitmaps and rectangle indexes
    sprintf(buf,"%s.bmp", bitmapName);
    if ((MV_SYS_BITMAPS = MGL_loadBitmap(buf,true)) == NULL)
        MGL_fatalError("Unable to load system bitmaps!\n");
    MV_SYS_BITMAP_RECTS = LoadSysRects(bitmapName,sbMaxSysBitmap);

    // Load the system glyphs and rectangle indexes
    sprintf(buf,"%s.fnt", glyphName);
    if ((MV_SYS_GLYPHS = MGL_loadFont(buf)) == NULL)
        MGL_fatalError("Unable to load system glyph font!\n");
    MV_SYS_GLYPH_RECTS = LoadSysRects(glyphName,sgMaxSysGlyph);
}

int MV_getSystemColor(int index)
/****************************************************************************
*
* Function:     MV_getSystemColor
* Parameters:   index   - System color index
* Returns:      Appropriate MGL color index value for this system color
*
* Description:  Looks up the appropriate system color from our color map
*               table and converts that to an MGL color palette index
*               (use MGL_realColor to get the actual MGL color value to
*               use).
*
****************************************************************************/
{
    if (index < 0 || index >= scMaxSystemColor)
        return scInvalid;
    return sysColors[index];
}

void MV_setSystemColor(int index,int color)
/****************************************************************************
*
* Function:     MV_getSystemColor
* Parameters:   index   - System color index
*               color   - New color index
*
****************************************************************************/
{
    if (index >= 0 || index < scMaxSystemColor)
        sysColors[index] = color;
}

char *MV_newStr(const char *str)
/****************************************************************************
*
* Function:     MV_newStr
* Parameters:   str - Pointer to string to copy
* Returns:      Pointer to the copied string on the heap
*
****************************************************************************/
{
    if (str) {
        char *temp = new char[strlen(str)+1];
        strcpy(temp,str);
        return temp;
        }
    else
        return NULL;
}

char *MV_newHotStr(const char *str,char& hotChar,uint& hotIndex)
/****************************************************************************
*
* Function:     MV_newHotStr
* Parameters:   str         - String to parse for hot character value
*               hotChar     - Place to store the hotCharacter
*               hotIndex    - Index of the hotCharacter in returned string
* Returns:      Pointer to copy of string with hot character stripped
*
* Description:  Finds the hot character value in a string and extracts
*               it if present, returning a copy of the string without the
*               hot character.
*
****************************************************************************/
{
    if (str == NULL) {
        hotChar = 0;
        return NULL;
        }

    char *p = (char*)strchr(str,'~');

    if (p == NULL) {
        hotChar = 0;
        hotIndex = 0;
        return MV_newStr(str);
        }
    else {
        CHECK(strchr(p+1,'~') == p+2 && strchr(p+3,'~') == NULL);
        char *temp = new char[strlen(str)-1];
        hotChar = p[1];
        p = temp;
        while (*str != '~')
            *p++ = *str++;
        hotIndex = p - temp;
        *p++ = hotChar;
        str += 3;
        while (*str != '\0')
            *p++ = *str++;
        *p = 0;
        hotChar = (char)tolower(hotChar);
        return temp;
        }
}

int MV_countLines(MGLDevCtx& dc,const char *msg,int maxWidth,
    int& maxTextWidth,ibool wrap)
/****************************************************************************
*
* Function:     MV_countLines
* Parameters:   msg             - Message string to count
*               maxWidth        - Maximum width of the string
*               maxTextWidth    - Place to return maximum width measured
*               wrap            - True if text should wrap at maximum width
* Returns:      Number of lines in the text string.
*
* Description:  Counts the number of lines in a text string, drawn in the
*               currently active font and wrapping at the specified
*               maximum width (if wrapping is true).
*
****************************************************************************/
{
    const char  *p = msg;
    int         width = 0;
    int         lines = 1;
    const char  *lastSpace = NULL;

    maxTextWidth = 0;
    while (*p) {
        if (*p == '\n') {
            width = 0;
            lines++;
            lastSpace = NULL;
            }
        else {
            if(*p == ' ')
                lastSpace = p;
            width += dc.charWidth(*p);
            if (wrap && lastSpace && width > maxWidth) {
                p = lastSpace;  // Back up to previous space character
                width = 0;
                lines++;
                lastSpace = NULL;
                }
            else if (width > maxTextWidth)
                maxTextWidth = width;
            }
        p++;
        }
    return lines;
}

void MV_beep(void)
/****************************************************************************
*
* Function:     MV_beep
*
* Description:  Beeps the speaker.
*
****************************************************************************/
{
    // TODO: We no longer have an MGL beep function!
}

void MV_putSysGlyph(int x,int y,int index)
/****************************************************************************
*
* Function:     MV_putSysGlyph
* Parameters:   dc      - Device context to put glyph on
*               x,y     - Location to display at
*               index   - Index of system glyph to display
*
* Description:  Blt's a system glyph from the system glyph device context
*               to the display given the specified bitmap index. Glyphs
*               are always monochrome bitmaps, while system bitmaps are
*               converted to the color format of the main display DC.
*
****************************************************************************/
{
    MGL_drawGlyph(MV_SYS_GLYPHS,x,y,(uchar)index);
}

void MV_getSysGlyphBounds(int index,MVRect& bounds)
/****************************************************************************
*
* Function:     MV_putSysGlyph
* Parameters:   dc      - Device context to put glyph on
*               x,y     - Location to display at
*               index   - Index of system glyph to display
*
* Description:  Blt's a system glyph from the system glyph device context
*               to the display given the specified bitmap index. Glyphs
*               are always monochrome bitmaps, while system bitmaps are
*               converted to the color format of the main display DC.
*
****************************************************************************/
{
    if (index < sgMaxSysGlyph)
        bounds = MV_SYS_GLYPH_RECTS[index];
}

void MV_putSysBitmap(MGLDC *dc,int x,int y,int index)
/****************************************************************************
*
* Function:     MV_putSysBitmap
* Parameters:   dc      - Device context to put bitmap on
*               x,y     - Location to display at
*               index   - Index of system bitmap to display
*
* Description:  Blt's a system bitmap from the system bitmap device context
*               to the display given the specified bitmap index.
*
****************************************************************************/
{
    if (index < sbMaxSysBitmap) {
        MGL_putBitmapSection(dc,
            MV_SYS_BITMAP_RECTS[index].left(),
            MV_SYS_BITMAP_RECTS[index].top(),
            MV_SYS_BITMAP_RECTS[index].right(),
            MV_SYS_BITMAP_RECTS[index].bottom(),
            x,y,MV_SYS_BITMAPS,MGL_REPLACE_MODE);
        }
}

void MV_getSysBitmapBounds(int index,MVRect& bounds)
/****************************************************************************
*
* Function:     MV_putSysBitmap
* Parameters:   dc      - Device context to put Bitmap on
*               x,y     - Location to display at
*               index   - Index of system Bitmap to display
*
* Description:  Blt's a system Bitmap from the system Bitmap device context
*               to the display given the specified bitmap index. Bitmaps
*               are always monochrome bitmaps, while system bitmaps are
*               converted to the color format of the main display DC.
*
****************************************************************************/
{
    if (index < sbMaxSysBitmap)
        bounds = MV_SYS_BITMAP_RECTS[index];
}

void MV_unloadSystemResources()
/****************************************************************************
*
* Function:     MV_unloadSystemResources
*
* Description:  Unloads system resources loaded with MV_loadSystemResources
*               and frees memory
*
* Written by: Edwin P. Berlin, Jr.
****************************************************************************/
{
    if (MV_ARROW_CURSOR) {
        delete MV_ARROW_CURSOR;
        delete MV_WAIT_CURSOR;
        delete MV_IBEAM_CURSOR;
        delete MV_MOVE_CURSOR;
        delete MV_SIZENESW_CURSOR;
        delete MV_SIZENS_CURSOR;
        delete MV_SIZENWSE_CURSOR;
        delete MV_SIZEWE_CURSOR;
        delete MV_ICON_WARNING;
        delete MV_ICON_ERROR;
        delete MV_ICON_INFORMATION;
        delete MV_ICON_CONFIRM;
        MV_ARROW_CURSOR = NULL;
        }

    MGL_unloadFont(MV_SYS_GLYPHS);
    MGL_unloadBitmap(MV_SYS_BITMAPS);
    delete MV_SYS_GLYPH_RECTS;
    delete MV_SYS_BITMAP_RECTS;

    MV_SYS_GLYPHS = NULL;
    MV_SYS_BITMAPS = NULL;
    MV_SYS_GLYPH_RECTS = NULL;
    MV_SYS_BITMAP_RECTS = NULL;
}
