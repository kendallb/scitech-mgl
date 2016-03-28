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
;*              fixed point trig operations using table lookup and linear
;*              interpolation:
;*
;*                  F386_sin    - 16.16 sine
;*                  F386_cos    - 16.16 cosine
;*                  F386_tan    - 16.16 tangent
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fxtrig                  ; Set up memory model

begdataseg  fxtrig

        cextern FXsin_table,ULONG

enddataseg  fxtrig

begcodeseg  fxtrig

;----------------------------------------------------------------------------
; FXFixed F386_sin(FXFixed angle)
;----------------------------------------------------------------------------
; Computes the sine of a fixed point angle and returns the result in fixed
; point. We use table lookup to find the closest values, then interpolate
; the result to get the accuracy we need.
;----------------------------------------------------------------------------
cprocstart  F386_sin

        ARG     angle:ULONG

        enter_c

; Ensure that the angle is within the range 0-360 degrees

        mov     eax,[angle]

@@WhileLess:
        cmp     eax,0
        jge     @@WhileLarger
        add     eax,1680000h    ; Add 360 degrees
        jmp     @@WhileLess

@@WhileLarger:
        cmp     eax,1680000h
        jle     @@Convert
        sub     eax,1680000h    ; Subtract 360 degrees
        jmp     @@WhileLarger

; Convert angle so that 90 degrees = 256. This gives us the following
; as the resulting number:
;
;   bits 25-26     16-24         0-15
;  +----------+-------------+-------------+
;  |.quadrant.|.table index.|.interpolant.|
;  +----------+-------------+-------------+

@@Convert:
        mov     edx,2D82Dh      ; EDX := REAL(256.0 / 90.0) = 2.84444..
        imul    edx
        shrd    eax,edx,16      ; EAX := angle * 2.8444...
        adc     eax,0

        mov     ebx,eax
        mov     esi,eax
        mov     edx,eax

        shr     ebx,14          ; EBX := 8 bit table index
        and     ebx,03FCh
        test    esi,01000000h   ; Handle quadrants 1 & 3
        jz      @@Quad02
        not     edx             ; Negate interpolation factor
        xor     ebx,03FCh       ; Index table in reverse order

@@Quad02:
        mov     ecx,[FXsin_table+ebx]   ; ECX := FXsin_table[index]
        mov     eax,[FXsin_table+ebx+4] ; EAX := FXsin_table[index+1]
        sub     eax,ecx                 ; EAX := diff

; Compute the interpolation factor and add in the interpolated difference

        and     edx,0FFFFh      ; Mask out interpolation value
        imul    edx
        shrd    eax,edx,16      ; EAX := diff * (a & 0xFFFF)
        adc     ecx,eax         ; Add in to result

; Handle quadrants 3 & 4 where the values are negated

        test    esi,02000000h
        jz      @@Positive
        neg     ecx             ; Negate the result

@@Positive:
        mov     eax,ecx
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_cos(FXFixed angle)
;----------------------------------------------------------------------------
; Computes the cosine of a fixed point angle and returns the result in fixed
; point. Simply calls the F386_sin routine to perform the calculation.
;----------------------------------------------------------------------------
cprocstart  F386_cos

        add     [ULONG esp+4],5A0000h   ; Access directly on stack
        jmp     F386_sin

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_tan(FXFixed angle)
;----------------------------------------------------------------------------
; Computes the tangent of a fixed point angle and returns the result in
; fixed point.
;----------------------------------------------------------------------------
cprocstart  F386_tan

        mov     eax,[esp+4]     ; Access directly on stack frame

; Ensure that the angle is within the range 0-360 degrees

@@WhileLess:
        cmp     eax,0
        jge     @@WhileLarger
        add     eax,1680000h    ; Add 360 degrees
        jmp     @@WhileLess

@@WhileLarger:
        cmp     eax,1680000h
        jle     @@OK
        sub     eax,1680000h    ; Subtract 360 degrees
        jmp     @@WhileLarger

@@OK:   cmp     eax,5A0000h     ; EAX == REAL(90)?
        je      @@Invalid
        cmp     eax,10E0000h    ; EAX == REAL(270)?
        je      @@Invalid

        push    eax
        call    F386_sin
        pop     ecx
        push    eax

        add     ecx,5A0000h
        push    ecx
        call    F386_sin
        pop     ecx
        pop     edx             ; EDX := FXsin(angle)
        mov     ecx,eax         ; ECX := FXcos(angle)

        xor     eax,eax
        shrd    eax,edx,16      ; position so that result ends up
        sar     edx,16          ; in EAX
        idiv    ecx             ; EAX := FXsin(angle) / FXcos(angle)

@@Exit: ret

@@Invalid:
        mov     eax,7FFFFFFFh
        jmp     @@Exit

cprocend

endcodeseg  fxtrig

        END                     ; End of module
