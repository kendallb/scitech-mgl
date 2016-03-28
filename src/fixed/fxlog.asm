;****************************************************************************
;*
;*                High Speed Fixed/Floating Point Library
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
;* Environment: IBM PC (MS DOS)
;*
;* Description: This module contains routines for high performance 16.16
;*              fixed point logarithm operations using table lookup and
;*              linear interpolation:
;*
;*                  F386_log10  - 16.16 base 10 logarithm
;*                  F386_log    - 16.16 natural logarithm
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fxlog                   ; Set up memory model

begdataseg  fxlog

        cextern FXlog10_table,USHORT
        cextern FXlog2_table,ULONG

enddataseg  fxlog

begcodeseg  fxlog

;----------------------------------------------------------------------------
; FXFixed F386_log10(FXFixed f);
;----------------------------------------------------------------------------
; Computes the common logarithm of a fixed point number using a table
; lookup and linear interpolation.
;----------------------------------------------------------------------------
cprocstart  F386_log10

        ARG     f:ULONG

        push    ebp
        mov     ebp,esp
        push    ebx             ; Save value of EBX across call
        mov     ebx,[f]         ; EBX := fixed point number

; First we isolate the first 8 bits of the mantissa in our
; fixed point number. We do this by scanning along until we
; find the first 1 bit in the number, and shift it all right
; until this is in bit position 7. Since IEEE floating point
; numbers have an implied 1 bit in the mantissa, we mask this
; bit out and use the 7 bits as an index into the table.

        or      ebx,ebx
        jle     @@Invalid       ; Taking log10 of 0 or -ve number

; Search for the index of the first 1 bit in the number (start of
; the mantissa. Note that we are only working with positive numbers
; here, so we ignore the sign bit (bit 31).

        shl     ebx,1           ; Mask out the sign bit
        bsr     ecx,ebx         ; ECX := index of 1st 1 bit in EBX
        mov     eax,ecx
        xor     cl,31           ; CL := bits to shift left
        sub     eax,17          ; eax := exponent for number
        shl     ebx,cl          ; Normalise so first 1 bit is in bit pos 31

; Compute the index into the table by shifting the mantissa
; so that the first 1 bit ends up in bit position 7, and mask it
; out.

        xor     edx,edx         ; Clear interpolant
        shr     ebx,8
        mov     dx,bx           ; EDX := interpolant
        shr     ebx,15          ; ebx := top 7 bits of mantissa + implied 1
        and     ebx,0FEh        ; Mask out index

; Lookup the values for the 7 bits of mantissa in the table, and
; linearly interpolate between the two entries.

        mov     ecx,eax         ; _CX := exponent of final result
        xor     eax,eax         ; Clear EAX
                                ; EBX high word is clear here!
        mov     ax,[FXlog10_table+ebx+2]; EAX := FXlog10_table[index+1]
        mov     bx,[FXlog10_table+ebx]  ; EBX := FXlog10_table[index]
        sub     eax,ebx         ; EAX := difference
        imul    edx             ; Interpolate
        shrd    eax,edx,16      ; EAX := diff * interpolant
        adc     eax,ebx         ; Add in to result

; Now find the appropriate power of 2 logarithm to add to the final
; result.

        or      ecx,ecx
        js      @@Neg

        mov     ebx,ecx
        shl     ebx,2           ; ebx = index into powers of 2 table
        add     eax,[FXlog2_table+ebx]
        jmp     @@Done

@@Neg:
        neg     ecx
        mov     ebx,ecx
        shl     ebx,2           ; ebx = index into powers of 2 table
        sub     eax,[FXlog2_table+ebx]

@@Done: pop     ebx             ; Restore EBX
        pop     ebp
        ret

@@Invalid:
        xor     eax,eax         ; Invalid - return 0
        jmp     @@Done

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_log(FXFixed f);
;----------------------------------------------------------------------------
; Computes the common logarithm of a fixed point number using a table
; lookup and linear interpolation.
;----------------------------------------------------------------------------
cprocstart  F386_log

        ARG     f:ULONG

        push    ebp
        mov     ebp,esp
        mov     eax,[f]
        push    eax
        call    F386_log10
        pop     ecx
        mov     edx,24D76h      ; EDX := REAL(2.302585093)
        imul    edx
        add     eax,8000h       ; Round by adding 2^-17
        adc     edx,0           ; Whole part of result is in DX
        shrd    eax,edx,16      ; EAX := f * REAL(2.302585093)
        pop     ebp
        ret

cprocend

endcodeseg  fxlog

        END                     ; End of module
