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
* Environment:  32-bit Windows NT device drivers.
*
* Description:  Implementation for the real mode software interrupt
*               handling functions.
*
****************************************************************************/

#include "pmapi.h"
#include "clib/os/os.h"
#include "mtrr.h"
#include "oshdr.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
We do have limited BIOS access under Windows NT device drivers.
****************************************************************************/
ibool PMAPI PM_haveBIOSAccess(void)
{
    // Return false unless we have full buffer passing!
    return false;
}

/****************************************************************************
PARAMETERS:
len     - Place to store the length of the buffer
rseg    - Place to store the real mode segment of the buffer
roff    - Place to store the real mode offset of the buffer

REMARKS:
This function returns the address and length of the global VESA transfer
buffer that is used for communicating with the VESA BIOS functions from
Win16 and Win32 programs under Windows.
****************************************************************************/
void * PMAPI PM_getVESABuf(
    uint *len,
    uint *rseg,
    uint *roff)
{
    // No buffers supported under Windows NT (Windows XP has them however if
    // we ever decide to support this!)
    return NULL;
}

/****************************************************************************
REMARKS:
Issue a protected mode software interrupt.
****************************************************************************/
int PMAPI PM_int386(
    int intno,
    PMREGS *in,
    PMREGS *out)
{
    PMSREGS sregs;
    PM_segread(&sregs);
    return PM_int386x(intno,in,out,&sregs);
}

/****************************************************************************
REMARKS:
Allocate a block of real mode memory
****************************************************************************/
void * PMAPI PM_allocRealSeg(
    uint size,
    uint *r_seg,
    uint *r_off)
{
    // Not supported in NT drivers
    (void)size;
    (void)r_seg;
    (void)r_off;
    return NULL;
}

/****************************************************************************
REMARKS:
Free a block of real mode memory.
****************************************************************************/
void PMAPI PM_freeRealSeg(
    void *mem)
{
    // Not supported in NT drivers
    (void)mem;
}

/****************************************************************************
REMARKS:
Call a V86 real mode function with the specified register values
loaded before the call. The call returns with a far ret.
****************************************************************************/
void PMAPI PM_callRealMode(
    uint seg,
    uint off,
    RMREGS *regs,
    RMSREGS *sregs)
{
    // Not used in NT drivers
    (void)seg;
    (void)off;
    (void)regs;
    (void)sregs;
}

/****************************************************************************
REMARKS:
Issue a V86 real mode interrupt with the specified register values
loaded before the interrupt.
****************************************************************************/
int PMAPI PM_int86(
    int intno,
    RMREGS *in,
    RMREGS *out)
{
    // If our display driver has come up and registered the miniport
    // PM_int86 entry point, we can do this. If not we can't implement this.
    if (_pPM_int86)
        return _pPM_int86(intno,in,out);
    *out = *in;
    return out->x.ax;
}

/****************************************************************************
REMARKS:
Issue a V86 real mode interrupt with the specified register values
loaded before the interrupt.
****************************************************************************/
int PMAPI PM_int86x(
    int intno,
    RMREGS *in,
    RMREGS *out,
    RMSREGS *sregs)
{
    // Not used in NT drivers
    (void)intno;
    (void)sregs;
    *out = *in;
    return out->x.ax;
}

