;****************************************************************************
;*
;*                         SciTech Display Doctor
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
;* Language:    80386 Assembler
;* Environment: Intel x86
;*
;* Description: Assembly language test routines for the Nucleus
;*              performance profiling program.
;*
;****************************************************************************

        IDEAL

include "scitech.mac"

header  _profile                    ; Set up memory model

begcodeseg  _profile

;----------------------------------------------------------------------------
; void clearBufLin(void *buffer,long value,uint len)
;----------------------------------------------------------------------------
cprocstart   clearBufLin

		ARG     buffer:DPTR, value:ULONG, len:UINT

		enter_c

		mov     edi,[buffer]    ; edi -> memory block
		mov     eax,[value]     ; EAX := value to store
		mov		ah,al
		mov		ecx,eax
		shl		ecx,16
		or		eax,ecx
		mov     ecx,[len]
		shr     ecx,2           ; Convert from bytes to DWORDS
		cld
	rep stosd                   ; Store all middle DWORD's fast!

		leave_c
		ret

cprocend

;----------------------------------------------------------------------------
; void clearBufDownLin(void *buffer,long value,uint len)
;----------------------------------------------------------------------------
cprocstart   clearBufDownLin

		ARG     buffer:DPTR, value:ULONG, len:UINT

		enter_c

		mov     edi,[buffer]    ; edi -> memory block
		mov     eax,[value]     ; EAX := value to store
		mov		ah,al
		mov		ecx,eax
		shl		ecx,16
		or		eax,ecx
		mov     ecx,[len]
		add		edi,ecx
		sub		edi,4			; edi -> end of memory block
		shr     ecx,2           ; Convert from bytes to DWORDS
		std						; Go down in video memory
	rep stosd                   ; Store all middle DWORD's fast!

		leave_c
		ret

cprocend

;----------------------------------------------------------------------------
; void readBufLin(void *buffer,uint len)
;----------------------------------------------------------------------------
cprocstart   readBufLin

		ARG     buffer:DPTR, len:UINT

		enter_c

		mov     edi,[buffer]    ; edi -> memory block
		mov     ecx,[len]
		shr		ecx,2			; Convert to DWORDs

@@Loop:	mov		eax,[edi]
		add		edi,4
		dec		ecx
		jnz		@@Loop

		leave_c
		ret

cprocend

;----------------------------------------------------------------------------
; void copyBufLin(void *buffer,char *image,uint len)
;----------------------------------------------------------------------------
cprocstart	copyBufLin

		ARG     buffer:DPTR, image:DPTR, len:UINT

		enter_c

		mov     edi,[buffer]    ; edi -> destination block
		mov     esi,[image]     ; esi -> source block
		mov     ecx,[len]
		shr     ecx,2           ; Convert from bytes to DWORDS
		cld
	rep movsd                   ; Copy all DWORDS's fast!

		leave_c
		ret

cprocend

endcodeseg  _profile

		END
