/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* grlib.c      Main module */


#include <stdlib.h>
#include <vga.h>
#include "inlstring.h"		/* include inline string operations */

#include "vgagl.h"
#include "def.h"
#include "driver.h"



/* Global variables */

#ifdef DLL_CONTEXT_SHADOW

/* The current context variable is shadowed in a read-only variable for */
/* external use. */

GraphicsContext __currentcontext;	/* Internal current context. */
GraphicsContext currentcontext;	/* Copy for external use. */

#else

GraphicsContext currentcontext;

#endif

void (*__svgalib_nonaccel_fillbox)(int, int, int, int, int);
static int screenoffset = 0;	/* Used by copy(box)toscreen. */


/* Framebuffer function pointers */

static framebufferfunctions ff8 =
{
    __svgalib_driver8_setpixel,
    __svgalib_driver8_getpixel,
    __svgalib_driver8_hline,
    __svgalib_driver8_fillbox,
    __svgalib_driver8_putbox,
    __svgalib_driver8_getbox,
    __svgalib_driver8_putboxmask,
    __svgalib_driver8_putboxpart,
    __svgalib_driver8_getboxpart,
    __svgalib_driver8_copybox
};

static framebufferfunctions ff16 =
{
    __svgalib_driver16_setpixel,
    __svgalib_driver16_getpixel,
    __svgalib_driver16_hline,
    __svgalib_driver16_fillbox,
    __svgalib_driver16_putbox,
    __svgalib_driver16_getbox,
    __svgalib_driver16_putboxmask,
    __svgalib_driver16_putboxpart,
    __svgalib_driver16_getboxpart,
    __svgalib_driver16_copybox
};

static framebufferfunctions ff24 =
{
    __svgalib_driver24_setpixel,
    __svgalib_driver24_getpixel,
    __svgalib_driver24_hline,
    __svgalib_driver24_fillbox,
    __svgalib_driver24_putbox,
    __svgalib_driver24_getbox,
    __svgalib_driver24_putboxmask,
    __svgalib_driver24_putboxpart,
    __svgalib_driver24_getboxpart,
    __svgalib_driver24_copybox
};

static framebufferfunctions ff32 =
{
    __svgalib_driver32_setpixel,
    __svgalib_driver32_getpixel,
    __svgalib_driver32_hline,
    __svgalib_driver32_fillbox,
    __svgalib_driver32_putbox,
    __svgalib_driver32_getbox,
    __svgalib_driver32_putboxmask,
    __svgalib_driver32_putboxpart,
    __svgalib_driver32_getboxpart,
    __svgalib_driver32_copybox
};

static framebufferfunctions ff8paged =
{
    __svgalib_driver8p_setpixel,
    __svgalib_driver8p_getpixel,
    __svgalib_driver8p_hline,
    __svgalib_driver8p_fillbox,
    __svgalib_driver8p_putbox,
    __svgalib_driver8p_getbox,
    __svgalib_driver8p_putboxmask,
    __svgalib_driver8p_putboxpart,
    __svgalib_driver8p_getboxpart,
    __svgalib_driver8p_copybox
};

static framebufferfunctions ff16paged =
{
    __svgalib_driver16p_setpixel,
    __svgalib_driver16p_getpixel,
    __svgalib_driver16p_hline,
    __svgalib_driver16p_fillbox,
    __svgalib_driver16p_putbox,
    __svgalib_driver16p_getbox,
    __svgalib_driver16p_putboxmask,
    __svgalib_driver16p_putboxpart,
    __svgalib_driver16p_getboxpart,
    __svgalib_driver16p_copybox
};

static framebufferfunctions ff24paged =
{
    __svgalib_driver24p_setpixel,
    __svgalib_driver24p_getpixel,
    __svgalib_driver24p_hline,
    __svgalib_driver24p_fillbox,
    __svgalib_driver24p_putbox,
    __svgalib_driver24p_getbox,
    __svgalib_driver24p_putboxmask,
    __svgalib_driver24p_putboxpart,
    __svgalib_driver24p_getboxpart,
    __svgalib_driver24p_copybox
};

static framebufferfunctions ff32paged =
{
    __svgalib_driver32p_setpixel,
    __svgalib_driver32p_getpixel,
    __svgalib_driver32p_hline,
    __svgalib_driver32p_fillbox,
    __svgalib_driver32p_putbox,
    __svgalib_driver32p_getbox,
    __svgalib_driver32p_putboxmask,
    __svgalib_driver32p_putboxpart,
    __svgalib_driver32p_getboxpart,
    __svgalib_driver32p_copybox
};

static framebufferfunctions ffplanar256 =
{
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    __svgalib_driverplanar256_putbox,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
    (void *) __svgalib_driverplanar256_nothing,
};

#if 0				/* Not yet used */
static framebufferfunctions ffplanar16 =
{
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
    (void *) __svgalib_driverplanar16_nothing,
};
#endif


/* Initialization and graphics contexts */

#define SCREENSIZE(gc) ((gc).bytewidth * (gc).height)

static int colorbits(int c)
{
    switch (c) {
    default:
    case 256:
	return 8;
    case 32768:
	return 15;
    case 65536:
	return 16;
    case 256 * 65536:
	return 24;
    }
}

int gl_setcontextvga(int m)
{
    framebufferfunctions *ff;
    vga_modeinfo *modeinfo;
    int accelfuncs;
    if (!vga_hasmode(m))
	return -1;
    modeinfo = vga_getmodeinfo(m);
    /* Set graphics context */
    WIDTH = modeinfo->width;
    HEIGHT = modeinfo->height;
    BYTESPERPIXEL = modeinfo->bytesperpixel;
    COLORS = modeinfo->colors;
    BITSPERPIXEL = colorbits(COLORS);
    BYTEWIDTH = modeinfo->linewidth;
    VBUF = vga_getgraphmem();
    MODEFLAGS = 0;
    __clip = 0;
    ff = &(__currentcontext.ff);
    if (modeinfo->flags & IS_MODEX) {
	/* Pretend it's a regular (linear) context. */
	BYTESPERPIXEL = 1;
	BYTEWIDTH *= 4;
	MODETYPE = CONTEXT_MODEX;
	if (BYTEWIDTH * HEIGHT * 2 <= 256 * 1024)
	    MODEFLAGS |= MODEFLAG_PAGEFLIPPING_CAPABLE;
	if (BYTEWIDTH * HEIGHT * 3 <= 256 * 1024)
	    MODEFLAGS |= MODEFLAG_TRIPLEBUFFERING_CAPABLE;
	__currentcontext.ff = ffplanar256;
    } else if (modeinfo->colors == 16) {
	/* Pretend it's a regular one byte per pixel context. */
	BYTESPERPIXEL = 1;
	BYTEWIDTH *= 8;
	MODETYPE = CONTEXT_PLANAR16;
	if (BYTEWIDTH * HEIGHT <= 256 * 1024)
	    MODEFLAGS |= MODEFLAG_PAGEFLIPPING_CAPABLE;
	if (BYTEWIDTH * HEIGHT * 3 / 2 <= 256 * 1024)
	    MODEFLAGS |= MODEFLAG_TRIPLEBUFFERING_CAPABLE;
    } else if ((modeinfo->flags & CAPABLE_LINEAR)	/* Creepy. Try linear addressing only if the mode is set. */
	       && vga_getcurrentmode() == m && (vga_setlinearaddressing() != -1)) {
	/* No banking. */
	/* Get get the fb address in case we set linear addressing. */
	VBUF = vga_getgraphmem();
	MODETYPE = CONTEXT_LINEAR;
	if (modeinfo->maxpixels >= WIDTH * HEIGHT * 2)
	    MODEFLAGS |= MODEFLAG_PAGEFLIPPING_CAPABLE;
	if (modeinfo->maxpixels >= WIDTH * HEIGHT * 3)
	    MODEFLAGS |= MODEFLAG_TRIPLEBUFFERING_CAPABLE;
	switch (BYTESPERPIXEL) {
	case 1:
	    __currentcontext.ff = ff8;
	    break;
	case 2:
	    __currentcontext.ff = ff16;
	    break;
	case 3:
	    __currentcontext.ff = ff24;
	    break;
	case 4:
	    __currentcontext.ff = ff32;
	    break;
	}
	if (modeinfo->flags & RGB_MISORDERED)
	    MODEFLAGS |= MODEFLAG_32BPP_SHIFT8;
    } else {
	/* Banked mode. */
	MODETYPE = CONTEXT_PAGED;
	if (modeinfo->maxpixels >= WIDTH * HEIGHT * 2)
	    MODEFLAGS |= MODEFLAG_PAGEFLIPPING_CAPABLE;
	if (modeinfo->maxpixels >= WIDTH * HEIGHT * 3)
	    MODEFLAGS |= MODEFLAG_TRIPLEBUFFERING_CAPABLE;
	if ((modeinfo->startaddressrange & 0x1ffff) == 0x10000) {
	    /* This hack is required for 320x200x256 page flipping */
	    /* on Trident, which doesn't work with bank boundary */
	    /* within the second page. */
	    MODEFLAGS |= MODEFLAG_FLIPPAGE_BANKALIGNED;
	}
	switch (BYTESPERPIXEL) {
	case 1:
	    __currentcontext.ff = ff8paged;
	    break;
	case 2:
	    __currentcontext.ff = ff16paged;
	    break;
	case 3:
	    __currentcontext.ff = ff24paged;
	    break;
	case 4:
	    __currentcontext.ff = ff32paged;
	    break;
	}
	if (modeinfo->flags & RGB_MISORDERED)
	    MODEFLAGS |= MODEFLAG_32BPP_SHIFT8;
    }
    if (vga_getcurrentmode() == m) {
	accelfuncs = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_ACCEL);
	__svgalib_nonaccel_fillbox = __currentcontext.ff.driver_fillbox_func;
	if (accelfuncs & ACCELFLAG_FILLBOX)
	    __currentcontext.ff.driver_fillbox_func =
		__svgalib_driver8a_fillbox;
	if (accelfuncs & ACCELFLAG_SCREENCOPY)
	    __currentcontext.ff.driver_copybox_func =
		__svgalib_driver8a_copybox;
    }
#ifdef DLL_CONTEXT_SHADOW
    currentcontext = __currentcontext;
#endif
    return 0;
}

int gl_setcontextvgavirtual(int m)
{
    vga_modeinfo *modeinfo;
    if (!vga_hasmode(m))
	return -1;
    modeinfo = vga_getmodeinfo(m);
    /* Set graphics context */
    WIDTH = modeinfo->width;
    HEIGHT = modeinfo->height;
    if (modeinfo->flags & IS_MODEX) {
	/* Use a regular virtual screen for planar 256 color modes. */
	BYTESPERPIXEL = 1;
	BYTEWIDTH = modeinfo->linewidth * 4;
    } else if (modeinfo->colors == 16) {
	/* Use a regular one byte per pixel virtual screen for */
	/* planar 16 color modes. */
	BYTESPERPIXEL = 1;
	BYTEWIDTH = modeinfo->linewidth * 8;
    } else {
	BYTESPERPIXEL = modeinfo->bytesperpixel;
	BYTEWIDTH = modeinfo->linewidth;
    }
    COLORS = modeinfo->colors;
    BITSPERPIXEL = colorbits(COLORS);
    VBUF = malloc(SCREENSIZE(__currentcontext));
    MODETYPE = CONTEXT_VIRTUAL;
    MODEFLAGS = 0;
    __clip = 0;
    switch (BYTESPERPIXEL) {
    case 1:
	__currentcontext.ff = ff8;
	break;
    case 2:
	__currentcontext.ff = ff16;
	break;
    case 3:
	__currentcontext.ff = ff24;
	break;
    case 4:
	__currentcontext.ff = ff32;
	break;
    }
#ifdef DLL_CONTEXT_SHADOW
    currentcontext = __currentcontext;
#endif
    return 0;
}

void gl_setcontextvirtual(int w, int h, int bpp, int bitspp, void *v)
{
    WIDTH = w;
    HEIGHT = h;
    BYTESPERPIXEL = bpp;
    BITSPERPIXEL = bitspp;
    COLORS = 1 << bitspp;
    BYTEWIDTH = WIDTH * BYTESPERPIXEL;
    VBUF = v;
    MODETYPE = CONTEXT_VIRTUAL;
    MODEFLAGS = 0;
    switch (BYTESPERPIXEL) {
    case 1:
	__currentcontext.ff = ff8;
	break;
    case 2:
	__currentcontext.ff = ff16;
	break;
    case 3:
	__currentcontext.ff = ff24;
	break;
    case 4:
	__currentcontext.ff = ff32;
	break;
    }
    __clip = 0;
#ifdef DLL_CONTEXT_SHADOW
    currentcontext = __currentcontext;
#endif
}

GraphicsContext *
 gl_allocatecontext()
{
    return malloc(sizeof(GraphicsContext));
}

void gl_setcontext(GraphicsContext * gc)
{
    __currentcontext = *gc;
#ifdef DLL_CONTEXT_SHADOW
    currentcontext = *gc;
#endif
}

void gl_getcontext(GraphicsContext * gc)
{
    *gc = __currentcontext;
}

void gl_freecontext(GraphicsContext * gc)
{
    if (gc->modetype == CONTEXT_VIRTUAL)
	free(gc->vbuf);
}

void gl_setcontextwidth(int w)
{
    __currentcontext.width = currentcontext.width = w;
    __currentcontext.bytewidth = currentcontext.bytewidth =
	w * BYTESPERPIXEL;
}

void gl_setcontextheight(int h)
{
    __currentcontext.height = currentcontext.height = h;
}


/* Clipping */

void gl_setclippingwindow(int x1, int y1, int x2, int y2)
{
    __clip = 1;
    __clipx1 = x1;
    __clipy1 = y1;
    __clipx2 = x2;
    __clipy2 = y2;
}

void gl_enableclipping()
{
    __clip = 1;
    __clipx1 = 0;
    __clipy1 = 0;
    __clipx2 = WIDTH - 1;
    __clipy2 = HEIGHT - 1;
}

void gl_disableclipping()
{
    __clip = 0;
}


/* Primitive functions */

void gl_setpixel(int x, int y, int c)
{
    if (__clip && outside(x, y))
	return;
    setpixel(x, y, c);
}

int gl_getpixel(int x, int y)
{
    if (__clip && outside(x, y))
	return -1;
    return getpixel(x, y);
}

void gl_hline(int x1, int y, int x2, int c)
{
    if (__clip) {
	if (y_outside(y))
	    return;
	clipxleft(x1);
	clipxright(x2);
	if (x1 > x2)
	    return;
    }
    hline(x1, y, x2, c);
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

void gl_fillbox(int x, int y, int w, int h, int c)
{
    if (__clip) {
	if (x + w < __clipx1 || x > __clipx2)
	    return;
	if (y + h < __clipy1 || y > __clipy2)
	    return;
	if (x < __clipx1) {
	    w -= __clipx1 - x;
	    x = __clipx1;
	}
	if (y < __clipy1) {
	    h -= __clipy1 - y;
	    y = __clipy1;
	}
	if (x + w > __clipx2 + 1)
	    w = __clipx2 - x + 1;
	if (y + h > __clipy2 + 1)
	    h = __clipy2 - y + 1;
    }
    if (w <= 0 || h <= 0)
	return;
    fillbox(x, y, w, h, c);
}

void gl_putboxpart(int x, int y, int w, int h, int ow, int oh, void *b,
		   int ox, int oy)
{
    putboxpart(x, y, w, h, ow, oh, b, ox, oy);
}

void gl_putbox(int x, int y, int w, int h, void *b)
{
    uchar *bp = b;
    if (w <= 0 || h <= 0)
	return;
    if (__clip) {
	int nx, ny, nw, nh;
	ADJUSTBITMAPBOX();
	if (nw <= 0 || nh <= 0)
	    return;
	if (nw != w || nh != h) {
	    putboxpart(nx, ny, nw, nh, w, h, bp, nx - x, ny - y);
	    return;
	}
    }
    putbox(x, y, w, h, bp, w);
}

static void emulate_putboxmask(int x, int y, int w, int h, void *b)
{
    void *box;
    GraphicsContext gc;
    box = alloca(w * h * BYTESPERPIXEL);
    gl_getbox(x, y, w, h, box);	/* does clipping */

    gl_getcontext(&gc);		/* save context */

    /* create context that is only the box */
    gl_setcontextvirtual(w, h, BYTESPERPIXEL, BITSPERPIXEL, box);
    gl_putboxmask(0, 0, w, h, b);

    gl_setcontext(&gc);		/* restore context */
    gl_putbox(x, y, w, h, box);
}

void gl_putboxmask(int x, int y, int w, int h, void *b)
{
    if (w <= 0 || h <= 0)
	return;
    if (__clip) {
	if (x + w < __clipx1 || x > __clipx2)
	    return;
	if (y + h < __clipy1 || y > __clipy2)
	    return;
	if (x < __clipx1 || y < __clipy1
	    || x + w > __clipx2 + 1 || y + h > __clipy2 + 1) {
	    /* clipping is not directly implemented */
	    emulate_putboxmask(x, y, w, h, b);
	    return;
	}
    }
    if (MODETYPE == CONTEXT_PAGED)
	/* paged primitive is not implemented */
	emulate_putboxmask(x, y, w, h, b);
    else
	putboxmask(x, y, w, h, b);
}

void gl_getbox(int x, int y, int w, int h, void *b)
{
    if (__clip) {
	int nx, ny, nw, nh;
	ADJUSTBITMAPBOX();
	if (nw <= 0 || nh <= 0)
	    return;
	if (nw != w || nh != h) {
	    getboxpart(nx, ny, nw, nh, w, h, b, nx - x, ny - y);
	    return;
	}
    }
    getbox(x, y, w, h, b, w);
}

void gl_copybox(int x1, int y1, int w, int h, int x2, int y2)
{
/* Doesn't handle clipping. */
    if (MODETYPE == CONTEXT_PAGED) {
	/* Paged primitive is not implemented. */
	void *box;
	box = alloca(w * h * BYTESPERPIXEL);
	getbox(x1, y1, w, h, box, w);
	putbox(x2, y2, w, h, box, w);
	return;
    }
    copybox(x1, y1, w, h, x2, y2);
}


/* Miscellaneous functions */

void gl_clearscreen(int c)
{
    gl_fillbox(0, 0, WIDTH, HEIGHT, c);
}

int gl_rgbcolor(int r, int g, int b)
{
    unsigned v;
    switch (BITSPERPIXEL) {
    case 8:
	/* assumes RGB palette at index 0-255 */
	/* bits 0-2 = blue (3 bits) */
	/*      3-5 = green (3 bits) */
	/*      6-7 = red (2 bits) */
	return (r & 0xc0) + ((g & 0xe0) >> 2) + (b >> 5);
    case 24:
    case 32:
	v = (r << 16) + (g << 8) + b;
	if (MODEFLAGS & MODEFLAG_32BPP_SHIFT8)
	    return v << 8;
	return v;
    case 15:
	return ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + (b >> 3);
    case 16:
	return ((r & 0xf8) << 8) + ((g & 0xfc) << 3) + (b >> 3);
    case 4:
	/* Now this is real fun. Map to standard EGA palette. */
	v = 0;
	if (b >= 64)
	    v += 1;
	if (g >= 64)
	    v += 2;
	if (r >= 64)
	    v += 4;
	if (b >= 192 || g >= 192 || r >= 192)
	    v += 8;
	return v;
    }
    return -1;
}

void gl_setpixelrgb(int x, int y, int r, int g, int b)
{
/* Color components range from 0 to 255 */
    if (__clip && outside(x, y))
	return;
    setpixel(x, y, gl_rgbcolor(r, g, b));
}

void gl_getpixelrgb(int x, int y, int *r, int *g, int *b)
{
    unsigned c;
    if (__clip && outside(x, y)) {
	*r = *g = *b = -1;
	return;
    }
    c = getpixel(x, y);
    switch (BITSPERPIXEL) {
    case 8:
	*b = (c & (1 + 2 + 4)) << 5;	/* bits 0-2 */
	*g = (c & (8 + 16 + 32)) << 2;	/* bits 3-5 */
	*r = (c & (64 + 128));	/* bits 6-7 */
	break;
    case 32:
	if (MODEFLAGS & MODEFLAG_32BPP_SHIFT8) {
	    *b = (c & 0xff00) >> 8;
	    *g = (c & 0xff0000) >> 16;
	    *r = c >> 24;
	    break;
	}
    case 24:
	*b = c & 0xff;
	*g = (c & 0xff00) >> 8;
	*r = c >> 16;
	break;
    case 15:
	*b = (c & (1 + 2 + 4 + 8 + 16)) << 3;
	*g = (c & (32 + 64 + 128 + 256 + 512)) >> 2;
	*r = (c & (1024 + 2048 + 4096 + 8192 + 16384)) >> 7;
	break;
    case 16:
	*b = (c & (1 + 2 + 4 + 8 + 16)) << 3;
	*g = (c & (32 + 64 + 128 + 256 + 512 + 1024)) >> 3;
	*r = (c & (2048 + 4096 + 8192 + 16384 + 32768)) >> 8;
	break;
    case 4:
	*b = (c & 1) * ((c & 8) ? 255 : 128);
	*g = (c & 2) * ((c & 8) ? 255 : 128);
	*r = (c & 4) * ((c & 8) ? 255 : 128);
	break;
    }
}

void gl_setdisplaystart(int x, int y)
{
    vga_setdisplaystart(y * BYTEWIDTH + x * BYTESPERPIXEL);
}


/* Screen copying */

void gl_setscreenoffset(int o)
{
    screenoffset = o;
}

int gl_enablepageflipping(GraphicsContext * gc)
{
    if (gc->modeflags & MODEFLAG_PAGEFLIPPING_CAPABLE) {
	gc->modeflags |= MODEFLAG_PAGEFLIPPING_ENABLED;
    }
    if (gc->modeflags & MODEFLAG_TRIPLEBUFFERING_CAPABLE) {
	gc->modeflags &= ~(MODEFLAG_PAGEFLIPPING_ENABLED);
	gc->modeflags |= MODEFLAG_TRIPLEBUFFERING_ENABLED;
    }
    gc->flippage = 0;
    if (gc->modeflags & MODEFLAG_TRIPLEBUFFERING_ENABLED)
	return 3;
    if (gc->modeflags & MODEFLAG_PAGEFLIPPING_ENABLED)
	return 2;
    return 0;
}

void gl_copyscreen(GraphicsContext * gc)
{
    int size;
    void *svp, *dvp;

    if (gc->modeflags & MODEFLAG_PAGEFLIPPING_ENABLED)
	gc->flippage ^= 1;
    if (gc->modeflags & MODEFLAG_TRIPLEBUFFERING_ENABLED)
	gc->flippage = (gc->flippage + 1) % 3;
    if (gc->modeflags & (MODEFLAG_PAGEFLIPPING_ENABLED |
			 MODEFLAG_TRIPLEBUFFERING_ENABLED)) {
	/* Calculate screen offset in bytes. */
	screenoffset = gc->bytewidth * HEIGHT * gc->flippage;
	if (gc->modeflags & MODEFLAG_FLIPPAGE_BANKALIGNED)
	    screenoffset = ((screenoffset + 0xffff) & ~0xffff);
    }
    if (gc->modetype == CONTEXT_MODEX) {
	vga_copytoplanar256(VBUF, BYTEWIDTH, screenoffset / 4,
			    gc->bytewidth / 4, WIDTH, HEIGHT);
	goto end;
    }
    if (gc->modetype == CONTEXT_PLANAR16) {
	if (WIDTH == 1024 && HEIGHT >= 512 &&
	    ((screenoffset / 8) & 0xffff) == 0) {
	    /* Kludge to allow 1024x768x16 with page flipping. */
	    int page;
	    page = (screenoffset / 8) >> 16;
	    vga_setpage(page);
	    vga_copytoplanar16(VBUF, BYTEWIDTH, 0,
			       gc->bytewidth / 8, WIDTH, 512);
	    vga_setpage(page + 1);
	    vga_copytoplanar16(VBUF + WIDTH * 512, BYTEWIDTH,
			       0, gc->bytewidth / 8, WIDTH, HEIGHT - 512);
	    return;
	}
	if (WIDTH * HEIGHT >= 512 * 1024)
	    /* We don't handle banking. */
	    return;

	vga_copytoplanar16(VBUF, BYTEWIDTH, screenoffset / 8,
			   gc->bytewidth / 8, WIDTH, HEIGHT);
	goto end;
    }
    if (BYTESPERPIXEL == 4 && gc->bytesperpixel == 3) {
	/* Special case. */
	int soffset, doffset;
	if (BYTEWIDTH / 4 != gc->bytewidth / 3) {
	    /* Even more special case for physical truecolor */
	    /* modes that have extra scanline padding. */
	    /* This has the effect of slowing down */
	    /* '3d' in some truecolor modes on ATI mach32. */
	    gl_copyboxtocontext(0, 0, WIDTH, HEIGHT, gc, 0, 0);
	    goto end;
	}
	soffset = 0;
	doffset = screenoffset;
	size = WIDTH * HEIGHT;
	while (soffset / 4 < size) {
	    int schunk, dchunk;
	    int count;
	    schunk = __svgalib_driver_setread(&__currentcontext, soffset, &svp);
	    dchunk = __svgalib_driver_setwrite(gc, doffset, &dvp);
	    if (dchunk == 1) {
		/* One byte left in segment. */
		int pix;
		pix = *(unsigned *) svp;	/* 32-bit pixel */
		*(unsigned char *) dvp = pix;
		dchunk = __svgalib_driver_setwrite(gc, doffset + 1, &dvp);
		*(unsigned short *) dvp = pix >> 8;
		count = 1;	/* 1 pixel handled. */
	    } else if (dchunk == 2) {
		/* Two bytes left. */
		int pix;
		pix = *(unsigned *) svp;	/* 32-bit pixel */
		*(unsigned short *) dvp = pix;
		dchunk = __svgalib_driver_setwrite(gc, doffset + 2, &dvp);
		*(unsigned char *) dvp = pix >> 16;
		count = 1;	/* 1 pixel handled. */
	    } else {
		count = min(min(schunk / 4, dchunk / 3),
			    size - (soffset / 4));
		__svgalib_memcpy4to3(dvp, svp, count);
	    }
	    soffset += count * 4;
	    doffset += count * 3;
	}
	goto end;
    }
    if (BYTESPERPIXEL == 4 && gc->bytesperpixel == 4 &&
	(gc->modeflags & MODEFLAG_32BPP_SHIFT8)) {
	int soffset = 0;
	int doffset = screenoffset;
	size = SCREENSIZE(__currentcontext);
	while (soffset < size) {
	    int schunk, dchunk;
	    int count;
	    schunk = __svgalib_driver_setread(&__currentcontext, soffset, &svp);
	    dchunk = __svgalib_driver_setwrite(gc, doffset, &dvp);
	    count = min(min(schunk, dchunk), (size - soffset));
	    __svgalib_memcpy32shift8(dvp, svp, count / 4);
	    soffset += count;
	    doffset += count;
	}
    } else {
	int soffset = 0;
	int doffset = screenoffset;
	size = SCREENSIZE(__currentcontext);
	while (soffset < size) {
	    int schunk, dchunk;
	    int count;
	    schunk = __svgalib_driver_setread(&__currentcontext, soffset, &svp);
	    dchunk = __svgalib_driver_setwrite(gc, doffset, &dvp);
	    count = min(min(schunk, dchunk), (size - soffset));
	    __memcpy(dvp, svp, count);
	    soffset += count;
	    doffset += count;
	}
    }

  end:
    if (gc->modeflags & (MODEFLAG_PAGEFLIPPING_ENABLED |
			 MODEFLAG_TRIPLEBUFFERING_ENABLED)) {
	GraphicsContext save;
	/* setdisplaystart will use BYTEWIDTH of the virtual screen, */
	/* which is what we want since vga_setdisplaystart is */
	/* defined in terms of pixel offset (except for hicolor */
	/* modes, which are defined in terms of bytes). */
	gl_getcontext(&save);
	gl_setcontext(gc);
	if (gc->modeflags & MODEFLAG_FLIPPAGE_BANKALIGNED)
	    vga_setdisplaystart(screenoffset);
	else
	    gl_setdisplaystart(0, gc->height * gc->flippage);
	gl_setcontext(&save);
	/* For page flipping, it might be appropriate to add a */
	/* waitverticalretrace here. */
    }
    screenoffset = 0;
}

void gl_copyboxtocontext(int x1, int y1, int w, int h, GraphicsContext * gc,
			 int x2, int y2)
{
/* This is now reasonably efficient if clipping is not enabled. */
    void *buf;
    GraphicsContext save;
    gl_getcontext(&save);
    if ((MODETYPE == CONTEXT_LINEAR || MODETYPE == CONTEXT_VIRTUAL) &&
	(BYTESPERPIXEL == gc->bytesperpixel) &&
	!__clip && !gc->clip) {
#ifdef DLL_CONTEXT_SHADOW
	__currentcontext = *gc;
#else
	gl_setcontext(gc);
#endif
/*
 * Note: Using save.bytewidth / BYTESPERPIXEL is probably not an optimal hack here.
 * it would be better to transfer save.bytewidth to putbox as it is what is really
 * used there. However, putbox is used all over interpreting the last entry as a
 * pixel count, so we keep it this way to avoid problems if some other place not
 * updated by accident.
 */
	putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h, save.vbuf +
	       y1 * save.bytewidth + x1 * BYTESPERPIXEL,
	       save.bytewidth / BYTESPERPIXEL);
	goto end;
    }
    buf = alloca(w * h * BYTESPERPIXEL);
    gl_getbox(x1, y1, w, h, buf);
#ifdef DLL_CONTEXT_SHADOW
    __currentcontext = *gc;
#else
    gl_setcontext(gc);
#endif

    if (save.bytesperpixel == 4 && gc->bytesperpixel == 3) {
	/* Special case conversion from 32-bit virtual screen to */
	/* 24-bit truecolor framebuffer. */
	if (gc->modetype == CONTEXT_PAGED || gc->clip) {
	    /* For paged modes or clipping, use another buffer. */
	    void *buf2;
	    buf2 = alloca(w * h * 3);
	    __svgalib_memcpy4to3(buf2, buf, w * h);
	    gl_putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h,
		      buf2);
	} else
	    /* No clipping, linear. */
	    __svgalib_driver24_putbox32(x2, y2, w, h, buf, w);
    } else			/* Contexts assumed to have same pixel size. */
	gl_putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h, buf);

  end:
#ifdef DLL_CONTEXT_SHADOW
    __currentcontext = save;
#else
    gl_setcontext(&save);
#endif
}

void gl_copyboxfromcontext(GraphicsContext * gc, int x1, int y1, int w, int h,
			   int x2, int y2)
{
    void *buf;
    GraphicsContext save;
    if ((gc->modetype == CONTEXT_LINEAR || gc->modetype == CONTEXT_VIRTUAL) &&
	(BYTESPERPIXEL == gc->bytesperpixel) &&
	!__clip && !gc->clip) {
/*
 * see above on  gc->bytewidth / BYTESPERPIXEL.
 */
	putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h, gc->vbuf +
	       y1 * gc->bytewidth + x1 * BYTESPERPIXEL,
	       gc->bytewidth / BYTESPERPIXEL);
	return;
    }
    gl_getcontext(&save);
#ifdef DLL_CONTEXT_SHADOW
    __currentcontext = *gc;
#else
    gl_setcontext(gc);
#endif
    buf = alloca(w * h * BYTESPERPIXEL);
    gl_getbox(x1, y1, w, h, buf);
#ifdef DLL_CONTEXT_SHADOW
    __currentcontext = save;
#else
    gl_setcontext(&save);
#endif

    if (gc->bytesperpixel == 4 && save.bytesperpixel == 3) {
	/* Special case conversion from 32-bit virtual screen to */
	/* 24-bit truecolor framebuffer. */
	if (save.modetype == CONTEXT_PAGED || save.clip) {
	    /* For paged modes or clipping, use another buffer. */
	    void *buf2;
	    buf2 = alloca(w * h * 3);
	    __svgalib_memcpy4to3(buf2, buf, w * h);
	    gl_putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h,
		      buf2);
	} else
	    /* No clipping, linear. */
	    __svgalib_driver24_putbox32(x2, y2, w, h, buf, w);
    } else			/* Contexts assumed to have same pixel size. */
	gl_putbox(x2, y2 + screenoffset / BYTEWIDTH, w, h, buf);
}
