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
* Description:  Simple demo showing how to use the MGL_copyPage function to
*               copy data from one display page to another using the
*               hardware blitter if available.
*
****************************************************************************/

#include <string.h>
#include "mglsamp.h"

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "copypage";

/*------------------------------ Implementation ---------------------------*/

void demo(MGLDC *dc)
{
    /* Check that we have two display pages available */
    if (MGL_maxPage(dc) == 0)
        MGL_fatalError("This sample requires two display pages!");

    /* Display new title message and draw first display page */
    mainWindow(dc,"CopyPage Demonstration program (Page 1)");
    statusLine("Press any key to view second page, ESC to Abort");
    MGL_setColor(MGL_GREEN);
    MGL_fillRectCoord(0,0,MGL_sizex(dc),MGL_sizey(dc));
    MGL_setColor(MGL_WHITE);
    MGL_drawStrXY(4,4,"Page 1");

    /* Draw second display page while it is hidden */
    defaultAttributes(dc);
    MGL_setActivePage(dc,1);
    mainWindow(dc,"CopyPage Demonstration program (Page 2)");
    statusLine("Press any key to copy data from first page, ESC to Abort");
    MGL_setColor(MGL_CYAN);
    MGL_fillRectCoord(0,0,MGL_sizex(dc),MGL_sizey(dc));
    MGL_setColor(MGL_WHITE);
    MGL_drawStrXY(4,4,"Page 2");
    if (waitEvent() == ESC) return;

    /* Make the second display page visible */
    MGL_setVisualPage(dc,1,true);
    if (waitEvent() == ESC) return;

    /* Now copy data from first page to second page */
    MGL_copyPageCoord(dc,0,0,0,MGL_sizex(dc)/4,MGL_sizey(dc)/4,
        MGL_sizex(dc)/2,MGL_sizey(dc)/2,MGL_REPLACE_MODE);

    /* Wait for key press and exit */
    pause();
}

