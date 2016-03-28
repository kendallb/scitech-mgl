/****************************************************************************
*
*                          SciTech SNAP Graphics
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
* Environment:  MSDOS, Win16, Win32
*
* Description:  Version number information for SciTech SNAP Graphics.
*
****************************************************************************/

#ifndef  NO_BUILD
#include "snap/graphics/snapbld.h"
#endif
#include "snap/graphics/snapver.h"

/* Release numbers for this release */

#ifndef NO_COPYRIGHT
#include "snap/copyrigh.h"
static char *copyright_str = SCITECH_COPYRIGHT_MSG;
#endif

static char *release_major = SNAP_RELEASE_MAJOR_STR;
#if defined(BETA)
static char *release_minor = SNAP_RELEASE_MINOR_STR " beta";
#else
static char *release_minor = SNAP_RELEASE_MINOR_STR;
#endif
#ifndef NO_BUILD
#ifndef NO_RELEASE_DATE
static char *release_date = __DATE__ " - Build " BUILD_NUMBER;
#ifndef NO_RELEASE_DATE2
static char *release_date2 = __DATE__;
#endif
#endif
#else
#ifndef NO_RELEASE_DATE
static char *release_date = __DATE__;
#ifndef NO_RELEASE_DATE2
static char *release_date2 = __DATE__;
#endif
#endif
#endif
