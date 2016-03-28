//
//  typeinfo.h  Type Information
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
#ifndef _TYPEINFO_H_INCLUDED
#define _TYPEINFO_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error typeinfo.h is for use with C++
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#include <exceptio.h>

class _WPRTLINK type_info {
public:
    ~type_info();
    bool operator ==( type_info const & ) const;
    bool operator !=( type_info const & ) const;
    bool before( type_info const & ) const;
    const char *name() const;
    const char *raw_name() const;
private:
    type_info( type_info const & );
    type_info & operator =( type_info const & );
    mutable void const *__data;
    char const __raw[1];
};

class _WPRTLINK bad_cast : public exception {
public:
    bad_cast( string const& what_arg ) _WCTHROWS(())
        : exception( what_arg ) {
    }
};

class _WPRTLINK bad_typeid : public exception {
public:
    bad_typeid( string const& what_arg ) _WCTHROWS(())
        : exception( what_arg ) {
    }
};

#endif
