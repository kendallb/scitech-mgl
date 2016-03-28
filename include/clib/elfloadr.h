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
* Description:  Header file for the simple Portable ELF module loader
*               library. This library can be used to load ELF modules under
*               any supported OS, provided the modules do not have any
*               imports in the import table.
*
*               NOTE: This loader makes certain assumptions about the
*                     structure of the ELF modules and may not work with
*                     ELF modules created by every tool.
*
****************************************************************************/

#ifndef __ELFLOADR_H
#define __ELFLOADR_H

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
#if !defined(__WINCE__)
#include <sys/types.h>
#endif
#endif

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

#if defined(__GNUC__) || (defined(__UNIX__) && !defined(__WATCOMC__))
#define ELFAPI
#else
#define ELFAPI  __cdecl
#endif

/****************************************************************************
REMARKS:
Structure of a module handle when loaded from disk
{secret}
****************************************************************************/
typedef struct {
    ulong   size;           /* Size of the module in memory             */
    uchar   *vaddr;         /* Virtual address of original image        */
    uchar   *pbase;         /* Base of image in memory                  */
    uchar   *symtab;        /* Symbol table section read from disk      */
    uchar   *dynsym;        /* Dynsym table section read from disk      */
    char    *strtab;        /* String table section read from disk      */
    char    *dynstr;        /* Dynamic string table section from disk   */
    ulong   numsym;         /* Number of entries in symbol table        */
    ulong   numdsym;        /* Number of entries in dyn symbol table    */
    ibool   shared;         /* True if module loaded in shared memory   */
    char    *modname;       /* Filename of the image                    */
    } ELF_MODULE;

/****************************************************************************
REMARKS:
Defines the error codes returned by the library

HEADER:
clib/peloader.h

MEMBERS:
ELF_ok                   - No error
ELF_fileNotFound         - Module file not found
ELF_outOfMemory          - Out of memory loading module
ELF_invalidImage         - Image is invalid or corrupted
ELF_unableToInitLibC     - Unable to initialise the C runtime library
ELF_unknownImageFormat   - Image is in a format that is not supported
****************************************************************************/
typedef enum {
    ELF_ok,
    ELF_fileNotFound,
    ELF_outOfMemory,
    ELF_invalidImage,
    ELF_unableToInitLibC,
    ELF_unknownImageFormat
    } ELF_errorCodes;

#pragma pack()

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

ulong        ELFAPI ELF_getFileSize(FILE *f,ulong startOffset);
ELF_MODULE * ELFAPI ELF_loadLibraryExt(FILE *f,ulong offset,ulong *size,ibool shared);
ELF_MODULE * ELFAPI ELF_loadLibrary(const char *szDLLName,ibool shared);
ELF_MODULE * ELFAPI ELF_loadLibraryMGL(const char *szDLLName,ibool shared);
void *       ELFAPI ELF_getProcAddress(ELF_MODULE *hModule,const char *szProcName);
void         ELFAPI ELF_freeLibrary(ELF_MODULE *hModule);
int          ELFAPI ELF_getError(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif  /* __cplusplus */

#endif  /* __ELFLOADR_H */

