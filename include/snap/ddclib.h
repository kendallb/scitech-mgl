/****************************************************************************
*
*                       SciTech SNAP DDC Architecture
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
* Description:  Header file for the SciTech SNAP Display Data Channel
*               Architecture device driver Hardware Abstraction Layer.
*
*               This library is a stripped down version of SciTech SNAP
*               Graphics which contains only the DPMS and DDC functionality
*               and will work across many operating systems and platforms.
*
****************************************************************************/

#ifndef __SNAP_DDCLIB_H
#define __SNAP_DDCLIB_H

#include "snap/graphics.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/****************************************************************************
REMARKS:
Structure defining all the SNAP Graphics API functions as exported from
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct __DDC_exports {
    N_uintptr       dwSize;
    int             (NAPIP DDC_status)(void);
    const char *    (NAPIP DDC_errorMsg)(N_int32 status);
    int             (NAPIP DDC_registerLicense)(uchar *license);
    int             (NAPIP DDC_enumerateDevices)(void);
    GA_devCtx *     (NAPIP DDC_loadDriver)(N_int32 deviceIndex);
    void            (NAPIP DDC_unloadDriver)(GA_devCtx *dc);
    ibool           (NAPIP DDC_queryFunctions)(GA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);
    int             (NAPIP DDC_init)(GA_devCtx *dc);
    int             (NAPIP DDC_initExt)(GA_devCtx *dc,N_int32 channel);
    ibool           (NAPIP DDC_readEDID)(N_int32 slaveAddr,uchar *edid,N_int32 length,N_int32 blockNumber,N_int32 channel);
    int             (NAPIP EDID_parse)(uchar *edid,EDID_record *rec,N_int32 requireDescriptor);
    int             (NAPIP MCS_begin)(GA_devCtx *dc);
    int             (NAPIP MCS_beginExt)(GA_devCtx *dc,N_int32 channel);
    int             (NAPIP MCS_getCapabilitiesString)(char *data,N_int32 maxlen);
    ibool           (NAPIP MCS_isControlSupported)(uchar controlCode);
    ibool           (NAPIP MCS_enableControl)(uchar controlCode,N_int32 enable);
    ibool           (NAPIP MCS_getControlMax)(uchar controlCode,ushort *max);
    ibool           (NAPIP MCS_getControlValue)(uchar controlCode,ushort *value);
    ibool           (NAPIP MCS_getControlValues)(N_int32 numControls,uchar *controlCodes,ushort *values);
    ibool           (NAPIP MCS_setControlValue)(uchar controlCode,ushort value);
    ibool           (NAPIP MCS_setControlValues)(N_int32 numControls,uchar *controlCodes,ushort *values);
    ibool           (NAPIP MCS_resetControl)(uchar controlCode);
    ibool           (NAPIP MCS_saveCurrentSettings)(void);
    ibool           (NAPIP MCS_getTimingReport)(uchar *flags,ushort *hFreq,ushort *vFreq);
    ibool           (NAPIP MCS_getSelfTestReport)(uchar *flags,uchar *data,uchar *length);
    void            (NAPIP MCS_end)(void);
    } DDC_exports;

/* Macros to call DDC and MCS functions via the __DDC_exports function pointer
 * table. This avoids the problems of apps linking against the PM library which
 * contains exports for the functions in the full graphics.bpd drivers.
 */

#define DDC_init(dc)                                                __DDC_exports.DDC_init(dc)
#define DDC_initExt(dc,channel)                                     __DDC_exports.DDC_initExt(dc,channel)
#define DDC_readEDID(slaveAddr,edid,length,blockNumber,channel)     __DDC_exports.DDC_readEDID(slaveAddr,edid,length,blockNumber,channel)
#define EDID_parse(edid,rec,requireDescriptor)                      __DDC_exports.EDID_parse(edid,rec,requireDescriptor)
#define MCS_begin(dc)                                               __DDC_exports.MCS_begin(dc)
#define MCS_beginExt(dc,channel)                                    __DDC_exports.MCS_beginExt(dc,channel)
#define MCS_getCapabilitiesString(data,maxlen)                      __DDC_exports.MCS_getCapabilitiesString(data,maxlen)
#define MCS_isControlSupported(controlCode)                         __DDC_exports.MCS_isControlSupported(controlCode)
#define MCS_enableControl(controlCode,enable)                       __DDC_exports.MCS_enableControl(controlCode,enable)
#define MCS_getControlMax(controlCode,max)                          __DDC_exports.MCS_getControlMax(controlCode,max)
#define MCS_getControlValue(controlCode,value)                      __DDC_exports.MCS_getControlValue(controlCode,value)
#define MCS_getControlValues(numControls,controlCodes,values)       __DDC_exports.MCS_getControlValues(numControls,controlCodes,values)
#define MCS_setControlValue(controlCode,value)                      __DDC_exports.MCS_setControlValue(controlCode,value)
#define MCS_setControlValues(numControls,controlCodes,values)       __DDC_exports.MCS_setControlValues(numControls,controlCodes,values)
#define MCS_resetControl(controlCode)                               __DDC_exports.MCS_resetControl(controlCode)
#define MCS_saveCurrentSettings()                                   __DDC_exports.MCS_saveCurrentSettings()
#define MCS_getTimingReport(flags,hFreq,vFreq)                      __DDC_exports.MCS_getTimingReport(flags,hFreq,vFreq)
#define MCS_getSelfTestReport(flags,data,length)                    __DDC_exports.MCS_getSelfTestReport(flags,data,length)
#define MCS_end()                                                   __DDC_exports.MCS_end()

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point
for loading the minimal DDC library.
{secret}
****************************************************************************/
typedef DDC_exports * (NAPIP DDC_initLibrary_t)(const char *path,const char *bpdname,PM_imports *pmImp,N_imports *nImp);

#pragma pack()

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* {secret} */
extern DDC_exports   _VARAPI __DDC_exports;

/*-------------------------- Function Prototypes --------------------------*/

/* Error handling functions for SNAP Graphics drivers */

int             NAPI DDC_status(void);
const char *    NAPI DDC_errorMsg(N_int32 status);

/* Utility function to register a linkable library license */

int             NAPI DDC_registerLicense(uchar *license);

/* Functions to load and unload the minimal DDC drivers for a specific graphics card */

int             NAPI DDC_enumerateDevices(void);
GA_devCtx *     NAPI DDC_loadDriver(N_int32 deviceIndex);
void            NAPI DDC_unloadDriver(GA_devCtx *dc);
ibool           NAPI DDC_queryFunctions(GA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_DDCLIB_H */

