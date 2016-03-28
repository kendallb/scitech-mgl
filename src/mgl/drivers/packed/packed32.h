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
* Description:  32 bit packed pixel memory device driver header file.
*
****************************************************************************/

#ifndef __DRIVERS_PACKED_PACKED32_H
#define __DRIVERS_PACKED_PACKED32_H

#include "drivers/common/common.h"

/*---------------------- Macro and type definitions -----------------------*/

typedef struct {
    mem_vec v;                  /* Memory device vectors and info       */
    } PACKED32_state;

/*------------------------- Function Prototypes ---------------------------*/

void *  MGLAPI PACKED32_createInstance(void);
ibool   MGLAPI PACKED32_initDriver(void *data,MGLDC *dc,modeent *mode,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
void    MGLAPI PACKED32_destroyInstance(void *data);
void    MGLAPI PACKED32_makeCurrent(MGLDC *dc,MGLDC *oldDC,ibool partial);

#endif  /* __DRIVERS_PACKED_PACKED32_H */
