//
//  iomanip.h   I/O streams manipulators
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
#ifndef _IOMANIP_H_INCLUDED
#define _IOMANIP_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error iomanip.h is for use with C++
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
#ifndef _IOSTREAM_H_INCLUDED
 #include <iostream.h>
#endif

#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
template<class T>
    class _WPRTLINK smanip;
template<class T>
    class _WPRTLINK sapp {
    public:
        sapp( ios &(*__f)( ios &, T ) ) :
            __fn( __f ) {};
        smanip<T> operator()( T __p ) { return smanip<T>( __fn, __p ); };
    private:
        ios &(*__fn)( ios &, T );
    };
template<class T>
    class _WPRTLINK smanip {
    public:
        smanip( ios &(*__f)( ios &, T ), T __p ) :
            __fn( __f ), __parm( __p ) {};
        friend _WPRTLINK istream &operator>>( istream &, const smanip<T> & );
        friend _WPRTLINK ostream &operator<<( ostream &, const smanip<T> & );
    private:
        ios &(*__fn)( ios &, T );
        T __parm;
    };
template<class T>
    _WPRTLINK istream &operator>>( istream &__is, const smanip<T> &__sm ) {
        __sm.__fn( __is, __sm.__parm );
        return( __is );
    }
template<class T>
    _WPRTLINK ostream &operator<<( ostream &__os, const smanip<T> &__sm ) {
        __sm.__fn( __os, __sm.__parm );
        return( __os );
    }

template<class T>
    class _WPRTLINK imanip;
template<class T>
    class _WPRTLINK iapp {
    public:
        iapp( istream &(*__f)( istream &, T ) ) :
            __fn( __f ) {};
        imanip<T> operator()( T __p ) { return imanip<T>( __fn, __p ) };
    private:
        istream &(*__fn)( istream &, T );
    };
template<class T>
    class _WPRTLINK imanip {
    public:
        imanip( istream &(*__f)( istream &, T ), T __p ) :
            __fn( __f ), __parm( __p ) {};
        friend _WPRTLINK istream &operator>>( istream &, const imanip<T> & );
    private:
        istream &(*__fn)( istream &, T );
        T __parm;
    };
template<class T>
    _WPRTLINK istream &operator>>( istream &__is, const imanip<T> &__im ) {
        __im.__fn( __is, __im.__parm );
        return( __is );
    }

template<class T>
    class _WPRTLINK omanip;
template<class T>
    class _WPRTLINK oapp {
    public:
        oapp( ostream &(*__f)( ostream &, T ) ) :
            __fn( __f ) {} ;
        omanip<T> operator()( T __p ) { return omanip<T>( __fn, __p ); };
    private:
        ostream &(*__fn)( ostream &, T );
    };
template<class T>
    class _WPRTLINK omanip {
    public:
        omanip( ostream &(*__f)( ostream &, T ), T __p ) :
            __fn( __f ), __parm( __p ) {};
        friend _WPRTLINK ostream &operator<<( ostream &, const omanip<T> & );
    private:
        ostream &(*__fn)( ostream &, T );
        T __parm;
    };
template<class T>
    _WPRTLINK ostream &operator<<( ostream &__os, const omanip<T> &__om ) {
        __om.__fn( __os, __om.__parm );
        return( __os );
    }

template<class T>
    class _WPRTLINK iomanip;
template<class T>
    class _WPRTLINK ioapp {
    public:
        ioapp( iostream &(*__f)( iostream &, T ) ) :
            __fn( __f ) {};
        iomanip<T> operator()( T __p ) { return iomanip<T>( __fn, __p ) };
    private:
        iostream &(*__fn)( iostream &, T );
    };
template<class T>
    class _WPRTLINK iomanip {
    public:
        iomanip( iostream &(*__f)( iostream &, T ), T __p ) :
            __fn( __f ), __parm( __p ) {};
        friend _WPRTLINK iostream &operator>>( iostream &, const iomanip<T> & );
    private:
        iostream &(*__fn)( iostream &, T );
        T __parm;
    };
template<class T>
    _WPRTLINK iostream &operator>>( iostream &__is, const iomanip<T> &__im ) {
        __im.__fn( __is, __im.__parm );
        return( __is );
    }

#pragma pack(__pop);

// applicator objects
_WPRTLINK extern sapp<long> _WCDATA resetiosflags;
_WPRTLINK extern sapp<int>  _WCDATA setbase;
_WPRTLINK extern sapp<int>  _WCDATA setfill;
_WPRTLINK extern sapp<long> _WCDATA setiosflags;
_WPRTLINK extern sapp<int>  _WCDATA setprecision;
_WPRTLINK extern sapp<int>  _WCDATA setw;

// define some compatibility macros for legacy code
#define SMANIP(__Typ)   smanip<__Typ>
#define SAPP(__Typ)     sapp<__Typ>
#define IMANIP(__Typ)   imanip<__Typ>
#define IAPP(__Typ)     iapp<__Typ>
#define OMANIP(__Typ)   omanip<__Typ>
#define OAPP(__Typ)     oapp<__Typ>
#define IOMANIP(__Typ)  iomanip<__Typ>
#define IOAPP(__Typ)    ioapp<__Typ>

#define SMANIP_define(__Typ)
#define IOMANIPdeclare(__Typ)

#endif
