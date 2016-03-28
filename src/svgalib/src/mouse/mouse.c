/* hhanemaa@cs.ruu.nl */
/* Mouse library functions */

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vga.h>
#include "vgamouse.h"
#include "../libvga.h"

void (*__svgalib_mouse_eventhandler) (int, int, int, int, int, int, int) = NULL;

static struct MouseCaps mouse_caps = {0, 0, 0, 0, 0, 0};

#include "ms.c"			/* include low-level mouse driver */


static struct sigaction oldsiga;
static void (*currentinthandler) (int);

static void inthandler(int signal)
{
    mouse_close();
    /* restore old interrupt */
    sigaction(SIGINT, &oldsiga, NULL);
    raise(SIGINT);
}

static void default_handler(int, int, int, int, int, int, int);

int mouse_init_return_fd(char *dev, int type, int samplerate)
{
    struct sigaction siga;

    if (strcmp(dev, "") == 0)
	m_dev = "/dev/mouse";
    else
	m_dev = dev;
    m_type = type & MOUSE_TYPE_MASK;
    m_modem_ctl = type & ~MOUSE_TYPE_MASK;

    m_sample = samplerate;

    currentinthandler = NULL;

    /* Initialize mouse device. */
    if (m_type == MOUSE_NONE || m_type < MOUSE_MICROSOFT || m_type > MOUSE_IMPS2)
	return -1;
    if (ms_init())
	return -1;

    /* Install default mouse handler. Old coordinates are preserved. */
    __svgalib_mouse_eventhandler = default_handler;

    /* Install interrupt handler. */
    currentinthandler = inthandler;
    siga.sa_handler = inthandler;
    siga.sa_flags = 0;
    zero_sa_mask(&(siga.sa_mask));
    sigaction(SIGINT, &siga, &oldsiga);

    return __svgalib_mouse_fd;	/* Return mouse fd. */
}

/* Old compatible mouse_init.
   Returns 0 if succesful, -1 on error. */

int mouse_init(char *dev, int type, int samplerate)
{
    if (mouse_init_return_fd(dev, type, samplerate) == -1)
	return -1;
    else
	return 0;
}

/* Reads an event from the mouse buffer.
   Warning: For now, we assume there's no console switching. */

int mouse_update()
{
    int result;
    result = get_ms_event(0);
    return result;
}

void mouse_waitforupdate()
{
#if 0
    fd_set *readfds, writefds, exceptfds;
    struct timeval timeout;
    FD_ZERO(readfds);
    FD_ZERO(writefds);
    FD_ZERO(exceptfds);
    FD_SET(__svgalib_mouse_fd, readfds);
    /* need to setup timeout. */
    select(readfds, writefds, exceptfds, &timeout);
#else
    get_ms_event(1);
#endif
    return;
}

void mouse_seteventhandler(__mouse_handler handler)
{
    __svgalib_mouse_eventhandler = handler;
}

void mouse_close()
{
    ms_close();
    if (currentinthandler != NULL)
	/* Restore old interrupt. */
	sigaction(SIGINT, &oldsiga, NULL);
}


/* Default event handler. */

void mouse_setdefaulteventhandler()
{
    __svgalib_mouse_eventhandler = default_handler;
}

static int mouse_x = 0;
static int mouse_y = 0;
static int mouse_z = 0;
static int mouse_rx = 0;
static int mouse_ry = 0;
static int mouse_rz = 0;
static int mouse_button = 0;
static int scale = 1;
static int minx = 0;
static int maxx = 32767;
static int miny = 0;
static int maxy = 32767;
static int minz = 0;
static int maxz = 32767;
static int minrx = 0;
static int maxrx = 32767;
static int minry = 0;
static int maxry = 32767;
static int minrz = 0;
static int maxrz = 32767;
static int wrap = 0;
static int wantcaps = 0;

/* Sets mouse wrap state*/
void mouse_setwrap(int state)
{
    wrap = state;
}

static void default_handler(int button, int dx, int dy, int dz, int drx,
                            int dry, int drz)
{
    mouse_button = button;
    mouse_x += dx;
    mouse_y += dy;
    mouse_z += dz;
    if (mouse_x / scale > maxx) {
	if (wrap & MOUSE_WRAPX)
	    mouse_x -= (maxx - minx) * scale;
	else
	    mouse_x = maxx * scale;
    }
    /* - 1; ??? */
    if (mouse_x / scale < minx) {
	if (wrap & MOUSE_WRAPX)
	    mouse_x += (maxx - minx) * scale;
	else
	    mouse_x = minx * scale;
    }
    if (mouse_y / scale > maxy) {
	if (wrap & MOUSE_WRAPY)
	    mouse_y -= (maxy - miny) * scale;
	else
	    mouse_y = maxy * scale;
    }
    /*  - 1; ??? */
    if (mouse_y / scale < miny) {
	if (wrap & MOUSE_WRAPY)
	    mouse_y += (maxy - miny) * scale;
	else
	    mouse_y = miny * scale;
    }
    if (mouse_z / scale > maxz) {
	if (wrap & MOUSE_WRAPZ)
	    mouse_z -= (maxz - minz) * scale;
	else
	    mouse_z = maxz * scale;
    }
    /*  - 1; ??? */
    if (mouse_z / scale < minz) {
	if (wrap & MOUSE_WRAPZ)
	    mouse_z += (maxz - minz) * scale;
	else
	    mouse_z = minz * scale;
    }
    switch (wrap & MOUSE_ROT_COORDS) {
      case MOUSE_ROT_INFINITESIMAL:
        mouse_rx = drx;
        mouse_ry = dry;
        mouse_rz = drz;
        break;
      case MOUSE_ROT_RX_RY_RZ:
        mouse_rx += drx;
        mouse_ry += dry;
        mouse_rz += drz;
        break;
    }
    if (mouse_rx / scale > maxrx) {
	if (wrap & MOUSE_WRAPRX)
	    mouse_rx -= (maxrx - minrx) * scale;
	else
	    mouse_rx = maxrx * scale;
    }
    /* - 1; ??? */
    if (mouse_rx / scale < minrx) {
	if (wrap & MOUSE_WRAPRX)
	    mouse_rx += (maxrx - minrx) * scale;
	else
	    mouse_rx = minrx * scale;
    }
    if (mouse_ry / scale > maxry) {
	if (wrap & MOUSE_WRAPRY)
	    mouse_ry -= (maxry - minry) * scale;
	else
	    mouse_ry = maxry * scale;
    }
    /*  - 1; ??? */
    if (mouse_ry / scale < minry) {
	if (wrap & MOUSE_WRAPRY)
	    mouse_ry += (maxry - minry) * scale;
	else
	    mouse_ry = minry * scale;
    }
    if (mouse_rz / scale > maxrz) {
	if (wrap & MOUSE_WRAPRZ)
	    mouse_rz -= (maxrz - minrz) * scale;
	else
	    mouse_rz = maxrz * scale;
    }
    /*  - 1; ??? */
    if (mouse_rz / scale < minrz) {
	if (wrap & MOUSE_WRAPRZ)
	    mouse_rz += (maxrz - minrz) * scale;
	else
	    mouse_rz = minrz * scale;
    }
}

/* Sets the mouse position */
void mouse_setposition(int x, int y)
{
    mouse_x = x * scale;
    mouse_y = y * scale;
}

/* Set the mouse position for up to six dimensions. */
void mouse_setposition_6d(int x, int y, int z, int rx, int ry, int rz,
                          int dim_mask)
{
  if (dim_mask & MOUSE_XDIM)
    mouse_x = x * scale;
  if (dim_mask & MOUSE_YDIM)
    mouse_y = y * scale;
  if (dim_mask & MOUSE_ZDIM)
    mouse_z = z * scale;
  if (dim_mask & MOUSE_RXDIM)
    mouse_rx = rx * scale;
  if (dim_mask & MOUSE_RYDIM)
    mouse_ry = ry * scale;
  if (dim_mask & MOUSE_RZDIM)
    mouse_rz = rz * scale;
  if ((dim_mask & MOUSE_CAPS) && (x == MOUSE_WANTCAPS))
    wantcaps = 1;
}

/* Set the mouse range along the x axis. */
void mouse_setxrange(int x1, int x2)
{
    minx = x1;
    maxx = x2;
}

/* Set the mouse range along the y axis. */
void mouse_setyrange(int y1, int y2)
{
    miny = y1;
    maxy = y2;
}

/* Set the mouse range for up to six dimensions. */
void mouse_setrange_6d(int x1, int x2, int y1, int y2, int z1, int z2,
      int rx1, int rx2, int ry1, int ry2, int rz1, int rz2, int dim_mask)
{

  if (dim_mask & MOUSE_XDIM) {
    minx = x1;
    maxx = x2;
  }
  if (dim_mask & MOUSE_YDIM) {
    miny = y1;
    maxy = y2;
  }
  if (dim_mask & MOUSE_ZDIM) {
    minz = z1;
    maxz = z2;
  }
  if (dim_mask & MOUSE_RXDIM) {
    minrx = rx1;
    maxrx = rx2;
  }
  if (dim_mask & MOUSE_RYDIM) {
    minry = ry1;
    maxry = ry2;
  }
  if (dim_mask & MOUSE_RZDIM) {
    minrz = rz1;
    maxrz = rz2;
  }
}

/* set the sensitivity of the mouse
   This routine sets the scale factor between the motion reported by the
   mouse and the size of one pixel.  The larger scale is, the slower the
   mouse cursor apears to move. 
 Bugs: Scale cannot be set less than one, since it is an integer.  This
   means that there is no war to make the mouse faster than it inherently
   is.*/
void mouse_setscale(int s)
{
    if (scale==0)
      printf("Mouse scale must be non-zero!\n");
    else {
      mouse_x = (mouse_x*s)/scale;
      mouse_y = (mouse_y*s)/scale;
      mouse_z = (mouse_z*s)/scale;
      mouse_rx = (mouse_rx*s)/scale;
      mouse_ry = (mouse_ry*s)/scale;
      mouse_rz = (mouse_rz*s)/scale;
      scale = s;
    }
}


int mouse_getx()
{
    return mouse_x / scale;
}

int mouse_gety()
{
    return mouse_y / scale;
}

void mouse_getposition_6d(int *x, int *y, int *z, int *rx, int *ry, int *rz)
{
    if (wantcaps) {
        /* Return the mouse capabilities */
        *x = mouse_caps.key;
        *y = mouse_caps.buttons;
        *z = mouse_caps.axes;
        *rx = mouse_caps.info;
        *ry = mouse_caps.reserved0;
        *rz = mouse_caps.reserved1;
        wantcaps = 0;
        return;
    }
  if (x)
    *x = mouse_x / scale;
  if (y)
    *y = mouse_y / scale;
  if (z)
    *z = mouse_z / scale;
  if (rx)
    *rx = mouse_rx / scale;
  if (ry)
    *ry = mouse_ry / scale;
  if (rz)
    *rz = mouse_rz / scale;
}

int mouse_getbutton()
{
    return mouse_button;
}
