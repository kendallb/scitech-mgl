/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* text.c       Text writing and fonts */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vga.h>
#include "inlstring.h"		/* include inline string operations */

#include "vgagl.h"
#include "def.h"



/* Text/font functions */

static int font_width = 8;
static int font_height = 8;
static char *font_address;
static int font_charactersize = 64;
static int font_writemode = WRITEMODE_OVERWRITE;
static int compressed_font_bg = 0;
static int compressed_font_fg = 15;

static void writecompressed(int x, int y, char *s);


void gl_colorfont(int fw, int fh, int fg, void *_dp)
{
    uchar *dp = _dp;
    int i;
    i = fw * fh * 256;
    switch (BYTESPERPIXEL) {
    case 1:
	while (i > 0) {
	    if (*dp)
		*dp = fg;
	    dp++;
	    i--;
	}
	break;
    case 2:
	while (i > 0) {
	    if (*(ushort *) dp)
		*(ushort *) dp = fg;
	    dp += 2;
	    i--;
	}
	break;
    case 3:
	while (i > 0) {
	    if (*(ushort *) dp || *(dp + 2)) {
		*(ushort *) dp = fg;
		*(dp + 2) = fg >> 16;
	    }
	    dp += 3;
	    i--;
	}
	break;
    case 4:
	while (i > 0) {
	    if (*(int *) dp)
		*(int *) dp = fg;
	    dp += 4;
	    i--;
	}
	break;

    }
}

void gl_setfont(int fw, int fh, void *font)
{
    font_width = fw;
    font_height = fh;
    font_charactersize = font_width * font_height * BYTESPERPIXEL;
    font_address = font;
}

void gl_setwritemode(int m)
{
    font_writemode = m;
}

void gl_write(int x, int y, char *s)
{
/* clipping in putbox */
    int i;
    if (font_writemode & FONT_COMPRESSED) {
	writecompressed(x, y, s);
	return;
    }
    i = 0;
    if (!(font_writemode & WRITEMODE_MASKED)) {
	while (s[i] != 0) {
	    gl_putbox(x + i * font_width, y, font_width,
		      font_height, font_address +
		      (unsigned char) s[i] * font_charactersize);
	    i++;
	}
    } else {			/* masked write */
	while (s[i] != 0) {
	    gl_putboxmask(x + i * font_width, y, font_width,
			  font_height, font_address +
			  (unsigned char) s[i] * font_charactersize);
	    i++;
	}
    }
}

void gl_writen(int x, int y, int n, char *s)
{
    char *str = alloca(n + 1);
    memcpy(str, s, n);
    str[n] = 0;
    gl_write(x, y, str);
}

#ifndef SVGA_AOUT

static int gl_nprintf(int sx, int sy, size_t bufs, const char *fmt, va_list args)
{
    char *buf;
    static int x = 0, y = 0, x_start = 0;
    int n;

    buf = alloca(bufs);
    n = vsnprintf(buf, bufs, fmt, args);
    if (n < 0)
	return n; /* buffer did not suffice, return and retry */

    if ((sx >= 0) && (sy >= 0)) {
	x = x_start = sx;
	y = sy;
    }
	
    for (; *buf; buf++)
	switch (*buf) {
	case '\a':		/* badly implemented */
	    fputc('\a', stdout);
	    fflush(stdout);
	    break;
	case '\b':
	    x -= font_width;
	    if (x < x_start) {
		x = WIDTH + (x_start % font_width);
		while(x + font_width > WIDTH)
		    x -= font_width;
		if (y >= font_height)
		    y -= font_height;
	    }
	    break;
	case '\n':
	  newline:
	    y += font_height;
	    if (y + font_height > HEIGHT)
		y %= font_height;
	case '\r':
	    x = x_start;
	    break;
	case '\t':
	    x += ((TEXT_TABSIZE - ((x - x_start) / font_width) % TEXT_TABSIZE) * font_width);
	    goto chk_wrap;
	    break;
	case '\v':
	    y += font_height;
	    if (y + font_height > HEIGHT)
		y %= font_height;
	    break;
	default:
	    gl_writen(x, y, 1, buf);
	    x += font_width;
	  chk_wrap:
	    if (x + font_width > WIDTH)
		goto newline;
	}
    return n;
}

int gl_printf(int x, int y, const char *fmt, ...)
{
    size_t bufs = BUFSIZ;
    int result;
    va_list args;

    va_start(args, fmt);

    /* Loop until buffer size suffices */
    do {
	result = gl_nprintf(x, y, bufs, fmt, args);
	bufs <<= 1;
    } while(result < 0);
	
    va_end(args);
    return result;
}

#endif
void gl_expandfont(int fw, int fh, int fg, void *_f1, void *_f2)
{
/* Convert bit-per-pixel font to byte(s)-per-pixel font */
    uchar *f1 = _f1;
    uchar *f2 = _f2;
    int i, x, y, b = 0;		/* keep gcc happy with b = 0 - MW */

    for (i = 0; i < 256; i++) {
	for (y = 0; y < fh; y++)
	    for (x = 0; x < fw; x++) {
		if (x % 8 == 0)
		    b = *f1++;
		if (b & (128 >> (x % 8)))	/* pixel */
		    switch (BYTESPERPIXEL) {
		    case 1:
			*f2 = fg;
			f2++;
			break;
		    case 2:
			*(ushort *) f2 = fg;
			f2 += 2;
			break;
		    case 3:
			*(ushort *) f2 = fg;
			*(f2 + 2) = fg >> 16;
			f2 += 3;
			break;
		    case 4:
			*(uint *) f2 = fg;
			f2 += 4;
		} else		/* no pixel */
		    switch (BYTESPERPIXEL) {
		    case 1:
			*f2 = 0;
			f2++;
			break;
		    case 2:
			*(ushort *) f2 = 0;
			f2 += 2;
			break;
		    case 3:
			*(ushort *) f2 = 0;
			*(f2 + 2) = 0;
			f2 += 3;
			break;
		    case 4:
			*(uint *) f2 = 0;
			f2 += 4;
		    }
	    }
    }
}

static void expandcharacter(int bg, int fg, int c, unsigned char *bitmap)
{
    int x, y;
    unsigned char *font;
    int b = 0;			/* keep gcc happy with b = 0 - MW */

    font = font_address + c * (font_height * ((font_width + 7) / 8));

    for (y = 0; y < font_height; y++)
	for (x = 0; x < font_width; x++) {
	    if (x % 8 == 0)
		b = *font++;
	    if (b & (128 >> (x % 8)))	/* pixel */
		switch (BYTESPERPIXEL) {
		case 1:
		    *bitmap = fg;
		    bitmap++;
		    break;
		case 2:
		    *(ushort *) bitmap = fg;
		    bitmap += 2;
		    break;
		case 3:
		    *(ushort *) bitmap = fg;
		    *(bitmap + 2) = fg >> 16;
		    bitmap += 3;
		    break;
		case 4:
		    *(uint *) bitmap = fg;
		    bitmap += 4;
	    } else		/* background pixel */
		switch (BYTESPERPIXEL) {
		case 1:
		    *bitmap = bg;
		    bitmap++;
		    break;
		case 2:
		    *(ushort *) bitmap = bg;
		    bitmap += 2;
		    break;
		case 3:
		    *(ushort *) bitmap = bg;
		    *(bitmap + 2) = bg;
		    bitmap += 3;
		    break;
		case 4:
		    *(uint *) bitmap = bg;
		    bitmap += 4;
		}
	}
}

/* Write using compressed font. */

static void writecompressed(int x, int y, char *s)
{
    unsigned char *bitmap;
    int i;
    i = 0;
    bitmap = alloca(font_width * font_height * BYTESPERPIXEL);
    if (!(font_writemode & WRITEMODE_MASKED)) {
	while (s[i] != 0) {
	    expandcharacter(compressed_font_bg,
			    compressed_font_fg, s[i], bitmap);
	    gl_putbox(x + i * font_width, y, font_width,
		      font_height, bitmap);
	    i++;
	}
    } else {			/* masked write */
	while (s[i] != 0) {
	    expandcharacter(0, compressed_font_fg, s[i], bitmap);
	    gl_putboxmask(x + i * font_width, y, font_width,
			  font_height, bitmap);
	    i++;
	}
    }
}

void gl_setfontcolors(int bg, int fg)
{
    compressed_font_bg = bg;
    compressed_font_fg = fg;
}
