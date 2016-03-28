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
* Description:  Test program for PM_getSNAPPath, PM_findBPD.
*
****************************************************************************/

#include "ztimer.h"
#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>

/*----------------------------- Implementation ----------------------------*/

int main(void)
{
    char szBPDName[] = "graphics.bpd";
    char szBPDPath[PM_MAX_PATH];

    printf("The OS name is: %s\n", PM_getOSName());
    printf("The SNAP path is: %s\n", PM_getSNAPPath());
    printf("The SNAP config path is: %s\n", PM_getSNAPConfigPath());
    if (PM_findBPD(szBPDName, szBPDPath))
        printf("The graphics BPD path is found at: %s%s\n", szBPDPath, szBPDName);
    else
        printf("The graphics BPD path cannot be found.\n");
    return 0;
}

