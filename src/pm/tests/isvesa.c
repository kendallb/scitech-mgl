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
* Description:  Test program to check the ability to allocate real mode
*               memory and to call real mode interrupt handlers such as
*               the VESA VBE BIOS from protected mode. Compile and link
*               with the appropriate command line for your DOS extender.
*
*               Functions tested:   PM_getVESABuf()
*                                   PM_mapRealPointer()
*                                   PM_int86x()
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pmapi.h"

/* SuperVGA information block */

#pragma pack(1)

typedef struct {
    char    VESASignature[4];       /* 'VESA' 4 byte signature          */
    short   VESAVersion;            /* VBE version number               */
    ulong   OEMStringPtr;           /* Far pointer to OEM string        */
    ulong   Capabilities;           /* Capabilities of video card       */
    ulong   VideoModePtr;           /* Far pointer to supported modes   */
    short   TotalMemory;            /* Number of 64kb memory blocks     */
    char    reserved[236];          /* Pad to 256 byte block size       */
    } VgaInfoBlock;

#pragma pack()

int main(void)
{
    RMREGS          regs;
    RMSREGS         sregs;
    VgaInfoBlock    vgaInfo;
    ushort          *mode;
    uint            vgLen;
    uchar           *vgPtr;
    unsigned        r_vgseg,r_vgoff;

    /* Allocate a 256 byte block of real memory for communicating with
     * the VESA BIOS.
     */
    if ((vgPtr = PM_getVESABuf(&vgLen,&r_vgseg,&r_vgoff)) == NULL) {
        printf("Unable to allocate VESA memory buffer!\n");
        exit(1);
        }

    /* Call the VESA VBE to see if it is out there */
    regs.x.ax = 0x4F00;
    regs.x.di = r_vgoff;
    sregs.es = r_vgseg;
    memcpy(vgPtr,"VBE2",4);
    PM_int86x(0x10, &regs, &regs, &sregs);
    memcpy(&vgaInfo,vgPtr,sizeof(VgaInfoBlock));
    if (regs.x.ax == 0x4F && strncmp(vgaInfo.VESASignature,"VESA",4) == 0) {
        printf("VESA VBE version %d.%d BIOS detected\n\n",
            vgaInfo.VESAVersion >> 8, vgaInfo.VESAVersion & 0xF);
        printf("Available video modes:\n");
        mode = PM_mapRealPointer(vgaInfo.VideoModePtr >> 16, vgaInfo.VideoModePtr & 0xFFFF);
        while (*mode != 0xFFFF) {
            printf("    %04hXh (%08X)\n", *mode, (int)mode);
            mode++;
            }
        }
    else
        printf("VESA VBE not found\n");
    return 0;
}
