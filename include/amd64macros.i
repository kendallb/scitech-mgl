############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU Lesser General Public License version 2.1 as published by the Free
#   Software Foundation and appearing in the file LICENSE.LGPL included
#   in the packaging of this file.
#
#   Licensees holding a valid Commercial License for this product from
#   SciTech Software, Inc. may use this file in accordance with the
#   Commercial License Agreement provided with the Software.
#
#   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
#   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   See http://www.scitechsoft.com/license/ for information about
#   the licensing options available and how to purchase a Commercial
#   License Agreement.
#
#   Contact license@scitechsoft.com if any conditions of this licensing
#   are not clear to you, or you have questions about licensing options.
#
#  ========================================================================
#
# Language:    GNU Assembler for PPC (AS)
# Environment: AMD64 aka x86-64 64-bit little-endian.
#
# Description: Macros to provide interface support for the AMD64 versions
#              of Scitech's software.
#
############################################################################

.set PTR_SIZE, 8  # set up pointer size (in bytes)

#-----------------------------------------------------------
# gets 64-bit parameter 'argno' into register 'regname'
# arguments 1 to 6 are passed in registers rdi, rsi, rdx, rcx, 
# r8 and r9; arguments 7 and up are stored on stack at
# (rbp + (argno - 6) * 8 + 16). Since the stub is called before
# stack frame was established, the arguments passed on stack 
# will actually be at esp + 8 (8 bytes taken by the saved return
# address). 
#
# This is based on the System V ABI (x86-64 supplement).
#-----------------------------------------------------------
.macro getarg destreg, argno
    .if \argno == 0
        .err
    .endif
    .if \argno < 7
        .if \argno == 1
            mov %rdi,\destreg
        .endif
        .if \argno == 2
            mov %rsi,\destreg
        .endif
        .if \argno == 3
            mov %rdx,\destreg
        .endif
        .if \argno == 4
            mov %rcx,\destreg
        .endif
        .if \argno == 5
            mov %r8,\destreg
        .endif
        .if \argno == 6
            mov %r9,\destreg
        .endif
    .else
        mov (\argno-6)*8(%rsp),\destreg
    .endif
.endm

#------------------------------------------------------------------------
# the address passed is that of a structure containing function pointers.
# the offset is that of the first function pointer we are to reference
#------------------------------------------------------------------------
.macro BEGIN_STUBS_DEF address, offset
    .set STUBS_START, \address
    .set LOFF, \offset
.endm

.macro END_STUBS_DEF
.endm

.macro BEGIN_IMPORTS_DEF address
    .set STUBS_START, \address
    .set LOFF, PTR_SIZE
.endm

.macro END_IMPORTS_DEF
.endm

#----------------------------------------------------------------------
# according to the System V ABI reference (PPC supplement), registers
# r11 and the CTR are volatile across function calls, meaning that it
# is OK for us to use them here without saving them.
#----------------------------------------------------------------------
.macro DECLARE_STUB stubname
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    # we want to branch to *(STUBS_START + LOFF)
.ifdef __PIC__
    jmp     *(STUBS_START + LOFF)(%rip)
.else
    jmp     *(STUBS_START + LOFF)
.endif

    .set LOFF, LOFF+PTR_SIZE
.endm

.macro DECLARE_IMP stubname, ignore
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    # we want to branch to *(STUBS_START + LOFF)
.ifdef __PIC__
    jmp     *(STUBS_START + LOFF)(%rip)
.else
    jmp     *(STUBS_START + LOFF)
.endif

    .set LOFF, LOFF+PTR_SIZE
.endm

.macro DECLARE_PTR ptrname, ignore
    DECLARE_STUB ptrname
.endm

.macro SKIP_STUB stubname
    .set LOFF, LOFF+PTR_SIZE
.endm

.macro SKIP_IMP impname, ignore
    SKIP_STUB impname
.endm

.macro SKIP_PTR impname, ignore
    SKIP_STUB impname
.endm

.macro SKIP_IMP2 skipname
    DECLARE_STUB skipname
.endm

.macro SKIP_IMP3 impname
   SKIP_STUB impname
.endm

#----------------------------------------------------------------------------
# Macro for functions that check to see if the ROP2 mix is one of the
# bogus mixes without any source, and then punts to the software
# rasteriser to handle that mix for consistent results. Some hardware will
# hang with some functions on these mixes, and other hardware will do
# things differently depending on when the mix is applied (ie: pre or post
# transparency etc). Normally the OS shell drivers will handle these
# special case ROPs much earlier to avoid these types of problems, but we
# handle them anyway to ensure our drivers are consistent.
#----------------------------------------------------------------------------
.macro DECLARE_ROP_STUB stubname
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    mov     REF_mix(%rip),%al               # get ROP2
    mov     %al,%ah                         # make it ROP3
    shr     $2,%ah
    xor     %ah,%al
    and     $3,%al
    jz      1f                              # jump to label 1 if clear

    jmpq    *(STUBS_START + LOFF)(%rip)     # jump to *(STUBS_START + LOFF)
1:
    jmpq    *(REF_draw2dSW + LOFF)(%rip)    # jump to *(REF_draw2dSW + LOFF)

    .set LOFF, LOFF+PTR_SIZE
.endm

.macro DECLARE_ROPPARM_STUB stubname, paramno
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    getarg  %rax,\paramno                   # get specified argument in rax
    mov     %al,%ah                         # make it ROP3
    shr     $2,%ah
    xor     %ah,%al
    and     $3,%al
    jz      1f                              # jump to label 1 if clear

    jmp     *(STUBS_START + LOFF)(%rip)     # jump to *(STUBS_START + LOFF)
1:
    jmp     *(REF_draw2dSW + LOFF)(%rip)    # jump to *(REF_draw2dSW + LOFF)
    
    .set LOFF, LOFF+PTR_SIZE
.endm

#----------------------------------------------------------------------------
# Macro for *BitBltSys functions that check for GA_REPLACE_MIX
# since the hardware will be slower than software for this operation.
#----------------------------------------------------------------------------
.macro DECLARE_SLOW_BLTSYS_STUB stubname, paramno
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    cmp     $1,REF_slowBltSys(%rip)
    je      1f
    getarg  %rax,\paramno                   # get specified argument in rax
    cmp     $0xC,%al                        # see if it is replace mix
    je      2f
1:
    mov     %al,%ah                         # make argument into ROP3
    shr     $2,%ah
    xor     %ah,%al
    and     $3,%al
    jz      2f                              # jump to label 2 if clear

    jmp     *(STUBS_START + LOFF)(%rip)     # jump to *(STUBS_START + LOFF)
2:
    jmp     *(REF_draw2dSW + LOFF)(%rip)    # jump to *(REF_draw2dSW + LOFF)
    
    .set LOFF, LOFF+PTR_SIZE
.endm
