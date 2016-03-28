/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  32-bit Windows VxD
*
* Description:  C library compatible functions for use within NT drivers.
*
****************************************************************************/

#ifndef __NT_DRIVER_H
#define __NT_DRIVER_H

// Prevent stdio.h and stdlib.h includes

#define _FILE_DEFINED
#define _SIZE_T_DEFINED
#define _INC_STDIO
#define _INC_STDLIB

/*---------------------- Macros and type definitions ----------------------*/

#define EOF             (-1)
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2

typedef struct {
    int     handle;
    int     writemode;
    int     text;
    long    offset;
    long    filesize;
    } FILE;

typedef unsigned int    size_t;
typedef long            time_t;

/*------------------------- Function Prototypes ---------------------------*/

// stdio.h

int     sprintf(char *_s, const char *_format, ...);
FILE *  fopen( const char *__filename, const char *__mode );
size_t  fread( void *__ptr, size_t __size, size_t __n, FILE *__fp );
size_t  fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
int     fflush( FILE *__fp );
int     fseek( FILE *__fp, long int __offset, int __whence );
long    ftell( FILE *__fp );
int     feof( FILE *__fp );
int     fclose( FILE *__fp );
char *  fgets( char *__s, int __n, FILE *__fp );
int     fputs( const char *__s, FILE *__fp );

// stdlib.h

void *   calloc(size_t _nelem, size_t _size);
void     free(void *_ptr);
void *   malloc(size_t _size);
void *   realloc(void *_ptr, size_t _size);

// Common Windows types

#define DWORD   ULONG
#define WORD    USHORT
#define BYTE    UCHAR

/*-------------------------------- Includes -------------------------------*/

#ifndef __NO_NTDDK
#include <ntddk.h>
#endif

#endif  /* __NT_DRIVER_H */

