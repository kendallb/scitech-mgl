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
* Description:  Main PHWIN device driver for rendering directly into a
*       Photon device context. The windowed DC device driver does
*       not directly produce output using MGL rendering commands,
*       except for the BitBlt operations between a MEMORYDC and
*       a WINDC. Using the MGL under Windows requires creating a
*       WINDC for communicating with Photon, and creating a
*       MEMORYDC for handling all MGL output. When the MEMORYDC
*       is constructed, it can then be Blt'ed to the display.
*
*       Hence the only thing that the PHWIN handles is BitBlt's and
*       palette management (to ensure our palettes are updated
*       correctly, so that we dont get BitBlt slowdown's due to
*       palette translation).
*
****************************************************************************/

#include "mgldd.h"
#include "drivers/qnx/phwin.h"
#include "drivers/common/common.h"
#include <Pt.h>

/*---------------------------- Driver Tables -----------------------------*/

drivertype  _VARAPI PHWIN_driver = {
    IDENT_FLAG,
    "PHWIN.DRV",
    "PHWIN.DRV",
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1996 SciTech Software\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    grNONE,
    PHWIN_createInstance,
    NULL,
    PHWIN_initDriver,
    PHWIN_destroyInstance,
    };

PRIVATE drv_vec *g_state = NULL;    /* Global state buffer pointer  */

/*------------------------- Implementation --------------------------------*/

PRIVATE void MGLAPI destroyDC(MGLDC *dc)
/****************************************************************************
*
* Function:     destroyDC
* Parameters:   dc  - Device context
*
* Description:  Destroys all internal memory allocated by the device
*               context.
*
****************************************************************************/
{
    if (dc->ownMemory)
        PM_free(dc->surface);
}

ibool MGLAPI PHWIN_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,
    int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate)
/****************************************************************************
*
* Function:     PHWIN_initDriver
* Parameters:   dc  - Device context.
* Returns:      True if the device was correctly initialised.
*
* Description:  Initialises the device driver, and starts the specified
*               graphics mode. This is also where we fill in all of the
*               vectors in the device context to initialise our device
*               context properly.
*
****************************************************************************/
{
    PhDim_t     dim;
    PtWidget_t  *window = (PtWidget_t *)hwnd;

    /* Allocate a single buffer for the driver specific state buffer */
    if (g_state == NULL) {
        if ((g_state = PM_calloc(1,sizeof(drv_vec))) == NULL) {
            _MGL_result = grNoMem;
            return false;
            }
        }
    dc->v = g_state;
    dc->v->m.refCount++;
        dc->wm.window = window;

    /* Initialise the driver */
    PtWidgetDim(window, &dim);
    dc->mi.xRes = dim.w-1;
    dc->mi.yRes = dim.h-1;
    dc->mi.maxPage = 0;
    dc->mi.pageSize = 0;
    dc->mi.bytesPerLine = 0;
    dc->surface = NULL;
    dc->mi.bitsPerPixel = PH_getBitsPerPixel(dc);
    PH_findPixelFormat(dc);
    switch (dc->mi.bitsPerPixel) {
        case 4:
            dc->mi.maxColor = 0xFUL;
            break;
        case 8:
            dc->mi.maxColor = 0xFFUL;
            break;
        case 15:
            dc->mi.maxColor = 0x7FFFUL;
            break;
        case 16:
            dc->mi.maxColor = 0xFFFFUL;
            break;
        case 24:
            dc->mi.maxColor = 0xFFFFFFUL;
            break;
        case 32:
            dc->mi.maxColor = 0xFFFFFFFFUL;
            break;
        }
    dc->mi.aspectRatio = 1000;  /* 1:1 aspect */

    /* Load the device context with the device information for the
     * selected video mode
     */
    dc->deviceType = MGL_WINDOWED_DEVICE;
    dc->xInch = 0;
    dc->yInch = 0;
    dc->ellipseFixup = false;
    dc->v->w.destroy        = destroyDC;

    /* Fill in required device context vectors */
    dc->r.getWinDC              = PH_getWinDC;
    dc->r.getDefaultPalette         = PH_getDefaultPalette;
    dc->r.realizePalette            = PH_realizePalette;
    dc->r.setClipRect           = DRV_setClipRect;

    /* Fill in remaining device vectors */
    dc->r.putImage              = PH_putImage;
    dc->r.stretchBlt1x2         = PH_stretchBlt1x2;
    dc->r.stretchBlt2x2         = PH_stretchBlt2x2;
    dc->r.stretchBlt            = PH_stretchBlt;

#ifndef MGL_LITE
    dc->r.translateImage            = PH_translateImage;
#endif

    /* Initialize event handling functions */
    _EVT_init();
    return true;
}
void * MGLAPI
PHWIN_createInstance(void)
{
    return (NULL);
}

void MGLAPI
PHWIN_destroyInstance(void *data)
{
}

ulong MGLAPI PH_getWinDC(MGLDC *dc)
{
    return (NULL);
}

ibool MGLAPI PH_noZbuf(MGLDC *dc)
{
    dc = dc;
    return false;
}

int PH_getBitsPerPixel(MGLDC *dc)
/****************************************************************************
*
* Function: PH_getBitsPerPixel
* Parameters:   dc  - MGL device context
*
* Description:  Determines the color depth of the current Windows display
*       mode.
*
****************************************************************************/
{
    PhSysInfo_t sysinfo;

    if (PtQuerySystemInfo(dc->wm.window, &sysinfo) != NULL &&
        sysinfo.gen.valid_fields & Ph_GEN_INFO_NUM_GFX &&
        sysinfo.gfx.valid_fields & Ph_GFX_COLOR_BITS)
        return (sysinfo.gfx.color_bits);
    else
        return -1;
}

PUBLIC void PH_findPixelFormat(MGLDC *dc)
/****************************************************************************
*
* Function:     PH_findPixelFormat
* Parameters:   dc  - Device context.
*               hdc - Windows HDC to get bitmap info from
*
* Description:  Determines the color depth and pixel format of the current
*               Windows display mode, so that we can create DIB sections
*               in exactly the same format for maximum blt'ing speed.
*
****************************************************************************/
{
    if (dc->mi.bitsPerPixel < 8)
        MGL_fatalError("Less than 8 bits per pixel not supported");

    /*
     * Until there is no better way to determine the bit format of
     * the display, we can only assume as to what is the optimum bit
     * format to use
     */
    switch (dc->mi.bitsPerPixel) {
        case 15:
            dc->pf = _MGL_pixelFormats[pfRGB555];
            dc->wm.bitfmt = Pg_IMAGE_DIRECT_555;
            break;
        case 16:
            dc->pf = _MGL_pixelFormats[pfRGB565];
            dc->wm.bitfmt = Pg_IMAGE_DIRECT_565;
            break;
        case 24:
            dc->pf = _MGL_pixelFormats[pfRGB24];
            dc->wm.bitfmt = Pg_IMAGE_DIRECT_888;
            break;
        case 32:
            dc->pf = _MGL_pixelFormats[pfARGB32];
            dc->wm.bitfmt = Pg_IMAGE_DIRECT_8888;
            break;
        case 8:
        default:
            dc->wm.bitfmt = Pg_IMAGE_PALETTE_BYTE;
        }
}

void * MGLAPI PH_getDefaultPalette(MGLDC *dc)
/****************************************************************************
*
* Function:     PH_getDefaultPalette
* Parameters:   dc  - Device context.
* Returns:      Pointer to the default palette structure
*
* Description:  Get the current system palette entries and store in a
*       static array.  Then return this as the default palette.
*
****************************************************************************/
{
    static palette_t    palette[_Pg_MAX_PALETTE];

    if (PgGetPalette((PgColor_t *)palette) != 0)
        return (NULL);
    else
        return (palette);
}

void MGLAPI PH_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,
    int waitVRT)
/****************************************************************************
*
* Function:     PH_realizePalette
* Parameters:   dc      - Device context
*               pal     - Palette of values to program
*               num     - Number of entries to program
*               index   - Index to start programming at
*
* Description:  Realizes the specified logical palette entries for the
*       WinDC. We also make the palette into an identity palette
*       for speed.
*
****************************************************************************/
{
    /*
     * The exact format of a PgColor_t is not clearly defined; it must
     * be in the same format as an MGL palette_t, since this works!
     */
    PgSetPalette((PgColor_t *)pal, 0, index, num, Pg_PALSET_SOFT, 0);
}

void MGLAPI PH_putImage(MGLDC *dc,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,int op,void *surface,
    int bytesPerLine,MGLDC *src)
/****************************************************************************
*
* Function:     PH_putImage
* Parameters:   dc      - Device context
*
* Description:  Blt's a system memory buffer DC of the same pixel depth
*       as the display device context to the display DC.
*
****************************************************************************/
{
    PhPoint_t   pos, size;

    pos.x = dstLeft;
    pos.y = dstTop;
    size.x = right - left;
    size.y = bottom - top;

    PgDrawImagemx(_MGL_pixelAddr2(surface, left, top,
        bytesPerLine, src->mi.bitsPerPixel),
        dc->wm.bitfmt, &pos, &size, bytesPerLine, 0);
}

void MGLAPI PH_translateImage(MGLDC *dc,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,int op,void *surface,int bytesPerLine,
    int bitsPerPixel,palette_t *pal,pixel_format_t *pf,MGLDC *src)
/****************************************************************************
*
* Function:     PH_translateImage
* Parameters:   dc              - Device context to Blt to
*
* Description:  Blt an image from a memory buffer onto the specified
*       device context, translating the pixel format on the fly.
*
****************************************************************************/
{
printf("PH_translateImage: %d,%d to %d,%d\n", left, top, right, bottom);
    PH_putImage(dc,left,top,right,bottom,dstLeft,dstTop,op,surface,
        bytesPerLine,src);
    bitsPerPixel = bitsPerPixel;
    pal = pal;
    pf = pf;
}

void MGLAPI PH_stretchBlt1x2(MGLDC *dst,MGLDC *src,int left,int top,
    int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     PH_stretchBlt1x2
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of possibly differing
*               pixel depths to the display device context with stretching.
*
****************************************************************************/
{
    PH_stretchBlt(dst,src,left,top,right,bottom,dstLeft,dstTop,
        MGL_FIX_1,MGL_FIX_2,pal,idPal);
}

void MGLAPI PH_stretchBlt2x2(MGLDC *dst,MGLDC *src,int left,int top,
    int right,int bottom,int dstLeft,int dstTop,palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     PH_stretchBlt2x2
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of possibly differing
*               pixel depths to the display device context with stretching.
*
****************************************************************************/
{
    PH_stretchBlt(dst,src,left,top,right,bottom,dstLeft,dstTop,
        MGL_FIX_2,MGL_FIX_2,pal,idPal);
}

void MGLAPI PH_stretchBlt(MGLDC *dst,MGLDC *src,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,fix32_t zoomx,fix32_t zoomy,
    palette_t *pal,ibool idPal)
/****************************************************************************
*
* Function:     PH_stretchBlt
* Parameters:   dst - Destination context
*               src - Source context
*
* Description:  Blt's a system memory buffer DC of the same pixel depth
*       as the display device context to the display DC with stretching.
*
****************************************************************************/
{
printf("StretchBlt (unimplemented): %d,%d to %d,%d\n", left, top, right, bottom);
}
