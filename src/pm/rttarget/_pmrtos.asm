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
;* Environment: RTOS-32
;*
;* Description: Assembler support routines for the Memory Type Range Register
;*              (MTRR) module on RTOS-32.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header      _mtrr               ; Set up memory model

begdataseg  _mtrr

%define R0_FLUSH_TLB        0
%define R0_SAVE_CR4         1
%define R0_RESTORE_CR4      2
%define R0_READ_MSR         3
%define R0_WRITE_MSR        4
%define R0_PAGING_ENABLED   5
%define R0_GET_PDB          6

enddataseg  _mtrr

begcodeseg  _mtrr               ; Start of code segment

P586

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
; ulong _PM_ring0_callgate(void);
;----------------------------------------------------------------------------
; Ring 0 callgate that we use to execute the MTRR support
; code. For comments about what this code does, please refer to the DOS
; version that is called directly and is more commented.
;----------------------------------------------------------------------------
cprocnear   __PM_ring0_callgate

        ARG     param:DPTR

        enter_c
        mov     ebp,[param]                 ;   _PM_R0
        cmp     [DWORD ebp+0],R0_FLUSH_TLB  ;   _PM_R0.service
        jne     @@1
        wbinvd                              ; Flush the CPU cache
        mov     eax,cr3
        mov     cr3,eax                     ; Flush the TLB
        jmp     @@Exit

@@1:    cmp     [DWORD ebp+0],R0_SAVE_CR4
        jne     @@2
        mov     ebx,cr4
        mov     eax,ebx
        and     al,7Fh
        mov     cr4,eax
        mov     eax,cr0
        or      eax,40000000h               ; set Cache Disable bit
        and     eax,0DFFFFFFFh              ; clear Not Write-through bit
        wbinvd
        mov     cr0,eax
        wbinvd
        mov     [ebp+4],ebx                 ;   _PM_R0.reg
        jmp     @@Exit

@@2:    cmp     [DWORD ebp+0],R0_RESTORE_CR4
        jne     @@3
        mov     eax,cr0
        and     eax,9FFFFFFFh               ; clear both CD and NW bits
        mov     cr0,eax
        mov     eax,[ebp+4]                 ;   _PM_R0.reg
        mov     cr4,eax
        jmp     @@Exit

@@3:    cmp     [DWORD ebp+0],R0_READ_MSR
        jne     @@4
        mov     ecx,[ebp+4]                 ;   _PM_R0.reg
        rdmsr
        mov     ebx,[ebp+8]                 ;   _PM_R0.eax
        mov     [ebx],eax
        mov     ebx,[ebp+12]                ;   _PM_R0.edx
        mov     [ebx],edx
        jmp     @@Exit

@@4:    cmp     [DWORD ebp+0],R0_WRITE_MSR
        jne     @@5
        mov     ecx,[ebp+4]                 ;   _PM_R0.reg
        mov     eax,[ebp+8]                 ;   _PM_R0.eax
        mov     edx,[ebp+12]                ;   _PM_R0.edx
        wrmsr
        jmp @@Exit

@@5:    cmp     [DWORD ebp+0],R0_PAGING_ENABLED
        jne     @@6
        mov     eax,cr0                     ; Load CR0
        shr     eax,31                      ; Isolate paging enabled bit
        mov     [ebp+8],eax                 ;   _PM_R0.eax
        jmp     @@Exit

@@6:    cmp     [DWORD ebp+0],R0_GET_PDB
        jne     @@7
        mov     eax,cr3
        and     eax,0FFFFF000h
        mov     [ebp+8],eax                 ;   _PM_R0.eax
        jmp     @@Exit

@@7:
@@Exit: mov     [DWORD ebp+0],-1
        mov     eax,0
        leave_c
        ret

cprocend

endcodeseg  _mtrr

        END                     ; End of module

