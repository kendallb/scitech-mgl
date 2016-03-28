/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Environment:  32-bit SMX embedded systems development.
*
* Description:  Header file for the MGLSMX bindings for SMX.
*
****************************************************************************/

#ifndef __MGLSMX_H
#define __MGLSMX_H

#ifndef MGLSMX
#define MGLSMX
#endif

/*---------------------- Macros and type definitions ----------------------*/

/* {secret} */
typedef void        *MGL_HDC;
/* {secret} */
typedef void        *MGL_HINSTANCE;
/* {secret} */
typedef void        *MGL_HPALETTE;
/* {secret} */
typedef void        *MGL_HGLRC;

typedef ibool       (*_MGL_userEventFilter)(event_t *evt);

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Install user supplied event filter callback */

void    MGLAPI MGL_setUserEventFilter(_MGL_userEventFilter filter);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __MGLSMX_H */
