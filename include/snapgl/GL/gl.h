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
* Environment:  Any
*
* Description:  Main OpenGL header file. This header file calls OpenGL
*               functions via function pointers stored in a global strcture
*               using macros.
*
*               Alternatively you can define NO_GL_MACROS and you will end
*               up calling the real function stubs that then call the
*               correct routines.
*
*               This header file is a pre-header file for the core Mesa
*               OpenGL header file, and sets up stuff specific to SciTech
*               SNAP OpenGL support prior to include the original Mesa
*               header file (renamed to GL/gl_mesa.h).
*
****************************************************************************/

#ifndef __gl_snap_h_
#define __gl_snap_h_

#include <GL/gl_macros.h>

/* Include the real Mesa header file */

#include <GL/gl_mesa.h>

/* Include the OpenGL stub definitions */

#include <GL/gl_stub.h>

/* Cleanup macros defined in GL/gl_macros.h */

#include <GL/gl_clean.h>

#endif /* __gl_snap_h_ */

