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

#include "snap/graphics.h"

/*---------------------------- Global Variables ---------------------------*/

/* Global pixel format descriptors */

GA_pixelFormat GA_pf15 = {
    0x1F,0x0A,3,0x1F,0x05,3,0x1F,0x00,3,0x01,0x0F,7,    /* 555 15bpp */
    };

GA_pixelFormat GA_pf16 = {
    0x1F,0x0B,3,0x3F,0x05,2,0x1F,0x00,3,0x00,0x00,0,    /* 565 16bpp */
    };

GA_pixelFormat GA_pf24_RGB = {
    0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0,    /* RGB 24bpp */
    };

GA_pixelFormat GA_pf24_BGR = {
    0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0x00,0x00,0,    /* BGR 24bpp */
    };

GA_pixelFormat GA_pf32_ARGB = {
    0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0xFF,0x18,0,    /* ARGB 32bpp */
    };

GA_pixelFormat GA_pf32_ABGR = {
    0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0,    /* ABGR 32bpp */
    };

GA_pixelFormat GA_pf32_RGBA = {
    0xFF,0x18,0,0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,    /* RGBA 32bpp */
    };

GA_pixelFormat GA_pf32_BGRA = {
    0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0,0xFF,0x00,0,    /* BGRA 32bpp */
    };


