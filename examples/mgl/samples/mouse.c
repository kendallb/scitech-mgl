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
* Description:  Simple demo showing how to use the MGL mouse functions.
*
****************************************************************************/

#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "mouse";

/*------------------------------ Implementation ---------------------------*/

ibool mouseCursorDemo(MGLDC *dc)
/****************************************************************************
*
* Function:     mouseCursorDemo
* Parameters:   dc  - Device context
*
* Description:  Display the mouse cursor on the screen, and change it to
*               a number of different styles.
*
****************************************************************************/
{
    cursor_t    *cursor;

    /* turn on the mouse */
    MS_show();

    statusLine("Press any key for Hour Glass cursor");
    waitEvent();

    if((cursor = MGL_loadCursor("wait.cur")) == NULL)
        MGL_fatalError("Unable to load WAIT.CUR cursor file");
    MS_setCursor(cursor);
    statusLine("Press any key for IBEAM cursor");
    waitEvent();

    MS_setCursor(MGL_DEF_CURSOR);
    MGL_unloadCursor(cursor);
    if((cursor = MGL_loadCursor("ibeam.cur")) == NULL)
        MGL_fatalError("Unable to load IBEAM.CUR cursor file");
    MS_setCursor(cursor);
    waitEvent();

    MS_setCursor(MGL_DEF_CURSOR);
    MGL_unloadCursor(cursor);

    if (!pause())
        return false;

    MS_hide();

    return true;
}

void demo(MGLDC *dc)
{
    /* Display title message at the top of the window. */
    mainWindow(dc,"Mouse Cursor Demonstration");

    /* set the background color to grey */
    if (MGL_getBitsPerPixel(dc) == 8) {
        MGL_setPaletteEntry(dc,1,100,100,100);
        MGL_realizePalette(dc,1,1,true);
        MGL_setBackColor(1);
        }
    else {
        MGL_setBackColor(MGL_packColor(&dc->pf,100,100,100));
        }

    MGL_clearViewport();

    if(!mouseCursorDemo(dc)) return;
}

