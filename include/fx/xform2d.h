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
* Description:  Header file for specialised 2D transformation functions for
*               computer graphics applications. We also include a C++
*               wrapper class for these functions.
*
****************************************************************************/

#ifndef __FX_XFORM2D_H
#define __FX_XFORM2D_H

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef __FX_VEC2D_H
#include "fx/vec2d.h"
#endif

/*--------------------------- Type Definitions ----------------------------*/

typedef real    FXMat2x3[2][3];

#ifndef __cplusplus

typedef struct {
    FXMat2x3    mat;    /* Data for the 3x3 matrix (only 2x3 really!)   */
    short       special;/* Flags if the matrix is special or not        */
    } FXForm2d;

#endif

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            // Use "C" linkage when in C++ mode
class FXForm2d;         // Forward reference to class
#endif

#define FXForm2dClear(m)                                                \
    memset((m)->mat,0,sizeof((m)->mat))

#define FXForm2dCopy(r,m)                                               \
    *(r) = *(m);

void FXForm2dSet(FXForm2d *xf,const real *data);
void FXForm2dIdentity(FXForm2d *xf);
void FXForm2dInverse(FXForm2d *r,const FXForm2d *xf);
void FXForm2dScale(FXForm2d *xf,real xscale,real yscale);
void FXForm2dScaleAbout(FXForm2d *xf,real xscale,real yscale,real x,real y);
void FXForm2dTranslate(FXForm2d *xf,real x,real y);
void FXForm2dRotate(FXForm2d *xf,real angle);
void FXForm2dRotateAbout(FXForm2d *xf,real angle,real x,real y);
void FXForm2dWorldToView(FXForm2d *xf,real Umax,real Vmax,real Xmin,real Xmax,
    real Ymin,real Ymax);
void FXForm2dViewToWorld(FXForm2d *xf,real Umax,real Vmax,real Xmin,real Xmax,
    real Ymin,real Ymax);

#ifdef  FIXED386

/* High performance 386 assembler routines for fixed point code */

void _ASMAPI F386_map2D(const FXFixed *m,FXFixed *result,const FXFixed *p,
    ibool special);
void _ASMAPI F386_mapVec2D(const FXFixed *m,FXFixed *result,const FXFixed *p,
    ibool special);
void _ASMAPI F386_concat3x3(FXFixed *result,const FXFixed *m1,const FXFixed *m2);

#define FXForm2dMap(xf,result,p)                                        \
    F386_map2D(&(xf)->mat[0][0],&(result)->x,&(p)->x,(xf)->special)

#define FXForm2dMapVec(xf,result,p)                                     \
    F386_mapVec2D(&(xf)->mat[0][0],&(result)->x,&(p)->x,(xf)->special)

#define FXForm2dConcat(r,xf1,xf2)                                       \
{   F386_concat3x3(&(r)->mat[0][0],&(xf1)->mat[0][0],&(xf2)->mat[0][0]);\
    (r)->special = ((xf1)->special && (xf2)->special); }

#else

void FL_map2D(const FXMat2x3 mat,FXPoint2d *result,const FXPoint2d *p,
    ibool special);
void FL_mapVec2D(const FXMat2x3 mat,FXVec2d *result,const FXVec2d *v,
    ibool special);
void FL_concat3x3(FXMat2x3 mat,const FXMat2x3 mat1,const FXMat2x3 mat2);

#define FXForm2dMap(xf,result,p)                                        \
    FL_map2D(*((const FXMat2x3*)&(xf)->mat),result,p,(xf)->special)

#define FXForm2dMapVec(xf,result,p)                                     \
    FL_mapVec2D(*((const FXMat2x3*)&(xf)->mat),result,p,(xf)->special)

#define FXForm2dConcat(r,xf1,xf2)                                       \
{   FL_concat3x3((r)->mat,*((const FXMat2x3*)&(xf1)->mat),              \
        *((const FXMat2x3*)&(xf2)->mat));                               \
    (r)->special = ((xf1)->special && (xf2)->special); }

#endif  /* FIXED386 */

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The following defines a 2D transformation class for performing
// transformations on 2d points. We do not provide a default constructor,
// so the transform MUST be set up somehow, as it will contain garbage by
// default.
//
// Note that we do not store the bottom row of the transformation matrix,
// since we take advantage of the fact that the form of all 2d
// transformation matrices is (foly sec 5.5):
//
//                      [ r11, r12, tx ]
//                      [ r21, r22, ty ]
//                      [ 0,   0,   1  ]
//
// so the bottom row is implicit. It is displayed however if you print the
// transformation for symmetry.
//
// Note that we also maintain a flag specifying if a transformation is
// 'special', or consists of only translational and scale components, which
// we can apply faster.
//---------------------------------------------------------------------------

class FXForm2d {
protected:
    FXMat2x3    mat;        // Data for the 3x3 matrix (only 2x3 really!)
    short       special;    // Flags if the matrix is special or not

public:
            // Default constructor
    inline  FXForm2d() {};

            // Constructor given an array of real numbers (6 of them)
    inline  FXForm2d(const real *data);

            // Assignment Operator given an array of real's
    inline  FXForm2d& operator = (const real *data);

            // Method to clear the transformation matrix to zero's.
    inline  FXForm2d& clear(void);

            // Create an identity transformation
    inline  FXForm2d& identity(void);

            // Method to compute a transformation's inverse
    inline  void inverse(FXForm2d& r);

            // Concatenate a scale transformation about the origin
    inline  FXForm2d& scale(real xscale,real yscale);

            // Concatenate a scale transformation about a specified point
    inline  FXForm2d& scaleAbout(real xscale,real yscale,real x,real y);
    inline  FXForm2d& scaleAbout(real xscale,real yscale,const FXPoint2d& p)
            { return scaleAbout(xscale,yscale,p.x,p.y); };

            // Concatenate a translation transformation
    inline  FXForm2d& translate(real xtrans,real ytrans);
    inline  FXForm2d& translate(const FXPoint2d& p)
            { return translate(p.x,p.y); };

            // Concatenate a rotation transformation about the origin
    inline  FXForm2d& rotate(real angle);

            // Concatenate a rotation transformation about a specified point
    inline  FXForm2d& rotateAbout(real angle,real x,real y);
    inline  FXForm2d& rotateAbout(real angle,const FXPoint2d& p)
            { return rotateAbout(angle,p.x,p.y); };

            // Build a world to viewport transformation
    inline  FXForm2d& worldToView(real Umax,real Vmax,real Xmin,real Xmax,
                real Ymin,real Ymax);

            // Build a viewport to world transformation
    inline  FXForm2d& viewToWorld(real Umax,real Vmax,real Xmin,real Xmax,
                real Ymin,real Ymax);

            // Return a reference to the (i,j)th element of the matrix
    inline  real& operator () (int i,int j) { return mat[i][j]; };

            // Map a 2d point by the transformation
    inline  void map(FXPoint2d& result,const FXPoint2d& p) const;
    inline  void map(FXPoint2d& p) const;

            // Map a 2d vector by the transformation
    inline  void map(FXVec2d& result,const FXVec2d& v) const;
    inline  void map(FXVec2d& v) const;

            // Concatenate two transformations together
    inline  FXForm2d& concat(const FXForm2d& m1,const FXForm2d& m2);

            // Concatenate a transformation with the current transformation
    inline  FXForm2d& concat(const FXForm2d& m);

            // Determine if the transformation is special or not
    inline  ibool isSpecial() const { return special; };

            // Friend operator to display the transformation matrix
    friend  ostream& operator << (ostream& o,const FXForm2d& m);
    };

/*------------------------- Inline member functions -----------------------*/

inline FXForm2d::FXForm2d(const real *data)
{ FXForm2dSet(this,data); };

inline FXForm2d& FXForm2d::operator = (const real *data)
{ FXForm2dSet(this,data); return *this; };

inline FXForm2d& FXForm2d::clear(void)
{ FXForm2dClear(this); return *this; }

inline FXForm2d& FXForm2d::identity(void)
{ FXForm2dIdentity(this); return *this; };

inline void FXForm2d::inverse(FXForm2d& r)
{ FXForm2dInverse(&r,this); };

inline FXForm2d& FXForm2d::scale(real xscale,real yscale)
{ FXForm2dScale(this,xscale,yscale); return *this; };

inline FXForm2d& FXForm2d::scaleAbout(real xscale,real yscale,real x,real y)
{ FXForm2dScaleAbout(this,xscale,yscale,x,y); return *this; };

inline FXForm2d& FXForm2d::translate(real xtrans,real ytrans)
{ FXForm2dTranslate(this,xtrans,ytrans); return *this; };

inline FXForm2d& FXForm2d::rotate(real angle)
{ FXForm2dRotate(this,angle); return *this; };

inline FXForm2d& FXForm2d::rotateAbout(real angle,real x,real y)
{ FXForm2dRotateAbout(this,angle,x,y); return *this; };

inline FXForm2d& FXForm2d::worldToView(real Umax,real Vmax,real Xmin,
    real Xmax,real Ymin,real Ymax)
{ FXForm2dWorldToView(this,Umax,Vmax,Xmin,Xmax,Ymin,Ymax); return *this; };

inline FXForm2d& FXForm2d::viewToWorld(real Umax,real Vmax,real Xmin,
    real Xmax,real Ymin,real Ymax)
{ FXForm2dViewToWorld(this,Umax,Vmax,Xmin,Xmax,Ymin,Ymax); return *this; };

inline void FXForm2d::map(FXPoint2d& result,const FXPoint2d& p) const
{ CHECK(&result != &p); FXForm2dMap(this,&result,&p); }

inline void FXForm2d::map(FXVec2d& result,const FXVec2d& v) const
{ CHECK(&result != &v); FXForm2dMapVec(this,&result,&v); }

inline FXForm2d& FXForm2d::concat(const FXForm2d& m1,const FXForm2d& m2)
{
    CHECK(this != &m1 && this != &m2);
    FXForm2dConcat(this,&m1,&m2);
    return *this;
}

// Set of inline member functions to map points and vectors in place
// (ie: the point being mapped is replaced by the result).

inline void FXForm2d::map(FXPoint2d& p) const
{
    FXPoint2d   result;
    map(result,p);
    p = result;
}

inline void FXForm2d::map(FXVec2d& v) const
{
    FXVec2d result;
    map(result,v);
    v = result;
}

inline FXForm2d& FXForm2d::concat(const FXForm2d& m)
{
    FXForm2d    result;
    result.concat(*this,m);
    *this = result;
    return *this;
}

#endif  /* __cplusplus */

#endif  /* __FX_XFORM2D_H */
