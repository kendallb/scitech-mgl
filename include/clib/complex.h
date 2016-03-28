//
//  complex.h   Complex Numbers
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
#ifndef _COMPLEX_H_INCLUDED
#define _COMPLEX_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error complex.h is for use with C++
#endif

#if defined( __AXP__) || defined( __PPC__ )
#pragma library( "cplx.lib" )
#else
#if defined(__SW_FPC)
#if defined(__386__)
#if defined(__3S__)
#pragma library ( "cplx3s.lib" )
#else
#pragma library ( "cplx3r.lib" )
#endif
#else
#if defined(__MEDIUM__)
#pragma library ( "cplxm.lib" )
#elif defined(__COMPACT__)
#pragma library ( "cplxc.lib" )
#elif defined(__LARGE__)
#pragma library ( "cplxl.lib" )
#elif defined(__HUGE__)
#pragma library ( "cplxh.lib" )
#else
#pragma library ( "cplxs.lib" )
#endif
#endif
#else
#if defined(__386__)
#if defined(__3S__)
#pragma library ( "cplx73s.lib" )
#else
#pragma library ( "cplx73r.lib" )
#endif
#else
#if defined(__MEDIUM__)
#pragma library ( "cplx7m.lib" )
#elif defined(__COMPACT__)
#pragma library ( "cplx7c.lib" )
#elif defined(__LARGE__)
#pragma library ( "cplx7l.lib" )
#elif defined(__HUGE__)
#pragma library ( "cplx7h.lib" )
#else
#pragma library ( "cplx7s.lib" )
#endif
#endif
#endif
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

class _WPRTLINK Complex {
    double  __r;
    double  __i;

public:
    Complex();
    Complex( Complex const & );
    Complex( double __real, double __imag = 0.0 );

    Complex &operator = ( Complex const & );
    Complex &operator = ( double );
    Complex &operator += ( Complex const & );
    Complex &operator += ( double );
    Complex &operator -= ( Complex const & );
    Complex &operator -= ( double );
    Complex &operator *= ( Complex const & );
    Complex &operator *= ( double );
    Complex &operator /= ( Complex const & );
    Complex &operator /= ( double );

    Complex operator + () const;
    Complex operator - () const;

    double imag () const;
    double real () const;

    friend _WPRTLINK istream &operator >> ( istream &, Complex & );
    friend _WPRTLINK ostream &operator << ( ostream &, Complex const & );
};
#pragma pack(__pop);

typedef Complex complex;        // for compatibility

_WPRTLINK extern Complex operator + ( Complex const &, Complex const & );
_WPRTLINK extern Complex operator + ( Complex const &, double );
_WPRTLINK extern Complex operator + ( double,         Complex const & );
_WPRTLINK extern Complex operator - ( Complex const &, Complex const & );
_WPRTLINK extern Complex operator - ( Complex const &, double );
_WPRTLINK extern Complex operator - ( double,         Complex const & );
_WPRTLINK extern Complex operator * ( Complex const &, Complex const & );
_WPRTLINK extern Complex operator * ( Complex const &, double );
_WPRTLINK extern Complex operator * ( double,         Complex const & );
_WPRTLINK extern Complex operator / ( Complex const &, Complex const & );
_WPRTLINK extern Complex operator / ( Complex const &, double );
_WPRTLINK extern Complex operator / ( double,         Complex const & );

_WPRTLINK extern int operator == ( Complex const &, Complex const & );
_WPRTLINK extern int operator == ( Complex const &, double );
_WPRTLINK extern int operator == ( double,         Complex const & );
_WPRTLINK extern int operator != ( Complex const &, Complex const & );
_WPRTLINK extern int operator != ( Complex const &, double );
_WPRTLINK extern int operator != ( double,         Complex const & );

_WPRTLINK extern double  abs  ( Complex const & );      // magnitude of vector
_WPRTLINK extern Complex acos ( Complex const & );      // arccosine
_WPRTLINK extern Complex acosh( Complex const & );      // hyperbolic arccosine
_WPRTLINK extern double  arg  ( Complex const & );      // angle of vector
_WPRTLINK extern Complex asin ( Complex const & );      // arcsin
_WPRTLINK extern Complex asinh( Complex const & );      // hyperbolic arcsin
_WPRTLINK extern Complex atan ( Complex const & );      // arctangent
_WPRTLINK extern Complex atanh( Complex const & );      // hyperbolic arctangent
_WPRTLINK extern Complex conj ( Complex const & );      // conjugate
_WPRTLINK extern Complex cos  ( Complex const & );      // cosine
_WPRTLINK extern Complex cosh ( Complex const & );      // hyperbolic cosine
_WPRTLINK extern Complex exp  ( Complex const & );      // e raised to a power
_WPRTLINK extern double  imag ( Complex const & );      // imaginary part
_WPRTLINK extern Complex log  ( Complex const & );      // log base e
_WPRTLINK extern Complex log10( Complex const & );      // log base 10
_WPRTLINK extern double  norm ( Complex const & );      // square of magnitude
_WPRTLINK extern Complex polar( double __mag,           // polar to Complex
                                double __angle = 0 );
_WPRTLINK extern Complex pow  ( Complex const &__base,  // Complex ** Complex
                                Complex const &__power );
_WPRTLINK extern Complex pow  ( Complex const &__base,  // Complex ** double
                                double         __power );
_WPRTLINK extern Complex pow  ( double         __base,  // double ** Complex
                                Complex const &__power );
_WPRTLINK extern Complex pow  ( Complex const &__base,  // Complex ** int
                                int            __power );
_WPRTLINK extern double  real ( Complex const & );      // real part
_WPRTLINK extern Complex sin  ( Complex const & );      // sin
_WPRTLINK extern Complex sinh ( Complex const & );      // hyperbolic sin
_WPRTLINK extern Complex sqrt ( Complex const & );      // square root
_WPRTLINK extern Complex tan  ( Complex const & );      // tan
_WPRTLINK extern Complex tanh ( Complex const & );      // hyperbolic tangent

inline Complex::Complex() {
    __r = 0.0;
    __i = 0.0;
}

inline Complex::Complex( Complex const &__cv ) {
    __r = __cv.__r;
    __i = __cv.__i;
}

inline Complex::Complex( double __real, double __imag ) {
    __r = __real;
    __i = __imag;
}

inline Complex &Complex::operator = ( Complex const &__cv ) {
    __r = __cv.__r;
    __i = __cv.__i;
    return( *this );
}

inline Complex &Complex::operator = ( double __dv ) {
    __r = __dv;
    __i = 0.0;
    return( *this );
}

inline Complex &Complex::operator += ( Complex const &__cv ) {
    __r += __cv.__r;
    __i += __cv.__i;
    return( *this );
}

inline Complex &Complex::operator += ( double __dv ) {
    __r += __dv;
    return( *this );
}

inline Complex &Complex::operator -= ( Complex const &__cv ) {
    __r -= __cv.__r;
    __i -= __cv.__i;
    return( *this );
}

inline Complex &Complex::operator -= ( double __dv ) {
    __r -= __dv;
    return( *this );
}

inline Complex &Complex::operator *= ( double __dv ) {
    __r *= __dv;
    __i *= __dv;
    return( *this );
}

inline Complex &Complex::operator /= ( double __dv ) {
    __r /= __dv;
    __i /= __dv;
    return( *this );
}

inline Complex operator + ( Complex const &__cv1, Complex const &__cv2 ) {
    return Complex( __cv1.real() + __cv2.real(), __cv1.imag() + __cv2.imag() );
}

inline Complex operator + ( Complex const &__cv, double __dv ) {
    return Complex( __cv.real() + __dv, __cv.imag() );
}

inline Complex operator + ( double __dv, Complex const &__cv ) {
    return Complex( __dv + __cv.real(), __cv.imag() );
}

inline Complex operator - ( Complex const &__cv1, Complex const &__cv2 ) {
    return Complex( __cv1.real() - __cv2.real(), __cv1.imag() - __cv2.imag() );
}

inline Complex operator - ( Complex const &__cv, double __dv ) {
    return Complex( __cv.real() - __dv, __cv.imag() );
}

inline Complex operator - ( double __dv, Complex const &__cv ) {
    return Complex( __dv - __cv.real(), - __cv.imag() );
}

inline Complex operator * ( Complex const &__cv1, Complex const &__cv2 ) {
    return Complex( __cv1.real()*__cv2.real() - __cv1.imag()*__cv2.imag(),
                    __cv1.imag()*__cv2.real() + __cv1.real()*__cv2.imag() );
}

inline Complex operator * ( Complex const &__cv, double __dv ) {
    return Complex( __cv.real() * __dv, __cv.imag() * __dv );
}

inline Complex operator * ( double __dv, Complex const &__cv ) {
    return Complex( __dv * __cv.real(), __dv * __cv.imag() );
}

inline Complex operator / ( Complex const &__cv, double __dv ) {
    return Complex( __cv.real() / __dv, __cv.imag() / __dv );
}

inline Complex Complex::operator + () const {
    return( *this );
}

inline Complex Complex::operator - () const {
    return Complex( -__r, -__i );
}

inline int operator == ( Complex const &__cv1, Complex const &__cv2 ) {
    return( __cv1.real() == __cv2.real()  &&  __cv1.imag() == __cv2.imag() );
}

inline int operator == ( Complex const &__cv, double __dv ) {
    return( __cv.imag() == 0.0  &&  __cv.real() == __dv );
}

inline int operator == ( double __dv, Complex const &__cv ) {
    return( __cv.imag() == 0.0  &&  __dv == __cv.real() );
}

inline int operator != ( Complex const &__cv1, Complex const &__cv2 ) {
    return( __cv1.real() != __cv2.real()  ||  __cv1.imag() != __cv2.imag() );
}

inline int operator != ( Complex const &__cv, double __dv ) {
    return( __cv.imag() != 0.0  ||  __cv.real() != __dv );
}

inline int operator != ( double __dv, Complex const &__cv ) {
    return( __cv.imag() != 0.0  ||  __dv != __cv.real() );
}

inline Complex conj( Complex const &__cv ) {
    return Complex( __cv.real(), -__cv.imag() );
}

inline double Complex::real() const {
    return( __r );
}

inline double real( Complex const &__cv ) {
    return( __cv.real() );
}

inline double Complex::imag() const {
    return( __i );
}

inline double imag( Complex const &__cv ) {
    return( __cv.imag() );
}

inline double norm( Complex const &__cv ) {
    return( __cv.real() * __cv.real() + __cv.imag() * __cv.imag() );
}


#endif
