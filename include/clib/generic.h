//
//  generic.h   Macros to support pseudo-templates
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
#ifndef _GENERIC_H_INCLUDED
#define _GENERIC_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error generic.h is for use with C++
#endif

#define name2(__n1,__n2)                __paste2(__n1,__n2)
#define __paste2(__p1,__p2)             __p1##__p2
#define name3(__n1,__n2,__n3)           __paste3(__n1,__n2,__n3)
#define __paste3(__p1,__p2,__p3)        __p1##__p2##__p3
#define name4(__n1,__n2,__n3,__n4)      __paste4(__n1,__n2,__n3,__n4)
#define __paste4(__p1,__p2,__p3,__p4)   __p1##__p2##__p3##__p4

#define declare(__Cls,__Typ1) \
        name2(__Cls,declare)(__Typ1)
#define implement(__Cls,__Typ1) \
        name2(__Cls,implement)(__Typ1)
#define declare2(__Cls,__Typ1,__Typ2) \
        name2(__Cls,declare)(__Typ1,__Typ2)
#define implement2(__Cls,__Typ1,__Typ2) \
        name2(__Cls,implement)(__Typ1,__Typ2)
#define callerror(__Cls,__Typ1,__Typ2,__Typ3) \
        (*errorhandler(__Cls,__Typ1))(__Typ2,__Typ3)
#define errorhandler(__Cls,__Typ1) \
        name3(__Typ1,__Cls,handler)
#define set_handler(__Cls,__Typ1,__Typ2) \
        name4(set_,__Typ1,__Cls,_handler)(__Typ2)

// undocumented AT&T function (WATCOM stub returns first argument)
extern int genericerror( int, char* );
typedef int (*GPT)( int, char* );

#endif
