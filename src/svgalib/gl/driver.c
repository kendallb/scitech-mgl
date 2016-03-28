/* driver.c     Framebuffer primitives */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>

#include "inlstring.h"		/* include inline string operations */
#include "vgagl.h"
#include "def.h"
#include "driver.h"


#define MAXBYTEWIDTH 4096	/* used in bitblt emulation */

/* All functions that simply call another function with slightly different
 * parameter values are declared inline. */
#define INLINE inline

#define NOTIMPL(s) { notimplemented(s); return; }

/* in: vp = video offset; out: rvp = video pointer, chunksize, page */
#define SETWRITEPAGED(vp, rvp, chunksize, page) \
	page = vp >> 16; \
	vga_setpage(page); \
	rvp = (vp & 0xffff) + VBUF; \
	chunksize = 0x10000 - (vp & 0xffff);

static inline int RGB2BGR(int c)
{
/* a bswap would do the same as the first 3 but in only ONE! cycle. */
/* However bswap is not supported by 386 */

    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED)
#ifdef __alpha__
	c = ((c >> 0) & 0xff) << 16 |
	    ((c >> 8) & 0xff) << 8 |
	    ((c >> 16) & 0xff) << 0;
#else
	asm("rorw  $8, %0\n"	/* 0RGB -> 0RBG */
	    "rorl $16, %0\n"	/* 0RBG -> BG0R */
	    "rorw  $8, %0\n"	/* BG0R -> BGR0 */
	    "shrl  $8, %0\n"	/* 0BGR -> 0BGR */
      : "=q"(c):"0"(c));
#endif
    return c;
}

/* RGB_swapped_memcopy returns the amount of bytes unhandled */
static inline int RGB_swapped_memcpy(char *dest, char *source, int len)
{
    int rest, tmp;

    tmp = len / 3;
    rest = len - 3 * tmp;
    len = tmp;

    while (len--) {
	*dest++ = source[2];
	*dest++ = source[1];
	*dest++ = source[0];
	source += 3;
    }

    return rest;
}

static void notimplemented(char *s)
{
    printf("vgagl: %s not implemented.\n", s);
}



/* One byte per pixel frame buffer primitives */

#define ASSIGNVP8(x, y, vp) vp = VBUF + (y) * BYTEWIDTH + (x);
#define ASSIGNVPOFFSET8(x, y, vp) vp = (y) * BYTEWIDTH + (x);

void __svgalib_driver8_setpixel(int x, int y, int c)
{
#ifdef __alpha__
    vga_setcolor(c);
    vga_drawpixel(x, y);
#else
    char *vp;
    ASSIGNVP8(x, y, vp);
    *vp = c;
#endif
}

void __svgalib_driver8p_setpixel(int x, int y, int c)
{
    int vp;
    ASSIGNVPOFFSET8(x, y, vp);
    vga_setpage(vp >> 16);
    *(VBUF + (vp & 0xffff)) = c;
}

int __svgalib_driver8_getpixel(int x, int y)
{
    char *vp;
    ASSIGNVP8(x, y, vp);
    return *vp;
}

int __svgalib_driver8p_getpixel(int x, int y)
{
    int vp;
    ASSIGNVPOFFSET8(x, y, vp);
    vga_setpage(vp >> 16);
    return *(VBUF + (vp & 0xffff));
}

void __svgalib_driver8_hline(int x1, int y, int x2, int c)
{
    char *vp;
    ASSIGNVP8(x1, y, vp);
    __memset(vp, c, x2 - x1 + 1);
}

void __svgalib_driver8p_hline(int x1, int y, int x2, int c)
{
    int vp;
    char *rvp;
    int l;
    int chunksize, page;
    ASSIGNVPOFFSET8(x1, y, vp);
    SETWRITEPAGED(vp, rvp, chunksize, page);
    l = x2 - x1 + 1;
    if (l <= chunksize)
	__memset(rvp, c, l);
    else {
	__memset(rvp, c, chunksize);
	vga_setpage(page + 1);
	__memset(VBUF, c, l - chunksize);
    }
}

void __svgalib_driver8_fillbox(int x, int y, int w, int h, int c)
{
    char *vp;
    int i;
    ASSIGNVP8(x, y, vp);
    for (i = 0; i < h; i++) {
	__memset(vp, c, w);
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8a_fillbox(int x, int y, int w, int h, int c)
{
    if (w * h < 128)
	(*__svgalib_nonaccel_fillbox)(x, y, w, h, c);
    else {
	vga_accel(ACCEL_SETFGCOLOR, c);
	vga_accel(ACCEL_FILLBOX, x, y, w, h);
    }
}

void __svgalib_driver8p_fillbox(int x, int y, int w, int h, int c)
{
    int vp;
    int page;
    int i;
    ASSIGNVPOFFSET8(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		__memset(VBUF + vp, c, 0x10000 - vp);
		page++;
		vga_setpage(page);
		__memset(VBUF, c, (vp + w) & 0xffff);
		vp = (vp + BYTEWIDTH) & 0xffff;
		continue;
	    }
	__memset(VBUF + vp, c, w);
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8_putbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP8(x, y, vp);
    bp = b;
    for (i = 0; i < h; i++) {
	__memcpy(vp, bp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8p_putbox(int x, int y, int w, int h, void *b, int bw)
{
/* extra argument width of source bitmap, so that putboxpart can use this */
    int vp;
    int page;
    char *bp = b;
    int i;
    ASSIGNVPOFFSET8(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		__memcpy(VBUF + vp, bp, 0x10000 - vp);
		page++;
		vga_setpage(page);
		__memcpy(VBUF, bp + 0x10000 - vp,
			 (vp + w) & 0xffff);
		vp = (vp + BYTEWIDTH) & 0xffff;
		bp += bw;
		continue;
	    }
	__memcpy(VBUF + vp, bp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8_getbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP8(x, y, vp);
    bp = b;
    for (i = 0; i < h; i++) {
	__memcpy(bp, vp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8p_getbox(int x, int y, int w, int h, void *b, int bw)
{
    int vp;
    int page;
    char *bp = b;
    int i;
    ASSIGNVPOFFSET8(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		__memcpy(bp, VBUF + vp, 0x10000 - vp);
		page++;
		vga_setpage(page);
		__memcpy(bp + 0x10000 - vp, VBUF,
			 (vp + w) & 0xffff);
		vp = (vp + BYTEWIDTH) & 0xffff;
		bp += bw;
		continue;
	    }
	__memcpy(bp, VBUF + vp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver8_putboxmask(int x, int y, int w, int h, void *b)
{
    uchar *bp = b;
    uchar *vp;
    int i;
    ASSIGNVP8(x, y, vp);
    for (i = 0; i < h; i++) {
	uchar *endoflinebp = bp + w;
	while (bp < endoflinebp - 3) {
	    unsigned int c4 = *(unsigned int *) bp;
	    if (c4 & 0xff)
		*vp = (uchar) c4;
	    c4 >>= 8;
	    if (c4 & 0xff)
		*(vp + 1) = (uchar) c4;
	    c4 >>= 8;
	    if (c4 & 0xff)
		*(vp + 2) = (uchar) c4;
	    c4 >>= 8;
	    if (c4 & 0xff)
		*(vp + 3) = (uchar) c4;
	    bp += 4;
	    vp += 4;
	}
	while (bp < endoflinebp) {
	    uchar c = *bp;
	    if (c)
		*vp = c;
	    bp++;
	    vp++;
	}
	vp += BYTEWIDTH - w;
    }
}

void __svgalib_driver8_putboxpart(int x, int y, int w, int h, int ow, int oh,
			void *b, int xo, int yo)
{
    __svgalib_driver8_putbox(x, y, w, h, b + yo * ow + xo, ow);	/* inlined */
}

void __svgalib_driver8p_putboxpart(int x, int y, int w, int h, int ow, int oh,
			 void *b, int xo, int yo)
{
    __svgalib_driver8p_putbox(x, y, w, h, b + yo * ow + xo, ow);	/* inlined */
}

void __svgalib_driver8_getboxpart(int x, int y, int w, int h, int ow, int oh,
			void *b, int xo, int yo)
{
    __svgalib_driver8_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

void __svgalib_driver8p_getboxpart(int x, int y, int w, int h, int ow, int oh,
			 void *b, int xo, int yo)
{
    __svgalib_driver8p_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

void __svgalib_driver8_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    char *svp, *dvp;
    /* I hope this works now. */
    if (y1 >= y2) {
	if (y1 == y2 && x2 >= x1) {	/* tricky */
	    int i;
	    if (x1 == x2)
		return;
	    /* use a temporary buffer to store a line */
	    /* using reversed movs would be much faster */
	    ASSIGNVP8(x1, y1, svp);
	    ASSIGNVP8(x2, y2, dvp);
	    for (i = 0; i < h; i++) {
		uchar linebuf[MAXBYTEWIDTH];
		__memcpy(linebuf, svp, w);
		__memcpy(dvp, linebuf, w);
		svp += BYTEWIDTH;
		dvp += BYTEWIDTH;
	    }
	} else {		/* copy from top to bottom */
	    int i;
	    ASSIGNVP8(x1, y1, svp);
	    ASSIGNVP8(x2, y2, dvp);
	    for (i = 0; i < h; i++) {
		__memcpy(dvp, svp, w);
		svp += BYTEWIDTH;
		dvp += BYTEWIDTH;
	    }
	}
    } else {			/* copy from bottom to top */
	int i;
	ASSIGNVP8(x1, y1 + h, svp);
	ASSIGNVP8(x2, y2 + h, dvp);
	for (i = 0; i < h; i++) {
	    svp -= BYTEWIDTH;
	    dvp -= BYTEWIDTH;
	    __memcpy(dvp, svp, w);
	}
    }
}

void __svgalib_driver8a_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    vga_accel(ACCEL_SCREENCOPY, x1, y1, x2, y2, w, h);
}



/* Two bytes per pixel graphics primitives */

#define ASSIGNVP16(x, y, vp) vp = VBUF + (y) * BYTEWIDTH + (x) * 2;
#define ASSIGNVPOFFSET16(x, y, vp) vp = (y) * BYTEWIDTH + (x) * 2;

void __svgalib_driver16_setpixel(int x, int y, int c)
{
    char *vp;
    ASSIGNVP16(x, y, vp);
    *(unsigned short *) vp = c;
}

void __svgalib_driver16p_setpixel(int x, int y, int c)
{
    int vp;
    ASSIGNVPOFFSET16(x, y, vp);
    vga_setpage(vp >> 16);
    *(unsigned short *) (VBUF + (vp & 0xffff)) = c;
}

int __svgalib_driver16_getpixel(int x, int y)
{
    char *vp;
    ASSIGNVP16(x, y, vp);
    return *(unsigned short *) vp;
}

int __svgalib_driver16p_getpixel(int x, int y)
{
    int vp;
    ASSIGNVPOFFSET16(x, y, vp);
    vga_setpage(vp >> 16);
    return *(unsigned short *) (VBUF + (vp & 0xffff));
}

void __svgalib_driver16_hline(int x1, int y, int x2, int c)
{
    char *vp;
    ASSIGNVP16(x1, y, vp);
    __memset2(vp, c, x2 - x1 + 1);
}

void __svgalib_driver16p_hline(int x1, int y, int x2, int c)
{
    int vp;
    char *rvp;
    int l;
    int chunksize, page;
    ASSIGNVPOFFSET16(x1, y, vp);
    SETWRITEPAGED(vp, rvp, chunksize, page);
    l = (x2 - x1 + 1) * 2;
    if (l <= chunksize)
	__memset2(rvp, c, l / 2);
    else {
	__memset2(rvp, c, chunksize / 2);
	vga_setpage(page + 1);
	__memset2(VBUF, c, (l - chunksize) / 2);
    }
}

void __svgalib_driver16_fillbox(int x, int y, int w, int h, int c)
{
    char *vp;
    int i;
    ASSIGNVP16(x, y, vp);
    for (i = 0; i < h; i++) {
	__memset2(vp, c, w);
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver16p_fillbox(int x, int y, int w, int h, int c)
{
    int vp;
    int page;
    int i;
    ASSIGNVPOFFSET16(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w * 2 > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		__memset2(VBUF + vp, c, (0x10000 - vp) / 2);
		page++;
		vga_setpage(page);
		__memset2(VBUF, c, ((vp + w * 2) & 0xffff) / 2);
		vp = (vp + BYTEWIDTH) & 0xffff;
		continue;
	    }
	__memset2(VBUF + vp, c, w);
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver16_putbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP16(x, y, vp);
    bp = b;
    for (i = 0; i < h; i++) {
	__memcpy(vp, bp, w * 2);
	bp += bw * 2;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver16p_putbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8p_putbox(x * 2, y, w * 2, h, b, bw * 2);
}

void __svgalib_driver16_getbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP16(x, y, vp);
    bp = b;
    for (i = 0; i < h; i++) {
	__memcpy(bp, vp, w * 2);
	bp += bw * 2;
	vp += BYTEWIDTH;
    }
}

INLINE void __svgalib_driver16p_getbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8p_getbox(x * 2, y, w * 2, h, b, bw * 2);
}

void __svgalib_driver16_putboxmask(int x, int y, int w, int h, void *b)
{
    uchar *bp = b;
    uchar *vp;
    int i;
    ASSIGNVP16(x, y, vp);
    for (i = 0; i < h; i++) {
	uchar *endoflinebp = bp + w * 2;
	while (bp < endoflinebp - 7) {
	    unsigned c2 = *(unsigned *) bp;
	    if (c2 & 0xffff)
		*(ushort *) vp = (ushort) c2;
	    c2 >>= 16;
	    if (c2 & 0xffff)
		*(ushort *) (vp + 2) = (ushort) c2;
	    c2 = *(unsigned *) (bp + 4);
	    if (c2 & 0xffff)
		*(ushort *) (vp + 4) = (ushort) c2;
	    c2 >>= 16;
	    if (c2 & 0xffff)
		*(ushort *) (vp + 6) = (ushort) c2;
	    bp += 8;
	    vp += 8;
	}
	while (bp < endoflinebp) {
	    ushort c = *(ushort *) bp;
	    if (c)
		*(ushort *) vp = c;
	    bp += 2;
	    vp += 2;
	}
	vp += BYTEWIDTH - w * 2;
    }
}

INLINE void __svgalib_driver16_putboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    __svgalib_driver8_putbox(x * 2, y, w * 2, h, b + yo * ow * 2 + xo * 2, ow * 2);
    /* inlined */
}

INLINE void __svgalib_driver16p_putboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    __svgalib_driver8p_putbox(x * 2, y, w * 2, h, b + yo * ow * 2 + xo * 2, ow * 2);
}

INLINE void __svgalib_driver16_getboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    __svgalib_driver16_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

INLINE void __svgalib_driver16p_getboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    __svgalib_driver16p_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

INLINE void __svgalib_driver16_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    __svgalib_driver8_copybox(x1 * 2, y1, w * 2, h, x2 * 2, y2);
}

void __svgalib_driver16a_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    int svp, dvp;
    ASSIGNVPOFFSET16(x1, y1, svp);
    ASSIGNVPOFFSET16(x2, y2, dvp);
    vga_bitblt(svp, dvp, w * 2, h, BYTEWIDTH);
}



/* Three bytes per pixel graphics primitives */

#define ASSIGNVP24(x, y, vp) vp = VBUF + (y) * BYTEWIDTH + (x) * 3;
#define ASSIGNVPOFFSET24(x, y, vp) vp = (y) * BYTEWIDTH + (x) * 3;
#define RGBEQUAL(c) ((c & 0xff) == ((c >> 8) & 0xff) && \
	(c & 0xff) == ((c >> 16) & 0xff))

void __svgalib_driver24_setpixel(int x, int y, int c)
{
    char *vp;
    c = RGB2BGR(c);
    ASSIGNVP24(x, y, vp);
    *(unsigned short *) vp = c;
    *(unsigned char *) (vp + 2) = c >> 16;
}

void __svgalib_driver24p_setpixel(int x, int y, int c)
{
    int vp, vpo;
    char *vbuf;
    int page;
    c = RGB2BGR(c);
    ASSIGNVPOFFSET24(x, y, vp);
    vbuf = VBUF;
    page = vp >> 16;
    vga_setpage(page);
    vpo = vp & 0xffff;
    if (vpo <= 0xfffd) {
	*(unsigned short *) (vbuf + vpo) = c;
	*(unsigned char *) (vbuf + vpo + 2) = c >> 16;
    } else if (vpo == 0xfffe) {
	*(unsigned short *) (vbuf + 0xfffe) = c;
	vga_setpage(page + 1);
	*(unsigned char *) vbuf = c >> 16;
    } else {			/* vpo == 0xffff */
	*(unsigned char *) (vbuf + 0xffff) = c;
	vga_setpage(page + 1);
	*(unsigned short *) vbuf = c >> 8;
    }
}

int __svgalib_driver24_getpixel(int x, int y)
{
    char *vp;
    ASSIGNVP24(x, y, vp);
    return RGB2BGR(*(unsigned short *) vp + (*(unsigned char *) (vp + 2) << 16));
}

int __svgalib_driver24p_getpixel(int x, int y)
{
    int vp, vpo;
    char *vbuf;
    int page;
    ASSIGNVPOFFSET24(x, y, vp);
    vbuf = VBUF;
    page = vp >> 16;
    vga_setpage(page);
    vpo = vp & 0xffff;
    if (vpo <= 0xfffd)
	return RGB2BGR(*(unsigned short *) (vbuf + vpo) +
		       (*(unsigned char *) (vbuf + vpo + 2) << 16));
    else if (vpo == 0xfffe) {
	int c;
	c = *(unsigned short *) (vbuf + 0xfffe);
	vga_setpage(page + 1);
	return RGB2BGR((*(unsigned char *) vbuf << 16) + c);
    } else {			/* vpo == 0xffff */
	int c;
	c = *(unsigned char *) (vbuf + 0xffff);
	vga_setpage(page + 1);
	return RGB2BGR((*(unsigned short *) vbuf << 8) + c);
    }
}

void __svgalib_driver24_hline(int x1, int y, int x2, int c)
{
    char *vp;
    c = RGB2BGR(c);
    ASSIGNVP24(x1, y, vp);
    if (RGBEQUAL(c))
	__memset(vp, c, (x2 - x1 + 1) * 3);
    else
	__memset3(vp, c, x2 - x1 + 1);
}

void __svgalib_driver24p_hline(int x1, int y, int x2, int c)
{
    int vp;
    char *rvp;
    int l;
    int chunksize, page;
    c = RGB2BGR(c);
    ASSIGNVPOFFSET24(x1, y, vp);
    SETWRITEPAGED(vp, rvp, chunksize, page);
    l = (x2 - x1 + 1) * 3;
    if (l <= chunksize)
	__memset3(rvp, c, l / 3);
    else {
	int n, m, o;
	n = chunksize / 3;
	m = chunksize % 3;
	__memset3(rvp, c, n);
	/* Handle page break within pixel. */
	if (m >= 1)
	    *(rvp + n * 3) = c;
	if (m == 2)
	    *(rvp + n * 3 + 1) = c >> 8;
	vga_setpage(page + 1);
	o = 0;
	if (m == 2) {
	    *(VBUF) = c >> 16;
	    o = 1;
	}
	if (m == 1) {
	    *(unsigned short *) (VBUF) = c >> 8;
	    o = 2;
	}
	__memset3(VBUF + o, c, (l - chunksize) / 3);
    }
}

void __svgalib_driver24_fillbox(int x, int y, int w, int h, int c)
{
    char *vp;
    int i, j;
    c = RGB2BGR(c);
    ASSIGNVP24(x, y, vp);
    if (RGBEQUAL(c))
	for (i = 0; i < h; i++) {
	    __memset(vp, c, w * 3);
	    vp += BYTEWIDTH;
    } else
	for (j = 0; j < h; j++) {
	    __memset3(vp, c, w);
	    vp += BYTEWIDTH;
	}
}

void __svgalib_driver24p_fillbox(int x, int y, int w, int h, int c)
{
    int vp;
    int page;
    int i;
    c = RGB2BGR(c);
    ASSIGNVPOFFSET24(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    if (RGBEQUAL(c)) {
	for (i = 0; i < h; i++) {
	    if (vp + w * 3 > 0x10000)
		if (vp >= 0x10000) {
		    page++;
		    vga_setpage(page);
		    vp &= 0xffff;
		} else {	/* Page break within line. */
		    __memset(VBUF + vp, c, 0x10000 - vp);
		    page++;
		    vga_setpage(page);
		    __memset(VBUF, c, (vp + w * 3) & 0xffff);
		    vp = (vp + BYTEWIDTH) & 0xffff;
		    continue;
		}
	    __memset(VBUF + vp, c, w * 3);
	    vp += BYTEWIDTH;
	}
    } else
	for (i = 0; i < h; i++) {
	    if (vp + w * 3 > 0x10000)
		if (vp >= 0x10000) {
		    page++;
		    vga_setpage(page);
		    vp &= 0xffff;
		} else {	/* Page break within line. */
		    int n, m, o;
		    n = (0x10000 - vp) / 3;
		    m = (0x10000 - vp) % 3;
		    __memset3(VBUF + vp, c, n);
		    /* Handle page break within pixel. */
		    if (m >= 1)
			*(VBUF + vp + n * 3) = c;
		    if (m == 2)
			*(VBUF + vp + n * 3 + 1) = c >> 8;
		    page++;
		    vga_setpage(page);
		    o = 0;
		    if (m == 2) {
			*(VBUF) = c >> 16;
			o = 1;
		    }
		    if (m == 1) {
			*(unsigned short *) (VBUF) = c >> 8;
			o = 2;
		    }
		    __memset3(VBUF + o, c, ((vp + w * 3) & 0xffff) / 3);
		    vp = (vp + BYTEWIDTH) & 0xffff;
		    continue;
		}
	    __memset3(VBUF + vp, c, w);
	    vp += BYTEWIDTH;
	}
}

void __svgalib_driver24_putbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP24(x, y, vp);
    bp = b;
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	for (i = 0; i < h; i++) {
	    RGB_swapped_memcpy(vp, bp, w * 3);
	    bp += bw * 3;
	    vp += BYTEWIDTH;
	}
    } else {
	for (i = 0; i < h; i++) {
	    __memcpy(vp, bp, w * 3);
	    bp += bw * 3;
	    vp += BYTEWIDTH;
	}
    }
}

static void driver24_rev_putbox(int x, int y, int w, int h, void *b, int bw)
{
/* extra argument width of source bitmap, so that putboxpart can use this */
    int vp;
    int page;
    char *bp = b, *bp2;
    int i, left;

    ASSIGNVPOFFSET8(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		left = RGB_swapped_memcpy(VBUF + vp, bp, 0x10000 - vp);
		bp2 = bp + (0x10000 - vp - left);
		switch (left) {
		case 2:
		    *(VBUF + 0xFFFE) = bp2[2];
		    *(VBUF + 0xFFFF) = bp2[1];
		    break;
		case 1:
		    *(VBUF + 0xFFFF) = bp2[2];
		    break;
		}

		page++;
		vga_setpage(page);

		switch (left) {
		case 1:
		    *(VBUF) = bp2[1];
		    *(VBUF + 1) = bp2[0];
		    left = 3 - left;
		    bp2 += 3;
		    break;
		case 2:
		    *(VBUF) = bp2[0];
		    left = 3 - left;
		    bp2 += 3;
		    break;
		}

		RGB_swapped_memcpy(VBUF + left, bp2, ((vp + w) & 0xffff) - left);
		vp = (vp + BYTEWIDTH) & 0xffff;
		bp += bw;
		continue;
	    }
	RGB_swapped_memcpy(VBUF + vp, bp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

INLINE void __svgalib_driver24p_putbox(int x, int y, int w, int h, void *b, int bw)
{
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	driver24_rev_putbox(x * 3, y, w * 3, h, b, bw * 3);
    } else {
	__svgalib_driver8p_putbox(x * 3, y, w * 3, h, b, bw * 3);
    }
}

void __svgalib_driver24_putbox32(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP24(x, y, vp);
    bp = b;
    for (i = 0; i < h; i++) {
	__svgalib_memcpy4to3(vp, bp, w);
	bp += bw * 4;
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver24_getbox(int x, int y, int w, int h, void *b, int bw)
{
    char *vp;			/* screen pointer */
    char *bp;			/* bitmap pointer */
    int i;
    ASSIGNVP24(x, y, vp);
    bp = b;
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	for (i = 0; i < h; i++) {
	    RGB_swapped_memcpy(bp, vp, w * 3);
	    bp += bw * 3;
	    vp += BYTEWIDTH;
	}
    } else {
	for (i = 0; i < h; i++) {
	    __memcpy(bp, vp, w * 3);
	    bp += bw * 3;
	    vp += BYTEWIDTH;
	}
    }
}

static void driver24_rev_getbox(int x, int y, int w, int h, void *b, int bw)
{
/* extra argument width of source bitmap, so that putboxpart can use this */
    int vp;
    int page;
    char *bp = b, *bp2;
    int i, left;

    ASSIGNVPOFFSET8(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		left = RGB_swapped_memcpy(bp, VBUF + vp, 0x10000 - vp);
		bp2 = bp + (0x10000 - vp - left);
		switch (left) {
		case 2:
		    bp2[2] = *(VBUF + 0xFFFE);
		    bp2[1] = *(VBUF + 0xFFFF);
		    break;
		case 1:
		    bp2[2] = *(VBUF + 0xFFFF);
		    break;
		}

		page++;
		vga_setpage(page);

		switch (left) {
		case 1:
		    bp2[1] = *(VBUF);
		    bp2[0] = *(VBUF + 1);
		    left = 3 - left;
		    bp2 += 3;
		    break;
		case 2:
		    bp2[0] = *(VBUF);
		    left = 3 - left;
		    bp2 += 3;
		    break;
		}

		RGB_swapped_memcpy(bp2, VBUF + left, ((vp + w) & 0xffff) - left);
		vp = (vp + BYTEWIDTH) & 0xffff;
		bp += bw;
		continue;
	    }
	RGB_swapped_memcpy(bp, VBUF + vp, w);
	bp += bw;
	vp += BYTEWIDTH;
    }
}

INLINE void __svgalib_driver24p_getbox(int x, int y, int w, int h, void *b, int bw)
{
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	driver24_rev_getbox(x * 3, y, w * 3, h, b, bw * 3);
    } else {
	__svgalib_driver8p_getbox(x * 3, y, w * 3, h, b, bw * 3);
    }
}

void __svgalib_driver24_putboxmask(int x, int y, int w, int h, void *b)
{
    uchar *bp = b;
    uchar *vp;
    int i;
    ASSIGNVP24(x, y, vp);
    for (i = 0; i < h; i++) {
	uchar *endoflinebp = bp + w * 3;
	while (bp < endoflinebp - 11) {
	    unsigned c = RGB2BGR(*(unsigned *) bp);
	    if (c & 0xffffff) {
		*(ushort *) vp = (ushort) c;
		*(vp + 2) = c >> 16;
	    }
	    c = RGB2BGR(*(unsigned *) (bp + 3));
	    if (c & 0xffffff) {
		*(ushort *) (vp + 3) = (ushort) c;
		*(vp + 5) = c >> 16;
	    }
	    c = RGB2BGR(*(unsigned *) (bp + 6));
	    if (c & 0xffffff) {
		*(ushort *) (vp + 6) = (ushort) c;
		*(vp + 8) = c >> 16;
	    }
	    c = RGB2BGR(*(unsigned *) (bp + 9));
	    if (c & 0xffffff) {
		*(ushort *) (vp + 9) = (ushort) c;
		*(vp + 11) = c >> 16;
	    }
	    bp += 12;
	    vp += 12;
	}
	while (bp < endoflinebp) {
	    uint c = RGB2BGR(*(uint *) bp);
	    if (c & 0xffffff) {
		*(ushort *) vp = (ushort) c;
		*(vp + 2) = c >> 16;
	    }
	    bp += 3;
	    vp += 3;
	}
	vp += BYTEWIDTH - w * 3;
    }
}

INLINE void __svgalib_driver24_putboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    /*
     * Actually I think all this could be done by just calling __svgalib_driver24_putbox
     * with correct args. But I'm too fearful. - Michael.
     */
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	__svgalib_driver24_putbox(x, y, w, h, b + yo * ow + xo, ow);
    } else {
	__svgalib_driver8_putbox(x * 3, y, w * 3, h, b + yo * ow * 3 + xo * 3, ow * 3);
    }
}

INLINE void __svgalib_driver24p_putboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    if (MODEFLAGS & MODEFLAG_24BPP_REVERSED) {
	driver24_rev_putbox(x * 3, y, w * 3, h, b + yo * ow * 3 + xo * 3, ow * 3);
    } else {
	__svgalib_driver8p_putbox(x * 3, y, w * 3, h, b + yo * ow * 3 + xo * 3, ow * 3);
    }
}

INLINE void __svgalib_driver24_getboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    __svgalib_driver24_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

INLINE void __svgalib_driver24p_getboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    __svgalib_driver24p_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

void __svgalib_driver24_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    __svgalib_driver8_copybox(x1 * 3, y1, w * 3, h, x2 * 3, y2);
}

void __svgalib_driver24a_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    int svp, dvp;
    ASSIGNVPOFFSET24(x1, y1, svp);
    ASSIGNVPOFFSET24(x2, y2, dvp);
    vga_bitblt(svp, dvp, w * 3, h, BYTEWIDTH);
}



/* Four bytes per pixel graphics primitives */

#define ASSIGNVP32(x, y, vp) vp = VBUF + (y) * BYTEWIDTH + (x) * 4;
#define ASSIGNVPOFFSET32(x, y, vp) vp = (y) * BYTEWIDTH + (x) * 4;

void __svgalib_driver32_setpixel(int x, int y, int c)
{
    char *vp;
    ASSIGNVP32(x, y, vp);
    *(unsigned *) vp = c;
}

void __svgalib_driver32p_setpixel(int x, int y, int c)
{
    int vp;
    ASSIGNVPOFFSET32(x, y, vp);
    vga_setpage(vp >> 16);
    *(unsigned *) (VBUF + (vp & 0xffff)) = c;
}

int __svgalib_driver32_getpixel(int x, int y)
{
    char *vp;
    ASSIGNVP32(x, y, vp);
    return *(unsigned *) vp;
}

int __svgalib_driver32p_getpixel(int x, int y)
{
    int vp;
    ASSIGNVPOFFSET32(x, y, vp);
    vga_setpage(vp >> 16);
    return *(unsigned *) (VBUF + (vp & 0xffff));
}

void __svgalib_driver32_hline(int x1, int y, int x2, int c)
{
    char *vp;
    ASSIGNVP32(x1, y, vp);
    __memsetlong(vp, c, x2 - x1 + 1);
}

void __svgalib_driver32p_hline(int x1, int y, int x2, int c)
{
    int vp;
    char *rvp;
    int l;
    int chunksize, page;
    ASSIGNVPOFFSET32(x1, y, vp);
    SETWRITEPAGED(vp, rvp, chunksize, page);
    l = (x2 - x1 + 1) * 4;
    if (l <= chunksize)
	__memsetlong(rvp, c, l / 4);
    else {
	__memsetlong(rvp, c, chunksize / 4);
	vga_setpage(page + 1);
	__memsetlong(VBUF, c, (l - chunksize) / 4);
    }
}

void __svgalib_driver32_fillbox(int x, int y, int w, int h, int c)
{
    char *vp;
    int i;
    ASSIGNVP32(x, y, vp);
    for (i = 0; i < h; i++) {
	__memsetlong(vp, c, w);
	vp += BYTEWIDTH;
    }
}

void __svgalib_driver32p_fillbox(int x, int y, int w, int h, int c)
{
    int vp;
    int page;
    int i;
    ASSIGNVPOFFSET32(x, y, vp);
    page = vp >> 16;
    vp &= 0xffff;
    vga_setpage(page);
    for (i = 0; i < h; i++) {
	if (vp + w * 4 > 0x10000)
	    if (vp >= 0x10000) {
		page++;
		vga_setpage(page);
		vp &= 0xffff;
	    } else {		/* page break within line */
		__memsetlong(VBUF + vp, c, (0x10000 - vp) / 4);
		page++;
		vga_setpage(page);
		__memsetlong(VBUF, c, ((vp + w * 4) & 0xffff) / 4);
		vp = (vp + BYTEWIDTH) & 0xffff;
		continue;
	    }
	__memsetlong(VBUF + vp, c, w);
	vp += BYTEWIDTH;
    }
}

INLINE void __svgalib_driver32_putbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8_putbox(x * 4, y, w * 4, h, b, bw * 4);
}

INLINE void __svgalib_driver32p_putbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8p_putbox(x * 4, y, w * 4, h, b, bw * 4);
}

INLINE void __svgalib_driver32_getbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8_getbox(x * 4, y, w * 4, h, b, bw * 4);
}

INLINE void __svgalib_driver32p_getbox(int x, int y, int w, int h, void *b, int bw)
{
    __svgalib_driver8p_getbox(x * 4, y, w * 4, h, b, bw * 4);
}

void __svgalib_driver32_putboxmask(int x, int y, int w, int h, void *b)
{
    uchar *bp = b;
    uchar *vp;
    int i;
    ASSIGNVP32(x, y, vp);
    for (i = 0; i < h; i++) {
	uchar *endoflinebp = bp + w * 4;
	while (bp < endoflinebp - 15) {
	    unsigned c = *(unsigned *) bp;
	    if (c)
		*(unsigned *) vp = c;
	    c = *(unsigned *) (bp + 4);
	    if (c)
		*(unsigned *) (vp + 4) = c;
	    c = *(unsigned *) (bp + 8);
	    if (c)
		*(unsigned *) (vp + 8) = c;
	    c = *(unsigned *) (bp + 12);
	    if (c)
		*(unsigned *) (vp + 12) = c;
	    bp += 16;
	    vp += 16;
	}
	while (bp < endoflinebp) {
	    unsigned c = *(unsigned *) bp;
	    if (c)
		*(unsigned *) vp = c;
	    bp += 4;
	    vp += 4;
	}
	vp += BYTEWIDTH - w * 4;
    }
}

INLINE void __svgalib_driver32_putboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    __svgalib_driver32_putbox(x, y, w, h, b + yo * ow + xo, ow);
    /* inlined */
}

INLINE void __svgalib_driver32p_putboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    __svgalib_driver32p_putbox(x, y, w, h, b + yo * ow + xo, ow);
    /* inlined */
}

INLINE void __svgalib_driver32_getboxpart(int x, int y, int w, int h, int ow, int oh,
				void *b, int xo, int yo)
{
    __svgalib_driver32_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

INLINE void __svgalib_driver32p_getboxpart(int x, int y, int w, int h, int ow, int oh,
				 void *b, int xo, int yo)
{
    __svgalib_driver32p_getbox(x, y, w, h, b + yo * ow + xo, ow);
}

INLINE void __svgalib_driver32_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    __svgalib_driver8_copybox(x1 * 4, y1, w * 4, h, x2 * 4, y2);
}



/* Planar 256 color mode graphics primitives (only putbox) */

void __svgalib_driverplanar256_nothing(void)
{
    NOTIMPL("planar 256 color mode primitive");
}

void __svgalib_driverplanar256_putbox(int x, int y, int w, int h, void *b, int bw)
{
    if ((w & 3) != 0 || (x & 3) != 0)
	NOTIMPL("planar 256 color mode unaligned putbox");
    vga_copytoplanar256(b, bw, y * BYTEWIDTH + x / 4, BYTEWIDTH,
			w, h);
}

void __svgalib_driverplanar16_nothing(void)
{
    NOTIMPL("planar 16 color mode primitive");
}


/* Memory primitives */

int __svgalib_driver_setread(GraphicsContext * gc, int i, void **vp)
{
    if (gc->modetype == CONTEXT_PAGED) {
	vga_setpage(i >> 16);
	*vp = (i & 0xffff) + gc->vbuf;
	return 0x10000 - (i & 0xffff);
    } else {
	*vp = gc->vbuf + i;
	return 0x10000;
    }
}

int __svgalib_driver_setwrite(GraphicsContext * gc, int i, void **vp)
{
    if (gc->modetype == CONTEXT_PAGED) {
	vga_setpage(i >> 16);
	*vp = (i & 0xffff) + gc->vbuf;
	return 0x10000 - (i & 0xffff);
    } else {
	*vp = gc->vbuf + i;
	return 0x10000;
    }
}



/* Functions that are not yet implemented */

void __svgalib_driver8p_putboxmask(int x, int y, int w, int h, void *b)
{
    NOTIMPL("8-bit paged putboxmask");
}

void __svgalib_driver8p_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    NOTIMPL("8-bit paged copybox (bitblt)");
}

void __svgalib_driver16p_putboxmask(int x, int y, int w, int h, void *b)
{
    NOTIMPL("16-bit paged putboxmask");
}

void __svgalib_driver16p_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    NOTIMPL("16-bit paged copybox");
}

void __svgalib_driver24p_putboxmask(int x, int y, int w, int h, void *b)
{
    NOTIMPL("24-bit paged putboxmask");
}

void __svgalib_driver24p_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    NOTIMPL("24-bit paged copybox");
}

void __svgalib_driver32p_putboxmask(int x, int y, int w, int h, void *b)
{
    NOTIMPL("32-bit paged putboxmask");
}

void __svgalib_driver32p_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
    NOTIMPL("32-bit paged copybox");
}
