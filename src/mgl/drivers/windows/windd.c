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
* Description:  Main Windowed DirectDraw device driver for rendering
*               directly into a window on the desktop. We use DirectDraw
*               to get direct access to the primary surface memory so we
*               can do all MGL rendering functions, as well as supporting
*               offscreen surface buffers for hardware accelerted blits
*               in a window.
*
****************************************************************************/

#include "mgldd.h"
#include "drivers/windows/windd.h"

/*---------------------------- Driver Tables -----------------------------*/

drivertype WINDD_driver = {
    MGL_WINDOWEDNAME,
    "SciTech Windowed DirectX Driver",
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1991-2004 SciTech Software, Inc.\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    WINDD_createInstance,
    NULL,
    WINDD_initDriver,
    WINDD_destroyInstance,
    };

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
Pointer to intialised instance data.

REMARKS:
This function initialises any necessary instance data for the device.
****************************************************************************/
void * MGLAPI WINDD_createInstance(void)
{
    return SNAP_initInstance(PM_calloc(1,sizeof(SNAP_data)));
}

/****************************************************************************
PARAMETERS:
dc  - Device context

REMARKS:
Destroys all internal memory allocated for the device context.
****************************************************************************/
void MGLAPI WINDD_destroyDC(
    MGLDC *dc)
{
    if (dc->deviceType == MGL_WINDOWED_DEVICE) {
        /* If we have a clipper associated with the DC, then destroy it */
        if (dc->wm.windc.clipper)
            dc->r.DestroyClipper(dc->wm.windc.clipper);

        /* If we have a palette associated with this DC, then destroy it */
        if (dc->wm.windc.hpal)
            DeleteObject(dc->wm.windc.hpal);

        /* If we own the DC for this window release it */
        if (dc->wm.windc.hdcOwned)
            ReleaseDC(dc->wm.windc.hwnd,dc->wm.windc.hdcOwned);
        }
}

/****************************************************************************
PARAMETERS:
hdc - Windows HDC to get bitmap info from

REMARKS:
Determines the color depth of the current Windows display mode.
****************************************************************************/
static int WIN_getBitsPerPixel(
    HDC hdc)
{
    int                 bitsPerPixel;
    HBITMAP             hbm;
    struct {
        BITMAPINFOHEADER    hdr;
        DWORD               maskRed;
        DWORD               maskGreen;
        DWORD               maskBlue;
        DWORD               res[253];
        } bm;

    /* Find pixel depth and number of planes */
    if ((bitsPerPixel = GetDeviceCaps(hdc,BITSPIXEL)) > 8) {
        /* Clear the BITMAP info structure, and fill in with the appropriate
         * bits per pixel value with biCompression set to BI_BITFIELDS and
         * the pixel masks set to 0.
         */
        memset(&bm,0,sizeof(bm));
        bm.hdr.biSize = sizeof(BITMAPINFOHEADER);
        bm.hdr.biWidth = 1;
        bm.hdr.biHeight = 1;
        bm.hdr.biPlanes = 1;
        bm.hdr.biBitCount = bitsPerPixel;
        bm.hdr.biCompression = winBI_BITFIELDS;

        /* Do a GetDIBits with NULL lpBits to complete the header information,
         * including the bitmap mask information, using a compatible bitmap
         * created from the display DC.
         */
        hbm = CreateCompatibleBitmap(hdc,1,1);
        GetDIBits(hdc,hbm,0,1,NULL,(LPBITMAPINFO)&bm,DIB_RGB_COLORS);
        DeleteObject(hbm);
        if (bitsPerPixel == 16) {
            if (bm.maskRed != 0xF800)
                bitsPerPixel = 15;
            }
        }
    return bitsPerPixel;
}

/****************************************************************************
PARAMETERS:
dc	- Device context.

RETURNS:
Pointer to the default palette structure

PARAMETERS:
Get the current system palette entries and store in the default Window's
HalfTone palette structure. Then return this as the default palette.
****************************************************************************/
void * MGLAPI WINDD_getDefaultPalette(
    MGLDC *dc)
{
	int				i;
	PALETTEENTRY	pe[10];
	HDC				hdc = GetDC(NULL);

	GetSystemPaletteEntries(hdc,0,10,pe);
	for (i = 0; i < 10; i++) {
		_MGL_halftonePal[i].red = pe[i].peRed;
		_MGL_halftonePal[i].green = pe[i].peGreen;
		_MGL_halftonePal[i].blue = pe[i].peBlue;
		}
	GetSystemPaletteEntries(hdc,246,10,pe);
	for (i = 0; i < 10; i++) {
		_MGL_halftonePal[i+246].red = pe[i].peRed;
		_MGL_halftonePal[i+246].green = pe[i].peGreen;
		_MGL_halftonePal[i+246].blue = pe[i].peBlue;
		}
	ReleaseDC(NULL,hdc);
	return _MGL_halftonePal;
}

/****************************************************************************
PARAMETERS:
dc		- Device context
pal		- Palette of values to program
num		- Number of entries to program
index	- Index to start programming at

REMARKS:
Realizes the specified logical palette entries for the WinDC. We also make
the palette into an identity palette for speed.
****************************************************************************/
void MGLAPI WINDD_realizePalette(
    MGLDC *dc,
    palette_t *pal,
    int num,
    int index,
	int waitVRT)
{
	LPRGBQUAD	aRGB = (LPRGBQUAD)pal;
	LogPal		Palette = { 0x300, 256 };
	int 		noStatic,isBackground,i;
	HDC			deskHDC = GetDC(NULL);

    /* Determine if we are in the background and if we need static colors */
	isBackground = (GetActiveWindow() != dc->wm.windc.hwnd);
	noStatic = (GetSystemPaletteUse(deskHDC) == SYSPAL_NOSTATIC);

	/* We don't program the palette if the app is in the background, as this
	 * will cause the desktop to repaint (we cant use AnimatePalette). Hence
	 * we simply leave the hardware palette set to what is was when the app
	 * went into the background and Windows will re-map the bitmaps on the
	 * fly to the current hardware palette.
     */
    if (!isBackground) {
		if (!dc->wm.windc.hpal || (noStatic != dc->wm.windc.palNoStatic)
				|| (isBackground != dc->wm.windc.isBackground) || waitVRT == -1) {
			/* The palette has not been allocated or has changed STATIC modes */
			if (_MGL_isBackground || isBackground || noStatic) {
				/* For SYSPAL_NOSTATIC or if we are running in the background,
				 * simply copy the color table into a PALETTEENTRY. In the
				 * background we wont get an identity palette, but the colors
				 * will be correct.
				 */
				for (i = 0; i < 256; i++) {
					Palette.pe[i].peRed = aRGB[i].rgbRed;
					Palette.pe[i].peGreen = aRGB[i].rgbGreen;
					Palette.pe[i].peBlue = aRGB[i].rgbBlue;
					Palette.pe[i].peFlags = isBackground ? PC_NOCOLLAPSE : PC_RESERVED;
					}
				if (_MGL_isBackground == 2) {
					for (i = 0; i < 10; i++) {
						Palette.pe[i].peFlags = 0;
						Palette.pe[i+246].peFlags = 0;
						}
					}
				}
			else {
				/* For SYSPAL_STATIC when we are in the foreground, get the twenty
				 * static colors into the array, then fill in the empty spaces with
				 * the given color table. Note that we copy the current system
				 * colors into our palette cache so that we can correctly match up
				 * the DIB colors with the system palette for an identity palette
				 * during blt operations.
				 */
				GetSystemPaletteEntries(deskHDC, 0, 256, Palette.pe);
				for (i = 0; i < 10; i++) {
					Palette.pe[i].peFlags = 0;
					Palette.pe[i+246].peFlags = 0;
					}

				/* Fill in the entries from the given color table */
				for (; i < 246; i++) {
					Palette.pe[i].peRed = aRGB[i].rgbRed;
					Palette.pe[i].peGreen = aRGB[i].rgbGreen;
					Palette.pe[i].peBlue = aRGB[i].rgbBlue;
					Palette.pe[i].peFlags = isBackground ? PC_NOCOLLAPSE : PC_RESERVED;
					}
				}

			/* Mark 0 as black with peFlags 0 */
			Palette.pe[0].peRed = aRGB[0].rgbRed = 0;
			Palette.pe[0].peGreen = aRGB[0].rgbGreen = 0;
			Palette.pe[0].peBlue = aRGB[0].rgbBlue = 0;
			Palette.pe[0].peFlags = 0;

			/* Mark 255 as white with peFlags 0 */
			Palette.pe[255].peRed = aRGB[255].rgbRed = 0xFF;
			Palette.pe[255].peGreen = aRGB[255].rgbGreen = 0xFF;
			Palette.pe[255].peBlue = aRGB[255].rgbBlue = 0xFF;
			Palette.pe[255].peFlags = 0;

			/* Create the new palette, select it and realize it */
			if (dc->wm.windc.hpal)
				DeleteObject(dc->wm.windc.hpal);
			dc->wm.windc.hpal = CreatePalette((LOGPALETTE *)&Palette);
			SelectPalette(dc->wm.windc.hdc,dc->wm.windc.hpal, false);
			RealizePalette(dc->wm.windc.hdc);
			}
		else {
			/* We are just changing the palette, so use AnimatePalette */
			for (i = index; i < index+num; i++) {
				Palette.pe[i].peRed = aRGB[i].rgbRed;
				Palette.pe[i].peGreen = aRGB[i].rgbGreen;
				Palette.pe[i].peBlue = aRGB[i].rgbBlue;
				Palette.pe[i].peFlags = PC_RESERVED;
				}

			/* Mark 0 as black with peFlags 0 */
			Palette.pe[0].peRed = aRGB[0].rgbRed = 0;
			Palette.pe[0].peGreen = aRGB[0].rgbGreen = 0;
			Palette.pe[0].peBlue = aRGB[0].rgbBlue = 0;
			Palette.pe[0].peFlags = 0;

			/* Mark 255 as white with peFlags 0 */
			Palette.pe[255].peRed = aRGB[255].rgbRed = 0xFF;
			Palette.pe[255].peGreen = aRGB[255].rgbGreen = 0xFF;
			Palette.pe[255].peBlue = aRGB[255].rgbBlue = 0xFF;
			Palette.pe[255].peFlags = 0;

			SelectPalette(dc->wm.windc.hdc,dc->wm.windc.hpal, false);
			AnimatePalette(dc->wm.windc.hpal,index,num,Palette.pe);
			RealizePalette(dc->wm.windc.hdc);
			}
		}
	dc->wm.windc.palNoStatic = noStatic;
	dc->wm.windc.isBackground = isBackground;
    ReleaseDC(NULL,deskHDC);

    /* Now make sure we also realize the DirectDraw palette */
    SNAP_realizePalette(dc,pal,num,index,waitVRT != 0);
}

/****************************************************************************
RETURNS:
Number of bits in the value
****************************************************************************/
static int NumBits(
    uint value)
{
    int count = 0;
    while (value & 1) {
        count++;
        value >>= 1;
        }
    return count;
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
ibool MGLAPI WINDD_initDriver(
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
    GA_mdEntry          *md;
    SNAP_data           *inst = (SNAP_data*)data;
    SNAP_state          *state = &inst->state;
    display_vec         *d;
    gmode_t             *mi = &dc->mi;
    POINT               pt;
    RECT                r;
    ulong               stylebits;
    int                 deskX,deskY;
    HDC                 hdc;

    /* Load the SciTech SNAP Graphics driver if it has not already been loaded */
    if (!SNAP_load(inst))
        return false;

    /* Determine whether the HDC is private or shared */
    dc->wm.windc.hwnd = (HWND)hwnd;
    dc->wm.windc.hpal = NULL;
    dc->wm.windc.hdc = GetDC(dc->wm.windc.hwnd);
    stylebits = GetClassLong(dc->wm.windc.hwnd, GCL_STYLE);
    if (stylebits & (CS_PARENTDC | CS_CLASSDC))
        dc->wm.windc.hdcOwned = NULL;
    else
        dc->wm.windc.hdcOwned = dc->wm.windc.hdc;

    /* Now get the window and desktop resolution and color depth from Windows */
    pt.x = pt.y = 0;
    ClientToScreen(dc->wm.windc.hwnd, &pt);
    GetClientRect(dc->wm.windc.hwnd,&r);
    OffsetRect(&r, pt.x, pt.y);
    dc->size.left = r.left;
    dc->size.top = r.top;
    dc->size.right = r.right;
    dc->size.bottom = r.bottom;
    mi->xRes = r.right - r.left - 1;
    mi->yRes = r.bottom - r.top - 1;
    mi->bitsPerPixel = WIN_getBitsPerPixel(dc->wm.windc.hdc);
    switch (mi->bitsPerPixel) {
        case 8:     mi->maxColor = 0xFFUL;       break;
        case 15:    mi->maxColor = 0x7FFFUL;     break;
        case 16:    mi->maxColor = 0xFFFFUL;     break;
        case 24:    mi->maxColor = 0xFFFFFFUL;   break;
        case 32:    mi->maxColor = 0xFFFFFFFFUL; break;
        }
    mi->maxPage = 0;
    mi->aspectRatio = GetDeviceCaps(dc->wm.windc.hdc,ASPECTY) * 100;

    /* If the window does not own its own DC release it */
    if (!(dc->wm.windc.hdcOwned)) {
        ReleaseDC(dc->wm.windc.hwnd, dc->wm.windc.hdc);
        dc->wm.windc.hdc = NULL;
        }

    /* Now find the compatible display mode in the DirectDraw driver */
    hdc = GetDC(NULL);
    dc->bounds.left = dc->bounds.top = 0;
    dc->bounds.right = deskX = GetDeviceCaps(hdc,HORZRES);
    dc->bounds.bottom = deskY = GetDeviceCaps(hdc,VERTRES);
    ReleaseDC(NULL,hdc);
    for (md = inst->svga.modeList; md->mode != -1; md++) {
        if (md->XResolution == deskX && md->YResolution == deskY
            && md->BitsPerPixel == mi->bitsPerPixel)
            break;
        }
    if (md->mode == -1)
        return false;
    mi->scratch1 = md->mode | gaWindowedMode | gaLinearBuffer;
    mi->scratch2 = md->Attributes;

    /* Copy DirectColor mask information to mi block */
    if (md->BitsPerPixel <= 8) {
        mi->redMaskSize         = 8;
        mi->redFieldPosition    = 0;
        mi->greenMaskSize       = 0;
        mi->greenFieldPosition  = 0;
        mi->blueMaskSize        = 0;
        mi->blueFieldPosition   = 0;
        mi->alphaMaskSize       = 0;
        mi->alphaFieldPosition  = 0;
        }
    else {
        mi->redMaskSize = NumBits(md->PixelFormat.RedMask);
        mi->redFieldPosition = md->PixelFormat.RedPosition;
        mi->greenMaskSize = NumBits(md->PixelFormat.GreenMask);
        mi->greenFieldPosition = md->PixelFormat.GreenPosition;
        mi->blueMaskSize = NumBits(md->PixelFormat.BlueMask);
        mi->blueFieldPosition = md->PixelFormat.BluePosition;
        mi->alphaMaskSize = NumBits(md->PixelFormat.AlphaMask);
        mi->alphaFieldPosition = md->PixelFormat.AlphaPosition;
        }

    /* Fill in remaining device context information */
    dc->deviceType          = MGL_WINDOWED_DEVICE;
    dc->xInch               = 9000;
    dc->yInch               = 7000;
    _MGL_computePF(dc);
    dc->v = (drv_vec*)state;
    d = &dc->v->d;
    d->destroy              = WINDD_destroyDC;
    d->hwnd                 = NULL;

    /* Set the video mode and return status */
    if (!SNAP_setMode(dc,mi->scratch1,&state->oldMode,virtualX,virtualY,
            numBuffers,stereo,refreshRate,useLinearBlits))
        return false;

    /* Create the windowed mode clipper object for this window */
    dc->wm.windc.clipper = dc->r.CreateClipper(dc->wm.windc.hwnd);
    SNAP_beginVisibleClipRegion(dc);
    SNAP_endVisibleClipRegion(dc);

    /* Override some vectors for windowed modes */
	dc->r.getDefaultPalette 	    = WINDD_getDefaultPalette;
	dc->r.realizePalette 		    = WINDD_realizePalette;
    dc->r.BeginVisibleClipRegion    = SNAP_beginVisibleClipRegion;
    dc->r.EndVisibleClipRegion      = SNAP_endVisibleClipRegion;
    return true;
}

/****************************************************************************
REMARKS:
Frees the internal data structures allocated during the detection process
****************************************************************************/
void MGLAPI WINDD_destroyInstance(
    void *data)
{
    if (data)
        PM_free(data);
}

