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
;* Environment: IBM PC Real mode and 16/32 bit protected mode
;*
;* Description: Low level assembly support for the PM library specific to
;*              MSDOS.
;*
;****************************************************************************

include "scitech.mac"               ; Memory model macros

header      _pmdos                  ; Set up memory model

begdataseg  _pmdos

ifdef flatmodel
    cextern _PM_savedDS,USHORT
    cextern _PM_VXD_off,UINT
    cextern _PM_VXD_sel,UINT
ifdef   DOS4GW
    cextern _PM_haveCauseWay,UINT
endif
endif
intel_id        db  "GenuineIntel"  ; Intel vendor ID

PMHELP_GETPDB       EQU 0026h
PMHELP_FLUSHTLB     EQU 0027h

enddataseg  _pmdos

P586

begcodeseg  _pmdos                  ; Start of code segment

;----------------------------------------------------------------------------
; void PM_segread(PMSREGS *sregs)
;----------------------------------------------------------------------------
; Read the current value of all segment registers
;----------------------------------------------------------------------------
cprocstartdll16 PM_segread

        ARG     sregs:DPTR

        enter_c

        mov     ax,es
        _les    _si,[sregs]
        mov     [_ES _si],ax
        mov     [_ES _si+2],cs
        mov     [_ES _si+4],ss
        mov     [_ES _si+6],ds
        mov     [_ES _si+8],fs
        mov     [_ES _si+10],gs

        leave_c
        ret

cprocend

; Create a table of the 256 different interrupt calls that we can jump
; into

%assign intno 0

intTable:
%rep    256
        db      0CDh
        db      intno
%assign intno   intno + 1
        ret
        nop
%endrep

;----------------------------------------------------------------------------
; _PM_genInt    - Generate the appropriate interrupt
;----------------------------------------------------------------------------
cprocnear   _PM_genInt

        push    _ax                     ; Save _ax
        push    _bx                     ; Save _bx
ifdef flatmodel
        mov     ebx,[UINT esp+12]       ; EBX := interrupt number
else
        mov     bx,sp                   ; Make sure ESP is zeroed
        mov     bx,[UINT ss:bx+6]       ; BX := interrupt number
endif
        mov     _ax,offset intTable     ; Point to interrupt generation table
        shl     _bx,2                   ; _BX := index into table
        add     _ax,_bx                 ; _AX := pointer to interrupt code
ifdef flatmodel
        xchg    eax,[esp+4]             ; Restore eax, and set for int
else
        mov     bx,sp
        xchg    ax,[ss:bx+2]            ; Restore ax, and set for int
endif
        pop     _bx                     ; restore _bx
        ret

cprocend

;----------------------------------------------------------------------------
; int PM_int386x(int intno, PMREGS *in, PMREGS *out,PMSREGS *sregs)
;----------------------------------------------------------------------------
; Issues a software interrupt in protected mode. This routine has been
; written to allow user programs to load CS and DS with different values
; other than the default.
;----------------------------------------------------------------------------
cprocstartdll16 PM_int386x

        ARG     intno:UINT, inptr:DPTR, outptr:DPTR, sregs:DPTR

        LOCAL   flags:UINT, sv_ds:UINT, sv_esi:ULONG = LocalSize

        enter_c
        push    ds
        push    es                  ; Save segment registers
        push    fs
        push    gs

        _lds    _si,[sregs]         ; DS:_SI -> Load segment registers
        mov     es,[_si]
        mov     bx,[_si+6]
        mov     [sv_ds],_bx         ; Save value of user DS on stack
        mov     fs,[_si+8]
        mov     gs,[_si+10]

        _lds    _si,[inptr]         ; Load CPU registers
        mov     eax,[_si]
        mov     ebx,[_si+4]
        mov     ecx,[_si+8]
        mov     edx,[_si+12]
        mov     edi,[_si+20]
        mov     esi,[_si+16]

        push    ds                  ; Save value of DS
        push    _bp                 ; Some interrupts trash this!
        clc                         ; Generate the interrupt
        push    [UINT intno]
        mov     ds,[WORD sv_ds]     ; Set value of user's DS selector
        call    _PM_genInt
        pop     _bp                 ; Pop intno from stack (flags unchanged)
        pop     _bp                 ; Restore value of stack frame pointer
        pop     ds                  ; Restore value of DS

        pushf                       ; Save flags for later
        pop     [UINT flags]
        push    esi                 ; Save ESI for later
        pop     [DWORD sv_esi]
        push    ds                  ; Save DS for later
        pop     [UINT sv_ds]

        _lds    _si,[outptr]        ; Save CPU registers
        mov     [_si],eax
        mov     [_si+4],ebx
        mov     [_si+8],ecx
        mov     [_si+12],edx
        push    [DWORD sv_esi]
        pop     [DWORD _si+16]
        mov     [_si+20],edi

        mov     _bx,[flags]         ; Return flags
        and     ebx,1h              ; Isolate carry flag
        mov     [_si+24],ebx        ; Save carry flag status

        _lds    _si,[sregs]         ; Save segment registers
        mov     [_si],es
        mov     _bx,[sv_ds]
        mov     [_si+6],bx          ; Get returned DS from stack
        mov     [_si+8],fs
        mov     [_si+10],gs

        pop     gs                  ; Restore segment registers
        pop     fs
        pop     es
        pop     ds
        leave_c
        ret

cprocend

ifndef flatmodel
_PM_savedDS     dw  _DATA           ; Saved value of DS
endif

;----------------------------------------------------------------------------
; void PM_saveDS(void)
;----------------------------------------------------------------------------
; Save the value of DS into a section of the code segment, so that we can
; quickly load this value at a later date in the PM_loadDS() routine from
; inside interrupt handlers etc. The method to do this is different
; depending on the DOS extender being used.
;----------------------------------------------------------------------------
cprocstartdll16 PM_saveDS

ifdef flatmodel
        mov     [_PM_savedDS],ds    ; Store away in data segment
endif
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_loadDS(void)
;----------------------------------------------------------------------------
; Routine to load the DS register with the default value for the current
; DOS extender. Only the DS register is loaded, not the ES register, so
; if you wish to call C code, you will need to also load the ES register
; in 32 bit protected mode.
;----------------------------------------------------------------------------
cprocstartdll16 PM_loadDS

        mov     ds,[cs:_PM_savedDS] ; We can access the proper DS through CS
        ret

cprocend

ifdef flatmodel

;----------------------------------------------------------------------------
; ibool DPMI_allocateCallback(void (*pmcode)(), void *rmregs, long *RMCB)
;----------------------------------------------------------------------------
cprocstart  _DPMI_allocateCallback

        ARG     pmcode:CPTR, rmregs:DPTR, RMCB:DPTR

        enter_c
        push    ds
        push    es

        push    cs
        pop     ds
        mov     esi,[pmcode]    ; DS:ESI -> protected mode code to call
        mov     edi,[rmregs]    ; ES:EDI -> real mode register buffer
        mov     ax,303h         ; AX := allocate realmode callback function
        int     31h
        mov     eax,0           ; Return failure!
        jc      @@Fail

        mov     eax,[RMCB]
        shl     ecx,16
        mov     cx,dx
        mov     [es:eax],ecx    ; Return real mode address
        mov     eax,1           ; Return success!

@@Fail: pop     es
        pop     ds
        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; void DPMI_freeCallback(long RMCB)
;----------------------------------------------------------------------------
cprocstart  _DPMI_freeCallback

        ARG     RMCB:ULONG

        enter_c

        mov     cx,[WORD RMCB+2]
        mov     dx,[WORD RMCB]  ; CX:DX := real mode callback
        mov     ax,304h
        int     31h

        leave_c
        ret

cprocend

endif

; Macro to delay briefly to ensure that enough time has elapsed between
; successive I/O accesses so that the device being accessed can respond
; to both accesses even on a very fast PC.

%macro  DELAY 0
        jmp     short $+2
        jmp     short $+2
        jmp     short $+2
%endmacro

%macro  IODELAYN 1
%rep    %1
        DELAY
%endrep
%endmacro

;----------------------------------------------------------------------------
; uchar _PM_readCMOS(int index)
;----------------------------------------------------------------------------
; Read the value of a specific CMOS register. We do this with both
; normal interrupts and NMI disabled.
;----------------------------------------------------------------------------
cprocstart  _PM_readCMOS

        ARG     index:UINT

        push    _bp
        mov     _bp,_sp
        pushfd
        mov     al,[BYTE index]
        or      al,80h              ; Add disable NMI flag
        cli
        out     70h,al
        IODELAYN 5
        in      al,71h
        mov     ah,al
        xor     al,al
        IODELAYN 5
        out     70h,al              ; Re-enable NMI
        sti
        mov     al,ah               ; Return value in AL
        popfd
        pop     _bp
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_writeCMOS(int index,uchar value)
;----------------------------------------------------------------------------
; Read the value of a specific CMOS register. We do this with both
; normal interrupts and NMI disabled.
;----------------------------------------------------------------------------
cprocstart  _PM_writeCMOS

        ARG     index:UINT, value:UCHAR

        push    _bp
        mov     _bp,_sp
        pushfd
        mov     al,[BYTE index]
        or      al,80h              ; Add disable NMI flag
        cli
        out     70h,al
        IODELAYN 5
        mov     al,[value]
        out     71h,al
        xor     al,al
        IODELAYN 5
        out     70h,al              ; Re-enable NMI
        sti
        popfd
        pop     _bp
        ret

cprocend

ifdef   flatmodel

;----------------------------------------------------------------------------
; int _PM_pagingEnabled(void)
;----------------------------------------------------------------------------
; Returns 1 if paging is enabled, 0 if not or -1 if not at ring 0
;----------------------------------------------------------------------------
cprocstart  _PM_pagingEnabled

        mov     eax,-1
ifdef   DOS4GW
        mov     cx,cs
        and     ecx,3
        jz      @@Ring0
        cmp     [UINT _PM_haveCauseWay],0
        jnz     @@Ring0
        jmp     @@Exit

@@Ring0:
        mov     eax,cr0                 ; Load CR0
        shr     eax,31                  ; Isolate paging enabled bit
endif
@@Exit: ret

cprocend

;----------------------------------------------------------------------------
; _PM_getPDB - Return the Page Table Directory Base address
;----------------------------------------------------------------------------
cprocstart  _PM_getPDB

ifdef   DOS4GW
        mov     ax,cs
        and     eax,3
        jz      @@Ring0
        cmp     [UINT _PM_haveCauseWay],0
        jnz     @@Ring0
endif

; Call VxD if running at ring 3 in a DOS box

        cmp     [WORD _PM_VXD_sel],0
        jz      @@Fail
        mov     eax,PMHELP_GETPDB
        call    far dword [_PM_VXD_off]
        ret

@@Ring0:
ifdef   DOS4GW
        mov     eax,cr3
        and     eax,0FFFFF000h
        ret
endif
@@Fail: xor     eax,eax
        ret

cprocend

;----------------------------------------------------------------------------
; PM_flushTLB - Flush the Translation Lookaside buffer
;----------------------------------------------------------------------------
cprocstart  PM_flushTLB

        mov     ax,cs
        and     eax,3
        jz      @@Ring0
ifdef   DOS4GW
        cmp     [UINT _PM_haveCauseWay],0
        jnz     @@Ring0
endif

; Call VxD if running at ring 3 in a DOS box

        cmp     [WORD _PM_VXD_sel],0
        jz      @@Fail
        mov     eax,PMHELP_FLUSHTLB
        call    far dword [_PM_VXD_off]
        ret

@@Ring0:
ifdef   DOS4GW
        wbinvd                  ; Flush the CPU cache
        mov     eax,cr3
        mov     cr3,eax         ; Flush the TLB
endif
@@Fail: ret

cprocend

endif

;----------------------------------------------------------------------------
; _PM_enableSSE - Enable SSE extensions in the processor
;----------------------------------------------------------------------------
; This function sets up the SSE/SSE2 features in the processor. To do this
; we must enable bit 9 (OSFXSR) of CR4 to enable SSE. We also make sure that
; bit 10 (OSXMMEXCPT) is set to 0, since we will not be installing a handler
; for SIMD Floating Point Exceptions (#XF). We will then get an exception 06h
; if a SIMD exception does occur. Note all this code is ring 0 so will not
; work in a DOS box (does work with CauseWay however).
;----------------------------------------------------------------------------
cprocstart  _PM_enableSSE

        mov     eax,cr4
        or      eax,0200h
        and     eax,0FFFFFBFFh
        mov     cr4,eax
        mov     eax,cr0
        ret

cprocend

;----------------------------------------------------------------------------
; void _PM_VxDCall(VXD_regs far *r,uint off,uint sel);
;----------------------------------------------------------------------------
cprocstart  _PM_VxDCall

        ARG     r:DPTR, off:UINT, sel:UINT

        enter_c

; Load all registers from the registers structure

        mov     ebx,[r]
        mov     eax,[ebx+0]
        mov     ecx,[ebx+8]
        mov     edx,[ebx+12]
        mov     esi,[ebx+16]
        mov     edi,[ebx+20]
        mov     ebx,[ebx+4]         ; Trashes BX structure pointer!

; Call the VxD entry point (on stack)

        call    far dword [off]

; Save all registers back in the structure

        push    ebx                 ; Push EBX onto stack for later
        mov     ebx,[r]
        mov     [ebx+0],eax
        mov     [ebx+8],ecx
        mov     [ebx+12],edx
        mov     [ebx+16],esi
        mov     [ebx+20],edi
        pop     [DWORD ebx+4]       ; Save value of EBX from stack

        leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; int PM_haveCauseway(void);
;----------------------------------------------------------------------------
; Detects if CauseWay is the DOS extender
;----------------------------------------------------------------------------
cprocstart  PM_haveCauseway

        pushad

; Are we running under CauseWay?

        mov     ax,0FFF9h
        int     31h
        jc      @@NotCauseway
        cmp     ecx,"CAUS"
        jnz     @@NotCauseway
        cmp     edx,"EWAY"
        jnz     @@NotCauseway

        mov     eax,1
        jmp     @@Exit

@@NotCauseway:
        xor     eax,eax

@@Exit: popad
        ret

cprocend

endcodeseg  _pmdos

        END                     ; End of module
