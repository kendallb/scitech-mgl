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
* Description:  Header file for the abstracted module loader library.
*               Either PE or ELF module supprt is chosen depending on
*               target platform.
*
****************************************************************************/

#ifndef __MODLOADR_H
#define __MODLOADR_H

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

#if defined(__386__) || defined(__INTEL__)

#include "clib/peloader.h"

#define MOD_getFileSize         PE_getFileSize
#define MOD_loadLibraryExt      PE_loadLibraryExt
#define MOD_loadLibrary         PE_loadLibrary
#define MOD_loadLibraryMGL      PE_loadLibraryMGL
#define MOD_getProcAddress      PE_getProcAddress
#define MOD_freeLibrary         PE_freeLibrary
#define MOD_getError            PE_getError

#define MODAPI PEAPI
typedef PE_MODULE MOD_MODULE;
#define MOD_FNPREFIX            "_"

#elif defined(__PPC__) || defined(__X86_64__) || defined(__MIPS__)

#include "clib/elfloadr.h"

#define MOD_getFileSize         ELF_getFileSize
#define MOD_loadLibraryExt      ELF_loadLibraryExt
#define MOD_loadLibrary         ELF_loadLibrary
#define MOD_loadLibraryMGL      ELF_loadLibraryMGL
#define MOD_getProcAddress      ELF_getProcAddress
#define MOD_freeLibrary         ELF_freeLibrary
#define MOD_getError            ELF_getError

#define MODAPI ELFAPI
typedef ELF_MODULE MOD_MODULE;
#define MOD_FNPREFIX

#else

#error "Module Loader not ported to this platform family yet!"

#endif

typedef enum {
    MOD_ok,
    MOD_fileNotFound,
    MOD_outOfMemory,
    MOD_invalidImage,
    MOD_unableToInitLibC,
    MOD_unknownImageFormat
    } MOD_errorCodes;

#endif  /* __MODLOADR_H */

