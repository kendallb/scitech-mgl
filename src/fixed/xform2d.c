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
* Language:     ANSI C
* Environment:  any
*
* Description:  Implementation for the 2D transformation routines.
*
****************************************************************************/

#include "fx/xform2d.h"

/*------------------------- Implementation --------------------------------*/

#ifdef  FIXED386

void _ASMAPI F386_scale2D(FXFixed *m,FXFixed xscale,FXFixed yscale);
void _ASMAPI F386_scaleAbout2D(FXFixed *m,FXFixed xscale,FXFixed yscale,
    FXFixed x,FXFixed y);
void _ASMAPI F386_rotate2D(FXFixed *T,FXFixed *m,FXFixed Sin,FXFixed Cos);
void _ASMAPI F386_rotateAbout2D(FXFixed *T,FXFixed *m,FXFixed Sin,FXFixed Cos,
    FXFixed x,FXFixed y);

#endif

void FXForm2dSet(FXForm2d *xf,const real *data)
/****************************************************************************
*
* Function:     FXForm2dSet
* Parameters:   xf      - XForm to set
*               data    - Array of real's to contruct matrix from
*
* Description:  Constructor given an array of data to use.
*
****************************************************************************/
{
    memcpy(xf->mat, data, sizeof(xf->mat));
    xf->special = (xf->mat[0][1] == 0 && xf->mat[1][0] == 0);
}

void FXForm2dIdentity(FXForm2d *xf)
/****************************************************************************
*
* Function:     FXForm2dIdentity
* Parameters:   xf  - XForm to set to the identity xform
*
****************************************************************************/
{
    FXForm2dClear(xf);
    xf->mat[0][0] = xf->mat[1][1] = REAL(1);
    xf->special = true;
}

void FXForm2dInverse(FXForm2d *r,const FXForm2d *xf)
/****************************************************************************
*
* Function:     FXForm2dInverse
* Parameters:   r   - Resultant inverse XForm
*               xf  - XForm to invert
*
* Description:  Computes the inverse of the 3x3 matrix, and returns a
*               reference to it. We calculate this with the following
*               formula:
*                        -1
*                       A   = ___1___ adjoint A
*                              det A
*
*               Since the bottom line of out 3x3 transformation matrices
*               will always be [0,0,1], we can take advantage of this to
*               eliminate a lot of the computation.
*
****************************************************************************/
{
    real    a1,a2,a3,b1,b2,b3,det;

    /* Assign to individual variable names to aid
     * selecting correct values
     */
    a1 = xf->mat[0][0]; a2 = xf->mat[0][1]; a3 = xf->mat[0][2];
    b1 = xf->mat[1][0]; b2 = xf->mat[1][1]; b3 = xf->mat[1][2];

    /* Calculate the 3x3 determinent */
    det = FXoneOver(FXmul(a1,b2) - FXmul(b1,a2));

    /* Scale the adjoint matrix to get the inverse */
    r->mat[0][0] = FXmul(b2,det);
    r->mat[1][0] = FXmul(-b1,det);
    r->mat[0][1] = FXmul(-a2,det);
    r->mat[1][1] = FXmul(a1,det);
    r->mat[0][2] = FXmul(FXmul(a2,b3) - FXmul(b2,a3),det);
    r->mat[1][2] = FXmul(FXmul(b1,a3) - FXmul(a1,b3),det);
}

void FXForm2dScale(FXForm2d *xf,real xscale,real yscale)
/****************************************************************************
*
* Function:     FXForm2dScale
* Parameters:   xf      - XForm to concat scale transform to
*               xscale  - Scale along x axis
*               yscale  - Scale along y axis
*
* Description:  Concatenates a scale matrix about the origin to the current
*               transformation matrix efficiently.
*
****************************************************************************/
{
#ifdef  FIXED386
    F386_scale2D(&xf->mat[0][0],xscale,yscale);
#else
    FXmuleq(xf->mat[0][0],xscale);
    FXmuleq(xf->mat[0][1],xscale);
    FXmuleq(xf->mat[0][2],xscale);
    FXmuleq(xf->mat[1][0],yscale);
    FXmuleq(xf->mat[1][1],yscale);
    FXmuleq(xf->mat[1][2],yscale);
#endif
    xf->special = (xf->mat[0][1] == 0 && xf->mat[1][0] == 0);
}

void FXForm2dScaleAbout(FXForm2d *xf,real xscale,real yscale,real x,real y)
/****************************************************************************
*
* Function:     FXForm2dScaleAbout
* Parameters:   xf      - XForm to concat scale transform to
*               xscale  - Scale along x axis
*               yscale  - Scale along y axis
*               x,y     - Point to scale about
*
* Description:  Concatenates a scale matrix about the point p to the current
*               transformation matrix efficiently.
*
****************************************************************************/
{
#ifdef  FIXED386
    F386_scaleAbout2D(&xf->mat[0][0],xscale,yscale,x,y);
#else
    FXmuleq(xf->mat[0][0],xscale);
    FXmuleq(xf->mat[0][1],xscale);
    xf->mat[0][2] = FXmul(xf->mat[0][2],xscale) + FXmul(x,REAL(1) - xscale);
    FXmuleq(xf->mat[1][0],yscale);
    FXmuleq(xf->mat[1][1],yscale);
    xf->mat[1][2] = FXmul(xf->mat[1][2],yscale) + FXmul(y,REAL(1) - yscale);
#endif
    xf->special = (xf->mat[0][1] == 0 && xf->mat[1][0] == 0);
}

void FXForm2dTranslate(FXForm2d *xf,real xtrans,real ytrans)
/****************************************************************************
*
* Function:     FXForm2dTranslate
* Parameters:   xf      - XForm to concat translate tranform to
*               xtrans  - Translation value for x coordinates
*               ytrans  - Translation value for y coordinates
*
* Description:  Concatenates a translation matrix to the current
*               transformation matrix efficiently.
*
****************************************************************************/
{
    xf->mat[0][2] += xtrans;
    xf->mat[1][2] += ytrans;
    xf->special = (xf->mat[0][1] == 0 && xf->mat[1][0] == 0);
}

void FXForm2dRotate(FXForm2d *xf,real angle)
/****************************************************************************
*
* Function:     FXForm2dRotate
* Parameters:   xf      - XForm to concat rotate transform to
*               angle   - Angle of rotation (in radians)
*
* Description:  Concatenates a rotation matrix about the origin to the
*               current transformation matrix efficiently.
*
****************************************************************************/
{
    real Sin,Cos,T[2][3];

    FXsincos(angle,&Sin,&Cos);
#ifdef  FIXED386
    F386_rotate2D(&T[0][0],&xf->mat[0][0],Sin,Cos);
#else
    T[0][0] = FXmul(xf->mat[0][0],Cos) - FXmul(xf->mat[1][0],Sin);
    T[0][1] = FXmul(xf->mat[0][1],Cos) - FXmul(xf->mat[1][1],Sin);
    T[0][2] = FXmul(xf->mat[0][2],Cos) - FXmul(xf->mat[1][2],Sin);
    T[1][0] = FXmul(xf->mat[1][0],Cos) + FXmul(xf->mat[0][0],Sin);
    T[1][1] = FXmul(xf->mat[1][1],Cos) + FXmul(xf->mat[0][1],Sin);
    T[1][2] = FXmul(xf->mat[1][2],Cos) + FXmul(xf->mat[0][2],Sin);
#endif
    memcpy(xf->mat,T,sizeof(T));
    xf->special = false;
}

void FXForm2dRotateAbout(FXForm2d *xf,real angle,real x,real y)
/****************************************************************************
*
* Function:     FXForm2dRotateAbout
* Parameters:   xf      - XForm to concat rotate matrix to
*               angle   - Angle of rotation (in radians)
*               x,y     - Point to rotate about.
*
* Description:  Concatenates a rotation matrix about the point p to the
*               current transformation matrix efficiently.
*
****************************************************************************/
{
    real Sin,Cos,T[2][3];

    FXsincos(angle,&Sin,&Cos);
#ifdef  FIXED386
    F386_rotateAbout2D(&T[0][0],&xf->mat[0][0],Sin,Cos,x,y);
#else
    T[0][0] = FXmul(xf->mat[0][0],Cos) - FXmul(xf->mat[1][0],Sin);
    T[0][1] = FXmul(xf->mat[0][1],Cos) - FXmul(xf->mat[1][1],Sin);
    T[0][2] = FXmul(xf->mat[0][2],Cos) - FXmul(xf->mat[1][2],Sin) +
              FXmul(x,REAL(1) - Cos) + FXmul(y,Sin);
    T[1][0] = FXmul(xf->mat[1][0],Cos) + FXmul(xf->mat[0][0],Sin);
    T[1][1] = FXmul(xf->mat[1][1],Cos) + FXmul(xf->mat[0][1],Sin);
    T[1][2] = FXmul(xf->mat[1][2],Cos) + FXmul(xf->mat[0][2],Sin) +
              FXmul(y,REAL(1) - Cos) - FXmul(x,Sin);
#endif
    memcpy(xf->mat,T,sizeof(T));
    xf->special = false;
}

void FXForm2dWorldToView(FXForm2d *xf,real Umax,real Vmax,real Xmin,real Xmax,
    real Ymin,real Ymax)
/****************************************************************************
*
* Function:     FXForm2dWorldToView
* Parameters:   xf      - XForm to concat matrix to
*               Umax    - Maximum viewport x coordinate
*               Vmax    - Maximum viewport y coordinate
*               Xmin    - Minimum world x coordinate
*               Xmax    - Maximum world x coordinate
*               Ymin    - Minimum world y coordinate
*               Ymax    - Maximum world y coordinate
*
* Description:  Builds a matrix to map points from a world coordinate
*               system to a viewport. It assumes that the minimum viewport
*               coordinate is always (0,0), and that points are mapped
*               into the viewport with (0,0) being the top left hand
*               corner.
*
****************************************************************************/
{
    FXForm2dClear(xf);
    xf->mat[0][0] = FXdiv(Umax,Xmax - Xmin);
    xf->mat[1][1] = FXdiv(-Vmax,Ymax - Ymin);
    xf->mat[0][2] = FXmul(-Xmin,xf->mat[0][0]);
    xf->mat[1][2] = FXmul(Ymin,-xf->mat[1][1]) + Vmax;
    xf->special = true;
}

void FXForm2dViewToWorld(FXForm2d *xf,real Umax,real Vmax,real Xmin,real Xmax,
    real Ymin,real Ymax)
/****************************************************************************
*
* Function:     FXForm2dViewToWorld
* Parameters:   xf      - XForm to concat matrix to
*               Umax    - Maximum viewport x coordinate
*               Vmax    - Maximum viewport y coordinate
*               Xmin    - Minimum world x coordinate
*               Xmax    - Maximum world x coordinate
*               Ymin    - Minimum world y coordinate
*               Ymax    - Maximum world y coordinate
*
* Description:  Builds a matrix to map points from viewport coordinates
*               to world coordinates. It assumes that the minimum viewport
*               coordinate is always (0,0), and that points are mapped
*               into the viewport with (0,0) being the top left hand
*               corner.
*
****************************************************************************/
{
    FXForm2dClear(xf);
    xf->mat[0][0] = FXdiv(Xmax - Xmin,Umax);
    xf->mat[1][1] = FXdiv(-(Ymax - Ymin),Vmax);
    xf->mat[0][2] = Xmin;
    xf->mat[1][2] = Ymax;
    xf->special = true;
}

#ifndef FIXED386        /* The 386 has special routines for this    */

void FL_map2D(const FXMat2x3 mat,FXPoint2d *result,const FXPoint2d *p,
    ibool special)
/****************************************************************************
*
* Function:     FL_map2D
* Parameters:   mat     - XForm to map with
*               result  - Place to store result of mapping
*               p       - Point to map
*               special - Is the XForm special cased?
*
* Description:  Maps the point p, by pre-multiplying it with the matrix.
*               Note that we do not calculate the homogenous point for
*               the matrix, since we take advantage of the fact that the
*               form of all 2d transformation matrices is (foly sec 5.5):
*
*                   [ r11, r12, tx ]
*                   [ r21, r22, ty ]
*                   [ 0,   0,   1  ]
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    if (special) {
        result->x = FXmul(p->x,mat[0][0]) + mat[0][2];
        result->y = FXmul(p->y,mat[1][1]) + mat[1][2];
        }
    else {
        result->x = FXmul(p->x,mat[0][0]) + FXmul(p->y,mat[0][1]) + mat[0][2];
        result->y = FXmul(p->x,mat[1][0]) + FXmul(p->y,mat[1][1]) + mat[1][2];
        }
}

void FL_mapVec2D(const FXMat2x3 mat,FXVec2d *result,const FXVec2d *v,
    ibool special)
/****************************************************************************
*
* Function:     FL_mapVec2D
* Parameters:   mat     - XForm to map with
*               result  - Place to store result of mapping
*               v       - Vector to map
*               special - Is the XForm special cased?
*
* Description:  Maps the vector v, by pre-multiplying it with the matrix.
*               Mapping a vector is different to mapping a point, as the
*               vector is not affected by translation, since it is a
*               directional measurement.
*
*               NOTE: The result vector CANNOT be the same as the input
*                     vector.
*
****************************************************************************/
{
    if (special) {
        result->x = FXmul(v->x,mat[0][0]);
        result->y = FXmul(v->y,mat[1][1]);
        }
    else {
        result->x = FXmul(v->x,mat[0][0]) + FXmul(v->y,mat[0][1]);
        result->y = FXmul(v->x,mat[1][0]) + FXmul(v->y,mat[1][1]);
        }
}

void FL_concat3x3(FXMat2x3 mat,const FXMat2x3 mat1,const FXMat2x3 mat2)
/****************************************************************************
*
* Function:     FL_concat3x3
* Parameters:   mat     - Place to store result
*               mat1    - First matrix to multiply
*               mat2    - Second matrix to multiply
*
* Description:  Concatenates the two transformation matrices together and
*               stores the result.
*
*               NOTE: The result matrix CANNOT be the same as the input
*                     matrix.
*
****************************************************************************/
{
    /* This routine is completely unrolled and is optimised for the special
     * case of 2d transformation matrices (the bottom row is always 0,0,1).
     */
    mat[0][0] = FXmul(mat1[0][0],mat2[0][0]) +
                FXmul(mat1[0][1],mat2[1][0]);
    mat[0][1] = FXmul(mat1[0][0],mat2[0][1]) +
                FXmul(mat1[0][1],mat2[1][1]);
    mat[0][2] = FXmul(mat1[0][0],mat2[0][2]) +
                FXmul(mat1[0][1],mat2[1][2]) + mat1[0][2];
    mat[1][0] = FXmul(mat1[1][0],mat2[0][0]) +
                FXmul(mat1[1][1],mat2[1][0]);
    mat[1][1] = FXmul(mat1[1][0],mat2[0][1]) +
                FXmul(mat1[1][1],mat2[1][1]);
    mat[1][2] = FXmul(mat1[1][0],mat2[0][2]) +
                FXmul(mat1[1][1],mat2[1][2]) + mat1[1][2];
}

#endif  // FIXED386
