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
* Environment:  Any
*
* Description:  Header file to include extra headers for device driver
*               code. This header will bring in the windows system headers
*               when compiling for Windows, so use sparingly.
*
****************************************************************************/

#ifndef __MGLDD_H
#define __MGLDD_H

#include "mgl.h"
#if     defined(MGLSMX)
#include "mglsmx/internal.h"
#elif   defined(MGLWIN)
#include "mglwin/internal.h"
#elif   defined(MGLQNX)
#include "mglqnx/internal.h"
#elif   defined(MGLLINUX)
#include "mgllinux/internal.h"
#elif   defined(MGLX11)
#include "mglx11/internal.h"
#elif   defined(MGLDOS)
#include "mgldos/internal.h"
#elif   defined(MGLOS2)
#include "mglos2/internal.h"
#elif   defined(MGLRTT)
#include "mglrtt/internal.h"
#else
#error  MGL not ported to this platform yet!
#endif

#endif  /* __MGLDD_H */
