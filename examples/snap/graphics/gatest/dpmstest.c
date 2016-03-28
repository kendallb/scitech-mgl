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
* Environment:  Any 32-bit protected choice environment
*
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture DPMS and DDC functions.
*
****************************************************************************/

#include "gatest.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Displays the menu of DPMS tests and allows the states to be tested.
****************************************************************************/
void DPMSTests(
    GC_devCtx *gc)
{
    int             choice,state;
    N_int16         supportedStates;
    GA_DPMSFuncs    dpms;

    /* Detect if the DPMS interface is active */
    dpms.dwSize = sizeof(dpms);
    if (!GA_queryFunctions(gc->dc,GA_GET_DPMSFUNCS,&dpms) || !dpms.DPMSdetect(&supportedStates) || supportedStates == 0) {
        GC_printf(gc,"\nDPMS services not available...\n");
        EVT_getch();
        return;
        }

    /* For NonVGA controllers we need to run this test in graphics modes
     * to ensure that we are running on the proper controller hardware
     * that we support DPMS on (ie: the Windows driver hardware).
     */
    SetGraphicsMode(gc);

    /* Display menu information on screen */
    gmoveto(40,40);
    gprintf("DPMS support active. Supports the following states:");
    if (supportedStates & DPMS_standby)
        gprintf("  STAND BY");
    if (supportedStates & DPMS_suspend)
        gprintf("  SUSPEND");
    if (supportedStates & DPMS_off)
        gprintf("  OFF");
    gnewline();
    gprintf("Enter which choice (ESC to quit):");
    gprintf("  [0]   - ON");
    gprintf("  [1]   - Stand-By");
    gprintf("  [2]   - Suspend");
    gprintf("  [3]   - Off");
    gprintf("  [Esc] - Quit");
    gnewline();
    gprintf("Which: ");

    /* Loop around trying the different DPMS states */
    for (;;) {
        choice = EVT_getch();
        if (choice == 0x1B || tolower(choice) == 'q')
            break;
        switch (choice) {
            case '1': state = DPMS_standby; break;
            case '2': state = DPMS_suspend; break;
            case '3': state = DPMS_off;     break;
            default:  state = DPMS_on;
            }
        dpms.DPMSsetState(state);
        }

    /* Reset to ON state before we exit! */
    dpms.DPMSsetState(DPMS_on);

    /* Restore text mode for NonVGA controllers */
    RestoreMode(gc);
}

