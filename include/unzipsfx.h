/****************************************************************************
*
*                   Header file for the Unzip SFX Library
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
* Environment:  Any
*
* Description:  Function used to interface to the UnZIP SFX library.
*
****************************************************************************/

#ifndef __UNZIPSFX_H
#define __UNZIPSFX_H

#include "scitech.h"

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Function to unzip files given C 'main' style arguments directly from the
 * ZIP file attached to the executeable.
 */

int     UnzipFromSelf(int argc,char *argv[]);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __UNZIPSFX_H */

