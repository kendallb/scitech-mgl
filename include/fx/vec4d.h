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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Header file for the 4d point and vector macros and C++
*               wrapper classes. They can be constructed from either IEEE
*               float's, double's or fixed point numbers.
*
****************************************************************************/

#ifndef __FX_VEC4D_H
#define __FX_VEC4D_H

#ifndef __FX_VEC3D_H
#include "fx/vec3d.h"
#endif

/*----------------------------- Inline functions --------------------------*/

#ifndef __cplusplus

/* Define the C style structures for representing 4d vectors */

typedef struct {
    real    x,y,z,w;
    } FXPoint4d;

typedef FXPoint4d   FXVec4d;

#endif

#define FXVec4dSet(v1,x1,y1,z1,w1)                                      \
{   (v1).x = (x1);                                                      \
    (v1).y = (y1);                                                      \
    (v1).z = (z1);                                                      \
    (v1).w = (w1); }

#define FXVec4dCopy(r,v1)                                               \
    (r) = (v1)

#define FXVec4dAdd(r,v1,v2)                                             \
{   (r).x = (v1).x + (v2).x;                                            \
    (r).y = (v1).y + (v2).y;                                            \
    (r).z = (v1).z + (v2).z;                                            \
    (r).w = (v1).w + (v2).w; }

#define FXVec4dAddEq(v1,v2)                                             \
{   (v1).x += (v2).x;                                                   \
    (v1).y += (v2).y;                                                   \
    (v1).z += (v2).z;                                                   \
    (v1).w += (v2).w; }

#define FXVec4dSub(r,v1,v2)                                             \
{   (r).x = (v1).x - (v2).x;                                            \
    (r).y = (v1).y - (v2).y;                                            \
    (r).z = (v1).z - (v2).z;                                            \
    (r).w = (v1).w - (v2).w; }

#define FXVec4dSubEq(v1,v2)                                             \
{   (v1).x -= (v2).x;                                                   \
    (v1).y -= (v2).y;                                                   \
    (v1).z -= (v2).z;                                                   \
    (v1).w -= (v2).w; }

#define FXVec4dMul(r,v1,v2)                                             \
{   (r).x = FXmul((v1).x,(v2).x);                                       \
    (r).y = FXmul((v1).y,(v2).y);                                       \
    (r).z = FXmul((v1).z,(v2).z);                                       \
    (r).w = FXmul((v1).w,(v2).w); }

#define FXVec4dMulEq(v1,v2)                                             \
{   FXmuleq((v1).x,(v2).x);                                             \
    FXmuleq((v1).y,(v2).y);                                             \
    FXmuleq((v1).z,(v2).z);                                             \
    FXmuleq((v1).w,(v2).w); }

#define FXVec4dScale(r,v1,s)                                            \
{   (r).x = FXmul((v1).x,s);                                            \
    (r).y = FXmul((v1).y,s);                                            \
    (r).z = FXmul((v1).z,s);                                            \
    (r).w = FXmul((v1).w,s); }

#define FXVec4dScaleEq(v1,s)                                            \
{   FXmuleq((v1).x,s);                                                  \
    FXmuleq((v1).y,s);                                                  \
    FXmuleq((v1).z,s);                                                  \
    FXmuleq((v1).w,s); }

#define FXVec4dScaleInt(r,v1,s)                                         \
{   (r).x = (v1).x * (int)(s);                                          \
    (r).y = (v1).y * (int)(s);                                          \
    (r).z = (v1).z * (int)(s);                                          \
    (r).w = (v1).w * (int)(s); }

#define FXVec4dScaleIntEq(v1,s)                                         \
{   (v1).x *= (int)(s);                                                 \
    (v1).y *= (int)(s);                                                 \
    (v1).z *= (int)(s);                                                 \
    (v1).w *= (int)(s); }

#ifdef  FX_FIXED

#define FXVec4dDiv(r,v1,s)                                              \
{   (r).x = FXdiv((v1).x,s);                                            \
    (r).y = FXdiv((v1).y,s);                                            \
    (r).z = FXdiv((v1).z,s);                                            \
    (r).w = FXdiv((v1).w,s); }

#define FXVec4dDivEq(v1,s)                                              \
{   FXdiveq((v1).x,s);                                                  \
    FXdiveq((v1).y,s);                                                  \
    FXdiveq((v1).z,s);                                                  \
    FXdiveq((v1).w,s); }

#define FXVec4dDivInt(r,v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec4dDiv(r,v1,_s); }

#define FXVec4dDivIntEq(v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec4dDivEq(v1,_s); }

#define FXVec4dNormalise(v1)                                            \
{   real _s = FXVec4dLength(v1);                                        \
    FXVec4dDivEq(v1,_s); }

#else   /* !FX_FIXED */

#define FXVec4dDiv(r,v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec4dScale(r,v1,_s); }

#define FXVec4dDivEq(v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec4dScaleEq(v1,_s); }

#define FXVec4dDivInt(r,v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec4dScale(r,v1,_s); }

#define FXVec4dDivIntEq(v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec4dScaleEq(v1,_s); }

#define FXVec4dNormalise(v1)                                            \
{   real _s = 1.0 / FXVec4dLength(v1);                                  \
    FXVec4dScaleEq(v1,_s); }

#endif  /* !FX_FIXED */

#define FXVec4dNegate(r,v1)                                             \
{   (r).x = -(v1).x;                                                    \
    (r).y = -(v1).y;                                                    \
    (r).z = -(v1).z;                                                    \
    (r).w = -(v1).w; }

#define FXVec4dNegateEq(v1) FXVec4dNegate(v1,v1)

#define FXVec4dLength(v1)                                               \
    FXsqrt(FXsquare((v1).x) + FXsquare((v1).y) + FXsquare((v1).z)       \
        + FXsquare((v1).w))

#define FXVec4dIsZero(v1)                                               \
    (_FZERO((v1).x,_FUDGE) && _FZERO((v1).y,_FUDGE)                     \
        && _FZERO((v1).z,_FUDGE) && _FZERO((v1).w,_FUDGE))

#define FXVec4dIsEqual(v1,v2)                                           \
    (_FEQ((v1).x,(v2).x,_FUDGE) && _FEQ((v1).y,(v2).y,_FUDGE)           \
        && _FEQ((v1).z,(v2).z,_FUDGE) && _FEQ((v1).z,(v2).w,_FUDGE))

#define FXVec4dTo3d(r,v1)                                               \
    FXVec3dDiv(r,v1,(v1).w)

#define FXVec4dHomogenise(v1)                                           \
{   FXVec3dDivEq(v1,(v1).w);                                            \
    (v1).w = REAL(1); }

/*--------------------------- Class Definition ----------------------------*/

#ifdef  __cplusplus

class   FXVec4d;                // Forward reference to class

//---------------------------------------------------------------------------
// The following defines a 4d point class. 4d points are derived directly
// from 3d points, and contain an extra homogenous coordinate. Operations
// such as the dot product and cross product are meaningless for 4d vectors
// unless the vector has been converted back to 3d.
//---------------------------------------------------------------------------

class FXPoint4d {
public:
    real    x,y,z,w;

            // Default constructor (does nothing)
    inline  FXPoint4d() {};

            // Constructor given 3 real numbers
    inline  FXPoint4d(real x1,real y1,real z1) { x = x1; y = y1; z = z1; w = REAL(1); };

            // Constructor given 4 real numbers
    inline  FXPoint4d(real x1,real y1,real z1,real w1) { x = x1; y = y1; z = z1; w = w1; };

            // Constructor given an array of real numbers
    inline  FXPoint4d(const real *v) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; };

            // Constructor given a reference to a 3d point
    inline  FXPoint4d(const FXPoint3d& p) { x = p.x; y = p.y; z = p.z; w = REAL(1); };

            // Copy constructor
    inline  FXPoint4d(const FXPoint4d& p) { x = p.x; y = p.y; z = p.z; w = p.w; };

            // Standard arithmetic operators for 4d vectors
    friend  FXPoint4d operator + (const FXPoint4d& v1,const FXVec4d& v2);
    friend  FXVec4d operator - (const FXPoint4d& v1,const FXPoint4d& v2);

            // Faster method to add
    inline  FXPoint4d& operator += (const FXVec4d& v);

            // Method to homogenise a 4d point into 3 space and return it
    inline  void to3d(FXPoint3d& r);

            // Method to homogenise a 4d point into 3 space internally.
    inline  FXPoint4d& homogenise();

            // Method to determine if two points are equal (fuzzy)
    inline  ibool operator == (const FXPoint4d& v) const;

            // Friend to display the contents of a 4d point
    friend  ostream& operator << (ostream& s,const FXPoint4d& v);
    };

//---------------------------------------------------------------------------
// The following defines a 4d vector class. The 4d vector class is derived
// directly from the 4d point class.
//---------------------------------------------------------------------------

class FXVec4d : public FXPoint4d {
public:
            // Default constructor
    inline  FXVec4d()   : FXPoint4d() {};

            // Constructor given 3 real numbers
    inline  FXVec4d(real x1,real y1,real z1) : FXPoint4d(x1,y1,z1) {};

            // Constructor given 4 real numbers
    inline  FXVec4d(real x1,real y1,real z1,real w1) : FXPoint4d(x1,y1,z1,w1) {};

            // Constructor given an array of real numbers
    inline  FXVec4d(const real *v) : FXPoint4d(v) {};

            // Constructor given a reference to a 3d vector
    inline  FXVec4d(const FXVec3d& v) : FXPoint4d(v.x,v.y,v.z) {};

            // Constructor given a 4d point
    inline  FXVec4d(const FXPoint4d& p) : FXPoint4d(p.x,p.y,p.z,p.w) {};

            // Copy constructor
    inline  FXVec4d(const FXVec4d& v) : FXPoint4d(v.x,v.y,v.z) { w = v.w; };

            // Standard arithmetic operators for 4d vectors
    friend  FXVec4d operator + (const FXVec4d& v1,const FXVec4d& v2);
    friend  FXVec4d operator * (const FXVec4d& v1,const FXVec4d& v2);
    friend  FXVec4d operator * (const FXVec4d& v,real s);
    friend  FXVec4d operator * (real s,const FXVec4d& v);
    friend  FXVec4d operator * (const FXVec4d& v,int s);
    friend  FXVec4d operator * (int s,const FXVec4d& v);
    friend  FXVec4d operator / (const FXVec4d& v,real s);
    friend  FXVec4d operator / (const FXVec4d& v,int s);

            // Faster methods to add and multiply
    inline  FXVec4d& operator += (const FXVec4d& v);
    inline  FXVec4d& operator -= (const FXVec4d& v);
    inline  FXVec4d& operator *= (const FXVec4d& v);
    inline  FXVec4d& operator *= (real s);
    inline  FXVec4d& operator *= (int s);
    inline  FXVec4d& operator /= (real s);
    inline  FXVec4d& operator /= (int s);

            // Methods to negate a vector
    inline  FXVec4d operator - () const;
    inline  FXVec4d& negate();          // Faster

            // Method to determine if a vector is zero length (fuzzy)
    inline  ibool isZero() const;

            // Method to return the length of the vector
    inline  real length() const;
    };

/*------------------------- Inline member functions -----------------------*/

//---------------------------------------------------------------------------
// Standard arithmetic operators for 4d vectors.
//---------------------------------------------------------------------------

inline FXPoint4d operator + (const FXPoint4d& p1,const FXVec4d& v2)
{ return FXPoint4d(p1.x + v2.x, p1.y + v2.y, p1.z + v2.z, p1.w + v2.w); }

inline FXVec4d operator + (const FXVec4d& v1,const FXVec4d& v2)
{ return FXVec4d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }

inline FXVec4d operator - (const FXPoint4d& v1,const FXPoint4d& v2)
{ return FXVec4d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }

inline FXVec4d operator * (const FXVec4d& v1,const FXVec4d& v2)
{ return FXVec4d(FXmul(v1.x,v2.x), FXmul(v1.y,v2.y),
               FXmul(v1.z,v2.z), FXmul(v1.w,v2.w)); }

inline FXVec4d operator * (const FXVec4d& v1,real s)
{ return FXVec4d(FXmul(v1.x,s), FXmul(v1.y,s), FXmul(v1.z,s), FXmul(v1.w,s)); }

inline FXVec4d operator * (real s,const FXVec4d& v1)
{ return v1 * s; }

inline FXVec4d operator * (const FXVec4d& v1,int s)
{ return FXVec4d(v1.x * s, v1.y * s, v1.z * s, v1.w * s); }

inline FXVec4d operator * (int s,const FXVec4d& v1)
{ return v1 * s; }

inline FXVec4d operator / (const FXVec4d& v1,real s)
{
    CHECK(s != 0);
#ifndef FX_FIXED
    return v1 * ((real)1.0 / s);
#else
    return FXVec4d(FXdiv(v1.x,s), FXdiv(v1.y,s), FXdiv(v1.z,s), FXdiv(v1.w,s));
#endif
}

inline FXVec4d operator / (const FXVec4d& v1,int s)
{ return v1 / FXintToReal(s); }

//---------------------------------------------------------------------------
// Faster methods to add and multiply 4d vectors.
//---------------------------------------------------------------------------

inline FXPoint4d& FXPoint4d::operator += (const FXVec4d& v)
{ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }

inline FXVec4d& FXVec4d::operator += (const FXVec4d& v)
{ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }

inline FXVec4d& FXVec4d::operator -= (const FXVec4d& v)
{ x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }

inline FXVec4d& FXVec4d::operator *= (const FXVec4d& v)
{ FXmuleq(x,v.x); FXmuleq(y,v.y); FXmuleq(z,v.z); FXmuleq(w,v.w); return *this; }

inline FXVec4d& FXVec4d::operator *= (real s)
{ FXmuleq(x,s); FXmuleq(y,s); FXmuleq(z,s); FXmuleq(w,s); return *this; }

inline FXVec4d& FXVec4d::operator *= (int s)
{ x *= s; y *= s; z *= s; w *= s; return *this; }

inline FXVec4d& FXVec4d::operator /= (real s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    FXdiveq(x,s);
    FXdiveq(y,s);
    FXdiveq(z,s);
    FXdiveq(w,s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
    z *= _s;
    w *= _s;
#endif
    return *this;
}

inline FXVec4d& FXVec4d::operator /= (int s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    real _s = FXintToReal(s);
    FXdiveq(x,_s);
    FXdiveq(y,_s);
    FXdiveq(z,_s);
    FXdiveq(w,_s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
    z *= _s;
    w *= _s;
#endif
    return *this;
}

//---------------------------------------------------------------------------
// Methods to negate vectors.
//---------------------------------------------------------------------------

inline FXVec4d FXVec4d::operator - () const
{ return FXVec4d(-x,-y,-z,-w); }

inline FXVec4d& FXVec4d::negate()
{ x = -x; y = -y; z = -z; w = -w; return *this; }

//---------------------------------------------------------------------------
// Miscellaneous operations.
//---------------------------------------------------------------------------

inline FXPoint4d& FXPoint4d::homogenise()
{
    CHECK(w != 0);
#ifdef  FX_FIXED
    FXdiveq(x,w);
    FXdiveq(y,w);
    FXdiveq(z,w);
#else
    real _w = 1.0 / (w);
    x *= _w;
    y *= _w;
    z *= _w;
#endif
    w = REAL(1);
    return *this;
}

inline void FXPoint4d::to3d(FXPoint3d& r)
{ CHECK(w != 0); FXVec4dTo3d(r,*this); }

inline ibool FXVec4d::isZero() const
{ return FXVec4dIsZero(*this); }

inline real FXVec4d::length() const
{ return FXVec4dLength(*this); }

inline ibool FXPoint4d::operator == (const FXPoint4d& v) const
{ return FXVec4dIsEqual(*this,v); }

inline ostream& operator << (ostream& o,const FXPoint4d& v)
{
    return  o << '(' << FXrealToDbl(v.x) << ',' << FXrealToDbl(v.y)
              << ',' << FXrealToDbl(v.z) << ',' << FXrealToDbl(v.w) << ')';
}

inline ostream& operator << (ostream& o,const FXVec4d& v)
{ return o << (FXPoint4d&)v; }

#endif  /* __cplusplus */

#endif  /* __FX_VEC4D_H */
