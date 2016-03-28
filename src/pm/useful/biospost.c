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
* Environment:  Linux
*
* Description:  Module containing useful code that we are not using at
*               present. This modules exists mainly to allow this code to
*               be preserved if we ever decide to use it in the future.
*
****************************************************************************/

ibool PMAPI PM_doBIOSPOST(
    ushort axVal,
    ulong BIOSPhysAddr,
    void *copyOfBIOS,
    ulong BIOSLen)
{
    char        *bios_ptr = (char*)0xC0000;
    char        *old_bios;
    ulong       Current10, Current6D, *rvec = 0;
    RMREGS      regs;
    RMSREGS     sregs;

    /* The BIOS is mapped to 0xC0000 with a private memory mapping enabled
     * which means we have a copy on write scheme. Hence we simply copy
     * the secondary BIOS image over the top of the old one.
     */
    if (!inited)
        PM_init();
    if ((old_bios = PM_malloc(BIOSLen)) == NULL)
        return false;
    if (BIOSPhysAddr != 0xC0000) {
        memcpy(old_bios,bios_ptr,BIOSLen);
        memcpy(bios_ptr,copyOfBIOS,BIOSLen);
        }

    /* The interrupt vectors should already be mmap()'ed from 0-0x400 in PM_init */
    Current10 = rvec[0x10];
    Current6D = rvec[0x6D];

    /* POST the secondary BIOS */
    rvec[0x10] = rvec[0x42]; /* Restore int 10h to STD-BIOS */
    regs.x.ax = axVal;
    PM_callRealMode(0xC000,0x0003,&regs,&sregs);

    /* Restore interrupt vectors */
    rvec[0x10] = Current10;
    rvec[0x6D] = Current6D;

    /* Restore original BIOS image */
    if (BIOSPhysAddr != 0xC0000)
        memcpy(bios_ptr,old_bios,BIOSLen);
    PM_free(old_bios);
    return true;
}

