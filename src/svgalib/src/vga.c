/****************************************************************************
*
*                         SciTech Display Doctor
*
*               Copyright (C) 1991-2004 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  IBM PC (Linux/i386)
*
* Description:  Main SciTech SNAP Graphics driver for SVGAlib.
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>		/* for printf */
#include <string.h>		/* for memset */
#include <sys/mman.h>		/* mmap */
#include <sys/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>
#include <sys/wait.h>
#ifdef __GLIBC__
#include <sys/perm.h>
#endif
#include <asm/io.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>

#include <vga.h>
#include "mouse/vgamouse.h"
#include "keyboard/vgakeyboard.h"
#include "driver.h"

/* SciTech includes */
#include "pmapi.h"
#include "snap/graphics.h"
#include "snap/ref2d.h"

/******* Exported variable *******/
unsigned char *graph_mem = NULL;

#define SETSIG(sa, sig, fun) {\
	sa.sa_handler = fun; \
	sa.sa_flags = SA_RESTART; \
	memset(&sa.sa_mask, 0, sizeof(sigset_t)); \
	sigaction(sig, &sa, NULL); \
}

static int snap_flags = 0, snap_mode;
static N_int32 snap_transparent = 0, snap_transcol, snap_oldmode;


static GA_palette snap_palette[256] = {{0}};
static int snap_color; // Current color
static N_int32 bytes_per_pixel, current_mode = TEXT;
extern N_int32 bytesPerLine, maxY;
static N_int32 snap_maxmem;

static int forbidvtrelease = 0;
static int forbidvtacquire = 0;
static struct vt_mode __svgalib_oldvtmode;

static void *fontbuf;
static int fontbufsize = 65536;

static int flip = 0;		/* flag: executing vga_flip() ? */
static int flip_mode = TEXT;	/* flipped video mode       */

static int lock_count = 0;
static int release_flag = 0;

int __svgalib_tty_fd = -1;	/* /dev/tty file descriptor */
int __svgalib_mem_fd = -1;	/* /dev/mem file descriptor  */
int __svgalib_driver_report = 1;
static int svgalib_vc = -1, startup_vc = -1, initialized = 0, my_pid;
static struct termios text_termio;	/* text mode termio parameters     */
static struct termios graph_termio;	/* graphics mode termio parameters */

static int mouse_support = 0, mouse_mode = 0;
extern int mouse_type, mouse_modem_ctl;
extern char *mouse_device;

static int page_offset = 0, using_linear = 0;
static void *page_emul_adr = 0;

static int __svgalib_oktowrite=1;
static int __svgalib_runinbackground = 0;

static GA_modeInfo snap_modeInfo;

static int SVGALIB_SNAP[__GLASTMODE+1] = {0};

static int claimed_mem;
static void *saved_mem = NULL;

static GA_devCtx       *DC;
static GA_initFuncs    *finit;
static GA_driverFuncs  *fdriver;
static REF2D_driver *ref2d;
static GA_2DRenderFuncs draw2d = {0};
static GA_2DStateFuncs state2d = {0};
static PE_MODULE *pemod;

static int saved_logicalwidth;
static int saved_displaystart, current_displaystart = 0;
static void *console_state, *vga_state;

/* We invoke the old interrupt handler after setting text mode */
/* We catch all signals that cause an exit by default (aka almost all) */
static char sig2catch[] =
{SIGHUP, SIGINT, SIGQUIT, SIGILL,
 SIGTRAP, SIGIOT, SIGBUS, SIGFPE,
 SIGSEGV, SIGPIPE, SIGALRM, SIGTERM,
 SIGXCPU, SIGXFSZ, SIGVTALRM,
/* SIGPROF ,*/ SIGPWR};
static struct sigaction old_signal_handler[sizeof(sig2catch)];

static void  __svgalib_snapaccel_ScreenCopy(int x1, int y1, int x2, int y2, int width, int height);
void (*ScreenCopy) (int x1, int y1, int x2, int y2, int width, int height) = __svgalib_snapaccel_ScreenCopy;

static void fill_snap_modes(void);
static void __svgalib_waitvtactive(void);
static void __svgalib_takevtcontrol(void);

extern void __svgalib_readconfigfile(void);

static void vga_drawpixel4(int x, int y);
static void vga_drawpixel8(int x, int y);
static void vga_drawpixel16(int x, int y);
static void vga_drawpixel24(int x, int y);
static void vga_drawpixel32(int x, int y);

void    NAPI BNK4_clear(void);
void    NAPI BNK4_drawPixel(N_int32 x,N_int32 y,GA_color color);
extern void    (NAPIP BNK_setBank)(N_int32 bank);
extern void	*bankBuffer;
extern int	cntBank,bankStart,bankOffset;

static void (*__vga_drawpixel)(int x, int y);

static int vga_getpixel4(int x, char *ptr);
static int vga_getpixel8(int x, char *ptr);
static int vga_getpixel16(int x, char *ptr);
static int vga_getpixel24(int x, char *ptr);
static int vga_getpixel32(int x, char *ptr);

static int (*__vga_getpixel)(int x, char *ptr);


/****************************************************************************
REMARKS:
Sets the text mode with the specified dimensions
****************************************************************************/
static void SetTextMode(int x, int y)
{
        GA_modeInfo     modeInfo;
	//        GA_CRTCInfo     crtc;
        N_uint16        *modes;
        int             xChars,yChars,textMode = -1;
        N_int32 virtualX, virtualY, bytesPerLine, maxMem;

        /* Search for the text mode to use with the same resolution */
        for (modes = DC->AvailableModes; *modes != 0xFFFF; modes++) {
            modeInfo.dwSize = sizeof(modeInfo);
            if (finit->GetVideoModeInfo(*modes,&modeInfo) != 0)
                continue;
            if (!(modeInfo.Attributes & gaIsTextMode))
                continue;
            xChars = modeInfo.XResolution / modeInfo.XCharSize;
            yChars = modeInfo.YResolution / modeInfo.YCharSize;
            if (xChars == x && yChars == y) {
                textMode = *modes;
                break;
            }
        }
	if(state2d.WaitTillIdle)
	  state2d.WaitTillIdle();
        if (textMode == -1) {
	  /* Set 80x25 text mode */
	  finit->SetVideoMode(3,&virtualX,&virtualY,&bytesPerLine,&maxMem,0, NULL); // &crtc);
        } else {
	  virtualX = virtualY = bytesPerLine = -1;
	  finit->SetVideoMode(textMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL); //&crtc);
        }
}

extern int __svgalib_kbd_fd;

static void set_graphtermio(void)
{
    /* Leave keyboard alone when rawkeyboard is enabled! */
    if (__svgalib_kbd_fd < 0) {
	/* set graphics mode termio parameters */
	ioctl(0, TCSETSW, &graph_termio);
    }
    ioctl(__svgalib_tty_fd, KDSETMODE, KD_GRAPHICS);
}


static void set_texttermio(void)
{
    /* Leave keyboard alone when rawkeyboard is enabled! */
    if (__svgalib_kbd_fd < 0) {
	/* restore text mode termio parameters */
	ioctl(0, TCSETSW, &text_termio);
    }
}

static void restoretextmode(void)
{
  N_uint32 dummy = -1;

  //  keyboard_close();
  if(state2d.WaitTillIdle)
    state2d.WaitTillIdle();

  GA_restoreCRTCTimings(DC);
  if(!snap_oldmode)
    SetTextMode(80,25);
  else
    finit->SetVideoMode(snap_oldmode,&dummy,&dummy,&dummy,&snap_maxmem,0,NULL);
  // PM_restoreConsoleState(console_state,__svgalib_tty_fd);
  PM_restoreVGAState(vga_state);
  ioctl(__svgalib_tty_fd, KDSETMODE, KD_TEXT); // already called within restoreConsoleState
  if(ioctl(__svgalib_tty_fd, PIO_FONT, fontbuf)<0){
    perror("ioctl(PIO_FONT)");
  }
  set_texttermio();
}

static void savestate(void)
{
  saved_logicalwidth = bytesPerLine;
  if(saved_mem)
    free(saved_mem);
  saved_mem = malloc(claimed_mem);
  memcpy(saved_mem, DC->LinearMem, claimed_mem);
}

static void restorestate(void)
{
  if(snap_modeInfo.BitsPerPixel <= 8)
    fdriver->SetPaletteData(snap_palette, 256, 0, false);
  memcpy(DC->LinearMem, saved_mem, claimed_mem);
  vga_setdisplaystart(saved_displaystart);
}

static void __svgalib_flipback(void)
{
    __joystick_flip_vc(1);
    if (flip_mode != TEXT) {
	__svgalib_oktowrite=1;
	flip = 1;
	vga_setmode(flip_mode);
	flip = 0;
	restorestate();
    }
}

static void __svgalib_flipaway(void)
{
  if(current_mode != TEXT){
	savestate();
	flip_mode = current_mode;
	flip = 1;
	vga_setmode(TEXT);
	flip = 0;
	__svgalib_oktowrite=0; /* screen is fliped, not __svgalib_oktowrite. */
  }else{
        __svgalib_oktowrite=1;
	flip = 1;
	vga_setmode(flip_mode);
	flip = 0;
	restorestate();
  }
}

static void __svgalib_releasevt_signal(int n)
{
    if (lock_count) {
	release_flag = 1;
	return;
    }
#ifdef DEBUG
    printf("Release request.\n");
#endif
    forbidvtacquire = 1;
    if (forbidvtrelease) {
	forbidvtacquire = 0;
	ioctl(__svgalib_tty_fd, VT_RELDISP, 0);
	return;
    }
    __svgalib_flipaway();
    ioctl(__svgalib_tty_fd, VT_RELDISP, 1);
#ifdef DEBUG
    printf("Finished release.\n");
#endif
    forbidvtacquire = 0;

    /* Suspend program until switched to again. */
#ifdef DEBUG
    printf("Suspended.\n");
#endif

    __svgalib_oktowrite = 0;
    if (!__svgalib_runinbackground)
	__svgalib_waitvtactive();
#ifdef DEBUG
    printf("Waked.\n");
#endif
}

static void __svgalib_acquirevt_signal(int n)
{
#ifdef DEBUG
    printf("Acquisition request.\n");
#endif
    forbidvtrelease = 1;
    if (forbidvtacquire) {
	forbidvtrelease = 0;
	return;
    }
    __svgalib_flipback();
    ioctl(__svgalib_tty_fd, VT_RELDISP, VT_ACKACQ);
#ifdef DEBUG
    printf("Finished acquisition.\n");
#endif
    forbidvtrelease = 0;
    __svgalib_oktowrite = 1;
}

static int check_owner(int vc)
{
    struct stat sbuf;
    char fname[30];

#ifdef ROOT_VC_SHORTCUT
    if (!getuid())
        return 1;               /* root can do it always */
#endif
    sprintf(fname, "/dev/tty%d", vc);
    if ((stat(fname, &sbuf) >= 0) && (getuid() == sbuf.st_uid)) {
        return 1;
    }
    printf("You must be the owner of the current console to use svgalib.\n");
    return 0;
}

static void disable_interrupt(void)
{
    struct termios cur_termio;

    /* Well, one could argue that sigint is not enabled at all when in __svgalib_nosigint
       but sometimes they *still* are enabled b4 graph_termio is set.. */
    ioctl(0, TCGETS, &cur_termio);
    cur_termio.c_lflag &= ~ISIG;
    ioctl(0, TCSETSW, &cur_termio);
}

static void __vga_atexit(void)
{
    if (getpid() == my_pid)	/* protect against forked processes */
	restoretextmode();
    if (__svgalib_tty_fd >= 0 && startup_vc > 0)
      ioctl(__svgalib_tty_fd, VT_ACTIVATE, startup_vc);
    keyboard_close();
    free(console_state); free(vga_state);
    if(pemod)
      PE_freeLibrary(pemod);
}

static void signal_handler(int v)
{
    int i;

    /* If we have accelerated functions, possibly wait for the
     * blitter to finish. I hope the PutBitmap functions disable
     * interrupts while writing data to the screen, otherwise
     * this will cause an infinite loop.
     */
    if(state2d.WaitTillIdle)
      state2d.WaitTillIdle();

    for (i = 0; i < sizeof(sig2catch); i++)
	if (sig2catch[i] == v) {
	    sigaction(v, old_signal_handler + i, NULL);
	    raise(v);
	    break;
	}
    restoretextmode();
    printf("svgalib: Signal %d: %s received%s.\n", v, strsignal(v),
	   (v == SIGINT) ? " (ctrl-c pressed)" : "");
    if (i >= sizeof(sig2catch)) {
	printf("svgalib: Aieeee! Illegal call to signal_handler, raising segfault.\n");
	raise(SIGSEGV);
    }
}

void __svgalib_open_devconsole(void)
{
    struct vt_mode vtm;
    struct vt_stat vts;
    struct stat sbuf;
    char fname[30];

    if (__svgalib_tty_fd >= 0)
        return;

    /*  The code below assumes file descriptors 0, 1, and 2
     *  are already open; make sure that's true.  */
    if (fcntl(0,F_GETFD) < 0) open("/dev/null", O_RDONLY);
    if (fcntl(1,F_GETFD) < 0) open("/dev/null", O_WRONLY);
    if (fcntl(2,F_GETFD) < 0) open("/dev/null", O_WRONLY);

    /*
     * Now, it would be great if we could use /dev/tty and see what it is connected to.
     * Alas, we cannot find out reliably what VC /dev/tty is bound to. Thus we parse
     * stdin through stderr for a reliable VC
     */
    for (__svgalib_tty_fd = 0; __svgalib_tty_fd < 3; __svgalib_tty_fd++) {
        if (fstat(__svgalib_tty_fd, &sbuf) < 0)
            continue;
        if (ioctl(__svgalib_tty_fd, VT_GETMODE, &vtm) < 0)
            continue;
        if ((sbuf.st_rdev & 0xff00) != 0x400)
            continue;
        if (!(sbuf.st_rdev & 0xff))
            continue;
        svgalib_vc = sbuf.st_rdev & 0xff;
        return;                 /* perfect */
    }

    if ((__svgalib_tty_fd = open("/dev/console", O_RDWR)) < 0) {
        printf("svgalib: can't open /dev/console \n");
        exit(1);
    }
    if (ioctl(__svgalib_tty_fd, VT_OPENQRY, &svgalib_vc) < 0)
        goto error;
    if (svgalib_vc <= 0)
        goto error;
    sprintf(fname, "/dev/tty%d", svgalib_vc);
    close(__svgalib_tty_fd);
    /* change our control terminal: */
    setsid();
    /* We must use RDWR to allow for output... */
    if (((__svgalib_tty_fd = open(fname, O_RDWR)) >= 0) &&
        (ioctl(__svgalib_tty_fd, VT_GETSTATE, &vts) >= 0)) {
        if (!check_owner(vts.v_active))
            goto error;
        /* success, redirect all stdios */
        if (__svgalib_driver_report)
            printf("[svgalib: allocated virtual console #%d]\n", svgalib_vc);
        fflush(stdin);
        fflush(stdout);
        fflush(stderr);
        close(0);
        close(1);
        close(2);
        dup(__svgalib_tty_fd);
        dup(__svgalib_tty_fd);
        dup(__svgalib_tty_fd);
        /* clear screen and switch to it */
        fwrite("\e[H\e[J", 6, 1, stderr);
        fflush(stderr);
        if (svgalib_vc != vts.v_active) {
            startup_vc = vts.v_active;
	    ioctl(__svgalib_tty_fd, VT_ACTIVATE, svgalib_vc);
            __svgalib_waitvtactive();
	}
    } else {
error:
    if (__svgalib_tty_fd > 2)
	close(__svgalib_tty_fd);
    __svgalib_tty_fd = - 1;
    printf("Not running in a graphics capable console,\n"
	 "and unable to find one.\n");
    }
}

static void initialize(void)
{
    int i;
    struct sigaction siga;

    if(initialized)
      return;

    __svgalib_open_devconsole();
    if (__svgalib_tty_fd < 0) {
	exit(1);
    }

    /* Make sure that textmode is restored at exit(). */
    if (my_pid == 0)
	my_pid = getpid();
    atexit(__vga_atexit);

#ifndef DONT_WAIT_VC_ACTIVE
    __svgalib_waitvtactive();
#endif

    /* save text mode termio parameters */
    ioctl(0, TCGETS, &text_termio);

    graph_termio = text_termio;

    /* change termio parameters to allow our own I/O processing */
    graph_termio.c_iflag &= ~(BRKINT | PARMRK | INPCK | IUCLC | IXON | IXOFF);
    graph_termio.c_iflag |= (IGNBRK | IGNPAR);

    graph_termio.c_oflag &= ~(ONOCR);

    graph_termio.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH);

    graph_termio.c_lflag |=  ISIG;	/* enable interrupt */

    graph_termio.c_cc[VMIN] = 1;
    graph_termio.c_cc[VTIME] = 0;
    graph_termio.c_cc[VSUSP] = 0;	/* disable suspend */

    disable_interrupt();	/* Is reenabled later by set_texttermio */

    __svgalib_takevtcontrol();	/* HH: Take control over VT */

    initialized = 1;

    /* do our own interrupt handling */
    for (i = 0; i < sizeof(sig2catch); i++) {
	siga.sa_handler = signal_handler;
	siga.sa_flags = 0;
	memset(&siga.sa_mask, 0, sizeof(sigset_t));
	sigaction((int) sig2catch[i], &siga, old_signal_handler + i);
    }

    /* vga_unlockvc(); */
}

int vga_setmode(int mode)
{
    N_int32 dummy = -1;

    if(!DC)
      vga_init();

    if(!initialized)
      initialize();

    printf("Setting mode %d.\n", mode);

    if(mode==TEXT){
      restoretextmode();
      return 0;
    }

    if(!SVGALIB_SNAP[mode])
	return 1;

    snap_modeInfo.dwSize = sizeof(snap_modeInfo);
    if(!finit->GetVideoModeInfo(SVGALIB_SNAP[mode], &snap_modeInfo)){
      GA_2DStateFuncs  *ref_state2d;
      GA_2DRenderFuncs *ref_draw2d;
      GA_buffer buf;
      ulong size;

      if(snap_modeInfo.BitsPerPixel < 8)
		fprintf(stderr, "WARNING: The SNAP Graphics SVGALIB driver doesn't support < 8bpp modes well yet.\n");

	/*  Load the SNAP Graphics reference rasteriser for the color depth */
      else if (!REF2D_loadDriver(DC,snap_modeInfo.BitsPerPixel,false,&ref2d,&pemod, &size)){
		PM_fatalError("Unable to load SNAP Graphics Reference Rasteriser!");
		return 0;
      }

      snap_flags = 0;
      if(snap_modeInfo.Attributes & gaHaveLinearBuffer)
		snap_flags |= gaLinearBuffer;
      else
		using_linear = 0;

      snap_mode = SVGALIB_SNAP[mode];

      //      bytesPerLine = snap_modeInfo.BytesPerScanLine;
      if(flip){
		bytesPerLine = saved_logicalwidth;
      }else{
		bytesPerLine = -1;
      }
      if(state2d.WaitTillIdle)
		state2d.WaitTillIdle();
      GA_restoreCRTCTimings(DC);
      if(finit->SetVideoMode(snap_mode | snap_flags,
							 &dummy, &dummy, &bytesPerLine, &snap_maxmem, 0, NULL ))
		return 1;

      buf.dwSize = sizeof(buf);
      buf.Offset = 0;
      buf.Stride = bytesPerLine;
      buf.Width = snap_modeInfo.XResolution;
      buf.Height = snap_modeInfo.YResolution;

      maxY = snap_modeInfo.YResolution - 1;

      if(snap_modeInfo.BitsPerPixel >= 8){ // TODO: Fix this for < 8
		ref2d->SetDrawBuffer(&buf, DC->LinearMem, snap_modeInfo.BitsPerPixel, &snap_modeInfo.PixelFormat, DC, FALSE);

		ref_state2d = ref2d->QueryFunctions(GA_GET_2DSTATEFUNCS);
		ref_draw2d = ref2d->QueryFunctions(GA_GET_2DRENDERFUNCS);
		draw2d = *ref_draw2d;
		state2d = *ref_state2d;
		ref_state2d->SetMix(GA_REPLACE_MIX);
      }else{
	// ref_state2d = DC->QueryFunctions(GA_GET_2DSTATEFUNCS,0);
	bankBuffer = DC->BankedMem;
	BNK_setBank = fdriver->SetBank;
	bankStart = bankOffset = cntBank = 0;

	memset(&draw2d, 0, sizeof(draw2d));
	memset(&state2d, 0, sizeof(state2d));
      }
      claimed_mem = buf.Height * bytesPerLine;

      switch(snap_modeInfo.BitsPerPixel){
      case 4:
	__vga_drawpixel = vga_drawpixel4;
	__vga_getpixel = vga_getpixel4;
	break;
      case 8:
	__vga_drawpixel = vga_drawpixel8;
	__vga_getpixel = vga_getpixel8;
	break;
      case 15: case 16:
	__vga_drawpixel = vga_drawpixel16;
	__vga_getpixel = vga_getpixel16;
	break;
      case 24:
	__vga_drawpixel = vga_drawpixel24;
	__vga_getpixel = vga_getpixel24;
	break;
      case 32:
	__vga_drawpixel = vga_drawpixel32;
	__vga_getpixel = vga_getpixel32;
	break;
      }
      current_mode = mode;
      bytes_per_pixel = snap_modeInfo.BitsPerPixel / 8;

      set_graphtermio();

      if (mouse_support) {
#ifdef DEBUG
	printf("svgalib: Opening mouse (type = %x).\n", mouse_type | mouse_modem_ctl);
#endif
	if (mouse_init(mouse_device, mouse_type | mouse_modem_ctl, MOUSE_DEFAULTSAMPLERATE))
	  printf("svgalib: Failed to initialize mouse.\n");
	else {
	  /* vga_lockvc(); */
	  mouse_setxrange(0, buf.Width - 1);
	  mouse_setyrange(0, buf.Height - 1);
	  mouse_setwrap(MOUSE_NOWRAP);
	  mouse_mode = mode;
	}
      }
      return 0;
    }
    return 1;
}

/* The following is rather messy and inelegant. The only solution I can */
/* see is getting a extra free VT for graphics like XFree86 does. */

static void __svgalib_waitvtactive(void)
{
    if (__svgalib_tty_fd < 0)
	return; /* Not yet initialized */

    while (ioctl(__svgalib_tty_fd, VT_WAITACTIVE, svgalib_vc) < 0) {
	if ((errno != EAGAIN) && (errno != EINTR)) {
	    perror("ioctl(VT_WAITACTIVE)");
	    exit(1);
	}
	usleep(150000);
    }
}

int vga_hasmode(int mode)
{
    if (mode == TEXT)
	return 1;
    if(!DC)
      vga_init();
    if (mode < 0 || mode > __GLASTMODE)
      return 0;
/*     return snap_findmode(convert_modes[mode].resx, */
/* 			    convert_modes[mode].resy, */
/* 			    convert_modes[mode].bpp */
/* 			    ) != -1; */
    return SVGALIB_SNAP[mode] != 0;
}

int vga_setflipchar(int c)
{
  //    __svgalib_flipchar = c;
    return 0;
}

int vga_getmousetype(void)
{
  __svgalib_readconfigfile();
  return mouse_type | mouse_modem_ctl;
}

void vga_setmousesupport(int s)
{
    mouse_support = s;
}

int vga_clear(void)
{
  if(draw2d.DrawRect){
    state2d.SetForeColor(0);
    draw2d.DrawRect(0,0,snap_modeInfo.XResolution,snap_modeInfo.YResolution);
  }else
    BNK4_clear();

  return 0;
}

int vga_flip(void)
{
  static ibool in_graphics;
  /* Switch between text and graphics mode, obsolete */
  if(in_graphics){
    savestate();
    restoretextmode();
  }else{
    N_int32 dummy = -1;
    if(state2d.WaitTillIdle)
      state2d.WaitTillIdle();
    finit->SetVideoMode(snap_mode | snap_flags,
			&dummy, &dummy, &bytesPerLine, &snap_maxmem, 0, NULL
			);
    restorestate();
  }
  in_graphics = ! in_graphics;
  return 0;
}

int vga_getxdim(void)
{
  if(DC)
    return snap_modeInfo.XResolution;
  else
    return 0;
}

int vga_getydim(void)
{
  if(DC)
    return snap_modeInfo.YResolution;
  else
    return 0;
}

int vga_getcolors(void)
{
  return 1 << snap_modeInfo.BitsPerPixel;
}

int vga_setpalette(int index, int red, int green, int blue)
{
  GA_palette c;

  c.Red = red << 2; c.Blue = blue << 2; c.Green = green << 2; c.Alpha = 0;
  snap_palette[index] = c;
  fdriver->SetPaletteData(&c, 1, index, false);
  return 1;
}

int vga_getpalette(int index, int *red, int *green, int *blue)
{
  GA_palette *c = &snap_palette[index];
  *red = c->Red >> 2;
  *green = c->Green >> 2;
  *blue = c->Blue >> 2;
  return 1;
}

int vga_setpalvec(int start, int num, int *pal)
{
  GA_palette *buf = calloc(sizeof(GA_palette), num);
  int i;

  for(i=0; i<num; i++){
    buf[i].Red = *pal ++ << 2;
    buf[i].Green = *pal ++ << 2;
    buf[i].Blue = *pal ++ << 2;
    buf[i].Alpha = 0;
  }
  fdriver->SetPaletteData(buf, num, start, false);
  free(buf);
  return num;
}

int vga_getpalvec(int start, int num, int *pal)
{
  int i;
  GA_palette *buf = &snap_palette[start];
  for(i=0; i<num; i++){
    *pal ++ = buf[i].Red >> 2;
    *pal ++ = buf[i].Green >> 2;
    *pal ++ = buf[i].Blue >> 2;
  }
  return num;
}

int vga_screenoff(void)
{
  PM_vgaBlankDisplay();
  return 0;
}

int vga_screenon(void)
{
  // Turn on the video output again
  PM_vgaUnblankDisplay();
  return 0;
}

int vga_setcolor(int color)
{
  snap_color = color;
  if(state2d.SetForeColor)
    // if(snap_mode & gaLinearBuffer)
    state2d.SetForeColor(color);
  return 0;
}

static void vga_drawpixel4(int x, int y)
{ // From MGL too
/*   unsigned char	mask,color,shift; */
/*   ptr += x >> 1; */
/*   shift = (x & 0x1) << 2; */
/*   mask = ~(0xF << shift); */
/*   color = ((snap_color & 0xF) << shift) & ~mask; */
/*   *ptr = (*ptr & mask) | color; */
  BNK4_drawPixel(x,y,snap_color);
}

static void vga_drawpixel8(int x, int y)
{
  char *ptr = graph_mem + y*bytesPerLine;
  *(ptr + x) = (char)snap_color;
}

static void vga_drawpixel16(int x, int y)
{
  char *ptr = graph_mem + y*bytesPerLine;
  *(short*)(ptr + 2*x) = (short)snap_color;
}

static void vga_drawpixel24(int x, int y)
{
  char *ptr = graph_mem + y*bytesPerLine;
  *(ptr + 3*x) = snap_color>>16;
  *(short*)(ptr + 3*x + 1) = (short)snap_color;
}

static void vga_drawpixel32(int x, int y)
{
  char *ptr = graph_mem + y*bytesPerLine;
  *(int*)(ptr + 4*x) = (int)snap_color;
}

static int vga_getpixel4(int x, char *ptr)
{ // From MGL ;-)
  ptr +=  x / 2;
  return (*ptr >> (x & 0x1)) & 0x3;
}

static int vga_getpixel8(int x, char *ptr)
{
   return *(ptr + x);
}

static int vga_getpixel16(int x, char *ptr)
{
    return *(short*)(ptr + 2*x);
}

static int vga_getpixel24(int x, char *ptr)
{
    return (*(ptr + 3*x) << 16) | *(short*)(ptr + 3*x + 1);
}

static int vga_getpixel32(int x, char *ptr)
{
    return *(int*)(ptr + 4*x);
}


int vga_drawpixel(int x, int y)
{
/*   if(state2d.WaitTillIdle) */
/*     state2d.WaitTillIdle(); */
/*   else if(state2d.EnableDirectAccess)  */
/*     state2d.EnableDirectAccess(); */
  __vga_drawpixel(x,y);
/*   if(state2d.DisableDirectAccess) */
/*     state2d.DisableDirectAccess(); */
  return 0;
}

#ifndef ABS
#define ABS(a) (((a)<0) ? -(a) : (a))
#endif

int vga_drawline(int x1, int y1, int x2, int y2)
{
  if(draw2d.DrawLineInt)
    draw2d.DrawLineInt(x1,y1,x2,y2, TRUE);
  else{
    /* This algorthm ripped from vgaline.c in svgalib */
    int dx = x2 - x1;
    int dy = y2 - y1;
    int ax = ABS(dx) << 1;
    int ay = ABS(dy) << 1;
    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;

    int x = x1;
    int y = y1;

    if (ax > ay) {
	int d = ay - (ax >> 1);
	while (x != x2) {
	    vga_drawpixel(x, y);
	    if (d > 0 || (d == 0 && sx == 1)) {
		y += sy;
		d -= ax;
	    }
	    x += sx;
	    d += ay;
	}
    } else {
	int d = ax - (ay >> 1);
	while (y != y2) {
	    vga_drawpixel(x, y);
	    if (d > 0 || (d == 0 && sy == 1)) {
		x += sx;
		d -= ay;
	    }
	    y += sy;
	    d += ax;
	}
    }
    vga_drawpixel(x, y);
  }
  return 0;
}

int vga_drawscanline(int line, unsigned char *colors)
{
  if(bytes_per_pixel)
    memcpy(graph_mem+line*bytesPerLine, colors, bytesPerLine);
  else
    vga_drawscansegment(colors, 0, line, snap_modeInfo.XResolution);
  return 0;
}

int vga_drawscansegment(unsigned char *colors, int x, int y, int length)
{
  if(bytes_per_pixel)
    memcpy(graph_mem + y*bytesPerLine + x*bytes_per_pixel, colors, length);
  else{ // 16 color mode
    //    memcpy(graph_mem + y*bytesPerLine + x/2, colors, length);
    int i;
    for(i=0; i<length; i++)
      BNK4_drawPixel(x+i,y,colors[i] & 0xF);
  }
  return 0;
}

int vga_getpixel(int x, int y)
{
  char *ptr = graph_mem + y*bytesPerLine;
  int ret;
  if(state2d.WaitTillIdle)
    state2d.WaitTillIdle();
  else if(state2d.EnableDirectAccess)
    state2d.EnableDirectAccess();
  ret =  __vga_getpixel(x,ptr);
  if(state2d.DisableDirectAccess)
    state2d.DisableDirectAccess();
  return ret;
}

int vga_getscansegment(unsigned char *colors, int x, int y, int length)
{
  memcpy(colors, graph_mem + y*bytesPerLine + x*bytes_per_pixel, length);
  return 0;
}

int vga_getch(void)
{
  return PM_getch();
}

int vga_getkey(void)
{
    struct timeval tv;
    fd_set fds;
    int fd = fileno(stdin);
    char c;

    tv.tv_sec = tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    if (select(fd + 1, &fds, 0, 0, &tv) > 0) {
	if (read(fileno(stdin), &c, 1) != 1) {
	    return 0;
	}
	return c;
    }
    return 0;
}

void vga_runinbackground(int stat, ...)
{
}

int vga_runinbackground_version(void) { return 0; } // Background mode unsupported

extern int __svgalib_kbd_fd, __svgalib_mouse_fd;

int vga_waitevent(int which, fd_set * in, fd_set * out, fd_set * except,
		  struct timeval *timeout)
{
    fd_set infdset;
    int fd, retval;

    if (!in) {
	in = &infdset;
	FD_ZERO(in);
    }
    fd = __svgalib_mouse_fd;	/* __svgalib_mouse_fd might change on
				   vc switch!! */
    if ((which & VGA_MOUSEEVENT) && (fd >= 0))
	FD_SET(fd, in);
    if (which & VGA_KEYEVENT) {
	fd = __svgalib_kbd_fd;
	if (fd >= 0) {		/* we are in raw mode */
	    FD_SET(fd, in);
	} else {
	    FD_SET(__svgalib_tty_fd, in);
	}
    }
    if (select(FD_SETSIZE, in, out, except, timeout) < 0)
	return -1;
    retval = 0;
    fd = __svgalib_mouse_fd;
    if ((which & VGA_MOUSEEVENT) && (fd >= 0)) {
	if (FD_ISSET(fd, in)) {
	    retval |= VGA_MOUSEEVENT;
	    FD_CLR(fd, in);
	    mouse_update();
	}
    }
    if (which & VGA_KEYEVENT) {
	fd = __svgalib_kbd_fd;
	if (fd >= 0) {		/* we are in raw mode */
	    if (FD_ISSET(fd, in)) {
		FD_CLR(fd, in);
		retval |= VGA_KEYEVENT;
		keyboard_update();
	    }
	} else if (FD_ISSET(__svgalib_tty_fd, in)) {
	    FD_CLR(__svgalib_tty_fd, in);
	    retval |= VGA_KEYEVENT;
	}
    }
    return retval;
}

int vga_dumpregs(void)
{
  printf("svgalib mode %d, SNAP Graphics mode 0x%x\n",
	 (int)current_mode, (int) SVGALIB_SNAP[current_mode]
	 );
  return 0;
}

vga_modeinfo *vga_getmodeinfo(int m)
{
  static vga_modeinfo modeinfo;
  GA_modeInfo mode;

  memset(&modeinfo, 0, sizeof(vga_modeinfo));

  if(!DC)
    vga_init();

  if(m == TEXT){ // Return basic information
    modeinfo.flags = HAVE_EXT_SET; // vga_ext_set() is defined
    modeinfo.startaddressrange = (DC->TotalMemory * 1024) -1;
    return &modeinfo;
  }

  if (m < 0 || m > __GLASTMODE || !SVGALIB_SNAP[m])
    return NULL;

  mode.dwSize = sizeof(mode);

  if(!finit)
    vga_init();
  if(finit->GetVideoModeInfo(SVGALIB_SNAP[m], &mode))
    return NULL;

    /* Create the new ModeInfo structure. */
  modeinfo.width = mode.XResolution;
  modeinfo.height = mode.YResolution;
  modeinfo.bytesperpixel = (mode.BitsPerPixel+1) / 8;

#ifdef DEBUG
  printf("Mode %d, bpp: %d, bytespp: %d\n", m, mode.BitsPerPixel, modeinfo.bytesperpixel);
#endif

  modeinfo.linewidth = mode.BytesPerScanLine;
  modeinfo.maxlogicalwidth = mode.MaxBytesPerScanLine;
  modeinfo.colors = 1 << (mode.BitsPerPixel>24 ? 24 : mode.BitsPerPixel);
  modeinfo.maxpixels = (DC->TotalMemory * 1024 * 8) / mode.BitsPerPixel;
  modeinfo.startaddressrange = (DC->TotalMemory * 1024) -1;

  if(mode.BitsPerPixel >= 8)
    modeinfo.haveblit = HAVE_BITBLIT | HAVE_FILLBLIT | HAVE_IMAGEBLIT | HAVE_BLITWAIT | HAVE_HLINELISTBLIT;
  else // No acceleration in 16 color modes
    modeinfo.haveblit = 0;
  modeinfo.chiptype = SNAP;
  modeinfo.memory = DC->TotalMemory;

  modeinfo.flags = HAVE_EXT_SET; // vga_ext_set() is defined
  if(mode.Attributes & gaHaveLinearBuffer)
    modeinfo.flags |= CAPABLE_LINEAR | IS_LINEAR;
  if(mode.Attributes & gaHaveInterlaced)
    modeinfo.flags |= IS_INTERLACED; // Mmh, probably not true

  // Look for MISORDERED attribute according to the pixel format
  // Flag must be set for 0BGR format or RGB
  /*     if( (mode.BitsPerPixel==32 && mode.PixelFormat.RedPosition!=0) || */
  /* 	(mode.BitsPerPixel==24 && mode.PixelFormat.RedPosition!=16) ) // Is it right ?? */
 if((mode.PixelFormat.RedPosition==0 && mode.BitsPerPixel==32) ||
    (mode.PixelFormat.BluePosition==0 && mode.BitsPerPixel==24))
   modeinfo.flags |= RGB_MISORDERED;
 return &modeinfo;
}

int vga_getcurrentmode(void)
{
  return current_mode;
}

int vga_getcurrentchipset(void) { return SNAP; }

int vga_lastmodenumber(void)
{
  return __GLASTMODE;
}

unsigned char *vga_getgraphmem(void)
{
  if(!DC)
    vga_init();
  if(using_linear)
    return DC->LinearMem;
  else // Bank emulation on top of linear framebuffer
    return page_emul_adr;
}

void vga_setpage(int p)
{
  if(snap_modeInfo.Attributes & gaHaveLinearBuffer){
    // Emulate bank switching with mmap() on linear buffer
    if(page_emul_adr)
      munmap(page_emul_adr, 65536);
    page_emul_adr = mmap(page_emul_adr, 65536, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED,
			 __svgalib_mem_fd, DC->LinearBasePtr + (p<<16) + page_offset);
  }else
    fdriver->SetBank(p);
}

void vga_setreadpage(int p) { fprintf(stderr, "Warning: " __FUNCTION__ " is not supported by the SNAP Graphics driver.\n"); }
void vga_setwritepage(int p) { fprintf(stderr, "Warning: " __FUNCTION__ " is not supported by the SNAP Graphics driver.\n"); }

void vga_setlogicalwidth(int w)
{
    N_uint32 dummy = -1;

    // SetVideoMode with new bytesPerLine and DontClear flag ?
    bytesPerLine = w;
    if(state2d.WaitTillIdle)
      state2d.WaitTillIdle();
    finit->SetVideoMode(snap_mode | snap_flags | gaDontClear,
			&dummy, &dummy, &bytesPerLine, &snap_maxmem, 0, NULL
			);
}

void vga_setdisplaystart(int a)
{
  fdriver->SetDisplayStart(current_displaystart = a, TRUE);
}

void vga_waitretrace(void)
{
  fdriver->WaitVSync();
}

extern int vga_claimvideomemory(int n)
{
  /* Set the amount of video memory to be switched when switching to text mode */
  claimed_mem = n;
  return 0;
}

void vga_disabledriverreport(void)
{
  __svgalib_driver_report = 0;
}

int vga_setmodeX(void) {
  fprintf(stderr,"Warning! Program trying to set ModeX layout.\n");
  /* Never succeeds with SNAP Graphics */
  return -1;
}

int vga_init(void)
{
    if(!DC) // TODO: Enumerate devices
      DC = GA_loadDriver(0,false);
    if(!DC){
	fprintf(stderr,"svgalib(SNAP): Unable to initialize the SNAP Graphics driver !\n");
	return -1;
    }

    switch(GA_getDaysLeft(false)){
    case 0:
      fprintf(stderr,"Sorry, your evaluation period for SciTech Display Doctor has expired.\n"
	      "Please contact SciTech Software to purchase a licensed copy.\n\n");
      return -1;
      break;
    case -1:
      if (__svgalib_driver_report)
	fprintf(stderr,"This copy of SciTech Display Doctor for Linux is registered.\n");
      break;
    default:
      if (__svgalib_driver_report)
	fprintf(stderr,"There are %d days left for this evaluation version of SDD/Linux.\n", GA_getDaysLeft(false));
    }

    if ((finit = DC->QueryFunctions(GA_GET_INITFUNCS,0)) == NULL){
	fprintf(stderr,"Unable to query the SNAP Graphics driver initialization!\n");
	return -1;
    }
    if ((fdriver = DC->QueryFunctions(GA_GET_DRIVERFUNCS,0)) == NULL){
	fprintf(stderr,"Unable to query the SNAP Graphics driver functions!\n");
	return -1;
    }

    fill_snap_modes();
    initialize();

    graph_mem = DC->LinearMem;
    if (__svgalib_driver_report)
      printf("Using SciTech SNAP Graphics driver, %liKB.\n",DC->TotalMemory);

    /*  Ensure that the open will get a file descriptor greater
     *  than 2, else problems can occur with stdio functions
     *  under certain strange conditions:  */
    if (fcntl(0,F_GETFD) < 0) open("/dev/null", O_RDONLY);
    if (fcntl(1,F_GETFD) < 0) open("/dev/null", O_WRONLY);
    if (fcntl(2,F_GETFD) < 0) open("/dev/null", O_WRONLY);

    if (__svgalib_mem_fd < 0)
      if ((__svgalib_mem_fd = open("/dev/mem", O_RDWR)) < 0) {
	printf("svgalib: Cannot open /dev/mem.\n");
	return -1;
      }

    if(DC->Attributes & gaHaveLinearBuffer) // Set up banked buffer emulation
       page_emul_adr = mmap(page_emul_adr, 65536, PROT_READ | PROT_WRITE, MAP_SHARED,
			    __svgalib_mem_fd, DC->LinearBasePtr);
/*       page_emul_adr = PM_mapPhysicalAddr(DC->LinearBasePtr, 65535); */
    else
      page_emul_adr = DC->BankedMem;

    __svgalib_readconfigfile();

    snap_oldmode = finit->GetVideoMode();
    fontbuf = malloc(fontbufsize);
    if(ioctl(__svgalib_tty_fd, GIO_FONT, fontbuf)<0){
      perror("ioctl(GIO_FONT)");
    }

    console_state = (void*)malloc(PM_getConsoleStateSize());
    vga_state = (void*)malloc(PM_getConsoleStateSize());
    PM_saveConsoleState(console_state, __svgalib_tty_fd);
    PM_saveVGAState(vga_state);
    return 0;
}

int vga_getmonitortype(void)
{
  /* Obsolete function, could be guessed from the SNAP Graphics monitor config */
  return MON1024_72;
}


void vga_lockvc(void)
{
    lock_count++;
    if (flip)
	__svgalib_waitvtactive();
}

void vga_unlockvc(void)
{
    if (--lock_count <= 0) {
	lock_count = 0;
	if (release_flag) {
	    release_flag = 0;
	    __svgalib_releasevt_signal(SIGUSR1);
	}
    }
}

int vga_oktowrite(void) { return __svgalib_oktowrite; }

// Those functions should never get called since we don't support ModeX layout at all
// But some games like XMame seem to call them anyway :(

void vga_copytoplanar256(unsigned char *virtualp, int pitch,
				int voffset, int vpitch, int w, int h)
{
  if(!voffset)
    draw2d.BitBltSys(virtualp, pitch, 0,0, w,h, 0,0, GA_REPLACE_MIX, false);
  else{
    voffset *= 4;
    draw2d.BitBltSys(virtualp, pitch, 0,0, w,h,
		     voffset % bytesPerLine, voffset / bytesPerLine,
		     GA_REPLACE_MIX, false);
  }
}

void vga_copytoplanar16(unsigned char *virtualp, int pitch,
			       int voffset, int vpitch, int w, int h)
{ fprintf(stderr,__FUNCTION__ ": Unsupported function called\n"); }

void vga_copytoplane(unsigned char *virtualp, int pitch,
			    int voffset, int vpitch, int w, int h, int plane)
{ fprintf(stderr,__FUNCTION__ ": Unsupported function called\n"); }

int vga_setlinearaddressing(void)
{
  if(snap_modeInfo.Attributes & gaHaveLinearBuffer){
    puts("Setting linear framebuffer.");
    using_linear = 1;
    return DC->LinearSize * 1024;
  }else{
    using_linear = 0;
    return -1;
  }
}

static void __svgalib_takevtcontrol(void)
{
    struct sigaction siga;
    struct vt_mode newvtmode;

    ioctl(__svgalib_tty_fd, VT_GETMODE, &__svgalib_oldvtmode);
    newvtmode = __svgalib_oldvtmode;
    newvtmode.mode = VT_PROCESS;	/* handle VT changes */
    newvtmode.relsig = SIGUSR1;	/* I didn't find SIGUSR1/2 anywhere */
    newvtmode.acqsig = SIGUSR2;	/* in the kernel sources, so I guess they are free */
    // TODO: Find other signals for that purpose, since it prevents from doing multithreaded programs
    SETSIG(siga, SIGUSR1, __svgalib_releasevt_signal);
    SETSIG(siga, SIGUSR2, __svgalib_acquirevt_signal);
    ioctl(__svgalib_tty_fd, VT_SETMODE, &newvtmode);
}

static void __svgalib_get_perm(void)
{
    static int done = 0;

    /* Only do this once. */
    if (done)
	return;
    done = 1;

    /* Get I/O permissions for VGA registers. */
    /* If IOPERM is set, assume permissions have already been obtained */
    /* by a calling (exec-ing) process, e.g. ioperm(1). */

    if (getenv("IOPERM") == NULL)
#ifdef __alpha__
	if (ioperm(0x0000, 0x10000, 1)) {
	    printf("svgalib: Cannot get I/O permissions.\n");
	    exit(1);
	}
#else
	if (ioperm(0x3b4, 0x3df - 0x3b4 + 1, 1)) {
	    printf("svgalib: Cannot get I/O permissions.\n");
	    exit(1);
	}
#endif

    __svgalib_open_devconsole();
}

void vga_safety_fork(void (*shutdown_routine) (void))
{
    pid_t childpid;
    int child_status, oldkbmode;

    if (initialized) {
	printf("svgalib: warning: vga_safety_fork() called when already initialized\n");
	goto no_fork;
    }
    initialize();

    /*
     * get current keyboard mode:
     *  If this didn't suffice we claim we are on an old system and just don't
     *  need to restore it.
     */
    ioctl(__svgalib_tty_fd, KDGKBMODE, &oldkbmode);

    childpid = fork();
    if (childpid < 0) {
      no_fork:
	printf("svgalib: warning: can't fork to enhance reliability; proceeding anyway");
	return;
    }
    if (childpid) {
	for (;;) {
	    while (waitpid(childpid, &child_status, WUNTRACED) != childpid);

	    if (shutdown_routine)
		shutdown_routine();

	    vga_setmode(TEXT);	/* resets termios as well */
	    ioctl(__svgalib_tty_fd, KDSKBMODE, oldkbmode);

	    if (WIFEXITED(child_status))
		exit(WEXITSTATUS(child_status));

	    if (WCOREDUMP(child_status))
		puts("svgalib:vga_safety_fork: Core dumped!");

	    if (WIFSIGNALED(child_status)) {
		printf("svgalib:vga_safety_fork: Killed by signal %d, %s.\n",
		       WTERMSIG(child_status),
		       strsignal(WTERMSIG(child_status)));
		exit(1);
	    }
	    if (WIFSTOPPED(child_status)) {
		printf("svgalib:vga_safety_fork: Stopped by signal %d, %s.\n",
		       WSTOPSIG(child_status),
		       strsignal(WSTOPSIG(child_status)));
		puts("\aWARNING! Continue stopped svgalib application at own risk. You are better\n"
		     "off killing it NOW!");
		continue;
	    }
	}
    }
    /* These need to be done again because the child doesn't inherit them.  */
    __svgalib_get_perm();

    /*
     * But alas. That doesn't suffice. We raise the iopl here what merely makes
     * the previous call pointless.
     *
     * If IOPERM is set, assume permissions have already been set by Olaf Titz'
     * ioperm(1).
     */

    if (getenv("IOPERM") == NULL) {
	if (iopl(3) < 0) {
	    printf("svgalib(vga_safety_fork): Cannot get I/O permissions.\n");
	    exit(1);
	}
    }

    /*
     * We might still want to do vc switches.
     */

    __svgalib_takevtcontrol();
}

void vga_setchipset(int c) { fprintf(stderr,"Using generic SNAP Graphics driver.\n"); }
void vga_setchipsetandfeatures(int c, int par1, int par2) {}

void vga_gettextfont(void *font)
{
  if(!DC)
    vga_init();
  memcpy(font,fontbuf, fontbufsize);
}

void vga_puttextfont(void *font)
{
  if(!DC)
    vga_init();
  memcpy(fontbuf, font, fontbufsize);
}

void vga_settextmoderegs(void *regs)
{
  PM_restoreConsoleState(regs, __svgalib_tty_fd);
}

void vga_gettextmoderegs(void *regs)
{
  memcpy(regs, console_state, PM_getConsoleStateSize());
}

int vga_white(void) {
  if(snap_modeInfo.BitsPerPixel > 8)
    return -1;
  else
    return 1;
}

#define packColorFast(pf,R,G,B)	\
 ((ulong)(((uchar)(R) >> (pf).RedAdjust) & (pf).RedMask) << (pf).RedPosition)	\
 | ((ulong)(((uchar)(G) >> (pf).GreenAdjust) & (pf).GreenMask) << (pf).GreenPosition)	\
 | ((ulong)(((uchar)(B) >> (pf).BlueAdjust) & (pf).BlueMask) << (pf).BluePosition)

int vga_setegacolor(int c)
{
  /* EGA colors */
  static const unsigned char egared[16] =  {0,   0,   0,   0, 168, 168, 168, 168, 84,  84,  84,  84, 255, 255, 255, 255};
  static const unsigned char egagreen[16] =  {0,   0, 168, 168,   0,   0,  84, 168, 84,  84, 255, 255,  84,  84, 255, 255};
  static const unsigned char egablue[16] =  {0, 168,   0, 168,   0, 168,   0, 168, 84, 255,  84, 255,  84, 255,  84, 255};

  if(bytes_per_pixel > 1)
    vga_setrgbcolor(egared[c], egagreen[c], egablue[c]);
  else{ /* Palette mode */
    GA_palette pal;
    pal.Red = egared[c];
    pal.Green = egagreen[c];
    pal.Blue = egablue[c];

    snap_color = c;
    fdriver->SetPaletteData(&pal, 1, c, false);
    if(state2d.SetForeColor)
      state2d.SetForeColor(c);
  }
  return 0;
}

int vga_setrgbcolor(int r, int g, int b)
{
  snap_color = packColorFast(snap_modeInfo.PixelFormat,r,g,b);
  if(state2d.SetForeColor)
    state2d.SetForeColor(snap_color);
  return 0;
}

/* Old style accelerator functions */

#define X_FROM_ADDR(a) (((a)%bytesPerLine)/bytes_per_pixel)
#define Y_FROM_ADDR(a) ((a)/bytesPerLine)

void vga_bitblt(int srcaddr, int destaddr, int w, int h, int pitch)
{
  draw2d.BitBlt(X_FROM_ADDR(srcaddr),  Y_FROM_ADDR(srcaddr),
		X_FROM_ADDR(destaddr), Y_FROM_ADDR(destaddr),
		w,h, GA_REPLACE_MIX
		);
}

void vga_imageblt(void *srcaddr, int destaddr, int w, int h, int pitch)
{
  draw2d.BitBltSys(srcaddr, pitch, 0,0, w,h, X_FROM_ADDR(destaddr), Y_FROM_ADDR(destaddr), GA_REPLACE_MIX, false);
}

void vga_fillblt(int destaddr, int w, int h, int pitch, int c)
{
  state2d.SetForeColor(c);
  draw2d.DrawRect(X_FROM_ADDR(destaddr),  Y_FROM_ADDR(destaddr),
		  w,h);
}

void vga_hlinelistblt(int ymin, int n, int *xmin, int *xmax, int pitch, int c)
{
  register int y;
  state2d.SetForeColor(c);
  for(y=0; y<n; y++)
    draw2d.DrawRect(xmin[y], ymin+y, xmax[y]-xmin[y]+1, 1);
}

void vga_blitwait(void)
{
  if(state2d.WaitTillIdle)
    state2d.WaitTillIdle();
}

/* Extended features */

int vga_ext_set(unsigned what,...)
{
  va_list params;
  register int retval = 0;

/*   if(snap_modeInfo.BitsPerPixel < 8) // TODO: Remove this when 4 bpp modes are supported */
/*     return 0; */

  switch(what) {
  case VGA_EXT_AVAILABLE:
    /* Does this use of the arglist corrupt non-AVAIL_ACCEL ext_set? */
    if(!bytes_per_pixel)
      return 0;
    va_start(params, what);
    switch (va_arg(params, int)) {
    case VGA_AVAIL_ACCEL:
      return  ACCELFLAG_SETFGCOLOR | ACCELFLAG_SETBGCOLOR |
	ACCELFLAG_SETTRANSPARENCY | ACCELFLAG_SETRASTEROP |
	ACCELFLAG_FILLBOX | ACCELFLAG_SCREENCOPY |
	ACCELFLAG_DRAWLINE | ACCELFLAG_PUTIMAGE |
	ACCELFLAG_DRAWHLINELIST | ACCELFLAG_PUTBITMAP |
	ACCELFLAG_SCREENCOPYMONO | ACCELFLAG_SYNC | ACCELFLAG_POLYLINE;
	/* ACCELFLAG_SETMODE | */
/* 	ACCELFLAG_POLYLINE | ACCELFLAG_POLYHLINE | */
/* 	ACCELFLAG_POLYFILLMODE ; */
      break;
    case VGA_AVAIL_ROP:
      return ACCELFLAG_FILLBOX | ACCELFLAG_SCREENCOPY | ACCELFLAG_SCREENCOPYMONO | ACCELFLAG_POLYLINE;
      break;
    case VGA_AVAIL_TRANSPARENCY:
      return ACCELFLAG_SCREENCOPY | ACCELFLAG_PUTBITMAP;
      break;
    case VGA_AVAIL_ROPMODES:
      return (1<<ROP_COPY) | (1<<ROP_OR) | (1<<ROP_AND) | (1<<ROP_XOR) | (1<<ROP_INVERT);
      break;
    case VGA_AVAIL_TRANSMODES:
      return (1<<ENABLE_TRANSPARENCY_COLOR) | (1<<ENABLE_BITMAP_TRANSPARENCY);
      break;
    case VGA_AVAIL_SET:
      retval = (1 << VGA_EXT_PAGE_OFFSET) |
	(1 << VGA_EXT_FONT_SIZE);	/* These are handled by us */
      break;
    }
    va_end(params);
    break;
  case VGA_EXT_PAGE_OFFSET:
    va_start(params, what);
    page_offset = va_arg(params, unsigned int);
    return -1;
  case VGA_EXT_FONT_SIZE:
    va_start(params, what);
    what = va_arg(params, unsigned int);
    va_end(params);
    retval = fontbufsize;
    if (what){
      fontbufsize = what;
      fontbuf = realloc(fontbuf, fontbufsize);
    }
    return retval;
  }
  return -1;
}

static void fill_snap_modes(void)
{
  static ibool inited = false;
  int i = 0;
  N_uint16 *mode_list;
  GA_modeInfo modeInfo;

  if(inited) return;

  for (mode_list = DC->AvailableModes; *mode_list != 0xFFFF; mode_list ++) {
    modeInfo.dwSize = sizeof(modeInfo);
    if(finit->GetVideoModeInfo(*mode_list, &modeInfo)) continue;

    switch(modeInfo.BitsPerPixel) {
    case 32:
      switch(modeInfo.YResolution){
      case 200: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x200x16M32]=*mode_list; break;
      case 240: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x240x16M32]=*mode_list; break;
      case 300: if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x300x16M32]=*mode_list; break;
      case 384: if(modeInfo.XResolution==512)
	SVGALIB_SNAP[G512x384x16M32]=*mode_list; break;
      case 400:
	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x400x16M32]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x400x16M32]=*mode_list; break;
      case 480:
	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x480x16M32]=*mode_list;
	else if(modeInfo.XResolution==848)
	  SVGALIB_SNAP[G848x480x16M32]=*mode_list;
	else if(modeInfo.XResolution==512)
	  SVGALIB_SNAP[G512x480x16M32]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x480x16M32]=*mode_list; break;
      case 540: if(modeInfo.XResolution==720)
	SVGALIB_SNAP[G720x540x16M32]=*mode_list; break;
      case 600: if(modeInfo.XResolution==800)
	SVGALIB_SNAP[G800x600x16M32]=*mode_list;
      else if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x600x16M32]=*mode_list;
      else if(modeInfo.XResolution==1072)
	SVGALIB_SNAP[G1072x600x16M32]=*mode_list; break;
      case 720:
	if(modeInfo.XResolution==960)
	  SVGALIB_SNAP[G960x720x16M32]=*mode_list;
	else if(modeInfo.XResolution==1280)
	  SVGALIB_SNAP[G1280x720x16M32]=*mode_list; break;
      case 768: if(modeInfo.XResolution==1024)
	SVGALIB_SNAP[G1024x768x16M32]=*mode_list;
	else if(modeInfo.XResolution==1360)
	  SVGALIB_SNAP[G1360x768x16M32]=*mode_list; break;
      case 864: if(modeInfo.XResolution==1152)
	SVGALIB_SNAP[G1152x864x16M32]=*mode_list; break;
      case 1012: if(modeInfo.XResolution==1800)
	SVGALIB_SNAP[G1800x1012x16M32]=*mode_list; break;
      case 1080: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1080x16M32]=*mode_list; break;
      case 1024: if(modeInfo.XResolution==1280)
	SVGALIB_SNAP[G1280x1024x16M32]=*mode_list; break;
      case 1152: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1152x16M32]=*mode_list; break;
      case 1440: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1440x16M32]=*mode_list; break;
      case 1536: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1536x16M32]=*mode_list; break;
      case 1200: if(modeInfo.XResolution==1600)
	SVGALIB_SNAP[G1600x1200x16M32]=*mode_list; break;
      }
      break;
    case 24:
      switch(modeInfo.YResolution){
      case 200: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x200x16M]=*mode_list; break;
      case 240: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x240x16M]=*mode_list; break;
      case 300: if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x300x16M]=*mode_list; break;
      case 384: if(modeInfo.XResolution==512)
	SVGALIB_SNAP[G512x384x16M]=*mode_list; break;
      case 400:
	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x400x16M]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x400x16M]=*mode_list; break;
      case 480:
	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x480x16M]=*mode_list;
	else if(modeInfo.XResolution==848)
	  SVGALIB_SNAP[G848x480x16M]=*mode_list;
	else if(modeInfo.XResolution==512)
	  SVGALIB_SNAP[G512x480x16M]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x480x16M]=*mode_list; break;
      case 540: if(modeInfo.XResolution==720)
	SVGALIB_SNAP[G720x540x16M]=*mode_list; break;
      case 600: if(modeInfo.XResolution==800)
	SVGALIB_SNAP[G800x600x16M]=*mode_list;
      else if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x600x16M]=*mode_list;
      else if(modeInfo.XResolution==1072)
	SVGALIB_SNAP[G1072x600x16M]=*mode_list; break;
      case 720:
	if(modeInfo.XResolution==960)
	  SVGALIB_SNAP[G960x720x16M]=*mode_list;
	else if(modeInfo.XResolution==1280)
	  SVGALIB_SNAP[G1280x720x16M]=*mode_list; break;
      case 768:
	if(modeInfo.XResolution==1024)
	  SVGALIB_SNAP[G1024x768x16M]=*mode_list;
	else if(modeInfo.XResolution==1360)
	  SVGALIB_SNAP[G1360x768x16M]=*mode_list; break;
      case 864: if(modeInfo.XResolution==1152)
	SVGALIB_SNAP[G1152x864x16M]=*mode_list; break;
      case 1012: if(modeInfo.XResolution==1800)
	SVGALIB_SNAP[G1800x1012x16M]=*mode_list; break;
      case 1080: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1080x16M]=*mode_list; break;
      case 1024: if(modeInfo.XResolution==1280)
	SVGALIB_SNAP[G1280x1024x16M]=*mode_list; break;
      case 1152: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1152x16M]=*mode_list; break;
      case 1440: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1440x16M]=*mode_list; break;
      case 1536: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1536x16M]=*mode_list; break;
      case 1200: if(modeInfo.XResolution==1600)
	SVGALIB_SNAP[G1600x1200x16M]=*mode_list; break;
      }
      break;
    case 15:
      switch(modeInfo.YResolution){
      case 200: if(modeInfo.XResolution==320)
	i=G320x200x32K; break;
      case 240: if(modeInfo.XResolution==320)
	i=G320x240x32K; break;
      case 300: if(modeInfo.XResolution==400)
	i=G400x300x32K; break;
      case 384: if(modeInfo.XResolution==512)
	i=G512x384x32K; break;
      case 400:
	if(modeInfo.XResolution==640)
	  i=G640x400x32K;
	else if(modeInfo.XResolution==320)
	  i=G320x400x32K; break;
      case 480:
	if(modeInfo.XResolution==640)
	  i=G640x480x32K;
	else if(modeInfo.XResolution==848)
	  i=G848x480x32K;
	else if(modeInfo.XResolution==512)
	  i=G512x480x32K;
	else if(modeInfo.XResolution==320)
	  i=G320x480x32K; break;
      case 540: if(modeInfo.XResolution==720)
	i=G720x540x32K; break;
      case 600:
	if(modeInfo.XResolution==800)
	  i=G800x600x32K;
	else if(modeInfo.XResolution==400)
	  i=G400x600x32K;
	else if(modeInfo.XResolution==1072)
	  i=G1072x600x32K; break;
      case 720:
	if(modeInfo.XResolution==960)
	  i=G960x720x32K;
	else if(modeInfo.XResolution==1280)
	  i=G1280x720x32K; break;
      case 768:
	if(modeInfo.XResolution==1024)
	  i=G1024x768x32K;
	else if(modeInfo.XResolution==1360)
	  i=G1360x768x32K; break;
      case 864: if(modeInfo.XResolution==1152)
	i=G1152x864x32K; break;
      case 1012: if(modeInfo.XResolution==1800)
	i=G1800x1012x32K; break;
      case 1080: if(modeInfo.XResolution==1920)
	i=G1920x1080x32K; break;
      case 1024: if(modeInfo.XResolution==1280)
	i=G1280x1024x32K; break;
      case 1152: if(modeInfo.XResolution==2048)
	i=G2048x1152x32K; break;
      case 1440: if(modeInfo.XResolution==1920)
	i=G1920x1440x32K; break;
      case 1536: if(modeInfo.XResolution==2048)
	i=G2048x1536x32K; break;
      case 1200: if(modeInfo.XResolution==1600)
	i=G1600x1200x32K; break;
      }
      SVGALIB_SNAP[i] = *mode_list;
      break;
    case 16:
      switch(modeInfo.YResolution){
      case 200: if(modeInfo.XResolution==320)
	i=G320x200x64K; break;
      case 240: if(modeInfo.XResolution==320)
	i=G320x240x64K; break;
      case 300: if(modeInfo.XResolution==400)
	i=G400x300x64K; break;
      case 384: if(modeInfo.XResolution==512)
	i=G512x384x64K; break;
      case 400:
	if(modeInfo.XResolution==640)
	  i=G640x400x64K;
	else if(modeInfo.XResolution==320)
	  i=G320x400x64K; break;
      case 480:
	if(modeInfo.XResolution==640)
	  i=G640x480x64K;
	else if(modeInfo.XResolution==848)
	  i=G848x480x64K;
	else if(modeInfo.XResolution==512)
	  i=G512x480x64K;
	else if(modeInfo.XResolution==320)
	  i=G320x480x64K;  break;
      case 540: if(modeInfo.XResolution==720)
	i=G720x540x64K; break;
      case 600: if(modeInfo.XResolution==800)
	i=G800x600x64K;
      else if(modeInfo.XResolution==400)
	i=G400x600x64K;
      else if(modeInfo.XResolution==1072)
	i=G1072x600x64K; break;
      case 720:
	if(modeInfo.XResolution==960)
	  i=G960x720x64K;
	else if(modeInfo.XResolution==1280)
	  i=G1280x720x64K; break;
      case 768:
	if(modeInfo.XResolution==1024)
	  i=G1024x768x64K;
	else if(modeInfo.XResolution==1360)
	  i=G1360x768x64K; break;
      case 864: if(modeInfo.XResolution==1152)
	i=G1152x864x64K; break;
      case 1012: if(modeInfo.XResolution==1800)
	i=G1800x1012x64K; break;
      case 1080: if(modeInfo.XResolution==1920)
	i=G1920x1080x64K; break;
      case 1024: if(modeInfo.XResolution==1280)
	i=G1280x1024x64K; break;
      case 1152: if(modeInfo.XResolution==2048)
	i=G2048x1152x64K; break;
      case 1440: if(modeInfo.XResolution==1920)
	i=G1920x1440x64K; break;
      case 1536: if(modeInfo.XResolution==2048)
	i=G2048x1536x64K; break;
      case 1200: if(modeInfo.XResolution==1600)
	i=G1600x1200x64K; break;
      }
      SVGALIB_SNAP[i] = *mode_list;
      break;
    case 8:
      switch(modeInfo.YResolution){
      case 200: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x200x256]=*mode_list; break;
      case 240: if(modeInfo.XResolution==320)
	SVGALIB_SNAP[G320x240x256V]=*mode_list;
	SVGALIB_SNAP[G320x240x256]=*mode_list; break;
      case 300: if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x300x256]=*mode_list; break;
      case 384: if(modeInfo.XResolution==512)
	SVGALIB_SNAP[G512x384x256]=*mode_list; break;
      case 400:
	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x400x256]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x400x256]=*mode_list; break;
      case 480:
 	if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x480x256]=*mode_list;
	else if(modeInfo.XResolution==848)
	  SVGALIB_SNAP[G848x480x256]=*mode_list;
	else if(modeInfo.XResolution==512)
	  SVGALIB_SNAP[G512x480x256]=*mode_list;
	else if(modeInfo.XResolution==360)
	  SVGALIB_SNAP[G360x480x256]=*mode_list;
	else if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x480x256]=*mode_list; break;
      case 540: if(modeInfo.XResolution==720)
	SVGALIB_SNAP[G720x540x256]=*mode_list; break;
      case 600: if(modeInfo.XResolution==800)
	SVGALIB_SNAP[G800x600x256]=*mode_list;
      else if(modeInfo.XResolution==400)
	SVGALIB_SNAP[G400x600x256]=*mode_list;
      else if(modeInfo.XResolution==1072)
	SVGALIB_SNAP[G1072x600x256]=*mode_list; break;
      case 720:
	if(modeInfo.XResolution==960)
	  SVGALIB_SNAP[G960x720x256]=*mode_list;
	else if(modeInfo.XResolution==180)
	  SVGALIB_SNAP[G1280x720x256]=*mode_list; break;
      case 768:
	if(modeInfo.XResolution==1024)
	  SVGALIB_SNAP[G1024x768x256]=*mode_list;
	else if(modeInfo.XResolution==1360)
	  SVGALIB_SNAP[G1360x768x256]=*mode_list; break;
      case 864: if(modeInfo.XResolution==1152)
	SVGALIB_SNAP[G1152x864x256]=*mode_list; break;
      case 1012: if(modeInfo.XResolution==1800)
	SVGALIB_SNAP[G1800x1012x256]=*mode_list; break;
      case 1080: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1080x256]=*mode_list; break;
      case 1024: if(modeInfo.XResolution==1280)
	SVGALIB_SNAP[G1280x1024x256]=*mode_list; break;
      case 1152: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1152x256]=*mode_list; break;
      case 1440: if(modeInfo.XResolution==1920)
	SVGALIB_SNAP[G1920x1440x256]=*mode_list; break;
      case 1536: if(modeInfo.XResolution==2048)
	SVGALIB_SNAP[G2048x1536x256]=*mode_list; break;
      case 1200: if(modeInfo.XResolution==1600)
	SVGALIB_SNAP[G1600x1200x256]=*mode_list; break;
      }
      break;
    case 4:
      switch(modeInfo.YResolution){
      case 200:
	if(modeInfo.XResolution==320)
	  SVGALIB_SNAP[G320x200x16]=*mode_list;
	else if(modeInfo.XResolution==640)
	  SVGALIB_SNAP[G640x200x16]=*mode_list;
	break;
      case 350: if(modeInfo.XResolution==640)
	SVGALIB_SNAP[G640x350x16]=*mode_list; break;
      case 480: if(modeInfo.XResolution==640)
	SVGALIB_SNAP[G640x480x16]=*mode_list; break;
      case 600: if(modeInfo.XResolution==800)
	SVGALIB_SNAP[G800x600x16]=*mode_list; break;
      case 768: if(modeInfo.XResolution==1024)
	SVGALIB_SNAP[G1024x768x16]=*mode_list; break;
      case 864: if(modeInfo.XResolution==1152)
	SVGALIB_SNAP[G1152x864x16]=*mode_list; break;
      case 1024: if(modeInfo.XResolution==1280)
	SVGALIB_SNAP[G1280x1024x16]=*mode_list; break;
      case 1200: if(modeInfo.XResolution==1600)
	SVGALIB_SNAP[G1600x1200x16]=*mode_list; break;
      }

      break;
    }
  }
  SVGALIB_SNAP[TEXT]=3;
  inited = true;
}

/* Conversion table for raster ops */
static GA_mixCodesType nuc_rops[] = {
/*  ROP_COPY */   GA_REPLACE_MIX,
/*  ROP_OR */     GA_OR_MIX,
/*  ROP_AND */    GA_AND_MIX,
/*  ROP_XOR */    GA_XOR_MIX,
/*  ROP_INVERT */ GA_R2_NOT
};

/* Current mix code */
static GA_mixCodesType curmix;


// TODO: Are all those checks really useful ??

static __inline__ void __svgalib_snapaccel_FillBox (int x, int y, int width, int height)
{
  if(draw2d.DrawRect)
    draw2d.DrawRect(x,y,width,height);
}

static void __svgalib_snapaccel_ScreenCopy (int x1, int y1, int x2, int y2, int width, int height)
{
  if(draw2d.BitBlt)
    draw2d.BitBlt(x1, y1, width, height, x2, y2, GA_REPLACE_MIX /* curmix */);
}

static __inline__ void __svgalib_snapaccel_ScreenCopyTrans (int x1, int y1, int x2, int y2, int width, int height)
{
  if(draw2d.SrcTransBlt)
    draw2d.SrcTransBlt(x1, y1, width, height, x2, y2, curmix, snap_transcol);
}

static __inline__ void __svgalib_snapaccel_PutImage (int x, int y, int width, int height, void *image)
{
  if(draw2d.BitBltSys)
    draw2d.BitBltSys(image, width*bytes_per_pixel, 0, 0, width, height, x, y, curmix, false);
}

static __inline__ void __svgalib_snapaccel_DrawLine (int x1, int y1, int x2, int y2)
{
  if(draw2d.DrawLineInt)
    draw2d.DrawLineInt(x1,y1,x2,y2, TRUE);
}

static __inline__ void __svgalib_snapaccel_SetFGColor (int c)
{
  if(state2d.SetForeColor)
    state2d.SetForeColor(c);
}

static __inline__ void __svgalib_snapaccel_SetBGColor (int c)
{
  if(state2d.SetBackColor)
    state2d.SetBackColor(c);
}

static __inline__ void __svgalib_snapaccel_SetRasterOp (int rop)
{
  if(state2d.SetMix)
    state2d.SetMix(curmix = nuc_rops[rop]);
}

static __inline__ void __svgalib_snapaccel_SetTransparency (int mode, int color)
{
    switch(mode){
    case DISABLE_TRANSPARENCY_COLOR:
	snap_transcol = -1;
	ScreenCopy = __svgalib_snapaccel_ScreenCopy;
	break;
    case ENABLE_TRANSPARENCY_COLOR:
	snap_transcol = color;
	ScreenCopy = __svgalib_snapaccel_ScreenCopyTrans;
	break;
    case DISABLE_BITMAP_TRANSPARENCY:
	snap_transparent = 0;
	break;
    case ENABLE_BITMAP_TRANSPARENCY:
	snap_transparent = 1;
	break;
    }
}

static __inline__ void __svgalib_snapaccel_PutBitmap (int x, int y, int w, int h, void *bitmap)
{
    int nbb = (w >> 3) & ~3;
    if(w & 3) nbb += 4; // 32 bits aligned scanlines
    draw2d.PutMonoImageLSBSys(x,y,w,h, nbb, bitmap, snap_transparent);
}

static __inline__ void __svgalib_snapaccel_ScreenCopyBitmap (int x1, int y1, int x2, int y2, int width, int height)
{
    int i, nbb = (width >> 3) & ~3, ofs = y1*bytesPerLine + (x1*bytes_per_pixel);
    if(width & 3) nbb += 4; // 32 bits aligned scanlines
    for(i=0; i<height; i++){ // There is no PutMonoImage function other than the Lin variant :(
	draw2d.PutMonoImageLSBLin(x2,y2+i, width,1, nbb, ofs, // TODO: Highly suspicious code
				  snap_transparent);
	ofs += bytesPerLine;
    }
}

static __inline__ void __svgalib_snapaccel_DrawHLineList (int ymin, int n, int *xmin, int *xmax)
{
    // I don't think that calling DrawScanList would be more efficient since it involves
    // converting the two arrays into one
    register int y;
    for(y=0; y<n; y++)
	draw2d.DrawRect(xmin[y], ymin+y, xmax[y]-xmin[y]+1, 1);
}

static __inline__ void __svgalib_snapaccel_Sync (void)
{
    if(state2d.WaitTillIdle)
	state2d.WaitTillIdle();
}

static __inline__ void  __svgalib_snapaccel_DrawPolyLine (int flag, int n, unsigned short *coords)
{
  static int prevx, prevy;
  int i;
  if(flag & ACCEL_START){
    prevx = *coords ++;
    prevy = *coords ++;
    n --;
  }
  for(i=0; i<n; i++){
    draw2d.DrawLineInt(prevx,prevy,coords[2*i],coords[2*i + 1], TRUE);
    prevx = coords[2*i]; prevy = coords[2*i + 1];
  }
}

/*
 * This calls one of the acceleration interface functions.
 */

int vga_accel(unsigned operation,...)
{
    va_list params;

    va_start(params, operation);

    vga_lockvc();

    /*
     * gcc doesn't produce glorious code here, it's much better with
     * only one va_arg traversal in a function.
     */

    switch (operation) {
    case ACCEL_FILLBOX:
	{
	    int x, y, w, h;
	    x = va_arg(params, int);
	    y = va_arg(params, int);
	    w = va_arg(params, int);
	    h = va_arg(params, int);
	    __svgalib_snapaccel_FillBox (x, y, w, h);
	    return 0;
	}
    case ACCEL_SCREENCOPY:
	{
	    int x1, y1, x2, y2, w, h;
	    x1 = va_arg(params, int);
	    y1 = va_arg(params, int);
	    x2 = va_arg(params, int);
	    y2 = va_arg(params, int);
	    w = va_arg(params, int);
	    h = va_arg(params, int);
	    ScreenCopy (x1, y1, x2, y2, w, h);
	    return 0;
	}
    case ACCEL_PUTIMAGE:
	{
	    int x, y, w, h;
	    void *p;
	    x = va_arg(params, int);
	    y = va_arg(params, int);
	    w = va_arg(params, int);
	    h = va_arg(params, int);
	    p = va_arg(params, void *);
	    __svgalib_snapaccel_PutImage (x, y, w, h, p);
	    return 0;
	}
    case ACCEL_DRAWLINE:
	{
	    int x1, x2, y1, y2;
	    x1 = va_arg(params, int);
	    y1 = va_arg(params, int);
	    x2 = va_arg(params, int);
	    y2 = va_arg(params, int);
	    __svgalib_snapaccel_DrawLine (x1, y1, x2, y2);
	    return 0;
	}
    case ACCEL_SETFGCOLOR:
	{
	    int c;
	    c = va_arg(params, int);
	    __svgalib_snapaccel_SetFGColor (c);
	    return 0;
	}
    case ACCEL_SETBGCOLOR:
	{
	    int c;
	    c = va_arg(params, int);
	    __svgalib_snapaccel_SetBGColor (c);
	    return 0;
	}
    case ACCEL_SETTRANSPARENCY:
	{
	    int m, c;
	    m = va_arg(params, int);
	    c = va_arg(params, int);
	    __svgalib_snapaccel_SetTransparency (m, c);
	    return 0;
	}
    case ACCEL_SETRASTEROP:
	{
	    int r;
	    r = va_arg(params, int);
	    __svgalib_snapaccel_SetRasterOp (r);
	    return 0;
	}
    case ACCEL_PUTBITMAP:
	{
	    int x, y, w, h;
	    void *p;
	    x = va_arg(params, int);
	    y = va_arg(params, int);
	    w = va_arg(params, int);
	    h = va_arg(params, int);
	    p = va_arg(params, void *);
	    __svgalib_snapaccel_PutBitmap (x, y, w, h, p);
	    return 0;
	}
    case ACCEL_SCREENCOPYBITMAP:
	{
	    int x1, y1, x2, y2, w, h;
	    x1 = va_arg(params, int);
	    y1 = va_arg(params, int);
	    x2 = va_arg(params, int);
	    y2 = va_arg(params, int);
	    w = va_arg(params, int);
	    h = va_arg(params, int);
	    __svgalib_snapaccel_ScreenCopyBitmap (x1, y1, x2, y2, w, h);
	    return 0;
	}
    case ACCEL_DRAWHLINELIST:
	{
	    int y, n, *x1, *x2;
	    y = va_arg(params, int);
	    n = va_arg(params, int);
	    x1 = va_arg(params, int *);
	    x2 = va_arg(params, int *);
	    __svgalib_snapaccel_DrawHLineList (y, n, x1, x2);
	    return 0;
	}
    case ACCEL_POLYLINE:
	{
	    int f, n;
	    unsigned short *c;
	    f = va_arg(params, int);
	    n = va_arg(params, int);
	    c = va_arg(params, unsigned short *);
	    __svgalib_snapaccel_DrawPolyLine (f, n, c);
	    return 0;
	}
    case ACCEL_SETMODE:
      // Unsupported
      break;
    case ACCEL_SYNC:
      __svgalib_snapaccel_Sync ();
      return 0;
    }				/* switch */

    va_end(params);

    vga_unlockvc();

    return -1;
}
