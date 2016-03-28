
/* Based on:
 * simple driver for serial mouse
 * Andrew Haylett, 14th December 1992
 * and on the driver in XFree86.
 * Edited for svgalib (hhanemaa@cs.ruu.nl).
 * This probably doesn't work with all types of bus mouse.
 * HH: Added PS/2 mouse support.
 * Fixed Logitech support thanks to Daniel Jackson.
 * MouseSystems movement overflow fixed by Steve VanDevender.
 * Logitech fixed again.
 * Michael: Added support for controlling DTR and RTS.
 * Added mouse acceleration 3/97 - Mike Chapman mike@paranoia.com
 * Added Intellimouse support 5/98 - Brion Vibber brion@pobox.com
 * Totally customisable mouse behaviour 28 Mar 1998 - by 101; 101@kempelen.inf.bme.hu
 * Added rx-axis support for IntelliMouse wheel and improved fake keyboard
 *  event scancode setting 7/98 - Brion
 */

/* This file is included by mouse.c. */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <vga.h>
#include <string.h>
#include <math.h>
#include "driver.h"

/* #define DEBUG */
/* #define DEBUG_ACCEL */
/* #define DEBUG_WHEEL */

static int    m_type;
static int    m_baud = 1200;		/* Should be 1200. */
static int    m_sample;
static char*  m_dev;
       int    __svgalib_mouse_fd = -1;
static int    m_fdmode = 0;		/* 0 means don't wait (NDELAY) */
static int    m_modem_ctl = 0;

/* Settings found on mouse open.. Probably std termios should be restored as well */
static unsigned long m_old_modem_info;	/*  original state of DTR/RTS */
static char m_modem_info_valid = 0;	/*  ==0 means: couldn't get it: old kernel? */

unsigned char __svgalib_m_ignore_dx = 0; /* These are flags set by keyboard.c */
unsigned char __svgalib_m_ignore_dy = 0;
unsigned char __svgalib_m_ignore_dz = 0;

static char   m_accel_type   = DEFAULT_ACCEL_TYPE;	/* don't accelerate mouse */
static int    m_accel_thresh = DEFAULT_ACCEL_THRESH;	/* movement threshold */
static float  m_accel_mult   = DEFAULT_ACCEL_MULT;	/* multiply */
static int    m_maxdelta     = DEFAULT_MAXDELTA;	/* before acceleration; no limit by default */
static int    m_accel_maxdelta = DEFAULT_ACCEL_MAXDELTA;/* after acceleration; no limit by default */
static float* m_accel_powertable = 0;   		/* precalculated table for power mode */
static float  m_accel_power  = DEFAULT_ACCEL_POWER;
static float  m_accel_offset = DEFAULT_ACCEL_OFFSET;
static int    m_force	     = 0;			/* Don't force parameters */

static int m_wheel_steps = DEFAULT_WHEEL_STEPS;         /* Number of steps that make up a full turn of the wheel (for IntelliMouse & co) */
static int m_wheel_delta = DEFAULT_WHEEL_DELTA;         /* Amount to change rotation about the X axis when wheel is turned */
static int m_fake_kbd_events = 0;                       /* Create fake keyboard events for turning the wheel */
static int m_fake_upscancode = 0;                       /* Scan code to produce for turning up */
static int m_fake_downscancode = 0;                     /* Scan code to produce for turning down */
static char *m_fake_upkeyname = NULL;                   /* Symbolic key name from which scancode is determined */
static char *m_fake_downkeyname = NULL;

static const unsigned short cflag[] = {
    (CS7 | CREAD | CLOCAL | HUPCL),			/* MicroSoft */
    (CS8 | CSTOPB | CREAD | CLOCAL | HUPCL),		/* MouseSystems */
    (CS8 | PARENB | PARODD | CREAD | CLOCAL | HUPCL),	/* MMSeries */
    (CS8 | CSTOPB | CREAD | CLOCAL | HUPCL),		/* Logitech */
    0,							/* BusMouse */
    0,							/* PS/2 */
    (CS7 | CREAD | CLOCAL | HUPCL),			/* MouseMan */
    (CS8 | CSTOPB | CREAD | CLOCAL | HUPCL),		/* GPM (MouseSystems) */
    (CS8 | CLOCAL | CREAD | IXON | IXOFF ),		/* Spaceball */
    0,							/* Dummy entry for MOUSE_NONE */
    (CS7 | CREAD | CLOCAL | HUPCL),			/* IntelliMouse (Serial) */
    CS7,       						/* IntelliMouse (PS/2) */
};

static const unsigned char proto[][5] =
{
    /*  hd_mask hd_id   dp_mask dp_id   nobytes */
    {0x40, 0x40, 0x40, 0x00, 3},	/* MicroSoft */
    {0xf8, 0x80, 0x00, 0x00, 5},	/* MouseSystems */
    {0xe0, 0x80, 0x80, 0x00, 3},	/* MMSeries */
    {0xe0, 0x80, 0x00, 0x00, 3},	/* Logitech */
    {0xf8, 0x80, 0x00, 0x00, 5},	/* BusMouse */
    {0xc0, 0x00, 0x00, 0x00, 3},	/* PS/2 mouse */
    {0x40, 0x40, 0x40, 0x00, 3},	/* Mouseman */
    {0xf8, 0x80, 0x00, 0x00, 5},	/* gpm (MouseSystems) */
    {0xe0, 0x40, 0x00, 0x00, 6},	/* Spaceball */
    {0, 0, 0, 0, 0},			/* Dummy entry for MOUSE_NONE */
    {0xc0, 0x40, 0xc0, 0x00, 4},	/* IntelliMouse (Serial) */
    {0xc8, 0x08, 0x00, 0x00, 4},	/* IntelliMouse (PS/2) */
};

static void ms_setspeed(const int old, const int new,
			const unsigned short c_cflag)
{
    struct termios tty;
    char *c;

    tcgetattr(__svgalib_mouse_fd, &tty);

    tty.c_iflag = IGNBRK | IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tty.c_line  = 0;
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN]  = 1;

    switch (old) {
    case 9600:
	tty.c_cflag = c_cflag | B9600;
	break;
    case 4800:
	tty.c_cflag = c_cflag | B4800;
	break;
    case 2400:
	tty.c_cflag = c_cflag | B2400;
	break;
    case 1200:
    default:
	tty.c_cflag = c_cflag | B1200;
	break;
    }

    tcsetattr(__svgalib_mouse_fd, TCSAFLUSH, &tty);

    switch (new) {
    case 9600:
	c = "*q";
	tty.c_cflag = c_cflag | B9600;
	break;
    case 4800:
	c = "*p";
	tty.c_cflag = c_cflag | B4800;
	break;
    case 2400:
	c = "*o";
	tty.c_cflag = c_cflag | B2400;
	break;
    case 1200:
    default:
	c = "*n";
	tty.c_cflag = c_cflag | B1200;
	break;
    }

    write(__svgalib_mouse_fd, c, 2);
    usleep(10000);
    tcsetattr(__svgalib_mouse_fd, TCSAFLUSH, &tty);
}

static char *mouse_config_options[] = {
  "mouse_accel_type", "mouse_accel_mult", "mouse_accel_thresh",
  "mouse_accel_power", "mouse_accel_maxdelta", "mouse_maxdelta",
  "mouse_accel_offset", "mouse_override", "mouse_force",
  "mouse_fake_kbd_event", "mouse_wheel_steps", NULL
};

static char* mouse_process_option(int option, int mode) {

 char *ptr;
 int newmtype;

#ifdef DEBUG_ACCEL
  printf("Processing option %ld (%s)\n", (long)option, mouse_config_options[option]);
#endif

   switch (option) {
    float *fptr;
    int   *iptr;
    char  **cptr;

     case 0: /* mouse_accel_type */
	if ( (ptr = strtok(NULL, " ")) == 0 ) {
     param_needed:
	  fprintf(stderr, "svgalib: mouse-config: \'%s\' requires a parameter\n",
	  				mouse_config_options[option]);
	  return ptr;
	} else if (!strcasecmp(ptr, "normal")) {
	  m_accel_type = MOUSE_ACCEL_TYPE_NORMAL;
	} else if (!strcasecmp(ptr, "power")) {
	  m_accel_type = MOUSE_ACCEL_TYPE_POWER;
	} else if (!strcasecmp(ptr, "off")) {
	  m_accel_type = 0;
	} else {
	  fprintf(stderr, "svgalib: mouse-config: ignoring unknown mouse acceleration \'%s\'\n", ptr);
	  m_accel_type = 0;
	}
     	break;

     case 1: /* mouse_accel_mult */
	fptr = &m_accel_mult;

   process_float:
	if ( (ptr = strtok((NULL), " ")) == 0) {
	  goto param_needed;
	} else {
	 char *endptr;
	 double dtmp = strtod(ptr, &endptr);
	  if (endptr == ptr) {
	    fprintf(stderr, "svgalib: mouse-config: illegal float \'%s\' %s\n",
					ptr,  mouse_config_options[option]);
	    return ptr;
	  } else {
	    *fptr = dtmp;
	  }
	}
	break;


     case 2: /* mouse_accel_thresh */
	iptr = &m_accel_thresh;

   process_int:
	if ( (ptr = strtok((NULL), " ")) == 0) {
	  goto param_needed;
	} else {
	 char *endptr;
	 long ltmp = strtol(ptr, &endptr, 10);
	  if (ptr == endptr) {
	    fprintf(stderr, "svgalib: mouse-config: illegal number \'%s\' for %s\n",
					ptr,  mouse_config_options[option]);
	    return ptr;
	  } else {
	    *iptr = ltmp;
	  }
	}
	break;

     case 3: /* mouse_accel_power */
	fptr = &m_accel_power;
	goto process_float;

     case 4: /* mouse_accel_maxdelta */
	iptr = &m_accel_maxdelta;
	goto process_int;

     case 5: /* mouse_maxdelta */
	iptr = &m_maxdelta;
	goto process_int;

     case 6: /* mouse_accel_offset */
	fptr = &m_accel_offset;
	goto process_float;

     case 7: /* mouse_override */
#ifdef ALLOW_MOUSE_OVERRIDE
	newmtype = vga_getmousetype();
	if (m_type != newmtype) {
	    printf("svgalib: mouse-init: mouse type override %d to %d\n",
		   m_type, newmtype);
	    m_type = newmtype;
	}
#else				/* ALLOW_MOUSE_OVERRIDE */
	fputs("svgalib: was compiled with SVGA_MOUSE_OVERRIDE disabled !", stderr);
#endif				/* ALLOW_MOUSE_OVERRIDE */
	break;

     case 8: /* mouse_force */
	m_force = 1;
        break;

     case 9: /* mouse_fake_kbd_event */
         /* Set fake keyboard events for mouse wheel...
            Turn them off temporarily in case the params are bad */
         m_fake_kbd_events = 0;

#ifdef DEBUG_WHEEL
         fprintf(stderr, " Setting up fake keyboard events");
#endif

         /* Parse the config for the scancodes */
         cptr = &m_fake_upkeyname;
         iptr = &m_fake_upscancode;
         while(cptr) {
             if ( (ptr = strtok(NULL, " ")) == 0 )
                 goto param_needed;

#ifdef DEBUG_WHEEL
             fprintf(stderr, "; keyname %s", ptr);
#endif

             /* Keep the name in case the keyboard gets remapped later */
             if(*cptr)
                 free(*cptr);
             *cptr = (char *)malloc(strlen(ptr) + 1);
             strcpy(*cptr, ptr);

             /* Determine the scancode */
             *iptr = __svgalib_mapkeyname(*cptr);

#ifdef DEBUG_WHEEL
             fprintf(stderr, " = scancode %d", *iptr);
#endif

             /* Move on to the next */
             iptr = ((cptr == &m_fake_upkeyname)?(&m_fake_downscancode):NULL);
             cptr = ((cptr == &m_fake_upkeyname)?(&m_fake_downkeyname):NULL);
         }

#ifdef DEBUG_WHEEL
         fprintf(stderr, "; done.\n");
#endif

         /* Now that we've survived all that, turn on fake events */
         m_fake_kbd_events = 1;

         break;

   case 10: /* mouse_wheel_steps */
       /* Set the number of steps that make up a complete turn of
          the wheel on an IntelliMouse or other wheel mouse. Zero
          disables X-axis rotation but not fake keyboard events. */
	iptr = &m_wheel_steps;
	goto process_int;

   }
   return strtok(NULL, " ");
}

static int ms_init(void)
{
/*------------------------------------------------------------------*/
/* Define ALLOW_MOUSE_OVERRIDE to recognize the SVGA_MOUSE_OVERRIDE */
/* environment variable.  If this environment variable is set       */
/* then ignore the program's specified mouse type and use           */
/* the configuration file's type.                                   */
/* In particular, DOOM does not understand "MouseMan" as a valid    */
/* mouse type and so defaults the mouse type to "MouseSystems".     */
/*------------------------------------------------------------------*/
    __svgalib_read_options(mouse_config_options, mouse_process_option);

    if (m_maxdelta < 0  &&  !m_force) {
      fprintf(stderr, "svgalib: mouse_maxdelta value '%ld' is invalid, should be > 0\n", (long)m_maxdelta);
      m_maxdelta = DEFAULT_MAXDELTA;
    }

    if (m_accel_maxdelta < 0  &&  !m_force) {
      fprintf(stderr, "svgalib: mouse_accel_maxdelta value '%ld' is invalid, should be > 0\n", (long)m_accel_maxdelta);
      m_accel_maxdelta = DEFAULT_ACCEL_MAXDELTA;
    }

    if (m_accel_type) {

      if ((m_accel_mult <= 0  ||  m_accel_mult > 200.0)  &&  !m_force) {
        fprintf(stderr, "svgalib: mouse_accel_mult value '%.2f' is invalid, should be between 0 and 200.0\n", (double)m_accel_mult);
        m_accel_mult = DEFAULT_ACCEL_MULT;
      }

      if ((m_accel_power < -200.0  ||  m_accel_power > 200.0)  &&  !m_force) {
        fprintf(stderr, "svgalib: mouse_accel_power value '%.2f' is invalid, should be between -200.0 and 200.0\n", m_accel_power);
	m_accel_power = DEFAULT_ACCEL_POWER;
      }

      if ((m_accel_offset < -100.0  ||  m_accel_offset > 100.0)  &&  !m_force) {
        fprintf(stderr, "svgalib: mouse_accel_offset value '%.2f' is invalid, should be between -100.0 and 100.0\n", m_accel_offset);
	m_accel_offset = DEFAULT_ACCEL_OFFSET;
      }

      if (!m_force) {
         if (m_accel_thresh <= 0  ||  m_accel_thresh > 200) {
            fprintf(stderr, "svgalib: mouse_accel_thresh value '%ld' is invalid, should be between 0 and 200\n", (long)m_accel_thresh);
	    m_accel_thresh = DEFAULT_ACCEL_THRESH;
         }
      } else if (m_accel_thresh < 0) {
	 fputs("svgalib: mouse_accel_thresh musn't be negative !\n", stderr);
	 m_accel_thresh = DEFAULT_ACCEL_THRESH;
      }

      if (m_accel_offset >= m_accel_mult) {
         fputs("svgalib: warning: accel_offset should be less then accel_mult !", stderr);
      }

      if (m_accel_thresh > 1 && m_accel_type == MOUSE_ACCEL_TYPE_POWER) {
        if ((m_accel_powertable = malloc(m_accel_thresh * sizeof(float))) == 0 ) {
  	  fputs("svgalib: out of memory in mouse init ! (Check SVGA_MOUSE_ACCEL_THRESH size) !", stderr);
	  fflush(stderr);
	  return 1;
        } else {
         int i;
#ifdef DEBUG_ACCEL
	  printf("m_accel_powertable:\n");
#endif
          for (i = 1; i < m_accel_thresh; i++) {
            m_accel_powertable[i] = pow((double)(i-1)/(m_accel_thresh-1), m_accel_power)
              * (m_accel_mult - m_accel_offset) + m_accel_offset;

#ifdef DEBUG_ACCEL
	    printf("  %ld => %f (%f)\n", (long)i, (double)m_accel_powertable[i] * i,
	  			     (double)m_accel_powertable[i]);
#endif
          }
        }
      }
    }

#ifdef DEBUG_ACCEL
    printf("m_accel_type: %ld\n",	(long)m_accel_type);
    printf("m_force: %ld\n",		(long)m_force);
    printf("m_accel_thresh: %ld\n",	(long)m_accel_thresh);
    printf("m_accel_offset: %ld\n",	(long)m_accel_offset);
    printf("m_accel_mult: %f\n",	(double)m_accel_mult);
    printf("m_accel_power: %f\n",	(double)m_accel_power);
    printf("m_maxdelta: %ld\n",		(long)m_maxdelta);
    printf("m_accel_maxdelta: %ld\n",	(long)m_accel_maxdelta);
#endif

    /*  Ensure that the open will get a file descriptor greater
     *  than 2, else problems can occur with stdio functions
     *  under certain strange conditions:  */
    if (fcntl(0,F_GETFD) < 0) open("/dev/null", O_RDONLY);
    if (fcntl(1,F_GETFD) < 0) open("/dev/null", O_WRONLY);
    if (fcntl(2,F_GETFD) < 0) open("/dev/null", O_WRONLY);

    /* Set the proper wheel delta */
    if(m_wheel_steps)
        m_wheel_delta = (360 / m_wheel_steps);
    else
        m_wheel_delta = 0;

    /* Added O_NDELAY here. */
    if ((__svgalib_mouse_fd = open(m_dev, O_RDWR | O_NDELAY )) < 0)
	return -1;

    if (m_type == MOUSE_BUSMOUSE || m_type == MOUSE_PS2 || m_type == MOUSE_IMPS2 || m_type == MOUSE_GPM)
	m_modem_ctl = 0;

    /* If no signal will change there is no need to restore
       or safe original settings. */
    if (!m_modem_ctl)
	m_modem_info_valid = 0;
    else {
	/* Get current modem signals; keep silent on errors.. */
	m_modem_info_valid = !ioctl(__svgalib_mouse_fd,
				   TIOCMGET, &m_old_modem_info);

	if (m_modem_info_valid) {
	    unsigned long param = m_old_modem_info;

	    /* Prepare new stat: */

	    /*set DTR as ordered.. */
	    if (m_modem_ctl & MOUSE_CHG_DTR) {
		param &= ~TIOCM_DTR;
		if (m_modem_ctl & MOUSE_DTR_HIGH)
		    param |= TIOCM_DTR;
	    }
	    /*set RTS as ordered.. */
	    if (m_modem_ctl & MOUSE_CHG_RTS) {
		param &= ~TIOCM_RTS;
		if (m_modem_ctl & MOUSE_RTS_HIGH)
		    param |= TIOCM_RTS;
	    }
	    if (ioctl(__svgalib_mouse_fd, TIOCMSET, &param))
		m_modem_info_valid = 0;	/* No try to restore if this failed */
	}
    }

    if (m_type == MOUSE_SPACEBALL) {
      m_baud = 9600;
      ms_setspeed(1200, m_baud, cflag[m_type]);
    } else if (m_type == MOUSE_LOGIMAN) {
	ms_setspeed(9600, 1200, cflag[m_type]);
	ms_setspeed(4800, 1200, cflag[m_type]);
	ms_setspeed(2400, 1200, cflag[m_type]);
	ms_setspeed(1200, 1200, cflag[m_type]);
	write(__svgalib_mouse_fd, "*X", 2);
	ms_setspeed(1200, m_baud, cflag[m_type]);
    } else if (m_type == MOUSE_IMPS2) {
	/* Initialize the mouse into wheel mode */
	write(__svgalib_mouse_fd, "\363\310\363\144\363\120", 6);
    } else if (m_type != MOUSE_BUSMOUSE && m_type != MOUSE_PS2) {
	ms_setspeed(9600, m_baud, cflag[m_type]);
	ms_setspeed(4800, m_baud, cflag[m_type]);
	ms_setspeed(2400, m_baud, cflag[m_type]);
	ms_setspeed(1200, m_baud, cflag[m_type]);

	if (m_type == MOUSE_LOGITECH) {
	    write(__svgalib_mouse_fd, "S", 1);
	    ms_setspeed(m_baud, m_baud, cflag[MOUSE_MMSERIES]);
	}
	if (m_sample <= 0)
	    write(__svgalib_mouse_fd, "O", 1);
	else if (m_sample <= 15)
	    write(__svgalib_mouse_fd, "J", 1);
	else if (m_sample <= 27)
	    write(__svgalib_mouse_fd, "K", 1);
	else if (m_sample <= 42)
	    write(__svgalib_mouse_fd, "L", 1);
	else if (m_sample <= 60)
	    write(__svgalib_mouse_fd, "R", 1);
	else if (m_sample <= 85)
	    write(__svgalib_mouse_fd, "M", 1);
	else if (m_sample <= 125)
	    write(__svgalib_mouse_fd, "Q", 1);
	else
	    write(__svgalib_mouse_fd, "N", 1);
    }

    /* Set the mouse caps */
    mouse_caps.key = MOUSE_GOTCAPS;

    /* Zero out first to be sure... */
    mouse_caps.buttons = mouse_caps.axes = mouse_caps.info =
        mouse_caps.reserved0 = mouse_caps.reserved1 = 0;

    switch(m_type) {
    case MOUSE_SPACEBALL:
        /* 6 axes, many buttons */
        mouse_caps.buttons =
            MOUSE_LEFTBUTTON | MOUSE_MIDDLEBUTTON | MOUSE_RIGHTBUTTON |
            MOUSE_FOURTHBUTTON | MOUSE_FIFTHBUTTON | MOUSE_SIXTHBUTTON |
            MOUSE_RESETBUTTON;
        mouse_caps.axes = MOUSE_6DIM;
        break;
        
    case MOUSE_INTELLIMOUSE:
    case MOUSE_IMPS2:
        /* X, Y, RX (wheel), 3 buttons, wheel */
        mouse_caps.axes = MOUSE_RXDIM;
        mouse_caps.info = MOUSE_INFO_WHEEL;

    case MOUSE_MOUSESYSTEMS:
    case MOUSE_MMSERIES:
    case MOUSE_LOGITECH:
    case MOUSE_BUSMOUSE:
    case MOUSE_PS2:
    case MOUSE_LOGIMAN:
    case MOUSE_GPM:
        /* Any of these _can_ have 3 buttons, but may not */
        mouse_caps.buttons = MOUSE_MIDDLEBUTTON;

    case MOUSE_MICROSOFT: /* Two buttons only */
        mouse_caps.buttons |= MOUSE_LEFTBUTTON | MOUSE_RIGHTBUTTON;
        mouse_caps.axes |= MOUSE_2DIM;
        break;
    }
    
    return 0;
}

/* Scooped from X driver. */
static inline void ms_close(void)
{
    if (m_accel_powertable)
        free(m_accel_powertable);
    if (__svgalib_mouse_fd == -1)
	return;
    if (m_type == MOUSE_LOGITECH) {
	write(__svgalib_mouse_fd, "U", 1);
	ms_setspeed(m_baud, 1200, cflag[MOUSE_LOGITECH]);
    }
    /* Try to restore modem signals if we could get them. */
    if (m_modem_info_valid)
	ioctl(__svgalib_mouse_fd, TIOCMSET, &m_old_modem_info);

    close(__svgalib_mouse_fd);
    __svgalib_mouse_fd = -1;
}

#define MOUSEBUFFERSIZE 256

static int get_ms_event(int wait) {
/*
   Changed to process multiple packets.
   wait value:
   0    Process any mouse events, and return status.
   1    Wait for mouse event, then return.

   Status indicates whether an event was processed.
 */

    static unsigned char buf[MOUSEBUFFERSIZE];
    static int nu_bytes = 0;
    int nu_packets = 0;
    char event_handled = 0;
    int bytesread;
    int i, wheel;
/*  int but; */
    static int but = 0;		/* static is hack for MouseMan */
    static int mouse_orientation = 0;
    int dx=0, dy=0, dz=0, drx=0, dry=0, drz=0;
    int j;
    char SpaceWare[] = "SpaceWare!";

    if (__svgalib_mouse_fd == -1)
	return -1;

  again:

    if (m_fdmode == 1) {
	/* We don't want to wait, set NDELAY mode. */
	fcntl(__svgalib_mouse_fd, F_SETFL, O_RDONLY | O_NDELAY );
	m_fdmode = 0;
    }
#ifdef BACKGROUND
    /* No need to read mouse while background. */
    if (vga_oktowrite()) {
#endif
        bytesread = read(__svgalib_mouse_fd,
		     &buf[nu_bytes], MOUSEBUFFERSIZE - nu_bytes);
#ifdef BACKGROUND
       }
     else
       bytesread=0;
#endif
    i = 0;

    if (bytesread >= 1)
	nu_bytes += bytesread;

#ifdef DEBUG
    printf("#bytes in buffer: %d\n", nu_bytes);
#endif

  handle_packets:

    /* Handle packets in buffer. */

#ifdef DEBUG
    printf("Bytes left in buffer: %d at %d, packet is %d bytes\n",
	   nu_bytes - i, i, proto[m_type][4]);
    if (nu_bytes - i > 0 )
      printf("Header byte: %c %d\n", (buf[i] & 0177), buf[i]);

#endif

    if ((m_type == MOUSE_LOGIMAN) &&
	((nu_bytes - i) >= 1) &&
	((buf[i] & proto[m_type][0]) != proto[m_type][1]) &&
	((char) (buf[i] & ~0x23) == 0)) {
	/* Hack-o-matic, stolen from xf86_Mouse.c */
	but = ((buf[i] & 0x20) >> 4) | (but & 0x05);
	__svgalib_mouse_eventhandler(but, 0, 0, 0, 0, 0, 0);
	event_handled++;
	i++;
    }
    if ((m_type == MOUSE_SPACEBALL)) {
      j=i;
      while ((nu_bytes - j > 0) && (buf[j]!=13))
        j++;
      nu_packets=(buf[j]==13);
    } else {
      nu_packets=1;
    }
    if ((nu_packets==0)||(nu_bytes - i < proto[m_type][4])) {
	/* No full packet available. */
	if (wait == 0 || (wait == 1 && event_handled)) {
	    if (i >= nu_bytes) {
		nu_bytes = 0;
		i = 0;
	    } else {
		/* Move partial packet to front of buffer. */
		for (j = i; j < nu_bytes; j++)
		    buf[j - i] = buf[j];
		nu_bytes -= i;
	    }
	    return event_handled;
	} else {		/* (wait == 1 && !event_handled) */
	    if (i >= nu_bytes) {
		nu_bytes = 0;
		i = 0;
	    }
	    /* Wait mode, we'll sleep on reads. */
	    fcntl(__svgalib_mouse_fd, F_SETFL, O_RDONLY);
	    m_fdmode = 1;
#ifdef BACKGROUND
            /* Waits while foreground again. No reading in background. */
	    while(!vga_oktowrite()) sleep(0);
#endif
	    read(__svgalib_mouse_fd, &buf[nu_bytes], 1);
            if ((m_type == MOUSE_SPACEBALL)) {
              nu_packets=(buf[nu_bytes]==13);
            } else {
              nu_packets=1;
            }
	    nu_bytes++;
	    if ((nu_packets==0)||(nu_bytes - i < proto[m_type][4]))
		/* Not a complete packet. */
		goto again;
	}
    }

    /* Check header byte. */
    if ((buf[i] & proto[m_type][0]) != proto[m_type][1]) {
	/* Not a header byte. */
#ifdef DEBUG
    printf("Bad header byte: %c %d\n", (buf[i] & 0177), buf[i]);
#endif
	i++;
	goto handle_packets;
    }
    /* Check whether it's a valid data packet. */
    if ((m_type != MOUSE_PS2)&&(m_type != MOUSE_IMPS2)&&(m_type != MOUSE_SPACEBALL))
	for (j = 1; j < proto[m_type][4]; j++)
	    if ((buf[i + j] & proto[m_type][2]) != proto[m_type][3]
		|| buf[i + j] == 0x80) {
		i = i + j + 1;
		goto handle_packets;
	    }
    /* Construct the event. */
    switch (m_type) {
    case MOUSE_MICROSOFT:	/* Microsoft */
    case MOUSE_LOGIMAN:	/* MouseMan / TrackMan */
    default:
	but = (but & 2) | ((buf[i] & 0x20) >> 3) | ((buf[i] & 0x10) >> 4);
	dx = (char) (((buf[i] & 0x03) << 6) | (buf[i + 1] & 0x3F));
	dy = (char) (((buf[i] & 0x0C) << 4) | (buf[i + 2] & 0x3F));
        break;
    case MOUSE_INTELLIMOUSE:    /* Serial IntelliMouse */
        /* This bit modified from gpm 1.13 */
        but = ((buf[i] & 0x20) >> 3)         /* left */
               | ((buf[i + 3] & 0x10) >> 3)  /* middle */
               | ((buf[i] & 0x10) >> 4);     /* right */
        dx = (char) (((buf[i] & 0x03) << 6) | (buf[i + 1] & 0x3F));
        dy = (char) (((buf[i] & 0x0C) << 4) | (buf[i + 2] & 0x3F));

        /* Did we turn the wheel? */
        if((wheel = buf[i + 3] & 0x0f) != 0) {
#ifdef DEBUG_WHEEL
            fprintf(stderr, " Wheel turned (0x%02x)", wheel);
#endif
            /* RX-axis */
            if(m_wheel_delta) {
                drx = ((wheel > 7) ? (-m_wheel_delta) : m_wheel_delta);
#ifdef DEBUG_WHEEL
                fprintf(stderr, "; RX axis delta = %d", drx);
#endif
            }

            /* Are fake keyboard events enabled? */
            if(m_fake_kbd_events && __svgalib_keyboard_eventhandler) {
                /* Fake keypresses... */
                int key = ((wheel > 7) ? m_fake_upscancode : m_fake_downscancode);
#ifdef DEBUG_WHEEL
                fprintf(stderr, "; pressing scancode %d", key);
#endif
                __svgalib_keyboard_eventhandler(key, 1); /* press */
                /* todo: delay */
#ifdef DEBUG_WHEEL
                fprintf(stderr, ", releasing scancode %d", key);
#endif
                __svgalib_keyboard_eventhandler(key, 0); /* release */
#ifdef DEBUG_WHEEL
            fprintf(stderr, ".\n");
#endif
            }
        }
        break;
    case MOUSE_MOUSESYSTEMS:	/* Mouse Systems Corp */
    case MOUSE_GPM:
	but = (~buf[i]) & 0x07;
	dx = (char) (buf[i + 1]);
	dx += (char) (buf[i + 3]);
	dy = -((char) (buf[i + 2]));
	dy -= (char) (buf[i + 4]);
	break;
    case MOUSE_MMSERIES:	/* MM Series */
    case MOUSE_LOGITECH:	/* Logitech */
	but = buf[i] & 0x07;
	dx = (buf[i] & 0x10) ? buf[i + 1] : -buf[i + 1];
	dy = (buf[i] & 0x08) ? -buf[i + 2] : buf[i + 2];
	break;
    case MOUSE_BUSMOUSE:	/* BusMouse */
	but = (~buf[i]) & 0x07;
	dx = (char) buf[i + 1];
	dy = -(char) buf[i + 2];
	break;
    case MOUSE_PS2:		/* PS/2 mouse */
	but = (buf[i] & 0x04) >> 1 |	/* Middle */
	    (buf[i] & 0x02) >> 1 |	/* Right */
	    (buf[i] & 0x01) << 2;	/* Left */
	dx = (buf[i] & 0x10) ? buf[i + 1] - 256 : buf[i + 1];
	dy = (buf[i] & 0x20) ? -(buf[i + 2] - 256) : -buf[i + 2];
	break;
    case MOUSE_IMPS2:           /* PS/2 IntelliMouse */
        /* This bit modified from the gpm 1.13 imps2 patch by Tim Goodwin */
        but = ((buf[i] & 1) << 2) /* left */
            | ((buf[i] & 6) >> 1); /* middle and right */
        dx = (buf[i] & 0x10) ? buf[i + 1] - 256 : buf[i + 1];
        dy = (buf[i] & 0x20) ? -(buf[i + 2] - 256) : -buf[i + 2];

        /* Did we turn the wheel? */
        if((wheel = buf[i + 3]) != 0) {
#ifdef DEBUG_WHEEL
            fprintf(stderr, " Wheel turned (0x%02x)", wheel);
#endif
            /* RX-axis */
            if(m_wheel_delta) {
                drx = ((wheel > 0x7f) ? (-m_wheel_delta) : m_wheel_delta);
#ifdef DEBUG_WHEEL
                fprintf(stderr, "; RX axis delta = %d", drx);
#endif
            }

            /* Are fake keyboard events enabled? */
            if(m_fake_kbd_events && __svgalib_keyboard_eventhandler) {
                /* Fake keypresses... */
                int key = ((wheel > 0x7f) ? m_fake_upscancode : m_fake_downscancode);
#ifdef DEBUG_WHEEL
                fprintf(stderr, "; pressing scancode %d", key);
#endif
                 __svgalib_keyboard_eventhandler(key, 1); /* press */
                /* todo: delay */
#ifdef DEBUG_WHEEL
                fprintf(stderr, ", releasing scancode %d", key);
#endif
                __svgalib_keyboard_eventhandler(key, 0); /* release */
            }
#ifdef DEBUG_WHEEL
            fprintf(stderr, ".\n");
#endif
        }
        break;
    case MOUSE_SPACEBALL:

	switch (buf[i]) {
	  case 'D':
	
	    but=0177 & buf[i+1];

	    /* Strip the MSB, which is a parity bit */
	    for (j = 2; j < 11; ++j) {
	        buf[i+j] &= 0177;           /* Make sure everything is 7bit */
	        buf[i+j] ^= SpaceWare[j-2]; /* What's this doing in the data? */
	    }

	    /* Turn chars into 10 bit integers */
	    if (mouse_orientation == MOUSE_ORIENTATION_VERTICAL) {
	      dx = ((buf[i+2] & 0177)<<3)|((buf[i+3] & 0160)>>4);
	      dy = ((buf[i+3] & 0017)<<6)|((buf[i+4] & 0176)>>1);
	      dz = ((buf[i+4] & 0001)<<9)|((buf[i+5] & 0177)<<2)|
                    ((buf[i+6] & 0140)>>5);
	      drx = ((buf[i+6] & 0037)<<5)|((buf[i+7] & 0174)>>2);
	      dry = ((buf[i+7] & 0003)<<8)|((buf[i+8] & 0177)<<1)|
                     ((buf[i+9] & 0100)>>6);
	      drz = ((buf[i+9] & 0077)<<4)|((buf[i+10] & 0170)>>3);
	    } else {
	      dx = ((buf[i+2] & 0177)<<3)|((buf[i+3] & 0160)>>4);
	      dz = ((buf[i+3] & 0017)<<6)|((buf[i+4] & 0176)>>1);
	      dy = ((buf[i+4] & 0001)<<9)|((buf[i+5] & 0177)<<2)|
                    ((buf[i+6] & 0140)>>5);
	      drx = ((buf[i+6] & 0037)<<5)|((buf[i+7] & 0174)>>2);
	      drz = ((buf[i+7] & 0003)<<8)|((buf[i+8] & 0177)<<1)|
                     ((buf[i+9] & 0100)>>6);
	      dry = ((buf[i+9] & 0077)<<4)|((buf[i+10] & 0170)>>3);
	    }

	    /* Get the sign right. */
	    if (dx > 511) dx -= 1024;
	    if (dy > 511) dy -= 1024;
	    if (dz > 511) dz -= 1024;
	    if (drx > 511) drx -= 1024;
	    if (dry > 511) dry -= 1024;
	    if (drz > 511) drz -= 1024;
            if (mouse_orientation == MOUSE_ORIENTATION_HORIZONTAL)
              { dz *= -1;
                drz *= -1;
              }
	    /*    if (fabs(dx) < sorb_trans_thresh[1]) dx = 0; */
            i+=13;
#ifdef DEBUG
    printf("Got D packet! but=%d, x=%d y=%d z=%d rx=%d ry=%d rz=%d\n",
            but,dx,dy,dz,drx,dry,drz);
#endif
            break;
	  case 'K':
	/* Button press/release w/out motion */
	    but=0177 & buf[i+2];
	    if (but==MOUSE_RESETBUTTON)
	      mouse_orientation=1-mouse_orientation;
#ifdef DEBUG
    printf("Got K packet! but=%d, x=%d y=%d z=%d rx=%d ry=%d rz=%d\n",
            but,dx,dy,dz,drx,dry,drz);
#endif
	    i+=6;
	    break;
	  case 'R':
#ifdef DEBUG
    printf("Got init string!\n");
#endif
	    for (j=i;((buf[j] !=13)&&(j<nu_bytes));j++)
              printf("%c",(buf[j] & 0177));
            printf("\n\n");
            i=j+1;
            break;
	  default:
#ifdef DEBUG
    printf("Got unknown packet!\n");
#endif
            i++;
            break;
	}

	break;
    }

    if (m_type != MOUSE_SPACEBALL)
      i += proto[m_type][4];

    /* Try to snag that optional mouseman fourth byte, if present */
    if ((m_type == MOUSE_LOGIMAN) &&
	((nu_bytes - i) >= 1) &&
	((buf[i] & proto[m_type][0]) != proto[m_type][1]) &&
	((char) (buf[i] & ~0x23) == 0)) {
	/* Hack-o-matic, stolen from xf86_Mouse.c */
	but = ((buf[i] & 0x20) >> 4) | (but & 0x05);
	i++;
    }

    if (__svgalib_m_ignore_dx)  dx = 0;
    if (__svgalib_m_ignore_dy)  dy = 0;
    if (__svgalib_m_ignore_dz)  dz = 0;


    if (m_maxdelta) {
      if (abs(dx) > m_maxdelta)
        dx = (dx > 0) ? m_maxdelta : -m_maxdelta;
      if (abs(dy) > m_maxdelta)
        dy = (dy > 0) ? m_maxdelta : -m_maxdelta;
      if (abs(dz) > m_maxdelta)
        dz = (dz > 0) ? m_maxdelta : -m_maxdelta;
    }


    switch (m_accel_type) {
      int delta;

      case MOUSE_ACCEL_TYPE_NORMAL:
#ifdef DEBUG_ACCEL 
  printf("%ld\t", (long)dx);
#endif
	if (abs(dx) > m_accel_thresh) dx = (int) ((float)dx * m_accel_mult);
	if (abs(dy) > m_accel_thresh) dy = (int) ((float)dy * m_accel_mult);
	if (abs(dz) > m_accel_thresh) dz = (int) ((float)dz * m_accel_mult);
#ifdef DEBUG_ACCEL
  printf("%ld\n", (long)dx);
#endif
        break;


      case MOUSE_ACCEL_TYPE_POWER:
#ifdef DEBUG_ACCEL 
  printf("%ld\t", (long)dx);
#endif
	delta = abs(dx);
	dx = (delta >= m_accel_thresh)  ?  (float)dx * m_accel_mult :
	     (float)dx * m_accel_powertable[delta];

	delta = abs(dy);
	dy = (delta >= m_accel_thresh)  ?  (float)dy * m_accel_mult :
	     (float)dy * m_accel_powertable[delta];

	delta = abs(dz);
	dz = (delta >= m_accel_thresh)  ?  (float)dz * m_accel_mult :
	     (float)dz * m_accel_powertable[delta];

#ifdef DEBUG_ACCEL
  printf("%ld\n", (long)dx);
#endif
        break;
    }

    if (m_accel_maxdelta  &&  m_accel_type) {
      if (abs(dx) > m_accel_maxdelta)
        dx = (dx > 0) ? m_accel_maxdelta : -m_accel_maxdelta;
      if (abs(dy) > m_accel_maxdelta)
        dy = (dy > 0) ? m_accel_maxdelta : -m_accel_maxdelta;
      if (abs(dz) > m_accel_maxdelta)
        dz = (dz > 0) ? m_accel_maxdelta : -m_accel_maxdelta;
    }

    __svgalib_mouse_eventhandler(but, dx, dy, dz, drx, dry, drz);

    event_handled = 1;

    goto handle_packets;
}

void __svgalib_mouse_update_keymap(void)
{
    /*
     The keyboard has been remapped, and we should accordingly remap any
     symbolically-specified scancodes for fake keyboard events.
     */
#ifdef DEBUG_WHEEL
    fprintf(stderr, " Remapping fake keyboard event scancodes... ");
#endif
    if(m_fake_upkeyname)
        m_fake_upscancode = __svgalib_mapkeyname(m_fake_upkeyname);
    if(m_fake_downkeyname)
        m_fake_downscancode = __svgalib_mapkeyname(m_fake_downkeyname);
#ifdef DEBUG_WHEEL
    fprintf(stderr, "done.\n");
#endif
}
