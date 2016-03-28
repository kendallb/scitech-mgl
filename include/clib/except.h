//
//  except.h -- C++ default exception handlers
//
//                          Open Watcom Project
//
//    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
//
//  ========================================================================
//
//    This file contains Original Code and/or Modifications of Original
//    Code as defined in and that are subject to the Sybase Open Watcom
//    Public License version 1.0 (the 'License'). You may not use this file
//    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
//    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
//    provided with the Original Code and Modifications, and is also
//    available at www.sybase.com/developer/opensource.
//
//    The Original Code and all software distributed under the License are
//    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
//    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
//    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
//    NON-INFRINGEMENT. Please see the License for the specific language
//    governing rights and limitations under the License.
//
//  ========================================================================
//
#ifndef _EXCEPT_H_INCLUDED
#define _EXCEPT_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error except.h is for use with C++
#endif
#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#ifndef _PFV_DEFINED
#define _PFV_DEFINED
#define _PFV_DEFINED_
typedef void (*PFV)( void );
#endif
#ifndef _PFU_DEFINED
#define _PFU_DEFINED
#define _PFU_DEFINED_
typedef int (*PFU)( unsigned );
#endif
#ifndef _PNH_DEFINED
#define _PNH_DEFINED
#define _PNH_DEFINED_
typedef int (*_PNH)( unsigned );
#endif
#if defined(__SW_BR)
  #ifndef _PPV_DEFINED
  #define _PPV_DEFINED
  #define _PPV_DEFINED_
  typedef void (*_PVV)( void *);
  #endif
  #ifndef _PUP_DEFINED
  #define _PUP_DEFINED
  #define _PUP_DEFINED_
  typedef void* (*_PUP)( unsigned );
  #endif
#endif

#ifndef _WATCOM_EXCEPTION_DEFINED
#define _WATCOM_EXCEPTION_DEFINED
#define _WATCOM_EXCEPTION_DEFINED_
struct __WATCOM_exception {
#if defined(__AXP__) || defined(__PPC__)
    void *__filler;
#endif
};
#endif

_WPRTLINK extern void terminate( void );
_WPRTLINK extern PFV set_terminate( PFV );
_WPRTLINK extern void unexpected( void );
_WPRTLINK extern PFV set_unexpected( PFV );

#endif
