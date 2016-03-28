;****************************************************************************
;*
;*                  SciTech OS Portability Manager Library
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
;* Environment: SMX 32 bit intel CPU
;*
;* Description: SMX does not support 486's, so this module is not necessary.
;*
;*  All registers and all flags are preserved by all routines, except
;*  interrupts which are always turned on
;*
;****************************************************************************

include "scitech.mac"

header      _lztimer

begdataseg  _lztimer

enddataseg  _lztimer

begcodeseg  _lztimer                ; Start of code segment

cprocstart   LZ_disable
        cli
        ret
cprocend

cprocstart   LZ_enable
        sti
        ret
cprocend

endcodeseg  _lztimer

        END

