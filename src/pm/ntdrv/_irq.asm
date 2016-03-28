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

include "scitech.mac"           ; Memory model macros

header      _irq                ; Set up memory model

begdataseg  _irq

    cextern _PM_rtcHandler,CPTR
    cextern _PM_prevRTC,FCPTR

sidtBuf     dd      0           ; Offset for sidt instruction
            dw      0           ; Segment for sidt instruction

enddataseg  _irq

begcodeseg  _irq                ; Start of code segment

cpublic _PM_irqCodeStart

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
; PM_rtcISR - Real time clock interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the timer interrupt, to dispatch control
; to high level C based subroutines. We save the state of all registers
; in this routine, and switch to a local stack. Interrupts are *off*
; when we call the user code and must *never* be re-enabled and it
; just return as soon as possible.
;----------------------------------------------------------------------------
cprocfar    _PM_rtcISR

        pushad                      ; Save _all_ extended registers
        cld                         ; Clear direction flag

; Clear priority interrupt controller and re-enable interrupts so we
; dont lock things up for long.

        mov     al,20h
        out     0A0h,al
        out     020h,al

; Clear real-time clock timeout

        in      al,70h              ; Read CMOS index register
        push    eax                 ;  and save for later
        IODELAYN 3
        mov     al,0Ch
        out     70h,al
        IODELAYN 5
        in      al,71h

; Call the C interrupt handler function with interrupts *off*!

        call    [CPTR _PM_rtcHandler]

@@Exit: pop     eax
        out     70h,al              ; Restore CMOS index register
        popad                       ; Restore all extended registers
        iret

cprocend

;----------------------------------------------------------------------------
; PM_rtcISR - Real time clock interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the timer interrupt, to dispatch control
; to high level C based subroutines. We save the state of all registers
; in this routine, and switch to a local stack. Interrupts are *off*
; when we call the user code and must *never* be re-enabled and it
; just return as soon as possible.
;----------------------------------------------------------------------------
cprocfar    _PM_rtcISRAlt

        pushad                      ; Save _all_ extended registers
        cld                         ; Clear direction flag

; Call the C interrupt handler function with interrupts *off*!

        call    [CPTR _PM_rtcHandler]

; Chain to previous handler which will take care of clearing the interrupt

        popad                       ; Restore all extended registers
        jmp     [_PM_prevRTC]

cprocend

cpublic _PM_irqCodeEnd

;----------------------------------------------------------------------------
; void _PM_getISR(int irq,PMFARPTR *handler);
;----------------------------------------------------------------------------
; Function to return the specific IRQ handler direct from the IDT.
;----------------------------------------------------------------------------
cprocstart  _PM_getISR

        ARG     idtEntry:UINT, handler:DPTR

        enter_c
        mov     ecx,[handler]           ; Get address of handler to fill in
        sidt    [sidtBuf]               ; Get IDTR register into sidtBuf
        mov     eax,[DWORD sidtBuf+2]   ; Get address of IDT into EAX
        mov     ebx,[idtEntry]
        lea     eax,[eax+ebx*8]         ; Get entry in the IDT
        movzx   edx,[WORD eax+6]        ; Get high order 16-bits
        shl     edx,16                  ; Move into top 16-bits of address
        mov     dx,[WORD eax]           ; Get low order 16-bits
        mov     [DWORD ecx],edx         ; Store linear address of handler
        mov     dx,[WORD eax+2]         ; Get selector value
        mov     [WORD ecx+4],dx         ; Store selector value
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_setISR(int irq,void *handler);
;----------------------------------------------------------------------------
; Function to set the specific IRQ handler direct in the IDT.
;----------------------------------------------------------------------------
cprocstart  _PM_setISR

        ARG     irq:UINT, handler:CPTR

        enter_c
        mov     ecx,[handler]           ; Get address of new handler
        mov     dx,cs                   ; Get selector for new handler
        sidt    [sidtBuf]               ; Get IDTR register into sidtBuf
        mov     eax,[DWORD sidtBuf+2]   ; Get address of IDT into EAX
        mov     ebx,[idtEntry]
        lea     eax,[eax+ebx*8]         ; Get entry in the IDT
        cli
        mov     [WORD eax+2],dx         ; Store code segment selector
        mov     [WORD eax],cx           ; Store low order bits of handler
        shr     ecx,16
        mov     [WORD eax+6],cx         ; Store high order bits of handler
        sti
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_restoreISR(int irq,PMFARPTR *handler);
;----------------------------------------------------------------------------
; Function to set the specific IRQ handler direct in the IDT.
;----------------------------------------------------------------------------
cprocstart  _PM_restoreISR

        ARG     irq:UINT, handler:CPTR

        enter_c
        mov     ecx,[handler]
        mov     dx,[WORD ecx+4]         ; Get selector for old handler
        mov     ecx,[DWORD ecx]         ; Get address of old handler
        sidt    [sidtBuf]               ; Get IDTR register into sidtBuf
        mov     eax,[DWORD sidtBuf+2]   ; Get address of IDT into EAX
        mov     ebx,[idtEntry]
        lea     eax,[eax+ebx*8]         ; Get entry in the IDT
        cli
        mov     [WORD eax+2],dx         ; Store code segment selector
        mov     [WORD eax],cx           ; Store low order bits of handler
        shr     ecx,16
        mov     [WORD eax+6],cx         ; Store high order bits of handler
        sti
        leave_c
        ret

cprocend

endcodeseg  _irq

        END                     ; End of module

