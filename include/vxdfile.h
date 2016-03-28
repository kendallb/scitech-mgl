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
* Description:  C library compatible I/O functions for use within a VxD.
*
****************************************************************************/

#ifndef __VXDFILE_H
#define __VXDFILE_H

/*---------------------- Macros and type definitions ----------------------*/

typedef struct {
    int     handle;
    int     writemode;
    int     text;
    long    offset;
    long    filesize;
    } FILE;

/*---------------------------- Global variables ---------------------------*/

/* Your VxD must declare this external variable and set the value after
 * the OnInitComplete message is handled.
 */

extern ibool    initComplete;

/* This variable should be declared and set to NULL unless you have hooked
 * the Int 10h handler in your VxD.
 */

extern int      (*pDisableTSR)(int value);

/*------------------------- Function Prototypes ---------------------------*/

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

#endif  /* __VXDFILE_H */

