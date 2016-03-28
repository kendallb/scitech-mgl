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
* Environment:  QNX
*
* Description:  MTRR helper functions module. To make it easier to implement
*               the MTRR support under QNX, we simply put our ring 0 helper
*               functions into stubs that run them at ring 0 using whatever
*               mechanism is available.
*
****************************************************************************/

#include "pmapi.h"
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#ifdef __QNXNTO__
#include <sys/neutrino.h>
#include <sys/syspage.h>
#else
#include <i86.h>
#include <sys/irqinfo.h>
#endif

/*--------------------------- Global variables ----------------------------*/

#define R0_FLUSH_TLB    0
#define R0_SAVE_CR4     1
#define R0_RESTORE_CR4  2
#define R0_READ_MSR     3
#define R0_WRITE_MSR    4

typedef struct {
    int     service;
    int     reg;
    ulong   eax;
    ulong   edx;
    } R0_data;

extern volatile R0_data _PM_R0;

/*----------------------------- Implementation ----------------------------*/

#ifdef __QNXNTO__
const struct sigevent * _ASMAPI _PM_ring0_isr(void *arg, int id);
#else
pid_t far _ASMAPI _PM_ring0_isr();
#endif

/****************************************************************************
REMARKS:
Return true if ring 0 (or if we can call the helpers functions at ring 0)
****************************************************************************/
ibool _ASMAPI _MTRR_isRing0(void)
{
    return true;
}

/****************************************************************************
REMARKS:
Function to execute a service at ring 0. This is done using the clock
interrupt handler since the code we attach to it will always run at ring 0.
****************************************************************************/
static void CallRing0(void)
{
#ifdef __QNXNTO__
    uint    clock_intno = SYSPAGE_ENTRY(qtime)->intr;
#else
    uint    clock_intno = 0;    /* clock irq */
#endif
    int     intrid;

#ifdef __QNXNTO__
    mlock((void*)&_PM_R0, sizeof(_PM_R0));
    ThreadCtl(_NTO_TCTL_IO, 0);
#endif
#ifdef __QNXNTO__
    if ((intrid = InterruptAttach(_NTO_INTR_CLASS_EXTERNAL | clock_intno,
        _PM_ring0_isr, (void*)&_PM_R0, sizeof(_PM_R0), _NTO_INTR_FLAGS_END)) == -1) {
#else
    if ((intrid = qnx_hint_attach(clock_intno, _PM_ring0_isr, FP_SEG(&_PM_R0))) == -1) {
#endif
        perror("Attach");
        exit(-1);
        }
    while (_PM_R0.service != -1)
        ;
#ifdef __QNXNTO__
    InterruptDetach(intrid);
#else
    qnx_hint_detach(intrid);
#endif
}

/****************************************************************************
REMARKS:
Flush the translation lookaside buffer.
****************************************************************************/
void PMAPI PM_flushTLB(void)
{
    _PM_R0.service = R0_FLUSH_TLB;
    CallRing0();
}

/****************************************************************************
REMARKS:
Read and return the value of the CR4 register
****************************************************************************/
ulong _ASMAPI _MTRR_saveCR4(void)
{
    _PM_R0.service = R0_SAVE_CR4;
    CallRing0();
    return _PM_R0.reg;
}

/****************************************************************************
REMARKS:
Restore the value of the CR4 register
****************************************************************************/
void _ASMAPI _MTRR_restoreCR4(ulong cr4Val)
{
    _PM_R0.service = R0_RESTORE_CR4;
    _PM_R0.reg = cr4Val;
    CallRing0();
}

/****************************************************************************
REMARKS:
Read a machine status register for the CPU.
****************************************************************************/
void _ASMAPI PM_readMSR(
    ulong reg,
    ulong *eax,
    ulong *edx)
{
    _PM_R0.service = R0_READ_MSR;
    _PM_R0.reg = reg;
    CallRing0();
    *eax = _PM_R0.eax;
    *edx = _PM_R0.edx;
}

/****************************************************************************
REMARKS:
Write a machine status register for the CPU.
****************************************************************************/
void _ASMAPI PM_writeMSR(
    ulong reg,
    ulong eax,
    ulong edx)
{
    _PM_R0.service = R0_WRITE_MSR;
    _PM_R0.reg = reg;
    _PM_R0.eax = eax;
    _PM_R0.edx = edx;
    CallRing0();
}
