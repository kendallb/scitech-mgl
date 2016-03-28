
#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <vgagl.h>


/* The logo was drawn by John Remyn. */
/* Feel free to provide a more beautiful/official/thought provoking/cool */
/* logo to replace it. */
#define LOGOWIDTH 201
#define LOGOHEIGHT 85

int VGAMODE;
int VIRTUAL;

GraphicsContext *backscreen;
GraphicsContext *physicalscreen;
void *logobitmap;


void loadbitmap(char *filename, void *buf)
{
    FILE *f;
    void *bufp;
    f = fopen(filename, "rb");
    bufp = buf;
    for (;;) {
	int count = fread(bufp, 1, 8192, f);
	if (count == 0)
	    break;
	if (feof(f))
	    break;
	bufp += count;
    }
    fclose(f);
}


void test(void)
{
    int i, j;
    unsigned char *bitmap;
    GraphicsContext *savedcontext;

    if (VIRTUAL)
	gl_setcontext(backscreen);

    gl_clearscreen(0);
    for (i = 0; i < 5; i++) {
	gl_clearscreen(0);
	for (j = 0; j < 20000; j++)
	    gl_setpixel(random() % WIDTH, random() % HEIGHT,
			random() % COLORS);
    }

    if (VIRTUAL)
	gl_copyscreen(physicalscreen);

    gl_clearscreen(0);
    for (i = 0; i < 5000; i++) {
	int x, y;
	x = random() % (WIDTH - 1);
	y = random() % (HEIGHT - 1);
	gl_fillbox(x, y, random() % (WIDTH - x), random()
		   % (HEIGHT - y), random() % COLORS);
    }

    if (VIRTUAL)
	gl_copyscreen(physicalscreen);

    gl_clearscreen(0);
    for (i = 0; i < 4000; i++)
	gl_line(random() % WIDTH, random() % HEIGHT,
		random() % WIDTH, random() % HEIGHT,
		random() % COLORS);

    if (VIRTUAL)
	gl_copyscreen(physicalscreen);

    /* Create bitmap. */
    bitmap = malloc(64 * 64 * BYTESPERPIXEL);
    /* Create temporary graphics context to create bitmap in. */
    savedcontext = gl_allocatecontext();
    gl_getcontext(savedcontext);
    gl_setcontextvirtual(64, 64, BYTESPERPIXEL, BITSPERPIXEL, bitmap);
    /* The rgb functions can be used to create nice bitmaps easily for */
    /* hicolor/truecolor modes. The 256 color 'emulated' truecolor */
    /* looks less impressive. */
    for (i = 0; i < 32; i++)
	for (j = 0; j < 32; j++) {
	    int c;
	    c = 255 - (i + j) * 4;
	    gl_setpixelrgb(31 - i, 31 - j, c, 0, 0);
	    gl_setpixelrgb(32 + i, 31 - j, c, c, 0);
	    gl_setpixelrgb(31 - i, 32 + j, c, 0, c);
	    gl_setpixelrgb(32 + i, 32 + j, c, c, c);
	}
    /* Restore previous context. */
    gl_setcontext(savedcontext);

    gl_clearscreen(0);
    for (i = 0; i < 4000; i++) {
	int x, y;
	x = random() % (WIDTH - 64 + 1);
	y = random() % (HEIGHT - 64 + 1);
	gl_putbox(x, y, 64, 64, bitmap);
    }

    free(bitmap);

    if (VIRTUAL)
	gl_copyscreen(physicalscreen);
}


void setcustompalette(void)
{
    /* colors 0-31 are an RGB mix (bits 0 and 1 red, 2 green, 3 and 4 blue) */
    /* 32-63    black to red */
    /* 64-95    black to green */
    /* 96-127   black to yellow */
    /* 128-159  black to blue */
    /* 160-191  black to magenta */
    /* 192-223  black to cyan */
    /* 224-255  black to white */
    Palette pal;
    int i;
    for (i = 0; i < 256; i++) {
	int r, g, b;
	r = g = b = 0;
	if ((i & 32) > 0)
	    r = (i & 31) << 1;
	if ((i & 64) > 0)
	    g = (i & 31) << 1;
	if ((i & 128) > 0)
	    b = (i & 31) << 1;
	if (i < 32) {
	    r = (i & 3) << 4;	/* 2 bits */
	    g = (i & 4) << 3;	/* 1 bit */
	    b = (i & 24) << 1;	/* 2 bits */
	}
	pal.color[i].red = r;
	pal.color[i].green = g;
	pal.color[i].blue = b;
    }
    gl_setpalette(&pal);
}


void logotest(void)
{
    int h;
    void *scaled;
    /* Set logo palette. */
    setcustompalette();
    /* Create logo bitmap */
    logobitmap = alloca(LOGOWIDTH * LOGOHEIGHT);
    loadbitmap("linuxlogo.bmp", logobitmap);
    /* Allocate buffer for scaled bitmap. */
    scaled = alloca(WIDTH * HEIGHT);
    gl_clearscreen(0);
    /* Stretch vertically. */
    for (h = 0; h <= LOGOHEIGHT; h++) {
	gl_scalebox(LOGOWIDTH, LOGOHEIGHT, logobitmap,
		    LOGOWIDTH, h, scaled);
	gl_putbox(0, 0, LOGOWIDTH, h, scaled);
	if (VIRTUAL)
	    gl_copyscreen(physicalscreen);
    }
    gl_clearscreen(0);
    /* Scale to screen resolution. */
    gl_scalebox(LOGOWIDTH, LOGOHEIGHT, logobitmap, WIDTH, HEIGHT, scaled);
    gl_putbox(0, 0, WIDTH, HEIGHT, scaled);
    gl_copyscreen(physicalscreen);
}


void main(void)
{
    vga_init();
    VGAMODE = vga_getdefaultmode();
    if (VGAMODE == -1)
	VGAMODE = G320x200x256;	/* Default mode. */

    if (!vga_hasmode(VGAMODE)) {
	printf("Mode not available.\n");
	exit(-1);
    }
    VIRTUAL = 0;		/* No virtual screen. */
    if (vga_getmodeinfo(VGAMODE)->colors == 16 ||
	(vga_getmodeinfo(VGAMODE)->flags & IS_MODEX))
	/* These modes are supported indirectly by vgagl. */
	VIRTUAL = 1;

    if (VIRTUAL) {
	/* Create virtual screen. */
	gl_setcontextvgavirtual(VGAMODE);
	backscreen = gl_allocatecontext();
	gl_getcontext(backscreen);
    }
    vga_setmode(VGAMODE);
    gl_setcontextvga(VGAMODE);	/* Physical screen context. */
    physicalscreen = gl_allocatecontext();
    gl_getcontext(physicalscreen);
    if (COLORS == 256)
	gl_setrgbpalette();

    test();

    /* Now do the same with clipping enabled. */
    gl_clearscreen(0);
    gl_setclippingwindow(WIDTH / 4, HEIGHT / 4, WIDTH - WIDTH / 4 - 1,
			 HEIGHT - HEIGHT / 4 - 1);

    test();

    gl_disableclipping();
    if (COLORS == 256)
	/* Show the logo if using 256 color mode. */
	logotest();

    getchar();

    if (VIRTUAL)
	gl_freecontext(backscreen);
    vga_setmode(TEXT);
    exit(0);
}
