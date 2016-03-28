/*
 *  signal.h    Signal definitions
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
#ifndef _SIGNAL_H_INCLUDED
#define _SIGNAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
typedef int     sig_atomic_t;



typedef void (*__sig_func)( int );

#define SIG_IGN         ((__sig_func) 1)
#define SIG_DFL         ((__sig_func) 2)
#define SIG_ERR         ((__sig_func) 3)

#define SIGABRT 1
#define SIGFPE  2
#define SIGILL  3
#define SIGINT  4
#define SIGSEGV 5
#define SIGTERM 6
#define SIGBREAK 7
/* following are OS/2 1.x process flag A,B and C */
#define SIGUSR1 8
#define SIGUSR2 9
#define SIGUSR3 10
/* following are for OS/2 2.x only */
#define SIGIDIVZ 11
#define SIGIOVFL 12

#define _SIGMAX     12
#define _SIGMIN     1


_WCRTLINK extern int  raise( int __sig );
_WCRTLINK extern void (*signal( int __sig, void (*__func)(int) ) )(int);

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
