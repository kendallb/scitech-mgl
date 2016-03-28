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
* Description:  Header file describing the OS specific driver import
*               functions, which are passed into the loaded drivers by the
*               OS specific loader code to implement OS specific features.
*
****************************************************************************/

#ifndef __DRIVERS_COMMON_IMPORTS_H
#define __DRIVERS_COMMON_IMPORTS_H

#include "clib/peloader.h"

/*---------------------- Macro and type definitions -----------------------*/

typedef struct {
    /* Size of the driver structures */
    long        dwSize;

    /* Pointers to useful information passed into driver */
    drivertype  *drvPacked;
    void        *_MGL_buf;

    /* OS specific memory allocation functions */
    void *  (MGLAPI *PM_malloc)(long size);
    void *  (MGLAPI *PM_calloc)(long s,long n);
    void    (MGLAPI *PM_free)(void *p);

    /* OS specific bitmap manipulation functions */
    long    (MGLAPI *MGL_allocateDIB)(MGLDC *dc,winBITMAPINFO *info);
    void    (MGLAPI *PM_freeDIB)(MGLDC *dc);
    void    (MGLAPI *PACKED8_realizePalette)(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
    } DRV_imports;

/* Macros to calls functions via our import table for the driver code */

#define PM_malloc(s)                _DRV_imports.MGL_malloc(s)
#define PM_calloc(s,n)              _DRV_imports.MGL_calloc(s,n)
#define PM_free(p)                  _DRV_imports.MGL_free(p)
#define MGL_allocateDIB(dc,info)    _DRV_imports.MGL_allocateDIB(dc,info)
#define PM_freeDIB(dc)              _DRV_imports.MGL_freeDIB(dc)

/* Define a macro to easily access global device context */

#define DC              _MGL_dc

typedef drivertype * (SNAP_CDECL *MGL_initDriver_t)(PM_imports *pmImp,N_imports *nImp,GA_imports *gaImp,DRV_imports *drvImp);

/*--------------------------- Global Variables ----------------------------*/

extern PM_imports   _VARAPI *_PM_imports;
extern N_imports    _VARAPI *_N_imports;
extern GA_imports   _VARAPI *_GA_imports;
extern DRV_imports  _VARAPI _DRV_imports;
extern MGLDC        _VARAPI _MGL_dc;
extern drivertype   DriverHeader;

#endif  /* __DRIVERS_COMMON_IMPORTS_H */
