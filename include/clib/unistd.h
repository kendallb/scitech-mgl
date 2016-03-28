/*
 *  unistd.h
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
#ifndef _UNISTD_H_INCLUDED
#define _UNISTD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED_
#ifdef __cplusplus
typedef long char wchar_t;
#else
typedef unsigned short wchar_t;
#endif
#endif

#ifndef __TYPES_H_INCLUDED
 #include <sys/types.h>
#endif

#ifndef NULL
 #if defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__) || defined(__AXP__) || defined(__PPC__)
  #define NULL   0
 #else
  #define NULL   0L
 #endif
#endif

/* Symbolic constants for the access() function */

#define R_OK    4       /*  Test for read permission    */
#define W_OK    2       /*  Test for write permission   */
#define X_OK    1       /*  Test for execute permission */
#define F_OK    0       /*  Test for existence of file  */

/* Symbolic constants for the lseek() function */

#ifndef _SEEKPOS_DEFINED_       /* If not already defined, define them  */
#define SEEK_SET    0           /* Seek relative to the start of file   */
#define SEEK_CUR    1           /* Seek relative to current position    */
#define SEEK_END    2           /* Seek relative to the end of the file */
#define _SEEKPOS_DEFINED_
#endif

/* Symbolic constants for stream I/O */

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2


/*
 *  POSIX 1003.1 Prototypes
 */


_WCRTLINK extern int    access( const char *__path, int __mode );
_WCRTLINK extern int    close( int __fildes );
_WCRTLINK extern int    isatty( int __fildes );
_WCRTLINK extern off_t  lseek( int __fildes, off_t __offset, int __whence );
_WCRTLINK extern ssize_t read( int __fildes, void *__buffer, size_t __len );
_WCRTLINK extern int    unlink( const char *__path );
_WCRTLINK extern ssize_t write( int __fildes, const void *__buf, size_t __len );

#if !defined(NO_EXT_KEYS) /* extensions enabled */

_WCRTLINK extern int    eof( int __fildes );
_WCRTLINK extern long   filelength( int __fildes );
_WCRTLINK extern off_t  tell( int __fildes );

#endif


#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif

