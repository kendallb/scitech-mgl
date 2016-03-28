/*
 *  fcntl.h     File control options used by open
 *
 *                          Open Watcom Project
 *
 *    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
 *
 *  ========================================================================
 *
 *    This file contains Original Code and/or Modifications of Original
 *    Code as defined in and that are subject to the Sybase Open Watcom
 *    Public License version 1.0 (the 'License'). You may not use this file
 *    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
 *    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
 *    provided with the Original Code and Modifications, and is also
 *    available at www.sybase.com/developer/opensource.
 *
 *    The Original Code and all software distributed under the License are
 *    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 *    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
 *    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
 *    NON-INFRINGEMENT. Please see the License for the specific language
 *    governing rights and limitations under the License.
 *
 *  ========================================================================
 */
#ifndef _FCNTL_H_INCLUDED
#define _FCNTL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#if defined(_M_IX86)
  #pragma pack(1)
#else
  #pragma pack(8)
#endif

#ifndef __OS_IMPORTS_H
 #include "scitech.h"
 #include <os/imports.h>
#endif

/* Flags are defined based on the flags in <os/imports.h> */

#define O_RDONLY        ___O_RDONLY
#define O_WRONLY        ___O_WRONLY
#define O_RDWR          ___O_RDWR

#define O_BINARY        ___O_BINARY
#define O_TEXT          ___O_TEXT
#define O_CREAT         ___O_CREAT
#define O_EXCL          ___O_EXCL
#define O_TRUNC         ___O_TRUNC
#define O_APPEND        ___O_APPEND

/*
 *  POSIX 1003.1 Prototypes.
 */
_WCRTLINK extern int open( const char *__path, int __oflag, ... );
_WCRTLINK extern int sopen( const char *__path, int __oflag, int __share, ... );

#pragma pack()
#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
