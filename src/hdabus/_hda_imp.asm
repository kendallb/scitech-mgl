;****************************************************************************
;*
;*                     SciTech SNAP HDA Bus Architecture
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
;* Environment: IBM PC 32 bit Protected Mode.
;*
;* Description: Module to implement the import stubs for all the SNAP
;*              HDA Bus API functions.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

BEGIN_IMPORTS_DEF   __HDA_exports
SKIP_IMP    HDA_init,3                          ; Implemented in C code
DECLARE_IMP HDA_exit,0
DECLARE_IMP HDA_sendCommand,6
DECLARE_IMP HDA_getResponse,0
DECLARE_IMP HDA_allocStream,3
DECLARE_IMP HDA_freeStream,1
DECLARE_IMP HDA_setStreamFormat,4
DECLARE_IMP HDA_startStream,3
DECLARE_IMP HDA_stopStream,1
END_IMPORTS_DEF

   END

