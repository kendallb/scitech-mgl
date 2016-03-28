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
;*              fixed point 2D vector mapping routines:
;*
;*                  F386_map2D          - Map a 2D point with 3x3 matrix
;*                  F386_mapVec2D       - Map a 2D vector with 3x3 matrix
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros
include "fxmacs.mac"            ; Fixed point macros

header  fxmap2d                 ; Set up memory model

begcodeseg  fxmap2d

;----------------------------------------------------------------------------
; void F386_map2D(long *m,long *result,long *p,bool special);
;----------------------------------------------------------------------------
; Maps a 2D point 'p' by the 3x3 transformation matrix 'm' to obtain the
; resultant point 'result'. The bottom row of the matrix is implicitly
; set to <0,0,1> and we dont include it in the multiplication.
;----------------------------------------------------------------------------
cprocstart  F386_map2D

        ARG     m:DPTR, result:DPTR, p:DPTR, special:BOOL

        enter_c

        mov     esi,[m]         ; ESI -> transform matrix
        mov     edi,[p]         ; EDI -> point to transform
        test    [special],1
        jnz     @@DoSpecial     ; We have a special case mapping!

        mov     eax,[esi]       ; EAX := mat[0][0]
        imul    [ULONG edi]
        mov     ecx,eax         ; EBX:ECX := p.x * mat[0][0]
        mov     ebx,edx

        mov     eax,[esi+4]     ; EAX := mat[0][1]
        imul    [ULONG edi+4]
        add     eax,ecx         ; EDX:EAX := p.x * mat[0][0] + p.y * mat[0][1]
        adc     edx,ebx
        ROUNDIT
        add     eax,[esi+8]     ; EAX := p.x * mat[0][0] + p.y * mat[0][1]
                                ;        + mat[0][2] = result.x
        push    eax

        mov     eax,[esi+12]    ; EAX := mat[1][0]
        imul    [ULONG edi]     ; EDX:EAX := p.x * mat[1][0]
        mov     ecx,eax         ; EBX:ECX := p.x * mat[1][0]
        mov     ebx,edx

        mov     eax,[esi+12+4]  ; EAX := mat[1][1]
        imul    [ULONG edi+4]   ; EDX:EAX = p.y * mat[1][1]
        add     eax,ecx         ; EDX:EAX := p.x * mat[1][0] + p.y * mat[1][1]
        adc     edx,ebx
        ROUNDIT
        add     eax,[esi+12+8]  ; EAX := p.x * mat[1][0] + p.y * mat[1][1]
                                ;        + mat[1][2] = result.y
        pop     ebx
        jmp     @@StoreResult

@@DoSpecial:
        mov     eax,[esi]       ; EAX := mat[0][0]
        imul    [ULONG edi]     ; EDX:EAX := p.x * mat[0][0]
        ROUNDIT                 ; EAX := rounded result
        mov     ebx,eax         ; EBX := p.x * mat[0][0]
        add     ebx,[esi+8]     ; EBX := p.x * mat[0][0] + mat[0][2] = result.x

        mov     eax,[esi+12+4]  ; EAX := mat[1][1]
        imul    [ULONG edi+4]   ; EDX:EAX = p.y * mat[1][1]
        ROUNDIT                 ; EAX := rounded result
        add     eax,[esi+12+8]  ; EAX := p.y * mat[1][1] + mat[1][2] = result.y

@@StoreResult:
        mov     esi,[result]    ; esi -> resultant point

        mov     [esi],ebx       ; Store result.x
        mov     [esi+4],eax     ; Store result.y

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void F386_mapVec2D(long *m,long *result,long *v,bool special);
;----------------------------------------------------------------------------
; Maps a 2D vector 'v' by the 3x3 transformation matrix 'm' to obtain the
; resultant vector 'result'. The bottom row of the matrix is implicitly
; set to <0,0,1> and we dont include it in the multiplication. Note that
; when mapping vectors any translational components of the matrix,
; must be left out of the calculation otherwise the resulting vector will
; incorrect.
;----------------------------------------------------------------------------
cprocstart  F386_mapVec2D

        ARG     m:DPTR, result:DPTR, v:DPTR, special:BOOL

        enter_c

        mov     esi,[m]         ; esi -> transform matrix
        mov     edi,[v]         ; edi -> point to transform
        test    [special],1
        jnz     @@DoSpecial     ; We have a special case mapping!

        mov     eax,[esi]       ; EAX := mat[0][0]
        imul    [ULONG edi]
        mov     ecx,eax         ; EBX:ECX := p.x * mat[0][0]
        mov     ebx,edx

        mov     eax,[esi+4]     ; EAX := mat[0][1]
        imul    [ULONG edi+4]
        add     eax,ecx         ; EDX:EAX := p.x * mat[0][0] + p.y * mat[0][1]
        adc     edx,ebx
        ROUNDIT
        push    eax

        mov     eax,[esi+12]    ; EAX := mat[1][0]
        imul    [ULONG edi]     ; EDX:EAX := p.x * mat[1][0]
        mov     ecx,eax         ; EBX:ECX := p.x * mat[1][0]
        mov     ebx,edx

        mov     eax,[esi+12+4]  ; EAX := mat[1][1]
        imul    [ULONG edi+4]   ; EDX:EAX = p.y * mat[1][1]
        add     eax,ecx         ; EDX:EAX := p.x * mat[1][0] + p.y * mat[1][1]
        adc     edx,ebx
        ROUNDIT
        pop     ebx
        jmp     @@StoreResult

@@DoSpecial:
        mov     eax,[esi]       ; EAX := mat[0][0]
        imul    [ULONG edi]     ; EDX:EAX := p.x * mat[0][0]
        ROUNDIT                 ; EAX := rounded result
        mov     ebx,eax         ; EBX := p.x * mat[0][0]

        mov     eax,[esi+12+4]  ; EAX := mat[1][1]
        imul    [ULONG edi+4]   ; EDX:EAX = p.y * mat[1][1]
        ROUNDIT                 ; EAX := rounded result

@@StoreResult:
        mov     esi,[result]    ; esi -> resultant point

        mov     [esi],ebx       ; Store result.x
        mov     [esi+4],eax     ; Store result.y

        leave_c
        ret

cprocend

endcodeseg  fxmap2d

        END                     ; End of module
