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
* Environment:  RTOS-32
*
* Description:  MTRR helper functions module. To make it easier to implement
*               the MTRR support under RTOS, we simply put our ring 0 helper
*               functions into stubs that run them at ring 0 using whatever
*               mechanism is available.
*
****************************************************************************/

#include "pmapi.h"
#include <rttarget.h>

/*--------------------------- Global variables ----------------------------*/

#define R0_FLUSH_TLB        0
#define R0_SAVE_CR4         1
#define R0_RESTORE_CR4      2
#define R0_READ_MSR         3
#define R0_WRITE_MSR        4
#define R0_PAGING_ENABLED   5
#define R0_GET_PDB          6

typedef struct {
    int     service;
    int     reg;
    ulong   eax;
    ulong   edx;
    } R0_data;

static R0_data _PM_R0;

/*----------------------------- Implementation ----------------------------*/

#ifdef __WATCOMC__
#define FASTCALL
#else
#define FASTCALL    __fastcall
#endif

/* External assembler functions */
ulong _ASMAPI __PM_ring0_callgate(void *p);

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
Function to execute a service at ring 0. Note that we need this to be
__fastcall but our assembler module is _cdecl, so we have a translation
function here.
****************************************************************************/
DWORD FASTCALL _PM_ring0_callgate(
    void *p)
{
    return __PM_ring0_callgate(p);
}

/****************************************************************************
REMARKS:
Flush the translation lookaside buffer.
****************************************************************************/
void PMAPI PM_flushTLB(void)
{
    _PM_R0.service = R0_FLUSH_TLB;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
}

/****************************************************************************
REMARKS:
Read and return the value of the CR4 register
****************************************************************************/
ulong _ASMAPI _MTRR_saveCR4(void)
{
    _PM_R0.service = R0_SAVE_CR4;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
    return _PM_R0.reg;
}

/****************************************************************************
REMARKS:
Restore the value of the CR4 register
****************************************************************************/
void _ASMAPI _MTRR_restoreCR4(
    ulong cr4Val)
{
    _PM_R0.service = R0_RESTORE_CR4;
    _PM_R0.reg = cr4Val;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
}

/****************************************************************************
REMARKS:
Read a machine status register for the CPU.
****************************************************************************/
void PMAPI PM_readMSR(
    ulong reg,
    PM_FAR ulong *eax,
    PM_FAR ulong *edx)
{
    _PM_R0.service = R0_READ_MSR;
    _PM_R0.reg = reg;
    _PM_R0.eax = (ulong)&*eax;
    _PM_R0.edx = (ulong)&*edx;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
}

/****************************************************************************
REMARKS:
Write a machine status register for the CPU.
****************************************************************************/
void PMAPI PM_writeMSR(
    ulong reg,
    ulong eax,
    ulong edx)
{
    _PM_R0.service = R0_WRITE_MSR;
    _PM_R0.reg = reg;
    _PM_R0.eax = eax;
    _PM_R0.edx = edx;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
}

/****************************************************************************
REMARKS:
Determins if paging is enabled or not.
****************************************************************************/
int _ASMAPI _PM_pagingEnabled(void)
{
    _PM_R0.service = R0_PAGING_ENABLED;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
    return _PM_R0.eax;
}

/****************************************************************************
REMARKS:
Obtains the page directory base register so we can access the page tables.
****************************************************************************/
ulong _ASMAPI _PM_getPDB(void)
{
    _PM_R0.service = R0_GET_PDB;
    RTCallRing0(_PM_ring0_callgate, (void*)&_PM_R0);
    return _PM_R0.eax;
}

