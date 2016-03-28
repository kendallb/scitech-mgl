/*

    3DKIT   version   1.3
    High speed 3D graphics and rendering library for Linux.

    1996  Paul Sheer   psheer@icon.co.za

    This file is an example program demonstrating the use of the
    3dkit library. It is not part of the library and is not copyright.

*/

/*
File: plane.h
*/

/*choose method (see 3dkit.h for explanation)*/
/*comment out for real world-like view:*/
/*#define WORLD_VIEW 0*/

#ifdef WORLD_VIEW
#define PL_METER 1200
	 /* In this demo PL_METER is about a meter
	    so the wing is 20 * 1200 = 24000 units across,
	    within the 65536 limit. */
#else
#define PL_METER 2400
#endif


void initwing (TD_Surface * surf, int lsf, int usd, int half);
void inittips (TD_Surface * surf, int lsf);
void initstab (TD_Surface * surf, int lsf, int usd);
void initfin (TD_Surface * surf, int usd);
void initfus (TD_Surface * surf, float quart);
void initfus1 (TD_Surface * surf, float quart);
void initfus2 (TD_Surface * surf, float quart);
void initfus3 (TD_Surface * surf, float quart);
void initnacelle (TD_Surface * surf, float quart, int lor);
void initnacelle2 (TD_Surface * surf, float quart, int lor);
