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
* Description:  ASCII to long long conversion routine.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "watcom.h"
#include "clibi64.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

_WCRTLINK void __F_NAME(__clib_atoll,__clib_watoll)( const CHAR_TYPE *p, unsigned __int64 *pv )  /* convert ASCII string to long integer */
    {
        unsigned __int64 value = 0;
        CHAR_TYPE    sign;

        __ptr_check( p, 0 );

        while( __F_NAME(isspace,iswspace)( *p ) ) ++p;
        sign = *p;
        if( sign == '+' || sign == '-' ) ++p;
        while( __F_NAME(isdigit,iswdigit)(*p) ) {
            value = value * 10 + *p - '0';
            ++p;
        }
        if( sign == '-' ) value = -value;
        *pv = value;
    }
