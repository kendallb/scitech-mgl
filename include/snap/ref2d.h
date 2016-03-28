/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  Header file for the SNAP Graphics 2D Reference Rasteriser.
*
****************************************************************************/

#ifndef __SNAP_REF2D_H
#define __SNAP_REF2D_H

#include "snap/graphics.h"
#include "clib/modloadr.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(4)

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Structure for the 2d referster rasteriser, which is returned by the
REF2D_loadDriver function. This structure also contains all the function
pointers used to communicate with the 2d reference rasteriser library.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!

Note:   The dwSize member is of type N_uintptr to ensure that it has the
        same size as each of the function pointers in the structure.
****************************************************************************/
typedef struct _REF2D_driver {
    N_uintptr           dwSize;
    ibool               (NAPIP QueryFunctions)(N_uint32 id,void *funcs);
    N_int32             (NAPIP SetDrawBuffer)(GA_buffer *drawBuf,void *framebuffer,N_int32 bitsPerPixel,GA_pixelFormat *pf,GA_devCtx *hwCtx,N_int32 softwareOnly);
    void                (NAPIP SetDrawSurface)(void *surface,N_int32 xRes,N_int32 yRes,N_int32 bytesPerLine,N_int32 bitsPerPixel,GA_pixelFormat *pf);
    void *              (NAPIP RotateBitmap)(void *src,N_int32 bitsPerPixel,N_int32 *stride,N_int32 width,N_int32 height);
    void                (NAPIP UnloadDriver)(void);
    ibool               (NAPIP InitFilter)(GA_modeInfo *modeInfo,N_int32 transferStart,struct _REF2D_driver *ref2d,struct _REF2D_driver **drv);
    void                (NAPIP DeInitFilter)(void);
    void                (NAPIP GlobalOptionsUpdated)(void);
    void *              res1;
    void                (NAPIP DrawRectExtSW)(void *buffer,N_int32 dstPitch,N_int32 bitsPerPixel,N_int32 flags,N_int32 left,N_int32 top,N_int32 width,N_int32 height,GA_color color,N_int32 mix);
    N_int32             (NAPIP ForceSoftwareOnly)(N_int32 enable);
    N_uint32            (NAPIP SetColorCompareMask)(N_uint32 mask);
    void                (NAPIP PostSwitchPhysicalResolution)(void);
    ibool               (NAPIP SetLSBOrdering)(ibool enable);
    N_int32             (NAPIP SetDrawBufferExt)(GA_buffer *drawBuf,void *framebuffer,void *surface,N_int32 stride,N_int32 bitsPerPixel,GA_pixelFormat *pf,GA_devCtx *hwCtx,N_int32 softwareOnly);
    /* {secret} */
    void *              res2[35];

    /* {secret} Internal pointer to the driver state buffer - used by filter drivers */
    void                _FAR_ *DriverState;

    /* {secret} Pointer to the start of driver and size of driver in bytes */
    void                _FAR_ *DriverStart;
    /* {secret} */
    N_uint32            DriverSize;
    } REF2D_driver;

#pragma pack()

/* {secret} */
typedef ibool (NAPIP GA_setActiveDevice_t)(N_int32 deviceIndex);

#ifndef __16BIT__
/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef REF2D_driver * (NAPIP REF2D_initLibrary_t)(
    GA_devCtx *hwCtx,
    GA_globalOptions *opt,
    struct __GA_exports *gaExp,
    PM_imports *pmImp);

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef REF2D_driver * (NAPIP FILT_initLibrary_t)(
    GA_devCtx *hwCtx,
    GA_globalOptions *opt,
    PM_imports *pmImp);

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef REF2D_driver * (NAPIP MULDISP_initLibrary_t)(
    GA_devCtx **hwCtx,
    N_int32 numDevices,
    GA_globalOptions *opt,
    PM_imports *pmImp,
    GA_setActiveDevice_t setActiveDevice);

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef ibool (NAPIP MULDISP_InitFilterExt_t)(
    GA_modeInfo *modeInfo,
    N_int32 transferStart,
    REF2D_driver **ref2d,
    REF2D_driver **drv);

/****************************************************************************
REMARKS:
Library initialisation entry point for the region manager.
{secret}
****************************************************************************/
typedef ibool (NAPIP RGN_initLibrary_t)(GA_regionFuncs *funcs);

/****************************************************************************
REMARKS:
Library exit entry point for the region manager.
{secret}
****************************************************************************/
typedef ibool (NAPIP RGN_exitLibrary_t)(void);
#endif

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {                        /* Use "C" linkage when in C++ mode */
#endif

/* Function to load and unload direct copies of ref2d (ie: not connected
 * with a SNAP graphics driver). This is mostly useful for applications
 * wishing to use the software rendering functions directly for system
 * memory buffers etc.
 */

ibool   NAPI REF2D_loadDriver(GA_devCtx _FAR_ *hwCtx,N_int32 unused,N_int32 shared,REF2D_driver _FAR_ *_FAR_ *drv,MOD_MODULE _FAR_ *_FAR_ *hModRef,ulong _FAR_ *size);
void    NAPI REF2D_unloadDriver(REF2D_driver _FAR_ *drv,MOD_MODULE _FAR_ *hModule);

/* {secret} */
ibool   NAPI SHADOW_loadDriver(GA_devCtx _FAR_ *hwCtx,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size);
/* {secret} */
ibool   NAPI DPVLHOOK_loadDriver(GA_devCtx _FAR_ *hwCtx,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size);
/* {secret} */
ibool   NAPI PORTRAIT_loadDriver(GA_devCtx _FAR_ *hwCtx,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size);
/* {secret} */
ibool   NAPI FLIPPED_loadDriver(GA_devCtx _FAR_ *hwCtx,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size);
/* {secret} */
ibool   NAPI INVERT_loadDriver(GA_devCtx _FAR_ *hwCtx,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size);
/* {secret} */
ibool   NAPI MULDISP_loadDriver(GA_devCtx _FAR_ * _FAR_ *hwCtx,N_int32 numDevices,GA_globalOptions *opt,REF2D_driver _FAR_ * _FAR_ *drv,MOD_MODULE _FAR_ * _FAR_ *hModule,ulong _FAR_ *size,GA_setActiveDevice_t setActiveDevice);
#ifdef  __DRIVER__
/* {secret} */
REF2D_driver * _CEXPORT REF2D_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,struct __GA_exports *gaExp,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT SHADOW_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT DPVLHOOK_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT PORTRAIT_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT FLIPPED_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT INVERT_initLibrary(GA_devCtx *hwCtx,GA_globalOptions *opt,PM_imports *pmImp);
/* {secret} */
REF2D_driver * _CEXPORT MULDISP_initLibrary(GA_devCtx **hwCtx,N_int32 numDevices,GA_globalOptions *opt,PM_imports *pmImp,GA_setActiveDevice_t setActiveDevice);
/* {secret} */
ibool   _CEXPORT MULDISP_InitFilterExt(GA_modeInfo *modeInfo,N_int32 transferStart,REF2D_driver **ref2d,REF2D_driver **drv);
#else
/* {secret} */
ibool   NAPI MULDISP_InitFilterExt(GA_modeInfo *modeInfo,N_int32 transferStart,REF2D_driver **ref2d,REF2D_driver **drv);
#endif

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_REF2D_H */

