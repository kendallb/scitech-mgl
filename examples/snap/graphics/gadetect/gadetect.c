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
* Description:  Simple program that detects the underlying hardware
*               configuration for SciTech SNAP Graphics. This program does
*               a quick detection of just the chipset manufacturer and
*               chipset name, using a small detection module that is easier
*               to download than the full driver. This module is intended
*               for technical support purposes.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/graphics.h"

/*---------------------------- Global Variables ---------------------------*/

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "detect.bpd"
#define MAX_DEVICES     4

/****************************************************************************
PARAMETERS:
numCards    - Place to return the number of graphics cards found
mfrNames    - Place to store the manufacturer names
chipNames   - Place to store the chipset names

HEADER:
snap/graphics/detect.h

RETURNS:
True if hardware is found, false if not.

REMARKS:
This function detects all the installed graphics graphics cards in the system
and returns the names of them in the mfrNames and chipNames fields. This is
used for quick detection on end user systems for support purposes. When you
call this function pass the maximum number of devices to return in numCards.
It will be modified on return to contain the actual number of devices
detected. Make sure the mfrNames and chipNames fields are big enough to hold
the number of devices passes in numCards.
****************************************************************************/
ibool NAPI GA_detectChipsets(
    int *numCards,
    char **mfrNames,
    char **chipNames)
{
    GA_detectChipsets_t GA_detectChipsets;
    PE_MODULE           *hModBPD;
    char                filename[PM_MAX_PATH];
    ibool               found = false;

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,filename))
        return -1;
    strcat(filename,DLL_NAME);
    if ((hModBPD = PE_loadLibrary(filename,false)) == NULL)
        return -1;
    if ((GA_detectChipsets = (GA_detectChipsets_t)PE_getProcAddress(hModBPD,"__GA_detectChipsets")) == NULL) {
        PE_freeLibrary(hModBPD);
        return -1;
        }
    found = GA_detectChipsets(filename,&_PM_imports,&_N_imports,numCards,mfrNames,chipNames);
    PE_freeLibrary(hModBPD);
    return found;
}

int main(int argc,char *argv[])
{
    int     i,detected,numCards = MAX_DEVICES;
    char    _mfrNames[MAX_DEVICES][80];
    char    _chipNames[MAX_DEVICES][80];
    char    *mfrNames[MAX_DEVICES];
    char    *chipNames[MAX_DEVICES];

    /* Initialise the lists of strings */
    for (i = 0; i < MAX_DEVICES; i++) {
        mfrNames[i] = &_mfrNames[i][0];
        chipNames[i] = &_chipNames[i][0];
        }

    /* Detect the graphics installed hardware */
    if ((detected = GA_detectChipsets(&numCards,mfrNames,chipNames)) <= 0) {
        if (detected == -1)
            printf("Unable to find hardware detection driver!\n");
        else
            printf("No compatible graphics hardware detected!\n");
        return -1;
        }

    /* Get configuration information and report it */
    printf("Compatible graphics hardware found!\n\n");
    for (i = 0; i < numCards; i++) {
        if (numCards > 1) {
            printf("Device %d:       ", i);
            }
        else
            printf("Primary device: ");
        printf("%s %s\n",mfrNames[i],chipNames[i]);
        }
    return 0;
}

