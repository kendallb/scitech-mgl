/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* cbitmap.c    Compiled bitmaps */


#include <stdio.h>
#include <stdlib.h>
#include <vga.h>
#include "inlstring.h"		/* include inline string operations */

#include "vgagl.h"
#include "def.h"



void gl_compileboxmask(int w, int h, void *_dp1, void *_dp2)
{
/* Compiled format: <bytes_to_skip (0-254)><number_of_pixels (0-255)> */
/*                  <pixel_data>[<end_of_line(255)>]... */
    uchar *dp1 = _dp1;
    uchar *dp2 = _dp2;
    int i;
    for (i = 0; i < h; i++) {
	int x = 0;
	while (x < w) {
	    int count;
	    /* count zeroes */
	    count = 0;
	    while (x < w && *(dp1 + count) == 0 && count < 254) {
		count++;
		x++;
	    }
	    dp1 += count;
	    if (x < w) {
		*dp2++ = count;
		/* count nonzeroes */
		count = 0;
		while (x < w && *(dp1 + count) != 0 && count < 255) {
		    *(dp2 + count + 1) = *(dp1 + count);
		    count++;
		    x++;
		}
		*dp2 = count;
		dp2 += count + 1;
		dp1 += count;
	    }
	}
	*dp2++ = 0xff;
    }
}

int gl_compiledboxmasksize(int w, int h, void *_dp1)
{
/* Compiled format: <bytes_to_skip (0-254)><number_of_pixels (0-255)> */
/*                  <pixel_data>[<end_of_line(255)>]... */
    uchar *dp1 = _dp1;
    int size = 0;
    int i;
    for (i = 0; i < h; i++) {
	int x = 0;
	while (x < w) {
	    int count;
	    /* count zeroes */
	    count = 0;
	    while (x < w && *(dp1 + count) == 0 && count < 254) {
		count++;
		x++;
	    }
	    size++;
	    dp1 += count;
	    /* count nonzeroes */
	    if (x < w) {
		count = 0;
		while (x < w && *(dp1 + count) != 0 && count < 255) {
		    count++;
		    x++;
		}
		size += count + 1;
		dp1 += count;
	    }
	}
	size++;
    }
    return size;
}

static void gl_putboxmaskcompiledclip(int nx, int ny, int nw, int nh, int _x,
				      int _y, int w, int h, void *_dp)
{
/* Special case costly clipping */
    uchar *dp = _dp;
    uchar *vp, *vpline;
    int y;
    vpline = VBUF + _y * BYTEWIDTH + _x;
    for (y = _y; y < ny + nh; y++) {
	int x = _x;
	vp = vpline;
	for (;;) {
	    int count = *dp++;
	    if (count == 0xff)
		break;		/* end of line */
	    vp += count;
	    x += count;
	    count = *dp++;
	    /* __memcpy gives severe bug here */
	    if (y >= ny)
		if (x >= nx)
		    if (x + count > __clipx2 + 1) {
			if (x <= __clipx2)
			    __memcpyb(vp, dp, __clipx2 - x + 1);
		    } else
			__memcpyb(vp, dp, count);
		else if (x + count > __clipx1)
		    if (x + count > __clipx2 + 1)
			__memcpyb(vp + __clipx1 - x,
				  dp + __clipx1 - x,
				  __clipx2 - __clipx1 + 1);
		    else
			__memcpy(vp + __clipx1 - x,
				 dp + __clipx1 - x,
				 count - __clipx1 + x);
	    x += count;
	    vp += count;
	    dp += count;
	}
	vpline += BYTEWIDTH;
    }
}

#define ADJUSTBITMAPBOX() \
	nw = w; nh = h; nx = x; ny = y;				\
	if (nx + nw < __clipx1 || nx > __clipx2)		\
		return;						\
	if (ny + nh < __clipy1 || ny > __clipy2)		\
		return;						\
	if (nx < __clipx1) {		/* left adjust */	\
		nw += nx - __clipx1;				\
		nx = __clipx1;					\
	}							\
	if (ny < __clipy1) {		/* top adjust */	\
		nh += ny - __clipy1;				\
		ny = __clipy1;					\
	}							\
	if (nx + nw > __clipx2)		/* right adjust */	\
		nw = __clipx2 - nx + 1;				\
	if (ny + nh > __clipy2)		/* bottom adjust */	\
		nh = __clipy2 - ny + 1;				\


void gl_putboxmaskcompiled(int x, int y, int w, int h, void *_dp)
{
/* no clipping */
    uchar *dp = _dp;
    uchar *vp, *vpline;
    int i;
    if (MODETYPE != CONTEXT_LINEAR && MODETYPE != CONTEXT_VIRTUAL) {
	printf("vgagl: putboxmaskcompiled only supported in linear framebuffer\n");
	return;
    }
    if (__clip) {
	int nx, ny, nw, nh;
	ADJUSTBITMAPBOX();
	if (nw != w || nh != h) {
	    gl_putboxmaskcompiledclip(nx, ny, nw, nh, x, y, w, h,
				      dp);
	    return;
	}
    }
    vpline = VBUF + y * BYTEWIDTH + x;
    for (i = 0; i < h; i++) {
	vp = vpline;
	for (;;) {
	    int count = *dp++;
	    if (count == 0xff)
		break;		/* end of line */
	    vp += count;
	    count = *dp++;
	    /* __memcpy gives severe bug here */
	    __memcpyb(vp, dp, count);
	    vp += count;
	    dp += count;
	}
	vpline += BYTEWIDTH;
    }
}
