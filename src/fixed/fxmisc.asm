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
;* Description: This module contains routines for manipulating 32 bit
;*              fixed point numbers (16.16 format) using 32 bit 80386
;*              assembly code:
;*
;*                  F386_mul        - 16.16 Fixed point multiplication
;*                  F386_div        - 16.16 Fixed point division
;*                  F386_muldiv     - 16.16 Fixed point multiply then divide
;*                  F386_square     - 16.16 Fixed point square
;*                  F386_oneOver    - 16.16 Fixed point inversion
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fxmisc                  ; Set up memory model

begcodeseg  fxmisc

;----------------------------------------------------------------------------
; FXFixed F386_mul(FXFixed a,FXFixed b);
;----------------------------------------------------------------------------
; Multiplies two 32 bit fixed point values stored in FXFixed integers
; together and returns the result.
;----------------------------------------------------------------------------
cprocstart  F386_mul

        mov     eax,[esp+4]     ; Access directly without stack frame
        imul    [ULONG esp+8]
        add     eax,8000h       ; Round by adding 2^-17
        adc     edx,0           ; Whole part of result is in DX
        shrd    eax,edx,16      ; EAX := a * b
        ret

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_Div(FXFixed dividend,FXFixed divisor);
;----------------------------------------------------------------------------
; Divides two 32 bit fixed point values stored in FXFixed integers
; and returns the result. We use a modified method of 32 bit FXFixed division
; to compute the result.
;----------------------------------------------------------------------------
cprocstart  F386_div

        mov     edx,[esp+4]     ; Access directly without stack frame
        xor     eax,eax
        shrd    eax,edx,16      ; position so that result ends up
        sar     edx,16          ; in EAX
        idiv    [ULONG esp+8]
        ret

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_muldiv(FXFixed a,FXFixed b,FXFixed c);
;----------------------------------------------------------------------------
; Computes the 32 bit product of the following:
;
;       (a * b) / c
;
; Maintaining full 64 bit precision for the intermediate result of (a * b).
;----------------------------------------------------------------------------
cprocstart  F386_muldiv

        mov     eax,[esp+4]     ; Access directly without stack frame
        imul    [ULONG esp+8]   ; EDX:EAX := 64 bit dividend
        idiv    [ULONG esp+12]  ; Divide the 64 bit dividend
        ret

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_square(FXFixed a);
;----------------------------------------------------------------------------
; Squares a 32 bit fixed point value and returns the result.
;----------------------------------------------------------------------------
cprocstart  F386_square

        mov     eax,[esp+4]     ; Access directly without stack frame
        imul    eax
        add     eax,8000h       ; Round by adding 2^-17
        adc     edx,0           ; Whole part of result is in DX
        shrd    eax,edx,16      ; EAX := a * b
        ret

cprocend

;----------------------------------------------------------------------------
; FXFixed F386_oneOver(FXFixed divisor);
;----------------------------------------------------------------------------
; Computes the value of 1.0 / divisor and returns the result.
;----------------------------------------------------------------------------
cprocstart  F386_oneOver

        xor     eax,eax
        mov     edx,1
        idiv    [ULONG esp+4]
        ret

cprocend

endcodeseg  fxmisc

        END                     ; End of module
