;****************************************************************************
;*
;*                    SciTech SNAP Graphics Architecture
;*
;*               Copyright (C) 1997-20- SciTech Software, Inc.
;*                            All rights reserved.
;*
;*  ======================================================================
;*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
;*  |                                                                    |
;*  |This copyrighted computer code is a proprietary trade secret of     |
;*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
;*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
;*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
;*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
;*  |written authorization from SciTech to possess or use this code, you |
;*  |may be subject to civil and/or criminal penalties.                  |
;*  |                                                                    |
;*  |If you received this code in error or you would like to report      |
;*  |improper use, please immediately contact SciTech Software, Inc. at  |
;*  |530-894-8400.                                                       |
;*  |                                                                    |
;*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
;*  ======================================================================
;*
;* Language:    NASM
;* Environment: SciTech SNAP Binary Portable DLL
;*
;* Description: DLL startup code and necessary C runtime library functions
;*              so we can build a DLL without the standard C library used
;*              by the compiler, but instead our own SNAP C runtime library.
;*
;****************************************************************************

include "scitech.mac"

header      dllstart

begcodeseg  dllstart

; We reference the following init function in this startup module to
; ensure that the code in the \scitech\src\drvlib\init.c module is pulled
; in so that the C runtime library is correctly initialised.

            extern _InitLibC

;----------------------------------------------------------------------------
; DLLEntry point
;----------------------------------------------------------------------------
; This is the entry point that is called to initialise the DLL when it is
; loaded from disk. We don't call this function, but it is used to make sure
; the C runtime init code is pulled in correctly.
;----------------------------------------------------------------------------
cglobalfunc DLLstart_
cglobalfunc __DllMainCRTStartup@12
cprocstart  DLLstart_
__DllMainCRTStartup@12:
..start:
        mov     eax,offset _InitLibC
        mov     eax,1
        ret     12

cprocend

endcodeseg  dllstart

        END

