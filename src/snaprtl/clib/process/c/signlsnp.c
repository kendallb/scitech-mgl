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
* Description:  Signal handling for SNAP
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <signal.h>

#include "scitech.h"
#include <os/imports.h>

#if 0
/* TODO: This is going to require some work to get going with SNAP. We
 *       may need to build a system where we install dynamic wrappers
 *       around the signal handlers, maybe on the OS specific side
 *       of the fence that will translate calling conventions as well
 *       as signal numbers. Or since we need to translate calling
 *       conventions before we can call xsignal, maybe that should be
 *       internal and the OS imports could provide a function to get
 *       access to a list of supported signal numbers that match up
 *       with the generic signal numbers for the BPD runtime library.
 *
 *       Note of course that all this is moot for device drivers, since
 *       signals do not make sense for device drivers.
 */
_WCRTLINK void (*signal( int sig, void (*func)(int) ))( int )
{
    return xsignal( sig, func );
}

_WCRTLINK int raise( int sig )
{
    return xraise( sig );
}
#endif

