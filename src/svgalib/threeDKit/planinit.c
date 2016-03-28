/*

    3DKIT   version   1.3
    High speed 3D graphics and rendering library for Linux.

    1996  Paul Sheer   psheer@icon.co.za

    This file is an example program demonstrating the use of the
    3dkit library. It is not part of the library and is not copyright.

    The author take no responsibility, for the results
    of compilation, execution or other usage of this program.
*/


/*
File: planinit.c
*/


/*  The following define the plane's geometry:
    I once had a list of what every variable did,
    not any more, but I'll label those that I know. */

/*fuselage radius*/
#define FRAD (float) 1.1

/*length of tapered portion of fuselage*/
#define FD 8

/*length of various other portions*/
#define FB (float)2.65
#define FA (float)2.78
#define FC (float)3.80
#define FE (float)1.75

#define QM (float).4
#define QM2 (float).15
#define WH (float).5

/*wing span*/
#define SPAN 20

/*root chord length*/
#define CHORD 3

/*wing dihedral*/
#define DIHEDRAL (float).12

/*wing taper*/
#define TAPER .4

#define PRAT (float)M_PI/3

/*elevation of stabiliser*/
#define TAILHEIGHT (float)2.8

/*stabiliser chord, dihedral, taper and span*/
#define TCHORD (float)1.2
#define TDIHEDRAL (float).02
#define TTAPER (float).7
#define TSPAN 5

/*fin length*/
#define VLENGTH (float)2.3

/*fin base elevation, taper, base chord*/
#define VHEIGHT (float).5
#define VTAPER (float).7
#define VCHORD (float)1.7

/*distance between propeller centres*/
#define PROPSPAN 4

/*nacelle radius, length, and two other shape params*/
#define NACRAD (float).4
#define NACLEN (float)1.6
#define RNACLEN (float)2.4
#define NACHEIGHT (float).4




#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "vga.h"
#include "vgagl.h"
#include "./3dkit.h"
#include "./plane.h"

extern int DENS;
extern int DENS2;

float rib[20][3] =
    {{-29.7, 0, 0}, {-29.5, 1, 0}, {-29, 2, 0},
     {-28, 3, 0}, {-25, 5.3, 0}, {-20, 6.7, 0}, {-12, 8, 0},
     {1, 9, 0}, {19, 9, 0}, {39, 7.5, 0}, {59, 4.5, 0}, {82, 0, 0}};


void initwing (TD_Surface * surf, int lsf, int usd, int half)
{
    int i, k;
    float j;
    int LSC = 110;
    int widtth, length;

    surf->l = widtth = 12;
    surf->w = length = DENS2 + 1;

    for (k = 0; k < length; k++)
	for (i = 0; i < widtth; i++) {
	    j = k;
	    if (lsf * usd == -1)
		j = length - k - 1;
	    j = j / 4 + (float) DENS2 *half / 4;
	    surf->point[i * length + k].x = (float) ((float) j / DENS2 * (SPAN / 2 - FRAD) + FRAD) * lsf * PL_METER;
	    surf->point[i * length + k].y = -(float) rib[i][0] / LSC * CHORD * (1 - (float) j / DENS2 * (1 - TAPER)) * PL_METER;
	    surf->point[i * length + k].z = ((float) rib[i][1] / LSC * CHORD * usd * (1 - (float) j / DENS2 * (1 - TAPER))
					     / ((float) 1.5 - (float) usd / 2) - WH + (float) j / DENS2 * DIHEDRAL * SPAN / 2) * PL_METER;
	}
}


void inittips (TD_Surface * surf, int lsf)
{
    int i, k, j, usd;
    int LSC = 110;
    int widtth, length;

    surf->l = widtth = 12;
    surf->w = length = 2;

    for (j = 0, k = -1; k < 2; k += 2, j++)
	for (i = 0; i < widtth; i++) {
	    usd = -k * lsf;
	    surf->point[i * length + j].x = (float) ((float) (SPAN / 2 - FRAD) + FRAD) * lsf * PL_METER;
	    surf->point[i * length + j].y = -(float) rib[i][0] / LSC * CHORD * (1 - (float) (1 - TAPER)) * PL_METER;
	    surf->point[i * length + j].z = ((float) rib[i][1] / LSC * CHORD * usd * (1 - (float) (1 - TAPER)) /
		     ((float) 1.5 - (float) usd / 2) - WH + (float) DIHEDRAL * SPAN / 2) * PL_METER;
	}
}

void initstab (TD_Surface * surf, int lsf, int usd)
{
    int i, j, k;
    int LSC = 110;
    int widtth, length;

    surf->l = widtth = 12;
    surf->w = length = DENS2 + 1;

    for (k = 0; k < length; k++)
	for (i = 0; i < widtth; i++) {
	    j = k;
	    if (lsf * usd == -1)
		j = length - k - 1;

	    surf->point[i * length + k].x = (float) j / DENS2 * TSPAN / 2 * lsf * PL_METER;
	    surf->point[i * length + k].y = (-(float) rib[i][0] / LSC * TCHORD * (1 - (float) j / DENS2 * (1 - TTAPER)) - FB - FD) * PL_METER;
	    surf->point[i * length + k].z = ((float) rib[i][1] / LSC * TCHORD * usd * (1 - (float) j / DENS2 * (1 - TTAPER)) / 2 + TAILHEIGHT + (float) j / DENS2 * TDIHEDRAL * TSPAN / 2) * PL_METER;
	}
}


void initfin (TD_Surface * surf, int usd)
{
    int i, j, k;
    int LSC = 110;
    int widtth, length;
    float locrad;

    surf->l = widtth = 12;
    surf->w = length = DENS2 + 1;

    locrad = (sin ((float) M_PI / 2 * VCHORD / FD)) * FRAD;

    for (k = 0; k < length; k++)
	for (i = 0; i < widtth; i++) {
	    j = k;
	    if (usd == 1)
		j = length - k - 1;

	    surf->point[i * length + k].x = ((float) rib[i][1] / LSC * VCHORD * usd * (1 - (float) j / DENS2 * (1 - VTAPER)) / 2) * PL_METER;
	    surf->point[i * length + k].y = (-(float) rib[i][0] / LSC * VCHORD * (1 - (float) j / DENS2 * (1 - VTAPER)) - FB - FD - (float) VCHORD / 2 * ((float) j / DENS2 - 1)) * PL_METER;
	    surf->point[i * length + k].z = ((float) j / DENS2 * (TAILHEIGHT - locrad) + locrad) * PL_METER;
	}
}



void initfus (TD_Surface * surf, float quart)
{
    int i, j;
    int widtth, length;
    float locrad;

    surf->w = widtth = surf->l = length = DENS + 1;

    for (j = length - 1; j >= 0; j--)
	for (i = 0; i < widtth; i++) {
	    locrad = (sin ((float) M_PI / 2 * j / DENS + .02)) * FRAD;
	    surf->point[i + j * widtth].x = (float) cos ((float) i / DENS * M_PI / 2 + quart) * PL_METER * locrad;
	    surf->point[i + j * widtth].y = ((float) j / DENS * FD - FB - FD) * PL_METER;
	    surf->point[i + j * widtth].z = (float) sin ((float) i / DENS * M_PI / 2 + quart) * PL_METER * locrad;
	}

}


void initfus1 (TD_Surface * surf, float quart)
{
    int i, j;
    int widtth, length;
    float locrad;

    surf->w = widtth = surf->l = length = DENS + 1;

    for (j = length - 1; j >= 0; j--)
	for (i = 0; i < widtth; i++) {
	    locrad = FRAD;
	    surf->point[i + j * widtth].x = (float) cos ((float) i / DENS * M_PI / 2 + quart) * PL_METER * locrad;
	    surf->point[i + j * widtth].y = ((float) j / DENS * (FA + FB) - FB) * PL_METER;
	    surf->point[i + j * widtth].z = (float) sin ((float) i / DENS * M_PI / 2 + quart) * PL_METER * locrad;
	}
}

void initfus2 (TD_Surface * surf, float quart)
{
    int i, j;
    int widtth, length;
    float locrad, ya, q;

    surf->w = widtth = surf->l = length = DENS + 1;


    for (j = length - 1; j >= 0; j--)
	for (i = 0; i < widtth; i++) {
	    ya = ((float) j / DENS * FC + FA);
	    locrad = (cos ((float) PRAT * j / DENS)) * FRAD;
	    q = (ya - FA) / FC * QM;
	    surf->point[i + j * widtth].x = ((float) cos ((float) i / DENS * M_PI / 2 + quart) * locrad) * PL_METER;
	    surf->point[i + j * widtth].y = ya * PL_METER;
	    surf->point[i + j * widtth].z = ((float) sin ((float) i / DENS * M_PI / 2 + quart) * locrad - q) * PL_METER;
	}

}

void initfus3 (TD_Surface * surf, float quart)
{
    int i, j;
    int widtth, length;
    float ya, locrad, q;

    surf->w = widtth = surf->l = length = DENS + 1;

    for (j = length - 1; j >= 0; j--)
	for (i = 0; i < widtth; i++) {
	    ya = ((float) j / DENS * FE + (float) FA + (float) FC);
	    locrad = (cos ((float) PRAT)) * FRAD * sqrt ((float) ((float) FE - ya + (float) FA + (float) FC + .01) / FE);
	    q = QM + (ya - FA - FC) / FE * QM2;
	    surf->point[i + j * widtth].x = ((float) cos ((float) i / DENS * M_PI / 2 + quart) * locrad) * PL_METER;
	    surf->point[i + j * widtth].y = ya * PL_METER;
	    surf->point[i + j * widtth].z = ((float) sin ((float) i / DENS * M_PI / 2 + quart) * locrad - q) * PL_METER;
	}

}

void initnacelle (TD_Surface * surf, float quart, int lor)
{
    int i, j;
    int widtth, length;
    float xa, ya, za, locrad, q, nz, ny;

    surf->w = widtth = surf->l = length = DENS + 1;

    nz = (float) DIHEDRAL *PROPSPAN - WH + NACHEIGHT;
    ny = (float) .27 *CHORD - (.27 * CHORD * PROPSPAN / SPAN / 2);

    for (j = length - 1; j >= 0; j--)
	for (i = 0; i < widtth; i++) {
	    ya = ((float) j / DENS * NACLEN + ny);
	    locrad = (float) NACRAD *sqrt ((float) ((float) NACLEN + ny - ya + .01) / NACLEN);
	    q = 0;
	    xa = (float) cos ((float) i / DENS * M_PI / 2 + quart) * locrad;
	    surf->point[i + j * widtth].x = (xa + lor * PROPSPAN) * PL_METER;
	    surf->point[i + j * widtth].y = ya * PL_METER;
	    if (quart > 1.6) {
		za = (float) sin ((float) i / DENS * M_PI / 2 + quart) * locrad * 2.5 - q;	/*%%%%%%%%%% */
	    } else {
		za = (float) sin ((float) i / DENS * M_PI / 2 + quart) * locrad - q;	/*%%%%%%%%%% */
	    }
	    surf->point[i + j * widtth].z = (za + nz) * PL_METER;

	}

}


void initnacelle2 (TD_Surface * surf, float quart, int lor)
{
    int i, j;
    int widtth, length;
    float xa, ya, za, locrad, q, nz, ny;

    surf->w = widtth = surf->l = length = DENS + 1;

    nz = (float) DIHEDRAL *PROPSPAN - WH + NACHEIGHT;
    ny = (float) .27 *CHORD - (.27 * CHORD * PROPSPAN / SPAN / 2);

    for (j = 0; j < length; j++)
	for (i = 0; i < widtth; i++) {
	    ya = ((float) -j / DENS * RNACLEN + ny);
	    locrad = (float) NACRAD *sqrt ((float) -((float) -RNACLEN + ny - ya - .01) / RNACLEN);
	    q = 0;
	    xa = (float) cos ((float) i / DENS * M_PI / 2 + quart) * locrad;
	    surf->point[i * length + j].x = (xa + lor * PROPSPAN) * PL_METER;
	    surf->point[i * length + j].y = ya * PL_METER;
	    za = (float) sin ((float) i / DENS * M_PI / 2 + quart) * locrad - q;
	    surf->point[i * length + j].z = (za + nz) * PL_METER;
	}
}










