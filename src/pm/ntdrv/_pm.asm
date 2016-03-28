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
;* Environment: 32-bit Windows NT device driver
;*
;* Description: Low level assembly support for the PM library specific to
;*              Windows NT device drivers.
;*
;****************************************************************************

include "scitech.mac"               ; Memory model macros

header      _pm                     ; Set up memory model

begdataseg  _pm

saveFPU     dclb 108                    ; Save buffer for FPU state
saveCR0     dd  0                       ; Save buffer for CR0 reg
saveGDT     dclb 6                      ; Save buffer for GDT reg

enddataseg  _pm

P586

begcodeseg  _pm                 ; Start of code segment

;----------------------------------------------------------------------------
; void PM_segread(PMSREGS *sregs)
;----------------------------------------------------------------------------
; Read the current value of all segment registers
;----------------------------------------------------------------------------
cprocstart  PM_segread

        ARG     sregs:DPTR

        enter_c

        mov     ax,es
        _les    _si,[sregs]
        mov     [_ES _si],ax
        mov     [_ES _si+2],cs
        mov     [_ES _si+4],ss
        mov     [_ES _si+6],ds
        mov     [_ES _si+8],fs
        mov     [_ES _si+10],gs

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; int PM_int386x(int intno, PMREGS *in, PMREGS *out,PMSREGS *sregs)
;----------------------------------------------------------------------------
; Issues a software interrupt in protected mode. This routine has been
; written to allow user programs to load CS and DS with different values
; other than the default.
;----------------------------------------------------------------------------
cprocstart  PM_int386x

; Not used for NT device drivers

        ret

cprocend

;----------------------------------------------------------------------------
; void PM_setBankA(int bank)
;----------------------------------------------------------------------------
cprocstart      PM_setBankA

; Not used for NT device drivers

        ret

cprocend

;----------------------------------------------------------------------------
; void PM_setBankAB(int bank)
;----------------------------------------------------------------------------
cprocstart      PM_setBankAB

; Not used for NT device drivers

        ret

cprocend

;----------------------------------------------------------------------------
; void PM_setCRTStart(int x,int y,int waitVRT)
;----------------------------------------------------------------------------
cprocstart      PM_setCRTStart

; Not used for NT device drivers

        ret

cprocend

; Macro to delay briefly to ensure that enough time has elapsed between
; successive I/O accesses so that the device being accessed can respond
; to both accesses even on a very fast PC.

%macro  DELAY 0
        jmp     short $+2
        jmp     short $+2
        jmp     short $+2
%endmacro

%macro  IODELAYN 1
%rep    %1
        DELAY
%endrep
%endmacro

;----------------------------------------------------------------------------
; uchar _PM_readCMOS(int index)
;----------------------------------------------------------------------------
; Read the value of a specific CMOS register. We do this with both
; normal interrupts and NMI disabled.
;----------------------------------------------------------------------------
cprocstart  _PM_readCMOS

        ARG     index:UINT

        push    _bp
        mov     _bp,_sp
        pushfd
        mov     al,[BYTE index]
        or      al,80h              ; Add disable NMI flag
        cli
        out     70h,al
        IODELAYN 5
        in      al,71h
        mov     ah,al
        xor     al,al
        IODELAYN 5
        out     70h,al              ; Re-enable NMI
        mov     al,ah               ; Return value in AL
        popfd
        pop     _bp
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_writeCMOS(int index,uchar value)
;----------------------------------------------------------------------------
; Read the value of a specific CMOS register. We do this with both
; normal interrupts and NMI disabled.
;----------------------------------------------------------------------------
cprocstart  _PM_writeCMOS

        ARG     index:UINT, value:UCHAR

        push    _bp
        mov     _bp,_sp
        pushfd
        mov     al,[BYTE index]
        or      al,80h              ; Add disable NMI flag
        cli
        out     70h,al
        IODELAYN 5
        mov     al,[value]
        out     71h,al
        xor     al,al
        IODELAYN 5
        out     70h,al              ; Re-enable NMI
        popfd
        pop     _bp
        ret

cprocend

;----------------------------------------------------------------------------
; double _ftol(double f)
;----------------------------------------------------------------------------
; Calls to __ftol are generated by the Borland C++ compiler for code
; that needs to convert a floating point type to an integral type.
;
; Input: floating point number on the top of the '87.
;
; Output: a (signed or unsigned) long in EAX
; All other registers preserved.
;-----------------------------------------------------------------------
cprocstart  _ftol

        LOCAL   temp1:WORD, temp2:QWORD

        enter_c

        fstcw   [temp1]                 ; save the control word
        fwait
        mov     al,[BYTE temp1+1]
        or      [BYTE temp1+1],0Ch      ; set rounding control to chop
        fldcw   [WORD temp1]
        fistp   [QWORD temp2]           ; convert to 64-bit integer
        mov     [BYTE temp1+1],al
        fldcw   [temp1]                 ; restore the control word
        mov     eax,[DWORD temp2]       ; return LS 32 bits
        mov     edx,[DWORD temp2+4]     ;        MS 32 bits

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; _PM_getPDB - Return the Page Table Directory Base address
;----------------------------------------------------------------------------
cprocstart  _PM_getPDB

        mov     eax,cr3
        and     eax,0FFFFF000h
        ret

cprocend

;----------------------------------------------------------------------------
; _PM_getGDT - Return the Global Descriptor Table Base address
;----------------------------------------------------------------------------
cprocstart  _PM_getGDTBase

        sgdt    [saveGDT]
        mov     dx,[WORD saveGDT]
        mov     eax,[DWORD saveGDT+2]
        ret

cprocend

;----------------------------------------------------------------------------
; _PM_getDS - Return the Data Selector
;----------------------------------------------------------------------------
cprocstart  _PM_getDS

        mov     eax,ds
        and     eax,0000FFF0h
        ret

cprocend

;----------------------------------------------------------------------------
; Flush the Translation Lookaside buffer
;----------------------------------------------------------------------------
cprocstart  PM_flushTLB

        wbinvd                  ; Flush the CPU cache
        mov     eax,cr3
        mov     cr3,eax         ; Flush the TLB
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_saveFPUState(void);
;----------------------------------------------------------------------------
; Save complete FPU state and re-initialize FPU with exceptions off.
;----------------------------------------------------------------------------
cprocstart  PM_saveFPUState

        mov     eax,cr0
        mov     [saveCR0],eax
        clts                            ; Clear task state flag
        fsave   [saveFPU]               ; Save FPU state
        finit                           ; All exceptions are masked off
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_restoreFPUState(void);
;----------------------------------------------------------------------------
; Restore FPU state which was saved above.
;----------------------------------------------------------------------------
cprocstart  PM_restoreFPUState

        frstor  [saveFPU]               ; Restore FPU state
        mov     eax,[saveCR0]
        mov     cr0,eax
        ret

cprocend

endcodeseg  _pm

        END                     ; End of module
