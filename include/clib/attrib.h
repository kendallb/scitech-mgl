/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  Header file for the OS specific function to set attribute
*               flags.
*
****************************************************************************/

#ifndef __ATTRIB_H
#define __ATTRIB_H

#ifndef __SYS_CDECL_H
#include "clib/sys/cdecl.h"
#endif

/* Attribue bits for _OS_setfileattr */

#define __A_NORMAL   0x00   /* Normal file, no attributes */
#define __A_RDONLY   0x01   /* Read only attribute */
#define __A_HIDDEN   0x02   /* Hidden file */
#define __A_SYSTEM   0x04   /* System file */

#ifdef  __cplusplus
extern "C" {
#endif

void    SNAP_CDECL _OS_setfileattr(const char *filename,unsigned attrib);
ulong   SNAP_CDECL _OS_getcurrentdate(void);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __ATTRIB_H */

