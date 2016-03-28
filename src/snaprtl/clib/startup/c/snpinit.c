/****************************************************************************
*
*                            Open Watcom Project
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
*
* Description:  Module to initialise the C runtime library for the SNAP
*               binary portable DLLs. Called by the PE loader library when
*               the DLL is first loaded and set up the callbacks to
*               platform specific code.
*
* TODO: Modify this for SNAP application imports! We may want to use
*       different exported names here so driver DLL's and app DLL's will
*       not get confused.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "scitech.h"
#include <os/imports.h>
#include <os/init.h>
#include "initfini.h"
#include "heap.h"

/*--------------------------- Global variables ----------------------------*/

long            _VARAPI ___drv_os_type;
LIBC_imports    _VARAPI ___imports;

/*----------------------------- Glue Functions ----------------------------*/

#define FATAL_MSG       "Unsupported C library import function called! Please re-compile!\n"
#define FATAL_MSG_LEN   (sizeof( FATAL_MSG ) - 1)

static void fatalErrorHandler( void )
{
    xwrite( 0, FATAL_MSG, FATAL_MSG_LEN );
    ___imports.xabort();
}

/* Initialise the C runtime library from the passed in list of imports
 * and the defined OS type variable.
 */
int _CEXPORT InitLibC( LIBC_imports *imports, long os_type )
{
    int             i,max = sizeof( ___imports ) / sizeof( InitLibC_t );
    unsigned long   *p;

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility.
     */
    for( i = 0, p = (unsigned long*)&___imports; i < max; i++ )
        *p++ = (unsigned long)fatalErrorHandler;

    /* Now copy all imported functions */
    memcpy( &___imports, imports, min(sizeof( ___imports ), imports->dwSize ) );
    ___drv_os_type = os_type;

    /* Run the clib initializers */
    __InitRtns( 255 );

    return 1;
}

/* Exit the C runtime library to clean up and free all allocated memory */
void _CEXPORT TerminateLibC( void )
{
    __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
    __FreeAllHeaps();
}

