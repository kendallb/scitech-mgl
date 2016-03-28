;****************************************************************************
;*
;*                High Speed Fixed/Floating Point Library
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
;* Environment: IBM PC (MS DOS)
;*
;* Description: This module contains routines for high performance
;*              floating point logarithms using the 80387.
;*
;*                  FLlog10 - floating point base 10 logarithm
;*                  FLlog   - floating point natural logarithm
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fllog                   ; Set up memory model

begcodeseg  fllog

;----------------------------------------------------------------------------
; real FXlog10(real f);
;----------------------------------------------------------------------------
cprocstart  FXlog10

        fldlg2                  ; ST(0) := log10(2)
        fld     [REAL esp+4]    ; ST(0) := f
        fyl2x                   ; ST(0) := log10(f)
        ret

cprocend

;----------------------------------------------------------------------------
; real FXlog(real f);
;----------------------------------------------------------------------------
cprocstart  FXlog

        fldln2                  ; ST(0) := loge(2)
        fld     [REAL esp+4]    ; ST(0) := f
        fyl2x                   ; ST(0) := loge(f)
        ret

cprocend

endcodeseg  fllog

        END                     ; End of module
