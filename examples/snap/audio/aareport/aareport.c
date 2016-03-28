/****************************************************************************
*
*                      SciTech SNAP Audio Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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
* Description:  Simple tool to report detected audio device.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/audio.h"

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

/*----------------------------- Implementation ----------------------------*/

int main(int argc,char *argv[])
{
    int             deviceIndex = 0;
    AA_devCtx       *dc;
    AA_initFuncs    init;
    AA_configInfo   info;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    AA_registerLicense(OemLicense);
#endif

    /* Get device index from command line */
    if (argc > 1)
        deviceIndex = atoi(argv[1]);

    /* Load the device driver for this device */
    if ((dc = AA_loadDriver(deviceIndex)) == NULL)
        PM_fatalError(AA_errorMsg(AA_status()));
    init.dwSize = sizeof(init);
    if (!AA_queryFunctions(dc,AA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);

    printf("\n"
           "Vendor Name:     %s\n"
           "Copyright:       %s\n\n"
           "Manufacturer:    %s\n"
           "Chipset:         %s\n"
           "Audio Codec:     %s\n"
           "Driver Revision: %d.%d, %s\n"
           "Driver Build:    %s\n",
           dc->OemVendorName, dc->OemCopyright, info.ManufacturerName, info.ChipsetName, info.CodecName,
           (int)dc->Version >> 8, (int)dc->Version & 0xFF, info.VersionInfo, info.BuildDate);

    /* Unload the device driver for this device */
    AA_unloadDriver(dc);

    return 0;
}
