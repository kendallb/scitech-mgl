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
;* Description: This module contains routines for dividing fixed point
;*              numbers into 4.28 format with full precision using
;*              32 bit 80386 assembly code:
;*
;*                  F386_divFF      - 4.28 = 16.16 / 16.16
;*                  F386_divZF      - 4.28 = 4.28 / 16.16
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fxzdiv                  ; Set up memory model

begcodeseg  fxzdiv

;----------------------------------------------------------------------------
; FXZFixed F386_divFF(FXFixed dividend,FXFixed divisor);
;----------------------------------------------------------------------------
; Divides a 16.16 fixed point value by a 16.16 fixed point value,
; returning the result as a 4.28 fixed point value. This routine calculates
; the resulting 4.28 number with full precision, by converting the remainder
; of the original divide into the final fraction bits to be added to the
; result.
;----------------------------------------------------------------------------
cprocstart  F386_divFF

        mov     edx,[esp+4]     ; Access directly without stack frame
        xor     eax,eax
        shrd    eax,edx,16      ; position so that result ends up
        sar     edx,16          ; in EAX
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
; FXZFixed F386_divZF(FXZFixed dividend,FXFixed divisor);
;----------------------------------------------------------------------------
; Divides a 4.28 fixed point value by a 16.16 fixed point value,
; returning the result as a 4.28 fixed point value. This routine calculates
; the resulting 4.28 number with full precision, by converting the remainder
; of the original divide into the final fraction bits to be added to the
; result.
;----------------------------------------------------------------------------
cprocstart  F386_divZF

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

endcodeseg  fxzdiv

        END                     ; End of module
