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
* Description:  Header file for the QMObject base class.
*
****************************************************************************/

#ifndef __QMODEL_OBJECT_HPP
#define __QMODEL_OBJECT_HPP

#ifndef __QUICK3D_QUICK3D_HPP
#include "quick3d/quick3d.hpp"
#endif

#ifndef __TCL_LIST_HPP
#include "tcl/list.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Base class for all objects in the OBJECT.
//---------------------------------------------------------------------------

class QMObject : public TCListNode {
public:
            // Constructor
            QMObject() {};

            // Destructor
    virtual ~QMObject() {};

            // Methods to render the object
    virtual void renderShaded(Quick3d& dc) = 0;
    virtual void renderWireframe(Quick3d& dc) = 0;
    virtual void renderSIPP(void) = 0;

            // Method to compute the dirty rectangle for the object
    virtual ibool computeDirtyRect(MGLRect& dirty,ibool viewChanged) = 0;

            // Method to pre-process the object prior to rendering
    virtual void preProcess();
    };

typedef TCList<QMObject>            QMObjList;
typedef TCListIterator<QMObject>    QMObjListIter;

//---------------------------------------------------------------------------
// Base class for all primitive objects which can be rendered in the OBJECT
//---------------------------------------------------------------------------

class QMPrimitive : public QMObject {
public:
            // Constructor
            QMPrimitive() {};

            // Destructor
    virtual ~QMPrimitive() {};

            // Method to return the bounds of a primitive
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max) = 0;
    };

//---------------------------------------------------------------------------
// Class to wrap a bounding box around an object or set of objects for
// efficient dirty rectangle calculations.
//---------------------------------------------------------------------------

class QMBoundingBox : public QMPrimitive {
protected:
    FXPoint3d   min,max;        // Cached bounding box for child
    QMObject    *child;         // Child object to wrap box around

public:
            // Constructor
            QMBoundingBox(QMObject *child) : child(child) {};

            // Destructor
    virtual ~QMBoundingBox();

            // Method to pre-process the object prior to rendering
    virtual void preProcess();

            // Method to return the bounds of a primitive
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max);

            // Method to compute the dirty rectangle for the object
    virtual ibool computeDirtyRect(MGLRect& dirty,ibool viewChanged);
    };

//---------------------------------------------------------------------------
// Primitive object representing a single flat shaded polygon
//---------------------------------------------------------------------------

class QMFlatPolygon : public QMPrimitive {
protected:
    int         count;          // Number of vertices in polygon
    FXPoint3d   *vertices;      // Array of polygon vertices
    FXVec3d     normal;         // Polygon normal

public:
            // Constructor
            QMFlatPolygon(int count,const FXPoint3d *vertices);
            QMFlatPolygon(int count,const FXPoint3d *vertices,
                const FXVec3d& normal);

            // Destructor
    virtual ~QMFlatPolygon();

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    virtual void renderWireframe(Quick3d& dc) = 0;
    virtual void renderSIPP(void) = 0;
    };

//---------------------------------------------------------------------------
// Primitive object representing a single smooth shaded polygon
//---------------------------------------------------------------------------

class QMSmoothPolygon : public QMPrimitive {
protected:
    int         count;          // Number of vertices in polygon
    FXPoint3d   *vertices;      // Array of polygon vertices
    FXVec3d     *normals;       // Polygon vertex normals

public:
            // Constructor
            QMSmoothPolygon(int count,const FXPoint3d *vertices,
                const FXVec3d *normals);

            // Destructor
    virtual ~QMSmoothPolygon();

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    virtual void renderWireframe(Quick3d& dc) = 0;
    virtual void renderSIPP(void) = 0;
    };

//---------------------------------------------------------------------------
// Primitive object representing a cube
//---------------------------------------------------------------------------

class QMCube : public QMPrimitive {
protected:
    FXPoint3d   vertices[8];    // Face vertices
    FXVec3d     normals[6];     // Face normal

public:
            // Constructor
            QMCube(const FXPoint3d vertices[8]);

            // Destructor
    virtual ~QMCube() {};

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);
    virtual void renderWireframe(Quick3d& dc) = 0;
    virtual void renderSIPP(void) = 0;
    };

/*------------------------- Inline member functions -----------------------*/

#endif  // __QMODEL_OBJECT_HPP
