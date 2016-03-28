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
* Description:  Member functions for the 3D transformation class. This class
*               provides specialised transformation operations on 3d points
*               used in computer graphics applications. Refer to
*               "Computer Graphics: Principles and Practice" by Foley,
*               van Dam, Feiner and Hughes for information on the matrices
*               generated and 3d viewing. The transformations can be
*               constructed from either IEEE float's, double's, long
*               double's or fixed point numbers.
*
****************************************************************************/

#include "fx/xform3d.h"
#include <math.h>

/*------------------------- Implementation --------------------------------*/

#ifdef  FIXED386
void _ASMAPI F386_scale3D(FXFixed *m,FXFixed xscale,FXFixed yscale,FXFixed zscale);
void _ASMAPI F386_rotatex3D(FXFixed *T,FXFixed *m,FXFixed Sin,FXFixed Cos);
void _ASMAPI F386_rotatey3D(FXFixed *T,FXFixed *m,FXFixed Sin,FXFixed Cos);
void _ASMAPI F386_rotatez3D(FXFixed *T,FXFixed *m,FXFixed Sin,FXFixed Cos);
void _ASMAPI F386_rotate3D(FXFixed *T,FXFixed *T2,FXFixed *m,FXFixed Sin,
    FXFixed Cos,FXFixed Cos_1,FXFixed x,FXFixed y,FXFixed z);
#endif

void FXForm3dSet(FXForm3d *xf,const real *data)
/****************************************************************************
*
* Function:     FXForm3dSet
* Parameters:   xf      - XForm to set
*               data    - Array of real's to contruct matrix from
*
* Description:  Constructor given an array of data to use.
*
****************************************************************************/
{
    memcpy(xf->mat, data, sizeof(xf->mat));
    xf->special = (xf->mat[3][0] == 0 && xf->mat[3][1] == 0
        && xf->mat[3][2] == 0 && xf->mat[3][3] == REAL(1));
}

void FXForm3dSetTranspose(FXForm3d *xf,const real *data)
/****************************************************************************
*
* Function:     FXForm3dSetTranspose
* Parameters:   xf      - XForm to set
*               data    - Array of real's to contruct matrix from
*
* Description:  Assign the transpose of a set of data to the matrix.
*
****************************************************************************/
{
#define _IN(row,col)   data[col*4+row]
    xf->mat[0][0] = _IN(0,0);   xf->mat[0][1] = _IN(1,0);
    xf->mat[0][2] = _IN(2,0);   xf->mat[0][3] = _IN(3,0);
    xf->mat[1][0] = _IN(0,1);   xf->mat[1][1] = _IN(1,1);
    xf->mat[1][2] = _IN(2,1);   xf->mat[1][3] = _IN(3,1);
    xf->mat[2][0] = _IN(0,2);   xf->mat[2][1] = _IN(1,2);
    xf->mat[2][2] = _IN(2,2);   xf->mat[2][3] = _IN(3,2);
    xf->mat[3][0] = _IN(0,3);   xf->mat[3][1] = _IN(1,3);
    xf->mat[3][2] = _IN(2,3);   xf->mat[3][3] = _IN(3,3);
#undef  _IN
    xf->special = (xf->mat[3][0] == 0 && xf->mat[3][1] == 0
        && xf->mat[3][2] == 0 && xf->mat[3][3] == REAL(1));
}

void FXForm3dIdentity(FXForm3d *xf)
/****************************************************************************
*
* Function:     FXForm3dIdentity
* Parameters:   xf  - XForm to set to the identity xform
*
****************************************************************************/
{
    FXForm3dClear(xf);
    xf->mat[0][0] = xf->mat[1][1] = xf->mat[2][2] = xf->mat[3][3] = REAL(1);
    xf->special = true;
}

void FXForm3dInverse(FXForm3d *r,const FXForm3d *xf)
/****************************************************************************
*
* Function:     FXForm3dInverse
* Parameters:   r   - Resultant inverse XForm
*               xf  - XForm to invert
*
* Description:  Computes the inverse of the 4x4 matrix, and returns a
*               reference to it. If the original matrix was special,
*               the inverse will be special also.
*
****************************************************************************/
{
    int         i,j,k,swap;
    real        t;
    FXForm3d    temp;

    FXForm3dIdentity(r);
    FXForm3dCopy(&temp,xf);

    for (i = 0; i < 4; i++) {
        swap = i;           /* Look for largest element in column   */
        t = FXabs(temp.mat[i][i]);
        for (j = i + 1; j < 4; j++)
            if (FXabs(temp.mat[j][i]) > t)
                swap = j;

        if (swap != i) {
            for (k = 0; k < 4; k++) {               /* Swap rows    */
                t = temp.mat[i][k];
                temp.mat[i][k] = temp.mat[swap][k];
                temp.mat[swap][k] = t;

                t = r->mat[i][k];
                r->mat[i][k] = r->mat[swap][k];
                r->mat[swap][k] = t;
                }
            }

        if (temp.mat[i][i] == 0) {
            /* No non-zero pivot. The matrix is singular, which typically
             * shouldn't happen.
             */
            return;
            }

        t = FXoneOver(temp.mat[i][i]);
        for (k = 0; k < 4; k++) {
            FXmuleq(temp.mat[i][k],t);
            FXmuleq(r->mat[i][k],t);
            }
        for (j = 0; j < 4; j++) {
            if (j != i) {
                t = temp.mat[j][i];
                for (k = 0; k < 4; k++) {
                    temp.mat[j][k] -= FXmul(temp.mat[i][k],t);
                    r->mat[j][k] -= FXmul(r->mat[i][k],t);
                    }
                }
            }
        }
}

void FXForm3dScale(FXForm3d *xf,real xscale,real yscale,real zscale)
/****************************************************************************
*
* Function:     FXForm3dcale
* Parameters:   xf      - XForm to concat scale transform to
*               xscale  - Scale along x axis
*               yscale  - Scale along y axis
*               zscale  - Scale about z axis
*
* Description:  Concatenates a scale matrix about the origin to the current
*               transformation matrix.
*
****************************************************************************/
{
#ifdef  FIXED386
    F386_scale3D(&xf->mat[0][0],xscale,yscale,zscale);
#else
    FXmuleq(xf->mat[0][0],xscale);
    FXmuleq(xf->mat[0][1],xscale);
    FXmuleq(xf->mat[0][2],xscale);
    FXmuleq(xf->mat[0][3],xscale);
    FXmuleq(xf->mat[1][0],yscale);
    FXmuleq(xf->mat[1][1],yscale);
    FXmuleq(xf->mat[1][2],yscale);
    FXmuleq(xf->mat[1][3],yscale);
    FXmuleq(xf->mat[2][0],zscale);
    FXmuleq(xf->mat[2][1],zscale);
    FXmuleq(xf->mat[2][2],zscale);
    FXmuleq(xf->mat[2][3],zscale);
#endif
}

void FXForm3dTranslate(FXForm3d *xf,real xtrans,real ytrans,real ztrans)
/****************************************************************************
*
* Function:     FXForm3dTranslate
* Parameters:   xf      - XForm to translate
*               xtrans  - Translation value for x coordinates
*               ytrans  - Translation value for y coordinates
*               ztrans  - Translation value for z coordinates
*
* Description:  Concatenates a translation matrix to the current transform.
*
****************************************************************************/
{
    xf->mat[0][3] += xtrans;
    xf->mat[1][3] += ytrans;
    xf->mat[2][3] += ztrans;
}

void FXForm3dRotatex(FXForm3d *xf,real angle)
/****************************************************************************
*
* Function:     FXForm3dRotatex
* Parameters:   angle   - Angle of rotation about x axis
*
* Description:  Creates a rotation matrix for rotating points about the
*               x axis. Looking down the x axis (from +ve) to the origin,
*               the rotation is anti-clockwise.
*
****************************************************************************/
{
    real Sin,Cos,T[2][4];

    FXsincos(angle,&Sin,&Cos);
#ifdef  FIXED386
    F386_rotatex3D(&T[0][0],&xf->mat[0][0],Sin,Cos);
#else
    T[0][0] = FXmul(xf->mat[1][0],Cos) - FXmul(xf->mat[2][0],Sin);
    T[0][1] = FXmul(xf->mat[1][1],Cos) - FXmul(xf->mat[2][1],Sin);
    T[0][2] = FXmul(xf->mat[1][2],Cos) - FXmul(xf->mat[2][2],Sin);
    T[0][3] = FXmul(xf->mat[1][3],Cos) - FXmul(xf->mat[2][3],Sin);
    T[1][0] = FXmul(xf->mat[2][0],Cos) + FXmul(xf->mat[1][0],Sin);
    T[1][1] = FXmul(xf->mat[2][1],Cos) + FXmul(xf->mat[1][1],Sin);
    T[1][2] = FXmul(xf->mat[2][2],Cos) + FXmul(xf->mat[1][2],Sin);
    T[1][3] = FXmul(xf->mat[2][3],Cos) + FXmul(xf->mat[1][3],Sin);
#endif
    memcpy(&xf->mat[1][0],T,sizeof(T));
}

void FXForm3dRotatey(FXForm3d *xf,real angle)
/****************************************************************************
*
* Function:     FXForm3dRotatey
* Parameters:   angle   - Angle of rotation about y axis
*
* Description:  Creates a rotation matrix for rotating points about the
*               y axis. Looking down the y axis (from +ve) to the origin,
*               the rotation is anti-clockwise.
*
****************************************************************************/
{
    real Sin,Cos,T[2][4];

    FXsincos(angle,&Sin,&Cos);
#ifdef  FIXED386
    F386_rotatey3D(&T[0][0],&xf->mat[0][0],Sin,Cos);
#else
    T[0][0] = FXmul(xf->mat[0][0],Cos) - FXmul(xf->mat[2][0],Sin);
    T[0][1] = FXmul(xf->mat[0][1],Cos) - FXmul(xf->mat[2][1],Sin);
    T[0][2] = FXmul(xf->mat[0][2],Cos) - FXmul(xf->mat[2][2],Sin);
    T[0][3] = FXmul(xf->mat[0][3],Cos) - FXmul(xf->mat[2][3],Sin);
    T[1][0] = FXmul(xf->mat[2][0],Cos) + FXmul(xf->mat[0][0],Sin);
    T[1][1] = FXmul(xf->mat[2][1],Cos) + FXmul(xf->mat[0][1],Sin);
    T[1][2] = FXmul(xf->mat[2][2],Cos) + FXmul(xf->mat[0][2],Sin);
    T[1][3] = FXmul(xf->mat[2][3],Cos) + FXmul(xf->mat[0][3],Sin);
#endif
    memcpy(&xf->mat[0][0],T,sizeof(T)/2);
    memcpy(&xf->mat[2][0],&T[1][0],sizeof(T)/2);
}


void FXForm3dRotatez(FXForm3d *xf,real angle)
/****************************************************************************
*
* Function:     FXForm3dRotatez
* Parameters:   angle   - Angle of rotation about z axis
*
* Description:  Creates a rotation xf->matrix for rotating points about the
*               z axis. Looking down the z axis (from +ve) to the origin,
*               the rotation is anti-clockwise.
*
****************************************************************************/
{
    real Sin,Cos,T[2][4];

    FXsincos(angle,&Sin,&Cos);
#ifdef  FIXED386
    F386_rotatez3D(&T[0][0],&xf->mat[0][0],Sin,Cos);
#else
    T[0][0] = FXmul(xf->mat[0][0],Cos) - FXmul(xf->mat[1][0],Sin);
    T[0][1] = FXmul(xf->mat[0][1],Cos) - FXmul(xf->mat[1][1],Sin);
    T[0][2] = FXmul(xf->mat[0][2],Cos) - FXmul(xf->mat[1][2],Sin);
    T[0][3] = FXmul(xf->mat[0][3],Cos) - FXmul(xf->mat[1][3],Sin);
    T[1][0] = FXmul(xf->mat[1][0],Cos) + FXmul(xf->mat[0][0],Sin);
    T[1][1] = FXmul(xf->mat[1][1],Cos) + FXmul(xf->mat[0][1],Sin);
    T[1][2] = FXmul(xf->mat[1][2],Cos) + FXmul(xf->mat[0][2],Sin);
    T[1][3] = FXmul(xf->mat[1][3],Cos) + FXmul(xf->mat[0][3],Sin);
#endif
    memcpy(xf->mat,T,sizeof(T));
}

void FXForm3dRotate(FXForm3d *xf,real angle,real x,real y,real z)
/****************************************************************************
*
* Function:     FXForm3dRotate
* Parameters:   angle   - Angle of rotation about the axis
*               x,y,z   - Vector to rotate about
*
* Description:  Concatenates a rotation xf->matrix for rotating points about an
*               arbitrary axis. The axis is defined as a vector passing
*               through the origin (does not need to be normalised). Looking
*               down the axis to the origin (ie: from the point n to the
*               origin or in the opposite direction to the vector) the
*               rotation is anti-clockwise.
*
****************************************************************************/
{
    real    Sin,Cos,Cos_1,T[3][3],T2[3][4];
    FXVec3d n;

    FXVec3dSet(n,x,y,z);
    FXVec3dNormalise(n);            /* Normalise the axis vector    */
    FXsincos(angle,&Sin,&Cos);
    Cos_1 = REAL(1) - Cos;
#ifdef  FIXED386
    F386_rotate3D(&T[0][0],&T2[0][0],&xf->mat[0][0],Sin,Cos,Cos_1,
        n.x,n.y,n.z);
#else
    // Create the temporary 3x3 sub-rotation xf->matrix
    T[0][0] = Cos + FXmul(FXsquare(n.x),Cos_1);
    T[0][1] = FXmul(FXmul(n.x,n.y),Cos_1) - FXmul(n.z,Sin);
    T[0][2] = FXmul(FXmul(n.x,n.z),Cos_1) + FXmul(n.y,Sin);
    T[1][0] = -T[0][1];
    T[1][1] = Cos + FXmul(FXsquare(n.y),Cos_1);
    T[1][2] = FXmul(FXmul(n.y,n.z),Cos_1) - FXmul(n.x,Sin);
    T[2][0] = -T[0][2];
    T[2][1] = -T[1][2];
    T[2][2] = Cos + FXmul(FXsquare(n.z),Cos_1);

    // Now concatentate this rotation xf->matrix with the upper left 3x3
    // sub-xf->matrix of the current transforxf->mation xf->matrix
    T2[0][0] =  FXmul(T[0][0],xf->mat[0][0]) + FXmul(T[0][1],xf->mat[1][0]) +
                FXmul(T[0][2],xf->mat[2][0]);
    T2[0][1] =  FXmul(T[0][0],xf->mat[0][1]) + FXmul(T[0][1],xf->mat[1][1]) +
                FXmul(T[0][2],xf->mat[2][1]);
    T2[0][2] =  FXmul(T[0][0],xf->mat[0][2]) + FXmul(T[0][1],xf->mat[1][2]) +
                FXmul(T[0][2],xf->mat[2][2]);

    T2[1][0] =  FXmul(T[1][0],xf->mat[0][0]) + FXmul(T[1][1],xf->mat[1][0]) +
                FXmul(T[1][2],xf->mat[2][0]);
    T2[1][1] =  FXmul(T[1][0],xf->mat[0][1]) + FXmul(T[1][1],xf->mat[1][1]) +
                FXmul(T[1][2],xf->mat[2][1]);
    T2[1][2] =  FXmul(T[1][0],xf->mat[0][2]) + FXmul(T[1][1],xf->mat[1][2]) +
                FXmul(T[1][2],xf->mat[2][2]);

    T2[2][0] =  FXmul(T[2][0],xf->mat[0][0]) + FXmul(T[2][1],xf->mat[1][0]) +
                FXmul(T[2][2],xf->mat[2][0]);
    T2[2][1] =  FXmul(T[2][0],xf->mat[0][1]) + FXmul(T[2][1],xf->mat[1][1]) +
                FXmul(T[2][2],xf->mat[2][1]);
    T2[2][2] =  FXmul(T[2][0],xf->mat[0][2]) + FXmul(T[2][1],xf->mat[1][2]) +
                FXmul(T[2][2],xf->mat[2][2]);
#endif
    T2[0][3] =  xf->mat[0][3];      /* Last column is not changed */
    T2[1][3] =  xf->mat[1][3];
    T2[2][3] =  xf->mat[2][3];
    memcpy(xf->mat,T2,sizeof(T2));
}

void FXForm3dViewOrientation(FXForm3d *xf,const FXPoint3d *VRP,
    const FXVec3d *VPN,const FXVec3d *VUP)
/****************************************************************************
*
* Function:     FXForm3dViewOrientation
* Parameters:   VRP - View reference point
*               VPN - View plane normal
*               VUP - View UP vector
*
* Description:  Calculates a view orientation matrix given the values of
*               the View Reference point (VRP), View Plane Normal (VPN) and
*               the View UP vector (VUP). The steps involved are:
*
*               1. Translate the VRP to the origin.
*
*               2. Rotate the View Reference Coordinate system (VRC) such
*                  that the n axis (VPN) becomes the z axis, the u axis
*                  becomes the x axis and the v axis becomes the y axis.
*
*                  We do this by utilising the properties of orthonormal
*                  vectors to construct the rotation matrix from the
*                  unit vectors along the u,v and n axes (foley pp220-2);
*
*   CAREFUL!    The VPN CANNOT be a zero length vector, and the VUP vector
*               must a different vector to the VPN. If this is not met, a
*               divide by zero will result!
*
****************************************************************************/
{
    FXForm3d    trans,rot;
    FXVec3d     Rz,Rx,Ry;

    FXForm3dIdentity(&trans);
    FXForm3dCopy(&rot,&trans);  /* Set to identity matrices */

    /* Translate the VRC to the origin */
    trans.mat[0][3] = -VRP->x;
    trans.mat[1][3] = -VRP->y;
    trans.mat[2][3] = -VRP->z;

    /* The unit vector along the n axis (which rotates into the z axis)
     * is the unit vector in the direction of VPN.
     */
    FXVec3dCopy(Rz,*VPN);
    FXVec3dNormalise(Rz);

    /* The unit vector along the u axis (which rotates into the x axis)
     * is the normalised cross product of VUP and VPN.
     */
    FXVec3dCross(Rx,*VUP,*VPN);
    FXVec3dNormalise(Rx);

    /* The unit vector along the v axis (which rotates into the y axis)
     * is the cross product of Rx and Rz. The result will be a unit vector
     * since Rx and Rz are already unit vectors.
     */
    FXVec3dCross(Ry,Rz,Rx);

    /* Fill in the rotation matrix with the row vectors Rx,Ry and Rz */
    rot.mat[0][0] = Rx.x;   rot.mat[0][1] = Rx.y;   rot.mat[0][2] = Rx.z;
    rot.mat[1][0] = Ry.x;   rot.mat[1][1] = Ry.y;   rot.mat[1][2] = Ry.z;
    rot.mat[2][0] = Rz.x;   rot.mat[2][1] = Rz.y;   rot.mat[2][2] = Rz.z;

    /* Combine the translation matrix and rotation matrix to obtain the
     * final View Orientation Matrix, or R.T(-VRP)
     */
    FXForm3dConcat(xf,&rot,&trans);
}

void FXForm3dViewMapping(FXForm3d *xf,real umin,real vmin,real umax,
    real vmax,int proj_type,const FXPoint3d *PRP,real F,real B,
    real NDC_maxx,real NDC_maxy,int aspect,ibool keep_square)
/****************************************************************************
*
* Function:     FXForm3dViewMapping
* Parameters:   umin        - Left edge of view plane window (in VRC's)
*               vmin        - Bottom edge of view plane window (in VRC's)
*               umax        - Right edge of view plane window (in VRC's)
*               vmax        - Top edge of view plane window (in VRC's)
*               proj_type   - Projection type (PERSPECTIVE or PARALLEL)
*               PRP         - Projection Reference Point (in VRC's)
*               F           - Front clipping plane (in VRC's)
*               B           - Back clipping plane (in VRC's)
*               NDC_maxx    - Maximum NDC x coordinate
*               NDC_maxy    - Maximum NDC y coordinate
*               aspect      - Output device pixel aspect ratio
*               keep_square - True if squares should look like true squares
*
* Description:  Calculates the view mapping matrix for the given
*               specifications. The view mapping matrix does the final
*               mapping to map visible points into Normalised Device
*               Coordinates (NDC), where all visible points will lie in
*               the ranges:
*
*                           0 <= x <= NDC_maxx
*                           0 <= y <= NDC_maxy
*                          -1 <= z <= 0
*
*               Note however that for perspective views the resulting
*               coordinates will be homogenous, but will lie in the
*               parallel-projection canonical view volume when homogenised
*               into 3 space. Thus all clipping can be done against the
*               above volume in homogenous coordinates. To map to Physical
*               Device Coordinates (PDC) we simply ignore the z coordinate
*               and scan convert (assuming the primitives have been
*               clipped!). The z coordinates can be subsequently used for
*               hidden line and hidden surface removal.
*
*               If keep_square is true, we scale the size of the View Plane
*               windows u coordinates (ie: x coordinates) to fit the size
*               of the viewport. We also take into account the given
*               output device's pixel aspect ratio to ensure that squares
*               appear as true squares. The value of aspect should be set
*               to:
*
*                       pixel x size
*                       ------------ * 1000
*                       pixel y size
*
*
*               There are a few points to note. Firstly the View Plane
*               Window (VPW) is defined in View Reference Coordinates (VRC),
*               which are the same scale as the World Coordinates used by
*               the application, as are the Front and Back clipping planes.
*
*   CAREFUL!    The PRP MUST be greater than zero, the Front Clipping Plane
*               MUST be less than the PRP and the back clipping
*               plane MUST be less than the Front Clipping plane. Failure
*               to observe these precautions will produce some interesting
*               3d projections!
*
****************************************************************************/
{
    FXForm3d    m,m2;
    FXVec3d     DOP;
    real        centre_u,centre_v,SHx,SHy,zmin;

    FXForm3dIdentity(xf);

    if (proj_type == FXForm3dPerspective) {
        /* Translate the PRP to the origin */
        xf->mat[0][3] = -PRP->x;
        xf->mat[1][3] = -PRP->y;
        xf->mat[2][3] = -PRP->z;
        }

    // Find the centre of the viewing window
#ifdef  FX_FIXED
    centre_u = (umax + umin) >> 1;
    centre_v = (vmax + vmin) >> 1;
#else
    centre_u = (umax + umin) * 0.5;
    centre_v = (vmax + vmin) * 0.5;
#endif

    if (keep_square) {
        /* Scale the umin and umax coordinates to fit the NDC coordinate
         * space (NDC_maxx, NDC_maxy), and take into account the aspect
         * ratio of the display. Note that we perform the scale about the
         * center of the window.
         */
        real factor = FXmul(FXdiv(NDC_maxx,NDC_maxy),
                            FXdiv(REAL(1000),FXintToReal(aspect)));
        umin = centre_u + FXmul(umin - centre_u, factor);
        umax = centre_u + FXmul(umax - centre_u, factor);
        }

    /* Set the Direction Of Projection (DOP) to be equal to the vector
     * from the PRP to the centre of the window. Use this to shear so
     * that the DOP lies along the z axis.
     */
    FXVec3dSet(DOP,centre_u - PRP->x, centre_v - PRP->y, -PRP->z);
    SHx = FXdiv(-DOP.x,DOP.z);              /* X shear value */
    SHy = FXdiv(-DOP.y,DOP.z);              /* Y shear value */

    /* We don't need to multiply the entire matrix, just two elements to
     * add the shear operation.
     */
    xf->mat[0][2] = SHx;
    xf->mat[1][2] = SHy;
    xf->mat[0][3] += FXmul(xf->mat[2][3], SHx);
    xf->mat[1][3] += FXmul(xf->mat[2][3], SHy);

    if (proj_type == FXForm3dPerspective) {
        /* Scale the perspective view volume to the canonical NDC perspective
         * view volume.
         */
        FXForm3dIdentity(&m);
        m.mat[0][0] = FXdiv(FXmuldiv(-PRP->z,NDC_maxx,umax-umin),B - PRP->z);
        m.mat[1][1] = FXdiv(FXmuldiv(PRP->z,NDC_maxy,vmax-vmin),B - PRP->z);
        m.mat[2][2] = -FXoneOver(B - PRP->z);
        FXForm3dConcat(&m2,&m,xf);

        /* Map the perspective canonical view volume into the parallel
         * canonical view volume (producing non-normalised homogenous
         * coordinates!).
         */
        FXForm3dClear(&m);
        m.special = false;                  /* Flag perspective transform */
        zmin = FXdiv(PRP->z - F, B - PRP->z);
        m.mat[0][0] = m.mat[1][1] = REAL(1);
        m.mat[3][2] = REAL(-1);
        m.mat[2][2] = FXoneOver(REAL(1) + zmin);
        m.mat[2][3] = FXmul(-zmin,m.mat[2][2]);

        FXForm3dConcat(xf,&m,&m2);
        }
    else {
        /* We are doing a PARALLEL projection.
         *
         * Translate front centre of the view volume to the origin.
         */
        FXForm3dIdentity(&m);
        m.mat[0][3] = -centre_u;
        m.mat[1][3] = -centre_v;
        m.mat[2][3] = -F;
        FXForm3dConcat(&m2,&m,xf);

        /* Scale the view volume to the size of the NPC view volume, with the
         * back clipping plane set to -1.0. Note that we also flip the
         * y coordinates to remap them for the orgin residing at the top
         * left hand corner of the screen.
         */
        FXForm3dIdentity(&m);
        m.mat[0][0] = FXdiv(NDC_maxx,umax-umin);
        m.mat[1][1] = FXdiv(-NDC_maxy,vmax-vmin);
        m.mat[2][2] = FXoneOver(F - B);
        FXForm3dConcat(xf,&m,&m2);
        }

    /* Lastly we translate the top left corner of the view volume to the
     * origin.
     */
    FXForm3dIdentity(&m);
#ifdef  FX_FIXED
    m.mat[0][3] = NDC_maxx >> 1;
    m.mat[1][3] = NDC_maxy >> 1;
#else
    m.mat[0][3] = NDC_maxx * 0.5;
    m.mat[1][3] = NDC_maxy * 0.5;
#endif
    FXForm3dCopy(&m2,xf);
    FXForm3dConcat(xf,&m,&m2);
}

#ifndef FIXED386        // The 386 has special routines for fixed point

void FL_map2Dto2D(const FXMat4x4 mat,real *result,const real *p)
/****************************************************************************
*
* Function:     FL_map2Dto2D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               p       - Point to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 2D point by a 4x4 matrix. The z coordinate of
*               the point is assumed to be 0 and the homogenous coordinate
*               of the point is assumed to be 1. Note also that the matrix
*               is assumed to be special. If not, then the point should
*               be mapped as a real 3d point with a z coord of 0.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    result[0] = FXmul(p[0],mat[0][0]) + FXmul(p[1],mat[0][1]) +
                                      +            mat[0][3];
    result[1] = FXmul(p[0],mat[1][0]) + FXmul(p[1],mat[1][1]) +
                                      +            mat[1][3];
}

void FL_map3Dto3D(const FXMat4x4 mat,real *result,const real *p,ibool special)
/****************************************************************************
*
* Function:     FL_map3Dto3D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               p       - Point to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 3D point by a 4x4 matrix. The homogenous
*               coordinate of the point is assumed to be 1, and the
*               resultant point will be homogenised back into 3 space
*               if it is homogenous, so that the homogenous coordinate will
*               be 1. This routine can be used to map any 3d point given
*               any 4x4 transformation matrix.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    if (special) {
        result[0] = FXmul(p[0],mat[0][0]) + FXmul(p[1],mat[0][1]) +
                    FXmul(p[2],mat[0][2]) +            mat[0][3];
        result[1] = FXmul(p[0],mat[1][0]) + FXmul(p[1],mat[1][1]) +
                    FXmul(p[2],mat[1][2]) +            mat[1][3];
        result[2] = FXmul(p[0],mat[2][0]) + FXmul(p[1],mat[2][1]) +
                    FXmul(p[2],mat[2][2]) +            mat[2][3];
        }
    else {
        // We can afford to loose a little precision by multiplying by the
        // inverse of w for speed, even for fixed point. Since this is not
        // a specialised mapping, we have to normalise back to 3D space
        // as the result will be a homogenous coordinate.

        real   w = FXoneOver(
                          FXmul(p[0],mat[3][0]) + FXmul(p[1],mat[3][1]) +
                          FXmul(p[2],mat[3][2]) +            mat[3][3]);
        result[0] = FXmul(FXmul(p[0],mat[0][0]) + FXmul(p[1],mat[0][1]) +
                          FXmul(p[2],mat[0][2]) +            mat[0][3], w);
        result[1] = FXmul(FXmul(p[0],mat[1][0]) + FXmul(p[1],mat[1][1]) +
                          FXmul(p[2],mat[1][2]) +            mat[1][3], w);
        result[2] = FXmul(FXmul(p[0],mat[2][0]) + FXmul(p[1],mat[2][1]) +
                          FXmul(p[2],mat[2][2]) +            mat[2][3], w);
        }
}

void FL_map3Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special)
/****************************************************************************
*
* Function:     FL_map3Dto4D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               p       - Point to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 3D point by a 4x4 matrix. The
*               resultant point is not homogenised into 3 space.
*               This routine can be used to map any 3d point given
*               any 4x4 transformation matrix onto a 4D output point.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    result[0] = FXmul(p[0],mat[0][0]) + FXmul(p[1],mat[0][1]) +
                FXmul(p[2],mat[0][2]) +            mat[0][3];
    result[1] = FXmul(p[0],mat[1][0]) + FXmul(p[1],mat[1][1]) +
                FXmul(p[2],mat[1][2]) +            mat[1][3];
    result[2] = FXmul(p[0],mat[2][0]) + FXmul(p[1],mat[2][1]) +
                FXmul(p[2],mat[2][2]) +            mat[2][3];
    if (special)
        result[3] = REAL(1);
    else
        result[3] = FXmul(p[0],mat[3][0]) + FXmul(p[1],mat[3][1]) +
                    FXmul(p[2],mat[3][2]) +            mat[3][3];
}

void FL_map4Dto4D(const FXMat4x4 mat,real *result,const real *p,ibool special)
/****************************************************************************
*
* Function:     FL_map4Dto4D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               p       - Point to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 4D point by a 4x4 matrix. The
*               resultant vector is not homogenised into 3 space.
*               This routine can be used to map any 4d point given
*               any 4x4 transformation matrix.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    result[0] = FXmul(p[0],mat[0][0]) + FXmul(p[1],mat[0][1]) +
                FXmul(p[2],mat[0][2]) + FXmul(p[3],mat[0][3]);
    result[1] = FXmul(p[0],mat[1][0]) + FXmul(p[1],mat[1][1]) +
                FXmul(p[2],mat[1][2]) + FXmul(p[3],mat[1][3]);
    result[2] = FXmul(p[0],mat[2][0]) + FXmul(p[1],mat[2][1]) +
                FXmul(p[2],mat[2][2]) + FXmul(p[3],mat[2][3]);
    if (special)
        result[3] = p[3];
    else
        result[3] = FXmul(p[0],mat[3][0]) + FXmul(p[1],mat[3][1]) +
                    FXmul(p[2],mat[3][2]) + FXmul(p[3],mat[3][3]);
}

void FL_mapVec3Dto3D(const FXMat4x4 mat,real *result,const real *v,
    ibool special)
/****************************************************************************
*
* Function:     FL_mapVec3Dto3D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               v       - Vector to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 3D vector by a 4x4 matrix. The homogenous
*               coordinate of the vector is assumed to be 0, and the
*               resultant vector will be homogenised back into 3 space
*               if it is homogenous, so that the homogenous coordinate will
*               be 1. Mapping a vector is different to mapping a point, as
*               the vector is not affected by the translation components.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    if (special) {
        result[0] = FXmul(v[0],mat[0][0]) + FXmul(v[1],mat[0][1]) +
                    FXmul(v[2],mat[0][2]);
        result[1] = FXmul(v[0],mat[1][0]) + FXmul(v[1],mat[1][1]) +
                    FXmul(v[2],mat[1][2]);
        result[2] = FXmul(v[0],mat[2][0]) + FXmul(v[1],mat[2][1]) +
                    FXmul(v[2],mat[2][2]);
        }
    else {
        // We can afford to loose a little precision by multiplying by the
        // inverse of w for speed, even for fixed point. Since this is not
        // a specialised mapping, we have to normalise back to 3D space
        // as the result will be a homogenous coordinate.
        real   w = FXoneOver(
                          FXmul(v[0],mat[3][0]) + FXmul(v[1],mat[3][1]) +
                          FXmul(v[2],mat[3][2]));
        result[0] = FXmul(FXmul(v[0],mat[0][0]) + FXmul(v[1],mat[0][1]) +
                          FXmul(v[2],mat[0][2]), w);
        result[1] = FXmul(FXmul(v[0],mat[1][0]) + FXmul(v[1],mat[1][1]) +
                          FXmul(v[2],mat[1][2]), w);
        result[2] = FXmul(FXmul(v[0],mat[2][0]) + FXmul(v[1],mat[2][1]) +
                          FXmul(v[2],mat[2][2]), w);
        }
}

void FL_mapVec3Dto4D(const FXMat4x4 mat,real *result,const real *v,
    ibool special)
/****************************************************************************
*
* Function:     FL_mapVec3Dto4D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               v       - Vector to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 3D vector by a 4x4 matrix. The
*               resultant vector is not homogenised into 3 space.
*               This routine can be used to map any 3d vector given
*               any 4x4 transformation matrix onto a 4D output vector.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    result[0] = FXmul(v[0],mat[0][0]) + FXmul(v[1],mat[0][1]) +
                FXmul(v[2],mat[0][2]);
    result[1] = FXmul(v[0],mat[1][0]) + FXmul(v[1],mat[1][1]) +
                FXmul(v[2],mat[1][2]);
    result[2] = FXmul(v[0],mat[2][0]) + FXmul(v[1],mat[2][1]) +
                FXmul(v[2],mat[2][2]);
    if (special)
        result[3] = REAL(0);
    else
        result[3] = FXmul(v[0],mat[3][0]) + FXmul(v[1],mat[3][1]) +
                    FXmul(v[2],mat[3][2]);
}

void FL_mapVec4Dto4D(const FXMat4x4 mat,real *result,const real *v,
    ibool special)
/****************************************************************************
*
* Function:     FL_mapVec4Dto4D
* Parameters:   mat     - Xform to map with
*               result  - Place to store result of mapping
*               v       - Vector to map
*               special - Is the XForm a special case
*
* Description:  Multiplies a 4D vector by a 4x4 matrix. The
*               resultant vector is not homogenised into 3 space.
*               This routine can be used to map any 4d vector given
*               any 4x4 transformation matrix.
*
*               NOTE: The result point CANNOT be the same as the input
*                     point.
*
****************************************************************************/
{
    result[0] = FXmul(v[0],mat[0][0]) + FXmul(v[1],mat[0][1]) +
                FXmul(v[2],mat[0][2]);
    result[1] = FXmul(v[0],mat[1][0]) + FXmul(v[1],mat[1][1]) +
                FXmul(v[2],mat[1][2]);
    result[2] = FXmul(v[0],mat[2][0]) + FXmul(v[1],mat[2][1]) +
                FXmul(v[2],mat[2][2]);
    if (special)
        result[3] = v[3];
    else
        result[3] = FXmul(v[0],mat[3][0]) + FXmul(v[1],mat[3][1]) +
                    FXmul(v[2],mat[3][2]);
}

void FL_concat4x4(FXMat4x4 mat,const FXMat4x4 mat1,const FXMat4x4 mat2,
    ibool special)
/****************************************************************************
*
* Function:     FL_concat4x4
* Parameters:   mat     - Place to store result
*               mat1    - First matrix to multiply
*               mat2    - Second matrix to multiply
*
* Description:  Concatenates (multiplies two matrices together). This
*               operation is performed extensively so needs to be as fast
*               as possible. We take advantage of when the matrices are in
*               a special form (the bottom row is <0,0,0,1>) to speed up
*               the concatenation process, and we unroll the entire process
*               to eliminate array index processing.
*
*               NOTE: The result matrix CANNOT be the same as the input
*                     matrix.
*
****************************************************************************/
{
    // The following is rather complicated, but it is a completely unrolled
    // loop for a 4x4 matrix multiplication. It may be large but at least
    // it is fast - consider that a lot of time will probably be spent in
    // this routine and the mapping routines.
    mat[0][0] = FXmul(mat1[0][0],mat2[0][0]) +
                FXmul(mat1[0][1],mat2[1][0]) +
                FXmul(mat1[0][2],mat2[2][0]) +
                FXmul(mat1[0][3],mat2[3][0]);
    mat[0][1] = FXmul(mat1[0][0],mat2[0][1]) +
                FXmul(mat1[0][1],mat2[1][1]) +
                FXmul(mat1[0][2],mat2[2][1]) +
                FXmul(mat1[0][3],mat2[3][1]);
    mat[0][2] = FXmul(mat1[0][0],mat2[0][2]) +
                FXmul(mat1[0][1],mat2[1][2]) +
                FXmul(mat1[0][2],mat2[2][2]) +
                FXmul(mat1[0][3],mat2[3][2]);
    mat[0][3] = FXmul(mat1[0][0],mat2[0][3]) +
                FXmul(mat1[0][1],mat2[1][3]) +
                FXmul(mat1[0][2],mat2[2][3]) +
                FXmul(mat1[0][3],mat2[3][3]);

    mat[1][0] = FXmul(mat1[1][0],mat2[0][0]) +
                FXmul(mat1[1][1],mat2[1][0]) +
                FXmul(mat1[1][2],mat2[2][0]) +
                FXmul(mat1[1][3],mat2[3][0]);
    mat[1][1] = FXmul(mat1[1][0],mat2[0][1]) +
                FXmul(mat1[1][1],mat2[1][1]) +
                FXmul(mat1[1][2],mat2[2][1]) +
                FXmul(mat1[1][3],mat2[3][1]);
    mat[1][2] = FXmul(mat1[1][0],mat2[0][2]) +
                FXmul(mat1[1][1],mat2[1][2]) +
                FXmul(mat1[1][2],mat2[2][2]) +
                FXmul(mat1[1][3],mat2[3][2]);
    mat[1][3] = FXmul(mat1[1][0],mat2[0][3]) +
                FXmul(mat1[1][1],mat2[1][3]) +
                FXmul(mat1[1][2],mat2[2][3]) +
                FXmul(mat1[1][3],mat2[3][3]);

    mat[2][0] = FXmul(mat1[2][0],mat2[0][0]) +
                FXmul(mat1[2][1],mat2[1][0]) +
                FXmul(mat1[2][2],mat2[2][0]) +
                FXmul(mat1[2][3],mat2[3][0]);
    mat[2][1] = FXmul(mat1[2][0],mat2[0][1]) +
                FXmul(mat1[2][1],mat2[1][1]) +
                FXmul(mat1[2][2],mat2[2][1]) +
                FXmul(mat1[2][3],mat2[3][1]);
    mat[2][2] = FXmul(mat1[2][0],mat2[0][2]) +
                FXmul(mat1[2][1],mat2[1][2]) +
                FXmul(mat1[2][2],mat2[2][2]) +
                FXmul(mat1[2][3],mat2[3][2]);
    mat[2][3] = FXmul(mat1[2][0],mat2[0][3]) +
                FXmul(mat1[2][1],mat2[1][3]) +
                FXmul(mat1[2][2],mat2[2][3]) +
                FXmul(mat1[2][3],mat2[3][3]);

    // Handle the special case matrices by not multiplying out the
    // bottom row if there is no need.
    if (!special) {
        mat[3][0] = FXmul(mat1[3][0],mat2[0][0]) +
                    FXmul(mat1[3][1],mat2[1][0]) +
                    FXmul(mat1[3][2],mat2[2][0]) +
                    FXmul(mat1[3][3],mat2[3][0]);
        mat[3][1] = FXmul(mat1[3][0],mat2[0][1]) +
                    FXmul(mat1[3][1],mat2[1][1]) +
                    FXmul(mat1[3][2],mat2[2][1]) +
                    FXmul(mat1[3][3],mat2[3][1]);
        mat[3][2] = FXmul(mat1[3][0],mat2[0][2]) +
                    FXmul(mat1[3][1],mat2[1][2]) +
                    FXmul(mat1[3][2],mat2[2][2]) +
                    FXmul(mat1[3][3],mat2[3][2]);
        mat[3][3] = FXmul(mat1[3][0],mat2[0][3]) +
                    FXmul(mat1[3][1],mat2[1][3]) +
                    FXmul(mat1[3][2],mat2[2][3]) +
                    FXmul(mat1[3][3],mat2[3][3]);
        }
    else {
        mat[3][0] = mat[3][1] = mat[3][2] = REAL(0);
        mat[3][3] = REAL(1);
        }
}

#endif  // !FIXED386
