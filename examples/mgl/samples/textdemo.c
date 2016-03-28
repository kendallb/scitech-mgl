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
* Description:  Demo showing how to use the text functions to draw text to
*               the screen.
*
****************************************************************************/

#include "mglsamp.h"
#include <string.h>
#include <stdlib.h>

char *FontLibname[] = {
    "char.fon",
    "charb.fon",
    "charbi.fon",
    "chari.fon",
    "cour.fon",
    "courb.fon",
    "courbi.fon",
    "couri.fon",
    "helv.fon",
    "helvb.fon",
    "helvbi.fon",
    "helvi.fon",
    "lucb.fon",
    "lucbb.fon",
    "lucbbi.fon",
    "lucbi.fon",
    "lucs.fon",
    "lucsb.fon",
    "lucsbi.fon",
    "lucsi.fon",
    "luct.fon",
    "luctb.fon",
    "ncen.fon",
    "ncenb.fon",
    "ncenbi.fon",
    "nceni.fon",
    "symbol.fon",
    "tms.fon",
    "tmsb.fon",
    "tmsbi.fon",
    "tmsi.fon",
    };
#define NUM_FontLibS   (sizeof(FontLibname) / sizeof(FontLibname[0]))

char *fontFilename[] = {
    "pc8x8.fnt",
    "fix14x26.fnt",
    "helv38.fnt",
    "astrolog.fnt",
    "romanc.fnt",
    "romancs.fnt",
    "romand.fnt",
    "romans.fnt",
    "romant.fnt",
    };
#define NUM_FONTS   (sizeof(fontFilename) / sizeof(fontFilename[0]))

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "textdemo";

/*------------------------------ Implementation ---------------------------*/

ibool textDump(MGLDC *dc)
/****************************************************************************
*
* Function:     textDump
* Parameters:   dc  - Device context
*
* Description:  Displays all the characters in each of the fonts in
*               the distribution.
*
****************************************************************************/
{
    int         i,j,maxx;
    font_t      *font;
    char        buf[80];

    maxx = MGL_maxx();

    for (i = 0; i < NUM_FONTS; i++) {   /* For each available font      */
        /* Attempt to load the font file from disk. If it cannot be
         * found, then go onto the next one
         */
        strcpy(buf,fontFilename[i]);
        font = MGL_loadFont(buf);
        err = MGL_result();
        if (err == grFontNotFound)
            continue;
        if ((err = MGL_result()) != grOK) {
            MGL_exit();
            exit(1);
            }
        if ((font->fontType & MGL_FONTTYPEMASK) == MGL_VECTORFONT)
            sprintf(buf,"Vector Font: '%s'", font->name);
        else
            sprintf(buf,"Bitmap Font: '%s' - %d Point", font->name, font->pointSize);
        mainWindow(dc,buf);

        /* Display all characters on the screen */
        MGL_useFont(font);
        buf[1] = '\0';
        for (j = ' '; j < 256; j++) {
            buf[0] = j;
            MGL_drawStr(buf);
            if (MGL_getX() + MGL_maxCharWidth() > maxx) {
                MGL_moveToCoord(0,MGL_getY() + MGL_textHeight());
                }
            }

        /* Unload the font from memory */
        MGL_unloadFont(font);
        MGL_useFont(defFont);
        if (!pause())
            return false;
        }

        defaultAttributes(dc);
    return true;
}

ibool textLibDump(MGLDC *dc)
/****************************************************************************
*
* Function:     textDump
* Parameters:   dc  - Device context
*
* Description:  Displays all the characters in each of the fonts in
*               the distribution.
*
****************************************************************************/
{
    int         i,j,maxx,pointSize = 18;
    font_t      *font;
    font_lib_t  *fontLib;
    char        buf[80];

    mainWindow(dc,"Loading font...");
    maxx = MGL_maxx();
    for (i = 0; i < NUM_FontLibS; i++) {   /* For each available font      */
        /* Attempt to load the font file from disk. If it cannot be
         * found, then go onto the next one.
         */
        strcpy(buf,FontLibname[i]);
        if ((fontLib = MGL_openFontLib(buf)) == NULL)
            continue;
        if ((font = MGL_loadFontInstance(fontLib,pointSize,0,0,false)) == NULL) {
            MGL_closeFontLib(fontLib);
            continue;
            }
        switch (fontLib->fontLibType) {
            case MGL_BITMAPFONT_LIB:
                sprintf(buf,"Bitmap Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            case MGL_TRUETYPEFONT_LIB:
                sprintf(buf,"TrueType Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            case MGL_TYPE1FONT_LIB:
                sprintf(buf,"Type1 Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            }
        mainWindow(dc,buf);

        /* Display all characters on the screen */
        MGL_useFont(font);
        buf[1] = '\0';
        for (j = ' '; j < 256; j++) {
            buf[0] = j;
            MGL_drawStr(buf);
            if (MGL_getX() + MGL_maxCharWidth() > maxx) {
                MGL_moveToCoord(0,MGL_getY() + MGL_textHeight());
                }
            }

        /* Unload the font from memory */
        MGL_useFont(defFont);
        MGL_unloadFont(font);
        MGL_closeFontLib(fontLib);
        if (!pause())
            return false;
        mainWindow(dc,"Loading font...");
        }
    defaultAttributes(dc);
    return true;
}

ibool textDir(MGLDC *dc)
/****************************************************************************
*
* Function:     textDir
* Parameters:   dc  - Device context
*
* Description:  Display text in varying directions on the screen.
*
****************************************************************************/
{
    int         maxx,maxy;
    int         horiz,vert;
    char        *str;
    font_t      *font = NULL;
    font_lib_t  *fontLib;
    char        buf[80];

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    strcpy(buf,FontLibname[0]);
    if ((fontLib = MGL_openFontLib(buf)) == NULL)
        return FALSE;
    if ((font = MGL_loadFontInstance(fontLib,40,0,0,false)) == NULL) {
        MGL_closeFontLib(fontLib);
        return FALSE;
        }

    sprintf(buf,"Character set (%s)", font->name);
    mainWindow(dc,buf);

    MGL_useFont(font);
    for (horiz = MGL_LEFT_TEXT; horiz <= MGL_RIGHT_TEXT; horiz++)
        for (vert = MGL_TOP_TEXT; vert <= MGL_BASELINE_TEXT; vert++) {
            MGL_clearDevice();
            MGL_setColor(MGL_RED);
            MGL_lineCoord(0,maxy/2,maxx,maxy/2);
            MGL_lineCoord(maxx/2,0,maxx/2,maxy);
            MGL_setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
            MGL_setTextDirection(MGL_RIGHT_DIR);
            MGL_setColor(MGL_GREEN);
            MGL_moveToCoord(0,0);
            MGL_drawStr("horiz: ");
            switch (horiz) {
                case MGL_LEFT_TEXT:
                    str = "LEFT";
                    break;
                case MGL_CENTER_TEXT:
                    str = "CENTER";
                    break;
                case MGL_RIGHT_TEXT:
                    str = "RIGHT";
                    break;
                default:
                    str = "UNKNOWN";
                }
            MGL_drawStr(str);
            MGL_moveToCoord(0,MGL_textHeight());
            MGL_drawStr("vert:  ");
            switch (vert) {
                case MGL_TOP_TEXT:
                    str = "TOP";
                    break;
                case MGL_CENTER_TEXT:
                    str = "CENTER";
                    break;
                case MGL_BOTTOM_TEXT:
                    str = "BOTTOM";
                    break;
                case MGL_BASELINE_TEXT:
                    str = "BASELINE";
                    break;
                default:
                    str = "UNKNOWN";
                }
            MGL_drawStr(str);
            MGL_makeCurrentDC(dc);

            MGL_setTextJustify(horiz,vert);
            MGL_useFont(font);

            MGL_setColor(MGL_BLUE);
            MGL_lineCoord(0,maxy/2+MGL_textHeight()-1,maxx,maxy/2+MGL_textHeight()-1);
            MGL_lineCoord(maxx/2+MGL_textHeight()-1,0,maxx/2+MGL_textHeight()-1,maxy);

            MGL_setColor(MGL_WHITE);
            MGL_setTextDirection(MGL_LEFT_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes left");

            MGL_setTextDirection(MGL_DOWN_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes down");

            MGL_setTextDirection(MGL_UP_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes up");

            MGL_setTextDirection(MGL_RIGHT_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes right");
            if (!pause())
                break;
            }

    /* Unload the font from memory */
    MGL_useFont(defFont);
    MGL_unloadFont(font);
    MGL_closeFontLib(fontLib);
    defaultAttributes(dc);
    return true;
}

void demo(MGLDC *dc)
{
    if (!textDump(dc)) return;
    if (!textLibDump(dc)) return;
    if (!textDir(dc)) return;
}
