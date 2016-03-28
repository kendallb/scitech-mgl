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

BEGIN_IMPORTS_DEF   _GLS_gluFuncs
DECLARE_STDCALL gluBeginCurve,4
DECLARE_STDCALL gluBeginPolygon,4
DECLARE_STDCALL gluBeginSurface,4
DECLARE_STDCALL gluBeginTrim,4
DECLARE_STDCALL gluBuild1DMipmaps,24
DECLARE_STDCALL gluBuild2DMipmaps,28
DECLARE_STDCALL gluCylinder,36
DECLARE_STDCALL gluDeleteNurbsRenderer,4
DECLARE_STDCALL gluDeleteQuadric,4
DECLARE_STDCALL gluDeleteTess,4
DECLARE_STDCALL gluDisk,28
DECLARE_STDCALL gluEndCurve,4
DECLARE_STDCALL gluEndPolygon,4
DECLARE_STDCALL gluEndSurface,4
DECLARE_STDCALL gluEndTrim,4
DECLARE_STDCALL gluErrorString,4
DECLARE_STDCALL gluGetNurbsProperty,12
DECLARE_STDCALL gluGetString,4
DECLARE_STDCALL gluGetTessProperty,12
DECLARE_STDCALL gluLoadSamplingMatrices,16
DECLARE_STDCALL gluLookAt,72
DECLARE_STDCALL gluNewNurbsRenderer,0
DECLARE_STDCALL gluNewQuadric,0
DECLARE_STDCALL gluNewTess,0
DECLARE_STDCALL gluNextContour,8
DECLARE_STDCALL gluNurbsCallback,12
DECLARE_STDCALL gluNurbsCurve,28
DECLARE_STDCALL gluNurbsProperty,12
DECLARE_STDCALL gluNurbsSurface,44
DECLARE_STDCALL gluOrtho2D,32
DECLARE_STDCALL gluPartialDisk,44
DECLARE_STDCALL gluPerspective,32
DECLARE_STDCALL gluPickMatrix,36
DECLARE_STDCALL gluProject,48
DECLARE_STDCALL gluPwlCurve,20
DECLARE_STDCALL gluQuadricCallback,12
DECLARE_STDCALL gluQuadricDrawStyle,8
DECLARE_STDCALL gluQuadricNormals,8
DECLARE_STDCALL gluQuadricOrientation,8
DECLARE_STDCALL gluQuadricTexture,8
DECLARE_STDCALL gluScaleImage,36
DECLARE_STDCALL gluSphere,20
DECLARE_STDCALL gluTessBeginContour,4
DECLARE_STDCALL gluTessBeginPolygon,8
DECLARE_STDCALL gluTessCallback,12
DECLARE_STDCALL gluTessEndContour,4
DECLARE_STDCALL gluTessEndPolygon,4
DECLARE_STDCALL gluTessNormal,28
DECLARE_STDCALL gluTessProperty,16
DECLARE_STDCALL gluTessVertex,12
DECLARE_STDCALL gluUnProject,48
END_IMPORTS_DEF

        END                     ; End of module
