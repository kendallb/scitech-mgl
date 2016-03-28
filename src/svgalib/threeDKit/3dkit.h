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

File 3dkit.h

*/

#ifndef THREEDKIT_H
#define THREEDKIT_H 1

#include "triangle.h"

#define TD_DEFAULT_MAXCOLOR 63
#define TD_DEFAULT_COLOR 0
#define TD_DEFAULT_SHADOW 7

#define TD_MESH 1
#define TD_MESH_AND_SOLID 2
#define TD_SOLID 3
#define TD_EDGES_ONLY 4
#define TD_PI 3.14159
#define TDOPTION_INIT_ROTATION_MATRIX 1
#define TDOPTION_ALL_SAME_RENDER 2
#define TDOPTION_SORT_SURFACES 4

/*Two ways to display the object:
1. Angles refer to camera view; camera position is specified 
	in x_cam, y_cam, z_cam.
2. Origin at screen centre; object s_cam away; angles refer to
	rotation of object:*/
#define TDOPTION_ROTATE_OBJECT 8

/* Tells that the surface data are signed 32 bit values.
   otherwise ussumes 16 bit values.
   This can be used to avoid working with cumbersome 32 bits 
   unless the surface arrays have some other use and need the
   accuracy, eg. CAD.
   If set, distance x_cam, y_cam and z_cam are also treated as 32 bit.
   (32/16 bit has nothing to do with the code itself) */
#define TDOPTION_32BIT_SURFACES 16

/* The light source is relative to the angle of the camera: */
#define TDOPTION_LIGHT_SOURCE_CAM 32
/* Otherwise it is fixed relative to the object. */

/*use flat triangle instead of interpolated triangles (slight speed increase)*/
#define TDOPTION_FLAT_TRIANGLE 64

typedef struct {
long x;
long y;
long z;
long dirx;
long diry;
long dirz;
} TD_Point;


typedef struct {
int w;      /*grid width and length*/
int l;
int bitmapwidth;      /*bitmap width and length*/
int bitmaplength;
int maxcolor;  /*There 256 colors divided into n scales.
	     maxcolor must point to the top of the scale you want
		less a few for roundoff*/
int shadow;  /*must point to the bottom of the scale plus a few for roundoff
		so that none of the previous scale is printed. */
int depth_per_color;  /*number of colors in a scale = depth_per_color ^ 2*/
int mesh_color; /*color of mesh if mesh is drawn*/
int render;      /*how it must be rendered*/
int backfacing;    /*enable backfacing*/
unsigned char *bitmap1;    /*1 byte per pixel bitmap data: triangle front side*/
unsigned char *bitmap2;    /*1 byte per pixel bitmap data: triangle back side*/
TD_Point *point;  /*3D data and normals*/
} TD_Surface;


typedef struct {
int num_surfaces;  /*number of surfaces*/

TD_Surface *surf;     /*array of surfaces*/

long a11, a12, a13;  /*rotation matrix*/
long a21, a22, a23;
long a31, a32, a33;

float alpha, beta, gamma;   /*eulerian rotation angles in radians*/

long xlight, ylight, zlight;  /*lighting vector. Magnitude of this
				vector must be less than 255*/

long xscale;      /*determines the size of the object*/
long yscale;

long distance;  /* distance of the camera from origin (always 16 bit)*/

long x_cam; /* position of the camera */
long y_cam;
long z_cam;

long s_cam;

int posx;   /*position of camera optical axis on screen*/
int posy;

long option_flags;

int render;  /*if option ALL_SAME_RENDER is set then all surfaces are rendered
               using this var. Else render is checked on each surface.*/

void (*draw_point) (int, int, int);

void (*draw_wtriangle) (int, int, int, int, int, \
			int, int, int, int, int, \
			int, int, int, int, int, \
		    TD_tridata *);

void (*draw_swtriangle) (int, int, int, int, \
			int, int, int, int, \
			int, int, int, int, int, \
		    TD_tridata *);

void (*draw_striangle) (int, int, int, int, int, int, int, int);
void (*draw_triangle) (int, int, int, int, int, int, int, int, int, int);
void (*draw_line) (int, int, int, int, int );

} TD_Solid;



typedef struct {
long x;
long y;
int color;
} TD_temppoint;


/*used internally*/
typedef struct {
long x, y, c, u, v;
} TD_Short_Point;


/*used internally*/
void TD_translate (TD_Solid * s, TD_Point * p, TD_Short_Point * scr);
long TD_finddistance (TD_Solid * s, TD_Point * p);
long TD_findcolor (TD_Solid * s, TD_Point * p, int which);
void TD_calc_rotation_matrix (TD_Solid * s);
void TD_drawwire (TD_Solid * s, int which);
void TD_drawmesh (TD_Solid * s, int which);
void TD_drawsurface (TD_Solid * s, int which);
void TD_drawwrapsurface (TD_Solid * s, int which);


/*Draws a 3D solid object composed of a number of surfaces, with
hidden surface elimination*/
void TD_draw_solid (TD_Solid * s);

/*initialises the color normal vectors to a surface*/
void TD_initcolor (TD_Surface * surf, int n);


#endif





