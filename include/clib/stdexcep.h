//
//  stdexcept.h Standard exceptions
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
#ifndef _STDEXCEPT_H_INCLUDED
#define _STDEXCEPT_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error stdexception.h is for use with C++
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#ifndef _WCTHROWS
#ifdef _CPPUNWIND
#define _WCTHROWS( __t )        throw __t
#else
#define _WCTHROWS( __t )
#endif
#endif

// corresponds to Header<stdexcept>

// to be fixed later by enhanced string class
//
//
#include <exstring.h>

typedef __ExcString string;

class _WPRTLINK exception {
    string _what_val;
public:
    exception( void ) _WCTHROWS(()) {
    }
    exception( exception const &src ) _WCTHROWS(())
        : _what_val( src._what_val ) {
    }
    exception( string const & what_val ) _WCTHROWS(())
        : _what_val( what_val ) {
    }
    exception& operator=( exception const & src ) _WCTHROWS(()) {
        _what_val = src._what_val;
        return *this;
    }
    virtual ~exception( void ) _WCTHROWS(()) {
    }
    virtual char const * what( void ) const _WCTHROWS(()) {
        return _what_val;
    }
};

class _WPRTLINK logic_error : public exception {
public:
    logic_error( string const& what_arg ) _WCTHROWS(())
        : exception( what_arg ) {
    }
};

class _WPRTLINK domain_error : public logic_error {
public:
    domain_error( string const& what_arg ) _WCTHROWS(())
        : logic_error( what_arg ) {
    }
};

class _WPRTLINK invalid_argument : public logic_error {
public:
    invalid_argument( string const& what_arg ) _WCTHROWS(())
        : logic_error( what_arg ) {
    }
};

class _WPRTLINK length_error : public logic_error {
public:
    length_error( string const& what_arg ) _WCTHROWS(())
        : logic_error( what_arg ) {
    }
};

class _WPRTLINK out_of_range : public logic_error {
public:
    out_of_range( string const& what_arg ) _WCTHROWS(())
        : logic_error( what_arg ) {
    }
};

class _WPRTLINK runtime_error : public exception {
public:
    runtime_error( string const& what_arg ) _WCTHROWS(())
        : exception( what_arg ) {
    }
};

class _WPRTLINK range_error : public runtime_error {
public:
    range_error( string const& what_arg ) _WCTHROWS(())
        : runtime_error( what_arg ) {
    }
};

class _WPRTLINK overflow_error : public runtime_error {
public:
    overflow_error( string const& what_arg ) _WCTHROWS(())
        : runtime_error( what_arg ) {
    }
};

#endif
