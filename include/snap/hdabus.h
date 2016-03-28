/****************************************************************************
*
*                     SciTech SNAP HDA Bus Architecture
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
* Description:  Header file for the SciTech SNAP High Definition Audio Bus
*               Architecture device driver Hardware Abstraction Layer.
*
****************************************************************************/

#ifndef __SNAP_HDABUS_H
#define __SNAP_HDABUS_H

#include "snap/common.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/****************************************************************************
REMARKS:
Callback for stream completion handling, which is called when the stream
has reached the half buffer point for either playback or recording.
****************************************************************************/
typedef void (NAPIP HDA_streamIOCProc)(struct HDA_stream *stream);

/****************************************************************************
REMARKS:
Structure to represent streams between the host processor and codecs on
the High Definition Audio Bus.

HEADER:
snap/audio.h

MEMBERS:
regOffset       - Stream descriptor register offset
BDL             - Virtual address of the BDL
BDLPhys         - Physical address of the BDL
cntPosition     - Buffer position readback pointer
intStatusMask   - Stream interrupt status mask
index           - Stream index
streamTag       - Assigned stream tag
format          - Current stream format flags
running         - True if currently running
IOCProc         - Completion interrupt handler callback
allocated       - True if the stream has been allocated
DMABuffer       - Pointer to DMA buffer for stream
DMABufferSize   - Size of DMA buffer for stream
DMAPhysList     - Physical memory list for DMA buffer
DMAPhysListNum  - Number of physical address list entries
****************************************************************************/
typedef struct HDA_stream {
    N_uint32            regOffset;
    N_uint32            *BDL;
    N_physAddr          BDLPhys;
    volatile N_uint32   *cntPosition;
    N_uint32            intStatusMask;
    int                 index;
    int                 streamTag;
    N_uint16            format;
    ibool               running;
    HDA_streamIOCProc   IOCProc;
    ibool               allocated;
    void                *DMABuffer;
    int                 DMABufferSize;
    N_physAddr          *DMAPhysList;
    int                 DMAPhysListNum;
    } HDA_stream;

/****************************************************************************
REMARKS:
Structure defining all the SciTech SNAP Audio API functions as exported from
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    N_int32         (NAPIP HDA_init)(N_uint16 vendorID,N_uint16 deviceID,N_uint32 *codecMask);
    void            (NAPIP HDA_exit)(void);
    N_uint32        (NAPIP HDA_sendCommand)(N_uint32 codecAddr,N_uint32 nodeID,ibool indirect,N_uint32 verb,N_uint32 payload,ibool getResponse);
    N_uint32        (NAPIP HDA_getResponse)(void);
    HDA_stream *    (NAPIP HDA_allocStream)(ibool inputStream,void *DMABuffer,N_int32 DMABufferSize);
    void            (NAPIP HDA_freeStream)(HDA_stream *stream);
    ibool           (NAPIP HDA_setStreamFormat)(HDA_stream *stream,N_int32 sampleRate,N_int32 bits,N_int32 channels);
    ibool           (NAPIP HDA_startStream)(HDA_stream *stream,N_int32 halfBufferSize,HDA_streamIOCProc IOCProc);
    void            (NAPIP HDA_stopStream)(HDA_stream *stream);
    } HDA_exports;

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef HDA_exports * (NAPIP HDA_initLibrary_t)(const char *path,const char *bpdname,PM_imports *pmImp,N_imports *nImp);

#pragma pack()

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Pointer to global exports structure. Should not be used by application programs. */
/* {secret} */
extern HDA_exports   _VARAPI __HDA_exports;

/*-------------------------- Function Prototypes --------------------------*/

/* Init/exit the HDA bus driver */

N_int32         NAPI HDA_init(N_uint16 vendorID,N_uint16 deviceID,N_uint32 *codecMask);
void            NAPI HDA_exit(void);

/* HDA bus command functions */

N_uint32        NAPI HDA_sendCommand(N_uint32 codecAddr,N_uint32 nodeID,ibool indirect,N_uint32 verb,N_uint32 payload,ibool getResponse);
N_uint32        NAPI HDA_getResponse(void);

/* HDA bus stream functions */

HDA_stream *    NAPI HDA_allocStream(ibool inputStream,void *DMABuffer,N_int32 DMABufferSize);
void            NAPI HDA_freeStream(HDA_stream *stream);
ibool           NAPI HDA_setStreamFormat(HDA_stream *stream,N_int32 sampleRate,N_int32 bits,N_int32 channels);
ibool           NAPI HDA_startStream(HDA_stream *stream,N_int32 halfBufferSize,HDA_streamIOCProc IOCProc);
void            NAPI HDA_stopStream(HDA_stream *stream);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_HDABUS_H */

