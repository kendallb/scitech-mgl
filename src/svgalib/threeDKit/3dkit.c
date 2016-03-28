/*

    3DKIT   version   1.3
    High speed 3D graphics and rendering library for Linux.

    Copyright (C) 1996, 1997  Paul Sheer   psheer@icon.co.za

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA

*/


/*

File: 3dkit.c

Comments or suggestions welcome.

This 3D graphics tool prints an object in three dimensions on the screen.
The object must be made up of one or more surfaces passed in a structure.
The algorithm calculates the light intensity at each point and does a color
interpolation so that surfaces appear uniform with smooth colour
graduations.

The TD_Object structure contains an array of surfaces comprising the object.
When printing, the surfaces are sorted from furthest to closest by
determining the distance from the eye point of their respective centres.
This removes hidden features.

The points of a surface are assumed to form a contorted rectangular mesh
having a length and a width - the number of points along the longitudinal
edges and lateral edges respectively. Although the surfaces are restricted
to rectangles, they can be infinitely contorted into spheres, triangles
etc., possibly with a whole side compressed into a single point.
It is advisable however to make up complex surfaces out of several less
contorted surfaces so that the sorting routine can place the correct parts 
of the surface in front of one another. A sphere for example can be
defined as eight surfaces, each a triangular octant.

Besides defining each 3D coord point of each surface array, the user must
also define the unit normal at each point. so that shading can be calculated.
The function TD_initcolor may be called to do this for you.

The surfaces are drawn on the screen using one of the following methods.
The integer surf.render determines the method.

0 : Interpolated trangles are drawn with each rectangle outlined.
1 : A wire frame is drawn of the edges of the surface only.
2 : Interpolated triangles only.
3 : Mesh - each rectangle outlined only.

The demo planukit.c demostrates usage in detail.

This code represents a complete re-write of the previous version, which
I wrote when I was first learning C (an excuse). It is far more structured,
efficient and readable. An important additional feature is that the 3D
camera position can now be defined, so that this code can be used as a
VR tool. Hence an object can be displayed as an object at the screen
centre, or as a 3D world. (See plane.h for how to modify the demo).


*/


#define TD_MULCONSTANT 4096

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vgagl.h>
#include "3dkit.h"

#define max(x,y)     (((x) > (y)) ? (x) : (y))
#define min(x,y)     (((x) < (y)) ? (x) : (y))


/*global for holding a surface temporarily:*/
TD_Short_Point *temp;


#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

/* The optimisation comes from svgalib-1.2.9/gl/line.c: */

/* Framebuffer Graphics Libary for Linux, Copyright 1993 Harm Hanemaayer */
/* line.c       Line drawing */

#ifdef __alpha__

static inline int muldiv64 (int m1, int m2, int d)
{
    return (long) m1 *(long) m2 / (long) d;
}

#else

#ifdef __i386__

/* We use the 32-bit to 64-bit multiply and 64-bit to 32-bit divide of the */
/* 386 (which gcc doesn't know well enough) to efficiently perform integer */
/* scaling without having to worry about overflows. */

static inline int muldiv64 (int m1, int m2, int d)
{
/* int32 * int32 -> int64 / int32 -> int32 */
    int result;
    __asm__ (
		"imull %%edx\n\t"
		"idivl %3\n\t"
  :		"=a" (result)	/* out */
  :		"a" (m1), "d" (m2), "g" (d)	/* in */
  :		"ax", "dx"	/* mod */
	);
    return result;
}

#else

static inline int muldiv64(int m1, int m2, int d)
{
    return (double) m1 * (double) m2 / ((double) d);
}

#endif				/* !__i386__ */
#endif				/* !__alpha__ */

#else

#define muldiv64(a,b,c) ((long) ((double) a * (double) b / ((double) c)))

#endif

void TD_translate (TD_Solid * s, TD_Point * p, TD_Short_Point * scr)
{
/* the following rotational transformation avoids floating point
   calculations entirely */

    if (s->option_flags & TDOPTION_32BIT_SURFACES) {
/* for super accuracy */
	double x = p->x + s->x_cam;
	double y = p->y + s->y_cam;
	double z = p->z + s->z_cam;
	double yt = x * s->a21 + y * s->a22 + z * s->a23 + s->s_cam;

	if (yt < 1) {
	    scr->x = scr->y = 32767;
	    return;
	} else {
	    double xt = x * s->a11 + y * s->a12 + z * s->a13;
	    double zt = x * s->a31 + y * s->a32 + z * s->a33;
	    scr->x = ((long) ((double) s->posx + xt * s->xscale / yt)) >> 16;
	    scr->y = ((long) ((double) s->posy - zt * s->yscale / yt)) >> 16;
	    return;
	}
    } else {
	long x = p->x + s->x_cam;
	long y = p->y + s->y_cam;
	long z = p->z + s->z_cam;
	long yt = x * s->a21 + y * s->a22 + z * s->a23 + s->s_cam;

/*(FIXME:) There may be problems if yt overflows, this just checks if the point
   is behind the cam: */
	if (yt < 1) {
	    scr->x = scr->y = 32767;	/*line and triangle routines must 
					   reject these values. */
	    return;
	} else {
	    long xt = x * s->a11 + y * s->a12 + z * s->a13;
	    long zt = x * s->a31 + y * s->a32 + z * s->a33;
	    scr->x = s->posx + muldiv64 (xt, s->xscale, yt);
	    scr->y = s->posy - muldiv64 (zt, s->yscale, yt);
	    return;
	}
    }
}



long TD_finddistance (TD_Solid * s, TD_Point * p)
{
/* the following rotational transformation avoids floating point
   calculations entirely */

    if (s->option_flags & TDOPTION_32BIT_SURFACES) {
/* for super accuracy */
	double x = p->x + s->x_cam;
	double y = p->y + s->y_cam;
	double z = p->z + s->z_cam;
	return ((long) ((double) x * s->a21 + y * s->a22 + z * s->a23 + s->s_cam)) >> 16;
    } else {
	long x = p->x + s->x_cam;
	long y = p->y + s->y_cam;
	long z = p->z + s->z_cam;
	return (x * s->a21 + y * s->a22 + z * s->a23 + s->s_cam);
    }
}



long TD_findcolor (TD_Solid * s, TD_Point * p, int which)
{
    long c, shadow = s->surf[which].shadow;

    /*this you can fool around with to get different shadowing effects. */
    /*c starts off as a signed 28 bit integer. Brightest = -2^28, darkest = +2^28 */

    if (s->option_flags & TDOPTION_LIGHT_SOURCE_CAM) {
/* do product of translated normal vector with lighting vector: */
	c = ((p->dirx * s->a11 + p->diry * s->a12 + p->dirz * s->a13) * s->xlight +
	     (p->dirx * s->a21 + p->diry * s->a22 + p->dirz * s->a23) * s->ylight +
	     (p->dirx * s->a31 + p->diry * s->a32 + p->dirz * s->a33) * s->zlight);
	c = (c >> 20) + 256;
    } else {
	c = p->dirx * s->xlight +
	    p->diry * s->ylight +
	    p->dirz * s->zlight;
	c = (c >> 8) + 256;
    }

    /*c now 9 bits */

/*
    c = s->surf[which].maxcolor
	- ((c * c) >> (16 - s->surf[which].depth_per_color));
*/
    /*:responds quadratically to light or.*/

    c = s->surf[which].maxcolor - (c >> (8 - s->surf[which].depth_per_color));

    /*:responds linearly to light.*/

    if (c < shadow)
	return shadow;
    else
	return c;
}


void TD_calc_rotation_matrix (TD_Solid * s)
{
/* This matrix comes from "Dynamics of Atmospheric Flight" by Bernard Etkin,
   John Wiley & Sons, Inc., and is much easier to copy down than to 
   derive yourself. */

    float tsi = s->alpha, theta = s->beta, phi = s->gamma;

    s->a22 = (float) TD_MULCONSTANT * (cos (theta) * cos (tsi));
    s->a21 = (float) TD_MULCONSTANT * (cos (theta) * sin (tsi));
    s->a23 = (float) TD_MULCONSTANT * (-sin (theta));

    s->a12 = (float) TD_MULCONSTANT * (sin (phi) * sin (theta) * cos (tsi) - cos (phi) * sin (tsi));
    s->a11 = (float) TD_MULCONSTANT * (sin (phi) * sin (theta) * sin (tsi) + cos (phi) * cos (tsi));
    s->a13 = (float) TD_MULCONSTANT * (sin (phi) * cos (theta));

    s->a32 = (float) TD_MULCONSTANT * (cos (phi) * sin (theta) * cos (tsi) + sin (phi) * sin (tsi));
    s->a31 = (float) TD_MULCONSTANT * (cos (phi) * sin (theta) * sin (tsi) - sin (phi) * cos (tsi));
    s->a33 = (float) TD_MULCONSTANT * (cos (phi) * cos (theta));

/* this is the classical rotations matrix of aerodynamics */
/*
    s->a11 = (float) TD_MULCONSTANT * (cos (s->alpha) * cos (s->gamma));
    s->a12 = (float) TD_MULCONSTANT * (cos (s->alpha) * sin (s->gamma));
    s->a13 = (float) TD_MULCONSTANT * (-sin (s->alpha));

    s->a21 = (float) TD_MULCONSTANT * (sin (s->beta) * sin (s->alpha) * cos (s->gamma) - cos (s->beta) * sin (s->gamma));
    s->a22 = (float) TD_MULCONSTANT * (sin (s->beta) * sin (s->alpha) * sin (s->gamma) - cos (s->beta) * cos (s->gamma));
    s->a23 = (float) TD_MULCONSTANT * (sin (s->beta) * cos (s->alpha));

    s->a31 = (float) TD_MULCONSTANT * (cos (s->beta) * sin (s->alpha) * cos (s->gamma) + sin (s->beta) * sin (s->gamma));
    s->a32 = (float) TD_MULCONSTANT * (cos (s->beta) * sin (s->alpha) * sin (s->gamma) + sin (s->beta) * cos (s->gamma));
    s->a33 = (float) TD_MULCONSTANT * (cos (s->beta) * cos (s->alpha));
*/

/*results are 14 bit + sign integers*/
}


void TD_drawwire (TD_Solid * s, int which)
{
    TD_Surface *surf = &s->surf[which];
    int w = surf->w;
    int l = surf->l;
    int i = 0, j = 0, c = surf->mesh_color;
    void (*dl) (int, int, int, int, int) = s->draw_line;

    while (j < w - 1)
	TD_translate (s, &surf->point[j++], &temp[i++]);

    while (j < (w * l - 1)) {
	TD_translate (s, &surf->point[j], &temp[i++]);
	j += w;
    }

    while (j > w * (l - 1))
	TD_translate (s, &surf->point[j--], &temp[i++]);

    while (j >= 0) {
	TD_translate (s, &surf->point[j], &temp[i++]);
	j -= w;
    }

    for (j = 0; j < i - 1; j++) {
	(*dl) (temp[j].x, temp[j].y, temp[j + 1].x, temp[j + 1].y, c);

    }
}


void TD_drawmesh (TD_Solid * s, int which)
{
    TD_Surface *surf = &s->surf[which];
    int w = surf->w;
    int l = surf->l;
    int i = 0, j = 0, k = 0, c = surf->mesh_color;
    void (*dl) (int, int, int, int, int) = s->draw_line;

    while (j < l * w) {
	TD_translate (s, &surf->point[j], &temp[j]);
	j++;
    }

    for (j = 0; j < l - 1; j++, k++) {
	for (i = 0; i < w - 1; i++, k++) {
	    (*dl) (temp[k + 1].x, temp[k + 1].y, temp[k].x, temp[k].y, c);
	    (*dl) (temp[k + w].x, temp[k + w].y, temp[k].x, temp[k].y, c);
	}
	(*dl) (temp[k + w].x, temp[k + w].y, temp[k].x, temp[k].y, c);

    }

    for (i = 0; i < w - 1; i++, k++)
	(*dl) (temp[k + 1].x, temp[k + 1].y, temp[k].x, temp[k].y, c);

}


void xchg (int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}


void TD_drawsurface (TD_Solid * s, int which)
{

    TD_Surface *surf = &s->surf[which];
    int w = surf->w;
    int l = surf->l;
    int i = 0, j = 0, k = 0, c = surf->mesh_color;
    void (*dl) (int, int, int, int, int) = s->draw_line;
    void (*dt) (int, int, int, int, int, int, int, int, int, int) = s->draw_triangle;
    void (*ds) (int, int, int, int, int, int, int, int) = s->draw_striangle;
    int mesh;
    long d1, d2, d3, d4, d;
    long x1, y1, c1;
    long x2, y2, c2;
    long x3, y3, c3;
    long x4, y4, c4;
    int furthest, clockwise = 0;
    TD_tridata tri;

    tri.bitmap1 = surf->bitmap1;
    tri.bitmap2 = surf->bitmap2;

    if (s->option_flags & TDOPTION_ALL_SAME_RENDER)
	mesh = (s->render == TD_MESH_AND_SOLID);
    else
	mesh = (surf->render == TD_MESH_AND_SOLID);

/*distance of four corners (numbered clockwise): */
    d1 = TD_finddistance (s, &surf->point[0]);
    d2 = TD_finddistance (s, &surf->point[w - 1]);
    d3 = TD_finddistance (s, &surf->point[w * l - 1]);
    d4 = TD_finddistance (s, &surf->point[w * (l - 1)]);

/*find furthest point */
    furthest = 1;

    d = d1;

    if (d2 > d) {
	furthest = 2;
	d = d2;
    }
    if (d3 > d) {
	furthest = 3;
	d = d3;
    }
    if (d4 > d)
	furthest = 4;


/*draw scanning from the furthest point to the second furthest point */
/*there are eight possibilities: */

    switch (furthest) {
    case 1:
	if (d2 > d4) {
	    clockwise = 0;
	    for (j = 0; j < l; j++)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i + j * w], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 1;
	    for (j = 0; j < l; j++)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i * l + j], which);
		    k++;
		}
	}
	break;

    case 2:
	if (d1 > d3) {
	    clockwise = 1;
	    for (j = 0; j < l; j++)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i + j * w], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 0;
	    for (j = l - 1; j >= 0; j--)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i * l + j], which);
		    k++;
		}
	}
	break;

    case 3:
	if (d4 > d2) {
	    clockwise = 0;
	    for (j = l - 1; j >= 0; j--)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i + j * w], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 1;
	    for (j = l - 1; j >= 0; j--)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i * l + j], which);
		    k++;
		}
	}
	break;

    case 4:
	if (d3 > d1) {
	    clockwise = 1;
	    for (j = l - 1; j >= 0; j--)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i + j * w], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 0;
	    for (j = 0; j < l; j++)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].c = TD_findcolor (s, &surf->point[i * l + j], which);
		    k++;
		}
	}
	break;
    }

    if (!surf->backfacing)
	clockwise = 2;

    for (k = 0, j = 0; j < l - 1; j++, k++) {
	for (i = 0; i < w - 1; i++, k++) {

/*define the grid square we are currently drawing: */
	    x1 = temp[k].x;
	    y1 = temp[k].y;
	    c1 = temp[k].c;

	    x2 = temp[k + 1].x;
	    y2 = temp[k + 1].y;
	    c2 = temp[k + 1].c;

	    x3 = temp[k + w + 1].x;
	    y3 = temp[k + w + 1].y;
	    c3 = temp[k + w + 1].c;

	    x4 = temp[k + w].x;
	    y4 = temp[k + w].y;
	    c4 = temp[k + w].c;

	    /*draw with two triangles */




	    if (furthest & 1) {	/*draw with hypotenuse from point 1 to point 3 */
		if (s->option_flags & TDOPTION_FLAT_TRIANGLE) {
		    c1 = (c1 + c2 + c3 + c4) >> 2;
		    (*ds) (x1, y1, x2, y2, x3, y3, c1, clockwise);
		    (*ds) (x1, y1, x3, y3, x4, y4, c1, clockwise);
		} else {
		    (*dt) (x1, y1, c1, x2, y2, c2, x3, y3, c3, clockwise);
		    (*dt) (x1, y1, c1, x3, y3, c3, x4, y4, c4, clockwise);
		}
	    } else {		/*draw with hypotenuse from point 2 to point 4 */
		if (s->option_flags & TDOPTION_FLAT_TRIANGLE) {
		    c1 = (c1 + c2 + c3 + c4) >> 2;
		    (*ds) (x1, y1, x2, y2, x4, y4, c1, clockwise);
		    (*ds) (x2, y2, x3, y3, x4, y4, c1, clockwise);
		} else {
		    (*dt) (x1, y1, c1, x2, y2, c2, x4, y4, c4, clockwise);
		    (*dt) (x2, y2, c2, x3, y3, c3, x4, y4, c4, clockwise);
		}
	    }

	    if (mesh) {
		(*dl) (x1, y1, x2, y2, c);
		(*dl) (x1, y1, x4, y4, c);
	    }
	}
	if (mesh)
	    (*dl) (temp[k + w].x, temp[k + w].y, temp[k].x, temp[k].y, c);
    }

    if (mesh) {
	for (i = 0; i < w - 1; i++, k++)
	    (*dl) (temp[k + 1].x, temp[k + 1].y, temp[k].x, temp[k].y, c);
    }
}


int compare (const void *vp, const void *vq)
{
    const int *p = vp;
    const int *q = vq;
    int diff = *p - *q;
    return ((diff >= 0) ? ((diff > 0) ? -1 : 0) : +1);
}

struct disttype {
    long distance;
    int number;
};


void TD_draw_solid (TD_Solid * s)
{
    int n = s->num_surfaces, w, l, i, j, render, num_existing_surfaces;
    long max = 0;

    struct disttype *sortarray = NULL;
    temp = NULL;

    gl_trisetdrawpoint(s->draw_point);

    if ((sortarray = malloc (s->num_surfaces * sizeof (struct disttype))) == NULL) {
	fprintf (stderr, "1. Error allocating memory.\n");
	goto fin;
    }
    if (s->option_flags & TDOPTION_INIT_ROTATION_MATRIX)
	TD_calc_rotation_matrix (s);

    for (j = 0, i = 0; i < n; i++) {
	if((s->surf[i].point)) {
	    sortarray[j++].number = i;
	    w = s->surf[i].w;
	    if (max < w)
		max = w;
	    l = s->surf[i].l;	/*find the largest surface */
	    if (max < l)
		max = l;
	}
    }

    num_existing_surfaces = j;
    if(!num_existing_surfaces) goto fin;

    if (s->option_flags & TDOPTION_SORT_SURFACES) {
	for (j = 0, i = 0; i < n; i++) {
	if((s->surf[i].point)) {
	    sortarray[j++].distance =
		TD_finddistance (s, &s->surf[i].point[s->surf[i].w / 2 
				+ s->surf[i].w * (s->surf[i].l / 2)]);
	    /*the distance of the middle point of the surface */
	    }
	}
	qsort (sortarray, num_existing_surfaces, sizeof (struct disttype), compare);
    }

    max++;

    if ((temp = malloc (max * max * sizeof (TD_Short_Point))) == NULL) {
	fprintf (stderr, "2. Error allocating memory.\n");
	goto fin;
    }
    if (s->option_flags & TDOPTION_ROTATE_OBJECT) {
	s->x_cam = 0;
	s->y_cam = 0;
	s->z_cam = 0;
	s->s_cam = s->distance * TD_MULCONSTANT;
    } else {
	s->s_cam = 0;
    }


    for (i = 0; i < num_existing_surfaces; i++) {
	if (s->option_flags & TDOPTION_ALL_SAME_RENDER)
	    render = s->render;
	else
	    render = s->surf[sortarray[i].number].render;

	switch (render) {
	case TD_SOLID:
	case TD_MESH_AND_SOLID:
	    if ((long) s->surf[sortarray[i].number].bitmap1 
		   | (long) s->surf[sortarray[i].number].bitmap2)
		TD_drawwrapsurface (s, sortarray[i].number);
	    else
		TD_drawsurface (s, sortarray[i].number);
	    break;
	case TD_EDGES_ONLY:
	    TD_drawwire (s, sortarray[i].number);
	    break;
	case TD_MESH:
	    TD_drawmesh (s, sortarray[i].number);
	    break;
	default:
	    TD_drawmesh (s, sortarray[i].number);
	}
    }

    fin:

    if(temp)
	free (temp);
    if(sortarray)
	free (sortarray);

}
