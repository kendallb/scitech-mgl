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
* Language:     ANSI C/C+
* Environment:  any
*
* Description:  Header file for the Color vector class. The vectors can be
*               constructed from either IEEE float's, double's,
*               long double's or fixed point numbers.
*
*               Color vectors are different to normal 3D vector's, since
*               they have different semantics when used in most graphical
*               applications. This class tries to capture those semantics.
*
****************************************************************************/

#ifndef __FX_COLOR_H
#define __FX_COLOR_H

#ifndef __FX_REAL_H
#include "fx/real.h"
#endif

#ifndef __STRING_H
#include <string.h>
#endif

/*----------------------------- Inline functions --------------------------*/

#ifdef  FX_FIXED
typedef FXFixed FXColor_t;
#else
typedef float   FXColor_t;
#endif

#ifndef __cplusplus

typedef struct {
    FXColor_t   r,g,b;
    } FXColor;

#endif

#define FXColorSet(c1,r1,g1,b1)                                         \
{   (c1).r = (r1);                                                      \
    (c1).g = (g1);                                                      \
    (c1).b = (b1); }

#define FXColorCopy(r1,c1)                                              \
    (r1) = (c1)

#define FXColorAdd(r1,c1,c2)                                            \
{   (r1).r = (c1).r + (c2).r;                                           \
    (r1).g = (c1).g + (c2).g;                                           \
    (r1).b = (c1).b + (c2).b; }

#define FXColorAddEq(c1,c2)                                             \
{   (c1).r += (c2).r;                                                   \
    (c1).g += (c2).g;                                                   \
    (c1).b += (c2).b; }

#define FXColorSub(r1,c1,c2)                                            \
{   (r1).r = (c1).r - (c2).r;                                           \
    (r1).g = (c1).g - (c2).g;                                           \
    (r1).b = (c1).b - (c2).b; }

#define FXColorSubEq(c1,c2)                                             \
{   (c1).r -= (c2).r;                                                   \
    (c1).g -= (c2).g;                                                   \
    (c1).b -= (c2).b; }

#define FXColorMul(r1,c1,c2)                                            \
{   (r1).r = FXmul((c1).r,(c2).r);                                      \
    (r1).g = FXmul((c1).g,(c2).g);                                      \
    (r1).b = FXmul((c1).b,(c2).b); }

#define FXColorMulEq(c1,c2)                                             \
{   FXmuleq((c1).r,(c2).r);                                             \
    FXmuleq((c1).g,(c2).g);                                             \
    FXmuleq((c1).b,(c2).b); }

#define FXColorScale(r1,c1,s)                                           \
{   (r1).r = FXmul((c1).r,s);                                           \
    (r1).g = FXmul((c1).g,s);                                           \
    (r1).b = FXmul((c1).b,s); }

#define FXColorScaleEq(c1,s)                                            \
{   FXmuleq((c1).r,s);                                                  \
    FXmuleq((c1).g,s);                                                  \
    FXmuleq((c1).b,s); }

#define FXColorScaleInt(r1,c1,s)                                        \
{   (r1).r = (c1).r * (int)(s);                                         \
    (r1).g = (c1).g * (int)(s);                                         \
    (r1).b = (c1).b * (int)(s); }

#define FXColorScaleIntEq(c1,s)                                         \
{   (c1).r *= (int)(s);                                                 \
    (c1).g *= (int)(s);                                                 \
    (c1).b *= (int)(s); }

#ifdef  FX_FIXED

#define FXColorDiv(r1,c1,s)                                             \
{   (r1).r = FXdiv((c1).r,s);                                           \
    (r1).g = FXdiv((c1).g,s);                                           \
    (r1).b = FXdiv((c1).b,s); }

#define FXColorDivEq(c1,s)                                              \
{   FXdiveq((c1).r,s);                                                  \
    FXdiveq((c1).g,s);                                                  \
    FXdiveq((c1).b,s); }

#define FXColorDivInt(r1,c1,s)                                          \
{   real _s = FXintToReal(s);                                           \
    FXColorDiv(r1,c1,_s); }

#define FXColorDivIntEq(c1,s)                                           \
{   real _s = FXintToReal(s);                                           \
    FXColorDivEq(c1,_s); }

#else   /* !FX_FIXED */

#define FXColorDiv(r1,c1,s)                                             \
{   real _s = 1.0 / (s);                                                \
    FXColorScale(r1,c1,_s); }

#define FXColorDivEq(c1,s)                                              \
{   real _s = 1.0 / (s);                                                \
    FXColorScaleEq(c1,_s); }

#define FXColorDivInt(r1,c1,s)                                          \
{   real _s = 1.0 / (s);                                                \
    FXColorScale(r1,c1,_s); }

#define FXColorDivIntEq(c1,s)                                           \
{   real _s = 1.0 / (s);                                                \
    FXColorScaleEq(c1,_s); }

#endif  /* !FX_FIXED */

#define FXColorNegate(r1,c1)                                            \
{   (r1).r = -(c1).r;                                                   \
    (r1).g = -(c1).g;                                                   \
    (r1).b = -(c1).b; }

#define FXColorNegateEq(c1) FXColorNegate(c1,c1)

#define FXColorIsZero(c1)                                               \
    (_FZERO((c1).r,_FUDGE) && _FZERO((c1).g,_FUDGE)                     \
        && _FZERO((c1).b,_FUDGE))

#define FXColorIsEqual(c1,c2)                                           \
    (_FEQ((c1).r,(c2).r,_FUDGE) && _FEQ((c1).g,(c2).g,_FUDGE)           \
        && _FEQ((c1).b,(c2).b,_FUDGE))

#define FXColorIsLT(c1,w)                                               \
    ((c1).r < (w) && (c1).g < (w) && (c1).b < (w))

#define FXColorIsLE(c1,w)                                               \
    ((c1).r <= (w) && (c1).g <= (w) && (c1).b <= (w))

#define FXColorIsGT(c1,w)                                               \
    !FXColorIsLE(c1,w)

#define FXColorIsGE(c1,w)                                               \
    !FXColorIsLT(c1,w)

#define FXColorIsClose(c1,c2,d)                                         \
    (_FEQ((c1).r,(c2).r,(d).r) && _FEQ((c1).g,(c2).g,(d).g)             \
        && _FEQ((c1).b,(c2).b,(d).b))

#define FXColorInterpolate(r1,lo,hi,alpha)                              \
{   (r1).r = _LERP((lo).r, (hi).r, (alpha));                            \
    (r1).g = _LERP((lo).g, (hi).g, (alpha));                            \
    (r1).b = _LERP((lo).b, (hi).b, (alpha)); }

#define FXColorCombine(r1,c1,s1,c2,s2)                                  \
{   (r1).r = FXmul((c1).r,(s1)) + FXmul((c2).r,(s2));                   \
    (r1).g = FXmul((c1).g,(s1)) + FXmul((c2).g,(s2));                   \
    (r1).b = FXmul((c1).b,(s1)) + FXmul((c2).b,(s2)); }

#define FXColorScaleAndAdd(r1,p,c,s)                                    \
{   (r1).r = (p).r + FXmul((c).r,(s));                                  \
    (r1).g = (p).g + FXmul((c).g,(s));                                  \
    (r1).b = (p).b + FXmul((c).b,(s)); }

/*--------------------------- Class Definition ----------------------------*/

#ifdef  __cplusplus

#ifndef __IOSTREAM_H
#include <iostream.h>
#endif

#ifndef __TCL_TECHLIB_HPP
#include "tcl\techlib.hpp"
#endif

//---------------------------------------------------------------------------
// The following defines a Color vector class. Note that unless we are
// using fixed point, colors are always reprented with float's (they
// never need double precision).
//---------------------------------------------------------------------------

class FXColor {
public:
    FXColor_t   r,g,b;                  // Components for the color

            // Default constructor (does nothing)
            FXColor()   {};

            // Constructor given 3 real numbers
            FXColor(FXColor_t r1,FXColor_t g1,FXColor_t b1)
            { r = r1; g = g1; b = b1; };

            // Constructor given an array of real numbers
            FXColor(const FXColor_t *v) { r = v[0]; g = v[1]; b = v[2]; };

            // Copy constructor
            FXColor(const FXColor& c) { r = c.r; g = c.g; b = c.b; };

            // Overload cast to an array of real's. This is useful for
            // passing vectors to C routines or to libraries like OpenGL.
            operator FXColor_t * () { return (FXColor_t*)&r; };

            // Standard arithmetic operators for FXColor vectors
    friend  FXColor operator + (const FXColor& v1,const FXColor& v2);
    friend  FXColor operator - (const FXColor& v1,const FXColor& v2);
    friend  FXColor operator * (const FXColor& v1,const FXColor& v2);
    friend  FXColor operator * (const FXColor& v,FXColor_t s);
    friend  FXColor operator * (FXColor_t s,const FXColor& v);
    friend  FXColor operator * (const FXColor& v,int s);
    friend  FXColor operator * (int s,const FXColor& v);
    friend  FXColor operator / (const FXColor& v,FXColor_t s);
    friend  FXColor operator / (const FXColor& v,int s);

            // Faster methods to add and multiply
            FXColor& operator += (const FXColor& v);
            FXColor& operator -= (const FXColor& v);
            FXColor& operator *= (const FXColor& v);
            FXColor& operator *= (FXColor_t s);
            FXColor& operator *= (int s);
            FXColor& operator /= (FXColor_t s);
            FXColor& operator /= (int s);

            // Methods to negate a FXColor vector
            FXColor operator - () const;
            FXColor& negate();          // Faster

            // Method to determine if a vector is zero length (fuzzy)
            ibool isZero() const;

            // Method to determine if two vectors are equal (fuzzy)
            ibool operator == (const FXColor& v) const;

            // Comparison methods for FXColor vectors. We compare the
            // components of the vector against a weight.
            ibool operator <  (FXColor_t weight) const;
            ibool operator <= (FXColor_t weight) const;
            ibool operator >  (FXColor_t weight) const;
            ibool operator >= (FXColor_t weight) const;

            // Method to determine if two colors are close
            ibool isClose(const FXColor& v,const FXColor& delta);

            // Friend to display the contents of a FXColor vector
    friend  ostream& operator << (ostream& s,FXColor& v);

            /* Set of inline friends to peform specialised arithmetic
             * generally used in computer graphics.
             */

            // Linearly interpolate between two FXColor vectors.
    friend  void interpolate(FXColor& result,const FXColor& lo,const FXColor& hi,
                FXColor_t alpha);

            // Combine two vectors together with scaling
    friend  void combine(FXColor& result,const FXColor& v1,FXColor_t scale1,
                const FXColor& v2,FXColor_t scale2);

            // Add and scale two vectors.
    friend  void scaleAndAdd(FXColor& result,const FXColor& v1,
                const FXColor& v2,FXColor_t scale);
    };

/*-------------------------- Inline member functions ----------------------*/

//---------------------------------------------------------------------------
// Standard arithmetic operators for FXColor vectors.
//---------------------------------------------------------------------------

inline FXColor operator + (const FXColor& v1,const FXColor& v2)
{ return FXColor(v1.r + v2.r, v1.g + v2.g, v1.b + v2.b); }

inline FXColor operator - (const FXColor& v1,const FXColor& v2)
{ return FXColor(v1.r - v2.r, v1.g - v2.g, v1.b - v2.b); }

inline FXColor operator * (const FXColor& v1,const FXColor& v2)
{ return FXColor(FXmul(v1.r,v2.r), FXmul(v1.g,v2.g), FXmul(v1.b,v2.b)); }

inline FXColor operator * (const FXColor& v1,FXColor_t s)
{ return FXColor(FXmul(v1.r,s), FXmul(v1.g,s), FXmul(v1.b,s)); }

inline FXColor operator * (FXColor_t s,const FXColor& v1)
{ return v1 * s; }

inline FXColor operator * (const FXColor& v1,int s)
{ return FXColor(v1.r * s, v1.g * s, v1.b * s); }

inline FXColor operator * (int s,const FXColor& v1)
{ return v1 * s; }

inline FXColor operator / (const FXColor& v1,FXColor_t s)
{
#ifndef FX_FIXED
    return v1 * ((FXColor_t)1.0 / s);
#else
    return FXColor(FXdiv(v1.r,s), FXdiv(v1.g,s), FXdiv(v1.b,s));
#endif
}

//---------------------------------------------------------------------------
// Faster methods to add and multiply FXColor vectors.
//---------------------------------------------------------------------------

inline FXColor& FXColor::operator += (const FXColor& v)
{ r += v.r; g += v.g; b += v.b; return *this; }

inline FXColor& FXColor::operator -= (const FXColor& v)
{ r -= v.r; g -= v.g; b -= v.b; return *this; }

inline FXColor& FXColor::operator *= (const FXColor& v)
{ FXmuleq(r,v.r); FXmuleq(g,v.g); FXmuleq(b,v.b); return *this; }

inline FXColor& FXColor::operator *= (FXColor_t s)
{ FXmuleq(r,s); FXmuleq(g,s); FXmuleq(b,s); return *this; }

inline FXColor& FXColor::operator *= (int s)
{ r *= s; g *= s; b *= s; return *this; }

inline FXColor& FXColor::operator /= (FXColor_t s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    FXdiveq(r,s);
    FXdiveq(g,s);
    FXdiveq(b,s);
#else
    real _s = 1.0 / (s);
    r *= _s;
    g *= _s;
    b *= _s;
#endif
    return *this;
}

inline FXColor& FXColor::operator /= (int s)
{
    CHECK(s != 0);
#ifdef  FX_FIXED
    real _s = FXintToReal(s);
    FXdiveq(r,_s);
    FXdiveq(g,_s);
    FXdiveq(b,_s);
#else
    real _s = 1.0 / (s);
    r *= _s;
    g *= _s;
    b *= _s;
#endif
    return *this;
}

//---------------------------------------------------------------------------
// Methods to negate vectors.
//---------------------------------------------------------------------------

inline FXColor FXColor::operator - () const
{ return FXColor(-r,-g,-b); }

inline FXColor& FXColor::negate()
{ r = -r; g = -g; b = -b; return *this; }

//---------------------------------------------------------------------------
// Miscellaneous operations.
//---------------------------------------------------------------------------

inline ibool FXColor::isZero() const
{ return FXColorIsZero(*this); }

inline ibool FXColor::operator == (const FXColor& v) const
{ return FXColorIsEqual(*this,v); }

inline ibool FXColor::operator <  (FXColor_t weight) const
{ return FXColorIsLT(*this,weight); };

inline ibool FXColor::operator <= (FXColor_t weight) const
{ return FXColorIsLE(*this,weight); };

inline ibool FXColor::operator >  (FXColor_t weight) const
{ return FXColorIsGT(*this,weight); };

inline ibool FXColor::operator >= (FXColor_t weight) const
{ return FXColorIsGE(*this,weight); };

inline ibool FXColor::isClose(const FXColor& v,const FXColor& d)
{ return FXColorIsClose(*this,v,d); };

inline void interpolate(FXColor& result,const FXColor& lo,const FXColor& hi,
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
{ FXColorInterpolate(result,lo,hi,alpha); }

inline void combine(FXColor& result,const FXColor& v1,real scale1,
        const FXColor& v2,real scale2)
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
{ FXColorCombine(result,v1,scale1,v2,scale2); }

inline void scaleAndAdd(FXColor& result,const FXColor& p,const FXColor& v,
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
{ FXColorScaleAndAdd(result,p,v,scale); }

inline ostream& operator << (ostream& o,FXColor& v)
{
    return  o << '(' << FXrealToDbl(v.r) << ',' << FXrealToDbl(v.g)
              << ',' << FXrealToDbl(v.b) << ')';
}

#endif  /* __cplusplus */

#endif  /* __FX_COLOR_H */
