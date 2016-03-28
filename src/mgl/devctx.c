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
* Description:  Device context manipulation routines.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

ibool   _MGL_checkIdentityPal = true;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Make a device context the current device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - New device context to make the current context

RETURNS:
Previous current device context (possibly NULL).

REMARKS:
This function makes the specified device context the current device context. The
current device context is the one that is used for all the output rasterizing routines,
and when a device context is made current, a copy of the device context is cached
internally for maximum speed in the rasterizing code. While a device context is
current, changes made to the global state of the current device context are changed
in the cached copy only and are not updated in the original device context. When
the device context is changed however, the values in the cached device context are
flushed back to the original device context to keep it up to date. You can flush the
current device context explicitly by passing a NULL for the new current device
context.

Because of this caching mechanism, changing the current device context is an
expensive operation, so you should try to minimize the need to change the current
device context. Normally you will maintain a single device context that will be used
for all rasterizing operations, and leave this as your current device context. If the
device context specified is already the current device context, this function simply
does nothing.

Note:   If the SciTech MGL has been initialised to support multiple display
        controllers, calling this function will ensure that the display
        device being accessed by the passed in device context is made the
        currently active display. If the display controllers can support
        'mixed' mode, the secondary displays remain active at the same time
        as the primrary display and there is no overhead for the switch.
        Some older display controllers cannot support mixed mode as the
        VGA compatible resources cannot be disabled, and in this case every
        call to this function will cause the active display device to be
        switched.

SEE ALSO:
MGL_isCurrentDC, MGL_initMultiMonitor
****************************************************************************/
MGLDC * MGLAPI MGL_makeCurrentDC(
    MGLDC *dc)
{
    MGLDC   *oldDC = _MGL_dcPtr;

    if (dc == _MGL_dcPtr || dc == &DC)
        return oldDC;
    if (_MGL_dcPtr) {
        *_MGL_dcPtr = _MGL_dc;          /* 'Write back' the old DC      */
        _MGL_dcPtr = NULL;              /* This DC is no longer cached  */
        }
    if (dc) {
        _MGL_dcPtr = dc;                /* Cache DC in global structure */
        _MGL_dc = *dc;                  /* Save pointer to original DC  */
        dc->r.makeCurrent(dc,oldDC,false);
        }
    return oldDC;
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is the currently active context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if DC is the currently active context, false otherwise.

REMARKS:
This function determines if the passed in device context is the currently
active device context. The currently active device context is where all
the output from all the MGL rendering functions will be displayed.

SEE ALSO:
MGL_makeCurrentDC
****************************************************************************/
ibool MGLAPI MGL_isCurrentDC(
    MGLDC *dc)
{
    return (dc == _MGL_dcPtr);
}

/****************************************************************************
PARAMETERS:
dc  - DC that has been updated

REMARKS:
This is an internal routine that gets called when major changes are made
to a DC through a pointer, not the current DC. If the DC being modified is
the current one, we flush all changes to the newly updated DC to the
current DC.
{secret}
****************************************************************************/
void _MGL_updateCurrentDC(
    MGLDC *dc)
{
    if (dc == _MGL_dcPtr)
        _MGL_dc = *_MGL_dcPtr;      /* Update the cached copy           */
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is a display DC, false if not.

REMARKS:
This function determines if the passed in device context is a fullscreen
display device context, or some other type of device context.

SEE ALSO:
MGL_isOffscreenDC, MGL_isOverlayDC, MGL_isMemoryDC, MGL_isWindowedDC
****************************************************************************/
ibool MGLAPI MGL_isDisplayDC(
    MGLDC *dc)
{
    return _MGL_isFullscreenDevice(dc);
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is an offscreen device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is an offscreen DC, false if not.

REMARKS:
This function determines if the passed in device context is an offscreen
device context, or some other type of device context.

SEE ALSO:
MGL_isDisplayDC, MGL_isOverlayDC, MGL_isMemoryDC, MGL_isWindowedDC
****************************************************************************/
ibool MGLAPI MGL_isOffscreenDC(
    MGLDC *dc)
{
    return _MGL_isOffscreenDevice(dc);
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is an overlay device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is an overlay DC, false if not.

REMARKS:
This function determines if the passed in device context is an overlay
device context, or some other type of device context.

SEE ALSO:
MGL_isDisplayDC, MGL_isOffscreenDC, MGL_isMemoryDC, MGL_isWindowedDC
****************************************************************************/
ibool MGLAPI MGL_isOverlayDC(
    MGLDC *dc)
{
    return _MGL_isOverlayDevice(dc);
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is a windowed DC, false if not.

REMARKS:
This function determines if the passed in device context is a windowed
device context, or some other type of device context.

SEE ALSO:
MGL_isDisplayDC, MGL_isOffscreenDC, MGL_isOverlayDC, MGL_isMemoryDC
****************************************************************************/
ibool MGLAPI MGL_isWindowedDC(
    MGLDC *dc)
{
    return dc->deviceType == MGL_WINDOWED_DEVICE;
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is a memory device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is a memory DC, false if not.

REMARKS:
This function determines if the passed in device context is a memory
device context, or some other type of device context.

SEE ALSO:
MGL_isDisplayDC, MGL_isOffscreenDC, MGL_isOverlayDC, MGL_isWindowedDC
****************************************************************************/
ibool MGLAPI MGL_isMemoryDC(
    MGLDC *dc)
{
    return dc->deviceType == MGL_MEMORY_DEVICE;
}

/****************************************************************************
DESCRIPTION:
Determines if the specified device context is stereo enabled.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
True if the device context is stereo enabled, false if not.

REMARKS:
This function determines if the passed in device context supports stereo
rendering.

SEE ALSO:
MGL_isDisplayDC, MGL_isOffscreenDC, MGL_isOverlayDC, MGL_isWindowedDC
****************************************************************************/
ibool MGLAPI MGL_isStereoDC(
    MGLDC *dc)
{
    return dc->flags & MGL_STEREO_ACCESS;
}

/*******************************************************************************
DESCRIPTION:
Sets the current palette snow level for a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context to set snow level for
level   - New snow level to set

REMARKS:
This function sets the number of palette entries that can be programmed during a
single vertical retrace before the onset of snow. By default MGL programs all 256
entries per retrace, but you may need to slow this down on systems with slower
hardware that causes snow during multiple palette realization commands.

SEE ALSO:
MGL_getPaletteSnowLevel, MGL_setPalette
*******************************************************************************/
void MGLAPI MGL_setPaletteSnowLevel(
    MGLDC *dc,
    int level)
{
    if (_MGL_isFullscreenDevice(dc)) {
        if (level > 0 && level <= 256)
            dc->v->d.maxProgram = level;
        }
}

/*******************************************************************************
DESCRIPTION:
Returns the current palette snow level.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Current palette snow level.

REMARKS:
This function returns the number of palette entries that can be programmed during a
single vertical retrace before the onset of snow. By default MGL programs use all 256
entries per retrace, but you may need to slow this down on systems with slower
hardware that causes snow during multiple palette realization commands.

SEE ALSO:
MGL_setPaletteSnowLevel
*******************************************************************************/
int MGLAPI MGL_getPaletteSnowLevel(
    MGLDC *dc)
{
    return _MGL_isFullscreenDevice(dc) ? dc->v->d.maxProgram : 0;
}

/****************************************************************************
DESCRIPTION:
Return the direct surface access flags.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Direct surface access flags for the device context.

REMARKS:
This function returns the direct surface access flags for the specified device context,
which can be used to determine if the surface for the device context is directly
accessible, and if the surface has been virtualized in software. The access flags
returned are enumerated in MGL_surfaceAccessFlagsType.

If the surface access flags is MGL_VIRTUAL_ACCESS, this means that the
surface for the device can be directly accessed, however the surface is actually
virtualized in software using a page fault handler for SuperVGA devices that do not
have a real hardware linear framebuffer. If the surface is virtualized, you must
ensure that when you directly access the surface you do so on BYTE, WORD and
DWORD aligned boundaries. If you access it on a non-aligned boundary across a
page fault, you will cause an infinite page fault loop to occur. If the surface access
flags is MGL_NO_ACCESS the framebuffer will be banked and if you wish to
rasterize directly to it you will need to use the SVGA_setBank functions to change
banks. In banked modes the surface pointer points to the start of the banked
framebuffer window (ie: 0xA0000).
****************************************************************************/
int MGLAPI MGL_surfaceAccessType(
    MGLDC *dc)
{
    return (dc->flags & MGL_SURFACE_FLAGS);
}

/****************************************************************************
DESCRIPTION:
Returns the current hardware acceleration flags for the display device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Current hardware acceleration flags for the device context.

REMARKS:
This function returns the current hardware acceleration flags for the display device
context (this function returns 0 for non-display device contexts). The set of
hardware acceleration flags inform the application of what specific hardware
acceleration features the underlying video hardware has, so that the application can
tailor it's use of specific MGL functions. For instance an application may check if
the hardware has transparent BitBlt capabilities for sprite animation, and if not will
uses it's own application specific set of routines that rasterize directly to the display
surface rather than using the MGL specific functions.


The set of hardware acceleration flags that are returned will be a logical
combination of one or more of the values enumerated in MGL_hardwareFlagsType.
****************************************************************************/
long MGLAPI MGL_getHardwareFlags(
    MGLDC *dc)
{
    return (dc->flags & MGL_HW_FLAGS);
}

/****************************************************************************
PARAMETERS:
dc  - Device context

REMARKS:
Computes the pixel format information from the mode info block for the
graphics mode.
{secret}
****************************************************************************/
void _MGL_computePF(
    MGLDC *dc)
{
    dc->pf.redAdjust = 8 - dc->mi.redMaskSize;
    dc->pf.redMask = 0xFF >> dc->pf.redAdjust;
    dc->pf.redPos = dc->mi.redFieldPosition;
    dc->pf.greenAdjust = 8 - dc->mi.greenMaskSize;
    dc->pf.greenMask = 0xFF >> dc->pf.greenAdjust;
    dc->pf.greenPos = dc->mi.greenFieldPosition;
    dc->pf.blueAdjust = 8 - dc->mi.blueMaskSize;
    dc->pf.blueMask = 0xFF >> dc->pf.blueAdjust;
    dc->pf.bluePos = dc->mi.blueFieldPosition;
    if (dc->mi.alphaMaskSize == 0) {
        dc->pf.alphaAdjust = 0;
        dc->pf.alphaMask = 0;
        dc->pf.alphaPos = 0;
        }
    else {
        dc->pf.alphaAdjust = 8 - dc->mi.alphaMaskSize;
        dc->pf.alphaMask = 0xFF >> dc->pf.alphaAdjust;
        dc->pf.alphaPos = dc->mi.alphaFieldPosition;
        }
}

/****************************************************************************
DESCRIPTION:
Returns the pixel depth for the device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Pointer to device context of interest

RETURNS:
Pixel depth for the device context.

SEE ALSO:
MGL_getPixelFormat
****************************************************************************/
int MGLAPI MGL_getBitsPerPixel(
    MGLDC *dc)
{
    return dc->mi.bitsPerPixel;
}

/****************************************************************************
DESCRIPTION:
Returns the maximum available color value.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check

RETURNS:
Maximum color value for current video mode.

REMARKS:
Returns the value of the largest available color value for the current video mode.
This value will always be one less than the number of available colors in that
particular video mode.
****************************************************************************/
color_t MGLAPI MGL_maxColor(
    MGLDC *dc)
{
    return dc->mi.maxColor;
}

/****************************************************************************
DESCRIPTION:
Returns the maximum available hardware video page index.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to check.

RETURNS:
Index of maximum available hardware video page.

REMARKS:
Returns the index of the highest hardware video page that is available. This value
will always be one less than the number of hardware video pages available. Some
video modes only have one hardware video page available, so this value will be 0.
****************************************************************************/
int MGLAPI MGL_maxPage(
    MGLDC *dc)
{
    return dc->mi.maxPage;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active hardware display page.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Currently active hardware display page.

REMARKS:
This function returns the currently active hardware display page number. The first
hardware display page is 0, the second is 1 and so on. The number of available
hardware pages depends on the type of underlying hardware, the video mode
resolution and amount of video memory installed. Thus not all video modes support
multiple hardware display pages.

All MGL output is always sent to the currently active hardware display page, and
changing the active and visual display pages is used to implement double buffering.

SEE ALSO:
MGL_setActivePage, MGL_getVisualPage, MGL_setVisualPage.
****************************************************************************/
int MGLAPI MGL_getActivePage(
    MGLDC *dc)
{
    return dc->activePage;
}

/****************************************************************************
DESCRIPTION:
Returns the currently visible hardware video page.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest

RETURNS:
Currently visible hardware video page.

REMARKS:
This function returns the currently visible hardware video page number for the
given device context. The first hardware video page is number 0, the second is 1
and so on. The number of available hardware video pages depends on the type of
underlying hardware, the video mode resolution and amount of video memory
installed. Thus not all video modes support multiple hardware video pages.

SEE ALSO:
MGL_setVisualPage, MGL_getActivePage, MGL_setActivePage
****************************************************************************/
int MGLAPI MGL_getVisualPage(
    MGLDC *dc)
{
    return dc->visualPage;
}

/****************************************************************************
DESCRIPTION:
Sets the currently active hardware display page for a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context of interest
page    - Number of active hardware display page to use

REMARKS:
This function sets the currently active hardware video page to which all output from
MGL is sent to. The first hardware video page is number 0, the second is 1 and so
on. The number of available hardware video pages depends on the type of
underlying hardware, the video mode resolution and amount of video memory
installed. Thus not all video modes support multiple hardware video pages.

SEE ALSO:
MGL_getActivePage, MGL_setVisualPage, MGL_getVisualPage, MGL_swapBuffers
****************************************************************************/
void MGLAPI MGL_setActivePage(
    MGLDC *dc,
    int page)
{
    if (_MGL_isFullscreenDevice(dc)) {
        dc->v->d.setActivePage(dc,page);
        if (dc == _MGL_dcPtr) {
            /* Make sure we update the globals cached in the current device
             * context also
             */
            DC.surface = dc->surface;
            DC.activeBuf = dc->activeBuf;
            DC.activePage = dc->activePage;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Sets the currently visible hardware video page for a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context
page    - New hardware display page
waitVRT - Wait for retrace flag (MGL_waitVRTFlagType)

REMARKS:
This function sets the currently visible hardware video page for a display
device context. The first hardware video page is number 0, the second is 1
and so on. The number of available hardware video pages depends on the type
of underlying hardware, the video mode resolution and amount of video memory
installed. Thus not all video modes support multiple hardware video pages.

When the visible display page is changed, you should normally allow MGL to
sync to the vertical retrace by passing a value of MGL_waitVRT in the waitVRT
parameter to ensure that the change occurs in the correct place, and that
you don't get flicker effects on the display. If you have more than two
display pages for your display device context, you can also pass in a value
of MGL_tripleBuffer, and if the hardware supports triple buffering the MGL
will implement proper support for triple buffering. If the hardware does
not support triple buffering, MGL_tripleBuffer behaves the same as
MGL_waitVRT. Triple buffering is a mechanism that allows the MGL to return
immediately without waiting for the vertical sync period after changing the
visual display page, but ensure thats if the application runs at a frame
rate higher than the refresh rate of the graphics adapter (ie: 60fps if the
refresh is 60Hz), the frame rate will lock to the vertical refresh frequency
and you will not get any flicker.

You may however want to turn off the vertical retrace synching if you are
synching up with the retrace period using some other means by passing a
value of MGL_dontWait to the waitVRT parameter. This is also useful if you
are measuring the performance of your application and you want it to run at
full speed without without the overhead of waiting for the vertical retrace.

Note that if you wish to implement both double buffering and hardware
scrolling or panning, you should call the MGL_setDisplayStart function first
with waitVRT set to -1, and then call this function with waitVRT set to
MGL_waitVRT or MGL_tripleBuffer to actually update the hardware. The first
call to MGL_setDisplayStart simply updates the internal display start
variables but does not program the hardware. For more information please see
the MGL_setDisplayStart function.

SEE ALSO:
MGL_getVisualPage, MGL_getActivePage, MGL_setActivePage,
MGL_swapBuffers, MGL_setDisplayStart
****************************************************************************/
void MGLAPI MGL_setVisualPage(
    MGLDC *dc,
    int page,
    int waitVRT)
{
    if (_MGL_isFullscreenDevice(dc)) {
        dc->v->d.setVisualPage(dc,page,waitVRT);
        if (dc == _MGL_dcPtr) {
            /* Make sure we update the globals cached in the current device
             * context also
             */
            DC.startX = dc->startX;
            DC.startY = dc->startY;
            DC.visualPage = dc->visualPage;
            DC.CRTCBase = dc->CRTCBase;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Changes the display start address for virtual scrolling.

HEADER:
mgraph.h

PARAMETERS:
dc      - Scrolling display device context to change
x       - New display start x coordinate
y       - New display start y coordinate
waitVRT - Wait for retrace flag (MGL_waitVRTFlagType)

REMARKS:
This function sets the CRTC display starting address for the hardware
scrolling device context to the specified (x,y) coordinate. You can use this
routine to implement hardware scrolling or panning by moving the display
start address coordinates.

The waitVRT flag is used for synchronizing with the vertical retrace and can
be one of the following values:

waitVRT         - Meaning
MGL_waitVRT     - Set coordinates and update hardware, waiting for a vertical
                  retrace during the update for flicker free panning.
MGL_dontWait    - Set coordinates and update hardware, but do not wait for
                  a vertical retrace when changing the hardware start
                  address.
minus1          - Set coordinates but don't update hardware display start.

Passing a waitVRT flag of -1 can be used to implement double buffering and
hardware scrolling at the same time. To do this you would call this function
first to set the display start x and y coordinates without updating the
hardware, and then call MGL_setVisualPage to swap display pages and the new
hardware start address will then be programmed.

Note:   This function does not allow the MGL_tripleBuffer flag to be passed
        in. If you are doing triple buffering with virtual scrolling, you
        must pass a value of -1 in the waitVRT parameter, and then pass
        a value of MGL_tripleBuffer in the waitVRT parameter of the
        MGL_setVisualPage function.

SEE ALSO:
MGL_getDisplayStart, MGL_createScrollingDC
****************************************************************************/
void MGLAPI MGL_setDisplayStart(
    MGLDC *dc,
    int x,
    int y,
    int waitVRT)
{
    if (_MGL_isFullscreenDevice(dc)) {
        dc->v->d.setDisplayStart(dc,x,y,waitVRT);
        if (dc == _MGL_dcPtr) {
            /* Make sure we update the globals cached in the current device
             * context also
             */
            DC.startX = dc->startX;
            DC.startY = dc->startY;
            DC.visualPage = dc->visualPage;
            DC.CRTCBase = dc->CRTCBase;
            }
        }
}

/****************************************************************************
DESCRIPTION:
Returns the current hardware display starting coordinates.

HEADER:
mgraph.h

PARAMETERS:
dc  - Hardware scrolling device context
x   - Place to store the display start x coordinate
y   - Place to store the display start y coordinate

REMARKS:
This function returns the current hardware display start coordinates for the hardware
scrolling display device context. You can change the display start address with the
MGL_setDisplayStart function.

SEE ALSO:
MGL_setDisplayStart
****************************************************************************/
void MGLAPI MGL_getDisplayStart(
    MGLDC *dc,
    int *x,
    int *y)
{
    if (_MGL_isFullscreenDevice(dc)) {
        *x = dc->startX;
        *y = dc->startY;
        }
    else
        *x = *y = 0;
}

/****************************************************************************
DESCRIPTION:
Waits for the vertical sync for a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context of interest

RETURNS:
True on success, false if function not available on hardware device.

REMARKS:
This function will wait until the next vertical sync comes along for the
current fullscreen graphics mode, before returning.

Note:   This function may not be available on all display devices, so make
        sure you check the return value to see if the function is
        implemented and available.

SEE ALSO:
MGL_isVSync, MGL_getCurrentScanLine
****************************************************************************/
ibool MGLAPI MGL_vSync(
    MGLDC *dc)
{
    if (dc->v->d.vSync) {
        dc->v->d.vSync(dc);
        return true;
        }
    return false;
}

/****************************************************************************
DESCRIPTION:
Returns true if the vertical sync is currently active

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context of interest

RETURNS:
True if in vSync, false if not and -1 if function not available on hardware
device.

REMARKS:
This function will wait until the next vertical sync comes along for the
current fullscreen graphics mode, before returning.

Note:   This function may not be available on all display devices, so make
        sure you check the return value to see if the function is
        implemented and available.

SEE ALSO:
MGL_vSync, MGL_getCurrentScanLine
****************************************************************************/
int MGLAPI MGL_isVSync(
    MGLDC *dc)
{
    if (dc->v->d.isVSync)
        return dc->v->d.isVSync(dc);
    return -1;
}

/****************************************************************************
DESCRIPTION:
Returns the line currently being displayed on the screen.

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context of interest

RETURNS:
Line currently being display on the screen, -1 if not available.

REMARKS:
This function will determine what line the hardware is currently display
on the screen (the current raster scan), and return this value. It will
be a value in the range from 0 to the vertical total value (usually about
10% more than the vertical number of lines in the mode). This can be useful
for implementing raster scan chasing algorithms for smooth animation
when fullscreen page flipping is not available.

Note:   This function may not be available on all display devices, so make
        sure you check the return value to see if the function is
        implemented and available. If the function is not available, this
        function will return a value of -1.

SEE ALSO:
MGL_vSync, MGL_isVSync
****************************************************************************/
int MGLAPI MGL_getCurrentScanLine(
    MGLDC *dc)
{
    if (dc->v->d.getCurrentScanLine)
        return dc->v->d.getCurrentScanLine(dc);
    return -1;
}

/****************************************************************************
DESCRIPTION:
Tests whether the palette for a device context uses an 8-bit wide palette.

HEADER:
mgraph.h

RETURNS:
True if palette is 8 bits wide, false if 6 bits wide.

PARAMETERS:
dc  - Display device context of interest.

REMARKS:
This function will return true if the display device context is using an
8-bit per primary hardware palette, or false if it is using a 6-bit per
primary hardware palette. The original VGA standard only supports 6-bits
per primary for the color palette, giving you the selection of 256 colors
out of 256,000 for 8bpp modes. However more modern controllers provide
support for 8-bits per primary, giving you a selection of 256 colors out of
a total 16.7 million for 8bpp modes.
****************************************************************************/
ibool MGLAPI MGL_haveWidePalette(
    MGLDC *dc)
{
    if (_MGL_isFullscreenDevice(dc))
        return dc->v->d.widePalette;
    return false;
}

/****************************************************************************
DESCRIPTION:
Enables double buffering for the specified display device context.

HEADER:
mgraph.h

dc  - Device context to enable double buffering for

RETURNS:
True if double buffering is now enabled, false if not.

REMARKS:
Enables double buffered graphics mode for the specified device context if possible.
When the device context is in double buffered mode, all active output is sent to the
hidden backbuffer, while the current front buffer is being displayed. You then make
calls to MGL_swapBuffers to swap the front and back buffers so that the previously
hidden backbuffer is instantly displayed.

If you intend to start double buffered graphics, you should make sure you call the
MGL_createDisplayDC function with the double buffer flag set to true, so that
some of offscreen video memory will be allocated for the backbuffer. If the device
context only has one video page available, double buffering cannot be started and
this function will fail.

SEE ALSO:
MGL_singleBuffer, MGL_swapBuffers
****************************************************************************/
ibool MGLAPI MGL_doubleBuffer(
    MGLDC *dc)
{
    if (dc->mi.maxPage > 0) {
        MGL_setActivePage(dc,(dc->visualPage+1)%2);
        return true;
        }
    else
        return false;
}

/****************************************************************************
DESCRIPTION:
Returns the display device context single buffered mode.

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context

REMARKS:
This function puts the display device context into single buffer mode. The active
display page is made to be the same as the current visual display page for hardware
double buffering. This may or may not be the first hardware video page.

SEE ALSO:
MGL_doubleBuffer, MGL_swapBuffers
****************************************************************************/
void MGLAPI MGL_singleBuffer(
    MGLDC *dc)
{
    MGL_setActivePage(dc,dc->visualPage);
}

/****************************************************************************
DESCRIPTION:
Swaps the currently active front and back buffers for a display device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context
waitVRT - Wait for retrace flag (MGL_waitVRTFlagType)

REMARKS:
This function swaps the currently active front and back buffers. This
routine should only be called after the MGL_doubleBuffer has been called to
initialize the double buffering for the device context. Once double
buffering has been set up, all output from MGL will go to the current
offscreen buffer, and the output can be made visible by calling this
routine. This routine is the standard technique used to achieve smooth
animation.

When the visible display buffer is changed, you should normally allow MGL to
sync to the vertical retrace by passing a value of MGL_waitVRT in the waitVRT
parameter to ensure that the change occurs in the correct place, and that
you don't get flicker effects on the display.

You may however want to turn off the vertical retrace synching if you are
synching up with the retrace period using some other means by passing a
value of MGL_dontWait to the waitVRT parameter. This is also useful if you
are measuring the performance of your application and you want it to run at
full speed without without the overhead of waiting for the vertical retrace.

Note: This function only implements double buffering so do /not/ pass a
      value of MGL_tripleBuffer in the waitVRT parameter. If you need triple
      buffering support, use the MGL_setActivePage and MGL_setVisual page
      functions directly to implement this.

SEE ALSO:
MGL_doubleBuffer, MGL_singleBuffer
****************************************************************************/
void MGLAPI MGL_swapBuffers(
    MGLDC *dc,
    int waitVRT)
{
    MGL_setActivePage(dc,(dc->activePage+1)%2);
    MGL_setVisualPage(dc,(dc->visualPage+1)%2,waitVRT);
}

/****************************************************************************
DESCRIPTION:
Returns the current packed pixel format information.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of interest
pf  - Place to store the pixel format information

REMARKS:
This function returns the current pixel format information for the specified device
context. This information is used by MGL to encode the packed pixel information,
and can be used by your application to work out how to pack values correctly for
the RGB device contexts.

SEE ALSO:
MGL_packColor, MGL_unpackColor
****************************************************************************/
void MGLAPI MGL_getPixelFormat(
    MGLDC *dc,
    pixel_format_t *pf)
{
    *pf = dc->pf;
}

/****************************************************************************
DESCRIPTION:
Computes the address of a pixel in the device context surface.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to compute the pixel address for
x   - X coordinate of pixel to address
y   - Y coordinate of pixel to address

RETURNS:
Pointer to the start of the pixel in the surface of the device context.

REMARKS:
This function computes the address of a pixel in the surface of a specific
device context. This function is most useful for developing custom rendering
routines that draw directly to the surface of a device context, and will
compute the address of the pixel correctly regardless of the color depth
of the device context. Essentially this function computes the following:

    addr = dc->surface + (y * bytesPerLine) + (x * bytesPerpixel)

If you are going to be doing a lot of address calculations, it will be
faster to optimise your code to do the calculations directly in place (such
as with a macro) and specifically to eliminate the last multiply if you
know in advance what color depth you are working with (ie: change it to be
x, x*2, x*3 or x*4 depending on the color depth).

Note: You cannot use this function to address the device context surface
      if the device surface access type returned by MGL_getSurfaceAccessType
      is set to MGL_NO_ACCESS.
****************************************************************************/
void * MGLAPI MGL_computePixelAddr(
    MGLDC *dc,
    int x,
    int y)
{
    return PIXEL_ADDR(x,y,dc->surface,dc->mi.bytesPerLine,dc->mi.bitsPerPixel);
}

/****************************************************************************
DESCRIPTION:
Turns on or off identity palette checking.

HEADER:
mgraph.h

PARAMETERS:
enable  - True to enable identity palette checking, false to disable

RETURNS:
Old value of the identity palette check flag.

REMARKS:
Turns on or off the checking of identity palette mappings for MGL. This is a
global flag, and by default, identity palette checking is turned on.

When any MGL blitting function is called for 4 and 8 bit source bitmaps being
copied to either 4 or 8bpp destination device contexts, MGL first checks if
the color palettes for the source and destination bitmaps are the same. If they
are not, MGL translates the pixel values from the source bitmap to the destination
color palette, looking for the closest match color if an exact match is not
found. In order to obtain maximum performance for blt’ing bitmaps in color index
modes, you should ensure that the color palette in the source device matches the
color palette in the destination device to avoid on the fly palette translation.
If you know in advance that the palette is identical for a series of blit
operations, you can turn off all identity palette checking in MGL with the
MGL_checkIdentityPalette function.

When any MGL blitting function called for 4 and 8 bit source bitmaps being
copied to RGB destination device contexts, MGL will convert the pixels in the
source bitmap using the source bitmap palette to map them to the destination
pixel format. If however you know in advance that the palette for all source
bitmaps is identical for a series of blit operations, you can use the
MGL_checkIdentityPalette function to disable source palette translation. In
this case the MGL will translate all color index bitmaps using the pre-defined
color translation palette stored in the destination device context. You
would then set the destination device context palette to the common palette for
all blit operations using MGL_setPalette. If you are translating a lot of
color index bitmaps, this will increase performance by avoiding the need to
convert the palette entries to the destination pixel format for every blit
operation.

SEE ALSO:
MGL_bitBlt, MGL_stretchBlt, MGL_putBitmap
****************************************************************************/
ibool MGLAPI MGL_checkIdentityPalette(
    ibool enable)
{
    ibool old = _MGL_checkIdentityPal;
    _MGL_checkIdentityPal = enable;
    return old;
}

/****************************************************************************
DESCRIPTION:
Returns the total device x coordinate dimensions.

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context

RETURNS:
Number of pixels in x direction for entire device - 1

REMARKS:
Returns the total number of pixels available along the x coordinate axis for the
currently active device context. This is different than the MGL_maxx routine which
returns the dimensions of the currently active viewport.

SEE ALSO:
MGL_sizey, MGL_maxx, MGL_maxy
****************************************************************************/
int MGLAPI MGL_sizex(
    MGLDC *dc)
{
    return (dc->size.right - dc->size.left)-1;
}

/****************************************************************************
DESCRIPTION:
Returns the total device y coordinate dimensions.

HEADER:
mgraph.h

PARAMETERS:
dc  - Display device context

RETURNS:
Number of pixels in y direction for entire device - 1

REMARKS:
Returns the total number of pixels available along the y coordinate axis for the
currently active device context. This is different than the MGL_maxy routine
which returns the dimensions of the currently active viewport.

SEE ALSO:
MGL_sizex, MGL_maxx
****************************************************************************/
int MGLAPI MGL_sizey(
    MGLDC *dc)
{
    return (dc->size.bottom - dc->size.top - 1);
}

/****************************************************************************
DESCRIPTION:
Returns the current maximum x coordinate.

HEADER:
mgraph.h

RETURNS:
Maximum x coordinate in current viewport.

REMARKS:
Returns the maximum x coordinate available in the currently active viewport. This
value will change if you change the dimensions of the current viewport.

Use the MGL_sizex routine to determine the dimensions of the physical display
area available to the application.

SEE ALSO:
MGL_maxxDC, MGL_maxy, MGL_sizex, MGL_sizey
****************************************************************************/
int MGLAPI MGL_maxx(void)
{
    return (DC.viewPort.right - DC.viewPort.left - 1);
}

/****************************************************************************
DESCRIPTION:
Returns the current maximum x coordinate for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of the target x coordinate

RETURNS:
Maximum x coordinate in current viewport.

REMARKS:
This function is the same as MGL_maxx, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_maxx, MGL_maxy, MGL_sizex, MGL_sizey
****************************************************************************/
int MGLAPI MGL_maxxDC(
    MGLDC *dc)
{
    return (dc->viewPort.right - dc->viewPort.left - 1);
}

/****************************************************************************
DESCRIPTION:
Returns the current maximum y coordinate.

HEADER:
mgraph.h

RETURNS:
Maximum y coordinate in current viewport.

REMARKS:
Returns the maximum y coordinate available in the currently active viewport. This
value will change if you change the dimensions of the current viewport.

Use the MGL_sizey routine to determine the dimensions of the physical display
area available to the program.

SEE ALSO:
MGL_maxyDC, MGL_maxx, MGL_sizex, MGL_sizey
****************************************************************************/
int MGLAPI MGL_maxy(void)
{
    return (DC.viewPort.bottom - DC.viewPort.top - 1);
}

/****************************************************************************
DESCRIPTION:
Returns the current maximum y coordinate for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context of the target y coordinate

RETURNS:
Maximum y coordinate in current viewport.

REMARKS:
This function is the same as MGL_maxy, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_maxy, MGL_maxx, MGL_sizex, MGL_sizey
****************************************************************************/
int MGLAPI MGL_maxyDC(
    MGLDC *dc)
{
    return (dc->viewPort.bottom - dc->viewPort.top - 1);
}

/****************************************************************************
DESCRIPTION:
Enables free running stereo display mode

HEADER:
mgraph.h

REMARKS:
This function enables the free running stereo display mode for LC shutter
glasses. This function only works if the display device context you created
was a stereo display device context created with MGL_createStereoDisplayDC.
By default free running stereo mode is on when you create the stereo
display device context.

SEE ALSO:
MGL_stopStereo, MGL_createStereoDisplayDC
****************************************************************************/
void MGLAPI MGL_startStereo(
    MGLDC *dc)
{
    if (_MGL_isFullscreenDevice(dc) && dc->v->d.isStereo != _MGL_NO_STEREO)
        dc->v->d.startStereo(dc);
}

/****************************************************************************
DESCRIPTION:
Disables free running stereo display mode

HEADER:
mgraph.h

REMARKS:
This function disables the free running stereo display mode for LC shutter
glasses. This function only works if the display device context you created
was a stereo display device context created with MGL_createStereoDisplayDC.
By default free running stereo mode is on when you create the stereo
display device context, and you can use this function to disable it in
parts of your application that dont require stereo (such as when navigating
the menu system etc). Note that when stereo mode is disabled, the MGL always
displays from the left eye buffer.

SEE ALSO:
MGL_startStereo, MGL_createStereoDisplayDC
****************************************************************************/
void MGLAPI MGL_stopStereo(
    MGLDC *dc)
{
    if (_MGL_isFullscreenDevice(dc) && dc->v->d.isStereo != _MGL_NO_STEREO)
        dc->v->d.stopStereo(dc);
}

/****************************************************************************
DESCRIPTION:
Sets the color index used for the blue code stereo sync mechanism

HEADER:
mgraph.h

REMARKS:
This function sets the color index used in the blue code stereo sync
mechanism. The blue code system as pioneered by StereoGraphics, requires
the MGL to draw pure blue sync lines at the bottom of the screen of differing
lengths to signal to the LC glasses which is the left and right eye
images. In order to do this in 8bpp color index modes, the MGL must take
up a single palette entry for drawing the blue codes, which by default
is set to index 255. You can use this function to set the blue code index
to another value other than 255 to suit your applications palette arrangement.

Note:   You must call this function /before/ you create a stereo display
        device context if you wish to change the blue code index.

SEE ALSO:
MGL_startStereo, MGL_stopStereo, MGL_createStereoDisplayDC,
MGL_setStereoSyncType
****************************************************************************/
void MGLAPI MGL_setBlueCodeIndex(
    int index)
{
    _MGL_blueCodeIndex = index;
}

