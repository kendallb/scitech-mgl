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

File: 3dinit.c

Contains the utility function initcolor for initialising the normal color
vectors of a surface, and the a function to initialise a 3D ellipse.


This file is incomplete and should contain a number of useful
tools to initialise different 3D primitives. 

*/



#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef DO_NOT_USE_VGALIB
#include <vga.h>
#endif

#include <vgagl.h>
#include "./3dkit.h"
#include "./3dinit.h"


double mag (Vec v)
{
    double r;
    if ((r = sqrt (v.x * v.x + v.y * v.y + v.z * v.z)) == 0)
	return 1;
    return r;
}


/*  adds the normal vector to v at point (i,j), calculated from the
    panel d. d is one of the four panels at (i,j). 
 i
-->

0|3  |
-+-  | j
1|2  v

*/


void norm_vec (TD_Surface * surf, int i, int j, Vec * v, int d)
{
    int i1 = 0, i2 = 0, j1 = 0, j2 = 0, w = surf->w;
    double x, y, z, r;
    double x1, y1, z1;
    double x2, y2, z2;
    Vec u;

    switch (d & 3) {
    case 0:
	j1 = -1;
	i2 = -1;
	break;
    case 1:
	i1 = -1;
	j2 = 1;
	break;
    case 2:
	j1 = 1;
	i2 = 1;
	break;
    case 3:
	i1 = 1;
	j2 = -1;
	break;
    }

    x = surf->point[i + j * w].x;
    y = surf->point[i + j * w].y;
    z = surf->point[i + j * w].z;

    x1 = surf->point[i + i1 + (j + j1) * w].x - x;
    y1 = surf->point[i + i1 + (j + j1) * w].y - y;
    z1 = surf->point[i + i1 + (j + j1) * w].z - z;

    x2 = surf->point[i + i2 + (j + j2) * w].x - x;
    y2 = surf->point[i + i2 + (j + j2) * w].y - y;
    z2 = surf->point[i + i2 + (j + j2) * w].z - z;

    u.x = y1 * z2 - z1 * y2;
    u.y = z1 * x2 - x1 * z2;
    u.z = x1 * y2 - y1 * x2;

    r = mag(u);

    v->x += u.x / r;
    v->y += u.y / r;
    v->z += u.z / r;

}

/*Following routine initialise a surface's normal vectors*/
/*(FIXME: this doesn't work 100% at the edges, I think it 
    needs Frenet-Sneret (spelling?) formula) */

/* n gives the brightness of the surface and the direction of the normal.
   normally +256 or -256 (can be less to give a darker surface) */

void TD_initcolor (TD_Surface * surf, int n)
{
    int i, j, k, w = surf->w, l = surf->l, m;

    double r, ru;
    int w0, ww;
    int l0, ll;

    Vec v, u;


    if (w > 2) {
	w0 = 1;
	ww = w - 1;
    } else {
	w0 = 0;
	ww = w;
    }


    if (l > 2) {
	l0 = 1;
	ll = l - 1;
    } else {
	l0 = 0;
	ll = l;
    }


    for (j = 0; j < l; j++)
	for (i = 0; i < w; i++) {

/* normal at a point is the average of the four cross products
   except at the edge points where the gradient of the normal near 
   the edge is considered as well */

	    v.x = v.y = v.z = 0;
	    u.x = u.y = u.z = 0;
	    m = 0;

	    if (i == 0) {
		m = 1;
		if (j != 0) {
		    norm_vec (surf, i, j, &v, 3);
		    norm_vec (surf, w0, j, &u, 3);
		}
		if (j != (l - 1)) {
		    norm_vec (surf, i, j, &v, 2);
		    norm_vec (surf, w0, j, &u, 2);
		}
	    }
	    if (i == (w - 1)) {
		m = 1;
		if (j != 0) {
		    norm_vec (surf, i, j, &v, 0);
		    norm_vec (surf, ww, j, &u, 0);
		}
		if (j != (l - 1)) {
		    norm_vec (surf, i, j, &v, 1);
		    norm_vec (surf, ww, j, &u, 1);
		}
	    }
	    if (j == 0) {
		m = 1;
		if (i != 0) {
		    norm_vec (surf, i, j, &v, 1);
		    norm_vec (surf, i, l0, &u, 1);
		}
		if (i != (w - 1)) {
		    norm_vec (surf, i, j, &v, 2);
		    norm_vec (surf, i, l0, &u, 2);
		}
	    }
	    if (j == (l - 1)) {
		m = 1;
		if (i != 0) {
		    norm_vec (surf, i, j, &v, 0);
		    norm_vec (surf, i, ll, &u, 0);
		}
		if (i != (w - 1)) {
		    norm_vec (surf, i, j, &v, 3);
		    norm_vec (surf, i, ll, &u, 3);
		}
	    }
	    if (m) {

		r = mag (v);
		ru = mag (u);

		v.x = (float) 3 * v.x / (2 * r) - u.x / (2 * ru);
		v.y = (float) 3 * v.y / (2 * r) - u.y / (2 * ru);
		v.z = (float) 3 * v.z / (2 * r) - u.z / (2 * ru);

	    } else {
		for (k = 0; k < 4; k++)
		    norm_vec (surf, i, j, &v, k);

	    }

	    r = mag (v);

	    surf->point[i + j * w].dirx = (double) v.x * n / r;
	    surf->point[i + j * w].diry = (double) v.y * n / r;
	    surf->point[i + j * w].dirz = (double) v.z * n / r;
	}

}



static inline void fxchg (double *a, double *b)
{
    double t = *a;
    *a = *b;
    *b = t;
}


void TD_initellipsoidpart (TD_Surface * surf, long x, long y, long z,
		      long a, long b, long c, int w, int dir, int col)
{
    int i, j;
    Vec v;
    float r;
    surf->w = surf->l = 2 * w + 1;

    for (i = -w; i <= w; i++)
	for (j = -w; j <= w; j++) {
	    v.x = (float) j / w;
	    v.y = (float) i / w;
	    v.z = 1;

	    switch (dir) {
	    case 0:
		v.z = -v.z;
		fxchg (&v.x, &v.y);
		break;
	    case 1:
		v.y = -v.y;
		fxchg (&v.x, &v.z);
		break;
	    case 2:
		v.z = -v.z;
		fxchg (&v.x, &v.z);
		break;
	    case 3:
		v.y = -v.y;
		fxchg (&v.y, &v.z);
		break;
	    case 4:
		v.z = -v.z;
		fxchg (&v.y, &v.z);
		break;
	    }

	    r = mag (v);
	    v.x *= (float) a / r;
	    v.y *= (float) b / r;
	    v.z *= (float) c / r;

	    surf->point[i + w + (j + w) * surf->w].x = v.x + x;
	    surf->point[i + w + (j + w) * surf->w].y = v.y + y;
	    surf->point[i + w + (j + w) * surf->w].z = v.z + z;

	    v.x /= (float) a * a;  /*normal vector*/
	    v.y /= (float) b * b;
	    v.z /= (float) c * c;

	    r = mag (v);

	    surf->point[i + w + (j + w) * surf->w].dirx = (float) col * v.x / r;
	    surf->point[i + w + (j + w) * surf->w].diry = (float) col * v.y / r;
	    surf->point[i + w + (j + w) * surf->w].dirz = (float) col * v.z / r;

	}
}



void TD_initellipsoid (TD_Surface * surf1, TD_Surface * surf2, TD_Surface * surf3,
	TD_Surface * surf4, TD_Surface * surf5, TD_Surface * surf6, long x, 
	long y, long z, long a, long b, long c, int w, int col)
{
    TD_initellipsoidpart (surf1, x, y, z, a, b, c, w, 0, col);
    TD_initellipsoidpart (surf2, x, y, z, a, b, c, w, 1, col);
    TD_initellipsoidpart (surf3, x, y, z, a, b, c, w, 2, col);
    TD_initellipsoidpart (surf4, x, y, z, a, b, c, w, 3, col);
    TD_initellipsoidpart (surf5, x, y, z, a, b, c, w, 4, col);
    TD_initellipsoidpart (surf6, x, y, z, a, b, c, w, 5, col);
}


void TD_initsellipsoid (TD_Solid *s, int n, long x, 
	long y, long z, long a, long b, long c, int w, int col)
{
    TD_initellipsoid(&s->surf[n], &s->surf[n+1], &s->surf[n+2], 
	&s->surf[n+3], &s->surf[n+4], &s->surf[n+5], x, y, z,
	a, b, c, w, col);
}


