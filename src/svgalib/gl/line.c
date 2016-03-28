/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* line.c       Line drawing */


#include <stdlib.h>

#ifndef DO_NOT_USE_VGALIB
#include <vga.h>
#endif

#include "inlstring.h"		/* include inline string operations */

#include "vgagl.h"
#include "def.h"
#include "driver.h"


#ifdef __alpha__

static inline int muldiv64(int m1, int m2, int d)
{
    return (long) m1 *(long) m2 / (long) d;
}

#else

#ifdef NO_ASSEMBLY

static inline int muldiv64(int m1, int m2, int d)
{
    return (float) m1 * (float) m2 / ((float) d);
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

#endif				/* !NO_ASSEMBLY */
#endif				/* !__alpha__ */

#ifdef __alpha__

static inline int gl_regioncode(int x, int y)
{
    int result = 0;
    if (x < __clipx1)
	result |= 1;
    else if (x > __clipx2)
	result |= 2;
    if (y < __clipy1)
	result |= 4;
    else if (y > __clipy2)
	result |= 8;
    return result;
}

#else
#ifdef NO_ASSEMBLY

static inline int gl_regioncode (int x, int y)
{
    int result = 0;
    if (x < __clipx1)
	result |= 1;
    else if (x > __clipx2)
	result |= 2;
    if (y < __clipy1)
	result |= 4;
    else if (y > __clipy2)
	result |= 8;
    return result;
}

#else

#define INC_IF_NEG(y, result)				\
{							\
	__asm__("btl $31,%1\n\t"			\
		"adcl $0,%0"				\
		: "=r" ((int) result)			\
		: "rm" ((int) (y)), "0" ((int) result)	\
		);					\
}

static inline int gl_regioncode (int x, int y)
{
    int dx1, dx2, dy1, dy2;
    int result;
    result = 0;
    dy2 = __clipy2 - y;
    INC_IF_NEG (dy2, result);
    result <<= 1;
    dy1 = y - __clipy1;
    INC_IF_NEG (dy1, result);
    result <<= 1;
    dx2 = __clipx2 - x;
    INC_IF_NEG (dx2, result);
    result <<= 1;
    dx1 = x - __clipx1;
    INC_IF_NEG (dx1, result);
    return result;
}

#endif				/* ! NO_ASSEMBLY */
#endif				/* !__alpha__ */


#define line_start_paged(s) \
	    fp = y * bytesperrow + x * s;	\
	    vga_setpage (fpp = (fp >> 16));	\
	    fp &= 0xFFFF;

#define line_start_linear(s) \
	    vp = VBUF + y * bytesperrow + x * s;


#define line_loop_paged_a(m,i,u,v)	\
	    {    \
	    int d = ay - (ax >> 1);	\
	    if ((x = abs (dx)))	\
		do {	\
		    i;	\
		    if (d m 0) {	\
			fp v;	\
			d -= ax;	\
		    }	\
		    fp u;	\
		    d += ay;	\
		    if (fp & 0xFFFF0000) {	/* has it cross a page boundary ? */	\
			fpp += fp >> 16;	\
			vga_setpage (fpp);	\
		    }	\
		    fp &= 0x0000FFFF;	\
		} while (--x);    \
	    }

#define line_loop_linear_a(m,i,u,v)	\
	    {    \
	    int d = ay - (ax >> 1);	\
	    if ((x = abs (dx)))	\
		do {	\
		    i;	\
		    if (d m 0) {	\
			vp v;	\
			d -= ax;	\
		    }	\
		    vp u;	\
		    d += ay;	\
		} while (--x);    \
	    }


#define line_loop_paged_b(m,i,u,v)	\
		{    \
		int d = ax - (ay >> 1);	\
		if ((y = abs (dy)))	\
		    do {	\
			i;	\
			if (d m 0) {	\
			    fp u;	\
			    d -= ay;	\
			}	\
			fp v;	\
			d += ax;	\
			if (fp & 0xFFFF0000) {	\
			    fpp += fp >> 16;	\
			    vga_setpage (fpp);	\
			}	\
			fp &= 0x0000FFFF;	\
		    } while (--y);    \
		}


#define line_loop_linear_b(m,i,u,v)	\
		{    \
		int d = ax - (ay >> 1);	\
		if ((y = abs (dy)))	\
		    do {	\
			i;	\
			if (d m 0) {	\
			    vp u;	\
			    d -= ay;	\
			}	\
			vp v;	\
			d += ax;	\
		    } while (--y);    \
		}



/* Partly based on the work which was partly based on vgalib by Tommy Frandsen */
/* This is a lot faster now that setpixel is inlined */

void gl_line (int x1, int y1, int x2, int y2, int c)
{
    int dx, dy, ax, ay, sx, sy, x, y;
    int bytesperrow;
    unsigned char *vp = NULL;

    if (__clip)
	/* Cohen & Sutherland algorithm */
	for (;;) {
	    int r1 = gl_regioncode (x1, y1);
	    int r2 = gl_regioncode (x2, y2);
	    if (!(r1 | r2))
		break;		/* completely inside */
	    if (r1 & r2)
		return;		/* completely outside */
	    if (r1 == 0) {
		swap (x1, x2);	/* make sure first */
		swap (y1, y2);	/* point is outside */
		r1 = r2;
	    }
	    if (r1 & 1) {	/* left */
		y1 += muldiv64 (__clipx1 - x1, y2 - y1, x2 - x1);
		x1 = __clipx1;
	    } else if (r1 & 2) {	/* right */
		y1 += muldiv64 (__clipx2 - x1, y2 - y1, x2 - x1);
		x1 = __clipx2;
	    } else if (r1 & 4) {	/* top */
		x1 += muldiv64 (__clipy1 - y1, x2 - x1, y2 - y1);
		y1 = __clipy1;
	    } else if (r1 & 8) {	/* bottom */
		x1 += muldiv64 (__clipy2 - y1, x2 - x1, y2 - y1);
		y1 = __clipy2;
	    }
	}
    dx = x2 - x1;
    dy = y2 - y1;
    ax = abs (dx) << 1;
    ay = abs (dy) << 1;
    sx = (dx >= 0) ? 1 : -1;
    sy = (dy >= 0) ? 1 : -1;
    x = x1;
    y = y1;

#ifdef __alpha__
    if (ax > ay) {
	int d = ay - (ax >> 1);
	while (x != x2) {
	    setpixel (x, y, c);

	    if (d > 0 || (d == 0 && sx == 1)) {
		y += sy;
		d -= ax;
	    }
	    x += sx;
	    d += ay;
	}
    } else {
	int d = ax - (ay >> 1);
	while (y != y2) {
	    setpixel (x, y, c);

	    if (d > 0 || (d == 0 && sy == 1)) {
		x += sx;
		d -= ay;
	    }
	    y += sy;
	    d += ax;
	}
    }

    setpixel (x, y, c);

#else

#define insert_pixel_1 *((unsigned char *) vp) = c;
#define insert_pixel_2 *((unsigned short *) vp) = c;

#define insert_pixel_3 *((unsigned char *) vp) = c;    \
		    *((unsigned char *) (vp + 1)) = (c>>8);    \
		    *((unsigned char *) (vp + 2)) = (c>>16);

#define insert_pixel_4 *((unsigned long *) vp) = c;

    bytesperrow = BYTEWIDTH;

    if (MODETYPE == CONTEXT_VIRTUAL || MODETYPE == CONTEXT_LINEAR) {
	switch BYTESPERPIXEL {
	case 1:
	    line_start_linear(1);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_linear_a(>=,insert_pixel_1,++,+=bytesperrow*sy);
		} else {
		    line_loop_linear_a(>,insert_pixel_1,--,+=bytesperrow*sy);
		}
	    } else {
		if(sy > 0) {
		    line_loop_linear_b(>=,insert_pixel_1,+=sx,+=bytesperrow);
		} else {
		    line_loop_linear_b(>,insert_pixel_1,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_1;
	    break;
	case 2:
	    line_start_linear(2);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_linear_a(>=,insert_pixel_2,+=2,+=bytesperrow*sy);
		} else {
		    line_loop_linear_a(>,insert_pixel_2,-=2,+=bytesperrow*sy);
		}
	    } else {
		sx <<= 1;
		if(sy > 0) {
		    line_loop_linear_b(>=,insert_pixel_2,+=sx,+=bytesperrow);
		} else {
		    line_loop_linear_b(>,insert_pixel_2,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_2;
	    break;
	case 3:
	    line_start_linear(3);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_linear_a(>=,insert_pixel_3,+=3,+=bytesperrow*sy);
		} else {
		    line_loop_linear_a(>,insert_pixel_3,-=3,+=bytesperrow*sy);
		}
	    } else {
		sx *= 3;
		if(sy > 0) {
		    line_loop_linear_b(>=,insert_pixel_3,+=sx,+=bytesperrow);
		} else {
		    line_loop_linear_b(>,insert_pixel_3,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_3;
	    break;
	case 4:
	    line_start_linear(4);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_linear_a(>=,insert_pixel_4,+=4,+=bytesperrow*sy);
		} else {
		    line_loop_linear_a(>,insert_pixel_4,-=4,+=bytesperrow*sy);
		}
	    } else {
		sx <<= 2;
		if(sy > 0) {
		    line_loop_linear_b(>=,insert_pixel_4,+=sx,+=bytesperrow);
		} else {
		    line_loop_linear_b(>,insert_pixel_4,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_4;
	    break;
	}
    }




#ifndef DO_NOT_USE_VGALIB

#undef insert_pixel_1
#undef insert_pixel_2
#undef insert_pixel_3
#undef insert_pixel_4

#define insert_pixel_1 *((unsigned char *) (vp + fp)) = c;
#define insert_pixel_2 *((unsigned short *) (vp + fp)) = c;

#define insert_pixel_3 *((unsigned char *) (vp + fp)) = c;    \
		    *((unsigned char *) (vp + fp + 1)) = (c>>8);    \
		    *((unsigned char *) (vp + fp + 2)) = (c>>16);

#define insert_pixel_4 *((unsigned long *) (vp + fp)) = c;


    if (MODETYPE == CONTEXT_PAGED) {
	vp = VBUF;
	switch BYTESPERPIXEL {
	int fpp;
	int fp;
	case 1:
	    line_start_paged(1);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_paged_a(>=,insert_pixel_1,++,+=bytesperrow*sy);
		} else {
		    line_loop_paged_a(>,insert_pixel_1,--,+=bytesperrow*sy);
		}
	    } else {
		if(sy > 0) {
		    line_loop_paged_b(>=,insert_pixel_1,+=sx,+=bytesperrow);
		} else {
		    line_loop_paged_b(>,insert_pixel_1,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_1;
	    break;
	case 2:
	    line_start_paged(2);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_paged_a(>=,insert_pixel_2,+=2,+=bytesperrow*sy);
		} else {
		    line_loop_paged_a(>,insert_pixel_2,-=2,+=bytesperrow*sy);
		}
	    } else {
		sx <<= 1;
		if(sy > 0) {
		    line_loop_paged_b(>=,insert_pixel_2,+=sx,+=bytesperrow);
		} else {
		    line_loop_paged_b(>,insert_pixel_2,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_2;
	    break;
	case 3:
	    line_start_paged(3);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_paged_a(>=,insert_pixel_3,+=3,+=bytesperrow*sy);
		} else {
		    line_loop_paged_a(>,insert_pixel_3,-=3,+=bytesperrow*sy);
		}
	    } else {
		sx *= 3;
		if(sy > 0) {
		    line_loop_paged_b(>=,insert_pixel_3,+=sx,+=bytesperrow);
		} else {
		    line_loop_paged_b(>,insert_pixel_3,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_3;
	    break;
	case 4:
	    line_start_paged(4);
	    if (ax > ay) {
		if(sx > 0) {
		    line_loop_paged_a(>=,insert_pixel_4,+=4,+=bytesperrow*sy);
		} else {
		    line_loop_paged_a(>,insert_pixel_4,-=4,+=bytesperrow*sy);
		}
	    } else {
		sx <<= 2;
		if(sy > 0) {
		    line_loop_paged_b(>=,insert_pixel_4,+=sx,+=bytesperrow);
		} else {
		    line_loop_paged_b(>,insert_pixel_4,+=sx,-=bytesperrow);
		}
	    }
	    insert_pixel_4;
	    break;
	}
    }

#endif

    if (!vp) {

	if (ax > ay) {
	    int d = ay - (ax >> 1);
	    while (x != x2) {
		setpixel (x, y, c);

		if (d > 0 || (d == 0 && sx == 1)) {
		    y += sy;
		    d -= ax;
		}
		x += sx;
		d += ay;
	    }
	} else {
	    int d = ax - (ay >> 1);
	    while (y != y2) {
		setpixel (x, y, c);

		if (d > 0 || (d == 0 && sy == 1)) {
		    x += sx;
		    d -= ay;
		}
		y += sy;
		d += ax;
	    }
	}
	setpixel (x, y, c);
    }
#endif
}


static void gl_setcirclepixels(int x, int y, int sx, int sy, int c)
{
    if (__clip) {
	int z = max(x, y);
	if (sx - z < __clipx1 || sx + z > __clipx2
	    || sy - z < __clipy1 || sy + z > __clipy2) {
	    /* use setpixel clipping */
	    gl_setpixel(sx + x, sy + y, c);
	    gl_setpixel(sx - x, sy + y, c);
	    gl_setpixel(sx + x, sy - y, c);
	    gl_setpixel(sx - x, sy - y, c);
	    gl_setpixel(sx + y, sy + x, c);
	    gl_setpixel(sx - y, sy + x, c);
	    gl_setpixel(sx + y, sy - x, c);
	    gl_setpixel(sx - y, sy - x, c);
	    return;
	}
    }
    setpixel(sx + x, sy + y, c);
    setpixel(sx - x, sy + y, c);
    setpixel(sx + x, sy - y, c);
    setpixel(sx - x, sy - y, c);
    setpixel(sx + y, sy + x, c);
    setpixel(sx - y, sy + x, c);
    setpixel(sx + y, sy - x, c);
    setpixel(sx - y, sy - x, c);
}

void gl_circle(int sx, int sy, int r, int c)
{
    int x, y, d;
    if (r < 1) {
	gl_setpixel(sx, sy, c);
	return;
    }
    if (__clip)
	if (sx + r < __clipx1 || sx - r > __clipx2
	    || sy + r < __clipy1 || sy - r > __clipy2)
	    return;
    x = 0;
    y = r;
    d = 1 - r;
    gl_setcirclepixels(x, y, sx, sy, c);
    while (x < y) {
	if (d < 0)
	    d += x * 2 + 3;
	else {
	    d += x * 2 - y * 2 + 5;
	    y--;
	}
	x++;
	gl_setcirclepixels(x, y, sx, sy, c);
    }
}
