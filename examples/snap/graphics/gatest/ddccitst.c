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
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture DPMS and DDC functions.
*
****************************************************************************/

#include "gatest.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Removes the new line from a string
****************************************************************************/
static void removeNewLine(
    char *s)
{
    int len = strlen(s);
    if (s[len-1] == '\n')
        s[len-1] = 0;
}

/****************************************************************************
REMARKS:
Gets a string with the new line removed
****************************************************************************/
static void safeGetLine(
    GC_devCtx *gc,
    char *s,
    int len)
{
    GetString(gc,s,len);
    removeNewLine(s);
}

/****************************************************************************
REMARKS:
Tests the MCS_isControlSupported function
****************************************************************************/
void testMCS_isControlSupported(
    GC_devCtx *gc)
{
    int     controlCode;
    char    buf[80];

    gprintf("Enter control code: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%x",&controlCode);
    if (MCS_isControlSupported(controlCode))
        gprintf("Control is supported");
    else
        gprintf("Control is not supported");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_enableControl function
****************************************************************************/
void testMCS_enableControl(
    GC_devCtx *gc)
{
    int     controlCode,enable;
    char    buf[80];

    gprintf("Enter control code: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%x",&controlCode);
    gprintf("Enter enable code (0 or 1): ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%d",&enable);
    if (MCS_enableControl(controlCode,enable))
        gprintf("Control enable updated");
    else
        gprintf("MCS_enableControl failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_getControlValue function
****************************************************************************/
void testMCS_getControlValue(
    GC_devCtx *gc)
{
    int     controlCode;
    ushort  value,max;
    char    buf[80];

    gprintf("Enter control code: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%x",&controlCode);
    if (MCS_getControlMax(controlCode,&max) && MCS_getControlValue(controlCode,&value)) {
        gprintf("Control Values:");
        gprintf(" value = %d", value);
        gprintf(" max   = %d", max);
        }
    else
        gprintf("MCS_getControlValue failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_setControlValue function
****************************************************************************/
void testMCS_setControlValue(
    GC_devCtx *gc)
{
    int     controlCode;
    ushort  value;
    char    buf[80];

    gprintf("Enter control code: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%x",&controlCode);
    gprintf("Enter value: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%hd",&value);
    if (MCS_setControlValue(controlCode,value))
        gprintf("Control value updated");
    else
        gprintf("MCS_setControlValue failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_resetControl function
****************************************************************************/
void testMCS_resetControl(
    GC_devCtx *gc)
{
    int     controlCode;
    char    buf[80];

    gprintf("Enter control code: ");
    safeGetLine(gc,buf,sizeof(buf));
    sscanf(buf,"%x",&controlCode);
    if (MCS_resetControl(controlCode))
        gprintf("Control value reset to factory default");
    else
        gprintf("MCS_resetControl failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_getTimingReport function
****************************************************************************/
void testMCS_getTimingReport(void)
{
    uchar   flags;
    ushort  hFreq,vFreq;

    if (MCS_getTimingReport(&flags,&hFreq,&vFreq)) {
        gprintf("Timing Report:");
        gprintf(" flags = %02X", flags);
        gprintf(" hFreq = %.2f kHz", (hFreq+50.0) / 100.0);
        gprintf(" vFreq = %.2f Hz", (vFreq+50.0) / 100.0);
        gprintf(" hSync = %c", (flags & MCS_hSyncPositive) ? '+' : '-');
        gprintf(" vSync = %c", (flags & MCS_vSyncPositive) ? '+' : '-');
        }
    else
        gprintf("MCS_getTimingReport failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_saveCurrentSettings function
****************************************************************************/
void testMCS_saveCurrentSettings(void)
{
    if (MCS_saveCurrentSettings())
        gprintf("Current settings saved in monitor NVRAM");
    else
        gprintf("MCS_saveCurrentSettings failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Tests the MCS_getSelfTestReport function
****************************************************************************/
void testMCS_getSelfTestReport(void)
{
    uchar   flags,data[256],length;

    if (MCS_getSelfTestReport(&flags,data,&length)) {
        gprintf("Self Test Report:");
        gprintf(" flags  = %02X", flags);
        gprintf(" length = %d", length);
        }
    else
        gprintf("MCS_getSelfTestReport failed!");
    EVT_getch();
}

#define MAX_CHARS   70

void testMCS_getCapabilitiesString(void)
{
    int     len;
    char    c;
    char    data[1024];
    char    *p;

    if (MCS_getCapabilitiesString(data,sizeof(data)) != -1) {
        gprintf("Capabilities String:");
        len = strlen(data);
        p = data;
        while (len >= MAX_CHARS) {
            c = p[MAX_CHARS];
            p[MAX_CHARS] = 0;
            gprintf("%s", p);
            p[MAX_CHARS] = c;
            p += MAX_CHARS;
            len -= MAX_CHARS;
            }
        if (len)
            gprintf("%s", p);
        }
    else
        gprintf("MCS_getCapabilitiesString failed!");
    EVT_getch();
}

/****************************************************************************
REMARKS:
Displays the menu of DDC tests and allows the DDC functionality to be
tested.
****************************************************************************/
void DDCCITests(
    GC_devCtx *gc)
{
    int         choice,err = ddcOk;
    GA_SCIFuncs sci;

    /* For NonVGA controllers we need to run this test in graphics modes
     * to ensure that we are running on the proper controller hardware
     * that we support DPMS on (ie: the Windows driver hardware).
     */
    SetGraphicsMode(gc);

    /* Allow a 1.5 second delay before trying to do DDC/CI communications */
    _OS_delay(1500000);

    /* Initialise DDC/CI communications */
    sci.dwSize = sizeof(sci);
    if (!GA_queryFunctions(dc,GA_GET_SCIFUNCS,&sci) || (err = MCS_begin(dc)) != ddcOk) {
        RestoreMode(gc);
        GC_clrscr(gc);
        if (err == ddcNotAvailable)
            GC_printf(gc,"DDC/CI not supported by monitor!\n\n");
        else
            GC_printf(gc,"DDC monitor not connected!\n\n");
        GC_printf(gc,"Press any key to continue");
        EVT_getch();
        return;
        }

    /* Display menu information on screen */
    for (;;) {
        ClearPage(0);
        gmoveto(40,40);
        gprintf("DDC/CI support active:");
        gnewline();
        gprintf("Enter function to test:");
        gprintf("  [0] - Is Control Supported");
        gprintf("  [1] - Enable Control");
        gprintf("  [2] - Get Control Value");
        gprintf("  [3] - Set Control Value");
        gprintf("  [4] - Reset Control");
        gprintf("  [5] - Get Timing Report");
        gprintf("  [6] - Save Current Settings");
        gprintf("  [7] - Get Self Test Report");
        gprintf("  [8] - Get Capabilities String");
        gprintf("  [Esc] - Quit");
        gprintf("Which: ");

        /* Loop around trying the different DPMS states */
        choice = EVT_getch();
        if (choice == 0x1B || tolower(choice) == 'q')
            break;
        gnewline();
        switch (choice) {
            case '0': testMCS_isControlSupported(gc);       break;
            case '1': testMCS_enableControl(gc);            break;
            case '2': testMCS_getControlValue(gc);          break;
            case '3': testMCS_setControlValue(gc);          break;
            case '4': testMCS_resetControl(gc);             break;
            case '5': testMCS_getTimingReport();            break;
            case '6': testMCS_saveCurrentSettings();        break;
            case '7': testMCS_getSelfTestReport();          break;
            case '8': testMCS_getCapabilitiesString();      break;
            }
        }

    /* Close the DDC/CI communications channel */
    MCS_end();

    /* Restore text mode for NonVGA controllers */
    RestoreMode(gc);
}

