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
* Description:  Header file for the QMFlatPolygon primitive.
*
****************************************************************************/

#ifndef __QMODEL_POLYGON_HPP
#define __QMODEL_POLYGON_HPP

#ifndef __QMODEL_PRIMITIV_HPP
#include "qmodel/primitiv.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Abstract base class for all polygon primitives
//---------------------------------------------------------------------------

class QMPolygon : public QMPrimitive {
protected:
    int         count;          // Number of vertices in polygon
    FXPoint3d   *vertices;      // Array of polygon vertices

public:
            // Constructor
            QMPolygon(int count,const FXPoint3d *vertices);

            // Destructor
    virtual ~QMPolygon();

            // Methods to render the object
    virtual void renderWireframe(Quick3d& dc);

            // Method to return the bounds of a primitive
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max);
    };

//---------------------------------------------------------------------------
// Primitive object representing a single flat shaded polygon
//---------------------------------------------------------------------------

class QMFlatPolygon : public QMPolygon {
protected:
    FXVec3d     normal;         // Polygon normal

public:
            // Constructor
            QMFlatPolygon(int count,const FXPoint3d *vertices,
                const FXVec3d& normal)
                : QMPolygon(count,vertices),normal(normal) {};

            // Destructor
    virtual ~QMFlatPolygon() {};

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    };

//---------------------------------------------------------------------------
// Primitive object representing a single smooth shaded polygon
//---------------------------------------------------------------------------

class QMSmoothPolygon : public QMPolygon {
protected:
    FXVec3d     *normals;       // Polygon vertex normals

public:
            // Constructor
            QMSmoothPolygon(int count,const FXPoint3d *vertices,
                const FXVec3d *normals);

            // Destructor
    virtual ~QMSmoothPolygon();

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    };

#endif  // __QMODEL_POLYGON_HPP
