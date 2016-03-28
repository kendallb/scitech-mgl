/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  Module to implement the code to test the SNAP Graphics
*               Architecture.
*
****************************************************************************/

#ifndef __PROFILE_H
#define __PROFILE_H

#include "snap/gasdk.h"

/*---------------------- Macros and type definitions ----------------------*/

typedef struct {
    float   intLinesPerSec;
    float   bresLinesPerSec;
    float   intStippleLinesPerSec;
    float   bresStippleLinesPerSec;
    float   clearsMbPerSec;
    float   monoPattMbPerSec;
    float   transMonoPattMbPerSec;
    float   colorPattMbPerSec;
    float   transColorPattMbPerSec;
    float   putMonoImageMbPerSec;
    float   bitBltMbPerSec;
    float   bitBltSysMbPerSec;
    float   readsCPUMbPerSec;
    float   getImagesCPUMbPerSec;
    float   putImagesCPUMbPerSec;
    float   clearCPUMbPerSec;
    float   reverseClearCPUMbPerSec;
    float   clearBaseLineMbPerSec;
    float   reverseClearBaseLineMbPerSec;
    float   readBaseLineMbPerSec;
    float   copyBaseLineMbPerSec;
    ibool   thrashCache;
    } ProfileRecord;

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

extern GA_initFuncs    init;

/*------------------------- Function Prototypes ---------------------------*/

/* Function to profile a particular display mode */

ibool ProfileIt(int x,int y,int bits,char *testName,ProfileRecord *R);
ibool ProfileHWOnly(int x,int y,int bits,ProfileRecord *R);

/* Function to compute a GA-Mark from the profile results */

float ComputeGAMark(ProfileRecord *R);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __PROFILE_H */

