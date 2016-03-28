/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Simple program that loads the AGP driver and reports the
*               detected hardware configuration.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/agp.h"

/*----------------------------- Implementation ----------------------------*/

const char *GetApertureSizeName(
    int size)
{
    switch (size) {
        case agpSize4MB:    return "4MB";
        case agpSize8MB:    return "8MB";
        case agpSize16MB:   return "16MB";
        case agpSize32MB:   return "32MB";
        case agpSize64MB:   return "64MB";
        case agpSize128MB:  return "128MB";
        case agpSize256MB:  return "256MB";
        }
    return "Unknown!";
}

int main(void)
{
    AGP_devCtx      *dc;
    AGP_driverFuncs agp;
    AGP_configInfo  info;

    // Load the device driver for this device
    PM_init();
    if ((dc = AGP_loadDriver(0)) == NULL)
        PM_fatalError(AGP_errorMsg(AGP_status()));
    agp.dwSize = sizeof(agp);
    if (!dc->QueryFunctions(AGP_GET_DRIVERFUNCS,&agp))
        PM_fatalError("Unable to get device driver functions!");
    info.dwSize = sizeof(info);
    agp.GetConfigInfo(&info);
    printf(
        "Manufacturer:    %s\n"
        "Chipset:         %s\n"
        "Driver Revision: %s\n"
        "Driver Build:    %s\n",
        info.ManufacturerName, info.ChipsetName,
        info.VersionInfo, info.BuildDate);
    printf(
        "Aperture Base:   0x%08lX\n"
        "Aperture Size:   %s\n",
        agp.GetApertureBase(),
        GetApertureSizeName(agp.GetApertureSize()));
    AGP_unloadDriver(dc);
    return 0;
}

