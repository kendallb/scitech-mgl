/****************************************************************************
*
*                         Techniques Class Library
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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Module to implement default error handling.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tcl/error.hpp"
#ifdef  __WINDOWS__
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*--------------------------- Global Variables ----------------------------*/

// Pointer to the currently installed error handler

void (*TCL_errorHandler)(int err) = TCL_defaultErrorHandler;

/*---------------------------- Implementation -----------------------------*/

void TCL_defaultErrorHandler(int err)
{
    char    buf[80];

    strcpy(buf,"TECH: ");
    switch (err) {
        case STK_UNDERFLOW:
            strcat(buf,"Stack underflow!");
            break;
        case STK_OVERFLOW:
            strcat(buf,"Stack overflow!");
            break;
        default:
            strcat(buf,"Unknown internal error!");
        }
#ifdef  __WINDOWS__
    MessageBox(NULL, buf,"TechLib Error",MB_ICONEXCLAMATION);
#else
    fprintf(stderr,buf);
#endif
    exit(EXIT_FAILURE);
}
