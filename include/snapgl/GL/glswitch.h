/****************************************************************************
*
*                       SciTech OpenGL Switching Library
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
* Environment:  Win32
*
* Description:  Public header file for the OpenGL switching library. This
*               library allows application developers to link with the
*               glswitch library instead of opengl32.lib, and switch between
*               multiple OpenGL DLL's at runtime.
*
****************************************************************************/

#ifndef __GLSWITCH_H
#define __GLSWITCH_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/wgl_stub.h>

/*--------------------- Macros and type definitions -----------------------*/

/* Define the calling conventions for all public functions */

#define GLSAPI  _ASMAPI         /* 'C' calling conventions for all      */
#define GLSAPIP _ASMAPIP        /* 'C' calling conventions for all      */

/****************************************************************************
REMARKS:
Defines the values passed to GLS_loadOpenGL to determine what version of
the GLU library should be loaded for use. A value of glsGLUAuto will
choose glu32.dll if opengl32.dll is the OpenGL library being loaded,
otherwise it will choose sgiglu.dll. The sgiglu.dll is a special build of
the Silicon Graphics OpenGL for Windows GLU DLL that knows how to switch
the internal OpenGL vectors between DLL's when it is loaded. If you pass
a value of glsGLUSGI, the SGI version of GLU will always be used even if
the Microsoft opengl32.dll library is used. This might be useful to ensure
consistent results for GLU functions across all OpenGL implementations.

HEADER:
glswitch.h

MEMBERS:
glsGLUNone  - Do not load and use the GLU libraries
glsGLUAuto  - Automatically choose the GLU based on DLL loaded
glsGLUsSGI  - Always use the SGI version of GLU
****************************************************************************/
typedef enum {
    glsGLUNone,
    glsGLUAuto,
    glsGLUSGI,
    } GLS_gluTypes;

/****************************************************************************
REMARKS:
Defines the error codes returned by the GLS_loadOpenGL function.

HEADER:
glswitch.h

MEMBERS:
glsOK                   - No error
glsErrGLNotFound        - OpenGL library not found
glsErrGLUNotFound       - GLU library not found
glsErrNoGLDirect        - GLDirect is not present
glsErrInvalidLicense    - Invalid Oem license file
****************************************************************************/
typedef enum {
    glsOK,
    glsErrGLNotFound,
    glsErrGLUNotFound,
    glsErrNoGLDirect,
    glsErrInvalidLicense,
    } GLS_errorCodes;

/*------------------------- Function Prototypes ---------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Function to initialise GLS and register an ISV license. This version
 * determines what version of OpenGL should be used by looking at the
 * GL_OPENGL environment variable. Set this to opengl32.dll to use
 * Microsoft OpenGL instead of the default gldirect.dll.
 */

void    GLSAPI GLS_init(const uchar *OemLicense);

/* Function to load the specific OpenGL and GLU libraries */

int     GLSAPI GLS_initExt(const char *GLName,int GLUType);

/* Function to register a GLDirect ISV license */

int     GLSAPI GLS_registerLicense(const uchar *OemLicense);

/* Function to unload the OpenGL and GLU libraries */

void    GLSAPI GLS_exit(void);

/* Utility function to determine if hardware acceleration is available */

ibool   GLSAPI GLS_haveHWOpenGL(void);

#ifdef __cplusplus
}
#endif

#endif /* __GLSWITCH_H */
