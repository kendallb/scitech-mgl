/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* scale.c      Scaling routine */


#include <stdlib.h>
#include <vga.h>
#include "inlstring.h"		/* include inline string operations */

#include "vgagl.h"
#include "def.h"


/* #define USE_ASM */


#ifdef __alpha__

static inline int muldiv64(int m1, int m2, int d)
{
    return (long) m1 *(long) m2 / (long) d;
}

#else

/* We use the 32-bit to 64-bit multiply and 64-bit to 32-bit divide of the */
/* 386 (which gcc doesn't know well enough) to efficiently perform integer */
/* scaling without having to worry about overflows. */

static inline int muldiv64(int m1, int m2, int d)
{
/* int32 * int32 -> int64 / int32 -> int32 */
    int result;
    __asm__(
	       "imull %%edx\n\t"
	       "idivl %3\n\t"
  :	       "=a"(result)	/* out */
  :	       "a"(m1), "d"(m2), "g"(d)		/* in */
  :	       "ax", "dx"	/* mod */
	);
    return result;
}

#endif				/* !__alpha__ */

/* This is a DDA-based algorithm. */
/* Iteration over target bitmap. */

void gl_scalebox(int w1, int h1, void *_dp1, int w2, int h2, void *_dp2)
{
    uchar *dp1 = _dp1;
    uchar *dp2 = _dp2;
    int xfactor;
    int yfactor;

    if (w2 == 0 || h2 == 0)
	return;

    xfactor = muldiv64(w1, 65536, w2);	/* scaled by 65536 */
    yfactor = muldiv64(h1, 65536, h2);	/* scaled by 65536 */

    switch (BYTESPERPIXEL) {
    case 1:
	{
	    int y, sy;
	    sy = 0;
	    for (y = 0; y < h2;) {
		int sx = 0;
		uchar *dp2old = dp2;
		int x;
		x = 0;
		while (x < w2 - 8) {
#ifdef USE_ASM

		    /* This saves just a couple of cycles per */
		    /* pixel on a 486, but I couldn't resist. */
		    __asm__ __volatile__("movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 1 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 2 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 3 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 4 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 5 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 6 (%1, %2)\n\t"
					 "movl %4, %%eax\n\t"
					 "shrl $16, %%eax\n\t"
					 "addl %5, %4\n\t"
					 "movb (%3, %%eax), %%al\n\t"
					 "movb %%al, 7 (%1, %2)\n\t"
					 :	/* output */
					 :	/* input */
				     "ax"(0), "r"(dp2), "r"(x), "r"(dp1),
					 "r"(sx), "r"(xfactor)
					 :"ax", "4"
		    );
#else
		    *(dp2 + x) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 1) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 2) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 3) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 4) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 5) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 6) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    *(dp2 + x + 7) = *(dp1 + (sx >> 16));
		    sx += xfactor;
#endif
		    x += 8;
		}
		while (x < w2) {
		    *(dp2 + x) = *(dp1 + (sx >> 16));
		    sx += xfactor;
		    x++;
		}
		dp2 += w2;
		y++;
		while (y < h2) {
		    int l;
		    int syint = sy >> 16;
		    sy += yfactor;
		    if ((sy >> 16) != syint)
			break;
		    /* Copy identical lines. */
		    l = dp2 - dp2old;
		    __memcpy(dp2, dp2old, l);
		    dp2old = dp2;
		    dp2 += l;
		    y++;
		}
		dp1 = _dp1 + (sy >> 16) * w1;
	    }
	}
	break;
    case 2:
	{
	    int y, sy;
	    sy = 0;
	    for (y = 0; y < h2;) {
		int sx = 0;
		uchar *dp2old = dp2;
		int x;
		x = 0;
		/* This can be greatly optimized with loop */
		/* unrolling; omitted to save space. */
		while (x < w2) {
		    *(unsigned short *) (dp2 + x * 2) =
			*(unsigned short *) (dp1 + (sx >> 16) * 2);
		    sx += xfactor;
		    x++;
		}
		dp2 += w2 * 2;
		y++;
		while (y < h2) {
		    int l;
		    int syint = sy >> 16;
		    sy += yfactor;
		    if ((sy >> 16) != syint)
			break;
		    /* Copy identical lines. */
		    l = dp2 - dp2old;
		    __memcpy(dp2, dp2old, l);
		    dp2old = dp2;
		    dp2 += l;
		    y++;
		}
		dp1 = _dp1 + (sy >> 16) * w1 * 2;
	    }
	}
	break;
    case 3:
	{
	    int y, sy;
	    sy = 0;
	    for (y = 0; y < h2;) {
		int sx = 0;
		uchar *dp2old = dp2;
		int x;
		x = 0;
		/* This can be greatly optimized with loop */
		/* unrolling; omitted to save space. */
		while (x < w2) {
		    *(unsigned short *) (dp2 + x * 3) =
			*(unsigned short *) (dp1 + (sx >> 16) * 3);
		    *(unsigned char *) (dp2 + x * 3 + 2) =
			*(unsigned char *) (dp1 + (sx >> 16) * 3 + 2);
		    sx += xfactor;
		    x++;
		}
		dp2 += w2 * 3;
		y++;
		while (y < h2) {
		    int l;
		    int syint = sy >> 16;
		    sy += yfactor;
		    if ((sy >> 16) != syint)
			break;
		    /* Copy identical lines. */
		    l = dp2 - dp2old;
		    __memcpy(dp2, dp2old, l);
		    dp2old = dp2;
		    dp2 += l;
		    y++;
		}
		dp1 = _dp1 + (sy >> 16) * w1 * 3;
	    }
	}
	break;
    case 4:
	{
	    int y, sy;
	    sy = 0;
	    for (y = 0; y < h2;) {
		int sx = 0;
		uchar *dp2old = dp2;
		int x;
		x = 0;
		/* This can be greatly optimized with loop */
		/* unrolling; omitted to save space. */
		while (x < w2) {
		    *(unsigned *) (dp2 + x * 4) =
			*(unsigned *) (dp1 + (sx >> 16) * 4);
		    sx += xfactor;
		    x++;
		}
		dp2 += w2 * 4;
		y++;
		while (y < h2) {
		    int l;
		    int syint = sy >> 16;
		    sy += yfactor;
		    if ((sy >> 16) != syint)
			break;
		    /* Copy identical lines. */
		    l = dp2 - dp2old;
		    __memcpy(dp2, dp2old, l);
		    dp2old = dp2;
		    dp2 += l;
		    y++;
		}
		dp1 = _dp1 + (sy >> 16) * w1 * 4;
	    }
	}
	break;
    }
}
