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
* Description:  Module to support BMP bitmap loading and saving with
*               SNAP Graphics Architecture.
*
****************************************************************************/

#ifndef	__GABMP_H
#define	__GABMP_H

#include "snap/graphics.h"

/*---------------------- Macros and type definitions ----------------------*/

/* Structure for bitmaps loaded from disk */

typedef struct {
	int         	width;
	int         	height;
	int         	bitsPerPixel;
	int         	bytesPerLine;
	N_uint8       	*surface;
	GA_palette   	*pal;
	GA_pixelFormat	*pf;
	/* ... palette, pixel format and bitmap data are store here */
	} bmp_t;

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Windows .BMP bitmap loading/saving functions */

bmp_t * LoadBitmap(const char *bitmapName,ibool loadPalette);
bmp_t * CreateBitmap(int width,int height,int bitsPerPixel,int bytesPerLine,GA_palette *pal,GA_pixelFormat *pf,void *surface);
ibool	SaveBitmap(bmp_t *bmp,const char *bitmapName);
void 	UnloadBitmap(bmp_t *bitmap);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif	/* __GABMP_H */

