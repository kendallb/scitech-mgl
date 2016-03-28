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
* Environment:  any
*
* Description:  Special routines to return the address of global variables
*               in 32 bit DLL's to avoid the need to export data. Watcom
*               C++ does not appear to be able to handle exported global
*               variables correctly at the moment.
*
****************************************************************************/

#ifdef  BUILD_DLL
#undef  BUILD_DLL
#define BUILD_MGLDLL
#include "mgraph.h"

void * MGLAPI _export VGA4_getDriverAddr(void)     { return VGA4_driver; }
void * MGLAPI _export VGA8_getDriverAddr(void)     { return VGA8_driver; }
void * MGLAPI _export VGAX_getDriverAddr(void)     { return VGAX_driver; }
void * MGLAPI _export SVGA4_getDriverAddr(void)    { return SVGA4_driver; }
void * MGLAPI _export SVGA8_getDriverAddr(void)    { return SVGA8_driver; }
void * MGLAPI _export SVGA16_getDriverAddr(void)   { return SVGA16_driver; }
void * MGLAPI _export SVGA24_getDriverAddr(void)   { return SVGA24_driver; }
void * MGLAPI _export SVGA32_getDriverAddr(void)   { return SVGA32_driver; }
void * MGLAPI _export LINEAR8_getDriverAddr(void)  { return LINEAR8_driver; }
void * MGLAPI _export LINEAR16_getDriverAddr(void) { return LINEAR16_driver; }
void * MGLAPI _export LINEAR24_getDriverAddr(void) { return LINEAR24_driver; }
void * MGLAPI _export LINEAR32_getDriverAddr(void) { return LINEAR32_driver; }
void * MGLAPI _export VBEAF8_getDriverAddr(void)   { return VBEAF8_driver; }
void * MGLAPI _export VBEAF16_getDriverAddr(void)  { return VBEAF16_driver; }
void * MGLAPI _export VBEAF24_getDriverAddr(void)  { return VBEAF24_driver; }
void * MGLAPI _export VBEAF32_getDriverAddr(void)  { return VBEAF32_driver; }
void * MGLAPI _export ACCEL8_getDriverAddr(void)   { return ACCEL8_driver; }
void * MGLAPI _export ACCEL16_getDriverAddr(void)  { return ACCEL16_driver; }
void * MGLAPI _export ACCEL24_getDriverAddr(void)  { return ACCEL24_driver; }
void * MGLAPI _export ACCEL32_getDriverAddr(void)  { return ACCEL32_driver; }
void * MGLAPI _export DDRAW8_getDriverAddr(void)   { return DDRAW8_driver; }
void * MGLAPI _export DDRAW16_getDriverAddr(void)  { return DDRAW16_driver; }
void * MGLAPI _export DDRAW24_getDriverAddr(void)  { return DDRAW24_driver; }
void * MGLAPI _export DDRAW32_getDriverAddr(void)  { return DDRAW32_driver; }
void * MGLAPI _export OPENGL_getDriverAddr(void)   { return OPENGL_driver; }
void * MGLAPI _export DRCTGL8_getDriverAddr(void)  { return DRCTGL8_driver; }
void * MGLAPI _export DRCTGL16_getDriverAddr(void) { return DRCTGL16_driver; }
void * MGLAPI _export DRCTGL24_getDriverAddr(void) { return DRCTGL24_driver; }
void * MGLAPI _export DRCTGL32_getDriverAddr(void) { return DRCTGL32_driver; }
void * MGLAPI _export PACKED1_getDriverAddr(void)  { return PACKED1_driver; }
void * MGLAPI _export PACKED4_getDriverAddr(void)  { return PACKED4_driver; }
void * MGLAPI _export PACKED8_getDriverAddr(void)  { return PACKED8_driver; }
void * MGLAPI _export PACKED16_getDriverAddr(void) { return PACKED16_driver; }
void * MGLAPI _export PACKED24_getDriverAddr(void) { return PACKED24_driver; }
void * MGLAPI _export PACKED32_getDriverAddr(void) { return PACKED32_driver; }

#endif
