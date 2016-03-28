/*

    3DKIT   version   1.2
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

File: wrapsurf.c

Comments or suggestions welcome.

This function wraps a bitmap over a surface.
See wrapdemo.c for a demonstration of its use.
Use bitmap data of only 256x512 (see wtriangle.c)
although any region within the bitmap can be drawn
to the full extents of the surface, so multiple
small bitmaps can reside in one image and be
used for different surfaces.

*/



#define TD_MULCONSTANT 4096

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef DO_NOT_USE_VGALIB
#include <vga.h>
#endif

#include <vgagl.h>
#include "3dkit.h"


/*global for holding a surface temporarily:*/
extern TD_Short_Point *temp;


static void xchg (int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}


void TD_drawwrapsurface (TD_Solid * s, int which)
{

    TD_Surface *surf = &s->surf[which];
    int w = surf->w;
    int l = surf->l;
    int bitmapwidth = surf->bitmapwidth;
    int bitmaplength = surf->bitmaplength;
    int i = 0, j = 0, k = 0, c = surf->mesh_color;
    void (*dl) (int, int, int, int, int) = s->draw_line;

    void (*dsw) (int, int, int, int,
		 int, int, int, int,
		 int, int, int, int, int,
	TD_tridata *) = s->draw_swtriangle;

    void (*dw) (int, int, int, int, int,
		int, int, int, int, int,
		int, int, int, int, int,
	TD_tridata *) = s->draw_wtriangle;
    int mesh;
    long d1, d2, d3, d4, d;
    long x1, y1, c1;
    long x2, y2, c2;
    long x3, y3, c3;
    long x4, y4, c4;
    long u1, v1;
    long u2, v2;
    long u3, v3;
    long u4, v4;
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
		    temp[k].u = i * bitmapwidth / (w - 1);
		    temp[k].v = j * bitmaplength / (l - 1);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 1;
	    for (j = 0; j < l; j++)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].u = j * bitmapwidth / (l - 1);
		    temp[k].v = i * bitmaplength / (w - 1);
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
		    temp[k].u = i * bitmapwidth / (w - 1);
		    temp[k].v = j * bitmaplength / (l - 1);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 0;
	    for (j = l - 1; j >= 0; j--)
		for (i = 0; i < w; i++) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].u = j * bitmapwidth / (l - 1);
		    temp[k].v = i * bitmaplength / (w - 1);
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
		    temp[k].u = i * bitmapwidth / (w - 1);
		    temp[k].v = j * bitmaplength / (l - 1);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 1;
	    for (j = l - 1; j >= 0; j--)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].u = j * bitmapwidth / (l - 1);
		    temp[k].v = i * bitmaplength / (w - 1);
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
		    temp[k].u = i * bitmapwidth / (w - 1);
		    temp[k].v = j * bitmaplength / (l - 1);
		    temp[k].c = TD_findcolor (s, &surf->point[i + j * w], which);
		    k++;
		}
	} else {
	    xchg (&l, &w);
	    clockwise = 0;
	    for (j = 0; j < l; j++)
		for (i = w - 1; i >= 0; i--) {
		    TD_translate (s, &surf->point[i * l + j], &temp[k]);
		    temp[k].u = j * bitmapwidth / (l - 1);
		    temp[k].v = i * bitmaplength / (w - 1);
		    temp[k].c = TD_findcolor (s, &surf->point[i * l + j], which);
		    k++;
		}
	}
	break;
    }

    if (!surf->backfacing)
	clockwise += 2;   /*Otherwise a different bitmap on either side*/

    tri.bf = clockwise;

    for (k = 0, j = 0; j < l - 1; j++, k++) {
	for (i = 0; i < w - 1; i++, k++) {

/*define the grid square we are currently drawing: */
	    x1 = temp[k].x;
	    y1 = temp[k].y;
	    u1 = temp[k].u;
	    v1 = temp[k].v;
	    c1 = temp[k].c;

	    x2 = temp[k + 1].x;
	    y2 = temp[k + 1].y;
	    u2 = temp[k + 1].u;
	    v2 = temp[k + 1].v;
	    c2 = temp[k + 1].c;

	    x3 = temp[k + w + 1].x;
	    y3 = temp[k + w + 1].y;
	    u3 = temp[k + w + 1].u;
	    v3 = temp[k + w + 1].v;
	    c3 = temp[k + w + 1].c;

	    x4 = temp[k + w].x;
	    y4 = temp[k + w].y;
	    u4 = temp[k + w].u;
	    v4 = temp[k + w].v;
	    c4 = temp[k + w].c;

	/*draw with two triangles */



	    if (furthest & 1) { /*draw with hypotenuse from point 1 to point 3*/
		if (s->option_flags & TDOPTION_FLAT_TRIANGLE) {
		    c1 = (c1 + c2 + c3 + c4) >> 2;
		    (*dsw) (x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, c1, &tri);
		    (*dsw) (x1, y1, u1, v1, x3, y3, u3, v3, x4, y4, u4, v4, c1, &tri);
		} else {
		    (*dw) (x1, y1, u1, v1, c1, x2, y2, u2, v2, c2, x3, y3, u3, v3, c3, &tri);
		    (*dw) (x1, y1, u1, v1, c1, x3, y3, u3, v3, c3, x4, y4, u4, v4, c4, &tri);
		}
	    } else { /*draw with hypotenuse from point 2 to point 4*/
		if (s->option_flags & TDOPTION_FLAT_TRIANGLE) {
		    c1 = (c1 + c2 + c3 + c4) >> 2;
		    (*dsw) (x1, y1, u1, v1, x2, y2, u2, v2, x4, y4, u4, v4, c1, &tri);
		    (*dsw) (x2, y2, u2, v2, x3, y3, u3, v3, x4, y4, u4, v4, c1, &tri);
		} else {
		    (*dw) (x1, y1, u1, v1, c1, x2, y2, u2, v2, c2, x4, y4, u4, v4, c4, &tri);
		    (*dw) (x2, y2, u2, v2, c2, x3, y3, u3, v3, c3, x4, y4, u4, v4, c4, &tri);
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


