/* Program to test the svgalib keyboard functions. */
/* and stress vga_safety_fork() */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

#define zero_sa_mask(maskptr) memset(maskptr, 0, sizeof(sigset_t))

static char sig2release[] =
{SIGHUP, SIGINT, SIGQUIT, SIGILL,
 SIGTRAP, SIGIOT, SIGBUS, SIGFPE,
 SIGSEGV, SIGPIPE, SIGALRM, SIGTERM,
 SIGXCPU, SIGXFSZ, SIGVTALRM,
 SIGPROF, SIGPWR};


static int newcolor(void)
{
    if (BYTESPERPIXEL == 1)
	return random() % 15 + 1;
    return gl_rgbcolor(random() & 255, random() & 255, random() & 255);
}


static void timeout(int sig)
{
    keyboard_close();
    vga_setmode(TEXT);
    puts("Automatic termination after 60 seconds.");
    exit(1);
}

void shutdown(void)
{
    puts("Shutdown called!");
}

void main(void)
{
    struct sigaction siga;
    int vgamode, color, leftpressed;
    int x, y;

    printf("\nWARNING: This program will set the keyboard to RAW mode.\n"
	   "The keyboard routines in svgalib have not been tested\n"
	   "very much. There may be no recovery if something goes\n"
	   "wrong.\n\n"
	   "Press ctrl-c now to bail out, enter to continue.\n"
	   "In the test itself, use 'q' or Escape to quit.\n"
	   "It will also terminate after 60 seconds.\n"
    "Use any cursor keys to move, keypad 0 or enter to change color.\n\n"
    "\aWARNING, this version of keytest explicitly removes all svgalib\n"
      "automatic restore funcs, s.t. when you kill it from the outside\n"
    "only vga_safety_fork() can rescue you. Use this svgalib test tool\n"
	   "with EXTREME! care.\n"
	);

    getchar();

    vga_safety_fork(shutdown);	/* Does already enter a videomode */

    vga_init();

    /* Never do this in your code! */
    siga.sa_flags = 0;
    zero_sa_mask(&(siga.sa_mask));
    for (x = 0; x < sizeof(sig2release); x++) {
	siga.sa_handler = SIG_DFL;
	sigaction(sig2release[x], &siga, NULL);
    }

    vgamode = vga_getdefaultmode();
    if ((vgamode == -1) || (vga_getmodeinfo(vgamode)->bytesperpixel != 1))
	vgamode = G320x200x256;

    if (!vga_hasmode(vgamode)) {
	printf("Mode not available.\n");
	exit(1);
    }
    vga_setmode(vgamode);
    gl_setcontextvga(vgamode);
    gl_enableclipping();

    signal(SIGALRM, timeout);

    /* This installs the default handler, which is good enough for most */
    /* purposes. */
    if (keyboard_init()) {
	printf("Could not initialize keyboard.\n");
	exit(1);
    }
    /* Translate to 4 keypad cursor keys, and unify enter key. */
    keyboard_translatekeys(TRANSLATE_CURSORKEYS | TRANSLATE_KEYPADENTER |
			   TRANSLATE_DIAGONAL);
    /* (TRANSLATE_DIAGONAL seems to give problems.) Michael: No doesn't...
       but might not do what you expect.. */

    alarm(60);			/* Terminate after 60 seconds for safety. */

    x = WIDTH / 2;
    y = HEIGHT / 2;
    color = newcolor();
    leftpressed = 0;
    for (;;) {
	/* Draw moving box. */
	gl_fillbox(x, y, 5, 5, color);

	/* Draw key status bar at top of screen. */
	gl_putbox(0, 0, 128, 1, keyboard_getstate());

	/* Wait about 1/100th of a second. */
	/* Note that use of this function makes things less */
	/* smooth because of timer latency. */
	usleep(10000);

	keyboard_update();

	/* Move. */
	if (keyboard_keypressed(SCANCODE_CURSORLEFT))
	    x--;
	if (keyboard_keypressed(SCANCODE_CURSORRIGHT))
	    x++;
	if (keyboard_keypressed(SCANCODE_CURSORUP))
	    y--;
	if (keyboard_keypressed(SCANCODE_CURSORDOWN))
	    y++;

	/* Boundary checks. */
	if (x < 0)
	    x = 0;
	if (x >= WIDTH)
	    x = WIDTH - 1;
	if (y < 1)
	    y = 1;
	if (y >= HEIGHT)
	    y = HEIGHT - 1;

	/* Check for color change. */
	if (keyboard_keypressed(SCANCODE_KEYPAD0) ||
	    keyboard_keypressed(SCANCODE_ENTER)) {
	    if (!leftpressed) {
		color = newcolor();
		leftpressed = 1;
	    }
	} else
	    leftpressed = 0;

	if (keyboard_keypressed(SCANCODE_Q) ||
	    keyboard_keypressed(SCANCODE_ESCAPE))
	    break;
    }

    keyboard_close();		/* Don't forget this! */

    vga_setmode(TEXT);
    exit(0);
}
