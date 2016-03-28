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
* Description:  Common framework code for fullscreen sample programs.
*               This simple framework takes care of initialising the
*               MGL in any of the available graphics modes, and provides
*               a common front end for all of the MGL sample programs.
*
*
****************************************************************************/

#ifndef __MGLSAMP_H
#define __MGLSAMP_H

#ifndef __MGRAPH_H
#include "mgraph.h"
#endif

/*---------------------- Macros and type definitions ----------------------*/

#define ESC     0x1B

/* Macro to swap two integer values */

#define SWAP(a,b)   { a^=b; b^=a; a^=b; }

/*----------------------------- Global Variables --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

extern int              mode;
extern int              palsize;
extern int              aspect;
extern double           aspectRatio;
extern int              err;
extern color_t          maxcolor;
extern color_t          colorMask;
extern palette_t        *defPal;
extern text_settings_t  defaultTextSettings;
extern font_t           *defFont;
extern font_t           *largeFont;
extern rect_t           fullView;
extern rect_t           titleView;
extern rect_t           statusView;
extern rect_t           demoView;

/* Name of demo program, provided by the demo source code */

extern  char    demoName[];

/*------------------------- Function Prototypes ---------------------------*/

/* Utility functions */

int     waitEvent(void);
ibool   checkEvent(void);
color_t randomColor(void);
int     gprintf(char *fmt, ... );
void    drawBorder(void);
void    statusLine(char *msg);
ibool   pause(void);
void    defaultAttributes(MGLDC *dc);
void    mainWindow(MGLDC *dc,char *heading);

/* Function to run the demo. When this function is called, the MGL has
 * been initialized in the selected display mode. When this function
 * exits the MGL will be exited.
 */

void    demo(MGLDC *dc);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif  /* __cplusplus */

#endif  /* __MGLSAMP_H */
