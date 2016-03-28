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
;* Environment: 16/32 bit Ring 0 device driver
;*
;* Description: Assembler support routines for ISA DMA controller.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header      _dma                ; Set up memory model

begdataseg  _dma                ; Start of data segment

cpublic _PM_DMADataStart

; DMA register I/O addresses for channels 0-7 (except 4)

DMAC_page       db 087h,083h,081h,082h, -1,08Bh,089h,08Ah
DMAC_addr       db 000h,002h,004h,006h, -1,0C4h,0C8h,0CCh
DMAC_cnt        db 001h,003h,005h,007h, -1,0C6h,0CAh,0CEh
DMAC_mask       db 00Ah,00Ah,00Ah,00Ah, -1,0D4h,0D4h,0D4h
DMAC_mode       db 00Bh,00Bh,00Bh,00Bh, -1,0D6h,0D6h,0D6h
DMAC_FF         db 00Ch,00Ch,00Ch,00Ch, -1,0D8h,0D8h,0D8h

cpublic _PM_DMADataEnd

enddataseg  _dma

begcodeseg  _dma                ; Start of code segment

ifdef   flatmodel

cpublic _PM_DMACodeStart

;----------------------------------------------------------------------------
; void PM_DMACDisable(int channel);
;----------------------------------------------------------------------------
; Masks DMA channel, inhibiting DMA transfers
;----------------------------------------------------------------------------
cprocstart  PM_DMACDisable

        ARG     channel:UINT

        push    ebp
        mov     ebp,esp
        mov     ecx,[channel]       ; ECX indexes DMAC register tables
        mov     dh,0                ; DH = 0 for DMAC register port access
        mov     al,cl
        and     al,11b
        or      al,100b             ; AL = (channel & 3) | "set mask bit"
        mov     dl,[DMAC_mask+ecx]
        out     dx,al
        pop     ebp
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_DMACEnable(int channel);
;----------------------------------------------------------------------------
; Unmasks DMA channel, enabling DMA transfers
;----------------------------------------------------------------------------
cprocstart  PM_DMACEnable

        ARG     channel:UINT

        push    ebp
        mov     ebp,esp
        mov     ecx,[channel]       ; ECX indexes DMAC register tables
        mov     dh,0                ; DH = 0 for DMAC register port access
        mov     al,cl
        and     al,11b              ; AL = (channel & 3), "set mask bit"=0
        mov     dl,[DMAC_mask+ecx]
        out     dx,al
        pop     ebp
        ret

cprocend

;----------------------------------------------------------------------------
; void PM_DMACProgram(int channel,int mode,ulong bufferPhys,int count);
;----------------------------------------------------------------------------
; Purpose: Program DMA controller to perform transfer from first 16MB
; based on previously selected mode and channel. DMA transfer may be enabled
; by subsequent call to PM_DMACEnable.
;
; Entry:    channel - DMA channel in use (0-7)
;           mode    - Selected DMAMODE type for transfer
;           buffer  - 32-bit physical address of DMA buffer
;           count   - DMA byte count (1-65536 bytes)
;----------------------------------------------------------------------------
cprocstart  PM_DMACProgram

        ARG     channel:UINT, mode:UINT, bufferPhys:ULONG, count:UINT

        enter_c
        pushfd
        cli                         ; Disable interrupts

; Mask DMA channel to disable it

        mov     ebx,[channel]       ; EBX indexes DMAC register tables
        mov     dh,0                ; DH = 0 for DMAC register port access
        mov     al,bl
        and     al,11b
        or      al,100b             ; AL = (channel & 3) | "set mask bit"
        mov     dl,[DMAC_mask+ebx]
        out     dx,al

; Generate IOW to clear FF toggle state

        mov     al,0
        mov     dl,[DMAC_FF+ebx]
        out     dx,al

; Compute buffer address to program

        mov     eax,[bufferPhys]    ; AX := DMA address offset
        mov     ecx,eax
        shr     ecx,16              ; CL := bufferPhys >> 16 (DMA page)
        mov     esi,[count]         ; ESI = # of bytes to transfer
        cmp     ebx,4               ; 16-bit channel?
        jb      @@WriteDMAC         ; No, program DMAC
        shr     eax,1               ; Yes, convert address and count
        shr     esi,1               ; to 16-bit, 128K/page format

; Set the DMA address word (bits 0-15)

@@WriteDMAC:
        mov     dl,[DMAC_addr+ebx]
        out     dx,al
        mov     al,ah
        out     dx,al

; Set DMA transfer count

        mov     eax,esi
        dec     eax                 ; ESI = # of bytes to transfer - 1
        mov     dl,[DMAC_cnt+ebx]
        out     dx,al
        mov     al,ah
        out     dx,al

; Set DMA page byte (bits 16-23)

        mov     al,cl
        mov     dl,[DMAC_page+ebx]
        out     dx,al

; Set the DMA channel mode

        mov     al,bl
        and     al,11b
        or      al,[BYTE mode]      ; EAX = (channel & 3) | mode
        mov     dl,[DMAC_mode+ebx]
        out     dx,al

        pop     eax                 ; SMP safe interrupt state restore!
        test    eax,200h
        jz      @@1
        sti
@@1:    leave_c
        ret

cprocend

;----------------------------------------------------------------------------
; ulong PMAPI PM_DMACPosition(int channel);
;----------------------------------------------------------------------------
; Returns the current position in a dma transfer. Interrupts should be
; disabled before calling this function.
;----------------------------------------------------------------------------
cprocstart  PM_DMACPosition

        ARG     channel:UINT

        enter_c
        mov     ecx,[channel]       ; ECX indexes DMAC register tables
        mov     dh,0                ; DH = 0 for DMAC register port access

; Generate IOW to clear FF toggle state

        mov     al,0
        mov     dl,[DMAC_FF+ebx]
        out     dx,al
        xor     eax,eax
        xor     ecx,ecx

; Now read the current position for the channel

@@ReadLoop:
        mov     dl,[DMAC_cnt+ebx]
        out     dx,al
        in      al,dx
        mov     cl,al
        in      al,dx
        mov     ch,al               ; ECX := first count read
        in      al,dx
        mov     ah,al
        in      al,dx
        xchg    al,ah               ; EAX := second count read
        sub     ecx,eax
        cmp     ecx,40h
        jg      @@ReadLoop
        cmp     ebx,4               ; 16-bit channel?
        jb      @@Exit              ; No, we are done
        shl     eax,1               ; Yes, adjust to byte address

@@Exit: leave_c
        ret

cprocend


cpublic _PM_DMACodeEnd

endif

endcodeseg  _dma

        END                     ; End of module
