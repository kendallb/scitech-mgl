/* Keyboard library functions */
/* H. Hanemaayer (hhanemaa@cs.ruu.nl) */

/* Apr 03 1998: Added fake mouse event and some cleanup by 101 (Attila Lendvai)
        				e-mail: 101@kempelen.inf.bme.hu */
/*
 * May 28 1998: Changed __keyboard_eventhandler to
 * __svgalib_keyboard_eventhandler and made non-static to allow for fake
 * keyboard events for the wheelmice (Brion Vibber <brion@pobox.com>)
 *
 * July 3 1998: Added keyboard remapping support - brion
*/


/*
 * Keyboard I/O based on showkey.c from kbd-0.84 package.
 * This is an initial version, it isn't very safe yet since it only catches
 * sigsegv.
*/

/* #define DEBUG_KEYBOARD */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
/* linux/keyboard.h defines NR_KEYS and some scancode-like constants, so it */
/* should also be useful for svgalib programs using the keyboard. It misses */
/* a few KERNEL ifdefs around kernel data structures though. */
#include <linux/keyboard.h>
#include <sys/vt.h>
/* Needed to check uid of keymap files */
#include <sys/stat.h>
#include <unistd.h>

#include <vga.h>
#include "../libvga.h"
#include "vgakeyboard.h"
#include "driver.h"

void (*__svgalib_keyboard_eventhandler) (int, int);

static struct termios oldkbdtermios, newkbdtermios;
static int oldkbmode;
/* vga.c needs to check that: */
int __svgalib_kbd_fd = -1; /* nowadays merely used as a flag */
static int c_state, ctrl_state, alt_state, functionkey_state;
static int translatemode = 0;
static unsigned char state[NR_KEYS];	/* NR_KEYS is defined in linux/keyboard.h */
static int keymap[NR_KEYS];
static int usekeymap = 0;    /* If nonzero, we translate scancodes */
static int rootkeymaps = 0;  /* If nonzero, only load keymaps owned by root */
static char keynames[NR_KEYS][MAX_KEYNAME_LEN] = {
    /* The default US QWERTY layout */
    "",
    "Escape", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "zero", "minus", "equal", "Delete",
    "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "bracketleft", "bracketright", "Return",
    "Control", "a", "s", "d", "f", "g", "h", "j", "k", "l", "semicolon", "apostrophe", "grave",
    "Shift", "backslash", "z", "x", "c", "v", "b", "n", "m", "comma", "period", "slash", "Shift", "KP_Multiply",
    "Alt", "space", "Caps_Lock",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
    "Num_Lock", "Scroll_Lock",
    "KP_7", "KP_8", "KP_9", "KP_Subtract",
    "KP_4", "KP_5", "KP_6", "KP_Add",
    "KP_1", "KP_2", "KP_3",
    "KP_0", "KP_Period",
    "Last_Console", "", "less", "F11", "F12", "", "", "", "", "", "", "",
    "KP_Enter", "Control", "KP_Divide", "Control_backslash", "AltGr", "Break",
    "Find", "Up", "Prior", "Left", "Right", "Select", "Down", "Next", "Insert",
    "Remove", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};
static void default_handler(int, int);
static char *kbd_load_keymap(char *ptr);
static void kbd_update_keymap(void);
static int kbd_mapkey(int inscancode);

static struct FakeMouseEvent {
  int	data;
  short type;
  short flags;
  int   scancode;
  char  keyname[MAX_KEYNAME_LEN];
} **fake_mouse_events = 0;

/* Number of events per scancode */
static short *fme_numberof = 0;

/* Number of scancodes used */
static int fme_used = 0;

/* Fake event flags */
#define FMEF_TRIGGERED	1	/* Triggered, */
#define FMEF_AT_PRESS	2	/* Request at press (or if false at release)*/
#define FMEF_AT_BOTH	4	/* Request at press/release too */
#define FMEF_REPEAT	8	/* Keep on sending fake events */

#define FME_TYPE_BUTTON  1
#define FME_TYPE_DELTAX  2
#define FME_TYPE_DELTAY  3
#define FME_TYPE_DELTAZ	 4
#define FME_TYPE_IGNOREX 5
#define FME_TYPE_IGNOREY 6
#define FME_TYPE_IGNOREZ 7
#define FME_TYPE_BUTTON1 8
#define FME_TYPE_BUTTON2 9
#define FME_TYPE_BUTTON3 10

static int expand_events(int n) {
    struct FakeMouseEvent *newptr = 0;
//    int n;

  if ( ! fake_mouse_events) {
#ifdef DEBUG_KEYBOARD
      fprintf(stderr," Allocating space for FMEs...\n");
#endif
    if ( (fake_mouse_events = malloc(sizeof(void *) * NR_KEYS)) == 0) {
 error:
      fputs("svgalib: keyboard-config: out of memory ?! Fake mouse events might be disabled !", stderr);
      return 0;
    } else {
      if ( (fme_numberof = malloc(sizeof(short) * NR_KEYS)) == 0) {
        free(fake_mouse_events);
        fake_mouse_events = 0;
        goto error;
      }
      memset(fake_mouse_events, 0, sizeof(void *) * NR_KEYS);
      memset(fme_numberof, 0, sizeof(short) * NR_KEYS);
    }
  }
#ifdef DEBUG_KEYBOARD
  fprintf(stderr," Expanding FME #%d... ", n);
#endif
  if ( (newptr = realloc(fake_mouse_events[n],
                         sizeof(struct FakeMouseEvent) * ++fme_numberof[n])) != 0 ) {
#ifdef DEBUG_KEYBOARD
  fprintf(stderr,"it's all good.\n");
#endif
    fake_mouse_events[n] = newptr;
    return 1;
  } else {
    goto error;
  }
}

static char *kbd_config_options[] = {
  "kbd_fake_mouse_event", "kbd_keymap", "kbd_only_root_keymaps", NULL
};

static char *kbd_process_option(int option, int mode) {
  char *ptr;
  short type   = 0;
  short ignore_type   = 0;
  short flags  = FMEF_AT_PRESS;
  int data     = 0;
  int scancode = 0;
  int event_ok = 0;
  int n = 0;
  struct FakeMouseEvent *event;
  char keyname[MAX_KEYNAME_LEN];
  
  switch (option) {
    case 0: /* kbd_fake_mouse_event */
	if ( (ptr = strtok(NULL, " ")) == 0) {
  param_needed:
	  fprintf(stderr, "svgalib: kbd-config: too few parameters for \'%s\'\n",
	  		kbd_config_options[option]);
	  return ptr;
        } else if ( (scancode = __svgalib_mapkeyname(ptr)) == -1) {
	  fprintf(stderr, "svgalib: kbd-config: \'%s\' is not a valid scancode\n", ptr);
	  return ptr;
        }

        strncpy(keyname, ptr, MAX_KEYNAME_LEN);

        /* Find the event for this key if there is one*/
#ifdef DEBUG_KEYBOARD
        fprintf(stderr," Looking for fme for key \'%s\'... ", keyname);
#endif
        for(n = 0; n < fme_used; n++)
            if(strcmp(fake_mouse_events[n]->keyname, keyname) == 0)
                break;
        if(n == fme_used)
            fme_used++;
#ifdef DEBUG_KEYBOARD
        fprintf(stderr," found at #%d.\n", n);
#endif
        
  read_event:
	if ( (ptr = strtok(NULL, " ")) == 0) {
	  if ( event_ok )
	    break;
	  else  
	    goto param_needed;
	}

	if ( ! strcasecmp(ptr, "both")) {
	   flags |= FMEF_AT_BOTH;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "up")) {
	   flags &= ~FMEF_AT_PRESS;
	   flags &= ~FMEF_AT_BOTH;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "down")) {
	   flags |= FMEF_AT_PRESS;
	   flags &= ~FMEF_AT_BOTH;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "repeat")) {
	   flags |= FMEF_REPEAT;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "discrete")) {
	   flags &= ~FMEF_REPEAT;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "button")) {
	   type = FME_TYPE_BUTTON;
	   if ( (ptr = strtok(NULL, " ")) == 0)
	     goto param_needed;
	   if ( (data = atoi(ptr)) == 0  ||  data > 10) {
	     fprintf(stderr, "svgalib: kbd-config: \'%s\' is not a valid mouse button\n", ptr);
	     return ptr;
	   }

  store_event:

#ifdef DEBUG_KEYBOARD
  printf(" Fake Mouse Event: scancode: %ld;  type: %ld;  data: %ld;  flags: %ld\n", (long)scancode, (long)type, (long)data, (long)flags);
#endif
	   if ( ! expand_events(n))
	     return ptr;
           event = &fake_mouse_events[n][fme_numberof[n] - 1];
	   event -> type  = type;
	   event -> flags = flags;
	   event -> data  = data;
           event -> scancode = scancode;
           strcpy(event -> keyname, keyname);
	   event_ok = 1;
	   goto read_event;
	} else if ( ! strcasecmp(ptr, "deltax")) {
	   type        = FME_TYPE_DELTAX;
	   ignore_type = FME_TYPE_IGNOREX;

  process_delta:
	   if ( (ptr = strtok(NULL, " ")) == 0)
	     goto param_needed;
	   if ( ! strcasecmp(ptr, "off")) {
	     type = ignore_type;
	     data = 1;
	     goto store_event;
	   } else if ( ! strcasecmp(ptr, "on")) {
	     type = ignore_type;
	     data = 0;
	     goto store_event;
	   } else if ( (data = atoi(ptr)) == 0) {
	     fprintf(stderr, "svgalib: kbd-config: \'%s\' is not a valid delta\n", ptr);
	     return ptr;
	   }
	   goto store_event;
	} else if ( ! strcasecmp(ptr, "deltay")) {
	   type        = FME_TYPE_DELTAY;
	   ignore_type = FME_TYPE_IGNOREY;
	   goto process_delta;
	} else if ( ! strcasecmp(ptr, "deltaz")) {
	   type        = FME_TYPE_DELTAZ;
	   ignore_type = FME_TYPE_IGNOREZ;
	   goto process_delta;
	} else if ( ! strcasecmp(ptr, "button1")) {
	   type = FME_TYPE_BUTTON1;
  process_button:
	   if ( (ptr = strtok(NULL, " ")) == 0)
	     goto param_needed;
	   if ( ! strcasecmp(ptr, "pressed")) {
	     data = 1;
	   } else if ( ! strcasecmp(ptr, "released")) {
	     data = 0;
	   } else {
	     fprintf(stderr, "svgalib: kbd-config: \'%s\' is not a legal parameter for command "
	     	"\'button[123]\'. Should be either \'pressed\' or \'released\'\n", ptr);
	     return ptr;
	   }
	   goto store_event;
	} else if ( ! strcasecmp(ptr, "button2")) {
	   type = FME_TYPE_BUTTON2;
	   goto process_button;
	} else if ( ! strcasecmp(ptr, "button3")) {
	   type = FME_TYPE_BUTTON3;
	   goto process_button;
	} else if ( ! event_ok) {
	  fprintf(stderr, "svgalib: kbd-config: illegal mouse event: \'%s\'\n", ptr);
	  return ptr;
	} else
	  return ptr;
    	break;

    case 1: /* kbd_keymap */
         if ( (ptr = strtok(NULL, " ")) == 0 ) {
             goto param_needed;
         } else
             if(kbd_load_keymap(ptr))
                 return ptr;

         break;

  case 2: /* kbd_only_root_keymaps */
#ifdef DEBUG_KEYBOARD
      fprintf(stderr, " Setting rootkeymaps to 1.\n");
#endif
      rootkeymaps = 1;
      break;

  }
  return strtok(NULL, " ");
};

int keyboard_init_return_fd(void) {
    char *ptr;

    keyboard_translatekeys(translatemode); /* Honour 'nosigint' setting */

    /* Install default keyboard handler. */
    __svgalib_keyboard_eventhandler = default_handler;

    __svgalib_open_devconsole();
    __svgalib_kbd_fd = __svgalib_tty_fd; /* We are initialized. */

    if (ioctl(__svgalib_kbd_fd, KDGKBMODE, &oldkbmode)) {
	printf("svgalib: cannot get keyboard mode.\n");
	return -1;
    }
    tcgetattr(__svgalib_kbd_fd, &oldkbdtermios);
    newkbdtermios = oldkbdtermios;

    newkbdtermios.c_lflag &= ~(ICANON | ECHO | ISIG);
    newkbdtermios.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
    newkbdtermios.c_cc[VMIN] = 0;	/* Making these 0 seems to have the */
    newkbdtermios.c_cc[VTIME] = 0;	/* desired effect. */

    tcsetattr(__svgalib_kbd_fd, TCSAFLUSH, &newkbdtermios);

    ioctl(__svgalib_kbd_fd, KDSKBMODE, K_MEDIUMRAW);

    keyboard_clearstate();

    __svgalib_read_options(kbd_config_options, kbd_process_option);

    /* Check SVGALIB_KEYMAP env var */
    if ( (ptr = getenv("SVGALIB_KEYMAP")) != 0) {
        kbd_load_keymap(ptr);
    }

    return __svgalib_kbd_fd;		/* OK, return fd. */
}

/* Old compatible init function. */

int keyboard_init(void)
{
    if (keyboard_init_return_fd() == -1)
	return -1;
    else
	return 0;
}

void keyboard_close(void) {

    if (__svgalib_kbd_fd < 0)
	return;

    if (fake_mouse_events) {
      int i;
      for (i = 0; i < NR_KEYS; i++) {
        if (fake_mouse_events[i])
          free(fake_mouse_events[i]);
      }
      free(fake_mouse_events);
      fake_mouse_events = NULL;
    }

    ioctl(__svgalib_kbd_fd, KDSKBMODE, oldkbmode);
    tcsetattr(__svgalib_kbd_fd, 0, &oldkbdtermios);

    __svgalib_kbd_fd = -1;
}

/* For now, we assume there's no console switching. */
/* (Actually, there won't be any unless we catch the console switching */
/* keys). */

#define KBDREADBUFFERSIZE 32

static int keyboard_getevents(int wait)
{
/* Read keyboard device, and handle events. */
/* If wait == 1, process at least one event and return. */
/* If wait == 0, handle all accumulated events; return 0 if no events */
/* were handled, 1 otherwise. */
/* Wait mode doesn't seem to work very well; the keyboard repeat delay is */
/* present. I don't understand fcntl. */
    static unsigned char buf[KBDREADBUFFERSIZE];
    static int kfdmode = 0;	/* 1 = DELAY, 0 = NDELAY */
    int bytesread, i;
    int eventhandled;

    eventhandled = 0;

  again:
    if (kfdmode == 1) {		/* This only happens for wait == 1. */
#if 0
	struct termios kbdtermios;
#endif
	int flags;
	/* We don't want to wait, set NDELAY mode. */
	fcntl(__svgalib_kbd_fd, F_GETFL, &flags);
	fcntl(__svgalib_kbd_fd, F_SETFL, flags | O_NDELAY);

#if 0
	tcgetattr(__svgalib_kbd_fd, &kbdtermios);
	kbdtermios.c_lflag = kbdtermios.c_lflag & ~(ICANON | ECHO | ISIG);
	kbdtermios.c_cc[VMIN] = 0;
	kbdtermios.c_cc[VTIME] = 0;
	tcsetattr(__svgalib_kbd_fd, TCSANOW, &kbdtermios);
#endif

	kfdmode = 0;
    }
    bytesread = read(__svgalib_kbd_fd, buf, KBDREADBUFFERSIZE);

    if (wait == 1 && bytesread < 1) {
#if 0
	struct termios kbdtermios;
#endif
	int flags;
	/* We already handled an event, no need to wait for another. */
	if (eventhandled)
	    return 1;
	/* Wait mode, we'll sleep on reads. */
	fcntl(__svgalib_kbd_fd, F_GETFL, &flags);
	fcntl(__svgalib_kbd_fd, F_SETFL, flags & ~O_NDELAY);

#if 0
	tcgetattr(__svgalib_kbd_fd, &kbdtermios);
	kbdtermios.c_lflag = kbdtermios.c_lflag & ~(ICANON | ECHO | ISIG);
	kbdtermios.c_cc[VMIN] = 0;
	kbdtermios.c_cc[VTIME] = 0;
	tcsetattr(__svgalib_kbd_fd, TCSANOW, &kbdtermios);
#endif

	kfdmode = 1;
	bytesread = read(__svgalib_kbd_fd, buf, 1);
    }
    if (wait == 0 && bytesread < 1)
	return eventhandled;

    if (bytesread >= 1)
	eventhandled = 1;

    for (i = 0; i < bytesread; i++) {
      unsigned char scancode = kbd_mapkey(buf[i] & 0x7f);
      unsigned char is_pressed = (buf[i] & 0x80) ? 0 : 1;

	/* Check for ctrl-c. */
	switch (scancode) {
	  case SCANCODE_C:
		c_state =  is_pressed;
		break;
		
	  case SCANCODE_LEFTCONTROL:
	  case SCANCODE_RIGHTCONTROL:
		ctrl_state = is_pressed;
		break;

	  case SCANCODE_LEFTALT:
	  case SCANCODE_RIGHTALT:
		alt_state = is_pressed;
		break;

	  case SCANCODE_F1...SCANCODE_F10:
		functionkey_state = (is_pressed)  ?
		  functionkey_state |   1 << (scancode - SCANCODE_F1)   :
		  functionkey_state & ~(1 << (scancode - SCANCODE_F1));
		break;
	}

	if (fake_mouse_events  && __svgalib_mouse_fd > -1) {
            int n;

            for(n = 0; n < fme_used; n++)
                if(fake_mouse_events[n][0].scancode == scancode)
                    break;

            if(fme_numberof[n])
            {
                int i;
                int dx=0, dy=0, dz=0, but=0; /* To gather mutiple events into one */
                int but_changed = 0;
                struct FakeMouseEvent *event;

                for (i = 0; i < fme_numberof[n]; i++) {
                    int request_at_down;
                    short flags;
                    event = &fake_mouse_events[n][i];
                    if (!is_pressed)
                        event -> flags &= ~FMEF_TRIGGERED;
                    flags = event -> flags;
                    request_at_down = (event -> flags & FMEF_AT_PRESS)  ?  1 : 0;
#ifdef DEBUG_KEYBOARD
                    printf(" event type: %ld;  flags: %ld;  data: %ld; is_pressed: %ld\n", (long)event->type, (long)flags, (long)event->data, (long)is_pressed);
#endif
                    if (flags & FMEF_AT_BOTH  ||  request_at_down == is_pressed) {
#ifdef DEBUG_KEYBOARD
                        printf("   flags: %ld\n", (long)flags);
#endif
                        if (  (! (flags & FMEF_TRIGGERED))  ||  flags & FMEF_REPEAT) {
#ifdef DEBUG_KEYBOARD
                            printf("      triggering\n");
#endif
                            switch (event -> type) {
                            case FME_TYPE_BUTTON1:
                                but = (event -> data) ?  but | 4  :  but & ~4;
                                but_changed = 1;
                                break;
                            case FME_TYPE_BUTTON2:
                                but = (event -> data) ?  but | 1  :  but & ~1;
                                but_changed = 1;
                                break;
                            case FME_TYPE_BUTTON3:
                                but = (event -> data) ?  but | 2  :  but & ~2;
                                but_changed = 1;
                                break;

                            case FME_TYPE_IGNOREX:
                                __svgalib_m_ignore_dx = event -> data;
                                break;
                            case FME_TYPE_IGNOREY:
                                __svgalib_m_ignore_dy = event -> data;
                                break;
                            case FME_TYPE_IGNOREZ:
                                __svgalib_m_ignore_dz = event -> data;
                                break;

                            case FME_TYPE_DELTAX:
                                dx += event -> data;
                                break;
                            case FME_TYPE_DELTAY:
                                dy += event -> data;
                                break;
                            case FME_TYPE_DELTAZ:
                                dz += event -> data;
                                break;
                            }
                            if (is_pressed)
                                event -> flags |= FMEF_TRIGGERED;
                        }
                    }
                }
                if ((dx || dy || dz || but_changed) && __svgalib_mouse_eventhandler) {
              __svgalib_mouse_eventhandler(but, dx, dy, dz, 0, 0, 0);

#ifdef DEBUG_KEYBOARD
            printf("\tfake_mouse_event triggered; but: %ld; dx: %ld; dy: %ld; dz: %ld\n",
                   (long)but, (long)dx, (long)dy, (long)dz);
#endif
          }
            }
	}

	if (ctrl_state && c_state && !(translatemode & DONT_CATCH_CTRLC))
	    raise(SIGINT);
	if (alt_state && functionkey_state) {
	    /* VT switch. */
	    /* *** what about F11 & F12? */
	    int j, vt = 0;
	    struct vt_stat vts;
	    for (j = 0; j < 10; j++)
		if (functionkey_state & (1 << j)) {
		    vt = j + 1;
		    break;
		}

	    /* Do not switch vt's if need not to */
	    ioctl(__svgalib_tty_fd, VT_GETSTATE, &vts);

	    if(vt != vts.v_active) { 
	        /* if switching vt's, need to clear keystates */
	        keyboard_clearstate();
	        /*
	         * This will generate a signal catched by
	         * svgalib to restore textmode.
	         */
	        ioctl(__svgalib_tty_fd, VT_ACTIVATE, vt);
	        return 1;
	    }
	}
	__svgalib_keyboard_eventhandler(scancode,
		    (buf[i] & 0x80) ? KEY_EVENTRELEASE : KEY_EVENTPRESS);
    }

    /* Handle other events that have accumulated. */
    goto again;
}

int keyboard_update(void)
{
    return keyboard_getevents(0);	/* Don't wait. */
}

void keyboard_waitforupdate(void)
{
    keyboard_getevents(1);	/* Wait for event. */
    return;
}

void keyboard_seteventhandler(void (*handler) (int, int))
{
    __svgalib_keyboard_eventhandler = handler;
}



/* Default event handler. */

void keyboard_setdefaulteventhandler(void)
{
    __svgalib_keyboard_eventhandler = default_handler;
}

static int checkscancode(int scancode)
{
    if (scancode < 0 || scancode >= NR_KEYS) {
	printf("svgalib: keyboard scancode out of range (%d).\n",
	       scancode);
	return 1;
    }
    return 0;
}

static void default_handler(int scancode, int newstate)
{
    if (checkscancode(scancode))
	return;
    if (translatemode & TRANSLATE_CURSORKEYS)
	/* Map cursor key block to keypad cursor keys. */
	switch (scancode) {
	case SCANCODE_CURSORBLOCKUP:
	    scancode = SCANCODE_CURSORUP;
	    break;
	case SCANCODE_CURSORBLOCKLEFT:
	    scancode = SCANCODE_CURSORLEFT;
	    break;
	case SCANCODE_CURSORBLOCKRIGHT:
	    scancode = SCANCODE_CURSORRIGHT;
	    break;
	case SCANCODE_CURSORBLOCKDOWN:
	    scancode = SCANCODE_CURSORDOWN;
	    break;
	}
    if (translatemode & TRANSLATE_DIAGONAL) {
	/* Translate diagonal keypad keys to two keypad cursor keys. */
	switch (scancode) {
	case SCANCODE_CURSORUPLEFT:
	    state[SCANCODE_CURSORUP] = newstate;
	    state[SCANCODE_CURSORLEFT] = newstate;
	    break;
	case SCANCODE_CURSORUPRIGHT:
	    state[SCANCODE_CURSORUP] = newstate;
	    state[SCANCODE_CURSORRIGHT] = newstate;
	    break;
	case SCANCODE_CURSORDOWNLEFT:
	    state[SCANCODE_CURSORDOWN] = newstate;
	    state[SCANCODE_CURSORLEFT] = newstate;
	    break;
	case SCANCODE_CURSORDOWNRIGHT:
	    state[SCANCODE_CURSORDOWN] = newstate;
	    state[SCANCODE_CURSORRIGHT] = newstate;
	    break;
	}
    }
    if ((translatemode & TRANSLATE_KEYPADENTER) && scancode ==
	SCANCODE_KEYPADENTER)
	scancode = SCANCODE_ENTER;

#if 0				/* This happens very often. */
    if (state[scancode] == newstate) {
	printf("svgalib: keyboard event does not match (scancode = %d)\n",
	       scancode);
	return;
    }
#endif
    state[scancode] = newstate;
}

void keyboard_clearstate(void)
{
    memset(state, 0, NR_KEYS);
    ctrl_state = c_state = alt_state = 0;
    functionkey_state = 0;
}

int keyboard_keypressed(int scancode)
{
    if (checkscancode(scancode))
	return 0;
    return state[scancode];
}

char *
 keyboard_getstate(void)
{
    return state;
}

void keyboard_translatekeys(int mode)
{
    translatemode = mode;
    if (__svgalib_nosigint)
	translatemode |= DONT_CATCH_CTRLC;
}

static int kbd_mapkey(int inscancode)
{
    if (usekeymap)
        return keymap[inscancode];
    else
        return inscancode;
}

static char *kbd_load_keymap(char *ptr)
{
    /* Find the specified keymap file */
    /* For now, use a full pathname */
    FILE *keymapfile;

#ifdef DEBUG_KEYBOARD
    fprintf(stderr, " Trying to load keymap \'%s\'...\n",ptr);
#endif

    /* If so configured check if the file is owned by root */
    if(rootkeymaps) {
        struct stat fs;
        if(stat(ptr, &fs)) {
            fprintf(stderr, "svgalib: kbd-config: cannot stat keymap file \'%s\'\n",
                    ptr);
            return ptr;
        } else if(fs.st_uid != 0) {
            fprintf(stderr, "svgalib: kbd-config: keymap file \'%s\' not owned by root\n",
                    ptr);
            return ptr;
        }
    }

    if((keymapfile = fopen(ptr, "rt"))) {
        /* Load it in! */
        char buf[81], nbuf[81];
        int i, l = 0, insc, outsc;

        buf[80] = nbuf[80] = 0; /* Protect somewhat against overruns */

        /* Disable the keymap until we're done in case something goes wrong... */
        usekeymap = 0;

        /* Initialize keymap */
        for (i = 0; i < NR_KEYS; keymap[i++] = i);

        while(!feof(keymapfile)) {
            /* Read */
            fgets(buf, 80, keymapfile);
            l++;

            /* Ignore comments & blank lines */
            if ((buf[0] != '#') && (buf[0] != '\n')) {
                /* Interpret the numbers */
                if((i = sscanf(buf, "%d %d %s", &insc, &outsc, nbuf) == 3)) {
                    if(checkscancode(insc)) continue;
                    if(checkscancode(outsc)) continue;
                    keymap[insc] = outsc;
                    strncpy(keynames[insc], nbuf, MAX_KEYNAME_LEN);
                } else {
                    fprintf(stderr, "svgalib: kbd-config: skipping line %d of keymap - bad %sput scancode\n",
                            l, ((i == 1)?("out"):("in")));
                }
            }
        }

        usekeymap = 1;

        /* Update the mouse driver's fake keyboard events */
#ifdef DEBUG_KEYBOARD
        fprintf(stderr, " Keymap loaded. Updating fake keyboard events...\n");
#endif
        __svgalib_mouse_update_keymap();

        /* And the fake mouse events */
#ifdef DEBUG_KEYBOARD
        fprintf(stderr, " Updating fake mouse events...\n");
#endif
        kbd_update_keymap();

        fclose(keymapfile);
    } else {
        fprintf(stderr, "svgalib: kbd-config: keymap file \'%s\' cannot be opened\n", ptr);
        return ptr;
    }

    return NULL;
}

int __svgalib_mapkeyname(const char *keyname)
{
    int scancode;
    char *ptr;

#ifdef DEBUG_KEYBOARD
    fprintf(stderr, " Attempting to map \'%s\' to a scancode...", keyname);
#endif
    
    if(!keyname) {
        fprintf(stderr, "svgalib: kbd-config: can't use NULL keyname!\n");
        return -1;
    }

    if(!*keyname) {
        fprintf(stderr, "svgalib: kbd-config: can't use empty keyname!\n");
        return -1;
    }

    /* Is it a number? */
    scancode = strtol(keyname, &ptr, 0);
    if(ptr != keyname) {
        /* Is it in range? */
        if((scancode < 0) || (scancode >= NR_KEYS)) {
            fprintf(stderr, "svgalib: kbd-config: scancode %s out of range!\n",keyname);
            return -1;
        } else {
#ifdef DEBUG_KEYBOARD
            fprintf(stderr, " mapped numerically to %d.\n", scancode);
#endif
            return scancode;
        }
    }

    /* Look up a symbolic name */
    for(scancode = 0; scancode < NR_KEYS; scancode++) {
#ifdef DEBUG_KEYBOARD
        fprintf(stderr, " (%s)", keynames[scancode]);
#endif
        if(strncasecmp(keyname, keynames[scancode], MAX_KEYNAME_LEN) == 0) {
#ifdef DEBUG_KEYBOARD
            fprintf(stderr, " mapped symbolically to %d", scancode);
            if(usekeymap)
                fprintf(stderr, ", remapped to %d", kbd_mapkey(scancode));
            fprintf(stderr, ".\n");
#endif
            return kbd_mapkey(scancode);
        }
    }

#ifdef DEBUG_KEYBOARD
    fprintf(stderr, " no match!\n");
#endif
    
    /* No match */
    return -1;
}

static void kbd_update_keymap(void)
{
    /* Remap the scancodes of the fake mouse events */
    int n, i=0;
    
    for(n = 0; n < fme_used; n++)
        if(fake_mouse_events[n]) {
            /*for(i = 0; i < fme_numberof[n]; i++)*/
                fake_mouse_events[n][i].scancode =
                    __svgalib_mapkeyname(fake_mouse_events[n][i].keyname);
        }
}
