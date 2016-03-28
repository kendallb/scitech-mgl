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
* Description:  Common header file for all Unix device drivers.
*
****************************************************************************/


#ifndef __DRIVERS_COMMON_GUNIX_H
#define __DRIVERS_COMMON_GUNIX_H

#include "drivers/common/common.h"

/*---------------------- Macro and type definitions -----------------------*/

/*------------------------- Function Prototypes ---------------------------*/

ibool MGLAPI  XWIN_detect(void);
void          XWIN_initInternal(MGLDC *dc);
void  MGLAPI  XWIN_useMode(modetab modes,int mode,int id,gmode_t *mi);
ulong MGLAPI XWIN_getWinDC(MGLDC *dc);
void * MGLAPI XWIN_getDefaultPalette(MGLDC *dc);
void   MGLAPI XWIN_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
void MGLAPI XWIN_putImage(MGLDC *dc,int left,int top,int right,
                           int bottom,int dstLeft,int dstTop,int op,void *surface,
                           int bytesPerLine,MGLDC *src);


#endif
