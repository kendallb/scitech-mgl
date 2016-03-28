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
* Description:  Header file for the specialised 3D transformation class
*               for computer graphics applications.
*
****************************************************************************/

#ifndef __FX_XFORM3D_H
#define __FX_XFORM3D_H

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef __FX_VEC4D_H
#include "fx/vec4d.h"
#endif

/*--------------------------- Type Definitions ----------------------------*/

typedef real    FXMat4x4[4][4];

#ifndef __cplusplus

typedef struct {
    FXMat4x4    mat;        /* Data for the 4x4 matrix                  */
    short       special;    /* Flags if the matrix is special or not    */
    } FXForm3d;

#endif

typedef enum {FXForm3dPerspective, FXForm3dParallel} FX3DProj_type;

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            // Use "C" linkage when in C++ mode
class FXForm3d;         // Forward reference to class
#endif

#define FXForm3dClear(m)                                                \
    memset((m)->mat,0,sizeof((m)->mat))

#define FXForm3dCopy(r,m)                                               \
    memcpy(r,m,sizeof(FXForm3d))

void FXForm3dSet(FXForm3d *xf,const real *data);
void FXForm3dSetTranspose(FXForm3d *xf,const real *data);
void FXForm3dIdentity(FXForm3d *xf);
void FXForm3dInverse(FXForm3d *r,const FXForm3d *xf);
void FXForm3dScale(FXForm3d *xf,real xscale,real yscale,real zscale);
void FXForm3dTranslate(FXForm3d *xf,real x,real y,real z);
void FXForm3dRotatex(FXForm3d *xf,real angle);
void FXForm3dRotatey(FXForm3d *xf,real angle);
void FXForm3dRotatez(FXForm3d *xf,real angle);
void FXForm3dRotate(FXForm3d *xf,real angle,real x,real y,real z);
void FXForm3dViewOrientation(FXForm3d *xf,const FXPoint3d *VRP,
    const FXVec3d *VPN,const FXVec3d *VUP);
void FXForm3dViewMapping(FXForm3d *xf,real umin,real vmin,real umax,
    real vmax,int proj_type,const FXPoint3d *PRP,real F,real B,
    real NDC_maxx,real NDC_maxy,int aspect,ibool keep_square);

#ifdef  FIXED386

/* High performance 32 bit assembler routines for fixed point */

void _ASMAPI F386_map2Dto2D(const FXFixed *m,FXFixed *result,const FXFixed *p);
void _ASMAPI F386_map3Dto3D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_map3Dto4D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_map4Dto4D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_mapVec3Dto3D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_mapVec3Dto4D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_mapVec4Dto4D(const FXFixed *m,FXFixed *result,const FXFixed *p,ibool special);
void _ASMAPI F386_concat4x4(FXFixed *result,const FXFixed *m1,const FXFixed *m2,ibool special);

#define FXForm3dMap2dTo2d(xf,r,p)                                       \
    F386_map2Dto2D(&(xf)->mat[0][0],(real*)(r),(real*)(p))

#define FXForm3dMap3dTo3d(xf,r,p)                                       \
    F386_map3Dto3D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dMap3dTo4d(xf,r,p)                                       \
    F386_map3Dto4D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dMap4dTo4d(xf,r,p)                                       \
    F386_map4Dto4D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dMapVec3dTo3d(xf,r,p)                                    \
    F386_mapVec3Dto3D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dMapVec3dTo4d(xf,r,p)                                    \
    F386_mapVec3Dto4D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dMapVec4dTo4d(xf,r,p)                                    \
    F386_mapVec4Dto4D(&(xf)->mat[0][0],(real*)(r),(real*)(p),(xf)->special)

#define FXForm3dConcat(r,xf1,xf2)                                       \
    F386_concat4x4(&(r)->mat[0][0],&(xf1)->mat[0][0],&(xf2)->mat[0][0], \
    ((r)->special = (xf1)->special && (xf2)->special))

#else

void FL_map2Dto2D(const FXMat4x4 mat,real *result,const real *p);
void FL_map3Dto3D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_map3Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_map4Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_mapVec3Dto3D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_mapVec3Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_mapVec4Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special);
void FL_concat4x4(FXMat4x4 mat,const FXMat4x4 mat1,const FXMat4x4 mat2,ibool special);

#define FXForm3dMap2dTo2d(xf,r,p)                                       \
    FL_map2Dto2D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p))

#define FXForm3dMap3dTo3d(xf,r,p)                                       \
    FL_map3Dto3D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dMap3dTo4d(xf,r,p)                                       \
    FL_map3Dto4D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dMap4dTo4d(xf,r,p)                                       \
    FL_map4Dto4D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dMapVec3dTo3d(xf,r,p)                                    \
    FL_mapVec3Dto3D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dMapVec3dTo4d(xf,r,p)                                    \
    FL_mapVec3Dto4D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dMapVec4dTo4d(xf,r,p)                                    \
    FL_mapVec4Dto4D(*((const FXMat4x4*)&(xf)->mat),(real*)(r),(const real*)(p),(xf)->special)

#define FXForm3dConcat(r,xf1,xf2)                                       \
    FL_concat4x4((r)->mat,*((const FXMat4x4*)&(xf1)->mat),              \
        *((const FXMat4x4*)&(xf2)->mat),                                \
    ((r)->special = (xf1)->special && (xf2)->special))

#endif  // FIXED386

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The following defines a 3D transformation class for performing
// transformations on 3d points. We do not provide a default constructor,
// so the transformation MUST be set up somehow, as it will contain garbage
// by default. We provide methods to create most 3d transformation, and
// methods to set up viewing transformation matrices.
//
// We take advantage whenever possible that the transformation matrices that
// do not contains any perspective transforms will have the bottom row set to
// <0,0,0,1>. These are marked as special case matrices and can be multiplied
// faster and can be used to map points faster.
//
//            special               perspective
//
//          [ a b c d ]             [ a b c d ]
//          [ e f g h ]     vs      [ e f g h ]
//          [ i j k l ]             [ i j k l ]
//          [ 0 0 0 1 ]             [ m n o p ] <- bottom row is not special
//
//---------------------------------------------------------------------------

class FXForm3d {
protected:
    FXMat4x4    mat;        // Data for the 4x4 matrix
    short       special;    // Special matrices (no perspective)

public:

            // Default constructor
    inline  FXForm3d() {};

            // Copy constructor
    inline  FXForm3d(const FXForm3d& m) { FXForm3dCopy(this,&m); };

            // Constructor given an array of real numbers
    inline  FXForm3d(const real *data);

            // Assignment Operator
    inline  FXForm3d& operator = (const FXForm3d& m);

            // Assignment Operator given an array of real's
    inline  FXForm3d& operator = (const real *data);

            // Assignment of a transpose matrox
    inline  FXForm3d& setTranspose(const real *data);

            // Overload cast to an array of real's.
    inline  operator const real * () { return &mat[0][0]; };

            // Method to clear the transformation to zero's.
    inline  FXForm3d& clear(void);

            // Create an identity transformation
    inline  FXForm3d& identity(void);

            // Method to compute a transformation's inverse
    inline  void inverse(FXForm3d& m);

            // Concatenate a scale transformation about the origin
    inline  FXForm3d& scale(real xscale,real yscale,real zscale);

            // Concatenate a translation transformation
    inline  FXForm3d& translate(real xtrans,real ytrans,real ztrans);
    inline  FXForm3d& translate(const FXPoint3d& p)
            { return translate (p.x,p.y,p.z); };

            // Concatenate a rotation transformation about the x axis
    inline  FXForm3d& rotatex(real angle);

            // Concatenate a rotation transformation about the y axis
    inline  FXForm3d& rotatey(real angle);

            // Concatenate a rotation transformation about the z axis
    inline  FXForm3d& rotatez(real angle);

            // Concatenate a rotation about an arbitrary axis through the origin
    inline  FXForm3d& rotate(real angle,real x,real y,real z);
    inline  FXForm3d& rotate(real angle,const FXVec3d& axis)
            { return rotate(angle,axis.x,axis.y,axis.z); };

            // Build a view orientation transformation
    inline  FXForm3d& viewOrientation(const FXPoint3d& VRP,const FXVec3d& VPN,
                const FXVec3d& VUP);

            // Build a view mapping transformation
    inline  FXForm3d& viewMapping(real umin,real vmin,real umax,real vmax,
                int proj_type,const FXPoint3d& PRP,real F,real B,
                real NDC_maxx,real NDC_maxy,int aspect,ibool keep_square = true);

            // Return a reference to the (i,j)th element of the matrix
    inline  real& operator () (int i,int j) { return mat[i][j]; };

            // Map a 2d point by multiplying by the transformation matrix
    inline  void map(FXPoint2d& result,const FXPoint2d& p) const;

            // Map a 3d point by multiplying by the transformation matrix
    inline  void map(FXPoint3d& result,const FXPoint3d& p) const;
    inline  void map(FXPoint3d& p) const;
    inline  void map(FXPoint4d& result,const FXPoint3d& p) const;

            // Map a 4d point by multiplying by the transformation matrix
    inline  void map(FXPoint4d& result,const FXPoint4d& p) const;
    inline  void map(FXPoint4d& p) const;

            // Special versions to map an array of reals of specified size
    inline  void map2(FXPoint2d& result,const real *p) const;
    inline  void map3(FXPoint3d& result,const real *p) const;
    inline  void map3(FXPoint4d& result,const real *p) const;
    inline  void map4(FXPoint4d& result,const real *p) const;

            // Map a 3d vector by multiplying by the transformation matrix
    inline  void map(FXVec3d& result,const FXVec3d& v) const;
    inline  void map(FXVec3d& v) const;
    inline  void map(FXVec4d& result,const FXVec3d& v) const;

            // Map a 4d vector by multiplying by the transformation matrix
    inline  void map(FXVec4d& result,const FXVec4d& v) const;
    inline  void map(FXVec4d& v) const;

            // Special versions to map an array of reals of specified size
    inline  void map3(FXVec3d& result,const real *p) const;
    inline  void map3(FXVec4d& result,const real *p) const;
    inline  void map4(FXVec4d& result,const real *p) const;

            // Concatenate two transformations together
    inline  FXForm3d& concat(const FXForm3d& m1,const FXForm3d& m2);

            // Concatenate a transformation with the current transformation
    inline  FXForm3d& concat(const FXForm3d& m);

            // Determine if the transformation is special or not
    inline  ibool isSpecial() const { return special; };

            // Friend operator to display the matrix
    friend  ostream& operator << (ostream& o,const FXForm3d& m);
    };

/*------------------------- Inline member functions -----------------------*/

inline FXForm3d::FXForm3d(const real *data)
{ FXForm3dSet(this,data); };

inline FXForm3d& FXForm3d::operator = (const FXForm3d& m)
{ FXForm3dCopy(this,&m); return *this; };

inline FXForm3d& FXForm3d::operator = (const real *data)
{ FXForm3dSet(this,data); return *this; };

inline FXForm3d& FXForm3d::setTranspose(const real *data)
{ FXForm3dSetTranspose(this,data); return *this; };

inline FXForm3d& FXForm3d::clear(void)
{ FXForm3dClear(this); return *this; }

inline FXForm3d& FXForm3d::identity(void)
{ FXForm3dIdentity(this); return *this; };

inline void FXForm3d::inverse(FXForm3d& r)
{ FXForm3dInverse(&r,this); };

inline FXForm3d& FXForm3d::scale(real xscale,real yscale,real zscale)
{ FXForm3dScale(this,xscale,yscale,zscale); return *this; };

inline FXForm3d& FXForm3d::translate(real xtrans,real ytrans,real ztrans)
{ FXForm3dTranslate(this,xtrans,ytrans,ztrans); return *this; };

inline FXForm3d& FXForm3d::rotatex(real angle)
{ FXForm3dRotatex(this,angle); return *this; };

inline FXForm3d& FXForm3d::rotatey(real angle)
{ FXForm3dRotatey(this,angle); return *this; };

inline FXForm3d& FXForm3d::rotatez(real angle)
{ FXForm3dRotatez(this,angle); return *this; };

inline FXForm3d& FXForm3d::rotate(real angle,real x,real y,real z)
{ FXForm3dRotate(this,angle,x,y,z); return *this; };

inline FXForm3d& FXForm3d::viewOrientation(const FXPoint3d& VRP,const FXVec3d& VPN,
    const FXVec3d& VUP)
{ FXForm3dViewOrientation(this,&VRP,&VPN,&VUP); return *this; }

inline FXForm3d& FXForm3d::viewMapping(real umin,real vmin,real umax,real vmax,
                int proj_type,const FXPoint3d& PRP,real F,real B,
                real NDC_maxx,real NDC_maxy,int aspect,ibool keep_square)
{
    FXForm3dViewMapping(this,umin,vmin,umax,vmax,proj_type,&PRP,F,B,
        NDC_maxx,NDC_maxy,aspect,keep_square);
    return *this;
}

inline void FXForm3d::map(FXPoint2d& result,const FXPoint2d& p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap2dTo2d(this,&result,&p); }

inline void FXForm3d::map(FXPoint3d& result,const FXPoint3d& p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap3dTo3d(this,&result,&p); }

inline void FXForm3d::map(FXPoint4d& result,const FXPoint3d& p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap3dTo4d(this,&result,&p); }

inline void FXForm3d::map(FXPoint4d& result,const FXPoint4d& p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap4dTo4d(this,&result,&p); }

inline void FXForm3d::map(FXVec3d& result,const FXVec3d& v) const
{ CHECK((void*)&result != (void*)&v); FXForm3dMapVec3dTo3d(this,&result,&v); }

inline void FXForm3d::map(FXVec4d& result,const FXVec3d& v) const
{ FXForm3dMapVec3dTo4d(this,&result,&v); }

inline void FXForm3d::map(FXVec4d& result,const FXVec4d& v) const
{ CHECK((void*)&result != (void*)&v); FXForm3dMapVec4dTo4d(this,&result,&v); }

inline void FXForm3d::map2(FXPoint2d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap2dTo2d(this,&result,p); }

inline void FXForm3d::map3(FXPoint3d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap3dTo3d(this,&result,p); }

inline void FXForm3d::map3(FXPoint4d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap3dTo4d(this,&result,p); }

inline void FXForm3d::map4(FXPoint4d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMap4dTo4d(this,&result,p); }

inline void FXForm3d::map3(FXVec3d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMapVec3dTo3d(this,&result,p); }

inline void FXForm3d::map3(FXVec4d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMapVec3dTo4d(this,&result,p); }

inline void FXForm3d::map4(FXVec4d& result,const real *p) const
{ CHECK((void*)&result != (void*)&p); FXForm3dMapVec4dTo4d(this,&result,p); }

inline FXForm3d& FXForm3d::concat(const FXForm3d& m1,const FXForm3d& m2)
{
    CHECK(this != &m1 && this != &m2);
    FXForm3dConcat(this,&m1,&m2);
    return *this;
}

// Set of inline member functions for mapping points and vectors in
// place (ie: the point being mapped is replaced by the result).

inline void FXForm3d::map(FXPoint3d& p) const
{
    FXPoint3d   result;
    map(result,p);
    p = result;
}

inline void FXForm3d::map(FXPoint4d& p) const
{
    FXPoint4d   result;
    map(result,p);
    p = result;
}

inline void FXForm3d::map(FXVec3d& v) const
{
    FXVec3d result;
    map(result,v);
    v = result;
}

inline void FXForm3d::map(FXVec4d& v) const
{
    FXVec4d result;
    map(result,v);
    v = result;
}

inline FXForm3d& FXForm3d::concat(const FXForm3d& m)
{
    FXForm3d result;
    result.concat(*this,m);
    *this = result;
    return *this;
}

#endif  /* __cplusplus */

#endif  /* __FX_XFORM3D_H */
