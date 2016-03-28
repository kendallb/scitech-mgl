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
;* Description: This module contains routines for high performance
;*              realing point trig operations by directly programming the
;*              80387 FPU (parameters passed as either floats or doubles
;*              depending on which compilation option is selected).
;*
;*                  FXsin       - floating point sine (degrees)
;*                  FXcos       - floating point cosine (degrees)
;*                  FXsincos    - floating point sine and cosine (degrees)
;*                  FXtan       - floating point tangent (degrees)
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

        P387                    ; Turn on i387 instructions

header  fltrig                  ; Set up memory model

begdataseg  fltrig

        cextern FXpi_180,REAL   ; Real value for PI/180

enddataseg  fltrig

begcodeseg  fltrig

;----------------------------------------------------------------------------
; real FXsin(real angle)
;----------------------------------------------------------------------------
cprocstart  FXsin

        fld     [REAL esp+4]    ; ST(0) := angle in degrees
        fmul    [REAL FXpi_180] ; ST(0) := angle in radians
        fsin                    ; ST(0) := sine of angle
        ret

cprocend

;----------------------------------------------------------------------------
; real FXcos(real angle)
;----------------------------------------------------------------------------
cprocstart  FXcos

        fld     [REAL esp+4]    ; ST(0) := angle in degrees
        fmul    [REAL FXpi_180] ; ST(0) := angle in radians
        fcos                    ; ST(0) := cosine of angle
        ret

cprocend

;----------------------------------------------------------------------------
; void FXsincos(real angle,real& Sin,real& Cos)
;----------------------------------------------------------------------------
; Computes both the sine and cosine of an angle as fast as possible.
;----------------------------------------------------------------------------
cprocstart  FXsincos

        ARG     angle:REAL, sin:DPTR, cos:DPTR

        push    ebp
        mov     ebp,esp
        push    ebx

        fld     [REAL angle]    ; ST(0) := angle in degrees
        fmul    [REAL FXpi_180] ; ST(0) := angle in radians
        fsincos                 ; ST(1) := sine of angle
                                ; ST(0) := cosine of angle
        wait                    ; Avoid bugs in fsincos instruction ;-(
        mov     ebx,[cos]       ; ES:_BX -> place for cosine
        fstp    [REAL ebx]      ; Store cosine value
        mov     ebx,[sin]       ; ES:_BX -> place for sine
        fstp    [REAL ebx]      ; Store sine value

        pop     ebx
        pop     ebp
        ret

cprocend

;----------------------------------------------------------------------------
; real FXtan(real angle)
;----------------------------------------------------------------------------
cprocstart  FXtan

        fld     [REAL esp+4]    ; ST(0) := angle in degrees
        fmul    [REAL FXpi_180] ; ST(0) := angle in radians
        fptan                   ; ST(0) := 1.0
                                ; ST(1) := tangent
        wait                    ; Avoid bugs with FPTAN
        nop
        fstp    st0             ; Clean up FPU stack
        ret

cprocend

endcodeseg  fltrig

        END                     ; End of module
