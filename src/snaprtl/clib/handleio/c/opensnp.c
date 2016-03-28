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
* Description:  Implementation of open() and sopen() for SNAP.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdarg.h>

#include "scitech.h"
#include <os/imports.h>


/* open() and sopen() are identical under SNAP, since the extra share
 * flags passed to sopen() are simply ignored. We implement sopen() because
 * the runtime library internally uses it.
 */

_WCRTLINK int open( const char *name, int oflag, ... )
{
    int         mode;
    va_list     args;

    va_start( args, oflag );
    mode = va_arg( args, int );
    va_end( args );
    return xopen( name, oflag, mode );
}


_WCRTLINK int sopen( const char *name, int oflag, int shflag, ... )
{
    int         mode;
    va_list     args;

    va_start( args, shflag );
    mode = va_arg( args, int );
    va_end( args );
    return xopen( name, oflag, mode );
}

