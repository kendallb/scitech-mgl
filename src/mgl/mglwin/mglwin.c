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
* Description:  Win32 specific code bindings for the MGL.
*
****************************************************************************/

#include "mgl.h"
#include "mglwin/internal.h"

/*--------------------------- Global Variables ----------------------------*/

static ibool    installed = false;
uint            _MGL_cw_default = 0;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
Number of display devices attached to the system.

REMARKS:
This function detects the number of display devices attached to the system,
and uses whatever mechanism is available on the target OS to determine
this (usually a combination of SciTech SNAP Graphics and the OS provided services).
****************************************************************************/
int _MGL_enumerateDevices(void)
{
    int devices;

    GET_DEFAULT_CW();
    devices = GA_enumerateDevices(false);
    if (GA_status() == nInvalidLicense)
        __MGL_result = grInvalidLicense;
    return devices;
}

/****************************************************************************
REMARKS:
Clears out the system palette so that we can ensure our identity palettes
are set up correctly for fast bitmap blt'ing performance.
****************************************************************************/
static void ClearSystemPalette(void)
{
    LogPal      Palette = { 0x300, 256 };
    HPALETTE    ScreenPalette;
    HDC         ScreenDC;
    int         Counter;

    /* Reset everything in the system palette to black */
    for (Counter = 0; Counter < 256; Counter++) {
        Palette.pe[Counter].peRed = 0;
        Palette.pe[Counter].peGreen = 0;
        Palette.pe[Counter].peBlue = 0;
        Palette.pe[Counter].peFlags = PC_NOCOLLAPSE;
        }

    /* Create, select, realize, deselect, and delete the palette */
    ScreenDC = GetDC(NULL);
    ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);
    if (ScreenPalette) {
        ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
        RealizePalette(ScreenDC);
        ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
        DeleteObject(ScreenPalette);
        }
    ReleaseDC(NULL, ScreenDC);
}

/****************************************************************************
REMARKS:
Function to initialise the internals of the MGL for normal operation.
****************************************************************************/
void _MGL_initInternal(void)
{
    ClearSystemPalette();
    installed = true;
}

/****************************************************************************
PARAMETERS:
device  - New display device to make active

REMARKS:
This function sets the active display device in the system to be used for
device numeration and detection.
****************************************************************************/
int _MGL_setActiveDevice(
    int device)
{
    return GA_setActiveDevice(device);
}

/****************************************************************************
RETURNS:
Currently active device index

REMARKS:
This function returns the currently active device index
{secret}
****************************************************************************/
int _MGL_getActiveDevice(void)
{
    return GA_getActiveDevice();
}

/****************************************************************************
REMARKS:
Closes down the internals of the graphics library specific to this OS.
****************************************************************************/
void _MGL_exitInternal(void)
{
    installed = false;
}

/****************************************************************************
REMARKS:
Initialise the bitmap handle as necessary for the memory DC. If the user
passed in a valid Windows HBITMAP, then use this to set up our device
context information so we can blit the image to a Windowed DC.
****************************************************************************/
void _MGL_initBitmapHandle(
    MGLDC *dc,
    MGL_HBITMAP hbm)
{
    if (hbm) {
        HDC hdc = GetDC(NULL);
        dc->wm.memdc.hdc = CreateCompatibleDC(hdc);
        SelectObject(dc->wm.memdc.hdc,hbm);
        dc->wm.memdc.hbm = hbm;
        ReleaseDC(NULL,hdc);
        }
}

/****************************************************************************
PARAMETERS:
dc  - Device context.

RETURNS:
Total size of the device context surface

REMARKS:
Allocates the memory for a memory device context surface. When running under
Windows we allocate a DIB using CreateDIBSection. If we are running in full
screen mode, we allocate our memory device context's with a simple malloc
since we dont need to interface with GDI at all.
****************************************************************************/
long _MGL_allocateSurface(
    MGLDC *dc)
{
    HBITMAP         hbm;
    winBITMAPINFO   info;
    ulong           *masks;

    /* Fill in the bitmap info header for different color depths */
    memset(&info,0,sizeof(info));
    info.header.biSize = sizeof(winBITMAPINFOHEADER);
    info.header.biWidth = dc->mi.xRes+1;
    info.header.biHeight = -(dc->mi.yRes+1);
    info.header.biPlanes = 1;
    switch (dc->mi.bitsPerPixel) {
        case 1:
            info.header.biBitCount = 1;
            info.header.biCompression = winBI_RGB;
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            info.header.biClrUsed = 2;
            break;
        case 4:
            info.header.biBitCount = 4;
            info.header.biCompression = winBI_RGB;
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            info.header.biClrUsed = 16;
            break;
        case 8:
            info.header.biBitCount = 8;
            info.header.biCompression = winBI_RGB;
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            info.header.biClrUsed = 256;
            break;
        case 15:
        case 16:
            info.header.biBitCount = 16;
            if (dc->mi.bitsPerPixel == 15) {
                /* Bitmap is in the standard 5:5:5 RGB format */
                info.header.biCompression = winBI_RGB;
                }
            else {
                /* Bitmap is in the extended 5:6:5 RGB format */
                info.header.biCompression = winBI_BITFIELDS;
                masks = (ulong*)&info.colors;
                masks[0] = 0x0000F800;
                masks[1] = 0x000007E0;
                masks[2] = 0x0000001F;
                }
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            break;
        case 24:
            info.header.biBitCount = 24;
            if (dc->pf.redPos == 0) {
                /* Bitmap is in the extended 24 bit BGR format */
                info.header.biCompression = winBI_BITFIELDS;
                masks = (ulong*)&info.colors;
                masks[0]    = 0x0000FF;
                masks[1]    = 0x00FF00;
                masks[2]    = 0xFF0000;
                }
            else {
                /* Bitmap is in the standard 24 bit RGB format */
                info.header.biCompression = winBI_RGB;
                }
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            break;
        case 32:
            info.header.biBitCount = 32;
            if (dc->pf.alphaPos != 0) {
                if (dc->pf.redPos != 0) {
                    /* Bitmap is in the standard 32 bit ARGB format */
                    info.header.biCompression = winBI_RGB;
                    }
                else {
                    /* Bitmap is in the extended 32 bit ABGR format */
                    info.header.biCompression = winBI_BITFIELDS;
                    masks = (ulong*)&info.colors;
                    masks[0]    = 0x000000FFUL;
                    masks[1]    = 0x0000FF00UL;
                    masks[2]    = 0x00FF0000UL;
                    }
                }
            else {
                if (dc->pf.redPos != 8) {
                    /* Bitmap is in the extended 32 bit RGBA format */
                    info.header.biCompression = winBI_BITFIELDS;
                    masks = (ulong*)&info.colors;
                    masks[0]    = 0xFF000000UL;
                    masks[1]    = 0x00FF0000UL;
                    masks[2]    = 0x0000FF00UL;
                    }
                else {
                    /* Bitmap is in the extended 32 bit BGRA format */
                    info.header.biCompression = winBI_BITFIELDS;
                    masks = (ulong*)&info.colors;
                    masks[0]    = 0x0000FF00UL;
                    masks[1]    = 0x00FF0000UL;
                    masks[2]    = 0xFF000000UL;
                    }
                }
            info.header.biSizeImage = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
            break;
        default:
            return 0;
        }

    /* Now allocate the bitmap surface */
    if (_MGL_fullScreen) {
        if ((dc->surface = PM_malloc(info.header.biSizeImage)) == NULL) {
            FATALERROR(grNoMem);
            return 0;
            }
        }
    else {
        HDC hdc = GetDC(NULL);
        dc->wm.memdc.hdc = CreateCompatibleDC(hdc);
        if ((hbm = CreateDIBSection(dc->wm.memdc.hdc,(LPBITMAPINFO)&info,DIB_RGB_COLORS,&dc->surface,0,0)) == NULL) {
            FATALERROR(grNoMem);
            return 0;
            }
        else {
            SelectObject(dc->wm.memdc.hdc,hbm);
            dc->wm.memdc.hbm = hbm;
            ReleaseDC(NULL,hdc);
            }
        }

    /* Clear the DIB surface, as this is not always guaranteed to be
     * cleared by Windows.
     */
    MGL_memset(dc->surface,0,info.header.biSizeImage);
    return info.header.biSizeImage;
}

/****************************************************************************
PARAMETERS:
dc  - Memory device context with surface to destroy

REMARKS:
Destroys the packed pixel device surface for a memory device context.

NOTE: This function never gets called if the bitmap was allocated via
      MGL_createCustomDC rather than MGL_createDC, so it is up to the
      user app to free the allocated memory block.
****************************************************************************/
void _PM_freeSurface(
    MGLDC *dc)
{
    if (dc->wm.memdc.hbm) {
        SelectObject(dc->wm.memdc.hdc,NULL);
        DeleteDC(dc->wm.memdc.hdc);
        DeleteObject(dc->wm.memdc.hbm);
        }
    else
        PM_free(dc->surface);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
pal     - Palette to realize
num     - Number of colors to realise
index   - First color to realize

REMARKS:
Realizes the palette for an 8 bit memory device context. Because the actual
bits in the bitmap are stored in the DIB color table, which is a separate
color table to the normal MGL color table, we need to re-build the DIB color
table correctly to make the DIB color table into an identity palette mapping.

If the system is changed to or from SYSPAL_NOSTATIC mode, all the memory
DC bitmaps will have to have their palettes realized again to ensure they
are mapped to identity palettes.
****************************************************************************/
void MGLAPI PACKED8_realizePalette(
    MGLDC *dc,
    palette_t *pal,
    int num,
    int index,
    int waitVRT)
{
    HDC             hdc;
    RGBQUAD         colors[256];
    PALETTEENTRY    pe[20];
    int             i;

    /* Copy all palette colors to begin with */
    if (!_MGL_fullScreen) {
        memcpy(colors,pal,256 * sizeof(RGBQUAD));
        hdc = GetDC(NULL);
        if (GetDeviceCaps(hdc,BITSPIXEL) <= 8) {
            if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC || _MGL_isBackground) {
                /* In SYSPAL_NOSTATIC mode we can assign all colors except for
                 * black and white which must be 0 and 255 respectively.
                 */
                colors[0].rgbRed = 0;
                colors[0].rgbGreen = 0;
                colors[0].rgbBlue = 0;
                colors[255].rgbRed = 0xFF;
                colors[255].rgbGreen = 0xFF;
                colors[255].rgbBlue = 0xFF;
                }
            else {
                /* Make sure we get the system palette entries to ensure an
                 * identity palette mapping.
                 */
                GetSystemPaletteEntries(hdc, 0, 10, &pe[0]);
                GetSystemPaletteEntries(hdc, 246, 10, &pe[10]);
                for (i = 0; i < 10; i++) {
                    colors[i].rgbRed = pe[i].peRed;
                    colors[i].rgbGreen = pe[i].peGreen;
                    colors[i].rgbBlue = pe[i].peBlue;
                    colors[i+246].rgbRed = pe[i+10].peRed;
                    colors[i+246].rgbGreen = pe[i+10].peGreen;
                    colors[i+246].rgbBlue = pe[i+10].peBlue;
                    }
                }
            }
        ReleaseDC(NULL, hdc);

        /* Now set the DIB color table entries */
        SetDIBColorTable(dc->wm.memdc.hdc,index,num,colors);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context

REMARKS:
Locks the static palette entries so that they cannot be changed.
****************************************************************************/
void _MGL_lockStaticPalette(
    MGLDC *dc)
{
    if (dc->mi.bitsPerPixel == 8) {
        HDC hdc = GetDC(NULL);
        SetSystemPaletteUse(hdc, SYSPAL_STATIC);
        ReleaseDC(NULL,hdc);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context

REMARKS:
Unlocks the static palette entries so that they can be changed for fullscreen
display modes.
****************************************************************************/
void _MGL_unlockStaticPalette(
    MGLDC *dc)
{
    if (dc->mi.bitsPerPixel == 8) {
        HDC hdc = GetDC(NULL);
        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
        ReleaseDC(NULL,hdc);
        }
}

