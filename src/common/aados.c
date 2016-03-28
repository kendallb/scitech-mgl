/****************************************************************************
*
*                     SciTech SNAP Audio Architecture
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
* Environment:  DOS
*
* Description:  OS specific SciTech SNAP Audio Architecture services for
*               DOS.
*
****************************************************************************/

#include "pm_help.h"
#include "pmapi.h"
#include "snap/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
PARAMETERS:
path    - Local path to the SciTech SNAP Audio driver files.

REMARKS:
This function is used by the application program to override the location
of the SciTech SNAP Audio driver files that are loaded. Normally the loader code
will look in the system SciTech SNAP Audio directories first, then in the 'drivers'
directory relative to the current working directory, and finally relative
to the MGL_ROOT environment variable.
****************************************************************************/
void NAPI AA_setLocalPath(
    const char *path)
{
    // TODO: We really need a separate local path for AA drivers!
    PM_setLocalBPDPath(path);
}

/****************************************************************************
RETURNS:
Pointer to the system wide PM library imports, or the internal version if none

REMARKS:
Nothing to do here for DOS. Basically since DOS has no system wide shared
library mechanism we are essentially screwed if the binary API changes.
By default for 32-bit DOS apps the local SciTech SNAP Audio drivers should always be
used in preference to the system wide SciTech SNAP Audio drivers.
****************************************************************************/
PM_imports * NAPI AA_getSystemPMImports(void)
{
    return &_PM_imports;
}

