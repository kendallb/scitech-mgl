/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Environment:  Any
*
* Description:  Internal header file for the Win32 bindings for the MGL.
*
****************************************************************************/

#ifndef __MGLWIN_INTERNAL_H
#define __MGLWIN_INTERNAL_H

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

/*---------------------- Macro and type definitions -----------------------*/

/* Windows LOGPALETTE palette structure */

typedef struct {
    uchar       peRed;
    uchar       peGreen;
    uchar       peBlue;
    uchar       peFlags;
    } win_PALENT;

typedef struct {
    ushort      version;
    ushort      entries;
    win_PALENT  pe[256];
    } LogPal;

/* Windows internal HBITMAP structure */

typedef struct {
    short       bmType;             /* 0 for main memory                    */
    ushort      bmWidth;            /* Width of bitmap in pixels            */
    ushort      bmHeight;           /* Height of bitmap in pixels           */
    ushort      bmWidthBytes;       /* #bytes per scan line                 */
    uchar       bmPlanes;           /* # of planes in bitmap                */
    uchar       bmBitsPixel;        /* # bits per pixel                     */
    uchar       *bmBits;            /* pointer to bits of main mem bitmap   */
    ulong       bmWidthPlanes;      /* bmWidthBytes * bmHeight              */
    uchar       *bmlpPDevice;       /* Pointer to associated pDevice        */
    ushort      bmSegmentIndex;     /* Index to planes next segment         */
    ushort      bmScanSegment;      /* Number of scans per segment          */
    ushort      bmFillBytes;        /* Number of unused bytes per segment   */
    } win_BITMAP;

/*------------------------- Function Prototypes ---------------------------*/

#endif  /* __MGLWIN_INTERNAL_H */

