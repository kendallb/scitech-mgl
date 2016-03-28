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
* Description:  Header file for the internal libc runtime library exports.
*
****************************************************************************/

#ifndef __CLIB_OS_INIT_H
#define __CLIB_OS_INIT_H

/* Define calling conventions for standard C calling conventions */

#if (defined(__UNIX__) && !defined(__WATCOMC__) && !defined(__GNUC__))
#define _CDECL
#else
#define _CDECL      __cdecl
#endif
#if defined(__IBMC__) || defined(__IBMCPP__)
#define _CDECLP     * _CDECL
#else
#define _CDECLP     _CDECL *
#endif
#ifndef __VARAPI
#ifdef  __WATCOMC__
#if (__WATCOMC__ >= 1050)
#define _VARAPI     __cdecl
#else
#define _VARAPI
#endif
#else
#define _VARAPI
#endif
#endif

#if !defined(__WIN32_VXD__) && !defined(__WINCE__)
#include <sys/types.h>
#endif

#if defined(__BORLANDC__) || defined(__SC__) || defined(_MSC_VER)
typedef long    off_t;
#endif

/* Structure for all imports to the standard C library */

typedef struct {
    long    dwSize;

    /* <stdlib.h> import functions */
    void    (_CDECLP xabort)(void);
    int     (_CDECLP ___atexit)(void (*)(void));                /* Obsolete - kept for old drivers */
    void *  (_CDECLP ___calloc)(size_t _nelem, size_t _size);   /* Obsolete - kept for old drivers */
    void    (_CDECLP xexit)(int _status);
    void    (_CDECLP xfree)(void *_ptr);
    char *  (_CDECLP xgetenv)(const char *_name);
    void *  (_CDECLP xmalloc)(size_t _size);
    void *  (_CDECLP ___realloc)(void *_ptr, size_t _size);     /* Obsolete - kept for old drivers */
    int     (_CDECLP xsystem)(const char *_s);
    int     (_CDECLP xputenv)(const char *_val);

    /* <libc/file.h> import functions */
    int     (_CDECLP xopen)(const char *_path, int _oflag, unsigned _mode);
    int     (_CDECLP xaccess)(const char *_path, int _amode);
    int     (_CDECLP xclose)(int _fildes);
    size_t  (_CDECLP xlseek)(int _fildes, size_t _offset, int _whence);
    size_t  (_CDECLP xread)(int _fildes, void *_buf, size_t _nbyte);
    int     (_CDECLP xunlink)(const char *_path);
    size_t  (_CDECLP xwrite)(int _fildes, const void *_buf, size_t _nbyte);
    int     (_CDECLP xisatty)(int _fildes);

    /* <stdio.h> import functions */
    int     (_CDECLP xremove)(const char *_filename);
    int     (_CDECLP xrename)(const char *_old, const char *_new);

    /* <signal.h> import functions */
    int     (_CDECLP xraise)(int);
    void *  (_CDECLP xsignal)(int, void *);

    /* <time.h> import functions */
    time_t  (_CDECLP xtime)(time_t *_tod);

    /* <clib/attrib.h> import functions */
    void            (_CDECLP _OS_setfileattr)(const char *filename,unsigned attrib);
    unsigned long   (_CDECLP _OS_getcurrentdate)(void);
    } LIBC_imports;

/* Prototype for the InitLibC and ExitLibC entry points in the DLL */

typedef int (_CDECLP InitLibC_t)(LIBC_imports *imports,long os_type);
typedef void (_CDECLP TerminateLibC_t)(void);

extern LIBC_imports _VARAPI ___imports;
extern LIBC_imports _VARAPI ___imports_shared;

#endif /* __CLIB_OS_INIT_H */

