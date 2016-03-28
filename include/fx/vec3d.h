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
* Description:  Header file for the 3d point and vector macros and C++
*               wrapper classes. They can be constructed from either IEEE
*               float's, double's or fixed point numbers.
*
****************************************************************************/

#ifndef __FX_VEC3D_H
#define __FX_VEC3D_H

#ifndef __FX_VEC2D_H
#include "fx/vec2d.h"
#endif

/*----------------------------- Inline functions --------------------------*/

#ifdef  __cplusplus

class   FXPoint3d;      // Forward reference to classes
class   FXVec3d;

#else

/* Define the C style structures for representing 3d vectors */

typedef struct {
    real    x,y,z;
    } FXPoint3d;

typedef FXPoint3d   FXVec3d;

#endif

#define FXVec3dSet(v1,x1,y1,z1)                                         \
{   (v1).x = (x1);                                                      \
    (v1).y = (y1);                                                      \
    (v1).z = (z1); }

#define FXVec3dCopy(r,v1)                                               \
    (r) = (v1)

#define FXVec3dAdd(r,v1,v2)                                             \
{   (r).x = (v1).x + (v2).x;                                            \
    (r).y = (v1).y + (v2).y;                                            \
    (r).z = (v1).z + (v2).z; }

#define FXVec3dAddEq(v1,v2)                                             \
{   (v1).x += (v2).x;                                                   \
    (v1).y += (v2).y;                                                   \
    (v1).z += (v2).z; }

#define FXVec3dSub(r,v1,v2)                                             \
{   (r).x = (v1).x - (v2).x;                                            \
    (r).y = (v1).y - (v2).y;                                            \
    (r).z = (v1).z - (v2).z; }

#define FXVec3dSubEq(v1,v2)                                             \
{   (v1).x -= (v2).x;                                                   \
    (v1).y -= (v2).y;                                                   \
    (v1).z -= (v2).z; }

#define FXVec3dMul(r,v1,v2)                                             \
{   (r).x = FXmul((v1).x,(v2).x);                                       \
    (r).y = FXmul((v1).y,(v2).y);                                       \
    (r).z = FXmul((v1).z,(v2).z); }

#define FXVec3dMulEq(v1,v2)                                             \
{   FXmuleq((v1).x,(v2).x);                                             \
    FXmuleq((v1).y,(v2).y);                                             \
    FXmuleq((v1).z,(v2).z); }

#define FXVec3dScale(r,v1,s)                                            \
{   (r).x = FXmul((v1).x,s);                                            \
    (r).y = FXmul((v1).y,s);                                            \
    (r).z = FXmul((v1).z,s); }

#define FXVec3dScaleEq(v1,s)                                            \
{   FXmuleq((v1).x,s);                                                  \
    FXmuleq((v1).y,s);                                                  \
    FXmuleq((v1).z,s); }

#define FXVec3dScaleInt(r,v1,s)                                         \
{   (r).x = (v1).x * (int)(s);                                          \
    (r).y = (v1).y * (int)(s);                                          \
    (r).z = (v1).z * (int)(s); }

#define FXVec3dScaleIntEq(v1,s)                                         \
{   (v1).x *= (int)(s);                                                 \
    (v1).y *= (int)(s);                                                 \
    (v1).z *= (int)(s); }

#define FXVec3dDot(v1,v2)                                               \
    (FXmul((v1).x,(v2).x) + FXmul((v1).y,(v2).y) + FXmul((v1).z,(v2).z))

#define FXVec3dLength(v1)                                               \
    FXsqrt(FXsquare((v1).x) + FXsquare((v1).y) + FXsquare((v1).z))

#define FXVec3dDistance(p1,p2)                                          \
    FXsqrt(FXsquare((p2).x - (p1).x) + FXsquare((p2).y - (p1).y)        \
        + FXsquare((p2).z - (p1).z))

#define FXVec3dCross(r,v1,v2)                                           \
{   (r).x = FXmul((v1).y,(v2).z) - FXmul((v1).z,(v2).y);                \
    (r).y = FXmul((v1).z,(v2).x) - FXmul((v1).x,(v2).z);                \
    (r).z = FXmul((v1).x,(v2).y) - FXmul((v1).y,(v2).x); }

#ifdef  FX_FIXED

#define FXVec3dDiv(r,v1,s)                                              \
{   (r).x = FXdiv((v1).x,s);                                            \
    (r).y = FXdiv((v1).y,s);                                            \
    (r).z = FXdiv((v1).z,s); }

#define FXVec3dDivEq(v1,s)                                              \
{   FXdiveq((v1).x,s);                                                  \
    FXdiveq((v1).y,s);                                                  \
    FXdiveq((v1).z,s); }

#define FXVec3dDivInt(r,v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec3dDiv(r,v1,_s); }

#define FXVec3dDivIntEq(v1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXVec3dDivEq(v1,_s); }

#define FXVec3dNormalise(v1)                                            \
{   real _s = FXVec3dLength(v1);                                        \
    FXVec3dDivEq(v1,_s); }

#else   /* !FX_FIXED */

#define FXVec3dDiv(r,v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec3dScale(r,v1,_s); }

#define FXVec3dDivEq(v1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXVec3dScaleEq(v1,_s); }

#define FXVec3dDivInt(r,v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec3dScale(r,v1,_s); }

#define FXVec3dDivIntEq(v1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXVec3dScaleEq(v1,_s); }

#define FXVec3dNormalise(v1)                                            \
{   real _s = 1.0 / FXVec3dLength(v1);                                  \
    FXVec3dScaleEq(v1,_s); }

#endif  /* !FX_FIXED */

#define FXVec3dNegate(r,v1)                                             \
{   (r).x = -(v1).x;                                                    \
    (r).y = -(v1).y;                                                    \
    (r).z = -(v1).z; }

#define FXVec3dNegateEq(v1) FXVec3dNegate(v1,v1)

#define FXVec3dIsZero(v1)                                               \
    (_FZERO((v1).x,_FUDGE) && _FZERO((v1).y,_FUDGE)                     \
        && _FZERO((v1).z,_FUDGE))

#define FXVec3dIsEqual(v1,v2)                                           \
    (_FEQ((v1).x,(v2).x,_FUDGE) && _FEQ((v1).y,(v2).y,_FUDGE)           \
        && _FEQ((v1).z,(v2).z,_FUDGE))

#define FXVec3dInterpolate(r,lo,hi,alpha)                               \
{   (r).x = _LERP((lo).x, (hi).x, (alpha));                             \
    (r).y = _LERP((lo).y, (hi).y, (alpha));                             \
    (r).z = _LERP((lo).z, (hi).z, (alpha)); }

#define FXVec3dCombine(r,v1,s1,v2,s2)                                   \
{   (r).x = FXmul((v1).x,(s1)) + FXmul((v2).x,(s2));                    \
    (r).y = FXmul((v1).y,(s1)) + FXmul((v2).y,(s2));                    \
    (r).z = FXmul((v1).z,(s1)) + FXmul((v2).z,(s2)); }

#define FXVec3dScaleAndAdd(r,p,v,s)                                     \
{   (r).x = (p).x + FXmul((v).x,(s));                                   \
    (r).y = (p).y + FXmul((v).y,(s));                                   \
    (r).z = (p).z + FXmul((v).z,(s)); }

/* Macros to convert from real to 16.16/4.28 fixed point */

#define FXVec3dToFixed(r,p)                                             \
{                                                                       \
    (r).x = FXrealToFixed((p).x);                                       \
    (r).y = FXrealToFixed((p).y);                                       \
    (r).z = FXrealToZFixed((p).z);                                      \
}

#define FXFixedToVec3d(r,p)                                             \
{                                                                       \
    (r).x = FXfixedToReal((p).x);                                       \
    (r).y = FXfixedToReal((p).y);                                       \
    (r).z = FXzfixedToReal((p).z);                                      \
}

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

void FXVec3dPerp(FXVec3d *r,const FXVec3d *v1);

/*--------------------------- Class Definition ----------------------------*/

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */

//---------------------------------------------------------------------------
// The following defines a 3d point class.
//---------------------------------------------------------------------------

class FXPoint3d {
public:
    real    x,y,z;

            // Default constructor (does nothing)
    inline  FXPoint3d() {};

            // Constructor given 3 real numbers
    inline  FXPoint3d(real x1,real y1,real z1) { x = x1; y = y1; z = z1; };

            // Constructor given an array of real numbers
    inline  FXPoint3d(const real *v) { x = v[0]; y = v[1]; z = v[2]; };

            // Copy constructor
    inline  FXPoint3d(const FXPoint3d& p) { x = p.x; y = p.y; z = p.z; };

            // Standard arithmetic operators for 3d vectors
    friend  FXPoint3d operator + (const FXPoint3d& v1,const FXVec3d& v2);
    friend  FXVec3d operator - (const FXPoint3d& v1,const FXPoint3d& v2);

            // Faster method to add
    inline  FXPoint3d& operator += (const FXVec3d& v);

            // Method to determine if two points are equal (fuzzy)
    inline  ibool operator == (const FXPoint3d& v) const;

            // Friend to display the contents of a 3d point
    friend  ostream& operator << (ostream& s,const FXPoint3d& v);

            /* Set of inline friends to peform specialised arithmetic
             * generally used in computer graphics.
             */

            // Linearly interpolate between two points.
    friend  void interpolate(FXPoint3d& result,const FXPoint3d& lo,
                const FXPoint3d& hi,real alpha);

            // Add and scale two vectors.
    friend  void scaleAndAdd(FXPoint3d& result,const FXPoint3d& p,
                const FXVec3d& v,real scale);

            // Compute the distance between two points
    friend  real distance(const FXPoint3d& p1,const FXPoint3d& p2);
    };

//---------------------------------------------------------------------------
// The following defines a 3d vector class. The 3d vector class is derived
// directly from the 3d point class, since anything you can do with a point
// you can do with a vector.
//---------------------------------------------------------------------------

class FXVec3d : public FXPoint3d {
public:
            // Default constructor
    inline  FXVec3d()   : FXPoint3d() {};

            // Constructor given 3 real numbers
    inline  FXVec3d(real x,real y,real z) : FXPoint3d(x,y,z) {};

            // Constructor given an array of real numbers
    inline  FXVec3d(const real *v) : FXPoint3d(v) {};

            // Constructor given a 3d point
    inline  FXVec3d(const FXPoint3d& p) : FXPoint3d(p.x,p.y,p.z) {};

            // Copy constructor
    inline  FXVec3d(const FXVec3d& v) : FXPoint3d(v.x,v.y,v.z) {};

            // Standard arithmetic operators for 3d vectors
    friend  FXVec3d operator + (const FXVec3d& v1,const FXVec3d& v2);
    friend  FXVec3d operator * (const FXVec3d& v1,const FXVec3d& v2);
    friend  FXVec3d operator * (const FXVec3d& v,real s);
    friend  FXVec3d operator * (real s,const FXVec3d& v);
    friend  FXVec3d operator * (const FXVec3d& v,int s);
    friend  FXVec3d operator * (int s,const FXVec3d& v);
    friend  FXVec3d operator / (const FXVec3d& v,real s);
    friend  FXVec3d operator / (const FXVec3d& v,int s);

            // Faster methods to add and multiply
    inline  FXVec3d& operator += (const FXVec3d& v);
    inline  FXVec3d& operator -= (const FXVec3d& v);
    inline  FXVec3d& operator *= (const FXVec3d& v);
    inline  FXVec3d& operator *= (real s);
    inline  FXVec3d& operator *= (int s);
    inline  FXVec3d& operator /= (real s);
    inline  FXVec3d& operator /= (int s);

            // Methods to negate a vector
    inline  FXVec3d operator - () const;
    inline  FXVec3d& negate();          // Faster

            // Compute dot product of two vectors
    friend  real operator % (const FXVec3d& v1,const FXVec3d& v2);

            // Compute the cross product of two vectors
    friend  FXVec3d operator ^ (const FXVec3d& v1,const FXVec3d& v2);

            // Faster method to compute the cross product
    inline  FXVec3d& cross(const FXVec3d& v1,const FXVec3d& v2);

            // Methods to normalise a vector
    inline  FXVec3d operator ~ () const;
    inline  FXVec3d& normalise();           // Faster

            // Method to obtain a perpendicular vector
    inline  void perp(FXVec3d& result) const;

            // Method to determine if a vector is zero length (fuzzy)
    inline  ibool isZero() const;

            // Method to determine the length of a vector
    inline  real length() const;

            /* Set of inline friends to peform specialised arithmetic
             * generally used in computer graphics.
             */

            // Combine two vectors together with scaling
    friend  void combine(FXVec3d& result,const FXVec3d& v1,real scale1,
                const FXVec3d& v2,real scale2);
    };

/*-------------------------- Inline member functions ----------------------*/

//---------------------------------------------------------------------------
// Standard arithmetic operators for 3d vectors.
//---------------------------------------------------------------------------

inline FXPoint3d operator + (const FXPoint3d& p1,const FXVec3d& v2)
{ return FXPoint3d(p1.x + v2.x, p1.y + v2.y, p1.z + v2.z); }

inline FXVec3d operator + (const FXVec3d& v1,const FXVec3d& v2)
{ return FXVec3d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

inline FXVec3d operator - (const FXPoint3d& v1,const FXPoint3d& v2)
{ return FXVec3d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

inline FXVec3d operator * (const FXVec3d& v1,const FXVec3d& v2)
{ return FXVec3d(FXmul(v1.x,v2.x), FXmul(v1.y,v2.y), FXmul(v1.z,v2.z)); }

inline FXVec3d operator * (const FXVec3d& v1,real s)
{ return FXVec3d(FXmul(v1.x,s), FXmul(v1.y,s), FXmul(v1.z,s)); }

inline FXVec3d operator * (real s,const FXVec3d& v1)
{ return v1 * s; }

inline FXVec3d operator * (const FXVec3d& v1,int s)
{ return FXVec3d(v1.x * s, v1.y * s, v1.z * s); }

inline FXVec3d operator * (int s,const FXVec3d& v1)
{ return v1 * s; }

inline FXVec3d operator / (const FXVec3d& v1,real s)
{
    CHECK(s != 0);
#ifndef FX_FIXED
    return v1 * ((real)1.0 / s);
#else
    return FXVec3d(FXdiv(v1.x,s), FXdiv(v1.y,s), FXdiv(v1.z,s));
#endif
}

inline FXVec3d operator / (const FXVec3d& v1,int s)
{ return v1 / FXintToReal(s); }

//---------------------------------------------------------------------------
// Faster methods to add and multiply 3d vectors.
//---------------------------------------------------------------------------

inline FXPoint3d& FXPoint3d::operator += (const FXVec3d& v)
{ x += v.x; y += v.y; z += v.z; return *this; }

inline FXVec3d& FXVec3d::operator += (const FXVec3d& v)
{ x += v.x; y += v.y; z += v.z; return *this; }

inline FXVec3d& FXVec3d::operator -= (const FXVec3d& v)
{ x -= v.x; y -= v.y; z -= v.z; return *this; }

inline FXVec3d& FXVec3d::operator *= (const FXVec3d& v)
{ FXmuleq(x,v.x); FXmuleq(y,v.y); FXmuleq(z,v.z); return *this; }

inline FXVec3d& FXVec3d::operator *= (real s)
{ FXmuleq(x,s); FXmuleq(y,s); FXmuleq(z,s); return *this; }

inline FXVec3d& FXVec3d::operator *= (int s)
{ x *= s; y *= s; z *= s; return *this; }

inline FXVec3d& FXVec3d::operator /= (real s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    FXdiveq(x,s);
    FXdiveq(y,s);
    FXdiveq(z,s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
    z *= _s;
#endif
    return *this;
}

inline FXVec3d& FXVec3d::operator /= (int s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    real _s = FXintToReal(s);
    FXdiveq(x,_s);
    FXdiveq(y,_s);
    FXdiveq(z,_s);
#else
    real _s = 1.0 / (s);
    x *= _s;
    y *= _s;
    z *= _s;
#endif
    return *this;
}

//---------------------------------------------------------------------------
// Methods to negate vectors.
//---------------------------------------------------------------------------

inline FXVec3d FXVec3d::operator - () const
{ return FXVec3d(-x,-y,-z); }

inline FXVec3d& FXVec3d::negate()
{ x = -x; y = -y; z = -z; return *this; }

//---------------------------------------------------------------------------
// Miscellaneous operations.
//---------------------------------------------------------------------------

inline real operator % (const FXVec3d& v1,const FXVec3d& v2)
{ return FXVec3dDot(v1,v2); }

inline FXVec3d operator ^ (const FXVec3d& v1,const FXVec3d& v2)
{ return FXVec3d(FXmul(v1.y,v2.z) - FXmul(v1.z,v2.y),
                 FXmul(v1.z,v2.x) - FXmul(v1.x,v2.z),
                 FXmul(v1.x,v2.y) - FXmul(v1.y,v2.x)); }

inline FXVec3d& FXVec3d::cross(const FXVec3d& v1,const FXVec3d& v2)
{
    x = FXmul(v1.y,v2.z) - FXmul(v1.z,v2.y);
    y = FXmul(v1.z,v2.x) - FXmul(v1.x,v2.z);
    z = FXmul(v1.x,v2.y) - FXmul(v1.y,v2.x);
    return *this;
}

inline FXVec3d FXVec3d::operator ~ () const
{ return FXVec3d(*this / (real)FXVec3dLength(*this)); }

inline FXVec3d& FXVec3d::normalise()
{
#ifdef  FX_FIXED
    real _s = FXVec3dLength(*this);
    FXdiveq(x,_s);
    FXdiveq(y,_s);
    FXdiveq(z,_s);
#else
    real _s = 1.0 / FXVec3dLength(*this);
    x *= _s;
    y *= _s;
    z *= _s;
#endif
    return *this;
}

inline void FXVec3d::perp(FXVec3d& result) const
{ FXVec3dPerp(&result,this); }

inline ibool FXVec3d::isZero() const
{ return FXVec3dIsZero(*this); }

inline real FXVec3d::length() const
{ return FXVec3dLength(*this); }

inline ibool FXPoint3d::operator == (const FXPoint3d& v) const
{ return FXVec3dIsEqual(*this,v); }

inline void interpolate(FXPoint3d& result,const FXPoint3d& lo,const FXPoint3d& hi,
        real alpha)
/****************************************************************************
*
* Function:     interpolate
* Parameters:   result  - 3d point to place the result in
*               lo      - Low bound on interpolation
*               hi      - High bound on interpolation
*               alpha   - Interpolation value
*
* Description:  This routine linearly interpolates a vector between
*               two vectors lo and hi depending on the value of alpha:
*
*                   alpha = 0       result = lo
*                   alpha = 1       result = hi
*
****************************************************************************/
{ FXVec3dInterpolate(result,lo,hi,alpha); }

inline void combine(FXVec3d& result,const FXVec3d& v1,real scale1,
        const FXVec3d& v2,real scale2)
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
{ FXVec3dCombine(result,v1,scale1,v2,scale2); }

inline void scaleAndAdd(FXPoint3d& result,const FXPoint3d& p,const FXVec3d& v,
    real scale)
/****************************************************************************
*
* Function:     scaleAndAdd
* Parameters:   result  - Point to place the result into
*               p       - First point to add
*               v       - Second vector to add
*               scale   - Amount to scale v by.
*
* Description:  This routine is a specialised routine to perform the
*               following operation:
*
*                   result = p + v * scale;
*
*               It is a LOT faster to do it in a friend function rather
*               than doing in manually as in the above example.
*
****************************************************************************/
{ FXVec3dScaleAndAdd(result,p,v,scale); }

inline real distance(const FXPoint3d& p1,const FXPoint3d& p2)
/****************************************************************************
*
* Function:     distance
* Parameters:   p1  - First point
*               p2  - Second point
* Returns:      The distance between point p1 and point p2.
*
****************************************************************************/
{ return FXVec3dDistance(p1,p2); }

inline ostream& operator << (ostream& o,const FXPoint3d& v)
{
    return  o << '(' << FXrealToDbl(v.x) << ',' << FXrealToDbl(v.y)
              << ',' << FXrealToDbl(v.z) << ')';
}

inline ostream& operator << (ostream& o,const FXVec3d& v)
{ return o << (FXPoint3d&)v; }

#endif  /* __cplusplus */

#endif  /* __FX_VEC3D_H */
