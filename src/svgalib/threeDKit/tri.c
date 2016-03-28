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


#ifdef WRAP
#ifdef INTERP
void gl_wtriangle (int x0, int y0, int xd0, int yd0, int z0,
		   int x1, int y1, int xd1, int yd1, int z1,
		   int x2, int y2, int xd2, int yd2, int z2,
		   TD_tridata * tri)
#else
void gl_swtriangle (int x0, int y0, int xd0, int yd0,
		    int x1, int y1, int xd1, int yd1,
		    int x2, int y2, int xd2, int yd2, int z0,
		    TD_tridata * tri)
#endif
#else
#ifdef INTERP
void gl_triangle (int x0, int y0, int z0,
		  int x1, int y1, int z1,
		  int x2, int y2, int z2, int bf)
#else
void gl_striangle (int x0, int y0,
		   int x1, int y1,
		   int x2, int y2, int z0, int bf)
#endif
#endif
{
    void (*colhline_pos) (void);
    void (*colhline_neg) (void);

    int dir;
    long X;

    long nz;
    long g0, g1h = 0, g1l = 0;
#ifdef INTERP
    long c0;
    long c_y;
#endif
#ifdef WRAP
    long X0, Y0;
    int bf = tri->bf;
    long xd_y, yd_y;
#endif

    dir = 1;

/*Max triangle size in the order of (2^31) >> SHLB)^(.5) : */

    if ((nz = (x0 - x1) * (y0 - y2) - (y0 - y1) * (x0 - x2)) == 0)
	return;			/*the points are collinear. */

#ifdef INTERP
    c_x = -(((y0 - y1) * (z0 - z2) - (z0 - z1) * (y0 - y2)) << SHLB) / nz;
    c_y = -(((z0 - z1) * (x0 - x2) - (x0 - x1) * (z0 - z2)) << SHLB) / nz;
#endif

#ifdef WRAP
    xd_x = -(((y0 - y1) * (xd0 - xd2) - (xd0 - xd1) * (y0 - y2)) << SHLB) / nz;
    xd_y = -(((xd0 - xd1) * (x0 - x2) - (x0 - x1) * (xd0 - xd2)) << SHLB) / nz;

    yd_x = -(((y0 - y1) * (yd0 - yd2) - (yd0 - yd1) * (y0 - y2)) << SHLB) / nz;
    yd_y = -(((yd0 - yd1) * (x0 - x2) - (x0 - x1) * (yd0 - yd2)) << SHLB) / nz;
#endif

#ifdef INTERP
    if ((abs (c_x) > (6 << SHLB)) || (abs (c_y) > (6 << SHLB))) {
	int tz0, tz1;

	/*so that high colour gradients don't screw up at the edges. */
	/*4 is the maximum gradient per pixel. */

	c_x >>= 2;
	c_y >>= 2;
	tz0 = ((2 * z0 + z1 + z2) << SHLB) / 4;
	tz1 = ((z0 + 2 * z1 + z2) << SHLB) / 4;
	z2 = ((z0 + z1 + 2 * z2) << SHLB) / 4;
	z0 = tz0;
	z1 = tz1;
    } else {
	z0 <<= SHLB;
	z1 <<= SHLB;
	z2 <<= SHLB;
    }
#endif

/************** BOOLEAN LOGIC HERE ************/
/* The following allows a triangle to have a different picture on either side */
/* To print triangles that don't appear when viewed from behind use bf = 0|1  */
/* To print triangles that appear with a different picture when viewed from   */
/*   behind use bf = 2|3                                                      */

#ifdef WRAP
    dat = tri->bitmap1;
    if (nz > 0) {		/* nz is the cross product of the vectors of the two sides
				   it indicates whether the points were ordered clockwise
				   or anti-clockwise (you can find out which way by testing) */
	if (bf == 1)
	    return;
	if (bf == 3)
	    dat = tri->bitmap1;
	dir++;
    } else {
	if (!bf)
	    return;
	if (bf == 2)
	    dat = tri->bitmap2;
    }
#else
    if (nz > 0) {
	if (bf == 1)
	    return;
	dir++;
    } else {
	if (!bf)
	    return;
    }
#endif

#define Xchg(a,b) {X=(a);(a)=(b);(b)=X;}

    if (y1 < y0) {
	Xchg (y0, y1);
	Xchg (x0, x1);
#ifdef INTERP
	Xchg (z0, z1);
#endif
#ifdef WRAP
	Xchg (xd0, xd1);
	Xchg (yd0, yd1);
#endif
	dir++;
    }
    if (y2 < y1) {
	Xchg (y2, y1);
	Xchg (x2, x1);
#ifdef INTERP
	Xchg (z2, z1);
#endif
#ifdef WRAP
	Xchg (xd2, xd1);
	Xchg (yd2, yd1);
#endif

	dir++;
    }
    if (y1 < y0) {
	Xchg (y0, y1);
	Xchg (x0, x1);
#ifdef INTERP
	Xchg (z0, z1);
#endif
#ifdef WRAP
	Xchg (xd0, xd1);
	Xchg (yd0, yd1);
#endif
	dir++;
    }
    _color_lookup = color_lookup;
#ifdef INTERP
    c0 = z0;
#else
    if(BYTESPERPIXEL == 1) {
	c = z0;
    } else {
	c = _color_lookup[z0];
    }
#endif

#ifdef WRAP
    X0 = xd0 << SHLB;
    Y0 = yd0 << SHLB;
#endif

    if (y2 == y0)
	return;
    g0 = ((long) (x2 - x0) << SHLB) / (y2 - y0);
    if (y1 != y0)
	g1h = ((long) (x1 - x0) << SHLB) / (y1 - y0);
    if (y2 != y1)
	g1l = ((long) (x2 - x1) << SHLB) / (y2 - y1);

    dir = dir & 1;


/* Very large triangles (larger than the screen) sometimes become a problem,
   if so: */
    if (__clip) {
	if (((abs (x0 - x1) + abs (x1 - x2) + abs (x0 - x2)) >
	     ((__clipx2 - __clipx1) * 2)) || ((y2 - y0) > (__clipy2 - __clipy1)))
	    return;
	if (y2 < __clipy1 || y0 > __clipy2 ||
	    (x0 < __clipx1 && x1 < __clipx1 && x2 < __clipx1) ||
	    (x0 > __clipx2 && x1 > __clipx2 && x2 > __clipx2))
	    return;
    }
    dx0 = x0;
    dy0 = y0;

    if(tri_drawpoint) {
	_tri_drawpoint = tri_drawpoint;
	colhline_pos = linefuncs[(7 * 2) + (BYTESPERPIXEL - 1) * 16];
	colhline_neg = linefuncs[(7 * 2) + (BYTESPERPIXEL - 1) * 16 + 1];
    } else {
	_tri_drawpoint = gl_setpixel;
	colhline_pos = linefuncs[(MODETYPE * 2) + (BYTESPERPIXEL - 1) * 16];
	colhline_neg = linefuncs[(MODETYPE * 2) + (BYTESPERPIXEL - 1) * 16 + 1];
    }

#ifdef tri_set_color
#undef tri_set_color
#endif



#ifdef WRAP
#ifdef INTERP
#define tri_set_color \
		xd = X0 + xd_x * px1 + xd_y * py; \
		yd = Y0 + yd_x * px1 + yd_y * py; \
		c = c0 + c_x * px1 + c_y * py;
#else
#define tri_set_color \
		xd = X0 + xd_x * px1 + xd_y * py; \
		yd = Y0 + yd_x * px1 + yd_y * py;
#endif
#else
#ifdef INTERP
#define tri_set_color \
		c = c0 + c_x * px1 + c_y * py;
#else
#define tri_set_color
#endif
#endif

    if (dir == 1) {
	if (y1 != y0) {
	    py = 0;
	    if (x1 < x0) {
		px1 = 0;
		px2 = -((abs (g1h) >> 1)) >> SHLB;
	    } else {
		px1 = ((abs (g0) >> 1)) >> SHLB;
		px2 = 0;
	    }
	    tri_set_color;

	    colhline_neg ();
	    if ((py = 1) < y1 - y0)
		for (; py < y1 - y0; py++) {
		    px1 = ((g0 * py) + (abs (g0) >> 1)) >> SHLB;
		    px2 = ((g1h * py) - (abs (g1h) >> 1)) >> SHLB;
		    tri_set_color;

		    colhline_neg ();
		}
	    px1 = min (((g0 * py) + (abs (g0) >> 1)) >> SHLB, max (x2, x0) - x0);
	    px2 = x1 - x0;
	    tri_set_color;
	    colhline_neg ();
	} else {
	    py = 0;
	    px1 = 0;
	    px2 = x1 - x0;
	    tri_set_color;

	    colhline_neg ();
	}
	if (y1 != y2) {
	    if ((py = y1 - y0 + 1) < y2 - y0)
		for (; py < y2 - y0; py++) {
		    px1 = ((g0 * py) + (abs (g0) >> 1)) >> SHLB;
		    px2 = (((g1l * (py - y1 + y0)) - (abs (g1l) >> 1)) >> SHLB) + x1 - x0;
		    tri_set_color;

		    colhline_neg ();
		}
	    if (x1 < x2) {
		px1 = x2 - x0;
		px2 = x2 - x0 - ((abs (g1l) >> 1) >> SHLB);
	    } else {
		px1 = x2 - x0 + ((abs (g0) >> 1) >> SHLB);
		px2 = x2 - x0;
	    }
	    tri_set_color;

	    colhline_neg ();
	}
    } else {
	if (y1 != y0) {
	    py = 0;
	    if (x1 > x0) {
		px1 = 0;
		px2 = ((abs (g1h) >> 1)) >> SHLB;
	    } else {
		px1 = -((abs (g0) >> 1)) >> SHLB;
		px2 = 0;
	    }
	    tri_set_color;

	    colhline_pos ();
	    if ((py = 1) < y1 - y0)
		for (; py < y1 - y0; py++) {
		    px1 = ((g0 * py) - (abs (g0) >> 1)) >> SHLB;
		    px2 = ((g1h * py) + (abs (g1h) >> 1)) >> SHLB;
		    tri_set_color;

		    colhline_pos ();
		}
	    px1 = max (((g0 * py) - (abs (g0) >> 1)) >> SHLB, min (x2, x0) - x0);
	    px2 = x1 - x0;
	    tri_set_color;
	    colhline_pos ();
	} else {
	    py = 0;
	    px1 = 0;
	    px2 = x1 - x0;
	    tri_set_color;
	    colhline_pos ();
	}
	if (y1 != y2) {
	    if ((py = y1 - y0 + 1) < y2 - y0)
		for (; py < y2 - y0; py++) {
		    px1 = ((g0 * py) - (abs (g0) >> 1)) >> SHLB;
		    px2 = (((g1l * (py - y1 + y0)) + (abs (g1l) >> 1)) >> SHLB) + x1 - x0;
		    tri_set_color;

		    colhline_pos ();
		}
	    if (x1 > x2) {
		px1 = x2 - x0;
		px2 = x2 - x0 + ((abs (g1l) >> 1) >> SHLB);
	    } else {
		px1 = x2 - x0 - ((abs (g0) >> 1) >> SHLB);
		px2 = x2 - x0;
	    }

	    tri_set_color;

	    colhline_pos ();
	}
    }
}
