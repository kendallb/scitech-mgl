/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture DPMS and DDC functions.
*
****************************************************************************/

#include "gatest.h"

/*----------------------------- Implementation ----------------------------*/

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

    ClearPage(0);
    gmoveto(40,40);
    if (msg) {
        gprintf(msg);
        gnewline();
        }
    for (i = 0; i < size; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                gprintf(buf);
            sprintf(buf2,"  %02X: ", i);
            strcpy(buf,buf2);
            p = buf + strlen(buf2);
            }
        sprintf(buf2,"%02X ", data[i]);
        strcat(buf,buf2);
        p += strlen(buf2);
        }
    gprintf(buf);
    gnewline();
    if (msg) {
        gprintf("Press any key to interpret data");
        EVT_getch();
        }
}

/****************************************************************************
REMARKS:
Dump the raw EDID data to a text file.
****************************************************************************/
static void DumpEdidToFile(
    uchar *data,
    int size)
{
    int     i;
    char    *p,buf[80],buf2[10];
    FILE    *f = fopen("edid.txt","w");

    for (i = 0; i < size; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                fprintf(f,"%s\n",buf);
            sprintf(buf2,"  %02X: ", i);
            strcpy(buf,buf2);
            p = buf + strlen(buf2);
            }
        sprintf(buf2,"%02X ", data[i]);
        strcat(buf,buf2);
        p += strlen(buf2);
        }
    fprintf(f,"%s\n",buf);
    fclose(f);

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
static char *GetSignalLevel(uint type)
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
static char *YesNo(int value)
{ return value ? "Yes" : "No"; }

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
        ClearPage(0);
        gmoveto(40,40);
        sprintf(buf,"Extracted monitor information (EDID Version %d.%d):", rec.version >> 8,rec.version & 0xFF);
        gprintf(buf);
        gnewline();
        sprintf(buf,"  Manufacturer:          %s (%s)", rec.mfrName, rec.mfrID);
        gprintf(buf);
        sprintf(buf,"  Model name:            %s (%d Numeric Mode Code)", rec.modelName, rec.productID);
        gprintf(buf);
        sprintf(buf,"  Plug and Play ID:      %s", rec.PNPID);
        gprintf(buf);
        sprintf(buf,"  Serial Number:         %s (%ld Numeric ID)", rec.serialNo, rec.serialID);
        gprintf(buf);
        sprintf(buf,"  Manufactured:          Week %d of %d", rec.mfrWeek, rec.mfrYear);
        gprintf(buf);
        sprintf(buf,"  Display Type:          %s, %s", GetDisplayType(rec.displayType), GetSignalLevel(rec.signalLevel));
        gprintf(buf);
        sprintf(buf,"  Display Size:          %d x %d cm (%.2f x %.2f in)",
            rec.maxHSize, rec.maxVSize,
            rec.maxHSize / 2.54, rec.maxVSize / 2.54);
        gprintf(buf);
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
        gprintf(buf);
        sprintf(buf,"  Horizontal scan:       min: %2d kHz, max: %3d kHz", rec.minHScan, rec.maxHScan);
        gprintf(buf);
        sprintf(buf,"  Vertical scan:         min: %2d Hz,  max: %3d Hz", rec.minVScan, rec.maxVScan);
        gprintf(buf);
        sprintf(buf,"  Maximum pixel clock:   %d MHz", rec.maxPClk);
        gprintf(buf);
        if (rec.flags & EDID_DPMSEnabled)
            sprintf(buf,"  DPMS Support:          Stand-By: %s, Suspend %s, Off: %s",
                YesNo(rec.flags & EDID_DPMSStandBy),
                YesNo(rec.flags & EDID_DPMSSuspend),
                YesNo(rec.flags & EDID_DPMSOff));
        else
            sprintf(buf,"  DPMS Support:          None");
        gprintf(buf);
        sprintf(buf,"  GTF support:           %s", YesNo(rec.flags & EDID_GTFEnabled));
        gprintf(buf);
        sprintf(buf,"  DDC2AB support:        %-3s      Blank to Blank needed: %s",
            YesNo(rec.flags & EDID_DDC2AB),
            YesNo(rec.flags & EDID_Blank2Blank));
        gprintf(buf);
        sprintf(buf,"  Separate Syncs:        %-3s      Composite Sync:        %s",
            YesNo(rec.flags & EDID_SyncSeparate),
            YesNo(rec.flags & EDID_SyncComposite));
        gprintf(buf);
        sprintf(buf,"  Sync On Green:         %-3s      Need Serration:        %s",
            YesNo(rec.flags & EDID_SyncOnGreen),
            YesNo(rec.flags & EDID_NeedSerration));
        gprintf(buf);
        sprintf(buf,"  Gamma Correction:      %.2f", rec.gamma / 65536.0);
        gprintf(buf);
        sprintf(buf,"  Chromaticity Data:     Rx: %.3f, Ry: %.3f, Gx: %.3f, Gy: %.3f, ",
            rec.Rx / 65536.0, rec.Ry / 65536.0, rec.Gx / 65536.0, rec.Gy / 65536.0);
        gprintf(buf);
        sprintf(buf,"                         Bx: %.3f, By: %.3f, Wx: %.3f, Wy: %.3f, ",
            rec.Bx / 65536.0, rec.By / 65536.0, rec.Wx / 65536.0, rec.Wy / 65536.0);
        gprintf(buf);
        }
    else {
        gprintf("EDID data is invalid!");
        }
    gnewline();
    gprintf("Press any key to continue");
    if (EVT_getch() == 'd')
        DumpEdidToFile(edid,size);
}

/****************************************************************************
REMARKS:
Tests the reading and interpretation of EDID data from the 0xA0 address (the
regular DDC2B EDID address).
****************************************************************************/
static void testEDID_A0(
    GA_devCtx *dc,
    int channel)
{
    uchar edid[128];

    /* Initialise DDC2B communications */
    if (DDC_initExt(dc,channel) != ddcOk) {
        gnewline();
        gprintf("Could not perform DDC2B communications!");
        gnewline();
        gprintf("Press any key to continue");
        EVT_getch();
        return;
        }

    /* Read EDID data */
    DDC_readEDID(0xA0,edid,sizeof(edid),0,channel);

    /* Display the RAW EDID data on the screen */
    DumpRawData(edid,sizeof(edid),"Raw EDID data from I2C address A0h:");

    /* Now parse it and try to make some sense out of it */
    DisplayEDIDData(edid,sizeof(edid));
}

#ifdef  PRO

/****************************************************************************
REMARKS:
Write the raw EDID data to a text file.
****************************************************************************/
static void storeEDID_A0(
    GA_devCtx *dc,
    int channel)
{
    uchar edid[128];

    /* Initialise DDC2B communications */
    if (DDC_initExt(dc,channel) != ddcOk) {
        gnewline();
        gprintf("Could not perform DDC2B communications!");
        gnewline();
        gprintf("Press any key to continue");
        EVT_getch();
        return;
        }

    /* Read EDID data */
    DDC_readEDID(0xA0,edid,sizeof(edid),0,channel);

    /* Write the RAW EDID data to edid.txt */
    DumpEdidToFile(edid,sizeof(edid));
}

/****************************************************************************
REMARKS:
Read the raw EDID data from a text file.
****************************************************************************/
static void ReadEdidFromFile(
    uchar *data,
    int size)
{
    int     i,j,value;
    char    *p,buf[256];
    FILE    *f = fopen("edid.txt","r");

    if (f) {
        i = 0;
        while (!feof(f)) {
            if (fgets(buf,sizeof(buf),f)) {
                /* Read 16 bytes of data */
                p = buf + 6;
                for (j = 0; j < 16; j++) {
                    sscanf(p,"%X",&value);
                    data[i++] = value;
                    p += 3;
                    }
                if (i == size)
                    break;
                }
            }
        fclose(f);
        }
}

/****************************************************************************
PARAMETERS:
dc      - Device context to write EDID data to
erase   - True to erase the EDID in the monitor to all zeroes

REMARKS:
Function to write a block of EDID data to the monitor.
****************************************************************************/
static void writeEDID_A0(
    GA_devCtx *dc,
    int erase,
    int channel)
{
    uchar           edid[128];
    GA_modeInfo     mi;
    GA_CRTCInfo     crtc;
    int             i,flags;
    N_int32         virtualX = -1,virtualY = -1,bytesPerLine = -1,maxMem = -1;

    /* Set 640x480 @ 70Hz with the sync polarity set to '-'. This allows us
     * to ensure that the VSYNC will be held high while we write the EDID
     * data, disabling the write protection for the EDID non-volatile memory.
     */
    ExitSoftwareRasterizer();
    mi.dwSize = sizeof(mi);
    if (init.GetCustomVideoModeInfo(640,480,-1,-1,8,&mi) != 0)
        PM_fatalError("Unable to get info for 640x480x8!");
    flags = gaRefreshCtrl;
    if (mi.Attributes & gaHaveLinearBuffer)
        flags |= gaLinearBuffer;
    if (!GA_computeCRTCTimings(dc,init.GetActiveHead(),&mi,70,false,&crtc,false))
        PM_fatalError("Unable to get timings for 640x480 @ 70Hz!");
    crtc.Flags |= gaVSyncNeg;
    if (init.SetCustomVideoMode(640,480,8,flags,&virtualX,&virtualY,&bytesPerLine,&maxMem,&crtc) != 0)
        PM_fatalError("Unable to set 640x480x8 @ 70Hz!");
    cntMode = flags;
    if (!InitSoftwareRasterizer(cntDevice,1,false))
        PM_fatalError("Unable to initialize reference rasteriser!");

    /* Read the EDID information from EDID.TXT */
    ReadEdidFromFile(edid,sizeof(edid));
    if (erase) {
        for (i = 0; i < sizeof(edid); i++)
            edid[i] = i;
        ClearPage(0);
        gmoveto(40,40);
        gprintf("Cleared EDID data:");
        gnewline();
        }

    /* Display the RAW EDID data on the screen */
    DumpRawData(edid,sizeof(edid),erase ? NULL : "Raw EDID data from file:");

    /* Now parse it to ensure it is valid */
    if (!erase)
        DisplayEDIDData(edid,sizeof(edid));
    gprintf("Verify the above is the correct EDID, and hit 'W' to write it!");
    if (EVT_getch() == 'W') {
        gnewline();
        if (DDC_writeEDID(dc,0xA0,edid,sizeof(edid),0,channel))
            gprintf("EDID data successfully written!");
        else
            gprintf("Could not write EDID data!");
        gnewline();
        gprintf("Press any key to continue");
        EVT_getch();
        }
}
#endif

/****************************************************************************
REMARKS:
Displays the menu of DDC tests and allows the DDC functionality to be
tested.
****************************************************************************/
void DDCTests(
    GC_devCtx *gc)
{
    int         choice;
    N_uint8     capabilities = 0;
    N_int32     numMonitorPorts;
    GA_SCIFuncs sci;

    /* Detect if the SCI interface is active */
    dc = gc->dc;
    sci.dwSize = sizeof(sci);
    if (!GA_queryFunctions(dc,GA_GET_SCIFUNCS,&sci) || !sci.SCIdetect(&capabilities,&numMonitorPorts)) {
        /* Try legacy DDC via the BIOS (which our drivers support) */
        if (DDC_init(dc) != ddcOk) {
            GC_printf(gc,"DDC services not available...");
            EVT_getch();
            return;
            }
        }

    /* For NonVGA controllers we need to run this test in graphics modes
     * to ensure that we are running on the proper controller hardware
     * that we support DPMS on (ie: the Windows driver hardware).
     */
    SetGraphicsMode(gc);

    /* Display menu information on screen */
    for (;;) {
        ClearPage(0);
        gmoveto(40,40);
        gprintf("DDC support active. Supports the following attributes:");
        if (!capabilities)
            gprintf("  Legacy BIOS Services");
        else {
            if (capabilities & SCI_writeSCL)
                gprintf("  Write SCL supported");
            if (capabilities & SCI_writeSDA)
                gprintf("  Write SDA supported");
            if (capabilities & SCI_readSCL)
                gprintf("  Read SCL supported");
            if (capabilities & SCI_readSDA)
                gprintf("  Read SDA supported");
            }
        gnewline();
        gprintf("Enter function to test:");
        gprintf("  [0] - Read EDID from primary monitor");
        if (dc->Attributes & gaHaveMultiHead)
            gprintf("  [1] - Read EDID from secondary monitor");
#ifdef  PRO
        gprintf("  [s] - Store EDID for primary display to file (edid.txt)");
        gprintf("  [w] - Write EDID for primary display from file (edid.txt)");
        gprintf("  [C] - Erase EDID for primary display");
#endif
        gprintf("  [Esc] - Quit");
        gnewline();
        gprintf("Which: ");
        choice = EVT_getch();
        if (choice == 0x1B || tolower(choice) == 'q')
            break;
        switch (choice) {
            case '0': testEDID_A0(dc,SCI_channelMonitorPrimary);        break;
            case '1': testEDID_A0(dc,SCI_channelMonitorSecondary);      break;
#ifdef  PRO
            case 's': storeEDID_A0(dc,SCI_channelMonitorPrimary);       break;
            case 'w': writeEDID_A0(dc,false,SCI_channelMonitorPrimary); break;
            case 'C': writeEDID_A0(dc,true,SCI_channelMonitorPrimary);  break;
#endif
            }
        }

    /* Restore text mode for NonVGA controllers */
    RestoreMode(gc);
}

