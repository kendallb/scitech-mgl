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
;* Environment: Win32
;*
;* Description: Low level assembly support for the PM library specific
;*              to Windows.
;*
;****************************************************************************

include "scitech.mac"               ; Memory model macros

header      _pmwin32                    ; Set up memory model

begdataseg  _pmwin32

        cglobal _PM_ioentry
        cglobal _PM_gdt
_PM_ioentry     dd  0               ; Offset to call gate
_PM_gdt         dw  0               ; Selector to call gate

enddataseg  _pmwin32

begcodeseg  _pmwin32                    ; Start of code segment

;----------------------------------------------------------------------------
; int PM_setIOPL(int iopl)
;----------------------------------------------------------------------------
; Change the IOPL level for the 32-bit task. Returns the previous level
; so it can be restored for the task correctly.
;----------------------------------------------------------------------------
cprocstart  _PM_setIOPLViaCallGate

        ARG     iopl:UINT

        enter_c
        pushfd                      ; Save the old EFLAGS for later
        mov     ecx,[iopl]          ; ECX := IOPL level
        xor     ebx,ebx             ; Change IOPL level function code
        call    far dword [_PM_ioentry]
        pop     eax
        and     eax,0011000000000000b
        shr     eax,12
        leave_c
        ret

cprocend

endcodeseg  _pmwin32

        END                         ; End of module

