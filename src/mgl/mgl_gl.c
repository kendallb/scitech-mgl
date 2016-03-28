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
* Description:  Platform independant module to support the OpenGL API in
*               the MGL. We compile in platform dependant code to handle
*               stuff specific to that OS platform.
*
****************************************************************************/

#include "mgldd.h"
#include "GL/gl.h"
#include "GL/glu.h"

/*--------------------------- Global Variables ----------------------------*/

static int          _MGL_whichOpenGL = MGL_GL_AUTO;
GA_glFuncs          _VARAPI _GA_glFuncs;
GA_gluFuncs         _VARAPI _GA_gluFuncs;

/*------------------ Platform dependent Implementation --------------------*/

#if     defined(__SMX32__)
#include "mglsmx/opengl.c"
#elif   defined(__RTTARGET__)
#include "mglrtt/opengl.c"
#elif   defined(__REALDOS__)
#include "mgldos/opengl.c"
#elif   defined(__WINDOWS32__)
#include "mglwin/opengl.c"
#elif   defined(__OS2__)
#include "mglos2/opengl.c"
#elif   defined(__LINUX__)
#include "mgllinux/opengl.c"
#elif   defined(__QNX__)
#include "mglqnx/opengl.c"
#else
#error  OpenGL module not ported to this platform yet!
#endif

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
True if OpenGL is loaded, false if not.

REMARKS:
Attempts to dynamically load the OpenGL implementation DLL and set our
vectors to point to the corresponding OpenGL functions.
{secret}
****************************************************************************/
ibool _MGL_glLoadOpenGL(
    MGLDC *dc)
{
    if (!DEV.glLoaded) {
        /* Determine which version of OpenGL to load */
        DEV.cntOpenGL = (GET_CURRENT_DEVICE() == 0) ? _MGL_whichOpenGL : MGL_GL_SNAP;
        if (DEV.cntOpenGL == MGL_GL_AUTO) {
            /* See if system OpenGL is accelerated */
            if (__MGL_glHaveHWOpenGL())
                DEV.cntOpenGL = MGL_GL_DEFAULT;
            else
                DEV.cntOpenGL = MGL_GL_SNAP;
            }
        if (DEV.cntOpenGL == MGL_GL_DEFAULT) {
            /* If we can't find the system OpenGL, use SNAP */
            if (!__MGL_glFindDefaultOpenGL())
                DEV.cntOpenGL = MGL_GL_SNAP;
            }

        /* Load the OpenGL libraries */
        if (DEV.cntOpenGL == MGL_GL_DEFAULT) {
            /* Load the system OpenGL */
            __MGL_loadDefaultOpenGL();
            }
        else {
            /* Load SNAP OpenGL implementation */
            if ((DEV.glPtr = GA_glLoadDriver(DEV.gaPtr)) == NULL)
                return false;
            DEV.glInit.dwSize = sizeof(DEV.glInit);
            if (!GA_glQueryFunctions(DEV.glPtr,GA_GET_GLINITFUNCS,&DEV.glInit))
                return false;
            if (!DEV.glInit.Init(DEV.ref2d))
                return false;
            _GA_glFuncs.dwSize = sizeof(_GA_glFuncs);
            if (!GA_glQueryFunctions(DEV.glPtr,GA_GET_GLFUNCS,&_GA_glFuncs))
                return false;
            _GA_gluFuncs.dwSize = sizeof(_GA_gluFuncs);
            if (!GA_glQueryFunctions(DEV.glPtr,GA_GET_GLUFUNCS,&_GA_gluFuncs))
                return false;
            }
        _MGL_glDevice = dc->v->d.device;
        }
    return DEV.glLoaded = true;
}

/****************************************************************************
REMARKS:
Unloads the currently loaded OpenGL library.
{secret}
****************************************************************************/
void _MGL_glUnloadOpenGL(void)
{
    __MGL_glUnloadOpenGL();
    if (DEV.glPtr) {
        GA_glUnloadDriver(DEV.glPtr);
        DEV.glPtr = NULL;
        }
    DEV.glLoaded = false;
}

/****************************************************************************
DESCRIPTION:
Checks for OpenGL hardware acceleration.

HEADER:
mgraph.h

RETURNS:
True if OpenGL is hardware accelerated on the target platform.

REMARKS:
This function will load the system OpenGL libraries and attempt to determine
if hardware acceleration is available or not. On the Win32 platform, this
involves loading the Microsoft OpenGL libraries and checking to see if
OpenGL is accelerated via an ICD or MCD hardware device driver. On other
platforms we check to see if acceleration is available via the system
OpenGL drivers or via the SNAP OpenGL compatible implementation.
****************************************************************************/
ibool MGLAPI MGL_glHaveHWOpenGL(void)
{
    if (__MGL_glHaveHWOpenGL())
        return true;
#if 0   /* SNAP OpenGL 3D acceleration not supported yet */
    if (DEV.gaPtr->Attributes & gaHaveAccel3D)
        return true;
#endif
    return false;
}

/****************************************************************************
DESCRIPTION:
Sets the OpenGL implementation type to use.

HEADER:
mgraph.h

PARAMETERS:
type    - New OpenGL type to set (MGL_glOpenGLType)

REMARKS:
This function sets the current OpenGL implementation type to use
according to the passed in type parameter (MGL_glOpenGLType enumeration).
In the AUTO mode we automatically determine which version of OpenGL to
use depending on the target runtime system. Unless there is hardware
acceleration available we choose the SNAP OpenGL compatible library, but
if hardware acceleration is present we use the system provided OpenGL
implementation.
****************************************************************************/
void MGLAPI MGL_glSetOpenGLType(
    int type)
{
    _MGL_glUnloadOpenGL();
    _MGL_whichOpenGL = type;
}

/****************************************************************************
PARAMETERS:
visual  - Structure containing OpenGL visual information
snapVis - SNAP visual to fill in

REMARKS:
This function examines the visual passed in and modifies the values to
best match the capabilities of the underlying OpenGL implementation.
If a requested capability is not supported, the structure will be modified
for the capabilities that the OpenGL implementation does support (ie: lowering
the depth buffer size to 16 bits etc).

SEE ALSO:
MGL_glSetVisual, MGL_glCreateContext
****************************************************************************/
static void _MGL_translateVisualToSNAP(
    MGLVisual *visual,
    GA_glVisual *snapVis)
{
    snapVis->dwSize         = sizeof(*snapVis);
    snapVis->DoubleBuffer   = visual->db_flag;
    snapVis->Stereo         = visual->stereo_flag;
    snapVis->ColorIndex     = !visual->rgb_flag;
    snapVis->DepthBits      = visual->depth_size;
    snapVis->AlphaBits      = visual->alpha_size;
    snapVis->StencilBits    = visual->stencil_size;
    snapVis->AccumBits      = visual->accum_size;
    snapVis->SoftwareOnly   = visual->software_only;
}

/****************************************************************************
PARAMETERS:
visual  - Structure containing OpenGL visual information
snapVis - SNAP visual to fill in

REMARKS:
This function examines the visual passed in and modifies the values to
best match the capabilities of the underlying OpenGL implementation.
If a requested capability is not supported, the structure will be modified
for the capabilities that the OpenGL implementation does support (ie: lowering
the depth buffer size to 16 bits etc).

SEE ALSO:
MGL_glSetVisual, MGL_glCreateContext
****************************************************************************/
static void _MGL_translateVisualFromSNAP(
    GA_glVisual *snapVis,
    MGLVisual *visual)
{
    visual->db_flag         = snapVis->DoubleBuffer;
    visual->stereo_flag     = snapVis->Stereo;
    visual->rgb_flag        = !snapVis->ColorIndex;
    visual->depth_size      = snapVis->DepthBits;
    visual->alpha_size      = snapVis->AlphaBits;
    visual->stencil_size    = snapVis->StencilBits;
    visual->accum_size      = snapVis->AccumBits;
    visual->software_only   = snapVis->SoftwareOnly;
}

/****************************************************************************
DESCRIPTION:
Choose an OpenGL visual to best match the passed in visual.

PARAMETERS:
dc      - MGL device context
visual  - Structure containing OpenGL visual information

REMARKS:
This function examines the visual passed in and modifies the values to
best match the capabilities of the underlying OpenGL implementation.
If a requested capability is not supported, the structure will be modified
for the capabilities that the OpenGL implementation does support (ie: lowering
the depth buffer size to 16 bits etc).

SEE ALSO:
MGL_glSetVisual, MGL_glCreateContext
****************************************************************************/
void MGLAPI MGL_glChooseVisual(
    MGLDC *dc,
    MGLVisual *visual)
{
    GA_glVisual snapVis;

    if (!_MGL_glLoadOpenGL(dc))
        MGL_fatalError("Unable to load OpenGL");
    if (DEV.cntOpenGL == MGL_GL_SNAP) {
        _MGL_translateVisualToSNAP(visual,&snapVis);
        if (dc->deviceType == MGL_DISPLAY_DEVICE)
            DEV.glInit.ChooseVisual(dc->r.GetPrimaryBuffer(),&snapVis);
        else if (dc->deviceType == MGL_OFFSCREEN_DEVICE)
            DEV.glInit.ChooseVisual(TO_BUF(dc->offBuf),&snapVis);
        else if (dc->deviceType == MGL_MEMORY_DEVICE)
            DEV.glInit.ChooseVisual(NULL,&snapVis);
        _MGL_translateVisualFromSNAP(&snapVis,visual);
        }
    else {
        if (!__MGL_glChooseVisual(dc,visual))
            memset(visual,0,sizeof(*visual));
        }
}

/****************************************************************************
DESCRIPTION:
Attempts to set the specified OpenGL visual for the MGL device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - MGL device context
visual  - Structure containing OpenGL visual information

RETURNS:
True on success, false if visual not supported by OpenGL implementation.

REMARKS:
This function sets the passed in OpenGL visual for the MGL device context
and makes it the visual that will be used in the call to MGL_glCreateContext.
Note that this function may fail if the OpenGL visual requested is invalid,
and you should call MGL_glChooseVisual first to find a visual that best
matches the underlying OpenGL implementation. For instance if the OpenGL
implementation only supports a 16-bit z-buffer, yet you request a 32-bit
z-buffer this function will fail.

The OpenGL visual is used to define the visual capabilities of the OpenGL
rendering context that will be created with the MGL_glCreateContext function,
and includes information such as whether the mode should be an RGB mode or
color index mode, whether it should be single buffered or double buffered,
whether a depth buffer (zbuffer) should be used and how many bits it should
be etc.

Note:   You can only set the visual for a context once, and it is an
        error to call MGL_glSetVisual more than once for an MGL device
        context, and you also cannot change a visual once you have set it
        without first destroying the OpenGL rendering context.

SEE ALSO:
MGL_glChooseVisual, MGL_glCreateContext
****************************************************************************/
ibool MGLAPI MGL_glSetVisual(
    MGLDC *dc,
    MGLVisual *visual)
{
    GA_glVisual snapVis;

    if (MGL_isMemoryDC(dc) && visual->db_flag)
        return false;
    if (!_MGL_glLoadOpenGL(dc))
        MGL_fatalError("Unable to load OpenGL");
    if (DEV.cntOpenGL == MGL_GL_SNAP) {
        MGL_makeCurrentDC(NULL);
        _MGL_translateVisualToSNAP(visual,&snapVis);
        if (dc->deviceType == MGL_DISPLAY_DEVICE) {
            if (!DEV.glInit.CheckVisual(dc->r.GetPrimaryBuffer(),&snapVis))
                return false;
            }
        else if (dc->deviceType == MGL_OFFSCREEN_DEVICE) {
            if (!DEV.glInit.CheckVisual(TO_BUF(dc->offBuf),&snapVis))
                return false;
            }
        else if (dc->deviceType == MGL_MEMORY_DEVICE) {
            if (!DEV.glInit.CheckVisual(NULL,&snapVis))
                return false;
            }
        dc->cntVis = *visual;
        }
    else {
        if (!__MGL_glSetVisual(dc,visual))
            return false;
        }
    return true;
}

/****************************************************************************
DESCRIPTION:
Returns the current OpenGL visual for the device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - MGL device context
visual  - Place to store the OpenGL visual information

REMARKS:
This function returns the current OpenGL visual that has been set for the
MGL device context.

SEE ALSO:
MGL_glSetVisual, MGL_glCreateContext
****************************************************************************/
void MGLAPI MGL_glGetVisual(
    MGLDC *dc,
    MGLVisual *visual)
{
    *visual = dc->cntVis;
}

/****************************************************************************
DESCRIPTION:
Creates the OpenGL rendering context for the MGL device context.

HEADER:
mgraph.h

PARAMETERS:
dc      - MGL device context to enable OpenGL API for
flags   - Flags to force type of OpenGL library used

RETURNS:
True if context was created successfully, false otherwise

REMARKS:
This function creates an OpenGL rendering context for the MGL device context,
and enables support for OpenGL functions for the MGL device context. To
provide a quick and easy way to get the MGL up and running with OpenGL
support, you can pass in some simple flags that let the MGL know what
OpenGL features you want enabled in the OpenGL visual for the MGL device
context, by combining values in the MGL_glContextType enumeration. For
instance if you wanted to start OpenGL with support for an RGB, double
buffered and z-buffered mode you would pass in:

 MGL_GL_RGB | MGL_GL_DOUBLE | MGL_GL_DEPTH

If you wish to have complete control over the OpenGL visual that is used
for the MGL device context, you can call MGL_glChooseVisual and MGL_glSetVisual
before calling MGL_glCreateContext, and then pass in a value of MGL_GL_VISUAL
to tell the MGL to use the visual you have already set for the device context
instead of trying to create one from the passed in flags.

Note:   Once you have created an OpenGL rendering context for the MGL
        device context, you must first call MGL_glMakeCurrent to make that
        specific MGL device context the current OpenGL rendering context
        before you call any core OpenGL API functions.

Note:   You /must/ call this function first before you attemp to make any
        calls to the core OpenGL API functions (such as glVertex3f etc),
        as the MGL will not have initialised its internal dispatch tables
        until this call is made.

SEE ALSO:
MGL_glChooseVisual, MGL_glSetVisual, MGL_glMakeCurrent
****************************************************************************/
ibool MGLAPI MGL_glCreateContext(
    MGLDC *dc,
    int flags)
{
    MGLVisual   visual = dc->cntVis;
    GA_glVisual snapVis;

    /* Optionally select the visual for the programmer based on the
     * passed in flags for easy setup and initialization.
     */
    if (!(flags & MGL_GL_VISUAL)) {
        memset(&visual,0,sizeof(visual));
        visual.rgb_flag = (flags & MGL_GL_INDEX) == 0;
        visual.db_flag = (flags & MGL_GL_DOUBLE) != 0;
        visual.stereo_flag = (flags & MGL_GL_STEREO) != 0;
        if (flags & MGL_GL_DEPTH)
            visual.depth_size = 16;
        if (flags & MGL_GL_ALPHA)
            visual.alpha_size = 8;
        if (flags & MGL_GL_STENCIL)
            visual.stencil_size = 8;
        if (flags & MGL_GL_ACCUM)
            visual.accum_size = 16;
        if (flags & MGL_GL_SOFTWARE)
            visual.software_only = true;
        MGL_glChooseVisual(dc,&visual);
        if (!MGL_glSetVisual(dc,&visual))
            return false;
        }

    /* Now create the OpenGL rendering context */
    if (DEV.cntOpenGL == MGL_GL_SNAP) {
        _MGL_translateVisualToSNAP(&dc->cntVis,&snapVis);
        if (dc->deviceType == MGL_DISPLAY_DEVICE) {
            if ((dc->rc = DEV.glInit.CreateContext(dc->r.GetPrimaryBuffer(),&snapVis,NULL)) == NULL)
                return false;
            }
        else if (dc->deviceType == MGL_OFFSCREEN_DEVICE) {
            if ((dc->rc = DEV.glInit.CreateContext(TO_BUF(dc->offBuf),&snapVis,NULL)) == NULL)
                return false;
            }
        else if (dc->deviceType == MGL_MEMORY_DEVICE) {
            if ((dc->rc = DEV.glInit.CreateSysMemContext(dc->mi.xRes+1,dc->mi.yRes+1,dc->mi.bitsPerPixel,
                    dc->surface,dc->mi.bytesPerLine,GAPF(&dc->pf),&snapVis,NULL)) == NULL)
                return false;
            }
        /* Fail any other type of MGL device context */
        else
            return false;
        }
    else if (!__MGL_glCreateContext(dc,flags))
        return false;

    /* Create a 3:3:2 RGB palette for the device context for 8bpp RGB modes */
    if (dc->mi.bitsPerPixel == 8) {
        palette_t pal[256];
        if (dc->cntVis.rgb_flag) {
            if (DEV.cntOpenGL == MGL_GL_SNAP)
                DEV.glInit.GetHalfTonePalette(GAPAL(pal),256);
            else
                __MGL_glCreateRGBPalette(dc,pal);
            }
        else
            MGL_getPalette(dc,pal,256,0);
        MGL_glSetPalette(dc,pal,256,0);
        MGL_glRealizePalette(dc,256,0,-1);
        }
    return true;
}

/****************************************************************************
DESCRIPTION:
Makes a MGL device context the current OpenGL rendering context.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to make the current OpenGL rendering context.

REMARKS:
This function makes the MGL device context the current rendering context for
all OpenGL rendering commands. You must first call MGL_glCreateContext for the
MGL device context to create a valid OpenGL rendering context before you can
call this function to make it the rendering current OpenGL rendering context.

The MGL and OpenGL allow you to create multiple rendering context, and to
switch between them for output you must use this function to make one of
them current at a time. You cannot have more than one OpenGL rendering
context current at one time. For instance you may be drawing to a fullscreen
OpenGL rendering context, but have an MGL memory device context that you
wish to render a small 3D scene into, so you would make the memory device
context the current OpenGL rendering context with a call to this function.
The any subsequent OpenGL commands will draw to the memory device context
instead of the display device context.

SEE ALSO:
MGL_glCreateContext, MGL_glDeleteContext
****************************************************************************/
void MGLAPI MGL_glMakeCurrent(
    MGLDC *dc)
{
    MGL_makeCurrentDC(dc);
    if (DEV.cntOpenGL == MGL_GL_SNAP)
        DEV.glInit.MakeCurrent(dc ? dc->rc : NULL);
    else
        __MGL_glMakeCurrent(dc);
}

/****************************************************************************
DESCRIPTION:
Delete the OpenGL rendering context associated with the MGL device contex.

HEADER:
mgraph.h

PARAMETERS:
dc  - MGL device context to delete OpenGL rendering context for

REMARKS:
This function destroys the OpenGL rendering context for the MGL device
context, and calls OpenGL to ensure that no rendering context is still
current. You must call this function before you destroy and MGL device
context if you have enabled OpenGL via MGL_glCreateContext.

Note:   After you have called this function, it is an error to make calls to
        the OpenGL API as OpenGL will not have a current rendering context
        active.

SEE ALSO:
MGL_glCreateContext, MGL_glMakeCurrent
****************************************************************************/
void MGLAPI MGL_glDeleteContext(
    MGLDC *dc)
{
    if (DEV.cntOpenGL == MGL_GL_SNAP)
        DEV.glInit.DeleteContext(dc->rc);
    else
        __MGL_glDeleteContext(dc);
}

/****************************************************************************
DESCRIPTION:
Swaps the display buffers for OpenGL rendering.

HEADER:
mgraph.h

PARAMETERS:
dc      - MGL device context to swap display buffers for
waitVRT - Wait for retrace flag (MGL_waitVRTFlagType)

REMARKS:
This function swaps the OpenGL rendering buffers, and the current back buffer
becomes the front buffer and vice versa. If you are running in a window,
the context of the back buffer will be copied to the current window. If you
are running in a fullscreen graphics mode with hardware page flipping, this
function will do a hardware page flip to swap the buffers.

When the OpenGL buffers are swapped, you should normally allow MGL/OpenGL
driver to sync to the vertical retrace to ensure that the change occurs in the
correct place, and that you don't get flicker effects on the display. You may
however turn off the vertical retrace synching if you are synching up with the
retrace period using some other means, or if you are measuring the performance
of your application.
****************************************************************************/
void MGLAPI MGL_glSwapBuffers(
    MGLDC *dc,
    int waitVRT)
{
    if (DEV.cntOpenGL == MGL_GL_SNAP)
        DEV.glInit.SwapBuffers(dc->rc,waitVRT);
    else
        __MGL_glSwapBuffers(dc,waitVRT);
}

/****************************************************************************
DESCRIPTION:
Resizes the OpenGL buffers for the windowed device context.

HEADER:
mgraph.h

PARAMETERS:
dc  - MGL device context to resize the buffers for

REMARKS:
This function informs the MGL that the dimensions of a windowed device
context have changed, and that the OpenGL rendering buffers need to be
re-sized to the new dimensions of the window. It is up to the application
programmer to capture the WM_SIZE messages in windowed modes, and call
this function when the window size changes to let the MGL correctly
update the buffer dimensions.

Note:   This function is not necessary in fullscreen modes.
****************************************************************************/
void MGLAPI MGL_glResizeBuffers(
    MGLDC *dc)
{
    PFNGLRESIZEBUFFERSMESAPROC  glResizeBuffersMESA;

    if (DEV.cntOpenGL == MGL_GL_SNAP) {
        if ((glResizeBuffersMESA = DEV.glInit.GetProcAddress("glResizeBuffersMESA")) != NULL)
            glResizeBuffersMESA();
        }
    else
        __MGL_glResizeBuffers(dc);
}

/****************************************************************************
DESCRIPTION:
Returns the address of an OpenGL extension function.

HEADER:
mgraph.h

PARAMETERS:
dc  - MGL device context to get the OpenGL extension function for

RETURNS:
Address of the specified extension function, NULL if not available.

REMARKS:
This function returns the address of an OpenGL extension function if
that extension is supported by the OpenGL implementation. Each OpenGL
implementation may export a number of OpenGL extension that may not be
supported by other OpenGL implementations, and this function is the
mechanism you can use to obtain the address of those extension functions.

Note that you should first check to see if an extension is available, but
calling the OpenGL function glGetString(GL_EXTENSIONS) to get a list of
all the available extensions. In order to check for a specific extension
by name, you can use the following code:

    ibool checkExtension(const char *name)
    {
        const char *p = (const char *)glGetString(GL_EXTENSIONS);
        while (p = strstr(p, name)) {
            const char *q = p + strlen(name);
            if (*q == ' ' || *q == '\0')
                return GL_TRUE;
            p = q;
            }
        return GL_FALSE;
    }

Note:   It is an error to call this function for an MGL device context that
        does not have an OpenGL rendering context associated with it via
        a call to MGL_glCreateContext.
****************************************************************************/
void * MGLAPI MGL_glGetProcAddress(
    const char *procName)
{
    if (DEV.cntOpenGL == MGL_GL_SNAP)
        return DEV.glInit.GetProcAddress(procName);
    else
        return __MGL_glGetProcAddress(procName);
}

/****************************************************************************
DESCRIPTION:
Sets the palette values for a device context when using OpenGL.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to set palette for
pal         - Palette to set
numColors   - Number of colors in the palette
startIndex  - Starting index in the palette

REMARKS:
This functions sets the color palette for an MGL device context when
running in OpenGL rendering modes. This function is identical to the
regular MGL_setPalette function, however if you are running OpenGL you
must use this function instead.

SEE ALSO:
MGL_glRealizePalette, MGL_setPalette
****************************************************************************/
void MGLAPI MGL_glSetPalette(
    MGLDC *dc,
    palette_t *pal,
    int numColors,
    int startIndex)
{
    if (DEV.cntOpenGL == MGL_GL_SNAP)
        DEV.glInit.SetPaletteData(dc->rc,GAPAL(pal),numColors,startIndex);
    MGL_setPalette(dc,pal,numColors,startIndex);
}

/****************************************************************************
DESCRIPTION:
Realizes the hardware palette for a device context when using OpenGL.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to set palette for
numColors   - Number of colors in the palette
startIndex  - Starting index in the palette
waitVRT     - True if routine should sync to vertical retrace, false if not.

REMARKS:
This function realizes the hardware palette associated with a display device
context. Calls to MGL_glSetPalette only update the palette values in the
color palette for the device context structure, but do not actually program
the hardware palette for display device contexts in 4 and 8 bits per pixel
modes. In order to program the hardware palette you must call this routine.

When the hardware palette is realized, you normally need to sync to the
vertical retrace to ensure that the palette values are programmed without
the onset of snow (see MGL_setPaletteSnowLevel to adjust the number of
colors programmed per retrace period). If however you wish to perform double
buffered animation and change the hardware color palette at the same time,
you should call this routine immediately after calling either
MGL_setVisualPage or MGL_swapBuffers with the waitVRT flag set to false.

Note:   This function is identical to the regular MGL_realizePalette
        function, however if you have enabled OpenGL you must use this
        function instead.

SEE ALSO:
MGL_glSetPalette, MGL_realizePalette
****************************************************************************/
void MGLAPI MGL_glRealizePalette(
    MGLDC *dc,
    int numColors,
    int startIndex,
    int waitVRT)
{
    MGL_realizePalette(dc,numColors,startIndex,waitVRT);
}

/****************************************************************************
DESCRIPTION:
Enables MGL 2D drawing functions using an OpenGL surface

HEADER:
mgraph.h

REMARKS:
This function enable support for MGL 2D functions for hardware
accelerated OpenGL surfaces. In order to be able to draw to a hardware OpenGL
surface using 2D OpenGL drawing functions, the MGL needs to re-program the
state of the OpenGL rendering context such that it maps to a 2D integer
coordinate system rather than the 2D or 3D coordinate system the user
application code is using for OpenGL drawing. Hence this function saves the
the state of the OpenGL rendering context so it can be restored with
MGL_glDisableMGLFuncs, and sets the rendring context into a state that is
suitable for 2D drawing via internal functions in the MGL.

Although it is not necessary to call this function to use the MGL 2D drawing
functions (such as MGL_fillRect, MGL_bitBlt, MGL_drawStr etc), the process of
saving and restoring the rendering context is expensive. Internally the
MGL will call this function before doing any 2D operations, which means the
state is saved and restore for every 2D MGL function that is called. Hence
if you will be calling a number of 2D MGL drawing functions, you can bracket
your code with MGL_glEnableMGLFuncs and MGL_glDisableMGLFuncs to ensure that
the MGL only saves and restores the rendering context once for all MGL 2D
drawing functions you call.

SEE ALSO:
MGL_glDisableMGLFuncs
****************************************************************************/
void MGLAPI MGL_glEnableMGLFuncs(void)
{
    uchar   R,G,B;

    if (!DEV.glLoaded)
        return;
    if (!DC.mglEnabled) {
        /* Save current bitmap tranfer modes */
        glGetIntegerv(GL_UNPACK_SWAP_BYTES, &DC.glSwapBytes);
        glGetIntegerv(GL_UNPACK_LSB_FIRST, &DC.glLsbFirst);
        glGetIntegerv(GL_UNPACK_ROW_LENGTH, &DC.glRowLength);
        glGetIntegerv(GL_UNPACK_SKIP_ROWS, &DC.glSkipRows);
        glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &DC.glSkipPixels);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &DC.glAlignment);
        DC.glLighting = glIsEnabled(GL_LIGHTING);
        DC.glDepthTest = glIsEnabled(GL_DEPTH_TEST);
        glGetIntegerv(GL_VIEWPORT,DC.glViewport);
        glGetDoublev(GL_CURRENT_COLOR,DC.glCntColor);

        /* Set the bitmap tranfer modes */
        glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei(GL_UNPACK_LSB_FIRST, GL_TRUE);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        /* Disable DC.glLighting and depth testing */
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        /* Set the current raster color */
        MGL_unpackColorFast(&DC.pf,DC.a.color,R,G,B);
        glColor3ub(R,G,B);

        /* Set the DC.glViewport and projection to othographic 2D mode */
        glViewport(0, 0, _MGL_dc.mi.xRes+1, _MGL_dc.mi.yRes+1);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glScalef(1,-1,1);
        glOrtho(0.0, _MGL_dc.mi.xRes, 0.0, _MGL_dc.mi.yRes, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        /* Flag that we have disabled OpenGL calls for the moment */
        DC.mglEnabled = true;
        }
}

/****************************************************************************
DESCRIPTION:
Disables MGL 2D drawing functions using an OpenGL surface

HEADER:
mgraph.h

REMARKS:
This function disable support for MGL 2D functions for hardware
accelerated OpenGL surfaces. This does the opposite of the MGL_glEnableMGLFuncs
function.

SEE ALSO:
MGL_glEnableMGLFuncs
****************************************************************************/
void MGLAPI MGL_glDisableMGLFuncs(void)
{
    if (!DEV.glLoaded)
        return;
    if (DC.mglEnabled) {
        /* Restore DC.glViewport, modelview and projection matrices */
        glViewport(DC.glViewport[0], DC.glViewport[1], DC.glViewport[2], DC.glViewport[3]);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        /* Restore DC.glLighting and depth testing */
        if (DC.glLighting)
            glEnable(GL_LIGHTING);
        if (DC.glDepthTest)
            glEnable(GL_DEPTH_TEST);

        /* Restore the current color */
        glColor4dv(DC.glCntColor);

        /* Restore bitmap transfer modes */
        glPixelStorei(GL_UNPACK_SWAP_BYTES, DC.glSwapBytes);
        glPixelStorei(GL_UNPACK_LSB_FIRST, DC.glLsbFirst);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, DC.glRowLength);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, DC.glSkipRows);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, DC.glSkipPixels);
        glPixelStorei(GL_UNPACK_ALIGNMENT, DC.glAlignment);

        /* Flag that we have re-enabled OpenGL calls */
        DC.mglEnabled = false;
        }
}

