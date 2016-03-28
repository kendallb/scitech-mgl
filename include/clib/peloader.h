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
* Environment:  Any
*
* Description:  Header file for the simple Portable Executeable DLL loader
*               library. This library can be used to load PE DLL's under
*               any Intel based OS, provided the DLL's do not have any
*               imports in the import table.
*
*               NOTE: This loader module expects the DLL's to be built with
*                     Watcom C++ and may produce unexpected results with
*                     DLL's linked by another compiler.
*
****************************************************************************/

#ifndef __PELOADER_H
#define __PELOADER_H

#include "scitech.h"
#include "clib/sys/cdecl.h"
#ifdef  __WIN32_VXD__
#include "vxdfile.h"
/* {secret} */
typedef long time_t;
#elif defined(__NT_DRIVER__)
#include "ntdriver.h"
#elif defined(__XFREE86__)
#include <xf86_libc.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(__WINCE__)
#include <time.h>
#include <sys/types.h>
#endif
#endif

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

#if defined(__GNUC__) || (defined(__UNIX__) && !defined(__WATCOMC__))
#define PEAPI
#else
#define PEAPI   __cdecl
#endif

/****************************************************************************
REMARKS:
Structure of a module handle when loaded from disk
{secret}
****************************************************************************/
typedef struct {
    ulong   size;           /* Size of the module in memory             */
    uchar   *pbase;         /* Base of image in memory                  */
    uchar   *ptext;         /* Text section read from disk              */
    uchar   *pdata;         /* Data section read from disk              */
    uchar   *pbss;          /* BSS section read                         */
    uchar   *pimport;       /* Import section read from disk            */
    uchar   *pexport;       /* Export section read from disk            */
    ulong   textBase;       /* Base of text section in image            */
    ulong   dataBase;       /* Base of data section in image            */
    ulong   bssBase;        /* Base of BSS data section in image        */
    ulong   importBase;     /* Offset of import section in image        */
    ulong   exportBase;     /* Offset of export section in image        */
    ulong   exportDir;      /* Offset of export directory               */
    ibool   shared;         /* True if module loaded in shared memory   */
    char    *modname;       /* Filename of the image                    */
    } PE_MODULE;

/****************************************************************************
REMARKS:
Defines the error codes returned by the library

HEADER:
clib/peloader.h

MEMBERS:
PE_ok                   - No error
PE_fileNotFound         - DLL file not found
PE_outOfMemory          - Out of memory loading DLL
PE_invalidDLLImage      - DLL image is invalid or corrupted
PE_unableToInitLibC     - Unable to initialise the C runtime library
PE_unknownImageFormat   - DLL image is in a format that is not supported
****************************************************************************/
typedef enum {
    PE_ok,
    PE_fileNotFound,
    PE_outOfMemory,
    PE_invalidDLLImage,
    PE_unableToInitLibC,
    PE_unknownImageFormat
    } PE_errorCodes;

#pragma pack()

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

ulong       PEAPI PE_getFileSize(FILE *f,ulong startOffset);
PE_MODULE * PEAPI PE_loadLibraryExt(FILE *f,ulong offset,ulong *size,ibool shared);
PE_MODULE * PEAPI PE_loadLibrary(const char *szDLLName,ibool shared);
PE_MODULE * PEAPI PE_loadLibraryMGL(const char *szDLLName,ibool shared);
void *      PEAPI PE_getProcAddress(PE_MODULE *hModule,const char *szProcName);
void        PEAPI PE_freeLibrary(PE_MODULE *hModule);
int         PEAPI PE_getError(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif  /* __cplusplus */

#endif  /* __PELOADER_H */

