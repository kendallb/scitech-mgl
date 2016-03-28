/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
* Environment:  QNX
*
* Description:  QNX specific OpenGL implementation. This module is pretty
*               simple because we currently don't support hardware OpenGL
*               under QNX.
*
****************************************************************************/

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
RETURNS:
True if OpenGL is hardware accelerated on the target platform.
****************************************************************************/
static ibool __MGL_glHaveHWOpenGL(void)
{
    /* No hardware acceleration for QNX yet */
    return false;
}

/****************************************************************************
REMARKS:
Unloads any currently loaded platform specific OpenGL library components.
****************************************************************************/
static void __MGL_glUnloadOpenGL(void)
{
}

/****************************************************************************
RETURNS:
True if the OpenGL libraries are found, false if not.

REMARKS:
Attempts to find the platform specific OpenGL library, and if found
fills in the names of the OpenGL and GLU libraries.
****************************************************************************/
static ibool __MGL_glFindDefaultOpenGL(void)
{
    /* No default OpenGL on QNX (well, we're just lazy) */
    return false;
}

/****************************************************************************
PARAMETERS:
GLName  - Place to store full path to OpenGL library
GLUName - Place to store full path to OpenGL GLU library

RETURNS:
True if the OpenGL libraries are found, false if not.

REMARKS:
Attempts to find the Windows specific GLDirect game library, and if found
fills in the names of the OpenGL and GLU libraries.
****************************************************************************/
static ibool __MGL_glFindGLDirect(
    char *GLName,
    char *GLUName)
{
    /* No GLDirect OpenGL on QNX */
    (void)GLName;
    (void)GLUName;
    return false;
}

/****************************************************************************
PARAMETERS:
GLName  - Place to store full path to OpenGL library
GLUName - Place to store full path to OpenGL GLU library

RETURNS:
True if the OpenGL libraries are found, false if not.

REMARKS:
Attempts to find the Windows specific GLDirect CAD library, and if found
fills in the names of the OpenGL and GLU libraries.
****************************************************************************/
static ibool __MGL_glFindGLDirectCAD(
    char *GLName,
    char *GLUName)
{
    /* No GLDirect CAD OpenGL on QNX */
    (void)GLName;
    (void)GLUName;
    return false;
}

/****************************************************************************
REMARKS:
Attempts to load and initialise the platform specific default OpenGL
libraries.
****************************************************************************/
static void __MGL_loadDefaultOpenGL(void)
{
}

/****************************************************************************
PARAMETERS:
GLName  - Place to store full path to OpenGL library
GLUName - Place to store full path to OpenGL GLU library

REMARKS:
Attempts to load and initialise the Windows specific GLDirect libraries.
****************************************************************************/
static void __MGL_loadGLDirect(
    const char *GLName,
    const char *GLUName)
{
    (void)GLName;
    (void)GLUName;
}

/****************************************************************************
PARAMETERS:
GLName  - Place to store full path to OpenGL library
GLUName - Place to store full path to OpenGL GLU library

REMARKS:
Attempts to load and initialise the Windows specific GLDirect CAD libraries.
****************************************************************************/
static void __MGL_loadGLDirectCAD(
    const char *GLName,
    const char *GLUName)
{
    (void)GLName;
    (void)GLUName;
}

/****************************************************************************
PARAMETERS:
dc      - MGL Device context
visual  - Place to store the visual information

REMARKS:
Implements the glChooseVisual function for the system default OpenGL
implementation.
****************************************************************************/
static ibool __MGL_glChooseVisual(
    MGLDC *dc,
    MGLVisual *visual)
{
    (void)dc;
    (void)visual;
    return false;
}

/****************************************************************************
PARAMETERS:
dc      - MGL Device context
visual  - Place to store the visual information

REMARKS:
Implements the glSetVisual function for the system default OpenGL
implementation.
****************************************************************************/
static ibool __MGL_glSetVisual(
    MGLDC *dc,
    MGLVisual *visual)
{
    (void)dc;
    (void)visual;
    return false;
}

/****************************************************************************
PARAMETERS:
dc  - MGL Device context
pal - Place to store the RGB palette

REMARKS:
Creates an RGB palette for the default system OpenGL driver for running in
8bpp display modes.
****************************************************************************/
static void __MGL_glCreateRGBPalette(
    MGLDC *dc,
    palette_t *pal)
{
    (void)dc;
    (void)pal;
}

/****************************************************************************
PARAMETERS:
dc      - MGL Device context
flags   - Context creation flags

REMARKS:
Implements the glCreateContext function for the system default OpenGL
implementation.
****************************************************************************/
static ibool __MGL_glCreateContext(
    MGLDC *dc,
    int flags)
{
    (void)dc;
    (void)flags;
    return false;
}

/****************************************************************************
PARAMETERS:
dc  - MGL Device context

REMARKS:
Implements the glMakeCurrent function for the system default OpenGL
implementation.
****************************************************************************/
static void __MGL_glMakeCurrent(
    MGLDC *dc)
{
    (void)dc;
}

/****************************************************************************
PARAMETERS:
dc  - MGL Device context

REMARKS:
Implements the glDeleteContext function for the system default OpenGL
implementation.
****************************************************************************/
static void __MGL_glDeleteContext(
    MGLDC *dc)
{
    (void)dc;
}

/****************************************************************************
PARAMETERS:
dc      - MGL Device context
waitVRT - Wait for vertical retrace

REMARKS:
Implements the glSwapBuffers function for the system default OpenGL
implementation.
****************************************************************************/
static void __MGL_glSwapBuffers(
    MGLDC *dc,
    int waitVRT)
{
    (void)dc;
    (void)waitVRT;
}

/****************************************************************************
PARAMETERS:
dc      - MGL Device context

REMARKS:
Implements the MGL_glResizeBuffers function and does any OS specific stuff
in here if necessary to resize the OpenGL buffers. Normally OpenGL
system implementations hook into the system resize messages automatically
so this usually does not do much for the system OpenGL implementation.
****************************************************************************/
static void __MGL_glResizeBuffers(
    MGLDC *dc)
{
    (void)dc;
}

/****************************************************************************
PARAMETERS:
procName    - Name to get the procedure address for

REMARKS:
Implements the glGetProcAddress function for the system default OpenGL
implementation. This function is used to get the address of OpenGL
extension functions from the system default OpenGL libraries.
****************************************************************************/
static void * __MGL_glGetProcAddress(
    const char *procName)
{
    (void)procName;
    return NULL;
}
