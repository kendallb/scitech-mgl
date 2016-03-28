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
* Environment:  Unix / X11
*
* Description:  Internal header file for the MGLX bindings for the MGL.
*
****************************************************************************/

#ifndef __MGLX_INTERNAL_H
#define __MGLX_INTERNAL_H

/*---------------------- Macro and type definitions -----------------------*/

#ifndef  MGLX11
#define  MGLX11
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>


/*--------------------------- Global Variables ----------------------------*/

/* The X11 Display */
extern Display *globalDisplay;

extern int                      _VARAPI _MGL_deskX;             /* Desktop resolution                           */
extern int                      _VARAPI _MGL_deskY;
extern int                      _VARAPI _MGL_mx;                /* Mouse coordinates                            */
extern int                      _VARAPI _MGL_my;
extern int                      _VARAPI _MGL_xRes;              /* Video mode resolution                        */
extern int                      _VARAPI _MGL_yRes;
/*------------------------- Function Prototypes ---------------------------*/

void _EVT_init(MGLDC *);
void _EVT_suspend(void);
void _EVT_resume(void);

void MGLAPI _MGL_delay(int milliseconds);

#endif  /* __MGLX_INTERNAL_H */
