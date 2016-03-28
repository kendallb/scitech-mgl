############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2003 SciTech Software, Inc. All rights reserved.
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
# Environment: PPC 32-bit big-endian.
#
# Description: Macros to provide interface support on the PowerPC versions
#              of Scitech's software.
#
############################################################################

.set PTR_SIZE, 4  # set up pointer size (in bytes)

#-----------------------------------------------------------
# gets 32-bit parameter 'argno' into register 'regname'
# arguments 1..8 are in r3..r10, arguments 9..n are
# expected to be at (r1 + (argno-9)*4 + 8). this is based
# on the System V ABI (PPC supplement).
#
# note that r1 (the stack pointer) is presumed to not have
# been adjusted into a new frame at the time this is called.
#-----------------------------------------------------------
.macro getarg destreg, argno
    .if \argno == 0
        .err
    .endif
    .if \argno < 9
        mr  \destreg,\argno+2
    .else
        lwz \destreg,(\argno-7)*4(r1)
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
    lis     r11, STUBS_START@ha           # load the upper 16 bits of the dest into r11
    addi    r11, r11, STUBS_START@l       # now add the lower 16 bits 
    lwz     r11, LOFF(r11)                # load the word at r11 + offset into r11
    mtctr   r11                           # move r11 to the counter register
    bctr                                  # branch to address in counter register
    .set LOFF, LOFF+PTR_SIZE
.endm

.macro DECLARE_IMP stubname, ignore
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    # we want to branch to *(STUBS_START + LOFF)
    lis     r11, STUBS_START@ha           # load the upper 16 bits of the dest into r11
    addi    r11, r11, STUBS_START@l       # now add the lower 16 bits
    lwz     r11, LOFF(r11)                # load the word at r11 + offset into r11
    mtctr   r11                           # move r11 to the counter register
    bctr                                  # branch to address in counter register
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
    lis     r11,REF_mix+3@ha                # get upper 16 bits of address of REF_mix
    lbz     r11,REF_mix+3@l(r11)            # get LSB of REF_mix into r11
    rlwinm  r12,r11,30,30,31                # shift left by two and clear top 30 bits
    xor     r12,r11,r12                     # xor with original value, result in r12
    andi.   r12,r12,3                       # test bottom two bits
    bt      eq,1f                           # branch forward to label 1 if clear

    # we want to branch to *(STUBS_START + LOFF)

    lis     r11, STUBS_START@ha             # load the upper 16 bits of the dest into r11
    addi    r11, r11, STUBS_START@l         # now add the lower 16 bits
    lwz     r11, LOFF(r11)                  # load the word at r11 + offset into r11
    mtctr   r11                             # move r11 to the counter register
    bctr                                    # branch to address in counter register

1:
    # we want to branch to *(REF_draw2dSW + LOFF)

    lis     r11, REF_draw2dSW@ha            # load the upper 16 bits of the dest into r11
    addi    r11, r11, REF_draw2dSW@l        # now add the lower 16 bits
    lwz     r11, LOFF(r11)                  # load the word at r11 + offset into r11
    mtctr   r11                             # move r11 to the counter register
    bctr                                    # branch to address in counter register
    
    .set LOFF, LOFF+PTR_SIZE
.endm

.macro DECLARE_ROPPARM_STUB stubname, paramno
    .align 2
    .global \stubname
    .type   \stubname,@function
\stubname:
    getarg      r11,\paramno                # get specified parameter into r11
    rlwinm      r12,r11,30,30,31            # shift left by two and clear top 30 bits
    xor         r12,r11,r12                 # xor with original value, result in r12
    andi.       r12,r12,3                   # test bottom two bits
    bt          eq,1f                       # branch forward to label 1 if clear

    # we want to branch to *(STUBS_START + LOFF)
    lis         r11, STUBS_START@ha         # load the upper 16 bits of the dest into r11
    addi        r11, r11, STUBS_START@l     # now add the lower 16 bits
    lwz         r11, LOFF(r11)              # load the word at r11 + offset into r11
    mtctr       r11                         # move r11 to the counter register
    bctr                                    # branch to address in counter register

1:
    # we want to branch to *(REF_draw2dSW + LOFF)
    lis         r11, REF_draw2dSW@ha        # load the upper 16 bits of the dest into r11
    addi        r11, r11, REF_draw2dSW@l    # now add the lower 16 bits
    lwz         r11, LOFF(r11)              # load the word at r11 + offset into r11
    mtctr       r11                         # move r11 to the counter register
    bctr                                    # branch to address in counter register
    
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
    getarg  r11,\paramno                # get specified parameter into r11
    lis     r12,REF_slowBltSys@ha       # get upper 16 bits of address of REF_slowBltSys
    lbz     r12,REF_slowBltSys@l(r12)   # get value of REF_slowBltSys into r12
    or.     r12,r12,r12                 # test it against 0
    bf      eq,1f                       # skip next test if it's non-zero

    getarg  r11,\paramno                # get specified parameter into r11
    cmplwi  r11,12                      # compare it to GA_REPLACE_MIX (12)
    bt      eq,2f                       # branch forward to label 2 if equal

1:
    rlwinm  r12,r11,30,30,31            # shift param left by two and clear top 30 bits
    xor     r12,r11,r12                 # xor with original value, result in r12
    andi.   r12,r12,3                   # test bottom two bits
    bt      eq,2f                       # branch forward to label 2 if clear

    # we want to branch to *(STUBS_START + LOFF)

    lis     r11, STUBS_START@ha         # load the upper 16 bits of the dest into r11
    addi    r11, r11, STUBS_START@l     # now add the lower 16 bits
    lwz     r11, LOFF(r11)              # load the word at r11 + offset into r11
    mtctr   r11                         # move r11 to the counter register
    bctr                                # branch to address in counter register

2:
    # we want to branch to *(REF_draw2dSW + LOFF)

    lis     r11, REF_draw2dSW@ha        # load the upper 16 bits of the dest into r11
    addi    r11, r11, REF_draw2dSW@l    # now add the lower 16 bits
    lwz     r11, LOFF(r11)              # load the word at r11 + offset into r11
    mtctr   r11                         # move r11 to the counter register
    bctr                                # branch to address in counter register
    
    .set LOFF, LOFF+PTR_SIZE
.endm

