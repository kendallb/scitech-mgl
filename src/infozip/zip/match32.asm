;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
;
; match32.asm by Jean-loup Gailly.

; match32.asm, optimized version of longest_match() in deflate.c
; To be used only with 32 bit flat model. To simplify the code, the option
; -DDYN_ALLOC is not supported.
; This file is only optional. If you don't have an assembler, use the
; C version (add -DNO_ASM to CFLAGS in makefile and remove match.o
; from OBJI). If you have reduced WSIZE in zip.h, then make sure this is
; assembled with an EQUivalent -DWSIZE=<whatever>.
;
; Win32 (Windows NT) version - 1994/04/13 by Steve Salisbury
; * works with Microsoft MASM 6.1X and Microsoft Visual C++ / 32-bit edition
;
; Adapted to work with Borland Turbo Assembler 5.0 by Cosmin Truta, 1997
;
; Ported to SciTech assembler macro package for TASM/NASM by Kendall Bennett
; February, 2002.
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used.
;

include "scitech.mac"

header      match32

begdataseg  match32

        cextern match_start,UINT
        cextern prev_length,UINT
        cextern good_match,UINT
ifndef FULL_SEARCH
        cextern nice_match,UINT
endif
        cextern strstart,UINT
        cextern max_chain_length,UINT
        cextern prev,USHORT
        cextern window,UCHAR

enddataseg  match32

begcodeseg  match32

ifndef      WSIZE
        WSIZE         EQU 32768         ; keep in sync with zip.h !
endif
        MIN_MATCH     EQU 3
        MAX_MATCH     EQU 258
        MIN_LOOKAHEAD EQU (MAX_MATCH+MIN_MATCH+1)
        MAX_DIST      EQU (WSIZE-MIN_LOOKAHEAD)

; initialize or check the variables used in match.asm.

cprocstart  match_init
        ret
cprocend

; -----------------------------------------------------------------------
; Set match_start to the longest match starting at the given string and
; return its length. Matches shorter or EQUal to prev_length are discarded,
; in which case the result is EQUal to prev_length and match_start is
; garbage.
; IN assertions: cur_match is the head of the hash chain for the current
;   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1

; int longest_match(cur_match)

cprocstart  longest_match

        ; return address                ; esp+16
        push    ebp                     ; esp+12
        push    edi                     ; esp+8
        push    esi                     ; esp+4
        push    ebx                     ; esp

;       match        EQU esi
;       scan         EQU edi
;       chain_length EQU ebp
;       best_len     EQU ebx
;       limit        EQU edx

        mov     esi,[esp+20]            ; esp+20 (cur_match on stack!)
        mov     edx,[strstart]
        mov     ebp,[max_chain_length]  ; chain_length = max_chain_length
        mov     edi,edx
        sub     edx,MAX_DIST            ; limit = strstart-MAX_DIST
        cld                             ; string ops increment esi and edi
        jae     @@limit_ok
        sub     edx,edx                 ; limit = NIL
@@limit_ok:
        add     edi,2+offset window     ; edi = offset(window + strstart + 2)
        mov     ebx,[prev_length]       ; best_len = prev_length
        mov     cx,[edi-2]              ; cx = scan[0..1]
        mov     ax,[ebx+edi-3]          ; ax = scan[best_len-1..best_len]
        cmp     ebx,[good_match]        ; do we have a good match already?
        jb      @@do_scan
        shr     ebp,2                   ; chain_length >>= 2
        jmp     @@do_scan

        align   4                       ; align destination of branch
@@long_loop:
; at this point, edi == scan+2, esi == cur_match
        mov     ax,[ebx+edi-3]          ; ax = scan[best_len-1..best_len]
        mov     cx,[edi-2]              ; cx = scan[0..1]
@@short_loop:
; at this point, edi == scan+2, esi == cur_match,
; ax = scan[best_len-1..best_len] and cx = scan[0..1]
        and     esi,WSIZE-1
        dec     ebp                     ; --chain_length
        mov     si,[prev+esi+esi]       ; cur_match = prev[cur_match]
                                        ; top word of esi is still 0
        jz      @@the_end
        cmp     esi,edx                 ; cur_match <= limit ?
        jbe     @@the_end
@@do_scan:
        cmp     ax,[WORD window+ebx+esi-1]   ; check match at best_len-1
        jne     @@short_loop
        cmp     cx,[WORD window+esi]         ; check min_match_length match
        jne     @@short_loop

        lea     esi,[window+esi+2]      ; esi = match
        mov     ecx,(MAX_MATCH-2)/2     ; scan for at most MAX_MATCH bytes
        mov     eax,edi                 ; eax = scan+2
        repe    cmpsw                   ; loop until mismatch
        je      @@maxmatch              ; match of length MAX_MATCH?
@@mismatch:
        mov     cl,[edi-2]              ; mismatch on first or second byte?
        xchg    eax,edi                 ; edi = scan+2, eax = end of scan
        sub     cl,[esi-2]              ; cl = 0 if first bytes EQUal
        sub     eax,edi                 ; eax = len
        sub     esi,2+offset window     ; esi = match - (2 + offset(window))
        sub     esi,eax                 ; esi = cur_match (= match - len)
        sub     cl,1                    ; set carry if cl == 0 (can't use DEC)
        adc     eax,0                   ; eax = carry ? len+1 : len
        cmp     eax,ebx                 ; len > best_len ?
        jle     @@long_loop
        mov     [match_start],esi       ; match_start = cur_match
        mov     ebx,eax                 ; ebx = best_len = len
ifdef FULL_SEARCH
        cmp     eax,MAX_MATCH           ; len >= MAX_MATCH ?
else
        cmp     eax,[nice_match]        ; len >= nice_match ?
endif
        jl      @@long_loop
@@the_end:
        mov     eax,ebx                 ; result = eax = best_len
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp
        ret
@@maxmatch:                             ; come here if maximum match
        cmpsb                           ; increment esi and edi
        jmp     @@mismatch              ; force match_length = MAX_LENGTH

cprocend

endcodeseg  match

        END

