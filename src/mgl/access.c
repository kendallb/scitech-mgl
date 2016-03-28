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
* Description:  Framebuffer access control functions for the MGL.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Enables direct framebuffer access.

HEADER:
mgraph.h

REMARKS:
Enables direct framebuffer access so that you can directly rasterize to the
linear framebuffer memory using your own custom routines. Note that calling
this function is absolutely necessary when using hardware acceleration, as
this function correctly arbitrates between the hardware accelerator graphics
engine and your direct framebuffer rasterizing code.

SEE ALSO:
MGL_beginDirectAccess, MGL_endDirectAccess, MGL_beginDirectAccessDC,
MGL_endDirectAccessDC
****************************************************************************/
void MGLAPI MGL_beginDirectAccess(void)
{
    if (DC.r.LockBuffer) {
        DC.r.LockBuffer(DC.activeBuf);
        _MGL_dcPtr->surface = DC.surface = DC.activeBuf->Surface;
        }
}

/****************************************************************************
DESCRIPTION:
Disables direct framebuffer access.

HEADER:
mgraph.h

REMARKS:
Disables direct framebuffer access so that you can use the accelerator
functions to draw to the framebuffer memory. Note that calling this function
is absolutely necessary when using hardware acceleration, as this function
and the corresponding MGL_beginDirectAccess correctly arbitrate between the
hardware accelerator graphics engine and your direct framebuffer writes.

SEE ALSO:
MGL_beginDirectAccess, MGL_endDirectAccess, MGL_beginDirectAccessDC,
MGL_endDirectAccessDC
****************************************************************************/
void MGLAPI MGL_endDirectAccess(void)
{
    if (DC.r.UnlockBuffer) {
        DC.r.UnlockBuffer(DC.activeBuf);
        _MGL_dcPtr->surface = DC.surface = NULL;
        }
}

/****************************************************************************
DESCRIPTION:
Enables direct framebuffer access (device context specific)

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to enable direct access to

REMARKS:
This is the same as MGL_beginDirectAccess but it takes a device context
pointer instead of requiring the device context to be the current device
context.

SEE ALSO:
MGL_beginDirectAccess, MGL_endDirectAccess, MGL_beginDirectAccessDC,
MGL_endDirectAccessDC
****************************************************************************/
void MGLAPI MGL_beginDirectAccessDC(
    MGLDC *dc)
{
    if (dc->r.LockBuffer) {
        dc->r.LockBuffer(dc->activeBuf);
        dc->surface = dc->activeBuf->Surface;
        if (dc == _MGL_dcPtr)
            DC.surface = dc->surface;
        }
}

/****************************************************************************
DESCRIPTION:
Disables direct framebuffer access (device context specific)

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to enable direct access to

REMARKS:
This is the same as MGL_endDirectAccess but it takes a device context
pointer instead of requiring the device context to be the current device
context.

SEE ALSO:
MGL_beginDirectAccess, MGL_endDirectAccess, MGL_beginDirectAccessDC,
MGL_endDirectAccessDC
****************************************************************************/
void MGLAPI MGL_endDirectAccessDC(
    MGLDC *dc)
{
    if (DC.r.UnlockBuffer) {
        DC.r.UnlockBuffer(dc->activeBuf);
        DC.surface = NULL;
        if (dc == _MGL_dcPtr)
            DC.surface = dc->surface;
        }
}

/****************************************************************************
DESCRIPTION:
Setup for high speed pixel drawing.

HEADER:
mgraph.h

REMARKS:
Sets up the video hardware for plotting single pixels as fast a possible. You
must call this routine before calling any of the MGL_pixel and MGL_getPixel
routines to ensure correct operation, and you must call the MGL_endPixel
routine after you have finished.

This routine is intended primarily to ensure fast operation if you need to
plot more than a single pixel at a time.

SEE ALSO:
MGL_endPixel, MGL_pixel, MGL_getPixel.
****************************************************************************/
void MGLAPI MGL_beginPixel(void)
{
    if (DC.deviceType == MGL_WINDOWED_DEVICE)
        DC.r.BeginVisibleClipRegion(&DC);
    else if (DC.r.LockBuffer)
        DC.r.LockBuffer(DC.activeBuf);
}

/****************************************************************************
DESCRIPTION:
Ends high speed pixel drawing operation.

HEADER:
mgraph.h

REMARKS:
This function ends a set of high speed pixel drawing operations, started
with a call to MGL_beginPixel. This routine is intended primarily to ensure
fast operation if you intend to plot more than a single pixel at a time.

SEE ALSO:
MGL_beginPixel
****************************************************************************/
void MGLAPI MGL_endPixel(void)
{
    /* Pixels are handled directly in the shadow buffer code */
    if (DC.deviceType == MGL_WINDOWED_DEVICE)
        DC.r.EndVisibleClipRegion(&DC);
    else if (DC.r.UnlockBuffer)
        DC.r.UnlockBuffer(DC.activeBuf);
}

