/* Graphics Library headerfile */

#ifndef VGAGL_H
#define VGAGL_H

#ifdef __cplusplus
extern "C"
{
#endif


/* Graphics context */

#define CONTEXT_VIRTUAL		0x0
#define CONTEXT_PAGED		0x1
#define CONTEXT_LINEAR		0x2
#define CONTEXT_MODEX		0x3
#define CONTEXT_PLANAR16	0x4

#define MODEFLAG_PAGEFLIPPING_CAPABLE		0x01
#define MODEFLAG_TRIPLEBUFFERING_CAPABLE	0x02
#define MODEFLAG_PAGEFLIPPING_ENABLED		0x04
#define MODEFLAG_TRIPLEBUFFERING_ENABLED	0x08
#define MODEFLAG_FLIPPAGE_BANKALIGNED		0x10
/*
 * The next two can never occur together, thus we use the same flag
 * (as svgalib does).
 */
#define MODEFLAG_32BPP_SHIFT8			0x20
#define MODEFLAG_24BPP_REVERSED			0x20

    typedef struct {
	void (*driver_setpixel_func) (int, int, int);
	int (*driver_getpixel_func) (int, int);
	void (*driver_hline_func) (int, int, int, int);
	void (*driver_fillbox_func) (int, int, int, int, int);
	void (*driver_putbox_func) (int, int, int, int, void *, int);
	void (*driver_getbox_func) (int, int, int, int, void *, int);
	void (*driver_putboxmask_func) (int, int, int, int, void *);
	void (*driver_putboxpart_func) (int, int, int, int, int, int, void *,
					int, int);
	void (*driver_getboxpart_func) (int, int, int, int, int, int, void *,
					int, int);
	void (*driver_copybox_func) (int, int, int, int, int, int);
    } framebufferfunctions;

    typedef struct {
	unsigned char modetype;	/* virtual, paged, linear, mode X */
	unsigned char modeflags;	/* or planar16 */
	unsigned char dummy;
	unsigned char flippage;
	int width;		/* width in pixels */
	int height;		/* height in pixels */
	int bytesperpixel;	/* bytes per pixel (1, 2, 3, or 4) */
	int colors;		/* number of colors */
	int bitsperpixel;	/* bits per pixel (8, 15, 16 or 24) */
	int bytewidth;		/* length of a scanline in bytes */
	char *vbuf;		/* address of framebuffer */
	int clip;		/* clipping enabled? */
	int clipx1;		/* top-left coordinate of clip window */
	int clipy1;
	int clipx2;		/* bottom-right coordinate of clip window */
	int clipy2;
	framebufferfunctions ff;
    } GraphicsContext;

    extern GraphicsContext currentcontext;

#define BYTESPERPIXEL (currentcontext.bytesperpixel)
#define BYTEWIDTH (currentcontext.bytewidth)
#define WIDTH (currentcontext.width)
#define HEIGHT (currentcontext.height)
#define VBUF (currentcontext.vbuf)
#define MODETYPE (currentcontext.modetype)
#define MODEFLAGS (currentcontext.modeflags)
#define BITSPERPIXEL (currentcontext.bitsperpixel)
#define COLORS (currentcontext.colors)

#define __clip (currentcontext.clip)
#define __clipx1 (currentcontext.clipx1)
#define __clipy1 (currentcontext.clipy1)
#define __clipx2 (currentcontext.clipx2)
#define __clipy2 (currentcontext.clipy2)


/* Configuration */

    int gl_setcontextvga(int m);
    int gl_setcontextvgavirtual(int m);
    void gl_setcontextvirtual(int w, int h, int bpp, int bitspp, void *vbuf);
    void gl_setcontextwidth(int w);
    void gl_setcontextheight(int h);
    GraphicsContext *gl_allocatecontext(void);
    void gl_setcontext(GraphicsContext * gc);
    void gl_getcontext(GraphicsContext * gc);
    void gl_freecontext(GraphicsContext * gc);

/* Line drawing */

    void gl_setpixel(int x, int y, int c);
    void gl_setpixelrgb(int x, int y, int r, int g, int b);
    int gl_getpixel(int x, int y);
    void gl_getpixelrgb(int x, int y, int *r, int *g, int *b);
    int gl_rgbcolor(int r, int g, int b);
    void gl_hline(int x1, int y, int x2, int c);
    void gl_line(int x1, int y1, int x2, int y2, int c);
    void gl_circle(int x, int y, int r, int c);

/* Box (bitmap) functions */

    void gl_fillbox(int x, int y, int w, int h, int c);
    void gl_getbox(int x, int y, int w, int h, void *dp);
    void gl_putbox(int x, int y, int w, int h, void *dp);
    void gl_putboxpart(int x, int y, int w, int h, int bw, int bh, void *b,
		       int xo, int yo);
    void gl_putboxmask(int x, int y, int w, int h, void *dp);
    void gl_copybox(int x1, int y1, int w, int h, int x2, int y2);
    void gl_copyboxtocontext(int x1, int y1, int w, int h, GraphicsContext * gc,
			     int x2, int y2);
    void gl_copyboxfromcontext(GraphicsContext * gc, int x1, int y1, int w, int h,
			       int x2, int y2);
/* The following functions only work in 256-color modes: */
    void gl_compileboxmask(int w, int h, void *sdp, void *ddp);
    int gl_compiledboxmasksize(int w, int h, void *sdp);
    void gl_putboxmaskcompiled(int x, int y, int w, int h, void *dp);

/* Miscellaneous */

    void gl_clearscreen(int c);
    void gl_scalebox(int w1, int h1, void *sb, int w2, int h2, void *db);
    void gl_setdisplaystart(int x, int y);
    void gl_enableclipping(void);
    void gl_setclippingwindow(int x1, int y1, int x2, int y2);
    void gl_disableclipping(void);

/* Screen buffering */

    void gl_copyscreen(GraphicsContext * gc);
    void gl_setscreenoffset(int o);
    int gl_enablepageflipping(GraphicsContext * gc);

/* Text */

/* Writemode flags. */
#define WRITEMODE_OVERWRITE 0
#define WRITEMODE_MASKED 1
#define FONT_EXPANDED 0
#define FONT_COMPRESSED 2

    void gl_expandfont(int fw, int fh, int c, void *sfdp, void *dfdp);
    void gl_setfont(int fw, int fh, void *fdp);
    void gl_colorfont(int fw, int fh, int c, void *fdp);
    void gl_setwritemode(int wm);
    void gl_write(int x, int y, char *s);
    void gl_writen(int x, int y, int n, char *s);
    void gl_setfontcolors(int bg, int fg);

/* gl_printf is only available in ELF libraries!! */
    int gl_printf(int x, int y, const char *fmt,...);

    extern unsigned char *gl_font8x8;	/* compressed 8x8 font */

/* 256-color Palette */

    typedef struct {
	struct {
	    unsigned char red;	/* 6-bit values */
	    unsigned char green;
	    unsigned char blue;
	} color[256];
    } Palette;

    void gl_setpalettecolor(int c, int r, int b, int g);
    void gl_getpalettecolor(int c, int *r, int *b, int *g);
    void gl_setpalettecolors(int s, int n, void *dp);
    void gl_getpalettecolors(int s, int n, void *dp);
    void gl_setpalette(void *p);
    void gl_getpalette(void *p);
    void gl_setrgbpalette(void);


#ifdef __cplusplus
}

#endif
#endif
