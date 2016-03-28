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

#ifdef lookup
#undef lookup
#endif

#if (TRI_BPP==8) || !defined(INTERP)
#define lookup(x) x
#else
#define lookup(x) _color_lookup[x]
#endif


#ifdef assignvp
#undef declarevp
#undef assignvp
#undef assignvpoffset
#undef incvp
#undef incvpoffset
#undef decvp
#undef decvpoffset
#endif


#if TRI_BPP==8
#define declarevp declarevp8
#define assignvp assignvp8
#define assignvpoffset assignvpoffset8
#define incvp incvp8
#define incvpoffset incvpoffset8
#define decvp decvp8
#define decvpoffset decvpoffset8
#endif
#if TRI_BPP==16
#define declarevp declarevp16
#define assignvp assignvp16
#define assignvpoffset assignvpoffset16
#define incvp incvp16
#define incvpoffset incvpoffset16
#define decvp decvp16
#define decvpoffset decvpoffset16
#endif
#if TRI_BPP==24
#define declarevp declarevp24
#define assignvp assignvp24
#define assignvpoffset assignvpoffset24
#define incvp incvp24
#define incvpoffset incvpoffset24
#define decvp decvp24
#define decvpoffset decvpoffset24
#endif
#if TRI_BPP==32
#define declarevp declarevp32
#define assignvp assignvp32
#define assignvpoffset assignvpoffset32
#define incvp incvp32
#define incvpoffset incvpoffset32
#define decvp decvp32
#define decvpoffset decvpoffset32
#endif


#ifdef color
#undef color
#endif

#ifdef WRAP
#ifdef INTERP
#define color \
    (dat[(xd >> SHLB) + (yd & S_MASK)] + (c >> (SHLB + SHC)))
#else
#define color \
    (dat[(xd >> SHLB) + (yd & S_MASK)] + c)
#endif
#else
#ifdef INTERP
#define color \
    (c >> (SHLB + SHC))
#else
#define color \
    c
#endif
#endif


/* these are actually all the same except for 8 */
#if TRI_BPP==8
static void colhline_neg_setpixel8 (void)
#endif
#if TRI_BPP==16
static void colhline_neg_setpixel16 (void)
#endif
#if TRI_BPP==24
static void colhline_neg_setpixel24 (void)
#endif
#if TRI_BPP==32
static void colhline_neg_setpixel32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;
	if (x1 > __clipx2 + 1) {
#ifdef WRAP
	    xd -= (x1 - __clipx2 - 1) * xd_x;
	    yd -= (x1 - __clipx2 - 1) * yd_x;
#endif
#ifdef INTERP
	    c -= (x1 - __clipx2 - 1) * c_x;
#endif
	    x1 = __clipx2 + 1;
	}
	if (x2 < __clipx1) {
	    x2 = __clipx1;
	}
    }
    count = x1 - x2;
    if (count > 0) {
	do {
	    _tri_drawpoint (--x1, y, lookup(color));
#ifdef WRAP
	    yd -= yd_x;
	    xd -= xd_x;
#endif
#ifdef INTERP
	    c -= c_x;
#endif
	} while (--count);
    }
}


#if TRI_BPP==8
static void colhline_pos_setpixel8 (void)
#endif
#if TRI_BPP==16
static void colhline_pos_setpixel16 (void)
#endif
#if TRI_BPP==24
static void colhline_pos_setpixel24 (void)
#endif
#if TRI_BPP==32
static void colhline_pos_setpixel32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;
	if (x1 < __clipx1) {
#ifdef WRAP
	    xd += (__clipx1 - x1) * xd_x;
	    yd += (__clipx1 - x1) * yd_x;
#endif
#ifdef INTERP
	    c += (__clipx1 - x1) * c_x;
#endif
	    x1 = __clipx1;
	}
	if (x2 > __clipx2 + 1) {
	    x2 = __clipx2 + 1;
	}
    }
    count = x2 - x1;
    if (count > 0) {
	do {
	    _tri_drawpoint (x1++, y, lookup(color));
#ifdef WRAP
	    yd += yd_x;
	    xd += xd_x;
#endif
#ifdef INTERP
	    c += c_x;
#endif
	} while (--count);
    }
}

#ifndef DO_NOT_USE_VGALIB
/*draw to 64k vga buffer setting vga page appropriately: */

#if TRI_BPP==8
static void colhline_neg_paged8 (void)
#endif
#if TRI_BPP==16
static void colhline_neg_paged16 (void)
#endif
#if TRI_BPP==24
static void colhline_neg_paged24 (void)
#endif
#if TRI_BPP==32
static void colhline_neg_paged32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    unsigned short offst;
    int pg;
    declarevp;
    long vp = 0;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;

	if (x1 > __clipx2 + 1) {
#ifdef WRAP
	    xd -= (x1 - __clipx2 - 1) * xd_x;
	    yd -= (x1 - __clipx2 - 1) * yd_x;
#endif
#ifdef INTERP
	    c -= (x1 - __clipx2 - 1) * c_x;
#endif
	    x1 = __clipx2 + 1;
	}
	if (x2 < __clipx1) {
	    x2 = __clipx1;
	}
    }
    count = x1 - x2;
    assignvpoffset (x1, y, vp);
    pg = vp >> 16;
    vga_setpage (pg);
    offst = vp;
    if (count > 0) {
	do {
	    decvpoffset;
#ifdef WRAP
	    yd -= yd_x;
	    xd -= xd_x;
#endif
#ifdef INTERP
	    c -= c_x;
#endif
	} while (--count);
    }
}


#if TRI_BPP==8
static void colhline_pos_paged8 (void)
#endif
#if TRI_BPP==16
static void colhline_pos_paged16 (void)
#endif
#if TRI_BPP==24
static void colhline_pos_paged24 (void)
#endif
#if TRI_BPP==32
static void colhline_pos_paged32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    unsigned short offst;
    int pg;
    declarevp;
    long vp = 0;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;

	if (x1 < __clipx1) {
#ifdef WRAP
	    xd += (__clipx1 - x1) * xd_x;
	    yd += (__clipx1 - x1) * yd_x;
#endif
#ifdef INTERP
	    c += (__clipx1 - x1) * c_x;
#endif
	    x1 = __clipx1;
	}
	if (x2 > __clipx2 + 1) {
	    x2 = __clipx2 + 1;
	}
    }
    count = x2 - x1;
    assignvpoffset (x1, y, vp);
    pg = vp >> 16;
    vga_setpage (pg);
    offst = vp;
    if (count > 0) {
	do {
	    incvpoffset;
#ifdef WRAP
	    yd += yd_x;
	    xd += xd_x;
#endif
#ifdef INTERP
	    c += c_x;
#endif
	} while (--count);
    }
}


#else


/*draw to 64k vga buffer setting vga page appropriately: */

#if TRI_BPP==8
static void colhline_neg_paged8 (void)
#endif
#if TRI_BPP==16
static void colhline_neg_paged16 (void)
#endif
#if TRI_BPP==24
static void colhline_neg_paged24 (void)
#endif
#if TRI_BPP==32
static void colhline_neg_paged32 (void)
#endif
{
}


#if TRI_BPP==8
static void colhline_pos_paged8 (void)
#endif
#if TRI_BPP==16
static void colhline_pos_paged16 (void)
#endif
#if TRI_BPP==24
static void colhline_pos_paged24 (void)
#endif
#if TRI_BPP==32
static void colhline_pos_paged32 (void)
#endif
{
}


#endif

/*draw to a linear address space (320x200 or virtual screen): */
#if TRI_BPP==8
static void colhline_neg_direct8 (void)
#endif
#if TRI_BPP==16
static void colhline_neg_direct16 (void)
#endif
#if TRI_BPP==24
static void colhline_neg_direct24 (void)
#endif
#if TRI_BPP==32
static void colhline_neg_direct32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    declarevp;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;
	if (x1 > __clipx2 + 1) {
#ifdef WRAP
	    xd -= (x1 - __clipx2 - 1) * xd_x;
	    yd -= (x1 - __clipx2 - 1) * yd_x;
#endif
#ifdef INTERP
	    c -= (x1 - __clipx2 - 1) * c_x;
#endif
	    x1 = __clipx2 + 1;
	}
	if (x2 < __clipx1)
	    x2 = __clipx1;
    }
    count = x1 - x2;
    assignvp (x1, y, vp);
    if (count > 0) {
	do {
	    decvp;
#ifdef WRAP
	    yd -= yd_x;
	    xd -= xd_x;
#endif
#ifdef INTERP
	    c -= c_x;
#endif
	} while (--count);
    }
}


#if TRI_BPP==8
static void colhline_pos_direct8 (void)
#endif
#if TRI_BPP==16
static void colhline_pos_direct16 (void)
#endif
#if TRI_BPP==24
static void colhline_pos_direct24 (void)
#endif
#if TRI_BPP==32
static void colhline_pos_direct32 (void)
#endif
{
    int count, y = py + dy0, x1 = px1 + dx0, x2 = px2 + dx0;
    declarevp;
    if (__clip) {
	if (y < __clipy1 || y > __clipy2)
	    return;
	if (x1 < __clipx1) {
#ifdef WRAP
	    xd += (__clipx1 - x1) * xd_x;
	    yd += (__clipx1 - x1) * yd_x;
#endif
#ifdef INTERP
	    c += (__clipx1 - x1) * c_x;
#endif
	    x1 = __clipx1;
	}
	if (x2 > __clipx2 + 1)
	    x2 = __clipx2 + 1;
    }
    count = x2 - x1;
    assignvp (x1, y, vp);
    if (count > 0) {
	do {
	    incvp;
#ifdef WRAP
	    yd += yd_x;
	    xd += xd_x;
#endif
#ifdef INTERP
	    c += c_x;
#endif
	} while (--count);
    }
}

/*The following have not yet been implemented */

/* Draws to planar 256 (these could be complicated) */
/*static void colhline_neg_planar (void);
static void colhline_pos_planar (void);*/

/* Draws using accelerated */
/*static void colhline_neg_accel (void);
static void colhline_pos_accel (void);*/


