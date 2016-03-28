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
* Environment:  Any
*
* Description:  Include file defining the external ring 0 helper functions
*               needed by the MTRR module. These functions may be included
*               directly for native ring 0 device drivers, or they may
*               be calls down to a ring 0 helper device driver where
*               appropriate (or the entire MTRR module may be located in
*               the device driver if the device driver is 32-bit).
*
****************************************************************************/

#ifndef __MTRR_H
#define __MTRR_H

#include "scitech.h"

/*--------------------------- Function Prototypes -------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Internal functions (requires ring 0 access or helper functions!) */

void    MTRR_init(void);
int     MTRR_enableWriteCombine(ulong base,ulong size,uint type);
int     MTRR_enumWriteCombine(PM_enumWriteCombine_t callback);

/* External assembler helper functions */

ibool   _ASMAPI _MTRR_isRing0(void);
ulong   _ASMAPI _MTRR_disableInt(void);
void    _ASMAPI _MTRR_restoreInt(ulong flags);
ulong   _ASMAPI _MTRR_saveCR4(void);
void    _ASMAPI _MTRR_restoreCR4(ulong cr4Val);
uchar   _ASMAPI _MTRR_getCx86(uchar reg);
void    _ASMAPI _MTRR_setCx86(uchar reg,uchar data);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __MTRR_H */
