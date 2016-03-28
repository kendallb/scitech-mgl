/* Program to test all accelerator functions and the 8 bit
   per color color lookup functions. */
/* written by Michael Weller (eowmob@exp-math.uni-essen.de) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "vga.h"
#include "clut.xbm"
#include "sixbpp.xbm"
#include "eightbpp.xbm"
#include "fish_monster.h"

#define RED(i)		(0)
#define GREEN(i)	(0)
#define BLUE(i)		(i)

#define LOGO_NAME	"linuxlogo.bmp"
#define LOGO_WIDTH	201
#define LOGO_HEIGHT	85

#undef BITFRAME

typedef struct {
    int n;
    int pels;
    int width;
    int offset;
    /*
       int xmin[n];
       int xmax[n]; */
} hlinelst;

/*Adjust hlist for new xcoord */
inline void adj_hlilst(hlinelst * lst, register int x)
{
    register int i, *ptr;

    i = x;
    x -= lst->offset;
    lst->offset = i;

    i = (lst->n) << 1;
    ptr = (int *) (((char *) lst) + sizeof(hlinelst));

    while (i--)
	*ptr++ += x;
}

inline int sizhlilst(int n)
{
    return sizeof(hlinelst) + (sizeof(int) * (n < 1 ? 2 : (n << 1)));
}

inline int isqr(int i)
{
    return i * i;
}

int waitmode = 1;

void my_wait(void)
{
    if (waitmode)
	vga_getch();
    else
	sleep(1);
}

void setcol(vga_modeinfo * modeinfo)
{
    int i;

    if (modeinfo->colors == 256)
	for (i = 0; i < 256; i++)
	    vga_setpalette(i, (i & 7) * 0x9, ((i >> 3) & 7) * 0x9, ((i >> 6) & 3) * 0x15);
    else if (modeinfo->colors < 256)
	for (i = 0; i < 16; i++)
	    vga_setegacolor(i);
}

int colorcol(int column, int width, int xmax)
{
    int color;

    color = ((column << 8) - column) / xmax;
    if (width != 8)
	color &= 0xfc;
    return color;
}

void drawtwidth(vga_modeinfo * modeinfo, char *bits, int x, int y, int tw, int th, int width)
{
    unsigned char color;
    int i, j, k, xmax, offset;

    tw = (tw + 7) >> 3;
    th <<= 1;
    th += x;
    xmax = vga_getxdim() - 1;
    y *= modeinfo->linewidth;

    for (i = x; i < th; i++) {
	color = colorcol(xmax - i, width, xmax);
	offset = y + i;

	if ((i - x) & 1)
	    bits -= tw;
	j = tw;
	while (j--) {
	    k = 1;
	    do {
		if (*bits & k) {
		    vga_setpage(offset >> 16);
		    graph_mem[offset & 0xffff] = color;
		}
		offset += modeinfo->linewidth;
		if (*bits & k) {
		    vga_setpage(offset >> 16);
		    graph_mem[offset & 0xffff] = color;
		}
		offset += modeinfo->linewidth;
		k <<= 1;
	    }
	    while (k & 0xff);
	    bits++;
	}
    }
}

void drawwidth(int mode, vga_modeinfo * modeinfo, int line, int height, int width)
{
    unsigned char color;
    int i, xmax, j, offset;

    xmax = vga_getxdim() - 1;

    for (i = 0; i < xmax; i++) {
	color = colorcol(i, width, xmax);
	offset = line * (modeinfo->linewidth) + i;

	j = height;
	while (j--) {
	    vga_setpage(offset >> 16);
	    graph_mem[offset & 0xffff] = color;
	    offset += modeinfo->linewidth;
	}
    }

    j = clut_width + sixbpp_width;
    j = height - j - j;
    j /= 3;

    drawtwidth(modeinfo, clut_bits, (xmax - 2 * clut_height) >> 1, j + line, clut_width, clut_height, width);
    j += j + clut_width;
    if (width == 6)
	drawtwidth(modeinfo, sixbpp_bits, (xmax - 2 * sixbpp_height) >> 1, j + line, sixbpp_width, sixbpp_height, width);
    else
	drawtwidth(modeinfo, eightbpp_bits, (xmax - 2 * eightbpp_height) >> 1, j + line, eightbpp_width, eightbpp_height, width);
}

void testwidth(int mode, vga_modeinfo * modeinfo)
{
    int ymax, i, old_flags;

    ymax = vga_getydim();

    if (vga_getxdim() < 640)
	puts("  Warning: Resolution too small, displayed text is\n"
	     "           probably scrambled.\n");
    if (vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_FLAGS) & VGA_CLUT8) {
	old_flags = vga_ext_set(VGA_EXT_SET, VGA_CLUT8);

	for (i = 0; i < 256; i++)
	    vga_setpalette(i, RED(i), GREEN(i), BLUE(i));

	puts("  Has support for CLUT width 8 bit");
	drawwidth(mode, modeinfo, 0, ymax >> 1, 6);
	drawwidth(mode, modeinfo, ymax >> 1, ymax >> 1, 8);
	my_wait();
	vga_clear();
	vga_ext_set(VGA_EXT_RESET, old_flags);
    } else {

	for (i = 0; i < 256; i++)
	    vga_setpalette(i, RED(i) >> 2, GREEN(i) >> 2, BLUE(i) >> 2);

	puts("  No support for CLUT width 8 bit");
	drawwidth(mode, modeinfo, 0, ymax, 6);
	my_wait();
    }
}

void testfill(vga_modeinfo * modeinfo)
{
    clock_t clk;
    unsigned long pels = 0;
    int xmax, ymax, lw, i, start, lim, x, y, w, h, mask;

    lw = modeinfo->linewidth;
    setcol(modeinfo);
    vga_clear();

    ymax = vga_getydim();
    xmax = vga_getxdim();

    srand(time(0));
    clk = clock();
    mask = modeinfo->colors - 1;
    lim = ((xmax < ymax) ? xmax : ymax) >> 1;

    for (i = 0, start = 0; i < lim; i++, start += lw + modeinfo->bytesperpixel) {
	vga_fillblt(start, xmax - (i << 1), ymax - (i << 1), lw, mask & i);
	pels += (xmax - (i << 1)) * (ymax - (i << 1));
    }
    for (i = 0; i < 10000; i++) {
	x = rand() % xmax;
	y = rand() % ymax;
	w = rand() % (xmax - x);
	h = rand() % (ymax - y);
	switch (modeinfo->bytesperpixel) {
	case 2:
	    x <<= 1;
	    break;
	case 3:
	    x += (x << 1);
	    break;
	case 4:
	    x <<= 2;
	    break;
	}

	vga_fillblt(x + y * lw, w, h, lw, mask & rand());
	pels += w * h;
    }
    clk = clock() - clk;
    printf("  Has FillBlt: %11lu Pixels in %.2f seconds -> %.2f Megapels\n",
	   pels, ((double) clk) / CLOCKS_PER_SEC, (pels / 1.0e6) / (((double) clk) / CLOCKS_PER_SEC));
    my_wait();
}

void getlogo(void *blitimage, vga_modeinfo * modeinfo)
{
    int i;
    FILE *fd;

    fd = fopen(LOGO_NAME, "r");
    if (!fd) {
      read_err:
	perror("Problems reading linuxlogo.bmp");
	exit(2);
    }
    if (1 != fread(blitimage, LOGO_WIDTH * LOGO_HEIGHT, 1, fd))
	goto read_err;
    fclose(fd);
    switch (modeinfo->bytesperpixel) {
	/* Nothing todo for 256 colors! */
    case 2:
	{
	    unsigned char *bmp = (void *) blitimage;
	    unsigned short *bmpsh = (void *) blitimage;
	    bmp += LOGO_WIDTH * LOGO_HEIGHT;
	    bmpsh += LOGO_WIDTH * LOGO_HEIGHT;
	    i = LOGO_WIDTH * LOGO_HEIGHT;

	    if (modeinfo->colors == 32768) {
		while (i--) {
		    bmp--;
		    *--bmpsh = ((((unsigned short) *bmp) & 7) << 12) |
			((((unsigned short) *bmp) & 0x38) << 4) |
			((((unsigned short) *bmp) & 0xc0) >> 3);
		}
	    } else {
		while (i--) {
		    bmp--;
		    *--bmpsh = ((((unsigned short) *bmp) & 7) << 13) |
			((((unsigned short) *bmp) & 0x38) << 5) |
			((((unsigned short) *bmp) & 0xc0) >> 3);
		}
	    }
	}
	break;
    case 3:
	{
	    unsigned char *bmp = (void *) blitimage;
	    unsigned char *bmp3 = (void *) blitimage;
	    bmp += LOGO_WIDTH * LOGO_HEIGHT;
	    bmp3 += 3 * LOGO_WIDTH * LOGO_HEIGHT;
	    i = LOGO_WIDTH * LOGO_HEIGHT;

	    while (i--) {
		bmp--;
		*--bmp3 = ((((unsigned int) *bmp) & 7) << 5);
		*--bmp3 = ((((unsigned int) *bmp) & 0x38) << 2);
		*--bmp3 = ((((unsigned int) *bmp) & 0xc0));
	    }
	}
	break;
    case 4:
	{
	    unsigned char *bmp = (void *) blitimage;
	    unsigned int *bmpi = (void *) blitimage;
	    bmp += LOGO_WIDTH * LOGO_HEIGHT;
	    bmpi += LOGO_WIDTH * LOGO_HEIGHT;
	    i = LOGO_WIDTH * LOGO_HEIGHT;

	    if (modeinfo->flags & RGB_MISORDERED) {
		while (i--) {
		    bmp--;
		    *--bmpi = ((((unsigned int) *bmp) & 7) << 29) |
			((((unsigned int) *bmp) & 0x38) << 18) |
			((((unsigned int) *bmp) & 0xc0) << 8);
		}
	    } else {
		while (i--) {
		    bmp--;
		    *--bmpi = ((((unsigned int) *bmp) & 7) << 21) |
			((((unsigned int) *bmp) & 0x38) << 10) |
			((((unsigned int) *bmp) & 0xc0));
		}
	    }
	}
	break;
    }
}

void testbit(vga_modeinfo * modeinfo)
{
    clock_t clk;
    unsigned long pels = 0, *blitimage;
    int xmax, ymax, lw, i, x, y, dx, dy, nx, ny;

    if ((modeinfo->bytesperpixel != 1) && (modeinfo->bytesperpixel != 2)) {
	puts("  Has BitBlt, but no test code for this pixelwidth implemented.");
	return;
    }
    ymax = vga_getydim();
    xmax = vga_getxdim();
    lw = modeinfo->linewidth;

    if ((xmax < 210) || (ymax < 90)) {
	puts("  Has BitBlt, but no test code for this small resolution implemented.");
	return;
    }
    setcol(modeinfo);
    vga_clear();

    pels = 0;
    blitimage = alloca(modeinfo->bytesperpixel * LOGO_WIDTH * LOGO_HEIGHT);
    getlogo(blitimage, modeinfo);

    if (modeinfo->bytesperpixel == 1) {
	unsigned char *ptr = (void *) blitimage;
	for (y = 1; y <= LOGO_HEIGHT; y++)
	    for (x = 1; x <= LOGO_WIDTH; x++) {
		vga_setcolor((int) *ptr++);
		vga_drawpixel(x, y);
	    }
    } else {
	unsigned short *ptr = (void *) blitimage;
	for (y = 1; y <= LOGO_HEIGHT; y++)
	    for (x = 1; x <= LOGO_WIDTH; x++) {
		vga_setcolor((int) *ptr++);
		vga_drawpixel(x, y);
	    }
    }
#ifdef BITFRAME
    vga_setcolor(200);
    vga_drawline(0, 0, 0, LOGO_HEIGHT + 1);
    vga_drawline(0, 0, LOGO_WIDTH + 1, 0);
    vga_drawline(LOGO_WIDTH + 1, 0, LOGO_WIDTH + 1, LOGO_HEIGHT + 1);
    vga_drawline(0, LOGO_HEIGHT + 1, LOGO_WIDTH + 1, LOGO_HEIGHT + 1);
#endif

    x = 0;
    y = 0;
    dx = 1;
    dy = 1;
    clk = clock();

    for (i = 0; i < 10000; i++) {
	nx = x + dx;
	ny = y + dy;
	if ((nx + LOGO_WIDTH + 2 > xmax) || (nx < 0)) {
	    dx = -dx;
	    nx = x + dx;
	}
	if ((ny + LOGO_HEIGHT + 2 > ymax) || (ny < 0)) {
	    dy = -dy;
	    ny = y + dy;
	}
	if (modeinfo->bytesperpixel == 2) {
	    vga_bitblt((x + y * (lw >> 1)) << 1,
		       (nx + ny * (lw >> 1)) << 1,
		       LOGO_WIDTH + 2, LOGO_HEIGHT + 2, lw);
	} else {
	    vga_bitblt(x + y * lw, nx + ny * lw, LOGO_WIDTH + 2, LOGO_HEIGHT + 2, lw);
	}

	pels += (LOGO_WIDTH + 2) * (LOGO_HEIGHT + 2);
	x = nx;
	y = ny;
    }

    clk = clock() - clk;
    printf("  Has BitBlt: %12lu Pixels in %.2f seconds -> %.2f Megapels\n",
	   pels, ((double) clk) / CLOCKS_PER_SEC, (pels / 1.0e6) / (((double) clk) / CLOCKS_PER_SEC));
    my_wait();
}

void testimage(vga_modeinfo * modeinfo)
{
    clock_t clk;
    unsigned long pels = 0, *blitimage;
    int xmax, ymax, lw, i, x, y;

    if ((modeinfo->bytesperpixel < 1) || (modeinfo->bytesperpixel > 4)) {
	puts("  Has ImageBlt, but no test code for this pixelwidth implemented.");
	return;
    }
    ymax = vga_getydim();
    xmax = vga_getxdim();
    lw = modeinfo->linewidth;

    if ((xmax < 210) || (ymax < 90)) {
	puts("  Has ImageBlt, but no test code for this small resolution implemented.");
	return;
    }
/* Prepare a simple test pattern for testuse: */
    switch (modeinfo->bytesperpixel) {
    default:
	{
	    /* 8bpp: */
	    register unsigned char *ptr, *pt_fish = fish_monster;

	    blitimage = alloca(fish_monster_w * fish_monster_h);
	    ptr = (unsigned char *) blitimage;

	    for (i = 0; i < (fish_monster_w * fish_monster_h); i++)
		*ptr++ = fish_monster233[(*pt_fish++) - '`'];
	}
	break;
    case 2:
	{
	    register unsigned char *pt_fish = fish_monster;
	    register unsigned short *ptr, *coltab;

	    blitimage = alloca(sizeof(unsigned short) * fish_monster_w * fish_monster_h);
	    ptr = (unsigned short *) blitimage;

	    if (modeinfo->colors == 32768)
		coltab = fish_monster555;
	    else
		coltab = fish_monster565;
	    for (i = 0; i < (fish_monster_w * fish_monster_h); i++)
		*ptr++ = coltab[(*pt_fish++) - '`'];
	}
	break;
    case 3:
	{
	    register unsigned char *pt_fish = fish_monster;
	    register unsigned char *ptr, *coltab;

	    blitimage = alloca(3 * fish_monster_w * fish_monster_h);
	    ptr = (unsigned char *) blitimage;

	    coltab = (unsigned char *) fish_monster888;

	    for (i = 0; i < (fish_monster_w * fish_monster_h); i++) {
		*ptr++ = coltab[(((*pt_fish) - '`') << 2)];
		*ptr++ = coltab[(((*pt_fish) - '`') << 2) + 1];
		*ptr++ = coltab[(((*pt_fish++) - '`') << 2) + 2];
	    }
	}
	break;
    case 4:
	{
	    register unsigned char *pt_fish = fish_monster;
	    register unsigned int *ptr, *coltab;

	    blitimage = alloca(sizeof(unsigned int) * fish_monster_w * fish_monster_h);
	    ptr = (unsigned int *) blitimage;

	    coltab = fish_monster888;
	    if (modeinfo->flags & RGB_MISORDERED) {
		for (i = 0; i < (fish_monster_w * fish_monster_h); i++)
		    *ptr++ = (coltab[(*pt_fish++) - '`'] << 8);
	    } else {
		for (i = 0; i < (fish_monster_w * fish_monster_h); i++)
		    *ptr++ = coltab[(*pt_fish++) - '`'];
	    }
	}
	break;
    }
    setcol(modeinfo);
    vga_clear();

    srand(time(0));
    for (i = 0; i < 10000; i++) {
	x = rand() % (xmax - fish_monster_w);
	y = rand() % (ymax - fish_monster_h);
	if (modeinfo->bytesperpixel > 1)
	    x *= modeinfo->bytesperpixel;

	vga_imageblt(blitimage, x + y * lw, fish_monster_w, fish_monster_h, lw);
	pels += fish_monster_w * fish_monster_h;
    }
    pels = 0;
    blitimage = alloca(modeinfo->bytesperpixel * LOGO_WIDTH * LOGO_HEIGHT);
    getlogo(blitimage, modeinfo);

    clk = clock();

    for (i = 0; i < 1000; i++) {
	x = rand() % (xmax - LOGO_WIDTH);
	y = rand() % (ymax - LOGO_HEIGHT);
	if (modeinfo->bytesperpixel > 1)
	    x *= modeinfo->bytesperpixel;

	vga_imageblt(blitimage, x + y * lw, LOGO_WIDTH, LOGO_HEIGHT, lw);
	pels += LOGO_WIDTH * LOGO_HEIGHT;
    }

    clk = clock() - clk;
    printf("  Has ImageBlt: %10lu Pixels in %.2f seconds -> %.2f Megapels\n",
	   pels, ((double) clk) / CLOCKS_PER_SEC, (pels / 1.0e6) / (((double) clk) / CLOCKS_PER_SEC));
    my_wait();
}

void hlinesquare(hlinelst * des, register int n)
{
    register int *xmin, *xmax;

    if (n < 1)
	n = 1;

    des->n = n;
    des->width = n;
    des->offset = 0;
    des->pels = n * n;

    xmin = (int *) ((char *) des + sizeof(hlinelst));
    xmax = xmin + n;

    while (n--) {
	*xmin++ = 0;
	*xmax++ = des->width - 1;
    }
}

void hlinedisk(hlinelst * des, register int n)
{
    register int *xmin, *xmax, radsq, rad, x;

    if (!(n & 1))
	n--;
    if (n < 1)
	n = 1;

    des->n = n;
    des->width = n;
    des->offset = 0;
    des->pels = n * n;
    rad = (n >> 1);
    radsq = rad * rad;

    xmin = (int *) ((char *) des + sizeof(hlinelst));
    xmax = xmin + n;

    while (n--) {
	x = sqrt(radsq - isqr(n - rad));
	*xmin++ = rad - x;
	*xmax++ = rad + x;
    }
}

void hlinecaro(hlinelst * des, register int n)
{
    register int *xmin, *xmax, i, j;

    if (!(n & 1))
	n--;
    if (n < 1)
	n = 1;

    des->n = n;
    des->width = n;
    des->offset = 0;
    des->pels = (n * n) / 2;

    xmin = (int *) ((char *) des + sizeof(hlinelst));
    xmax = xmin + n;

    i = 1 + (n >> 1);
    j = 0;

    while (i--) {
	*xmin++ = (n >> 1) - j;
	*xmax++ = (n >> 1) + j;
	j++;
    }
    i = (n >> 1);
    j -= 2;
    while (i--) {
	*xmin++ = (n >> 1) - j;
	*xmax++ = (n >> 1) + j;
	j--;
    }
}

void testhline(vga_modeinfo * modeinfo)
{
#define SHAPES 9
    clock_t clk;
    unsigned long pels = 0;
    hlinelst *shape[SHAPES], *curs;
    int xmax, ymax, lw, i, x, y, mask;

    setcol(modeinfo);
    vga_clear();

    ymax = vga_getydim();
    xmax = vga_getxdim();
    lw = modeinfo->linewidth;
    mask = modeinfo->colors - 1;

    srand(time(0));

    i = 0;
    shape[i] = alloca(sizhlilst(ymax / 2));
    hlinesquare(shape[i++], ymax / 2);
    shape[i] = alloca(sizhlilst(ymax / 4));
    hlinesquare(shape[i++], ymax / 4);
    shape[i] = alloca(sizhlilst(ymax / 8));
    hlinesquare(shape[i++], ymax / 8);

    shape[i] = alloca(sizhlilst(ymax / 2));
    hlinecaro(shape[i++], ymax / 2);
    shape[i] = alloca(sizhlilst(ymax / 4));
    hlinecaro(shape[i++], ymax / 4);
    shape[i] = alloca(sizhlilst(ymax / 8));
    hlinecaro(shape[i++], ymax / 8);

    shape[i] = alloca(sizhlilst(ymax / 2));
    hlinedisk(shape[i++], ymax / 2);
    shape[i] = alloca(sizhlilst(ymax / 4));
    hlinedisk(shape[i++], ymax / 4);
    shape[i] = alloca(sizhlilst(ymax / 8));
    hlinedisk(shape[i++], ymax / 8);


    clk = clock();

    for (i = 0; i < 1000; i++) {
	curs = shape[rand() % SHAPES];
	x = rand() % (xmax - (curs->width));
	y = rand() % (ymax - (curs->n));
	adj_hlilst(curs, x);
	pels += curs->pels;
	vga_hlinelistblt(y, curs->n,
			 (int *) (((char *) curs) + sizeof(hlinelst)),
			 (int *) (((char *) curs) + sizeof(hlinelst) + (curs->n) * sizeof(int)),
			 lw, mask & rand());
    }
    clk = clock() - clk;
    clk++;
    printf("  Has HlineLst: %10lu Pixels in %.2f seconds -> %.2f Megapels\n",
	   pels, ((double) clk) / CLOCKS_PER_SEC, (pels / 1.0e6) / (((double) clk) / CLOCKS_PER_SEC));
    my_wait();
}

void testmode(int mode)
{
    vga_modeinfo *modeinfo;

    printf("Testing mode %2d: %s...", mode, vga_getmodename(mode));
    if (!vga_hasmode(mode)) {
	puts(" not available");
	return;
    }
    puts("");

    vga_setmode(mode);
    modeinfo = vga_getmodeinfo(mode);

    if ((modeinfo->colors == 256) && !(modeinfo->flags & IS_MODEX)) {
	testwidth(mode, modeinfo);
    } else
	puts("  Dacwidth test not applicable.");
    if (modeinfo->haveblit & HAVE_BITBLIT)
	testbit(modeinfo);
    if (modeinfo->haveblit & HAVE_FILLBLIT)
	testfill(modeinfo);
    if (modeinfo->haveblit & HAVE_IMAGEBLIT)
	testimage(modeinfo);
    if (modeinfo->haveblit & HAVE_HLINELISTBLIT)
	testhline(modeinfo);
    vga_setmode(TEXT);
}

void usage(void)
{
    puts("Usage: testaccel [-nowait] {all|[mode [mode [mode...]]]}\n"
	 "\ttests accelerated features and extended options in\n"
	 "\tall or the given modes.\n"
     "\tAll standard svgalib ways of writing modenames are understood.\n"
      "\tIf no parameters are given the defaultmode or G640x480x256 is\n"
	 "\tused.\n"
    "\tIf -nowait is given, don't wait for a keypress after each test.");
    exit(2);
}

int main(int argc, char *argv[])
{
    int mode;

    vga_init();

    puts(
	    "Note: Timings include main cpu calculations (random numbers and such\n"
	    "      things). Thus they are esp. for lower resolutions much too slow!");

    if ((argc > 1) && !strcmp(argv[1], "-nowait")) {
	argc--;
	argv++;
	waitmode = 0;
    }
    if ((argc == 2) && !strcmp(argv[1], "all")) {
	int flag = 0;
	for (mode = 1; mode <= GLASTMODE; mode++)
	    if (vga_hasmode(mode)) {
		flag = 1;
		testmode(mode);
	    }
	if (!flag)
	    puts("testaccel: Not any graphicsmode available!");
    } else if (argc > 1) {
	for (mode = 1; mode < argc; mode++)
	    if (vga_getmodenumber(argv[mode]) < 0) {
		printf("testaccel: Parameter %s is not a valid graphicsmode.\n", argv[mode]);
		usage();
	    }
	for (mode = 1; mode < argc; mode++)
	    testmode(vga_getmodenumber(argv[mode]));
    } else {
	mode = vga_getdefaultmode();
	if (mode < 0)
	    mode = G640x480x256;
	if (vga_hasmode(mode))
	    testmode(mode);
	else
	    puts("testaccel: could not set defaultmode or G640x480x256!");
    }
    return 0;
}
