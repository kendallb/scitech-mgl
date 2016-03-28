/*
   Animation/smooth scrolling demo, uses Mode X/page flipping and
   linear virtual screen in system memory.

   First method copies window to page-flipped Mode X-style video memory
   for scrolling.
   Linear addressing, and page flipping (no shearing) and triple
   buffering, but relatively slow.

   Second method uses Mode X-style hardware scrolling.
   Limited logical screen size, page flipping if no scrolling, tricky and
   slow animation, scrolling fast (page flipping with scrolling is very
   tricky).

   Third method copies window to linear "Mode 13h" video memory.
   Linear addressing, no page flipping (scrolling looks bad), speed
   depends      on bus (fast on a good ISA card; very fast on VLB).
   Some SVGA cards can support page-flipping in linear 320x200x256.

   Adding animated objects is trivial with the virtual screen methods (1
   and 3).
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>


/* Virtual screen size. */
/* Limited to 256K by hardware scrolling method (automatically clipped). */
/* Width must be multiple of 8. */
#define VWIDTH 640
#define VHEIGHT 400

/* Scrolling window size for system memory virtual screen demo. */
#define WINWIDTH 320
#define WINHEIGHT 200

/* Define this to use triple-buffering in first method. */
/* #define TRIPLEBUFFERING */


unsigned char *vbuf;


void boxes(void)
{
    int x, y;
    for (x = 0; x <= VWIDTH - 8; x += 8)
	for (y = 0; y <= VHEIGHT - 8; y += 8) {
	    int r, g, b;
	    /* Draw red tiles. */
	    r = rand() & 255;
	    b = 0;
	    g = 0;
	    if ((rand() & 15) == 15) {	/* Add occasional */
		r = 0;		/* blue specks. */
		b = rand() & 127;
	    }
	    gl_fillbox(x, y, 7, 7, gl_rgbcolor(r, g, b));
	}
}


void demo1(void)
{
    int x, y;
    int targetx, targety;
    int pageoffset[3] =
    {
	0,
	320 * 240 / 4,
	2 * 320 * 240 / 4
    };
    int writepage;
    int count, startclock;

    /* Window coordinate initially at center. */
    x = VWIDTH / 2 - WINWIDTH / 2;
    y = VHEIGHT / 2 - WINHEIGHT / 2;
    targetx = x;
    targety = y;

    /* Page flipping initialization. */
    vga_setdisplaystart(0);	/* Display page 0, write to page 1. */
    writepage = 1;

    count = 0;
    startclock = clock();

    for (;;) {
	/* Copy window to screen. */
	vga_copytoplanar256(vbuf + y * WIDTH + x, WIDTH,
			 pageoffset[writepage], 80, WINWIDTH, WINHEIGHT);

	/* Flip pages. */
	vga_setdisplaystart(pageoffset[writepage] * 4);

#ifndef TRIPLEBUFFERING
	/* Conventional double-buffering (page-flipping). */
	vga_waitretrace();
	writepage ^= 1;
#else
	/* Triple buffering; no need to wait for vertical retrace. */
	writepage = (writepage + 1) % 3;
#endif

	if (x == targetx && y == targety) {
	    /* Create new target. */
	    targetx = rand() % (VWIDTH - WINWIDTH);
	    targety = rand() % (VHEIGHT - WINHEIGHT);
	}
	/* Move towards target. */
	if (x < targetx)
	    x++;
	if (x > targetx)
	    x--;
	if (y < targety)
	    y++;
	if (y > targety)
	    y--;

	/* Boundary checks. */
	if (x < 0)
	    x = 0;
	if (x > VWIDTH - WINWIDTH)
	    x = VWIDTH - WINWIDTH;
	if (y < 0)
	    y = 0;
	if (y > VHEIGHT - WINHEIGHT)
	    y = VHEIGHT - WINHEIGHT;

	if (vga_getkey())
	    break;

	count++;
    }

    printf("Method 1: frame rate %ld\n", count * 1000L
	   / (clock() - startclock));
}


void demo2(void)
{
    int x, y;
    int targetx, targety;
    int vwidth, vheight;
    int count, startclock;

    /* Make sure window fits in video memory. */
    vwidth = VWIDTH;
    if (vwidth > 640)
	vwidth = 640;
    vheight = VHEIGHT;
    if (vheight > 400)
	vheight = 400;

    vga_setlogicalwidth(vwidth);

    /* Copy virtual screen to logical screen in video memory. */
    vga_copytoplanar256(vbuf, VWIDTH, 0, vwidth / 4,
			vwidth, VHEIGHT);

    /* Window coordinates initially at center. */
    x = vwidth / 2 - WINWIDTH / 2;
    y = vheight / 2 - WINHEIGHT / 2;
    targetx = x;
    targety = y;
    count = 0;
    startclock = clock();

    for (;;) {
	/* Set video memory window. */
	vga_setdisplaystart((y * vwidth / 4) * 4 + x);
	vga_waitretrace();

	if (x == targetx && y == targety) {
	    /* Create new target. */
	    targetx = rand() % (vwidth - WINWIDTH);
	    targety = rand() % (vheight - WINHEIGHT);
	}
	/* Move towards target. */
	if (x < targetx)
	    x++;
	if (x > targetx)
	    x--;
	if (y < targety)
	    y++;
	if (y > targety)
	    y--;

	/* Boundary checks. */
	if (x < 0)
	    x = 0;
	if (x > vwidth - WINWIDTH)
	    x = vwidth - WINWIDTH;
	if (y < 0)
	    y = 0;
	if (y > vheight - WINHEIGHT)
	    y = vheight - WINHEIGHT;

	if (vga_getkey())
	    break;

	count++;
    }
    printf("Method 2: frame rate %ld\n", count*1000L
	   /(clock() - startclock));
}


void demo3(void)
{
    int x, y;
    int targetx, targety;
    int count, startclock;
    GraphicsContext *virtualscreen;
    GraphicsContext *physicalscreen;

    /* Window coordinate initially at center. */
    x = VWIDTH / 2 - WINWIDTH / 2;
    y = VHEIGHT / 2 - WINHEIGHT / 2;
    targetx = x;
    targety = y;

    virtualscreen = gl_allocatecontext();
    gl_getcontext(virtualscreen);
    gl_setcontextvga(G320x200x256);
    physicalscreen = gl_allocatecontext();
    gl_getcontext(physicalscreen);
    gl_setcontext(virtualscreen);

    count = 0;
    startclock = clock();

    for (;;) {
        vga_waitretrace();
	/* Copy window to screen. */
	gl_copyboxtocontext(x, y, WINWIDTH, WINHEIGHT, physicalscreen,
			    0, 0);

	if (x == targetx && y == targety) {
	    /* Create new target. */
	    targetx = rand() % (VWIDTH - WINWIDTH);
	    targety = rand() % (VHEIGHT - WINHEIGHT);
	}
	/* Move towards target. */
	if (x < targetx)
	    x++;
	if (x > targetx)
	    x--;
	if (y < targety)
	    y++;
	if (y > targety)
	    y--;

	/* Boundary checks. */
	if (x < 0)
	    x = 0;
	if (x > VWIDTH - WINWIDTH)
	    x = VWIDTH - WINWIDTH;
	if (y < 0)
	    y = 0;
	if (y > VHEIGHT - WINHEIGHT)
	    y = VHEIGHT - WINHEIGHT;

	if (vga_getkey())
	    break;

	count++;
    }

    printf("Method 3: frame rate %ld\n", count * 1000L
	   / (clock() - startclock));
}


void main(void)
{
    vga_init();

    /* Create virtual screen. */
    vbuf = malloc(VWIDTH * VHEIGHT);
    gl_setcontextvirtual(VWIDTH, VHEIGHT, 1, 8, vbuf);

    /* Set Mode X-style 320x240x256. */
    vga_setmode(G320x240x256);
    gl_setrgbpalette();
    vga_clear();

    boxes();

    demo1();

    demo2();

    vga_setmode(G320x200x256);	/* Set linear 320x200x256. */
    gl_setrgbpalette();

    demo3();

    vga_setmode(TEXT);
    exit(0);
}
