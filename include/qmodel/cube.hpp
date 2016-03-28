/****************************************************************************
*
*               QuickModeler - A Real-Time 3D Modeling System
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
* Description:  Header file for the QMCube primitive.
*
****************************************************************************/

#ifndef __QMODEL_CUBE_HPP
#define __QMODEL_CUBE_HPP

#ifndef __QMODEL_PRIMITIV_HPP
#include "qmodel/primitiv.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Primitive object representing a unit cube
//---------------------------------------------------------------------------

class QMCube : public QMPrimitive {
protected:
    FXPoint3d   *v;             // Face vertices
    FXVec3d     *n;             // Face normals

public:
            // Constructor for unit cube
            QMCube();

            // Destructor
    virtual ~QMCube() {};

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    virtual void renderWireframe(Quick3d& dc);

            // Method to return the bounds of a primitive
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max);

            // Method to compute the dirty rectangle for the object
    virtual void computeDirtyRect(Quick3d& dc,MGLRect& dirty,ibool viewChanged);
    };

#endif  // __QMODEL_CUBE_HPP
