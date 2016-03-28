/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C/C++
* Environment:  any
*
* Description:  Header file for the 2d point and vector macros and C++
*               wrapper classes. They can be constructed from either IEEE
*               float's, double's or fixed point numbers.
*
****************************************************************************/

#ifndef __FX_VEC2D_H
#define __FX_VEC2D_H

#ifndef __FX_REAL_H
#include "fx/real.h"
#endif

/*----------------------------- Inline functions --------------------------*/

#ifndef __cplusplus

/* Define the C style structures for representing 2d vectors */

typedef struct {
    real    x,y;
    } FXPoint2d;

typedef FXPoint2d   FXVec2d;

#endif

#define FXVec2dSet(v1,x1,y1)                                            \
{   (v1).x = (x1);                                                      \
    (v1).y = (y1); }

#define FXVec2dCopy(r,v1)                                               \
    (r) = (v1)

#define FXVec2dAdd(r,v1,v2)                                             \
{   (r).x = (v1).x + (v2).x;                                            \
    (r).y = (v1).y + (v2).y; }

#define FXVec2dAddEq(v1,v2)                                             \
{   (v1).x += (v2).x;                                                   \
    (v1).y += (v2).y; }

#define FXVec2dSub(r,v1,v2)                                             \
{   (r).x = (v1).x - (v2).x;                                            \
    (r).y = (v1).y - (v2).y; }

#define FXVec2dSubEq(v1,v2)                                             \
{   (v1).x -= (v2).x;                                                   \
    (v1).y -= (v2).y; }

#define FXVec2dMul(r,v1,v2)                                             \
{   (r).x = FXmul((v1).x,(v2).x);                                       \
    (r).y = FXmul((v1).y,(v2).y); }

#define FXVec2dMulEq(v1,v2)                                             \
{   FXmuleq((v1).x,(v2).x);                                             \
    FXmuleq((v1).y,(v2).y); }

#define FXVec2dScale(r,v1,s)                                            \
{   (r).x = FXmul((v1).x,s);                                            \
    (r).y = FXmul((v1).y,s); }

#define FXVec2dScaleEq(v1,s)                                            \
{   FXmuleq((v1).x,s);                                                  \
    FXmuleq((v1).y,s); }

#define FXVec2dScaleInt(r,v1,s)                                         \
{   (r).x = (v1).x * (int)(s);                                          \
    (r).y = (v1).y * (int)(s); }

#define FXVec2dScaleIntEq(v1,s)                                         \
{   (v1).x *= (int)(s);                                                 \
    (v1).y *= (int)(s); }

#ifdef  FX_FIXED

#define FXVec2dDiv(r,v1,s)                                              \
{   (r).x = FXdiv((v1).x,s);                                            \
    (r).y = FXdiv((v1).y,s); }

#define FXVec2dDivEq(v1,s)                                              \
{   FXdiveq(((v1).x),s);                                                \
    FXdiveq(((v1).y),s); }

#define FXVec2dDivInt(r,v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec2dDiv(r,v1,_s); }

#define FXVec2dDivIntEq(v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec2dDivEq(v1,_s); }

#define FXVec2dNormalise(v1)                                            \
{   real _s = FXVec2dLength(v1);                                        \
    FXVec2dDivEq(v1,_s); }

#else   /* !FX_FIXED */

#define FXVec2dDiv(r,v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec2dScale(r,v1,_s); }

#define FXVec2dDivEq(v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec2dScaleEq(v1,_s); }

#define FXVec2dDivInt(r,v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec2dScale(r,v1,_s); }

#define FXVec2dDivIntEq(v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec2dScaleEq(v1,_s); }

#define FXVec2dNormalise(v1)                                            \
{   real _s = 1.0 / FXVec2dLength(v1);                                  \
    FXVec2dScaleEq(v1,_s); }

#endif  /* !FX_FIXED */

#define FXVec2dNegate(r,v1)                                             \
{   (r).x = -(v1).x;                                                    \
    (r).y = -(v1).y; }

#define FXVec2dNegateEq(v1) FXVec2dNegate(v1,v1)

#define FXVec2dDot(v1,v2)                                               \
    (FXmul((v1).x,(v2).x) + FXmul((v1).y,(v2).y))

#define FXVec2dLength(v1)                                               \
    FXsqrt(FXsquare((v1).x) + FXsquare((v1).y))

#define FXVec2dPerp(r,v1)                                               \
{   (r).x = -(v1).y;                                                    \
    (r).y = (v1).x; }

#define FXVec2dIsZero(v1)                                               \
    (_FZERO((v1).x,_FUDGE) && _FZERO((v1).y,_FUDGE))

#define FXVec2dIsEqual(v1,v2)                                           \
    (_FEQ((v1).x,(v2).x,_FUDGE) && _FEQ((v1).y,(v2).y,_FUDGE))

#define FXVec2dInterpolate(r,lo,hi,alpha)                               \
{   (r).x = _LERP((lo).x, (hi).x, (alpha));                             \
    (r).y = _LERP((lo).y, (hi).y, (alpha)); }

#define FXVec2dCombine(r,v1,s1,v2,s2)                                   \
{   (r).x = FXmul((v1).x,(s1)) + FXmul((v2).x,(s2));                    \
    (r).y = FXmul((v1).y,(s1)) + FXmul((v2).y,(s2)); }

#define FXVec2dScaleAndAdd(r,p,v,s)                                     \
{   (r).x = (p).x + FXmul((v).x,(s));                                   \
    (r).y = (p).y + FXmul((v).y,(s)); }

#define FXVec2dDistance(p1,p2)                                          \
    FXsqrt(FXsquare((p2).x - (p1).x) + FXsquare((p2).y - (p1).y))

/* Macros to convert from real to integer points */

#define FXVec2dToInt(r,p)                                               \
{                                                                       \
    (r).x = FXrndToInt((p).x);                                          \
    (r).y = FXrndToInt((p).y);                                          \
}

#define FXIntToVec2d(r,p)                                               \
{                                                                       \
    (r).x = FXintToReal((p).x);                                         \
    (r).y = FXintToReal((p).y);                                         \
}

/* Macros to convert from real to 16.16 fixed point */

#define FXVec2dToFixed(r,p)                                             \
{                                                                       \
    (r).x = FXrealToFixed((p).x);                                       \
    (r).y = FXrealToFixed((p).y);                                       \
}

#define FXFixedToVec2d(r,p)                                             \
{                                                                       \
    (r).x = FXfixedToReal((p).x);                                       \
    (r).y = FXfixedToReal((p).y);                                       \
}

/*-------------------------- Class Definitions ----------------------------*/

#ifdef  __cplusplus

#ifndef __IOSTREAM_H
#include <iostream.h>
#endif

#ifndef __TCL_TECHLIB_HPP
#include "tcl/techlib.hpp"
#endif

class FXVec2d;              // Forward reference to vector class

//---------------------------------------------------------------------------
// The following defines a 2d point class.
//---------------------------------------------------------------------------

class FXPoint2d {
public:
    real    x,y;            // Coordinates of the vector

            // Default constructor (does nothing)
    inline  FXPoint2d() {};

            // Constructor given 2 real numbers
    inline  FXPoint2d(real x1,real y1)  { x = x1; y = y1; };

            // Copy constructor
    inline  FXPoint2d(const FXPoint2d& p)   { x = p.x; y = p.y; };

            // Constructor given an array of real numbers
    inline  FXPoint2d(const real *v)    { x = v[0]; y = v[1]; };

            // Overload cast to an array of real's. This is useful for
            // passing points to C routines or to libraries like OpenGL.
    inline  operator real * ()  { return (real*)&x; };

            // Standard arithmetic operators for 2d points
    friend  FXPoint2d operator + (const FXPoint2d& v1,const FXVec2d& v2);
    friend  FXVec2d operator - (const FXPoint2d& v1,const FXPoint2d& v2);

            // Faster method to add
    inline  FXPoint2d& operator += (const FXVec2d& v);

            // Method to determine if two points are equal (fuzzy)
    inline  ibool operator == (const FXPoint2d& v) const;

            // Friend to display the contents of a 2d point
    friend  ostream& operator << (ostream& s,const FXPoint2d& v);

            /* Set of inline friends to peform specialised arithmetic
             * generally used in computer graphics.
             */

            // Linearly interpolate between two points
    friend  void interpolate(FXPoint2d& result,const FXPoint2d& lo,
                const FXPoint2d& hi,real alpha);

            // Add and scale a point and a vector.
    friend  void scaleAndAdd(FXPoint2d& result,const FXPoint2d& p,
                const FXVec2d& v,real scale);

            // Compute the distance between two points
    friend  real distance(const FXPoint2d& p1,const FXPoint2d& p2);
    };

//---------------------------------------------------------------------------
// The following defines a 2d vector class. The 2d vector class is a
// specialisation of the 2d point class since semantically they are
// similar - anything you can do with points you can do with vectors.
//---------------------------------------------------------------------------

class FXVec2d : public FXPoint2d {
public:
            // Default constructor
    inline  FXVec2d() : FXPoint2d() {};

            // Constructor given 2 real numbers
    inline  FXVec2d(real x,real y) : FXPoint2d(x,y) {};

            // Constructor given an array of real numbers
    inline  FXVec2d(const real *v) : FXPoint2d(v) {};

            // Constructor given a 2d point
    inline  FXVec2d(const FXPoint2d& p) : FXPoint2d(p.x,p.y) {};

            // Copy constructor
    inline  FXVec2d(const FXVec2d& v) : FXPoint2d(v.x,v.y) {};

            // Standard arithmetic operators for 2d vectors
    friend  FXVec2d operator + (const FXVec2d& v1,const FXVec2d& v2);
    friend  FXVec2d operator * (const FXVec2d& v1,const FXVec2d& v2);
    friend  FXVec2d operator * (const FXVec2d& v,real s);
    friend  FXVec2d operator * (real s,const FXVec2d& v);
    friend  FXVec2d operator * (const FXVec2d& v,int s);
    friend  FXVec2d operator * (int s,const FXVec2d& v);
    friend  FXVec2d operator / (const FXVec2d& v,real s);
    friend  FXVec2d operator / (const FXVec2d& v,int s);

            // Faster methods to add and multiply
    inline  FXVec2d& operator += (const FXVec2d& v);
    inline  FXVec2d& operator -= (const FXVec2d& v);
    inline  FXVec2d& operator *= (const FXVec2d& v);
    inline  FXVec2d& operator *= (real s);
    inline  FXVec2d& operator *= (int s);
    inline  FXVec2d& operator /= (real s);
    inline  FXVec2d& operator /= (int s);

            // Methods to negate a vector
    inline  FXVec2d operator - () const;
    inline  FXVec2d& negate();          // Faster

            // Compute dot product of two vectors
    friend  real operator % (const FXVec2d& v1,const FXVec2d& v2);

            // Methods to normalise a vector
    inline  FXVec2d operator ~ () const;
    inline  FXVec2d& normalise();           // Faster

            // Method to return a perpendicular vector
    inline  void perp(FXVec2d& result) const;

            // Method to determine if a vector is zero length (fuzzy)
    inline  ibool isZero() const;

            // Method to determine the vector's length
    inline  real length() const;

            /* Set of inline friends to peform specialised arithmetic
             * generally used in computer graphics.
             */

            // Combine two vectors together with scaling
    friend  void combine(FXVec2d& result,const FXVec2d& v1,real scale1,
                const FXVec2d& v2,real scale2);
    };

/*------------------------- Inline member functions -----------------------*/

//---------------------------------------------------------------------------
// Standard arithmetic operators for 2d vectors.
//---------------------------------------------------------------------------

inline FXPoint2d operator + (const FXPoint2d& p1,const FXVec2d& v2)
{ return FXPoint2d(p1.x + v2.x, p1.y + v2.y); }

inline FXVec2d operator + (const FXVec2d& v1,const FXVec2d& v2)
{ return FXVec2d(v1.x + v2.x, v1.y + v2.y); }

inline FXVec2d operator - (const FXPoint2d& p1,const FXPoint2d& p2)
{ return FXVec2d(p1.x - p2.x, p1.y - p2.y); }

inline FXVec2d operator * (const FXVec2d& v1,const FXVec2d& v2)
{ return FXVec2d(FXmul(v1.x,v2.x), FXmul(v1.y,v2.y)); }

inline FXVec2d operator * (const FXVec2d& v1,real s)
{ return FXVec2d(FXmul(v1.x,s), FXmul(v1.y,s)); }

inline FXVec2d operator * (real s,const FXVec2d& v1)
{ return v1 * s; }

inline FXVec2d operator * (const FXVec2d& v1,int s)
{ return FXVec2d(v1.x * s, v1.y * s); }

inline FXVec2d operator * (int s,const FXVec2d& v1)
{ return v1 * s; }

inline FXVec2d operator / (const FXVec2d& v1,real s)
//---------------------------------------------------------------------------
// To divide a vector by a real number, it is actually faster
// to compute the inverse of the scale and multiply the vector
// out. The following method is marginally slower on a '286
// IBM PC with no co-pro, but faster on machines with co-pros.
// If we are doing fixed point arithmetic, divisions are only slightly
// slower (for 386 based code), so we do the division explicitly. This
// also gives us better precision for the division than multipling by
// the inverse (we only get 16 bits of fractional precision otherwise!).
//---------------------------------------------------------------------------
{
    CHECK(s != 0);
#ifndef FX_FIXED
    return v1 * ((real)1.0 / s);
#else
    return FXVec2d(FXdiv(v1.x,s), FXdiv(v1.y,s));
#endif
}

inline FXVec2d operator / (const FXVec2d& v1,int s)
{ return v1 / FXintToReal(s); }

//---------------------------------------------------------------------------
// Faster methods to add and multiply 2d vectors.
//---------------------------------------------------------------------------

inline FXPoint2d& FXPoint2d::operator += (const FXVec2d& v)
{ x += v.x; y += v.y; return *this; }

inline FXVec2d& FXVec2d::operator += (const FXVec2d& v)
{ x += v.x; y += v.y; return *this; }

inline FXVec2d& FXVec2d::operator -= (const FXVec2d& v)
{ x -= v.x; y -= v.y; return *this; }

inline FXVec2d& FXVec2d::operator *= (const FXVec2d& v)
{ FXmuleq(x,v.x); FXmuleq(y,v.y); return *this; }

inline FXVec2d& FXVec2d::operator *= (real s)
{ FXmuleq(x,s); FXmuleq(y,s); return *this; }

inline FXVec2d& FXVec2d::operator *= (int s)
{ x *= s; y *= s; return *this; }

inline FXVec2d& FXVec2d::operator /= (real s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    FXdiveq(x,s);
    FXdiveq(y,s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
#endif
    return *this;
}

inline FXVec2d& FXVec2d::operator /= (int s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    real _s = FXintToReal(s);
    FXdiveq(x,_s);
    FXdiveq(y,_s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
#endif
    return *this;
}

//---------------------------------------------------------------------------
// Methods to negate vectors.
//---------------------------------------------------------------------------

inline FXVec2d FXVec2d::operator - () const
{ return FXVec2d(-x,-y); }

inline FXVec2d& FXVec2d::negate()
{ x = -x; y = -y; return *this; }

//---------------------------------------------------------------------------
// Miscellaneous operations.
//---------------------------------------------------------------------------

inline real operator % (const FXVec2d& v1,const FXVec2d& v2)
{ return FXVec2dDot(v1,v2); }

inline FXVec2d FXVec2d::operator ~ () const
{ return FXVec2d(*this / (real)FXVec2dLength(*this)); }

inline FXVec2d& FXVec2d::normalise()
{
#ifdef  FX_FIXED
    real _s = FXVec2dLength(*this);
    FXdiveq(x,_s);
    FXdiveq(y,_s);
#else
    real _s = 1.0 / FXVec2dLength(*this);
    x *= _s;
    y *= _s;
#endif
    return *this;
}

inline void FXVec2d::perp(FXVec2d& result) const
{ FXVec2dPerp(result,*this); }

inline ibool FXVec2d::isZero() const
{ return FXVec2dIsZero(*this); }

inline real FXVec2d::length() const
{ return FXVec2dLength(*this); }

inline ibool FXPoint2d::operator == (const FXPoint2d& v) const
{ return FXVec2dIsEqual(*this,v); }

inline void interpolate(FXPoint2d& result,const FXPoint2d& lo,const FXPoint2d& hi,
        real alpha)
/****************************************************************************
*
* Function:     interpolate
* Parameters:   result  - 2d Point to place the result in
*               lo      - Low bound on interpolation
*               hi      - High bound on interpolation
*               alpha   - Interpolation value
*
* Description:  This routine linearly interpolates between two points lo
*               and hi depending on the value of alpha:
*
*                   alpha = 0       result = lo
*                   alpha = 1       result = hi
*
****************************************************************************/
{ FXVec2dInterpolate(result,lo,hi,alpha); }

inline void combine(FXVec2d& result,const FXVec2d& v1,real scale1,
        const FXVec2d& v2,real scale2)
/****************************************************************************
*
* Function:     combine
* Parameters:   result  - Vector to place the result in
*               v1      - First vector to combine
*               scale1  - Scale for first vector
*               v2      - Second vector to combine
*               scale2  - Scale for second vector
*
* Description:  This routine is a specialised routine to perform the
*               following vector operation:
*
*                   result = v1 * scale1 + v2 * scale2
*
*               It is a LOT faster to do it in a friend function rather
*               than doing in manually as in the above example.
*
****************************************************************************/
{ FXVec2dCombine(result,v1,scale1,v2,scale2); }

inline void scaleAndAdd(FXPoint2d& result,const FXPoint2d& p,const FXVec2d& v,
    real scale)
/****************************************************************************
*
* Function:     scaleAndAdd
* Parameters:   result  - Point to place the result into
*               p       - First point to add
*               v       - Second vector to scale and add
*               scale   - Amount to scale v by.
*
* Description:  This routine is a specialised routine to perform the
*               following operation (generally used in raytracing):
*
*                   result = p + v * scale;
*
*               It is a LOT faster to do it in a friend function rather
*               than doing in manually as in the above example.
*
****************************************************************************/
{ FXVec2dScaleAndAdd(result,p,v,scale); }

inline real distance(const FXPoint2d& p1,const FXPoint2d& p2)
/****************************************************************************
*
* Function:     distance
* Parameters:   p1  - First point
*               p2  - Second point
* Returns:      The distance between point p1 and point p2.
*
****************************************************************************/
{ return FXVec2dDistance(p1,p2); }

inline ostream& operator << (ostream& o,const FXPoint2d& v)
{
    return  o << '(' << FXrealToDbl(v.x) << ',' << FXrealToDbl(v.y) << ')';
}

inline ostream& operator << (ostream& o,const FXVec2d& v)
{ return o << (const FXPoint2d&)v; }

#endif  /* __cplusplus */

#endif  /* __FX_VEC2D_H */
