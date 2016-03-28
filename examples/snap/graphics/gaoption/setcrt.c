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
* Description:  Simple program that allows the SciTech SNAP Graphics
*               option for CRT display output to be changed or queried.
*
*               The SNAP Graphics chipset driver will normally attempt
*               to detect attached output devices for CRT, LCD, DFP, or TV
*               via various schemes such as BIOS calls or chipset specific
*               register contents, some of which may provide detection info
*               persistent from bootup time. The chipset specific option for
*               display output can be overidden for CRTs attached later,
*               allowing wider range of display modes and refresh rates.
*
* Note:         This particular version sets chipset specific options
*               which should only be performed by advanced users!
*
****************************************************************************/

#include "snap/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------- Global Variables ---------------------------*/

static GA_devCtx    *dc = NULL;
static GA_initFuncs init;
static GA_options   opt;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Display the help screen.
****************************************************************************/
static void help(void)
{
    printf("\nUsage:\n\n"
           "  setcrt show\n"
           "  setcrt on|off\n");
    exit(-1);
}

/****************************************************************************
REMARKS:
Show the current setting for the chipset specific option for display output.
Chipset specific options require that the SNAP Graphics driver be loaded.
****************************************************************************/
static void ShowSettings(
    void)
{
    printf("Current Output Settings = CRT: %s, LCD: %s, DFP: %s, TV: %s\n\n",
           init.GetDisplayOutput() & gaOUTPUT_CRT ? "on" : "off",
           init.GetDisplayOutput() & gaOUTPUT_LCD ? "on" : "off",
           init.GetDisplayOutput() & gaOUTPUT_DFP ? "on" : "off",
           init.GetDisplayOutput() & gaOUTPUT_TV  ? "on" : "off");
}

/****************************************************************************
REMARKS:
Change the current setting for the chipset specific option for display output.
Chipset specific options require that the SNAP Graphics driver be loaded.
****************************************************************************/
static void ChangeSettings(
    ibool enable)
{
    int         flags, cntMode = init.GetVideoMode();
    N_int32     virtualX = -1,virtualY = -1,bytesPerLine = -1,maxMem = -1;
    GA_CRTCInfo crtc;

    /* Set the flags for available devices */
    flags = gaOUTPUT_CRT;
    if (dc->Attributes & gaHaveTVOutput)
        flags |= gaOUTPUT_TV;
    if (dc->Attributes & gaHaveLCDOutput)
        flags |= gaOUTPUT_LCD;
    if (dc->Attributes & gaHaveDFPOutput)
        flags |= gaOUTPUT_DFP;

    /* Set the output device(s) */
    if (enable)
        init.SetDisplayOutput(gaOUTPUT_CRT);
    else
        init.SetDisplayOutput(flags);

    /* Save the display options to disk */
    init.GetOptions(&opt);
    GA_saveOptions(dc,&opt);

    /* Reset current display mode */
    init.SetVideoMode(cntMode | gaDontClear,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,&crtc);

    ShowSettings();
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(
    int argc,
    char *argv[])
{
    /* Load the driver */
    if (!dc) {
        if ((dc = GA_loadDriver(0,false)) == NULL)
            PM_fatalError(GA_errorMsg(GA_status()));
        init.dwSize = sizeof(init);
        if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
            PM_fatalError("Unable to get device driver functions!");
        opt.dwSize = sizeof(opt);
        init.GetOptions(&opt);
        }

    /* Now handle the query */
    if (argc >= 2) {
        if (stricmp(argv[1],"show") == 0)
            ShowSettings();
        else if (stricmp(argv[1],"on") == 0)
            ChangeSettings(true);
        else if (stricmp(argv[1],"off") == 0)
            ChangeSettings(false);
        else
            help();
        }
    else
        help();

    /* Unload the driver */
    if (dc)
        GA_unloadDriver(dc);

    return 0;
}

