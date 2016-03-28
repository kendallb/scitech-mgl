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
* Description:  Linux specific code bindings for the MGL.
*
****************************************************************************/

#include "mgl.h"
#include "mgllinux/internal.h"

/*--------------------------- Global Variables ----------------------------*/

static ibool    installed = false;
static PM_HWND  hwndConsole;

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
    int devices = GA_enumerateDevices(false);
    if (GA_status() == nInvalidLicense)
        __MGL_result = grInvalidLicense;
    return devices;
}

/****************************************************************************
REMARKS:
Function to initialise the internals of the MGL for normal operation.
****************************************************************************/
void _MGL_initInternal(void)
{
    installed = true;
    hwndConsole = PM_openConsole((PM_HWND)NULL,0,640,480,8,true);
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
    PM_closeConsole(hwndConsole);
    installed = false;
}

/****************************************************************************
REMARKS:
Initialise the bitmap handle as necessary for the memory DC.
****************************************************************************/
void _MGL_initBitmapHandle(
    MGLDC *dc,
    MGL_HBITMAP hbm)
{
    (void)dc;
    (void)hbm;
}

/****************************************************************************
PARAMETERS:
dc  - Device context.

RETURNS:
Total size of the device context surface

REMARKS:
Allocate the memory for the device surface. This version
simply uses calloc() to allocate a buffer large enough.
****************************************************************************/
long _MGL_allocateSurface(
    MGLDC *dc)
{
    long size = (long)dc->mi.bytesPerLine * (dc->mi.yRes+1);
    if ((dc->surface = PM_calloc(1,size)) == NULL) {
        FATALERROR(grNoMem);
        return 0;
        }
    return size;
}

/****************************************************************************
PARAMETERS:
dc  - Memory device context with surface to destroy

REMARKS:
Destroys the packed pixel device surface for a memory device context.
****************************************************************************/
void _PM_freeSurface(
    MGLDC *dc)
{
    PM_free(dc->surface);
}

/****************************************************************************
PARAMETERS:
dc      - Device context
pal     - Palette to realize
num     - Number of colors to realise
index   - First color to realize

REMARKS:
Realizes the palette for an 8 bit memory device context. If anything
special needs to be handled by the OS, it should be done in here.
****************************************************************************/
void MGLAPI PACKED8_realizePalette(
    MGLDC *dc,
    palette_t *pal,
    int num,
    int index,
    int waitVRT)
{
    (void)dc;
    (void)pal;
    (void)num;
    (void)index;
    (void)waitVRT;
}

/****************************************************************************
DESCRIPTION:
Function to hook the window procedure for a windowed DC

PARAMETERS:
dc  - Device context to hook the window procedure for

REMARKS:
This is an internal function that allows the OS specific code to hook the
window event handling procedure for the passed in windowed device context,
so that we can directly manage and handle common events on behalf of the
application program. We especially need to hook the window procedure to
hook into the windowing system to maintain a complex clip list for the
window as necessary.
{secret}
****************************************************************************/
void _MGL_hookWindowProc(
    MGLDC *dc)
{
    (void)dc;
}

/****************************************************************************
DESCRIPTION:
Function to unhook the window procedure for a windowed DC

PARAMETERS:
dc  - Device context to unhook the window procedure for

REMARKS:
This is an internal function that allows the OS specific code to unhook the
window event handling procedure for the passed in windowed device context
to properly clean up when a windowed DC is destroyed.
{secret}
****************************************************************************/
void _MGL_unhookWindowProc(
    MGLDC *dc)
{
    (void)dc;
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
}

