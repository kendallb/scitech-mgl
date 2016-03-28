/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  MIPS
*
* Description:  Main module to implement I/O port access functions
*               on MIPS platforms.
*
****************************************************************************/

#include "pmapi.h"
//#include <asm/io.h>

extern volatile void           *_PM_ioBase ;

void PMAPI PM_outpb(int port,u8 val)
{
    *(volatile u8 *)(_PM_ioBase + port) = val;
}

void PMAPI PM_outpw(int port,u16 val)
{
    *(volatile u16 *)(_PM_ioBase + port) = val;
}

void PMAPI PM_outpd(int port,u32 val)
{
    *(volatile u32 *)(_PM_ioBase + port) = val;
}

u8 PMAPI PM_inpb(int port)
{
    return *(volatile u8 *)(_PM_ioBase + port);
}

u16 PMAPI PM_inpw(int port)
{
    return *(volatile u16 *)(_PM_ioBase + port);
}

u32 PMAPI PM_inpd(int port)
{
    return *(volatile u32 *)(_PM_ioBase + port);
}
