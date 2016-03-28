//
//  wcqueue.h    Defines the WATCOM Queue Container Class
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
#ifndef _WCQUEUE_H_INCLUDED
#define _WCQUEUE_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error wcqueue.h is for use with C++
#endif

#ifndef _WCDEFS_H_INCLUDED
 #include <wcdefs.h>
#endif
#ifndef _WCLIST_H_INCLUDED
 #include <wclist.h>
#endif



//
//  The WCQueue template class defines a queue.  The template supplies
//  the type of the data maintained in the queue, and the methods for
//  manipulating the queue.
//
//  The insert operation does an append.  This is because an item
//  inserted into a queue is actually the last item removed.
//
//  The class 'Type' should be properly defined for copy and assignment
//  operations.
//

template<class Type, class FType>
class WCQueue : private FType {
public:
    inline WCQueue() {};
    inline WCQueue( void * (*user_alloc)( size_t )
                  , void (*user_dealloc)( void *, size_t )
                ) : FType( user_alloc, user_dealloc ) {};
    inline ~WCQueue() {};

    inline WCbool insert( const Type & data )  {
        return( FType::append( data ) );
    };

    inline WCbool isEmpty() const {
        return( FType::isEmpty() );
    };

    inline int entries() const {
        return( FType::entries() );
    };

    inline Type get() {
        return( FType::get() );
    };

    inline Type first() const {
        return( FType::find( 0 ) );
    };

    inline Type last() const {
        return( FType::findLast() );
    };

    inline void clear() {
        FType::clear();
    };

    inline wc_state exceptions() const {
        return( FType::exceptions() );
    };

    inline wc_state exceptions( wc_state const set_flags ) {
        return( FType::exceptions( set_flags ) );
    };
};

#endif
