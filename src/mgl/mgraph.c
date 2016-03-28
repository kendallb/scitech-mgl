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
* Description:  Main platform independant interface and setup code.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */

/*--------------------------- Global Variables ----------------------------*/

/* Private global variables */

static ibool    inited = false;     /* True if the library is inited    */
static ibool    consoleInited = false;

/* Public global variables */

MGLDC       _MGL_dc;                    /* Global device context            */
MGLDC *     _MGL_dcPtr = NULL;          /* Pointer to active context        */
int         _MGL_bufSize = DEF_MGL_BUFSIZE;
void *      _MGL_buf;                   /* Internal MGL scratch buffer      */
int         __MGL_result;               /* Result of last operation         */
ibool       __MGL_useLinearBlits = true;/* True if linear blits are used    */
char        _MGL_path[PM_MAX_PATH];     /* Root directory for all MGL files */
int         _MGL_blueCodeIndex = 0xFF;  /* Blue code index for stereo       */
LIST *      _MGL_memDCList = NULL;      /* List of memory contexts          */
LIST *      _MGL_winDCList = NULL;      /* List of windowed contexts        */
ibool       _MGL_fullScreen = true;     /* True if in fullscreen mode       */
ibool       _MGL_isBackground = false;  /* True if running in background    */
MGL_HWND    _MGL_hwndFullScreen = NULL; /* Fullscreen window handle         */
ibool       _MGL_consoleSupport = true; /* Console management enabled       */

/* Pre-defined pixel formats table */

pixel_format_t _MGL_pixelFormats[] = {
    {0xFF,0x00,0,0x00,0x00,0,0x00,0x00,0,0x00,0x00,0},  /* 8bit CI      */
    {0xFF,0x00,0,0x00,0x00,0,0x00,0x00,0,0xFF,0x08,0},  /* 8bit alpha   */
    {0x1F,0x0A,3,0x1F,0x05,3,0x1F,0x00,3,0x01,0x0F,7},  /* 555 15bpp    */
    {0x1F,0x0B,3,0x3F,0x05,2,0x1F,0x00,3,0x00,0x00,0},  /* 565 16bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0},  /* RGB 24bpp    */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0x00,0x00,0},  /* BGR 24bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0xFF,0x18,0},  /* ARGB 32bpp   */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0},  /* ABGR 32bpp   */
    {0xFF,0x18,0,0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0},  /* RGBA 32bpp   */
    {0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0,0xFF,0x00,0},  /* BGRA 32bpp   */
    };

/* Table of all display devices enumerated in the system */

int             _MGL_numDevices = 1;
int             _MGL_cntDevice = 0;
int             _MGL_glDevice = 0;
device_entry    _MGL_deviceTable[MAX_DISPLAY_DEVICES] = {{{{""}}}};

/* Table of static device drivers, not associated with a hardware device */

driverent _MGL_staticDrivers[] = {
    {MGL_PACKED8NAME,true,false,NULL,NULL,&PACKED8_driver,NULL},
    {MGL_PACKED16NAME,true,false,NULL,NULL,&PACKED16_driver,NULL},
    {MGL_PACKED24NAME,true,false,NULL,NULL,&PACKED24_driver,NULL},
    {MGL_PACKED32NAME,true,false,NULL,NULL,&PACKED32_driver,NULL},
#ifdef  __WINDOWS__
    {MGL_WINDOWEDNAME,true,false,NULL,NULL,&WINDD_driver,NULL},
#endif
    {"",false,false,NULL,NULL,NULL,NULL}
    };

/*------------------------- Implementation --------------------------------*/

#include "mgl/license.h"

/****************************************************************************
REMARKS:
This function registers a single MGL device driver for use in the device
driver detection list.
****************************************************************************/
static void _MGL_registerDriver(
    const char *name,
    ibool enabled,
    ibool opengl,
    drivertype *driver)
{
    int i = DEV.numDrivers++;
    strcpy(DEV.driverTable[i].name,name);
    DEV.driverTable[i].enabled = enabled;
    DEV.driverTable[i].opengl = opengl;
    DEV.driverTable[i].hModBPD = NULL;
    DEV.driverTable[i].hModOS = NULL;
    DEV.driverTable[i].driver = driver;
    DEV.driverTable[i].data = NULL;
}

/****************************************************************************
REMARKS:
This function registers all MGL device drivers by default in the device
detection list.
****************************************************************************/
static void _MGL_registerDrivers(void)
{
    _MGL_registerDriver(MGL_VBENAME,true,false,&VBE_driver);
    _MGL_registerDriver(MGL_SNAPNAME,true,false,&SNAP_driver);
#ifdef  __WINDOWS__
//  _MGL_registerDriver(MGL_OPENGLNAME,false,true,&OPENGL_driver);
//  _MGL_registerDriver(MGL_GLDIRECTNAME,false,true,&GLDIRECT_driver);
#endif
}

/****************************************************************************
REMARKS:
Fatal error handler for non-exported GA_glFuncs.
****************************************************************************/
static void _GL_fatalErrorHandler(void)
{
    MGL_fatalError("OpenGL supported not initialised!\n");
}

/****************************************************************************
REMARKS:
Function to initialise the exported OpenGL functions for the OpenGL
implementation we have connected to. This ensures that if an application
makes an OpenGL call before it is initialised, the program will not crash.
****************************************************************************/
static void _MGL_initGLFuncs(void)
{
    int     i,max;
    ulong   *p;

    max = sizeof(GA_glFuncs)/sizeof(GA_initLibrary_t);
    for (i = 0,p = (ulong*)&_GA_glFuncs; i < max; i++)
        *p++ = (ulong)_GL_fatalErrorHandler;
    max = sizeof(GA_gluFuncs)/sizeof(GA_initLibrary_t);
    for (i = 0,p = (ulong*)&_GA_gluFuncs; i < max; i++)
        *p++ = (ulong)_GL_fatalErrorHandler;
}

/****************************************************************************
DESCRIPTION:
Disables internal console management support.

HEADER:
mgraph.h

PARAMETERS:
enable  - True to enable console support, false to disable it

REMARKS:

SEE ALSO:
MGL_quickInit, MGL_init
****************************************************************************/
void MGLAPI MGL_enableConsoleSupport(
    ibool enable)
{
    _MGL_consoleSupport = enable;
}

/****************************************************************************
DESCRIPTION:
Initializes MGL graphics library.

HEADER:
mgraph.h

PARAMETERS:
mglpath - Path to standard MGL resource files
server  - Name of server to connect to (NULL for local system)

RETURNS:
Number of display devices attached, or 0 on error.

REMARKS:
This function initializes MGL for use, and should always be the first
function called in MGL applications. The mglpath parameter is used by the
MGL to locate all MGL resource files in their standard locations. The value
passed in here should point to the base directory where all the resources
are located. This may be the current directory if all resources are relative
to the current directory of the application (i.e. a value of "." passed in
the mglpath parameter).

After you have called MGL_init, the MGL will automatically detect the
installed graphics hardware when any of MGL_availablePages,
MGL_modeResolution, MGL_modeFlags or MGL_findMode are called. The priority
ordering of which driver will be used depends on the capabilities of the
underlying display hardware, and the drivers will be chosen in the following
order (first one in table is selected in preference to ones below it):

Driver          - Highest performance driver selected
Snap            - SciTech SNAP Graphics driver
DirectDraw      - SciTech SNAP Graphics DirectDraw driver
VBE2            - SciTech SNAP Graphics VBE/Core 2.0 driver
VBE1            - SciTech SNAP Graphics VBE/Core 1.2 driver
StandardVGA     - SciTech SNAP Graphics VGA driver

For instance if you had DirectDraw installed on your system and MGL found a
SciTech SNAP Graphics driver, those modes supported by the SciTech SNAP Graphics
drivers will use the SciTech SNAP Graphics driver in preference to the DirectDraw
driver. If however the DirectDraw driver has additional modes not supported by the
SciTech SNAP Graphics driver, the DirectDraw drivers would be used for those modes.
If however the user has DirectDraw installed and only has a VBE 2.0 driver
available, the DirectDraw drivers will be used for all modes whenever
possible.

To change this default behavior you can selectively disable those drivers
that you do not wish to be used immediately after calling MGL_init. A typical
sequence of code to register drivers and allows the program to disable
SciTech SNAP Graphics might be as follows:

    int     mode;
    MGLDC   *dc;

    if (MGL_init(".",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_disableDriver(MGL_SNAPNAME);
    if ((mode = MGL_findMode(640,480,8)) == -1)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    dc = MGL_createDisplayDC(mode,1,MGL_DEFAULT_REFRESH);
    ...

When the MGL is searching for resource files (bitmaps, icons, fonts and
cursors), it will first attempt to find the files just by using the filename
itself. Hence if you wish to look for a specific bitmap or font file, you
should pass the full pathname to the file that you want. If the filename is
a simple relative filename (i.e. "myfont.fnt"), MGL will then search in the
standard directories relative to the path specified in mglpath. The standard
locations that MGL will look for the resource files are as follows:

Resource    - Base pathname
Bitmaps     - mglpath/bitmaps
Fonts       - mglpath/fonts
Icons       - mglpath/icons
Cursors     - mglpath/cursors

As a final resort the MGL will also look for the files relative to the
MGL_ROOT environment variable. The MGL_ROOT variable can be set to point to
a standard base directory where all MGL resources will be stored on the end
user's machine. For MGL developer machines, MGL_ROOT is usually set to point
to the directory where the SciTech MGL files were installed (ie: c:\scitech
by default for DOS, Windows and OS/2 installations, $HOME/scitech for
Unix installations).

The server parameter is used to connect to a remote server to display the
output of the of the MGL program on a remote machine. For remote
connections, the server name should either be the DNS name or TCP/IP address
of the machine to connect to. For local operation on the client machine,
this parameter should simply be set to NULL.

Once you have initialised the MGL, you then need to find an appropriate
display mode to use with MGL_findDisplayMode, and then create a device
context for all output, using MGL_createDisplayDC (for fullscreen display
device contexts). To support multiple display controllers, please see the
documenation for MGL_selectDisplayDevice to initialise all installed
display devices.

If anything goes wrong during the initialization process, MGL will return a
result code via the MGL_result routine. You can then use this result code
to determine the cause of the problem, and use the MGL_errorMsg routine to
display an appropriate error message for the user.

Note:   To get up and running quickly with your first MGL program, check
        out the MGL_quickInit() function which allows you to start the
        MGL with a single line of code! Or alternatively check our the
        Game Framework API if you are developing game type applications.

Note:   Remote client/server operation is not supported at this time. This
        is something scheduled for a future release of the MGL.

SEE ALSO:
MGL_quickInit, MGL_selectDisplayDevice, MGL_enableAllDrivers,
MGL_disableDriver, MGL_availablePages, MGL_modeResolution, MGL_findMode,
MGL_addCustomMode, MGL_createDisplayDC, MGL_exit, MGL_result
****************************************************************************/
int MGLAPI MGL_init(
    const char *mglpath,
    const char *server)
{
    int     i;
    char    bpdpath[PM_MAX_PATH];

    /* Register an ISV license if being compiled into the library */
#ifndef EMPTY_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Ensure that we only get initialised once! */
    __MGL_result = grOK;
    if (inited)
        MGL_fatalError("MGL_init() called twice!");

    /* Find the number of devices attached to the system, and populate the
     * global device driver tables for all secondary controllers by copying
     * the default data from the primary controller's table.
     */
    strcpy(bpdpath,mglpath);
    PM_backslash(bpdpath);
    strcat(bpdpath,"drivers");
    PM_setLocalBPDPath(bpdpath);
    _MGL_numDevices = _MGL_enumerateDevices();
    _MGL_registerDrivers();
    for (i = 1; i < _MGL_numDevices; i++)
        memcpy(&_MGL_deviceTable[i],&_MGL_deviceTable[0],sizeof(_MGL_deviceTable[0]));

    /* Initialise the OS specific internals of the MGL */
    strcpy(_MGL_path, mglpath);
    _MGL_initInternal();

    /* Initialise all internal modules */
    if (_MGL_consoleSupport) {
        EVT_init(_MS_moveCursor);
        _MS_init();
        consoleInited = true;
        }
    _MGL_initGLFuncs();

    /* Create the internal scratch buffer */
    if ((_MGL_buf = PM_malloc(_MGL_bufSize)) == NULL)
        MGL_fatalError("Not enough memory to allocate scratch buffer!\n");

    /* Create all region algebra memory pools */
    _MGL_createSegmentList();
    _MGL_createSpanList();
    _MGL_createRegionList();

    /* Override the default fatal error cleanup function for PM library */
    PM_setFatalErrorCleanup(MGL_exit);

    /* Indicate that we have initialised ourselves */
    inited = true;

    /* Return number of display devices found attached */
    (void)server;
    return _MGL_numDevices;
}

/****************************************************************************
DESCRIPTION:
Quick initialisation function for first time MGL users.

HEADER:
mgraph.h

PARAMETERS:
xRes            - Horizontal resolution for the display mode in pixels
yRes            - Vertical resolution for the display mode in lines
bitsPerPixel    - Color depth for the display mode
numPages        - Number of display pages to use

RETURNS:
Pointer to a fullscreen display device context.

REMARKS:
This function is intended to help first time MGL users get up and running
quickly. Using this utility function you can initialise the MGL for use
in a fullscreen display mode with a single line of code. Note that this
function will automatically bail out with an error message if any of the
initialisation code fails, so you don't need to check for error conditions
on return from this function.

A small 'Hello World' type application using the MGL might be coded as
follows:

    int main(void)
    {
        MGLDC   *dc;
        event_t evt;
        font_t  *font;

        dc = MGL_quickInit(640,480,8,1);
        if ((font = MGL_loadFont("pc8x8.fnt")) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        MGL_useFont(font);
        MGL_drawStrXY(0,0,"Hello World!");
        EVT_halt(&evt,EVT_KEYDOWN);
        MGL_exit();
    }

Note:   Once you are more familiar with the MGL, we highly recommend you
        add proper initialisation code to your program usin MGL_init and
        related functions. Unless your application is running on a dedicated
        system, it is not usually a good idea to hard code a resolution
        and color depth into the application, but rather allow the user
        to select the mode to be used via configuration files or menus.

Note:   This function

SEE ALSO:
MGL_init, MGL_exit
****************************************************************************/
MGLDC * MGLAPI MGL_quickInit(
    int xRes,
    int yRes,
    int bitsPerPixel,
    int numPages)
{
    MGLDC   *dc;
    int     mode;

    if (MGL_init(".",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((mode = MGL_findMode(xRes,yRes,bitsPerPixel)) == -1) {
        /* If we are selecting 640x480x8 (the default for all examples) and
         * it is not available, select the first mode in the list. We do
         * that so that licensed drivers will work properly.
         */
        if (xRes == 640 && yRes == 480 && bitsPerPixel == 8) {
            if ((mode = MGL_findMode(640,480,4)) == -1)
                mode = 0;
            }
        else
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    if ((dc = MGL_createDisplayDC(mode,numPages,MGL_DEFAULT_REFRESH)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_makeCurrentDC(dc);
    return dc;
}

/****************************************************************************
DESCRIPTION:
Selects the specified display device as the active device.

HEADER:
mgraph.h

PARAMETERS:
device  - New display device index to make active

RETURNS:
Index of previously active device, or -1 on failure.

REMARKS:
This function is used to select one of the attached display devices as the
active display device from this point forward. This function is used to
change the active display device after a call to MGL_init, and all
subsequent calls to device initialisation functions work with the new
active device. The device numbering starts at 0 for the primary display
controller, and increments by one for each supports display controller. Ie:
the second controller is device 1 while the third controller is device 2
etc. The maximum number of display devices is defined by the compile time
constant MAX_DISPLAY_DEVICES.

You can also select a 'mixed' mode of operation by passing in the
MM_MODE_MIXED parameter, which fully enables the primary display controller
but only enables the secondary display controllers relocateable memory mapped
regions. If this mode is selected, you must have first initialised all
attached secondary controllers to a graphics display mode or this call
will fail.

Note:   Some older display controllers cannot support mixed mode as the
        VGA compatible resources cannot be disabled. These controllers will
        still work fine, however there is more overhead involved in the
        calls to MGL_makeCurrentDC as this function must switch the active
        display device every time it is called. If mixed mode is enabled,
        the MGL_makeCurrentDC function does not have any extra overhead for
        multiple display controller support.

The general function call sequence to use multiple controllers in the MGL
is as follows:

    int     numDevices,device,mode;
    MGLDC   *dc[MAX_DISPLAY_DEVICES];

    // Initialise the library
    if ((numDevices = MGL_init(".",NULL)) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    for (device = 0; device < numDevices; device++) {
        MGL_selectDisplayDevice(device);
        if ((mode = MGL_findMode(640,480,8)) == -1)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        dc[device] = MGL_createDisplayDC(mode,1,MGL_DEFAULT_REFRESH);
        if (!dc[device])
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    MGL_selectDisplayDevice(MM_MODE_MIXED);

    // Now draw something on each device
    for (device = 0; device < numDevices; device++) {
        MGL_makeCurrentDC(dc[0]);
        MGL_line(0,0,10,10);
        }

    // Now close down the MGL
    MGL_exit();

Note:   Once you have initialised the display and created a display device
        context for each of the displays in the system, you do not need
        to call this function to switch the active display from within
        your rendering code. This is handled automatically by the
        MGL_makeCurrentDC function every time you switch the active
        MGL device context.

SEE ALSO:
MGL_init, MGL_enableAllDrivers, MGL_disableDriver, MGL_availablePages,
MGL_modeResolution, MGL_findMode, MGL_addCustomMode, MGL_createDisplayDC,
MGL_exit, MGL_result
****************************************************************************/
int MGLAPI MGL_selectDisplayDevice(
    int device)
{
    int oldDevice = _MGL_cntDevice;

    /* Switch the active display device for Multi-Monitor support */
    if (_MGL_numDevices > 1 && ((device & ~MM_MODE_MIXED) < _MGL_numDevices)) {
        if (device == _MGL_cntDevice)
            return _MGL_cntDevice;
        /* If turning on mixed mode, we have to hit the hardware
         * NB: The primary device will always be set active in hardware but
         * MGL internally considers the requested device 'current'.
         */
        if ((device & MM_MODE_MIXED) && !(_MGL_cntDevice & MM_MODE_MIXED)) {
            PM_lockSNAPAccess(0, true);
            _MGL_setActiveDevice(0 | MM_MODE_MIXED);
            PM_unlockSNAPAccess(0);
            }
        _MGL_cntDevice = device;
        return oldDevice;
        }
    return -1;
}

/****************************************************************************
DESCRIPTION:
Enables all available non-OpenGL MGL device drivers for use.

HEADER:
mgraph.h

REMARKS:
This function enables all non-OpenGL specific MGL device drivers available
on the target system for use, so that they will be used as part of the
dynamic hardware detection process. This is normally the default case
when you first initialise the MGL, so you only need to call this function
if you have disabled any of the device drivers to modify the detection
process. To enable OpenGL hardware support, use the MGL_enableOpenGLDrivers
function.

SEE ALSO:
MGL_enableOpenGLDrivers, MGL_disableDriver
****************************************************************************/
void MGLAPI MGL_enableAllDrivers(void)
{
    int i;

    for (i = 0; i < DEV.numDrivers; i++) {
        driverent *entry = &DEV.driverTable[i];
        if (!entry->opengl) {
            entry->enabled = true;
            if (entry->driver) {
                /* Destroy and unload the driver if it is currently loaded */
                if (entry->data)
                    entry->driver->destroyInstance(entry->data);
                if (entry->hModOS)
                    PM_freeLibrary(entry->hModOS);
                else if (entry->hModBPD)
                    MOD_freeLibrary(entry->hModBPD);
                }
            entry->hModOS = NULL;
            entry->hModBPD = NULL;
            entry->data = NULL;
            }
        }

    /* Ensure hardware detection runs next time round */
    DEV.numModes = 0;
}

/****************************************************************************
DESCRIPTION:
Enables all available OpenGL specific MGL device drivers for use.

HEADER:
mgraph.h

REMARKS:
This function enables all OpenGL specific MGL device drivers available
on the target system for use, so that they will be used as part of the
dynamic hardware detection process. If you wish to use OpenGL hardware
acceleration in your application, you should call this function to
register all the OpenGL specific drivers. OpenGL specific drivers are
drivers that work with OS specific OpenGL hardware device drivers, and
should only be used if the application is doing OpenGL rendering on the
target OS. For non OpenGL applications, those drivers will be less
efficient than the regular 2D only drivers.

Note:   This function needs to be called for *every* device in the system,
        since each device contains a separate list of enabled device
        drivers to use.

SEE ALSO:
MGL_enableAllDrivers, MGL_disableDriver
****************************************************************************/
void MGLAPI MGL_enableOpenGLDrivers(void)
{
    int i;

    for (i = 0; i < DEV.numDrivers; i++) {
        driverent *entry = &DEV.driverTable[i];
        if (entry->opengl) {
            entry->enabled = true;
            if (entry->driver) {
                /* Destroy and unload the driver if it is currently loaded */
                if (entry->data)
                    entry->driver->destroyInstance(entry->data);
                if (entry->hModOS)
                    PM_freeLibrary(entry->hModOS);
                else if (entry->hModBPD)
                    MOD_freeLibrary(entry->hModBPD);
                }
            entry->hModOS = NULL;
            entry->hModBPD = NULL;
            entry->data = NULL;
            }
        }

    /* Ensure hardware detection runs next time round */
    DEV.numModes = 0;
}

/****************************************************************************
PARAMETERS:
name    - Name of the driver to find

RETURNS:
Address of the driver, or NULL if not found.

REMARKS:
This function finds the entry for a driver in the global static device driver
table from the name of the driver.
{secret}
****************************************************************************/
driverent *_MGL_findStaticDriver(
    const char *name)
{
    int i;

    for (i = 0; _MGL_staticDrivers[i].name[0] != 0; i++) {
        if (!strcmp(_MGL_staticDrivers[i].name,name) && _MGL_staticDrivers[i].driver)
            return &_MGL_staticDrivers[i];
        }
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Disables a driver from being used in the detection process.

HEADER:
mgraph.h

PARAMETERS:
name    - Name of the driver to register

RETURNS:
grOK on success, error code on failure.

REMARKS:
This function disables a specific device driver so that it will no longer
be used as part of the dynamic detection process for the currently
active device. By default all device drivers are enabled, and this function
allows the programmer to control the device detection process. For instance
SciTech SNAP Graphics is used in preference to DirectDraw on Windows systems. To
disable the SciTech SNAP Graphicsdriver and allow DirectDraw to be used, you
would do the following:

    MGL_disableDriver(MGL_SNAPNAME);

The names of the standard device drivers currently supported are:

Driver              - Description
MGL_VBENAME         - SciTech SNAP Graphics VGA and VESA VBE display driver
MGL_SNAPNAME        - SciTech SNAP Graphics Accelerated display driver
MGL_DDRAWNAME       - Microsoft DirectDraw accelerated display driver
MGL_OPENGLNAME      - Microsoft hardware OpenGL display driver
MGL_GLDIRECTNAME    - SciTech GLDirect hardware OpenGL display driver

SEE ALSO:
MGL_enableAllDrivers, MGL_enableAllOpenGLDrivers
****************************************************************************/
int MGLAPI MGL_disableDriver(
    const char *name)
{
    int i,found = false;

    for (i = 0; i < DEV.numDrivers; i++) {
        if (!strcmp(DEV.driverTable[i].name,name) && DEV.driverTable[i].driver) {
            found = true;
            DEV.driverTable[i].enabled = false;
            break;
            }
        }
    if (!found)
        FATALERROR(grInvalidName);
    return __MGL_result;
}

/****************************************************************************
REMARKS:
This function autodetects the presence of all the standard graphics adapters
supported by MGL. If no suitable hardware is detected, it returns false.
****************************************************************************/
static ibool _MGL_detectHardware(void)
{
    int         i,found = false;
    PM_HWND     hwndConsole = (PM_HWND)NULL;
    void        *stateBuf = NULL;

    /* Open a new fullscreen console during detection, and save it's state.
     * Note that we only need to do this for the primary graphics device, not
     * any secondary controllers that we will be initialising.
     */
    if (_MGL_consoleSupport && (GET_CURRENT_DEVICE() == 0)) {
        /* Running in a window on OS/2 desktop will corrupt the display */
        if (PM_runningInAWindow()) {
            FATALERROR(grNeedsFullScreen);
            return false;
            }
        hwndConsole = PM_openConsole((PM_HWND)NULL,GET_CURRENT_DEVICE(),640,480,8,true);
        if ((stateBuf = PM_malloc(PM_getConsoleStateSize())) == NULL) {
            FATALERROR(grNoMem);
            return false;
            }
        PM_saveConsoleState(stateBuf,hwndConsole);
        }

    /* Set the global OS type flag */
    ___drv_os_type = PM_getOSType();

    /* Initialise the mode table to empty */
    for (i = 0; i < MAX_DRIVER_MODES; i++) {
        DEV.availableModes[i].driver = 0xFF;
        DEV.availableModes[i].xRes = 0;
        DEV.availableModes[i].yRes = 0;
        DEV.availableModes[i].bits = 0;
        DEV.availableModes[i].pages = 0;
        DEV.availableModes[i].flags = 0;
        }
    DEV.numModes = 0;

    /* Call the driver detection code for all enabled drivers in order */
    __MGL_result = grNotDetected;
    for (i = 0; i < DEV.numDrivers; i++) {
        driverent *entry = &DEV.driverTable[i];
        if (entry->enabled) {
            if (!entry->data) {
                /* Create an instance of the driver */
                if ((entry->data = entry->driver->createInstance()) == NULL) {
                    FATALERROR(grLoadMem);
                    found = false;
                    break;
                    }
                }
            if (entry->driver->detect(entry->data,i,&DEV.numModes,DEV.availableModes)) {
                __MGL_result = grOK;
                found = true;
                }
            else {
                /* Destroy the instance of the driver if it was not detected */
                entry->driver->destroyInstance(entry->data);
                entry->data = NULL;
                if (entry->hModOS)
                    PM_freeLibrary(entry->hModOS);
                else if (entry->hModBPD)
                    MOD_freeLibrary(entry->hModBPD);
                }
            }
        }

    /* Restore console, cleanup and exit */
    if (hwndConsole) {
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_free(stateBuf);
        PM_closeConsole(hwndConsole);
        }
    if (__MGL_result != grOK)
        FATALERROR(__MGL_result);
    return found;
}

/****************************************************************************
DESCRIPTION:
Determine the number of available video pages for a specific graphics mode.

PARAMETERS:
mode    - MGL mode number to query

HEADER:
mgraph.h

RETURNS:
Number of available display pages for mode, -1 for invalid mode number.

REMARKS:
Returns the number of pages of physical display pages available for a
specific MGL graphics mode. You may call this routine before creating
a display device context with MGL_createDisplayDC, but you cannot call this
function until after you have called MGL_init. This function allows you to
ignore display modes that do not have the required number of hardware
display pages that your application requires.

Note:   If the hardware has not been detected when this call is made,
        the MGL will automatically detect the installed hardware the first
        time this function is called.

SEE ALSO:
MGL_init, MGL_modeResolution, MGL_findMode, MGL_addCustomMode,
MGL_createDisplayDC
****************************************************************************/
int MGLAPI MGL_availablePages(
    int mode)
{
    if (DEV.numModes == 0) {
        if (!_MGL_detectHardware())
            return -1;
        }
    if (mode < 0 || mode >= DEV.numModes) {
        __MGL_result = grInvalidMode;
        return -1;
        }
    __MGL_result = grOK;
    return DEV.availableModes[mode].pages;
}

/****************************************************************************
DESCRIPTION:
Returns the resolution and pixel depth of a specific graphics mode.

HEADER:
mgraph.h

PARAMETERS:
mode            - graphics mode number to get resolution for
xRes            - Place to store the x resolution
yRes            - Place to store the y resolution
bitsPerPixel    - Place to store the pixel depth

RETURNS:
True on success, false for an invalid graphics mode number.

REMARKS:
This function returns the resolution and color depth of the specified MGL
graphics mode. This function is useful for displaying the list of available
modes to the user, or to search for specific graphics modes for use by an
application program depending on the desired resolution or color depth.

Note:   If the hardware has not been detected when this call is made,
        the MGL will automatically detect the installed hardware the first
        time this function is called.

SEE ALSO:
MGL_init, MGL_availablePages, MGL_findMode, MGL_addCustomMode,
MGL_createDisplayDC
****************************************************************************/
ibool MGLAPI MGL_modeResolution(
    int mode,
    int *xRes,
    int *yRes,
    int *bitsPerPixel)
{
    if (DEV.numModes == 0) {
        if (!_MGL_detectHardware())
            return false;
        }
    if (mode < 0 || mode >= DEV.numModes) {
        __MGL_result = grInvalidMode;
        return false;
        }
    __MGL_result = grOK;
    *xRes = DEV.availableModes[mode].xRes;
    *yRes = DEV.availableModes[mode].yRes;
    *bitsPerPixel = DEV.availableModes[mode].bits;
    return true;
}

/****************************************************************************
DESCRIPTION:
Returns the mode flags for the specific graphics mode.

HEADER:
mgraph.h

PARAMETERS:
mode    - graphics mode number to get flags for

RETURNS:
Flags for the specific graphics mode, or 0 for invalid mode.

REMARKS:
This function returns mode flags for the specified mode. The mode flags
are available after calling MGL_init, and provides information about the
hardware capabilities of the graphics mode, such as whether it supports 2D
or 3D acceleration, video acceleration, refresh rate control and hardware
stereo support. You can use these flags to make choices about the graphics
mode to use for your application prior to initialising a specific graphics
mode.

Specific mode flags are enumerated in MGL_modeFlagsType.

Note:   If the hardware has not been detected when this call is made,
        the MGL will automatically detect the installed hardware the first
        time this function is called.

SEE ALSO:
MGL_init, MGL_availablePages, MGL_modeResolution, MGL_findMode,
MGL_addCustomMode, MGL_createDisplayDC
****************************************************************************/
ulong MGLAPI MGL_modeFlags(
    int mode)
{
    if (DEV.numModes == 0) {
        if (!_MGL_detectHardware())
            return 0;
        }
    if (mode < 0 || mode >= DEV.numModes) {
        __MGL_result = grInvalidMode;
        return 0;
        }
    __MGL_result = grOK;
    return DEV.availableModes[mode].flags;
}

/****************************************************************************
DESCRIPTION:
Finds the number of a mode given the resolution and color depth.

HEADER:
mgraph.h

PARAMETERS:
xRes            - Horizontal resolution for the display mode in pixels
yRes            - Vertical resolution for the display mode in lines
bitsPerPixel    - Color depth for the display mode

RETURNS:
MGL mode number for the mode, or -1 if not found.

REMARKS:
This function searches the list of available display modes in the MGL,
looking for one that matches the specified resolution and color depth. This
function is useful if your application always runs in a specific resolution
and color depth, allowing you to quickly initialise the MGL without needing
to write your own code to search the list of available modes.

Note:   If the hardware has not been detected when this call is made,
        the MGL will automatically detect the installed hardware the first
        time this function is called.

SEE ALSO:
MGL_init, MGL_availablePages, MGL_modeResolution, MGL_modeFlags,
MGL_createDisplayDC
****************************************************************************/
int MGLAPI MGL_findMode(
    int xRes,
    int yRes,
    int bitsPerPixel)
{
    int     i;
    modeent *m = &DEV.availableModes[0];

    if (DEV.numModes == 0) {
        if (!_MGL_detectHardware())
            return -1;
        }
    __MGL_result = grInvalidMode;
    for (i = 0; i < DEV.numModes; i++,m++) {
        if (m->xRes == xRes && m->yRes == yRes && m->bits == bitsPerPixel) {
            __MGL_result = grOK;
            return i;
            }
        }
    return -1;
}

/****************************************************************************
REMARKS:
Destroys all device context for the currently active display device.
****************************************************************************/
static void _MGL_destroyDevice(void)
{
    int i;

    /* Uncache the current DC */
    MGL_makeCurrentDC(NULL);

    /* Destroy all active overlay contexts */
    while (DEV.overDCList)
        _MGL_destroyOverlayDC(_LST_first(DEV.overDCList));

    /* Destroy all active offscreen contexts */
    while (DEV.offDCList)
        _MGL_destroyOffscreenDC(_LST_first(DEV.offDCList));

    /* Destroy all active display contexts, which restores text mode */
    while (DEV.dispDCList)
        _MGL_destroyDisplayDC(_LST_first(DEV.dispDCList));

    /* Destroy all instances of driver to force it to unload. */
    for (i = 0; i < DEV.numDrivers; i++) {
        driverent *entry = &DEV.driverTable[i];
        if (entry->enabled && entry->driver) {
            if (entry->data)
                entry->driver->destroyInstance(entry->data);
            if (entry->hModOS)
                PM_freeLibrary(entry->hModOS);
            else if (entry->hModBPD)
                MOD_freeLibrary(entry->hModBPD);
            entry->hModOS = NULL;
            entry->hModBPD = NULL;
            entry->data = NULL;
            }
        }

    /* Disable any cached memory (which has been freed) */
    DEV.gaPtr = NULL;
    DEV.gaInfo = NULL;

    /* Unregister all drivers for this device */
    MGL_enableAllDrivers();
    DEV.numDrivers = 0;
}

/****************************************************************************
DESCRIPTION:
Adds a new custom display mode to the mode list

HEADER:
mgraph.h

PARAMETERS:
xRes            - Horizontal resolution for the display mode in pixels
yRes            - Vertical resolution for the display mode in lines
bitsPerPixel    - Color depth for the display mode

RETURNS:
True on success, false on failure.

REMARKS:
This function attempts to use the SciTech SNAP Graphics API functions to create
a new custom display mode. If the custom display mode creation succeeds,
this function returns true. If it fails it returns false. The most
common scenario for the failure to create a custom display mode, is
that the requested mode is not compatible with the underlying hardware.
In this case you may want to search for a compatible mode by stepping the
X resolution in 8 pixel increments until you find a suitable mode that
is larger than what you want.

Note:   Some display hardware will require that the X resolution be aligned
        on a 16 pixel boundary, so might fail if you try to set a mode
        that where the X resolution is not divisible by 16. Hence we
        advise that when you create new modes you try to use modes that
        are divisible by 16 to ensure they will work on all available
        hardware devices.

Note:   If the hardware has not been detected when this call is made,
        the MGL will automatically detect the installed hardware the first
        time this function is called.

SEE ALSO:
MGL_init, MGL_availablePages, MGL_modeResolution, MGL_modeFlags,
MGL_createDisplayDC, MGL_findMode
****************************************************************************/
ibool MGLAPI MGL_addCustomMode(
    int xRes,
    int yRes,
    int bitsPerPixel)
{
    /* First check if the mode already exists */
    if (MGL_findMode(xRes,yRes,bitsPerPixel) != -1)
        return true;

    /* Call SNAP Graphics to add the new mode */
    __MGL_result = grInvalidMode;
    if (!GA_addMode(DEV.gaPtr,xRes,yRes,bitsPerPixel,false))
        return false;

    /* Make sure we do a hardware re-detect so we pick up the new mode */
    _MGL_destroyDevice();
    if (!_MGL_detectHardware())
        return false;
    __MGL_result = grOK;
    return true;
}

/****************************************************************************
DESCRIPTION:
Closes down the MGL.

HEADER:
mgraph.h

REMARKS:
This function closes down the MGL, deallocating any memory allocated for use
by the MGL, and restoring the system back into the original display mode that
was active before the MGL was started. This routine also properly removes
all interrupt handlers and other system services that MGL hooked when it
was initialized.

You must call this routine before you exit your application, to ensure that
the system is properly terminated.

SEE ALSO:
MGL_init
****************************************************************************/
void MGLAPI MGL_exit(void)
{
    int i;

    if (inited) {
        /* Destroy fonts cache */
        _MGL_destroyFontEnumCache();

        /* Uncache the current DC */
        MGL_makeCurrentDC(NULL);

        /* Kill mouse and event handling */
        if (consoleInited) {
            MS_hide();
            EVT_exit();
            consoleInited = false;
            }

        /* Destroy all display DC and offscreen DC's for all devices */
        for (i = _MGL_numDevices-1; i > 0; i--) {
            MGL_selectDisplayDevice(i | MM_MODE_MIXED);
            _MGL_destroyDevice();
            }

        /* Make sure only the primary device is enabled and destroy it too; */
        /* This may restore the VGA console and that requires only the */
        /* primary device to be enabled */
        MGL_selectDisplayDevice(0);
        _MGL_setActiveDevice(0);
        _MGL_destroyDevice();

        /* Destroy all active windowed device contexts */
        while (_MGL_winDCList)
            _MGL_destroyWindowedDC(_LST_first(_MGL_winDCList));

        /* Destroy all active memory device contexts */
        while (_MGL_memDCList)
            _MGL_destroyMemoryDC(_LST_first(_MGL_memDCList));

        /* Destory all region algebra memory pools */
        _PM_freeSegmentList();
        _PM_freeSpanList();
        _MGL_freeRegionList();

        /* Destroy the memory buffer */
        if (_MGL_buf) {
            PM_free(_MGL_buf);
            _MGL_buf = NULL;
            }

        /* Perform any OS specific exit code */
        _MGL_exitInternal();

        /* Indicate that we have not exited */
        inited = false;
        }
}

/****************************************************************************
DESCRIPTION:
Sets the size of the internal MGL buffer.

HEADER:
mgraph.h

PARAMETERS:
size    - New size of the internal MGL buffer

REMARKS:
This function sets the size of the internal MGL scratch buffer, which the
MGL uses for local scratch space in various places. The default size of this
buffer is 32Kb, which is adequate for most needs. If however you attempt to
render some primitives and MGL runs out of local storage space you will need
to increase the size of this internal buffer.

If you are running on an embedded system and need to trim the amount of
memory used by the MGL, you may want to use this function to set a smaller
buffer that is suitable for your application needs to decrease the memory
footprint used by the MGL at runtime.

Note that this routine must be called before MGL_init is called for the
first time.

SEE ALSO:
MGL_init, MGL_setMaxScanLineWidth
****************************************************************************/
void MGLAPI MGL_setBufSize(
    unsigned size)
{
    _MGL_bufSize = size;
}

/****************************************************************************
DESCRIPTION:
Declare a fatal error and exit gracefully.

HEADER:
mgraph.h

PARAMETERS:
msg - Message to display
... - Variable argument list to display

REMARKS:
A fatal internal error has occurred, so we shutdown the graphics systems,
display the error message and quit. You should call this function to
display your own internal fatal errors. You can use this function like
printf(), allowing you to format the results for the output message, so
long as the entire message string is less than 1024 bytes in length.
****************************************************************************/
void MGLAPI MGL_fatalError(
    const char *msg,
    ...)
{
    va_list argptr;
    char    buf[1024];

    va_start(argptr, msg);
    vsprintf(buf, msg, argptr);
    va_end(argptr);
    PM_fatalError(buf);
}

/****************************************************************************
REMARKS:
Internal fatal error handler if the scratch buffer is too small
{secret}
****************************************************************************/
void _MGL_scratchTooSmall(void)
{
    MGL_fatalError("Scratch buffer too small for rendering operation!\n");
}

/****************************************************************************
DESCRIPTION:
Returns result code of the last graphics operation.

HEADER:
mgraph.h

RETURNS:
Result code of the last graphics operation

REMARKS:
This function returns the result code of the last graphics operation. The
internal result code is reset back to grOK on return from this routine, so
you should only call the routine once after the graphics operation. Error
codes returned by this function are enumerated in MGL_errorType.

SEE ALSO:
MGL_setResult, MGL_errorMsg
****************************************************************************/
int MGLAPI MGL_result(void)
{
    int result;

    result = __MGL_result;
    __MGL_result = grOK;
    return result;
}

/****************************************************************************
DESCRIPTION:
Sets the internal MGL result flag.

HEADER:
mgraph.h

PARAMETERS:
result  - New internal result flag

REMARKS:
Sets the internal MGL result flag to the specified value. This routine is
primarily for extension libraries, but you can use it to add your own
extension functions to the MGL that will return result codes in the same
manner as the MGL itself;

SEE ALSO:
MGL_result, MGL_errorMsg
****************************************************************************/
void MGLAPI MGL_setResult(
    int result)
{
    __MGL_result = result;
}

/****************************************************************************
PARAMETERS:
enable  - Flag to enable and disable linear blits

REMARKS:
This internal function disables linear blit operations in the SNAP Graphics
drivers, forcing the MGL to use rectangular blits. This is an internal
testing function to allow the proper testing of both linear and rectangular
blits for conformance testing purposes. It has little if any use for
real world applications (linear blits are always much better than
rectangular blits for offscreen memory buffers!).
{secret}
****************************************************************************/
void MGLAPI MGL_useLinearBlit(
    ibool enable)
{
    __MGL_useLinearBlits = enable;
}

/****************************************************************************
DESCRIPTION:
Returns a string describing an error condition code.

HEADER:
mgraph.h

PARAMETERS:
err - Error code to obtain string for

RETURNS:
Pointer to string describing the error condition.

REMARKS:
Returns a pointer to a string describing a specified error code. You can use
this to convert the error codes from a numerical id return by MGL_result to
a string which you can display for the users of your programs.

SEE ALSO:
MGL_result
****************************************************************************/
const char * MGLAPI MGL_errorMsg(
    int err)
{
    int i;
    static struct {
        int     error;
        char    *msg;
    } errors[] = {
        { grOK                ,"No error" },
        { grNoInit            ,"Graphics driver has not been installed" },
        { grNotDetected       ,"Graphics hardware not detected" },
        { grDriverNotFound    ,"Couldn't find graphics driver. Make sure you have renamed graphics.vbe to\n"
                               "graphics.bpd, or have installed SciTech SNAP Graphics." },
        { grBadDriver         ,"Graphics driver file corrupted" },
        { grLoadMem           ,"Not enough memory to load graphics driver" },
        { grInvalidMode       ,"Invalid mode for selected driver" },
        { grError             ,"General graphics error" },
        { grInvalidName       ,"Invalid graphics driver name" },
        { grNoMem             ,"Not enough memory to perform operation" },
        { grNoModeSupport     ,"Selected graphics mode not supported" },
        { grInvalidFont       ,"Invalid font data" },
        { grBadFontFile       ,"Font file corrupted" },
        { grFontNotFound      ,"Couldn't find font file" },
        { grOldDriver         ,"Incorrect version for driver file" },
        { grInvalidDevice     ,"Invalid device type for operation" },
        { grInvalidDC         ,"Invalid device context" },
        { grInvalidCursor     ,"Invalid cursor file" },
        { grCursorNotFound    ,"Cursor file was not found" },
        { grInvalidIcon       ,"Invalid icon file" },
        { grIconNotFound      ,"Icon file was not found" },
        { grInvalidBitmap     ,"Invalid bitmap file" },
        { grBitmapNotFound    ,"Bitmap file was not found" },
        { grNewFontFile       ,"Only Windows 2.x font files supported" },
        { grNoDoubleBuff      ,"Double buffering is not available" },
        { grNoHardwareBlt     ,"OffscreenDC's require hardware bitBlt support" },
        { grNoOffscreenMem    ,"No available Offscreen display memory" },
        { grInvalidPF         ,"Invalid pixel format for memory DC" },
        { grInvalidBuffer     ,"Invalid offscreen buffer" },
        { grNoDisplayDC       ,"Display DC has not been created" },
        { grFailLoadRef2d     ,"2D reference rasteriser driver failed to load" },
        { grErrorBPD          ,"Graphics error in external BPD module" },
        { grInvalidLicense    ,"Invalid SciTech SNAP Graphics License. Make sure you have renamed graphics.vbe\n"
                               "to graphics.bpd, or have installed SciTech SNAP Graphics." },
        { grNeedsFullScreen   ,"This program must be run in a fullscreen session." },
        };
    for (i = 0; i < (sizeof(errors)/sizeof((errors)[0])); i++) {
        if (errors[i].error == err)
            return errors[i].msg;
        }
    return "Invalid error number";
}

/****************************************************************************
DESCRIPTION:
Get a the name of the device driver used for particular graphics mode.

HEADER:
mgraph.h

PARAMETERS:
mode    - graphics mode number

RETURNS:
Pointer to the name of device driver serving this mode

REMARKS:
This function returns the name of the device driver that is currently being
used to support the specified graphics mode. The MGL provides a number of
device drivers for supporting the fullscreen graphics mode resolutions
depending on the capabilities of the underlying hardware. This function
allows you to determine which driver is currently being used to support
each mode.
****************************************************************************/
const char * MGLAPI MGL_modeDriverName(
    int mode)
{
    if (0 <= mode && mode < DEV.numModes) {
        int driver = DEV.availableModes[mode].driver;
        if (driver != 0xFF && DEV.driverTable[driver].driver)
            return DEV.driverTable[driver].driver->realname;
        else
            return "Invalid graphics mode";
        }
    else
        return "Invalid graphics mode";
}

/****************************************************************************
DESCRIPTION:
Block until a specific time has elapsed since the last call

HEADER:
mgraph.h

PARAMETERS:
milliseconds    - Number of milliseconds for delay

REMARKS:
This function will block the calling thread or process until the specified
number of milliseconds have passed since the /last/ call to this function.
The first time this function is called, it will return immediately. On
subsquent calls it will block until the specified time has elapsed, or it
will return immediately if the time has already elapsed.

This function is useful to provide constant time functionality in a
program, such as a frame rate limiter for graphics applications etc.
****************************************************************************/
void MGLAPI MGL_lockToFrameRate(
    ulong milliseconds)
{
    PM_blockUntilTimeout(milliseconds);
}
