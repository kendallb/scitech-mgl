/* Program to test the svgalib joystick functions. */
/* Written by M. Weller <eowmob@exp-math.uni-essen.de> */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <vgajoystick.h>
#include <vgakeyboard.h>

extern char *optarg;
extern int optind, opterr, optopt;

#ifdef SVGA_AOUT

void main(int argc, char *argv[]) {
    puts("Sorry, this demo works only with the ELF svgalib.\n");
    exit(1);
}

#else

#define PENCILSIZE 5

struct {
    int wx;		/* xoffset to add to scaled joystick position to get cx */
    int cx, cy;		/* onscreen coords of pencil */
    int jx, jy;		/* current joystick status */
    int tx;		/* onscreen xcoord of text for joypos */
    char bitmap[PENCILSIZE * PENCILSIZE * 4]; /* big enough in any screen mode */
    int ox, oy; /* location of saved patch, ox < 0 for no data saved */
    int color;		/* drawing color */
    int drawing;	/* we are drawing (actually shadows button 1 state) */
    int newpos;		/* cx/cy changed, draw new pencil position */
} joypanel[2];

int wy, sx, sy, ty;	/* wy, ty y coords of wx, tx. sx/sy are scale values:
                         * (jx * sx + 128) / 256 is pencil coords (add wx for screen
                         * coords). Same for y. */

int newcolor(void)
{
    if (BYTESPERPIXEL == 1)
	return random() % 15 + 1;
    return gl_rgbcolor(random() & 255, random() & 255, random() & 255);
}

void draw_pencil(int i) {
    char msg[100];

    if (!joypanel[i].newpos)
	return;

    sprintf(msg, "x = %4d, y = %4d", joypanel[i].jx, joypanel[i].jy);
    gl_write(joypanel[i].tx, ty, msg);

    if (joypanel[i].ox >= 0)
	gl_putbox(joypanel[i].ox, joypanel[i].oy, PENCILSIZE, PENCILSIZE, joypanel[i].bitmap);

    /* If not drawing, save destination area */
    if (!joypanel[i].drawing)
	gl_getbox(joypanel[i].ox = joypanel[i].cx, joypanel[i].oy = joypanel[i].cy,
		  PENCILSIZE, PENCILSIZE, joypanel[i].bitmap);
    else
	joypanel[i].ox = -1;
  
    gl_fillbox(joypanel[i].cx, joypanel[i].cy, PENCILSIZE, PENCILSIZE, joypanel[i].color);
    joypanel[i].newpos = 0;
}

void init_screen(void) {
    int white;

    gl_clearscreen(0);

    white = vga_white();

    gl_line(0, 0, WIDTH - 2, 0, white);
    gl_line(0, 0, 0, HEIGHT - 1, white);
    gl_line(WIDTH/2, 0, WIDTH/2, HEIGHT - 1, white);
    gl_line(WIDTH - 2, 0, WIDTH - 2 , HEIGHT - 1, white);
    gl_line(0, 11, WIDTH - 2, 11, white);
    gl_line(0, HEIGHT - 1, WIDTH - 2, HEIGHT - 1, white);

    ty = 2;
    sx = WIDTH / 2 - 3 - PENCILSIZE;
    sy = HEIGHT - 6 - PENCILSIZE - 9;
    wy = 2 + 8 + 3 + (((sy << 7) + 128) >> 8);

    joypanel[0].color = white;
    joypanel[0].drawing = 0;
    joypanel[0].newpos = 1;
    joypanel[0].ox = -1;
    joypanel[0].tx = 2;
    joypanel[0].jx = 0;
    joypanel[0].jy = 0;
    joypanel[0].wx = 2 + (((sx << 7) + 128) >> 8);
    joypanel[0].cx = joypanel[0].wx;
    joypanel[0].cy = wy;

    draw_pencil(0);

    joypanel[1].color = white;
    joypanel[1].drawing = 0;
    joypanel[1].newpos = 1;
    joypanel[1].ox = -1;
    joypanel[1].tx = WIDTH / 2 + 2;
    joypanel[1].jx = 0;
    joypanel[1].jy = 0;
    joypanel[1].wx = joypanel[0].wx + joypanel[1].tx;
    joypanel[1].cx = joypanel[0].cx + joypanel[1].tx;
    joypanel[1].cy = wy;

    draw_pencil(1);
}

void myhandler(int event, int number, char value, int joydev) {
#if 0 
    printf("%d: %d %d %d\n", joydev, event, number, (int)value);
#endif
    switch(event) {
	case JOY_EVENTBUTTONUP:
	    if (!number)
		joypanel[joydev].drawing = 0;
	    else {
		joypanel[joydev].color = newcolor();
		joypanel[joydev].newpos = 1;
	    }
	    break;
	case JOY_EVENTBUTTONDOWN:
	    if (!number)
		joypanel[joydev].drawing = 1;
	    break;
	case JOY_EVENTAXIS:
	    switch(number) {
		case 0: /* x */
		    joypanel[joydev].jx = value;
		    joypanel[joydev].cx = joypanel[joydev].wx + ((((int)value) * sx + 128) / 256);
		    joypanel[joydev].newpos = 1;
		    break;
		case 1:
		    joypanel[joydev].jy = value;
		    joypanel[joydev].cy = wy + ((((int)value) * sy + 128) / 256);
		    joypanel[joydev].newpos = 1;
		    break;
	    }
    }
    /* Note that any reserved events are ignored */
}

void usage(void) {
    puts("Usage: mjoytest [-j <joystick number>] [svgalib mode]\n"
	 "\ttest multiple joystick support and joystick sharing.");
    exit(1);
}

void mycalout(const char *msg) {
    gl_printf(-1, -1, msg);
}

void main(int argc, char *argv[]) {
    int vgamode = -1;
    int which, joymask = 3;
    struct timeval timeout;

    while(EOF != (which = getopt(argc, argv, "j:m:"))) {
	switch(which) {
	    case 'j':
		if (!strcmp(optarg, "0"))
		    joymask = 1;
		else if (!strcmp(optarg, "1"))
		    joymask = 1;
		else
		    usage();
		break;
	    case 'm':
		vgamode = vga_getmodenumber(optarg);
		if (vgamode < 0)
		    usage();
		break;
	    default:
		usage();
	}
    }
    if (optind < argc) {
	if (optind != 1 + argc)
	    usage();
	if (vgamode >= 0)
	    usage();
	vgamode = vga_getmodenumber(argv[optind]);
	if (vgamode < 0)
	    usage();
    }
    vga_init();
    if (vgamode < 0)
	vgamode = vga_getdefaultmode();
    if (vgamode < 0)
	vgamode = G320x200x256;

    if (!vga_hasmode(vgamode)) {
	printf("Mode not available.\n");
	exit(-1);
    }

    puts("In the demo, press\n"
	 "<1> to calibrate joystick 1.\n"
	 "<2> to calibrate joystick 2.\n"
	 "<c> to clear the screen (<1> & <2> do this too).\n"
	 "<q> to exit (<Ctrl>-C should work too.\n"
	 "Joystick button 1 enables drawing while pressed.\n"
	 "Joystick button 2 selects next color.\n"
	 "\nNow hit <Return> to start the demo.");

    getchar();
    fflush(stdin);

    for (which = 0; which < 2; which++) {
	if (!(joymask & (1 << which)))
	    continue;
	errno = 0;
	if (joystick_init(which, JOY_CALIB_STDOUT) < 0) {
	    if (errno)
		printf("Unable to initialize joystick %d: %s.\n", which, strerror(errno));
	    else
		printf("Unable to initialize joystick %d.\n", which);
	}
    }

    joystick_sethandler(-1, myhandler);

    vga_setmode(vgamode);
    gl_setcontextvga(vgamode);

    gl_setwritemode(FONT_COMPRESSED | WRITEMODE_OVERWRITE);
    gl_setfontcolors(0, vga_white());
    gl_setfont(8, 8, gl_font8x8);

    init_screen();

    for(;;) {
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;
	which = vga_waitevent(VGA_KEYEVENT, NULL, NULL, NULL, &timeout);
	if (which & VGA_KEYEVENT) {
	    switch(vga_getch()) {
		case '1':
		    gl_printf(2, 2, "");
		    if (1 & joymask) {
			vga_lockvc();
			joystick_init(0, mycalout);
		        /* IMPORTANT, reenable ownhandler! */
    		        joystick_sethandler(0, myhandler);
			vga_unlockvc();
		    }
		    init_screen();
		    break;
		case '2':
		    gl_printf(2, 2, "");
		    if (2 & joymask) {
			vga_lockvc();
			joystick_init(1, mycalout);
		        /* IMPORTANT, reenable ownhandler! */
    		        joystick_sethandler(1, myhandler);
			vga_unlockvc();
		    }
		    init_screen();
		    break;
		case 'c':
		case 'C':
		    init_screen();
		    break;
		case 'q':
		case 'Q':
		    goto leave_loop;
		default:
		    putchar('\a');
		    fflush(stdout);
		    break;
	    }
	}
	which = joystick_update();
	if (which & 1);
	    draw_pencil(0); /* It makes only sense to check for the newpos flag
			     * if something happened with the joystick at all */
	if (which & 2);
	    draw_pencil(1);
    }
  leave_loop:
    printf("Shutting down.\n");

    vga_setmode(TEXT);
    exit(0);
}

#endif /* SVGA_AOUT */
