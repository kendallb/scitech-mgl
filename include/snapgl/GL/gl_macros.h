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
* Description:  Header file to define these as macros compatible with
*               <windows.h> for Windows code. Note that we also set things
*               up so that we can remove the #defines at the end of our
*               header to avoid conflicts with the real <windows.h> headers
*               later on. Note that we also allow you to include the
*               regular <windows.h> header before GL/gl.h for porting
*               code, however you would be better off removing it as your
*               code will compile significantly faster ;-).
*
****************************************************************************/

#include "scitech.h"

#ifndef WINGDIAPI
#define clean_WINGDIAPI
#define WINGDIAPI
#endif

#ifndef APIENTRY
#define clean_APIENTRY
#define APIENTRY    _STDCALL
#endif

#ifndef CALLBACK
#define clean_CALLBACK
#define CALLBACK    _STDCALL
#endif

#undef  GLAPI
#undef  GLAPIENTRY
#undef  GLAPIENTRYP
#undef  GLCALLBACK
#undef  GLCALLBACKP
#undef  GLCALLBACKPCAST
#define GLAPI       extern
#define GLAPIENTRY  _STDCALL
#define GLCALLBACK  CALLBACK
#define GLWINAPI
#define GLWINAPIV

#ifdef  PTR_DECL_IN_FRONT
#define APIENTRYP   * APIENTRY
#define GLAPIENTRYP * GLAPIENTRY
#define CALLBACKP   * CALLBACK
#define GLCALLBACKP * GLCALLBACK
#define GLCALLBACKPCAST * GLCALLBACK
#else
#define APIENTRYP   APIENTRY *
#define GLAPIENTRYP GLAPIENTRY *
#define CALLBACKP   CALLBACK *
#define GLCALLBACKP GLCALLBACK *
#define GLCALLBACKPCAST GLCALLBACK *
#endif

/* Let the internal headers know we are building for SciTech SNAP */

#define __SCITECH_SNAP__

