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
;*              fixed point square root operations using table lookup
;*              and linear interpolation:
;*
;*                  F386_sqrt       - 16.16 square root
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fxsqrt                  ; Set up memory model

begdataseg  fxsqrt

        cextern FXsqrt_table,ULONG

enddataseg  fxsqrt

begcodeseg  fxsqrt

;----------------------------------------------------------------------------
; FXFixed F386_sqrt(FXFixed f);
;----------------------------------------------------------------------------
; Computes the square root of a fixed point number using a table lookup.
;----------------------------------------------------------------------------
cprocstart  F386_sqrt

        push    ebx             ; Save value of EBX across call
        mov     ebx,[esp+8]     ; EBX := fixed point number

; First we isolate the first 8 bits of the mantissa in our
; fixed point number. We do this by scanning along until we
; find the first 1 bit in the number, and shift it all right
; until this is in bit position 7. Since IEEE floating point
; numbers have an implied 1 bit in the mantissa, we mask this
; bit out and use the 7 bits as an index into the table. The
; 8th bit is a 1 if the exponent of our fixed point number
; was odd (position of the binary point with respect to the
; first 1 bit in the number).

        or      ebx,ebx
        jle     @@Invalid       ; Taking sqrt of 0 or -ve number

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
; out. If the exponent is odd then we index into the second half
; of the table.

        xor     edx,edx         ; Clear interpolant
        shr     ebx,8
        mov     dx,bx           ; EDX := interpolant
        shr     ebx,14          ; ebx := top 7 bits of mantissa + implied 1
        test    eax,1           ; Is the exponent odd?
        jz      @@EvenExp       ; Yes, dont modify
        and     ebx,03FCh       ; Index into second half of table
        jmp     @@DoLookup

@@EvenExp:
        and     ebx,01FCh       ; Index into second half of table

; Divide the exponent by two (square root it) while preserving the
; sign, then construct the square root by combining the mantissa from
; the lookup table and the square rooted exponent. To do this we add
; the implied 1 bit to the number and shift it the number of places
; indicated by the exponent. A positive exponent means shift left, negative
; means shift right.

@@DoLookup:
        sar     eax,1           ; Divide exponent by 2
        sub     eax,7           ; Adjust to 24 bits of precision in table
        mov     ecx,eax         ; ECX := exponent of final result
        mov     eax,[FXsqrt_table+ebx+4]    ; EAX := FXsqrt_table[index+1]
        mov     ebx,[FXsqrt_table+ebx]  ; EBX := FXsqrt_table[index]
        sub     eax,ebx         ; EAX := difference
        or      ecx,ecx         ; Check sign of exponent
        jl      @@Neg           ; Exponent is negative
        shl     eax,cl          ; Shift the result to correct position
        shl     ebx,cl
        jmp     @@Interpolate
@@Neg:  neg     ecx             ; Negative exponent shift right
        shr     eax,cl          ; Shift the result to correct position
        shr     ebx,cl

; Add in the interpolated difference

@@Interpolate:
        imul    edx
        shrd    eax,edx,16      ; EAX := diff * interpolant
        adc     eax,ebx         ; Add in to result

@@Exit:
        pop     ebx             ; Restore EBX
        ret

@@Invalid:
        xor     eax,eax         ; Invalid - return 0
        jmp     @@Exit

cprocend

endcodeseg  fxsqrt

        END                     ; End of module
