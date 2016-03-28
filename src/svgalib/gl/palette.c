/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* palette.c    Palette functions (wrapper over vgalib) */


#include <stdlib.h>
#include <vga.h>

#include "vgagl.h"
#include "def.h"


/* 256-color palette functions */

/* There was a horrible bug here in 0.8x -- green and blue were swapped... */
void gl_getpalettecolor(int c, int *r, int *g, int *b)
{
    vga_getpalette(c, r, g, b);
}

void gl_setpalettecolor(int c, int r, int g, int b)
{
    vga_setpalette(c, r, g, b);
}

void gl_setpalettecolors(int s, int n, void *_dp)
{
    uchar *dp = _dp;
    int i;
    for (i = s; i < s + n; i++) {
	unsigned char r, g, b;
	r = *(dp++);
	g = *(dp++);
	b = *(dp++);
	vga_setpalette(i, r, g, b);
    }
}

void gl_getpalettecolors(int s, int n, void *_dp)
{
    uchar *dp = _dp;
    int i;
    for (i = s; i < s + n; i++) {
	int r, g, b;
	vga_getpalette(i, &r, &g, &b);
	*(dp++) = (unsigned char) r;
	*(dp++) = (unsigned char) g;
	*(dp++) = (unsigned char) b;
    }
}

void gl_getpalette(void *p)
{
    gl_getpalettecolors(0, 256, p);
}

void gl_setpalette(void *p)
{
    gl_setpalettecolors(0, 256, p);
}

void gl_setrgbpalette()
{
    int i;
    Palette pal;
    for (i = 0; i < 256; i++) {
	pal.color[i].blue = (i & 7) * (64 / 8);		/* 3 bits */
	pal.color[i].green = ((i & 56) >> 3) * (64 / 8);	/* 3 bits */
	pal.color[i].red = ((i & 192) >> 6) * (64 / 4);		/* 2 bits */
    }
    gl_setpalette(&pal);
}
