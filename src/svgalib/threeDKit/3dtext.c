/*
   Copyright (C) 1996, 1997 Paul Sheer

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
 */


/* 3dtext */

#include <config.h>
#include <stdlib.h>
#include <math.h>
#include <my_string.h>
#include <stdio.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include "app_glob.c"
#include "coolwidget.h"
#include "widget3d.h"
#include "quickmath.h"
#include "dialog.h"

/*

 this processes a text file into a 3d world
 the text file contains the following commands seperated
 by zero or more newlines. Charaacters after a # at the
 beginning of a line are ignored.


# x, y, z, a, b, h, w, and c are floats. (x,y,z) is a vector.

scale a
# specifies the absolute value of the maximum extent of the scene in 3D space
# (after offset has been subtracted (see next)). This must come first.

offset x y z
# specifies the a vector that is to be subtracted from the given position of
# forthcoming object. Must also come before any drawing commands.

cylinder x y z a b c r
# draws a cylinder beginning at (a,b,c) ending at (a,b,c)+(x,y,z) of radius r

cappedcylinder x y z a b c r
# draws a cylinder beginning at (a,b,c) ending at (a,b,c)+(x,y,z) of radius r
# with closed ends.

surface a b x y z x y z x y z ... x y z
# draws a surface of grid size a by b there must be a*b (x, y, z) points.

trapezium x y z a b c u v w p q r
# draws a trapezium with one corner at (x,y,z) and the other three at (x,y,z)+(a,b,c) etc.

pipe r a x y z x y z x y z x y z ... x y z
# draw a pipe with corners at (x,y,z) the pipe diameter is r and the corner radii are a
* the first (x,y,z) is the start the last is the finish. Points mus be more than 2a appart

cappedpipe  r a x y z x y z x y z x y z ... x y z
# same with closed ends

rectangle a b c x y z
# rectangle with (height,width,depth) = (x,y,z), corner at (a,b,c)

ellipse a b c x y z
# an ellipse with (height,width,depth) = (x,y,z), centre at (a,b,c)

density a
# will set the density of the grid making up any of the specific surfaces above.
# can be called before each surface command.

*/

/* globals: */

int GridDensity = 6;
double DimensionScale = 1;
Vec DimensionOffset = {0, 0, 0};

static inline void assignTD (TD_Point *p, Vec v)
{
    p->x = (double) (v.x + DimensionOffset.x) * DimensionScale;
    p->y = (double) (v.y + DimensionOffset.y) * DimensionScale;
    p->z = (double) (v.z + DimensionOffset.z) * DimensionScale;
}




static void third_cyl (double t, TD_Point * p, Vec A, Vec X, Vec r1, Vec r2, int g, double f)
{
    int i = 0;
    double h;
    double alpha = t;
    Vec rv;
    while (alpha < (2 * PI / 3 + t + 0.001)) {
	for (h = 0; h <= 1; h += 0.5) {
	    rv = plus (plus (plus (times (r1, cos (alpha) * (1 + h * (f - 1))), times (r2, sin (alpha) * (1 + h * (f - 1)))), A), times (X, h));
	    assignTD(&(p[i]), rv);
	    i++;
	}
	alpha += (2 * PI / 3) / g;
    }
}



void Cdraw3d_cone (const char *ident, double x, double y, double z, double a, double b, double c, double ra, double rb)
{
    int g = 4 * GridDensity / 3;
    TD_Point *p = Cmalloc ((g + 1) * 3 * sizeof (TD_Point));
    Vec r1;
    Vec r2;
    Vec A, X;
    double f = rb / ra;
    A.x = a;
    A.y = b;
    A.z = c;
    X.x = x;
    X.y = y;
    X.z = z;

    orth_vectors (X, &r1, &r2, ra);

    third_cyl (0, p, A, X, r1, r2, g, f);
    Cinit_surf_points (ident, 3, g + 1, p);
    third_cyl (2 * PI / 3, p, A, X, r1, r2, g, f);
    Cinit_surf_points (ident, 3, g + 1, p);
    third_cyl (4 * PI / 3, p, A, X, r1, r2, g, f);
    Cinit_surf_points (ident, 3, g + 1, p);

    free (p);
}




void Cdraw3d_cylinder (const char *ident, double x, double y, double z, double a, double b, double c, double r)
{
    Cdraw3d_cone (ident, x, y, z, a, b, c, r, r);
}



void Cdraw3d_roundplate (const char *ident, double x, double y, double z, double a, double b, double c, double r)
{
    TD_Point *p = Cmalloc ((GridDensity * 4 + 1) * 2 * sizeof (TD_Point));
    double alpha = 0;
    Vec r1;
    Vec r2;
    Vec rv;
    Vec A;
    Vec X;
    int i = 0;
    A.x = a;
    A.y = b;
    A.z = c;
    X.x = x;
    X.y = y;
    X.z = z;

    orth_vectors (X, &r1, &r2, r);

    while (alpha < (2 * PI + 0.001)) {
	rv = plus (plus (times (r1, cos (alpha)), times (r2, sin (alpha))), A);
	assignTD (&p[i], rv);
	i++;
	assignTD (&p[i], A);
	i++;
	alpha += (2 * PI) / (GridDensity * 4);
    }
    Cinit_surf_points (ident, 2, GridDensity * 4 + 1, p);
    free (p);
}


void Cdraw3d_cappedcylinder (const char *ident, double x, double y, double z, double a, double b, double c, double r)
{
    Cdraw3d_cylinder (ident, x, y, z, a, b, c, r);
    Cdraw3d_roundplate (ident, -x, -y, -z, a, b, c, r);
    Cdraw3d_roundplate (ident, x, y, z, x + a, y + b, z + c, r);
}


void textformaterror (int line, const char *ident)
{
    Cerrordialog (CMain, 20, 20, " Compile text to 3D ", " A text format error was encounted at line %d,\nwhile trying to draw 3d item to widget %s.\n ", line, ident);
}


void Cdraw3d_scale (const char *ident, double a)
{
    DimensionScale = 32767 / a;
}

void Cdraw3d_offset (const char *ident, double x, double y, double z)
{
    DimensionOffset.x = x;
    DimensionOffset.y = y;
    DimensionOffset.z = z;
}

void Cdraw3d_density (const char *ident, double a)
{
    GridDensity = a;
}

void draw3d_surface(const char *ident, int w, int h, Vec *v)
{
    int i;
    TD_Point *p = Cmalloc (w * h * sizeof (TD_Point));
    for(i=0;i<w*h;i++)
	assignTD(&p[i], v[i]);
    Cinit_surf_points (ident, w, h, p);
    free (p);
}


void Cdraw3d_surface(const char *ident, int w, int h,...)
{
    va_list pa;
    int i;
    TD_Point *p = Cmalloc (w * h * sizeof (TD_Point));

    va_start(pa, h);
    for(i = 0; i < w * h; i++) {
	p[i].x = va_arg(pa, double);
	p[i].y = va_arg(pa, double);
	p[i].z = va_arg(pa, double);
	p[i].dirx = 0;
	p[i].diry = 0;
	p[i].dirz = 0;
    }
    va_end(pa);
    Cinit_surf_points (ident, w, h, p);
    free (p);
}





static void fxchg (double *a, double *b)
{
    double t = *a;
    *a = *b;
    *b = t;
}


void initellipsoidpart (TD_Point *p, double x, double y, double z,
		      double a, double b, double c, int w, int dir, double f)
{
    int i, j;
    Vec v;
    double r;
    int d = 2 * w + 1;
    Vec X;
    X.x = x;
    X.y = y;
    X.z = z;


    for (i = -w; i <= w; i++)
	for (j = -w; j <= w; j++) {
	    v.x = (double) j / w;
	    v.y = (double) i / w;
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

	    r = norm (v);
	    v.x *= (f + (1 - f) / r) * a;
	    v.y *= (f + (1 - f) / r) * b;
	    v.z *= (f + (1 - f) / r) * c;

	    assignTD(&p[i + w + (j + w) * d], plus(v, X));
	}
}

void Cdraw3d_ellipsoid (const char *ident, double x, double y, double z, double a, double b, double c, double f)
{
    int w = GridDensity / 2;
    int g = 2 * w + 1;
    TD_Point *p = Cmalloc (g * g * sizeof (TD_Point));

    initellipsoidpart (p, x, y, z, a, b, c, w, 0, f);
    Cinit_surf_points (ident, g, g, p);
    initellipsoidpart (p, x, y, z, a, b, c, w, 1, f);
    Cinit_surf_points (ident, g, g, p);
    initellipsoidpart (p, x, y, z, a, b, c, w, 2, f);
    Cinit_surf_points (ident, g, g, p);
    initellipsoidpart (p, x, y, z, a, b, c, w, 3, f);
    Cinit_surf_points (ident, g, g, p);
    initellipsoidpart (p, x, y, z, a, b, c, w, 4, f);
    Cinit_surf_points (ident, g, g, p);
    initellipsoidpart (p, x, y, z, a, b, c, w, 5, f);
    Cinit_surf_points (ident, g, g, p);
    free (p);
}

void Cdraw3d_cappedcone (const char *ident, double x, double y, double z, double a, double b, double c, double ra, double rb)
{
    Cdraw3d_cone (ident, x, y, z, a, b, c, ra, rb);
    Cdraw3d_roundplate (ident, -x, -y, -z, a, b, c, ra);
    Cdraw3d_roundplate (ident, x, y, z, x + a, y + b, z + c, rb);
}


void Cdraw3d_rectangle (const char *ident, double x, double y, double z, double a, double b, double c)
{
    Cdraw3d_ellipsoid (ident, x, y, z, a, b, c, 1);
}


void Cdraw3d_sphere (const char *ident, double x, double y, double z, double r)
{
    Cdraw3d_ellipsoid (ident, x, y, z, r, r, r, 0);
}


/* returns -1 on error, zero on success */
int Cdraw3d_from_text (const char *ident, const char *text)
{
    char *p = (char *) text;
    int line = 1;
    double x, y, z, a, b, c, r, r2;
    Vec *v;
    int w, h, i, k;

    do {
	p += strspn(p, " \t\r");
	if(!*p)
	    break;
	if (*p == '#' || *p == '\n') {
	    /* comment, do nothing */ ;
	} else if (!strncmp (p, "scale ", 6)) {
	    if (sscanf (p, "scale %lf", &x) == 1)
		Cdraw3d_scale (ident, x);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "offset ", 7)) {
	    if (sscanf (p, "offset %lf %lf %lf", &x, &y, &z) == 3)
		Cdraw3d_offset (ident, x, y, z);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "density ", 7)) {
	    if (sscanf (p, "density %lf", &x) == 1)
		Cdraw3d_density (ident, x);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "cylinder ", 8)) {
	    if (sscanf (p, "cylinder %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r) == 7)
		Cdraw3d_cylinder (ident, x, y, z, a, b, c, r);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "roundplate ", 11)) {
	    if (sscanf (p, "roundplate %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r) == 7) {
		Cdraw3d_roundplate (ident, x, y, z, a, b, c, r);
		Cdraw3d_roundplate (ident, -x, -y, -z, a, b, c, r);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "cone ", 5)) {
	    if (sscanf (p, "cone %lf %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r, &r2) == 8)
		Cdraw3d_cone (ident, x, y, z, a, b, c, r, r2);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "cappedcone ", 11)) {
	    if (sscanf (p, "cappedcone %lf %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r, &r2) == 8)
		Cdraw3d_cappedcone (ident, x, y, z, a, b, c, r, r2);
	    else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "cappedcylinder ", 15)) {
	    if (sscanf (p, "cappedcylinder %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r) == 7) {
		Cdraw3d_cappedcylinder (ident, x, y, z, a, b, c, r);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "ellipsoid ", 10)) {
	    if (sscanf (p, "ellipsoid %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c, &r) == 7) {
		Cdraw3d_ellipsoid (ident, x, y, z, a, b, c, r);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "rectangle ", 10)) {
	    if (sscanf (p, "rectangle %lf %lf %lf %lf %lf %lf", &x, &y, &z, &a, &b, &c) == 6) {
		Cdraw3d_rectangle (ident, x, y, z, a, b, c);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "sphere ", 7)) {
	    if (sscanf (p, "sphere %lf %lf %lf %lf ", &x, &y, &z, &r) == 4) {
		Cdraw3d_sphere (ident, x, y, z, r);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else if (!strncmp (p, "surface ", 8)) {
	    if (sscanf (p, "surface %d %d %n", &w, &h, &i) == 2) {
		v = Cmalloc(w * h * sizeof(Vec));
		for(k = 0; k < w * h; k++) {
		    p += i;
		    if(sscanf(p, "%lf %lf %lf %n", &(v[k].x), &(v[k].y), &(v[k].z), &i) != 3) {
			textformaterror (line, ident);
			free(v);
			return -1;
		    }
		}
		draw3d_surface(ident, w, h, v);
		free(v);
	    } else {
		textformaterror (line, ident);
		return -1;
	    }
	} else {
	    textformaterror (line, ident);
	    return -1;
	}

	while (*p != '\n' && *p)
	    p++;
	line++;
    } while (*(p++));

    Credraw3dobject(ident, 1);
    return 0;
}


