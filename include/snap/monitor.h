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
* Description:  Header file for the monitor configuration information
*               for SciTech SNAP Graphics Architecture.
*
****************************************************************************/

#ifndef __SNAP_MONITOR_H
#define __SNAP_MONITOR_H

#include "snap/common.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

#define MONITOR_MFR_LEN     40
#define MONITOR_MODEL_LEN   60
#define MDBX_SIG_LEN        15
#define MDBX_SIGNATURE      "MONITOR.DBX"

/****************************************************************************
REMARKS:
Error codes returned by monitor database routines

HEADER:
snap/monitor.h

MEMBERS:
MDBX_ok                 - No error
MDBX_fileNotFound       - Database file not found
MDBX_corrupt            - Database is corrupted
MDBX_outOfMemory        - Not enough memory to load DB
MDBX_notFound           - Entry was not found in DB
MDBX_IOError            - Fatal I/O error
MDBX_parseError         - Error parsing monitor database
MDBX_invalidDB          - Database handle is invalid
MDBX_dbNotOpen          - Attempted to access the database before opening it
****************************************************************************/
typedef enum {
    MDBX_ok,
    MDBX_fileNotFound,
    MDBX_corrupt,
    MDBX_outOfMemory,
    MDBX_notFound,
    MDBX_IOError,
    MDBX_parseError,
    MDBX_invalidDB,
    MDBX_dbNotOpen,
    MDBX_noRecords
    } MDBX_errCodes;

/****************************************************************************
REMARKS:
This enumeration defines the flags for the capabilities for monitors as
defined in the GA_monitor record.

HEADER:
snap/monitor.h

MEMBERS:
Monitor_DPMSEnabled     - Monitor supports DPMS Power Management
Monitor_GTFEnabled      - Monitor supports VESA Generalised Timing Formula
Monitor_FixedFreq       - Monitor is a fixed frequency monitor
Monitor_HSyncNeg        - HSync- is required for fixed frequency
Monitor_VSyncNeg        - VSync- is required for fixed frequency
Monitor_Widescreen      - Monitor supports widescreen (16:9 or 16:10) aspect ratio
Monitor_Exclude4to3     - Driver should exclude all 4:3 aspect modes
Monitor_Changed         - Monitor record has been changed
****************************************************************************/
typedef enum {
    Monitor_DPMSEnabled     = 0x01,
    Monitor_GTFEnabled      = 0x02,
    Monitor_FixedFreq       = 0x04,
    Monitor_HSyncNeg        = 0x08,
    Monitor_VSyncNeg        = 0x10,
    Monitor_Widescreen      = 0x20,
    Monitor_Exclude4to3     = 0x40,
    Monitor_Changed         = 0x80
    } GA_monitorFlagsType;

/****************************************************************************
REMARKS:
Monitor configuration information structure. This structure defines the
capabilities of the attached display monitor, and is used internally
in SNAP Graphics to decide what features the driver should report for the
attached monitor.

HEADER:
snap/monitor.h

MEMBERS:
mfr             - Monitor manufacturer (key)
model           - Monitor model name (sub-key)
PNPID           - Plug and Play ID (optional)
maxResolution   - Maximum resolution id
minHScan        - Minimum horizontal scan
maxHScan        - Maximum horizontal scan
minVScan        - Minimum vertical scan
maxVScan        - Maximum vertical scan
flags           - Capabilities flags
****************************************************************************/
typedef struct {
    char        mfr[MONITOR_MFR_LEN+1];
    char        model[MONITOR_MODEL_LEN+1];
    char        PNPID[8];
    N_uint8     maxResolution;
    N_uint8     minHScan;
    N_uint8     maxHScan;
    N_uint8     minVScan;
    N_uint8     maxVScan;
    N_uint8     flags;
    } GA_monitor;

/* {secret} */
typedef struct {
    char        signature[MDBX_SIG_LEN];    /* Signature                    */
    N_uint32    entries;                    /* Number of entries in database*/
    /* Data goes in here */
    } MDBX_header;

#pragma pack()

/*------------------------ Function Prototypes ----------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Return error code for previous operation. The error code will only be
 * updated if the function actually failed, and is not cleared after
 * being returned.
 */

int     NAPI MDBX_getErrCode(void);
const char *  NAPI MDBX_getErrorMsg(void);

/* Routines for reading/writing monitors from the MONITOR.DBX database */

ibool   NAPI MDBX_open(const char *filename);
void    NAPI MDBX_close(void);
int     NAPI MDBX_first(GA_monitor *rec);
int     NAPI MDBX_last(GA_monitor *rec);
int     NAPI MDBX_next(GA_monitor *rec);
int     NAPI MDBX_prev(GA_monitor *rec);
int     NAPI MDBX_insert(GA_monitor *rec);
int     NAPI MDBX_update(GA_monitor *rec);
int     NAPI MDBX_flush(void);
int     NAPI MDBX_importINF(const char *filename,char *mfr);

#ifdef __cplusplus
}
#endif

#endif  /* __SNAP_MONITOR_H */
