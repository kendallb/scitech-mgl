;****************************************************************************
;*
;*                  SciTech OS Portability Manager Library
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
;* Environment: Intel 32 bit Protected Mode.
;*
;* Description: Helper assembler functions for the PE loader
;*
;****************************************************************************

include "scitech.mac"

header      _peloader

begcodeseg  _peloader                  ; Start of code segment

;----------------------------------------------------------------------------
; void EnterDebuggerWithMessage( const char *msg );
;----------------------------------------------------------------------------
; Return the CS selector value
;----------------------------------------------------------------------------
cprocstart  _PE_enterDebuggerWithMessage

        ARG     msg:DPTR

        enter_c
        mov     ax,ss
        and     eax,0FFFFh
        mov     edx,[msg]
        push    eax
        push    edx
        db      0CCh
        jmp short @L1
        db      'W' 
        db      'V' 
        db      'I' 
        db      'D'
        db      'E'
        db      'O'
@L1:    pop     edx
        pop     eax    
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; ushort PM_getCS(void);
;----------------------------------------------------------------------------
; Return the CS selector value
;----------------------------------------------------------------------------
cprocstart  _PE_getCS

        mov     ax,cs
        ret

cprocend

endcodeseg  _peloader

        END
