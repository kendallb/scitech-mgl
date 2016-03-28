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
* Description:  Header file for the internal C runtime library imports.
*               These functions are for internal C runtime use only,
*               renamed not to conflict with public file access functions.
*
****************************************************************************/

#ifndef __OS_IMPORTS_H
#define __OS_IMPORTS_H

#if !defined(__WIN32_VXD__) && !defined(__WINCE__)
#include <sys/types.h>
#endif

#ifndef _CDECL
#define _CDECL __cdecl
#endif

#ifndef _CEXPORT
#define _CEXPORT __cdecl __export
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ___O_RDONLY     0x0001
#define ___O_WRONLY     0x0002
#define ___O_RDWR       0x0004

#define ___O_BINARY     0x0008
#define ___O_TEXT       0x0010
#define ___O_CREAT      0x0100
#define ___O_EXCL       0x0200
#define ___O_TRUNC      0x0800
#define ___O_APPEND     0x1000

#ifndef __BUILDING_PE_LOADER__

/* External OS memory allocator functions */

void *  _CDECL xmalloc(size_t _size);
void    _CDECL xfree(void *_ptr);

/* External OS process functions */

void    _CDECL xabort(void);
void    _CDECL xexit(int _status);
int     _CDECL xsystem(const char *_s);

/* External OS environment functions */

char *  _CDECL xgetenv(const char *_name);
int     _CDECL xputenv(const char *_val);

/* External OS I/O functions */

int     _CDECL xopen(const char *_path, int _oflag, unsigned _mode);
int     _CDECL xaccess(const char *_path, int _amode);
int     _CDECL xclose(int _fildes);
size_t  _CDECL xlseek(int _fildes, size_t _offset, int _whence);
size_t  _CDECL xread(int _fildes, void *_buf, size_t _nbyte);
int     _CDECL xunlink(const char *_path);
size_t  _CDECL xwrite(int _fildes, const void *_buf, size_t _nbyte);
int     _CDECL xisatty(int _fildes);

/* <stdio.h> import functions */

int     _CDECL xremove(const char *_filename);
int     _CDECL xrename(const char *_old, const char *_new);

/* <signal.h> import functions */
typedef void _CDECL _Sigfun(int);
int     _CDECL xraise(int);
_Sigfun * _CDECL xsignal(int, _Sigfun *);

/* <time.h> import functions */
time_t  _CDECL xtime(time_t *_tod);

#endif /* !__BUILDING_PE_LOADER__ */

#ifdef __cplusplus
}
#endif

#endif /* __OS_IMPORTS_H */

