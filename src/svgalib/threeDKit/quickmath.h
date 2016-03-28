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

#ifndef QUICK_MATH_H
#define QUICK_MATH_H

#ifndef SVGALIB
#include "../config.h"
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct {
    double x, y, z;
} Vec;


#define sqr(x) ((x)*(x))

#define fswap(a, b) \
{ \
    double __t_var = (a); \
    (a) = (b); \
    (b) = __t_var; \
}


#define swap(a, b) \
{ \
    long __t_var = (a); \
    (a) = (b); \
    (b) = __t_var; \
}

#define max(x,y)     (((x) > (y)) ? (x) : (y))
#define min(x,y)     (((x) < (y)) ? (x) : (y))

#ifndef __GNUC__

double fsqr (double x);
long lsqr (long x);
double fmax (double a, double b);
double fmin (double a, double b);
double fsgn (double a);
double dot (Vec a, Vec b);
Vec cross (Vec a, Vec b);
Vec plus (Vec a, Vec b);
Vec minus (Vec a, Vec b);
Vec times (Vec a, double f);
double norm (Vec a);

#else

extern inline double fsqr (double x)
{
    return x * x;
}

extern inline long lsqr (long x)
{
    return (long) x *x;
}

extern inline double fmax (double a, double b)
{
    return max (a, b);
}

extern inline double fmin (double a, double b)
{
    return min (a, b);
}

extern inline double fsgn (double a)
{
    return (a == 0.0 ? 0.0 : (a > 0.0 ? 1.0 : -1.0));
}

extern inline double dot (Vec a, Vec b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

extern inline Vec cross (Vec a, Vec b)
{
    Vec c;
    c.x = a.y * b.z - a.z * b.y;
    c.y = a.z * b.x - a.x * b.z;
    c.z = a.x * b.y - a.y * b.x;
    return c;
}

extern inline Vec plus (Vec a, Vec b)
{
    Vec c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

extern inline Vec minus (Vec a, Vec b)
{
    Vec c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

extern inline Vec times (Vec a, double f)
{
    Vec c;
    c.x = a.x * f;
    c.y = a.y * f;
    c.z = a.z * f;
    return c;
}

extern inline double norm (Vec a)
{
    return sqrt (sqr (a.x) + sqr (a.y) + sqr (a.z));
}

#endif

void orth_vectors (Vec X, Vec * r1, Vec * r2, double r);

#endif

