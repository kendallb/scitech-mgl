/****************************************************************************
*
*                       SciTech SNAP Graphics DDC Driver
*
*               Copyright (C) 1997-1998 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Module to implement parsing of the EDID monitor information
*               blocks, as well as constructing correct EDID information
*               from passed in information. All information is extracted
*               from the binary EDID information block into the EDID_record
*               structure.
*
****************************************************************************/

#include "edidfmt.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
type    - Type code for the diplay type

RETURNS:
String describing the display type.
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
PARAMETERS:
type    - Type code for the diplay type

RETURNS:
String describing the signal level type.
****************************************************************************/
static char *GetSignalLevel(
    uint type)
{
    static char *signalLevels[] = {
        "0.700-0.300 (1.0V p-p)",
        "0.714-0.286 (1.0V p-p)",
        "1.000-0.400 (1.4V p-p)",
        "0.700-0.300 (0.7V p-p)",
        "Digital Signal",
        };
    if (type <= 4)
        return signalLevels[type];
    return "Uknown Signal Level";
}

/****************************************************************************
PARAMETERS:
value   - Boolean value to check

RETURNS:
String value of either Yes or No.
****************************************************************************/
static char *YesNo(
    int value)
{ return value ? "Yes" : "No"; }

/****************************************************************************
PARAMETERS:
xSize   - X dimension for the viewable image
ySize   - Y dimension for the viewable image

RETURNS:
Returns the viewable image size for the monitor.
****************************************************************************/
static double GetViewable(
    float xSize,
    float ySize)
{
    return sqrt(xSize*xSize + ySize*ySize);
}

/****************************************************************************
PARAMETERS:
edid    - EDID record to parse

RETURNS:
Name of the maximum resolution supported by the monitor from the EDID.
****************************************************************************/
static const char *GetMaxResolution(
    EDID_record *edid)
{
    static char     buf[80];

    switch (edid->maxResolution) {
        case MaxRes_640x480:    strcpy(buf,"640x480");      break;
        case MaxRes_800x600:    strcpy(buf,"800x600");      break;
        case MaxRes_1024x768:   strcpy(buf,"1024x768");     break;
        case MaxRes_1152x864:   strcpy(buf,"1152x864");     break;
        case MaxRes_1280x1024:  strcpy(buf,"1280x1024");    break;
        case MaxRes_1600x1200:  strcpy(buf,"1600x1200");    break;
        case MaxRes_1920x1440:  strcpy(buf,"1920x1440");    break;
        case MaxRes_2048x1536:  strcpy(buf,"2048x1536");    break;
        default:                strcpy(buf,"Unknown!");     break;      }
    return buf;
}

/****************************************************************************
PARAMETERS:
flags   - Flags for the standard timing

RETURNS:
Name of the timing flag
****************************************************************************/
static const char *GetTimingName(
    uchar flags)
{
    const char *names[] = {
        "VGA Compatible",
        "XGA Compatible",
        "Macinstosh Compatible",
        "VESA Standard",
        };
    if (flags < EDID_MaxTimingType)
        return names[flags];
    return "Unknown";
}

/****************************************************************************
DESCRIPTION:
Formats the EDID information into a readable text file on disk.

PARAMETERS:
edid        - EDID information block to parse
filename    - Name of the file to format data to

RETURNS:
True on success, false on error.

REMARKS:
This function takes the parsed EDID record, and formats the data to a text
file that can be read directly by the end user.
****************************************************************************/
ibool EDID_format(
    EDID_record *edid,
    char *filename)
{
    int     i;
    FILE    *f;

    /* Dump the parsed EDID information to our output file */
    if ((f = fopen(filename,"wt")) == NULL)
        return false;
    fprintf(f,"General Information (EDID Version %d.%d):\n", edid->version >> 8,edid->version & 0xFF);
    fprintf(f,"\n");
    fprintf(f,"  Manufacturer Name:    %s\n", edid->mfrName);
    fprintf(f,"  Monitor Name:         %s\n", edid->modelName);
    fprintf(f,"  Serial Number:        %s\n", edid->serialNo);
    fprintf(f,"  PnP ID:               %s\n", edid->PNPID);
    fprintf(f,"  Manufacture Date:     Week %d of %d\n", edid->mfrWeek, edid->mfrYear);
    fprintf(f,"  Maximum Image Size:   %d x %d cm (%.1f x %.1f in)\n",
        edid->maxHSize, edid->maxVSize,
        edid->maxHSize / 2.54, edid->maxVSize / 2.54);
    fprintf(f,"  Viewable Image Size:  %.1f cm (%.1f in)\n",
        GetViewable(edid->maxHSize, edid->maxVSize),
        GetViewable(edid->maxHSize, edid->maxVSize) / 2.54);
    fprintf(f,"  Display Type:         %s\n", GetDisplayType(edid->displayType));
    fprintf(f,"  Signal Level:         %s\n", GetSignalLevel(edid->signalLevel));
    fprintf(f,"  Sync Types Supported: ");
    if (edid->flags & EDID_SyncSeparate) {
        fprintf(f,"Separate Syncs");
        if (edid->flags & (EDID_SyncSeparate | EDID_SyncOnGreen))
            fprintf(f,", ");
        }
    if (edid->flags & EDID_SyncComposite) {
        fprintf(f,"Composite Sync");
        if (edid->flags & EDID_SyncOnGreen)
            fprintf(f,", ");
        }
    if (edid->flags & EDID_SyncOnGreen)
        fprintf(f,"Sync On Green");
    fprintf(f,"\n");
    fprintf(f,"\n");
    fprintf(f,"Feature Support:\n");
    fprintf(f,"\n");
    fprintf(f,"  DPMS Support:         ");
    if (edid->flags & EDID_DPMSEnabled) {
        if (edid->flags & EDID_DPMSStandBy)
            fprintf(f,"Stand-By, ");
        if (edid->flags & EDID_DPMSSuspend)
            fprintf(f,"Suspend, ");
        if (edid->flags & EDID_DPMSOff)
            fprintf(f,"Off");
        fprintf(f,"\n");
        }
    else
        fprintf(f,"None\n");
    fprintf(f,"  GTF Support:          %s\n", YesNo(edid->flags & EDID_GTFEnabled));
    fprintf(f,"\n");
    fprintf(f,"Monitor Range Limits:\n");
    fprintf(f,"\n");
    fprintf(f,"  Maximum Resolution:   %s\n", GetMaxResolution(edid));
    fprintf(f,"  Horizontal Scan:      min: %2d kHz, max: %3d kHz\n", edid->minHScan, edid->maxHScan);
    fprintf(f,"  Vertical Scan:        min: %2d Hz,  max: %3d Hz\n", edid->minVScan, edid->maxVScan);
    fprintf(f,"  Maximum Pixel Clock:  %d MHz\n", edid->maxPClk);
    fprintf(f,"\n");
    fprintf(f,"Standard Timings:\n");
    fprintf(f,"\n");
    for (i = 0; i < edid->numStandardTimings; i++) {
        fprintf(f,"  %4d x %4d @ %2d Hz %s\n",
            edid->standardTimings[i].xRes,
            edid->standardTimings[i].yRes,
            edid->standardTimings[i].Hz,
            GetTimingName(edid->standardTimings[i].flags));
        }
    fprintf(f,"\n");
    for (i = 0; i < edid->numDetailedTimings; i++) {
        fprintf(f,"Detailed Timing (%4d x %4d @ %2d Hz)",
            edid->detailedTimings[i].hActive,
            edid->detailedTimings[i].vActive,
            edid->detailedTimings[i].Hz);
        if (i == 0 && (edid->flags & EDID_PreferredTiming))
            fprintf(f," (Preferred)");
        fprintf(f,":\n");
        fprintf(f,"\n");
        fprintf(f,"  Horizontal Timings:\n");
        fprintf(f,"    Active              = %4d pixels\n", edid->detailedTimings[i].hActive);
        fprintf(f,"    Blank               = %4d pixels\n", edid->detailedTimings[i].hBlank);
        fprintf(f,"    Sync Offset         = %4d pixels\n", edid->detailedTimings[i].hSyncOffset);
        fprintf(f,"    Sync Width          = %4d pixels\n", edid->detailedTimings[i].hSyncWidth);
        fprintf(f,"    Border              = %4d pixels\n", edid->detailedTimings[i].hBorder);
        fprintf(f,"    Polarity            = %4c\n", edid->detailedTimings[i].hSyncPol);
        fprintf(f,"  Vertical Timings:\n");
        fprintf(f,"    Active              = %4d lines\n", edid->detailedTimings[i].vActive);
        fprintf(f,"    Blank               = %4d lines\n", edid->detailedTimings[i].vBlank);
        fprintf(f,"    Sync Offset         = %4d lines\n", edid->detailedTimings[i].vSyncOffset);
        fprintf(f,"    Sync Width          = %4d lines\n", edid->detailedTimings[i].vSyncWidth);
        fprintf(f,"    Border              = %4d lines\n", edid->detailedTimings[i].vBorder);
        fprintf(f,"    Polarity            = %4c\n", edid->detailedTimings[i].vSyncPol);
        fprintf(f,"  Pixel Clock           = %6.2f MHz\n", edid->detailedTimings[i].pixelClock / 100.0);
        fprintf(f,"  Horizontal Frequency  = %6.2f kHz\n", edid->detailedTimings[i].hFreq / 100.0);
        fprintf(f,"  Vertical Frequency    = %6d Hz\n", edid->detailedTimings[i].Hz);
        fprintf(f,"  Horizontal Size       = %d mm (%.2f in)\n", edid->detailedTimings[i].hSize, edid->detailedTimings[i].hSize / 25.4);
        fprintf(f,"  Vertical Size         = %d mm (%.2f in)\n", edid->detailedTimings[i].vSize, edid->detailedTimings[i].vSize / 25.4);
        fprintf(f,"\n");
        }
    fprintf(f,"Color Characteristics:\n");
    fprintf(f,"\n");
    fprintf(f,"  Red Chromaticities:   Rx: %.3f, Ry: %.3f\n", edid->Rx, edid->Ry);
    fprintf(f,"  Green Chromaticities: Gx: %.3f, Gy: %.3f\n", edid->Gx, edid->Gy);
    fprintf(f,"  Blue Chromaticities:  Bx: %.3f, By: %.3f\n", edid->Bx, edid->By);
    fprintf(f,"  Default White Point:  Wx: %.3f, Wy: %.3f\n", edid->Wx, edid->Wy);
    fprintf(f,"  Default Gamma:        %.2f\n", edid->gamma);
    fprintf(f,"\n");
    fclose(f);
    return true;
}

