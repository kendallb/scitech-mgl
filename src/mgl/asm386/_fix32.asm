;****************************************************************************
;*
;*                  SciTech Multi-platform Graphics Library
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
;* Environment: Intel x86
;*
;* Description: Assembly language fixed point routines.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  _fix32                  ; Setup for MGL hybrid 16/32 bit segment

begcodeseg      _fix32          ; Start of hybrid 16/32 code segment

;----------------------------------------------------------------------------
; long MGL_FixMul(long a,long b);
;----------------------------------------------------------------------------
; Multiplies two 32 bit fixed point values stored in long integers
; together and returns the result.
;----------------------------------------------------------------------------
cprocstart  MGL_FixMul

        ARG     a:ULONG, b:ULONG

        mov     eax,[esp+4]     ; Access directly without stack frame
        imul    [ULONG esp+8]
        add     eax,8000h       ; Round by adding 2^-17
        adc     edx,0           ; Whole part of result is in DX
        shrd    eax,edx,16      ; EAX := a * b
        ret

cprocend

;----------------------------------------------------------------------------
; long MGL_FixDiv(long dividend,long divisor);
;----------------------------------------------------------------------------
; Divides two 32 bit fixed point values stored in long integers
; and returns the result. We use a modified method of 32 bit long division
; to compute the result.
;----------------------------------------------------------------------------
cprocstart  MGL_FixDiv

        ARG     dividend:ULONG, divisor:ULONG

        mov     edx,[esp+4]     ; Access directly without stack frame
        xor     eax,eax
        shrd    eax,edx,16      ; position so that result ends up
        sar     edx,16          ; in EAX
        idiv    [ULONG esp+8]
        ret

cprocend

;----------------------------------------------------------------------------
; zfix32_t MGL_ZFixDiv(zfix32_t dividend,fix32_t divisor);
;----------------------------------------------------------------------------
; Divides a 4.28 bit fixed point value by a 16.16 fixed point value,
; returning the result as a 4.28 fixed point value. This routine calculates
; the resulting 4.28 number with full precision, by converting the remainder
; of the original divide into the final fraction bits to be added to the
; result.
;----------------------------------------------------------------------------
cprocstart  MGL_ZFixDiv

        ARG     dividend:ULONG, divisor:ULONG

        mov     edx,[esp+4]     ; Access directly without stack frame
        xor     eax,eax
        shrd    eax,edx,28      ; position so that result ends up
        sar     edx,28          ; in EAX
        idiv    [ULONG esp+8]

; Compute remaining 12 bits of precision

        mov     ecx,eax
        shl     edx,12          ; Normalise remainder portion
        mov     eax,edx
        xor     edx,edx         ; Position so result ends up in EAX
        div     [ULONG esp+8]   ; Find last 12 fractional bits
        shl     eax,20          ; EAX := bottom 12 bits in 32:20
        shld    ecx,eax,12      ; ECX := result
        mov     eax,ecx
        ret

cprocend

;----------------------------------------------------------------------------
; long MGL_FixMuldiv(long a,long b,long c);
;----------------------------------------------------------------------------
; Computes the 32 bit product of the following:
;
;       (a * b) / c
;
; Maintaining full 64 bit precision for the intermediate result of (a * b).
;----------------------------------------------------------------------------
cprocstart  MGL_FixMulDiv

        ARG     a:ULONG, b:ULONG, c:ULONG

        mov     eax,[esp+4]     ; Access directly without stack frame
        imul    [ULONG esp+8]   ; EDX:EAX := 64 bit dividend
        idiv    [ULONG esp+12]  ; Divide the 64 bit dividend
        ret

cprocend

;----------------------------------------------------------------------------
; int MGL_backfacing(fix32_t dx1,fix32_t dy1,fix32_t dx2,fix32_t dy2);
;----------------------------------------------------------------------------
; Determine whether a polygon is backfacing given two fixed point vectors.
; We need to maintain full 64 bit precision during this calculation to
; avoid overflow for screen space polygons.
;----------------------------------------------------------------------------
cprocstart  MGL_backfacing

        ARG     dx1:ULONG, dy1:ULONG, dx2:ULONG, dy2:ULONG

        push    ebp
        mov     ebp,esp
        push    ebx

        mov     eax,[dx1]
        imul    [ULONG dy2]     ; EDX:EAX := dx1 * dy2
        mov     ebx,eax
        mov     ecx,edx         ; ECX:EBX := dx1 * dy2
        mov     eax,[dx2]
        imul    [ULONG dy1]     ; EDX:EAX := dx2 * dy1
        sub     eax,ebx
        mov     eax,1           ; Default to backfacing
        sbb     edx,ecx         ; EDX:EAX := dx1 * dy2 - dx2 * dy1
        jns     @@Backfacing
        xor     eax,eax         ; Polygon is frontfacing
@@Backfacing:
        pop     ebx
        pop     ebp
        ret

cprocend

endcodeseg  _fix32

        END                     ; End of module
