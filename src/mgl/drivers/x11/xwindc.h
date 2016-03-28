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
* Environment:  Unix/X11
*
* Description:  Header file for the windowed X11 driver.
*
****************************************************************************/

#ifndef __DRIVERS_UNIX_XWINDC_H
#define __DRIVERS_UNIX_XWINDC_H

#include "mglunix.h"
#include "mgldd.h"

#include "drivers/common/gunix.h"

typedef struct {
    // What the hell do I have to put here ?
    Display *dpy;
    Window  w;
    //  XImage  *img;
} XWINDC_data;

/*------------------------- Function Prototypes ---------------------------*/

/* 8 bit C based routines */

ibool   MGLAPI XWINDC_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
void   *MGLAPI XWINDC_createInstance(void);
void    MGLAPI XWINDC_destroyInstance(void *data);

#endif /* __DRIVERS_UNIX_XWIN8_H */
