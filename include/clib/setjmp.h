/*
 *  setjmp.h
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
#ifndef _SETJMP_H_INCLUDED
#define _SETJMP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#if defined(__PPC__)
 typedef unsigned int jmp_buf[1];
#elif defined(__AXP__)
 typedef double jmp_buf[24];
#else
 typedef unsigned int jmp_buf[13];
#endif

_WCRTLINK extern int  _setjmp( jmp_buf __env );
_WCRTLINK extern void longjmp( jmp_buf __env, int __val );

#if !defined(_SETJMPEX_H_INCLUDED_)
 #define setjmp(__env)  _setjmp(__env)
#endif

#if defined(__PPC__)
#elif defined(__AXP__)
#elif defined(__X86_64__)
#elif defined(__MIPS__)
#elif defined(__386__)
 #pragma aux _setjmp __parm __caller [__eax] __modify [__8087];
 #ifndef __SW_3R
  #pragma aux longjmp __aborts;
 #endif
#else
 #error unknown platform;
#endif


#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
