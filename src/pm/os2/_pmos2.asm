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
;* Environment: OS/2 32 bit protected mode
;*
;* Description: Low level assembly support for the PM library specific
;*              to OS/2
;*
;****************************************************************************

include "scitech.mac"               ; Memory model macros

header      _pmos2                  ; Set up memory model

begdataseg  _pmos2

        cglobal _PM_ioentry
        cglobal _PM_gdt
_PM_ioentry     dd  0               ; Offset to call gate
_PM_gdt         dw  0               ; Selector to call gate

enddataseg  _pmos2

begcodeseg  _pmos2                  ; Start of code segment

;----------------------------------------------------------------------------
; int PM_setIOPL(int iopl)
;----------------------------------------------------------------------------
; Change the IOPL level for the 32-bit task. Returns the previous level
; so it can be restored for the task correctly.
;----------------------------------------------------------------------------
cprocstart  PM_setIOPL

        ARG     iopl:UINT

        enter_c
        pushfd                      ; Save the old EFLAGS for later
        mov     ecx,[iopl]          ; ECX := IOPL level
        xor     ebx,ebx             ; Change IOPL level function code (0)
        call    far dword [_PM_ioentry]
        pop     eax
        and     eax,0011000000000000b
        shr     eax,12
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_setGDTSelLimit(ushort selector, ulong limit);
;----------------------------------------------------------------------------
; Change the GDT selector limit to given value. Used to change selector
; limits to address the entire system address space.
;----------------------------------------------------------------------------
cprocstart  _PM_setGDTSelLimit

        ARG     selector:USHORT, limit:UINT

        enter_c
        sub     esp,20              ; Make room for selector data on stack
        mov     ecx,esp             ; ECX := selector data structure
        mov     bx,[selector]       ; Fill out the data structure
        and     bx,0FFF8h           ; Kick out the LDT/GDT and DPL bits
        mov     [WORD ecx],bx
        mov     ebx,[limit]
        mov     [DWORD ecx+4],ebx
        mov     ebx,5               ; Set GDT selector limit function code
        call    far dword [_PM_ioentry]
        add     esp,20
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
; ulong _MTRR_disableInt(void);
;----------------------------------------------------------------------------
; Return processor interrupt status and disable interrupts.
;----------------------------------------------------------------------------
cprocstart  _MTRR_disableInt

; Do nothing!
        ret

cprocend

;----------------------------------------------------------------------------
; void _MTRR_restoreInt(ulong ps);
;----------------------------------------------------------------------------
; Restore processor interrupt status.
;----------------------------------------------------------------------------
cprocstart  _MTRR_restoreInt

; Do nothing!
        ret

cprocend

;----------------------------------------------------------------------------
; void DebugInt(void)
;----------------------------------------------------------------------------
cprocstart  DebugInt

        int     3
        ret

cprocend

endcodeseg  _pmos2

        END                         ; End of module

