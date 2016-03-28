#define __ASSEMBLY__
#include <linux/linkage.h>

#ifndef	SYMBOL_NAME
#define	SYMBOL_NAME(name)	_ ## name
#endif
#ifndef ENTRY
#define ENTRY(name) .align 4; .globl _ ## name ## ; _ ## name ## :
#endif
	
	.file "mem.S"

 /* This file contains unrolled memcpy functions. */


 /* Prototype: memcpy4to3( void *dest, void *src, int n ) */
 /* Copies pixels from 4-byte-per-pixel screen to 3-byte-per-pixel screen,
*/
 /* discarding the last byte of each pixel. */
 /* Only uses 32-bit aligned word accesses. */
 /* Instructions have been shuffled a bit for possible avoidance of */
 /* pipeline hazards. */

.text
ENTRY(memcpy4to3)
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	pushl %ecx
	movl 8(%ebp),%edi	/* destination address */
	movl 12(%ebp),%esi	/* source address */
	movl 16(%ebp),%ecx	/* number of pixels */

	/* Handle chunks of 8 pixels. */
1:	cmpl $8,%ecx
	jl 2f

	movl (%esi),%eax	/* pixel 0 */
	movl 4(%esi),%ebx	/* pixel 1 */
	shll $8,%eax		/* BGR0 in 8-31 */
	shrd $8,%ebx,%eax	/* BGR0 in 0-23, B1 in 24-31 */
	movl %eax,(%edi)	/* write word */
	shll $8,%ebx		/* GR1 in 16-31 */
	movl 8(%esi),%eax	/* pixel 2 */
	shrd $16,%eax,%ebx	/* GR1 in 0-15, BG2 in 16-31 */
	movl %ebx,4(%edi)	/* write word */
	shll $8,%eax		/* move R2 into 24-31 */
	movl 12(%esi),%ebx	/* pixel 3 */
	shrd $24,%ebx,%eax	/* R2 in 0-7, BGR3 in 8-31 */
	movl %eax,8(%edi)	/* write word */

	movl 16(%esi),%eax	/* pixel 4 */
	shll $8,%eax		/* BGR4 in 8-31 */
	movl 20(%esi),%ebx	/* pixel 5 */
	shrd $8,%ebx,%eax	/* BGR4 in 0-23, B5 in 24-31 */
	movl %eax,12(%edi)	/* write word */
	shll $8,%ebx		/* GR5 in 16-31 */
	movl 24(%esi),%eax	/* pixel 6 */
	shrd $16,%eax,%ebx	/* GR5 in 0-15, BG6 in 16-31 */
	movl %ebx,16(%edi)	/* write word */
	subl $8,%ecx		/* blended end-of-loop instruction */
	shll $8,%eax		/* move R6 into 24-31 */
	movl 28(%esi),%ebx	/* pixel 7 */
	shrd $24,%ebx,%eax	/* R6 in 0-7, BGR7 in 8-31 */
	addl $32,%esi		/* blended end-of-loop instruction */
	movl %eax,20(%edi)	/* write word */

	addl $24,%edi
	jmp 1b

2:	/* Do the remaining pixels. */

	andl %ecx,%ecx
	jz 4f			/* none left */

3:	movl (%esi),%eax
	movw %eax,(%edi)
	shrl $16,%eax
	movb %al,2(%edi)
	addl $4,%esi
	addl $3,%edi
	decl %ecx
	jnz 3b

4:
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

 /* Prototype: memcpy32shift8( void *dest, void *src, int n ) */
 /* Copies pixels from 4-byte-per-pixel screen organized as BGR0 to */
 /* 0BGR 4-byte-per-pixel screen. */
 /* Used by copyscreen for ATI mach32 32-bit truecolor modes. */

.text
ENTRY(memcpy32shift8)
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %ebx
	movl 8(%ebp),%edi	/* destination address */
	movl 12(%ebp),%esi	/* source address */
	movl 16(%ebp),%ecx	/* number of pixels */

	/* Handle chunks of 8 pixels. */
1:	cmpl $8,%ecx
	jl 2f

	movl (%esi),%eax
	shll $8,%eax
	movl %eax,(%edi)
	movl 4(%esi),%edx
	shll $8,%edx
	movl %edx,4(%edi)
	movl 8(%esi),%eax
	shll $8,%eax
	movl %eax,8(%edi)
	movl 12(%esi),%edx
	shll $8,%edx
	movl %edx,12(%edi)
	movl 16(%esi),%eax
	shll $8,%eax
	movl %eax,16(%edi)
	movl 20(%esi),%edx
	shll $8,%edx
	movl %edx,20(%edi)
	movl 24(%esi),%eax
	subl $8,%ecx
	shll $8,%eax
	movl %eax,24(%edi)
	movl 28(%esi),%edx
	addl $32,%esi
	shll $8,%edx
	movl %edx,28(%edi)
	addl $32,%edi
	jmp 1b

2:	andl %ecx,%ecx
	jz 4f

3:	movl (%esi),%eax
	shll $8,%eax
	movl %eax,(%edi)
	addl $4,%esi
	addl $4,%edi
	decl %ecx
	jnz 3b

4:	
	popl %ebx
	popl %ecx
	popl %esi
	popl %edi
	popl %ebp
	ret


/* Optimized memcpy. */
/* Performance on par with inlined 32-bit aligned rep movsl on slow */
/* motherboard. */
/* Hypothesized to be fast on motherboards that handle writes efficiently */
/* and suffer with slow rep movsl microcode in 486/Pentium. */
/* (esp. Cyrix 486DX WB, Headland HTK 486 chipset, Pentium). */

/* Arguments passed in registers: */
/* destination address in %esi */
/* source address in %edx */
/* count in %ecx */

#define MOVEBYTE(n) movb n(%edx),%al; movb %al,n(%esi)

#define MOVESHORT(n) movw n(%edx),%ax; movw %ax,n(%esi)

#define MOVEWORD(n) movl n(%edx),%eax; movl %eax,n(%esi)

ENTRY(_memcpy_jumptable)
	.long copy0
	.long copy1, copy2, copy3, copy4
	.long copy5, copy6, copy7, copy8
	.long copy9, copy10, copy11, copy12
	.long copy13, copy14, copy15, copy16
	.long copy17, copy18, copy19, copy20
	.long copy21, copy22, copy23, copy24
	.long copy25, copy26, copy27, copy28
	.long copy29, copy30, copy31, copy32

jumptable2:
	.long align0, align1, align2, align3

ENTRY(_memcpyasm_regargs)

	/* This is only valid if nu_bytes >= 3. */

	/* Align destination to 32-bit boundary */
	movl %esi,%eax
	andl $3,%eax
	jmp *jumptable2(,%eax,4)

align1:	MOVESHORT(0)
	MOVEBYTE(2)
	addl $3,%edx
	addl $3,%esi
	subl $3,%ecx
	jmp copyaligned

align3:	MOVEBYTE(0)
	incl %edx
	incl %esi
	decl %ecx
	jmp copyaligned

align2:	MOVESHORT(0)
	addl $2,%edx
	addl $2,%esi
	subl $2,%ecx
align0:

copyaligned:
	cmpl $32,%ecx
	ja copyunrolled
	/* <= 32 bytes. */
	/* Copy remaining bytes (0-32). */
	jmp *SYMBOL_NAME(_memcpy_jumptable)(,%ecx,4)
	.align 4,0x90

/* memcpyasm_regargs_aligned is only called if nu_bytes > 32. */
ENTRY(_memcpyasm_regargs_aligned)

copyunrolled:	
	/* Copy chunks of 32 bytes. */
	/* End-of-loop increment instructions blended in. */
	addl $32,%esi			/*P ok */
	movl (%edx),%eax
	movl %eax,(0-32)(%esi)		/*P ok */
	movl 4(%edx),%eax
	movl %eax,(4-32)(%esi)		/*P ok */
	movl 8(%edx),%eax
	movl %eax,(8-32)(%esi)		/*P ok */
	movl 12(%edx),%eax
	movl %eax,(12-32)(%esi)		/*P ok */
	movl 16(%edx),%eax
	addl $32,%edx			/*P ok */
	movl %eax,(16-32)(%esi)
	subl $32,%ecx			/*P ok */
	movl (20-32)(%edx),%eax
	movl %eax,(20-32)(%esi)		/*P ok */
	movl (24-32)(%edx),%eax
	movl %eax,(24-32)(%esi)		/*P ok */
	movl (28-32)(%edx),%eax
	movl %eax,(28-32)(%esi)		/*P ok */
	cmpl $32,%ecx
	jge copyunrolled		/*P fail */
	/* Copy remaining bytes (less than 32). */
	jmp *SYMBOL_NAME(_memcpy_jumptable)(,%ecx,4)

#define END ret

copy0:	END

copy1:	MOVEBYTE(0)
	END

copy2:	MOVESHORT(0)
	END

copy3:	MOVESHORT(0)
	MOVEBYTE(2)
	END

copy4:	MOVEWORD(0)
	END

copy5:	MOVEWORD(0)
	MOVEBYTE(4)
	END

copy6:	MOVEWORD(0)
	MOVESHORT(4)
	END

copy7:	MOVEWORD(0)
	MOVESHORT(4)
	MOVEBYTE(6)
	END

copy8:	MOVEWORD(0)
	MOVEWORD(4)
	END

copy9:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEBYTE(8)
	END

copy10:	MOVEWORD(0)
	MOVEWORD(4)
	MOVESHORT(8)
	END

copy11:	MOVEWORD(0)
	MOVEWORD(4)
	MOVESHORT(8)
	MOVEBYTE(10)
	END

copy12:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	END

copy13:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEBYTE(12)
	END

copy14:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVESHORT(12)
	END

copy15:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVESHORT(12)
	MOVEBYTE(14)
	END

copy16:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	END

copy17:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEBYTE(16)
	END

copy18:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVESHORT(16)
	END

copy19:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVESHORT(16)
	MOVEBYTE(18)
	END

copy20:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	END

copy21:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEBYTE(20)
	END

copy22:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVESHORT(20)
	END

copy23:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVESHORT(20)
	MOVEBYTE(22)
	END

copy24:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	END

copy25:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEBYTE(24)
	END

copy26:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVESHORT(24)
	END

copy27:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVESHORT(24)
	MOVEBYTE(26)
	END

copy28:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEWORD(24)
	END

copy29:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEWORD(24)
	MOVEBYTE(28)
	END

copy30:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEWORD(24)
	MOVESHORT(28)
	END

copy31:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEWORD(24)
	MOVESHORT(28)
	MOVEBYTE(30)
	END

copy32:	MOVEWORD(0)
	MOVEWORD(4)
	MOVEWORD(8)
	MOVEWORD(12)
	MOVEWORD(16)
	MOVEWORD(20)
	MOVEWORD(24)
	MOVEWORD(28)
	END
