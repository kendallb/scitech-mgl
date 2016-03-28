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
* Description:  32 bit packed pixel device context routines. This code will
*               be linked in if this driver is registered for use.
*
****************************************************************************/

#include "mgl.h"
#include "drivers/packed/packed32.h"

/*---------------------------- Driver Tables -----------------------------*/

drivertype PACKED32_driver = {
    MGL_PACKED32NAME,
    "SciTech SNAP Graphics 32bpp Packed Pixel Driver",
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1991-2004 SciTech Software, Inc.\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    PACKED32_createInstance,
    NULL,
    PACKED32_initDriver,
    PACKED32_destroyInstance,
    };

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
Pointer to intialised instance data.

REMARKS:
This function initialises any necessary instance data for the device.
****************************************************************************/
void * MGLAPI PACKED32_createInstance(void)
{
    return PM_calloc(1,sizeof(PACKED32_state));
}

/****************************************************************************
PARAMETERS:
dc  - Device context

REMARKS:
Destroys all internal memory allocated for the device context.
****************************************************************************/
static void MGLAPI PACKED32_destroyDC(
    MGLDC *dc)
{
    if (--dc->v->m.refCount == 0) {
        /* Unload the reference rasteriser when the last DC is destroyed */
        REF2D_unloadDriver(dc->v->m.ref2d,dc->v->m.hModRef2d);
        dc->v->m.ref2d = NULL;
        }
    if (dc->ownMemory)
        _PM_freeSurface(dc);
}

/****************************************************************************
REMARKS:
Make the new device context the currently active device context for
rendering, which includes updating any necessary hardware state that
is cached in the SciTech SNAP Graphics drivers.
****************************************************************************/
void MGLAPI PACKED32_makeCurrent(
    MGLDC *dc,
    MGLDC *oldDC,
    ibool partial)
{
    (void)oldDC;
    dc->r.SetDrawSurface(dc->surface,dc->mi.xRes+1,dc->mi.yRes+1,dc->mi.bytesPerLine,dc->mi.bitsPerPixel,GAPF(&dc->pf));
    DRV_makeCurrent(dc,partial);
}

/****************************************************************************
PARAMETERS:
data            - Instance data for the driver
dc              - Device context to initialise
mode            - Display mode to set
hwnd            - Handle to OS window for the mode
virtualX        - Virtual X screen dimensions (-1 if not scrolling)
virtualY        - Virtual Y screen dimensions (-1 if not scrolling)
numBuffers      - Number of buffers to allocate
stereo          - True if stereo should be enabled
refreshRate     - Desired refresh rate for mode

RETURNS:
True on sucess, false on failure.

REMARKS:
This function initialise the device driver for use with the passed in
device context. We also switch the system into graphics mode when this
function is called.
****************************************************************************/
ibool MGLAPI PACKED32_initDriver(
    void *data,
    MGLDC *dc,
    modeent *mode,
    ulong hwnd,
    int virtualX,
    int virtualY,
    int numBuffers,
    ibool stereo,
    int refreshRate,
    ibool useLinearBlits)
{
    PACKED32_state      *state = (PACKED32_state*)data;
    gmode_t             *mi = &dc->mi;
    REF2D_driver        *ref2d;
#ifndef TEST_HARNESS
    GA_buffer           drawBuf;
#endif

    /* Check for compatible pixel formats */
    if (!CMP_PF(dc->pf,_MGL_pixelFormats[pfARGB32])
            && !CMP_PF(dc->pf,_MGL_pixelFormats[pfABGR32])
            && !CMP_PF(dc->pf,_MGL_pixelFormats[pfRGBA32])
            && !CMP_PF(dc->pf,_MGL_pixelFormats[pfBGRA32])) {
        FATALERROR(grInvalidPF);
        return false;
        }

    /* Load the reference rasteriser if this is the first instance. We
     * share the reference rasteriser with all packed pixel memory context
     * of the same color depth, so we only need to load it once.
     */
    dc->v = (drv_vec*)state;
    dc->v->m.destroy = PACKED32_destroyDC;
    if (dc->v->m.refCount++ == 0) {
        if (!REF2D_loadDriver(NULL,mi->bitsPerPixel,false,&ref2d,
                &dc->v->m.hModRef2d,&dc->v->m.ref2dSize)) {
            FATALERROR(grFailLoadRef2d);
            return false;
            }
        dc->v->m.ref2d = ref2d;
        }
    else {
        ref2d = dc->v->m.ref2d;
        }

    /* Load the device information  */
    dc->deviceType              = MGL_MEMORY_DEVICE;
    dc->flags                   |= MGL_LINEAR_ACCESS;
    mi->maxColor                = 0xFFFFFFFFUL;
    mi->maxPage                 = 0;
    mi->aspectRatio             = 1000;
    dc->r.getDefaultPalette     = DRV_getDefaultPalette;
    dc->r.makeCurrent           = PACKED32_makeCurrent;

    /* Setup all rendering vectors for SciTech SNAP Graphics functions */
    DRV_setSNAPRenderingVectors(dc,ref2d);

    /* Allocate the device context surface and return */
    if (!dc->surface) {
        /* Allocate the device context surface if we have not done this */
        dc->mi.bytesPerLine = (dc->mi.xRes+1) * 4;
        dc->ownMemory = true;
        if (_MGL_allocateSurface(dc) == 0)
            return false;

#ifndef TEST_HARNESS
        /* Set the draw buffer to the entire memory DC */
        drawBuf.dwSize = sizeof(drawBuf);
        drawBuf.Offset = 0;
        drawBuf.Stride = dc->mi.bytesPerLine;
        drawBuf.Width = dc->mi.xRes+1;
        drawBuf.Height = dc->mi.yRes+1;
        ref2d->SetDrawBuffer(&drawBuf,dc->surface,dc->mi.bitsPerPixel,GAPF(&dc->pf),NULL,true);
#endif
        }
    (void)mode;
    (void)virtualX;
    (void)virtualY;
    (void)numBuffers;
    (void)stereo;
    (void)refreshRate;
    (void)useLinearBlits;
    (void)hwnd;
    return true;
}

/****************************************************************************
REMARKS:
Frees the internal data structures allocated during the detection process
****************************************************************************/
void MGLAPI PACKED32_destroyInstance(
    void *data)
{
    if (data)
        PM_free(data);
}

