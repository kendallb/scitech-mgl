/*

    3DKIT   version   1.3
    High speed 3D graphics and rendering library for Linux.

    1996  Paul Sheer   psheer@icon.co.za

    This file is an example program demonstrating the use of the
    3dkit library. It is not part of the library and is not copyright.

    The author takes no responsibility, for the results
    of compilation, execution or other usage of this program.

*/


/*
File: planukit.c

comments or suggestions welcome, send to:  psheer@icon.co.za

Demo of 3D graphics tool for drawing shaded 3D surfaces with a light source.
This demo sets up the surfaces (in a crude fashion) and the function
drawobject from 3dkit.c draws them at the specified angle of azimuth,
rotation and elevation. The surfaces are sorted from furthest to closest
and drawn from their furthest corner forward toward their second furthest
corner. So any object made up of reasonable surfaces will be drawn solid
with hidden surfaces properly removed. Backfaced triangles are not drawn
to improve speed.

This demo draws a turbo-prop aeroplane (done originally for a 3rd year
aeronautical engineering design project).

see the handle_key function below for what all the keys do.

*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>		/*for stderr */
#include <math.h>
#include <string.h>

#include <vga.h>
#include <vgagl.h>
#include "3dkit.h"
#include "3dinit.h"
#include "plane.h"

#ifdef WORLD_VIEW

#define PL_TDOPTION_ROTATE_OBJECT 0
#define PL_TDOPTION_LIGHT_SOURCE_CAM 0

#else

#define PL_TDOPTION_ROTATE_OBJECT TDOPTION_ROTATE_OBJECT

	/*Lighting vector follows camera: */
#define PL_TDOPTION_LIGHT_SOURCE_CAM TDOPTION_LIGHT_SOURCE_CAM

#endif


/*closer to 1.25 on my screen: */
#define PL_SCREEN_ASPECT 1.333

/*Number of surfaces in ths plane */
#ifdef WORLD_VIEW
#define PL_NUMSURFACES 62
#else
#define PL_NUMSURFACES 52
/*52 */
#endif

/*maximum width or length of a surface (for malloc) */
#define PL_SURF_SIZE 20



/*globals used for initialisation of surfaces */

/*width and breadth of body surfaces (in grid points) */
int DENS = 2;

/*width of wing surfaces (in grid points). */
int DENS2 = 2;

/* length of wing surfaces is inherent in the following 
   made-up aerofoil: */


int gmode;
int PL_screen_width;
int PL_screen_height;


/*A trivial example of how to initialise a surface: */
void initplate (TD_Surface * surf, float xstart, float ystart, float zstart, float x, float y, int w, int l)
{
    int i, k, j;

/*setup width and length */
    surf->w = w + 1;
    surf->l = l + 1;

/*initialise a 6 meter square plate with its centre at the origin */
    for (k = 0; k < w + 1; k++)
	for (i = 0; i < l + 1; i++) {
	    j = l - i;
	    surf->point[i * (w + 1) + k].x = (float) PL_METER *(xstart + (float) x * k / w);
	    surf->point[i * (w + 1) + k].y = (float) PL_METER *(ystart + (float) y * j / l);
	    surf->point[i * (w + 1) + k].z = (float) PL_METER *zstart;
	}
}


/*exchanges the x and y values of a surface, making y negative */
/* This is a patch to get the coords aligned with flight-dynamic's
   axes. */
void xchgxy (TD_Surface * surf)
{
    int j;
    long t;

    for (j = 0; j < surf->l * surf->w; j++) {
	t = surf->point[j].x;
	surf->point[j].x = surf->point[j].y;
	surf->point[j].y = -t;
    }
}

/*
void gl_triangle (int x1, int y1, int z1, int x2, int y2, int z2,
		  int x3, int y3, int z3, int bf);
void gl_striangle (int x1, int y1, int x2, int y2, int x3, int y3,
		   int c, int bf);
*/

/*returns 0 on error */
TD_Solid *PL_init_solid (void)
{
    TD_Solid *plane_demo;
    int i;
    int n = PL_NUMSURFACES;

    if ((plane_demo = malloc (sizeof (TD_Solid))) == NULL)
	return 0;
    memset (plane_demo, 0, sizeof (TD_Solid));

    plane_demo->num_surfaces = n;

    if ((plane_demo->surf = malloc (n * sizeof (TD_Surface))) == NULL)
	return 0;

    for (i = 0; i < n; i++) {
	if ((plane_demo->surf[i].point
	     = malloc (PL_SURF_SIZE * PL_SURF_SIZE * sizeof (TD_Point))) == NULL)
	    return 0;
	/*      plane_demo->surf[i].render = TD_MESH_AND_SOLID; *//*can leave out and set option ALL_SAME_RENDER */
	plane_demo->surf[i].shadow = TD_DEFAULT_COLOR + TD_DEFAULT_SHADOW;
	plane_demo->surf[i].maxcolor = TD_DEFAULT_COLOR + TD_DEFAULT_MAXCOLOR;
	plane_demo->surf[i].mesh_color = 191;	/*navy blue in from the palette set */
	plane_demo->surf[i].backfacing = 1;	/*don't draw any of surface that faces away */
	plane_demo->surf[i].depth_per_color = 6;	/*2^6 = 64 colors in the grey scale */
    }

    plane_demo->alpha = 0;	/* begin all at zero (flight dynamics */
    plane_demo->beta = 0;	/* says plane is level */
    plane_demo->gamma = 0;

    plane_demo->xlight = -147;	/* lighting out of the screen,... */
    plane_demo->ylight = -147;	/* ...to the right,... */
    plane_demo->zlight = 147;	/* ...and from the top. */

    plane_demo->distance = PL_METER * 35;	/* distance of the camera from the */
    /* origin, PL_METER * meters. */

/*if PL_TDOPTION_ROTATE_OBJECT is set to zero then we need to
   define the full camera position instead: */
    plane_demo->x_cam = PL_METER * 35;
    plane_demo->y_cam = PL_METER * 0;
    plane_demo->z_cam = PL_METER * 0;

/* These two are scale factors for the screen: */
/* xscale is now calculated so that the maximum volume (-2^15 to 2^15 or
   -2^31 to 2^31) will just fit inside the screen width at this distance: */
    plane_demo->xscale = (long) plane_demo->distance * PL_screen_width / (32768 * 2);
    plane_demo->yscale = (float) plane_demo->xscale * PL_SCREEN_ASPECT
	* PL_screen_height / PL_screen_width;	/*to get display aspect square */

/*The above gives an average (not to telescopic, and not to wide angle) view */

/*use any triangle or linedrawing routine: */
    plane_demo->draw_triangle = gl_triangle;
    plane_demo->draw_striangle = gl_striangle;
    plane_demo->draw_line = gl_line;

/* very important to set TDOPTION_INIT_ROTATION_MATRIX if you don't
   calculate the rotation matrix yourself. */

    plane_demo->option_flags = TDOPTION_INIT_ROTATION_MATRIX
	| TDOPTION_ALL_SAME_RENDER | TDOPTION_SORT_SURFACES
	| PL_TDOPTION_ROTATE_OBJECT | PL_TDOPTION_LIGHT_SOURCE_CAM;

    plane_demo->render = TD_MESH_AND_SOLID;	/*how we want to render it */

    return plane_demo;
}

void PL_init_surfaces (TD_Solid * plane)
{
    int i;

/* To see what an example of the ellipsoid initialisation: */
/*
   TD_initsellipsoid (plane, 0, 0, 0, 0, 
   PL_METER * 8, PL_METER * 4, PL_METER * 4, 3);
   for(i=0;i<6;i++)
   TD_initcolor (&plane->surf[i], -256); 
   return;
 */

    for (i = 0; i < 4; i++) {
	initfus (&plane->surf[i], i * TD_PI / 2);
    }

    for (i = 0; i < 4; i++) {
	initfus1 (&plane->surf[i + 4], i * TD_PI / 2);
    }

    for (i = 0; i < 4; i++) {
	initfus2 (&plane->surf[i + 8], i * TD_PI / 2);
    }

    initwing (&plane->surf[12], 1, 1, 0);
    initwing (&plane->surf[13], -1, 1, 0);
    initwing (&plane->surf[14], 1, -1, 0);
    initwing (&plane->surf[15], -1, -1, 0);
    initwing (&plane->surf[16], 1, 1, 1);
    initwing (&plane->surf[17], -1, 1, 1);
    initwing (&plane->surf[18], 1, -1, 1);
    initwing (&plane->surf[19], -1, -1, 1);
    initwing (&plane->surf[20], 1, 1, 2);
    initwing (&plane->surf[21], -1, 1, 2);
    initwing (&plane->surf[22], 1, -1, 2);
    initwing (&plane->surf[23], -1, -1, 2);
    initwing (&plane->surf[24], 1, 1, 3);
    initwing (&plane->surf[25], -1, 1, 3);
    initwing (&plane->surf[26], 1, -1, 3);
    initwing (&plane->surf[27], -1, -1, 3);
    initstab (&plane->surf[28], 1, 1);
    initstab (&plane->surf[29], -1, 1);
    initstab (&plane->surf[30], 1, -1);
    initstab (&plane->surf[31], -1, -1);
    initfin (&plane->surf[32], 1);
    initfin (&plane->surf[33], -1);

    for (i = 0; i < 4; i++) {
	initfus3 (&plane->surf[i + 34], i * TD_PI / 2);
    }

    for (i = 0; i < 4; i++) {
	initnacelle (&plane->surf[i + 38], i * TD_PI / 2, -1);
    }

    for (i = 0; i < 4; i++) {
	initnacelle (&plane->surf[i + 42], i * TD_PI / 2, 1);
    }

    for (i = 0; i < 2; i++) {
	initnacelle2 (&plane->surf[i + 46], i * TD_PI / 2, -1);
    }

    for (i = 0; i < 2; i++) {
	initnacelle2 (&plane->surf[i + 48], i * TD_PI / 2, 1);
    }

    inittips (&plane->surf[50], 1);
    inittips (&plane->surf[51], -1);

#ifdef WORLD_VIEW
    for (i = 0; i < 10; i++)
	initplate (&plane->surf[i + 52], -20 + (float) i * 4.44, -20, -3.5, 0.4, 40, 1, 10);
#endif

    for (i = 0; i < PL_NUMSURFACES; i++) {
	xchgxy (&plane->surf[i]);

	TD_initcolor (&plane->surf[i], -256);
	/*initialises the color vector (vector normal to each point) */
    }
}


/*returns 1 on error */

int PL_init_plane (TD_Solid ** plane)
{
    if (!(*plane = PL_init_solid ()))
	return 1;
    PL_init_surfaces (*plane);
    return 0;
}


void PL_init_palette (void)
{
/*  Here the depth_per_color is 5 (for 64 colors).
   256 / 64 gives 4 colors so TD_Surface->color 
   can be 0, 64, 128, OR 192 */

    int i;
    unsigned char palette[768];

    for (i = 0; i < 64; i++) {
	palette[i * 3] = i;
	palette[i * 3 + 1] = i;
	palette[i * 3 + 2] = 16 + i / 2;
    }

    for (i = 0; i < 64; i++) {
	palette[(i + 64) * 3 + 0] = i;
	palette[(i + 64) * 3 + 1] = 0;
	palette[(i + 64) * 3 + 2] = 0;
    }

    for (i = 0; i < 64; i++) {
	palette[(i + 128) * 3 + 0] = 0;
	palette[(i + 128) * 3 + 1] = i;
	palette[(i + 128) * 3 + 2] = 0;
    }

    for (i = 0; i < 64; i++) {
	palette[(i + 192) * 3 + 0] = 0;
	palette[(i + 192) * 3 + 1] = 0;
	palette[(i + 192) * 3 + 2] = i;
    }

    gl_setpalette (&palette);
}




/*returns 1 if exit key is pressed */
int PL_handle_key (TD_Solid * plane)
{
    static float incr = 0.1047198;
    int finished = 0;
    int c;

/*plane->gamma += incr;
   plane->beta = -0.5;
   plane->render = TD_SOLID; 
   return 0; *//*---> a screen saver*/


    switch (c = getchar ()) {
    case 'q':
	plane->alpha += incr;
	break;
    case 'a':
	plane->alpha -= incr;
	break;
    case 'o':
	plane->beta += incr;
	break;
    case 'p':
	plane->beta -= incr;
	break;
    case 'z':
	plane->gamma += incr;
	break;
    case 'x':
	plane->gamma -= incr;
	break;
    case 't':
	plane->z_cam += PL_METER;
	break;
    case 'v':
	plane->z_cam -= PL_METER;
	break;
    case 'g':
	plane->x_cam += PL_METER;
	break;
    case 'f':
	plane->x_cam -= PL_METER;
	break;
    case 'w':
	plane->distance += PL_METER;
	plane->y_cam += PL_METER;
	break;
    case 's':
	plane->distance -= PL_METER;
	plane->y_cam -= PL_METER;
	break;
    case 'c':
	finished = 1;
	break;
    case 'i':
	plane->gamma = 0;
	plane->alpha = 0;
	plane->beta = 0;
	break;
    case ' ':
	switch (plane->render) {
	case TD_MESH:
	    plane->render = TD_MESH_AND_SOLID;
	    break;
	case TD_MESH_AND_SOLID:
	    plane->render = TD_SOLID;
	    break;
	case TD_SOLID:
	    plane->render = TD_EDGES_ONLY;
	    break;
	case TD_EDGES_ONLY:
	    plane->render = TD_MESH;
	    break;
	}
	break;
    case 'r':
	if (plane->option_flags & TDOPTION_FLAT_TRIANGLE)
	    plane->option_flags &= 0xFFFFFFFF - TDOPTION_FLAT_TRIANGLE;
	else
	    plane->option_flags |= TDOPTION_FLAT_TRIANGLE;
	break;
    case '1':
	incr += .01047198;
	break;
    case '2':
	incr -= .01047198;
	break;
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
	DENS = c - '2';
	PL_init_surfaces (plane);
    }

    if (incr < 0)
	incr = 0;

    return (finished);
}


/*WRITE-PAGE FLIPPING*/

/*
  The following routines redirect the setpage functions to take advantage
  of the vga memory: writing graphics functions to one half of the memory
  while viewing
  the other. This saves us a copyscreen, while costing the extra
  time it takes to draw to vga memory instead of linear (i.e. the
  virtual screen) memory. vga.c should have the minor modification
  that would allow this to be done more simply. The following was the
  only way that seemed to work without altering vga.c. We will
  call the method "Write-page Flipping", as apposed to "Page Flipping"
  where pages are flipped, but writing is done to a virtual screen
  which is then copied to the vga memory not being viewed. Write-page
  Flipping writes directly to the vga memory not being viewed.

  The method even works on my TVGA8900CL/D in 320x200 (though it's not
  supposed to), and doesn't work in 640x480 (where it is supposed to)
  so I have given both options at startup. Note that Write-page flipping
  can only work on linear or paged modes (320x200, 640x480, 800x600,
  1024x768) since graphics functions to write directly to planar 
  modes are not supported by svgalib.
*/

GraphicsContext physcr, virscr;
int winflipping, vgawindow = 0;
int Startpage[2];
int gmode, chipset;

void PL_redraw (TD_Solid * plane)
{
    gl_clearscreen (64);
    TD_draw_solid (plane);
}


void PL_cleanup (TD_Solid * plane)
{
/*this function should free all allocated memory*/
    return;
}


void winpointto (int win)
{
    if (chipset == TVGA8900 && gmode == G320x200x256) {
	/*trident has 4 bpp in this mode */
	vga_ext_set(VGA_EXT_PAGE_OFFSET, (Startpage[win] * 4) >> 16);
    } else {
	vga_ext_set(VGA_EXT_PAGE_OFFSET, Startpage[win] >> 16);
    }

    vga_setpage (0);
}


void winview (int win)
{
    vga_waitretrace ();
    vga_setdisplaystart (Startpage[win] * win);
}

void winflip (void)
{
    winview (vgawindow);
    vgawindow = 1 - vgawindow;
    winpointto (vgawindow);
}


void PL_animate (TD_Solid * plane, void (*PL_redraw_callback) (TD_Solid *),
	    int (*PL_key_callback) (TD_Solid *))
{
    do {
	PL_redraw_callback (plane);
	if(winflipping) {
	    winflip ();
	} else {
	    gl_setscreenoffset( HEIGHT * WIDTH * currentcontext.flippage );
	    gl_copyscreen (&physcr);
	}
    } while (!(int *) PL_key_callback (plane));
}

int pl_getchar (void)
{
    int c = 0;
    while (c == 0 || c == '\n') {
	c = vga_getkey ();
    }
    if (c >= 'a' && c <= 'z')
	c += 'A' - 'a';
    return c;
}


int main (void)
{
    int mode[7] =
    {5, 6, 7, 8, 10, 11, 12};
    int Winflipping[7] =
    {1, 0, 0, 0, 1, 1, 1};
    int Winflippages[7] =
    {65536, 0, 0, 0, 8 * 65536, 8 * 65536, 16 * 256};
    int c, c2;
    vga_modeinfo *ginfo;
    TD_Solid *plane;

/* Note that in this demo, graphics are written to all modes as
   virtual modes, so that the triangle routine optimisations will
   operate all the time (see triangle.c). */

    vga_init ();
    if (!(vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_SET) & (1 << VGA_EXT_PAGE_OFFSET))) {
	puts("You need at least svgalib 1.2.10 to run this program!\n");
	exit(1);
    }
    do {
	printf ("\n256 color modes:\n\n1: 320x200\n2: 320x240\n3: 320x400\n");
	printf ("4: 360x480\n5: 640x480\n6: 800x600\n7: 1024x768\n");
	printf ("\nWhich? ");
	c = pl_getchar () - '1';
	printf ("\n");
    } while (c < 0 || c > 6);

    printf("Want (W)rite-page flipping, normal (P)age flipping\n");
    printf("using copyscreen, or (N)o page flipping (W/F/N)\n");
    printf("(W is faster but may not work, N will always work\n");
    printf("but sometimes looks tacky) ?\n");

    c2 = pl_getchar();

    printf ("\n");

    gmode = mode[c];
    winflipping = Winflipping[c];

    if (!vga_hasmode (gmode)) {
	fprintf (stderr, "Mode not available.\n");
	exit (-1);
    }

    vga_setmode (gmode);
    gl_setcontextvga (gmode);

    ginfo = vga_getmodeinfo (gmode);

    PL_screen_width = ginfo->width;
    PL_screen_height = ginfo->height;

    if (PL_init_plane (&plane)) {
	fprintf (stderr, "Unable to intialise data structures.\n");
    }

    plane->posx = PL_screen_width / 2;	/*Where origin will be printed */
    plane->posy = PL_screen_height / 2;

    PL_init_palette ();

/* to see what the palette looks like: */
/* for(i=0;i<256;i++) gl_line(0,i,PL_screen_width,i,i);   getchar(); */


    /* Allow write flipping 
	on 320x200 even though ginfo doesn't report more 
	than 64k of memory:*/
    if ((PL_screen_width * PL_screen_height * 2 > ginfo->maxpixels
		    && gmode != G320x200x256) || c2 != 'W')
	winflipping = 0;

    if (winflipping) {
	printf("Using Write-page Flipping.\n");
	Startpage[0] = 0;  /*define pages offsets into memory*/
	Startpage[1] = Winflippages[c];

	winflip ();
    } else {
	gl_getcontext (&physcr);
	gl_setcontextvgavirtual (gmode);
	gl_getcontext (&virscr);
	if(c2 != 'N') {
	    if(gl_enablepageflipping (&physcr))
		printf("Using Page Flipping.\n"); 
	}
    }

    gl_enableclipping ();

    PL_animate (plane, PL_redraw, PL_handle_key);

    PL_cleanup (plane);
    vga_setmode (TEXT);
    return 0;
}
