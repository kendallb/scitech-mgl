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
* Description:  Implementation for 3d vectors routines.
*
****************************************************************************/

#include "fx/vec3d.h"

/*------------------------- Implementation --------------------------------*/

#define SELECT  REAL(0.7071)    // selection constant (roughly .5*sqrt(2)

void FXVec3dPerp(FXVec3d *result,const FXVec3d *v1)
/****************************************************************************
*
* Function:     FXVec3d::perp
* Parameters:   result  - Place to store vector perpendicular to v1
*               v1      - Vector to start with
*
* Description:  Finds a vector perpendicular another vector. The method
*               used is to take any vector (say <0,1,0>) and subtract the
*               portion of it pointing in the vectors direction. This
*               doesn't work if the vector IS <0,1,0> or is very near it.
*               So in this case we use <0,0,1> instead.
*
****************************************************************************/
{
    FXVec3d r,norm;
    real    dot;

    /* Start by trying vector <0,1,0> */
    FXVec3dSet(r,REAL(0),REAL(1),REAL(0));
    FXVec3dCopy(norm,*v1);
    FXVec3dNormalise(norm);

    /* Check to see if the vector is too close to <0,1,0>. If so, use
     * <0,0,1> instead.
     */
    dot = FXVec3dDot(norm,r);
    if (dot > SELECT || dot < -SELECT) {
        r.x = REAL(0);
        r.y = REAL(1);
        dot = FXVec3dDot(norm,r);
        }

    /* Subtract off the non-perpendicular part */
    r.x -= FXmul(dot,norm.x);
    r.y -= FXmul(dot,norm.y);
    r.z -= FXmul(dot,norm.z);

    /* Make result unit length */
    FXVec3dNormalise(r);
    FXVec3dCopy(*result,r);
}
