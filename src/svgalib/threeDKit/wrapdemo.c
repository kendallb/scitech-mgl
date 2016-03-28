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

File: wrapdemo.c

comments or suggestions welcome.

This program wraps a portrait by Rubens of Susanna Lunden (1622 to 1625)
around an ellipsoid. Because the ellipsoid is defined by six surfaces,
each with the same bitmap the pattern is repeated over the ellipse.

*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>		/*for stderr */
#include <math.h>

#include <vga.h>
#include <vgagl.h>
#include "3dkit.h"
#include "3dinit.h"
#include "wrapdemo.h"

#ifdef WORLD_VIEW

#define EL_TDOPTION_ROTATE_OBJECT 0
#define EL_TDOPTION_LIGHT_SOURCE_CAM 0

#else

#define EL_TDOPTION_ROTATE_OBJECT TDOPTION_ROTATE_OBJECT

	/*Lighting vector follows camera: */
#define EL_TDOPTION_LIGHT_SOURCE_CAM TDOPTION_LIGHT_SOURCE_CAM

#endif


/*closer to 1.25 on my screen: */
#define EL_SCREEN_ASPECT 1.333

/*Number of surfaces in ths ellip */
#define EL_NUMSURFACES 6

/*maximum width or length of a surface (for malloc) */
#define EL_SURF_SIZE 20



/*globals used for initialisation of surfaces */
unsigned char *susanna;

/*width and breadth of body surfaces (in grid points) */
int DENS = 2;

/*width of wing surfaces (in grid points). */
int DENS2 = 2;

/* length of wing surfaces is inherent in the following 
   made-up aerofoil: */


int gmode;
int EL_screen_width;
int EL_screen_height;


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
	    surf->point[i * (w + 1) + k].x = (float) EL_METER *(xstart + (float) x * k / w);
	    surf->point[i * (w + 1) + k].y = (float) EL_METER *(ystart + (float) y * j / l);
	    surf->point[i * (w + 1) + k].z = (float) EL_METER *zstart;
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


/*returns 0 on error */
TD_Solid *EL_init_solid (void)
{
    TD_Solid *ellip_demo;
    int i;
    int n = EL_NUMSURFACES;

    if ((ellip_demo = malloc (sizeof (TD_Solid))) == NULL)
	return 0;

    ellip_demo->num_surfaces = n;

    if ((ellip_demo->surf = malloc (n * sizeof (TD_Surface))) == NULL)
	return 0;

    for (i = 0; i < n; i++) {
	if ((ellip_demo->surf[i].point
	     = malloc (EL_SURF_SIZE * EL_SURF_SIZE * sizeof (TD_Point))) == NULL)
	    return 0;
	/*      ellip_demo->surf[i].render = TD_MESH_AND_SOLID; *//*can leave out and set option ALL_SAME_RENDER */
	ellip_demo->surf[i].shadow = 1;
	ellip_demo->surf[i].maxcolor = 15;
	ellip_demo->surf[i].mesh_color = 111;
	ellip_demo->surf[i].backfacing = 1;
	ellip_demo->surf[i].depth_per_color = 4; /*2^4 = 16 colors in
							 the grey scale */
	ellip_demo->surf[i].bitmap1 = susanna + 245 + 256 * 8; /*skip
					     header and an unsightly border*/
	ellip_demo->surf[i].bitmap2 = NULL;   /*no bitmap on reverse
							 side of surface*/
	ellip_demo->surf[i].bitmapwidth = 244;  /*skip border on sides*/
	ellip_demo->surf[i].bitmaplength = 352;  /*and on bottom*/
    }

    ellip_demo->alpha = 0;	/* begin all at zero (flight dynamics */
    ellip_demo->beta = 0;	/* says plane is level */
    ellip_demo->gamma = 0;

    ellip_demo->xlight = -147;	/* lighting out of the screen,... */
    ellip_demo->ylight = -147;	/* ...to the right,... */
    ellip_demo->zlight = 147;	/* ...and from the top. */

    ellip_demo->distance = EL_METER * 35;  /* distance of the camera from the */
    /* origin, EL_METER * meters. */

/*if EL_TDOPTION_ROTATE_OBJECT is set to zero then we need to
   define the full camera position instead: */
    ellip_demo->x_cam = EL_METER * 35;
    ellip_demo->y_cam = EL_METER * 0;
    ellip_demo->z_cam = EL_METER * 0;

/* These two are scale factors for the screen: */
/* xscale is now calculated so that the maximum volume (-2^15 to 2^15 or
   -2^31 to 2^31) will just fit inside the screen width at this distance: */
    ellip_demo->xscale = (long) ellip_demo->distance * EL_screen_width / (32768 * 2);
    ellip_demo->yscale = (float) ellip_demo->xscale * EL_SCREEN_ASPECT
	* EL_screen_height / EL_screen_width;	/*to get display aspect square */

/*The above gives an average (not to telescopic, and not to wide angle) view */

/*use any triangle or linedrawing routine: */
    ellip_demo->draw_triangle = gl_triangle;
    ellip_demo->draw_striangle = gl_striangle;
    ellip_demo->draw_wtriangle = gl_wtriangle;
    ellip_demo->draw_swtriangle = gl_swtriangle;
    ellip_demo->draw_line = gl_line;
    ellip_demo->draw_point = gl_setpixel;

/* very important to set TDOPTION_INIT_ROTATION_MATRIX if you don't
   calculate the rotation matrix yourself. */

    ellip_demo->option_flags = TDOPTION_INIT_ROTATION_MATRIX
	| TDOPTION_ALL_SAME_RENDER | TDOPTION_SORT_SURFACES
	| EL_TDOPTION_ROTATE_OBJECT | EL_TDOPTION_LIGHT_SOURCE_CAM;

    ellip_demo->render = TD_MESH_AND_SOLID;	/*how we want to render it */

    return ellip_demo;
}



void EL_init_surfaces (TD_Solid * ellip)
{
/* To see what an example of the ellipsoid initialisation: */

    TD_initsellipsoid (ellip, 0, 0, 0, 0,
		       EL_METER * 12, EL_METER * 8, EL_METER * 8, 6, -256);

/*    initplate (&ellip->surf[6], -10, -14.3, -10, 20, 28.7, 2, 3);
    TD_initcolor (&ellip->surf[6], -256);*/

    /*initialises the color vector (vector normal to each point) */
}



/*returns 1 on error */

int EL_init_ellip (TD_Solid ** ellip)
{
    if (!(*ellip = EL_init_solid ()))
	return 1;
    EL_init_surfaces (*ellip);
    return 0;
}


void EL_init_palette2 (void)
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
	palette[(i + 64) * 3 + 1] = i;
	palette[(i + 64) * 3 + 2] = 0;
	palette[(i + 64) * 3 + 3] = 0;
    }

    for (i = 0; i < 64; i++) {
	palette[(i + 128) * 3 + 1] = 0;
	palette[(i + 128) * 3 + 2] = i;
	palette[(i + 128) * 3 + 3] = 0;
    }

    for (i = 0; i < 64; i++) {
	palette[(i + 192) * 3 + 1] = 0;
	palette[(i + 192) * 3 + 2] = 0;
	palette[(i + 192) * 3 + 3] = i;
    }

    gl_setpalette (&palette);
}



void EL_init_palette (void)
{

/*  Here the depth_per_color is 4 (for 16 colors).
   256 / 16 gives 16 colors so TD_Surface->color 
   can be 0, 16, 32, 48,... */

    unsigned char pal16susanna[16][3] =
    {
	{40, 47, 47},
	{98, 96, 129},
	{114, 60, 59},
	{138, 143, 159},
	{70, 73, 71},
	{117, 105, 111},
	{142, 160, 195},
	{61, 58, 70},
	{145, 129, 137},
	{62, 54, 53},
	{166, 173, 219},
	{110, 83, 86},
	{160, 152, 161},
	{150, 115, 105},
	{83, 87, 90},
	{120, 124, 140}
    };

    unsigned char palette[768];

    int i, j, k;
    int max = 0;

    for (i = 0; i < 16; i++)
	for (j = 0; j < 3; j++)
	    if (max < pal16susanna[i][j])
		max = pal16susanna[i][j];

    for (i = 0; i < 16; i++)	/*through all 16 susanna */
	for (j = 0; j < 16; j++)	/*through all shades */
	    for (k = 0; k < 3; k++) 	/*through red green and blue */
		palette[((i * 16) + j) * 3 + k] =
		    (int) pal16susanna[i][k] * ( (16 - (15 - j)*12/15 ) * 4 - 1) / max;

    gl_setpalette (&palette);
}




/*returns 1 if exit key is pressed */
int EL_handle_key (TD_Solid * ellip)
{
    static float incr = 0.1047198;
    int finished = 0;
    int c;

/*ellip->gamma += incr;
   ellip->beta = -0.5;
   ellip->render = TD_SOLID; 
   return 0; *//*---> a screen saver*/


    switch (c = getchar ()) {
    case 'q':
	ellip->alpha += incr;
	break;
    case 'a':
	ellip->alpha -= incr;
	break;
    case 'o':
	ellip->beta += incr;
	break;
    case 'p':
	ellip->beta -= incr;
	break;
    case 'z':
	ellip->gamma += incr;
	break;
    case 'x':
	ellip->gamma -= incr;
	break;
    case 't':
	ellip->z_cam += EL_METER;
	break;
    case 'v':
	ellip->z_cam -= EL_METER;
	break;
    case 'g':
	ellip->x_cam += EL_METER;
	break;
    case 'f':
	ellip->x_cam -= EL_METER;
	break;
    case 'w':
	ellip->distance += EL_METER;
	ellip->y_cam += EL_METER;
	break;
    case 's':
	ellip->distance -= EL_METER;
	ellip->y_cam -= EL_METER;
	break;
    case 'c':
	finished = 1;
	break;
    case 'i':
	ellip->gamma = 0;
	ellip->alpha = 0;
	ellip->beta = 0;
	break;
    case ' ':
	switch (ellip->render) {
	case TD_MESH:
	    ellip->render = TD_MESH_AND_SOLID;
	    break;
	case TD_MESH_AND_SOLID:
	    ellip->render = TD_SOLID;
	    break;
	case TD_SOLID:
	    ellip->render = TD_EDGES_ONLY;
	    break;
	case TD_EDGES_ONLY:
	    ellip->render = TD_MESH;
	    break;
	}
	break;
    case 'r':
	if (ellip->option_flags & TDOPTION_FLAT_TRIANGLE)
	    ellip->option_flags &= 0xFFFFFFFF - TDOPTION_FLAT_TRIANGLE;
	else
	    ellip->option_flags |= TDOPTION_FLAT_TRIANGLE;
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
	EL_init_surfaces (ellip);
    }

    if (incr < 0)
	incr = 0;

    return (finished);
}


/*WRITE-PAGE FLIPPING*/

GraphicsContext physcr, virscr;
int winflipping, vgawindow = 0;
int Startpage[2];
int gmode, chipset;

void EL_redraw (TD_Solid * ellip)
{
    gl_clearscreen (0);
    TD_draw_solid (ellip);
}


void EL_cleanup (TD_Solid * ellip)
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
    vga_waitretrace();
    vga_setdisplaystart (Startpage[win] * win);
}

void winflip (void)
{
    winview (vgawindow);
    vgawindow = 1 - vgawindow;
    winpointto (vgawindow);
}


void EL_animate (TD_Solid * ellip, void (*EL_redraw_callback) (TD_Solid *),
	    int (*EL_key_callback) (TD_Solid *))
{
    do {
	EL_redraw_callback (ellip);
	if(winflipping) {
	    winflip ();
	} else {
	    gl_setscreenoffset( HEIGHT * WIDTH * currentcontext.flippage );
	    gl_copyscreen (&physcr);
	}
    } while (!(int *) EL_key_callback (ellip));
}

int el_getchar (void)
{
    int c = 0;
    while (c == 0 || c == '\n') {
	c = vga_getkey ();
    }
    if (c >= 'a' && c <= 'z')
	c += 'A' - 'a';
    return c;
}


void main (void)
{
    int i;
    int mode[7] = {5, 6, 7, 8, 10, 11, 12};
    int Winflipping[7] = {1, 0, 0, 0, 1, 1, 1};
    int Winflippages[7] = {65536, 0, 0, 0, 8 * 65536, 8 * 65536, 16 * 256};
    int c, c2;
    vga_modeinfo *ginfo;
    TD_Solid *ellip;
    FILE *in;

/* Call vga_init as early as possible to get rid of root priv when reading files. */

    vga_init ();
    if (!(vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_SET) & (1 << VGA_EXT_PAGE_OFFSET))) {
	puts("You need at least svgalib 1.2.10 to run this program!\n");
	exit(1);
    }
    if((susanna = malloc(150000))==NULL) {
	fprintf(stderr, "Error returned from malloc.\n");
	exit(1);
    }

/*susanna[109000] = 'A';*/

    if ((in = fopen ("susannaRUBENS.bmp", "rb")) == NULL) {
	fprintf (stderr, "Cannot open input file.");
	exit (1);
    }

    i = 0;

    while ((c = fgetc (in)) != EOF) {

/*to prevent picture aliasing, use random():*/
	if(random() & 1) {
	    susanna[i++] = c & 0xf0;
	    susanna[i++] = (c >> 4) & 0x0f;
	} else {
	    susanna[i++] = (c >> 4) & 0x0f;
	    susanna[i++] = c & 0xf0;
	}
    }

    fclose (in);

/*printf("\n\n%d\n\n\n", susanna[109000]);*/
    

/* Note that in this demo, graphics are written to all modes as
   virtual modes, so that the triangle routine optimisations will
   operate all the time (see triangle.c). */

    do {
	printf ("\n256 color modes:\n\n1: 320x200\n2: 320x240\n3: 320x400\n");
	printf ("4: 360x480\n5: 640x480\n6: 800x600\n7: 1024x768\n");
	printf ("\nWhich? ");
	c = el_getchar () - '1';
	printf ("\n");
    } while (c < 0 || c > 6);

    printf("Want (W)rite-page flipping, normal (P)age flipping\n");
    printf("using copyscreen, or (N)o page flipping (W/F/N)\n");
    printf("(W is faster but may not work, N will always work\n");
    printf("but sometimes looks tacky) ?\n");

    c2 = el_getchar();

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

    EL_screen_width = ginfo->width;
    EL_screen_height = ginfo->height;

    if (EL_init_ellip (&ellip)) {
	fprintf (stderr, "Unable to intialise data structures.\n");
    }

    ellip->posx = EL_screen_width / 2;	/*Where origin will be printed */
    ellip->posy = EL_screen_height / 2;

    EL_init_palette ();

/* to see what the palette looks like: */
/* for(i=0;i<256;i++) gl_line(0,i,EL_screen_width,i,i);   el_getchar(); */

/*to see the bitmap*/
/*
for (j = 0; j < (EL_screen_height < 416 ? EL_screen_height : 416); j++)
    for (i = 0; i < 256; i++)
	gl_setpixel (i, j, 15 + susanna[240 + i + (j << 8)]);
el_getchar ();
*/

    /* My trident 8900CL/D will allow write flipping 
	on 320x200 even though ginfo doesn't report more 
	than 64k of memory:*/
    if ((EL_screen_width * EL_screen_height * 2 > ginfo->maxpixels
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

    EL_animate (ellip, EL_redraw, EL_handle_key);

    EL_cleanup (ellip);
    vga_setmode (TEXT);
    return;
}
