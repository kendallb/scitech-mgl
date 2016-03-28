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
* Description:  Header file to pull in OS specific headers for the target
*               OS environment.
*
****************************************************************************/

#if     defined(__SMX32__)
#include "smx/oshdr.h"
#elif   defined(__RTTARGET__)
#include "rttarget/oshdr.h"
#elif   defined(__REALDOS__)
#include "dos/oshdr.h"
#elif   defined(__WIN32_VXD__)
#include "vxd/oshdr.h"
#elif   defined(__NT_DRIVER__)
#include "ntdrv/oshdr.h"
#elif   defined(__WINCE__)
#include "cedrv/oshdr.h"
#elif   defined(__WINDOWS32__)
#include "win32/oshdr.h"
#elif   defined(__OS2_VDD__)
#include "vxd/oshdr.h"
#elif   defined(__OS2__)
#if     defined(__OS2_PM__)
#include "os2pm/oshdr.h"
#else
#include "os2/oshdr.h"
#endif
#elif   defined(__LINUX__)
#if     defined(__USE_X11__)
#include "x11/oshdr.h"
#else
#include "linux/oshdr.h"
#endif
#elif   defined(__QNX__)
#if     defined(__USE_PHOTON__)
#include "photon/oshdr.h"
#elif   defined(__USE_X11__)
#include "x11/oshdr.h"
#else
#include "qnx/oshdr.h"
#endif
#elif   defined(__BEOS__)
#include "beos/oshdr.h"
#elif   defined(__UUU__)
#include "uuu/oshdr.h"
#else
#error  PM library not ported to this platform yet!
#endif

