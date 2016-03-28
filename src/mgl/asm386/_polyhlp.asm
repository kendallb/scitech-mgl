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
;* Description: 32 bit optimised assembly language routines for the polygon
;*              rendering helper functions.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  _polyhlp                ; Setup for MGL hybrid 16/32 bit segment

begcodeseg  _polyhlp

;----------------------------------------------------------------------------
; int _MGL_computeSlope(fxpoint_t *v1,fxpoint_t *v2,fix32_t *slope)
;----------------------------------------------------------------------------
; Compute the slope of the edge and return the sign of the slope.
;
; Entry:        v1      - First vertex in edge
;               v2      - Second vertex in edge
;               slope   - Pointer to place to store slope
;
; Exit:         _AX     - Sign of slope (1 = +ve, 0, -1 = i-ve)
;
;----------------------------------------------------------------------------
cprocstart  _MGL_computeSlope

        ARG     v1:DPTR, v2:DPTR, slope:DPTR

        push    ebp
        mov     ebp,esp
        push    ebx
        push    esi

        mov     ecx,[v2]
        mov     ebx,[v1]
        mov     esi,[ecx+4]
        sub     esi,[ebx+4]     ; ESI := v2.y - v1.y
        jle     @@InvalidEdge   ; Check for negative/0 edges
        cmp     esi,10000h
        jle     @@QuickSlope    ; Handle divide overflow

        mov     eax,[ecx]
        sub     eax,[ebx]       ; EAX := v2.x - v1.x
        mov     edx,eax
        xor     eax,eax
        mov     ebx,[slope]
        shrd    eax,edx,16      ; position so that result ends up
        sar     edx,16          ; in EAX
        idiv    esi
        mov     [ebx],eax       ; Store the resulting slope
        mov     eax,1           ; Positive edge

@@Exit:
        pop     esi
        pop     ebx
        pop     ebp
        ret

@@QuickSlope:
        mov     eax,[ecx]
        sub     eax,[ebx]       ; EAX := v2.x - v1.x
        mov     ebx,[slope]
        mov     [ebx],eax       ; Store the resulting slope
        mov     eax,1           ; Positive edge
        jmp     @@Exit

@@InvalidEdge:
        mov     eax,-1          ; Negative edge
        jl      @@Exit
        xor     eax,eax         ; Zero height edge
        jmp     @@Exit

cprocend

endcodeseg  _polyhlp

        END                     ; End of module
