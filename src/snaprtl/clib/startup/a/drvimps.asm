;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  Import stubs for platform dependent C library functions
;*               for SNAP device drivers.
;*
;*****************************************************************************

.386

assume ds:flat

; Macro to begin a data segment. Segment attributes were specified in
; the header macro that is always required.

begdataseg  macro
_DATA segment public use32 'DATA'
            endm

; Macro to end a data segment

enddataseg  macro
_DATA ends
            endm

; Macro to begin a code segment

begcodeseg  macro
_TEXT   segment public use32 'CODE'
            endm

; Macro to end a code segment

endcodeseg  macro
_TEXT   ends
            endm

; macros to declare assembler import stubs for binary loadable drivers

BEGIN_IMPORTS_DEF   macro   p1
begdataseg
extern _&p1:BYTE
STUBS_START equ _&p1
enddataseg
begcodeseg
off = 4
                    endm

DECLARE_IMP macro   p1
    public _&p1
_&p1:
    jmp     [DWORD PTR STUBS_START + off]
    off = off+4
            endm

END_IMPORTS_DEF macro
endcodeseg
                endm

BEGIN_IMPORTS_DEF   ___imports
DECLARE_IMP xabort
DECLARE_IMP ___atexit         ; Obsolete and no longer used
DECLARE_IMP ___calloc         ; Obsolete and no longer used
DECLARE_IMP xexit
DECLARE_IMP xfree
DECLARE_IMP xgetenv
DECLARE_IMP xmalloc
DECLARE_IMP ___realloc        ; Obsolete and no longer used
DECLARE_IMP xsystem
DECLARE_IMP xputenv
DECLARE_IMP xopen
DECLARE_IMP xaccess
DECLARE_IMP xclose
DECLARE_IMP xlseek
DECLARE_IMP xread
DECLARE_IMP xunlink
DECLARE_IMP xwrite
DECLARE_IMP xisatty
DECLARE_IMP xremove
DECLARE_IMP xrename
DECLARE_IMP xraise
DECLARE_IMP xsignal
DECLARE_IMP xtime
DECLARE_IMP _OS_setfileattr
DECLARE_IMP _OS_getcurrentdate
END_IMPORTS_DEF

        END

