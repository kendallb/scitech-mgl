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
;* Description: Assembly language memory block clearing routines.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  _memset

begcodeseg  _memset

;----------------------------------------------------------------------------
; void MGL_memset(void *p,int c,uint n)
;----------------------------------------------------------------------------
; Set a block of 'n' words to the value 'c'. The number of bytes can be no
; more than 64k in 16 bit real mode, but can be any size in 32 bit modes.
;----------------------------------------------------------------------------
cprocstart  MGL_memset

        ARG     p:DPTR, c:UINT, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     ecx,[n]
        mov     al,[BYTE c]
        mov     ah,al           ; AX := value to store
        mov     ebx,eax
        shl     eax,16
        mov     ax,bx           ; EAX := value to store
        cld

@@ForceAlignment:
        test    edi,3
        jz      @@Aligned
        mov     [edi],al
        inc     edi
        dec     ecx
        jnz     @@ForceAlignment

@@Aligned:
        push    ecx
        shr     ecx,2
    rep stosd                   ; Store all middle WORD's fast!
        pop     ecx
        and     ecx,3
    rep stosb                   ; Store the last bytes if any

@@Done: leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void MGL_memsetw(void *p,int c,uint n)
;----------------------------------------------------------------------------
; Set a block of 'n' words to the value 'c'. The number of bytes can be no
; more than 64k in 16 bit real mode, but can be any size in 32 bit modes.
;----------------------------------------------------------------------------
cprocstart  MGL_memsetw

        ARG     p:DPTR, c:UINT, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     ecx,[n]
        mov     ax,[WORD c]
        mov     ebx,eax
        shl     eax,16
        mov     ax,bx           ; EAX := value to store
        cld

        test    edi,1
        jz      @@Aligned
        jcxz    @@Done          ; Exit if count of 0
        stosw                   ; Force dword alignment
        dec     ecx
@@Aligned:
        shr     ecx,1
    rep stosd                   ; Store all middle DWORD's fast!
        adc     ecx,ecx
    rep stosw                   ; Store the last word if any

@@Done: leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void MGL_memsetl(void *p,long c,uint n)
;----------------------------------------------------------------------------
; Set a block of 'n' double words to the value 'c'. The number of double
; words can be no more than 64k in 16 bit modes, but can be any size in 32
; bit modes.
;----------------------------------------------------------------------------
cprocstart  MGL_memsetl

        ARG     p:DPTR, c:ULONG, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     ecx,[n]
        mov     eax,[c]         ; EAX := value to store
        cld
    rep stosd                   ; Store all middle DWORD's fast!
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void _MGL_memcpy(void *p,void *s,uint n)
;----------------------------------------------------------------------------
cprocstart  MGL_memcpy

        ARG     p:DPTR, s:DPTR, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     esi,[s]         ; ESI -> memory block
        mov     ecx,[n]
        cld

        mov     eax,ecx
        shr     ecx,2
    rep movsd
        mov     ecx,eax
        and     ecx,3
    rep movsb

@@Done: leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void MGL_memcpyVIRTSRC(void *p,void *s,uint n)
;----------------------------------------------------------------------------
cprocstart  MGL_memcpyVIRTSRC

        ARG     p:DPTR, s:DPTR, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     esi,[s]         ; ESI -> memory block
        mov     ecx,[n]
        cld

; Force DWORD alignment for transfers in a virtualised buffer

@@ForceAlignment:
        test    esi,3
        jz      @@Start
        movsb
        dec     ecx
        jnz     @@ForceAlignment

@@Start:
        mov     eax,ecx
        shr     ecx,2
    rep movsd
        mov     ecx,eax
        and     ecx,3
    rep movsb

@@Done: leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void MGL_memcpyVIRTDST(void *p,void *s,uint n)
;----------------------------------------------------------------------------
cprocstart  MGL_memcpyVIRTDST

        ARG     p:DPTR, s:DPTR, n:UINT

        enter_c

        mov     edi,[p]         ; EDI -> memory block
        mov     esi,[s]         ; ESI -> memory block
        mov     ecx,[n]
        cld

; Force DWORD alignment for transfers in a virtualised buffer

@@ForceAlignment:
        test    edi,3
        jz      @@Start
        movsb
        dec     ecx
        jnz     @@ForceAlignment

@@Start:
        mov     eax,ecx
        shr     ecx,2
    rep movsd
        mov     ecx,eax
        and     ecx,3
    rep movsb

@@Done: leave_c
        ret

cprocend

endcodeseg      _memset

        END                     ; End of module
