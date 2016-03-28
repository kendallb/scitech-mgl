
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>


/* #define LINEAR_ADDRESSING */


int VGAMODE, USEGL;
GraphicsContext *physicalscreen;
GraphicsContext *backscreen;


void screen1(void)
{
    int x, y;
    for (y = 0; y < HEIGHT; y++)
	for (x = 0; x < WIDTH; x++)
	    /* limited RGB palette in 256-color modes */
	    /* some color information is not used in */
	    /* 15-bit color modes */
	    gl_setpixelrgb(x, y,
			   x * 256 / WIDTH,
			   255 - x * 256 / WIDTH,
			   y * 256 / HEIGHT);
}


void configure(void)
{
    int allowed[GLASTMODE + 1];

    for (;;) {
	int i;
	int m;
	for (i = G320x200x16; i <= GLASTMODE; i++) {
	    allowed[i] = 0;
	    if (vga_hasmode(i)) {
		printf("%2d  %s\n", i, vga_getmodename(i));
		allowed[i] = 1;
	    }
	}

	printf("\nWhich mode? ");
	scanf("%d", &m);
	getchar();
	printf("\n");
	if (m >= G320x200x16 && m <= GLASTMODE) {
	    VGAMODE = m;
	    if (vga_getmodeinfo(m)->bytesperpixel >= 1)
		USEGL = 1;
	    else
		USEGL = 0;
	    break;
	}
    }

    vga_setmode(VGAMODE);
#ifdef LINEAR_ADDRESSING
    vga_setlinearaddressing();
#endif
    if (USEGL) {
	gl_setcontextvga(VGAMODE);
	physicalscreen = gl_allocatecontext();
	gl_getcontext(physicalscreen);
    }
}


void blit(void)
{
    gl_clearscreen(0x86);
    vga_imageblt(vga_getgraphmem(), 0, WIDTH - 128, HEIGHT - 128,
		 WIDTH * BYTESPERPIXEL);
/*      vga_bitblt(0, 100 * WIDTH * BYTESPERPIXEL, 50, 50, WIDTH * BYTESPERPIXEL);
   vga_fillblt(100 * BYTESPERPIXEL, 50, 50, WIDTH * BYTESPERPIXEL, 0x86);
 */
}


#if 0

/* May help on well-designed motherboards. */

/* IT DOES MAKE A DIFFERENCE! REP STOSL IS SLOWER */
/* (CL-GD5434 VLB zero-wait writes -- 2/3 cycles); rep stos takes 4 */

#if 1

static inline void *
 __memset(void *s, char c, size_t count)
{
    __asm__(
	       "cld\n\t"
	       "cmpl $12,%%edx\n\t"
	       "jl 1f\n\t"	/* if (count >= 12) */

	       "movzbl %%al,%%ax\n\t"
	       "movl %%eax,%%ecx\n\t"
	       "shll $8,%%ecx\n\t"	/* c |= c << 8 */
	       "orl %%ecx,%%eax\n\t"
	       "movl %%eax,%%ecx\n\t"
	       "shll $16,%%ecx\n\t"	/* c |= c << 16 */
	       "orl %%ecx,%%eax\n\t"

	       "movl %%edx,%%ecx\n\t"
	       "negl %%ecx\n\t"
	       "andl $3,%%ecx\n\t"	/* (-s % 4) */
	       "subl %%ecx,%%edx\n\t"	/* count -= (-s % 4) */
	       "rep ; stosb\n\t"	/* align to longword boundary */

	       "movl %%edx,%%ecx\n\t"
	       "shrl $2,%%ecx\n\t"

	       "cmpl $32,%%ecx\n\t"	/* do loop unrolling for */
	       "jl 2f\n\t"	/* chunks of 128 bytes */
	       "jmp 3f\n\t"
	       ".align 4,0x90\n\t"

	       "3:\n\t"
	       "movl %%eax,(%%edi)\n\t"
	       "movl %%eax,4(%%edi)\n\t"
	       "movl %%eax,8(%%edi)\n\t"
	       "movl %%eax,12(%%edi)\n\t"
	       "movl %%eax,16(%%edi)\n\t"
	       "movl %%eax,20(%%edi)\n\t"
	       "movl %%eax,24(%%edi)\n\t"
	       "movl %%eax,28(%%edi)\n\t"
	       "movl %%eax,32(%%edi)\n\t"
	       "movl %%eax,36(%%edi)\n\t"
	       "movl %%eax,40(%%edi)\n\t"
	       "movl %%eax,44(%%edi)\n\t"
	       "movl %%eax,48(%%edi)\n\t"
	       "movl %%eax,52(%%edi)\n\t"
	       "movl %%eax,56(%%edi)\n\t"
	       "movl %%eax,60(%%edi)\n\t"
	       "movl %%eax,64(%%edi)\n\t"
	       "movl %%eax,68(%%edi)\n\t"
	       "movl %%eax,72(%%edi)\n\t"
	       "movl %%eax,76(%%edi)\n\t"
	       "movl %%eax,80(%%edi)\n\t"
	       "movl %%eax,84(%%edi)\n\t"
	       "movl %%eax,88(%%edi)\n\t"
	       "movl %%eax,92(%%edi)\n\t"
	       "movl %%eax,96(%%edi)\n\t"
	       "movl %%eax,100(%%edi)\n\t"
	       "movl %%eax,104(%%edi)\n\t"
	       "movl %%eax,108(%%edi)\n\t"
	       "subl $32,%%ecx\n\t"
	       "movl %%eax,112(%%edi)\n\t"
	       "movl %%eax,116(%%edi)\n\t"
	       "movl %%eax,120(%%edi)\n\t"
	       "movl %%eax,124(%%edi)\n\t"
	       "addl $128,%%edi\n\t"
	       "cmpl $32,%%ecx\n\t"
	       "jge 3b\n\t"

	       "2:\n\t"
	       "rep ; stosl\n\t"	/* fill remaining longwords */

	       "andl $3,%%edx\n"	/* fill last few bytes */
	       "1:\tmovl %%edx,%%ecx\n\t"	/* <= 12 entry point */
	       "rep ; stosb\n\t"
  : :	       "a"(c), "D"(s), "d"(count)
  :	       "ax", "cx", "dx", "di");
    return s;
}

#else				/* 8-bit writes. */

static inline void *
 __memset(void *s, char c, size_t count)
{
    __asm__(
	       "cld\n\t"
	       "cmpl $12,%%edx\n\t"
	       "jl 1f\n\t"	/* if (count >= 12) */

	       "movzbl %%al,%%ax\n\t"
	       "movl %%eax,%%ecx\n\t"
	       "shll $8,%%ecx\n\t"	/* c |= c << 8 */
	       "orl %%ecx,%%eax\n\t"
	       "movl %%eax,%%ecx\n\t"
	       "shll $16,%%ecx\n\t"	/* c |= c << 16 */
	       "orl %%ecx,%%eax\n\t"

	       "movl %%edx,%%ecx\n\t"
	       "negl %%ecx\n\t"
	       "andl $3,%%ecx\n\t"	/* (-s % 4) */
	       "subl %%ecx,%%edx\n\t"	/* count -= (-s % 4) */
	       "rep ; stosb\n\t"	/* align to longword boundary */

	       "movl %%edx,%%ecx\n\t"
	       "shrl $2,%%ecx\n\t"

	       "cmpl $32,%%ecx\n\t"	/* do loop unrolling for */
	       "jl 2f\n\t"	/* chunks of 128 bytes */
	       "jmp 3f\n\t"
	       ".align 4,0x90\n\t"

	       "3:\n\t"
	       "movb %%al,(%%edi)\n\t"
	       "movb %%al,1(%%edi)\n\t"
	       "movb %%al,2(%%edi)\n\t"
	       "movl %%al,3(%%edi)\n\t"
	       "movl %%al,4(%%edi)\n\t"
	       "movl %%al,5(%%edi)\n\t"
	       "movl %%al,6(%%edi)\n\t"
	       "movl %%al,7(%%edi)\n\t"
	       "movl %%al,8(%%edi)\n\t"
	       "movl %%al,9(%%edi)\n\t"
	       "movl %%al,10(%%edi)\n\t"
	       "movl %%al,11(%%edi)\n\t"
	       "movl %%al,12(%%edi)\n\t"
	       "movl %%al,13(%%edi)\n\t"
	       "movl %%al,14(%%edi)\n\t"
	       "movl %%al,15(%%edi)\n\t"
	       "movl %%al,16(%%edi)\n\t"
	       "movl %%al,17(%%edi)\n\t"
	       "movl %%al,18(%%edi)\n\t"
	       "movl %%al,19(%%edi)\n\t"
	       "movl %%al,20(%%edi)\n\t"
	       "movl %%al,21(%%edi)\n\t"
	       "movl %%al,22(%%edi)\n\t"
	       "movl %%al,23(%%edi)\n\t"
	       "movl %%al,24(%%edi)\n\t"
	       "movl %%al,25(%%edi)\n\t"
	       "movl %%al,26(%%edi)\n\t"
	       "movl %%al,27(%%edi)\n\t"
	       "movl %%al,28(%%edi)\n\t"
	       "subl $8,%%ecx\n\t"
	       "movl %%al,29(%%edi)\n\t"
	       "movl %%al,30(%%edi)\n\t"
	       "movl %%al,31(%%edi)\n\t"
	       "addl $32,%%edi\n\t"
	       "cmpl $8,%%ecx\n\t"
	       "jge 3b\n\t"

	       "2:\n\t"
	       "rep ; stosl\n\t"	/* fill remaining longwords */

	       "andl $3,%%edx\n"	/* fill last few bytes */
	       "1:\tmovl %%edx,%%ecx\n\t"	/* <= 12 entry point */
	       "rep ; stosb\n\t"
  : :	       "a"(c), "D"(s), "d"(count)
  :	       "ax", "cx", "dx", "di");
    return s;
}

#endif

#define memset __memset

#endif


void speed(void)
{
    int i;
    int start_clock;
    int finish_clock;
    int diff_clock;
    unsigned char *vgabase = vga_getgraphmem();

#ifndef LINEAR_ADDRESSING
    if (VGAMODE >= G640x480x256)
	vga_setpage(0);
#endif

    start_clock = clock();

    for (i = 0; i < 500; i++) {
	memset(vgabase, i & 255, 65536);
/*              vga_imageblt(backscreen.vbuf, 0x100000, 256, 256, WIDTH); */
/*              vga_bitblt(WIDTH * BYTESPERPIXEL * 256 + (i & 63), 0, 256,
   256, WIDTH * BYTESPERPIXEL); */
/*              vga_fillblt(0, 256, 256, WIDTH * BYTESPERPIXEL, i & 255); */
    }

    finish_clock = clock();

    diff_clock = finish_clock - start_clock;
    printf("Timing: %3d.%1ds, %dK/s\n", diff_clock / 100,
	   (diff_clock % 100) / 10, 3200000 / diff_clock);
}


void main(void)
{
    vga_init();

    printf("This is a video memory speed tester. Note that the first "
	   "screen doesn't test\nanything (nor does the 3 second pause "
	   "that follows).\n\n");

    configure();

    if (COLORS == 256)
	gl_setrgbpalette();	/* set RGB palette */

    if (USEGL)
	screen1();
    sleep(2);

/*      vga_screenoff(); */

    speed();

    vga_setmode(TEXT);
    exit(0);
}
