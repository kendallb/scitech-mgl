/* Program to test the vga_waitevent function. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <vgakeyboard.h>

#define USE_RAWKEYBOARD

static int newcolor(void)
{
    if (BYTESPERPIXEL == 1)
	return random() % 15 + 1;
    return gl_rgbcolor(random() & 255, random() & 255, random() & 255);
}

static void ping(void)
{
    putchar('\a');
    fflush(stdout);
}

/* read data from fd. Write string if '\n' encountered */
void process_input(int fd)
{
    static char textbuf[80], *data = textbuf;
    char inbuf[80];
    int len, i;

    len = read(fd, inbuf, 80);
    if (len <= 0)
	return;
    for (i = 0; i < len; i++) {
	if (inbuf[i] == '\n') {
	    *data = 0;
	    gl_write(0, 10, textbuf);
	    ping();
	    data = textbuf;
	} else {
	    *data++ = inbuf[i];
	}
    }
}

void child(int fd)
{
    time_t last_time = 0, now;
    FILE *output;

    output = fdopen(fd, "w");
    for (;;) {			/* when parent dies we get killed by SIGPIPE */
	now = time(NULL);
	if (now / 5 > last_time) {	/* a new minute started */
	    last_time = now / 5;
	    fputs(ctime(&now), output);
	    fputc('\n', output);
	    fflush(output);
	}
	sleep(1);
    }
}

void main(void)
{
    struct timeval timeout;
    fd_set inputs;
    char bitmap[16 * 16 * 4];	/* big enough for 10x10 bitmap in any mode */
    int vgamode, color, pipefd[2], x, y, button, event, cursorsize = 5;
    char loop = 1, drawcursor = 1;
#ifdef USE_RAWKEYBOARD
    char space_pressed = 0;
#endif

    puts("This is a demo showing the abilities of the new vga_waitevent() function\n"
	 "If something goes wrong it might hang your machine. Thus hit <ctrl>-C now\n"
	 "to bailout if in doubt.\n"
	 "Use mouse to move cursor. 1-9,0 to set the cursor size. Space to change the\n"
    "cursor color. Left button to draw. Right button or 'Q' to bailout.\n"
	 "The cursor goes on/off every half second by usage of a timeout passed to\n"
	 "vga_waitevent. Every 5 secs a string from a child process (the time) arrives\n"
	 "asynchronously and is displayed by the frontend.");
#ifdef USE_RAWKEYBOARD
    puts("\nBEWARE! This has been compiled to use the raw keyboard. A crash might\n"
	 "render the console unusable. (but shouldn't).");
#endif
    fputs("\nHit <Enter> if brave enough, else ^C to bailout: ", stdout);
    fflush(stdout);
    getchar();
    fflush(stdin);		/* clear I/O buffer */

    pipe(pipefd);
    if (fork() == 0) {		/* fork off b4 touching graphix to avoid side effects */
	close(pipefd[0]);	/* Important: close reading side, else it remains     */
	/* opened by child when parent exits and we don't get */
	/* a SIGPIPE!                                         */
	child(pipefd[1]);
    }
    vga_init();
    vgamode = vga_getdefaultmode();
    if (vgamode == -1)
	vgamode = G320x200x256;

    if (!vga_hasmode(vgamode)) {
	printf("Mode not available.\n");
	exit(-1);
    }
    /* Enable automatic mouse setup at mode set. */
    vga_setmousesupport(1);
    vga_setmode(vgamode);
    /* Disable wrapping (default). */
    /* mouse_setwrap(MOUSE_NOWRAP); */
    gl_setcontextvga(vgamode);
    gl_enableclipping();

    /* There might be some scrap data in the serial buffer
       from the mouse. It will make vga_waitevent block
       because it thinks the mouse wants to send data but
       then no mouse packet arrives. */
    color = newcolor();
    x = 0;
    y = 0;
    gl_setwritemode(WRITEMODE_OVERWRITE | FONT_COMPRESSED);
    gl_setfont(8, 8, gl_font8x8);
    gl_setfontcolors(0, newcolor());

#ifdef USE_RAWKEYBOARD
    if (keyboard_init()) {
	printf("Could not initialize keyboard.\n");
	exit(1);
    }
#endif

    while (loop) {
	gl_getbox(x, y, 10, 10, bitmap);
	if (drawcursor) {
	    gl_hline(x, y, x + cursorsize, color);
	    gl_hline(x, y + cursorsize, x + cursorsize, color);
	    gl_line(x, y, x, y + cursorsize, color);
	    gl_line(x + cursorsize, y, x + cursorsize, y + cursorsize, color);
	}
	FD_ZERO(&inputs);
	FD_SET(pipefd[0], &inputs);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;	/* 0.5 second time out */
	event = vga_waitevent(VGA_MOUSEEVENT | VGA_KEYEVENT,
			      &inputs, NULL, NULL, &timeout);
	gl_putbox(x, y, 10, 10, bitmap);
	if (timeout.tv_sec || timeout.tv_usec) {
	    /* No timeout. An actual event occured. Reset to visible
	       cursor. Note:
	       This is actually a bug as the cursor will get visible on time
	       updates. However, it's better this way for demo/test
	       purposes. */
	    drawcursor = 1;
	} else {
	    drawcursor ^= 1;
	}
	if (FD_ISSET(pipefd[0], &inputs))
	    process_input(pipefd[0]);
	if (event & VGA_MOUSEEVENT) {
	    x = mouse_getx();
	    y = mouse_gety();
	    button = mouse_getbutton();
	    if (button & MOUSE_LEFTBUTTON)
		gl_fillbox(x, y, cursorsize + 1, cursorsize + 1, color);
	    if (button & MOUSE_RIGHTBUTTON)
		loop = 0;
	}
	if (event & VGA_KEYEVENT) {
#ifdef USE_RAWKEYBOARD
	    if (keyboard_keypressed(SCANCODE_1))
		cursorsize = 0;
	    if (keyboard_keypressed(SCANCODE_2))
		cursorsize = 1;
	    if (keyboard_keypressed(SCANCODE_3))
		cursorsize = 2;
	    if (keyboard_keypressed(SCANCODE_4))
		cursorsize = 3;
	    if (keyboard_keypressed(SCANCODE_5))
		cursorsize = 4;
	    if (keyboard_keypressed(SCANCODE_6))
		cursorsize = 5;
	    if (keyboard_keypressed(SCANCODE_7))
		cursorsize = 6;
	    if (keyboard_keypressed(SCANCODE_8))
		cursorsize = 7;
	    if (keyboard_keypressed(SCANCODE_9))
		cursorsize = 8;
	    if (keyboard_keypressed(SCANCODE_0))
		cursorsize = 9;
	    if (keyboard_keypressed(SCANCODE_Q))
		loop = 0;
	    if (keyboard_keypressed(SCANCODE_SPACE)) {
		if (!space_pressed) {
		    color = newcolor();
		    space_pressed = 1;
		}
	    } else {
		space_pressed = 0;
	    }
#else
	    switch (vga_getch()) {
	    case '1':
		cursorsize = 0;
		break;
	    case '2':
		cursorsize = 1;
		break;
	    case '3':
		cursorsize = 2;
		break;
	    case '4':
		cursorsize = 3;
		break;
	    case '5':
		cursorsize = 4;
		break;
	    case '6':
		cursorsize = 5;
		break;
	    case '7':
		cursorsize = 6;
		break;
	    case '8':
		cursorsize = 7;
		break;
	    case '9':
		cursorsize = 8;
		break;
	    case '0':
		cursorsize = 9;
		break;
	    case ' ':
		color = newcolor();
		break;
	    case 'q':
	    case 'Q':
		loop = 0;
		break;
	    default:
		ping();
		break;
	    }
#endif
	}
    }

#ifdef USE_RAWKEYBOARD
    keyboard_close();		/* Don't forget this! */
#endif
    vga_setmode(TEXT);
    exit(0);
}
