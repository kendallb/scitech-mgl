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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------- Global variables ----------------------------*/

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
    printf("DDCTest - SciTech SNAP Graphics DDC reading Program\n");
    printf("          %s\n", SCITECH_COPYRIGHT_MSG_SHORT);
    printf("\n");
}

/****************************************************************************
REMARKS:
Display the raw EDID data on the screen
****************************************************************************/
static void DumpRawData(
    uchar *data,
    int size,
    char *msg)
{
    int     i;
    char    *p,buf[80],buf2[10];

    if (msg)
        printf("%s\n", msg);
    for (i = 0; i < size; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf("%s\n", buf);
            sprintf(buf2,"  %02X: ", i);
            strcpy(buf,buf2);
            p = buf + strlen(buf2);
            }
        sprintf(buf2,"%02X ", data[i]);
        strcat(buf,buf2);
        p += strlen(buf2);
        }
    printf("%s\n\n", buf);
}

/****************************************************************************
REMARKS:
Returns the display type as a string
****************************************************************************/
static char *GetDisplayType(
    uint type)
{
    static char *displayTypes[] = {
        "GrayScale",
        "RGB Color",
        "NonRGB Color",
        };
    if (type <= 2)
        return displayTypes[type];
    return "Uknown";
}

/****************************************************************************
REMARKS:
Returns the signal level as a string
****************************************************************************/
static char *GetSignalLevel(
    uint type)
{
    static char *signalLevels[] = {
        "0.700 - 0.300 (1.0V p-p) Signal Level",
        "0.714 - 0.286 (1.0V p-p) Signal Level",
        "1.000 - 0.400 (1.4V p-p) Signal Level",
        "0.700 - 0.300 (0.7V p-p) Signal Level",
        "Digital Signal",
        };
    if (type <= 4)
        return signalLevels[type];
    return "Uknown Signal Level";
}

/****************************************************************************
REMARKS:
Returns Yes or No as a string
****************************************************************************/
static char *YesNo(
    int value)
{
    return value ? "Yes" : "No";
}

/****************************************************************************
REMARKS:
Dumps the interpeted EDID data to the screen.
****************************************************************************/
static void DisplayEDIDData(
    uchar *edid,
    int size)
{
    char        buf[80];
    EDID_record rec;

    if (EDID_parse(edid,&rec,false)) {
        printf("Extracted monitor information (EDID Version %d.%d):\n", rec.version >> 8,rec.version & 0xFF);
        printf("  Manufacturer:          %s (%s)\n", rec.mfrName, rec.mfrID);
        printf("  Model name:            %s (%d Numeric Mode Code)\n", rec.modelName, rec.productID);
        printf("  Plug and Play ID:      %s\n", rec.PNPID);
        printf("  Serial Number:         %s (%ld Numeric ID)\n", rec.serialNo, rec.serialID);
        printf("  Manufactured:          Week %d of %d\n", rec.mfrWeek, rec.mfrYear);
        printf("  Display Type:          %s, %s\n", GetDisplayType(rec.displayType), GetSignalLevel(rec.signalLevel));
        printf("  Display Size:          %d x %d cm (%.2f x %.2f in)\n",
            rec.maxHSize, rec.maxVSize,
            rec.maxHSize / 2.54, rec.maxVSize / 2.54);
        strcpy(buf,"  Max resolution:        ");
        if ((rec.signalLevel == EDID_Level_Digital) && rec.numDetailedTimings > 0) {
            char bufres[16];
            sprintf(bufres,"%dx%d", rec.detailedTimings[0].hActive, rec.detailedTimings[0].vActive);
            strcat(buf, bufres);
            }
        else switch (rec.maxResolution) {
            case MaxRes_640x480:    strcat(buf,"640x480");      break;
            case MaxRes_800x600:    strcat(buf,"800x600");      break;
            case MaxRes_1024x768:   strcat(buf,"1024x768");     break;
            case MaxRes_1152x864:   strcat(buf,"1152x864");     break;
            case MaxRes_1280x1024:  strcat(buf,"1280x1024");    break;
            case MaxRes_1600x1200:  strcat(buf,"1600x1200");    break;
            case MaxRes_1920x1440:  strcat(buf,"1920x1440");    break;
            case MaxRes_2048x1536:  strcat(buf,"2048x1536");    break;
            default:                strcat(buf,"Unknown!");     break;
            }
        printf("%s\n", buf);
        printf("  Horizontal scan:       min: %2d kHz, max: %3d kHz\n", rec.minHScan, rec.maxHScan);
        printf("  Vertical scan:         min: %2d Hz,  max: %3d Hz\n", rec.minVScan, rec.maxVScan);
        printf("  Maximum pixel clock:   %d MHz\n", rec.maxPClk);
        if (rec.flags & EDID_DPMSEnabled)
            printf("  DPMS Support:          Stand-By: %s, Suspend %s, Off: %s\n",
                YesNo(rec.flags & EDID_DPMSStandBy),
                YesNo(rec.flags & EDID_DPMSSuspend),
                YesNo(rec.flags & EDID_DPMSOff));
        else
            printf("  DPMS Support:          None\n");
        printf("  GTF support:           %s\n", YesNo(rec.flags & EDID_GTFEnabled));
        printf("  DDC2AB support:        %-3s      Blank to Blank needed: %s\n",
            YesNo(rec.flags & EDID_DDC2AB),
            YesNo(rec.flags & EDID_Blank2Blank));
        printf("  Separate Syncs:        %-3s      Composite Sync:        %s\n",
            YesNo(rec.flags & EDID_SyncSeparate),
            YesNo(rec.flags & EDID_SyncComposite));
        printf("  Sync On Green:         %-3s      Need Serration:        %s\n",
            YesNo(rec.flags & EDID_SyncOnGreen),
            YesNo(rec.flags & EDID_NeedSerration));
        printf("  Gamma Correction:      %.2f\n", rec.gamma / 65536.0);
        printf("  Chromaticity Data:     Rx: %.3f, Ry: %.3f, Gx: %.3f, Gy: %.3f, \n",
            rec.Rx / 65536.0, rec.Ry / 65536.0, rec.Gx / 65536.0, rec.Gy / 65536.0);
        printf("                         Bx: %.3f, By: %.3f, Wx: %.3f, Wy: %.3f, \n",
            rec.Bx / 65536.0, rec.By / 65536.0, rec.Wx / 65536.0, rec.Wy / 65536.0);
        }
    else {
        printf("EDID data is invalid!\n");
        }
}

/****************************************************************************
REMARKS:
Load the SNAP driver and intialise it.
****************************************************************************/
static GA_devCtx *LoadDriver(
    int deviceIndex)
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
    N_int32         channel = SCI_channelMonitorPrimary;

    /* Command line option for DDC channel */
    if (argc > 1) {
        channel = atoi(argv[1]);
        }

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
    printf("Doing DDC for %s %s (device %d, channel %d):\n\n",
        info.ManufacturerName, info.ChipsetName,
        dc->DeviceIndex, channel);

    /* Detect if the SCI interface is active */
    sci.dwSize = sizeof(sci);
    if (DDC_queryFunctions(dc,GA_GET_SCIFUNCS,&sci) && sci.SCIdetect(&capabilities,&numMonitorPorts)) {
        /* Initialise DDC2B communications */
        if (DDC_initExt(dc,channel) == 0) {
            /* Read EDID data */
            DDC_readEDID(0xA0,edid,sizeof(edid),0,channel);

            /* Display the RAW EDID data on the screen */
            DumpRawData(edid,sizeof(edid),"Raw EDID data from I2C address A0h:");

            /* Now parse it and try to make some sense out of it */
            DisplayEDIDData(edid,sizeof(edid));
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

