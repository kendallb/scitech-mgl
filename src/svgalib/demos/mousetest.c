/* Program to test the svgalib mouse functions. */

#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <vgakeyboard.h>

/* Manually open and close mouse? */
#define MANUALLY_SETUP_MOUSE_NOT


static int newcolor(void)
{
    if (BYTESPERPIXEL == 1)
	return random() % 15 + 1;
    return gl_rgbcolor(random() & 255, random() & 255, random() & 255);
}


void main(void)
{
    int vgamode, color, leftpressed;
    int x, y, button;
    vga_init();
    vgamode = vga_getdefaultmode();
    if (vgamode == -1)
	vgamode = G320x200x256;

    if (!vga_hasmode(vgamode)) {
	printf("Mode not available.\n");
	exit(-1);
    }
#ifndef MANUALLY_SETUP_MOUSE
    /* Enable automatic mouse setup at mode set. */
    vga_setmousesupport(1);
#endif
    vga_setmode(vgamode);
    /* Disable wrapping (default). */
    /* mouse_setwrap(MOUSE_NOWRAP); */
    gl_setcontextvga(vgamode);
    gl_enableclipping();

#ifdef MANUALLY_SETUP_MOUSE
    mouse_init("/dev/mouse", MOUSE_MICROSOFT, MOUSE_DEFAULTSAMPLERATE);
    mouse_setxrange(0, WIDTH - 1);
    mouse_setyrange(0, HEIGHT - 1);
    mouse_setwrap(MOUSE_NOWRAP);
#endif

/* To be able to test fake mouse events... */
    if (keyboard_init()) {
	printf("Could not initialize keyboard.\n");
	exit(1);
    }

    color = newcolor();
    leftpressed = 0;
    x = 0;
    y = 0;
    for (;;) {
	keyboard_update();
	gl_fillbox(x, y, 5, 5, color);
	mouse_update();
	x = mouse_getx();
	y = mouse_gety();
	button = mouse_getbutton();
	if (button & MOUSE_LEFTBUTTON) {
	    if (!leftpressed) {
		color = newcolor();
		leftpressed = 1;
	    }
	} else
	    leftpressed = 0;
	if (button & MOUSE_RIGHTBUTTON)
	    break;
    }

#ifdef MANUALLY_SETUP_MOUSE
    mouse_close();
#endif

    vga_setmode(TEXT);
    exit(0);
}
