/*

    3DKIT   version   1.2
    High speed 3D graphics and rendering library for Linux.

    1996  Paul Sheer   psheer@icon.co.za

    This file is an example program demonstrating the use of the
    3dkit library. It is not part of the library and is not copyright.

*/

/*
File: wrapdemo.h
*/

/*choose method (see 3dkit.h for explanation)*/
/*comment out for real world-like view:*/
/*#define WORLD_VIEW 0*/

#ifdef WORLD_VIEW
#define EL_METER 1200
	 /* In this demo EL_METER is about a meter
	    so the wing is 20 * 1200 = 24000 units across,
	    within the 65536 limit. */
#else
#define EL_METER 2400
#endif


