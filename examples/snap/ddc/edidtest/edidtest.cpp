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
* Description:  Simple program that performs DDC monitor detection using
*               the SciTech SNAP DDC API functions.
*
****************************************************************************/

#include "snap/ddclib.h"
#include "snap/copyrigh.h"
#include "edidfmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------- Global variables ----------------------------*/

#define EDID_FILENAME   "edid.txt"

#ifdef ISV_LICENSE
#include "isv.c"
#endif

GA_initFuncs    init;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the sign on banner.
****************************************************************************/
static void banner(void)
{
    printf("EDIDTest - SciTech SNAP Graphics DDC EDID reading Program\n");
    printf("          %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
    printf("\n");
}

/****************************************************************************
REMARKS:
Load the SNAP driver and intialise it.
****************************************************************************/
static GA_devCtx *LoadDriver(
    N_int32 deviceIndex)
{
    GA_devCtx *dc;

#ifdef ISV_LICENSE
    /* Register ISV license for SNAP DDC API */
    DDC_registerLicense(OemLicense);
#endif

    if ((dc = DDC_loadDriver(deviceIndex)) == NULL)
        PM_fatalError(DDC_errorMsg(DDC_status()));
    init.dwSize = sizeof(init);
    if (!DDC_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    return dc;
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    GA_devCtx       *dc = NULL;
    GA_configInfo   info;
    N_uint8         capabilities = 0;
    N_int32         numMonitorPorts;
    GA_SCIFuncs     sci;
    uchar           edid[128];
    EDID_record     rec;
    N_int32         channel = SCI_channelMonitorPrimary;

    /* Get command line option for SNAP DDC channel */
    if (argc > 1)
        channel = atoi(argv[1]);

    /* Now handle the query */
    dc = LoadDriver(0);
    if (!dc) {
        printf("Unable to load SNAP DDC driver!\n");
        return -1;
        }

    /* Display graphics driver info */
    banner();
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    printf("Doing DDC for %s %s (device %d) on channel %d:\n\n",
        info.ManufacturerName, info.ChipsetName,
        dc->DeviceIndex, channel);

    /* Detect if the SCI interface is active */
    sci.dwSize = sizeof(sci);
    if (DDC_queryFunctions(dc,GA_GET_SCIFUNCS,&sci) && sci.SCIdetect(&capabilities,&numMonitorPorts)) {
        /* Initialise DDC2B communications */
        if (DDC_initExt(dc,channel) == 0) {
            /* Read EDID data */
            DDC_readEDID(0xA0,edid,sizeof(edid),0,channel);
            /* Parse EDID data and dump into formatted text file */
            if (!EDID_parse(edid,&rec,false))
                printf("Unable to parse EDID data!\n");
            else if (!EDID_format(&rec,EDID_FILENAME))
                printf("Unable to format EDID data!\n");
            else
                printf("EDID data formatted into text file " EDID_FILENAME " \n");
            printf("DDC channel %d EDID: %s %s\n",channel,rec.mfrName,rec.modelName);
            }
        else {
            printf("Could not perform DDC2B communications!\n");
            }
        }
    else {
        printf("No DDC services available in driver!\n");
        }

    /* Unload SNAP and exit */
    DDC_unloadDriver(dc);
    return 0;
}

