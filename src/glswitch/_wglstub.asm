;****************************************************************************
;*
;*                      SciTech OpenGL Switching Library
;*
;*  ========================================================================
;*
;*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
;*
;*   This file may be distributed and/or modified under the terms of the
;*   GNU General Public License version 2.0 as published by the Free
;*   Software Foundation and appearing in the file LICENSE.GPL included
;*   in the packaging of this file.
;*
;*   Licensees holding a valid Commercial License for this product from
;*   SciTech Software, Inc. may use this file in accordance with the
;*   Commercial License Agreement provided with the Software.
;*
;*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
;*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;*   PURPOSE.
;*
;*   See http://www.scitechsoft.com/license/ for information about
;*   the licensing options available and how to purchase a Commercial
;*   License Agreement.
;*
;*   Contact license@scitechsoft.com if any conditions of this licensing
;*   are not clear to you, or you have questions about licensing options.
;*
;*  ========================================================================
;*
;* Language:    NASM
;* Environment: Intel x86
;*
;* Description: OpenGL stub functions to call dynamically linked OpenGL
;*              code in any version of OpenGL on the system.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

BEGIN_IMPORTS_DEF   _GLS_glsFuncs

; GDI32 functions
DECLARE_STDCALL glsChoosePixelFormat,8
DECLARE_STDCALL glsDescribePixelFormat,16
DECLARE_STDCALL glsGetPixelFormat,4
DECLARE_STDCALL glsSetPixelFormat,12
DECLARE_STDCALL glsSwapBuffers,4

; OpenGL gls functions
DECLARE_STDCALL glsCreateContext,4
DECLARE_STDCALL glsDeleteContext,4
DECLARE_STDCALL glsGetCurrentContext,0
DECLARE_STDCALL glsGetCurrentDC,0
DECLARE_STDCALL glsGetProcAddress,4
DECLARE_STDCALL glsMakeCurrent,8
DECLARE_STDCALL glsShareLists,8
DECLARE_STDCALL glsUseFontBitmapsA,16
DECLARE_STDCALL glsUseFontBitmapsW,16
DECLARE_STDCALL glsUseFontOutlinesA,32
DECLARE_STDCALL glsUseFontOutlinesW,32

END_IMPORTS_DEF

        END                     ; End of module
