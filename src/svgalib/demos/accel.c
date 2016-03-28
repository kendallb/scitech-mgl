
/*
 * This is only a tool to test the acceleration primitives.
 * Do not use the primitives as library graphics functions;
 * have higher level functions make use of acceleration
 * primitives if available, otherwise using normal framebuffer code.
 * For example, the vgagl should use acceleration when it can.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <alloca.h>
#include <string.h>
#include "vga.h"
#include "vgagl.h"
#include "wizard.xbm"
#include "wizardmsk.xbm"

#define CROSS_SZ 5

#define LOGO_NAME	"linuxlogo.bmp"
#define LOGO_WIDTH	201
#define LOGO_HEIGHT	85

#define BOXES_Y 3
#define BOXES_X 4
#define BOXES_B 10

#define BOXES_WIDTH (WIDTH/BOXES_X)
#define BOXES_HEIGHT ((HEIGHT - 8) / BOXES_Y)

#define COPY_OFF 3

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define abs(a) (((a) < 0) ? -(a) : (a))

#define QIXLINES 200

int vgamode;
int background_blits;
unsigned char *bitmap;
int accelfuncs, ropfuncs, transfuncs;
int ropmodes, transmodes;
int maxy;

unsigned char blitimage[LOGO_WIDTH * LOGO_HEIGHT], *scaleimage, *expandimage;
unsigned char conv_wizard_bits[wizard_height * ((wizard_width + 31) & ~31)];
unsigned char conv_wizardmsk_bits[wizardmsk_height * ((wizardmsk_width + 31) & ~31)];

void Configure(void);
void DrawDots(void);
void DoAccelTest(void (*accelfunc) (void), char *name, int exp_pixelrate,
		 int pixels_per_call);
void FillBoxTest(void);
void HlineBoxTest(void);
void ScreenCopyTest(void);
void ScrollTest(void);
void FillBoxXORTest(void);
void PutBitmapTest(void);
void PositionTest(void);
void GetLogo(void);
void ScaleLogo(void);
void RopTest(void);
void QixDemo(int rop, char *txt);

void convxbm(int width, int height, unsigned char *bits, unsigned char *conv_bits);

void main(void)
{
    vga_modeinfo *minfo;

    vga_init();

    Configure();

    /* Getting accel info only works if the mode it set. */
    vga_setlinearaddressing();
    vga_setmode(vgamode);

    accelfuncs = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_ACCEL);
    ropfuncs = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_ROP);
    transfuncs = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_TRANSPARENCY);
    ropmodes = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_ROPMODES);
    transmodes = vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_TRANSMODES);
    minfo = vga_getmodeinfo(vgamode);
    if (minfo->bytesperpixel)
	maxy = minfo->maxpixels / (minfo->linewidth / minfo->bytesperpixel);
    else
	maxy = HEIGHT;

    usleep(100000);
    vga_setmode(TEXT);

    if (accelfuncs == 0) {
	printf("No acceleration supported.\n");
	exit(0);
    }
    printf("Accelflags: 0x%08X\n", accelfuncs);

    if (accelfuncs & ACCELFLAG_FILLBOX)
	printf("FillBox supported.\n");
    if (accelfuncs & ACCELFLAG_SCREENCOPY)
	printf("ScreenCopy supported.\n");
    if (accelfuncs & ACCELFLAG_PUTIMAGE)
	printf("PutImage supported.\n");
    if (accelfuncs & ACCELFLAG_DRAWLINE)
	printf("DrawLine.\n");
    if (accelfuncs & ACCELFLAG_SETFGCOLOR)
	printf("SetFGColor supported.\n");
    if (accelfuncs & ACCELFLAG_SETBGCOLOR)
	printf("SetBGColor supported.\n");
    if (accelfuncs & ACCELFLAG_SETTRANSPARENCY)
	printf("SetTransparency supported.\n");
    if (accelfuncs & ACCELFLAG_SETRASTEROP)
	printf("SetRasterOp supported.\n");
    if (accelfuncs & ACCELFLAG_PUTBITMAP)
	printf("PutBitmap supported.\n");
    if (accelfuncs & ACCELFLAG_SCREENCOPYBITMAP)
	printf("ScreenCopyBitmap supported.\n");
    if (accelfuncs & ACCELFLAG_DRAWHLINELIST)
	printf("DrawHLineList supported.\n");
    if (accelfuncs & ACCELFLAG_POLYLINE)
	printf("PolyLine supported.\n");
    if (accelfuncs & ACCELFLAG_POLYHLINE)
	printf("PolyHLine supported.\n");
    if (accelfuncs & ACCELFLAG_POLYFILLMODE)
	printf("PolyFillMode supported.\n");
    if (accelfuncs & ACCELFLAG_SETMODE)
	printf("SetMode supported.\n");
    if (accelfuncs & ACCELFLAG_SYNC)
	printf("Sync supported.\n");
    
    if (accelfuncs & ACCELFLAG_SETRASTEROP) {
	printf("\nRopAccelflags: 0x%08X\n", ropfuncs);
	printf("RopModes: 0x%08X", ropmodes);
	if (ropmodes & (1 << ROP_COPY))
	    printf(" Copy");
	if (ropmodes & (1 << ROP_OR))
	    printf(" Or");
	if (ropmodes & (1 << ROP_AND))
	    printf(" And");
	if (ropmodes & (1 << ROP_XOR))
	    printf(" Xor");
	if (ropmodes & (1 << ROP_INVERT))
	    printf(" Invert");
	printf("\n");
	if (ropfuncs & ACCELFLAG_FILLBOX)
	    printf("FillBox supported.\n");
	if (ropfuncs & ACCELFLAG_SCREENCOPY)
	    printf("ScreenCopy supported.\n");
	if (ropfuncs & ACCELFLAG_PUTIMAGE)
	    printf("PutImage supported.\n");
	if (ropfuncs & ACCELFLAG_DRAWLINE)
	    printf("DrawLine.\n");
	if (ropfuncs & ACCELFLAG_PUTBITMAP)
	    printf("PutBitmap supported.\n");
	if (ropfuncs & ACCELFLAG_SCREENCOPYBITMAP)
	    printf("ScreenCopyBitmap supported.\n");
	if (ropfuncs & ACCELFLAG_DRAWHLINELIST)
	    printf("DrawHLineList supported.\n");
	if (ropfuncs & ACCELFLAG_POLYLINE)
	    printf("PolyLine supported.\n");
	if (ropfuncs & ACCELFLAG_POLYHLINE)
	    printf("PolyHLine supported.\n");
	if (ropfuncs & ACCELFLAG_POLYFILLMODE)
	    printf("PolyFillMode supported.\n");
    }

    if (accelfuncs & ACCELFLAG_SETTRANSPARENCY) {
	printf("\nTransparencyAccelflags: 0x%08X\n", transfuncs);
	printf("TranparencyModes: 0x%08X", transmodes);
	if (transmodes & (1 << ENABLE_TRANSPARENCY_COLOR))
	    printf(" TransparentColor");
	if (transmodes & (1 << ENABLE_BITMAP_TRANSPARENCY))
	    printf(" TransparentBitmap");
	printf("\n");
	if (transfuncs & ACCELFLAG_FILLBOX)
	    printf("FillBox supported.\n");
	if (transfuncs & ACCELFLAG_SCREENCOPY)
	    printf("ScreenCopy supported.\n");
	if (transfuncs & ACCELFLAG_PUTIMAGE)
	    printf("PutImage supported.\n");
	if (transfuncs & ACCELFLAG_DRAWLINE)
	    printf("DrawLine.\n");
	if (transfuncs & ACCELFLAG_PUTBITMAP)
	    printf("PutBitmap supported.\n");
	if (transfuncs & ACCELFLAG_SCREENCOPYBITMAP)
	    printf("ScreenCopyBitmap supported.\n");
	if (transfuncs & ACCELFLAG_DRAWHLINELIST)
	    printf("DrawHLineList supported.\n");
	if (transfuncs & ACCELFLAG_POLYLINE)
	    printf("PolyLine supported.\n");
	if (transfuncs & ACCELFLAG_POLYHLINE)
	    printf("PolyHLine supported.\n");
	if (transfuncs & ACCELFLAG_POLYFILLMODE)
	    printf("PolyFillMode supported.\n");
    }
    
    printf("\nPress enter to start test.\n");
    getchar();

    convxbm(wizard_width, wizard_height, wizard_bits, conv_wizard_bits);
    convxbm(wizardmsk_width, wizardmsk_height, wizardmsk_bits, conv_wizardmsk_bits);
    GetLogo();

    vga_setmode(vgamode);
    gl_setcontextvga(vgamode);

    ScaleLogo();
    gl_setcontextvga(vgamode);

    if (COLORS == 256)
	gl_setrgbpalette();
    gl_setwritemode(FONT_COMPRESSED);
    gl_setfont(8, 8, gl_font8x8);
    gl_setfontcolors(gl_rgbcolor(0, 0, 0), gl_rgbcolor(255, 255, 255));

    background_blits = 0;
    if (accelfuncs & ACCELFLAG_SYNC)
	background_blits = 1;

    PositionTest();
    gl_clearscreen(0);
    if (accelfuncs & ACCELFLAG_SETRASTEROP)
	RopTest();

    QixDemo(ROP_COPY, "Replace QuixDemo");
    QixDemo(ROP_XOR, "Xor QuixDemo");
    if (accelfuncs & ACCELFLAG_FILLBOX)
	DoAccelTest(
		       FillBoxTest,
		       "FillBox",
		       50000000 / BYTESPERPIXEL,
		       WIDTH * HEIGHT * 256
	    );

    if (accelfuncs & ACCELFLAG_SCREENCOPY) {
	DrawDots();
	gl_write(1, 1, "Measuring ACCEL_SCREENCOPY");
	DoAccelTest(
		       ScreenCopyTest,
		       "ScreenCopy",
		       20000000 / BYTESPERPIXEL,
		       WIDTH * (HEIGHT / 3) * 3
	    );
    }
    if ((accelfuncs & ACCELFLAG_SCREENCOPY)
	&& (accelfuncs & ACCELFLAG_FILLBOX)
	&& (maxy > HEIGHT)) {
	DrawDots();
	gl_write(1, HEIGHT - 9, "Scrolling with SCREENCOPY & FILLBOX");
	DoAccelTest(
		       ScrollTest,
		       "Scroll Demo",
		       20000000 / BYTESPERPIXEL,
		       WIDTH * HEIGHT
	    );
    }
    if (accelfuncs & ACCELFLAG_DRAWHLINELIST)
	DoAccelTest(
		       HlineBoxTest,
		       "FillBox with DrawHlineList",
		       50000000 / BYTESPERPIXEL,
		       WIDTH * HEIGHT * 256
	    );

    if ((accelfuncs & ACCELFLAG_FILLBOX)
	&& (accelfuncs & ACCELFLAG_SETRASTEROP)
	&& (ropfuncs & ACCELFLAG_FILLBOX)) {
	DrawDots();
	gl_write(1, 1, "FILLBOX, xor with varying colors");
	DoAccelTest(
		       FillBoxXORTest,
		       "FillBox XOR",
		       30000000 / BYTESPERPIXEL,
		       WIDTH * HEIGHT * 256
		);
    }
    if (accelfuncs & ACCELFLAG_PUTBITMAP) {
	bitmap = malloc(WIDTH * HEIGHT / 8);
	memset(bitmap, 0x55, WIDTH * HEIGHT / 8);
	DrawDots();
	DoAccelTest(
		       PutBitmapTest,
		       "PutBitmap",
		       30000000 / BYTESPERPIXEL,
		       WIDTH * HEIGHT * 2
	    );
    }
    vga_setmode(TEXT);
    exit(-1);
}


void Configure(void)
{
    int allowed[GLASTMODE + 1];

    for (;;) {
	int i;
	int m;
	for (i = G320x200x16; i <= GLASTMODE; i++) {
	    allowed[i] = 0;
	    if (vga_hasmode(i)
		&& vga_getmodeinfo(i)->bytesperpixel >= 1) {
		printf("%2d  %s\n", i, vga_getmodename(i));
		allowed[i] = 1;
	    }
	}

	printf("\nWhich mode? ");
	scanf("%d", &m);
	getchar();
	printf("\n");
	if (m >= G320x200x16 && m <= GLASTMODE) {
	    vgamode = m;
	    break;
	}
    }
}

void convxbm(int width, int height, unsigned char *bits, unsigned char *conv_bits)
{
    int scanline = ((width + 31) & ~31);
    int y;

    scanline >>= 3;
    width = (width + 7) >> 3;
    for(y = 0; y < height; y++, conv_bits += scanline, bits += width) {
	memcpy(conv_bits, bits, width);
	if (width < scanline)
	    memset(conv_bits + width, 0, scanline - width);
    }
}

static unsigned char rotbyte(unsigned char c)
{
	unsigned char bit = 0x80, res = 0;

	for (; c; c >>= 1, bit >>= 1)
	    if (c & 1)
		res |= bit;
	return res;
}

void DrawDots(void)
{
    int i, n;
    /* Fill the screen with random dots. */
    gl_clearscreen(0);
    n = WIDTH * HEIGHT / 100;	/* 1% filled. */
    for (i = 0; i < n; i++)
	gl_setpixel(rand() % WIDTH, rand() % HEIGHT, rand() % COLORS);
}

void DoAccelTest(void (*accelfunc) (void), char *name, int exp_pixelrate,
		 int pixels_per_call)
{
    int i, n, startclock, diffclock;
    int rate, byterate;
    if (exp_pixelrate < 0)
	/* Special case, #iterations indicated. */
	n = -exp_pixelrate;
    else
	/* Aim for about 5 seconds. */
	n = exp_pixelrate * 5 / pixels_per_call + 1;

    if (background_blits)
	vga_accel(ACCEL_SETMODE, BLITS_IN_BACKGROUND);

    startclock = clock();
    for (i = 0; i < n; i++)
	(*accelfunc) ();
    if (background_blits)
	vga_accel(ACCEL_SYNC);
    diffclock = clock() - startclock;
    if (diffclock == 0) {
	/* At least let the program continue if something strange happens */
	printf("%s: Timing Error",name);
    } else {
	rate = (long long) n *pixels_per_call * 100 / diffclock;
	byterate = rate * BYTESPERPIXEL;
	printf("%s: %ld.%ld Mpixels/s (%ld.%ld Mbytes/s)\n", name,
	   rate / 1000000L, (rate % 1000000L) / 100000L,
	   byterate / 1000000L, (byterate % 1000000L) / 100000L);
    }
}

void FillBoxTest(void)
{
    int i;
    for (i = 0; i < 256; i++) {
	vga_accel(ACCEL_SETFGCOLOR, i);
	vga_accel(ACCEL_FILLBOX, 0, 0, WIDTH, HEIGHT);
    }
}

void ScreenCopyTest(void)
{
    /* Copy first 1/3 to second 1/3. */
    vga_accel(ACCEL_SCREENCOPY, 0, 0, 0, HEIGHT / 3, WIDTH, HEIGHT / 3);
    /* Copy third 1/3 to first 1/3. */
    vga_accel(ACCEL_SCREENCOPY, 0, (HEIGHT / 3) * 2, 0, 0,
	      WIDTH, HEIGHT / 3);
    /* Copy second 1/3 to third 1/3. */
    vga_accel(ACCEL_SCREENCOPY, 0, HEIGHT / 3, 0, (HEIGHT / 3) * 2,
	      WIDTH, HEIGHT / 3);
}

void ScrollTest(void)
{
    int i;
    /*
     * First write the line that will be scrolled into the screen,
     * located after the visible screen.
     */
    /* Clear the line. */
    vga_accel(ACCEL_SETFGCOLOR, 0);
    vga_accel(ACCEL_FILLBOX, 0, HEIGHT, WIDTH, 1);
    if (background_blits)
	vga_accel(ACCEL_SYNC);
    /* Write some new dots. */
    for (i = 0; i < WIDTH / 100; i++)
	gl_setpixel(rand() % WIDTH, HEIGHT, rand() % COLORS);
    /* Scroll. */
    vga_accel(ACCEL_SCREENCOPY, 0, 1, 0, 0,
	      WIDTH, HEIGHT);
}

void FillBoxXORTest(void)
{
    vga_accel(ACCEL_SETRASTEROP, ROP_XOR);
    vga_accel(ACCEL_SYNC);
    FillBoxTest();
    vga_accel(ACCEL_SETRASTEROP, ROP_COPY);
}

void PutBitmapTest(void)
{
    vga_accel(ACCEL_SETBGCOLOR, 0);
    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
    vga_accel(ACCEL_PUTBITMAP, 0, 0, WIDTH, HEIGHT, bitmap);
    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 255));
    vga_accel(ACCEL_PUTBITMAP, 0, 0, WIDTH, HEIGHT, bitmap);
}

void DrawCross(int x, int y, int color) {
    int i;

    for(i = y - CROSS_SZ; i <= y + CROSS_SZ; i++)
	if (y != i)
	    gl_setpixel(x, i, color);
    for(i = x - CROSS_SZ; i <= x + CROSS_SZ; i++)
	gl_setpixel(i, y, color);
}

void GetLogo(void) {
    FILE *fd;

    fd = fopen(LOGO_NAME, "rb");
    if (!fd) {
      read_err:
	perror("Problems reading linuxlogo.bmp");
	exit(2);
    }
    if (1 != fread(blitimage, LOGO_WIDTH * LOGO_HEIGHT, 1, fd))
	goto read_err;
    fclose(fd);
}

void ScaleLogo(void) {
    unsigned char *ptr;
    int w, h, x, y;

    expandimage = malloc(LOGO_WIDTH * LOGO_HEIGHT * BYTESPERPIXEL);
    w = BOXES_WIDTH - 2 * BOXES_B - 2;
    h = BOXES_HEIGHT - 2 * BOXES_B - 2;
    scaleimage = malloc(w * h * BYTESPERPIXEL);
    if (!scaleimage || !expandimage) {
	perror("Out of buffer memory");
	exit(2);
    }
    gl_setcontextvirtual(LOGO_WIDTH, LOGO_HEIGHT, BYTESPERPIXEL, BITSPERPIXEL, expandimage);
    for(y = 0, ptr = blitimage; y < LOGO_HEIGHT; y++)
	for(x = 0; x < LOGO_WIDTH; x++, ptr++) {
	    gl_setpixelrgb(x, y, (7 & *ptr) << 5, (7 & (*ptr >> 3)) << 5,
			   (3 & (*ptr >> 6)) << 6);
    }
    gl_scalebox(LOGO_WIDTH, LOGO_HEIGHT, expandimage, w, h, scaleimage);
}

void DrawFrame(int x1, int y1, int x2, int y2, int color) {
    DrawCross(x1, y1, color);
    DrawCross(x2, y1, color);
    DrawCross(x1, y2, color);
    DrawCross(x2, y2, color);
}

void DrawHlines(int x, int y, int w, int h) {
    int i, *xmin, *xmax;

    xmin = alloca(sizeof(int) * h);
    xmax = alloca(sizeof(int) * h);

    for(i = 0; i < h; i++)
	xmin[i] = x + (i * w  - i + (h >> 2)) / (h - 1);

    x += w - 1;
    for(i = 0; i < h; xmax[i++] = x);
    vga_accel(ACCEL_DRAWHLINELIST, y, h, xmin, xmax);
}

void HlineBoxTest(void) {
    int i, *xmin, *xmax;

    xmin = alloca(sizeof(int) * HEIGHT);
    xmax = alloca(sizeof(int) * HEIGHT);

    memset(xmin, 0, sizeof(int) * HEIGHT);
    for(i = 0; i < HEIGHT; xmax[i++] = WIDTH - 1);
    for (i = 0; i < 256; i++) {
	vga_accel(ACCEL_SETFGCOLOR, i);
	vga_accel(ACCEL_DRAWHLINELIST, 0, HEIGHT, xmin, xmax);
    }
}

static void draw_poly(unsigned short *coords) {
    /* Intended bug: */
    vga_accel(ACCEL_POLYLINE, ACCEL_START | ACCEL_END, 2, coords);
    vga_accel(ACCEL_POLYLINE, ACCEL_START, 2, coords + 2);
    /* Proper continuation: */
    vga_accel(ACCEL_POLYLINE, 0, 7, coords + 4);
}

static void draw_polyhli(int x, int y, int w, int h) {
    int n, i, j, dp, flag = ACCEL_START;
    unsigned short coords[40];

    vga_lockvc();
    for(i = 0; i < h;) {
	for(j = n = dp = 0; (j < 3) && (i < h); j++, i++) {
	    n++;
	    if (i < (h >> 1)) {
		coords[dp++] = 6;
		coords[dp++] = x;
		coords[dp++] = x + (((w / 4) * (h - i)) / h);
		coords[dp++] = x + (w / 2) - (((w / 6) * ((h >> 1) - i)) / h);
		coords[dp++] = x + (w / 2) + (((w / 6) * ((h >> 1) - i)) / h);
		coords[dp++] = x + w - 1 - (((w / 4) * (h - i)) / h);
		coords[dp++] = x + w - 1;
	    } else {
		coords[dp++] = 8;
		coords[dp++] = x;
		coords[dp++] = x + (((w / 4) * (h - i)) / h);
		coords[dp++] = x + ((3 * w) / 8) - (((w / 6) * (i - (h >> 1))) / h);
		coords[dp++] = x + ((3 * w) / 8) + (((w / 6) * (i - (h >> 1))) / h);
		coords[dp++] = x + ((5 * w) / 8) - (((w / 6) * (i - (h >> 1))) / h);
		coords[dp++] = x + ((5 * w) / 8) + (((w / 6) * (i - (h >> 1))) / h);
		coords[dp++] = x + w - 1 - (((w / 4) * (h - i)) / h);
		coords[dp++] = x + w - 1;
	    }
	}
	if (n)
	    vga_accel(ACCEL_POLYHLINE, flag | ((i >= h) ? ACCEL_END : 0), y, n, coords);
	flag = 0;
    }
    vga_unlockvc();
}

static void draw_polygon(int mode, int x, int y, int w, int h) {
    const unsigned short W = x + w - 1, H = y + h - 1;
    unsigned short coords[] = {
	x, y, W, y, W, H, (x + W)/2, y,
	x, H, W, (y + H)/2, (x + W)/2, H,
	x, (y + H)/2, W, (y + H)/2, 
	x, y};
    if (mode) {
	int i;

	for(i = 0; i < (sizeof(coords) / sizeof(unsigned short) - 3); i += 2)
	    if (coords[i + 1] != coords[i + 3])
		vga_accel(ACCEL_DRAWLINE, coords[i], coords[i + 1], coords[i + 2], coords[i + 3]);
    } else {
	vga_accel(ACCEL_POLYLINE, ACCEL_START, sizeof(coords) / (2 * sizeof(unsigned short)),
	    coords);
    }
}

void PositionTest(void) {
    int x, y, red, i = 0, w, h;

    red = gl_rgbcolor(255, 0, 0);
    w = BOXES_WIDTH - 2 * BOXES_B;
    h = BOXES_HEIGHT - 2 * BOXES_B;
    gl_write(0, 0, "Testing accelerator positioning:");

    for(y = BOXES_B + 8; y < HEIGHT; y += BOXES_HEIGHT)
	for(x = BOXES_B; x < WIDTH; x += BOXES_WIDTH) {
	    DrawFrame(x, y, x + w - 1, y + h - 1, red);
	    switch(i++) {
		case 0:
		    if (accelfuncs & ACCELFLAG_FILLBOX) {
			gl_write(x + 1, y + h + 3, "Fillbox");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			vga_accel(ACCEL_FILLBOX, x + 1, y + 1, w - 2, h - 2);
		    }
		    break;
		case 1:
		    if (accelfuncs & ACCELFLAG_DRAWLINE) {
			gl_write(x + 1, y + h + 3, "Linedraw");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			vga_accel(ACCEL_DRAWLINE, x + 1, y + 1, x + w - 2, y + h - 2);
			vga_accel(ACCEL_DRAWLINE, x + w - 2, y + 1, x + 1, y + h - 2);
		    }
		    break;
		case 2:
		    if (accelfuncs & ACCELFLAG_PUTIMAGE) {
			gl_write(x + 1, y + h + 3, "Putimage");
			vga_accel(ACCEL_PUTIMAGE, x + 1, y + 1, w - 2, h - 2, scaleimage);
		    }
		    break;
		case 3:
		    if (accelfuncs & ACCELFLAG_SCREENCOPY) {
			gl_write(x + 1, y + h + 3, "Screencopy (l->h)");
			gl_putbox(x + 1, y + 1, w - 2, h - 2, scaleimage); 
			vga_accel(ACCEL_SCREENCOPY, x + 1, y + 1,
				  x + COPY_OFF, y + COPY_OFF, w - 2, h - 2);
			DrawFrame(x + COPY_OFF - 1, y + COPY_OFF - 1,
				  x + COPY_OFF + w - 2, y + COPY_OFF + h - 2,
				  gl_rgbcolor(0, 255, 0));
		    }
		    break;
		case 4:
		    if (accelfuncs & ACCELFLAG_SCREENCOPY) {
			gl_write(x + 1, y + h + 3, "Screencopy (h->l)");
			gl_putbox(x + 1, y + 1, w - 2, h - 2, scaleimage); 
			vga_accel(ACCEL_SCREENCOPY, x + 1, y + 1,
				  x - COPY_OFF, y - COPY_OFF, w - 2, h - 2);
			DrawFrame(x - COPY_OFF - 1, y - COPY_OFF - 1,
				  x - COPY_OFF + w - 2, y - COPY_OFF + h - 2,
				  gl_rgbcolor(0, 255, 0));
		    }
		    break;
		case 5:
		    if (accelfuncs & ACCELFLAG_DRAWHLINELIST) {
			gl_write(x + 1, y + h + 3, "Hlinelist");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			DrawHlines(x + 1, y + 1, w - 2, h - 2);
		    }
		    break;
		case 6:
		    if (accelfuncs & ACCELFLAG_PUTBITMAP) {
			int xo, yo, i;
			unsigned int bmaptmp[8];

			gl_write(x + 1, y + h + 3, "PutBitmap");
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			for(i = 0; i < 8; i++)
			    bmaptmp[i] = rotbyte(gl_font8x8['0' * 8 + i]);
			vga_accel(ACCEL_PUTBITMAP, x + 1, y + 1,
				8, 8, bmaptmp);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(0, 255, 0));
			for(i = 0; i < 8; i++)
			    bmaptmp[i] = rotbyte(gl_font8x8['1' * 8 + i]);
			vga_accel(ACCEL_PUTBITMAP, x + w - 9, y + 1,
				8, 8, bmaptmp);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(255, 255, 255));
			for(i = 0; i < 8; i++)
			    bmaptmp[i] = rotbyte(gl_font8x8['2' * 8 + i]);
			vga_accel(ACCEL_PUTBITMAP, x + 1, y + h - 9,
				8, 8, bmaptmp);
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 255));
			for(i = 0; i < 8; i++)
			    bmaptmp[i] = rotbyte(gl_font8x8['3' * 8 + i]);
			vga_accel(ACCEL_PUTBITMAP, x + w - 9, y + h - 9,
				8, 8, bmaptmp);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 0));
			xo = (w - wizard_width) / 2;
			yo = (h - wizard_height) / 2;
			vga_accel(ACCEL_PUTBITMAP, x + xo, y + yo,
				wizard_width, wizard_height, conv_wizard_bits);
		    }
		    break;
		case 7:
		    if ((accelfuncs & ACCELFLAG_PUTBITMAP) &&
			    (accelfuncs & ACCELFLAG_SETTRANSPARENCY) &&
			    (transfuncs & ACCELFLAG_PUTBITMAP) &&
			    (transmodes & (1 << ENABLE_BITMAP_TRANSPARENCY))) {

			gl_write(x + 1, y + h + 3, "BitmapTransparency");
			gl_putbox(x + 1, y + 1, w - 2, h - 2, scaleimage); 
			vga_accel(ACCEL_SETTRANSPARENCY, ENABLE_BITMAP_TRANSPARENCY);
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_PUTBITMAP, x + 1, y + 1,
				wizardmsk_width, wizardmsk_height, conv_wizardmsk_bits);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 0));
			vga_accel(ACCEL_PUTBITMAP, x + 1, y + 1,
				wizard_width, wizard_height, conv_wizard_bits);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_PUTBITMAP, x + w - wizard_width - 1, y + 1,
				wizard_width, wizard_height, conv_wizard_bits);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			vga_accel(ACCEL_PUTBITMAP, x + 1, y + h - wizard_height - 1,
				wizard_width, wizard_height, conv_wizard_bits);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 255));
			vga_accel(ACCEL_PUTBITMAP, x + w - wizard_width - 1,
				y + h - wizard_height - 1,
				wizard_width, wizard_height, conv_wizard_bits);
			vga_accel(ACCEL_SETTRANSPARENCY, DISABLE_BITMAP_TRANSPARENCY);
		    }
		    break;
		case 8:
		    if (accelfuncs & ACCELFLAG_SCREENCOPYMONO) {
			gl_write(x + 1, y + h + 3, "MonoScreenCopy");
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			vga_accel(ACCEL_SCREENCOPYMONO, x + 1, y + h + 3,
				x + 1, y + 1, 14 * 8, 8);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(0, 255, 0));
			vga_accel(ACCEL_SCREENCOPYMONO, x + 1, y + h + 3,
				x + w - 1 - 14 * 8, y + 1, 14 * 8, 8);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(255, 255, 255));
			vga_accel(ACCEL_SCREENCOPYMONO, x + 1, y + h + 3,
				x + 1, y + h - 9, 14 * 8, 8);
			vga_accel(ACCEL_SETBGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 255));
			vga_accel(ACCEL_SCREENCOPYMONO, x + 1, y + h + 3,
				x + w - 1 - 14 * 8, y + h - 9, 14 * 8, 8);
		   }
		   break;
		case 9:
		   if (accelfuncs & ACCELFLAG_POLYLINE) {
			unsigned short X = x + 1, Y = y + 1, W = x + w - 2,
				       H = y + (h >> 1) - 2;
			
			unsigned short coords[] = {
				X, Y, (X + W)/2, Y, W, Y,
				W, H, X + (3 * (W - X))/4, H,
				(X + W)/2, (Y + H)/2,
				X + (W - X)/4, H, X, H, X, Y
			};

			gl_write(x + 1, y + h + 3, "Polyline");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));

			draw_poly(coords);

			if ((ropfuncs & ACCELFLAG_POLYLINE) &&
				(accelfuncs & ACCELFLAG_SETRASTEROP) &&
				(ropmodes & (1 << ROP_XOR))) {
			    vga_accel(ACCEL_SETRASTEROP, ROP_XOR);
			    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 255));
			}
			
			H = y + h - 2 - H;
			for (X = 1; X < (sizeof(coords) / sizeof(unsigned short)); X += 2) {
			    coords[X] += H;
			}
			draw_poly(coords);

			if ((ropfuncs & ACCELFLAG_POLYLINE) &&
				(accelfuncs & ACCELFLAG_SETRASTEROP)) {
			    vga_accel(ACCEL_SETRASTEROP, ROP_COPY);
			}
		   }
		   break;
		case 10:
		   if (accelfuncs & ACCELFLAG_POLYHLINE) {
			gl_write(x + 1, y + h + 3, "PolyHLine");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));

			draw_polyhli(x + 1, y + 1, w - 2, h - 2);
		   }
		   break;
		case 11:
		   if ((accelfuncs & ACCELFLAG_POLYLINE) &&
				(accelfuncs & ACCELFLAG_SETRASTEROP) &&
				(accelfuncs & ACCELFLAG_SCREENCOPYMONO) &&
				(accelfuncs & ACCELFLAG_POLYFILLMODE) &&
				(accelfuncs & ACCELFLAG_DRAWLINE) &&
				(ropfuncs & ACCELFLAG_DRAWLINE) &&
				(maxy > (HEIGHT + h)) &&
				(ropmodes & (1 << ROP_XOR))) {
			gl_write(x + 1, y + h + 3, "PolygonFillMode");
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 0));
			vga_accel(ACCEL_FILLBOX, x + 1, HEIGHT, w - 2, h - 2);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 255));
			vga_accel(ACCEL_SETRASTEROP, ROP_XOR);
			vga_accel(ACCEL_POLYFILLMODE, 1);
			draw_polygon(1, x + 1, HEIGHT, w - 2, h - 2);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
			vga_accel(ACCEL_SETRASTEROP, ROP_COPY);
			vga_accel(ACCEL_SCREENCOPYMONO, x + 1, HEIGHT,
				  x + 1, y + 1, w - 2, h - 2);
			vga_accel(ACCEL_POLYFILLMODE, 0);
			vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
			draw_polygon(0, x + 1, y + 1, w - 2, h - 2);
		   }
		   goto endtest;
	    }
	}

  endtest:
    vga_getch();
}

void drawdisk(int x, int y, int n, int *xmin, int *xmax) {
    int i;
    int *xmin2, *xmax2;

    xmin2 = alloca(sizeof(int) * n);
    xmax2 = alloca(sizeof(int) * n);
    for (i = 0; i < n; i++) {
	xmin2[i] = xmin[i] + x;
	xmax2[i] = xmax[i] + x;
    }
    vga_accel(ACCEL_DRAWHLINELIST, y, n, xmin2, xmax2);
}

char *RopNames[] = { "Copy/Replace", "Or", "And", "Xor", "Invert" };

void RopTest(void) {
    int x, y, i, w, h, wsq, xo, yo, red, x1, x2;
    int *xmin, *xmax;

    if (!(ropfuncs & (ACCEL_DRAWHLINELIST | ACCELFLAG_FILLBOX))) {
	printf("Rasteroperation test needs HLINELIST or FILLBOX\n");
	return;
    }

    vga_accel(ACCEL_SETTRANSPARENCY, ENABLE_TRANSPARENCY_COLOR, gl_rgbcolor(255, 0, 0));
    w = BOXES_WIDTH - 2 * BOXES_B;
    h = BOXES_HEIGHT - 2 * BOXES_B;
    wsq = min((w * 2)/3, (h * 2)/3);

    xmin = alloca(sizeof(int) * wsq);
    xmax = alloca(sizeof(int) * wsq);

    xo = wsq / 2;
    yo = xo * xo;
    for (i = 0; i < wsq; i++) {
	red = sqrt(yo - (i - xo) * (i - xo));
	xmin[i] = xo - red;
	xmax[i] = xo + red;
    }

    yo = h - wsq - 1;
    xo = (w - wsq) / 2;
    red = gl_rgbcolor(255, 0, 0);
    x1 = xo - (wsq - yo)/2;
    if (x1 < 1)
	x1 = 1;
    x2 = w - wsq - x1;

    gl_write(0, 0, "Testing raster operations:");

    for(y = BOXES_B + 8, i = 0; y < HEIGHT; y += BOXES_HEIGHT)
	for(x = BOXES_B; x < WIDTH; x += BOXES_WIDTH) {
	    DrawFrame(x, y, x + w - 1, y + h - 1, red);
	    if (! (ropmodes & (1 << i)))
		goto nextrop;
	    if ((i == ROP_AND) || (i == ROP_XOR)) {
		vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 255));
		vga_accel(ACCEL_FILLBOX, x + 1, y + 1, w - 2, h - 2);
		if (accelfuncs & ACCELFLAG_DRAWHLINELIST) {
		    vga_accel(ACCEL_SETRASTEROP, i);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 255));
		    drawdisk(x + x1, y + 1, wsq, xmin, xmax);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 255));
		    drawdisk(x + x2, y + 1, wsq, xmin, xmax);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 0));
		    drawdisk(x + xo, y + yo, wsq, xmin, xmax);
		} else {
		    vga_accel(ACCEL_SETRASTEROP, i);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 255));
		    vga_accel(ACCEL_FILLBOX, x + x1, y + 1, wsq, wsq);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 255));
		    vga_accel(ACCEL_FILLBOX, x + x2, y + 1, wsq, wsq);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 255, 0));
		    vga_accel(ACCEL_FILLBOX, x + xo, y + yo, wsq, wsq);
		}
	    } else {
		if (accelfuncs & ACCELFLAG_DRAWHLINELIST) {
		    vga_accel(ACCEL_SETRASTEROP, i);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
		    drawdisk(x + x1, y + 1, wsq, xmin, xmax);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
		    drawdisk(x + x2, y + 1, wsq, xmin, xmax);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 255));
		    drawdisk(x + xo, y + yo, wsq, xmin, xmax);
		} else {
		    vga_accel(ACCEL_SETRASTEROP, i);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(255, 0, 0));
		    vga_accel(ACCEL_FILLBOX, x + x1, y + 1, wsq, wsq);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 255, 0));
		    vga_accel(ACCEL_FILLBOX, x + x2, y + 1, wsq, wsq);
		    vga_accel(ACCEL_SETFGCOLOR, gl_rgbcolor(0, 0, 255));
		    vga_accel(ACCEL_FILLBOX, x + xo, y + yo, wsq, wsq);
		}
	    }
	    vga_accel(ACCEL_SETRASTEROP, ROP_COPY);
	    gl_write(x + 1, y + h + 3, RopNames[i]);
	  nextrop:
	    i++;
	    if (i >= (sizeof(RopNames) / sizeof(char *)))
		goto endtest;
	}

  endtest:
    vga_getch();
}

void QixDemo(int rop, char *txt) {
    int current = 0, dx1, dy1, dx2, dy2, i, cols = 1, cx1, cx2, cy1, cy2;
    int startclock, diffclock;
    unsigned long pixels = 0, numlines = 0;
    struct {
	unsigned short x1;	
	unsigned short y1;	
	unsigned short x2;	
	unsigned short y2;	
	int col;
	int pix;
    } lines[QIXLINES];

    if ( !(ropmodes & (1 << rop)) ||
	 !(accelfuncs & ACCELFLAG_DRAWLINE))
		return;
    if ((rop != ROP_COPY) &&
	 !(accelfuncs & ACCELFLAG_SETRASTEROP))
		return;
    DrawDots();
    gl_write(1, 1, txt);
    memset(lines, 0, sizeof(lines));
    cx1 = rand() % WIDTH;
    cy1 = rand() % HEIGHT;
    cx2 = rand() % WIDTH;
    cy2 = rand() % HEIGHT;
    dx1 = 5 - rand() % 10;
    dy1 = 5 - rand() % 10;
    dx2 = 5 - rand() % 10;
    dy2 = 5 - rand() % 10;
    if (rop != ROP_COPY)
	vga_accel(ACCEL_SETRASTEROP, rop);
    startclock = clock();
    for(i = 0; i < 10000; i++) {
	diffclock = clock() - startclock;
	if (diffclock > 5 * CLOCKS_PER_SEC)
	    break;
	if (lines[current].col) {
	    if (rop != ROP_COPY)
		vga_accel(ACCEL_SETFGCOLOR, lines[current].col);
	    else
		vga_accel(ACCEL_SETFGCOLOR, 0);
	    vga_accel(ACCEL_DRAWLINE, lines[current].x1, lines[current].y1,
		      lines[current].x2, lines[current].y2);
	    numlines++;
	    pixels += lines[current].pix;
	}
	cx1 += dx1;
	if ((cx1 < 0) || (cx1 >= WIDTH)) {
	    dx1 = -dx1;
	    cx1 += dx1 << 1;
	}
	cx2 += dx2;
	if ((cx2 < 0) || (cx2 >= WIDTH)) {
	    dx2 = -dx2;
	    cx2 += dx2 << 1;
	}
	cy1 += dy1;
	if ((cy1 < 0) || (cy1 >= HEIGHT)) {
	    dy1 = -dy1;
	    cy1 += dy1 << 1;
	}
	cy2 += dy2;
	if ((cy2 < 0) || (cy2 >= HEIGHT)) {
	    dy2 = -dy2;
	    cy2 += dy2 << 1;
	}
	lines[current].x1 = cx1;
	lines[current].x2 = cx2;
	lines[current].y1 = cy1;
	lines[current].y2 = cy2;
	lines[current].col = cols;
	vga_accel(ACCEL_SETFGCOLOR, cols++);
	if (cols >= COLORS)
	    cols = 1;
	vga_accel(ACCEL_DRAWLINE, lines[current].x1, lines[current].y1,
		  lines[current].x2, lines[current].y2);
	numlines++;
	pixels += lines[current].pix = max(abs(lines[current].x1 - lines[current].x2),
					   abs(lines[current].y1 - lines[current].y2));
	current++;
	if (current >= QIXLINES)
	    current = 0;
    }
    if (rop != ROP_COPY)
	vga_accel(ACCEL_SETRASTEROP, ROP_COPY);
    cy1 = ((long long) numlines * CLOCKS_PER_SEC) / diffclock;
    cx1 = ((long long) pixels * CLOCKS_PER_SEC) / diffclock;
    cx2 = cx1 * BYTESPERPIXEL;
    printf("%s: %ld.%ld Klines/s (%ld.%ld Mpixels/s or %ld.%ld Mbytes/s)\n", txt,
	   cy1 / 1000L, (cy1 % 1000L) / 100L,
	   cx1 / 1000000L, (cx1 % 1000000L) / 100000L,
	   cx2 / 1000000L, (cx2 % 1000000L) / 100000L);
}
