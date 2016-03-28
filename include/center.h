/****************************************************************************
*
*                  Display Doctor Windows Interface Code
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
* Environment:  IBM PC (MS DOS)
*
* Description:  Header file for centering windows.
*
****************************************************************************/

#ifndef __CENTER_H
#define __CENTER_H

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "scitech.h"

#ifndef _EXPORT
#define _EXPORT __declspec(dllexport)
#endif

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

void _EXPORT CenterWindow(HWND hWndCenter, HWND parent, BOOL repaint);
void _EXPORT CenterLogo(HWND hWndLogo, HWND hWndParent, int CenterY);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#pragma pack()

#endif  /* __CENTER_H */

