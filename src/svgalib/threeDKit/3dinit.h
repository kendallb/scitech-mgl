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

File: 3dinit.h

*/

#include "quickmath.h"

void TD_initcolor (TD_Surface * surf, int n);

void TD_initellipsoidpart (TD_Surface * surf, long x, long y, long z,
		      long a, long b, long c, int w, int dir, int col);
void TD_initellipsoid (TD_Surface * surf1, TD_Surface * surf2, TD_Surface * surf3,
	TD_Surface * surf4, TD_Surface * surf5, TD_Surface * surf6, long x, 
	long y, long z, long a, long b, long c, int w, int col);
void TD_initsellipsoid (TD_Solid *s, int n, long x, 
	long y, long z, long a, long b, long c, int w, int col);


