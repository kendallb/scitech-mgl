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
* Environment:  Windows 95
*
* Description:  Mini driver version of the "old" DIB.DRV for Windows 95.
*
*               MGL uses the MGLDIB.DRV like so:
*
*               hdc = CreateDC("MGLDIB",NULL,NULL,&MGL_dibInfo);
*
*               this makes a DC that will draw onto the passed packed DIB
*
****************************************************************************/

#ifndef __MGLDIB_H
#define __MGLDIB_H

/*---------------------- Macros and type definitions ----------------------*/

#define MGLDIB_SELECTDIB        41
#define MGLDIB_SETSURFACEPTR    42
#define MGLDIB_SETPALETTE       43

typedef struct {
    DWORD               maskRed;
    DWORD               maskGreen;
    DWORD               maskBlue;
    } MGL_dibMasks;

typedef  struct {
    BITMAPINFOHEADER    h;
    union {
        RGBQUAD         pal[256];
        MGL_dibMasks    m;
        } p;
    DWORD               dwSurface;
    } MGL_dibInfo;

#endif  /* __MGLDIB_H */
