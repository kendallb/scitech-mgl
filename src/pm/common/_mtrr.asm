;****************************************************************************
;*
;*                  SciTech OS Portability Manager Library
;*
;*  ========================================================================
;*
;*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
;*
;*   This file may be distributed and/or modified under the terms of the
;*   GNU General Public License version 2.0 as published by the Free
;*   Software Foundation and appearing in the file LICENSE.GPL included
;*   in the packaging of this file.
;*
;*   Licensees holding a valid Commercial License for this product from
;*   SciTech Software, Inc. may use this file in accordance with the
;*   Commercial License Agreement provided with the Software.
;*
;*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
;*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;*   PURPOSE.
;*
;*   See http://www.scitechsoft.com/license/ for information about
;*   the licensing options available and how to purchase a Commercial
;*   License Agreement.
;*
;*   Contact license@scitechsoft.com if any conditions of this licensing
;*   are not clear to you, or you have questions about licensing options.
;*
;*  ========================================================================
;*
;* Language:    NASM
;* Environment: 16/32 bit Ring 0 device driver
;*
;* Description: Assembler support routines for the Memory Type Range Register
;*              (MTRR) module.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header      _mtrr               ; Set up memory model

begdataseg  _mtrr

ifdef   DOS4GW
    cextern _PM_haveCauseWay,UINT
endif

enddataseg  _mtrr

begcodeseg  _mtrr               ; Start of code segment

P586

;----------------------------------------------------------------------------
; ibool _MTRR_isRing0(void);
;----------------------------------------------------------------------------
; Checks to see if we are running at ring 0. This check is only relevant
; for 32-bit DOS4GW and compatible programs. If we are not running under
; DOS4GW, then we simply assume we are a ring 0 device driver.
;----------------------------------------------------------------------------
cprocnear   _MTRR_isRing0

; Are we running under CauseWay?

ifdef   DOS4GW
        enter_c
        mov     ax,cs
        and     eax,3
        xor     eax,3
        jnz     @@Exit

; CauseWay runs the apps at ring 3, but implements support for specific
; ring 0 instructions that we need to get stuff done under real DOS.

        mov     eax,1
        cmp     [UINT _PM_haveCauseWay],0
        jnz     @@Exit
@@Fail: xor     eax,eax
@@Exit: leave_c
        ret
else
ifdef __SMX32__
        mov     eax,1                   ; SMX is ring 0!
        ret
else
ifdef __VXD__
        mov     eax,1                   ; VxD is ring 0!
        ret
else
ifdef __NT_DRIVER__
        mov     eax,1                   ; NT/W2K is ring 0!
        ret
else
        xor     eax,eax                 ; Assume ring 3 for 32-bit DOS
        ret
endif
endif
endif
endif

cprocend

;----------------------------------------------------------------------------
; ulong _MTRR_disableInt(void);
;----------------------------------------------------------------------------
; Return processor interrupt status and disable interrupts.
;----------------------------------------------------------------------------
cprocstart  _MTRR_disableInt

        pushfd                  ; Put flag word on stack
        cli                     ; Disable interrupts!
        pop     eax             ; deposit flag word in return register
        ret

cprocend

;----------------------------------------------------------------------------
; void _MTRR_restoreInt(ulong ps);
;----------------------------------------------------------------------------
; Restore processor interrupt status.
;----------------------------------------------------------------------------
cprocstart  _MTRR_restoreInt

        ARG     ps:ULONG

        push    ebp
        mov     ebp,esp         ; Set up stack frame
        mov     ecx,[ps]
        test    ecx,200h        ; SMP safe interrupt flag restore!
        jz      @@1
        sti
@@1:    pop     ebp
        ret

cprocend

;----------------------------------------------------------------------------
; ulong _MTRR_saveCR4(void);
;----------------------------------------------------------------------------
; Save the value of CR4 and clear the Page Global Enable (bit 7). We also
; disable and flush the caches.
;----------------------------------------------------------------------------
cprocstart  _MTRR_saveCR4

        enter_c

; Save value of CR4 and clear Page Global Enable (bit 7)

        mov     ebx,cr4
        mov     eax,ebx
        and     al,7Fh
        mov     cr4,eax

; Disable and flush caches

        mov     eax,cr0
        or      eax,40000000h   ; set Cache Disable bit
        and     eax,0DFFFFFFFh  ; clear Not Write-through bit
        wbinvd
        mov     cr0,eax
        wbinvd

; Return value from CR4

        mov     eax,ebx
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void _MTRR_restoreCR4(ulong cr4Val)
;----------------------------------------------------------------------------
; Save the value of CR4 and clear the Page Global Enable (bit 7). We also
; disable and flush the caches.
;----------------------------------------------------------------------------
cprocstart  _MTRR_restoreCR4

        ARG     cr4Val:ULONG

        enter_c

; Enable caches

        mov     eax,cr0
        and     eax,9FFFFFFFh   ; clear both CD and NW bits
        mov     cr0,eax
        mov     eax,[cr4Val]
        mov     cr4,eax
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; uchar _MTRR_getCx86(uchar reg);
;----------------------------------------------------------------------------
; Read a Cyrix CPU indexed register
;----------------------------------------------------------------------------
cprocstart  _MTRR_getCx86

        ARG     reg:UCHAR

        enter_c
        mov     al,[reg]
        out     22h,al
        in      al,23h
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; uchar _MTRR_setCx86(uchar reg,uchar val);
;----------------------------------------------------------------------------
; Write a Cyrix CPU indexed register
;----------------------------------------------------------------------------
cprocstart  _MTRR_setCx86

        ARG     reg:UCHAR, val:UCHAR

        enter_c
        mov     al,[reg]
        out     22h,al
        mov     al,[val]
        out     23h,al
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_readMSR(uong reg, ulong FAR *eax, ulong FAR *edx);
;----------------------------------------------------------------------------
; Writes the specific Machine Status Register used on the newer Intel
; Pentium Pro and Pentium II motherboards.
;----------------------------------------------------------------------------
cprocnear   PM_readMSR

        ARG     reg:ULONG, v_eax:DPTR, v_edx:DPTR

        enter_c
        mov     ecx,[reg]
        rdmsr
        mov     ebx,[v_eax]
        mov     [ebx],eax
        mov     ebx,[v_edx]
        mov     [ebx],edx
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_writeMSR(ulong reg, ulong eax, ulong edx);
;----------------------------------------------------------------------------
; Writes the specific Machine Status Register used on the newer Intel
; Pentium Pro and Pentium II motherboards.
;----------------------------------------------------------------------------
cprocnear   PM_writeMSR

        ARG     reg:ULONG, v_eax:ULONG, v_edx:ULONG

        enter_c
        mov     ecx,[reg]
        mov     eax,[v_eax]
        mov     edx,[v_edx]
        wrmsr
        leave_c
        ret

cprocend

endcodeseg  _mtrr

        END                     ; End of module
