;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; crc_i386.asm, optimized CRC calculation function for Zip and UnZip,
; created by Paul Kienitz and Christian Spieler.  Last revised 24 Dec 98.
;
; Revised 06-Oct-96, Scott Field (sfield@microsoft.com)
;   fixed to assemble with masm by not using .model directive which makes
;   assumptions about segment alignment.  Also,
;   avoid using loop, and j[e]cxz where possible.  Use mov + inc, rather
;   than lodsb, and other misc. changes resulting in the following performance
;   increases:
;
;      unrolled loops                NO_UNROLLED_LOOPS
;      *8    >8      <8              *8      >8      <8
;
;      +54%  +42%    +35%            +82%    +52%    +25%
;
;   first item in each table is input buffer length, even multiple of 8
;   second item in each table is input buffer length, > 8
;   third item in each table is input buffer length, < 8
;
; Revised 02-Apr-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's 32-bit-reads optimization as found in the
;   UNIX AS source crc_i386.S. This new code is currently always on.
;
; Revised 12-Oct-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's additional tweaks for 32-bit-optimized CPUs
;   (like the Pentium Pro, Pentium II, and probably some Pentium clones).
;   This optimization is always enabled by default. This default is based
;   on the assumption that most users these days people will have at least
;   a Pentium II class or higher machine ...)
;
; FLAT memory model assumed.
;
; Ported to SciTech assembler macro package for TASM/NASM by Kendall Bennett
; February, 2002. Removed old legacy macros to clean up the code somewhat
; for our target audience (Intel Pentium II class and later machines).
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used.
;

include "scitech.mac"

header      crc_i386

        cexternfunc get_crc_table,NEAR    ; ZCONST ulg near *get_crc_table(void);

begcodeseg  crc_i386

; These two (three) macros make up the loop body of the CRC32 cruncher.
; registers modified:
;   eax  : crc value "c"
;   esi  : pointer to next data byte (or dword) "buf++"
; registers read:
;   edi  : pointer to base of crc_table array
; scratch registers:
;   ebx  : index into crc_table array
;          (requires upper three bytes = 0 when __686 is undefined)
ifdef   USE_NASM
%macro Do_CRC 0
                movzx   ebx,al              ; tmp = c & 0xFF
                shr     eax,8               ; c = (c >> 8)
                xor     eax,[edi+ebx*4]     ;  ^ table[tmp]
%endmacro
%macro Do_CRC_byte 0
                xor     al,[BYTE esi]       ; c ^= *buf
                inc     esi                 ; buf++
                Do_CRC                      ; c = (c >> 8) ^ table[c & 0xFF]
%endmacro
%macro Do_CRC_dword 0
                xor     eax,[DWORD esi]     ; c ^= *(ulg *)buf
                add     esi,4               ; ((ulg *)buf)++
                Do_CRC
                Do_CRC
                Do_CRC
                Do_CRC
%endmacro
else
MACRO Do_CRC
                movzx   ebx,al              ; tmp = c & 0xFF
                shr     eax,8               ; c = (c >> 8)
                xor     eax,[edi+ebx*4]     ;  ^ table[tmp]
ENDM
MACRO Do_CRC_byte
                xor     al,[BYTE esi]       ; c ^= *buf
                inc     esi                 ; buf++
                Do_CRC                      ; c = (c >> 8) ^ table[c & 0xFF]
ENDM
MACRO Do_CRC_dword
                xor     eax,[DWORD esi]     ; c ^= *(ulg *)buf
                add     esi,4               ; ((ulg *)buf)++
                Do_CRC
                Do_CRC
                Do_CRC
                Do_CRC
ENDM
endif   ; !USE_NASM

; ulg crc32(ulg crc, ZCONST uch *buf, extent len)

cprocstart  crc32

        ARG     crc:ULONG, buf:DPTR, len:UINT

        push    ebp
        mov     ebp,esp
        push    edi
        push    esi
        push    ebx
        push    edx
        push    ecx

        mov     esi,[buf]           ; 2nd arg: uch *buf
        sub     eax,eax             ;> if (!buf)
        test    esi,esi             ;>   return 0;
        jz      @@fine              ;> else {

        call    get_crc_table
        mov     edi,eax
        mov     eax,[crc]           ; 1st arg: ulg crc
        mov     ecx,[len]           ; 3rd arg: extent len
        not     eax                 ;>   c = ~crc;

        test    ecx,ecx
        jz      @@bail
@@align_loop:
        test    esi,3               ; align buf pointer on next
        jz      @@aligned_now       ;  dword boundary
        Do_CRC_byte
        dec     ecx
        jnz     @@align_loop
@@aligned_now:
        mov     edx,ecx             ; save len in edx
        shr     ecx,3               ; ecx = len / 8
        jz      @@No_Eights

@@Next_Eight:
        Do_CRC_dword
        Do_CRC_dword
        dec     ecx
        jnz     @@Next_Eight
@@No_Eights:
        mov     ecx,edx
        and     ecx,000000007H      ; ecx = len % 8
        jz      @@bail              ;>   if (len)

@@loop:                             ;>     do {
        Do_CRC_byte                 ;        c = CRC32(c, *buf++);
        dec     ecx                 ;>     } while (--len);
        jnz     @@loop

@@bail:                             ;> }
        not     eax                 ;> return ~c;
@@fine:
        pop     ecx
        pop     edx
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp
        ret

cprocend

endcodeseg  crc_i386

            END

