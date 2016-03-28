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
* Environment:  any
*
* Description:  Test program to check the ability to call a real mode
*               procedure. We simply copy a terribly simple assembly
*               language routine into a real mode block that we allocate,
*               and then attempt to call the routine and verify that it
*               was successful.
*
*               Functions tested:   PM_allocRealSeg()
*                                   PM_freeRealSeg()
*                                   PM_callRealMode()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pmapi.h"

/* Block of real mode code we will eventually call */

static unsigned char realModeCode[] = {
    0x93,           /*  xchg    ax,bx   */
    0x87, 0xCA,     /*  xchg    cx,dx   */
    0xCB            /*  retf            */
    };

int main(void)
{
    RMREGS          regs;
    RMSREGS         sregs;
    uchar           *p;
    unsigned        r_seg,r_off;

    /* Allocate a the block of real mode memory */
    if ((p = PM_allocRealSeg(sizeof(realModeCode), &r_seg, &r_off)) == NULL) {
        printf("Unable to allocate real mode memory!\n");
        exit(1);
        }

    /* Copy the real mode code */
    memcpy(p,realModeCode,sizeof(realModeCode));

    /* Now call the real mode code */
    regs.x.ax = 1;
    regs.x.bx = 2;
    regs.x.cx = 3;
    regs.x.dx = 4;
    regs.x.si = 5;
    regs.x.di = 6;
    sregs.es = 7;
    sregs.ds = 8;
    PM_callRealMode(r_seg,r_off,&regs,&sregs);
    if (regs.x.ax != 2 || regs.x.bx != 1 || regs.x.cx != 4 || regs.x.dx != 3
            || regs.x.si != 5 || regs.x.di != 6 || sregs.es != 7
            || sregs.ds != 8) {
        printf("Real mode call failed!\n");
        printf("\n");
        printf("ax = %04X, bx = %04X, cx = %04X, dx = %04X\n",
            regs.x.ax,regs.x.bx,regs.x.cx,regs.x.dx);
        printf("si = %04X, di = %04X, es = %04X, ds = %04X\n",
            regs.x.si,regs.x.di,sregs.es,sregs.ds);
        }
    else
        printf("Real mode call succeeded!\n");

    /* Free the memory we allocated */
    PM_freeRealSeg(p);
    return 0;
}
