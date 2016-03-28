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
;*              fixed point 3D transformation matrix manipulation routin
;*
;*                  F386_scale3D        - Concat a 3D scale matrix
;*                  F386_rotatex3D      - Concat a 3D rotate X axis matrix
;*                  F386_rotatey3D      - Concat a 3D rotate Y axis matrix
;*                  F386_rotatez3D      - Concat a 3D rotate Z axis matrix
;*                  F386_rotate3D       - Concat a 3D rotate any axis matrix
;*                  F386_viewOrientation- Build a 3D view orienation matrix
;*                  F386_concat4x4      - Concatenate two 4x4 matrices
;*
;*              All of the 4x4 matrix routines take into account special
;*              case matrices to obtain higher performance operation. The
;*              special case flag is passed by the higher level interface.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros
include "fxmacs.mac"            ; Fixed point macros

header  fxmat3d             ; Set up memory model

begcodeseg  fxmat3d

;----------------------------------------------------------------------------
; void F386_scale3D(long *m,long xscale,long yscale,zscale)
;----------------------------------------------------------------------------
; Concatenate a scale transformation efficiently to the current
; transformation matrix. The bottom row is assumed to be <0,0,1>.
;----------------------------------------------------------------------------
cprocstart  F386_scale3D

        ARG     m:DPTR, xscale:ULONG, yscale:ULONG, zscale:ULONG

        enter_c

        mov     esi,[m]         ; esi -> xform matrix
        mov     ebx,[xscale]    ; EBX := x scale factor

off = 0
        REPT    4
        mov     eax,[esi+off]
        imul    ebx             ; EDX:EAX := mat[0][off] * xscale
        ROUNDIT
        mov     [esi+off],eax   ; Store result
off = off + 4
        ENDM

        mov     ebx,[yscale]    ; EBX := y scale factor

        REPT    4
        mov     eax,[esi+off]
        imul    ebx             ; EDX:EAX := mat[1][off] * yscale
        ROUNDIT
        mov     [esi+off],eax   ; Store result
off = off + 4
        ENDM

        mov     ebx,[zscale]    ; EBX := z scale factor

        REPT    4
        mov     eax,[esi+off]
        imul    ebx             ; EDX:EAX := mat[1][off] * yscale
        ROUNDIT
        mov     [esi+off],eax   ; Store result
off = off + 4
        ENDM

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_rotatex3D(long *T,long *m,long Sin,long Cos)
;----------------------------------------------------------------------------
; Concatenate a rotate transformation efficiently to the current
; transformation matrix. The bottom row is assumed to be <0,0,1>.
;----------------------------------------------------------------------------
cprocstart  F386_rotatex3D

        ARG     T:DPTR, m:DPTR, Sin:ULONG, Cos:ULONG

        enter_c

        mov     esi,[m]         ; esi -> xform matrix
        mov     edi,[T]         ; edi -> resultant temporary matrix

off = 0
        REPT    4
        mov     eax,[esi+off+16]
        imul    [Cos]
        mov     ecx,eax         ; EBX:ECX := mat[1][off] * Cos
        mov     ebx,edx
        mov     eax,[esi+off+32]
        imul    [Sin]
        sub     ecx,eax         ; EBX:ECX := mat[1][off] * Cos - mat[2][off] * Sin
        sbb     ebx,edx
        ROUNDIT_EBX_ECX
        mov     [edi+off],eax; Store result
off = off + 4
        ENDM

off = 0
        REPT    4
        mov     eax,[esi+off+16]
        imul    [Sin]
        mov     ecx,eax         ; EBX:ECX := mat[1][off] * Sin
        mov     ebx,edx
        mov     eax,[esi+off+32]
        imul    [Cos]
        add     eax,ecx         ; EDX:EAX := mat[2][off] * Cos + mat[1][off] * Sin
        adc     edx,ebx
        ROUNDIT
        mov     [edi+off+16],eax; Store result
off = off + 4
        ENDM

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_rotatey3D(long *T,long *m,long Sin,long Cos)
;----------------------------------------------------------------------------
; Concatenate a rotate transformation efficiently to the current
; transformation matrix. The bottom row is assumed to be <0,0,1>.
;----------------------------------------------------------------------------
cprocstart  F386_rotatey3D

        ARG     T:DPTR, m:DPTR, Sin:ULONG, Cos:ULONG

        enter_c

        mov     esi,[m]         ; esi -> xform matrix
        mov     edi,[T]         ; edi -> resultant temporary matrix

off = 0
        REPT    4
        mov     eax,[esi+off]
        imul    [Cos]
        mov     ecx,eax         ; EBX:ECX := mat[0][off] * Cos
        mov     ebx,edx
        mov     eax,[esi+off+32]
        imul    [Sin]
        sub     ecx,eax         ; EBX:ECX := mat[0][off] * Cos - mat[2][off] * Sin
        sbb     ebx,edx
        ROUNDIT_EBX_ECX
        mov     [edi+off],eax; Store result
off = off + 4
        ENDM

off = 0
        REPT    4
        mov     eax,[esi+off]
        imul    [Sin]
        mov     ecx,eax         ; EBX:ECX := mat[0][off] * Sin
        mov     ebx,edx
        mov     eax,[esi+off+32]
        imul    [Cos]
        add     eax,ecx         ; EDX:EAX := mat[2][off] * Cos + mat[0][off] * Sin
        adc     edx,ebx
        ROUNDIT
        mov     [edi+off+16],eax; Store result
off = off + 4
        ENDM

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_rotatez3D(long *T,long *m,long Sin,long Cos)
;----------------------------------------------------------------------------
; Concatenate a rotate transformation efficiently to the current
; transformation matrix. The bottom row is assumed to be <0,0,1>.
;----------------------------------------------------------------------------
cprocstart  F386_rotatez3D

        ARG     T:DPTR, m:DPTR, Sin:ULONG, Cos:ULONG

        enter_c

        mov     esi,[m]         ; esi -> xform matrix
        mov     edi,[T]         ; edi -> resultant temporary matrix

off = 0
        REPT    4
        mov     eax,[esi+off]
        imul    [Cos]
        mov     ecx,eax         ; EBX:ECX := mat[0][off] * Cos
        mov     ebx,edx
        mov     eax,[esi+off+16]
        imul    [Sin]
        sub     ecx,eax         ; EBX:ECX := mat[0][off] * Cos - mat[1][off] * Sin
        sbb     ebx,edx
        ROUNDIT_EBX_ECX
        mov     [edi+off],eax; Store result
off = off + 4
        ENDM

off = 0
        REPT    4
        mov     eax,[esi+off]
        imul    [Sin]
        mov     ecx,eax         ; EBX:ECX := mat[0][off] * Sin
        mov     ebx,edx
        mov     eax,[esi+off+16]
        imul    [Cos]
        add     eax,ecx         ; EDX:EAX := mat[1][off] * Cos + mat[0][off] * Sin
        adc     edx,ebx
        ROUNDIT
        mov     [edi+off+16],eax; Store result
off = off + 4
        ENDM

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_rotate3D(long *T,long *T2,long *m,long Sin,long Cos,long Cos_1,
;   long x,long y,long z)
;----------------------------------------------------------------------------
; Concatenate a rotate transformation about and arbitrary axis efficiently
; to the current transformation matrix. The bottom row is assumed to be
; <0,0,1>.
;----------------------------------------------------------------------------
cprocstart  F386_rotate3D

        ARG     T:DPTR, T2:DPTR, m:DPTR, Sin:ULONG, Cos:ULONG,          \
                Cos_1:ULONG, x:ULONG, y:ULONG, z:ULONG

        enter_c

        mov     edi,[T]         ; edi -> resultant temporary matrix

; Create the temporary 3x3 sub-rotation matrix

        mov     eax,[x]
        imul    eax
        ROUNDIT                 ; EAX := x*x
        imul    [Cos_1]
        ROUNDIT                 ; EAX := x*x*(1-Cos)
        add     eax,[Cos]       ; EAX := Cos + x*x(1-Cos)
        mov     [edi],eax       ; T[0][0] = EAX

        mov     eax,[x]
        imul    [y]
        ROUNDIT                 ; EAX := x*y
        imul    [Cos_1]
        ROUNDIT
        mov     ebx,eax         ; EBX := x*y*(1-Cos)
        mov     eax,[z]
        imul    [Sin]
        ROUNDIT                 ; EAX := z*Sin
        sub     ebx,eax         ; EBX := x*y*(1-Cos) - z*Sin
        mov     [edi+4],ebx     ; T[0][1] = EBX
        neg     ebx             ; EBX := -T[0][1]
        mov     [edi+12],ebx    ; T[1][0] = -T[0][1]

        mov     eax,[x]
        imul    [z]
        ROUNDIT                 ; EAX := x*z
        imul    [Cos_1]
        ROUNDIT
        mov     ebx,eax         ; EBX := x*z*(1-Cos)
        mov     eax,[y]
        imul    [Sin]
        ROUNDIT                 ; EAX := y*Sin
        add     eax,ebx         ; EAX := x*z*(1-Cos) + y*Sin
        mov     [edi+8],eax     ; T[0][2] = EAX
        neg     eax             ; EAX := -T[0][2]
        mov     [edi+24],eax    ; T[2][0] = -T[0][2]

        mov     eax,[y]
        imul    eax
        ROUNDIT                 ; EAX := y*y
        imul    [Cos_1]
        ROUNDIT                 ; EAX := y*y*(1-Cos)
        add     eax,[Cos]       ; EAX := Cos + y*y(1-Cos)
        mov     [edi+16],eax    ; T[1][1] = EAX

        mov     eax,[y]
        imul    [z]
        ROUNDIT                 ; EAX := y*z
        imul    [Cos_1]
        ROUNDIT
        mov     ebx,eax         ; EBX := y*z*(1-Cos)
        mov     eax,[x]
        imul    [Sin]
        ROUNDIT                 ; EAX := x*Sin
        sub     ebx,eax         ; EBX := y*z*(1-Cos) - x*Sin
        mov     [edi+20],ebx    ; T[1][2] = EBX
        neg     ebx             ; EBX := -T[1][2]
        mov     [edi+28],ebx    ; T[2][1] = -T[1][2]

        mov     eax,[z]
        imul    eax
        ROUNDIT                 ; EAX := z*z
        imul    [Cos_1]
        ROUNDIT                 ; EAX := z*z*(1-Cos)
        add     eax,[Cos]       ; EAX := Cos + z*z(1-Cos)
        mov     [edi+32],eax    ; T[2][2] = EAX

        mov     esi,[T]         ; esi -> sub-matrix T
        mov     edi,[m]         ; edi -> original transformation matrix

; Now concatentate this rotation matrix with the upper left 3x3
; sub-matrix of the current transformation matrix

roff=0                          ; Row offset starts at zero
troff=0
        REPT    3               ; Repeat once for all rows

coff=0                          ; Column offset starts at zero
        REPT    3               ; Do once for each column

        mov     eax,[esi+troff] ; EAX := T[troff][0]
        imul    [ULONG edi+coff]; EDX:EAX := T[troff][0] * m[0][coff]
        mov     ecx,eax
        mov     ebx,edx         ; EBX:ECX := running total

        mov     eax,[esi+troff+4]; EAX := T[troff][1]
        imul    [ULONG edi+16+coff]; EDX:EAX := T[troff][1] * m[1][coff]
        add     ecx,eax         ; Update running total
        adc     ebx,edx

        mov     eax,[esi+troff+8]; EAX := T[troff][2]
        imul    [ULONG edi+32+coff]; EDX:EAX := T[troff][2] * m[2][coff]
        add     eax,ecx         ; Update running total
        adc     edx,ebx
        ROUNDIT

        use_ds
        mov     ebx,[T2]
        mov     [ebx+roff+coff],eax ; T2[roff][coff] := EAX
        unuse_ds

coff=coff+4                     ; Point to next column offset

        ENDM                    ; End of inner column loop

roff=roff+16                    ; Point to next row
troff=troff+12

        ENDM                    ; End of outer row loop

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_concat4x4(long *result,long *m1,long *m2,bool special);
;----------------------------------------------------------------------------
; Concatenate (multiplies) the two fixed point 4x4 matrices together. If
; the matrix multiplication is special, then the bottom row of the matrix
; will be simply <0,0,0,1>.
;
; Note that we compute the result of each element in the matrix with 64 bit
; precision, and round the result once at the end (faster and more accurate).
;----------------------------------------------------------------------------
cprocstart  F386_concat4x4

        ARG     result:DPTR, m1:DPTR, m2:DPTR, special:BOOL

        enter_c

        mov     esi,[m1]        ; esi -> matrix m1
        mov     edi,[m2]        ; edi -> matrix m2

; Set up two imbedded macros to code the unrolled loops to multiply the
; matrices.

roff=0                          ; Row offset starts at zero
        REPT    3               ; Repeat once for all rows except last

coff=0                          ; Column offset starts at zero
        REPT    4               ; Do once for each column

        mov     eax,[esi+roff]  ; EAX := m1.mat[roff][0]
        imul    [ULONG edi+coff]; EDX:EAX := m1.mat[roff][0] * m2.mat[0][coff]
        mov     ecx,eax
        mov     ebx,edx         ; EBX:ECX := running total

        mov     eax,[esi+roff+4]; EAX := m1.mat[roff][1]
        imul    [ULONG edi+16+coff]; EDX:EAX := m1.mat[roff][1] * m2.mat[1][coff]
        add     ecx,eax         ; Update running total
        adc     ebx,edx

        mov     eax,[esi+roff+8]; EAX := m1.mat[roff][2]
        imul    [ULONG edi+32+coff]; EDX:EAX := m1.mat[roff][2] * m2.mat[2][coff]
        add     ecx,eax         ; Update running total
        adc     ebx,edx

        mov     eax,[esi+roff+12]; EAX := m1.mat[roff][3]
        imul    [ULONG edi+48+coff]; EDX:EAX := m1.mat[roff][3] * m2.mat[3][coff]
        add     eax,ecx         ; Update running total
        adc     edx,ebx
        ROUNDIT

        use_ds
        mov     ebx,[result]
        mov     [ebx+roff+coff],eax ; result.mat[roff][coff] := EAX
        unuse_ds

coff=coff+4                     ; Point to next column offset

        ENDM                    ; End of inner column loop

roff=roff+16                    ; Point to next row

        ENDM                    ; End of outer row loop

        cmp     [special],0     ; Do we have a special case matrix mult?
        je      @@ComputeBot    ; No, compute Bottom row of matrix

; Set the bottom row of the matrix to be <0,0,0,1>

        mov     ebx,[result]
        xor     eax,eax
        mov     [ebx+roff],eax  ; Store each zero
        mov     [ebx+roff+4],eax; Store each zero
        mov     [ebx+roff+8],eax; Store each zero
        or      eax,10000h
        mov     [ebx+roff+12],eax; Store the 1.0
        jmp     @@Exit

; Compute the bottom row for the matrix

@@ComputeBot:

coff=0                          ; Column offset starts at zero
        REPT    4               ; Do once for each column

        mov     eax,[esi+roff]  ; EAX := m1.mat[roff][0]
        imul    [ULONG edi+coff]; EDX:EAX := m1.mat[roff][0] * m2.mat[0][coff]
        mov     ecx,eax         ; Set running total
        mov     ebx,edx

        mov     eax,[esi+roff+4]; EAX := m1.mat[roff][1]
        imul    [ULONG edi+16+coff]; EDX:EAX := m1.mat[roff][1] * m2.mat[1][coff]
        add     ecx,eax         ; Update running total
        adc     ebx,edx

        mov     eax,[esi+roff+8]; EAX := m1.mat[roff][2]
        imul    [ULONG edi+32+coff]; EDX:EAX := m1.mat[roff][2] * m2.mat[2][coff]
        add     ecx,eax         ; Update running total
        adc     ebx,edx

        mov     eax,[esi+roff+12]; EAX := m1.mat[roff][3]
        imul    [ULONG edi+48+coff]; EDX:EAX := m1.mat[roff][3] * m2.mat[3][coff]
        add     eax,ecx         ; Update running total
        adc     edx,ebx
        ROUNDIT

        mov     ebx,[result]
        mov     [ebx+roff+coff],eax ; result.mat[roff][coff] := EAX

coff=coff+4                     ; Point to next column offset

        ENDM                    ; End of inner column loop

@@Exit: leave_c
        ret

cprocend

endcodeseg  fxmat3d

        END                     ; End of module
