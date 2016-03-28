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
* Environment:  Unix/X11
*
* Description:  Unix/X11 code binding for the NGL.
*
****************************************************************************/

#include "mgl.h"
#include "mglunix.h"
#include "mgldd.h"
#include <unistd.h>
#include <time.h>

static  ibool   installed = false;  /* Is the MGL installed?            */
static  ibool   initWindowed = true;
int     _MGL_driverId;              /* Driver selected                  */
int     _MGL_modeId;                /* Mode selected                    */
int     _MGL_deskX;                 /* Desktop resolution               */
int     _MGL_deskY;                 /* Desktop resolution               */

extern drivertype XWINDC_driver;

Display *globalDisplay;
// Screen  *scr;
// Window   wnd;
// GC       gc;

ibool MGLAPI MGL_initWindowed(
    const char *mglpath)
{
    // TODO
    if (initWindowed)
        return true;

    //  wnd = XCreateSimpleWindow(dpy, RootWindowOfScreen(scr));

    //  if (!_MGL_initCommon(mglpath))
    return false;

}

void MGLAPI MGL_setX11Display(
    Display *dpy)
{
    globalDisplay = dpy;
}

/*******************************************************************************
DESCRIPTION:
Create a new windowed device context.

HEADER:
mglunix.h

PARAMETERS:
dpy     - X11 display handle with which to associate new device context
hwnd    - Window handle with which to associate new device context

RETURNS:
Pointer to the allocated windowed device context, or NULL if not enough memory.

REMARKS:
Creates a new windowed device context for drawing information into a window on
the X-Window desktop. When you create a Windowed device context, you associate
it with a standard Windows HWND for the window that you wish MGL to display
it’s output on. Windowed device contexts are special device contexts in that you
cannot directly access the surface for the device, nor can you actually use the MGL
rasterizing functions to draw on the device surface. The only rasterizing functions
supported are the MGL_bitBlt and MGL_stretchBlt for blt’ing data from memory device contexts to the window on
the desktop.

However in order to change the color palette values for the data copied to the
window, you must use the MGL palette functions on the windowed display device
context. Note that MGL automatically takes care of creating a proper Windows
identity palette for the windowed device context, so as long as you program the
same palette values for the windowed device and the memory device you should get
the maximum performance blt’ing speed.

MGL automatically uses the highest performance method for implementing the
BitBlt operations under Windows, and requires the WinG library to be installed if
the target platform is Windows 3.1 or Windows NT 3.1. When using WinG, the
only pixel depth supported for Windowed device contexts is 8 bits per pixel, and the
only pixel format valid for BitBlt operations is 8 bit memory device contexts. You
can still create memory device contexts with higher pixel formats, but you will need
to Blt the data to a real 8 bit DC before you can display it in the window.

If the target platform is Windows 95 or Windows NT 3.5 or later, MGL will use
CreateDIBSection for maximum performance, and can create and Blt memory
device contexts of any pixel depth to the display device context. For maximum
performance you should create your memory device contexts with the same pixel
format used by the windowed display device context.

Note that if you wish to only use windowed output and you intend to target
Windows NT for your application, you must use the MGL_initWindowed function
to initialize MGL. This will not attempt to load the WinDirect full screen support
DLL’s, which are not compatible with Windows NT.

SEE ALSO:
MGL_createMemoryDC, MGL_createDisplayDC, MGL_destroyDC,
MGL_setWinDC, MGL_activatePalette, MGL_initWindowed
*******************************************************************************/
MGLDC * MGLAPI MGL_createWindowedDC(
    MGL_HWND hwnd)
{
    MGLDC   *dc;
    driverent de;

    if ((dc = _LST_newNode(sizeof(MGLDC))) == NULL) {
        __MGL_result = grNoMem;
        return NULL;
        }
    if (_MGL_winDCList == NULL) {
        if ((_MGL_winDCList = _LST_create()) == NULL) {
            __MGL_result = grNoMem;
            goto Error;
            }
        }

    de.driver = &XWINDC_driver;
    dc->wm.xwindc.dpy = globalDisplay;
    if (!_MGL_initDC(dc,&de,hwnd,-1,-1,1,false,MGL_DEFAULT_REFRESH))
        goto Error;
    _MGL_deskX = 0;
    _MGL_deskY = 0;
    RESET_DEFAULT_CW();

    /* Add the new DC to the start of the DC chain */
    _LST_addAfter(_MGL_winDCList,dc,LST_HEAD(_MGL_winDCList));
    return dc;

Error:
    if (_MGL_winDCList && _MGL_winDCList->count == 0) {
        _LST_destroy(_MGL_winDCList,_LST_freeNode);
        _MGL_winDCList = NULL;
        }
    _LST_freeNode(dc);
    return NULL;
}

void MGL_delay(int msecs)
{
    usleep(msecs * 1000);
}

// TODO
ibool MGLAPI MGL_init(
    int *pDriver,
    int *pMode,
    const char *_mglpath)
{
    char *mglpath = strdup(_mglpath);
    char *ptr;

    /* Substitute \ with / in the path for Unix file systems. */
    for(ptr = mglpath; *ptr; ptr++)
        if(*ptr == '\\')
            *ptr = '/';

    globalDisplay = XOpenDisplay(NULL);
    if(!globalDisplay)
        MGL_fatalError("Could not open X11 display! Check your DISPLAY variable and the access permissions.");

    if (!MGL_initWindowed(mglpath)){
        free(mglpath);
        return false;
        }
    MGL_detectGraph(pDriver,pMode);
    if (*pDriver == grNONE) {
        __MGL_result = grNotDetected;
        free(mglpath);
        return false;
        }

    /* Save selected driver and mode number for later */
    _MGL_driverId = *pDriver;
    if (*pMode != grDETECT) {
        if (!MGL_changeDisplayMode(*pMode))
            return false;
        }
    _MGL_initInternal();

    free(mglpath);
    return true;
}

/****************************************************************************
DESCRIPTION:
Declare a fatal error and exit gracefully.

HEADER:
mgraph.h

PARAMETERS:
msg - Message to display

REMARKS:
A fatal internal error has occurred, so we shutdown the graphics systems, display
the error message and quit. You should call this function to display your own
internal fatal errors.
****************************************************************************/
void MGLAPI MGL_fatalError(
    const char *msg)
{
    MGL_exit();
    fprintf(stderr, "*** MGL Fatal Error! ***\n%s\n", msg);
    exit(EXIT_FAILURE);
}

void MGLAPI MGL_exit(void)
{
    /* Uncache the current DC */
    MGL_makeCurrentDC(NULL);

#ifndef MGL_LITE
    /* Destroy all region algebra memory pools */
    _PM_freeSegmentList();
    _PM_freeSpanList();
    _MGL_freeRegionList();
#endif

}

/****************************************************************************
DESCRIPTION:
Suspend low level interrupt handling.

HEADER:
mgldos.h

REMARKS:
This function suspends the low level interrupt handling code used by the SciTech MGL when it
is initialized since MGL takes over the keyboard and mouse interrupt handlers to
manage it's own event queue. If you wish to shell out to DOS or to spawn another
application program temporarily, you must call this function to suspend interrupt
handling or else the spawned application will not be able to access the keyboard and
mouse correctly.

SEE ALSO:
MGL_resume

****************************************************************************/
void MGLAPI MGL_suspend(void)
{
    if (installed) {
        _EVT_suspend();
        }
}

/****************************************************************************
DESCRIPTION:
Resume low level event handling code.

HEADER:
mgldos.h

REMARKS:
Resumes the event handling code for MGL. This function should be used to re-
enable the MGL event handling code after shelling out to DOS from your
application code or running another application.

SEE ALSO:
MGL_suspend, MGL_init
****************************************************************************/
void MGL_resume(void)
{
    if (installed) {
        _EVT_resume();
        }
}

void MGLHW_init(void)
{
    // Nothing to really do here in X11
}

/* {secret} */
static  ibool _MGL_destroyWindowedDC(MGLDC *dc)
/****************************************************************************
*
* Function:     _MGL_destroyWindowedDC
* Parameters:   dc  - Device context to destroy
* Returns:      True if the context was destroyed, false on error.
*
* Description:  Searches for the windowed device context in the list of
*               currently allocated contexts, and destroys it if it
*               exists.
*
****************************************************************************/
{
    MGLDC   *prev;

    /* Remove the DC from the list of allocated memory DC's             */

    if ((prev = _LST_findNode(_MGL_winDCList,dc)) == NULL) {
        __MGL_result = grInvalidDC;
        return false;
        }
    _LST_removeNext(_MGL_winDCList,prev);

    /* Free up all memory occupied by the DC */
    PM_free(dc->colorTab);
    dc->v->w.destroy(dc);
    _LST_freeNode(dc);

    if (_MGL_winDCList->count == 0) {
        /* TODO: Delete all the global X11 stuff here */
        /* This is the last windowed device context, so destroy the list */
        _LST_destroy(_MGL_winDCList,_LST_freeNode);
        _MGL_winDCList = NULL;
        }

    return true;
}

/****************************************************************************
DESCRIPTION:
Destroy a given device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to be destroyed

RETURNS:
True if context was destroyed, false on error.

REMARKS:
Destroys a specified device context, freeing up all resources allocated by the device
context. This can fail for a number of reasons, so check the MGL_result code to
determine the cause of the failure.

If the device context that was destroyed was the last active display device context,
the video mode is reset back to the original video mode (or back to the normal GDI
desktop for Windows). Note that calling MGL_exit automatically destroys all
currently allocated device contexts.

SEE ALSO:
MGL_createDisplayDC, MGL_createOffscreenDC, MGL_createLinearOffscreenDC,
MGL_createMemoryDC, MGL_createWindowedDC
****************************************************************************/
ibool MGLAPI MGL_destroyDC(
    MGLDC *dc)
{
    if(!dc)
        return true;
    if (dc == _MGL_dcPtr)
        MGL_makeCurrentDC(NULL);
    switch (dc->deviceType) {
        case MGL_DISPLAY_DEVICE:
        case MGL_FULLDIB_DEVICE:
        case MGL_OPENGL_DEVICE:
        case MGL_FS_OPENGL_DEVICE:
            return _MGL_destroyDisplayDC(dc);
#ifndef MGL_LITE
        case MGL_OFFSCREEN_DEVICE:
        case MGL_LINOFFSCREEN_DEVICE:
            return _MGL_destroyOffscreenDC(dc);
#endif
        case MGL_WINDOWED_DEVICE:
            return _MGL_destroyWindowedDC(dc);
        case MGL_MEMORY_DEVICE:
            return _MGL_destroyMemoryDC(dc);
        }
    __MGL_result = grInvalidDC;
    return false;
}

/****************************************************************************
DESCRIPTION:
Get the current timer tick count.

HEADER:
mgldos.h, mglwin.h

RETURNS:
Current timer tick count as a 32 bit integer.

REMARKS:
This function returns the current timer tick as a 32-bit integer value.
The number of ticks in a single second can be determined with the
MGL_getTickResolution function.

SEE ALSO:
MGL_getTickResolution
****************************************************************************/
ulong MGLAPI MGL_getTicks(void)
{
    return clock();
}

/****************************************************************************
DESCRIPTION:
Get duration of a timer tick.

HEADER:
mgldos.h, mglwin.h

RETURNS:
Number of seconds in a timer tick * 1,000,000.

REMARKS:
This function returns an unsigned long value indicating the duration of a timer tick
in seconds multiplied by one million. The duration of a timer tick changes
depending on the target environment, so you should use this function to convert the
value to a standard representation.

SEE ALSO:
MGL_getTicks
****************************************************************************/
ulong MGLAPI MGL_getTickResolution(void)
{
    return CLOCKS_PER_SEC;
}

/****************************************************************************
REMARKS:
Function to initialise the internals of the MGL for normal operation. This
should only need to be done once for the life of a program.
****************************************************************************/
void _MGL_initInternal(void)
{
    /* Install event handling hooks */
    if (!installed) {
        MGLHW_init();
        _MGL_initMalloc();
        _MS_init();

        /* Create the internal scratch buffer */
        if ((_MGL_buf = PM_malloc(_MGL_bufSize)) == NULL)
            MGL_fatalError("Not enough memory to allocate scratch buffer!\n");

#ifndef MGL_LITE
        /* Create all region algebra memory pools */
        _MGL_createSegmentList();
        _MGL_createSpanList();
        _MGL_createRegionList();
#endif
        installed = true;
        }
}

/****************************************************************************
REMARKS:
Destroys all device context for the currently active display device.
****************************************************************************/
static void _MGL_destroyDevice(void)
{
    /* Uncache the current DC */
    MGL_makeCurrentDC(NULL);

#ifndef MGL_LITE
    /* Destroy all active offscreen contexts */
    while (DEV.offDCList)
        _MGL_destroyOffscreenDC(_LST_first(DEV.offDCList));
#endif
    /* Destroy all active display contexts, which restores text mode */
    while (DEV.dispDCList)
        _MGL_destroyDisplayDC(_LST_first(DEV.dispDCList));

    /* Unregister all drivers for this device */
    MGL_unregisterAllDrivers();
}

/****************************************************************************
REMARKS:
Closes down the internals of the graphics library.
****************************************************************************/
void _MGL_exitInternal(void)
{
    int i;

    if (installed) {
        /* Uncache the current DC */
        MGL_makeCurrentDC(NULL);

        /* Hide mouse cursor */
        MS_hide();

        /* Remove event handling hooks */
        MGL_suspend();

        /* Destroy all active memory device contexts */
        while (_MGL_memDCList)
            _MGL_destroyMemoryDC(_LST_first(_MGL_memDCList));

        /* Destroy all display DC and offscreen DC's for all devices */
        for (i = 0; i < _MGL_numDevices; i++) {
            MGL_selectDisplayDevice(i);
            _MGL_destroyDevice();
            }
#ifndef MGL_LITE
        /* Destory all region algebra memory pools */
        _PM_freeSegmentList();
        _PM_freeSpanList();
        _MGL_freeRegionList();
#endif
        /* Destroy the memory buffer */
        if (_MGL_buf) {
            PM_free(_MGL_buf);
            _MGL_buf = NULL;
            }

        installed = false;
        }
}

/****************************************************************************
DESCRIPTION:
Changes the current fullscreen mode or switches to windowed mode.

HEADER:
mgldos.h, mglwin.h

PARAMETERS:
mode    - New display mode to use

RETURNS:
True if the mode is available, false if mode is invalid.

REMARKS:
This function changes the current fullscreen display mode used by MGL, or informs
MGL that you are about to switch to windowed mode (for Windows versions). The
application should destroy all display and offscreen device contexts currently
allocated before calling this function, and then re-create all the required device
contexts for the new mode after calling this function. A typical code sequence to
change display modes would be as follows:

    MGLDC *dc;
    ... init MGL and create DC as per normal ...
    MGL_destroyDC(dc);
    MGL_changeDisplayMode(grSVGA_640x480x256);
    dc = MGL_createDisplayDC();
    ... mode has been changed to the new mode ...


Note that if there are any active display device contexts and offscreen device
contexts when this function is called, they will be destroyed by this call and the
system will be reset back to text mode. However none of the device contexts will be
re-created and it is up to the application to recreate all necessary device contexts.

If you are using this function to change display modes on the fly in MGL and you
wish to allow the user to switch to a windowed mode under Windows, you must call
this function with the grWINDOWED parameter before you create your windowed
window, or call MGL_exit after finishing in fullscreen modes. For example the
following code might be used to switch to a windowed mode.

    // Destroy the existing fullscreen mode and DC's
    MGL_destroyDC (mgldc);
    MGL_destroyDC (memdc);
    mgldc = memdc = NULL;

    // Signal to MGL that we are going windowed
    MGL_changeDisplayMode(grWINDOWED);

    // Create the windowed window
    window = CreateWindow(...);
    ShowWindow(window, SW_SHOWDEFAULT);

SEE ALSO:
MGL_init, MGL_createDisplayDC
****************************************************************************/
ibool MGLAPI MGL_changeDisplayMode(
    int mode)
{
    __MGL_result = grOK;

    /* Check if the mode is currently available */
    if (mode >= grMAXMODE || DEV.availableModes[mode].driver == 0xFF) {
        __MGL_result = grInvalidMode;
        return false;
        }
    DEV.modeId = mode;

    /* Uncache the current DC */
    MGL_makeCurrentDC(NULL);

#ifndef MGL_LITE
    /* Destroy all active offscreen contexts for this device */
    while (DEV.offDCList)
        _MGL_destroyOffscreenDC(_LST_first(DEV.offDCList));
#endif

    /* Destroy all active display contexts, which restores text mode */
    while (DEV.dispDCList)
        _MGL_destroyDisplayDC(_LST_first(DEV.dispDCList));
    return true;
}

