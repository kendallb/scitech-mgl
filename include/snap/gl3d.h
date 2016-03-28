/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Header file for the SciTech SNAP Graphics Architecture
*               device driver Hardware Abstraction Layer.
*
*               This header file contains all the macros, type definitions
*               and function prototypes for the generic SNAP Graphics based
*               OpenGL compatible GL3D device driver interface.
*
****************************************************************************/

#ifndef __SNAP_GL3D_H
#define __SNAP_GL3D_H

#include "snap/graphics.h"
#include "GL/gl.h"
#include "GL/glu.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Signature for the graphics driver file */

#define GL3D_SIGNATURE          "GL3D"

/* Define the interface version */

#define GL3D_VERSION            0x100

/****************************************************************************
REMARKS:
This enumeration defines the identifiers used to obtain the device context
function group pointer structures. As new features and capabilities are
added to the future versions of the specification, new identifiers will
be added to extract new function pointers from the drivers.

HEADER:
snap/gl3d.h

MEMBERS:
GA_GET_GLINITFUNCS      - Get GA_glInitFuncs structure
GA_GET_GLFUNCS          - Get GA_glFuncs structure
****************************************************************************/
typedef enum {
    GA_GET_GLINITFUNCS,
    GA_GET_GLFUNCS,
    GA_GET_GLUFUNCS
    } GA_glFuncGroupsType;

/****************************************************************************
REMARKS:
Structure representing the information passed to the SNAP Graphics GL3D binding
functions to both choose an appropriate visual that is supported by the
GL3D implementation and to pass in the information for the visual when
a rendering context is created.

HEADER:
snap/gl3d.h

MEMBERS:
dwSize          - Set to size of structure in bytes
DoubleBuffer    - True to enable double buffered mode
Stereo          - True to enable stereo mode
ColorIndex      - True if the visual is color index
DepthBits       - Size of depth buffer in bits (0 for none)
AlphaBits       - Size of alpha buffer in bits (0 for none)
StencilBits     - Size of stencil buffer in bits (0 for none)
AccumBits       - Size of accumulation buffer in bits (0 for none)
SoftwareOnly    - Force rendering for the context to be entirely in software
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_int32         DoubleBuffer;
    N_int32         Stereo;
    N_int32         ColorIndex;
    N_int32         DepthBits;
    N_int32         AlphaBits;
    N_int32         StencilBits;
    N_int32         AccumBits;
    N_int32         SoftwareOnly;
    } GA_glVisual;

/* {secret} Type definition for internal GL3D rendering context object */
typedef struct GA_glContext *GA_HGLRC;

/****************************************************************************
REMARKS:
Main GL3D device driver structure. This structure consists of a header
block that contains configuration information about the OpenGL compatible
device driver.

The Signature member is filled with the null terminated string 'GL3D\0'
by the driver implementation. This can be used to verify that the file loaded
really is an OpenGL comaptible 3D device driver.

The Version member is a BCD value which specifies what revision level of the
device driver specification is implemented in the driver. The high byte
specifies the major version number and the low byte specifies the minor
version number. For example, the BCD value for version 1.0 is 0x100 and the
BCD value for version 2.2 would be 0x202.

The DriverRev member specifies the driver revision level, and is used by the
driver configuration software. This is a three digit BCD value, so 1.2.3 is
represented as 0x010203.

The OpenGLRev member specifies the version of the OpenGL API that is
implemented by this driver. This is three digit BCD value so OpenGL 1.4
is represented as 0x010400.

The OemVendorName member contains the name of the vendor that developed the
device driver implementation, and can be up to 80 characters in length.

The OemCopyright member contains a copyright string for the vendor that
developed the device driver implementation and may be up to 80 characters
in length.

HEADER:
snap/gl3d.h

MEMBERS:
Signature           - 'GL3D\0' 20 byte signature
Version             - Driver Interface Version (BCD)
DriverRev           - Driver revision number
OemVendorName       - Vendor Name string
OemCopyright        - Vendor Copyright string
****************************************************************************/
typedef struct GA_glCtx {
    /*------------------------------------------------------------------*/
    /* Device driver header block                                       */
    /*------------------------------------------------------------------*/
    char            Signature[20];
    N_uint32        Version;
    N_uint32        DriverRev;
    N_uint32        OpenGLRev;
    char            OemVendorName[80];
    char            OemCopyright[80];
    N_uint32        res1[20];

    /*------------------------------------------------------------------*/
    /* Driver initialization functions                                  */
    /*------------------------------------------------------------------*/
    ibool           (NAPIP QueryFunctions)(N_uint32 id,void _FAR_ *funcs);
    void            (NAPIP Unload)(void);
    } GA_glCtx;

/****************************************************************************
REMARKS:
Main device driver init functions, including all mode information, setup and
initialisation functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    ibool       (NAPIP Init)(REF2D_driver *ref2d);
    void        (NAPIP ChooseVisual)(GA_buf *buf,GA_glVisual *visual);
    ibool       (NAPIP CheckVisual)(GA_buf *buf,GA_glVisual *visual);
    GA_HGLRC    (NAPIP CreateContext)(GA_buf *buf,GA_glVisual *visual,GA_HGLRC shareList);
    GA_HGLRC    (NAPIP CreateSysMemContext)(int width,int height,int bitsPerPixel,void *surface,int stride,GA_pixelFormat *pf,GA_glVisual *visual,GA_HGLRC shareList);
    void        (NAPIP DeleteContext)(GA_HGLRC ctx);
    ibool       (NAPIP MakeCurrent)(GA_HGLRC ctx);
    GA_HGLRC    (NAPIP GetCurrentContext)(void);
    void        (NAPIP SwapBuffers)(GA_HGLRC ctx,N_int32 waitVRT);
    void        (NAPIP SetPaletteData)(GA_HGLRC ctx,GA_palette *pal,N_int32 num,N_int32 index);
    void        (NAPIP GetHalfTonePalette)(GA_palette *pal,N_int32 num);
    void *      (NAPIP GetProcAddress)(const char *procName);
    } GA_glInitFuncs;

#pragma pack()

#endif  /* __SNAP_GL3D_H */

