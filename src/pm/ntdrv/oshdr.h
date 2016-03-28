/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  32-bit Windows NT drivers
*
* Description:  Include file to include all OS specific header files.
*
****************************************************************************/

#ifndef __NTDRV_OSHDR_H
#define __NTDRV_OSHDR_H

/*---------------------------- Global variables ---------------------------*/

int (PMAPIP _pPM_int86)(int intno,RMREGS *in,RMREGS *out);

/*--------------------------- Function Prototypes -------------------------*/

/* Internal unicode string handling functions */

UNICODE_STRING *    _PM_CStringToUnicodeString(const char *cstr);
void                _PM_FreeUnicodeString(UNICODE_STRING *uniStr);

#endif  // __NTDRV_OSHDR_H

