/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  Module to support common pixel formats used with
*               SNAP Graphics Architecture.
*
****************************************************************************/

#ifndef	__GAPF_H
#define	__GAPF_H

#include "snap/graphics.h"

/*---------------------- Macros and type definitions ----------------------*/

/* Macros for packing RGB color values for different pixel formats */

#define	PACK_COLOR_15(R,G,B)		\
	( (((N_uint32)(R) & 0xF8) << 7) 	\
	| (((N_uint32)(G) & 0xF8) << 2) 	\
	| (((N_uint32)(B) & 0xF8) >> 3))

#define	PACK_COLOR_16(R,G,B)		\
	( (((N_uint32)(R) & 0xF8) << 8)	\
	| (((N_uint32)(G) & 0xFC) << 3)	\
	| (((N_uint32)(B) & 0xF8) >> 3))

#define	UNPACK_COLOR_15(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 7) & 0xF8);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 2) & 0xF8);			\
 (B) = (N_uint8)(((N_uint32)(c) << 3) & 0xF8);			\
}

#define	UNPACK_COLOR_16(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 8) & 0xF8);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 3) & 0xFC);			\
 (B) = (N_uint8)(((N_uint32)(c) << 3) & 0xF8);			\
}

#define	PACK_COLOR_RGB(R,G,B)		\
	( (N_uint32)(R)  << 16)			\
	| ((N_uint32)(G) << 8)				\
	| ((N_uint32)(B) << 0)

#define	PACK_COLOR_BGR(R,G,B)		\
	( (N_uint32)(R)  << 0)				\
	| ((N_uint32)(G) << 8)				\
	| ((N_uint32)(B) << 16)

#define	UNPACK_COLOR_RGB(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 0) & 0xFF);			\
}

#define	UNPACK_COLOR_BGR(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 0) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
}

#define	PACK_COLOR_ARGB(R,G,B)		\
	( (N_uint32)(R) << 16)				\
	| ((N_uint32)(G) << 8)				\
	| ((N_uint32)(B) << 0)

#define	PACK_COLOR_ABGR(R,G,B)		\
	( (N_uint32)(R) << 0)				\
	| ((N_uint32)(G) << 8)				\
	| ((N_uint32)(B) << 16)

#define	PACK_COLOR_RGBA(R,G,B)		\
	( (N_uint32)(R) << 24)				\
	| ((N_uint32)(G) << 16)			\
	| ((N_uint32)(B) << 8)

#define	PACK_COLOR_BGRA(R,G,B)		\
	( (N_uint32)(R) << 8)				\
	| ((N_uint32)(G) << 16)			\
	| ((N_uint32)(B) << 24)

#define	UNPACK_COLOR_ARGB(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 0) & 0xFF);			\
}

#define	UNPACK_COLOR_ABGR(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 0) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
}

#define	UNPACK_COLOR_RGBA(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 24) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
}

#define	UNPACK_COLOR_BGRA(c,R,G,B)					\
{													\
 (R) = (N_uint8)(((N_uint32)(c) >> 8) & 0xFF);			\
 (G) = (N_uint8)(((N_uint32)(c) >> 16) & 0xFF);			\
 (B) = (N_uint8)(((N_uint32)(c) >> 24) & 0xFF);			\
}

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Global pixel format descriptors */

extern GA_pixelFormat GA_pf15;
extern GA_pixelFormat GA_pf16;
extern GA_pixelFormat GA_pf24_RGB;
extern GA_pixelFormat GA_pf24_BGR;
extern GA_pixelFormat GA_pf32_ARGB;
extern GA_pixelFormat GA_pf32_ABGR;
extern GA_pixelFormat GA_pf32_RGBA;
extern GA_pixelFormat GA_pf32_BGRA;

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif	/* __GAPF_H */

